#pragma once

namespace Texas::detail
{
#ifdef TEXAS_ENABLE_KTX_READ
    struct MemReqs_KTX_BackendData
    {
        const unsigned char* srcImageDataStart = nullptr;
    };
#endif

#ifdef TEXAS_ENABLE_PNG_READ
	struct MemReqs_PNG_BackendData
    {
        const unsigned char* idatChunkStart = nullptr;
        const unsigned char* plteChunkStart = nullptr;
        unsigned long plteChunkDataLength = 0;
    };
#endif

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