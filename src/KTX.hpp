#pragma once

#include "Texas/LoadResult.hpp"
#include "Texas/Result.hpp"
#include "Texas/MetaData.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/MemReqs.hpp"

namespace Texas::detail::KTX
{
    constexpr std::uint8_t identifier[12] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

    Result loadFromBuffer_Step1(
        const bool fileIdentifierConfirmed,
        ConstByteSpan srcBuffer,
        MetaData& metaData,
        detail::MemReqs_KTX_BackendData& backendDataBuffer);

    Result loadFromBuffer_Step2(
        const MetaData& metaData,
        detail::MemReqs_KTX_BackendData& backendDataBuffer,
        const ByteSpan dstImageBuffer,
        const ByteSpan workingMemory);
}