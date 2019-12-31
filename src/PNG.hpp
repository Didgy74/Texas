#pragma once

#include "Texas/TextureDocument.hpp"
#include "Texas/ResultType.hpp"

#include <fstream>
#include <filesystem>
#include <string_view>

namespace Texas::detail::PNG
{
	using ChunkSize_T = uint32_t;
	using ChunkType_T = uint8_t[4];
	using ChunkCRC_T = uint8_t[4];

	struct Header
	{
		static constexpr ChunkType_T IHDR_ChunkTypeValue = { 73, 72, 68, 82 };
		static constexpr ChunkType_T PLTE_ChunkTypeValue = { 80, 76, 84, 69 };
		static constexpr ChunkType_T IDAT_ChunkTypeValue = { 73, 68, 65, 84 };
		static constexpr ChunkType_T IEND_ChunkTypeValue = { 73, 69, 78, 68 };

		static constexpr ChunkType_T SRGB_ChunkTypeValue = { 115, 82, 71, 66 };

		static constexpr size_t totalSize = sizeof(uint8_t) * 33;

		static constexpr uint32_t identifierSize = 8;
		using Identifier_T = uint8_t[identifierSize];
		static constexpr Identifier_T identifier = { 137, 80, 78, 71, 13, 10, 26, 10 };
		static constexpr size_t identifier_Offset = 0;

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

	bool LoadHeader_Backend(MetaData& metaData, std::ifstream& fstream, ResultType& ResultType, std::string_view& errorMessage);
}