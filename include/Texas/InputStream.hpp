#pragma once

#include "Texas/Result.hpp"
#include "Texas/ByteSpan.hpp"

#include <cstddef>

namespace Texas
{
	class InputStream
	{
	public:
		[[nodiscard]] virtual Result read(ByteSpan dst) noexcept = 0;
		[[nodiscard]] virtual void ignore(std::size_t amount) noexcept = 0;

		[[nodiscard]] virtual std::size_t tell() noexcept = 0;
		virtual void seek(std::size_t pos) noexcept = 0;
	};
}