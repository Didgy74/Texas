#pragma once

#include "DTex/LoadResult.hpp"
#include "DTex/TextureDocument.hpp"

#include <fstream>
#include <filesystem>

namespace DTex::detail::PNG
{
	using ChunkSize_T = uint32_t;
	using ChunkType_T = uint8_t[4];
	using ChunkCRC_T = uint8_t[4];

	struct Header
	{
		static constexpr size_t totalSize = sizeof(uint8_t) * 33;

		static constexpr uint32_t identifierSize = 8;
		using Identifier_T = uint8_t[identifierSize];
		static constexpr Identifier_T identifier = { 137, 80, 78, 71, 13, 10, 26, 10 };
		static constexpr size_t identifierOffset = 0;

		static constexpr size_t ihdrChunkSizeOffset = identifierSize;

		static constexpr size_t ihdrChunkTypeOffset = ihdrChunkSizeOffset + sizeof(ChunkSize_T);

		using Dimension_T = uint32_t;
		static constexpr size_t widthOffset = ihdrChunkTypeOffset + sizeof(ChunkType_T);

		static constexpr size_t heightOffset = widthOffset + sizeof(Dimension_T);

		static constexpr size_t bitDepthOffset = heightOffset + sizeof(Dimension_T);

		static constexpr size_t colorTypeOffset = bitDepthOffset + sizeof(uint8_t);

		static constexpr size_t compressionMethodOffset = colorTypeOffset + sizeof(uint8_t);

		static constexpr size_t filterMethodOffset = compressionMethodOffset + sizeof(uint8_t);

		static constexpr size_t interlaceMethodOffset = filterMethodOffset + sizeof(uint8_t);

	};

	LoadResult<TextureDocument> LoadPNG(std::filesystem::path path);

	bool LoadHeader_Backend(MetaData& metaData, std::ifstream& fstream, ResultInfo& resultInfo, std::string& errorMessage);
}