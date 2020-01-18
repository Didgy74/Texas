#pragma once

#include "Texas/MetaData.hpp"
#include "Texas/Optional.hpp"
#include "Texas/Allocator.hpp"
#include "Texas/ByteSpan.hpp"

#include <cstddef>
#include <cstdint>

namespace Texas
{
	namespace detail
	{
		class PrivateAccessor;
	}

	/*
		Represents the entirety of a file loaded with Texas.
		Contains both metadata and the actual imagedata.
	*/
	class Texture
	{
	public:
		Texture(const Texture&) = delete;
		Texture(Texture&&) noexcept;

		~Texture();

		[[nodiscard]] const MetaData& metaData() const noexcept;
		[[nodiscard]] FileFormat srcFileFormat() const noexcept;
		[[nodiscard]] TextureType textureType() const noexcept;
		[[nodiscard]] PixelFormat pixelFormat() const noexcept;
		[[nodiscard]] ChannelType channelType() const noexcept;
		[[nodiscard]] ColorSpace colorSpace() const noexcept;
		[[nodiscard]] Dimensions baseDimensions() const noexcept;
		[[nodiscard]] std::uint64_t mipLevelCount() const noexcept;
		[[nodiscard]] std::uint64_t arrayLayerCount() const noexcept;

		[[nodiscard]] Optional<std::uint64_t> mipOffset(std::uint64_t mipLevelIndex) const noexcept;
		[[nodiscard]] Optional<std::uint64_t> mipSize(std::uint64_t mipLevelIndex) const noexcept;
		/*
			Returns nullptr if you pass it out-of-bounds parameters.
		*/
		[[nodiscard]] const std::byte* mipData(std::uint64_t mipLevelIndex) const noexcept;
		[[nodiscard]] ConstByteSpan mipSpan(std::uint64_t mipLevelIndex) const noexcept;

		[[nodiscard]] Optional<std::uint64_t> arrayLayerOffset(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept;
		[[nodiscard]] Optional<std::uint64_t> arrayLayerSize(std::uint64_t mipLevelIndex) const noexcept;
		/*
			Returns nullptr if you pass it out-of-bounds parameters.
		*/
		[[nodiscard]] const std::byte* arrayLayerData(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept;
		[[nodiscard]] ConstByteSpan arrayLayerSpan(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept;

		[[nodiscard]] const std::byte* rawBufferData() const noexcept;
		[[nodiscard]] std::uint64_t totalDataSize() const noexcept;
		[[nodiscard]] ConstByteSpan rawBufferSpan() const noexcept;

		/*
			A Texture produced by the Texas library is never invalid.
			However, a Texture object will become invalid when you move it.
		*/
		bool isValid() const noexcept;

	private:
		Texture() = default;

		MetaData m_metaData{};
		ByteSpan m_buffer = {};
		Allocator* m_allocator = nullptr;

		friend detail::PrivateAccessor;
	};
}