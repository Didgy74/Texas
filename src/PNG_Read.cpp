#include "PNG.hpp"

#include "PrivateAccessor.hpp"

#include "zlib/zlib.h"

// For std::memcpy and std::memcmp
#include <cstring>

namespace Texas::detail::PNG
{
	using ChunkSize_T = std::uint32_t;
	using ChunkType_T = std::uint8_t[4];
	using ChunkCRC_T = std::uint8_t[4];

	namespace Header
	{
		constexpr std::size_t ihdrChunkSizeOffset = 8;
		constexpr std::size_t ihdrChunkTypeOffset = 12;
		constexpr std::size_t ihdrChunkDataSize = 13;

		constexpr std::size_t widthOffset = 16;
		constexpr std::size_t heightOffset = 20;
		constexpr std::size_t bitDepthOffset = 24;
		constexpr std::size_t colorTypeOffset = 25;
		constexpr std::size_t compressionMethodOffset = 26;
		constexpr std::size_t filterMethodOffset = 27;
		constexpr std::size_t interlaceMethodOffset = 28;

		// Total size of all header data.
		constexpr std::size_t totalSize = 33;
	};

	// APNG
	namespace Chunk_acTL
	{
		constexpr std::size_t numFramesOffset = 0;
		using NumFramesT = std::uint32_t;
		constexpr std::size_t numPlaysOffset = 4;
		using NumPlaysT = std::uint32_t;
		constexpr std::size_t totalSize = 8;
	}
	namespace Chunk_fcTL
	{
		constexpr std::size_t sequenceNumberOffset = 0;
		using SequenceNumber_T = std::uint32_t;
		constexpr std::size_t widthOffset = 4;
		using Width_T = std::uint32_t;
		constexpr std::size_t heightOffset = 8;
		using Height_T = std::uint32_t;
		constexpr std::size_t xOffsetOffset = 12;
		using XOffset_T = std::uint32_t;
		constexpr std::size_t yOffsetOffset = 16;
		using YOffset_T = std::uint32_t;
		constexpr std::size_t delayNumOffset = 20;
		using DelayNum_T = std::uint16_t;
		constexpr std::size_t delayDenOffset = 22;
		using DelayDen_T = std::uint16_t;
		constexpr std::size_t disposeOpOffset = 24;
		using DisposeOp_T = std::uint8_t;
		constexpr std::size_t blendOpOffset = 25;
		using BlendOp_T = std::uint8_t;
		constexpr std::size_t totalSize = 26;
	}
	namespace Chunk_fdAT
	{
		constexpr std::size_t sequenceNumberOffset = 0;
		using SequenceNumer_T = std::uint32_t;
		constexpr std::size_t headerSize = 4;
		constexpr std::size_t frameDataOffset = 4;
	}

	enum class ColorType : char;
	enum class ChunkType : char;
	enum class FilterType : char;

	// Turns a 32-bit unsigned integer into correct endian, regardless of system endianness.
	[[nodiscard]] static constexpr std::uint32_t toCorrectEndian_u32(std::byte const* in) noexcept
	{
		return std::uint32_t(in[0]) << 24 | std::uint32_t(in[1]) << 16 | std::uint32_t(in[2]) << 8 | std::uint32_t(in[3]);
	}
	[[nodiscard]] static constexpr std::uint32_t toCorrectEndian_u32(char const* in) noexcept
	{
		return std::uint32_t(in[0]) << 24 | std::uint32_t(in[1]) << 16 | std::uint32_t(in[2]) << 8 | std::uint32_t(in[3]);
	}

	[[nodiscard]] static bool validateColorTypeAndBitDepth(ColorType colorType, std::uint8_t bitDepth) noexcept;

	[[nodiscard]] static ChunkType getChunkType(std::byte const* in) noexcept;

	[[nodiscard]] static PixelFormat toPixelFormat(ColorType colorType, std::uint8_t bitDepth) noexcept;

	[[nodiscard]] static std::uint8_t getPixelWidth(PixelFormat pixelFormat) noexcept;

	[[nodiscard]] static inline std::uint8_t paethPredictor(std::uint8_t a, std::uint8_t b, std::uint8_t c) noexcept;

	[[nodiscard]] static std::uint64_t calcWorkingMemRequired_Stream(
		Dimensions baseDims,
		PixelFormat pFormat,
		bool isIndexed,
		detail::FileInfo_PNG_BackendData const& backendData) noexcept;

	/*
		Defilters uncompressed data and immediately copies the result over to dstMem.
		Should only be used when colour-type != indexed colour.
	*/
	[[nodiscard]] static Result defilterIntoDstBuffer(
		TextureInfo const& textureInfo,
		ByteSpan dstMem,
		ByteSpan uncompressedData) noexcept;

	/*
		Defilters uncompressed data in place. Only handles when each pixel (index) is 1 byte wide.
		This does NOT remove the byte for filter method at the start of each row.
		The defiltering is only applied to the bytes where they are right now,
		the rows will still be padded with +1 for storing the filtering method.
	*/
	[[nodiscard]] static Result defilterIndicesInPlace(
		Dimensions baseDims,
		ByteSpan filteredIndexData);

	[[nodiscard]] static Result deindexData(
		Dimensions baseDims,
		ByteSpan paletteData,
		ByteSpan dstImageBuffer,
		ByteSpan defilteredIndices);

	[[nodiscard]] static Result decompressIdatChunks_Stream(
		InputStream& stream,
		ByteSpan dst_filteredData,
		ByteSpan workingMem) noexcept;
}

enum class Texas::detail::PNG::ColorType : char
{
	Greyscale = 0,
	Truecolour = 2,
	Indexed_colour = 3,
	Greyscale_with_alpha = 4,
	Truecolour_with_alpha = 6
};

// Yes, I could make these be the IHDR_ChunkTypeValue stuff right away
// But I'm using this enum to map into an array when parsing chunks, so fuck you.
enum class Texas::detail::PNG::ChunkType : char
{
	Invalid,

	// Mandatory

	IHDR,
	PLTE,
	IDAT,
	IEND,

	// Optional

	//cHRM,
	//gAMA,
	iCCP,
	//sBIT,
	sRGB,
	/*
	bKGD,
	hIST,
	tRNS,
	pHYs,
	sPLT,
	tIME,
	iTXt,
	tEXt,
	zTXt,
	*/

	// APNG
	acTL,
	fcTL,
	fdAT,

	COUNT
};

