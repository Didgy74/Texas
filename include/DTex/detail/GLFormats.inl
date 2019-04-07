#pragma once

#include <cstdint>

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

			// BCn
			constexpr auto GL_COMPRESSED_RGBA_BPTC_UNORM = 0x8E8C;
			constexpr auto GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM = 0x8E8D;
		}

		namespace GLTypes
		{
			constexpr auto GL_UNSIGNED_BYTE = 0x1401;
		}
	}
}

#include "../TextureDocument.hpp"

namespace DTex
{
	namespace detail
	{
		constexpr uint32_t ToGLTarget(TextureDocument::Target target)
		{
			using namespace GLTextureTargets;

			using T = TextureDocument::Target;

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

		constexpr uint32_t ToGLType(TextureDocument::Format format)
		{
			using namespace GLTypes;

			using F = TextureDocument::Format;

			switch (format)
			{
			case F::R8G8B8_UNorm:
			case F::R8G8B8A8_UNorm:
				return GL_UNSIGNED_BYTE;
			}

			return 0;
		}

		constexpr uint32_t ToGLFormat(TextureDocument::Format format)
		{
			using namespace GLFormats;

			using F = TextureDocument::Format;

			switch (format)
			{
				// Standard
			case F::R8G8B8_UNorm:
				return GL_RGB;
			case F::R8G8B8A8_UNorm:
				return GL_RGBA;

				// BCn
			case F::BC7_RGBA_Unorm:
				return GL_COMPRESSED_RGBA_BPTC_UNORM;
			case F::BC7_sRGB_Unorm:
				return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
			}

			return 0;
		}

		constexpr TextureDocument::Format GLFormatAndGLTypeToFormat(uint32_t glFormat, uint32_t glType)
		{
			using namespace GLFormats;
			using namespace GLTypes;

			using F = TextureDocument::Format;

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

				// BCn
			case GL_COMPRESSED_RGBA_BPTC_UNORM:
				return F::BC7_RGBA_Unorm;
			case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
				return F::BC7_sRGB_Unorm;
			}

			return F::Invalid;
		}
	}
}