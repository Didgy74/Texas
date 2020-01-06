#include "PNG.hpp"

#include "PrivateAccessor.hpp"

#include "zlib/zlib.h"

namespace Texas::detail::PNG
{
    using ChunkSize_T = std::uint32_t;
    using ChunkType_T = std::uint8_t[4];
    using ChunkCRC_T = std::uint8_t[4];

    [[nodiscard]] static inline constexpr std::uint32_t setupChunkTypeValue(std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d)
    {
        return a << 24 | b << 16 | c << 8 | d;
    }

    constexpr std::uint32_t IHDR_ChunkTypeValue = setupChunkTypeValue(73, 72, 68, 82);
    constexpr std::uint32_t PLTE_ChunkTypeValue = setupChunkTypeValue(80, 76, 84, 69);
    constexpr std::uint32_t IDAT_ChunkTypeValue = setupChunkTypeValue(73, 68, 65, 84);
    constexpr std::uint32_t IEND_ChunkTypeValue = setupChunkTypeValue(73, 69, 78, 68);
    constexpr std::uint32_t cHRM_ChunkTypeValue = setupChunkTypeValue(99, 72, 82, 77);
    constexpr std::uint32_t gAMA_ChunkTypeValue = setupChunkTypeValue(103, 65, 77, 65);
    constexpr std::uint32_t iCCP_ChunkTypeValue = setupChunkTypeValue(105, 67, 67, 80);
    constexpr std::uint32_t sBIT_ChunkTypeValue = setupChunkTypeValue(115, 66, 73, 84);
    constexpr std::uint32_t sRGB_ChunkTypeValue = setupChunkTypeValue(115, 82, 71, 66);
    constexpr std::uint32_t bKGD_ChunkTypeValue = setupChunkTypeValue(98, 75, 71, 68);
    constexpr std::uint32_t hIST_ChunkTypeValue = setupChunkTypeValue(104, 73, 83, 84);
    constexpr std::uint32_t tRNS_ChunkTypeValue = setupChunkTypeValue(116, 82, 78, 83);
    constexpr std::uint32_t pHYs_ChunkTypeValue = setupChunkTypeValue(112, 72, 89, 115);
    constexpr std::uint32_t sPLT_ChunkTypeValue = setupChunkTypeValue(115, 80, 76, 84);
    constexpr std::uint32_t tIME_ChunkTypeValue = setupChunkTypeValue(116, 73, 77, 69);
    constexpr std::uint32_t iTXt_ChunkTypeValue = setupChunkTypeValue(105, 84, 88, 116);
    constexpr std::uint32_t tEXt_ChunkTypeValue = setupChunkTypeValue(116, 69, 88, 116);
    constexpr std::uint32_t zTXt_ChunkTypeValue = setupChunkTypeValue(122, 84, 88, 116);

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

    enum class ChunkType : std::uint32_t;

    // Turns a 32-bit unsigned integer into correct endian, regardless of system endianness.
    [[nodiscard]] static inline std::uint32_t toCorrectEndian_u32(const std::byte* const ptr);

    [[nodiscard]] static inline bool validateColorTypeAndBitDepth(const PNG::ColorType colorType, const std::uint8_t bitDepth);

    [[nodiscard]] static inline PNG::ChunkType getChunkType(const std::byte* const in);

    [[nodiscard]] static inline PixelFormat toPixelFormat(PNG::ColorType colorType, std::uint8_t bitDepth);

    [[nodiscard]] static inline std::uint8_t getPixelWidth(PixelFormat pixelFormat);

    [[nodiscard]] static inline std::uint8_t paethPredictor(std::uint8_t a, std::uint8_t b, std::uint8_t c);

    [[nodiscard]] static inline Result loadFromBuffer_Step2_Defilter(const MetaData& metaData, const ByteSpan dstMem, const ByteSpan workingMem);
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
enum class Texas::detail::PNG::ChunkType : std::uint32_t
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

static inline std::uint8_t Texas::detail::PNG::getPixelWidth(PixelFormat pixelFormat)
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

static inline std::uint8_t Texas::detail::PNG::paethPredictor(std::uint8_t a, std::uint8_t b, std::uint8_t c)
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
    metaData.baseDimensions.width = origWidth;
    // Dimensions are stored in big endian, we must convert to correct endian.
    const std::uint32_t origHeight = PNG::toCorrectEndian_u32(srcBuffer.data() + Header::heightOffset);
    if (origHeight == 0)
        return { ResultType::CorruptFileData, "PNG IHDR field 'Height' is equal to 0. PNG specification requires it to be >0." };
    metaData.baseDimensions.height = origHeight;

    const std::uint8_t bitDepth = static_cast<std::uint8_t>(srcBuffer.data()[Header::bitDepthOffset]);
    const PNG::ColorType colorType = static_cast<PNG::ColorType>(srcBuffer.data()[Header::colorTypeOffset]);
    if (PNG::validateColorTypeAndBitDepth(colorType, bitDepth) == false)
        return { ResultType::CorruptFileData, "PNG does not allow this combination of values from "
                                              "IHDR fields 'Colour type' and 'Bit depth'." };
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

            backendData.idatChunkStart = chunkStart;
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

            backendData.plteChunkStart = chunkStart;
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

