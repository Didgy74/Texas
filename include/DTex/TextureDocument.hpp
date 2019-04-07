#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <fstream>
#include <cassert>
#include <array>
#include <vector>
#include <cstring>

namespace DTex
{
	class TextureDocument
	{
	public:
		enum class Target : uint32_t;
		enum class Format : uint32_t;

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
		uint32_t mipLevels{};
		uint32_t arrayLayers{};
	};

	enum class TextureDocument::Target : uint32_t
	{
		Invalid,
		Texture1D,
		Texture2D,
		Texture3D,
	};

	enum class TextureDocument::Format : uint32_t
	{
		Invalid,
		R8G8B8_UNorm,
		R8G8B8A8_UNorm,
		BC7_RGBA_Unorm,
		BC7_sRGB_Unorm
	};

	struct TextureDocument::CreateInfo
	{
		std::vector<std::byte> byteArray;
		std::array<DataInfo, 16> mipMapDataInfo;
		std::array<uint32_t, 3> baseDimensions{};
		Target target{};
		Format format{};
		uint32_t mipLevels{};
		uint32_t arrayLayers{};
	};

	using TexDoc = TextureDocument;

	namespace detail
	{
		constexpr TextureDocument::Target ToTarget(std::array<uint32_t, 3> dimensions, uint32_t arrayLayers)
		{
			using T = TextureDocument::Target;

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

#include "detail/GLFormats.inl"

namespace DTex
{
	namespace detail
	{
		namespace KTX
		{
			//std::array<uint_8_t, 12> kTXIdentifier = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

			//uint32_t correctEndian = 0x04030201;

			struct Header
			{
				std::array<char, 12> identifier;
				uint32_t endianness;
				uint32_t glType;
				uint32_t glTypeSize;
				uint32_t glFormat;
				uint32_t glInternalFormat;
				uint32_t glBaseInternalFormat;
				uint32_t pixelWidth;
				uint32_t pixelHeight;
				uint32_t pixelDepth;
				uint32_t numberOfArrayElements;
				uint32_t numberOfFaces;
				uint32_t numberOfMipmapLevels;
				uint32_t bytesOfKeyValueData;
			};
		}
	}

	std::optional<TexDoc> LoadFromFile(std::string_view path);
#ifdef DTEX_IMPLEMENTATION
	std::optional<TexDoc> LoadFromFile(std::string_view path)
	{
		std::ifstream file(path.data(), std::ios::binary);

		//assert(file.is_open());

		detail::KTX::Header head;


		// Loads all header data.

		file.read(head.identifier.data(), head.identifier.size());

		//assert(head.identifier == detail::KTX::kTXIdentifier);

		file.read(reinterpret_cast<char*>(&head.endianness), sizeof(head.endianness));
		//assert(head.endianness == detail::KTX::correctEndian);

		file.read(reinterpret_cast<char*>(&head.glType), sizeof(head.glType));
		
		file.read(reinterpret_cast<char*>(&head.glTypeSize), sizeof(head.glTypeSize));

		file.read(reinterpret_cast<char*>(&head.glFormat), sizeof(head.glFormat));

		file.read(reinterpret_cast<char*>(&head.glInternalFormat), sizeof(head.glInternalFormat));

		file.read(reinterpret_cast<char*>(&head.glBaseInternalFormat), sizeof(head.glBaseInternalFormat));

		file.read(reinterpret_cast<char*>(&head.pixelWidth), sizeof(head.pixelWidth));

		file.read(reinterpret_cast<char*>(&head.pixelHeight), sizeof(head.pixelHeight));

		file.read(reinterpret_cast<char*>(&head.pixelDepth), sizeof(head.pixelDepth));

		file.read(reinterpret_cast<char*>(&head.numberOfArrayElements), sizeof(head.numberOfArrayElements));

		file.read(reinterpret_cast<char*>(&head.numberOfFaces), sizeof(head.numberOfFaces));

		file.read(reinterpret_cast<char*>(&head.numberOfMipmapLevels), sizeof(head.numberOfMipmapLevels));

		file.read(reinterpret_cast<char*>(&head.bytesOfKeyValueData), sizeof(head.bytesOfKeyValueData));


		// Load KeyValuePair. Currently doesn't do anything.
		struct KeyValuePair
		{
			uint32_t keyAndValueByteSize;
			std::vector<uint8_t> keyAndValue;
			uint32_t valuePadding;
		};

		uint32_t keyValueBytesRemaining = head.bytesOfKeyValueData;
		std::vector<KeyValuePair> keyValuePairs;
		while (keyValueBytesRemaining > 4)
		{
			KeyValuePair temp;
			file.read(reinterpret_cast<char*>(&temp.keyAndValueByteSize), sizeof(temp.keyAndValueByteSize));

			temp.keyAndValue.resize(temp.keyAndValueByteSize);
			file.read(reinterpret_cast<char*>(temp.keyAndValue.data()), temp.keyAndValueByteSize);

			temp.valuePadding = 3 - ((temp.keyAndValueByteSize + 3) % 4);
			file.ignore(temp.valuePadding);

			keyValuePairs.push_back(temp);

			keyValueBytesRemaining -= (temp.keyAndValueByteSize + temp.valuePadding);
		}

		// Loads all the image data
		std::vector<std::byte> imageData;

		std::streampos imageDataStart = file.tellg();
		file.seekg(0, file.end);
		std::streampos imageDataEnd = file.tellg();

		size_t imageDataByteLength = imageDataEnd - imageDataStart;
		imageData.resize(imageDataByteLength);
		
		file.seekg(imageDataStart);

		file.read(reinterpret_cast<char*>(imageData.data()), imageDataByteLength);

		file.close();


		// Converts loaded info to TexDoc
		TextureDocument::CreateInfo createInfo{};

		createInfo.byteArray = std::move(imageData);

		createInfo.baseDimensions = { head.pixelWidth, head.pixelHeight, head.pixelDepth };

		createInfo.arrayLayers = head.numberOfArrayElements;

		createInfo.mipLevels = head.numberOfMipmapLevels;
		
		createInfo.target = detail::ToTarget(createInfo.baseDimensions, createInfo.arrayLayers);

		createInfo.format = detail::GLFormatAndGLTypeToFormat(head.glFormat, head.glType);


		// Read DataInfo from the imageData buffer
		const std::byte* ptr = createInfo.byteArray.data();
		for (size_t i = 0; i < createInfo.mipLevels; i++)
		{
			uint32_t imageByteLength = *reinterpret_cast<const uint32_t*>(ptr);
			size_t offset = ptr - createInfo.byteArray.data();

			createInfo.mipMapDataInfo[i].byteLength = imageByteLength;
			createInfo.mipMapDataInfo[i].offset = offset;

			ptr += sizeof(imageByteLength) + imageByteLength + (offset % 4);
		}

		return std::optional<TextureDocument>{ std::move(createInfo) };
	}
#endif
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

	TextureDocument::Target TextureDocument::GetTarget() const
	{
		return target;
	}

	TextureDocument::Format TextureDocument::GetFormat() const
	{
		return format;
	}

	bool TextureDocument::IsCompressed() const
	{
		return false;
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
}
#endif