#pragma once

#include <fstream>
#include <optional>

#include "DTex/MetaData.hpp"
#include "DTex/FileFormat.hpp"
#include "DTex/Dimensions.hpp"
#include "DTex/PixelFormat.hpp"
#include "DTex/Colorspace.hpp"

#include "DTex/Tools.hpp"

namespace DTex
{
	namespace detail
	{
		class PrivateAccessor;
	}

	class OpenFile
	{
	public:

		uint32_t GetArrayLayerCount() const;

		/*
			Returns the base dimensions of the loaded image.
		*/
		const Dimensions& GetBaseDimensions() const;

		/*
			Returns the color space type of the loaded image.
		*/
		ColorSpace GetColorSpace() const;

		constexpr const MetaData& GetMetaData() const;

		/*
			Returns the dimensions of the mipmap at the specified miplevel.
			Use miplevel 0 for base dimensions.
			Returns an empty optional if MipLevel argument is equal or higher than GetMipLevelCount()
		*/
		std::optional<Dimensions> GetMipDimensions(uint32_t mipLevel) const;

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

		/*
			Returns the pixel format of the loaded image.
		*/
		PixelFormat GetPixelFormat() const;

		/*
			Gets the file format of the file that was loaded.
		*/
		FileFormat GetSourceFileFormat() const;

		/*
			Returns the texture type of the loaded image.
		*/
		TextureType GetTextureType() const;

		/*
			Returns the total size required for storing all of the imagedata.
		*/
		size_t GetTotalSizeRequired() const;

		/*
			Returns true if the PixelFormat of the image is a compressed type.
		*/
		bool IsCompressed() const;

	private:
		OpenFile() = default;

		MetaData metaData;

		mutable std::ifstream filestream;

		friend class detail::PrivateAccessor;
	};
}

constexpr const DTex::MetaData& DTex::OpenFile::GetMetaData() const
{
	return metaData;
}