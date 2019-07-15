#include "PNG.hpp"

#include "PrivateAccessor.hpp"

#include "zlib/zlib.h"

#include <vector>
#include <string_view>
#include <cstring>

namespace DTex::detail::PNG
{
	static constexpr uint8_t GetPixelWidth(PixelFormat pixelFormat)
	{
		switch (pixelFormat)
		{
		case PixelFormat::R_8:
			return sizeof(uint8_t);
		case PixelFormat::RG_8:
			return sizeof(uint8_t) * 2;
		case PixelFormat::RGB_8:
			return sizeof(uint8_t) * 3;
		case PixelFormat::RGBA_8:
			return sizeof(uint8_t) * 4;
		}

		return 0;
	}

	static uint8_t PaethPredictor(uint8_t a, uint8_t b, uint8_t c)
	{
		const int16_t p = a + b - c;

		const int16_t pa = std::abs(p - a);
		const int16_t pb = std::abs(p - b);
		const int16_t pc = std::abs(p - c);

		if (pa <= pb && pa <= pc)
			return a;
		else if (pb <= pc)
			return b;
		else
			return c;
	}

	static constexpr PixelFormat ToPixelFormat(uint8_t colorType, uint8_t bitDepth)
	{
		constexpr auto greyscale = 0;
		constexpr auto truecolor = 2;
		constexpr auto indexedColor = 3;
		constexpr auto greyscaleWithAlpha = 4;
		constexpr auto trueColorWithAlpha = 6;

		switch (colorType)
		{
		case greyscale:
			switch (bitDepth)
			{
			case 1:
			case 2:
			case 4:
			case 8:
				return PixelFormat::R_8;
			}
		case truecolor:
			switch (bitDepth)
			{
			case 8:
				return PixelFormat::RGB_8;
			}
		case trueColorWithAlpha:
			switch (bitDepth)
			{
			case 8:
				return PixelFormat::RGBA_8;
			}
		}

		return PixelFormat::Invalid;
	}
}

