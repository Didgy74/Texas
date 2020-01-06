#pragma once

#include "Texas/LoadResult.hpp"
#include "Texas/Result.hpp"
#include "Texas/MetaData.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/OpenBuffer.hpp"

namespace Texas::detail::KTX
{
    namespace Header
    {
        constexpr std::size_t totalSize = sizeof(std::uint8_t) * 64;

        constexpr std::size_t identifier_Offset = 0;
        constexpr std::uint8_t correctIdentifier[12] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

        constexpr std::uint32_t correctEndian = 0x04030201;
        constexpr std::size_t endianness_Offset = identifier_Offset + sizeof(std::uint8_t[12]);

        constexpr std::size_t glType_Offset = endianness_Offset + sizeof(std::uint32_t);

        constexpr std::size_t glTypeSize_Offset = glType_Offset + sizeof(std::uint32_t);

        constexpr std::size_t glFormat_Offset = glTypeSize_Offset + sizeof(std::uint32_t);

        constexpr std::size_t glInternalFormat_Offset = glFormat_Offset + sizeof(std::uint32_t);

        constexpr std::size_t glBaseInternalFormat_Offset = glInternalFormat_Offset + sizeof(std::uint32_t);

        constexpr std::size_t pixelWidth_Offset = glBaseInternalFormat_Offset + sizeof(std::uint32_t);

        constexpr std::size_t pixelHeight_Offset = pixelWidth_Offset + sizeof(std::uint32_t);

        constexpr std::size_t pixelDepth_Offset = pixelHeight_Offset + sizeof(std::uint32_t);

        constexpr std::size_t numberOfArrayElements_Offset = pixelDepth_Offset + sizeof(std::uint32_t);

        constexpr std::size_t numberOfFaces_Offset = numberOfArrayElements_Offset + sizeof(std::uint32_t);

        constexpr std::size_t numberOfMipmapLevels_Offset = numberOfFaces_Offset + sizeof(std::uint32_t);

        constexpr std::size_t bytesOfKeyValueData_Offset = numberOfMipmapLevels_Offset + sizeof(std::uint32_t);
    };

    Result loadFromBuffer_Step1(
        const bool fileIdentifierConfirmed,
        ConstByteSpan srcBuffer,
        MetaData& metaData,
        OpenBuffer::KTX_BackendData& backendDataBuffer);

    Result loadFromBuffer_Step2(
        const MetaData& metaData,
        OpenBuffer::KTX_BackendData& backendDataBuffer,
        const ByteSpan dstImageBuffer,
        const ByteSpan workingMemory);
}