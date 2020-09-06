#include "KTX.hpp"

#include "Texas/Span.hpp"
#include "PrivateAccessor.hpp"

#include "detail_GLTools.hpp"

#include "Texas/Tools.hpp"

// For std::memcmp and std::memcpy
#include <cstring>

namespace Texas::detail::KTX
{
    [[nodiscard]] static std::uint32_t toU32(std::byte const* ptr)
    {
        std::uint32_t temp = 0;
        std::memcpy(&temp, ptr, sizeof(std::uint32_t));
        return temp;
    }

    [[nodiscard]] static constexpr TextureType toTextureType(
        std::uint32_t const* dimensions, 
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

    [[nodiscard]] static constexpr bool isCubemap(TextureType texType)
    {
        return texType == TextureType::Cubemap || texType == TextureType::ArrayCubemap;
    }
}

Texas::Result Texas::detail::KTX::loadFromStream(
    InputStream& stream, 
    TextureInfo& textureInfo)
{
    Result result{};

    textureInfo.fileFormat = FileFormat::KTX;

    std::byte headerBuffer[Header::totalSize] = {};
    result = stream.read({ headerBuffer, Header::totalSize });
    if (!result.isSuccessful())
        return result;

    // Test that identifier is correct.
    std::byte identifier[12] = {};
    std::memcpy(identifier, headerBuffer, 12);
    if (std::memcmp(identifier, KTX::identifier, 12) != 0)
        return { ResultType::CorruptFileData, "Identifier of file does not match KTX identifier." };

    // Check if file endianness matches system's
    if (KTX::toU32(headerBuffer + Header::endianness_Offset) != Header::correctEndian)
        return { ResultType::FileNotSupported, 
                 "KTX-file's endianness does not match system endianness. "
                 "Texas not capable of converting." };

    // Grab pixel format
    // TODO: Implement validation around these OpenGL enums.
    //detail::GLEnum const fileGLType = static_cast<detail::GLEnum>(KTX::toU32(headerBuffer + Header::glType_Offset));
    //detail::GLEnum const fileGLFormat = static_cast<detail::GLEnum>(KTX::toU32(headerBuffer + Header::glFormat_Offset));
    detail::GLEnum const fileGLInternalFormat = static_cast<detail::GLEnum>(
        KTX::toU32(
            headerBuffer + Header::glInternalFormat_Offset));
    //detail::GLEnum const fileGLBaseInternalFormat = static_cast<detail::GLEnum>(
    //    KTX::toU32(
    //        headerBuffer + Header::glBaseInternalFormat_Offset));

    
    textureInfo.colorSpace = detail::GLToColorSpace(fileGLInternalFormat);
    textureInfo.pixelFormat = detail::GLToPixelFormat(fileGLInternalFormat);
    textureInfo.channelType = detail::GLToChannelType(fileGLInternalFormat);
    if (textureInfo.pixelFormat == PixelFormat::Invalid || 
        textureInfo.colorSpace == ColorSpace::Invalid || 
        textureInfo.channelType == ChannelType::Invalid)
        return { ResultType::FileNotSupported, "KTX pixel-format not supported." };


    // Grab dimensions
    std::uint32_t const origBaseDimensions[3] = {
        KTX::toU32(headerBuffer + Header::pixelWidth_Offset),
        KTX::toU32(headerBuffer + Header::pixelHeight_Offset),
        KTX::toU32(headerBuffer + Header::pixelDepth_Offset)
    };
    if (origBaseDimensions[0] == 0)
        return { ResultType::CorruptFileData, 
        "KTX specification does not allow field 'pixelWidth' to be 0." };
    if (origBaseDimensions[2] > 0 && origBaseDimensions[1] == 0)
        return { ResultType::CorruptFileData, 
        "KTX specification does not allow field 'pixelHeight' to be 0 \
                      when field 'pixelDepth' is >0." };


    // Grab array layer count
    std::uint32_t const origArrayLayerCount = KTX::toU32(headerBuffer + Header::numberOfArrayElements_Offset);


    // Grab number of faces.
    std::uint32_t const origNumberOfFaces = KTX::toU32(headerBuffer + Header::numberOfFaces_Offset);
    if (origNumberOfFaces != 1 && origNumberOfFaces != 6)
        return { ResultType::CorruptFileData, "KTX specification requires field 'numberOfFaces' to be 1 or 6." };
    bool const texIsCubemap = origNumberOfFaces == 6;
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
    textureInfo.layerCount = origArrayLayerCount;
    if (textureInfo.layerCount == 0)
        textureInfo.layerCount = 1;
    // Grab amount of mip levels
    // Usually, mipCount = 0 means a mipmap pyramid should be generated at loadtime. But we ignore it.
    textureInfo.mipCount = KTX::toU32(headerBuffer + Header::numberOfMipmapLevels_Offset);
    // KTX supports 32 mip levels maximally
    if (textureInfo.mipCount > 32)
        return { ResultType::Success, "KTX specification doesn't allow mip-level count higher than 32." };
    if (textureInfo.mipCount == 0)
        textureInfo.mipCount = 1;


    // For now we don't do anything with the key-value data.
    std::uint32_t const totalKeyValueDataSize = KTX::toU32(headerBuffer + Header::bytesOfKeyValueData_Offset);

    stream.ignore(totalKeyValueDataSize);

    return Texas::successResult;
}

Texas::Result Texas::detail::KTX::loadImageData(
    InputStream& stream,
    ByteSpan dstBuffer,
    TextureInfo const& textureInfo,
    FileInfo_KTX_BackendData const& backendData)
{
    Result result{};
    std::size_t dstMemOffset = 0;

    if (isCubemap(textureInfo.textureType))
    {
        return Result(ResultType::FileNotSupported, "KTX cubemaps not yet supported.");
    }
    else
    {
        for (std::uint32_t mipIndex = 0; mipIndex < textureInfo.mipCount; mipIndex += 1)
        {
            // Contains the amount of data from all array images of this mip level
            std::uint32_t mipDataSize = 0;
            result = stream.read({ reinterpret_cast<std::byte*>(&mipDataSize), sizeof(mipDataSize) });
            if (!result.isSuccessful())
                return result;
            if (mipDataSize == 0)
                return { ResultType::CorruptFileData , "KTX spec doesn't allow a mip-level to have size 0." };
            // TODO: Maybe add some validation to check if size is correct multiple...

            // Copy all the data of this mip-level.
            result = stream.read({ dstBuffer.data() + dstMemOffset, mipDataSize });
            if (!result.isSuccessful())
                return result;

            std::uint8_t const padding = (3 - ((mipDataSize + 3) % 4));
            stream.ignore(padding);
            dstMemOffset += mipDataSize;
        }
    }

    return Texas::successResult;
}