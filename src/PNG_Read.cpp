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

    [[nodiscard]] static inline constexpr std::uint32_t setupChunkTypeValue(std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d)
    {
        return a << 24 | b << 16 | c << 8 | d;
    }

    constexpr auto IHDR_ChunkTypeValue = setupChunkTypeValue(73, 72, 68, 82);
    constexpr auto PLTE_ChunkTypeValue = setupChunkTypeValue(80, 76, 84, 69);
    constexpr auto IDAT_ChunkTypeValue = setupChunkTypeValue(73, 68, 65, 84);
    constexpr auto IEND_ChunkTypeValue = setupChunkTypeValue(73, 69, 78, 68);
    constexpr auto cHRM_ChunkTypeValue = setupChunkTypeValue(99, 72, 82, 77);
    constexpr auto gAMA_ChunkTypeValue = setupChunkTypeValue(103, 65, 77, 65);
    constexpr auto iCCP_ChunkTypeValue = setupChunkTypeValue(105, 67, 67, 80);
    constexpr auto sBIT_ChunkTypeValue = setupChunkTypeValue(115, 66, 73, 84);
    constexpr auto sRGB_ChunkTypeValue = setupChunkTypeValue(115, 82, 71, 66);
    constexpr auto bKGD_ChunkTypeValue = setupChunkTypeValue(98, 75, 71, 68);
    constexpr auto hIST_ChunkTypeValue = setupChunkTypeValue(104, 73, 83, 84);
    constexpr auto tRNS_ChunkTypeValue = setupChunkTypeValue(116, 82, 78, 83);
    constexpr auto pHYs_ChunkTypeValue = setupChunkTypeValue(112, 72, 89, 115);
    constexpr auto sPLT_ChunkTypeValue = setupChunkTypeValue(115, 80, 76, 84);
    constexpr auto tIME_ChunkTypeValue = setupChunkTypeValue(116, 73, 77, 69);
    constexpr auto iTXt_ChunkTypeValue = setupChunkTypeValue(105, 84, 88, 116);
    constexpr auto tEXt_ChunkTypeValue = setupChunkTypeValue(116, 69, 88, 116);
    constexpr auto zTXt_ChunkTypeValue = setupChunkTypeValue(122, 84, 88, 116);

    namespace Header
    {
        constexpr std::size_t ihdrChunkSizeOffset = 8;
        constexpr std::size_t ihdrChunkTypeOffset = 12;
        constexpr std::size_t ihdrChunkDataSize = 13;

        using Dimension_T = std::uint32_t;
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

    enum class ColorType : std::uint8_t;
    enum class ChunkType : std::uint8_t;
    enum class FilterType : std::uint8_t;

    // Turns a 32-bit unsigned integer into correct endian, regardless of system endianness.
    [[nodiscard]] static inline std::uint32_t toCorrectEndian_u32(const std::byte* const ptr);

    [[nodiscard]] static inline bool validateColorTypeAndBitDepth(const PNG::ColorType colorType, const std::uint8_t bitDepth);

    [[nodiscard]] static inline PNG::ChunkType getChunkType(const std::byte* const in);

    [[nodiscard]] static inline PixelFormat toPixelFormat(PNG::ColorType colorType, std::uint8_t bitDepth);

    [[nodiscard]] static inline std::uint64_t calcWorkingMemRequired(
        Dimensions baseDims,
        PixelFormat pFormat,
        bool isIndexed) noexcept;

    [[nodiscard]] static inline std::uint8_t getPixelWidth(PixelFormat pixelFormat);

    [[nodiscard]] static inline std::uint8_t paethPredictor(std::uint8_t a, std::uint8_t b, std::uint8_t c);

    /*
        Defilters uncompressed data and immediately copies the result over to dstMem. Should only be used when colour-type != indexed colour.
    */
    [[nodiscard]] static inline Result loadFromBuffer_Step2_DefilterIntoDstBuffer(const TextureInfo& textureInfo, const ByteSpan dstMem, const ByteSpan uncompressedData);

    /*
        Defilters uncompressed data in place. Only handles when each pixel (index) is 1 byte wide.
    */
    [[nodiscard]] static inline Result loadFromBuffer_Step2_DefilterInPlace(const Dimensions& baseDims, const ByteSpan uncompressedData);

    [[nodiscard]] static inline Result loadFromBuffer_Step2_Deindex(
        const Dimensions baseDims,
        const std::byte* const plteChunkStart,
        const unsigned int previous_plteChunkDataLength,
        const ByteSpan dstImageBuffer,
        const ByteSpan workingMem);
}

enum class Texas::detail::PNG::ColorType : std::uint8_t
{
    Greyscale = 0,
    Truecolour = 2,
    Indexed_colour = 3,
    Greyscale_with_alpha = 4,
    Truecolour_with_alpha = 6
};

// Yes, I could make these be the IHDR_ChunkTypeValue stuff right away
// But I'm using this enum to map into an array when parsing chunks, so fuck you.
enum class Texas::detail::PNG::ChunkType : std::uint8_t
{
    Invalid,

    // Mandatory

