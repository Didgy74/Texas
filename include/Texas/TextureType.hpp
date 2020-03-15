#pragma once

namespace Texas
{
    enum class TextureType : char
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
    };
}
