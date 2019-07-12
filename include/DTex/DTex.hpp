#pragma once

#include "DTex/Typedefs.hpp"
#include "DTex/LoadResult.hpp"
#include "DTex/TextureDocument.hpp"
#include "DTex/OpenFile.hpp"
#include "DTex/Dimensions.hpp"
#include "DTex/MetaData.hpp"

#include <filesystem>

namespace DTex
{
	LoadResult<TextureDocument> LoadFromFile(std::filesystem::path path);

	LoadResult<OpenFile> LoadFromFile_CustomBuffer(std::filesystem::path path);

	void LoadFromOpenFile(const OpenFile& file, uint8_t* dstBuffer);
}