bool DTex::detail::PNG::LoadHeader_Backend(MetaData& metaData, std::ifstream& fstream, ResultInfo& resultInfo, std::string_view& errorMessage)
{
	using namespace std::literals;

	uint8_t headerBuffer[Header::totalSize];
	fstream.read(reinterpret_cast<char*>(headerBuffer), sizeof(headerBuffer));

	// Check for end of file while reading header
	if (fstream.eof())
	{
		resultInfo = ResultInfo::CorruptFileData;
		errorMessage = "Reached end-of-file while reading file header."sv;
		return false;
	}

	metaData.srcFileFormat = FileFormat::PNG;
	metaData.textureType = TextureType::Texture2D;
	metaData.baseDimensions.depth = 1;
	metaData.arrayLayerCount = 1;
	metaData.mipLevelCount = 1;
	metaData.colorSpace = ColorSpace::Linear;

	const Header::Identifier_T* const fileIdentifier = reinterpret_cast<const Header::Identifier_T*>(headerBuffer + Header::identifierOffset);
	if (memcmp(fileIdentifier, Header::identifier, sizeof(fileIdentifier)) != 0)
	{
		resultInfo = ResultInfo::CorruptFileData;
		errorMessage = "File-identifier does not match PNG file-identifier."sv;
		return false;
	}

	// Dimensions are stored in big endian, we must convert to correct endian.
	const uint8_t* const widthPtr = reinterpret_cast<const uint8_t * const>(headerBuffer + Header::widthOffset);
	metaData.baseDimensions.width = uint32_t(widthPtr[3]) | (widthPtr[2] << uint32_t(8)) | (widthPtr[1] << uint32_t(16)) | (widthPtr[0] << uint32_t(24));

	const uint8_t* const heightPtr = reinterpret_cast<const uint8_t * const>(headerBuffer + Header::heightOffset);
	metaData.baseDimensions.height = uint32_t(heightPtr[3]) | (heightPtr[2] << uint32_t(8)) | (heightPtr[1] << uint32_t(16)) | (heightPtr[0] << uint32_t(24));

	const uint8_t& bitDepth = *reinterpret_cast<const uint8_t*>(headerBuffer + Header::bitDepthOffset);

	const uint8_t& colorType = *reinterpret_cast<const uint8_t*>(headerBuffer + Header::colorTypeOffset);

	metaData.pixelFormat = PNG::ToPixelFormat(colorType, bitDepth);
	if (metaData.pixelFormat == PixelFormat::Invalid)
	{
		resultInfo = ResultInfo::FileNotSupported;
		errorMessage = "PNG colortype and bitdepth combination is not supported."sv;
		return false;
	}

	const uint8_t& compressionMethod = *reinterpret_cast<const uint8_t*>(headerBuffer + Header::compressionMethodOffset);
	if (compressionMethod != 0)
	{
		resultInfo = ResultInfo::FileNotSupported;
		errorMessage = "PNG compression method is not supported."sv;
		return false;
	}

	const uint8_t& filterMethod = *reinterpret_cast<const uint8_t*>(headerBuffer + Header::filterMethodOffset);
	if (filterMethod != 0)
	{
		resultInfo = ResultInfo::FileNotSupported;
		errorMessage = "PNG filter method is not supported."sv;
		return false;
	}

	const uint8_t& interlaceMethod = *reinterpret_cast<const uint8_t*>(headerBuffer + Header::interlaceMethodOffset);
	if (interlaceMethod != 0)
	{
		resultInfo = ResultInfo::FileNotSupported;
		errorMessage = "PNG interlace method is not supported."sv;
		return false;
	}

	// Move through chunks looking for more metadata until we find IDAT chunk.
	while (true)
	{
		const std::streampos chunkStreamPos = fstream.tellg();

		// Load chunk size and chunk type
		uint8_t chunkSizeAndTypeBuffer[sizeof(PNG::ChunkSize_T) + sizeof(PNG::ChunkType_T)];

		fstream.read(reinterpret_cast<char*>(chunkSizeAndTypeBuffer), sizeof(chunkSizeAndTypeBuffer));

		const uint32_t chunkDataLength = uint32_t(chunkSizeAndTypeBuffer[3]) | (chunkSizeAndTypeBuffer[2] << uint32_t(8)) | (chunkSizeAndTypeBuffer[1] << uint32_t(16)) | (chunkSizeAndTypeBuffer[0] << uint32_t(24));

		// Handle IDAT chunk
		if (*reinterpret_cast<const uint32_t*>(chunkSizeAndTypeBuffer + sizeof(PNG::ChunkSize_T)) == *reinterpret_cast<const uint32_t*>(Header::IDAT_ChunkTypeValue))
		{
			fstream.seekg(chunkStreamPos);
	
			break;
		}
		// Handle sRGB chunk
		else if (*reinterpret_cast<const uint32_t*>(chunkSizeAndTypeBuffer + sizeof(PNG::ChunkSize_T)) == *reinterpret_cast<const uint32_t*>(Header::SRGB_ChunkTypeValue))
		{
			// Do stuff with sRGB chunk
			metaData.colorSpace = ColorSpace::sRGB;
		}
		else if (*reinterpret_cast<const uint32_t*>(chunkSizeAndTypeBuffer + sizeof(PNG::ChunkSize_T)) == *reinterpret_cast<const uint32_t*>(Header::IEND_ChunkTypeValue))
		{
			resultInfo = ResultInfo::CorruptFileData;
			errorMessage = "PNG IEND chunk appeared before IDATA chunk. File is corrupt."sv;
			return false;
		}

		fstream.ignore(chunkDataLength + sizeof(PNG::ChunkCRC_T));
	}

	return true;
}

bool DTex::detail::PrivateAccessor::PNG_LoadImageData(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer)
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