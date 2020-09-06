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

    [[nodiscard]] static constexpr std::uint32_t setupChunkTypeValue(
        std::uint32_t a, 
        std::uint32_t b, 
        std::uint32_t c, 
        std::uint32_t d)
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

    enum class ColorType : char;
    enum class ChunkType : char;
    enum class FilterType : char;

    // Turns a 32-bit unsigned integer into correct endian, regardless of system endianness.
    [[nodiscard]] static std::uint32_t toCorrectEndian_u32(std::byte const* ptr) noexcept;

    [[nodiscard]] static bool validateColorTypeAndBitDepth(PNG::ColorType colorType, std::uint8_t bitDepth) noexcept;

    [[nodiscard]] static PNG::ChunkType getChunkType(std::byte const* in) noexcept;

    [[nodiscard]] static PixelFormat toPixelFormat(PNG::ColorType colorType, std::uint8_t bitDepth) noexcept;

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

enum class Texas::detail::PNG::FilterType : char
{
    None = 0,
    Sub = 1,
    Up = 2,
    Average = 3,
    Paeth = 4,
};

static std::uint32_t Texas::detail::PNG::toCorrectEndian_u32(std::byte const* ptr)  noexcept
{
    std::uint32_t temp[4] = {
        static_cast<std::uint32_t>(ptr[0]),
        static_cast<std::uint32_t>(ptr[1]),
        static_cast<std::uint32_t>(ptr[2]),
        static_cast<std::uint32_t>(ptr[3]),
    };
    return temp[3] | (temp[2] << 8) | (temp[1] << 16) | (temp[0] << 24);
}

