#include "Texas/Tools.hpp"
#include "Texas/detail/Tools.hpp"
#include "Texas/detail/Exception.hpp"

#include "Texas/Dimensions.hpp"

#include <cmath>

std::uint64_t Texas::calcMaxMipCount(Dimensions baseDims)
{
    if (baseDims.width == 0 || baseDims.height == 0 || baseDims.depth == 0)
        return 0;

    std::uint64_t max = baseDims.width;
    if (baseDims.height > max)
        max = baseDims.height;
    if (baseDims.depth > max)
        max = baseDims.depth;
    return static_cast<std::uint64_t>(std::log2(max)) + 1;
}

Texas::Dimensions Texas::calcMipDimensions(Dimensions baseDims, std::uint64_t mipIndex)
{
    TEXAS_DETAIL_EXCEPTION(
        mipIndex < calcMaxMipCount(baseDims), 
        std::out_of_range("Called calcMipDimensions with a mipIndex larger than the baseDims can support."));

    if (baseDims.width == 0 || baseDims.height == 0 || baseDims.depth == 0)
        return {};
    if (mipIndex == 0)
        return baseDims;

    const std::uint64_t powerOf2 = std::uint64_t(1) << mipIndex;
    Dimensions returnValue{};
    returnValue.width = baseDims.width / powerOf2;
    if (returnValue.width == 0)
        returnValue.width = 1;
    returnValue.height = baseDims.height / powerOf2;
    if (returnValue.height == 0)
        returnValue.height = 1;
    returnValue.depth = baseDims.depth / powerOf2;
    if (returnValue.depth == 0)
        returnValue.depth = 1;
    return returnValue;
}

std::uint64_t Texas::calcTotalSizeRequired(Dimensions baseDims, PixelFormat pFormat, std::uint64_t mipCount, std::uint64_t arrayCount)
{
    if (baseDims.width == 0 || baseDims.height == 0 || baseDims.depth == 0 || mipCount == 0 || arrayCount == 0)
        return 0;

    std::uint64_t sum = 0;
    for (std::uint64_t i = 0; i < mipCount; i++)
        sum += calcArrayLayerSize(calcMipDimensions(baseDims, i), pFormat);
    return sum * arrayCount;
}

std::uint64_t Texas::calcTotalSizeRequired(TextureInfo const& meta)
{
    return calcTotalSizeRequired(meta.baseDimensions, meta.pixelFormat, meta.mipLevelCount, meta.arrayLayerCount);
}

std::uint64_t Texas::calcMipOffset(
    Dimensions baseDims, 
    PixelFormat pFormat, 
    std::uint64_t arrayCount,
    std::uint64_t mipIndex)
{
    if (mipIndex == 0)
        return 0;
    return { calcTotalSizeRequired(baseDims, pFormat, mipIndex, arrayCount) };
}

std::uint64_t Texas::calcMipOffset(TextureInfo const& meta, std::uint64_t mipLevelIndex)
{
    return calcMipOffset(meta.baseDimensions, meta.pixelFormat, meta.arrayLayerCount, mipLevelIndex);
}

std::uint64_t Texas::calcArrayLayerOffset(
    Dimensions baseDimensions,
    PixelFormat pixelFormat,
    std::uint64_t mipLevelIndex,
    std::uint64_t arrayLayerCount,
    std::uint64_t arrayLayerIndex)
{
    TEXAS_DETAIL_EXCEPTION(
        arrayLayerIndex < arrayLayerCount, 
        std::out_of_range("Tried to calculate array-layer offset with an array-layer index equal or higher than the array-layer count."));

    if (mipLevelIndex == 0 && arrayLayerCount == 0)
        return 0;

    // Calculates size of all mip except the one we want to index into
    std::uint64_t sum = calcTotalSizeRequired(baseDimensions, pixelFormat, mipLevelIndex, arrayLayerCount);
    // Then calculates the size of every individiual array-layer up until our wanted index.
    sum += calcArrayLayerSize(calcMipDimensions(baseDimensions, mipLevelIndex), pixelFormat) * arrayLayerIndex;
    return sum;
}

std::uint64_t Texas::calcArrayLayerOffset(
    const TextureInfo& textureInfo,
    std::uint64_t mipLevelIndex,
    std::uint64_t arrayLayerIndex)
{
    TEXAS_DETAIL_EXCEPTION(
        mipLevelIndex < textureInfo.mipLevelCount, 
        std::out_of_range("Inputted mipLevelIndex is equal to or higher than the mip-level count."));
    return calcArrayLayerOffset(textureInfo.baseDimensions, textureInfo.pixelFormat, mipLevelIndex, textureInfo.arrayLayerCount,arrayLayerIndex);
}

namespace Texas::detail
{
    [[nodiscard]] static inline constexpr std::uint8_t getPixelWidth_UncompressedOnly(PixelFormat pFormat)
    {
        switch (pFormat)
        {
        case PixelFormat::R_8:
            return 1;
        case PixelFormat::RG_8:
        case PixelFormat::RA_8:
        case PixelFormat::R_16:
            return 2;
        case PixelFormat::RGB_8:
        case PixelFormat::BGR_8:
            return 3;
        case PixelFormat::RGBA_8:
        case PixelFormat::BGRA_8:
        case PixelFormat::RG_16:
        case PixelFormat::RA_16:
        case PixelFormat::R_32:
            return 4;
        case PixelFormat::RGB_16:
        case PixelFormat::BGR_16:
            return 6;
        case PixelFormat::RGBA_16:
        case PixelFormat::BGRA_16:
        case PixelFormat::RG_32:
        case PixelFormat::RA_32:
            return 8;
        case PixelFormat::RGB_32:
        case PixelFormat::BGR_32:
            return 12;
        case PixelFormat::RGBA_32:
        case PixelFormat::BGRA_32:
            return 16;
        default:
            return 0;
        }
    }
}

std::uint64_t Texas::calcArrayLayerSize(Dimensions dims, PixelFormat pFormat)
{
    const detail::BlockInfo blockInfo = detail::getBlockInfo(pFormat);

    if (isBCnCompressed(pFormat))
    {
        std::uint64_t blockCountX = static_cast<std::uint64_t>(std::ceilf(static_cast<float>(dims.width) / static_cast<float>(blockInfo.width)));
        if (blockCountX == 0)
            blockCountX = 1;
        std::uint64_t blockCountY = static_cast<std::uint64_t>(std::ceilf(static_cast<float>(dims.height) / static_cast<float>(blockInfo.height)));
        if (blockCountY == 0)
            blockCountY = 1;

        return blockCountX * blockCountY * dims.depth * blockInfo.size;
    }

    return dims.width * dims.height * dims.depth * detail::getPixelWidth_UncompressedOnly(pFormat);
}