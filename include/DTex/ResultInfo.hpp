#pragma once

#include <cstdint>
#include <string_view>
#include <cassert>

namespace DTex
{
	enum class ResultInfo : uint8_t;
	constexpr bool IsEnumerated(ResultInfo info);
	constexpr std::string_view ToString(ResultInfo info);
}

enum class DTex::ResultInfo : uint8_t
{
	Success,
	CouldNotReadFile,
	CorruptFileData,
	FileNotSupported,
	PixelFormatNotSupported,

	COUNT
};

constexpr bool DTex::IsEnumerated(ResultInfo info)
{
	return static_cast<uint8_t>(info) < static_cast<uint8_t>(ResultInfo::COUNT);
}

constexpr std::string_view DTex::ToString(ResultInfo info)
{
	assert(IsEnumerated(info) && "Invalid input sent to DTex::ToString(ResultInfo).");

	switch (info)
	{
	case ResultInfo::Success:
		return "ResultInfo::Success";
	case ResultInfo::CouldNotReadFile:
		return "ResultInfo::CouldNotReadFile";
	case ResultInfo::CorruptFileData:
		return "ResultInfo::CorruptFileData";
	case ResultInfo::FileNotSupported:
		return "ResultInfo::FileNotSupported";
	case ResultInfo::PixelFormatNotSupported:
		return "ResultInfo::PixelFormatNotSupported";
	default:
		return "Invalid enum value";
	}
}