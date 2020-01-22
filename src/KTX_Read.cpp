#include "KTX.hpp"

#include "Texas/ByteSpan.hpp"
#include "PrivateAccessor.hpp"

#include "Texas/detail/GLTools.hpp"

#include "Texas/Tools.hpp"

// For std::memcmp and std::memcpy
#include <cstring>

namespace Texas::detail::KTX
{
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

    [[nodiscard]] static inline std::uint32_t toU32(const std::byte* ptr)
    {
        std::uint32_t temp = 0;
        std::memcpy(&temp, ptr, sizeof(std::uint32_t));
        return temp;
    }

    [[nodiscard]] static inline constexpr TextureType toTextureType(
        const std::uint32_t* dimensions, 
        std::uint_least32_t arrayCount, 
        bool isCubemap) noexcept
    {
        if (arrayCount > 0)
        {
            if (isCubemap)
                return TextureType::ArrayCubemap;
            else
            {
                if (dimensions[2] > 0)
                    return TextureType::Array3D;
                else 
                {
                    if (dimensions[1] > 0)
                        return TextureType::Array2D;
                    else
                        return TextureType::Array1D;
                }
            }
        }
        else
        {
            if (isCubemap)
                return TextureType::Cubemap;
            else
            {
                if (dimensions[2] > 0)
                    return TextureType::Texture3D;
                else
                {
                    if (dimensions[1] > 0)
                        return TextureType::Texture2D;
                    else
                        return TextureType::Texture1D;
                }
            }
        }
    }

    [[nodiscard]] static inline constexpr bool isCubemap(TextureType texType)
    {
        return texType == TextureType::Cubemap || texType == TextureType::ArrayCubemap;
    }

