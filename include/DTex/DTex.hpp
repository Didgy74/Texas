#pragma once

#include "DTex/Typedefs.hpp"
#include "DTex/LoadResult.hpp"
#include "DTex/TextureDocument.hpp"
#include "DTex/Dimensions.hpp"

#include <filesystem>

namespace DTex
{
	LoadResult<TextureDocument> LoadFromFile(std::filesystem::path path);

	constexpr TextureType ToTextureType(Dimensions dimensions, uint32_t arrayLayers);
}

constexpr DTex::TextureType DTex::ToTextureType(Dimensions dimensions, uint32_t arrayLayers)
{
	using T = TextureType;

	if (dimensions[1] == 0 && dimensions[2] == 0)
		return T::Texture1D;
	else if (dimensions[2] == 0)
		return T::Texture2D;
	else if (dimensions[2] != 0)
		return T::Texture3D;

	return T::Invalid;
}
