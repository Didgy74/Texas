#include "DTex/DTex.hpp"

#include "KTX.hpp"
#include "PNG.hpp"

#include <fstream>
#include <filesystem>

#include "DTex/TextureDocument.hpp"

namespace DTex
{
	LoadResult<TextureDocument> LoadFromFile(std::filesystem::path path)
	{
		using ReturnType = LoadResult<TextureDocument>;

		if (path.has_extension())
		{
			auto extension = path.extension();
			if (extension == ".ktx")
				return detail::KTX::LoadKTX(path);
			else if (extension == ".png")
				return detail::PNG::LoadPNG(path);
		}
		
		return ReturnType(ResultInfo::FileNotSupported, "Loader limitation: Can't load files without an extension.");
	}
}