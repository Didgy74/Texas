#pragma once

#include <cstdint>

namespace Texas::detail
{
    struct MemReqs_KTX_BackendData
    {
        const unsigned char* srcFileBufferStart = nullptr;
        std::uint64_t srcFileBufferLength = 0;
        const unsigned char* srcImageDataStart = nullptr;
    };

	struct MemReqs_PNG_BackendData
    {
        const unsigned char* srcFileBufferStart = nullptr;
        std::uint64_t srcFileBufferLength = 0;
        const unsigned char* idatChunkStart = nullptr;
        std::uint32_t idatChunkCount = 0;
        const unsigned char* plteChunkStart = nullptr;
        std::uint64_t plteChunkDataLength = 0;
    };

    struct MemReqs_BackendData
    {
#ifdef TEXAS_ENABLE_KTX_READ
        MemReqs_KTX_BackendData ktx;
#endif
#ifdef TEXAS_ENABLE_PNG_READ
        MemReqs_PNG_BackendData png;
#endif
    };
}