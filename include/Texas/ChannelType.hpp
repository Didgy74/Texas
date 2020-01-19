#pragma once

#include <cstdint>

namespace Texas
{
    /*
        Provides a hint as to how each channel
        of an image should be interpretted.
    */
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
    };
}
