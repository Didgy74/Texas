#pragma once

#include <cstdint>
#include <cstddef>

namespace Texas
{
	/*
		Polymorphic allocator interface.

		Inherit from this if you want to control the library's dynamic allocations.
	*/
	class Allocator
	{
	public:
		enum class MemoryType : char
		{
			// Specifies this memory is strictly for image-data only.
			ImageData,
			// Specifies this memory is strictly for working memory only.
			WorkingData
		};

		[[nodiscard]] virtual std::byte* allocate(std::size_t amount, MemoryType memType) = 0;
		virtual void deallocate(std::byte* ptr, MemoryType memType) = 0;
	};
}