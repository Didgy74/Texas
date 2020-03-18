#pragma once

#include "Texas/InputStream.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/Result.hpp"
#include "Texas/TextureInfo.hpp"
#include "Texas/Span.hpp"
#include "Texas/FileInfo.hpp"

#include <cstdint>
#include <cstddef>

namespace Texas::detail::KTX
{
    constexpr std::uint8_t identifier[12] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

    [[nodiscard]] Result loadFromStream(
        InputStream& stream,
        TextureInfo& textureInfo);

    [[nodiscard]] Result loadImageData(
        InputStream& stream,
        ByteSpan dstBuffer,
        TextureInfo const& textureInfo,
        FileInfo_KTX_BackendData const& backendData);

    namespace Header
    {
        constexpr std::uint32_t correctEndian = 0x04030201;
        constexpr std::size_t totalSize = 64;
        constexpr std::size_t identifier_Offset = 0;
        constexpr std::size_t endianness_Offset = 12;
        constexpr std::size_t glType_Offset = 16;
        constexpr std::size_t glTypeSize_Offset = 20;
        constexpr std::size_t glFormat_Offset = 24;
        constexpr std::size_t glInternalFormat_Offset = 28;
        constexpr std::size_t glBaseInternalFormat_Offset = 32;
        constexpr std::size_t pixelWidth_Offset = 36;
        constexpr std::size_t pixelHeight_Offset = 40;
        constexpr std::size_t pixelDepth_Offset = 44;
        constexpr std::size_t numberOfArrayElements_Offset = 48;
        constexpr std::size_t numberOfFaces_Offset = 52;
        constexpr std::size_t numberOfMipmapLevels_Offset = 56;
        constexpr std::size_t bytesOfKeyValueData_Offset = 60;
    }
}