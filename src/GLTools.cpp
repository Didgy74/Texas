#include "Texas/GLTools.hpp"
#include "detail_GLTools.hpp"

Texas::PixelFormat Texas::GLToPixelFormat(std::uint32_t GLInternalFormat) noexcept
{
    return detail::GLToPixelFormat(static_cast<detail::GLEnum>(GLInternalFormat));
}

Texas::ColorSpace Texas::GLToColorSpace(std::uint32_t GLInternalFormat) noexcept
{
    return detail::GLToColorSpace(static_cast<detail::GLEnum>(GLInternalFormat));
}

Texas::ChannelType Texas::GLToChannelType(std::uint32_t GLInternalFormat) noexcept
{
    return detail::GLToChannelType(static_cast<detail::GLEnum>(GLInternalFormat));
}

std::uint32_t Texas::toGLTarget(TextureType texType) noexcept
{
    return static_cast<std::uint32_t>(detail::toGLTarget(texType));
}

std::uint32_t Texas::toGLType(PixelFormat pFormat, ChannelType chType) noexcept
{
    return static_cast<std::uint32_t>(detail::toGLType(pFormat, chType));
}

std::uint32_t Texas::toGLTypeSize(PixelFormat pFormat) noexcept
{
    return detail::toGLTypeSize(pFormat);
}

std::uint32_t Texas::toGLFormat(PixelFormat pFormat) noexcept
{
    return static_cast<std::uint32_t>(detail::toGLFormat(pFormat));
}

std::uint32_t Texas::toGLInternalFormat(PixelFormat pFormat, ColorSpace cSpace, ChannelType chType) noexcept
{
    return static_cast<std::uint32_t>(detail::toGLInternalFormat(pFormat, cSpace, chType));
}

std::uint32_t Texas::toGLInternalFormat(TextureInfo const& textureInfo) noexcept
{
  return static_cast<std::uint32_t>(detail::toGLInternalFormat(textureInfo.pixelFormat, textureInfo.colorSpace, textureInfo.channelType));
}

std::uint32_t Texas::toGLBaseInternalFormat(PixelFormat pFormat) noexcept
{
    return static_cast<std::uint32_t>(detail::toGLBaseInternalFormat(pFormat));
}

