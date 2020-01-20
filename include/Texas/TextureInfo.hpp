#pragma once

#include "Texas/FileFormat.hpp"
#include "Texas/Dimensions.hpp"
#include "Texas/PixelFormat.hpp"
#include "Texas/TextureType.hpp"
#include "Texas/ChannelType.hpp"
#include "Texas/Colorspace.hpp"

namespace Texas
{
    /*
        Holds information about a texture.
    */
    struct TextureInfo
    {
        FileFormat fileFormat = {};
        TextureType textureType = {};
        Dimensions baseDimensions = {};
        PixelFormat pixelFormat = {};
        ChannelType channelType = {};
        ColorSpace colorSpace = {};
        std::uint64_t mipLevelCount = 0;
        std::uint64_t arrayLayerCount = 0;
    };
}

