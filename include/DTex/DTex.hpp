#pragma once

#include <optional>
#include <string_view>
#include <variant>

#include "detail/KTX.hpp"
#include "detail/PNG.hpp"

#include "GLFormats.hpp"

#include "LoadResult.hpp"

#include "TextureDocument.hpp"

namespace DTex
{
	inline LoadResult<TextureDocument> LoadFromFile(std::string_view path)
	{
		using ReturnType = LoadResult<TextureDocument>;

		std::ifstream file(path.data(), std::ios::binary);

		if (!file.is_open())
			return ReturnType{ ResultInfo::CouldNotReadFile };

		// Check file extension
		std::string extension;
		constexpr char yo = '.';
		size_t extensionStartIndex = path.find_first_of(yo, 0);
		if (extensionStartIndex != std::string_view::npos)
			extension = path.substr(extensionStartIndex + 1);

		if (extension == "ktx")
			return detail::KTX::LoadKTX(std::move(file));
		else if (extension == "png")
			return detail::PNG::LoadPNG(std::move(file));
		else
			return ReturnType( ResultInfo::FileNotSupported );
	}
}