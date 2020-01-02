#pragma once

#include "Texas/LoadResult.hpp"
#include "Texas/Result.hpp"
#include "Texas/Span.hpp"
#include "Texas/TextureDocument.hpp"
#include "Texas/OpenFile.hpp"
#include "Texas/OpenBuffer.hpp"

namespace Texas
{
	/*
		Loads the metadata from an image buffer.

		The returned struct can be used to allocate a buffer large enough to load the imagedata onto.
		This can be done with Texas::loadImageData().
	*/
	[[nodiscard]] LoadResult<OpenBuffer> loadFromBuffer(const void* fileBuffer, std::size_t bufferLength);

	/*
		Loads the metadata from an image buffer.

		The returned struct can be used to allocate a buffer large enough to load the imagedata onto.
		This can be done with Texas::loadImageData().
	*/
	[[nodiscard]] LoadResult<OpenBuffer> loadFromBuffer(ConstByteSpan inputBuffer);

	/*
		Loads the imagedata of a file opened with LoadFromBuffer into dstBuffer.

		Requirements:

			dstBuffer MUST be ATLEAST the size returned by OpenBuffer::memoryRequired().

			workingMemory MUST be ATLEAST the size return by OpenBuffer::memoryRequired().
			If the size returned by OpenBuffer::workingMemoryRequired is 0, you can pass a default-constructed ByteSpan, or one that points to nullptr.
	*/
	[[nodiscard]] Result loadImageData(const OpenBuffer& file, ByteSpan dstBuffer, ByteSpan workingMemory);

	/*
		Loads the imagedata of a file opened with LoadFromBuffer into dstBuffer.

		Requirements:

			dstBuffer MUST be ATLEAST the size returned by OpenBuffer::memoryRequired().

			workingMemory MUST be ATLEAST the size return by OpenBuffer::workingMemoryRequired().
			If the size returned by OpenBuffer::workingMemoryRequired() is 0, you can pass nullptr and 0
	*/
	[[nodiscard]] Result loadImageData(const OpenBuffer& file, void* dstBuffer, std::size_t dstBufferSize, void* workingMemory, std::size_t workingMemorySize);
}