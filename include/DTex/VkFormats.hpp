#pragma once

#include <cstdint>

#include "DTex/PixelFormat.hpp"
#include "DTex/Colorspace.hpp"

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

	/*
		Returns max value of uint32_t upon failure.
	*/
	constexpr uint32_t ToVkImageType(TextureType type)
	{
		using namespace detail::VkImageTypes;

		switch (type)
		{
		case TextureType::Texture1D:
			return vk_IMAGE_TYPE_1D;
		case TextureType::Texture2D:
			return vk_IMAGE_TYPE_2D;
		case TextureType::Texture3D:
			return vk_IMAGE_TYPE_3D;
		}

		return static_cast<uint32_t>(-1);
	}

	constexpr uint32_t ToVkFormat(PixelFormat format, ColorSpace colorSpace)
	{
		using namespace detail::VkFormats;

		switch (format)
		{

		// Standard
		case PixelFormat::RGB_8:
			return vk_FORMAT_R8G8B8_UNORM;
		case PixelFormat::RGBA_8:
			return vk_FORMAT_R8G8B8A8_UNORM;


		// BCn
		case PixelFormat::BC1_RGB:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return vk_FORMAT_BC1_RGB_UNORM_BLOCK;
			case ColorSpace::sRGB:
				return vk_FORMAT_BC1_RGB_SRGB_BLOCK;
			}
		case PixelFormat::BC1_RGBA:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return vk_FORMAT_BC1_RGBA_UNORM_BLOCK;
			case ColorSpace::sRGB:
				return vk_FORMAT_BC1_RGBA_SRGB_BLOCK;
			}
		case PixelFormat::BC2:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return vk_FORMAT_BC2_UNORM_BLOCK;
			case ColorSpace::sRGB:
				return vk_FORMAT_BC2_SRGB_BLOCK;
			}
		case PixelFormat::BC3:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return vk_FORMAT_BC3_UNORM_BLOCK;
			case ColorSpace::sRGB:
				return vk_FORMAT_BC3_SRGB_BLOCK;
			}
		case PixelFormat::BC4_Unsigned:
			return vk_FORMAT_BC4_UNORM_BLOCK;
		case PixelFormat::BC4_Signed:
			return vk_FORMAT_BC4_SNORM_BLOCK;
		case PixelFormat::BC5_Unsigned:
			return vk_FORMAT_BC5_UNORM_BLOCK;
		case PixelFormat::BC5_Signed:
			return vk_FORMAT_BC5_SNORM_BLOCK;
		case PixelFormat::BC6H_UFloat:
			return vk_FORMAT_BC6H_UFLOAT_BLOCK;
		case PixelFormat::BC6H_SFloat:
			return vk_FORMAT_BC6H_SFLOAT_BLOCK;
		case PixelFormat::BC7:
			switch (colorSpace)
			{
			case ColorSpace::Linear:
				return vk_FORMAT_BC7_UNORM_BLOCK;
			case ColorSpace::sRGB:
				return vk_FORMAT_BC7_SRGB_BLOCK;
			}
		}

		return vk_FORMAT_UNDEFINED;
	}
}