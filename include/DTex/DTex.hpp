#pragma once

#include <optional>
#include <string_view>

#include "detail/KTX.hpp"

#include "TextureDocument.hpp"

namespace DTex
{
	std::optional<TexDoc> LoadFromFile(std::string_view path);
}

//#ifdef DTEX_IMPLEMENTATION
namespace DTex
{
	std::optional<TexDoc> LoadFromFile(std::string_view path)
	{
		std::ifstream file(path.data(), std::ios::binary);

		assert(file.is_open());

		detail::KTX::Header head{};


		// Loads all header data.

		file.read(reinterpret_cast<char*>(head.identifier.data()), head.identifier.size());
		assert(head.identifier == detail::KTX::kTXIdentifier);

		file.read(reinterpret_cast<char*>(&head.endianness), sizeof(head.endianness));
		assert(head.endianness == detail::KTX::correctEndian);

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
			uint32_t keyAndValueByteSize{};
			std::vector<uint8_t> keyAndValue;
			uint32_t valuePadding{};
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
		file.seekg(0, std::ifstream::end);
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

		bool isCompressed;
		createInfo.format = detail::ToFormat(head.glInternalFormat, head.glType, isCompressed);
		
		createInfo.isCompressed = isCompressed;

		// Read DataInfo from the imageData buffer
		const std::byte * ptr = createInfo.byteArray.data();
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
}
//#endif