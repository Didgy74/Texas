#pragma once

#include "detail/PNG.hpp"

#include "zlib/zlib.h"

namespace DTex
{
	namespace detail
	{
		namespace PNG
		{
			uint8_t PaethPredictor(uint8_t a, uint8_t b, uint8_t c);

			LoadResult<TextureDocument> LoadPNG(std::filesystem::path path)
			{
				using ReturnType = LoadResult<TextureDocument>;

				std::ifstream file(path, std::ios::binary);
				if (!file.is_open())
					return ReturnType( ResultInfo::CouldNotReadFile, "Could not open file in path: " + path.string());

				std::array<uint8_t, 8> fileIdentifier{};
				file.read(reinterpret_cast<char*>(fileIdentifier.data()), sizeof(fileIdentifier));
				if (file.eof())
					return ReturnType{ ResultInfo::CorruptFileData, "Reached end of file while loading identifier." };
				if (fileIdentifier != identifier)
					return ReturnType{ ResultInfo::CorruptFileData, "File-identifier does not match PNG file-identifier." };

				// Loads rest of the file data
				auto imageDataStart = file.tellg();
				file.seekg(0, std::ifstream::end);
				auto imageDataEnd = file.tellg();

				std::vector<uint8_t> fileData;
				fileData.resize(size_t(imageDataEnd - imageDataStart));

				file.seekg(imageDataStart);

				file.read(reinterpret_cast<char*>(fileData.data()), fileData.size());
				file.close();

				size_t streamPos = 0;

				// First chunk length must be always 13. No need to check
				streamPos += sizeof(uint32_t);

				std::array<char, 4> chunkType1{};
				std::memcpy(chunkType1.data(), &fileData[streamPos], sizeof(chunkType1));
				streamPos += sizeof(chunkType1);

				uint32_t imageWidth = (fileData[streamPos + 3] << 0) | (fileData[streamPos + 2] << 8) | (fileData[streamPos + 1] << 16) | (fileData[streamPos + 0] << 24);
				streamPos += sizeof(imageWidth);

				uint32_t imageHeight = (fileData[streamPos + 3] << 0) | (fileData[streamPos + 2] << 8) | (fileData[streamPos + 1] << 16) | (fileData[streamPos + 0] << 24);
				streamPos += sizeof(imageHeight);

				uint8_t bitDepth{};
				std::memcpy(&bitDepth, &fileData[streamPos], sizeof(bitDepth));
				streamPos += sizeof(bitDepth);
				if (bitDepth != 8)
					return ReturnType(ResultInfo::FileNotSupported, "Loader limitation: Can't read PNGs with bitdepth NOT 8.");

				uint8_t colorType{};
				std::memcpy(&colorType, &fileData[streamPos], sizeof(colorType));
				streamPos += sizeof(colorType);
				if (colorType != 6 && colorType != 2)
					return ReturnType(ResultInfo::FileNotSupported, "Loader limitation: Can't read PNGs not encoded as true-color RGB or RGBA.");

				uint8_t compressionMethod{};
				std::memcpy(&compressionMethod, &fileData[streamPos], sizeof(compressionMethod));
				streamPos += sizeof(compressionMethod);
				if (compressionMethod != 0)
					return ReturnType(ResultInfo::FileNotSupported, "Loader limitation: Can't read PNGs with compression method != 0.");

				uint8_t filterMethod{};
				std::memcpy(&filterMethod, &fileData[streamPos], sizeof(filterMethod));
				streamPos += sizeof(filterMethod);
				if (filterMethod != 0)
					return ReturnType(ResultInfo::FileNotSupported, "Loader limitation: Can't read PNGs with filtering method != 0.");

				uint8_t interlaceMethod{};
				std::memcpy(&interlaceMethod, &fileData[streamPos], sizeof(interlaceMethod));
				streamPos += sizeof(interlaceMethod);
				if (interlaceMethod != 0)
					return ReturnType(ResultInfo::FileNotSupported, "Loader limitation: Can't read PNGs with interlacing method != 0.");

				// Skip CRC
				streamPos += sizeof(uint32_t);

				uint8_t* dataPtr = nullptr;
				uint32_t dataLength = 0;
				while (true)
				{
					uint32_t chunkLength = (fileData[streamPos + 3] << 0) | (fileData[streamPos + 2] << 8) | (fileData[streamPos + 1] << 16) | (fileData[streamPos + 0] << 24);
					streamPos += sizeof(chunkLength);

					std::array<char, 4> chunkType{};
					std::memcpy(chunkType.data(), &fileData[streamPos], sizeof(chunkType));
					streamPos += sizeof(chunkType);
					if (std::string_view(chunkType.data(), 4) == "IDAT")
					{
						dataPtr = &fileData[streamPos];
						dataLength = chunkLength;
						streamPos += chunkLength;
						// Skip CRC
						streamPos += 4;
						break;
					}

					streamPos += chunkLength;
					// Skip CRC
					streamPos += 4;
				}

				size_t pixelByteLength = 4;

				// We use + imageheight to take account into the filteringType at the start of every scanline
				std::vector<uint8_t> uncompressed(imageHeight * imageWidth * pixelByteLength + imageHeight);

				z_stream zLibDecompressJob{};
				zLibDecompressJob.next_in = (Bytef*)dataPtr;
				zLibDecompressJob.next_out = (Bytef*)uncompressed.data();
				zLibDecompressJob.avail_in = dataLength;
				zLibDecompressJob.avail_out = uInt(uncompressed.size());

				auto test = inflateInit(&zLibDecompressJob);

				constexpr auto ok = Z_OK;
				assert(test == ok);
				auto err = inflate(&zLibDecompressJob, Z_FINISH);
				constexpr auto streamend = Z_STREAM_END;
				assert(err == Z_STREAM_END);
				std::string errorMessage = zError(err);
				inflateEnd(&zLibDecompressJob);


				// UNFILTERING

				std::vector<uint8_t> unfiltered(imageHeight * imageWidth * pixelByteLength);

				// Unfilter first row
				const uint8_t filterType = uncompressed[0];
				if (filterType == 0)
					std::memcpy(&unfiltered[0], &uncompressed[1], imageWidth * pixelByteLength);
				else if (filterType == 1)
				{
					std::memcpy(&unfiltered[0], &uncompressed[1], pixelByteLength);
					for (size_t widthByte = pixelByteLength; widthByte < imageWidth * pixelByteLength; widthByte++)
					{
						const uint8_t filterX = uncompressed[widthByte + 1];
						const uint8_t reconA = unfiltered[widthByte - pixelByteLength];
						unfiltered[widthByte] = filterX + reconA;
					}
				}
				else if (filterType == 2)
				{
					for (size_t widthByte = 0; widthByte < imageWidth * pixelByteLength; widthByte++)
					{
						const uint8_t filterX = uncompressed[1 + widthByte];
						const uint8_t reconB = 0;
						unfiltered[0 + widthByte] = filterX + reconB;
					}
				}
				else if (filterType == 3)
				{
					// NOT WORKING I THINK
					for (size_t widthByte = 0; widthByte < imageWidth * pixelByteLength; widthByte++)
					{
						const uint8_t filterX = uncompressed[1 + widthByte];
						const uint8_t reconA = unfiltered[widthByte - pixelByteLength];
						const uint8_t reconB = 0;
						unfiltered[0 + widthByte] = filterX + uint8_t(std::floor((reconA + reconB) / float(2)));
					}
				}
				else
					return ReturnType(ResultInfo::FileNotSupported, "");
				for (size_t y = 1; y < imageHeight; y++)
				{
					const size_t filterTypeIndex = y * imageWidth * pixelByteLength + y;
					const uint8_t filterType = uncompressed[filterTypeIndex];

					const size_t uncompressedRow = filterTypeIndex + 1;
					const size_t unfilteredRow = filterTypeIndex - y;

					// Copy all the pixels in the scanline
					if (filterType == 0)
						std::memcpy(&unfiltered[unfilteredRow], &uncompressed[uncompressedRow], imageWidth * pixelByteLength);
					else if (filterType == 1)
					{
						std::memcpy(&unfiltered[unfilteredRow], &uncompressed[uncompressedRow], pixelByteLength);
						for (size_t widthByte = pixelByteLength; widthByte < imageWidth * pixelByteLength; widthByte++)
						{
							const uint8_t filterX = uncompressed[uncompressedRow + widthByte];
							const uint8_t reconA = unfiltered[unfilteredRow + widthByte - pixelByteLength];
							unfiltered[unfilteredRow + widthByte] = filterX + reconA;
						}
					}
					else if (filterType == 2)
					{
						for (size_t widthByte = 0; widthByte < imageWidth * pixelByteLength; widthByte++)
						{
							const uint8_t filterX = uncompressed[uncompressedRow + widthByte];
							const uint8_t reconB = unfiltered[unfilteredRow + widthByte - (imageWidth * pixelByteLength)];
							unfiltered[unfilteredRow + widthByte] = filterX + reconB;
						}
					}
					else if (filterType == 3)
					{
						for (size_t widthByte = 0; widthByte < pixelByteLength; widthByte++)
						{
							const uint8_t filterX = uncompressed[uncompressedRow + widthByte];
							const uint8_t reconA = 0;
							const uint8_t reconB = unfiltered[unfilteredRow + widthByte - (imageWidth * pixelByteLength)];
							unfiltered[unfilteredRow + widthByte] = filterX + uint8_t(std::floor((uint16_t(reconA) + uint16_t(reconB)) / float(2)));
						}

						for (size_t widthByte = pixelByteLength; widthByte < imageWidth * pixelByteLength; widthByte++)
						{
							const uint8_t filterX = uncompressed[uncompressedRow + widthByte];
							const uint8_t reconA = unfiltered[unfilteredRow + widthByte - pixelByteLength];
							const uint8_t reconB = unfiltered[unfilteredRow + widthByte - (imageWidth * pixelByteLength)];
							unfiltered[unfilteredRow + widthByte] = filterX + uint8_t(std::floor((uint16_t(reconA) + uint16_t(reconB)) / float(2)));
						}
					}
					else if (filterType == 4)
					{
						for (size_t widthByte = 0; widthByte < pixelByteLength; widthByte++)
						{
							const uint8_t filterX = uncompressed[uncompressedRow + widthByte];
							const size_t currentByteIndex = unfilteredRow + widthByte;
							const uint8_t reconA = 0;
							const uint8_t reconB = unfiltered[currentByteIndex - (imageWidth * pixelByteLength)];
							const uint8_t reconC = 0;
							unfiltered[unfilteredRow + widthByte] = filterX + PaethPredictor(reconA, reconB, reconC);
						}

						for (size_t widthByte = pixelByteLength; widthByte < imageWidth * pixelByteLength; widthByte++)
						{
							const uint8_t filterX = uncompressed[uncompressedRow + widthByte];
							const size_t currentByteIndex = unfilteredRow + widthByte;
							const uint8_t reconA = unfiltered[currentByteIndex - pixelByteLength];
							const uint8_t reconB = unfiltered[currentByteIndex - (imageWidth * pixelByteLength)];
							const uint8_t reconC = unfiltered[currentByteIndex - (imageWidth * pixelByteLength) - pixelByteLength];
							unfiltered[unfilteredRow + widthByte] = filterX + PaethPredictor(reconA, reconB, reconC);
						}
					}
					else
						return ReturnType(ResultInfo::FileNotSupported, "Library limitiation: Can't decode PNG scanline filterType " + std::to_string(filterType));
				}

				TexDoc::CreateInfo createInfo;
				createInfo.byteArray = std::move(unfiltered);
				createInfo.baseDimensions = { imageWidth, imageHeight, 1 };
				createInfo.pixelFormat = PixelFormat::R8G8B8A8_UNorm;
				createInfo.mipMapDataInfo[0].offset = 0;
				createInfo.mipMapDataInfo[0].byteLength = createInfo.byteArray.size();

				createInfo.textureType = TextureType::Texture2D;

				createInfo.isCompressed = false;
				createInfo.mipLevels = 1;
				createInfo.arrayLayers = 1;

				return ReturnType(TexDoc(std::move(createInfo)));
			}

			uint8_t PaethPredictor(uint8_t a, uint8_t b, uint8_t c)
			{
				const int16_t p = a + b - c;
				const int16_t pa = p > a ? (p - a) : (a - p);
				const int16_t pb = p > b ? (p - b) : (b - p);
				const int16_t pc = p > c ? (p - c) : (c - p);
				return int16_t((pa <= pb && pa <= pc) ? a : pb <= pc ? b : c);
			}
		}
	}
}