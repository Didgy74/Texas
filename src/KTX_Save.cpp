#include "Texas/KTX_Save.hpp"
#include "KTX.hpp"
#include "PrivateAccessor.hpp"
#include "NumericLimits.hpp"
#include "Texas/detail/GLTools.hpp"
#include "Texas/Tools.hpp"

#include <cstring>

Texas::ResultValue<std::uint64_t> Texas::KTX::calcFileSize(Texas::TextureInfo const& texInfo) noexcept
{
	return detail::PrivateAccessor::KTX_calcFileSize(texInfo);
}

namespace Texas::detail::KTX
{
	[[nodiscard]] static inline bool isArrayType(TextureType type) noexcept
	{
		switch (type)
		{
		case TextureType::Array1D:
		case TextureType::Array2D:
		case TextureType::Array3D:
		case TextureType::ArrayCubemap:
			return true;
		}

		return false;
	}

	[[nodiscard]] static inline bool is1DType(TextureType type) noexcept
	{
		switch (type)
		{
		case TextureType::Texture1D:
		case TextureType::Array1D:
			return true;
		}

		return false;
	}

	[[nodiscard]] static inline bool is2DType(TextureType type) noexcept
	{
		switch (type)
		{
		case TextureType::Texture2D:
		case TextureType::Array2D:
		case TextureType::Cubemap:
		case TextureType::ArrayCubemap:
			return true;
		}

		return false;
	}

	[[nodiscard]] static inline bool is3DType(TextureType type) noexcept
	{
		switch (type)
		{
		case TextureType::Texture3D:
		case TextureType::Array3D:
			return true;
		}
		return false;
	}

	[[nodiscard]] static inline Result isValid(TextureInfo const& texInfo) noexcept
	{
		if (texInfo.textureType == TextureType::Array3D)
			return { ResultType::InvalidLibraryUsage, "KTX format does not support 3D arrays." };

		if (texInfo.baseDimensions.width == 0)
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'width' equal to 0 as KTX format." };
		if (texInfo.baseDimensions.height == 0)
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'height' equal to 0 as KTX format." };
		if (texInfo.baseDimensions.depth == 0)
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'depth' equal to 0 as KTX format." };
		if (texInfo.layerCount == 0)
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'arrayLayerCount' equal to 0 as KTX format." };
		if (texInfo.mipCount == 0)
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'mipCount' equal to 0 as KTX format." };

		if (texInfo.baseDimensions.width > detail::maxValue<std::uint32_t>())
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'width' higher than uint32 max value as KTX format." };
		if (texInfo.baseDimensions.height > detail::maxValue<std::uint32_t>())
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'height' higher than uint32 max value as KTX format." };
		if (texInfo.baseDimensions.depth > detail::maxValue<std::uint32_t>())
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'depth' higher than uint32 max value as KTX format." };
		if (texInfo.layerCount > detail::maxValue<std::uint32_t>())
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'arrayLayerCount' higher than uint32 max value as KTX format." };
		if (texInfo.mipCount > 32)
			return { ResultType::InvalidLibraryUsage, 
					 "Cannot export texture with field 'mipCount' higher than 32 as KTX format." };

		if (texInfo.mipCount > calculateMaxMipCount(texInfo.baseDimensions))
			return { ResultType::InvalidLibraryUsage, 
					 "Passed in texture-info with 'mipCount' higher than 'baseDimensions' can hold." };

		return { ResultType::Success, nullptr };
	}
}

