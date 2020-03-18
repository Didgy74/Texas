#pragma once

#include "Texas/detail/BlockInfo.hpp"
#include "Texas/PixelFormat.hpp"

namespace Texas::detail
{

    /*
        Returns the block-info for given pixelFormat.
    */
    [[nodiscard]] constexpr BlockInfo getBlockInfo(PixelFormat pixelFormat) noexcept;

    constexpr BlockInfo getBlockInfo(PixelFormat pixelFormat) noexcept
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
}