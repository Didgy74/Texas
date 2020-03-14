#pragma once

#include <cstdint>

namespace Texas
{
	class OutputStream
	{
	public:
		virtual void write(char const* data, std::uint64_t size) noexcept = 0;
	};
}