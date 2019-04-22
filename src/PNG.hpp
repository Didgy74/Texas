#pragma once

#include "DTex/LoadResult.hpp"
#include "DTex/TextureDocument.hpp"

#include <fstream>
#include <array>
#include <filesystem>

namespace DTex
{
	namespace detail
	{
		namespace PNG
		{
			constexpr std::array<uint8_t, 8> identifier = { 137, 80, 78, 71, 13, 10, 26, 10 };

			constexpr size_t minimumChunkSize = 12;

			constexpr size_t IHDRChunkSize = 13;

			LoadResult<TextureDocument> LoadPNG(std::filesystem::path path);
		}
	}
}