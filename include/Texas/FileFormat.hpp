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

        COUNT
    };

    constexpr bool IsEnumerated(FileFormat fileFormat)
    {
        return static_cast<uint8_t>(fileFormat) < static_cast<uint8_t>(FileFormat::COUNT);
    }

    constexpr const char* ToString(FileFormat fileFormat)
    {
        switch(fileFormat)
        {
        case FileFormat::Unsupported:
            return "Texas::FileFormat::Unsupported";

// KTX
#if defined(TEXAS_ENABLE_KTX_READ) || defined(TEXAS_ENABLE_KTX_WRITE)
        case FileFormat::KTX:
            return "Texas::FileFormat::KTX";
#endif

// KTX2
#if defined(TEXAS_ENABLE_KTX2_READ) || defined(TEXAS_ENABLE_KTX2_WRITE)
        case FileFormat::KTX2:
            return "Texas::FileFormat::KTX2";
#endif

// PNG
#if defined(TEXAS_ENABLE_PNG_READ) || defined(TEXAS_ENABLE_PNG_WRITE)
        case FileFormat::PNG:
            return "Texas::FileFormat::PNG";
#endif

        default:
            return nullptr;
        }
    }
}