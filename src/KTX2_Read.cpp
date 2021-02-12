#include "KTX2.hpp"

#include <Texas/VkTools.hpp>

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace Texas::detail::KTX2
{
	namespace Header
	{
		// Offset means N bytes from start of header.
		constexpr std::size_t vkFormatOffset = 12;
		using VkFormat_T = std::uint32_t;
		constexpr std::size_t typeSizeOffset = 16;
		using TypeSize_T = std::uint32_t;
		constexpr std::size_t pixelWidthOffset = 20;
		using PixelWidth_T = std::uint32_t;
		constexpr std::size_t pixelHeightOffset = 24;
		using PixelHeight_T = std::uint32_t;
		constexpr std::size_t pixelDepthOffset = 28;
		using PixelDepth_T = std::uint32_t;
		constexpr std::size_t layerCountOffset = 32;
		using LayerCount_T = std::uint32_t;
		constexpr std::size_t faceCountOffset = 36;
		using FaceCount_T = std::uint32_t;
		constexpr std::size_t levelCountOffset = 40;
		using LevelCount_T = std::uint32_t;
		constexpr std::size_t superCompressionSchemeOffset = 44;
		using SuperCompressionScheme_T = std::uint32_t;

		constexpr std::size_t totalSize = 64;
	}
}

using namespace Texas;
using namespace Texas::detail;

Result KTX2::parseStream(
	InputStream& stream,
	TextureInfo& textureInfo) noexcept
{
	textureInfo.fileFormat = FileFormat::KTX2;

	Result result{};

	std::byte headerBuffer[Header::totalSize];
	result = stream.read({ headerBuffer, sizeof(headerBuffer) });
	if (!result.isSuccessful())
		return result;

	Header::VkFormat_T vkFormat;
	std::memcpy(&vkFormat, headerBuffer + Header::vkFormatOffset, sizeof(vkFormat));
	textureInfo.pixelFormat = VkFormatToPixelFormat(vkFormat);
	if (textureInfo.pixelFormat == PixelFormat::Invalid)
		return {
			ResultType::FileNotSupported,
			"Texas does not recognize this VkFormat of this KTX2 file." };

	Header::TypeSize_T typeSize;
	std::memcpy(&typeSize, headerBuffer + Header::typeSizeOffset, sizeof(typeSize));
	
	Header::PixelWidth_T pixelWidth;
	std::memcpy(&pixelWidth, headerBuffer + Header::pixelWidthOffset, sizeof(pixelWidth));
	textureInfo.baseDimensions.width = pixelWidth;

	Header::PixelHeight_T pixelHeight;
	std::memcpy(&pixelHeight, headerBuffer + Header::pixelHeightOffset, sizeof(pixelHeight));
	textureInfo.baseDimensions.height = pixelHeight;

	Header::PixelDepth_T pixelDepth;
	std::memcpy(&pixelDepth, headerBuffer + Header::pixelDepthOffset, sizeof(pixelDepth));
	textureInfo.baseDimensions.depth = pixelDepth;

	Header::LayerCount_T layerCount;
	std::memcpy(&layerCount, headerBuffer + Header::layerCountOffset, sizeof(layerCount));
	textureInfo.layerCount = layerCount;
	if (textureInfo.layerCount == 0)
		textureInfo.layerCount = 1;

	Header::FaceCount_T faceCount;
	std::memcpy(&faceCount, headerBuffer + Header::faceCountOffset, sizeof(faceCount));
	if (faceCount != 1 && faceCount != 6)
		return {
			ResultType::FileNotSupported,
			"Texas does not support KTX2 files with incomplete cubemaps." };

	Header::LevelCount_T levelCount;
	std::memcpy(&levelCount, headerBuffer + Header::levelCountOffset, sizeof(levelCount));
	textureInfo.mipCount = levelCount;

	Header::SuperCompressionScheme_T superCompressionScheme;
	std::memcpy(&superCompressionScheme, headerBuffer + Header::superCompressionSchemeOffset, sizeof(superCompressionScheme));
	if (superCompressionScheme != 0)
		return {
			ResultType::FileNotSupported,
			"Texas does not support KTX2 file with this supercompression sceheme." };

	return { ResultType::FileNotSupported, "Texas does not yet support KTX2 properly." };
}