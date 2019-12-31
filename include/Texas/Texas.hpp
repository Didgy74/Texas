#pragma once

#include "Texas/LoadResult.hpp"
#include "Texas/Result.hpp"
#include "Texas/TextureDocument.hpp"
#include "Texas/OpenFile.hpp"
#include "Texas/OpenBuffer.hpp"

namespace Texas
{
	/*
		Loads the metadata from an image buffer.

		The returned struct can be used to allocate a buffer large enough to load the imagedata onto.
		This can be done with Texas::LoadImageData.
	*/
	[[nodiscard]] LoadResult<OpenBuffer> loadFromBuffer_Deferred(const void* fileBuffer, std::size_t bufferLength);

	/*
		Loads the imagedata of a file opened with LoadFromFile_Deferred into dstBuffer.

		Requirements:

			dstBuffer memory MUST be ATLEAST the size returned by OpenBuffer.memoryRequired().

			workingMemory MUST be ATLEAST the size return by OpenBuffer.workingMemoryRequired().
	*/
	[[nodiscard]] Result LoadImageData(const OpenBuffer& file, std::uint8_t* dstBuffer, std::uint8_t* workingMemory);

	/*
		Loads an entire image file. Both metadata and imagedata is loaded.
	*/
	[[nodiscard]] LoadResult<TexDoc> LoadFromFile(const char* path);

	/*
		Loads the metadata from an image file.

		The returned struct can be used to allocate a buffer large enough to load the imagedata onto.
		This can be done with LoadImageData.
	*/
	[[nodiscard]] LoadResult<OpenFile> LoadFromFile_Deferred(const char* path);

	/*
		Loads the imagedata of a file opened with LoadFromFile_Deferred into dstBuffer.

		Note! The size of dstBuffer MUST BE ATLEAST the size returned by OpenFile::GetTotalSizeRequired().
	*/
	void LoadImageData(const OpenFile& file, uint8_t* dstBuffer);
}