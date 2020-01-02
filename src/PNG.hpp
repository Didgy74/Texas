#pragma once

#include "Texas/ResultType.hpp"
#include "Texas/Pair.hpp"
#include "Texas/MetaData.hpp"
#include "Texas/Span.hpp"

#include <cstdint>

namespace Texas::detail::PNG
{
	enum class ColorType : std::uint8_t;

	using ChunkSize_T = std::uint32_t;
	using ChunkType_T = std::uint8_t[4];
	using ChunkCRC_T = std::uint8_t[4];

	namespace Header
	{
		constexpr std::uint8_t IHDR_ChunkTypeValue[4] = { 73, 72, 68, 82 };
		constexpr std::uint8_t PLTE_ChunkTypeValue[4] = { 80, 76, 84, 69 };
		constexpr std::uint8_t IDAT_ChunkTypeValue[4] = { 73, 68, 65, 84 };
		constexpr std::uint8_t IEND_ChunkTypeValue[4] = { 73, 69, 78, 68 };
		constexpr std::uint8_t sRGB_ChunkTypeValue[4] = { 115, 82, 71, 66 };

		constexpr std::uint32_t identifierSize = 8;
		using Identifier_T = std::uint8_t[identifierSize];
		constexpr Identifier_T identifier = { 137, 80, 78, 71, 13, 10, 26, 10 };
		constexpr std::size_t identifier_Offset = 0;

		constexpr std::size_t ihdrChunkSizeOffset = identifierSize;
		constexpr std::size_t ihdrChunkTypeOffset = ihdrChunkSizeOffset + sizeof(ChunkSize_T);
		constexpr std::size_t ihdrChunkDataSize = sizeof(std::uint8_t) * 13;

		using Dimension_T = std::uint32_t;
		constexpr std::size_t widthOffset = ihdrChunkTypeOffset + sizeof(ChunkType_T);

		constexpr std::size_t heightOffset = widthOffset + sizeof(Dimension_T);

		constexpr std::size_t bitDepthOffset = heightOffset + sizeof(Dimension_T);

		constexpr std::size_t colorTypeOffset = bitDepthOffset + sizeof(std::uint8_t);

		constexpr std::size_t compressionMethodOffset = colorTypeOffset + sizeof(std::uint8_t);

		constexpr std::size_t filterMethodOffset = compressionMethodOffset + sizeof(std::uint8_t);

		constexpr std::size_t interlaceMethodOffset = filterMethodOffset + sizeof(std::uint8_t);

		constexpr std::size_t interlaceMethodFieldSize = sizeof(std::uint8_t);

		// Total size of all header data.
		constexpr std::size_t totalSize = interlaceMethodOffset + interlaceMethodFieldSize + sizeof(ChunkCRC_T);
	};

	Pair<ResultType, const char*> loadFromBuffer_Step1(
		const bool fileIdentifierConfirmed, 
		ConstByteSpan srcBuffer,
		MetaData& metaData);
}

enum class Texas::detail::PNG::ColorType : std::uint8_t
{
	Greyscale = 0,
	Truecolour = 2,
	Indexed_colour = 3,
	Greyscale_with_alpha = 4,
	Truecolour_with_alpha = 6
};