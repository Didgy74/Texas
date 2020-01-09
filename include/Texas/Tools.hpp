#pragma once

#include "Texas/PixelFormat.hpp"
#include "Texas/Dimensions.hpp"
#include "Texas/TextureType.hpp"
#include "Texas/BlockInfo.hpp"

#include <cstdint>
#include <cstddef>

namespace Texas::Tools
{
    /*
        Returns 0 upon failure, such as invalid dimensions for pixelformat
    */
    std::uint64_t calcSingleImageDataSize(Dimensions dimensions, PixelFormat pixelFormat);

    std::uint64_t calcMaxMipLevelCount(Dimensions in);

    Dimensions calcMipmapDimensions(Dimensions baseDimensions, std::uint64_t mipLevel);

    std::uint64_t calcTotalSizeRequired(Dimensions baseDimensions, std::uint64_t mipLevelCount, std::uint64_t arrayLayerCount, PixelFormat pixelFormat);

    /*
        Returns the block-info for given pixelFormat.
    */
    constexpr BlockInfo getBlockInfo(PixelFormat pixelFormat);

    constexpr bool isBCnCompressed(PixelFormat pixelFormat);

    /*
        Returns true if pixelFormat is a compressed format.
    */
    constexpr bool isCompressed(PixelFormat pixelFormat);

    constexpr BlockInfo getBlockInfo(PixelFormat pixelFormat)
    {
        switch (pixelFormat)
        {
        case PixelFormat::BC1_RGB:
        case PixelFormat::BC1_RGBA:
            return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 8 };
        case PixelFormat::BC2_RGBA:
        case PixelFormat::BC3_RGBA:
            return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 16 };
        case PixelFormat::BC4:
            return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 8 };
        case PixelFormat::BC5:
        case PixelFormat::BC6H:
        case PixelFormat::BC7_RGBA:
            return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 16 };
        }

        return {};
    }

    constexpr bool isBCnCompressed(PixelFormat pixelFormat)
    {
        switch (pixelFormat)
        {
        case PixelFormat::BC1_RGB:
        case PixelFormat::BC1_RGBA:
        case PixelFormat::BC2_RGBA:
        case PixelFormat::BC3_RGBA:
        case PixelFormat::BC4:
        case PixelFormat::BC5:
        case PixelFormat::BC6H:
        case PixelFormat::BC7_RGBA:
            return true;
        }

        return false;
    }

    constexpr bool isCompressed(PixelFormat pixelFormat)
    {
        return isBCnCompressed(pixelFormat);
    }
}