Texas::PixelFormat Texas::detail::GLToPixelFormat(GLEnum GLInternalFormat) noexcept
{
    switch (GLInternalFormat)
    {
        // Red
    case GLEnum::R8:
    case GLEnum::R8_SNORM:
    case GLEnum::R8I:
    case GLEnum::R8UI:
        return PixelFormat::R_8;
    case GLEnum::R16:
    case GLEnum::R16_SNORM:
    case GLEnum::R16I:
    case GLEnum::R16UI:
    case GLEnum::R16F:
        return PixelFormat::R_16;
    case GLEnum::R32I:
    case GLEnum::R32UI:
    case GLEnum::R32F:
        return PixelFormat::R_32;
        // RG
    case GLEnum::RG8:
    case GLEnum::RG8_SNORM:
    case GLEnum::RG8I:
    case GLEnum::RG8UI:
        return PixelFormat::RG_8;
    case GLEnum::RG16:
    case GLEnum::RG16_SNORM:
    case GLEnum::RG16I:
    case GLEnum::RG16UI:
    case GLEnum::RG16F:
        return PixelFormat::RG_16;
    case GLEnum::RG32I:
    case GLEnum::RG32UI:
    case GLEnum::RG32F:
        return PixelFormat::RG_32;
        // RGB
    case GLEnum::RGB8:
    case GLEnum::RGB8_SNORM:
    case GLEnum::RGB8I:
    case GLEnum::RGB8UI:
        return PixelFormat::RGB_8;
    case GLEnum::RGB16:
    case GLEnum::RGB16_SNORM:
    case GLEnum::RGB16I:
    case GLEnum::RGB16UI:
    case GLEnum::RGB16F:
        return PixelFormat::RGB_16;
    case GLEnum::RGB32I:
    case GLEnum::RGB32UI:
    case GLEnum::RGB32F:
        return PixelFormat::RGB_32;
        // RGBA
    case GLEnum::RGBA8:
    case GLEnum::RGBA8_SNORM:
    case GLEnum::RGBA8I:
    case GLEnum::RGBA8UI:
        return PixelFormat::RGBA_8;
    case GLEnum::RGBA16:
    case GLEnum::RGBA16_SNORM:
    case GLEnum::RGBA16I:
    case GLEnum::RGBA16UI:
    case GLEnum::RGBA16F:
        return PixelFormat::RGBA_16;
    case GLEnum::RGBA32I:
    case GLEnum::RGBA32UI:
    case GLEnum::RGBA32F:
        return PixelFormat::RGBA_32;

    case GLEnum::SRGB8:
        return PixelFormat::RGB_8;
    case GLEnum::SRGB8_ALPHA8:
        return PixelFormat::RGBA_8;

        // BCn
    case GLEnum::COMPRESSED_RGB_S3TC_DXT1_ANGLE:
    case GLEnum::COMPRESSED_SRGB_S3TC_DXT1_EXT:
        return PixelFormat::BC1_RGB;
    case GLEnum::COMPRESSED_RGBA_S3TC_DXT1_ANGLE:
    case GLEnum::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
        return PixelFormat::BC1_RGBA;
    case GLEnum::COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
        return PixelFormat::BC2_RGBA;
    case GLEnum::COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
        return PixelFormat::BC3_RGBA;
    case GLEnum::COMPRESSED_RED_RGTC1:
    case GLEnum::COMPRESSED_SIGNED_RED_RGTC1:
        return PixelFormat::BC4;
    case GLEnum::COMPRESSED_RG_RGTC2:
    case GLEnum::COMPRESSED_SIGNED_RG_RGTC2:
        return PixelFormat::BC5;
    case GLEnum::COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
    case GLEnum::COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
        return PixelFormat::BC6H;
    case GLEnum::COMPRESSED_RGBA_BPTC_UNORM:
    case GLEnum::COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
        return PixelFormat::BC7_RGBA;

        // ASTC
    case GLEnum::COMPRESSED_RGBA_ASTC_4x4_KHR:
        return PixelFormat::ASTC_4x4;
    case GLEnum::COMPRESSED_RGBA_ASTC_5x4_KHR:
        return PixelFormat::ASTC_5x4;
    case GLEnum::COMPRESSED_RGBA_ASTC_5x5_KHR:
        return PixelFormat::ASTC_5x5;
    case GLEnum::COMPRESSED_RGBA_ASTC_6x5_KHR:
        return PixelFormat::ASTC_6x5;
    case GLEnum::COMPRESSED_RGBA_ASTC_6x6_KHR:
        return PixelFormat::ASTC_6x6;
    case GLEnum::COMPRESSED_RGBA_ASTC_8x5_KHR:
        return PixelFormat::ASTC_8x5;
    case GLEnum::COMPRESSED_RGBA_ASTC_8x6_KHR:
        return PixelFormat::ASTC_8x6;
    case GLEnum::COMPRESSED_RGBA_ASTC_8x8_KHR:
        return PixelFormat::ASTC_8x8;
    case GLEnum::COMPRESSED_RGBA_ASTC_10x5_KHR:
        return PixelFormat::ASTC_10x5;
    case GLEnum::COMPRESSED_RGBA_ASTC_10x6_KHR:
        return PixelFormat::ASTC_10x6;
    case GLEnum::COMPRESSED_RGBA_ASTC_10x8_KHR:
        return PixelFormat::ASTC_10x8;
    case GLEnum::COMPRESSED_RGBA_ASTC_10x10_KHR:
        return PixelFormat::ASTC_10x10;
    case GLEnum::COMPRESSED_RGBA_ASTC_12x10_KHR:
        return PixelFormat::ASTC_12x10;
    case GLEnum::COMPRESSED_RGBA_ASTC_12x12_KHR:
        return PixelFormat::ASTC_12x12;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
        return PixelFormat::ASTC_4x4;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
        return PixelFormat::ASTC_5x4;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
        return PixelFormat::ASTC_5x5;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
        return PixelFormat::ASTC_6x5;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
        return PixelFormat::ASTC_6x6;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
        return PixelFormat::ASTC_8x5;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
        return PixelFormat::ASTC_8x6;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
        return PixelFormat::ASTC_8x8;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
        return PixelFormat::ASTC_10x5;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
        return PixelFormat::ASTC_10x6;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
        return PixelFormat::ASTC_10x8;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
        return PixelFormat::ASTC_10x10;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
        return PixelFormat::ASTC_12x10;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
        return PixelFormat::ASTC_12x12;

    default:
        break;
    }

    return PixelFormat::Invalid;
}

