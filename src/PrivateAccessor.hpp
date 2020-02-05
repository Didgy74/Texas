#pragma once

#include "Texas/Result.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/FileInfo.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/Texture.hpp"

#if defined(TEXAS_ENABLE_KTX_SAVE)
#include "Texas/KTX/TextureInfo.hpp"
#endif

#include <cstddef>

namespace Texas::detail
{
    class PrivateAccessor final
    {
    private:
        PrivateAccessor() = delete;
        virtual ~PrivateAccessor() = 0;

    public:
        [[nodiscard]] static ResultValue<FileInfo> parseBuffer(ConstByteSpan inputBuffer) noexcept;

        [[nodiscard]] static Result loadImageData(FileInfo const& file, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept;

        [[nodiscard]] static ResultValue<TextureInfo> loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept;

        // Allows nullptr for allocator if the macro TEXAS_ENABLE_DYNAMIC_ALLOCATIONS is defined.
        [[nodiscard]] static ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer, Allocator* allocator) noexcept;

#if defined(TEXAS_ENABLE_KTX_SAVE)
        [[nodiscard]] static ResultValue<std::uint64_t> KTX_calcFileSize(TextureInfo const& texInfo) noexcept;
#endif
    };
}