static bool Texas::detail::PNG::validateColorTypeAndBitDepth(
    PNG::ColorType colorType, 
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

static Texas::detail::PNG::ChunkType Texas::detail::PNG::getChunkType(std::byte const* in)  noexcept
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

static Texas::PixelFormat Texas::detail::PNG::toPixelFormat(
    PNG::ColorType colorType, 
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

Texas::Result Texas::detail::PNG::parseStream(
    InputStream& stream,
    TextureInfo& textureInfo,
    std::uint64_t& workingMemRequired,
    detail::FileInfo_PNG_BackendData& backendData) noexcept
{
    backendData = detail::FileInfo_PNG_BackendData();
    textureInfo.fileFormat = FileFormat::PNG;
    textureInfo.textureType = TextureType::Texture2D;
    textureInfo.baseDimensions.depth = 1;
    textureInfo.layerCount = 1;
    textureInfo.mipCount = 1;
    textureInfo.colorSpace = ColorSpace::Linear;
    textureInfo.channelType = ChannelType::UnsignedNormalized;

    Result result{};

    std::byte headerBuffer[Header::totalSize] = {};
    result = stream.read({ headerBuffer, Header::totalSize });
    if (!result.isSuccessful())
        return result;

    std::uint32_t const ihdrChunkDataSize = PNG::toCorrectEndian_u32(headerBuffer + Header::ihdrChunkSizeOffset);
    if (ihdrChunkDataSize != Header::ihdrChunkDataSize)
        return { ResultType::CorruptFileData, 
                 "PNG IHDR chunk data size does not equal 13. "
                 "PNG specification requires it to be 13." };

    std::byte const* const ihdrChunkType = headerBuffer + Header::ihdrChunkTypeOffset;
    if (PNG::getChunkType(ihdrChunkType) != PNG::ChunkType::IHDR)
        return { ResultType::CorruptFileData, 
                 "PNG first chunk is not of type 'IHDR'. "
                 "PNG requires the 'IHDR' chunk to appear first in the filestream." };

    // Dimensions are stored in big endian, we must convert to correct endian.
    std::uint32_t const origWidth = PNG::toCorrectEndian_u32(headerBuffer + Header::widthOffset);
    if (origWidth == 0)
        return { ResultType::CorruptFileData, 
                 "PNG IHDR field 'Width' is equal to 0. "
                 "PNG specification requires it to be >0." };
    textureInfo.baseDimensions.width = origWidth;
    // Dimensions are stored in big endian, we must convert to correct endian.
    std::uint32_t const origHeight = PNG::toCorrectEndian_u32(headerBuffer + Header::heightOffset);
    if (origHeight == 0)
        return { ResultType::CorruptFileData, 
                 "PNG IHDR field 'Height' is equal to 0. "
                 "PNG specification requires it to be >0." };
    textureInfo.baseDimensions.height = origHeight;

    std::uint8_t const bitDepth = static_cast<std::uint8_t>(headerBuffer[Header::bitDepthOffset]);
    PNG::ColorType const colorType = static_cast<PNG::ColorType>(headerBuffer[Header::colorTypeOffset]);
    if (PNG::validateColorTypeAndBitDepth(colorType, bitDepth) == false)
        return { ResultType::CorruptFileData, 
                 "PNG spec does not allow this combination of values from "
                 "IHDR fields 'Colour type' and 'Bit depth'." };
    if (bitDepth != 8)
        return { ResultType::FileNotSupported, 
                 "Texas does not support PNG files where bit-depth is not 8." };
    textureInfo.pixelFormat = PNG::toPixelFormat(colorType, bitDepth);
    if (textureInfo.pixelFormat == PixelFormat::Invalid)
        return { ResultType::FileNotSupported, 
                 "PNG colortype and bitdepth combination is not supported." };

    std::uint8_t const compressionMethod = static_cast<std::uint8_t>(headerBuffer[Header::compressionMethodOffset]);
    if (compressionMethod != 0)
        return { ResultType::FileNotSupported, "PNG compression method is not supported." };

    std::uint8_t const filterMethod = static_cast<std::uint8_t>(headerBuffer[Header::filterMethodOffset]);
    if (filterMethod != 0)
        return { ResultType::FileNotSupported, "PNG filter method is not supported." };

    std::uint8_t const interlaceMethod = static_cast<std::uint8_t>(headerBuffer[Header::interlaceMethodOffset]);
    if (interlaceMethod != 0)
        return { ResultType::FileNotSupported, "PNG interlace method is not supported." };

    // Move through chunks looking for more metadata until we find IDAT chunk.
    //std::uint64_t memOffsetTracker = Header::totalSize;
    std::uint8_t chunkTypeCounts[(std::size_t)PNG::ChunkType::COUNT] = {};
    PNG::ChunkType previousChunkType = PNG::ChunkType::Invalid;
    while (chunkTypeCounts[(std::size_t)PNG::ChunkType::IEND] == 0)
    {
        std::byte chunkLengthAndTypeBuffer[8] = {};
        result = stream.read({ chunkLengthAndTypeBuffer, 8 });
        if (!result.isSuccessful())
            return result;

        // Chunk data length is the first entry in the chunk. It's a uint32_t
        std::uint32_t const chunkDataLength = PNG::toCorrectEndian_u32(chunkLengthAndTypeBuffer);
        // Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
        PNG::ChunkType const chunkType = PNG::getChunkType(chunkLengthAndTypeBuffer + sizeof(PNG::ChunkSize_T));
        if (chunkType == PNG::ChunkType::Invalid)
            return { ResultType::CorruptFileData, 
                     "Encountered a PNG chunk type not defined in the PNG specification." };

        switch (chunkType)
        {
        case ChunkType::IDAT:
            if (previousChunkType != ChunkType::IDAT && chunkTypeCounts[(std::size_t)PNG::ChunkType::IDAT] > 1)
                return { ResultType::CorruptFileData, 
                         "PNG IDAT chunk appeared when a chain of IDAT chunk(s) has already been found. "
                         "PNG specification requires that all IDAT chunks appear consecutively." };
            if (chunkDataLength == 0)
                return { ResultType::CorruptFileData, 
                         "PNG IDAT chunk's `Length' field is 0. PNG specification requires it to be >0." };

            if (chunkTypeCounts[(std::size_t)PNG::ChunkType::IDAT] == 0)
                // We subtract 8 because the chunk started 8 bytes ago.
                backendData.firstIdatChunkStreamPos = stream.tell() - 8;
            if (chunkDataLength > backendData.maxIdatChunkDataLength)
                backendData.maxIdatChunkDataLength = chunkDataLength;
            break;

        case ChunkType::IEND:
            if (chunkTypeCounts[(std::size_t)PNG::ChunkType::IDAT] == 0)
                return { ResultType::CorruptFileData, 
                         "PNG IEND chunk appears before any IDAT chunk. "
                         "PNG specification requires IEND to be the last chunk." };
            if (chunkDataLength != 0)
                return { ResultType::CorruptFileData , 
                         "PNG IEND chunk's data field is non-zero. "
                         "PNG specification requires IEND chunk's field 'Data length' to be 0."};
            break;

        case ChunkType::PLTE:
            if (chunkTypeCounts[(std::size_t)PNG::ChunkType::PLTE] > 0)
                return { ResultType::CorruptFileData, 
                         "Encountered a second PLTE chunk in PNG file. "
                         "PNG specification requires that only one PLTE chunk exists in file." };
            if (chunkTypeCounts[(std::size_t)PNG::ChunkType::IDAT] > 0)
                return { ResultType::CorruptFileData , 
                         "PNG PLTE chunk appeared after any IDAT chunks. "
                         "PNG specification requires PLTE chunk to appear before any IDAT chunk(s)." };
            if (chunkDataLength == 0)
                return { ResultType::CorruptFileData , 
                         "PNG PLTE chunk has field 'Data length' equal to 0. "
                         "PNG specification requires PLTE data length to be non-zero. " };
            if (chunkDataLength > 768)
                return { ResultType::CorruptFileData , 
                         "PNG PLTE chunk has field 'Data length' higher than 768 bytes. "
                         "PNG specification requires PLTE data length to be smaller than or equal to 768 bytes." };
            if (chunkDataLength % 3 != 0)
                return { ResultType::CorruptFileData , 
                         "PNG PLTE chunk field 'Data length' value not divisible by 3. "
                         "PNG specification requires PLTE data length be divisible by 3." };

            // We subtract 8 bytes to get back to the start of the chunk
            backendData.plteChunkStreamPos = stream.tell() - 8;
            backendData.plteChunkDataLength = chunkDataLength;
            break;

        case ChunkType::sRGB:
            if (chunkTypeCounts[(std::size_t)PNG::ChunkType::sRGB] > 0)
                return { ResultType::CorruptFileData, 
                         "Encountered a second sRGB chunk in PNG file. "
                         "PNG specification requires that only one sRGB chunk exists in file." };
            if (chunkTypeCounts[(std::size_t)PNG::ChunkType::IDAT] > 0)
                return { ResultType::CorruptFileData, 
                         "PNG sRGB chunk appeared after IDAT chunk(s). "
                         "PNG specification requires sRGB chunk to appear before any IDAT chunk." };
            if (colorType == PNG::ColorType::Indexed_colour && chunkTypeCounts[(std::size_t)PNG::ChunkType::PLTE] > 0)
                return { ResultType::CorruptFileData, 
                         "PNG sRGB chunk appeared after a PLTE chunk. "
                         "PNG specification requires sRGB chunk to appear before any "
                         "PLTE chunk when IHDR field 'Colour type' equals 'Indexed colour'." };
            if (chunkTypeCounts[(std::size_t)PNG::ChunkType::iCCP] > 0)
                return { ResultType::CorruptFileData, 
                         "PNG sRGB chunk appeared when a iCCP chunk has already been found. "
                         "PNG specification requires that only of one either sRGB or iCCP chunks may exist." };
            if (chunkDataLength != 1)
                return { ResultType::CorruptFileData , 
                         "PNG sRGB chunk's data field is not equal to 1. "
                         "PNG specification requires sRGB chunk's field 'Data length' to be 1." };

            textureInfo.colorSpace = ColorSpace::sRGB;
            break;

        case ChunkType::gAMA:
            if (chunkTypeCounts[(std::size_t)PNG::ChunkType::gAMA] > 0)
                return { ResultType::CorruptFileData, 
                         "Encountered a second gAMA chunk in PNG file. "
                         "PNG specification requires that only one gAMA chunk exists in file." };
            if (colorType == PNG::ColorType::Indexed_colour && chunkTypeCounts[(std::size_t)PNG::ChunkType::PLTE] > 0)
                return { ResultType::CorruptFileData, 
                         "PNG gAMA chunk appeared after a PLTE chunk. "
                         "PNG specification requires gAMA chunk to appear before any "
                         "PLTE chunk when IHDR field 'Colour type' equals 'Indexed colour'." };
            if (chunkTypeCounts[(std::size_t)PNG::ChunkType::IDAT] > 0)
                return { ResultType::CorruptFileData, "PNG gAMA chunk appeared after IDAT chunk(s). "
                         "PNG specification requires gAMA chunk to appear before any IDAT chunk." };
            if (chunkDataLength != 4)
                return { ResultType::CorruptFileData, "Chunk data length of PNG gAMA chunk is not equal to 4. "
                         "PNG specification demands that chunk data length of gAMA chunk is equal to 4." };

            // TODO: At some point, TextureInfo might contain gamma. Catch it here

            break;

        default:
            break;
        };

        
        // We ignore the chunk data and CRC part of the chunk
        stream.ignore(chunkDataLength + sizeof(ChunkType_T));
        chunkTypeCounts[(std::size_t)chunkType] += 1;
        previousChunkType = chunkType;
    }

    if (chunkTypeCounts[(std::size_t)PNG::ChunkType::IDAT] == 0)
        return { ResultType::CorruptFileData, 
                 "Found no IDAT chunk in PNG file. "
                 "PNG specification requires the file to have atleast one IDAT chunk." };
    if (chunkTypeCounts[(std::size_t)PNG::ChunkType::IEND] == 0)
        return { ResultType::CorruptFileData, 
                 "Found no IEND chunk in PNG file. "
                 "PNG specification requires the file to have exactly one IEND chunk." };
    if (colorType == PNG::ColorType::Indexed_colour && chunkTypeCounts[(std::size_t)PNG::ChunkType::PLTE] == 0)
        return { ResultType::CorruptFileData, 
                 "Found no PLTE chunk in PNG file with color-type 'Indexed colour'. "
                 "PNG specification requires a PLTE chunk to exist when color-type is 'Indexed colour'" };

    //backendData.idatChunkCount = chunkTypeCounts[(int)PNG::ChunkType::IDAT];

    bool const isIndexedColor = colorType == ColorType::Indexed_colour;
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
        PNG::FilterType const filterType = static_cast<PNG::FilterType>(filteredDataPtr[0]);
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

        PNG::FilterType const filterType = static_cast<PNG::FilterType>(filteredDataPtr[filterTypeOffset]);
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
                dstBuffer[unfilterRowOffset + xByte] = std::byte(filterX + PNG::paethPredictor(reconA, reconB, reconC));
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
    PNG::FilterType const filterType = static_cast<PNG::FilterType>(filteredData[0]);
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

        PNG::FilterType const filterType = static_cast<PNG::FilterType>(filteredData[filterTypeOffset]);
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
                const std::uint8_t filterX = std::uint8_t(filteredData[filterRowOffset]);
                const std::uint8_t reconB = std::uint8_t(filteredData[filterRowOffset - totalRowWidth]);
                filteredData[filterRowOffset] = std::byte(filterX + reconB);
            }
            // Traverse every byte of this row after the first pixel
            for (std::size_t widthByte = 1; widthByte < rowWidth; widthByte++)
            {
                const std::uint8_t filterX = std::uint8_t(filteredData[filterRowOffset + widthByte]);
                const std::uint8_t reconA = std::uint8_t(filteredData[filterRowOffset + widthByte - 1]); // Visual Studio says there's something wrong with this line.
                const std::uint8_t reconB = std::uint8_t(filteredData[filterRowOffset + widthByte - totalRowWidth]);
                const std::uint8_t reconC = std::uint8_t(filteredData[filterRowOffset + widthByte - totalRowWidth - 1]);
                filteredData[filterRowOffset + widthByte] = std::byte(filterX + PNG::paethPredictor(reconA, reconB, reconC));
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
            return { ResultType::CorruptFileData, 
            "PNG IDAT chunk data length is 0. "
            "The file has changed since it was parsed."};
        if (chunkDataLength > workingMem.size())
            return { ResultType::CorruptFileData, 
            "PNG IDAT chunk data length is larger than what was recorded when parsing the file. "
            "The file has changed since it was parsed." };

        // Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
        PNG::ChunkType const chunkType = PNG::getChunkType(chunkLengthAndTypeBuffer + sizeof(PNG::ChunkSize_T));
        if (chunkType != PNG::ChunkType::IDAT)
            return { ResultType::CorruptFileData, 
            "Found no IDAT chunk when decompressing PNG file. "
            "The file has changed since it was parsed." };

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
        std::uint32_t const chunkDataLength = PNG::toCorrectEndian_u32(chunkLengthAndTypeBuffer);
        if (chunkDataLength != backendData.plteChunkDataLength)
            return { ResultType::CorruptFileData, 
                     "PLTE chunk data length is different from when PNG was parsed." };

        // Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
        PNG::ChunkType const chunkType = PNG::getChunkType(chunkLengthAndTypeBuffer + sizeof(PNG::ChunkSize_T));
        if (chunkType != PNG::ChunkType::PLTE)
            return { ResultType::CorruptFileData, 
                     "Found no PLTE when de-indexing PNG file. "
                     "The file has changed since it was parsed." };

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