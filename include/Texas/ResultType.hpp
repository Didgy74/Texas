#pragma once

#include <cstdint>

namespace Texas
{
	enum class ResultType : std::uint8_t;
	constexpr bool IsEnumerated(ResultType info);
	// Returns nullptr if info is not enumerated in Texas::ResultType
	constexpr const char* ToString(ResultType info);
}

enum class Texas::ResultType : std::uint8_t
{
	Success,

	CouldNotOpenFile,
	CorruptFileData,
	FileNotSupported,
	InvalidInputParameter,
	PixelFormatNotSupported,
	PrematureEndOfFile,

	COUNT
};

constexpr bool Texas::IsEnumerated(ResultType info)
{
	return static_cast<std::uint8_t>(info) < static_cast<std::uint8_t>(ResultType::COUNT);
}

constexpr const char* Texas::ToString(ResultType info)
{
	switch (info)
	{
	case ResultType::Success:
		return "Texas::ResultType::Success";

	case ResultType::CouldNotOpenFile:
		return "Texas::ResultType::CouldNotOpenFile";
	case ResultType::CorruptFileData:
		return "Texas::ResultType::CorruptFileData";
	case ResultType::FileNotSupported:
		return "Texas::ResultType::FileNotSupported";
	case ResultType::InvalidInputParameter:
		return "Texas::ResultType::InvalidInputParameter";
	case ResultType::PixelFormatNotSupported:
		return "Texas::ResultType::PixelFormatNotSupported";
	case ResultType::PrematureEndOfFile:
		return "Texas::ResultType::PrematureEndOfFile";

	default:
		return "Invalid enum value";
	}

	return nullptr;
}