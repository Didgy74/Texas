#pragma once

#include <Texas/InputStream.hpp>
#include <Texas/Result.hpp>
#include <Texas/TextureInfo.hpp>
#include <Texas/Span.hpp>
#include <Texas/FileInfo.hpp>

#include <cstdint>

namespace Texas::detail::PNG
{
    constexpr std::uint8_t identifier[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

    Result parseStream(
        InputStream& stream,
        TextureInfo& metaData,
        std::uint64_t& workingMemRequired,
        detail::FileInfo_PNG_BackendData& backendData) noexcept;

    Result loadFromStream(
        InputStream& stream,
        TextureInfo const& textureInfo,
        detail::FileInfo_PNG_BackendData const& backendData,
        ByteSpan dstImageBuffer,
        ByteSpan workingMem) noexcept;
}