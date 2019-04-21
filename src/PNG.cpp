#pragma once

#include "detail/PNG.hpp"

#include "zlib/zlib.h"

namespace DTex
{
	namespace detail
	{
		namespace PNG
		{
			constexpr uint8_t GetPixelWidth(uint8_t colorType, uint8_t bitDepth);
			bool DecompressIDATs(std::vector<uint8_t>& fileStream, size_t& streamPos, std::vector<uint8_t>& uncompressedData);
			uint8_t PaethPredictor(uint8_t a, uint8_t b, uint8_t c);

			constexpr PixelFormat ToPixelFormat(uint8_t colorType, uint8_t bitDepth);

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
				
				size_t restOfFileSize = size_t(imageDataEnd - imageDataStart);
				if (restOfFileSize <= PNG::IHDRChunkSize)
					return ReturnType(ResultInfo::CorruptFileData, "Reached premature end-of-file when loading header data.");
				std::vector<uint8_t> fileData(restOfFileSize);

				file.seekg(imageDataStart);
				file.read(reinterpret_cast<char*>(fileData.data()), fileData.size());
				file.close();

				size_t streamPos = 0;

				// Skip chunk length and chunk type
				streamPos += sizeof(uint32_t) * 2;

				uint32_t imageWidth = (fileData[streamPos + 3] << 0) | (fileData[streamPos + 2] << 8) | (fileData[streamPos + 1] << 16) | (fileData[streamPos + 0] << 24);
				streamPos += sizeof(imageWidth);

				uint32_t imageHeight = (fileData[streamPos + 3] << 0) | (fileData[streamPos + 2] << 8) | (fileData[streamPos + 1] << 16) | (fileData[streamPos + 0] << 24);
				streamPos += sizeof(imageHeight);

				const uint8_t& bitDepth = fileData[streamPos];
				streamPos += sizeof(uint8_t);
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

				size_t pixelWidth = PNG::GetPixelWidth(colorType, bitDepth);
				if (pixelWidth == 0)
					return ReturnType(ResultInfo::CorruptFileData, "PNG has invalid combination of color-type and bit-depth.");

				// We use +imageheight to take account into the filteringType at the start of every scanline
				std::vector<uint8_t> uncompressed(imageHeight * imageWidth * pixelWidth + imageHeight);

				while (streamPos <= fileData.size() - PNG::minimumChunkSize)
				{
					uint32_t chunkLength = (fileData[streamPos + 3] << 0) | (fileData[streamPos + 2] << 8) | (fileData[streamPos + 1] << 16) | (fileData[streamPos + 0] << 24);
					streamPos += sizeof(chunkLength);

					if (std::string_view(reinterpret_cast<char*>(&fileData[streamPos]), 4) == "IDAT")
					{
						// Process IDAT chunks
						streamPos -= sizeof(chunkLength);
						bool success = DecompressIDATs(fileData, streamPos, uncompressed);
						if (success == false)
							return ReturnType(ResultInfo::CorruptFileData, "Could not decompress PNG filestream.");
					}
					else
					{
						// Skip chunk type
						streamPos += 4;

						streamPos += chunkLength;
						// Skip CRC
						streamPos += 4;
					}
				}

				// UNFILTERING

				// Unfilter first row
				const uint8_t filterType = uncompressed[0];
				if (filterType == 0)
					std::copy(&uncompressed[1], &uncompressed[1 + imageWidth * pixelWidth], &uncompressed[0]);
				else if (filterType == 1)
				{
					std::copy(&uncompressed[1], &uncompressed[1 + pixelWidth], &uncompressed[0]);
					for (size_t widthByte = pixelWidth; widthByte < imageWidth * pixelWidth; widthByte++)
					{
						const uint8_t filterX = uncompressed[widthByte + 1];
						const uint8_t reconA = uncompressed[widthByte - pixelWidth];
						uncompressed[widthByte] = filterX + reconA;
					}
				}
				else if (filterType == 2)
				{
					std::copy(&uncompressed[1], &uncompressed[1 + imageWidth * pixelWidth], &uncompressed[0]);
				}
				else if (filterType == 3)
				{
					std::copy(&uncompressed[1], &uncompressed[1 + imageWidth * pixelWidth], &uncompressed[0]);

					for (size_t widthByte = pixelWidth; widthByte < imageWidth * pixelWidth; widthByte++)
					{
						const uint8_t filterX = uncompressed[1 + widthByte];
						const uint8_t reconA = uncompressed[widthByte - pixelWidth];
						uncompressed[widthByte] = filterX + uint8_t((uint16_t(reconA)) / float(2));
					}
				}
				else
					return ReturnType(ResultInfo::FileNotSupported, "");

