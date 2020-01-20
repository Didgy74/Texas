#pragma once

#include "Texas/Result.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/ParsedFileInfo.hpp"
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
        [[nodiscard]] static ResultValue<ParsedFileInfo> parseBuffer(ConstByteSpan inputBuffer) noexcept;

        [[nodiscard]] static Result loadImageData(const ParsedFileInfo& file, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept;

        [[nodiscard]] static ResultValue<TextureInfo> loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept;

        // Allows nullptr for allocator if the macro TEXAS_ENABLE_DYNAMIC_ALLOCATIONS is defined.
        [[nodiscard]] static ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer, Allocator* allocator) noexcept;
    };
}