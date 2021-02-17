#pragma once

#include <Texas/ChannelType.hpp>
#include <Texas/ColorSpace.hpp>
#include <Texas/Texture.hpp>
#include <Texas/TextureInfo.hpp>
#include <Texas/TextureType.hpp>
#include <Texas/PixelFormat.hpp>

#include <cstdint>

struct VkExtent3D;

namespace Texas
{
	[[nodiscard]] VkExtent3D toVkExtent3D(Dimensions dimensions) noexcept;
	[[nodiscard]] VkExtent3D toVkExtent3D(TextureInfo const& texInfo) noexcept;
	[[nodiscard]] VkExtent3D toVkExtent3D(Texture const& texInfo) noexcept;
	[[nodiscard]] std::uint32_t toVkImageType(TextureType type) noexcept;
	[[nodiscard]] std::uint32_t toVkImageViewType(TextureType type) noexcept;
	[[nodiscard]] std::uint32_t toVkFormat(
		PixelFormat pixelFormat, 
		ColorSpace colorSpace, 
		ChannelType channelType) noexcept;
	[[nodiscard]] std::uint32_t toVkFormat(TextureInfo const& texInfo) noexcept;
	[[nodiscard]] std::uint32_t toVkFormat(Texture const& texture) noexcept;

	[[nodiscard]] PixelFormat VkFormatToPixelFormat(std::uint32_t vkFormat) noexcept;
	[[nodiscard]] ChannelType VkFormatToChannelType(std::uint32_t vkFormat) noexcept;
}
