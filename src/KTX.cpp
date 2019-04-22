#include "KTX.hpp"

#include "DTex/DTex.hpp"

#include "DTex/GLFormats.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <array>
#include <fstream>
#include <cstring>

namespace DTex
{
	namespace detail
	{
		namespace KTX
		{
			LoadResult<TextureDocument> LoadKTX(std::filesystem::path path)
			{
				using ReturnType = LoadResult<TextureDocument>;

				std::array<uint8_t, KTX::headerSize> headerData{};

				std::ifstream file(path, std::ios::binary);
				if (!file.is_open())
					return ReturnType(ResultInfo::CouldNotReadFile, "Could not open file with path: " + path.string());

				file.read(reinterpret_cast<char*>(headerData.data()), sizeof(headerData));
				if (file.eof())
					return ReturnType(ResultInfo::CorruptFileData, "Reached end-of-file while reading file identifier.");

				Header& head = *reinterpret_cast<Header*>(headerData.data());
				if (head.identifier != identifier)
					return ReturnType(ResultInfo::CorruptFileData, "File identifier does not match KTX identifier.");
				else if (head.endianness != correctEndian)
					return ReturnType(ResultInfo::FileNotSupported, "Loader limitation: File endianness does not match system endianness. Loader is not capable of converting.");
				else if (head.glInternalFormat == 0)
					return ReturnType(ResultInfo::PixelFormatNotSupported, "glInternalFormat of KTX file was 0.");

				// Loads rest of the file data
				auto imageDataStart = file.tellg();
				file.seekg(0, std::ifstream::end);
				auto imageDataEnd = file.tellg();
				file.seekg(imageDataStart);

				std::vector<uint8_t> fileData(size_t(imageDataEnd - imageDataStart));

				file.read(reinterpret_cast<char*>(fileData.data()), fileData.size());
				file.close();

				size_t indexInBuffer = 0;

				// Round keyValueDataByteLength down to nearest multiplum of 4
				size_t keyValueDataLength = head.bytesOfKeyValueData - (head.bytesOfKeyValueData % sizeof(uint32_t));
				std::vector<std::byte> keyValueData(keyValueDataLength);
				std::memcpy(keyValueData.data(), &fileData[indexInBuffer], keyValueDataLength);
				indexInBuffer += keyValueDataLength;


				// Converts loaded info to TexDoc
				TextureDocument::CreateInfo createInfo{};
				createInfo.byteArray = std::move(fileData);
				createInfo.baseDimensions = { head.pixelWidth, head.pixelHeight, head.pixelDepth };
				createInfo.arrayLayers = std::max(uint32_t(1), head.numberOfArrayElements);
				createInfo.mipLevels = std::max(uint32_t(1), head.numberOfMipmapLevels);
				createInfo.textureType = ToTextureType(createInfo.baseDimensions, createInfo.arrayLayers);

				bool isCompressed = false;
				createInfo.pixelFormat = ToFormat(head.glInternalFormat, head.glType, isCompressed);
				if (createInfo.pixelFormat == PixelFormat::Invalid)
					return ReturnType{ ResultInfo::PixelFormatNotSupported, "Loader limitation: KTX file is encoded in a pixel format currently not supported." };

				createInfo.isCompressed = isCompressed;

				// Read DataInfo from the imageData buffer
				for (size_t i = 0; i < createInfo.mipLevels; i++)
				{
					if (indexInBuffer > createInfo.byteArray.size() - sizeof(uint32_t))
						return ReturnType{ ResultInfo::CorruptFileData, "Reached premature end-of-file when loading mipmaps in KTX-file." };

					uint32_t imageByteLength{};
					std::memcpy(&imageByteLength, &createInfo.byteArray[indexInBuffer], sizeof(imageByteLength));
					if (imageByteLength > createInfo.byteArray.size() - indexInBuffer)
						return ReturnType{ ResultInfo::CorruptFileData, "" };

					indexInBuffer += sizeof(imageByteLength);

					createInfo.mipMapDataInfo[i].byteLength = imageByteLength;
					createInfo.mipMapDataInfo[i].offset = indexInBuffer;

					size_t padding = ((indexInBuffer + 4) % 4);
					indexInBuffer += imageByteLength + padding;
				}

				return ReturnType{ TextureDocument{ std::move(createInfo) } };
			}
		}
	}
}