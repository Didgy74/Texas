#pragma once
#include "Texas/detail/VulkanTools.hpp"
#include "Texas/detail/Exception.hpp"
#include "Texas/detail/NumericLimits.hpp"
#include "Texas/TextureInfo.hpp"

#include <cstdint>

#include "vulkan/vulkan.h"

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

    [[nodiscard]] inline VkExtent3D toVkExtent3D(Dimensions dimensions)
    {
        TEXAS_DETAIL_EXCEPTION(dimensions.width <= detail::maxValue<std::uint32_t>(), );
        TEXAS_DETAIL_EXCEPTION(dimensions.height <= detail::maxValue<std::uint32_t>(), );
        TEXAS_DETAIL_EXCEPTION(dimensions.depth <= detail::maxValue<std::uint32_t>(), );

        VkExtent3D temp{};
        temp.width = static_cast<std::uint32_t>(dimensions.width);
        temp.height = static_cast<std::uint32_t>(dimensions.height);
        temp.depth = static_cast<std::uint32_t>(dimensions.depth);
        return temp;
    }
}