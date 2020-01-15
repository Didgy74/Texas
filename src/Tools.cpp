#include "Texas/Tools.hpp"
#include "Texas/detail/Tools.hpp"

#include "Texas/Dimensions.hpp"

#include <cmath>

std::uint64_t Texas::Tools::calcMaxMipLevelCount(Dimensions baseDims) noexcept
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

Texas::Dimensions Texas::Tools::calcMipmapDimensions(const Dimensions baseDims, const std::uint64_t mipIndex) noexcept
{
    if (baseDims.width == 0 || baseDims.height == 0 || baseDims.depth == 0)
        return {};
    if (mipIndex == 0)
        return baseDims;
    if (mipIndex >= calcMaxMipLevelCount(baseDims))
        return Texas::Dimensions{};

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

std::uint64_t Texas::Tools::calcTotalSizeRequired(Dimensions baseDims, PixelFormat pFormat, std::uint64_t mipCount, std::uint64_t arrayCount) noexcept
{
    if (baseDims.width == 0 || baseDims.height == 0 || baseDims.depth == 0)
        return 0;
    if (mipCount > calcMaxMipLevelCount(baseDims) || arrayCount == 0)
        return 0;

    std::uint64_t sum = 0;
    for (std::uint64_t i = 0; i < mipCount; i++)
        sum += calcSingleImageDataSize(calcMipmapDimensions(baseDims, i), pFormat);
    return sum * arrayCount;
}

std::uint64_t Texas::Tools::calcTotalSizeRequired(const MetaData& meta) noexcept
{
    return calcTotalSizeRequired(meta.baseDimensions, meta.pixelFormat, meta.mipLevelCount, meta.arrayLayerCount);
}

Texas::Optional<std::uint64_t> Texas::Tools::calcMipOffset(
    Dimensions baseDims, 
    PixelFormat pFormat, 
    std::uint64_t arrayCount,
    std::uint64_t mipCount,
    std::uint64_t mipIndex) noexcept
{
    if (mipIndex == 0)
        return { 0 };
    if (mipIndex >= mipCount)
        return {};
    return { calcTotalSizeRequired(baseDims, pFormat, mipIndex, arrayCount) };
}

Texas::Optional<std::uint64_t> Texas::Tools::calcMipOffset(const MetaData& meta, std::uint64_t mipLevelIndex) noexcept
{
    return calcMipOffset(meta.baseDimensions, meta.pixelFormat, meta.arrayLayerCount, meta.mipLevelCount, mipLevelIndex);
}

Texas::Optional<std::uint64_t> Texas::Tools::calcArrayLayerOffset(
    Dimensions baseDims,
    PixelFormat pFormat,
    std::uint64_t mipCount,
    std::uint64_t mipIndex,
    std::uint64_t arrayCount,
    std::uint64_t arrayIndex) noexcept
{
    if (mipIndex == 0 && arrayIndex == 0)
        return { 0 };
    if (mipCount > calcMaxMipLevelCount(baseDims))
        return {};
    if (mipIndex >= mipCount || arrayIndex >= arrayCount)
        return {};

    // Calculates size of all mip except the one we want to index into
    std::uint64_t sum = calcTotalSizeRequired(baseDims, pFormat, mipIndex, arrayCount);
    // Then calculates the size of every individiual array-layer up until our wanted index.
    sum += calcSingleImageDataSize(calcMipmapDimensions(baseDims, mipIndex), pFormat) * arrayIndex;
    return sum;
}

Texas::Optional<std::uint64_t> Texas::Tools::calcArrayLayerOffset(
    const MetaData& meta,
    std::uint64_t mipLevelIndex,
    std::uint64_t arrayLayerIndex) noexcept
{
    if (mipLevelIndex >= meta.mipLevelCount || arrayLayerIndex >= meta.arrayLayerCount)
        return {};
    return calcArrayLayerOffset(meta.baseDimensions, meta.pixelFormat, meta.mipLevelCount, mipLevelIndex, meta.arrayLayerCount, arrayLayerIndex);
}

namespace Texas::detail
{
    [[nodiscard]] static inline constexpr std::uint8_t getPixelWidth_UncompressedOnly(PixelFormat pFormat) noexcept
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

std::uint64_t Texas::Tools::calcSingleImageDataSize(const Dimensions dims, const PixelFormat pFormat) noexcept
{
    const detail::BlockInfo blockInfo = detail::getBlockInfo(pFormat);

    if (detail::isBCnCompressed(pFormat))
    {
        std::size_t blockCountX = static_cast<std::size_t>(std::ceilf(static_cast<float>(dims.width) / static_cast<float>(blockInfo.width)));
        if (blockCountX == 0)
            blockCountX = 1;
        std::size_t blockCountY = static_cast<std::size_t>(std::ceilf(static_cast<float>(dims.height) / static_cast<float>(blockInfo.height)));
        if (blockCountY == 0)
            blockCountY = 1;

        return blockCountX * blockCountY * dims.depth * blockInfo.size;
    }

    return dims.width * dims.height * dims.depth * detail::getPixelWidth_UncompressedOnly(pFormat);
}

std::uint64_t Texas::Tools::calcSingleImageDataSize(const MetaData& meta) noexcept
{
    return calcSingleImageDataSize(meta.baseDimensions, meta.pixelFormat);
}