enum class Texas::detail::PNG::FilterType : char
{
	None = 0,
	Sub = 1,
	Up = 2,
	Average = 3,
	Paeth = 4,
};

static bool Texas::detail::PNG::validateColorTypeAndBitDepth(
	ColorType colorType,
	std::uint8_t bitDepth)  noexcept
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
		}
		break;
	case ColorType::Truecolour:
	case ColorType::Greyscale_with_alpha:
	case ColorType::Truecolour_with_alpha:
		switch (bitDepth)
		{
		case 8:
		case 16:
			return true;
		}
		break;
	case ColorType::Indexed_colour:
		switch (bitDepth)
		{
		case 1:
		case 2:
		case 4:
		case 8:
			return true;
		}
		break;
	}
	return false;
}

static Texas::detail::PNG::ChunkType Texas::detail::PNG::getChunkType(std::byte const* in) noexcept
{
	std::uint32_t const value = toCorrectEndian_u32(in);
	switch (value)
	{
	case toCorrectEndian_u32("IHDR"):
		return ChunkType::IHDR;
	case toCorrectEndian_u32("PLTE"):
		return ChunkType::PLTE;
	case toCorrectEndian_u32("IDAT"):
		return ChunkType::IDAT;
	case toCorrectEndian_u32("IEND"):
		return ChunkType::IEND;
	
	// Optional chunk types
	case toCorrectEndian_u32("iCCP"):
		return ChunkType::iCCP;
	case toCorrectEndian_u32("sRGB"):
		return ChunkType::sRGB;

	// APNG
	case toCorrectEndian_u32("acTL"):
		return ChunkType::acTL;
	case toCorrectEndian_u32("fcTL"):
		return ChunkType::fcTL;
	case toCorrectEndian_u32("fdAT"):
		return ChunkType::fdAT;

	default:
		return ChunkType::Invalid;
	}
}

static Texas::PixelFormat Texas::detail::PNG::toPixelFormat(
	ColorType colorType,
	std::uint8_t bitDepth) noexcept
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
		case 16:
			return PixelFormat::R_16;
		}
		break;
	case ColorType::Greyscale_with_alpha:
		switch (bitDepth)
		{
		case 8:
			return PixelFormat::RG_8;
		case 16:
			return PixelFormat::RG_16;
		}
		break;
	case ColorType::Truecolour:
		switch (bitDepth)
		{
		case 8:
			return PixelFormat::RGB_8;
		case 16:
			return PixelFormat::RGB_16;
		}
		break;
	case ColorType::Truecolour_with_alpha:
		switch (bitDepth)
		{
		case 8:
			return PixelFormat::RGBA_8;
		case 16:
			return PixelFormat::RGBA_16;
		}
		break;
	case ColorType::Indexed_colour:
		switch (bitDepth)
		{
		case 1:
		case 2:
		case 4:
		case 8:
			return PixelFormat::RGB_8;
		}
		break;
	}

	return PixelFormat::Invalid;
}

static std::uint64_t Texas::detail::PNG::calcWorkingMemRequired_Stream(
	Dimensions baseDims,
	PixelFormat pFormat,
	bool isIndexed,
	detail::FileInfo_PNG_BackendData const& backendData) noexcept
{
	std::uint64_t sum = 0;

	sum += backendData.maxIdatChunkDataLength;

	if (isIndexed)
	{
		if (backendData.plteChunkDataLength > backendData.maxIdatChunkDataLength)
			sum += backendData.plteChunkDataLength - backendData.maxIdatChunkDataLength;

		// One byte per index, one byte extra per row for filter-method.
		sum += baseDims.width * baseDims.height + baseDims.height;
	}
	else
	{
		// The decompressed data will be filtered. It will contain all pixels of the image,
		// but each row will have 1 additional byte for storing the filtering method.
		// So we add +baseDims.height to accomodate this.
		sum += baseDims.width * baseDims.height * getPixelWidth(pFormat) + baseDims.height;
	}

	return sum;
}

static std::uint8_t Texas::detail::PNG::getPixelWidth(PixelFormat pixelFormat) noexcept
{
	switch (pixelFormat)
	{
	case PixelFormat::R_8:
		return 1;
	case PixelFormat::RG_8:
		return 2;
	case PixelFormat::RGB_8:
		return 3;
	case PixelFormat::RGBA_8:
		return 4;
	default:
		return 0;
	}
}

static std::uint8_t Texas::detail::PNG::paethPredictor(
	std::uint8_t a,
	std::uint8_t b,
	std::uint8_t c) noexcept
{
	std::int32_t const p = std::int32_t(a) + b - c;

	std::int32_t pa = p - a;
	if (pa < 0)
		pa = -pa;
	std::int32_t pb = p - b;
	if (pb < 0)
		pb = -pb;
	std::int32_t pc = p - c;
	if (pc < 0)
		pc = -pc;

	if (pa <= pb && pa <= pc)
		return a;
	else if (pb <= pc)
		return b;
	else
		return c;
}

namespace Texas::detail::PNG
{
	struct TempParsingData
	{
		ColorType colorType;
	};

