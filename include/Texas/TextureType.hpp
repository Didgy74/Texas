#pragma once

namespace Texas
{
    /*
        Enum for categorizing textures.
    */
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
