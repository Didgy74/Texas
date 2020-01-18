#pragma once

#include <cstdint>

namespace Texas
{
    enum class ColorSpace : std::uint8_t
    {
        Invalid,

        Linear,
        sRGB,
    };
}