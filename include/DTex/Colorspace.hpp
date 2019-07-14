#pragma once

#include <cstdint>
#include <string_view>
#include <cassert>

namespace DTex
{
	enum class ColorSpace : uint8_t
	{
		Invalid,
		Linear,
		sRGB,
		COUNT
	};

	constexpr bool IsEnumerated(ColorSpace colorSpace)
	{
		return static_cast<uint8_t>(colorSpace) < static_cast<uint8_t>(ColorSpace::COUNT);
	}

	constexpr bool IsValid(ColorSpace colorSpace)
	{
		return IsEnumerated(colorSpace) && colorSpace != ColorSpace::Invalid;
	}

	constexpr std::string_view ToString(ColorSpace colorSpace)
	{
		assert(IsEnumerated(colorSpace) && "Error. Invalid value sent to DTex::ToString(ColorSpace).");

		switch (colorSpace)
		{
		case ColorSpace::Invalid:
			return "ColorSpace::Invalid";
		case ColorSpace::Linear:
			return "ColorSpace::Linear";
		case ColorSpace::sRGB:
			return "ColorSpace::sRGB";
		default:
			return "Invalid enum value.";
		}
	}
}