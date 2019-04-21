#pragma once

#include <optional>
#include <filesystem>

#include "LoadResult.hpp"
#include "TextureDocument.hpp"

namespace DTex
{
	LoadResult<TextureDocument> LoadFromFile(std::filesystem::path path);
}