				for (size_t y = 1; y < imageHeight; y++)
				{
					const size_t filterTypeIndex = y * imageWidth * pixelWidth + y;
					const uint8_t filterType = uncompressed[filterTypeIndex];

					const size_t uncompRow = filterTypeIndex + 1;
					const size_t unfiltRow = filterTypeIndex - y;

					// Copy all the pixels in the scanline
					if (filterType == 0)
						std::copy(&uncompressed[uncompRow], &uncompressed[uncompRow + imageWidth * pixelWidth], &uncompressed[unfiltRow]);
					else if (filterType == 1)
					{
						std::copy(&uncompressed[uncompRow], &uncompressed[uncompRow + pixelWidth], &uncompressed[unfiltRow]);

						for (size_t xByte = pixelWidth; xByte < imageWidth * pixelWidth; xByte++)
						{
							const uint8_t filterX = uncompressed[uncompRow + xByte];
							const uint8_t reconA = uncompressed[unfiltRow + xByte - pixelWidth];
							uncompressed[unfiltRow + xByte] = filterX + reconA;
						}
					}
					else if (filterType == 2)
					{
						for (size_t xByte = 0; xByte < imageWidth * pixelWidth; xByte++)
						{
							const uint8_t filterX = uncompressed[uncompRow + xByte];
							const uint8_t reconB = uncompressed[unfiltRow + xByte - (imageWidth * pixelWidth)];
							uncompressed[unfiltRow + xByte] = filterX + reconB;
						}
					}
					else if (filterType == 3)
					{
						for (size_t widthByte = 0; widthByte < pixelWidth; widthByte++)
						{
							const uint8_t filterX = uncompressed[uncompRow + widthByte];
							const uint8_t reconB = uncompressed[unfiltRow + widthByte - (imageWidth * pixelWidth)];
							uncompressed[unfiltRow + widthByte] = filterX + uint8_t((uint16_t(reconB)) / float(2));
						}

						for (size_t widthByte = pixelWidth; widthByte < imageWidth * pixelWidth; widthByte++)
						{
							const uint8_t filterX = uncompressed[uncompRow + widthByte];
							const uint8_t reconA = uncompressed[unfiltRow + widthByte - pixelWidth];
							const uint8_t reconB = uncompressed[unfiltRow + widthByte - (imageWidth * pixelWidth)];
							uncompressed[unfiltRow + widthByte] = filterX + uint8_t((uint16_t(reconA) + uint16_t(reconB)) / float(2));
						}
					}
					else if (filterType == 4)
					{
						for (size_t xByte = 0; xByte < pixelWidth; xByte++)
						{
							const uint8_t filterX = uncompressed[uncompRow + xByte];
							const uint8_t reconB = uncompressed[unfiltRow + xByte - (imageWidth * pixelWidth)];
							uncompressed[unfiltRow + xByte] = filterX + reconB;
						}

						for (size_t xByte = pixelWidth; xByte < imageWidth * pixelWidth; xByte++)
						{
							const uint8_t filterX = uncompressed[uncompRow + xByte];
							const uint8_t reconA = uncompressed[unfiltRow + xByte - pixelWidth];
							const uint8_t reconB = uncompressed[unfiltRow + xByte - (imageWidth * pixelWidth)];
							const uint8_t reconC = uncompressed[unfiltRow + xByte - (imageWidth * pixelWidth) - pixelWidth];
							uncompressed[unfiltRow + xByte] = filterX + PaethPredictor(reconA, reconB, reconC);
						}
					}
					else
						return ReturnType(ResultInfo::FileNotSupported, "Library limitiation: Can't decode PNG scanline filterType " + std::to_string(filterType));
				}

				uncompressed.resize(imageWidth * imageHeight * pixelWidth);

				TexDoc::CreateInfo createInfo;
				createInfo.byteArray = std::move(uncompressed);
				createInfo.baseDimensions = { imageWidth, imageHeight, 1 };
				createInfo.pixelFormat = ToPixelFormat(colorType, bitDepth);
				if (createInfo.pixelFormat == PixelFormat::Invalid)
					return ReturnType(ResultInfo::PixelFormatNotSupported, "Loader limitation: Can't load PNG of colorType " + std::to_string(colorType) + "and bitdepth " + std::to_string(bitDepth));
				createInfo.mipMapDataInfo[0].offset = 0;
				createInfo.mipMapDataInfo[0].byteLength = createInfo.byteArray.size();