	static Texas::Result parseHeader(
		InputStream& stream,
		TextureInfo& textureInfo,
		TempParsingData& tempParsingData)
	{
		Result result{};

		std::byte headerBuffer[Header::totalSize];
		result = stream.read({ headerBuffer, Header::totalSize });
		if (!result.isSuccessful())
			return result;

		std::uint32_t const ihdrChunkDataSize = toCorrectEndian_u32(headerBuffer + Header::ihdrChunkSizeOffset);
		if (ihdrChunkDataSize != Header::ihdrChunkDataSize)
			return { ResultType::CorruptFileData,
							 "PNG IHDR chunk data size does not equal 13. "
							 "PNG specification requires it to be 13." };

		std::byte const* const ihdrChunkType = headerBuffer + Header::ihdrChunkTypeOffset;
		if (getChunkType(ihdrChunkType) != ChunkType::IHDR)
			return { ResultType::CorruptFileData,
							 "PNG first chunk is not of type 'IHDR'. "
							 "PNG requires the 'IHDR' chunk to appear first in the filestream." };

		// Dimensions are stored in big endian, we must convert to correct endian.
		std::uint32_t const origWidth = toCorrectEndian_u32(headerBuffer + Header::widthOffset);
		if (origWidth == 0)
			return { ResultType::CorruptFileData,
							 "PNG IHDR field 'Width' is equal to 0. "
							 "PNG specification requires it to be higher than 0." };
		textureInfo.baseDimensions.width = origWidth;
		// Dimensions are stored in big endian, we must convert to correct endian.
		std::uint32_t const origHeight = toCorrectEndian_u32(headerBuffer + Header::heightOffset);
		if (origHeight == 0)
			return { ResultType::CorruptFileData,
							 "PNG IHDR field 'Height' is equal to 0. "
							 "PNG specification requires it to be higher than 0." };
		textureInfo.baseDimensions.height = origHeight;

		std::uint8_t const bitDepth = static_cast<std::uint8_t>(headerBuffer[Header::bitDepthOffset]);
		ColorType const colorType = static_cast<ColorType>(headerBuffer[Header::colorTypeOffset]);
		if (validateColorTypeAndBitDepth(colorType, bitDepth) == false)
			return { ResultType::CorruptFileData,
							 "PNG spec does not allow this combination of values from "
							 "IHDR fields 'Colour type' and 'Bit depth'." };
		if (bitDepth != 8)
			return { ResultType::FileNotSupported,
							 "Texas does not support PNG files where bit-depth is not 8." };
		textureInfo.pixelFormat = toPixelFormat(colorType, bitDepth);
		if (textureInfo.pixelFormat == PixelFormat::Invalid)
			return { ResultType::FileNotSupported,
							 "PNG colortype and bitdepth combination is not supported." };
		tempParsingData.colorType = colorType;

		std::uint8_t const compressionMethod = static_cast<std::uint8_t>(headerBuffer[Header::compressionMethodOffset]);
		if (compressionMethod != 0)
			return { ResultType::FileNotSupported, "PNG compression method is not supported." };

		std::uint8_t const filterMethod = static_cast<std::uint8_t>(headerBuffer[Header::filterMethodOffset]);
		if (filterMethod != 0)
			return { ResultType::FileNotSupported, "PNG filter method is not supported." };

		std::uint8_t const interlaceMethod = static_cast<std::uint8_t>(headerBuffer[Header::interlaceMethodOffset]);
		if (interlaceMethod != 0)
			return { ResultType::FileNotSupported, "PNG interlace method is not supported." };

		return successResult;
	}
}

