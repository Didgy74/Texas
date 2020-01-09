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
        static LoadResult<MemReqs> loadFromBuffer(const ConstByteSpan inputBuffer);

        static Result loadImageData(const MemReqs& file, ByteSpan dstBuffer, ByteSpan workingMemory);

    };
}