				createInfo.textureType = TextureType::Texture2D;

				createInfo.isCompressed = false;
				createInfo.mipLevels = 1;
				createInfo.arrayLayers = 1;

				return ReturnType(TexDoc(std::move(createInfo)));
			}

			constexpr uint8_t GetPixelWidth(uint8_t colorType, uint8_t bitDepth)
			{
				constexpr auto greyscale = 0;
				constexpr auto truecolor = 2;
				constexpr auto indexedColor = 3;
				constexpr auto greyscaleWithAlpha = 4;
				constexpr auto trueColorWithAlpha = 6;

				switch (colorType)
				{
				case greyscale:
					switch (bitDepth)
					{
					case 1:
					case 2:
					case 4:
					case 8:
						return sizeof(uint8_t);
					case 16:
						return sizeof(uint16_t);
					}
				case truecolor:
					switch (bitDepth)
					{
					case 8:
						return sizeof(uint8_t) * 3;
					case 16:
						return sizeof(uint16_t) * 3;
					}
				case indexedColor:
					switch (bitDepth)
					{
					case 1:
					case 2:
					case 4:
					case 8:
						return sizeof(uint8_t);
					}
				case greyscaleWithAlpha:
					switch (bitDepth)
					{
					case 8:
						return sizeof(uint8_t) * 2;
					case 16:
						return sizeof(uint16_t) * 2;
					}
				case trueColorWithAlpha:
					switch (bitDepth)
					{
					case 8:
						return sizeof(uint8_t) * 4;
					case 16:
						return sizeof(uint16_t) * 4;
					}
				default:
					return 0;
				}
			}

			bool DecompressIDATs(std::vector<uint8_t>& fileStream, size_t& streamPos, std::vector<uint8_t>& uncompressedData)
			{
				z_stream zLibDecompressJob{};
				zLibDecompressJob.next_out = (Bytef*)uncompressedData.data();
				zLibDecompressJob.avail_out = (uInt)uncompressedData.size();

				auto initErr = inflateInit(&zLibDecompressJob);
				if (initErr != Z_OK)
				{
					inflateEnd(&zLibDecompressJob);
					return false;
				}

				while (streamPos <= fileStream.size() - PNG::minimumChunkSize)
				{
					uint32_t chunkLength = (fileStream[streamPos + 3] << 0) | (fileStream[streamPos + 2] << 8) | (fileStream[streamPos + 1] << 16) | (fileStream[streamPos + 0] << 24);
					streamPos += sizeof(chunkLength);

					if (std::string_view(reinterpret_cast<const char*>(&fileStream[streamPos]), sizeof(uint32_t)) != "IDAT")
					{
						inflateEnd(&zLibDecompressJob);
						return false;
					}
					// Skip length of the chunkType field.
					streamPos += sizeof(uint32_t);

					zLibDecompressJob.next_in = (Bytef*)&fileStream[streamPos];
					zLibDecompressJob.avail_in = (uInt)chunkLength;
					
					auto err = inflate(&zLibDecompressJob, 0);
					if (err == Z_STREAM_END)
					{
						streamPos += chunkLength;
						// Skip CRC
						streamPos += sizeof(uint32_t);

						inflateEnd(&zLibDecompressJob);
						return true;
					}

					streamPos += chunkLength;
					// Skip CRC
					streamPos += sizeof(uint32_t);
				}

				return false;
			}

			uint8_t PaethPredictor(uint8_t a, uint8_t b, uint8_t c)
			{
				const int16_t p = a + b - c;

				const int16_t pa = std::abs(p - a);
				const int16_t pb = std::abs(p - b);
				const int16_t pc = std::abs(p - c);

				if (pa <= pb && pa <= pc)
					return a;
				else
				{
					if (pb <= pc)
						return b;
					else
						return c;
				}
			}

			constexpr PixelFormat ToPixelFormat(uint8_t colorType, uint8_t bitDepth)
			{
				using F = PixelFormat;

				constexpr auto greyscale = 0;
				constexpr auto truecolor = 2;
				constexpr auto indexedColor = 3;
				constexpr auto greyscaleWithAlpha = 4;
				constexpr auto trueColorWithAlpha = 6;

				switch (colorType)
				{
				case truecolor:
					switch (bitDepth)
					{
					case 8:
						return F::R8G8B8_UNorm;
					}
				case trueColorWithAlpha:
					switch (bitDepth)
					{
					case 8:
						return F::R8G8B8A8_UNorm;
					}
				default:
					return F::Invalid;
				}
			}
		}
	}
}