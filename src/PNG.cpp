#include "PNG.hpp"

#include "PrivateAccessor.hpp"

#include "zlib/zlib.h"

namespace Texas::detail::PNG
{
	// Turns a 32-bit unsigned integer into correct endian, regardless of system endianness.
	[[nodiscard]] static inline std::uint32_t toCorrect32UIntEndian(const std::byte* const ptr)
	{
		std::uint8_t temp[4] = {};
		std::memcpy(temp, ptr, sizeof(std::uint8_t) * 4);
		return static_cast<std::uint32_t>(temp[3]) | (temp[2] << static_cast<std::uint32_t>(8)) | (temp[1] << static_cast<std::uint32_t>(16)) | (temp[0] << static_cast<std::uint32_t>(24));
	}

	[[nodiscard]] static inline constexpr bool validateColorTypeAndBitDepth(const PNG::ColorType colorType, const std::uint8_t bitDepth)
	{
		switch (colorType)
		{
		case ColorType::Greyscale:
			switch (bitDepth)
			{
			case 1:
			case 2:
			case 4:
			case 8:
			case 16:
				return true;
			default:
				return false;
			}
		case ColorType::Truecolour:
		case ColorType::Greyscale_with_alpha:
		case ColorType::Truecolour_with_alpha:
			switch (bitDepth)
			{
			case 8:
			case 16:
				return true;
			default:
				return false;
			}
		case ColorType::Indexed_colour:
			switch (bitDepth)
			{
			case 1:
			case 2:
			case 4:
			case 8:
				return false;
			}
		default:
			return false;
		}
	}

