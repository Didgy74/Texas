#include "PNG.hpp"

#include "PrivateAccessor.hpp"

namespace Texas::detail::PNG
{
	// Turns a 32-bit unsigned integer into correct endian, regardless of system endianness.
	static inline std::uint32_t toCorrectEndian(const std::uint32_t in)
	{
		const std::uint8_t* const ptr = reinterpret_cast<const std::uint8_t*>(&in);
		return static_cast<std::uint32_t>(ptr[3]) | (ptr[2] << static_cast<std::uint32_t>(8)) | ptr[1] << (static_cast<std::uint32_t>(16)) | ptr[0] << (static_cast<std::uint32_t>(24));
	}

	static inline constexpr bool validateColorTypeAndBitDepth(const PNG::ColorType colorType, const std::uint8_t bitDepth)
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

	static inline constexpr std::uint8_t getPixelWidth(PixelFormat pixelFormat)
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

	static inline std::uint8_t paethPredictor(std::uint8_t a, std::uint8_t b, std::uint8_t c)
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

	static inline constexpr PixelFormat toPixelFormat(PNG::ColorType colorType, std::uint8_t bitDepth)
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
}

Texas::Pair<Texas::ResultType, const char*> Texas::detail::PNG::loadFromBuffer_Step1(
	const bool fileIdentifierConfirmed,
	const ConstByteSpan srcBuffer,
	MetaData& metaData)
{
	// Check if srcBuffer is large enough hold the header, and more to fit the rest of the chunks
	if (srcBuffer.size() <= Header::totalSize)
		return { ResultType::PrematureEndOfFile, "Source buffer is too small to hold PNG header-data, let alone any image data." };

	metaData.srcFileFormat = FileFormat::PNG;
	metaData.textureType = TextureType::Texture2D;
	metaData.baseDimensions.depth = 1;
	metaData.arrayLayerCount = 1;
	metaData.mipLevelCount = 1;
	metaData.colorSpace = ColorSpace::Linear;

	if (fileIdentifierConfirmed == false)
	{
		const Header::Identifier_T* const fileIdentifier = reinterpret_cast<const Header::Identifier_T*>(srcBuffer.data()) + Header::identifier_Offset;
		if (std::memcmp(fileIdentifier, Header::identifier, sizeof(fileIdentifier)) != 0)
			return { ResultType::CorruptFileData, "File-identifier does not match PNG file-identifier." };
	}

	const std::uint32_t ihdrChunkDataSize = toCorrectEndian(*reinterpret_cast<const std::uint32_t*>((const std::uint8_t*)srcBuffer.data() + Header::ihdrChunkSizeOffset));
	if (ihdrChunkDataSize != Header::ihdrChunkDataSize)
		return { ResultType::CorruptFileData, "PNG IHDR chunk data size does not equal 13. PNG specification requires it to be 13." };

	const std::uint8_t* const ihdrChunkType = static_cast<const std::uint8_t*>(srcBuffer.data()) + Header::ihdrChunkTypeOffset;
	if (std::memcmp(ihdrChunkType, Header::IHDR_ChunkTypeValue, sizeof(Header::IHDR_ChunkTypeValue)) != 0)
		return { ResultType::CorruptFileData, "PNG first chunk is not of type 'IHDR'. PNG requires the 'IHDR' chunk to appear first." };

	// Dimensions are stored in big endian, we must convert to correct endian.
	const std::uint32_t origWidth = toCorrectEndian(*reinterpret_cast<const std::uint32_t*>((const std::uint8_t*)srcBuffer.data() + Header::widthOffset));
	if (origWidth == 0)
		return { ResultType::CorruptFileData, "PNG IHDR field 'Width' is equal to 0. PNG specification requires it to be >0." };
	metaData.baseDimensions.width = origWidth;

	// Dimensions are stored in big endian, we must convert to correct endian.
	const std::uint32_t origHeight = toCorrectEndian(*reinterpret_cast<const std::uint32_t*>((const std::uint8_t*)srcBuffer.data() + Header::heightOffset));
	if (origHeight == 0)
		return { ResultType::CorruptFileData, "PNG IHDR field 'Height' is equal to 0. PNG specification requires it to be >0." };
	metaData.baseDimensions.height = origHeight;

	const std::uint8_t bitDepth = *(static_cast<const std::uint8_t*>(srcBuffer.data()) + Header::bitDepthOffset);
	const PNG::ColorType colorType = (PNG::ColorType)*(static_cast<const std::uint8_t*>(srcBuffer.data()) + Header::colorTypeOffset);
	if (validateColorTypeAndBitDepth(colorType, bitDepth) == false)
		return { ResultType::CorruptFileData, "PNG does not allow this combination of values from IHDR field 'Colour type' and 'Bit depth'." };
	if (bitDepth != 8)
		return { ResultType::FileNotSupported, "Texas does not support PNG files where bit-depth is not 8." };
	metaData.pixelFormat = PNG::toPixelFormat(colorType, bitDepth);
	if (metaData.pixelFormat == PixelFormat::Invalid)
		return { ResultType::FileNotSupported, "PNG colortype and bitdepth combination is not supported." };

	const std::uint8_t compressionMethod = *(static_cast<const std::uint8_t*>(srcBuffer.data()) + Header::compressionMethodOffset);
	if (compressionMethod != 0)
		return { ResultType::FileNotSupported, "PNG compression method is not supported." };

	const std::uint8_t filterMethod = *(static_cast<const std::uint8_t*>(srcBuffer.data()) + Header::filterMethodOffset);
	if (filterMethod != 0)
		return { ResultType::FileNotSupported, "PNG filter method is not supported." };

	const std::uint8_t interlaceMethod = *(static_cast<const std::uint8_t*>(srcBuffer.data()) + Header::interlaceMethodOffset);
	if (interlaceMethod != 0)
		return { ResultType::FileNotSupported, "PNG interlace method is not supported." };


	// Move through chunks looking for more metadata until we find IDAT chunk.
	std::size_t memOffsetTracker = Header::totalSize;
	while (memOffsetTracker < srcBuffer.size())
	{
		const std::uint8_t* const chunkStart = static_cast<const std::uint8_t*>(srcBuffer.data()) + memOffsetTracker;

		const std::uint32_t chunkDataLength = toCorrectEndian(*reinterpret_cast<const uint32_t*>(chunkStart));
		const std::uint8_t* const chunkType = chunkStart + sizeof(ChunkType_T);

		// Handle IDAT chunk
		if (std::memcmp(chunkType, Header::IDAT_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
		{
			if (chunkDataLength == 0)
				return { ResultType::CorruptFileData, "PNG IDAT chunk's `Length' field is 0. PNG specification requires it to be >0." };
		}
		// Handle sRGB chunk
		else if (std::memcmp(chunkType, Header::sRGB_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
		{
			// Do stuff with sRGB chunk
			metaData.colorSpace = ColorSpace::sRGB;
		}
		else if (std::memcmp(chunkType, Header::IEND_ChunkTypeValue, sizeof(ChunkType_T)) == 0)
		{
			return { ResultType::Success, nullptr };
		}

		memOffsetTracker += sizeof(ChunkSize_T) + sizeof(ChunkType_T) + chunkDataLength + sizeof(ChunkCRC_T);
	}

	return { ResultType::Success, nullptr };
}

/*
bool Texas::detail::PrivateAccessor::PNG_LoadImageData(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer)
{
	// We add metaData.baseDimensions.height because every row starts with 1 byte specifying filter method for the row.
	std::vector<uint8_t> uncompressedData(metaData.GetTotalSizeRequired() + metaData.baseDimensions.height);

	z_stream zLibDecompressJob{};

	zLibDecompressJob.next_out = (Bytef*)uncompressedData.data();
	zLibDecompressJob.avail_out = (uInt)uncompressedData.size();

	auto initErr = inflateInit(&zLibDecompressJob);
	if (initErr != Z_OK)
	{
		inflateEnd(&zLibDecompressJob);
		return false;
	}

	while (true)
	{
		// Load chunk size and chunk type
		uint8_t chunkSizeAndTypeBuffer[sizeof(PNG::ChunkSize_T) + sizeof(PNG::ChunkType_T)];

		fstream.read(reinterpret_cast<char*>(chunkSizeAndTypeBuffer), sizeof(chunkSizeAndTypeBuffer));

		// Stop if we don't find a IDAT chunk.
		if (*reinterpret_cast<const uint32_t*>(chunkSizeAndTypeBuffer + sizeof(PNG::ChunkType_T)) != *reinterpret_cast<const uint32_t*>(PNG::Header::IDAT_ChunkTypeValue))
		{
			inflateEnd(&zLibDecompressJob);
			return false;
		}

		const uint32_t chunkDataLength = uint32_t(chunkSizeAndTypeBuffer[3]) | (chunkSizeAndTypeBuffer[2] << uint32_t(8)) | (chunkSizeAndTypeBuffer[1] << uint32_t(16)) | (chunkSizeAndTypeBuffer[0] << uint32_t(24));

		fstream.read(reinterpret_cast<char*>(dstBuffer), chunkDataLength);

		zLibDecompressJob.next_in = (Bytef*)dstBuffer;
		zLibDecompressJob.avail_in = (uInt)chunkDataLength;

		const auto err = inflate(&zLibDecompressJob, 0);
		if (err == Z_STREAM_END)
		{
			// No more IDAT chunks to decompress
			inflateEnd(&zLibDecompressJob);
			break;
		}

		// We ignore CRC data.
		fstream.ignore(sizeof(PNG::ChunkCRC_T));
	}

	// UNFILTERING

	const uint8_t pixelWidth = PNG::GetPixelWidth(metaData.pixelFormat);
	const size_t rowWidth = pixelWidth * size_t(metaData.baseDimensions.width);

	// Unfilter first row
	const uint8_t filterType = uncompressedData[0];
	if (filterType == 0 || filterType == 2)
		// Copy entire row
		std::memcpy(dstBuffer, &uncompressedData[1], rowWidth);
	else if (filterType == 1)
	{
		// Copy first pixel of the row.
		std::memcpy(dstBuffer, &uncompressedData[1], pixelWidth);
		// Then do defiltering on rest of the row.
		for (size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
		{
			//const uint8_t filterX = uncompressedData[widthByte + 1];
			//const uint8_t reconA = dstBuffer[widthByte - pixelWidth];
			dstBuffer[widthByte] = uncompressedData[widthByte + 1] + dstBuffer[widthByte - pixelWidth];
		}
	}
	else if (filterType == 3)
	{
		// Copy first pixel of the row
		std::memcpy(dstBuffer, &uncompressedData[1], pixelWidth);

		for (size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
		{
			const uint8_t filterX = uncompressedData[1 + widthByte];
			const uint8_t reconA = dstBuffer[widthByte - pixelWidth];
			dstBuffer[widthByte] = filterX + uint8_t(reconA / float(2));
		}
	}
	else
		return false;

	// Defilter rest of the rows
	for (uint32_t y = 1; y < metaData.baseDimensions.height; y++)
	{
		const size_t filterTypeIndex = size_t(y) * size_t(metaData.baseDimensions.width) * size_t(pixelWidth) + size_t(y);
		const uint8_t filterType = uncompressedData[filterTypeIndex];

		const size_t uncompRow = filterTypeIndex + 1;
		const size_t unfiltRow = filterTypeIndex - y;

		// Copy all the pixels in the scanline
		if (filterType == 0)
			std::memcpy(dstBuffer + unfiltRow, &uncompressedData[uncompRow], rowWidth);
		else if (filterType == 1)
		{
			// Copy first pixel of the scanline
			std::memcpy(dstBuffer + unfiltRow, &uncompressedData[uncompRow], rowWidth);

			for (size_t xByte = pixelWidth; xByte < rowWidth; xByte++)
			{
				const uint8_t filterX = uncompressedData[uncompRow + xByte];
				const uint8_t reconA = dstBuffer[unfiltRow + xByte - pixelWidth];
				dstBuffer[unfiltRow + xByte] = filterX + reconA;
			}
		}
		else if (filterType == 2)
		{
			for (size_t xByte = 0; xByte < rowWidth; xByte++)
			{
				const uint8_t filterX = uncompressedData[uncompRow + xByte];
				const uint8_t reconB = dstBuffer[unfiltRow + xByte - rowWidth];
				dstBuffer[unfiltRow + xByte] = filterX + reconB;
			}
		}
		else if (filterType == 3)
		{
			for (size_t widthByte = 0; widthByte < pixelWidth; widthByte++)
			{
				const uint8_t filterX = uncompressedData[uncompRow + widthByte];
				const uint8_t reconB = dstBuffer[unfiltRow + widthByte - rowWidth];
				dstBuffer[unfiltRow + widthByte] = filterX + uint8_t(reconB / float(2));
			}

			for (size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
			{
				const uint8_t filterX = uncompressedData[uncompRow + widthByte];
				const uint8_t reconA = dstBuffer[unfiltRow + widthByte - pixelWidth];
				const uint8_t reconB = dstBuffer[unfiltRow + widthByte - rowWidth];
				dstBuffer[unfiltRow + widthByte] = filterX + uint8_t((uint16_t(reconA) + uint16_t(reconB)) / float(2));
			}
		}
		else if (filterType == 4)
		{
			for (size_t xByte = 0; xByte < pixelWidth; xByte++)
			{
				const uint8_t filterX = uncompressedData[uncompRow + xByte];
				const uint8_t reconB = dstBuffer[unfiltRow + xByte - rowWidth];
				dstBuffer[unfiltRow + xByte] = filterX + reconB;
			}

			for (size_t xByte = pixelWidth; xByte < rowWidth; xByte++)
			{
				const uint8_t filterX = uncompressedData[uncompRow + xByte];
				const uint8_t reconA = dstBuffer[unfiltRow + xByte - pixelWidth];
				const uint8_t reconB = dstBuffer[unfiltRow + xByte - rowWidth];
				const uint8_t reconC = dstBuffer[unfiltRow + xByte - rowWidth - pixelWidth];
				dstBuffer[unfiltRow + xByte] = filterX + PNG::PaethPredictor(reconA, reconB, reconC);;
			}
		}
		else
			return false;
	}

	return true;
}
*/