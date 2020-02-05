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
			return { ResultType::InvalidInputParameter, "KTX format does not support 3D arrays." };

		if (texInfo.baseDimensions.width == 0)
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'width' equal to 0 as KTX format." };
		if (texInfo.baseDimensions.height == 0)
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'height' equal to 0 as KTX format." };
		if (texInfo.baseDimensions.depth == 0)
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'depth' equal to 0 as KTX format." };
		if (texInfo.arrayLayerCount == 0)
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'arrayLayerCount' equal to 0 as KTX format." };
		if (texInfo.mipLevelCount == 0)
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'mipLevelCount' equal to 0 as KTX format." };

		if (texInfo.baseDimensions.width > detail::maxValue<std::uint32_t>())
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'width' higher than uint32 max value as KTX format." };
		if (texInfo.baseDimensions.height > detail::maxValue<std::uint32_t>())
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'height' higher than uint32 max value as KTX format." };
		if (texInfo.baseDimensions.depth > detail::maxValue<std::uint32_t>())
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'depth' higher than uint32 max value as KTX format." };
		if (texInfo.arrayLayerCount > detail::maxValue<std::uint32_t>())
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'arrayLayerCount' higher than uint32 max value as KTX format." };
		if (texInfo.mipLevelCount > 32)
			return { ResultType::InvalidInputParameter, "Cannot export texture with field 'mipLevelCount' higher than 32 as KTX format." };

		if (texInfo.mipLevelCount > calcMaxMipCount(texInfo.baseDimensions))
			return { ResultType::InvalidInputParameter, "Passed in texture-info with 'mipLevelCount' higher than 'baseDimensions' can hold." };

		return { ResultType::Success, nullptr };
	}
}

Texas::ResultValue<std::uint64_t> Texas::detail::PrivateAccessor::KTX_calcFileSize(TextureInfo const& texInfo) noexcept
{
	Result isValidResult = detail::KTX::isValid(texInfo);
	if (!isValidResult.isSuccessful())
		return isValidResult;

	std::uint64_t totalSize = 0;
	
	totalSize += KTX::Header::totalSize;

	// TODO: Add keyValueData-stuff

	for (std::uint_least32_t mipLevel = 0; mipLevel < texInfo.mipLevelCount; mipLevel += 1)
	{
		// Add the size of the `imageSize` field.
		totalSize += 4;

		// Add the total size of this mip-level. Includes 3D depth, array-layers, 
		Dimensions mipDims = calcMipDimensions(texInfo.baseDimensions, mipLevel);
		totalSize += calcTotalSize(texInfo.baseDimensions, texInfo.pixelFormat, 1, texInfo.arrayLayerCount);

		// Align to 4 bytes
		totalSize += totalSize % 4;
	}

	return totalSize;
}

Texas::Result Texas::saveToBuffer(ByteSpan dstBuffer, TextureInfo const& texInfo, Span<ConstByteSpan const> mipLevels)  noexcept
{
	return { ResultType::Success, nullptr };
}

#include <fstream>
Texas::Result Texas::KTX::saveToFile(char const* path, Texas::TextureInfo const& texInfo, Span<ConstByteSpan const> mipLevels) noexcept
{
	struct StdIfstream : WriteStream
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

	streamObject.stream.close();

	return { ResultType::Success, nullptr };
}

[[nodiscard]] Texas::Result Texas::KTX::saveToFile(char const* path, Texture const& texture) noexcept
{
	// KTX doesn't support more than 32 miplevels.
	ConstByteSpan mipLevelSpans[32] = {};
	for (std::uint32_t mipLevelIndex = 0; mipLevelIndex < static_cast<std::uint32_t>(texture.mipLevelCount()); mipLevelIndex += 1)
		mipLevelSpans[mipLevelIndex] = { texture.mipSpan(mipLevelIndex) };
	Span<ConstByteSpan const> mipLevels = { mipLevelSpans, texture.mipLevelCount() };

	return saveToFile(path, texture.textureInfo(), mipLevels);
}

