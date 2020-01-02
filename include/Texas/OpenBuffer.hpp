#pragma once

#include "Texas/MetaData.hpp"
#include "Texas/Dimensions.hpp"

#include <cstdint>

namespace Texas
{
	namespace detail
	{
		class PrivateAccessor;
	}

	/*
		Contains info on an image parsed from a buffer.

		This includes metadata and some fileformat-specific data for loading
		imagedata from said fileformat.
	*/
	class OpenBuffer
	{
	public:
		[[nodiscard]] inline constexpr const MetaData& metaData() const;

		[[nodiscard]] inline constexpr Dimensions baseDimensions() const;

		[[nodiscard]] std::size_t memoryRequired() const;

		[[nodiscard]] std::size_t workingMemoryRequired() const;

		[[nodiscard]] std::size_t mipOffset(std::uint32_t mipIndex) const;

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

	[[nodiscard]] inline constexpr Dimensions OpenBuffer::baseDimensions() const
	{
		return m_metaData.baseDimensions;
	}
}