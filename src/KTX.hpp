#pragma once

#include "Texas/ResultValue.hpp"
#include "Texas/Result.hpp"
#include "Texas/TextureInfo.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/FileInfo.hpp"

namespace Texas::detail::KTX
{
    constexpr std::uint8_t identifier[12] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

    Result loadFromBuffer_Step1(
        ConstByteSpan srcBuffer,
        TextureInfo& metaData,
        detail::FileInfo_KTX_BackendData& backendDataBuffer);

    Result loadFromBuffer_Step2(
        TextureInfo const& textureInfo,
        detail::FileInfo_KTX_BackendData& backendDataBuffer,
        ByteSpan dstImageBuffer,
        ByteSpan workingMemory);
}