	[[nodiscard]] static inline PNG::ChunkType getChunkType(const std::byte* const in)
	{
		if (std::memcmp(in, IHDR_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::IHDR;
		else if (std::memcmp(in, PLTE_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::PLTE;
		else if (std::memcmp(in, IDAT_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::IDAT;
		else if (std::memcmp(in, IEND_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::IEND;
		else if (std::memcmp(in, cHRM_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::cHRM;
		else if (std::memcmp(in, gAMA_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::gAMA;
		else if (std::memcmp(in, iCCP_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::iCCP;
		else if (std::memcmp(in, sBIT_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::sBIT;
		else if (std::memcmp(in, sRGB_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::sRGB;
		else if (std::memcmp(in, bKGD_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::bKGD;
		else if (std::memcmp(in, hIST_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::hIST;
		else if (std::memcmp(in, tRNS_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::tRNS;
		else if (std::memcmp(in, pHYs_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::pHYs;
		else if (std::memcmp(in, sPLT_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::sPLT;
		else if (std::memcmp(in, tIME_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::tIME;
		else if (std::memcmp(in, iTXt_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::iTXt;
		else if (std::memcmp(in, tEXt_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::tEXt;
		else if (std::memcmp(in, zTXt_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
			return ChunkType::zTXt;

		return ChunkType::Invalid;
	}

	[[nodiscard]] static inline constexpr PixelFormat toPixelFormat(PNG::ColorType colorType, std::uint8_t bitDepth)
	{
		switch (colorType)
		{
		case ColorType::Greyscale:
			switch (bitDepth)
			{
			case 1:
			case 2:
			case 4:
			case 8:
				return PixelFormat::R_8;
			}
		case ColorType::Truecolour:
			switch (bitDepth)
			{
			case 8:
				return PixelFormat::RGB_8;
			}
		case ColorType::Truecolour_with_alpha:
			switch (bitDepth)
			{
			case 8:
				return PixelFormat::RGBA_8;
			}
		}

		return PixelFormat::Invalid;
	}

	[[nodiscard]] static inline constexpr std::uint8_t getPixelWidth(PixelFormat pixelFormat)
	{
		switch (pixelFormat)
		{
		case PixelFormat::R_8:
			return sizeof(std::uint8_t);
		case PixelFormat::RG_8:
			return sizeof(std::uint8_t) * 2;
		case PixelFormat::RGB_8:
			return sizeof(std::uint8_t) * 3;
		case PixelFormat::RGBA_8:
			return sizeof(std::uint8_t) * 4;
		}

		return 0;
	}

	[[nodiscard]] static inline std::uint8_t paethPredictor(std::uint8_t a, std::uint8_t b, std::uint8_t c)
	{
		const std::int16_t p = a + b - c;

		const std::int16_t pa = std::abs(p - a);
		const std::int16_t pb = std::abs(p - b);
		const std::int16_t pc = std::abs(p - c);

		if (pa <= pb && pa <= pc)
			return a;
		else if (pb <= pc)
			return b;
		else
			return c;
	}
}

Texas::Result Texas::detail::PNG::loadFromBuffer_Step1(
	const bool fileIdentifierConfirmed,
	const ConstByteSpan srcBuffer,
	MetaData& metaData,
	OpenBuffer::PNG_BackendData& backendData)
{
	backendData = OpenBuffer::PNG_BackendData();

	// Check if srcBuffer is large enough hold the header, and more to fit the rest of the chunks
	if (srcBuffer.size() <= Header::totalSize)
		return { ResultType::PrematureEndOfFile, "Source buffer is too small to hold PNG header-data, let alone any image data." };

	metaData.srcFileFormat = FileFormat::PNG;
	metaData.textureType = TextureType::Texture2D;
	metaData.baseDimensions.depth = 1;
	metaData.arrayLayerCount = 1;
	metaData.mipLevelCount = 1;
	metaData.colorSpace = ColorSpace::Linear;
	metaData.channelType = ChannelType::UnsignedNormalized;

	if (fileIdentifierConfirmed == false)
	{
		const std::byte* const fileIdentifier = srcBuffer.data() + Header::identifier_Offset;
		if (std::memcmp(fileIdentifier, Header::identifier, sizeof(fileIdentifier)) != 0)
			return { ResultType::CorruptFileData, "File-identifier does not match PNG file-identifier." };
	}

	const std::uint32_t ihdrChunkDataSize = PNG::toCorrect32UIntEndian(srcBuffer.data() + Header::ihdrChunkSizeOffset);
	if (ihdrChunkDataSize != Header::ihdrChunkDataSize)
		return { ResultType::CorruptFileData, "PNG IHDR chunk data size does not equal 13. PNG specification requires it to be 13." };

	const std::byte* const ihdrChunkType = srcBuffer.data() + Header::ihdrChunkTypeOffset;
	if (PNG::getChunkType(ihdrChunkType) != PNG::ChunkType::IHDR)
		return { ResultType::CorruptFileData, "PNG first chunk is not of type 'IHDR'. PNG requires the 'IHDR' chunk to appear first." };

	// Dimensions are stored in big endian, we must convert to correct endian.
	const std::uint32_t origWidth = PNG::toCorrect32UIntEndian(srcBuffer.data() + Header::widthOffset);
	if (origWidth == 0)
		return { ResultType::CorruptFileData, "PNG IHDR field 'Width' is equal to 0. PNG specification requires it to be >0." };
	metaData.baseDimensions.width = origWidth;

	// Dimensions are stored in big endian, we must convert to correct endian.
	const std::uint32_t origHeight = PNG::toCorrect32UIntEndian(srcBuffer.data() + Header::heightOffset);
	if (origHeight == 0)
		return { ResultType::CorruptFileData, "PNG IHDR field 'Height' is equal to 0. PNG specification requires it to be >0." };
	metaData.baseDimensions.height = origHeight;

	const std::uint8_t bitDepth = static_cast<std::uint8_t>(srcBuffer.data()[Header::bitDepthOffset]);
	const PNG::ColorType colorType = static_cast<PNG::ColorType>(srcBuffer.data()[Header::colorTypeOffset]);
	if (PNG::validateColorTypeAndBitDepth(colorType, bitDepth) == false)
		return { ResultType::CorruptFileData, "PNG does not allow this combination of values from "
											  "IHDR field 'Colour type' and 'Bit depth'." };
	if (bitDepth != 8)
		return { ResultType::FileNotSupported, "Texas does not support PNG files where bit-depth is not 8." };
	metaData.pixelFormat = PNG::toPixelFormat(colorType, bitDepth);
	if (metaData.pixelFormat == PixelFormat::Invalid)
		return { ResultType::FileNotSupported, "PNG colortype and bitdepth combination is not supported." };

	const std::uint8_t compressionMethod = static_cast<std::uint8_t>(srcBuffer.data()[Header::compressionMethodOffset]);
	if (compressionMethod != 0)
		return { ResultType::FileNotSupported, "PNG compression method is not supported." };

	const std::uint8_t filterMethod = static_cast<std::uint8_t>(srcBuffer.data()[Header::filterMethodOffset]);
	if (filterMethod != 0)
		return { ResultType::FileNotSupported, "PNG filter method is not supported." };

	const std::uint8_t interlaceMethod = static_cast<std::uint8_t>(srcBuffer.data()[Header::interlaceMethodOffset]);
	if (interlaceMethod != 0)
		return { ResultType::FileNotSupported, "PNG interlace method is not supported." };


	// Move through chunks looking for more metadata until we find IDAT chunk.
	std::size_t memOffsetTracker = Header::totalSize;
	std::uint8_t chunkTypeCount[(int)PNG::ChunkType::COUNT] = {};
	PNG::ChunkType previousChunkType = PNG::ChunkType::Invalid;
	while (memOffsetTracker < srcBuffer.size() && chunkTypeCount[(int)PNG::ChunkType::IEND] == 0)
	{
		const std::byte* const chunkStart = srcBuffer.data() + memOffsetTracker;

		// Chunk data length is the first entry in the chunk. It's a uint32_t
		const std::uint32_t chunkDataLength = PNG::toCorrect32UIntEndian(chunkStart);
		// Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
		const PNG::ChunkType chunkType = PNG::getChunkType(chunkStart + sizeof(PNG::ChunkSize_T));

		switch (chunkType)
		{
		case ChunkType::IDAT:
		{
			if (previousChunkType != ChunkType::IDAT && chunkTypeCount[(int)PNG::ChunkType::IDAT] > 1)
				return { ResultType::CorruptFileData, "PNG IDAT chunk appeared when a chain of IDAT chunks has already been found. "
													  "PNG specification requires that all IDAT chunks appear consecutively." };
			if (chunkDataLength == 0)
				return { ResultType::CorruptFileData, "PNG IDAT chunk's `Length' field is 0. PNG specification requires it to be >0." };

			backendData.idatChunkStart = chunkStart;

			break;
		}
		case ChunkType::sRGB:
		{
			if (chunkTypeCount[(int)PNG::ChunkType::sRGB] > 0)
				return { ResultType::CorruptFileData, "Encountered a second sRGB chunk in PNG file. "
													  "PNG specification requires that only one sRGB chunk exists in file." };
			if (chunkTypeCount[(int)PNG::ChunkType::IDAT] > 0)
				return { ResultType::CorruptFileData, "PNG sRGB chunk appeared after IDAT chunk(s). PNG specification requires sRGB chunk to appear before any IDAT chunk." };
			if (chunkTypeCount[(int)PNG::ChunkType::iCCP] > 0)
				return { ResultType::CorruptFileData, "PNG sRGB chunk appeared when a iCCP chunk has already been found. PNG specification requires that only of one either sRGB or iCCP chunks may exist." };

			metaData.colorSpace = ColorSpace::sRGB;

			break;
		}
		case ChunkType::gAMA:
		{
			if (chunkTypeCount[(int)PNG::ChunkType::gAMA] > 0)
				return { ResultType::CorruptFileData, "Encountered a second gAMA chunk in PNG file. "
													  "PNG specification requires that only one gAMA chunk exists in file." };
			if (chunkDataLength != sizeof(std::uint32_t))
				return { ResultType::CorruptFileData, "Chunk data length of PNG gAMA chunk is not equal to 4. "
													  "PNG specification demands that chunk data length of gAMA chunk is equal to 4." };

			// TODO: At some point, MetaData might contain gamma. Catch it here

			break;
		}
		case ChunkType::IEND:
		{
			if (chunkTypeCount[(int)PNG::ChunkType::IDAT] == 0)
				return { ResultType::CorruptFileData, "PNG IEND chunk appears before any IDAT chunk. PNG specification requires IEND to be the last chunk." };


			break;
		}
		};

		
		memOffsetTracker += sizeof(PNG::ChunkSize_T) + sizeof(PNG::ChunkType_T) + chunkDataLength + sizeof(PNG::ChunkCRC_T);
		chunkTypeCount[(int)chunkType] += 1;
		previousChunkType = chunkType;
	}

	return { ResultType::Success, nullptr };
}

Texas::Result Texas::detail::PNG::loadFromBuffer_Step2(
	const MetaData& metaData,
	OpenBuffer::PNG_BackendData& backendData,
	const ByteSpan dstImageBuffer,
	const ByteSpan workingMemory)
{
	z_stream zLibDecompressJob{};

	zLibDecompressJob.next_out = (Bytef*)workingMemory.data();
	zLibDecompressJob.avail_out = (uInt)workingMemory.size();

	const int initErr = inflateInit(&zLibDecompressJob);
	if (initErr != Z_OK)
	{
		inflateEnd(&zLibDecompressJob);
		return { ResultType::NoIdea, "During PNG decompression, zLib failed to initialize the decompression job. I don't why." };
	}

	// For every IDAT chunk
	{
		std::size_t memOffsetTracker = 0;
		while (true)
		{
			const std::byte* const chunkStart = backendData.idatChunkStart + memOffsetTracker;

			// Chunk data length is the first entry in the chunk. It's a uint32_t
			const std::uint32_t chunkDataLength = PNG::toCorrect32UIntEndian(chunkStart);
			// Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
			const PNG::ChunkType chunkType = PNG::getChunkType(chunkStart + sizeof(PNG::ChunkSize_T));
			if (chunkType == PNG::ChunkType::IDAT && chunkDataLength == 0)
				return { ResultType::CorruptFileData, "PNG IDAT chunk's 'Length' field is 0. PNG specification requires it to be > 0." };
			// TODO: Validate chunk type.

			const std::byte* const chunkData = chunkStart + sizeof(PNG::ChunkSize_T) + sizeof(PNG::ChunkType_T);
			zLibDecompressJob.next_in = (Bytef*)chunkData;
			zLibDecompressJob.avail_in = (uInt)chunkDataLength;

			const int err = inflate(&zLibDecompressJob, 0);
			if (err == Z_STREAM_END)
			{
				// No more IDAT chunks to decompress
				inflateEnd(&zLibDecompressJob);
				break;
			}

			memOffsetTracker += sizeof(PNG::ChunkSize_T) + sizeof(PNG::ChunkType_T) + chunkDataLength + sizeof(PNG::ChunkCRC_T);
		}
	}

	// UNFILTERING
	{
		const std::byte* const uncompressedData = workingMemory.data();
		std::byte* const dstBuffer = dstImageBuffer.data();

		const std::uint8_t pixelWidth = PNG::getPixelWidth(metaData.pixelFormat);
		const std::size_t rowWidth = static_cast<std::size_t>(pixelWidth) * static_cast<std::size_t>(metaData.baseDimensions.width);

		// Unfilter first row
		{
			const std::uint8_t filterType = static_cast<std::uint8_t>(uncompressedData[0]);
			if (filterType == 0 || filterType == 2)
				// Copy entire row
				std::memcpy(dstBuffer, &uncompressedData[1], rowWidth);
			else if (filterType == 1)
			{
				// Copy first pixel of the row.
				std::memcpy(dstBuffer, &uncompressedData[1], pixelWidth);
				// Then do defiltering on rest of the row.
				for (std::size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
					dstBuffer[widthByte] = static_cast<std::byte>(static_cast<std::uint8_t>(uncompressedData[widthByte + 1]) + static_cast<std::uint8_t>(dstBuffer[widthByte - pixelWidth]));
			}
			else if (filterType == 3)
			{
				// Copy first pixel of the row
				std::memcpy(dstBuffer, &uncompressedData[1], pixelWidth);

				// Traverse every byte of this row after the first pixel
				for (std::size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
				{
					const std::uint8_t filterX = static_cast<std::uint8_t>(uncompressedData[1 + widthByte]);
					const std::uint8_t reconA = static_cast<std::uint8_t>(dstBuffer[widthByte - pixelWidth]);
					dstBuffer[widthByte] = static_cast<std::byte>(filterX + std::uint8_t(reconA / float(2)));
				}
			}
			else
				return { ResultType::CorruptFileData, "Encountered unknown filterType when decompressing PNG imagedata." };
		}

		// Defilter rest of the rows
		for (std::uint32_t y = 1; y < metaData.baseDimensions.height; y++)
		{
			const std::size_t filterTypeIndex = static_cast<std::size_t>(y) * metaData.baseDimensions.width * pixelWidth + static_cast<std::size_t>(y);
			const std::uint8_t filterType = static_cast<std::uint8_t>(uncompressedData[filterTypeIndex]);

			const std::size_t uncompRow = filterTypeIndex + 1;
			const std::size_t unfiltRow = filterTypeIndex - y;

			// Copy all the pixels in the row
			if (filterType == 0)
				std::memcpy(dstBuffer + unfiltRow, &uncompressedData[uncompRow], rowWidth);
			else if (filterType == 1)
			{
				// Copy first pixel of the row
				std::memcpy(dstBuffer + unfiltRow, &uncompressedData[uncompRow], rowWidth);

				for (std::size_t xByte = pixelWidth; xByte < rowWidth; xByte++)
				{
					const std::uint8_t filterX = static_cast<std::uint8_t>(uncompressedData[uncompRow + xByte]);
					const std::uint8_t reconA = static_cast<std::uint8_t>(dstBuffer[unfiltRow + xByte - pixelWidth]);
					dstBuffer[unfiltRow + xByte] = static_cast<std::byte>(filterX + reconA);
				}
			}
			else if (filterType == 2)
			{
				for (std::size_t xByte = 0; xByte < rowWidth; xByte++)
				{
					const std::uint8_t filterX = static_cast<std::uint8_t>(uncompressedData[uncompRow + xByte]);
					const std::uint8_t reconB = static_cast<std::uint8_t>(dstBuffer[unfiltRow + xByte - rowWidth]);
					dstBuffer[unfiltRow + xByte] = static_cast<std::byte>(filterX + reconB);
				}
			}
			else if (filterType == 3)
			{
				for (std::size_t widthByte = 0; widthByte < pixelWidth; widthByte++)
				{
					const std::uint8_t filterX = static_cast<std::uint8_t>(uncompressedData[uncompRow + widthByte]);
					const std::uint8_t reconB = static_cast<std::uint8_t>(dstBuffer[unfiltRow + widthByte - rowWidth]);
					dstBuffer[unfiltRow + widthByte] = static_cast<std::byte>(filterX + static_cast<std::uint8_t>(reconB / float(2)));
				}

				for (std::size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
				{
					const std::uint8_t filterX = static_cast<std::uint8_t>(uncompressedData[uncompRow + widthByte]);
					const std::uint8_t reconA = static_cast<std::uint8_t>(dstBuffer[unfiltRow + widthByte - pixelWidth]);
					const std::uint8_t reconB = static_cast<std::uint8_t>(dstBuffer[unfiltRow + widthByte - rowWidth]);
					dstBuffer[unfiltRow + widthByte] = static_cast<std::byte>(filterX + static_cast<std::uint8_t>((static_cast<std::uint8_t>(reconA) + reconB) / float(2)));
				}
			}
			else if (filterType == 4)
			{
				// Traverse every byte of the first pixel in this row.
				for (std::size_t xByte = 0; xByte < pixelWidth; xByte++)
				{
					const std::uint8_t filterX = static_cast<std::uint8_t>(uncompressedData[uncompRow + xByte]);
					const std::uint8_t reconB = static_cast<std::uint8_t>(dstBuffer[unfiltRow + xByte - rowWidth]);
					dstBuffer[unfiltRow + xByte] = static_cast<std::byte>(filterX + reconB);
				}

				// Traverse every byte of this row after the first pixel
				for (std::size_t xByte = pixelWidth; xByte < rowWidth; xByte++)
				{
					const std::uint8_t filterX = static_cast<std::uint8_t>(uncompressedData[uncompRow + xByte]);
					const std::uint8_t reconA = static_cast<std::uint8_t>(dstBuffer[unfiltRow + xByte - pixelWidth]); // Visual Studio says there's something wrong with this line.
					const std::uint8_t reconB = static_cast<std::uint8_t>(dstBuffer[unfiltRow + xByte - rowWidth]);
					const std::uint8_t reconC = static_cast<std::uint8_t>(dstBuffer[unfiltRow + xByte - rowWidth - pixelWidth]);
					dstBuffer[unfiltRow + xByte] = static_cast<std::byte>(filterX + PNG::paethPredictor(reconA, reconB, reconC));
				}
			}
			else
				return { ResultType::CorruptFileData, "Encountered unknown filterType when decompressing PNG imagedata." };
		}
	}

	return { ResultType::Success, nullptr };
}
