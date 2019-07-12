#pragma once

#include <cstdint>

namespace DTex
{
	enum class FileFormat : uint8_t
	{
		Unsupported,
		KTX,
		KTX2,
		PNG
	};
}