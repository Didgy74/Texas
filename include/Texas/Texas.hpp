#pragma once

#include "Texas/InputStream.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/Result.hpp"
#include "Texas/ByteSpan.hpp"
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
    /*
        Parses a buffer containing file-data without loading imagedata.

        If successful, the returned struct holds MetaData along with the memory requirements for loading imagedata from this buffer.
    */
    [[nodiscard]] ResultValue<FileInfo> parseBuffer(ConstByteSpan inputBuffer) noexcept;

    /*
        Loads only the imagedata of a file opened with getMemReqs() into dstBuffer.

        Requirements:
            The file-buffer passed into Texas::getMemReqs() must NOT have changed since calling said function.
            
            dstBuffer MUST be ATLEAST the size returned by OpenBuffer::memoryRequired().

            workingMemory MUST be ATLEAST the size return by MemReqs::workingMemoryRequired().
            If the size returned by OpenBuffer::workingMemoryRequired is 0, you can pass a default-constructed ByteSpan, or one that points to nullptr.
    */
    [[nodiscard]] Result loadImageData(FileInfo const& file, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept;

    [[nodiscard]] ResultValue<Texture> loadFromStream(InputStream& stream, Allocator& allocator) noexcept;

    /*
        Loads a texture in its entirety from a buffer containing file-data, by allocating with a custom allocator.
    */
    [[nodiscard]] ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer, Allocator& allocator) noexcept;
}

#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
namespace Texas
{
    [[nodiscard]] ResultValue<Texture> loadFromStream(InputStream& stream) noexcept;
    [[nodiscard]] ResultValue<Texture> loadFromPath(char const* path) noexcept;

    /*
        Loads a texture in its entirety from a buffer containing file-data.

        Note: This loading path will make dynamic allocations under the hood.
    */
    [[nodiscard]] ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer) noexcept;
}
#endif