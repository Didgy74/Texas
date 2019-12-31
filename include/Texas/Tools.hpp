#pragma once

#include "Texas/PixelFormat.hpp"
#include "Texas/Dimensions.hpp"
#include "Texas/TextureType.hpp"
#include "Texas/BlockInfo.hpp"

#include <cstdint>
#include <cstddef>

namespace Texas::Tools
{
	/*
		Returns 0 upon failure, such as invalid dimensions for pixelformat
	*/
	std::size_t CalcImageDataSize(Dimensions dimensions, PixelFormat pixelFormat);

	std::uint32_t CalcMaxMipLevelCount(Dimensions in);

	Dimensions CalcMipmapDimensions(Dimensions baseDimensions, std::uint32_t mipLevel);

	std::size_t CalcTotalSizeRequired(Dimensions baseDimensions, std::uint32_t mipLevelCount, std::uint32_t arrayLayerCount, PixelFormat pixelFormat);

	/*
		Returns the block-info for given pixelFormat.
	*/
	constexpr BlockInfo GetBlockInfo(PixelFormat pixelFormat);

	constexpr bool IsBCnCompressed(PixelFormat pixelFormat);

	/*
		Returns true if pixelFormat is a compressed format.
	*/
	constexpr bool IsCompressed(PixelFormat pixelFormat);


	//
	// Definitions
	//


	constexpr BlockInfo GetBlockInfo(PixelFormat pixelFormat)
	{
		switch (pixelFormat)
		{
		case PixelFormat::BC1_RGB:
		case PixelFormat::BC1_RGBA:
			return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 8 };
			return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 16 };
		case PixelFormat::BC4:
			return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 8 };
		case PixelFormat::BC5:
		case PixelFormat::BC6H:
		case PixelFormat::BC7_RGBA:
			return BlockInfo{ 4, 4, 1, sizeof(uint8_t) * 16 };
		}

		return {};
	}

	constexpr bool IsBCnCompressed(PixelFormat pixelFormat)
	{
		switch (pixelFormat)
		{
		case PixelFormat::BC1_RGB:
		case PixelFormat::BC1_RGBA:
		case PixelFormat::BC4:
		case PixelFormat::BC5:
		case PixelFormat::BC6H:
		case PixelFormat::BC7_RGBA:
			return true;
		}

		return false;
	}

	constexpr bool Tools::IsCompressed(PixelFormat pixelFormat)
	{
		return IsBCnCompressed(pixelFormat);
	}
}


