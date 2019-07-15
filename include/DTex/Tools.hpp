#pragma once

#include "DTex/PixelFormat.hpp"
#include "DTex/Dimensions.hpp"
#include "DTex/TextureType.hpp"
#include "DTex/BlockInfo.hpp"

#include <cstdint>

namespace DTex::Tools
{
	/*
		Returns 0 upon failure, such as invalid dimensions for pixelformat
	*/
	size_t CalcImageDataSize(const Dimensions& dimensions, PixelFormat pixelFormat);

	/*
		Does the same as Tools::CalcImageDataSize but doesn't check if dimensions don't fit the pixelFormat.
	*/
	size_t CalcImageDataSize_Unsafe(const Dimensions& dimensions, PixelFormat pixelFormat);

	uint32_t CalcMaxMipLevelCount(const Dimensions& in);

	Dimensions CalcMipmapDimensions(const Dimensions& baseDimensions, uint32_t mipLevel);

	size_t CalcTotalSizeRequired(const Dimensions& baseDimensions, uint32_t mipLevelCount, uint32_t arrayLayerCount, PixelFormat pixelFormat);

	size_t CalcTotalSizeRequired_Unsafe(const Dimensions& baseDimensions, uint32_t mipLevelCount, uint32_t arrayLayerCount, PixelFormat pixelFormat);

	/*
		Returns the block-info for given pixelFormat.
	*/
	constexpr BlockInfo GetBlockInfo(PixelFormat pixelFormat);

	constexpr bool IsBCnCompressed(PixelFormat pixelFormat);

	/*
		Returns true if pixelFormat is a compressed format.
	*/
	constexpr bool IsCompressed(PixelFormat pixelFormat);
}

constexpr DTex::BlockInfo DTex::Tools::GetBlockInfo(PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
	case PixelFormat::BC1_RGB:
	case PixelFormat::BC1_RGBA:
		return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 8 };
	case PixelFormat::BC2:
	case PixelFormat::BC3:
		return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 16 };
	case PixelFormat::BC4_Signed:
	case PixelFormat::BC4_Unsigned:
		return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 8 };
	case PixelFormat::BC5_Signed:
	case PixelFormat::BC5_Unsigned:
	case PixelFormat::BC6H_SFloat:
	case PixelFormat::BC6H_UFloat:
	case PixelFormat::BC7:
		return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 16 };
	}

	return {};
}

constexpr bool DTex::Tools::IsBCnCompressed(PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
	case PixelFormat::BC1_RGB:
	case PixelFormat::BC1_RGBA:
	case PixelFormat::BC2:
	case PixelFormat::BC3:
	case PixelFormat::BC4_Signed:
	case PixelFormat::BC4_Unsigned:
	case PixelFormat::BC5_Signed:
	case PixelFormat::BC5_Unsigned:
	case PixelFormat::BC6H_SFloat:
	case PixelFormat::BC6H_UFloat:
	case PixelFormat::BC7:
		return true;
	}

	return false;
}

constexpr bool DTex::Tools::IsCompressed(PixelFormat pixelFormat)
{
	return IsBCnCompressed(pixelFormat);
}
