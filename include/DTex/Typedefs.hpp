#pragma once

#include <cstdint>

namespace DTex
{
	enum class ResultInfo : uint8_t;

	struct Dimensions;
	struct Dimensions2D;

	class TextureDocument;
	using TexDoc = TextureDocument;

	class OpenFile;

	template<typename T>
	class LoadResult;

	namespace detail
	{
		class PrivateAccessor;
	}
}

enum class DTex::ResultInfo : uint8_t
{
	Success,
	CouldNotReadFile,
	CorruptFileData,
	FileNotSupported,
	PixelFormatNotSupported,
};