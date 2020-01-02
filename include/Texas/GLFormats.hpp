#pragma once

#include "Texas/PixelFormat.hpp"
#include "Texas/Colorspace.hpp"

#include <cstdint>

namespace Texas::Tools::detail
{
	enum class GLEnum : std::uint32_t
	{
		TEXTURE_1D = 0x0DE0,
		TEXTURE_2D = 0x0DE1,
		TEXTURE_3D = 0x806F,

		// GLType
		BYTE = 0x1400,
		UNSIGNED_BYTE = 0x1401,
		SHORT = 0x1402,
		UNSIGNED_SHORT = 0x1403,
		INT = 0x1404,
		UNSIGNED_INT = 0x1405,
		FLOAT = 0x1406,

		// Standard
		RED = 0x1903,
		RG = 0x8227,
		RGB = 0x1907,
		RGBA = 0x1908,

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

	[[nodiscard]] inline constexpr PixelFormat ToPixelFormat(GLEnum GLInternalFormat, GLEnum GLType);

	[[nodiscard]] inline constexpr ColorSpace ToColorSpace(GLEnum GLInternalFormat, GLEnum GLType);

	[[nodiscard]] inline constexpr ChannelType ToChannelType(GLEnum GLInternalFormat, GLEnum GLType);

	[[nodiscard]] inline constexpr PixelFormat ToPixelFormat(GLEnum GLInternalFormat, GLEnum GLType)
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
		}

		return PixelFormat::Invalid;
	}

	[[nodiscard]] inline constexpr ColorSpace ToColorSpace(GLEnum GLInternalFormat, GLEnum GLType)
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
		};

		return ColorSpace::Invalid;
	}

	[[nodiscard]] inline constexpr ChannelType ToChannelType(GLEnum GLInternalFormat, GLEnum GLType)
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
		}

		return ChannelType::Invalid;
	}
}