#pragma once

#include <cstdint>
#include <cstddef>

namespace Texas::detail
{
    struct FileInfo_KTX_BackendData
    {
    };

	struct FileInfo_PNG_BackendData
    {
        std::size_t firstIdatChunkStreamPos = 0;
        std::uint32_t maxIdatChunkDataLength = 0;
        // Leave as 0 if no PLTE chunk is found.
        std::size_t plteChunkStreamPos = 0;
        std::uint32_t plteChunkDataLength = 0;
    };

    union FileInfo_BackendData
    {
#ifdef TEXAS_ENABLE_KTX_READ
        FileInfo_KTX_BackendData ktx{};
#endif
#ifdef TEXAS_ENABLE_PNG_READ
        FileInfo_PNG_BackendData png;
#endif
    };
}