    IHDR,
    PLTE,
    IDAT,
    IEND,

    // Optional

    cHRM,
    gAMA,
    iCCP,
    sBIT,
    sRGB,
    bKGD,
    hIST,
    tRNS,
    pHYs,
    sPLT,
    tIME,
    iTXt,
    tEXt,
    zTXt,

    COUNT
};

enum class Texas::detail::PNG::FilterType : std::uint8_t
{
    None = 0,
    Sub = 1,
    Up = 2,
    Average = 3,
    Paeth = 4,
};

static inline std::uint32_t Texas::detail::PNG::toCorrectEndian_u32(const std::byte* const ptr)
{
    std::uint32_t temp[4] = {
        static_cast<std::uint32_t>(ptr[0]),
        static_cast<std::uint32_t>(ptr[1]),
        static_cast<std::uint32_t>(ptr[2]),
        static_cast<std::uint32_t>(ptr[3]),
    };
    return temp[3] | (temp[2] << 8) | (temp[1] << 16) | (temp[0] << 24);
}

static inline bool Texas::detail::PNG::validateColorTypeAndBitDepth(const PNG::ColorType colorType, const std::uint8_t bitDepth)
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

static inline Texas::detail::PNG::ChunkType Texas::detail::PNG::getChunkType(const std::byte* in)
{
    const std::uint32_t value = PNG::toCorrectEndian_u32(in);
    switch (value)
    {
    case IHDR_ChunkTypeValue:
        return ChunkType::IHDR;
    case PLTE_ChunkTypeValue:
        return ChunkType::PLTE;
    case IDAT_ChunkTypeValue:
        return ChunkType::IDAT;
    case IEND_ChunkTypeValue:
        return ChunkType::IEND;
    case cHRM_ChunkTypeValue:
        return ChunkType::cHRM;
    case gAMA_ChunkTypeValue:
        return ChunkType::gAMA;
    case iCCP_ChunkTypeValue:
        return ChunkType::iCCP;
    case sBIT_ChunkTypeValue:
        return ChunkType::sBIT;
    case sRGB_ChunkTypeValue:
        return ChunkType::sRGB;
    case bKGD_ChunkTypeValue:
        return ChunkType::bKGD;
    case hIST_ChunkTypeValue:
        return ChunkType::hIST;
    case tRNS_ChunkTypeValue:
        return ChunkType::tRNS;
    case pHYs_ChunkTypeValue:
        return ChunkType::pHYs;
    case sPLT_ChunkTypeValue:
        return ChunkType::sPLT;
    case tIME_ChunkTypeValue:
        return ChunkType::tIME;
    case iTXt_ChunkTypeValue:
        return ChunkType::iTXt;
    case tEXt_ChunkTypeValue:
        return ChunkType::tEXt;
    case zTXt_ChunkTypeValue:
        return ChunkType::zTXt;
    default:
        return ChunkType::Invalid;
    }
}

static inline Texas::PixelFormat Texas::detail::PNG::toPixelFormat(PNG::ColorType colorType, std::uint8_t bitDepth)
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
            return PixelFormat::RA_8;
        case 16:
            return PixelFormat::RA_16;
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

std::uint64_t Texas::detail::PNG::calcWorkingMemRequired(
    Dimensions baseDims,
    PixelFormat pFormat,
    bool isIndexed) noexcept
{
    if (isIndexed)
    {
        // For indexed, we inflate all rows including the 1 byte for filter-type at the start of each row.
        // And then we defilter in place, and de-index directly onto destination image-buffer.
        // Each pixel here is 1 byte. Each row is +1 byte for the filtertype.
        return baseDims.width * baseDims.height + baseDims.height;
    }
    else
    {
        // For indexed, we inflate all rows including the 1 byte for filter-type at the start of each row.
        // And then we defilter directly onto dstImgBuffer.
        // Each row is +1 byte for the filtertype.
        return baseDims.width * baseDims.height * getPixelWidth(pFormat) + baseDims.height;
    }
}

static inline std::uint8_t Texas::detail::PNG::getPixelWidth(PixelFormat pixelFormat)
{
    switch (pixelFormat)
    {
    case PixelFormat::R_8:
        return 1;
    case PixelFormat::RA_8:
        return 2;
    case PixelFormat::RGB_8:
        return 3;
    case PixelFormat::RGBA_8:
        return 4;
    }

    return 0;
}

static inline std::uint8_t Texas::detail::PNG::paethPredictor(std::uint8_t a, std::uint8_t b, std::uint8_t c)
{
    const int p = int(a) + b - c;

    int pa = p - a;
    if (pa < 0)
        pa = -pa;
    int pb = p - b;
    if (pb < 0)
        pb = -pb;
    int pc = p - c;
    if (pc < 0)
        pc = -pc;

    if (pa <= pb && pa <= pc)
        return a;
    else if (pb <= pc)
        return b;
    else
        return c;
}

