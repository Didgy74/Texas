#pragma once

#include <cstdint>

#include "../Typedefs.hpp"

namespace DTex
{
	namespace detail
	{
		namespace VkImageTypes
		{
			constexpr auto VK_IMAGE_TYPE_1D = 0;
			constexpr auto VK_IMAGE_TYPE_2D = 1;
			constexpr auto VK_IMAGE_TYPE_3D = 2;
		}

		constexpr uint32_t ToVkImageType(Type type)
		{
			using namespace VkImageTypes;

			using T = Type;

			switch (type)
			{
			case T::Texture1D:
				return VK_IMAGE_TYPE_1D;
			case T::Texture2D:
				return VK_IMAGE_TYPE_2D;
			case T::Texture3D:
				return VK_IMAGE_TYPE_3D;
			}

			return static_cast<uint32_t>(-1);
		}

		namespace VkFormats
		{
			constexpr auto VK_FORMAT_UNDEFINED = 0;

			// Standard
			constexpr auto VK_FORMAT_R8G8B8_UNORM = 23;
			constexpr auto VK_FORMAT_R8G8B8_SRGB = 29;

			constexpr auto VK_FORMAT_R8G8B8A8_UNORM = 37;
			constexpr auto VK_FORMAT_R8G8B8A8_SRGB = 43;

			// BCn
			constexpr auto VK_FORMAT_BC1_RGB_UNORM_BLOCK = 131;
			constexpr auto VK_FORMAT_BC1_RGB_SRGB_BLOCK = 132;
			constexpr auto VK_FORMAT_BC1_RGBA_UNORM_BLOCK = 133;
			constexpr auto VK_FORMAT_BC1_RGBA_SRGB_BLOCK = 134;
			constexpr auto VK_FORMAT_BC2_UNORM_BLOCK = 135;
			constexpr auto VK_FORMAT_BC2_SRGB_BLOCK = 136;
			constexpr auto VK_FORMAT_BC3_UNORM_BLOCK = 137;
			constexpr auto VK_FORMAT_BC3_SRGB_BLOCK = 138;
			constexpr auto VK_FORMAT_BC4_UNORM_BLOCK = 139;
			constexpr auto VK_FORMAT_BC4_SNORM_BLOCK = 140;
			constexpr auto VK_FORMAT_BC5_UNORM_BLOCK = 141;
			constexpr auto VK_FORMAT_BC5_SNORM_BLOCK = 142;
			constexpr auto VK_FORMAT_BC6H_UFLOAT_BLOCK = 143;
			constexpr auto VK_FORMAT_BC6H_SFLOAT_BLOCK = 144;
			constexpr auto VK_FORMAT_BC7_UNORM_BLOCK = 145;
			constexpr auto VK_FORMAT_BC7_SRGB_BLOCK = 146;
		}

		constexpr uint32_t ToVkFormat(Format format)
		{
			using namespace VkFormats;

			using F = Format;

			switch (format)
			{


			// Standard
			case F::R8G8B8_UNorm:
				return VK_FORMAT_R8G8B8_UNORM;
			case F::R8G8B8A8_UNorm:
				return VK_FORMAT_R8G8B8A8_UNORM;


			// BCn
			case F::BC1_RGB_UNorm:
				return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
			case F::BC1_RGB_sRGB:
				return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
			case F::BC1_RGBA_UNorm:
				return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
			case F::BC1_RGBA_sRGB:
				return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
			case F::BC2_UNorm:
				return VK_FORMAT_BC2_UNORM_BLOCK;
			case F::BC2_sRGB:
				return VK_FORMAT_BC2_SRGB_BLOCK;
			case F::BC3_UNorm:
				return VK_FORMAT_BC3_UNORM_BLOCK;
			case F::BC3_sRGB:
				return VK_FORMAT_BC3_SRGB_BLOCK;
			case F::BC4_UNorm:
				return VK_FORMAT_BC4_UNORM_BLOCK;
			case F::BC4_SNorm:
				return VK_FORMAT_BC4_SNORM_BLOCK;
			case F::BC5_UNorm:
				return VK_FORMAT_BC5_UNORM_BLOCK;
			case F::BC5_SNorm:
				return VK_FORMAT_BC5_SNORM_BLOCK;
			case F::BC6H_UFloat:
				return VK_FORMAT_BC6H_UFLOAT_BLOCK;
			case F::BC6H_SFloat:
				return VK_FORMAT_BC6H_SFLOAT_BLOCK;
			case F::BC7_UNorm:
				return VK_FORMAT_BC7_UNORM_BLOCK;
			case F::BC7_sRGB:
				return VK_FORMAT_BC7_SRGB_BLOCK;
			}

			return VK_FORMAT_UNDEFINED;
		}
	}
}