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

#ifdef TEXAS_ENABLE_KTX_READ
		struct KTX_BackendData
		{
			const std::byte* srcImageDataStart = nullptr;
		};
#endif

#ifdef TEXAS_ENABLE_PNG_READ
		struct PNG_BackendData
		{
			const std::byte* idatChunkStart = nullptr;
		};
#endif

	private:
		OpenBuffer() = default;

		MetaData m_metaData = {};

		union BackendData
		{
#ifdef TEXAS_ENABLE_KTX_READ
			KTX_BackendData ktx{};
#endif
#ifdef TEXAS_ENABLE_PNG_READ
			PNG_BackendData png;
#endif
		};

		mutable BackendData m_backendData{};

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