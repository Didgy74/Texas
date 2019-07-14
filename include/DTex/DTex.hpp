#pragma once

#include "DTex/LoadInfo.hpp"
#include "DTex/TextureDocument.hpp"
#include "DTex/OpenFile.hpp"
#include "DTex/MetaData.hpp"

#include <filesystem>

namespace DTex
{
	/*
		Loads an entire image file. Both metadata and imagedata is loaded.
	*/
	LoadInfo<TexDoc> LoadFromFile(const std::filesystem::path& path);

	/*
		Loads the metadata from an image file.

		The returned struct can be used to allocate a buffer large enough to load the imagedata onto.
		This can be done with LoadImageData.
	*/
	LoadInfo<OpenFile> LoadFromFile_Deferred(const std::filesystem::path& path);

	/*
		Loads the imagedata of a file opened with LoadFromFile_Deferred into dstBuffer.

		Note! The size of dstBuffer MUST BE ATLEAST the size returned by OpenFile::GetTotalSizeRequired().
	*/
	void LoadImageData(const OpenFile& file, uint8_t* dstBuffer);
}