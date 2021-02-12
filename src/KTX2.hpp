#pragma once

#include <Texas/InputStream.hpp>
#include <Texas/Result.hpp>
#include <Texas/TextureInfo.hpp>

#include <cstdint>

namespace Texas::detail::KTX2
{
	constexpr std::uint8_t identifier[12] = {
		0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

	[[nodiscard]] Result parseStream(
		InputStream& stream,
		TextureInfo& textureInfo) noexcept;
}