Texas::Result Texas::detail::PNG::loadFromBuffer_Step1(
    ConstByteSpan srcBuffer,
    TextureInfo& textureInfo,
    std::uint64_t& workingMemRequired,
    detail::ParsedFileInfo_PNG_BackendData& backendData)
{
    backendData = detail::ParsedFileInfo_PNG_BackendData();
    backendData.srcFileBufferStart = reinterpret_cast<const unsigned char*>(srcBuffer.data());
    backendData.srcFileBufferLength = srcBuffer.size();

    // Check if srcBuffer is large enough hold the header, and more to fit the rest of the chunks
    if (srcBuffer.size() <= Header::totalSize)
        return { ResultType::PrematureEndOfFile, "Source buffer is too small to hold PNG header-data, let alone any image data." };

    textureInfo.fileFormat = FileFormat::PNG;
    textureInfo.textureType = TextureType::Texture2D;
    textureInfo.baseDimensions.depth = 1;
    textureInfo.arrayLayerCount = 1;
    textureInfo.mipLevelCount = 1;
    textureInfo.colorSpace = ColorSpace::Linear;
    textureInfo.channelType = ChannelType::UnsignedNormalized;

    const std::uint32_t ihdrChunkDataSize = PNG::toCorrectEndian_u32(srcBuffer.data() + Header::ihdrChunkSizeOffset);
    if (ihdrChunkDataSize != Header::ihdrChunkDataSize)
        return { ResultType::CorruptFileData, "PNG IHDR chunk data size does not equal 13. PNG specification requires it to be 13." };

    const std::byte* const ihdrChunkType = srcBuffer.data() + Header::ihdrChunkTypeOffset;
    if (PNG::getChunkType(ihdrChunkType) != PNG::ChunkType::IHDR)
        return { ResultType::CorruptFileData, "PNG first chunk is not of type 'IHDR'. PNG requires the 'IHDR' chunk to appear first." };

    // Dimensions are stored in big endian, we must convert to correct endian.
    const std::uint32_t origWidth = PNG::toCorrectEndian_u32(srcBuffer.data() + Header::widthOffset);
    if (origWidth == 0)
        return { ResultType::CorruptFileData, "PNG IHDR field 'Width' is equal to 0. PNG specification requires it to be >0." };
    textureInfo.baseDimensions.width = origWidth;
    // Dimensions are stored in big endian, we must convert to correct endian.
    const std::uint32_t origHeight = PNG::toCorrectEndian_u32(srcBuffer.data() + Header::heightOffset);
    if (origHeight == 0)
        return { ResultType::CorruptFileData, "PNG IHDR field 'Height' is equal to 0. PNG specification requires it to be >0." };
    textureInfo.baseDimensions.height = origHeight;

    const std::uint8_t bitDepth = static_cast<std::uint8_t>(srcBuffer.data()[Header::bitDepthOffset]);
    const PNG::ColorType colorType = static_cast<PNG::ColorType>(srcBuffer.data()[Header::colorTypeOffset]);
    if (PNG::validateColorTypeAndBitDepth(colorType, bitDepth) == false)
        return { ResultType::CorruptFileData, "PNG does not allow this combination of values from "
                                              "IHDR fields 'Colour type' and 'Bit depth'." };
    if (bitDepth != 8)
        return { ResultType::FileNotSupported, "Texas does not support PNG files where bit-depth is not 8." };
    textureInfo.pixelFormat = PNG::toPixelFormat(colorType, bitDepth);
    if (textureInfo.pixelFormat == PixelFormat::Invalid)
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
    std::uint64_t memOffsetTracker = Header::totalSize;
    std::uint8_t chunkTypeCount[(int)PNG::ChunkType::COUNT] = {};
    PNG::ChunkType previousChunkType = PNG::ChunkType::Invalid;
    while (memOffsetTracker < srcBuffer.size() && chunkTypeCount[(int)PNG::ChunkType::IEND] == 0)
    {
        // 12 bytes is the smallest a chunk can be
        if (memOffsetTracker + 12 >= srcBuffer.size())
            return { ResultType::CorruptFileData, "Encountered unexpected end of file while parsing PNG chunks." };

        const std::byte* const chunkStart = srcBuffer.data() + memOffsetTracker;

        // Chunk data length is the first entry in the chunk. It's a uint32_t
        const std::uint32_t chunkDataLength = PNG::toCorrectEndian_u32(chunkStart);
        // Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
        const PNG::ChunkType chunkType = PNG::getChunkType(chunkStart + sizeof(PNG::ChunkSize_T));
        if (chunkType == PNG::ChunkType::Invalid)
            return { ResultType::CorruptFileData, "Encountered a PNG chunk type not defined in the PNG specification." };

        switch (chunkType)
        {
        case ChunkType::IDAT:
        {
            if (previousChunkType != ChunkType::IDAT && chunkTypeCount[(int)PNG::ChunkType::IDAT] > 1)
                return { ResultType::CorruptFileData, "PNG IDAT chunk appeared when a chain of IDAT chunk(s) has already been found. "
                                                      "PNG specification requires that all IDAT chunks appear consecutively." };
            if (chunkDataLength == 0)
                return { ResultType::CorruptFileData, "PNG IDAT chunk's `Length' field is 0. PNG specification requires it to be >0." };

            backendData.idatChunkOffset = memOffsetTracker;
        }
            break;
        case ChunkType::IEND:
        {
            if (chunkTypeCount[(int)PNG::ChunkType::IDAT] == 0)
                return { ResultType::CorruptFileData, "PNG IEND chunk appears before any IDAT chunk. "
                                                      "PNG specification requires IEND to be the last chunk." };
            if (chunkDataLength != 0)
                return { ResultType::CorruptFileData , "PNG IEND chunk's data field is non-zero. "
                                                       "PNG specification requires IEND chunk's field 'Data length' to be 0."};
        }
            break;
        case ChunkType::PLTE:
        {
            if (chunkTypeCount[(int)PNG::ChunkType::PLTE] > 0)
                return { ResultType::CorruptFileData, "Encountered a second PLTE chunk in PNG file. "
                                                      "PNG specification requires that only one PLTE chunk exists in file." };
            if (chunkTypeCount[(int)PNG::ChunkType::IDAT] > 0)
                return { ResultType::CorruptFileData , "PNG PLTE chunk appeared after any IDAT chunks. "
                                                       "PNG specification requires PLTE chunk to appear before any IDAT chunk(s)." };
            if (chunkDataLength == 0)
                return { ResultType::CorruptFileData , "PNG PLTE chunk has field 'Data length' equal to 0. "
                                                       "PNG specification requires PLTE data length to be non-zero. " };
            if (chunkDataLength > 768)
                return { ResultType::CorruptFileData , "PNG PLTE chunk has field 'Data length' higher than 768 bytes. "
                                                       "PNG specification requires PLTE data length to be smaller than or equal to 768 bytes." };
            if (chunkDataLength % 3 != 0)
                return { ResultType::CorruptFileData , "PNG PLTE chunk field 'Data length' value not divisible by 3. "
                                                       "PNG specification requires PLTE data length be divisible by 3." };

            backendData.plteChunkOffset = memOffsetTracker;
            backendData.plteChunkDataLength = chunkDataLength;
        }
            break;
        case ChunkType::sRGB:
        {
            if (chunkTypeCount[(int)PNG::ChunkType::sRGB] > 0)
                return { ResultType::CorruptFileData, "Encountered a second sRGB chunk in PNG file. "
                                                      "PNG specification requires that only one sRGB chunk exists in file." };
            if (chunkTypeCount[(int)PNG::ChunkType::IDAT] > 0)
                return { ResultType::CorruptFileData, "PNG sRGB chunk appeared after IDAT chunk(s). "
                                                      "PNG specification requires sRGB chunk to appear before any IDAT chunk." };
            if (colorType == PNG::ColorType::Indexed_colour && chunkTypeCount[(int)PNG::ChunkType::PLTE] > 0)
                return { ResultType::CorruptFileData, "PNG sRGB chunk appeared after a PLTE chunk. "
                                                      "PNG specification requires sRGB chunk to appear before any PLTE chunk when IHDR field 'Colour type' equals 'Indexed colour'." };
            if (chunkTypeCount[(int)PNG::ChunkType::iCCP] > 0)
                return { ResultType::CorruptFileData, "PNG sRGB chunk appeared when a iCCP chunk has already been found. "
                                                      "PNG specification requires that only of one either sRGB or iCCP chunks may exist." };
            if (chunkDataLength != 1)
                return { ResultType::CorruptFileData , "PNG sRGB chunk's data field is not equal to 1. "
                                                       "PNG specification requires sRGB chunk's field 'Data length' to be 1." };

            textureInfo.colorSpace = ColorSpace::sRGB;
        }
            break;
        case ChunkType::gAMA:
        {
            if (chunkTypeCount[(int)PNG::ChunkType::gAMA] > 0)
                return { ResultType::CorruptFileData, "Encountered a second gAMA chunk in PNG file. "
                                                      "PNG specification requires that only one gAMA chunk exists in file." };
            if (colorType == PNG::ColorType::Indexed_colour && chunkTypeCount[(int)PNG::ChunkType::PLTE] > 0)
                return { ResultType::CorruptFileData, "PNG gAMA chunk appeared after a PLTE chunk. "
                                                      "PNG specification requires gAMA chunk to appear before any PLTE chunk when IHDR field 'Colour type' equals 'Indexed colour'." };
            if (chunkTypeCount[(int)PNG::ChunkType::IDAT] > 0)
                return { ResultType::CorruptFileData, "PNG gAMA chunk appeared after IDAT chunk(s). "
                                                      "PNG specification requires gAMA chunk to appear before any IDAT chunk." };
            if (chunkDataLength != 4)
                return { ResultType::CorruptFileData, "Chunk data length of PNG gAMA chunk is not equal to 4. "
                                                      "PNG specification demands that chunk data length of gAMA chunk is equal to 4." };

            // TODO: At some point, TextureInfo might contain gamma. Catch it here

            
        }
            break;
        };
        
        memOffsetTracker += sizeof(PNG::ChunkSize_T) + sizeof(PNG::ChunkType_T) + chunkDataLength + sizeof(PNG::ChunkCRC_T);
        chunkTypeCount[(int)chunkType] += 1;
        previousChunkType = chunkType;
    }

    if (chunkTypeCount[(int)PNG::ChunkType::IDAT] == 0)
        return { ResultType::CorruptFileData, "Found no IDAT chunk in PNG file. PNG specification requires the file to have atleast one IDAT chunk." };
    if (chunkTypeCount[(int)PNG::ChunkType::IEND] == 0)
        return { ResultType::CorruptFileData, "Found no IEND chunk in PNG file. PNG specification requires the file to have atleast one IEND chunk." };
    if (colorType == PNG::ColorType::Indexed_colour && chunkTypeCount[(int)PNG::ChunkType::PLTE] == 0)
        return { ResultType::CorruptFileData, "Found no PLTE chunk in PNG file with color-type 'Indexed colour'. "
                                              "PNG specification requires a PLTE chunk to exist when color-type is 'Indexed colour'" };

    backendData.idatChunkCount = chunkTypeCount[(int)PNG::ChunkType::IDAT];

    const bool isIndexedColor = colorType == ColorType::Indexed_colour;
    workingMemRequired = calcWorkingMemRequired(textureInfo.baseDimensions, textureInfo.pixelFormat, isIndexedColor);

    return { ResultType::Success, nullptr };
}

