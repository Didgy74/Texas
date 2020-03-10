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
		enum class MemoryType : char
		{
			ImageData,
			WorkingData
		};

		virtual std::byte* allocate(std::size_t amount, MemoryType memType) = 0;
		virtual void deallocate(std::byte* ptr, MemoryType memType) = 0;
	};
}