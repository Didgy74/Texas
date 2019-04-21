#pragma once

#include <cassert>
#include <cstdint>
#include <utility>

namespace DTex
{
	enum class ResultInfo : uint32_t;	
	enum class PixelFormat : uint32_t;
	enum class TextureType : uint32_t;
	
	struct Dimensions;

	class TextureDocument;
	using TexDoc = TextureDocument;

	template<typename T>
	class LoadResult;

	inline constexpr TextureType ToTextureType(Dimensions dimensions, uint32_t arrayLayers);
}

enum class DTex::ResultInfo : uint32_t
{
	Success,
	CouldNotReadFile,
	CorruptFileData,
	FileNotSupported,
	PixelFormatNotSupported,
};

enum class DTex::TextureType : uint32_t
{
	Invalid,
	Texture1D,
	Texture2D,
	Texture3D,
};

enum class DTex::PixelFormat : uint32_t
{
	Invalid,

	// Standard
	R8G8B8_UNorm,
	R8G8B8A8_UNorm,

	// BCn
	BC1_RGB_UNorm,
	BC1_RGB_sRGB,
	BC1_RGBA_UNorm,
	BC1_RGBA_sRGB,
	BC2_UNorm,
	BC2_sRGB,
	BC3_UNorm,
	BC3_sRGB,
	BC4_UNorm,
	BC4_SNorm,
	BC5_UNorm,
	BC5_SNorm,
	BC6H_UFloat,
	BC6H_SFloat,
	BC7_UNorm,
	BC7_sRGB,
};

struct DTex::Dimensions
{
	uint32_t width;
	uint32_t height;
	uint32_t depth;

	inline constexpr uint32_t& operator[](size_t i) { return const_cast<uint32_t&>(std::as_const(*this)[i]); }
	inline constexpr const uint32_t& operator[](size_t i) const
	{
		switch (i)
		{
		case 0:
			return width;
		case 1:
			return height;
		case 2:
			return depth;
		default:
			return width;
		}
	}

	inline constexpr uint32_t& At(size_t i) { return const_cast<uint32_t&>(std::as_const(*this).At(i)); }
	inline constexpr const uint32_t& At(size_t i) const
	{
		assert(i >= 0 && i < 3);
		switch (i)
		{
		case 0:
			return width;
		case 1:
			return height;
		case 2:
			return depth;
		default:
			assert(false);
			return width;
		}
	}
};

inline constexpr DTex::TextureType DTex::ToTextureType(Dimensions dimensions, uint32_t arrayLayers)
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