Texas::Result Texas::detail::PNG::loadFromBuffer_Step2_Deindex(
    const Dimensions baseDims,
    const std::byte* const plteChunkStart,
    const unsigned int previous_plteChunkDataLength, 
    const ByteSpan dstImageBuffer,
    const ByteSpan workingMem)
{
    // Unindex the data
    const std::uint32_t plteChunkDataLength = PNG::toCorrectEndian_u32(plteChunkStart);
    // Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
    const PNG::ChunkType chunkType = PNG::getChunkType(plteChunkStart + sizeof(PNG::ChunkSize_T));
    if (chunkType != PNG::ChunkType::PLTE)
        return { ResultType::CorruptFileData, "Found no PLTE chunk when deindexing PNG file. "
                                              "The file has changed since Texas::parseBuffer() was called." };
    if (plteChunkDataLength != previous_plteChunkDataLength)
        return { ResultType::CorruptFileData, "PLTE chunk data length is not the same as it was when parsing the PNG file. "
                                              "The file has changed since Texas::parseBuffer() was called." };

    const std::byte* const paletteColors = plteChunkStart + sizeof(PNG::ChunkSize_T) + sizeof(PNG::ChunkType_T);
    const unsigned int paletteColorCount = plteChunkDataLength / 3;

    for (std::uint_least32_t y = 0; y < static_cast<std::uint_least32_t>(baseDims.height); y++)
    {
        const std::uint64_t rowIndicesOffset = 1 + (y * (baseDims.height + 1));
        // Pointer to the row of indices, does not include the filter-type byte.
        const std::byte* const rowIndices = workingMem.data() + rowIndicesOffset;

        for (std::uint_least32_t x = 0; x < static_cast<std::uint_least32_t>(baseDims.width); x++)
        {
            const std::uint8_t paletteIndex = std::uint8_t(rowIndices[x]);
            if (paletteIndex >= paletteColorCount)
                return { ResultType::CorruptFileData, "Encountered an out-of-bounds index while de-indexing PNG file." };
            const std::byte* colorPalettePtr = paletteColors + static_cast<std::size_t>(paletteIndex) * 3;

            const std::size_t dstBufferOffset = (y * static_cast<std::size_t>(baseDims.height) + x) * 3;
            std::memcpy(dstImageBuffer.data() + dstBufferOffset, colorPalettePtr, 3);
        }
    }

    return { ResultType::Success, nullptr };
}

