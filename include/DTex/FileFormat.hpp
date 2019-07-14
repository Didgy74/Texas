#pragma once

#include <cstdint>
#include <string_view>
#include <cassert>

namespace DTex
{
	enum class FileFormat : uint8_t
	{
		Unsupported,
		KTX,
		KTX2,
		PNG,
		COUNT
	};

	constexpr bool IsEnumerated(FileFormat fileFormat)
	{
		return static_cast<uint8_t>(fileFormat) < static_cast<uint8_t>(FileFormat::COUNT);
	}

	constexpr std::string_view ToString(FileFormat fileFormat)
	{
		assert(IsEnumerated(fileFormat) && "Error. Invalid value passed into DTex::ToString(FileFormat).");

		switch(fileFormat)
		{
		case FileFormat::Unsupported:
			return "FileFormat::Unsupported";
		case FileFormat::KTX:
			return "FileFormat::KTX";
		case FileFormat::KTX2:
			return "FileFormat::KTX2";
		case FileFormat::PNG:
			return "FileFormat::PNG";
		default:
			return "Invalid enum value.";
		}
	}
}