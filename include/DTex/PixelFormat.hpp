#pragma once

#include <cstdint>
#include <string_view>
#include <cassert>

namespace DTex
{
	enum class PixelFormat : uint8_t
	{
		Invalid,

		// Standard
		R_8,
		RG_8,
		RGB_8,
		RGBA_8,

		// BCn
		BC1_RGB,
		BC1_RGBA,
		BC2,
		BC3,
		BC4_Unsigned,
		BC4_Signed,
		BC5_Unsigned,
		BC5_Signed,
		BC6H_UFloat,
		BC6H_SFloat,
		BC7,

		// MUST BE AT END
		COUNT
	};
	
	constexpr bool IsEnumerated(PixelFormat pixelFormat)
	{
		return static_cast<uint8_t>(pixelFormat) < static_cast<uint8_t>(PixelFormat::COUNT);
	}

	constexpr bool IsValid(PixelFormat pixelFormat)
	{
		return IsEnumerated(pixelFormat) && pixelFormat != PixelFormat::Invalid;
	}

	constexpr std::string_view ToString(PixelFormat pixelFormat)
	{
		assert(IsEnumerated(pixelFormat) && "Error. Invalid enum value sent to DTex::ToString(PixelFormat).");

		switch (pixelFormat)
		{
		case PixelFormat::Invalid:
			return "PixelFormat::Invalid";
		case PixelFormat::R_8:
			return "PixelFormat::R_8";
		case PixelFormat::RG_8:
			return "PixelFormat::RG_8";
		case PixelFormat::RGB_8:
			return "PixelFormat::RGB_8";
		case PixelFormat::RGBA_8:
			return "PixelFormat::RGBA_8";


			// BCn
		case PixelFormat::BC1_RGB:
			return "PixelFormat::BC1_RGB";
		case PixelFormat::BC1_RGBA:
			return "PixelFormat::BC1_RGBA";
		case PixelFormat::BC2:
			return "PixelFormat::BC2";
		case PixelFormat::BC3:
			return "PixelFormat::BC3";
		case PixelFormat::BC4_Unsigned:
			return "PixelFormat::BC4_Unsigned";
		case PixelFormat::BC4_Signed:
			return "PixelFormat::BC4_Signed";
		case PixelFormat::BC5_Unsigned:
			return "PixelFormat::BC5_Unsigned";
		case PixelFormat::BC5_Signed:
			return "PixelFormat::BC5_Signed";
		case PixelFormat::BC6H_UFloat:
			return "PixelFormat::BC6H_UFloat";
		case PixelFormat::BC6H_SFloat:
			return "PixelFormat::BC6H_SFloat";
		case PixelFormat::BC7:
			return "PixelFormat::BC7";
			
		default:
			return "Invalid enum value.";
		}
	}
}
