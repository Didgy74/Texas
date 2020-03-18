#pragma once
#include "Texas/detail/VulkanTools.hpp"
#include "Texas/detail/Exception.hpp"
#include "Texas/detail/NumericLimits.hpp"
#include "Texas/TextureInfo.hpp"

#include <cstdint>

namespace Texas
{
    [[nodiscard]] inline std::uint32_t toVkImageType(TextureType type) noexcept
    {
        return static_cast<std::uint32_t>(detail::toVkImageType(type));
    }

    [[nodiscard]] inline std::uint32_t toVkImageViewType(TextureType type) noexcept
    {
        return static_cast<std::uint32_t>(detail::toVkImageViewType(type));
    }

    [[nodiscard]] inline std::uint32_t toVkFormat(PixelFormat pixelFormat, ColorSpace colorSpace, ChannelType channelType) noexcept
    {
        return static_cast<std::uint32_t>(detail::toVkFormat(pixelFormat, colorSpace, channelType));
    }

    [[nodiscard]] inline std::uint32_t toVkFormat(TextureInfo const& metaData) noexcept
    {
        return static_cast<std::uint32_t>(toVkFormat(metaData.pixelFormat, metaData.colorSpace, metaData.channelType));
    }
}