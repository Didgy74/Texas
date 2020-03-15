#pragma once

#include "Texas/InputStream.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/Span.hpp"
#include "Texas/FileInfo.hpp"
#include "Texas/Texture.hpp"
#include "Texas/Allocator.hpp"

#if defined(TEXAS_ENABLE_KTX_SAVE)
#   include "Texas/KTX_Save.hpp"
#endif

namespace Texas
{
    [[nodiscard]] ResultValue<Texture> loadFromStream(InputStream& stream, Allocator& allocator) noexcept;
    [[nodiscard]] ResultValue<Texture> loadFromPath(char const* path, Allocator& allocator) noexcept;

    [[nodiscard]] ResultValue<FileInfo> parseStream(InputStream& stream) noexcept;
    [[nodiscard]] Result loadImageData(
        InputStream& stream,
        FileInfo const& file, 
        ByteSpan dstBuffer,
        ByteSpan workingMemory) noexcept;
}

#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
namespace Texas
{
    [[nodiscard]] ResultValue<Texture> loadFromStream(InputStream& stream) noexcept;
    [[nodiscard]] ResultValue<Texture> loadFromPath(char const* path) noexcept;
}
#endif