Texas::Result Texas::detail::PNG::parseStream(
	InputStream& stream,
	TextureInfo& textureInfo,
	std::uint64_t& workingMemRequired,
	detail::FileInfo_PNG_BackendData& backendData) noexcept
{
	workingMemRequired = 0;
	backendData = detail::FileInfo_PNG_BackendData{};
	textureInfo.fileFormat = FileFormat::PNG;
	textureInfo.textureType = TextureType::Texture2D;
	textureInfo.baseDimensions.depth = 1;
	textureInfo.layerCount = 1;
	textureInfo.mipCount = 1;
	textureInfo.colorSpace = ColorSpace::Linear;
	textureInfo.channelType = ChannelType::UnsignedNormalized;

	Result result{};

	TempParsingData tempParsingData{};

	result = parseHeader(stream, textureInfo, tempParsingData);
	if (!result.isSuccessful())
		return result;

	// Move through chunks looking for more metadata until we find IDAT chunk.
	// Pro gamer move: chunkTypeCounts[ChunkType::Invali] will contain the number
	// of chunks parsed by not recognized.
	std::uint8_t chunkTypeCounts[(std::size_t)ChunkType::COUNT] = {};
	ChunkType previousChunkType = ChunkType::Invalid;

	float fps = 0.f;
	bool fpsSet = false;

	while (chunkTypeCounts[(std::size_t)ChunkType::IEND] == 0)
	{
		std::byte chunkLengthAndTypeBuffer[8] = {};
		result = stream.read({ chunkLengthAndTypeBuffer, 8 });
		if (!result.isSuccessful())
			return result;

		// Chunk data length is the first entry in the chunk. It's a uint32_t
		std::uint32_t const chunkDataLength = toCorrectEndian_u32(chunkLengthAndTypeBuffer);
		// Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
		ChunkType const chunkType = getChunkType(chunkLengthAndTypeBuffer + sizeof(ChunkSize_T));

		std::size_t chunkDataRead = 0;

		switch (chunkType)
		{
			case ChunkType::IDAT:
			{
				if (previousChunkType != ChunkType::IDAT && chunkTypeCounts[(std::size_t)ChunkType::IDAT] > 1)
					return { ResultType::CorruptFileData,
									 "PNG IDAT chunk appeared when a chain of IDAT chunk(s) has already been found. "
									 "PNG specification requires that all IDAT chunks appear consecutively." };
				if (chunkDataLength == 0)
					return { ResultType::CorruptFileData,
									 "PNG IDAT chunk's `Length' field is 0. PNG specification requires it to be >0." };

				if (chunkTypeCounts[(std::size_t)ChunkType::IDAT] == 0)
					// We subtract 8 because the chunk started 8 bytes ago.
					backendData.firstIdatChunkStreamPos = stream.tell() - 8;
				if (chunkDataLength > backendData.maxIdatChunkDataLength)
					backendData.maxIdatChunkDataLength = chunkDataLength;
			}
			break;

			case ChunkType::IEND:
			{
				if (chunkTypeCounts[(std::size_t)ChunkType::IDAT] == 0)
					return { ResultType::CorruptFileData,
										"PNG IEND chunk appears before any IDAT chunk. "
										"PNG specification requires IEND to be the last chunk." };
				if (chunkDataLength != 0)
					return { ResultType::CorruptFileData ,
										"PNG IEND chunk's data field is non-zero. "
										"PNG specification requires IEND chunk's field 'Data length' to be 0." };
			}
			break;

			case ChunkType::PLTE:
			{
				if (chunkTypeCounts[(std::size_t)ChunkType::PLTE] > 0)
					return { ResultType::CorruptFileData,
									 "Encountered a second PLTE chunk in PNG file. "
									 "PNG specification requires that only one PLTE chunk exists in file." };
				if (chunkTypeCounts[(std::size_t)ChunkType::IDAT] > 0)
					return { ResultType::CorruptFileData,
									 "PNG PLTE chunk appeared after any IDAT chunks. "
									 "PNG specification requires PLTE chunk to appear before any IDAT chunk(s)." };
				if (chunkDataLength == 0)
					return { ResultType::CorruptFileData,
									 "PNG PLTE chunk has field 'Data length' equal to 0. "
									 "PNG specification requires PLTE data length to be non-zero. " };
				if (chunkDataLength > 768)
					return { ResultType::CorruptFileData,
									 "PNG PLTE chunk has field 'Data length' higher than 768 bytes. "
									 "PNG specification requires PLTE data length to be smaller than or equal to 768 bytes." };
				if (chunkDataLength % 3 != 0)
					return { ResultType::CorruptFileData,
									 "PNG PLTE chunk field 'Data length' value not divisible by 3. "
									 "PNG specification requires PLTE data length be divisible by 3." };

				// We subtract 8 bytes to get back to the start of the chunk
				backendData.plteChunkStreamPos = stream.tell() - 8;
				backendData.plteChunkDataLength = chunkDataLength;
			}
			break;

			case ChunkType::sRGB:
			{
				if (chunkTypeCounts[(std::size_t)ChunkType::sRGB] > 0)
					return { ResultType::CorruptFileData,
									 "Encountered a second sRGB chunk in PNG file. "
									 "PNG specification requires that only one sRGB chunk exists in file." };
				if (chunkTypeCounts[(std::size_t)ChunkType::IDAT] > 0)
					return { ResultType::CorruptFileData,
									 "PNG sRGB chunk appeared after IDAT chunk(s). "
									 "PNG specification requires sRGB chunk to appear before any IDAT chunk." };
				if (tempParsingData.colorType == ColorType::Indexed_colour && chunkTypeCounts[(std::size_t)ChunkType::PLTE] > 0)
					return { ResultType::CorruptFileData,
									 "PNG sRGB chunk appeared after a PLTE chunk. "
									 "PNG specification requires sRGB chunk to appear before any "
									 "PLTE chunk when IHDR field 'Colour type' equals 'Indexed colour'." };
				if (chunkTypeCounts[(std::size_t)ChunkType::iCCP] > 0)
					return { ResultType::CorruptFileData,
									 "PNG sRGB chunk appeared when a iCCP chunk has already been found. "
									 "PNG specification requires that only of one either sRGB or iCCP chunks may exist." };
				if (chunkDataLength != 1)
					return { ResultType::CorruptFileData ,
									 "PNG sRGB chunk's data field is not equal to 1. "
									 "PNG specification requires sRGB chunk's field 'Data length' to be 1." };

				textureInfo.colorSpace = ColorSpace::sRGB;
			}
			break;

			case ChunkType::acTL:
			{
				if (chunkTypeCounts[(std::size_t)ChunkType::IDAT] > 0)
					return {
						ResultType::CorruptFileData,
						"PNG acTL chunk did not appear before the first IDAT chunk."
						"PNG specification requires acTL chunk to appear before the first IDAT chunk." };
				if (chunkDataLength != Chunk_acTL::totalSize)
					return {
						ResultType::CorruptFileData, 
						"PNG acTL chunk does not have chunk data length of 8."
						"PNG specification demands that chunk data length of acTL chunk is equal to 4." };

				std::byte acTLChunkDataBuffer[Chunk_acTL::totalSize];
				result = stream.read({ acTLChunkDataBuffer, sizeof(acTLChunkDataBuffer) });
				if (!result.isSuccessful())
					return result;
				chunkDataRead += sizeof(acTLChunkDataBuffer);

				Chunk_acTL::NumFramesT numFrames = toCorrectEndian_u32(acTLChunkDataBuffer + Chunk_acTL::numFramesOffset);
				if (numFrames == 0)
					return {
						ResultType::CorruptFileData,
						"Member num_frames of acTL chunk in APNG file is equal to 0."
						"PNG specification requires this member to be higher than 0." };
				textureInfo.layerCount = (std::uint64_t)numFrames;

				Chunk_acTL::NumPlaysT numPlays = toCorrectEndian_u32(acTLChunkDataBuffer + Chunk_acTL::numPlaysOffset);
			}
			break;

			case ChunkType::fcTL:
			{
				if (chunkTypeCounts[(std::size_t)ChunkType::fcTL] == 0 && chunkTypeCounts[(std::size_t)ChunkType::IDAT] > 0)
					return {
						ResultType::CorruptFileData,
						"First PNG fcTL chunk did not appear before the first IDAT chunk."
						"PNG specification demands when a fcTL chunk is present, the first one must appear before the first IDAT chunk." };
				if (chunkDataLength != Chunk_fcTL::totalSize)
					return {
						ResultType::CorruptFileData,
						"PNG fcTL chunk does not have chunk data length of 26."
						"PNG specification demands that chunk data length of fcTL chunk is equal to 26." };

				std::byte chunkDataBuffer[Chunk_fcTL::totalSize];
				result = stream.read({ chunkDataBuffer, sizeof(chunkDataBuffer) });
				if (!result.isSuccessful())
					return result;
				chunkDataRead = sizeof(chunkDataBuffer);

				Chunk_fcTL::SequenceNumber_T sequenceNumber = toCorrectEndian_u32(chunkDataBuffer + Chunk_fcTL::sequenceNumberOffset);
				
				Chunk_fcTL::Width_T const width = toCorrectEndian_u32(chunkDataBuffer + Chunk_fcTL::widthOffset);
				Chunk_fcTL::Height_T const height = toCorrectEndian_u32(chunkDataBuffer + Chunk_fcTL::heightOffset);

				Chunk_fcTL::XOffset_T const xOffset = toCorrectEndian_u32(chunkDataBuffer + Chunk_fcTL::xOffsetOffset);
				if ((std::uint64_t)xOffset + width > textureInfo.baseDimensions.width)
					return {
						ResultType::CorruptFileData,
						"PNG fcTL members x_offset + width is higher than base image width." };

				Chunk_fcTL::YOffset_T const yOffset = toCorrectEndian_u32(chunkDataBuffer + Chunk_fcTL::yOffsetOffset);
				if ((std::uint64_t)yOffset + height > textureInfo.baseDimensions.height)
					return {
						ResultType::CorruptFileData,
						"PNG fcTL chunk members y_offset + height is higher than base image height." };

				if (width != textureInfo.baseDimensions.width || height != textureInfo.baseDimensions.height
						|| xOffset != 0 || yOffset != 0)
					return {
						ResultType::FileNotSupported,
						"Texas does not support APNG files where animated frames are not same size as the full image." };
				
				Chunk_fcTL::DelayNum_T const delayNum = toCorrectEndian_u32(chunkDataBuffer + Chunk_fcTL::delayNumOffset);
				Chunk_fcTL::DelayDen_T delayDen = toCorrectEndian_u32(chunkDataBuffer + Chunk_fcTL::delayDenOffset);
				if (delayDen == 0)
					delayDen = 100;
				float newFPS = (float)delayNum / delayDen;
				if (fpsSet)
				{
					if (newFPS != fps)
						return {
							ResultType::FileNotSupported,
							"Texas does not support APNG files where all frames do not have the same frame-timing." };
				}
				else
				{
					fps = newFPS;
					fpsSet = true;
				}
			}
			break;

			case ChunkType::fdAT:
			{
				std::byte chunkDataBuffer[Chunk_fdAT::headerSize];
				result = stream.read({ chunkDataBuffer, sizeof(chunkDataBuffer) });
				if (!result.isSuccessful())
					return result;
				chunkDataRead = sizeof(chunkDataBuffer);

				Chunk_fdAT::SequenceNumer_T sequenceNumber = toCorrectEndian_u32(chunkDataBuffer + Chunk_fdAT::sequenceNumberOffset);

				int i = 0; // TESTING.
			}
			break;

		default:
			break;
		};


		// We ignore the chunk data and CRC part of the chunk
		stream.ignore(chunkDataLength - chunkDataRead + sizeof(ChunkType_T));
		chunkTypeCounts[(std::size_t)chunkType] += 1;
		previousChunkType = chunkType;
	}

	if (chunkTypeCounts[(std::size_t)ChunkType::IDAT] == 0)
		return { ResultType::CorruptFileData,
						 "Found no IDAT chunk in PNG file. "
						 "PNG specification requires the file to have atleast one IDAT chunk." };
	if (chunkTypeCounts[(std::size_t)ChunkType::IEND] == 0)
		return { ResultType::CorruptFileData,
						 "Found no IEND chunk in PNG file. "
						 "PNG specification requires the file to have exactly one IEND chunk." };
	if (tempParsingData.colorType == ColorType::Indexed_colour && chunkTypeCounts[(std::size_t)ChunkType::PLTE] == 0)
		return { ResultType::CorruptFileData,
						 "Found no PLTE chunk in PNG file with color-type 'Indexed colour'. "
						 "PNG specification requires a PLTE chunk to exist when color-type is 'Indexed colour'" };
	if (chunkTypeCounts[(std::size_t)ChunkType::fcTL] != textureInfo.layerCount)
		return {
			ResultType::CorruptFileData,
			"Number of PNG fcTL chunks does not match number described by acTL chunk." };

	//backendData.idatChunkCount = chunkTypeCounts[(int)ChunkType::IDAT];

	bool const isIndexedColor = tempParsingData.colorType == ColorType::Indexed_colour;
	workingMemRequired = calcWorkingMemRequired_Stream(
		textureInfo.baseDimensions,
		textureInfo.pixelFormat,
		isIndexedColor,
		backendData);

	return { ResultType::Success, nullptr };
}

