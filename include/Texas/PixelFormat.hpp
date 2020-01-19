#pragma once

#include <cstdint>

namespace Texas
{
    /*
        Describes a pixel's layout of channels,
        and the bit-depth of each channel
    */
    enum class PixelFormat : uint8_t
    {
        Invalid,

        R_8,
        RA_8,
        RG_8,
        RGB_8,
        BGR_8,
        RGBA_8,
        BGRA_8,

        R_16,
        RA_16,
        RG_16,
        RGB_16,
        BGR_16,
        RGBA_16,
        BGRA_16,

        R_32,
        RG_32,
        RA_32,
        RGB_32,
        BGR_32,
        RGBA_32,
        BGRA_32,

        BC1_RGB,
        BC1_RGBA,
        BC2_RGBA,
        BC3_RGBA,
        BC4,
        BC5,
        BC6H,
        BC7_RGBA,
    };

    [[nodiscard]] constexpr bool isBCnCompressed(PixelFormat pixelFormat) noexcept;

    /*
        Returns true if pixelFormat is a compressed format.
    */
    [[nodiscard]] constexpr bool isCompressed(PixelFormat pixelFormat) noexcept;
}

constexpr bool Texas::isBCnCompressed(PixelFormat pFormat) noexcept
{
    switch (pFormat)
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

    default:
        return false;
    }

    return false;
}

constexpr bool Texas::isCompressed(PixelFormat pFormat) noexcept
{
    return isBCnCompressed(pFormat);
}
