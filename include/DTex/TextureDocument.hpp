#pragma once

#include <cstdint>
#include <cstddef>
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
		const uint8_t* GetData(uint32_t mipLevel) const;

		/*
			Returns buffer-size of texture at specified miplevel.
			Use miplevel 0 for base-texture.
		*/
		uint32_t GetDataSize(uint32_t mipLevel) const;


		/*
			Returns a pointer to the first element in the internal buffer.
			Mipmaps may or may not be stored contiguously in this buffer.
		*/
		const uint8_t* GetInternalBuffer() const;

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
		std::vector<uint8_t> byteArray;
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
		std::vector<uint8_t> byteArray;
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