static Texas::Result Texas::detail::PNG::deindexData(
	Dimensions baseDims,
	ByteSpan paletteData,
	ByteSpan dstImageBuffer,
	ByteSpan defilteredIndices)
{
	std::byte const* const paletteColors = paletteData.data();
	std::uint32_t const paletteColorCount = static_cast<std::uint32_t>(paletteData.size() / 3);

	for (std::uint32_t y = 0; y < static_cast<std::uint32_t>(baseDims.height); y++)
	{
		std::size_t const rowIndicesOffset = 1 + (y * (baseDims.height + 1));
		// Pointer to the row of indices, does not include the filter-type byte.
		std::byte const* const rowIndices = defilteredIndices.data() + rowIndicesOffset;

		for (std::uint32_t x = 0; x < static_cast<std::uint32_t>(baseDims.width); x++)
		{
			std::uint8_t const paletteIndex = std::uint8_t(rowIndices[x]);
			if (paletteIndex >= paletteColorCount)
				return { ResultType::CorruptFileData, "Encountered an out-of-bounds index while de-indexing PNG file." };
			std::byte const* colorPalettePtr = paletteColors + static_cast<std::size_t>(paletteIndex) * 3;

			std::size_t const dstBufferOffset = (y * static_cast<std::size_t>(baseDims.height) + x) * 3;
			std::memcpy(dstImageBuffer.data() + dstBufferOffset, colorPalettePtr, 3);
		}
	}

	return { ResultType::Success, nullptr };
}

