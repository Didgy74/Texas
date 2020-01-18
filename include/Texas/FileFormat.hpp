#pragma once

#include <cstdint>

namespace Texas
{
    enum class FileFormat : std::uint8_t
    {
        Unsupported,

#if defined(TEXAS_ENABLE_KTX_READ) || defined(TEXAS_ENABLE_KTX_WRITE)
        KTX,
#endif

#if defined(TEXAS_ENABLE_KTX2_READ) || defined(TEXAS_ENABLE_KTX2_WRITE)
        KTX2,
#endif

#if defined(TEXAS_ENABLE_PNG_READ) || defined(TEXAS_ENABLE_PNG_WRITE)
        PNG,
#endif
    };
}