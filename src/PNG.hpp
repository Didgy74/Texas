#pragma once

#include "Texas/ResultType.hpp"
#include "Texas/Result.hpp"
#include "Texas/MetaData.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/MemReqs.hpp"

#include <cstdint>

namespace Texas::detail::PNG
{
    constexpr std::uint8_t identifier[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

    Result loadFromBuffer_Step1(
        const bool identifierConfirmed,
        ConstByteSpan srcBuffer,
        MetaData& metaData,
        std::uint64_t& workingMemRequired,
        detail::MemReqs_PNG_BackendData& backendData);

    Result loadFromBuffer_Step2(
        const MetaData& metaData,
        detail::MemReqs_PNG_BackendData& backendData,
        const ByteSpan dstImageBuffer,
        const ByteSpan workingMemory);
}