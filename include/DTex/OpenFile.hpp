#pragma once

#include <fstream>
#include <optional>

#include "DTex/MetaData.hpp"
#include "DTex/Typedefs.hpp"
#include "DTex/FileFormat.hpp"
#include "DTex/Dimensions.hpp"
#include "DTex/PixelFormat.hpp"
#include "DTex/Colorspace.hpp"

#include "DTex/Tools.hpp"

namespace DTex
{
	class OpenFile
	{
	public:
		/*
		Gets the file format of the file that was loaded.
		*/
		FileFormat GetSourceFileFormat() const;

		/*
			Returns the base dimensions of the loaded image.
		*/
		const Dimensions& GetBaseDimensions() const;

		/*
			Returns the dimensions of the mipmap at the specified miplevel.
			Use miplevel 0 for base dimensions.
			Returns an empty optional if MipLevel argument is equal or higher than GetMipLevelCount()
		*/
		std::optional<Dimensions> GetDimensions(uint32_t mipLevel = 0) const;

		/*
			Returns the pixel format of the loaded image.
		*/
		PixelFormat GetPixelFormat() const;

		/*
			Returns true if the PixelFormat of the image is a compressed type.
		*/
		bool IsCompressed() const;

		/*
			Returns the color space type of the loaded image.
		*/
		ColorSpace GetColorSpace() const;

		/*
			Returns the texture type of the loaded image.
		*/
		TextureType GetTextureType() const;

		/*
			Returns the total size required for storing all the mips and image array elements.
		*/
		size_t GetTotalSizeRequired() const;

		/*
			Returns the length of the mipmap chain of the loaded image.
		*/
		uint32_t GetMipLevelCount() const;

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

		uint32_t GetArrayLayerCount() const;

	private:
		OpenFile() = default;

		MetaData metaData;

		mutable std::ifstream filestream;

		friend class detail::PrivateAccessor;
	};
}

inline DTex::FileFormat DTex::OpenFile::GetSourceFileFormat() const
{
	return metaData.srcFileFormat;
}

inline const DTex::Dimensions& DTex::OpenFile::GetBaseDimensions() const
{
	return metaData.baseDimensions;
}

inline DTex::PixelFormat DTex::OpenFile::GetPixelFormat() const
{
	return metaData.pixelFormat;
}

inline bool DTex::OpenFile::IsCompressed() const
{
	return Tools::IsCompressed(metaData.pixelFormat);
}

inline DTex::ColorSpace DTex::OpenFile::GetColorSpace() const
{
	return metaData.colorSpace;
}

inline DTex::TextureType DTex::OpenFile::GetTextureType() const
{
	return Tools::ToTextureType(metaData.baseDimensions, metaData.arrayLayerCount);
}

inline uint32_t DTex::OpenFile::GetMipLevelCount() const
{
	return metaData.mipLevelCount;
}