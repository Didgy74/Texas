#include "Texas/detail/VulkanTools.hpp"
#include "Texas/VulkanTools.hpp"

namespace Texas::detail
{
    detail::VkImageType toVkImageType(TextureType type) noexcept
    {
        switch (type)
        {
        case TextureType::Texture1D:
        case TextureType::Array1D:
            return VkImageType::VK_IMAGE_TYPE_1D;
        case TextureType::Texture2D:
        case TextureType::Array2D:
        case TextureType::Cubemap:
        case TextureType::ArrayCubemap:
            return VkImageType::VK_IMAGE_TYPE_2D;
        case TextureType::Texture3D:
        case TextureType::Array3D:
            return VkImageType::VK_IMAGE_TYPE_3D;
        }

        return VkImageType::VK_IMAGE_TYPE_MAX_ENUM;
    }

    detail::VkImageViewType toVkImageViewType(TextureType type) noexcept
    {
        switch (type)
        {
        case TextureType::Texture1D:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;
        case TextureType::Texture2D:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
        case TextureType::Texture3D:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_3D;

        case TextureType::Array1D:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        case TextureType::Array2D:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY;

        case TextureType::Cubemap:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
        case TextureType::ArrayCubemap:
            return VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        }

        return VkImageViewType::VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    }
}

#define TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_8_BIT(vkname) \
    switch (cSpace) \
    { \
    case ColorSpace::sRGB: \
        return VkFormat::VK_FORMAT_## vkname ##_SRGB; \
    case ColorSpace::Linear: \
        switch (chType) \
        { \
        case ChannelType::UnsignedNormalized: \
            return VkFormat::VK_FORMAT_## vkname ##_UNORM; \
        case ChannelType::SignedNormalized: \
            return VkFormat::VK_FORMAT_## vkname ##_SNORM; \
        case ChannelType::UnsignedInteger: \
            return VkFormat::VK_FORMAT_## vkname ##_UINT; \
        case ChannelType::SignedInteger: \
            return VkFormat::VK_FORMAT_## vkname ##_SINT; \
        case ChannelType::UnsignedScaled: \
            return VkFormat::VK_FORMAT_## vkname ##_USCALED; \
        case ChannelType::SignedScaled: \
            return VkFormat::VK_FORMAT_## vkname ##_USCALED; \
        default: \
            return VkFormat::VK_FORMAT_MAX_ENUM; \
        } \
    default: \
        return VkFormat::VK_FORMAT_MAX_ENUM; \
    } \

#define TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_BC1(layout) \
    switch (cSpace) \
    { \
    case ColorSpace::sRGB: \
        return VkFormat::VK_FORMAT_BC1_## layout ##_SRGB_BLOCK; \
    case ColorSpace::Linear: \
        switch (chType) \
        { \
        case ChannelType::UnsignedNormalized: \
            return VkFormat::VK_FORMAT_BC1_## layout ##_UNORM_BLOCK; \
        default: \
            return VkFormat::VK_FORMAT_MAX_ENUM; \
        } \
    default: \
        return VkFormat::VK_FORMAT_MAX_ENUM; \
    } \

#define TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_BC2_OR_BC3(bcn) \
    switch (cSpace) \
    { \
    case ColorSpace::sRGB: \
        return VkFormat::VK_FORMAT_## bcn ##_SRGB_BLOCK; \
    case ColorSpace::Linear: \
        switch (chType) \
        { \
        case ChannelType::UnsignedNormalized: \
            return VkFormat::VK_FORMAT_## bcn ##_UNORM_BLOCK; \
        default: \
            return VkFormat::VK_FORMAT_MAX_ENUM; \
        } \
    default: \
        return VkFormat::VK_FORMAT_MAX_ENUM; \
    } \

#define TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_BC4_OR_BC5(bcn) \
    switch (cSpace) \
    { \
    case ColorSpace::Linear: \
        switch (chType) \
        { \
        case ChannelType::UnsignedNormalized: \
            return VkFormat::VK_FORMAT_## bcn ##_UNORM_BLOCK; \
        case ChannelType::SignedNormalized: \
            return VkFormat::VK_FORMAT_## bcn ##_SNORM_BLOCK; \
        default: \
            return VkFormat::VK_FORMAT_MAX_ENUM; \
        } \
    default: \
        return VkFormat::VK_FORMAT_MAX_ENUM; \
    } \

namespace Texas::detail
{
    detail::VkFormat toVkFormat(const PixelFormat pFormat, const ColorSpace cSpace, const ChannelType chType) noexcept
    {
        switch (pFormat)
        {
            // Standard 8 bit
        case PixelFormat::R_8:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_8_BIT(R8)
        case PixelFormat::RG_8:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_8_BIT(R8G8)
        case PixelFormat::RGB_8:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_8_BIT(R8G8B8)
        case PixelFormat::BGR_8:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_8_BIT(B8G8R8)
        case PixelFormat::RGBA_8:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_8_BIT(R8G8B8A8)
        case PixelFormat::BGRA_8:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_8_BIT(B8G8R8A8)


        // BCn
        case PixelFormat::BC1_RGB:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_BC1(RGB)
        case PixelFormat::BC1_RGBA:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_BC1(RGBA)
        case PixelFormat::BC2_RGBA:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_BC2_OR_BC3(BC2)
        case PixelFormat::BC3_RGBA:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_BC2_OR_BC3(BC3)
        case PixelFormat::BC4:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_BC4_OR_BC5(BC4)
        case PixelFormat::BC5:
            TEXAS_VULKANTOOLS_TOVKFORMAT_GENERATE_CASES_FOR_BC4_OR_BC5(BC5)
        case PixelFormat::BC6H:
            switch (cSpace)
            {
            case ColorSpace::Linear:
                switch (chType)
                {
                case ChannelType::UnsignedFloat:
                    return VkFormat::VK_FORMAT_BC6H_UFLOAT_BLOCK;
                case ChannelType::SignedFloat:
                    return VkFormat::VK_FORMAT_BC6H_SFLOAT_BLOCK;
                default:
                    return VkFormat::VK_FORMAT_MAX_ENUM;
                }
            default:
                return VkFormat::VK_FORMAT_MAX_ENUM;
            }
        case PixelFormat::BC7_RGBA:
            switch (cSpace)
            {
            case ColorSpace::sRGB:
                return VkFormat::VK_FORMAT_BC7_SRGB_BLOCK;
            case ColorSpace::Linear:
                return VkFormat::VK_FORMAT_BC7_UNORM_BLOCK;
            default:
                return VkFormat::VK_FORMAT_MAX_ENUM;
            }

        default:
            return VkFormat::VK_FORMAT_MAX_ENUM;
        }
    }
}