static Texas::Result Texas::detail::PNG::defilterIntoDstBuffer(
	TextureInfo const& textureInfo,
	ByteSpan dstMem,
	ByteSpan filteredData) noexcept
{
	std::byte const* const filteredDataPtr = filteredData.data();
	std::byte* const dstBuffer = dstMem.data();

	// Size is in bytes.
	std::uint8_t const pixelWidth = PNG::getPixelWidth(textureInfo.pixelFormat);
	// Size is in bytes
	// Does not include the byte for filter-type.
	std::size_t const rowWidth = pixelWidth * textureInfo.baseDimensions.width;
	// Size is in bytes
	// Includes the byte for filter-type.
	std::size_t const totalRowWidth = rowWidth + 1;

	// Unfilter first row
	{
		FilterType const filterType = static_cast<FilterType>(filteredDataPtr[0]);
		switch (filterType)
		{
		case FilterType::None:
		case FilterType::Up:
			// If FilterType is None, then we just copy the entire row as it is over.
			// If FilterType is Up, we copy all the bytes of the previous, but since there is no row above, 
			// it just adds 0 to the entire row, so we just copy all of it over to destination buffer.
			std::memcpy(dstBuffer, &filteredDataPtr[1], rowWidth);
			break;
		case FilterType::Sub:
			// Copy first pixel of the row.
			std::memcpy(dstBuffer, &filteredDataPtr[1], pixelWidth);
			// Then do the Sub defiltering on all the rest of the pixels in the row
			for (std::size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
			{
				// We offset by 1 because we have to jump over the byte that contains filtertype
				std::uint8_t const filtX = std::uint8_t(filteredDataPtr[1 + widthByte]);
				std::uint8_t const reconA = std::uint8_t(dstBuffer[widthByte - pixelWidth]);
				dstBuffer[widthByte] = std::byte(filtX + reconA);
			}
			break;
		case FilterType::Average:
			// This Filter does work based on the pixel to the left, and the pixel above.
			// Neither exists for the first pixel on the first row, so we just copy it over.
			std::memcpy(dstBuffer, &filteredDataPtr[1], pixelWidth);
			// Do Average defiltering on the rest of the bytes.
			// We don't use the Recon(b) value because it's always 0 inside the first row.
			for (std::size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
			{
				// We offset by 1 to jump over the byte containing filtertype
				std::uint8_t const filtX = std::uint8_t(filteredDataPtr[1 + widthByte]);
				std::uint8_t const reconA = std::uint8_t(dstBuffer[widthByte - pixelWidth]);
				dstBuffer[widthByte] = std::byte(filtX + reconA / 2);
			}
			break;
		case FilterType::Paeth:
			// We have no work to do on the first pixel of the first row, so we just copy it over
			// We offset by one to take the filtertype into account
			std::memcpy(dstBuffer, &filteredDataPtr[1], pixelWidth);
			// Traverse every byte of this row after the first pixel
			// widthByte does not take into account the first byte of the scanline that holds filtertype
			// Recon(b) and Recon(c) exist on the previous scanline,
			// but there is not previous scanline for first row. So we just use 0.
			for (std::size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
			{
				// We offset by 1 to jump over the byte containing filtertype
				std::uint8_t const filtX = std::uint8_t(filteredDataPtr[1 + widthByte]);
				std::uint8_t const reconA = std::uint8_t(dstBuffer[widthByte - pixelWidth]); // Visual Studio says there's something wrong with this line.
				dstBuffer[widthByte] = std::byte(filtX + reconA);
			}
			break;
		default:
			return { ResultType::CorruptFileData, "Encountered unknown filter-type when defiltering PNG imagedata." };
		}
	}

	// Defilter rest of the rows
	for (std::uint32_t y = 1; y < textureInfo.baseDimensions.height; y++)
	{
		std::size_t const filterTypeOffset = y * totalRowWidth;

		// This is where the filtered data start after the filtertype-byte starts.
		std::size_t const filterRowOffset = filterTypeOffset + 1;
		// This is where the unfiltered data starts.
		std::size_t const unfilterRowOffset = filterTypeOffset - y;

		FilterType const filterType = static_cast<FilterType>(filteredDataPtr[filterTypeOffset]);
		switch (filterType)
		{
		case FilterType::None:
			// Copy all the pixels in the row
			std::memcpy(&dstBuffer[unfilterRowOffset], &filteredDataPtr[filterRowOffset], rowWidth);
			break;
		case FilterType::Sub:
			// Copy first pixel of the row, since Recon(b) is 0 anyways.
			std::memcpy(&dstBuffer[unfilterRowOffset], &filteredDataPtr[filterRowOffset], pixelWidth);
			// Then defilter the rest of the row.
			for (std::size_t widthByte = pixelWidth; widthByte < rowWidth + 1; widthByte++)
			{
				std::uint8_t const filterX = std::uint8_t(filteredDataPtr[filterRowOffset + widthByte]);
				std::uint8_t const reconA = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - pixelWidth]);
				dstBuffer[unfilterRowOffset + widthByte] = std::byte(filterX + reconA);
			}
			break;
		case FilterType::Up:
			for (std::size_t widthByte = 0; widthByte < rowWidth; widthByte++)
			{
				std::uint8_t const filterX = std::uint8_t(filteredDataPtr[filterRowOffset + widthByte]);
				std::uint8_t const reconB = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - rowWidth]);
				dstBuffer[unfilterRowOffset + widthByte] = std::byte(filterX + reconB);
			}
			break;
		case FilterType::Average:
			// First traverse the first pixel of this row.
			// We do this because for the first pixel, Recon(a) will always be 0.
			for (std::size_t widthByte = 0; widthByte < pixelWidth; widthByte++)
			{
				std::uint8_t const filterX = std::uint8_t(filteredDataPtr[filterRowOffset + widthByte]);
				std::uint8_t const reconB = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - rowWidth]);
				dstBuffer[unfilterRowOffset + widthByte] = std::byte(filterX + reconB / 2);
			}
			// Then we perform the defiltering for the rest of the row.
			for (std::size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
			{
				std::uint8_t const filterX = std::uint8_t(filteredDataPtr[filterRowOffset + widthByte]);
				std::uint8_t const reconA = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - pixelWidth]);
				std::uint8_t const reconB = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - rowWidth]);
				dstBuffer[unfilterRowOffset + widthByte] = static_cast<std::byte>(filterX + (reconA + reconB) / 2);
			}
			break;
		case FilterType::Paeth:
			// Traverse every byte of the first pixel in this row.
			// We do this because Recon(a) and Recon(c) will always be 0 for the first pixel of a row.
			for (std::size_t widthByte = 0; widthByte < pixelWidth; widthByte++)
			{
				std::uint8_t const filterX = std::uint8_t(filteredDataPtr[filterRowOffset + widthByte]);
				std::uint8_t const reconB = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - rowWidth]);
				dstBuffer[unfilterRowOffset + widthByte] = std::byte(filterX + reconB);
			}
			// Traverse every byte of this row after the first pixel
			for (std::size_t xByte = pixelWidth; xByte < rowWidth; xByte++)
			{
				std::uint8_t const filterX = std::uint8_t(filteredDataPtr[filterRowOffset + xByte]);
				std::uint8_t const reconA = std::uint8_t(dstBuffer[unfilterRowOffset + xByte - pixelWidth]); // Visual Studio says there's something wrong with this line.
				std::uint8_t const reconB = std::uint8_t(dstBuffer[unfilterRowOffset + xByte - rowWidth]);
				std::uint8_t const reconC = std::uint8_t(dstBuffer[unfilterRowOffset + xByte - rowWidth - pixelWidth]);
				dstBuffer[unfilterRowOffset + xByte] = std::byte(filterX + paethPredictor(reconA, reconB, reconC));
			}
			break;
		default:
			return { ResultType::CorruptFileData, "Encountered unknown filter-type when defiltering PNG imagedata." };
		}
	}

	return { ResultType::Success, nullptr };
}