Texas::Result Texas::KTX::saveToStream(Texas::TextureInfo const& texInfo, Span<ConstByteSpan const> mipLevels, WriteStream& stream) noexcept
{
	Result isValidResult = detail::KTX::isValid(texInfo);
	if (!isValidResult.isSuccessful())
		return isValidResult;

	if (mipLevels.data() == nullptr)
		return { ResultType::InvalidInputParameter, "Passed in nullptr for mip-level data." };
	if (mipLevels.size() == 0)
		return { ResultType::InvalidInputParameter, "Passed in no mip-levels of imagedata." };
	if (mipLevels.size() != texInfo.mipLevelCount)
		return { ResultType::InvalidInputParameter, "mipLevels.size() does not match texInfo.mipLevelCount." };
	for (decltype(mipLevels.size()) mipLevel = 0; mipLevel < mipLevels.size(); mipLevel += 1)
	{
		if (mipLevels.data()[mipLevel].data() == nullptr)
			return { ResultType::InvalidInputParameter, "Passed in nullptr for one of the mip-levels." };
		if (mipLevels.data()[mipLevel].size() < calcTotalSize(calcMipDimensions(texInfo.baseDimensions, mipLevel), texInfo.pixelFormat, 1, texInfo.arrayLayerCount))
			return { ResultType::InvalidInputParameter, "One of the mip-levels size is too small to hold the image-data." };
	}

	std::uint64_t memOffsetTracker = 0;

	// Set the file identifer field
	stream.write(reinterpret_cast<char const*>(detail::KTX::identifier), sizeof(detail::KTX::identifier));
	memOffsetTracker += sizeof(detail::KTX::identifier);

	// Set the 'endianness' field
	stream.write(reinterpret_cast<char const*>(&detail::KTX::Header::correctEndian), sizeof(detail::KTX::Header::correctEndian));
	memOffsetTracker += sizeof(detail::KTX::Header::correctEndian);

	// Set the 'glType' field
	std::uint32_t const glType = static_cast<std::uint32_t>(detail::toGLType(texInfo.channelType));
	stream.write(reinterpret_cast<char const*>(&glType), sizeof(glType));
	memOffsetTracker += sizeof(glType);

	// Set the 'glTypeSize' field
	std::uint32_t const glTypeSize = static_cast<std::uint32_t>(detail::toGLType(texInfo.channelType));
	stream.write(reinterpret_cast<char const*>(&glTypeSize), sizeof(glTypeSize));
	memOffsetTracker += sizeof(glTypeSize);

	// Set the 'glFormat' field
	std::uint32_t const glFormat = static_cast<std::uint32_t>(detail::toGLFormat(texInfo.pixelFormat));
	stream.write(reinterpret_cast<char const*>(&glFormat), sizeof(glFormat));
	memOffsetTracker += sizeof(glFormat);

	// Set the 'glInternalFormat' field
	std::uint32_t const glInternalFormat = static_cast<std::uint32_t>(detail::toGLInternalFormat(texInfo.pixelFormat, texInfo.colorSpace, texInfo.channelType));
	stream.write(reinterpret_cast<char const*>(&glInternalFormat), sizeof(glInternalFormat));
	memOffsetTracker += sizeof(glInternalFormat);

	// Set the 'glBaseInternalFormat' field
	std::uint32_t const glBaseInternalFormat = 0;
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
	std::uint32_t const numberOfArrayElements = detail::KTX::isArrayType(texInfo.textureType) ? static_cast<std::uint32_t>(texInfo.arrayLayerCount) : 0;
	stream.write(reinterpret_cast<char const*>(&numberOfArrayElements), sizeof(numberOfArrayElements));
	memOffsetTracker += sizeof(numberOfArrayElements);

	// Set the 'numberOfFaces' field
	std::uint32_t numberOfFaces = (texInfo.textureType == Texas::TextureType::Cubemap || texInfo.textureType == Texas::TextureType::ArrayCubemap) ? 6 : 1;
	stream.write(reinterpret_cast<char const*>(&numberOfFaces), sizeof(numberOfFaces));
	memOffsetTracker += sizeof(numberOfFaces);

	// Set the 'numberOfMipmapLevels' field
	std::uint32_t const numberOfMipmapLevels = static_cast<std::uint32_t>(texInfo.mipLevelCount);
	stream.write(reinterpret_cast<char const*>(&numberOfMipmapLevels), sizeof(numberOfMipmapLevels));
	memOffsetTracker += sizeof(numberOfMipmapLevels);

	// Set the 'bytesOfKeyValueData' field
	std::uint32_t const bytesOfKeyValueData = 0;
	stream.write(reinterpret_cast<char const*>(&bytesOfKeyValueData), sizeof(bytesOfKeyValueData));
	memOffsetTracker += sizeof(bytesOfKeyValueData);

	for (std::uint32_t mipLevelIndex = 0; mipLevelIndex < static_cast<std::uint32_t>(mipLevels.size()); mipLevelIndex += 1)
	{
		Dimensions mipDims = calcMipDimensions(texInfo.baseDimensions, mipLevelIndex);
		std::uint32_t imageSize = static_cast<std::uint32_t>(calcTotalSize(mipDims, texInfo.pixelFormat, 1, texInfo.arrayLayerCount));

		// Write the 'imageSize' 
		stream.write(reinterpret_cast<char const*>(&imageSize), sizeof(imageSize));
		memOffsetTracker += sizeof(imageSize);

		// Write the actual image-data
		stream.write(reinterpret_cast<char const*>(mipLevels.data()[mipLevelIndex].data()), imageSize);
		memOffsetTracker += imageSize;

		constexpr char paddingBuffer[3] = {};
		std::uint_least8_t paddingAmount = memOffsetTracker % 4;
		// Add padding to align to 4 bytes
		stream.write(paddingBuffer, paddingAmount);
		memOffsetTracker += paddingAmount;
	}

	return { ResultType::Success, nullptr };
}

Texas::Result Texas::KTX::saveToStream(Texture const& texture, WriteStream& stream) noexcept
{
	// KTX doesn't support more than 32 miplevels.
	ConstByteSpan mipLevelSpans[32] = {};
	for (std::uint32_t mipLevelIndex = 0; mipLevelIndex < static_cast<std::uint32_t>(texture.mipLevelCount()); mipLevelIndex += 1)
		mipLevelSpans[mipLevelIndex] = { texture.mipSpan(mipLevelIndex) };
	Span<ConstByteSpan const> mipLevels = { mipLevelSpans, texture.mipLevelCount() };

	return saveToStream(texture.textureInfo(), mipLevels, stream);
}