#pragma once

#include "DTex/PixelFormat.hpp"
#include "DTex/Colorspace.hpp"

#include <cstdint>
#include <utility>

namespace DTex
{
	namespace Tools
	{
		namespace GLEnums
		{
			constexpr uint32_t gl_TEXTURE_1D = 0x0DE0;
			constexpr uint32_t gl_TEXTURE_2D = 0x0DE1;
			constexpr uint32_t gl_TEXTURE_3D = 0x806F;

			constexpr auto gl_UNSIGNED_BYTE = 0x1401;

			// Standard
			constexpr uint32_t gl_RED = 0x1903;
			constexpr uint32_t gl_RG = 0x8227;
			constexpr uint32_t gl_RGB = 0x1907;
			constexpr uint32_t gl_RGBA = 0x1908;

			constexpr uint32_t gl_R8 = 0x8229;
			constexpr uint32_t gl_RG8 = 0x822B;
			constexpr uint32_t gl_RGB8 = 0x8051;
			constexpr uint32_t gl_RGBA8 = 0x8058;

			constexpr uint32_t gl_SRGB = 0x8C40;
			constexpr uint32_t gl_SRGB8 = 0x8C41; // Core
			constexpr uint32_t gl_SRGB_ALPHA = 0x8C42;
			constexpr uint32_t gl_SRGB8_ALPHA8 = 0x8C43; // Core

			// BCn
			// BC1 - BC3
			constexpr uint32_t gl_COMPRESSED_SRGB_S3TC_DXT1_EXT = 0x8C4C;
			constexpr uint32_t gl_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT = 0x8C4D;
			constexpr uint32_t gl_COMPRESSED_RGB_S3TC_DXT1_ANGLE = 0x83F0;
			constexpr uint32_t gl_COMPRESSED_RGBA_S3TC_DXT1_ANGLE = 0x83F1;
			constexpr uint32_t gl_COMPRESSED_RGBA_S3TC_DXT3_ANGLE = 0x83F2;
			constexpr uint32_t gl_COMPRESSED_RGBA_S3TC_DXT5_ANGLE = 0x83F3;
			// BC4 - BC5
			constexpr uint32_t gl_COMPRESSED_RED_RGTC1 = 0x8DBB;
			constexpr uint32_t gl_COMPRESSED_SIGNED_RED_RGTC1 = 0x8DBC;
			constexpr uint32_t gl_COMPRESSED_RG_RGTC2 = 0x8DBD;
			constexpr uint32_t gl_COMPRESSED_SIGNED_RG_RGTC2 = 0x8DBE;
			// B6H - BC7
			constexpr uint32_t gl_COMPRESSED_RGB_BPTC_SIGNED_FLOAT = 0x8E8E;
			constexpr uint32_t gl_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = 0x8E8F;
			constexpr uint32_t gl_COMPRESSED_RGBA_BPTC_UNORM = 0x8E8C;
			constexpr uint32_t gl_COMPRESSED_SRGB_ALPHA_BPTC_UNORM = 0x8E8D;
		}

	}

	inline constexpr uint32_t ToGLTarget(TextureType target)
	{
		using namespace Tools::GLEnums;

		using T = TextureType;

		switch (target)
		{
		case T::Texture1D:
			return gl_TEXTURE_1D;
		case T::Texture2D:
			return gl_TEXTURE_2D;
		case T::Texture3D:
			return gl_TEXTURE_3D;
		}

		return 0;
	}

	inline constexpr uint32_t ToGLType(PixelFormat format)
	{
		using namespace Tools::GLEnums;

		switch (format)
		{
		case PixelFormat::RGB_8:
		case PixelFormat::RGBA_8:
			return gl_UNSIGNED_BYTE;
		}

		return 0;
	}

	/*
		Returns 0 on error.
	*/
	inline constexpr uint32_t ToGLFormat(PixelFormat format)
	{
		using namespace Tools::GLEnums;

		switch (format)
		{
		case PixelFormat::R_8:
			return gl_RED;
		case PixelFormat::RG_8:
			return gl_RG;
		case PixelFormat::RGB_8:
			return gl_RGB;
		case PixelFormat::RGBA_8:
			return gl_RGBA;
		}

		return 0;
	}

	/*
		Returns 0 on error.
	*/
	inline constexpr uint32_t ToGLInternalFormat(PixelFormat format, ColorSpace colorSpace)
	{
		using namespace Tools::GLEnums;

		switch (format)
		{
			// Standard
		case PixelFormat::R_8:
			return gl_R8;
		case PixelFormat::RG_8:
			return gl_RG8;
		case PixelFormat::RGB_8:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return gl_RGB8;
			case ColorSpace::sRGB:
				return gl_SRGB8;
			}
			break;
		case PixelFormat::RGBA_8:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return gl_RGBA8;
			case ColorSpace::sRGB:
				return gl_SRGB8_ALPHA8;
			}
			break;