Texas::Result Texas::detail::PNG::loadFromBuffer_Step2(
    const TextureInfo& textureInfo,
    detail::ParsedFileInfo_PNG_BackendData& backendData,
    const ByteSpan dstImageBuffer,
    const ByteSpan workingMem)
{
    z_stream zLibDecompressJob{};

    zLibDecompressJob.next_out = reinterpret_cast<Bytef*>(workingMem.data());
    zLibDecompressJob.avail_out = static_cast<uInt>(workingMem.size());

    const int initErr = inflateInit(&zLibDecompressJob);
    if (initErr != Z_OK)
    {
        inflateEnd(&zLibDecompressJob);
        return { ResultType::CorruptFileData, "During PNG decompression, zLib failed to initialize the decompression job." };
    }

    // Decompress every IDAT chunk
    {
        std::uint64_t memOffsetTracker = 0;
        while (true)
        {
            // An IDAT chunk must be atleast 13 bytes to be valid.
            // The file also needs to fit an IEND afterwards, which is 12 bytes.
            // So we need atleast 25 more bytes for the file to be valid
            if (backendData.idatChunkOffset + 25 >= backendData.srcFileBufferLength)
                return { ResultType::CorruptFileData, "Encountered unexpected end of file while decompressing PNG IDAT chunks." };

            const std::byte* const chunkStart = reinterpret_cast<const std::byte*>(backendData.srcFileBufferStart + backendData.idatChunkOffset + memOffsetTracker);

            // Chunk data length is the first entry in the chunk. It's a uint32_t
            const std::uint32_t chunkDataLength = PNG::toCorrectEndian_u32(chunkStart);
            // Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
            const PNG::ChunkType chunkType = PNG::getChunkType(chunkStart + sizeof(PNG::ChunkSize_T));
            if (chunkType != PNG::ChunkType::IDAT)
                return { ResultType::CorruptFileData, "Found no IDAT chunk when decompressing PNG file. "
                                                      "The file has changed since Texas::parseBuffer() was called." };
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
            if (err == Z_OK)
            {
                // more IDAT chunks to decompress
            }

            memOffsetTracker += sizeof(PNG::ChunkSize_T) + sizeof(PNG::ChunkType_T) + chunkDataLength + sizeof(PNG::ChunkCRC_T);
        }
    }

    if (backendData.plteChunkOffset == 0)
    {
        // If the plteChunk is nullptr, then we are not dealing with indexed colors.
        Result result = loadFromBuffer_Step2_DefilterIntoDstBuffer(textureInfo, dstImageBuffer, workingMem);
        if (!result.isSuccessful())
            return result;
    }
    else
    {
        // We are dealing with indexed colours. We defilter all the pixels (indices) in place.
        Result result = loadFromBuffer_Step2_DefilterInPlace(textureInfo.baseDimensions, workingMem);
        if (!result.isSuccessful())
            return result;

        result = loadFromBuffer_Step2_Deindex(
            textureInfo.baseDimensions,
            reinterpret_cast<const std::byte*>(backendData.srcFileBufferStart + backendData.plteChunkOffset),
            backendData.plteChunkDataLength,
            dstImageBuffer, 
            workingMem);
        if (!result.isSuccessful())
            return result;
    }

    return { ResultType::Success, nullptr };
}

