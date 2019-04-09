#pragma once

#include <cstdint>

#include "Typedefs.hpp"

namespace DTex
{
	namespace detail
	{
		namespace GLTextureTargets
		{
			constexpr auto GL_TEXTURE_1D = 0x0DE0;
			constexpr auto GL_TEXTURE_2D = 0x0DE1;
			constexpr auto GL_TEXTURE_3D = 0x806F;
		}

		namespace GLFormats
		{
			// Standard
			constexpr auto GL_RGB = 0x1907;
			constexpr auto GL_RGBA = 0x1908;
			constexpr auto GL_RGB4 = 0x804F;
			constexpr auto GL_RGB5 = 0x8050;
			constexpr auto GL_RGB8 = 0x8051;
			constexpr auto GL_RGB10 = 0x8052;
			constexpr auto GL_RGB12 = 0x8053;
			constexpr auto GL_RGB16 = 0x8054;
			constexpr auto GL_RGBA2 = 0x8055;
			constexpr auto GL_RGBA4 = 0x8056;
			constexpr auto GL_RGB5_A1 = 0x8057;
			constexpr auto GL_RGBA8 = 0x8058;
			constexpr auto GL_RGB10_A2 = 0x8059;
			constexpr auto GL_RGBA12 = 0x805A;
			constexpr auto GL_RGBA16 = 0x805B;

			// BCn
			// BC1 - BC3
			constexpr auto GL_COMPRESSED_SRGB_S3TC_DXT1_EXT = 0x8C4C;
			constexpr auto GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT = 0x8C4D;
			constexpr auto GL_COMPRESSED_RGB_S3TC_DXT1_ANGLE = 0x83F0;
			constexpr auto GL_COMPRESSED_RGBA_S3TC_DXT1_ANGLE = 0x83F1;
			constexpr auto GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE = 0x83F2;
			constexpr auto GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE = 0x83F3;
			// BC4 - BC5
			constexpr auto GL_COMPRESSED_RED_RGTC1 = 0x8DBB;
			constexpr auto GL_COMPRESSED_SIGNED_RED_RGTC1 = 0x8DBC;
			constexpr auto GL_COMPRESSED_RG_RGTC2 = 0x8DBD;
			constexpr auto GL_COMPRESSED_SIGNED_RG_RGTC2 = 0x8DBE;
			// B6H - BC7
			constexpr auto GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT = 0x8E8E;
			constexpr auto GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT = 0x8E8F;
			constexpr auto GL_COMPRESSED_RGBA_BPTC_UNORM = 0x8E8C;
			constexpr auto GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM = 0x8E8D;
		}

		namespace GLTypes
		{
			constexpr auto GL_UNSIGNED_BYTE = 0x1401;
		}
	}

	inline constexpr uint32_t ToGLTarget(TextureType target)
	{
		using namespace detail::GLTextureTargets;

		using T = TextureType;

		switch (target)
		{
		case T::Texture1D:
			return GL_TEXTURE_1D;
		case T::Texture2D:
			return GL_TEXTURE_2D;
		case T::Texture3D:
			return GL_TEXTURE_3D;
		}

		return 0;
	}

	inline constexpr uint32_t ToGLType(PixelFormat format)
	{
		using namespace detail::GLTypes;

		using F = PixelFormat;

		switch (format)
		{
		case F::R8G8B8_UNorm:
		case F::R8G8B8A8_UNorm:
			return GL_UNSIGNED_BYTE;
		}

		return 0;
	}

	inline constexpr uint32_t ToGLFormat(PixelFormat format)
	{
		using namespace detail::GLFormats;

		using F = PixelFormat;

		switch (format)
		{
			// Standard
		case F::R8G8B8_UNorm:
			return GL_RGB;
		case F::R8G8B8A8_UNorm:
			return GL_RGBA;



			/*
				BCn
			*/
		case F::BC1_RGB_UNorm:
			return GL_COMPRESSED_RGB_S3TC_DXT1_ANGLE;
		case F::BC1_RGB_sRGB:
			return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
		case F::BC1_RGBA_UNorm:
			return GL_COMPRESSED_RGBA_S3TC_DXT1_ANGLE;
		case F::BC1_RGBA_sRGB:
			return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
		case F::BC2_UNorm:
			return GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE;
		case F::BC3_UNorm:
			return GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE;
		case F::BC4_UNorm:
			return GL_COMPRESSED_RED_RGTC1;
		case F::BC4_SNorm:
			return GL_COMPRESSED_SIGNED_RED_RGTC1;
		case F::BC5_UNorm:
			return GL_COMPRESSED_RG_RGTC2;
		case F::BC5_SNorm:
			return GL_COMPRESSED_SIGNED_RG_RGTC2;
		case F::BC6H_SFloat:
			return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
		case F::BC6H_UFloat:
			return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
		case F::BC7_UNorm:
			return GL_COMPRESSED_RGBA_BPTC_UNORM;
		case F::BC7_sRGB:
			return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
		}

		return 0;
	}

	inline constexpr PixelFormat ToFormat(uint32_t glFormat, uint32_t glType, bool& isCompressedFormat)
	{
		using namespace detail::GLFormats;
		using namespace detail::GLTypes;

		using F = PixelFormat;

		isCompressedFormat = false;
		switch (glFormat)
		{

			// Standard
		case GL_RGB:
			switch (glType)
			{
			case GL_UNSIGNED_BYTE:
				return F::R8G8B8_UNorm;
			}
		case GL_RGBA:
			switch (glType)
			{
			case GL_UNSIGNED_BYTE:
				return F::R8G8B8A8_UNorm;
			}
				
		case GL_RGB8:
			return F::R8G8B8_UNorm;
		case GL_RGBA8:
			return F::R8G8B8A8_UNorm;

		}

		isCompressedFormat = true;
		switch (glFormat)
		{

			// BCn
		case GL_COMPRESSED_RGB_S3TC_DXT1_ANGLE:
			return F::BC1_RGB_UNorm;
		case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
			return F::BC1_RGB_sRGB;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_ANGLE:
			return F::BC1_RGBA_UNorm;
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
			return F::BC1_RGBA_sRGB;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE:
			return F::BC2_UNorm;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE:
			return F::BC3_UNorm;
		case GL_COMPRESSED_RED_RGTC1:
			return F::BC4_UNorm;
		case GL_COMPRESSED_SIGNED_RED_RGTC1:
			return F::BC4_SNorm;
		case GL_COMPRESSED_RG_RGTC2:
			return F::BC5_UNorm;
		case GL_COMPRESSED_SIGNED_RG_RGTC2:
			return F::BC5_SNorm;
		case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
			return F::BC6H_SFloat;
		case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
			return F::BC6H_UFloat;
		case GL_COMPRESSED_RGBA_BPTC_UNORM:
			return F::BC7_UNorm;
		case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
			return F::BC7_sRGB;

		}

		isCompressedFormat = false;
		return F::Invalid;
	}

	constexpr PixelFormat ToFormat(uint32_t glFormat, uint32_t glType)
	{
		bool unusedBool{};
		return ToFormat(glFormat, glType, unusedBool);
	}
}