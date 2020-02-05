#pragma once

#include "Texas/KTX/TextureInfo.hpp"

#include "Texas/Texture.hpp"
#include "Texas/Result.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/ByteSpan.hpp"
#include "Texas/WriteStream.hpp"

namespace Texas
{
	[[nodiscard]] Result saveToBuffer(ByteSpan dstBuffer, TextureInfo const& texInfo, Span<ConstByteSpan const> mipLevels) noexcept;
}

namespace Texas::KTX
{
	[[nodiscard]] ResultValue<std::uint64_t> calcFileSize(TextureInfo const& texInfo) noexcept;

	/*
		Writes a KTX to polymorphic stream.

		Dimensions must all be higher than 0.
		Dimensions must be smaller than uint32 max value.
		TextureType cannot be Array3D.
		mipLevels.size() must have the same length as texInfo.length
	*/
	[[nodiscard]] Result saveToStream(TextureInfo const& texInfo, Span<ConstByteSpan const> mipLevels, WriteStream& stream) noexcept;
	/*
		Writes a KTX to polymorphic stream.

		Dimensions must all be higher than 0.
		Dimensions must all be smaller than uint32 max value.
		TextureType cannot be Array3D.
		mipLevels.size() must have the same length as texInfo.length
	*/
	[[nodiscard]] Result saveToStream(Texture const& texture, WriteStream& stream) noexcept;

	/*
		Writes a KTX to file.

		Dimensions must all be higher than 0.
		Dimensions must be smaller than uint32 max value.
		TextureType cannot be Array3D.
		mipLevels.size() must have the same length as texInfo.length
	*/
	[[nodiscard]] Result saveToFile(char const* path, TextureInfo const& texInfo, Span<ConstByteSpan const> mipLevels) noexcept;

	[[nodiscard]] Result saveToFile(char const* path, Texture const& texture) noexcept;
}