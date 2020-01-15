#include "Texas/Tools.hpp"
#include "Texas/detail/Tools.hpp"

#include "Texas/Dimensions.hpp"

#include <cmath>

std::uint64_t Texas::Tools::calcMaxMipLevelCount(Dimensions in)
{
    std::uint64_t max = in.width;
    if (in.height > max)
        max = in.height;
    if (in.depth > max)
        max = in.depth;
    return static_cast<std::uint64_t>(std::log2(max));
}

Texas::Dimensions Texas::Tools::calcMipmapDimensions(const Dimensions baseDimensions, const std::uint64_t mipLevel)
{
    if (mipLevel == 0)
        return baseDimensions;
    if (mipLevel >= calcMaxMipLevelCount(baseDimensions))
        return Texas::Dimensions{};

    const std::uint64_t powerOf2 = std::uint64_t(1) << mipLevel;
    Dimensions returnValue{};
    returnValue.width = baseDimensions.width / powerOf2;
    if (returnValue.width == 0)
        returnValue.width = 1;
    returnValue.height = baseDimensions.height / powerOf2;
    if (returnValue.height == 0)
        returnValue.height = 1;
    returnValue.depth = baseDimensions.depth / powerOf2;
    if (returnValue.depth == 0)
        returnValue.depth = 1;
    return returnValue;
}

std::size_t Texas::Tools::calcTotalSizeRequired(Dimensions baseDimensions, std::uint64_t arrayLayerCount, std::uint64_t mipLevelCount,  PixelFormat pixelFormat)
{
    if ((mipLevelCount > 1 &&  mipLevelCount >= calcMaxMipLevelCount(baseDimensions)) || arrayLayerCount == 0)
        return 0;

    std::size_t sum = 0;
    for (std::uint64_t i = 0; i < mipLevelCount; i++)
        sum += calcSingleImageDataSize(calcMipmapDimensions(baseDimensions, i), pixelFormat);
    return sum * arrayLayerCount;
}

std::uint64_t Texas::Tools::calcTotalSizeRequired(const MetaData& metaData)
{
    return calcTotalSizeRequired(metaData.baseDimensions, metaData.arrayLayerCount, metaData.mipLevelCount, metaData.pixelFormat);
}

Texas::Optional<std::size_t> Texas::Tools::calcMipOffset(
    Dimensions baseDims, 
    PixelFormat pFormat, 
    std::uint64_t arrayLayerCount,
    std::uint64_t mipLevelCount,
    std::uint64_t mipLevelIndex) noexcept
{
    if (mipLevelIndex == 0)
        return { 0 };
    if (mipLevelIndex >= mipLevelCount)
        return {};
    return { calcTotalSizeRequired(baseDims, arrayLayerCount, mipLevelIndex, pFormat) };
}

Texas::Optional<std::size_t> Texas::Tools::calcMipOffset(const MetaData& metaData, std::uint64_t mipLevelIndex) noexcept
{
    return calcMipOffset(metaData.baseDimensions, metaData.pixelFormat, metaData.arrayLayerCount, metaData.mipLevelCount, mipLevelIndex);
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