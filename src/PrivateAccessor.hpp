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

		static LoadInfo<OpenFile> LoadFile_CustomBuffer(const std::filesystem::path& path);
		static void LoadImageData(const OpenFile& file, uint8_t* dstBuffer);

		static void KTX_LoadImageData(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer);

		static void PNG_LoadImageData(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer);
	};
}