Texas::ColorSpace Texas::detail::GLToColorSpace(GLEnum GLInternalFormat) noexcept
{
    switch (GLInternalFormat)
    {
        // Red
    case GLEnum::R8:
    case GLEnum::R8_SNORM:
    case GLEnum::R8I:
    case GLEnum::R8UI:
    case GLEnum::R16:
    case GLEnum::R16_SNORM:
    case GLEnum::R16I:
    case GLEnum::R16UI:
    case GLEnum::R16F:
    case GLEnum::R32I:
    case GLEnum::R32UI:
    case GLEnum::R32F:
        // RG
    case GLEnum::RG8:
    case GLEnum::RG8_SNORM:
    case GLEnum::RG8I:
    case GLEnum::RG8UI:
    case GLEnum::RG16:
    case GLEnum::RG16_SNORM:
    case GLEnum::RG16I:
    case GLEnum::RG16UI:
    case GLEnum::RG16F:
    case GLEnum::RG32I:
    case GLEnum::RG32UI:
    case GLEnum::RG32F:
        // RGB
    case GLEnum::RGB8:
    case GLEnum::RGB8_SNORM:
    case GLEnum::RGB8I:
    case GLEnum::RGB8UI:
    case GLEnum::RGB16:
    case GLEnum::RGB16_SNORM:
    case GLEnum::RGB16I:
    case GLEnum::RGB16UI:
    case GLEnum::RGB16F:
    case GLEnum::RGB32I:
    case GLEnum::RGB32UI:
    case GLEnum::RGB32F:
        // RGBA
    case GLEnum::RGBA8:
    case GLEnum::RGBA8_SNORM:
    case GLEnum::RGBA8I:
    case GLEnum::RGBA8UI:
    case GLEnum::RGBA16:
    case GLEnum::RGBA16_SNORM:
    case GLEnum::RGBA16I:
    case GLEnum::RGBA16UI:
    case GLEnum::RGBA16F:
    case GLEnum::RGBA32I:
    case GLEnum::RGBA32UI:
    case GLEnum::RGBA32F:
        return ColorSpace::Linear;

    case GLEnum::SRGB8:
    case GLEnum::SRGB8_ALPHA8:
        return ColorSpace::sRGB;

        // BCn
    case GLEnum::COMPRESSED_RGB_S3TC_DXT1_ANGLE:
        return ColorSpace::Linear;
    case GLEnum::COMPRESSED_SRGB_S3TC_DXT1_EXT:
        return ColorSpace::sRGB;
    case GLEnum::COMPRESSED_RGBA_S3TC_DXT1_ANGLE:
        return ColorSpace::Linear;
    case GLEnum::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
        return ColorSpace::sRGB;
    case GLEnum::COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
    case GLEnum::COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
    case GLEnum::COMPRESSED_RED_RGTC1:
    case GLEnum::COMPRESSED_SIGNED_RED_RGTC1:
    case GLEnum::COMPRESSED_RG_RGTC2:
    case GLEnum::COMPRESSED_SIGNED_RG_RGTC2:
    case GLEnum::COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
    case GLEnum::COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
    case GLEnum::COMPRESSED_RGBA_BPTC_UNORM:
        return ColorSpace::Linear;
    case GLEnum::COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
        return ColorSpace::sRGB;

        // ASTC
    case GLEnum::COMPRESSED_RGBA_ASTC_4x4_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_5x4_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_5x5_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_6x5_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_6x6_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_8x5_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_8x6_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_8x8_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_10x5_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_10x6_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_10x8_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_10x10_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_12x10_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_12x12_KHR:
        return ColorSpace::Linear;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
        return ColorSpace::sRGB;

    default:
        break;
    };

    return ColorSpace::Invalid;
}

