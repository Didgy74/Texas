#pragma once

#include <cstdint>

namespace Texas::detail
{
    struct BlockInfo
    {
        std::uint8_t width;
        std::uint8_t height;
        std::uint8_t depth;
        std::uint8_t size;
    };
}