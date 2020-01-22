#pragma once

#include "Texas/Result.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/FileInfo.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/Texture.hpp"

#include <cstddef>

namespace Texas::detail
{
    class PrivateAccessor final
    {
    private:
        PrivateAccessor() = delete;
        virtual ~PrivateAccessor() = 0;

    public:
        [[nodiscard]] static ResultValue<FileInfo> parseBuffer(ConstByteSpan inputBuffer);

        [[nodiscard]] static Result loadImageData(FileInfo const& file, ByteSpan dstBuffer, ByteSpan workingMemory);

        [[nodiscard]] static ResultValue<TextureInfo> loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMemory);

        // Allows nullptr for allocator if the macro TEXAS_ENABLE_DYNAMIC_ALLOCATIONS is defined.
        [[nodiscard]] static ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer, Allocator* allocator);
    };
}