Texas::ChannelType Texas::detail::GLToChannelType(GLEnum GLInternalFormat) noexcept
{
    switch (GLInternalFormat)
    {
    case GLEnum::R8:
    case GLEnum::R16:
    case GLEnum::RG8:
    case GLEnum::RG16:
    case GLEnum::RGB8:
    case GLEnum::RGB16:
    case GLEnum::RGBA8:
    case GLEnum::RGBA16:
        return ChannelType::UnsignedNormalized;
    case GLEnum::R8_SNORM:
    case GLEnum::R16_SNORM:
    case GLEnum::RG8_SNORM:
    case GLEnum::RG16_SNORM:
    case GLEnum::RGB8_SNORM:
    case GLEnum::RGB16_SNORM:
    case GLEnum::RGBA8_SNORM:
    case GLEnum::RGBA16_SNORM:
        return ChannelType::SignedNormalized;
    case GLEnum::R8UI:
    case GLEnum::R16UI:
    case GLEnum::R32UI:
    case GLEnum::RG8UI:
    case GLEnum::RG16UI:
    case GLEnum::RG32UI:
    case GLEnum::RGB8UI:
    case GLEnum::RGB16UI:
    case GLEnum::RGB32UI:
    case GLEnum::RGBA8UI:
    case GLEnum::RGBA16UI:
    case GLEnum::RGBA32UI:
        return ChannelType::UnsignedInteger;
    case GLEnum::R8I:
    case GLEnum::R16I:
    case GLEnum::R32I:
    case GLEnum::RG8I:
    case GLEnum::RG16I:
    case GLEnum::RG32I:
    case GLEnum::RGB8I:
    case GLEnum::RGB16I:
    case GLEnum::RGB32I:
    case GLEnum::RGBA8I:
    case GLEnum::RGBA16I:
    case GLEnum::RGBA32I:
        return ChannelType::SignedInteger;
    case GLEnum::R16F:
    case GLEnum::R32F:
    case GLEnum::RG16F:
    case GLEnum::RG32F:
    case GLEnum::RGB16F:
    case GLEnum::RGB32F:
    case GLEnum::RGBA16F:
    case GLEnum::RGBA32F:
        return ChannelType::SignedFloat;

    case GLEnum::SRGB8:
    case GLEnum::SRGB8_ALPHA8:
        return ChannelType::UnsignedNormalized;

        // BCn
    case GLEnum::COMPRESSED_RGB_S3TC_DXT1_ANGLE:
    case GLEnum::COMPRESSED_RGBA_S3TC_DXT1_ANGLE:
    case GLEnum::COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
    case GLEnum::COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
    case GLEnum::COMPRESSED_RED_RGTC1:
    case GLEnum::COMPRESSED_RG_RGTC2:
    case GLEnum::COMPRESSED_RGBA_BPTC_UNORM:
        return ChannelType::UnsignedNormalized;
    case GLEnum::COMPRESSED_SIGNED_RED_RGTC1:
    case GLEnum::COMPRESSED_SIGNED_RG_RGTC2:
        return ChannelType::SignedNormalized;
    case GLEnum::COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
        return ChannelType::SignedFloat;
    case GLEnum::COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
        return ChannelType::UnsignedFloat;
    case GLEnum::COMPRESSED_SRGB_S3TC_DXT1_EXT:
    case GLEnum::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
    case GLEnum::COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
        return ChannelType::sRGB;

        // ASTC
    case GLEnum::COMPRESSED_RGBA_ASTC_4x4_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_5x4_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_5x5_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_6x5_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_6x6_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_8x5_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_8x6_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_8x8_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_10x5_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_10x6_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_10x8_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_10x10_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_12x10_KHR:
    case GLEnum::COMPRESSED_RGBA_ASTC_12x12_KHR:
        return ChannelType::UnsignedNormalized;
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
        return ChannelType::sRGB;

    default:
        break;
    }

    return ChannelType::Invalid;
}

