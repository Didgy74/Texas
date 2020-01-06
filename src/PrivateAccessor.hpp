#pragma once

#include "Texas/Result.hpp"
#include "Texas/LoadResult.hpp"
#include "Texas/OpenBuffer.hpp"
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
        static LoadResult<OpenBuffer> loadFromBuffer(const ConstByteSpan inputBuffer);

        static Result loadImageData(const OpenBuffer& file, ByteSpan dstBuffer, ByteSpan workingMemory);

    };
}