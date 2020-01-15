#pragma once

#include "Texas/Result.hpp"
#include "Texas/LoadResult.hpp"
#include "Texas/MemReqs.hpp"
#include "Texas/ByteSpan.hpp"

#include <cstddef>

namespace Texas::detail
{
    class PrivateAccessor final
    {
    private:
        PrivateAccessor() = delete;
        virtual ~PrivateAccessor() = 0;

    public:
        static LoadResult<MemReqs> getMemReqs(ConstByteSpan inputBuffer);

        static Result loadImageData(const MemReqs& file, ByteSpan dstBuffer, ByteSpan workingMemory);

        static LoadResult<MetaData> loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMemory);

    };
}