Texas::detail::GLEnum Texas::detail::toGLTarget(TextureType texType) noexcept
{
    switch (texType)
    {
    case TextureType::Texture1D:
        return GLEnum::TEXTURE_1D;
    case TextureType::Texture2D:
        return GLEnum::TEXTURE_2D;
    case TextureType::Texture3D:
        return GLEnum::TEXTURE_3D;
    case TextureType::Array1D:
        return GLEnum::TEXTURE_1D_ARRAY;
    case TextureType::Array2D:
        return GLEnum::TEXTURE_2D_ARRAY;

    default:
        break;
    }

    return GLEnum::Invalid;
}

Texas::detail::GLEnum Texas::detail::toGLType(PixelFormat pFormat, ChannelType chType) noexcept
{
    if (isCompressed(pFormat))
        return GLEnum(0);

    switch (pFormat)
    {
    case PixelFormat::R_8:
    case PixelFormat::RG_8:
    case PixelFormat::RGB_8:
    case PixelFormat::BGR_8:
    case PixelFormat::RGBA_8:
    case PixelFormat::BGRA_8:
        switch (chType)
        {
        case ChannelType::UnsignedNormalized:
            return GLEnum::UNSIGNED_BYTE;
        case ChannelType::SignedNormalized:
            return GLEnum::BYTE;
        default:
            return GLEnum::Invalid;
        }
        break;
    case PixelFormat::R_16:
    case PixelFormat::RG_16:
    case PixelFormat::RGB_16:
    case PixelFormat::RGBA_16:
        switch (chType)
        {
        case ChannelType::UnsignedInteger:
            return GLEnum::UNSIGNED_SHORT;
        case ChannelType::SignedInteger:
            return GLEnum::SHORT;
        case ChannelType::SignedFloat:
            return GLEnum::HALF_FLOAT;
        default:
            return GLEnum::Invalid;
        }
        break;
    case PixelFormat::R_32:
    case PixelFormat::RG_32:
    case PixelFormat::RGB_32:
    case PixelFormat::RGBA_32:
        switch (chType)
        {
        case ChannelType::UnsignedInteger:
            return GLEnum::UNSIGNED_INT;
        case ChannelType::SignedInteger:
            return GLEnum::INT;
        case ChannelType::SignedFloat:
            return GLEnum::FLOAT;
        default:
            return GLEnum::Invalid;
        }
        break;

    default:
        break;
    }

    return GLEnum::Invalid;
}

std::uint32_t Texas::detail::toGLTypeSize(PixelFormat pFormat) noexcept
{
    if (isCompressed(pFormat))
        return 1;

    switch (pFormat)
    {
    case PixelFormat::R_8:
    case PixelFormat::RG_8:
    case PixelFormat::RGB_8:
    case PixelFormat::BGR_8:
    case PixelFormat::RGBA_8:
    case PixelFormat::BGRA_8:
        return 1;
    case PixelFormat::R_16:
    case PixelFormat::RG_16:
    case PixelFormat::RGB_16:
    case PixelFormat::RGBA_16:
        return 2;
    case PixelFormat::R_32:
    case PixelFormat::RG_32:
    case PixelFormat::RGB_32:
    case PixelFormat::RGBA_32:
        return 4;

    default:
        break;
    }

    return 0;
}


Texas::detail::GLEnum Texas::detail::toGLFormat(PixelFormat pFormat) noexcept
{
    if (isCompressed(pFormat))
        return GLEnum(0);

    switch (pFormat)
    {
    case PixelFormat::R_8:
    case PixelFormat::R_16:
    case PixelFormat::R_32:
        return GLEnum::RED;
    case PixelFormat::RG_8:
    case PixelFormat::RG_16:
    case PixelFormat::RG_32:
        return GLEnum::RG;
    case PixelFormat::RGB_8:
    case PixelFormat::RGB_16:
    case PixelFormat::RGB_32:
        return GLEnum::RGB;
    case PixelFormat::BGR_8:
        return GLEnum::BGR;
    case PixelFormat::RGBA_8:
    case PixelFormat::RGBA_16:
    case PixelFormat::RGBA_32:
        return GLEnum::RGBA;
    case PixelFormat::BGRA_8:
        return GLEnum::BGRA;

    default:
        break;
    }

    return GLEnum::Invalid;
}

