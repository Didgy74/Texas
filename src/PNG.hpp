#pragma once

#include "Texas/InputStream.hpp"
#include "Texas/Result.hpp"
#include "Texas/TextureInfo.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/FileInfo.hpp"

#include <cstdint>

namespace Texas::detail::PNG
{
    constexpr std::uint8_t identifier[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

    Result loadFromBuffer_Step1(
        ConstByteSpan srcBuffer,
        TextureInfo& metaData,
        std::uint64_t& workingMemRequired,
        detail::FileInfo_PNG_BackendData& backendData);

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

    Result loadFromBuffer_Step2(
        const TextureInfo& textureInfo,
        detail::FileInfo_PNG_BackendData& backendData,
        const ByteSpan dstImageBuffer,
        const ByteSpan workingMemory);
}