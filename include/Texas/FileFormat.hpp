#pragma once

namespace Texas
{
    enum class FileFormat : char
    {
        Invalid,

#if defined(TEXAS_ENABLE_KTX_READ) || defined(TEXAS_ENABLE_KTX_WRITE)
        KTX,
#endif

#if defined(TEXAS_ENABLE_PNG_READ) || defined(TEXAS_ENABLE_PNG_WRITE)
        PNG,
#endif
    };
}