#define TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(dimensions) \
    switch (cSpace) \
    { \
    case ColorSpace::Linear: \
      return GLEnum::COMPRESSED_RGBA_ASTC_## dimensions ##_KHR; \
    case ColorSpace::sRGB: \
      return GLEnum::COMPRESSED_SRGB8_ALPHA8_ASTC_## dimensions ##_KHR; \
    default: \
      return GLEnum::Invalid; \
    } \

Texas::detail::GLEnum Texas::detail::toGLInternalFormat(
    PixelFormat const pFormat, 
    ColorSpace const cSpace, 
    ChannelType const chType) noexcept
{
    switch (pFormat)
    {
    case PixelFormat::R_8:
        switch (chType)
        {
        case ChannelType::UnsignedNormalized:
            return GLEnum::R8;
        case ChannelType::SignedNormalized:
            return GLEnum::R8_SNORM;
        case ChannelType::UnsignedInteger:
            return GLEnum::R8UI;
        case ChannelType::SignedInteger:
            return GLEnum::R8I;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::R_16:
        switch (chType)
        {
        case ChannelType::UnsignedNormalized:
            return GLEnum::R16;
        case ChannelType::SignedNormalized:
            return GLEnum::R16_SNORM;
        case ChannelType::UnsignedInteger:
            return GLEnum::R16UI;
        case ChannelType::SignedInteger:
            return GLEnum::R16I;
        case ChannelType::SignedFloat:
            return GLEnum::R16F;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::R_32:
        switch (chType)
        {
        case ChannelType::UnsignedInteger:
            return GLEnum::R32UI;
        case ChannelType::SignedInteger:
            return GLEnum::R32I;
        case ChannelType::SignedFloat:
            return GLEnum::R32F;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::RG_8:
        switch (chType)
        {
        case ChannelType::UnsignedNormalized:
            return GLEnum::RG8;
        case ChannelType::SignedNormalized:
            return GLEnum::RG8_SNORM;
        case ChannelType::UnsignedInteger:
            return GLEnum::RG8UI;
        case ChannelType::SignedInteger:
            return GLEnum::RG8I;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::RG_16:
        switch (chType)
        {
        case ChannelType::UnsignedNormalized:
            return GLEnum::RG16;
        case ChannelType::SignedNormalized:
            return GLEnum::RG16_SNORM;
        case ChannelType::UnsignedInteger:
            return GLEnum::RG16UI;
        case ChannelType::SignedInteger:
            return GLEnum::RG16I;
        case ChannelType::SignedFloat:
            return GLEnum::RG16F;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::RG_32:
        switch (chType)
        {
        case ChannelType::UnsignedInteger:
            return GLEnum::RG32UI;
        case ChannelType::SignedInteger:
            return GLEnum::RG32I;
        case ChannelType::SignedFloat:
            return GLEnum::RG32F;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::RGB_8:
        switch (chType)
        {
        case ChannelType::UnsignedNormalized:
            return GLEnum::RGB8;
        case ChannelType::SignedNormalized:
            return GLEnum::RGB8_SNORM;
        case ChannelType::UnsignedInteger:
            return GLEnum::RGB8UI;
        case ChannelType::SignedInteger:
            return GLEnum::RGB8I;
        case ChannelType::sRGB:
            return GLEnum::SRGB8;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::RGB_16:
        switch (chType)
        {
        case ChannelType::UnsignedNormalized:
            return GLEnum::RGB16;
        case ChannelType::SignedNormalized:
            return GLEnum::RGB16_SNORM;
        case ChannelType::UnsignedInteger:
            return GLEnum::RGB16UI;
        case ChannelType::SignedInteger:
            return GLEnum::RGB16I;
        case ChannelType::SignedFloat:
            return GLEnum::RGB16F;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::RGB_32:
        switch (chType)
        {
        case ChannelType::UnsignedInteger:
            return GLEnum::RGB32UI;
        case ChannelType::SignedInteger:
            return GLEnum::RGB32UI;
        case ChannelType::SignedFloat:
            return GLEnum::RGB32F;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::RGBA_8:
        switch (chType)
        {
        case ChannelType::UnsignedNormalized:
            return GLEnum::RGBA8;
        case ChannelType::SignedNormalized:
            return GLEnum::RGBA8_SNORM;
        case ChannelType::UnsignedInteger:
            return GLEnum::RGBA8UI;
        case ChannelType::SignedInteger:
            return GLEnum::RGBA8I;
        case ChannelType::sRGB:
            return GLEnum::SRGB8_ALPHA8;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::RGBA_16:
        switch (chType)
        {
        case ChannelType::UnsignedNormalized:
            return GLEnum::RGBA16;
        case ChannelType::SignedNormalized:
            return GLEnum::RGBA16_SNORM;
        case ChannelType::UnsignedInteger:
            return GLEnum::RGBA16UI;
        case ChannelType::SignedInteger:
            return GLEnum::RGBA16I;
        case ChannelType::SignedFloat:
            return GLEnum::RGBA16F;
        default:
            return GLEnum::Invalid;
        }
    case PixelFormat::RGBA_32:
        switch (chType)
        {
        case ChannelType::UnsignedInteger:
            return GLEnum::RGBA32UI;
        case ChannelType::SignedInteger:
            return GLEnum::RGBA32I;
        case ChannelType::SignedFloat:
            return GLEnum::RGBA32F;
        default:
            return GLEnum::Invalid;
        }


    case PixelFormat::BC7_RGBA:
        switch (cSpace)
        {
        case ColorSpace::Linear:
            return GLEnum::COMPRESSED_RGBA_BPTC_UNORM;
        case ColorSpace::sRGB:
            return GLEnum::COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
        default:
            return GLEnum::Invalid;
        }


        // ASTC
    case PixelFormat::ASTC_4x4:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(4x4);
    case PixelFormat::ASTC_5x4:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(5x4);
    case PixelFormat::ASTC_5x5:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(5x5);
    case PixelFormat::ASTC_6x5:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(6x5);
    case PixelFormat::ASTC_6x6:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(6x6);
    case PixelFormat::ASTC_8x5:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(8x5);
    case PixelFormat::ASTC_8x6:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(8x6);
    case PixelFormat::ASTC_8x8:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(8x8);
    case PixelFormat::ASTC_10x5:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(10x5);
    case PixelFormat::ASTC_10x6:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(10x6);
    case PixelFormat::ASTC_10x8:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(10x8);
    case PixelFormat::ASTC_10x10:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(10x10);
    case PixelFormat::ASTC_12x10:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(12x10);
    case PixelFormat::ASTC_12x12:
        TEXAS_DETAIL_TOGLINTERNALFORMAT_CREATE_ASTC_CASES(12x12);

    default:
        return GLEnum::Invalid;
    }
}

Texas::detail::GLEnum Texas::detail::toGLBaseInternalFormat(PixelFormat pFormat) noexcept
{
    if (!isCompressed(pFormat))
        return detail::toGLFormat(pFormat);

    switch (pFormat)
    {
        // BCn
    case PixelFormat::BC1_RGB:
        return GLEnum::RGB;
    case PixelFormat::BC1_RGBA:
        return GLEnum::RGBA;
    case PixelFormat::BC2_RGBA:
        return GLEnum::RGBA;
    case PixelFormat::BC3_RGBA:
        return GLEnum::RGBA;
    // TODO: Add remaining BCn cases here
    case PixelFormat::BC7_RGBA:
        return GLEnum::RGBA;

        // ASTC
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
      return GLEnum::RGBA;


    default:
        return GLEnum::Invalid;
    }
}