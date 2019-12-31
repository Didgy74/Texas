#pragma once

#include "Texas/LoadResult.hpp"
#include "Texas/OpenBuffer.hpp"

#include <cstddef>

namespace Texas::detail
{
	class PrivateAccessor final
	{
	private:
		PrivateAccessor() = delete;
		virtual ~PrivateAccessor() = 0;

	public:
		static LoadResult<OpenBuffer> loadFromBuffer_Deferred(const void* fileBuffer, std::size_t bufferLength);

		static Result LoadImageData(const OpenBuffer& file, std::uint8_t* dstBuffer, std::uint8_t* workingMemory);

	};
}