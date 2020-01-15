#include "KTX.hpp"

#include "Texas/ByteSpan.hpp"
#include "PrivateAccessor.hpp"

#include "Texas/GLFormats.hpp"

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

    [[nodiscard]] static inline std::uint32_t toU32(const std::byte* const ptr)
    {
        std::uint32_t temp = 0;
        std::memcpy(&temp, ptr, sizeof(std::uint32_t));
        return temp;
    }

    [[nodiscard]] static inline constexpr TextureType ToTextureType(
        const std::uint32_t* dimensions, 
        std::uint32_t arrayCount, 
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

    [[nodiscard]] static inline constexpr bool isCubemap(const TextureType texType)
    {
        return texType == TextureType::Cubemap || texType == TextureType::ArrayCubemap;
    }

    Result loadFromBuffer_Step1(
        const bool fileIdentifierConfirmed,
        ConstByteSpan srcBuffer,
        MetaData& metaData,
        detail::MemReqs_KTX_BackendData& backendData)
    {
        // Check if buffer is long enough to hold the KTX header
        if (srcBuffer.size() <= Header::totalSize)
            return { ResultType::PrematureEndOfFile, "KTX-file is not large enough to hold all header-data." };

        backendData = detail::MemReqs_KTX_BackendData();

        metaData.srcFileFormat = FileFormat::KTX;


        // Check that the file-identifier is correct.
        if (!fileIdentifierConfirmed && std::memcmp(srcBuffer.data(), identifier, sizeof(identifier) != 0))
            return { ResultType::CorruptFileData, "KTX-file's identifier is not correct." };


        // Check if file endianness matches system's
        if (KTX::toU32(srcBuffer.data() + Header::endianness_Offset) != Header::correctEndian)
            return { ResultType::FileNotSupported, "KTX-file's endianness does not match system endianness. Texas not capable of converting." };


        // Grab pixel format
        const Tools::detail::GLEnum fileGLType = static_cast<Tools::detail::GLEnum>(KTX::toU32(srcBuffer.data() + Header::glType_Offset));
        const Tools::detail::GLEnum fileGLFormat = static_cast<Tools::detail::GLEnum>(KTX::toU32(srcBuffer.data() + Header::glFormat_Offset));
        const Tools::detail::GLEnum fileGLInternalFormat = static_cast<Tools::detail::GLEnum>(KTX::toU32(srcBuffer.data() + Header::glInternalFormat_Offset));
        const Tools::detail::GLEnum fileGLBaseInternalFormat = static_cast<Tools::detail::GLEnum>(KTX::toU32(srcBuffer.data() + Header::glBaseInternalFormat_Offset));
        metaData.colorSpace = Tools::detail::toColorSpace(fileGLInternalFormat, fileGLType);
        metaData.pixelFormat = Tools::detail::toPixelFormat(fileGLInternalFormat, fileGLType);
        metaData.channelType = Tools::detail::toChannelType(fileGLInternalFormat, fileGLType);
        if (metaData.pixelFormat == PixelFormat::Invalid || metaData.colorSpace == ColorSpace::Invalid || metaData.channelType == ChannelType::Invalid)
            return { ResultType::PixelFormatNotSupported, "KTX pixel-format not supported." };;


        // Grab dimensions
        const std::uint32_t origBaseDimensions[3] = {
            KTX::toU32(srcBuffer.data() + Header::pixelWidth_Offset),
            KTX::toU32(srcBuffer.data() + Header::pixelHeight_Offset),
            KTX::toU32(srcBuffer.data() + Header::pixelDepth_Offset)
        };
        if (origBaseDimensions[0] == 0)
            return { ResultType::CorruptFileData, "KTX specification does not allow field 'pixelWidth' to be 0." };
        if (origBaseDimensions[2] > 0 && origBaseDimensions[1] == 0)
            return { ResultType::CorruptFileData, "KTX specification does not allow field 'pixelHeight' to be 0 when field 'pixelDepth' is >0." };


        // Grab array layer count
        const std::uint32_t origArrayLayerCount = KTX::toU32(srcBuffer.data() + Header::numberOfArrayElements_Offset);


        // Grab number of faces.
        const std::uint32_t origNumberOfFaces = KTX::toU32(srcBuffer.data() + Header::numberOfFaces_Offset);
        if (origNumberOfFaces != 1 && origNumberOfFaces != 6)
            return { ResultType::CorruptFileData, "KTX specification requires field 'numberOfFaces' to be 1 or 6." };
        bool texIsCubemap = origNumberOfFaces == 6;
        if (texIsCubemap)
        {
            if (texIsCubemap)
                return { ResultType::FileNotSupported, "KTX cubemaps not yet supported." };
            if (origBaseDimensions[1] == 0)
                return { ResultType::CorruptFileData, "KTX specification requires cubemaps to have field 'pixelHeight' be >0." };
            if (origBaseDimensions[2] != 0)
                return { ResultType::CorruptFileData, "KTX specification requires cubemaps to have field 'pixelDepth' be 0." };
        }

        metaData.textureType = ToTextureType(origBaseDimensions, origArrayLayerCount, texIsCubemap);

        // Grab dimensions
        metaData.baseDimensions.width = origBaseDimensions[0];
        metaData.baseDimensions.height = origBaseDimensions[1];
        if (metaData.baseDimensions.height == 0)
            metaData.baseDimensions.height = 1;
        metaData.baseDimensions.depth = origBaseDimensions[2];
        if (metaData.baseDimensions.depth == 0)
            metaData.baseDimensions.depth = 1;
        // Grab amount of array layers
        metaData.arrayLayerCount = origArrayLayerCount;
        if (metaData.arrayLayerCount == 0)
            metaData.arrayLayerCount = 1;
        // Grab amount of mip levels
        // Usually, mipCount = 0 means a mipmap pyramid should be generated at loadtime. But we ignore it.
        metaData.mipLevelCount = KTX::toU32(srcBuffer.data() + Header::numberOfMipmapLevels_Offset);
        if (metaData.mipLevelCount == 0)
            metaData.mipLevelCount = 1;


        // For now we don't do anything with the key-value data.
        const std::uint32_t totalKeyValueDataSize = KTX::toU32(srcBuffer.data() + Header::bytesOfKeyValueData_Offset);
        // Check the buffer is long enough to hold all the key-value-data
        if (srcBuffer.size() <= Header::totalSize + totalKeyValueDataSize)
            return { ResultType::PrematureEndOfFile, "KTX-file is not large enough to hold any image-data." };
        std::uint32_t keyValueDataCounter = 0;
        while (keyValueDataCounter < totalKeyValueDataSize)
        {
            const std::uint32_t keyValuePairSize = KTX::toU32(srcBuffer.data() + Header::bytesOfKeyValueData_Offset + sizeof(std::uint32_t) + keyValueDataCounter);
            const std::byte* key = srcBuffer.data() + Header::bytesOfKeyValueData_Offset + sizeof(std::uint32_t) + keyValueDataCounter + sizeof(std::uint32_t);
            keyValueDataCounter += keyValuePairSize + (3 - ((keyValuePairSize + 3) % 4));
        }

        backendData.srcImageDataStart = reinterpret_cast<const unsigned char*>(srcBuffer.data() + Header::totalSize + totalKeyValueDataSize);

        return { ResultType::Success, nullptr };
    }

    Result loadFromBuffer_Step2(
        const MetaData& metaData,
        detail::MemReqs_KTX_BackendData& backendData,
        const ByteSpan dstImageBuffer,
        const ByteSpan workingMem)
    {
        std::size_t srcMemOffset = 0;
        std::size_t dstMemOffset = 0;

        if (isCubemap(metaData.textureType))
        {
            return Result(ResultType::FileNotSupported, "KTX cubemaps not yet supported.");
        }
        else
        {
            for (std::uint32_t mipIndex = 0; mipIndex < metaData.mipLevelCount; mipIndex++)
            {
                // Contains the amount of data from all array images of this mip level
                const std::uint32_t mipDataSize = KTX::toU32(reinterpret_cast<const std::byte*>(backendData.srcImageDataStart + srcMemOffset));
                
                std::memcpy(dstImageBuffer.data() + dstMemOffset, backendData.srcImageDataStart + srcMemOffset + sizeof(std::uint32_t), mipDataSize);
                const std::uint8_t padding = (3 - ((mipDataSize + 3) % 4));
                srcMemOffset += sizeof(mipDataSize) + mipDataSize + padding;
                dstMemOffset += mipDataSize;
            }
        }



        return Result(ResultType::Success, nullptr);
    }
}