            metaData.colorSpace = ColorSpace::sRGB;
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

            // TODO: At some point, MetaData might contain gamma. Catch it here

            
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

    return { ResultType::Success, nullptr };
}

Texas::Result Texas::detail::PNG::loadFromBuffer_Step2(
    const MetaData& metaData,
    OpenBuffer::PNG_BackendData& backendData,
    const ByteSpan dstImageBuffer,
    const ByteSpan workingMemory)
{
    // TODO: Add length validation to the decompression stuff

    z_stream zLibDecompressJob{};

    zLibDecompressJob.next_out = (Bytef*)workingMemory.data();
    zLibDecompressJob.avail_out = (uInt)workingMemory.size();

    const int initErr = inflateInit(&zLibDecompressJob);
    if (initErr != Z_OK)
    {
        inflateEnd(&zLibDecompressJob);
        return { ResultType::NoIdea, "During PNG decompression, zLib failed to initialize the decompression job. I don't know why." };
    }

    // For every IDAT chunk
    {
        std::size_t memOffsetTracker = 0;
        while (true)
        {
            const std::byte* const chunkStart = backendData.idatChunkStart + memOffsetTracker;

            // Chunk data length is the first entry in the chunk. It's a uint32_t
            const std::uint32_t chunkDataLength = PNG::toCorrectEndian_u32(chunkStart);
            // Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
            const PNG::ChunkType chunkType = PNG::getChunkType(chunkStart + sizeof(PNG::ChunkSize_T));
            if (chunkType != PNG::ChunkType::IDAT)
                return { ResultType::CorruptFileData, "Found no IDAT chunk when decompressing PNG file. "
                                                      "The file has changed since Texas::loadFromBuffer() was called." };
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

    
    if (backendData.plteChunkStart == nullptr)
    {
        // If the plteChunk is nullptr, then we are not dealing with indexed colors.
        Result result = loadFromBuffer_Step2_Defilter(metaData, dstImageBuffer, workingMemory);
        if (result.resultType() != ResultType::Success)
            return result;
    }
    else
    {
        // Handle indexed colors

        const std::byte* const chunkStart = backendData.plteChunkStart;

        // Chunk data length is the first entry in the chunk. It's a uint32_t
        const std::uint32_t chunkDataLength = PNG::toCorrectEndian_u32(chunkStart);
        // Chunk type appears after chunk-data-length, so we offset 4 bytes extra.
        const PNG::ChunkType chunkType = PNG::getChunkType(chunkStart + sizeof(PNG::ChunkSize_T));
        if (chunkType != PNG::ChunkType::PLTE)
            return { ResultType::CorruptFileData, "Found no PLTE chunk when decompressing PNG file. "
                                                  "The file has changed since Texas::loadFromBuffer() was called." };
        if (chunkDataLength == 0)
            return { ResultType::CorruptFileData , "PNG PLTE chunk has field 'Data length' equal to 0. "
                                                   "The file has changed since Texas::loadFromBuffer() was called." };
        if (chunkDataLength > 768)
            return { ResultType::CorruptFileData , "PNG PLTE chunk has field 'Data length' higher than 768 bytes. "
                                                   "The file has changed since Texas::loadFromBuffer() was called." };
        if (chunkDataLength % 3 != 0)
            return { ResultType::CorruptFileData , "PNG PLTE chunk field 'Data length' value not divisible by 3. "
                                                   "The file has changed since Texas::loadFromBuffer() was called." };

        const std::byte* const paletteColors = chunkStart + sizeof(PNG::ChunkSize_T) + sizeof(PNG::ChunkType_T);
        const std::size_t paletteColorsLength = chunkDataLength / 3;

        for (size_t i = 0; i < static_cast<std::size_t>(metaData.baseDimensions.width) * metaData.baseDimensions.height; i++)
        {
            const std::uint8_t paletteColorIndex = static_cast<std::uint8_t>(workingMemory.data()[i]);

            if (paletteColorIndex >= paletteColorsLength)
                return { ResultType::CorruptFileData , "Found a pixel using an out-of-bounds index when unpacking PNG image with colour-type 'Index colour'."};

            const std::byte* const paletteColorPtr = paletteColors + static_cast<std::size_t>(paletteColorIndex) * 3;

            std::memcpy(dstImageBuffer.data() + i * 3, paletteColorPtr, sizeof(std::byte) * 3);
        }

    }

    return { ResultType::Success, nullptr };
}

static inline Texas::Result Texas::detail::PNG::loadFromBuffer_Step2_Defilter(const MetaData& metaData, const ByteSpan dstMem, const ByteSpan workingMem)
{
    const std::byte* const uncompressedData = workingMem.data();
    std::byte* const dstBuffer = dstMem.data();

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
            return { ResultType::CorruptFileData, "Encountered unknown filter-type when decompressing PNG imagedata." };
    }

    // Defilter rest of the rows
    for (std::uint32_t y = 1; y < metaData.baseDimensions.height; y++)
    {
        const std::size_t filterTypeIndex = static_cast<std::size_t>(y)* metaData.baseDimensions.width* pixelWidth + static_cast<std::size_t>(y);
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
            return { ResultType::CorruptFileData, "Encountered unknown filter-type when decompressing PNG imagedata." };
    }

    return { ResultType::Success, nullptr };
}