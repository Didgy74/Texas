#pragma once

#include <cstdint>

namespace Texas
{
    enum class ColorSpace : std::uint8_t
    {
        Invalid,

        Linear,
        sRGB,

        COUNT
    };

    constexpr bool IsEnumerated(ColorSpace colorSpace)
    {
        return static_cast<std::uint8_t>(colorSpace) < static_cast<std::uint8_t>(ColorSpace::COUNT);
    }

    constexpr bool IsValid(ColorSpace colorSpace)
    {
        return IsEnumerated(colorSpace) && colorSpace != ColorSpace::Invalid;
    }

    constexpr const char* ToString(ColorSpace colorSpace)
    {
        switch (colorSpace)
        {
        case ColorSpace::Invalid:
            return "Texas::ColorSpace::Invalid";
        case ColorSpace::Linear:
            return "Texas::ColorSpace::Linear";
        case ColorSpace::sRGB:
            return "Texas::ColorSpace::sRGB";
        default:
            return "Invalid enum value.";
        }
    }
}