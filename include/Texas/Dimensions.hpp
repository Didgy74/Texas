#pragma once

#include <cstdint>

namespace Texas
{
    /*
        Specifies the dimensions of a texture in terms of pixels.
    */
    struct Dimensions
    {
        std::uint64_t width;
        std::uint64_t height;
        std::uint64_t depth;
    };
}

