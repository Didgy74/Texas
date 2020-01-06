#pragma once

#include <cstdint>

namespace Texas
{
    enum class TextureType : std::uint8_t
    {
        Invalid,

        Texture1D,
        Texture2D,
        Texture3D,

        Array1D,
        Array2D,
        Array3D,

        Cubemap,
        ArrayCubemap,

        // Holds the amount of enumerated values in Texas::TextureType
        COUNT
    };
}