static Texas::Result Texas::detail::PNG::defilterIndicesInPlace(
	Dimensions baseDims,
	ByteSpan filteredIndexData)
{
	// Start of the buffer that includes all filtered bytes, includes all filter-type bytes.
	std::byte* const filteredData = filteredIndexData.data();

	// Size in bytes
	// This does not include the byte for holding filter-type
	std::size_t const rowWidth = baseDims.width;
	// Size in bytes
	// This includes the byte for holding filter-type
	std::size_t const totalRowWidth = baseDims.width + 1;

	// Unfilter first row

	// This is where the filtered data start excluding the filtertype-byte.
	std::size_t const filterRowOffset = 1;
	PNG::FilterType const filterType = static_cast<FilterType>(filteredData[0]);
	switch (filterType)
	{
	case FilterType::None:
	case FilterType::Up:
		// If FilterType is None, then we do nothing.
		// If FilterType is Up, we copy all the bytes of the previous, but since there is no row above, 
		// it just adds 0 to the entire row, so we do nothing.
		break;
	case FilterType::Sub:
		// Recon(A) is 0 for the first pixel here, so we skip it
		// since defiltering would be pointless.
		//
		// Then do the Sub defiltering on all the rest of the pixels in the row
		// And we offset by 1 to take into account that we skipped the first pixel (index).
		for (std::size_t widthByte = 1; widthByte < rowWidth; widthByte++)
		{
			std::uint8_t const filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
			std::uint8_t const reconA = std::uint8_t(filteredData[filterRowOffset + widthByte - 1]);
			filteredData[filterRowOffset + widthByte] = std::byte(filterX + reconA);
		}
		break;
	case FilterType::Average:
		// This Filter does work based on the pixel to the left, and the pixel above.
		// Neither exists for the first pixel on the first row
		// so we do nothing, for the first pixel.
		//
		// Average defiltering on the rest of the bytes in the row.
		// We don't use the Recon(b) value because it's always 0 inside the first row.
		// And we offset by 1 to take into account that we skipped the first pixel (index).
		for (std::size_t widthByte = 1; widthByte < rowWidth; widthByte++)
		{
			// We offset by 1 to jump over the byte containing filtertype
			std::uint8_t const filtX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
			std::uint8_t const reconA = std::uint8_t(filteredData[filterRowOffset + widthByte - 1]);
			filteredData[filterRowOffset + widthByte] = std::byte(filtX + reconA / 2);
		}
		break;
	case FilterType::Paeth:
		// This can probably be combined with the Sub case.
		// 
		// We have no work to do on the first pixel of the first row, so we skip it.
		// We offset by one to take the filtertype byte into account
		//
		// Traverse every byte of this row after the first pixel
		// widthByte does not take into account the first byte of the scanline that holds filtertype
		// Recon(b) and Recon(c) exist on the previous scanline,
		// but there is not previous scanline for first row. So we just use 0.
		// And we offset by 1 to take into account that we skipped the first pixel (index)
		for (std::size_t widthByte = 1; widthByte < rowWidth; widthByte++)
		{
			std::uint8_t const filtX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
			std::uint8_t const reconA = std::uint8_t(filteredData[filterRowOffset + widthByte - 1]);
			filteredData[filterRowOffset + widthByte] = std::byte(filtX + reconA);
		}
		break;
	default:
		return { ResultType::CorruptFileData, "Encountered unknown filter-type when defiltering PNG imagedata." };
	}

	// Defilter rest of the rows
	for (std::uint32_t y = 1; y < baseDims.height; y++)
	{
		std::size_t const filterTypeOffset = y * totalRowWidth;
		// This is where the filtered data start excluding the filtertype-byte.
		std::size_t const filterRowOffset = filterTypeOffset + 1;

		FilterType const filterType = static_cast<FilterType>(filteredData[filterTypeOffset]);
		switch (filterType)
		{
		case FilterType::None:
			// If FilterType is None, then we do nothing.
			break;
		case FilterType::Sub:
			// Do nothing with the first pixel (index) of the row, since Recon(a) is 0 anyways.
			// Then defilter the rest of the row.
			// And we offset by 1 to take into account that we skipped the first pixel (index)
			for (std::size_t widthByte = 1; widthByte < rowWidth; widthByte++)
			{
				std::uint8_t const filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
				std::uint8_t const reconA = std::uint8_t(filteredData[filterRowOffset + widthByte - 1]);
				filteredData[filterRowOffset + widthByte] = std::byte(filterX + reconA);
			}
			break;
		case FilterType::Up:
			for (std::size_t widthByte = 1; widthByte < rowWidth; widthByte++)
			{
				std::uint8_t const filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
				std::uint8_t const reconB = std::uint8_t(filteredData[filterRowOffset + widthByte - totalRowWidth]);
				filteredData[filterRowOffset + widthByte] = std::byte(filterX + reconB);
			}
			break;
		case FilterType::Average:
			// First defilter the first pixel (index) of the row.
			// We don't need a loop because the pixel is only 1 byte wide anyways
			// since it's an index.
			// We do this because for the first pixel because Recon(a) will always be 0.
		{
			std::uint8_t const filterX = std::uint8_t(filteredData[filterRowOffset + 1]);
			std::uint8_t const reconB = std::uint8_t(filteredData[filterRowOffset - totalRowWidth]);
			filteredData[filterRowOffset] = std::byte(filterX + reconB / 2);
		}

		// Then we perform the defiltering for the rest of the row.
		// We offset by 1 because we skip the first pixel
		for (std::size_t widthByte = 1; widthByte < rowWidth; widthByte++)
		{
			std::uint8_t const filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
			std::uint8_t const reconA = std::uint8_t(filteredData[filterRowOffset + widthByte - 1]);
			std::uint8_t const reconB = std::uint8_t(filteredData[filterRowOffset + widthByte - totalRowWidth]);
			filteredData[filterRowOffset + widthByte] = std::byte(filterX + (reconA + reconB) / 2);
		}
		break;
		case FilterType::Paeth:
			// First defilter the first pixel (index) of the row.
			// We do this because Recon(a) and Recon(c) will always be 0 for the first pixel of a row.
		{
			std::uint8_t const filterX = std::uint8_t(filteredData[filterRowOffset]);
			std::uint8_t const reconB = std::uint8_t(filteredData[filterRowOffset - totalRowWidth]);
			filteredData[filterRowOffset] = std::byte(filterX + reconB);
		}
		// Traverse every byte of this row after the first pixel
		for (std::size_t widthByte = 1; widthByte < rowWidth; widthByte++)
		{
			std::uint8_t const filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
			std::uint8_t const reconA = std::uint8_t(filteredData[filterRowOffset + widthByte - 1]); // Visual Studio says there's something wrong with this line.
			std::uint8_t const reconB = std::uint8_t(filteredData[filterRowOffset + widthByte - totalRowWidth]);
			std::uint8_t const reconC = std::uint8_t(filteredData[filterRowOffset + widthByte - totalRowWidth - 1]);
			filteredData[filterRowOffset + widthByte] = std::byte(filterX + paethPredictor(reconA, reconB, reconC));
		}
		break;
		default:
			return { ResultType::CorruptFileData, "Encountered unknown filter-type when defiltering PNG imagedata." };
		}
	}

	return { ResultType::Success, nullptr };
}

