#pragma once

#include "Texas/MetaData.hpp"

#include <cstdint>

namespace Texas
{
	namespace detail
	{
		class PrivateAccessor;
	}

	class OpenBuffer
	{
	public:
		[[nodiscard]] inline constexpr const MetaData& metaData() const;

		[[nodiscard]] std::size_t memoryRequired() const;

		[[nodiscard]] std::size_t workingMemoryRequired() const;

	private:
		OpenBuffer() = default;

		MetaData m_metaData = {};

		const std::uint8_t* imageDataStart = nullptr;

		friend class detail::PrivateAccessor;
	};

	constexpr const MetaData& OpenBuffer::metaData() const
	{
		return m_metaData;
	}
}