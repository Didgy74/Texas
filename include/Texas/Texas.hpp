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
    /*
        Loads an entire texture from a polymorphic stream, by using a custom memory allocator.
    */
    [[nodiscard]] ResultValue<Texture> loadFromStream(InputStream& stream, Allocator& allocator) noexcept;
    /*
        Loads an entire texture from file at the specified path, by using a custom memory allocator.
    */
    [[nodiscard]] ResultValue<Texture> loadFromPath(char const* path, Allocator& allocator) noexcept;

    /*
        Parses for texture-info from a polymorphic stream

        Note: This loading path is designed to be used in conjunction with Texas::loadImageData
    */
    [[nodiscard]] ResultValue<FileInfo> parseStream(InputStream& stream) noexcept;

    /*
        Loads imagedata into dstBuffer by using information gathered with Texas::parseStream
    */
    [[nodiscard]] Result loadImageData(
        InputStream& stream,
        FileInfo const& file, 
        ByteSpan dstBuffer,
        ByteSpan workingMemory) noexcept;
}

#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
namespace Texas
{
    /*
        Loads an entire texture from a polymorphic stream

        Note: This loading path uses dynamic allocations in the implementation.
    */
    [[nodiscard]] ResultValue<Texture> loadFromStream(InputStream& stream) noexcept;

    /*
        Loads an entire texture from file at the specified path

        Note: This loading path uses dynamic allocations in the implementation.
    */
    [[nodiscard]] ResultValue<Texture> loadFromPath(char const* path) noexcept;
}
#endif