// Assumes the stream is placed at the start of the IDAT chunk(s)
// Decompresses the entire chain of IDAT chunks into `dst_filteredData`
// Size of workingMem must be equal to the biggest IDAT chunk data length
static Texas::Result Texas::detail::PNG::decompressIdatChunks_Stream(
	InputStream& stream,
	ByteSpan dst_filteredData,
	ByteSpan workingMem) noexcept
{
	Result result{};

	z_stream zLibDecompressJob{};
	zLibDecompressJob.next_out = reinterpret_cast<Bytef*>(dst_filteredData.data());
	zLibDecompressJob.avail_out = static_cast<uInt>(dst_filteredData.size());

	int const initErr = inflateInit(&zLibDecompressJob);
	if (initErr != Z_OK)
	{
		inflateEnd(&zLibDecompressJob);
		return { ResultType::CorruptFileData, "During PNG decompression, zLib failed to initialize the decompression job." };
	}

	// Decompress every IDAT chunk
	while (true)
	{
		std::byte chunkLengthAndTypeBuffer[8] = {};
		result = stream.read({ chunkLengthAndTypeBuffer, 8 });
		if (!result.isSuccessful())
			return result;

		// Chunk data length is the first entry in the chunk. It's a uint32_t
		std::uint32_t const chunkDataLength = PNG::toCorrectEndian_u32(chunkLengthAndTypeBuffer);

		if (chunkDataLength == 0)
			return { 
				ResultType::CorruptFileData,
				"PNG IDAT chunk data length is 0. The file has changed since it was parsed." };
		if (chunkDataLength > workingMem.size())
			return { 
				ResultType::CorruptFileData,
					"PNG IDAT chunk data length is larger than what was recorded when parsing the file. "
					"The file has changed since it was parsed." };

		// Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
		ChunkType const chunkType = getChunkType(chunkLengthAndTypeBuffer + sizeof(ChunkSize_T));
		if (chunkType != ChunkType::IDAT)
			return {
				ResultType::CorruptFileData,
				"Found no IDAT chunk when decompressing PNG file. The file has changed since it was parsed." };

		// Stream chunk data into working-memory
		result = stream.read({ workingMem.data(), chunkDataLength });
		if (!result.isSuccessful())
			return result;
		// Then ignore the CRC field of the chunk.
		stream.ignore(4);

		zLibDecompressJob.next_in = reinterpret_cast<Bytef*>(workingMem.data());
		zLibDecompressJob.avail_in = static_cast<uInt>(chunkDataLength);

		int const zLibError = inflate(&zLibDecompressJob, 0);
		if (zLibError == Z_STREAM_END)
		{
			// No more IDAT chunks to decompress
			inflateEnd(&zLibDecompressJob);
			break;
		}
		else if (zLibError == Z_OK)
		{
			// more IDAT chunks to decompress
		}
		else if (zLibError == Z_DATA_ERROR)
		{
			return { ResultType::CorruptFileData,
					"zLib reported a data error while running inflate on PNG IDAT data." };
		}
	}

	return { ResultType::Success, nullptr };
}

Texas::Result Texas::detail::PNG::loadFromStream(
	InputStream& stream,
	TextureInfo const& textureInfo,
	detail::FileInfo_PNG_BackendData const& backendData,
	ByteSpan dstImageBuffer,
	ByteSpan workingMem) noexcept
{
	/*
			The algorithm for this divides up the working memory in 2 parts.

			The first is primarily used to temporarily store the IDAT chunk data
			while we are moving through each IDAT chunk and passing it to zLib.
			This decompressed data gets stored in the second part of the working memory.
			After the decompression and the defiltering, this first part of the memory
			will now be used for storing all the colors in the PLTE chunk.

			The second part is to store filtered data.
	*/


	Result result{};

	std::size_t workingMemFirstSize = backendData.maxIdatChunkDataLength;
	if (backendData.plteChunkDataLength > workingMemFirstSize)
		workingMemFirstSize = backendData.plteChunkDataLength;
	ByteSpan filteredData = {
			workingMem.data() + workingMemFirstSize,
			workingMem.size() - workingMemFirstSize };

	stream.seek(backendData.firstIdatChunkStreamPos);

	// We use the first part of workingMem to store the temporary
	// partial zLib data stream. Then it gets unpacked into filteredData.
	result = decompressIdatChunks_Stream(
		stream,
		filteredData,
		{ workingMem.data(), backendData.maxIdatChunkDataLength });
	if (!result.isSuccessful())
		return result;

	if (backendData.plteChunkStreamPos == 0)
	{
		// If the plteChunk is nullptr, then we are not dealing with indexed colors.
		result = defilterIntoDstBuffer(textureInfo, dstImageBuffer, filteredData);
		if (!result.isSuccessful())
			return result;
		return { ResultType::Success, nullptr };
	}
	else
	{
		// We are dealing with indexed colours.
		// We first load the palette of colors into workingmem
		stream.seek(backendData.plteChunkStreamPos);

		std::byte chunkLengthAndTypeBuffer[8] = {};
		result = stream.read({ chunkLengthAndTypeBuffer, 8 });
		if (!result.isSuccessful())
			return result;
		// Chunk data length is the first entry in the chunk. It's a uint32_t
		std::uint32_t const chunkDataLength = toCorrectEndian_u32(chunkLengthAndTypeBuffer);
		if (chunkDataLength != backendData.plteChunkDataLength)
			return { ResultType::CorruptFileData, "PLTE chunk data length is different from when PNG was parsed." };

		// Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
		ChunkType const chunkType = getChunkType(chunkLengthAndTypeBuffer + sizeof(ChunkSize_T));
		if (chunkType != ChunkType::PLTE)
			return {
				ResultType::CorruptFileData,
				"Found no PLTE when de-indexing PNG file. The file has changed since it was parsed." };

		result = stream.read({ workingMem.data(), chunkDataLength });
		if (!result.isSuccessful())
			return result;

		// We defilter all the pixels (indices) in place.
		result = defilterIndicesInPlace(
			textureInfo.baseDimensions,
			filteredData);
		if (!result.isSuccessful())
			return result;

		// Previous function defiltered the memory in place
		// The filter was applied in-place, so each row is still padded
		// with +1 for the filter-method.
		ByteSpan defilteredIndices = filteredData;
		result = deindexData(
			textureInfo.baseDimensions,
			{ workingMem.data(), backendData.plteChunkDataLength },
			dstImageBuffer,
			defilteredIndices);
		if (!result.isSuccessful())
			return result;
	}

	return { ResultType::Success, nullptr };
}