#pragma once

#include "Texas/Texture.hpp"
#include "Texas/Result.hpp"
#include "Texas/ResultValue.hpp"
#include "Texas/Span.hpp"
#include "Texas/OutputStream.hpp"

namespace Texas::KTX
{
	[[nodiscard]] Result canSave(TextureInfo const& texInfo) noexcept;

	[[nodiscard]] ResultValue<std::uint64_t> calcFileSize(TextureInfo const& texInfo) noexcept;

	/*
		Writes a KTX to polymorphic stream.

		Dimensions must all be higher than 0.
		Dimensions must be smaller than uint32 max value.
		TextureType cannot be Array3D.
		mipLevels.size() must have the same length as texInfo.length
	*/
	[[nodiscard]] Result saveToStream(
		TextureInfo const& texInfo, 
		Span<ConstByteSpan const> mipLevels, 
		OutputStream& stream) noexcept;
	/*
		Writes a KTX to polymorphic stream.

		Dimensions must all be higher than 0.
		Dimensions must all be smaller than uint32 max value.
		TextureType cannot be Array3D.
		mipLevels.size() must have the same length as texInfo.length
	*/
	[[nodiscard]] Result saveToStream(Texture const& texture, OutputStream& stream) noexcept;

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