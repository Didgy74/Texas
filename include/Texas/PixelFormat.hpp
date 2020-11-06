#pragma once

namespace Texas
{
    /*
        Describes a pixel's layout of channels,
        and the bit-depth of each channel
    */
    enum class PixelFormat : char
    {
        Invalid,

        R_8,
        RG_8,
        RGB_8,
        BGR_8,
        RGBA_8,
        BGRA_8,

        R_16,
        RG_16,
        RGB_16,
        RGBA_16,

        R_32,
        RG_32,
        RGB_32,
        RGBA_32,

        BC1_RGB,
        BC1_RGBA,
        BC2_RGBA,
        BC3_RGBA,
        BC4,
        BC5,
        BC6H,
        BC7_RGBA,

        ASTC_4x4,
        ASTC_5x4,
        ASTC_5x5,
        ASTC_6x5,
        ASTC_6x6,
        ASTC_8x5,
        ASTC_8x6,
        ASTC_8x8,
        ASTC_10x5,
        ASTC_10x6,
        ASTC_10x8,
        ASTC_10x10,
        ASTC_12x10,
        ASTC_12x12,
    };

    [[nodiscard]] constexpr bool isBCnCompressed(PixelFormat pixelFormat) noexcept;

    [[nodiscard]] constexpr bool isASTCCompressed(PixelFormat pixelFormat) noexcept;

    [[nodiscard]] constexpr bool isCompressed(PixelFormat pixelFormat) noexcept;
}

/*
    Returns true if pixelFormat is in the BCn compression family.
*/
constexpr bool Texas::isBCnCompressed(PixelFormat pixelFormat) noexcept
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

    default:
        return false;
    }
}

[[nodiscard]] constexpr bool Texas::isASTCCompressed(PixelFormat pixelFormat) noexcept
{
    switch (pixelFormat)
    {
    case PixelFormat::ASTC_4x4:
    case PixelFormat::ASTC_5x4:
    case PixelFormat::ASTC_5x5:
    case PixelFormat::ASTC_6x5:
    case PixelFormat::ASTC_6x6:
    case PixelFormat::ASTC_8x5:
    case PixelFormat::ASTC_8x6:
    case PixelFormat::ASTC_8x8:
    case PixelFormat::ASTC_10x5:
    case PixelFormat::ASTC_10x6:
    case PixelFormat::ASTC_10x8:
    case PixelFormat::ASTC_10x10:
    case PixelFormat::ASTC_12x10:
    case PixelFormat::ASTC_12x12:
        return true;

    default:
        return false;
    }
}

/*
    Returns true if pixelFormat is a compressed format type.
*/
constexpr bool Texas::isCompressed(PixelFormat pixelFormat) noexcept
{
    return isBCnCompressed(pixelFormat) || isASTCCompressed(pixelFormat);
}
