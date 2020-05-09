#include "Texas/Tools.hpp"
#include "Texas/detail/Tools.hpp"

#include "Texas/Dimensions.hpp"

#include <cmath>

std::uint64_t Texas::calculateMaxMipCount(Dimensions baseDims) noexcept
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

Texas::Dimensions Texas::calculateMipDimensions(Dimensions baseDims, std::uint8_t mipIndex) noexcept
{
    if (baseDims.width == 0 || baseDims.height == 0 || baseDims.depth == 0)
        return {};
    if (mipIndex == 0)
        return baseDims;

    std::uint64_t const powerOf2 = std::uint64_t(1) << mipIndex;
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

std::uint64_t Texas::calculateTotalSize(
    Dimensions baseDims, 
    PixelFormat pFormat, 
    std::uint8_t mipCount, 
    std::uint64_t arrayCount) noexcept
{
    if (baseDims.width == 0 || baseDims.height == 0 || baseDims.depth == 0 || mipCount == 0 || arrayCount == 0)
        return 0;

    std::uint64_t sum = 0;
    for (std::uint8_t i = 0; i < mipCount; i++)
        sum += calculateSingleImageSize(calculateMipDimensions(baseDims, i), pFormat);
    return sum * arrayCount;
}

std::uint64_t Texas::calculateTotalSize(TextureInfo const& meta) noexcept
{
    return calculateTotalSize(meta.baseDimensions, meta.pixelFormat, meta.mipCount, meta.layerCount);
}

std::uint64_t Texas::calculateMipOffset(
    Dimensions baseDims, 
    PixelFormat pFormat, 
    std::uint64_t arrayCount,
    std::uint8_t mipIndex) noexcept
{
    if (mipIndex == 0)
        return 0;
    return calculateTotalSize(baseDims, pFormat, mipIndex, arrayCount);
}

std::uint64_t Texas::calculateMipOffset(
    TextureInfo const& meta, 
    std::uint8_t mipIndex) noexcept
{
    return calculateMipOffset(meta.baseDimensions, meta.pixelFormat, meta.layerCount, mipIndex);
}

std::uint64_t Texas::calculateLayerOffset(
    Dimensions baseDimensions,
    PixelFormat pixelFormat,
    std::uint8_t mipIndex,
    std::uint64_t layerCount,
    std::uint64_t layerIndex) noexcept
{
    if (mipIndex == 0 && layerCount == 0)
        return 0;

    // Calculates size of all mip except the one we want to index into
    std::uint64_t sum = calculateTotalSize(baseDimensions, pixelFormat, mipIndex, layerCount);
    // Then calculates the size of every individiual array-layer up until our wanted index.
    sum += calculateSingleImageSize(calculateMipDimensions(baseDimensions, mipIndex), pixelFormat) * layerIndex;
    return sum;
}

std::uint64_t Texas::calculateLayerOffset(
    TextureInfo const& textureInfo,
    std::uint8_t mipIndex,
    std::uint64_t layerIndex) noexcept
{
    return calculateLayerOffset(
        textureInfo.baseDimensions, 
        textureInfo.pixelFormat, 
        mipIndex, 
        textureInfo.layerCount, 
        layerIndex);
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
        case PixelFormat::R_16:
            return 2;
        case PixelFormat::RGB_8:
        case PixelFormat::BGR_8:
            return 3;
        case PixelFormat::RGBA_8:
        case PixelFormat::BGRA_8:
        case PixelFormat::RG_16:
        case PixelFormat::R_32:
            return 4;
        case PixelFormat::RGB_16:
            return 6;
        case PixelFormat::RGBA_16:
        case PixelFormat::RG_32:
            return 8;
        case PixelFormat::RGB_32:
            return 12;
        case PixelFormat::RGBA_32:
            return 16;
        default:
            return 0;
        }
    }
}

std::uint64_t Texas::calculateSingleImageSize(Dimensions dims, PixelFormat pFormat) noexcept
{
    detail::BlockInfo const blockInfo = detail::getBlockInfo(pFormat);

    if (isBCnCompressed(pFormat))
    {
        std::uint64_t blockCountX = static_cast<std::uint64_t>(std::ceil(static_cast<float>(dims.width) / static_cast<float>(blockInfo.width)));
        if (blockCountX == 0)
            blockCountX = 1;
        std::uint64_t blockCountY = static_cast<std::uint64_t>(std::ceil(static_cast<float>(dims.height) / static_cast<float>(blockInfo.height)));
        if (blockCountY == 0)
            blockCountY = 1;

        return blockCountX * blockCountY * dims.depth * blockInfo.size;
    }

    return dims.width * dims.height * dims.depth * detail::getPixelWidth_UncompressedOnly(pFormat);
}