    Result loadFromBuffer_Step1(
        ConstByteSpan srcBuffer,
        TextureInfo& textureInfo,
        detail::FileInfo_KTX_BackendData& backendData)
    {
        // Check if buffer is long enough to hold the KTX header
        if (srcBuffer.size() <= Header::totalSize)
            return { ResultType::PrematureEndOfFile, "KTX-file is not large enough to hold all header-data." };

        backendData = detail::FileInfo_KTX_BackendData();

        textureInfo.fileFormat = FileFormat::KTX;

        // Check if file endianness matches system's
        if (KTX::toU32(srcBuffer.data() + Header::endianness_Offset) != Header::correctEndian)
            return { ResultType::FileNotSupported, "KTX-file's endianness does not match system endianness. Texas not capable of converting." };

        // Grab pixel format
        const detail::GLEnum fileGLType = static_cast<detail::GLEnum>(KTX::toU32(srcBuffer.data() + Header::glType_Offset));
        const detail::GLEnum fileGLFormat = static_cast<detail::GLEnum>(KTX::toU32(srcBuffer.data() + Header::glFormat_Offset));
        const detail::GLEnum fileGLInternalFormat = static_cast<detail::GLEnum>(KTX::toU32(srcBuffer.data() + Header::glInternalFormat_Offset));
        const detail::GLEnum fileGLBaseInternalFormat = static_cast<detail::GLEnum>(KTX::toU32(srcBuffer.data() + Header::glBaseInternalFormat_Offset));
        textureInfo.colorSpace = detail::toColorSpace(fileGLInternalFormat, fileGLType);
        textureInfo.pixelFormat = detail::toPixelFormat(fileGLInternalFormat, fileGLType);
        textureInfo.channelType = detail::toChannelType(fileGLInternalFormat, fileGLType);
        if (textureInfo.pixelFormat == PixelFormat::Invalid || textureInfo.colorSpace == ColorSpace::Invalid || textureInfo.channelType == ChannelType::Invalid)
            return { ResultType::PixelFormatNotSupported, "KTX pixel-format not supported." };;


        // Grab dimensions
        const std::uint_least32_t origBaseDimensions[3] = {
            KTX::toU32(srcBuffer.data() + Header::pixelWidth_Offset),
            KTX::toU32(srcBuffer.data() + Header::pixelHeight_Offset),
            KTX::toU32(srcBuffer.data() + Header::pixelDepth_Offset)
        };
        if (origBaseDimensions[0] == 0)
            return { ResultType::CorruptFileData, "KTX specification does not allow field 'pixelWidth' to be 0." };
        if (origBaseDimensions[2] > 0 && origBaseDimensions[1] == 0)
            return { ResultType::CorruptFileData, "KTX specification does not allow field 'pixelHeight' to be 0 when field 'pixelDepth' is >0." };


        // Grab array layer count
        const std::uint_least32_t origArrayLayerCount = KTX::toU32(srcBuffer.data() + Header::numberOfArrayElements_Offset);


        // Grab number of faces.
        const std::uint_least32_t origNumberOfFaces = KTX::toU32(srcBuffer.data() + Header::numberOfFaces_Offset);
        if (origNumberOfFaces != 1 && origNumberOfFaces != 6)
            return { ResultType::CorruptFileData, "KTX specification requires field 'numberOfFaces' to be 1 or 6." };
        const bool texIsCubemap = origNumberOfFaces == 6;
        if (texIsCubemap)
        {
            if (texIsCubemap)
                return { ResultType::FileNotSupported, "KTX cubemaps not yet supported." };
            if (origBaseDimensions[1] == 0)
                return { ResultType::CorruptFileData, "KTX specification requires cubemaps to have field 'pixelHeight' be >0." };
            if (origBaseDimensions[2] != 0)
                return { ResultType::CorruptFileData, "KTX specification requires cubemaps to have field 'pixelDepth' be 0." };
        }

        textureInfo.textureType = toTextureType(origBaseDimensions, origArrayLayerCount, texIsCubemap);

        // Grab dimensions
        textureInfo.baseDimensions.width = origBaseDimensions[0];
        textureInfo.baseDimensions.height = origBaseDimensions[1];
        if (textureInfo.baseDimensions.height == 0)
            textureInfo.baseDimensions.height = 1;
        textureInfo.baseDimensions.depth = origBaseDimensions[2];
        if (textureInfo.baseDimensions.depth == 0)
            textureInfo.baseDimensions.depth = 1;
        // Grab amount of array layers
        textureInfo.arrayLayerCount = origArrayLayerCount;
        if (textureInfo.arrayLayerCount == 0)
            textureInfo.arrayLayerCount = 1;
        // Grab amount of mip levels
        // Usually, mipCount = 0 means a mipmap pyramid should be generated at loadtime. But we ignore it.
        textureInfo.mipLevelCount = KTX::toU32(srcBuffer.data() + Header::numberOfMipmapLevels_Offset);
        if (textureInfo.mipLevelCount == 0)
            textureInfo.mipLevelCount = 1;


        // For now we don't do anything with the key-value data.
        const std::uint_least32_t totalKeyValueDataSize = KTX::toU32(srcBuffer.data() + Header::bytesOfKeyValueData_Offset);
        // Check the buffer is long enough to hold all the key-value-data
        if (srcBuffer.size() <= Header::totalSize + totalKeyValueDataSize)
            return { ResultType::PrematureEndOfFile, "KTX-file is not large enough to hold any image-data." };
        std::uint_least32_t keyValueDataCounter = 0;
        while (keyValueDataCounter < totalKeyValueDataSize)
        {
            const std::uint_least32_t keyValuePairSize = KTX::toU32(srcBuffer.data() + Header::bytesOfKeyValueData_Offset + sizeof(std::uint32_t) + keyValueDataCounter);
            const std::byte* key = srcBuffer.data() + Header::bytesOfKeyValueData_Offset + sizeof(std::uint32_t) + keyValueDataCounter + sizeof(std::uint32_t);
            keyValueDataCounter += keyValuePairSize + (3 - ((keyValuePairSize + 3) % 4));
        }

        backendData.srcImageDataStart = reinterpret_cast<const unsigned char*>(srcBuffer.data() + Header::totalSize + totalKeyValueDataSize);

        return { ResultType::Success, nullptr };
    }

    Result loadFromBuffer_Step2(
        TextureInfo const& textureInfo,
        detail::FileInfo_KTX_BackendData& backendData,
        const ByteSpan dstImageBuffer,
        const ByteSpan workingMem)
    {
        std::size_t srcMemOffset = 0;
        std::size_t dstMemOffset = 0;

        if (isCubemap(textureInfo.textureType))
        {
            return Result(ResultType::FileNotSupported, "KTX cubemaps not yet supported.");
        }
        else
        {
            for (std::uint_least32_t mipIndex = 0; mipIndex < textureInfo.mipLevelCount; mipIndex++)
            {
                // Contains the amount of data from all array images of this mip level
                const std::uint_least32_t mipDataSize = KTX::toU32(reinterpret_cast<const std::byte*>(backendData.srcImageDataStart + srcMemOffset));
                
                std::memcpy(dstImageBuffer.data() + dstMemOffset, backendData.srcImageDataStart + srcMemOffset + sizeof(std::uint32_t), mipDataSize);
                const std::uint8_t padding = (3 - ((mipDataSize + 3) % 4));
                srcMemOffset += sizeof(mipDataSize) + mipDataSize + padding;
                dstMemOffset += mipDataSize;
            }
        }



        return Result(ResultType::Success, nullptr);
    }
}