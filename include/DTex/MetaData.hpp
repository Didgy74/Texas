#pragma once

#include "FileFormat.hpp"
#include "DTex/Dimensions.hpp"
#include "DTex/PixelFormat.hpp"
#include "DTex/TextureType.hpp"
#include "DTex/Colorspace.hpp"
#include "DTex/Tools.hpp"

#include <optional>

namespace DTex
{
	struct MetaData
	{
		/*
			Gets the file format of the file that was loaded.
		*/
		constexpr FileFormat GetSourceFileFormat() const;

		/*
			Returns the base dimensions of the loaded image.
		*/
		constexpr const Dimensions& GetBaseDimensions() const;

		/*
			Returns the dimensions of the mipmap at the specified miplevel.
			Use miplevel 0 for base dimensions.
			Returns an empty optional if MipLevel argument is equal or higher than GetMipLevelCount()
		*/
		std::optional<Dimensions> GetDimensions(uint32_t mipLevel = 0) const;

		/*
			Returns the pixel format of the loaded image.
		*/
		constexpr PixelFormat GetPixelFormat() const;

		/*
			Returns true if the PixelFormat of the image is a compressed type.
		*/
		constexpr bool IsCompressed() const;

		/*
			Returns the color space type of the loaded image.
		*/
		constexpr ColorSpace GetColorSpace() const;

		/*
			Returns the texture type of the loaded image.
		*/
		constexpr TextureType GetTextureType() const;

		/*
			Returns the total size required for storing all the mips and image array elements.
		*/
		size_t GetTotalSizeRequired() const;

		/*
			Returns the length of the mipmap chain of the loaded image.
		*/
		constexpr uint32_t GetMipLevelCount() const;

		/*
			Returns the offset for a given MipLevel from the start of a linear buffer.
			Returns an empty optional if MipLevel argument is equal or higher than GetMipLevelCount()
		*/
		std::optional<size_t> GetMipLevelOffset(uint32_t mipLevel) const;

		/*
			Returns the size of a given MipLevel in bytes.
			Returns an empty optional if MipLevel argument is equal or higher than GetMipLevelCount()
		*/
		std::optional<size_t> GetMipLevelSize(uint32_t mipLevel) const;

		constexpr uint32_t GetArrayLayerCount() const;

		FileFormat srcFileFormat = {};
		Dimensions baseDimensions = {};
		PixelFormat pixelFormat = {};
		ColorSpace colorSpace = {};
		uint32_t mipLevelCount = 0;
		uint32_t arrayLayerCount = 0;
	};
}

constexpr DTex::FileFormat DTex::MetaData::GetSourceFileFormat() const
{
	return srcFileFormat;
}

constexpr const DTex::Dimensions& DTex::MetaData::GetBaseDimensions() const
{
	return baseDimensions;
}

constexpr DTex::PixelFormat DTex::MetaData::GetPixelFormat() const
{
	return pixelFormat;
}

constexpr bool DTex::MetaData::IsCompressed() const
{
	return Tools::IsCompressed(pixelFormat);
}

constexpr DTex::ColorSpace DTex::MetaData::GetColorSpace() const
{
	return colorSpace;
}

constexpr DTex::TextureType DTex::MetaData::GetTextureType() const
{
	return Tools::ToTextureType(baseDimensions, arrayLayerCount);
}

inline size_t DTex::MetaData::GetTotalSizeRequired() const
{
	return Tools::CalcTotalSizeRequired(baseDimensions, mipLevelCount, arrayLayerCount, pixelFormat);
}

constexpr uint32_t DTex::MetaData::GetMipLevelCount() const
{
	return mipLevelCount;
}