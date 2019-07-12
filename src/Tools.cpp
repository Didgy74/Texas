#include "DTex/Tools.hpp"

#include "DTex/Dimensions.hpp"

#include <algorithm>
#include <cmath>

namespace DTex::Tools::detail
{
	template<bool includeTests>
	static size_t CalcImageDataSize_Impl(const Dimensions& dimensions, PixelFormat pixelFormat);
}

uint32_t DTex::Tools::CalcMaxMipLevelCount(const Dimensions& in)
{
	return static_cast<uint32_t>(1) + static_cast<uint32_t>(std::floor(std::log2(std::max({ in.width, in.height, in.depth }))));
}

DTex::Dimensions DTex::Tools::CalcMipmapDimensions(const Dimensions& baseDimensions, const uint32_t mipLevel)
{
	if (mipLevel == 0)
		return baseDimensions;
	if (mipLevel >= CalcMaxMipLevelCount(baseDimensions))
		return {};

	Dimensions returnValue{};
	const Dimensions::ValueType powerOf2 = uint32_t(1) << mipLevel;
	for (size_t dim = 0; dim < 3; dim++)
	{
		returnValue[dim] = baseDimensions[dim] / powerOf2;
		if (returnValue[dim] == Dimensions::ValueType(0))
			returnValue[dim] = Dimensions::ValueType(1);
	}
	return returnValue;
}

size_t DTex::Tools::CalcImageDataSize(const Dimensions& dimensions, PixelFormat pixelFormat)
{
	return detail::CalcImageDataSize_Impl<true>(dimensions, pixelFormat);
}

size_t DTex::Tools::CalcImageDataSize_Unsafe(const Dimensions& dimensions, PixelFormat pixelFormat)
{
	return detail::CalcImageDataSize_Impl<false>(dimensions, pixelFormat);
}

size_t DTex::Tools::CalcTotalSizeRequired(const Dimensions& baseDimensions, uint32_t mipLevelCount, uint32_t arrayLayerCount, PixelFormat pixelFormat)
{
	if (mipLevelCount > CalcMaxMipLevelCount(baseDimensions) || arrayLayerCount == 0)
		return 0;

	size_t sum = 0;
	for (uint32_t i = 0; i < mipLevelCount; i++)
		sum += CalcImageDataSize(CalcMipmapDimensions(baseDimensions, i), pixelFormat);
	return sum * arrayLayerCount;
}

size_t DTex::Tools::CalcTotalSizeRequired_Unsafe(const Dimensions& baseDimensions, uint32_t mipLevelCount, uint32_t arrayLayerCount, PixelFormat pixelFormat)
{
	size_t sum = 0;
	for (uint32_t i = 0; i < mipLevelCount; i++)
		sum += CalcImageDataSize_Unsafe(CalcMipmapDimensions(baseDimensions, i), pixelFormat);
	return sum * arrayLayerCount;
}

template<bool includeTests>
inline size_t DTex::Tools::detail::CalcImageDataSize_Impl(const Dimensions& dimensions, PixelFormat pixelFormat)
{
	if constexpr (includeTests)
	{
		if (!IsValid(pixelFormat))
			return 0;
	}

	const BlockInfo blockInfo = GetBlockInfo(pixelFormat);

	if (IsBCnCompressed(pixelFormat))
	{
		size_t blockCountX = size_t(ceilf(float(dimensions.width) / float(blockInfo.width)));
		if (blockCountX <= 0)
			blockCountX = 1;
		size_t blockCountY = size_t(ceilf(float(dimensions.height) / float(blockInfo.height)));
		if (blockCountY <= 0)
			blockCountY = 1;

		return blockCountX * blockCountY * dimensions.depth * blockInfo.size;
	}

	switch (pixelFormat)
	{
	case PixelFormat::R_8:
		return size_t(dimensions.width) * dimensions.height * dimensions.depth * sizeof(uint8_t);
	case PixelFormat::RG_8:
		return size_t(dimensions.width) * dimensions.height * dimensions.depth * sizeof(uint8_t) * 2;
	case PixelFormat::RGB_8:
		return size_t(dimensions.width) * dimensions.height * dimensions.depth * sizeof(uint8_t) * 3;
	case PixelFormat::RGBA_8:
		return size_t(dimensions.width) * dimensions.height * dimensions.depth * sizeof(uint8_t) * 4;
	default:
		break;
	}

	return 0;
}