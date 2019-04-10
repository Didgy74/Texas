#pragma once

#include <cstdint>

#include "Typedefs.hpp"

namespace DTex
{
	namespace detail
	{
		namespace VkImageTypes
		{
			constexpr auto vk_IMAGE_TYPE_1D = 0;
			constexpr auto vk_IMAGE_TYPE_2D = 1;
			constexpr auto vk_IMAGE_TYPE_3D = 2;
		}

		namespace VkFormats
		{
			constexpr auto vk_FORMAT_UNDEFINED = 0;

			// Standard
			constexpr auto vk_FORMAT_R8G8B8_UNORM = 23;
			constexpr auto vk_FORMAT_R8G8B8_SRGB = 29;

			constexpr auto vk_FORMAT_R8G8B8A8_UNORM = 37;
			constexpr auto vk_FORMAT_R8G8B8A8_SRGB = 43;

			// BCn
			constexpr auto vk_FORMAT_BC1_RGB_UNORM_BLOCK = 131;
			constexpr auto vk_FORMAT_BC1_RGB_SRGB_BLOCK = 132;
			constexpr auto vk_FORMAT_BC1_RGBA_UNORM_BLOCK = 133;
			constexpr auto vk_FORMAT_BC1_RGBA_SRGB_BLOCK = 134;
			constexpr auto vk_FORMAT_BC2_UNORM_BLOCK = 135;
			constexpr auto vk_FORMAT_BC2_SRGB_BLOCK = 136;
			constexpr auto vk_FORMAT_BC3_UNORM_BLOCK = 137;
			constexpr auto vk_FORMAT_BC3_SRGB_BLOCK = 138;
			constexpr auto vk_FORMAT_BC4_UNORM_BLOCK = 139;
			constexpr auto vk_FORMAT_BC4_SNORM_BLOCK = 140;
			constexpr auto vk_FORMAT_BC5_UNORM_BLOCK = 141;
			constexpr auto vk_FORMAT_BC5_SNORM_BLOCK = 142;
			constexpr auto vk_FORMAT_BC6H_UFLOAT_BLOCK = 143;
			constexpr auto vk_FORMAT_BC6H_SFLOAT_BLOCK = 144;
			constexpr auto vk_FORMAT_BC7_UNORM_BLOCK = 145;
			constexpr auto vk_FORMAT_BC7_SRGB_BLOCK = 146;
		}
	}

	constexpr uint32_t ToVkImageType(TextureType type)
	{
		using namespace detail::VkImageTypes;

		using T = TextureType;

		switch (type)
		{
		case T::Texture1D:
			return vk_IMAGE_TYPE_1D;
		case T::Texture2D:
			return vk_IMAGE_TYPE_2D;
		case T::Texture3D:
			return vk_IMAGE_TYPE_3D;
		}

		return static_cast<uint32_t>(-1);
	}

	constexpr uint32_t ToVkFormat(PixelFormat format)
	{
		using namespace detail::VkFormats;

		using F = PixelFormat;

		switch (format)
		{


		// Standard
		case F::R8G8B8_UNorm:
			return vk_FORMAT_R8G8B8_UNORM;
		case F::R8G8B8A8_UNorm:
			return vk_FORMAT_R8G8B8A8_UNORM;


		// BCn
		case F::BC1_RGB_UNorm:
			return vk_FORMAT_BC1_RGB_UNORM_BLOCK;
		case F::BC1_RGB_sRGB:
			return vk_FORMAT_BC1_RGB_SRGB_BLOCK;
		case F::BC1_RGBA_UNorm:
			return vk_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case F::BC1_RGBA_sRGB:
			return vk_FORMAT_BC1_RGBA_SRGB_BLOCK;
		case F::BC2_UNorm:
			return vk_FORMAT_BC2_UNORM_BLOCK;
		case F::BC2_sRGB:
			return vk_FORMAT_BC2_SRGB_BLOCK;
		case F::BC3_UNorm:
			return vk_FORMAT_BC3_UNORM_BLOCK;
		case F::BC3_sRGB:
			return vk_FORMAT_BC3_SRGB_BLOCK;
		case F::BC4_UNorm:
			return vk_FORMAT_BC4_UNORM_BLOCK;
		case F::BC4_SNorm:
			return vk_FORMAT_BC4_SNORM_BLOCK;
		case F::BC5_UNorm:
			return vk_FORMAT_BC5_UNORM_BLOCK;
		case F::BC5_SNorm:
			return vk_FORMAT_BC5_SNORM_BLOCK;
		case F::BC6H_UFloat:
			return vk_FORMAT_BC6H_UFLOAT_BLOCK;
		case F::BC6H_SFloat:
			return vk_FORMAT_BC6H_SFLOAT_BLOCK;
		case F::BC7_UNorm:
			return vk_FORMAT_BC7_UNORM_BLOCK;
		case F::BC7_sRGB:
			return vk_FORMAT_BC7_SRGB_BLOCK;
		}

		return vk_FORMAT_UNDEFINED;
	}
}