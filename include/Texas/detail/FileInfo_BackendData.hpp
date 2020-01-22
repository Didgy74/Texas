#pragma once

#include <cstdint>

namespace Texas::detail
{
    struct FileInfo_KTX_BackendData
    {
        const unsigned char* srcFileBufferStart = nullptr;
        std::uint64_t srcFileBufferLength = 0;
        const unsigned char* srcImageDataStart = nullptr;
    };

	struct FileInfo_PNG_BackendData
    {
        const unsigned char* srcFileBufferStart = nullptr;
        std::uint64_t srcFileBufferLength = 0;
        std::uint64_t idatChunkOffset = 0;
        std::uint32_t idatChunkCount = 0;
        std::uint64_t plteChunkOffset = 0;
        unsigned int plteChunkDataLength = 0;
    };

    struct FileInfo_BackendData
    {
#ifdef TEXAS_ENABLE_KTX_READ
        FileInfo_KTX_BackendData ktx;
#endif
#ifdef TEXAS_ENABLE_PNG_READ
        FileInfo_PNG_BackendData png;
#endif
    };
}