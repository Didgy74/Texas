#pragma once

#include <cstdint>

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
	
	/*
		Return true if input is an enumerated value, is not PixelFormat::COUNT and is not PixelFormat::Invalid.
	*/
	inline constexpr bool IsValid(PixelFormat pixelFormat)
	{
		return static_cast<uint8_t>(pixelFormat) < static_cast<uint8_t>(PixelFormat::COUNT) && pixelFormat != PixelFormat::Invalid;
	}
}
