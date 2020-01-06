#pragma once

#include <cstdint>

namespace Texas
{
	/*
		R = Red
		G = Green
		B = Blue
		A = Alpha
	*/
	enum class PixelFormat : uint8_t
	{
		Invalid,

		R_8,
		RA_8,
		RG_8,
		RGB_8,
		BGR_8,
		RGBA_8,
		BGRA_8,

		R_16,
		RA_16,
		RG_16,
		RGB_16,
		BGR_16,
		RGBA_16,
		BGRA_16,


		R_32,
		RG_32,
		RGB_32,
		BGR_32,
		RGBA_32,
		BGRA_32,

		BC1_RGB,
		BC1_RGBA,
		BC2_RGBA,
		BC3_RGBA,
		BC4,
		BC5,
		BC6H,
		BC7_RGBA,

		// Holds the amount of enumerations in Texas::PixelFormat
		COUNT
	};
}