static inline Texas::Result Texas::detail::PNG::loadFromBuffer_Step2_DefilterIntoDstBuffer(const TextureInfo& textureInfo, const ByteSpan dstMem, const ByteSpan uncompressedData)
{
    const std::byte* const filteredData = uncompressedData.data();
    std::byte* const dstBuffer = dstMem.data();
    
    // Size is in bytes.
    const std::uint8_t pixelWidth = PNG::getPixelWidth(textureInfo.pixelFormat);
    // Size is in bytes
    // Does not include the byte for filter-type.
    const std::uint64_t rowWidth = pixelWidth * textureInfo.baseDimensions.width;
    // Size is in bytes
    // Includes the byte for filter-type.
    const std::uint64_t totalRowWidth = rowWidth + 1;

    // Unfilter first row
    {
        PNG::FilterType filterType = static_cast<PNG::FilterType>(filteredData[0]);
        switch (filterType)
        {
        case FilterType::None:
        case FilterType::Up:
            // If FilterType is None, then we just copy the entire row as it is over.
            // If FilterType is Up, we copy all the bytes of the previous, but since there is no row above, 
            // it just adds 0 to the entire row, so we just copy all of it over to destination buffer.
            std::memcpy(dstBuffer, &filteredData[1], rowWidth);
            break;
        case FilterType::Sub:
            // Copy first pixel of the row.
            std::memcpy(dstBuffer, &filteredData[1], pixelWidth);
            // Then do the Sub defiltering on all the rest of the pixels in the row
            for (std::uint64_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
            {
                // We offset by 1 because we have to jump over the byte that contains filtertype
                const std::uint8_t filtX = std::uint8_t(filteredData[1 + widthByte]);
                const std::uint8_t reconA = std::uint8_t(dstBuffer[widthByte - pixelWidth]);
                dstBuffer[widthByte] = std::byte(filtX + reconA);
            }
            break;
        case FilterType::Average:
            // This Filter does work based on the pixel to the left, and the pixel above.
            // Neither exists for the first pixel on the first row, so we just copy it over.
            std::memcpy(dstBuffer, &filteredData[1], pixelWidth);
            // Do Average defiltering on the rest of the bytes.
            // We don't use the Recon(b) value because it's always 0 inside the first row.
            for (std::uint64_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
            {
                // We offset by 1 to jump over the byte containing filtertype
                const std::uint8_t filtX = std::uint8_t(filteredData[1 + widthByte]);
                const std::uint8_t reconA = std::uint8_t(dstBuffer[widthByte - pixelWidth]);
                dstBuffer[widthByte] = std::byte(filtX + reconA / 2);
            }
            break;
        case FilterType::Paeth:
            // We have no work to do on the first pixel of the first row, so we just copy it over
            // We offset by one to take the filtertype into account
            std::memcpy(dstBuffer, &filteredData[1], pixelWidth);
            // Traverse every byte of this row after the first pixel
            // widthByte does not take into account the first byte of the scanline that holds filtertype
            // Recon(b) and Recon(c) exist on the previous scanline,
            // but there is not previous scanline for first row. So we just use 0.
            for (std::size_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
            {
                // We offset by 1 to jump over the byte containing filtertype
                const std::uint8_t filtX = std::uint8_t(filteredData[1 + widthByte]);
                const std::uint8_t reconA = std::uint8_t(dstBuffer[widthByte - pixelWidth]); // Visual Studio says there's something wrong with this line.
                dstBuffer[widthByte] = std::byte(filtX + reconA);
            }
            break;
        default:
            return { ResultType::CorruptFileData, "Encountered unknown filter-type when defiltering PNG imagedata." };
        }
    }

    // Defilter rest of the rows
    for (unsigned int y = 1; y < textureInfo.baseDimensions.height; y++)
    {
        const std::uint64_t filterTypeOffset = y * totalRowWidth;
        
        // This is where the filtered data start after the filtertype-byte starts.
        const std::uint64_t filterRowOffset = filterTypeOffset + 1;
        // This is where the unfiltered data starts.
        const std::uint64_t unfilterRowOffset = filterTypeOffset - y;

        const PNG::FilterType filterType = static_cast<PNG::FilterType>(filteredData[filterTypeOffset]);
        switch (filterType)
        {
        case FilterType::None:
            // Copy all the pixels in the row
            std::memcpy(&dstBuffer[unfilterRowOffset], &filteredData[filterRowOffset], rowWidth);
            break;
        case FilterType::Sub:
            // Copy first pixel of the row, since Recon(b) is 0 anyways.
            std::memcpy(&dstBuffer[unfilterRowOffset], &filteredData[filterRowOffset], pixelWidth);
            // Then defilter the rest of the row.
            for (std::uint64_t widthByte = pixelWidth; widthByte < rowWidth + 1; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
                const std::uint8_t reconA = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - pixelWidth]);
                dstBuffer[unfilterRowOffset + widthByte] = std::byte(filterX + reconA);
            }
            break;
        case FilterType::Up:
            for (std::uint64_t widthByte = 0; widthByte < rowWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
                const std::uint8_t reconB = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - rowWidth]);
                dstBuffer[unfilterRowOffset + widthByte] = std::byte(filterX + reconB);
            }
            break;
        case FilterType::Average:
            // First traverse the first pixel of this row.
            // We do this because for the first pixel, Recon(a) will always be 0.
            for (std::uint64_t widthByte = 0; widthByte < pixelWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
                const std::uint8_t reconB = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - rowWidth]);
                dstBuffer[unfilterRowOffset + widthByte] = std::byte(filterX + reconB / 2);
            }
            // Then we perform the defiltering for the rest of the row.
            for (std::uint64_t widthByte = pixelWidth; widthByte < rowWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
                const std::uint8_t reconA = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - pixelWidth]);
                const std::uint8_t reconB = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - rowWidth]);
                dstBuffer[unfilterRowOffset + widthByte] = static_cast<std::byte>(filterX + (reconA + reconB) / 2);
            }
            break;
        case FilterType::Paeth:
            // Traverse every byte of the first pixel in this row.
            // We do this because Recon(a) and Recon(c) will always be 0 for the first pixel of a row.
            for (std::uint64_t widthByte = 0; widthByte < pixelWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
                const std::uint8_t reconB = std::uint8_t(dstBuffer[unfilterRowOffset + widthByte - rowWidth]);
                dstBuffer[unfilterRowOffset + widthByte] = std::byte(filterX + reconB);
            }
            // Traverse every byte of this row after the first pixel
            for (std::uint64_t xByte = pixelWidth; xByte < rowWidth; xByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filteredData[filterRowOffset + xByte]);
                const std::uint8_t reconA = std::uint8_t(dstBuffer[unfilterRowOffset + xByte - pixelWidth]); // Visual Studio says there's something wrong with this line.
                const std::uint8_t reconB = std::uint8_t(dstBuffer[unfilterRowOffset + xByte - rowWidth]);
                const std::uint8_t reconC = std::uint8_t(dstBuffer[unfilterRowOffset + xByte - rowWidth - pixelWidth]);
                dstBuffer[unfilterRowOffset + xByte] = static_cast<std::byte>(filterX + PNG::paethPredictor(reconA, reconB, reconC));
            }
            break;
        default:
            return { ResultType::CorruptFileData, "Encountered unknown filter-type when defiltering PNG imagedata." };
        }
    }

    return { ResultType::Success, nullptr };
}

