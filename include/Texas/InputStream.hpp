#pragma once

#include <Texas/Result.hpp>
#include <Texas/Span.hpp>

#include <cstddef>

namespace Texas
{
	/*
		Polymorphic stream interface for reading bytes.
	*/
	class InputStream
	{
	public:
		[[nodiscard]] virtual Result read(ByteSpan dst) noexcept = 0;
		virtual void ignore(std::size_t amount) noexcept = 0;

		[[nodiscard]] virtual std::size_t tell() noexcept = 0;
		virtual void seek(std::size_t pos) noexcept = 0;
	};
}