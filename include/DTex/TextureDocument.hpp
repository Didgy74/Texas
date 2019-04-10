#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <fstream>
#include <cassert>
#include <array>
#include <vector>
#include <cstring>
#include <cmath>

#include "Typedefs.hpp"

namespace DTex
{
	class TextureDocument
	{
	public:
		struct DataInfo
		{
			size_t offset;
			size_t byteLength;
		};
		struct CreateInfo;

		TextureDocument() = delete;
		TextureDocument(const TextureDocument&) = delete;
		TextureDocument(TextureDocument&&) = default;
		explicit TextureDocument(CreateInfo&&);

		TextureDocument& operator=(const TextureDocument&) = delete;
		TextureDocument& operator=(TextureDocument&&) = delete;

		/*
			Returns the dimensions of the mipmap at the specified miplevel.
			Use miplevel 0 for base dimensions.
		*/
		Dimensions GetDimensions(uint32_t mipLevel) const;

		/*
			Returns a pointer to the texture-data at the specified miplevel.
			Use miplevel 0 for base-texture.
		*/
		const std::byte* GetData(uint32_t mipLevel) const;

		/*
			Returns buffer-size of texture at specified miplevel.
			Use miplevel 0 for base-texture.
		*/
		uint32_t GetDataSize(uint32_t mipLevel) const;


		/*
			Returns a pointer to the first element in the internal buffer.
			Mipmaps may or may not be stored contiguously in this buffer.
		*/
		const std::byte* GetInternalBuffer() const;

		/*
			Returns the total byte size of the internal buffer.
		*/
		size_t GetInternalBufferSize() const;

		TextureType GetTextureType() const;
		PixelFormat GetPixelFormat() const;

		bool IsCompressed() const;

		uint32_t GetArrayLayers() const;
		uint32_t GetMipLevels() const;


	private:
		std::vector<std::byte> byteArray;
		std::array<DataInfo, 16> mipMapDataInfo;
		Dimensions baseDimensions{};
		TextureType textureType{};
		PixelFormat pixelFormat{};
		bool isCompressed{};
		uint32_t mipLevels{};
		uint32_t arrayLayers{};
	};

	struct TextureDocument::CreateInfo
	{
		std::vector<std::byte> byteArray;
		std::array<DataInfo, 16> mipMapDataInfo;
		Dimensions baseDimensions{};
		TextureType textureType{};
		PixelFormat pixelFormat{};
		bool isCompressed{};
		uint32_t mipLevels{};
		uint32_t arrayLayers{};
	};

	using TexDoc = TextureDocument;
}


namespace DTex
{
	inline TextureDocument::TextureDocument(CreateInfo&& in)
	{
		byteArray = std::move(in.byteArray);
		mipMapDataInfo = in.mipMapDataInfo;
		baseDimensions = in.baseDimensions;
		textureType = in.textureType;
		pixelFormat = in.pixelFormat;
		isCompressed = in.isCompressed;
		mipLevels = in.mipLevels;
		arrayLayers = in.arrayLayers;
	}

	inline Dimensions TextureDocument::GetDimensions(uint32_t mipLevel) const
	{
		Dimensions returnValue;
		for (size_t dim = 0; dim < 3; dim++)
			returnValue[dim] = uint32_t(baseDimensions[dim] / (std::pow(2, mipLevel)));
		return returnValue;
	}

	inline const std::byte* TextureDocument::GetData(uint32_t mipLevel) const
	{
		return byteArray.data() + mipMapDataInfo[mipLevel].offset;
	}

	inline uint32_t TextureDocument::GetDataSize(uint32_t mipLevel) const
	{
		return uint32_t(mipMapDataInfo[mipLevel].byteLength);
	}

	inline const std::byte* TextureDocument::GetInternalBuffer() const
	{
		return byteArray.data();
	}

	inline size_t TextureDocument::GetInternalBufferSize() const
	{
		return byteArray.size();
	}

	inline TextureType TextureDocument::GetTextureType() const
	{
		return textureType;
	}

	inline PixelFormat TextureDocument::GetPixelFormat() const
	{
		return pixelFormat;
	}

	inline bool TextureDocument::IsCompressed() const
	{
		return isCompressed;
	}

	inline uint32_t TextureDocument::GetArrayLayers() const
	{
		return arrayLayers;
	}

	inline uint32_t TextureDocument::GetMipLevels() const
	{
		return mipLevels;
	}
}