			/*
				BCn
			*/
		case PixelFormat::BC1_RGB:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return gl_COMPRESSED_RGB_S3TC_DXT1_ANGLE;
			case ColorSpace::sRGB:
				return gl_COMPRESSED_SRGB_S3TC_DXT1_EXT;
			}
			break;
		case PixelFormat::BC1_RGBA:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return gl_COMPRESSED_RGBA_S3TC_DXT1_ANGLE;
			case ColorSpace::sRGB:
				return gl_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
			}
			break;
		case PixelFormat::BC2:
			return gl_COMPRESSED_RGBA_S3TC_DXT3_ANGLE;
		case PixelFormat::BC3:
			return gl_COMPRESSED_RGBA_S3TC_DXT5_ANGLE;
		case PixelFormat::BC4_Unsigned:
			return gl_COMPRESSED_RED_RGTC1;
		case PixelFormat::BC4_Signed:
			return gl_COMPRESSED_SIGNED_RED_RGTC1;
		case PixelFormat::BC5_Unsigned:
			return gl_COMPRESSED_RG_RGTC2;
		case PixelFormat::BC5_Signed:
			return gl_COMPRESSED_SIGNED_RG_RGTC2;
		case PixelFormat::BC6H_SFloat:
			return gl_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
		case PixelFormat::BC6H_UFloat:
			return gl_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
		case PixelFormat::BC7:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return gl_COMPRESSED_RGBA_BPTC_UNORM;
			case ColorSpace::sRGB:
				return gl_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
			}
			break;
		}

		return 0;
	}

	constexpr std::pair<PixelFormat, ColorSpace> ToPixelFormat(uint32_t glFormat, uint32_t glType)
	{
		using namespace Tools::GLEnums;

		switch (glFormat)
		{
			// Standard
		case gl_RGB:
			switch (glType)
			{
			case gl_UNSIGNED_BYTE:
				return { PixelFormat::RGB_8, ColorSpace::Linear };
			}
			break;
		case gl_RGBA:
			switch (glType)
			{
			case gl_UNSIGNED_BYTE:
				return { PixelFormat::RGBA_8,  ColorSpace::Linear };
			}
			break;
		case gl_RGB8:
			return { PixelFormat::RGB_8, ColorSpace::Linear };
		case gl_RGBA8:
			return { PixelFormat::RGBA_8, ColorSpace::Linear };
		}

		switch (glFormat)
		{
			// BCn
		case gl_COMPRESSED_RGB_S3TC_DXT1_ANGLE:
			return { PixelFormat::BC1_RGB, ColorSpace::Linear };
		case gl_COMPRESSED_SRGB_S3TC_DXT1_EXT:
			return { PixelFormat::BC1_RGB, ColorSpace::sRGB };
		case gl_COMPRESSED_RGBA_S3TC_DXT1_ANGLE:
			return { PixelFormat::BC1_RGBA, ColorSpace::Linear };
		case gl_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
			return { PixelFormat::BC1_RGBA, ColorSpace::sRGB };
		case gl_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
			return { PixelFormat::BC2, ColorSpace::Linear };
		case gl_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
			return { PixelFormat::BC3, ColorSpace::sRGB };
		case gl_COMPRESSED_RED_RGTC1:
			return { PixelFormat::BC4_Unsigned, ColorSpace::Linear };
		case gl_COMPRESSED_SIGNED_RED_RGTC1:
			return { PixelFormat::BC4_Signed, ColorSpace::Linear };
		case gl_COMPRESSED_RG_RGTC2:
			return { PixelFormat::BC5_Unsigned, ColorSpace::Linear };
		case gl_COMPRESSED_SIGNED_RG_RGTC2:
			return { PixelFormat::BC5_Signed, ColorSpace::Linear };
		case gl_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
			return { PixelFormat::BC6H_SFloat, ColorSpace::Linear };
		case gl_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
			return { PixelFormat::BC6H_UFloat, ColorSpace::Linear };
		case gl_COMPRESSED_RGBA_BPTC_UNORM:
			return { PixelFormat::BC7, ColorSpace::Linear };
		case gl_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
			return { PixelFormat::BC7, ColorSpace::sRGB };

		}
		return { PixelFormat::Invalid, ColorSpace::Invalid };
	}
}