Texas::Result Texas::KTX::canSave(TextureInfo const& texInfo) noexcept
{
	if (texInfo.textureType == TextureType::Array3D)
		return { ResultType::InvalidLibraryUsage, "KTX format does not support 3D arrays." };

	if (texInfo.baseDimensions.width == 0)
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'width' equal to 0 as KTX format." };
	if (texInfo.baseDimensions.height == 0)
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'height' equal to 0 as KTX format." };
	if (texInfo.baseDimensions.depth == 0)
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'depth' equal to 0 as KTX format." };
	if (texInfo.layerCount == 0)
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'arrayLayerCount' equal to 0 as KTX format." };
	if (texInfo.mipCount == 0)
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'mipCount' equal to 0 as KTX format." };

	if (texInfo.baseDimensions.width > detail::maxValue<std::uint32_t>())
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'width' higher than uint32 max value as KTX format." };
	if (texInfo.baseDimensions.height > detail::maxValue<std::uint32_t>())
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'height' higher than uint32 max value as KTX format." };
	if (texInfo.baseDimensions.depth > detail::maxValue<std::uint32_t>())
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'depth' higher than uint32 max value as KTX format." };
	if (texInfo.layerCount > detail::maxValue<std::uint32_t>())
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'arrayLayerCount' higher than uint32 max value as KTX format." };
	if (texInfo.mipCount > 32)
		return { ResultType::InvalidLibraryUsage, 
				 "Cannot export texture with field 'mipCount' higher than 32 as KTX format." };

	if (texInfo.mipCount > calculateMaxMipCount(texInfo.baseDimensions))
		return { ResultType::InvalidLibraryUsage, 
				 "Passed in texture-info with 'mipCount' higher than 'baseDimensions' can hold." };

	return { ResultType::Success, nullptr };
}

Texas::ResultValue<std::uint64_t> Texas::detail::PrivateAccessor::KTX_calcFileSize(TextureInfo const& texInfo) noexcept
{
	Result isValidResult = detail::KTX::isValid(texInfo);
	if (!isValidResult.isSuccessful())
		return isValidResult;

	std::uint64_t totalSize = 0;
	
	totalSize += KTX::Header::totalSize;

	// TODO: Add keyValueData-stuff

	for (std::uint_least32_t mipLevel = 0; mipLevel < texInfo.mipCount; mipLevel += 1)
	{
		// Add the size of the `imageSize` field.
		totalSize += 4;

		// Add the total size of this mip-level. Includes 3D depth, array-layers, 
		Dimensions mipDims = calculateMipDimensions(texInfo.baseDimensions, mipLevel);
		totalSize += calculateTotalSize(texInfo.baseDimensions, texInfo.pixelFormat, 1, texInfo.layerCount);

		// Align to 4 bytes
		totalSize += totalSize % 4;
	}

	return totalSize;
}

#include <fstream>
Texas::Result Texas::KTX::saveToFile(
	char const* path, 
	Texas::TextureInfo const& texInfo, 
	Span<ConstByteSpan const> mipLevels) noexcept
{
	struct StdIfstream : OutputStream
	{
		std::ofstream stream{};
		char const* path = nullptr;

		virtual void write(char const* data, std::uint64_t size) noexcept override
		{
			stream.write(data, size);
		}
	};

	StdIfstream streamObject{};
	streamObject.path = path;
	streamObject.stream.open(path, std::ofstream::binary);
	if (!streamObject.stream.is_open())
		return { ResultType::CouldNotOpenFile, "Could not open file." };

	Result saveResult = saveToStream(texInfo, mipLevels, streamObject);
	if (!saveResult.isSuccessful())
		return saveResult;

	return { ResultType::Success, nullptr };
}

[[nodiscard]] Texas::Result Texas::KTX::saveToFile(char const* path, Texture const& texture) noexcept
{
	// KTX doesn't support more than 32 miplevels.
	ConstByteSpan mipLevelSpans[32] = {};
	for (std::uint32_t mipLevelIndex = 0; mipLevelIndex < texture.mipCount(); mipLevelIndex += 1)
		mipLevelSpans[mipLevelIndex] = { texture.mipSpan(mipLevelIndex) };
	Span<ConstByteSpan const> mipLevels = { mipLevelSpans, texture.mipCount() };

	return saveToFile(path, texture.textureInfo(), mipLevels);
}

