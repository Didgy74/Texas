#pragma once

#include "Texas/ResultType.hpp"
#include "Texas/Result.hpp"
#include "Texas/MetaData.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/OpenBuffer.hpp"

#include <cstdint>

namespace Texas::detail::PNG
{
	namespace Header
	{
		constexpr std::uint32_t identifierSize = 8;
		using Identifier_T = std::uint8_t[identifierSize];
		constexpr Identifier_T identifier = { 137, 80, 78, 71, 13, 10, 26, 10 };
		constexpr std::size_t identifier_Offset = 0;
	}

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