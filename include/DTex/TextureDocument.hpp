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
		TextureDocument(TextureDocument&&) = default;

		TextureDocument& operator=(const TextureDocument&) = delete;
		TextureDocument& operator=(TextureDocument&&) = default;

		std::array<uint32_t, 3> GetDimensions(uint32_t mipLevel) const;
		const std::byte* GetData(uint32_t mipLevel) const;
		size_t GetDataOffset(uint32_t mipLevel) const;
		uint32_t GetByteLength(uint32_t mipLevel) const;

		Target GetTarget() const;
		Format GetFormat() const;

		bool IsCompressed() const;

		uint32_t GetArrayLayers() const;
		uint32_t GetMipLevels() const;

		uint32_t GetGLType() const;
		uint32_t GetGLFormat() const;
		uint32_t GetGLTarget() const;

		uint32_t GetVulkanFormat() const;

	private:
		std::vector<std::byte> byteArray;
		std::array<DataInfo, 16> mipMapDataInfo;
		std::array<uint32_t, 3> baseDimensions{};
		Target target{};
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
		Target target{};
		Format format{};
		bool isCompressed{};
		uint32_t mipLevels{};
		uint32_t arrayLayers{};
	};

	using TexDoc = TextureDocument;

	namespace detail
	{
		constexpr Target ToTarget(std::array<uint32_t, 3> dimensions, uint32_t arrayLayers)
		{
			using T = Target;

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
		target = in.target;
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
		return byteArray.data() + GetDataOffset(mipLevel);
	}

	size_t TextureDocument::GetDataOffset(uint32_t mipLevel) const
	{
		return mipMapDataInfo[mipLevel].offset;
	}

	uint32_t TextureDocument::GetByteLength(uint32_t mipLevel) const
	{
		return uint32_t(mipMapDataInfo[mipLevel].byteLength);
	}

	Target TextureDocument::GetTarget() const
	{
		return target;
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
		return detail::ToGLTarget(target);
	}

	uint32_t TextureDocument::GetVulkanFormat() const
	{
		return detail::ToVkFormat(format);
	}
}
#endif