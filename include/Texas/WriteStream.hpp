#pragma once

#include <cstdint>

namespace Texas
{
	class WriteStream
	{
	public:
		virtual void write(char const* data, std::uint64_t size) noexcept = 0;
	};
}