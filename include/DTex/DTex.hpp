#pragma once

#include "DTex/LoadInfo.hpp"
#include "DTex/TextureDocument.hpp"
#include "DTex/OpenFile.hpp"
#include "DTex/MetaData.hpp"

#include <filesystem>

namespace DTex
{
	LoadInfo<TexDoc> LoadFromFile(const std::filesystem::path& path);

	LoadInfo<OpenFile> LoadFromFile_Deferred(const std::filesystem::path& path);

	void LoadImageData(const OpenFile& file, uint8_t* dstBuffer);
}