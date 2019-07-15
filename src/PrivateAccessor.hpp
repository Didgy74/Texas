#pragma once

#include "DTex/LoadInfo.hpp"
#include "DTex/MetaData.hpp"
#include "DTex/OpenFile.hpp"
#include <DTex/TextureDocument.hpp>

#include <cstddef>
#include <filesystem>

namespace DTex::detail
{
	class PrivateAccessor final
	{
	private:
		PrivateAccessor() = delete;
		virtual ~PrivateAccessor() = 0;

	public:
		static LoadInfo<TextureDocument> LoadFromFile(const std::filesystem::path& path);

		static LoadInfo<OpenFile> LoadFromFile_Deferred(const std::filesystem::path& path);

		static bool LoadImageData(const OpenFile& openFile, uint8_t* const dstBuffer);

		static bool LoadImageData(const MetaData& file, std::ifstream& fstream, uint8_t* const dstBuffer);

		static bool KTX_LoadImageData(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer);

		static bool PNG_LoadImageData(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer);
	};
}