[[nodiscard]] static inline Texas::Result Texas::detail::PNG::loadFromBuffer_Step2_DefilterInPlace(const Dimensions& baseDims, const ByteSpan uncompressedData)
{
    // Start of the buffer that includes all filtered bytes, includes all filter-type bytes.
    std::byte* const filterData = uncompressedData.data();

    // Size in bytes
    // This does not include the byte for holding filter-type
    const std::uint64_t rowWidth = baseDims.width;
    // Size in bytes
    // This includes the byte for holding filter-type
    const std::uint64_t totalRowWidth = baseDims.width + 1;

    // Unfilter first row
    {
        // This is where the filtered data start excluding the filtertype-byte.
        const std::uint64_t filterRowOffset = 1;
        PNG::FilterType filterType = static_cast<PNG::FilterType>(filterData[0]);
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
            for (std::uint64_t widthByte = 1; widthByte < rowWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filterData[filterRowOffset + widthByte]);
                const std::uint8_t reconA = std::uint8_t(filterData[filterRowOffset + widthByte - 1]);
                filterData[filterRowOffset + widthByte] = std::byte(filterX + reconA);
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
            for (std::uint64_t widthByte = 1; widthByte < rowWidth; widthByte++)
            {
                // We offset by 1 to jump over the byte containing filtertype
                const std::uint8_t filtX = std::uint8_t(filterData[filterRowOffset + widthByte]);
                const std::uint8_t reconA = std::uint8_t(filterData[filterRowOffset + widthByte - 1]);
                filterData[filterRowOffset + widthByte] = std::byte(filtX + reconA / 2);
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
            for (std::uint64_t widthByte = 1; widthByte < rowWidth; widthByte++)
            {
                const std::uint8_t filtX = std::uint8_t(filterData[filterRowOffset + widthByte]);
                const std::uint8_t reconA = std::uint8_t(filterData[filterRowOffset + widthByte - 1]);
                filterData[filterRowOffset + widthByte] = std::byte(filtX + reconA);
            }
            break;
        default:
            return { ResultType::CorruptFileData, "Encountered unknown filter-type when defiltering PNG imagedata." };
        }
    }

    // Defilter rest of the rows
    for (unsigned int y = 1; y < baseDims.height; y++)
    {
        const std::uint64_t filterTypeOffset = y * totalRowWidth;
        // This is where the filtered data start excluding the filtertype-byte.
        const std::uint64_t filterRowOffset = filterTypeOffset + 1;

        const PNG::FilterType filterType = static_cast<PNG::FilterType>(filterData[filterTypeOffset]);
        switch (filterType)
        {
        case FilterType::None:
            // If FilterType is None, then we do nothing.
            break;
        case FilterType::Sub:
            // Do nothing with the first pixel (index) of the row, since Recon(a) is 0 anyways.
            // Then defilter the rest of the row.
            // And we offset by 1 to take into account that we skipped the first pixel (index)
            for (std::uint64_t widthByte = 1; widthByte < rowWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filterData[filterRowOffset + widthByte]);
                const std::uint8_t reconA = std::uint8_t(filterData[filterRowOffset + widthByte - 1]);
                filterData[filterRowOffset + widthByte] = std::byte(filterX + reconA);
            }
            break;
        case FilterType::Up:
            for (std::uint64_t widthByte = 1; widthByte < rowWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filterData[filterRowOffset + widthByte]);
                const std::uint8_t reconB = std::uint8_t(filterData[filterRowOffset + widthByte - totalRowWidth]);
                filterData[filterRowOffset + widthByte] = std::byte(filterX + reconB);
            }
            break;
        case FilterType::Average:
            // First defilter the first pixel (index) of the row.
            // We don't need a loop because the pixel is only 1 byte wide anyways
            // since it's an index.
            // We do this because for the first pixel because Recon(a) will always be 0.
            {
                const std::uint8_t filterX = std::uint8_t(filterData[filterRowOffset + 1]);
                const std::uint8_t reconB = std::uint8_t(filterData[filterRowOffset - totalRowWidth]);
                filterData[filterRowOffset] = std::byte(filterX + reconB / 2);
            }

            // Then we perform the defiltering for the rest of the row.
            // We offset by 1 because we skip the first pixel
            for (std::uint64_t widthByte = 1; widthByte < rowWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filterData[filterRowOffset + widthByte]);
                const std::uint8_t reconA = std::uint8_t(filterData[filterRowOffset + widthByte - 1]);
                const std::uint8_t reconB = std::uint8_t(filterData[filterRowOffset + widthByte - totalRowWidth]);
                filterData[filterRowOffset + widthByte] = std::byte(filterX + (reconA + reconB) / 2);
            }
            break;
        case FilterType::Paeth:
            // First defilter the first pixel (index) of the row.
            // We do this because Recon(a) and Recon(c) will always be 0 for the first pixel of a row.
            {
                const std::uint8_t filterX = std::uint8_t(filterData[filterRowOffset]);
                const std::uint8_t reconB = std::uint8_t(filterData[filterRowOffset - totalRowWidth]);
                filterData[filterRowOffset] = std::byte(filterX + reconB);
            }
            // Traverse every byte of this row after the first pixel
            for (std::uint64_t widthByte = 1; widthByte < rowWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filterData[filterRowOffset + widthByte]);
                const std::uint8_t reconA = std::uint8_t(filterData[filterRowOffset + widthByte - 1]); // Visual Studio says there's something wrong with this line.
                const std::uint8_t reconB = std::uint8_t(filterData[filterRowOffset + widthByte - totalRowWidth]);
                const std::uint8_t reconC = std::uint8_t(filterData[filterRowOffset + widthByte - totalRowWidth - 1]);
                filterData[filterRowOffset + widthByte] = std::byte(filterX + PNG::paethPredictor(reconA, reconB, reconC));
            }
            break;
        default:
            return { ResultType::CorruptFileData, "Encountered unknown filter-type when defiltering PNG imagedata." };
        }
    }

    return { ResultType::Success, nullptr };
}