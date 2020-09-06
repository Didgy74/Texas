#pragma once

#include "Texas/FileFormat.hpp"
#include "Texas/Dimensions.hpp"
#include "Texas/PixelFormat.hpp"
#include "Texas/TextureType.hpp"
#include "Texas/ChannelType.hpp"
#include "Texas/ColorSpace.hpp"

#include <cstdint>

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
        std::uint8_t mipCount = 0;
        std::uint64_t layerCount = 0;
    };
}


