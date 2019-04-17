#pragma once

#include <optional>
#include <string_view>
#include <variant>

#include "detail/KTX.hpp"

#include "GLFormats.hpp"

#include "LoadResult.hpp"

#include "TextureDocument.hpp"

namespace DTex
{
	inline LoadResult<TextureDocument> LoadFromFile(std::string_view path)
	{
		using ReturnType = LoadResult<TextureDocument>;

		std::ifstream file(path.data(), std::ios::binary);

		if (!file.is_open())
			return ReturnType{ ResultInfo::CouldNotReadFile };

		detail::KTX::Header head{};

		// Loads all header data.

		file.read(reinterpret_cast<char*>(head.identifier.data()), head.identifier.size());
		if (head.identifier != detail::KTX::kTXIdentifier)
			return ReturnType{ ResultInfo::CorruptFileData };

		file.read(reinterpret_cast<char*>(&head.endianness), sizeof(head.endianness));
		if (head.endianness != detail::KTX::correctEndian)
			return ReturnType{ ResultInfo::CorruptFileData };

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
		if (head.numberOfMipmapLevels <= 0)
			return ReturnType{ ResultInfo::FileNotSupported };

		file.read(reinterpret_cast<char*>(&head.bytesOfKeyValueData), sizeof(head.bytesOfKeyValueData));
		if (file.eof())
			return ReturnType{ ResultInfo::CorruptFileData };

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

		size_t imageDataByteLength = size_t(imageDataEnd - imageDataStart);
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

		createInfo.textureType = ToTextureType(createInfo.baseDimensions, createInfo.arrayLayers);

		bool isCompressed = false;
		createInfo.pixelFormat = ToFormat(head.glInternalFormat, head.glType, isCompressed);
		if (createInfo.pixelFormat == PixelFormat::Invalid)
			return ReturnType{ ResultInfo::PixelFormatNotSupported };

		createInfo.isCompressed = isCompressed;

		// Read DataInfo from the imageData buffer
		size_t index = 0;
		for (size_t i = 0; i < createInfo.mipLevels; i++)
		{
			if (index > createInfo.byteArray.size() - sizeof(uint32_t))
				return ReturnType{ ResultInfo::CorruptFileData };

			uint32_t imageByteLength = *reinterpret_cast<const uint32_t*>(&createInfo.byteArray.at(index));
			if (imageByteLength > createInfo.byteArray.size() - index)
				return ReturnType{ ResultInfo::CorruptFileData };

			index += sizeof(imageByteLength);

			createInfo.mipMapDataInfo[i].byteLength = imageByteLength;
			createInfo.mipMapDataInfo[i].offset = index;

			size_t padding = ((index + 4) % 4);
			index += imageByteLength + padding;
		}

		return ReturnType{ TextureDocument{ std::move(createInfo) } };
	}
}