#pragma once

#include "../LoadResult.hpp"
#include "../TextureDocument.hpp"

#include <fstream>
#include <array>
#include <cstdint>

namespace DTex
{
	namespace detail
	{
		namespace PNG
		{
			constexpr std::array<uint8_t, 8> identifier = { 137, 80, 78, 71, 13, 10, 26, 10 };

			struct Chunk
			{
				uint32_t length;

			};

			inline LoadResult<TextureDocument> LoadPNG(std::ifstream file)
			{
				using ReturnType = LoadResult<TextureDocument>;

				std::array<uint8_t, 8> fileIdentifier{};
				file.read(reinterpret_cast<char*>(fileIdentifier.data()), fileIdentifier.size());
				if (fileIdentifier != identifier)
					return ReturnType{ ResultInfo::CorruptFileData };

				uint32_t chunkLength{};
				file.read(reinterpret_cast<char*>(&chunkLength), sizeof(uint32_t));

				std::array<uint8_t, 4> chunkType{};
				file.read(reinterpret_cast<char*>(chunkType.data()), sizeof(chunkType));
				if (chunkType)

				return ReturnType(ResultInfo::CouldNotReadFile);
			}
		}
	}
}