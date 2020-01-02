#include "Texas/Tools.hpp"

#include "Texas/Dimensions.hpp"

#include <cmath>

std::uint32_t Texas::Tools::CalcMaxMipLevelCount(Dimensions in)
{
	std::uint32_t max = in.width;
	if (in.height > max)
		max = in.height;
	if (in.depth > max)
		max = in.depth;
	return static_cast<std::uint32_t>(std::log2(max));
}

Texas::Dimensions Texas::Tools::CalcMipmapDimensions(const Dimensions baseDimensions, const std::uint32_t mipLevel)
{
	if (mipLevel == 0)
		return baseDimensions;
	if (mipLevel >= CalcMaxMipLevelCount(baseDimensions))
		return Texas::Dimensions{};

	const std::uint32_t powerOf2 = 1 << mipLevel;
	Dimensions returnValue{};
	returnValue.width = baseDimensions.width / powerOf2;
	if (returnValue.width == 0)
		returnValue.width = 1;
	returnValue.height = baseDimensions.height / powerOf2;
	if (returnValue.height == 0)
		returnValue.height = 1;
	returnValue.depth = baseDimensions.depth / powerOf2;
	if (returnValue.depth == 0)
		returnValue.depth = 1;
	return returnValue;
}

std::size_t Texas::Tools::CalcTotalSizeRequired(Dimensions baseDimensions, std::uint32_t mipLevelCount, std::uint32_t arrayLayerCount, PixelFormat pixelFormat)
{
	if ((mipLevelCount > 1 &&  mipLevelCount >= CalcMaxMipLevelCount(baseDimensions)) || arrayLayerCount == 0)
		return 0;

	std::size_t sum = 0;
	for (std::uint32_t i = 0; i < mipLevelCount; i++)
		sum += CalcImageDataSize(CalcMipmapDimensions(baseDimensions, i), pixelFormat);
	return sum * arrayLayerCount;
}

std::size_t Texas::Tools::CalcImageDataSize(const Dimensions dimensions, const PixelFormat pixelFormat)
{
	const BlockInfo blockInfo = GetBlockInfo(pixelFormat);

	if (IsBCnCompressed(pixelFormat))
	{
		std::size_t blockCountX = static_cast<std::size_t>(ceilf(static_cast<float>(dimensions.width) / static_cast<float>(blockInfo.width)));
		if (blockCountX == 0)
			blockCountX = 1;
		std::size_t blockCountY = static_cast<std::size_t>(ceilf(static_cast<float>(dimensions.height) / static_cast<float>(blockInfo.height)));
		if (blockCountY == 0)
			blockCountY = 1;

		return blockCountX * blockCountY * dimensions.depth * blockInfo.size;
	}

	switch (pixelFormat)
	{
	case PixelFormat::R_8:
		return static_cast<std::size_t>(dimensions.width) * dimensions.height * dimensions.depth * sizeof(std::uint8_t);
	case PixelFormat::RG_8:
		return static_cast<std::size_t>(dimensions.width) * dimensions.height * dimensions.depth * sizeof(std::uint8_t) * 2;
	case PixelFormat::RGB_8:
	case PixelFormat::BGR_8:
		return static_cast<std::size_t>(dimensions.width) * dimensions.height * dimensions.depth * sizeof(std::uint8_t) * 3;
	case PixelFormat::RGBA_8:
	case PixelFormat::BGRA_8:
		return static_cast<std::size_t>(dimensions.width) * dimensions.height * dimensions.depth * sizeof(std::uint8_t) * 4;
	default:
		break;
	}

	return 0;
}