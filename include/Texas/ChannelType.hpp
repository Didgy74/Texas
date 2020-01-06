#pragma once

#include <cstdint>

namespace Texas
{
    enum class ChannelType : std::uint8_t
    {
        Invalid,

        UnsignedNormalized,
        SignedNormalized,

        UnsignedScaled,
        SignedScaled,

        UnsignedInteger,
        SignedInteger,

        UnsignedFloat,
        SignedFloat,

        sRGB,

        // Holds the amount of enumerations in Texas::ChannelType
        COUNT
    };

    constexpr const char* ToString(ChannelType pixelFormat)
    {
        return nullptr;
    }
}
