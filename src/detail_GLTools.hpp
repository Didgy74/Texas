/*
    Don't move this code into a .cpp file.
    The file-backends are using this private header.
*/

#pragma once

namespace Texas::detail
{
    enum class GLEnum : std::uint32_t;

    [[nodiscard]] PixelFormat GLToPixelFormat(GLEnum GLInternalFormat) noexcept;
    [[nodiscard]] ColorSpace GLToColorSpace(GLEnum GLInternalFormat) noexcept;
    [[nodiscard]] ChannelType GLToChannelType(GLEnum GLInternalFormat) noexcept;

    [[nodiscard]] GLEnum toGLTarget(TextureType texType) noexcept;
    [[nodiscard]] GLEnum toGLType(PixelFormat pFormat, ChannelType chType) noexcept;
    [[nodiscard]] std::uint32_t toGLTypeSize(PixelFormat pFormat) noexcept;
    [[nodiscard]] GLEnum toGLFormat(PixelFormat pFormat) noexcept;
    [[nodiscard]] GLEnum toGLInternalFormat(PixelFormat pFormat, ColorSpace cSpace, ChannelType chType) noexcept;
    [[nodiscard]] GLEnum toGLBaseInternalFormat(PixelFormat pFormat) noexcept;
}

enum class Texas::detail::GLEnum : std::uint32_t
{
    Invalid = static_cast<std::uint32_t>(-1),

    TEXTURE_1D = 0x0DE0,
    TEXTURE_1D_ARRAY = 0x8C18,
    TEXTURE_2D = 0x0DE1,
    TEXTURE_2D_ARRAY = 0x8C1A,
    TEXTURE_3D = 0x806F,

    // GLType
    BYTE = 0x1400,
    UNSIGNED_BYTE = 0x1401,
    SHORT = 0x1402,
    UNSIGNED_SHORT = 0x1403,
    INT = 0x1404,
    UNSIGNED_INT = 0x1405,
    FLOAT = 0x1406,
    HALF_FLOAT = 0x140B,

    RED = 0x1903,
    RG = 0x8227,
    RGB = 0x1907,
    BGR = 0x80E0,
    RGBA = 0x1908,
    BGRA = 0x80E1,

    R8 = 0x8229,
    R8_SNORM = 0x8F94,
    R8I = 0x8231,
    R8UI = 0x8232,
    R16 = 0x822A,
    R16_SNORM = 0x8F98,
    R16F = 0x822D,
    R16I = 0x8233,
    R16UI = 0x8234,
    R32I = 0x8235,
    R32UI = 0x8236,
    R32F = 0x822E,
    RG8 = 0x822B,
    RG8_SNORM = 0x8F95,
    RG8I = 0x8237,
    RG8UI = 0x8238,
    RG16 = 0x822C,
    RG16_SNORM = 0x8F99,
    RG16F = 0x822F,
    RG16I = 0x8239,
    RG16UI = 0x823A,
    RG32I = 0x823B,
    RG32UI = 0x823C,
    RG32F = 0x8230,
    RGB8 = 0x8051,
    RGB8_SNORM = 0x8F96,
    RGB8I = 0x8D8F,
    RGB8UI = 0x8D7D,
    RGB16 = 0x8054,
    RGB16_SNORM = 0x8F9A,
    RGB16I = 0x8D89,
    RGB16UI = 0x8D77,
    RGB16F = 0x881B,
    RGB32I = 0x8D83,
    RGB32UI = 0x8D71,
    RGB32F = 0x8815,
    RGBA8 = 0x8058,
    RGBA8_SNORM = 0x8F97,
    RGBA8I = 0x8D8E,
    RGBA8UI = 0x8D7C,
    RGBA16 = 0x805B,
    RGBA16_SNORM = 0x8F9B,
    RGBA16I = 0x8D88,
    RGBA16UI = 0x8D76,
    RGBA16F = 0x881A,
    RGBA32I = 0x8D82,
    RGBA32UI = 0x8D70,
    RGBA32F = 0x8814,

    SRGB8 = 0x8C41,
    SRGB8_ALPHA8 = 0x8C43,

    // BCn
    // BC1 - BC3
    COMPRESSED_SRGB_S3TC_DXT1_EXT = 0x8C4C,
    COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT = 0x8C4D,
    COMPRESSED_RGB_S3TC_DXT1_ANGLE = 0x83F0,
    COMPRESSED_RGBA_S3TC_DXT1_ANGLE = 0x83F1,
    COMPRESSED_RGBA_S3TC_DXT3_ANGLE = 0x83F2,
    COMPRESSED_RGBA_S3TC_DXT5_ANGLE = 0x83F3,
    // BC4 - BC5
    COMPRESSED_RED_RGTC1 = 0x8DBB,
    COMPRESSED_SIGNED_RED_RGTC1 = 0x8DBC,
    COMPRESSED_RG_RGTC2 = 0x8DBD,
    COMPRESSED_SIGNED_RG_RGTC2 = 0x8DBE,
    // B6H - BC7
    COMPRESSED_RGB_BPTC_SIGNED_FLOAT = 0x8E8E,
    COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = 0x8E8F,
    COMPRESSED_RGBA_BPTC_UNORM = 0x8E8C,
    COMPRESSED_SRGB_ALPHA_BPTC_UNORM = 0x8E8D,
};