#pragma once

#include "Texas/Result.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/InputStream.hpp"
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
        virtual ~PrivateAccessor() = 0;

    public:
        [[nodiscard]] static ResultValue<FileInfo> parseBuffer(ConstByteSpan inputBuffer) noexcept;

        [[nodiscard]] static Result loadImageData(
            FileInfo const& file, 
            ByteSpan dstBuffer, 
            ByteSpan workingMemory) noexcept;
        [[nodiscard]] static Result loadImageData(
            InputStream& stream,
            FileInfo const& file,
            ByteSpan dstBuffer,
            ByteSpan workingMem) noexcept;

        [[nodiscard]] static ResultValue<FileInfo> parseStream(InputStream& stream) noexcept;
        [[nodiscard]] static ResultValue<Texture> loadFromStream(InputStream& stream, Allocator* allocator) noexcept;

        [[nodiscard]] static ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer, Allocator* allocator) noexcept;

#if defined(TEXAS_ENABLE_KTX_SAVE)
        [[nodiscard]] static ResultValue<std::uint64_t> KTX_calcFileSize(TextureInfo const& texInfo) noexcept;
#endif
    };
}