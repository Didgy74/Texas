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

#include "detail/GLFormats.hpp"
#include "detail/VkFormats.hpp"

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
		explicit TextureDocument(CreateInfo&&);
		explicit TextureDocument(TextureDocument&&) = default;

		TextureDocument& operator=(const TextureDocument&) = delete;
		TextureDocument& operator=(TextureDocument&&) = default;

		/*
			Returns the dimensions of the mipmap at the specified miplevel.
			Use miplevel 0 for base dimensions.
		*/
		std::array<uint32_t, 3> GetDimensions(uint32_t mipLevel) const;

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

		Type GetType() const;
		Format GetFormat() const;

		bool IsCompressed() const;

		uint32_t GetArrayLayers() const;
		uint32_t GetMipLevels() const;

		/*
			Returns the GL type for this TextureDocument's pixel data, i.e GL_UNSIGNED_BYTE.
		*/
		uint32_t GetGLType() const;
		/*
			Returns the GL texture format for this TextureDocument, i.e GL_RGBA or GL_COMPRESSED_RGBA_BPTC_UNORM.
		*/
		uint32_t GetGLFormat() const;
		/*
			Returns the GL texture target for this TextureDocument, i.e GL_TEXTURE_2D or  GL_TEXTURE_2D_ARRAY.
		*/
		uint32_t GetGLTarget() const;

		uint32_t GetVkFormat() const;
		uint32_t GetVkImageType() const;

	private:
		std::vector<std::byte> byteArray;
		std::array<DataInfo, 16> mipMapDataInfo;
		std::array<uint32_t, 3> baseDimensions{};
		Type type{};
		Format format{};
		bool isCompressed{};
		uint32_t mipLevels{};
		uint32_t arrayLayers{};
	};

	struct TextureDocument::CreateInfo
	{
		std::vector<std::byte> byteArray;
		std::array<DataInfo, 16> mipMapDataInfo;
		std::array<uint32_t, 3> baseDimensions{};
		Type type{};
		Format format{};
		bool isCompressed{};
		uint32_t mipLevels{};
		uint32_t arrayLayers{};
	};

	using TexDoc = TextureDocument;

	namespace detail
	{
		constexpr Type ToType(std::array<uint32_t, 3> dimensions, uint32_t arrayLayers)
		{
			using T = Type;

			if (dimensions[1] == 0 && dimensions[2] == 0)
				return T::Texture1D;
			else if (dimensions[2] == 0)
				return T::Texture2D;
			else if (dimensions[2] != 0)
				return T::Texture3D;

			return T::Invalid;
		}
	}
}

#ifdef DTEX_IMPLEMENTATION
namespace DTex
{
	TextureDocument::TextureDocument(CreateInfo&& in)
	{
		byteArray = std::move(in.byteArray);
		mipMapDataInfo = in.mipMapDataInfo;
		baseDimensions = in.baseDimensions;
		type = in.type;
		format = in.format;
		isCompressed = in.isCompressed;
		mipLevels = in.mipLevels;
		arrayLayers = in.arrayLayers;
	}

	std::array<uint32_t, 3> TextureDocument::GetDimensions(uint32_t mipLevel) const
	{
		std::array<uint32_t, 3> returnValue;
		for (size_t dim = 0; dim < 3; dim++)
			returnValue[dim] = uint32_t(baseDimensions[dim] / (std::pow(2, mipLevel)));
		return returnValue;
	}

	const std::byte* TextureDocument::GetData(uint32_t mipLevel) const
	{
		return byteArray.data() + mipMapDataInfo[mipLevel].offset;
	}

	uint32_t TextureDocument::GetDataSize(uint32_t mipLevel) const
	{
		return uint32_t(mipMapDataInfo[mipLevel].byteLength);
	}

	const std::byte* TextureDocument::GetInternalBuffer() const
	{
		return byteArray.data();
	}

	size_t TextureDocument::GetInternalBufferSize() const
	{
		return byteArray.size();
	}

	Type TextureDocument::GetType() const
	{
		return type;
	}

	Format TextureDocument::GetFormat() const
	{
		return format;
	}

	bool TextureDocument::IsCompressed() const
	{
		return isCompressed;
	}

	uint32_t TextureDocument::GetArrayLayers() const
	{
		return arrayLayers;
	}

	uint32_t TextureDocument::GetMipLevels() const
	{
		return mipLevels;
	}

	uint32_t TextureDocument::GetGLFormat() const
	{
		return detail::ToGLFormat(format);
	}

	uint32_t TextureDocument::GetGLType() const
	{
		return detail::ToGLType(format);
	}

	uint32_t TextureDocument::GetGLTarget() const
	{
		return detail::ToGLTarget(type);
	}

	uint32_t TextureDocument::GetVkFormat() const
	{
		return detail::ToVkFormat(format);
	}

	uint32_t TextureDocument::GetVkImageType() const
	{
		return detail::ToVkImageType(type);
	}
}
#endif