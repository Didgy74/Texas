#pragma once

#include "Texas/InputStream.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/Result.hpp"
#include "Texas/Span.hpp"
#include "Texas/FileInfo.hpp"
#include "Texas/TextureInfo.hpp"
#include "Texas/Texture.hpp"
#include "Texas/LoadSettings.hpp"
#include "Texas/Allocator.hpp"
#if defined(TEXAS_ENABLE_KTX_SAVE)
#   include "Texas/KTX_Save.hpp"
#endif

#include <cstddef>
#include <cstdint>

namespace Texas
{
    [[nodiscard]] ResultValue<Texture> loadFromStream(InputStream& stream, Allocator& allocator) noexcept;
    [[nodiscard]] ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer, Allocator& allocator) noexcept;
    [[nodiscard]] ResultValue<Texture> loadFromPath(char const* path, Allocator& allocator) noexcept;

    [[nodiscard]] ResultValue<FileInfo> parseStream(InputStream& stream) noexcept;
    [[nodiscard]] ResultValue<FileInfo> parseBuffer(ConstByteSpan inputBuffer) noexcept;
    [[nodiscard]] Result loadImageData(FileInfo const& file, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept;
}

#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
namespace Texas
{
    [[nodiscard]] ResultValue<Texture> loadFromStream(InputStream& stream) noexcept;
    [[nodiscard]] ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer) noexcept;
    [[nodiscard]] ResultValue<Texture> loadFromPath(char const* path) noexcept;
}
#endif