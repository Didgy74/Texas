#pragma once

#include "Texas/PixelFormat.hpp"
#include "Texas/ColorSpace.hpp"
#include "Texas/TextureType.hpp"
#include "Texas/ChannelType.hpp"

#include <cstdint>

namespace Texas
{
	[[nodiscard]] std::uint32_t toVkImageType(TextureType type) noexcept;
	[[nodiscard]] std::uint32_t toVkImageViewType(TextureType type) noexcept;
	[[nodiscard]] std::uint32_t toVkFormat(PixelFormat pFormat, ColorSpace cSpace, ChannelType chType) noexcept;
}
