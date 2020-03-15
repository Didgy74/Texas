#pragma once

#include "Texas/Result.hpp"

#include <cstdint>

namespace Texas
{
	class OutputStream
	{
	public:
		[[nodiscard]] virtual Result write(char const* data, std::uint64_t size) noexcept = 0;
		inline virtual ~OutputStream() {}
	};
}