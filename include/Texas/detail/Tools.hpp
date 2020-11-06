#pragma once

#include <Texas/detail/BlockInfo.hpp>
#include <Texas/PixelFormat.hpp>

namespace Texas::detail
{
    /*
        Returns the block-info for given pixelFormat.
    */
    [[nodiscard]] constexpr BlockInfo getBlockInfo(PixelFormat pixelFormat) noexcept;
}


constexpr Texas::detail::BlockInfo Texas::detail::getBlockInfo(PixelFormat pixelFormat) noexcept
{
  switch (pixelFormat)
  {
  case PixelFormat::BC1_RGB:
  case PixelFormat::BC1_RGBA:
    return BlockInfo{ 4, 4, 1, 8 };
  case PixelFormat::BC2_RGBA:
  case PixelFormat::BC3_RGBA:
    return BlockInfo{ 4, 4, 1, 16 };
  case PixelFormat::BC4:
    return BlockInfo{ 4, 4, 1, 8 };
  case PixelFormat::BC5:
  case PixelFormat::BC6H:
  case PixelFormat::BC7_RGBA:
    return BlockInfo{ 4, 4, 1, 16 };

  case PixelFormat::ASTC_4x4:
    return BlockInfo{ 4, 4, 1, 16 };
  case PixelFormat::ASTC_5x4:
    return BlockInfo{ 5, 4, 1, 16 };
  case PixelFormat::ASTC_5x5:
    return BlockInfo{ 5, 5, 1, 16 };
  case PixelFormat::ASTC_6x5:
    return BlockInfo{ 6, 5, 1, 16 };
  case PixelFormat::ASTC_6x6:
    return BlockInfo{ 6, 6, 1, 16 };
  case PixelFormat::ASTC_8x5:
    return BlockInfo{ 8, 5, 1, 16 };
  case PixelFormat::ASTC_8x6:
    return BlockInfo{ 8, 6, 1, 16 };
  case PixelFormat::ASTC_8x8:
    return BlockInfo{ 8, 8, 1, 16 };
  case PixelFormat::ASTC_10x5:
    return BlockInfo{ 10, 5, 1, 16 };
  case PixelFormat::ASTC_10x6:
    return BlockInfo{ 10, 6, 1, 16 };
  case PixelFormat::ASTC_10x8:
    return BlockInfo{ 10, 8, 1, 16 };
  case PixelFormat::ASTC_10x10:
    return BlockInfo{ 10, 10, 1, 16 };
  case PixelFormat::ASTC_12x10:
    return BlockInfo{ 12, 10, 1, 16 };
  case PixelFormat::ASTC_12x12:
    return BlockInfo{ 12, 12, 1, 16 };

  default:
    return {};
  }
}