Texas::Result Texas::KTX::saveToStream(
	Texas::TextureInfo const& texInfo, 
	Span<ConstByteSpan const> mipLevels, 
	OutputStream& stream) noexcept
{
	Result isValidResult = detail::KTX::isValid(texInfo);
	if (!isValidResult.isSuccessful())
		return isValidResult;

	if (mipLevels.data() == nullptr)
		return { ResultType::InvalidLibraryUsage, "Passed in nullptr for mip-level data." };
	if (mipLevels.size() == 0)
		return { ResultType::InvalidLibraryUsage, "Passed in no mip-levels of imagedata." };
	if (mipLevels.size() != texInfo.mipCount)
		return { ResultType::InvalidLibraryUsage, "mipLevels.size() does not match texInfo.mipCount." };
	for (std::uint8_t mipLevel = 0; mipLevel < mipLevels.size(); mipLevel += 1)
	{
		if (mipLevels.data()[mipLevel].data() == nullptr)
			return { ResultType::InvalidLibraryUsage, "Passed in nullptr for one of the mip-levels." };
		std::uint64_t const totalSize = calculateTotalSize(
			calculateMipDimensions(texInfo.baseDimensions, mipLevel), 
			texInfo.pixelFormat, 
			1, 
			texInfo.layerCount);
		if (mipLevels.data()[mipLevel].size() < totalSize)
			return { ResultType::InvalidLibraryUsage, "One of the mip-levels size is too small to hold the image-data." };
	}

	std::uint64_t memOffsetTracker = 0;

	// Set the file identifer field
	stream.write(reinterpret_cast<char const*>(detail::KTX::identifier), sizeof(detail::KTX::identifier));
	memOffsetTracker += sizeof(detail::KTX::identifier);

	// Set the 'endianness' field
	stream.write(reinterpret_cast<char const*>(&detail::KTX::Header::correctEndian), sizeof(detail::KTX::Header::correctEndian));
	memOffsetTracker += sizeof(detail::KTX::Header::correctEndian);

	// Set the 'glType' field
	detail::GLEnum const glType = detail::toGLType(texInfo.pixelFormat, texInfo.channelType);
	stream.write(reinterpret_cast<char const*>(&glType), sizeof(glType));
	memOffsetTracker += sizeof(glType);

	// Set the 'glTypeSize' field
	std::uint32_t const glTypeSize = detail::toGLTypeSize(texInfo.pixelFormat);
	stream.write(reinterpret_cast<char const*>(&glTypeSize), sizeof(glTypeSize));
	memOffsetTracker += sizeof(glTypeSize);

	// Set the 'glFormat' field
	detail::GLEnum const glFormat = detail::toGLFormat(texInfo.pixelFormat);
	stream.write(reinterpret_cast<char const*>(&glFormat), sizeof(glFormat));
	memOffsetTracker += sizeof(glFormat);

	// Set the 'glInternalFormat' field
	detail::GLEnum const glInternalFormat = detail::toGLInternalFormat(texInfo.pixelFormat, texInfo.colorSpace, texInfo.channelType);
	stream.write(reinterpret_cast<char const*>(&glInternalFormat), sizeof(glInternalFormat));
	memOffsetTracker += sizeof(glInternalFormat);

	// Set the 'glBaseInternalFormat' field
	detail::GLEnum const glBaseInternalFormat = (detail::GLEnum)0;
	stream.write(reinterpret_cast<char const*>(&glBaseInternalFormat), sizeof(glBaseInternalFormat));
	memOffsetTracker += sizeof(glBaseInternalFormat);

	// Set the 'pixelWidth' field
	std::uint32_t const pixelWidth = static_cast<std::uint32_t>(texInfo.baseDimensions.width);
	stream.write(reinterpret_cast<char const*>(&pixelWidth), sizeof(pixelWidth));
	memOffsetTracker += sizeof(pixelWidth);

	// Set the 'pixelHeight' field
	std::uint32_t pixelHeight = static_cast<std::uint32_t>(texInfo.baseDimensions.height);
	if (detail::KTX::is1DType(texInfo.textureType))
		pixelHeight = 0;
	stream.write(reinterpret_cast<char const*>(&pixelHeight), sizeof(pixelHeight));
	memOffsetTracker += sizeof(pixelHeight);

	// Set the 'pixelDepth' field
	std::uint32_t pixelDepth = static_cast<std::uint32_t>(texInfo.baseDimensions.depth);
	if (detail::KTX::is1DType(texInfo.textureType) || detail::KTX::is2DType(texInfo.textureType))
		pixelDepth = 0;
	stream.write(reinterpret_cast<char const*>(&pixelDepth), sizeof(pixelDepth));
	memOffsetTracker += sizeof(pixelDepth);

	// Set the 'numberOfArrayElements' field
	std::uint32_t const numberOfArrayElements = detail::KTX::isArrayType(texInfo.textureType) ? static_cast<std::uint32_t>(texInfo.layerCount) : 0;
	stream.write(reinterpret_cast<char const*>(&numberOfArrayElements), sizeof(numberOfArrayElements));
	memOffsetTracker += sizeof(numberOfArrayElements);

	// Set the 'numberOfFaces' field
	std::uint32_t numberOfFaces = (texInfo.textureType == Texas::TextureType::Cubemap || texInfo.textureType == Texas::TextureType::ArrayCubemap) ? 6 : 1;
	stream.write(reinterpret_cast<char const*>(&numberOfFaces), sizeof(numberOfFaces));
	memOffsetTracker += sizeof(numberOfFaces);

	// Set the 'numberOfMipmapLevels' field
	std::uint32_t const numberOfMipmapLevels = static_cast<std::uint32_t>(texInfo.mipCount);
	stream.write(reinterpret_cast<char const*>(&numberOfMipmapLevels), sizeof(numberOfMipmapLevels));
	memOffsetTracker += sizeof(numberOfMipmapLevels);

	// Set the 'bytesOfKeyValueData' field
	std::uint32_t const bytesOfKeyValueData = 0;
	stream.write(reinterpret_cast<char const*>(&bytesOfKeyValueData), sizeof(bytesOfKeyValueData));
	memOffsetTracker += sizeof(bytesOfKeyValueData);

	for (std::uint32_t mipLevelIndex = 0; mipLevelIndex < static_cast<std::uint32_t>(mipLevels.size()); mipLevelIndex += 1)
	{
		Dimensions mipDims = calculateMipDimensions(texInfo.baseDimensions, mipLevelIndex);
		std::uint32_t const imageSize = static_cast<std::uint32_t>(calculateTotalSize(mipDims, texInfo.pixelFormat, 1, texInfo.layerCount));

		// Write the 'imageSize' 
		stream.write(reinterpret_cast<char const*>(&imageSize), sizeof(imageSize));
		memOffsetTracker += sizeof(imageSize);

		// Write the actual image-data
		stream.write(reinterpret_cast<char const*>(mipLevels.data()[mipLevelIndex].data()), imageSize);
		memOffsetTracker += imageSize;

		constexpr char const paddingBuffer[3] = {};
		std::uint_least8_t paddingAmount = memOffsetTracker % 4;
		// Add padding to align to 4 bytes
		stream.write(paddingBuffer, paddingAmount);
		memOffsetTracker += paddingAmount;
	}

	return { ResultType::Success, nullptr };
}

Texas::Result Texas::KTX::saveToStream(Texture const& texture, OutputStream& stream) noexcept
{
	// KTX doesn't support more than 32 miplevels.
	ConstByteSpan mipLevelSpans[32] = {};
	for (std::uint32_t mipLevelIndex = 0; mipLevelIndex < static_cast<std::uint32_t>(texture.mipCount()); mipLevelIndex += 1)
		mipLevelSpans[mipLevelIndex] = { texture.mipSpan(mipLevelIndex) };
	Span<ConstByteSpan const> mipLevels = { mipLevelSpans, texture.mipCount() };

	return saveToStream(texture.textureInfo(), mipLevels, stream);
}