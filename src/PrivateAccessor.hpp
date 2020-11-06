#pragma once

#include "Texas/Result.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/InputStream.hpp"
#include "Texas/FileInfo.hpp"
#include "Texas/Span.hpp"
#include "Texas/Texture.hpp"

#include <cstdint>

namespace Texas::detail
{
    class PrivateAccessor
    {
    private:
        virtual ~PrivateAccessor() = 0;

    public:
        [[nodiscard]] static ResultValue<Texture> loadFromStream(InputStream& stream, Allocator* allocator) noexcept;
        [[nodiscard]] static ResultValue<FileInfo> parseStream(InputStream& stream) noexcept;

        [[nodiscard]] static Result loadImageData(
            InputStream& stream,
            FileInfo const& file,
            ByteSpan dstBuffer,
            ByteSpan workingMem) noexcept;

#if defined(TEXAS_ENABLE_KTX_SAVE)
        [[nodiscard]] static ResultValue<std::uint64_t> KTX_calcFileSize(TextureInfo const& texInfo) noexcept;
#endif
    };
}