#pragma once

#include <cstdint>
#include <cstddef>

namespace Texas
{
	/*
		Polymorphic allocator interface.

		Inherit from this if you want to control the library's dynamic allocations .
	*/
	class Allocator
	{
	public:
		virtual std::byte* allocate(std::uint64_t amount) = 0;
		virtual void deallocate(std::byte* ptr) = 0;
	};
}