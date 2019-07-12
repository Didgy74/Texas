#pragma once

#include "DTex/MetaData.hpp"
#include "DTex/OpenFile.hpp"

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
		static LoadResult<TextureDocument> LoadFromFile(const std::filesystem::path& path);

		static LoadResult<OpenFile> LoadFile_CustomBuffer(const std::filesystem::path& path);
		static void LoadFromOpenFile(const OpenFile& file, uint8_t* dstBuffer);

		static void KTX_LoadImageData_CustomBuffer(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer);

		static void PNG_LoadImageData_CustomBuffer(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer);
	};
}