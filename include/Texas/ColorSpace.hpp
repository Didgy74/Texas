#pragma once

namespace Texas
{
    /*
        Provides a hint as to what color-space the image-data is encoded in.

        Note: Alpha channels will always be linearly encoded.
    */
    enum class ColorSpace : char
    {
        Invalid,

        Linear,
        sRGB,
    };
}