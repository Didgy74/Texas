#pragma once

#include "Texas/ResultType.hpp"
#include "Texas/Result.hpp"
#include "Texas/MetaData.hpp"
#include "Texas/Span.hpp"
#include "Texas/OpenBuffer.hpp"

#include <cstdint>

namespace Texas::detail::PNG
{
	enum class ColorType : std::uint8_t;
	enum class ChunkType : std::uint8_t;

	using ChunkSize_T = std::uint32_t;
	using ChunkType_T = std::uint8_t[4];
	using ChunkCRC_T = std::uint8_t[4];

	constexpr std::uint8_t IHDR_ChunkTypeValue[4] = { 73, 72, 68, 82 };
	constexpr std::uint8_t PLTE_ChunkTypeValue[4] = { 80, 76, 84, 69 };
	constexpr std::uint8_t IDAT_ChunkTypeValue[4] = { 73, 68, 65, 84 };
	constexpr std::uint8_t IEND_ChunkTypeValue[4] = { 73, 69, 78, 68 };
	constexpr std::uint8_t cHRM_ChunkTypeValue[4] = { 99, 72, 82, 77 };
	constexpr std::uint8_t gAMA_ChunkTypeValue[4] = { 103, 65, 77, 65, };
	constexpr std::uint8_t iCCP_ChunkTypeValue[4] = { 105, 67, 67, 80 };
	constexpr std::uint8_t sBIT_ChunkTypeValue[4] = { 115, 66, 73, 84 };
	constexpr std::uint8_t sRGB_ChunkTypeValue[4] = { 115, 82, 71, 66 };
	constexpr std::uint8_t bKGD_ChunkTypeValue[4] = { 98, 75, 71, 68 };
	constexpr std::uint8_t hIST_ChunkTypeValue[4] = { 104, 73, 83, 84 };
	constexpr std::uint8_t tRNS_ChunkTypeValue[4] = { 116, 82, 78, 83 };
	constexpr std::uint8_t pHYs_ChunkTypeValue[4] = { 112, 72, 89, 115 };
	constexpr std::uint8_t sPLT_ChunkTypeValue[4] = { 115, 80, 76, 84 };
	constexpr std::uint8_t tIME_ChunkTypeValue[4] = { 116, 73, 77, 69 };
	constexpr std::uint8_t iTXt_ChunkTypeValue[4] = { 105, 84, 88, 116 };
	constexpr std::uint8_t tEXt_ChunkTypeValue[4] = { 116, 69, 88, 116 };
	constexpr std::uint8_t zTXt_ChunkTypeValue[4] = { 122, 84, 88, 116 };

	namespace Header
	{
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

	Result loadFromBuffer_Step1(
		const bool fileIdentifierConfirmed, 
		ConstByteSpan srcBuffer,
		MetaData& metaData,
		OpenBuffer::PNG_BackendData& backendDataBuffer);

	Result loadFromBuffer_Step2(
		const MetaData& metaData,
		OpenBuffer::PNG_BackendData& backendDataBuffer,
		const ByteSpan dstImageBuffer,
		const ByteSpan workingMemory);
}

enum class Texas::detail::PNG::ColorType : std::uint8_t
{
	Greyscale = 0,
	Truecolour = 2,
	Indexed_colour = 3,
	Greyscale_with_alpha = 4,
	Truecolour_with_alpha = 6
};

enum class Texas::detail::PNG::ChunkType : std::uint8_t
{
	Invalid,

	// Mandatory

	IHDR,
	PLTE,
	IDAT,
	IEND,

	// Optional 

	cHRM,
	gAMA,
	iCCP,
	sBIT,
	sRGB,
	bKGD,
	hIST,
	tRNS,
	pHYs,
	sPLT,
	tIME,
	iTXt,
	tEXt,
	zTXt,

	COUNT
};