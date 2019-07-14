#pragma once

#include "DTex/LoadInfo.hpp"
#include "DTex/TextureDocument.hpp"
#include "DTex/OpenFile.hpp"
#include "DTex/MetaData.hpp"

#include <fstream>
#include <string_view>

namespace DTex::detail::KTX
{
	struct Header
	{
		static constexpr size_t totalSize = sizeof(uint8_t) * 64;

		using Member_T = uint32_t;

		using Identifier_T = uint8_t[12];
		static constexpr size_t identifierOffset = 0;
		static constexpr Identifier_T correctIdentifier = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

		static constexpr uint32_t correctEndian = 0x04030201;
		static constexpr size_t endianOffset = identifierOffset + sizeof(Identifier_T);

		static constexpr size_t glTypeOffset = endianOffset + sizeof(Member_T);

		static constexpr size_t glTypeSizeOffset = glTypeOffset + sizeof(Member_T);

		static constexpr size_t glFormatOffset = glTypeSizeOffset + sizeof(Member_T);

		static constexpr size_t glInternalFormatOffset = glFormatOffset + sizeof(Member_T);

		static constexpr size_t glBaseInternalFormatOffset = glInternalFormatOffset + sizeof(Member_T);

		static constexpr size_t pixelWidthOffset = glBaseInternalFormatOffset + sizeof(Member_T);

		static constexpr size_t pixelHeightOffset = pixelWidthOffset + sizeof(Member_T);

		static constexpr size_t pixelDepthOffset = pixelHeightOffset + sizeof(Member_T);

		static constexpr size_t numberOfArrayElementsOffset = pixelDepthOffset + sizeof(Member_T);

		static constexpr size_t numberOfFacesOffset = numberOfArrayElementsOffset + sizeof(Member_T);

		static constexpr size_t numberOfMipmapLevelsOffset = numberOfFacesOffset + sizeof(Member_T);

		static constexpr size_t bytesOfKeyValueDataOffset = numberOfMipmapLevelsOffset + sizeof(Member_T);
	};

	bool LoadHeader_Backend(MetaData& metaData, std::ifstream& fstream, ResultInfo& resultInfo, std::string_view& errorMessage);
}