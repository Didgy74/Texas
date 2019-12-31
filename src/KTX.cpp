#include "KTX.hpp"

#include "Texas/GLFormats.hpp"

#include "PrivateAccessor.hpp"

namespace Texas::detail::KTX
{
	static inline constexpr TextureType ToTextureType(
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

	Pair<ResultType, const char*> loadTest(
		const bool fileIdentifierConfirmed,
		const Span<const std::uint8_t> srcBuffer,
		MetaData& metaData,
		const std::uint8_t*& imageDataStart)
	{
		// Check if buffer is long enough to hold the KTX header
		if (srcBuffer.size() <= Header::totalSize)
			return { ResultType::PrematureEndOfFile, "KTX-file is not large enough to hold all header-data." };


		metaData.srcFileFormat = FileFormat::KTX;


		// Check that the file-identifier is correct.
		if (!fileIdentifierConfirmed && std::memcmp(srcBuffer.data(), Header::correctIdentifier, sizeof(Header::correctIdentifier) != 0))
			return { ResultType::CorruptFileData, "KTX-file's identifier is not correct." };


		// Check if file endianness matches system's
		if (*reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::endianness_Offset) != Header::correctEndian)
			return { ResultType::FileNotSupported, "KTX-file's endianness does not match system endianness. Texas not capable of converting." };


		// Grab pixel format
		const Tools::detail::GLEnum fileGLType = static_cast<Tools::detail::GLEnum>(*reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::glType_Offset));
		const Tools::detail::GLEnum fileGLFormat = static_cast<Tools::detail::GLEnum>(*reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::glFormat_Offset));
		const Tools::detail::GLEnum fileGLInternalFormat = static_cast<Tools::detail::GLEnum>(*reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::glInternalFormat_Offset));
		const Tools::detail::GLEnum fileGLBaseInternalFormat = static_cast<Tools::detail::GLEnum>(*reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::glBaseInternalFormat_Offset));
		metaData.colorSpace = Tools::detail::ToColorSpace(fileGLInternalFormat, fileGLType);
		metaData.pixelFormat = Tools::detail::ToPixelFormat(fileGLInternalFormat, fileGLType);
		metaData.channelType = Tools::detail::ToChannelType(fileGLInternalFormat, fileGLType);
		if (metaData.pixelFormat == PixelFormat::Invalid || metaData.colorSpace == ColorSpace::Invalid || metaData.channelType == ChannelType::Invalid)
			return { ResultType::PixelFormatNotSupported, "KTX pixel-format not supported." };;


		// Grab dimensions
		const std::uint32_t origBaseDimensions[3] = {
			*reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::pixelWidth_Offset),
			* reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::pixelHeight_Offset),
			* reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::pixelDepth_Offset)
		};
		if (origBaseDimensions[0] == 0)
			return { ResultType::CorruptFileData, "KTX specification does not allow field 'pixelWidth' to be 0." };
		if (origBaseDimensions[2] > 0 && origBaseDimensions[1] == 0)
			return { ResultType::CorruptFileData, "KTX specification does not allow field 'pixelHeight' to be 0 when field 'pixelDepth' is >0." };


		// Grab array layer count
		const std::uint32_t origArrayLayerCount = *reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::numberOfArrayElements_Offset);


		// Grab number of faces.
		const std::uint32_t origNumberOfFaces = *reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::numberOfFaces_Offset);
		if (origNumberOfFaces != 1 && origNumberOfFaces != 6)
			return { ResultType::CorruptFileData, "KTX specification requires field 'numberOfFaces' to be 1 or 6." };
		bool isCubemap = origNumberOfFaces == 6;
		if (isCubemap)
		{
			if (origBaseDimensions[1] == 0)
				return { ResultType::CorruptFileData, "KTX specification requires cubemaps to have field 'pixelHeight' be >0." };
			if (origBaseDimensions[2] != 0)
				return { ResultType::CorruptFileData, "KTX specification requires cubemaps to have field 'pixelDepth' be 0." };
		}


		metaData.textureType = ToTextureType(origBaseDimensions, origArrayLayerCount, isCubemap);


		metaData.baseDimensions.width = origBaseDimensions[0];
		metaData.baseDimensions.height = origBaseDimensions[1];
		if (metaData.baseDimensions.height == 0)
			metaData.baseDimensions.height = 1;
		metaData.baseDimensions.depth = origBaseDimensions[2];
		if (metaData.baseDimensions.depth == 0)
			metaData.baseDimensions.depth = 1;


		// Grab amount of array layers
		metaData.arrayLayerCount = *reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::numberOfArrayElements_Offset);
		if (metaData.arrayLayerCount == 0)
			metaData.arrayLayerCount = 1;


		// Grab amount of mip levels
		// Usually, mipCount = 0 means a mipmap pyramid should be generated at loadtime. But we ignore it.
		metaData.mipLevelCount = *reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::numberOfMipmapLevels_Offset);
		if (metaData.mipLevelCount == 0)
			metaData.mipLevelCount = 1;


		// For now we don't do anything with the key-value data.
		const std::uint32_t totalKeyValueDataSize = *reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::bytesOfKeyValueData_Offset);
		// Check the buffer is long enough to hold all the key-value-data
		if (srcBuffer.size() <= Header::totalSize + totalKeyValueDataSize)
			return { ResultType::PrematureEndOfFile, "KTX-file is not large enough to hold any image-data." };
		std::uint32_t keyValueDataCounter = 0;
		while (keyValueDataCounter < totalKeyValueDataSize)
		{
			const std::uint32_t keyValuePairSize = *reinterpret_cast<const std::uint32_t*>(srcBuffer.data() + Header::bytesOfKeyValueData_Offset + sizeof(std::uint32_t) + keyValueDataCounter);
			const char* key = reinterpret_cast<const char*>(srcBuffer.data() + Header::bytesOfKeyValueData_Offset + sizeof(std::uint32_t) + keyValueDataCounter + 4);
			keyValueDataCounter += keyValuePairSize + (3 - ((keyValuePairSize + 3) % 4));
		}

		imageDataStart = srcBuffer.data() + Header::totalSize + totalKeyValueDataSize;

		return { ResultType::Success, nullptr };
	}

	Result loadImageDataFromBuffer(const MetaData& metaData, const std::uint8_t* srcImageDataStart, std::uint8_t* dstImageBuffer)
	{
		std::size_t memOffset = 0;

		for (std::uint32_t mipIndex = 0; mipIndex < metaData.mipLevelCount; mipIndex++)
		{
			const std::uint32_t mipDataSize = *reinterpret_cast<const std::uint32_t*>(srcImageDataStart + memOffset);
			std::memcpy(dstImageBuffer, srcImageDataStart + memOffset + sizeof(std::uint32_t), mipDataSize);
			memOffset += sizeof(mipDataSize) + mipDataSize + (3-((mipDataSize+3)%4));
		}

		return Result(ResultType::Success, nullptr);
	}
}