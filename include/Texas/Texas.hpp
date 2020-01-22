#pragma once

#include "Texas/ResultValue.hpp"
#include "Texas/Result.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/FileInfo.hpp"
#include "Texas/TextureInfo.hpp"
#include "Texas/Texture.hpp"
#include "Texas/LoadSettings.hpp"
#include "Texas/Allocator.hpp"

#include <cstddef>
#include <cstdint>

namespace Texas
{
    /*
        Parses a buffer containing file-data without loading imagedata. 

        If successful, the returned struct holds MetaData along with the memory requirements for loading imagedata from this buffer.
    */
    [[nodiscard]] ResultValue<FileInfo> parseBuffer(const std::byte* inputBuffer, std::size_t bufferSize);

    /*
        Parses a buffer containing file-data without loading imagedata.

        If successful, the returned struct holds MetaData along with the memory requirements for loading imagedata from this buffer.
    */
    [[nodiscard]] ResultValue<FileInfo> parseBuffer(ConstByteSpan inputBuffer);

    /*
        Loads the image-data into dstBuffer after parsing inputBuffer.

        This is useful for the scenario when the inputBuffer has moved/changed since calling Texas::getMemReqs(),
        or when you already know your buffers are large enough to parse and load the file.
    */
    [[nodiscard]] ResultValue<TextureInfo> loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMemory);

    /*
        Loads only the image-data into dstBuffer after parsing inputBuffer.

        This is useful for the scenario when the inputBuffer has moved/changed since calling Texas::getMemReqs(),
        or when you already know your buffers are large enough to parse and load the file.
    */
    [[nodiscard]] ResultValue<TextureInfo> loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMemory);

    /*
        Loads only the imagedata of a file opened with getMemReqs() into dstBuffer.

        Requirements:
            The file-buffer passed into Texas::getMemReqs() must NOT have changed since calling said function.
            
            dstBuffer MUST be ATLEAST the size returned by OpenBuffer::memoryRequired().

            workingMemory MUST be ATLEAST the size return by MemReqs::workingMemoryRequired().
            If the size returned by OpenBuffer::workingMemoryRequired is 0, you can pass a default-constructed ByteSpan, or one that points to nullptr.
    */
    [[nodiscard]] Result loadImageData(FileInfo const& file, ByteSpan dstBuffer, ByteSpan workingMemory);

    /*
        Loads only the imagedata of a file parsed with getMemReqs() into dstBuffer.

        Requirements:
            The file-buffer passed into Texas::getMemReqs() must NOT have changed since calling said function.

            dstBuffer MUST be ATLEAST the size returned by MemReqs::memoryRequired().

            workingMemory MUST be ATLEAST the size return by MemReqs::workingMemoryRequired().
    */
    [[nodiscard]] Result loadImageData(FileInfo const& file, std::byte* dstBuffer, std::size_t dstBufferSize, std::byte* workingMemory, std::size_t workingMemorySize);

    /*
        Loads a texture in its entirety from a buffer containing file-data, by allocating with a custom allocator.
    */
    [[nodiscard]] ResultValue<Texture> loadFromBuffer(const std::byte* inputBuffer, std::size_t inputBufferSize, Allocator& allocator);

    /*
        Loads a texture in its entirety from a buffer containing file-data, by allocating with a custom allocator.
    */
    [[nodiscard]] ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer, Allocator& allocator);
}

#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
namespace Texas
{
    /*
        Loads a texture in its entirety from a buffer containing file-data.

        Note: This loading path will make dynamic allocations under the hood.
    */
    [[nodiscard]] ResultValue<Texture> loadFromBuffer(const std::byte* inputBuffer, std::size_t bufferLength);

    /*
        Loads a texture in its entirety from a buffer containing file-data.

        Note: This loading path will make dynamic allocations under the hood.
    */
    [[nodiscard]] ResultValue<Texture> loadFromBuffer(ConstByteSpan inputBuffer);
}
#endif