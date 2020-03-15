#pragma once

#include "Texas/PixelFormat.hpp"
#include "Texas/Dimensions.hpp"
#include "Texas/TextureInfo.hpp"

#include <cstdint>

namespace Texas
{
    /*
        Returns the maximum amount of mips a texture with baseDimensions can hold.
    */
    [[nodiscard]] std::uint64_t calculateMaxMipCount(Dimensions baseDimensions) noexcept;

    /*
        Returns the dimensions of a mip-level based on the baseDimensions.

        Causes undefined behavior if:
            mipIndex is equal to or higher than the maximum mip count baseDimensions can support.
    */
    [[nodiscard]] Dimensions calculateMipDimensions(Dimensions baseDimensions, std::uint8_t mipIndex) noexcept;

    /*
        Returns the total amount of memory a texture's imagedata will require.
    */
    [[nodiscard]] std::uint64_t calculateTotalSize(
        Dimensions baseDimensions, 
        PixelFormat pixelFormat, 
        std::uint8_t mipCount, 
        std::uint64_t arrayCount) noexcept;

    /*
        Returns the total amount of memory a texture's imagedata will require.
    */
    [[nodiscard]] std::uint64_t calculateTotalSize(TextureInfo const& textureInfo) noexcept;

    /*
        Returns the offset from the start of a buffer to a mip level.
    */
    [[nodiscard]] std::uint64_t calculateMipOffset(
        Dimensions baseDimensions,
        PixelFormat pixelFormat,
        std::uint64_t arrayLayerCount,
        std::uint8_t mipIndex) noexcept;

    /*
        Returns the offset from the start of a buffer to a mip level.
    */
    [[nodiscard]] std::uint64_t calculateMipOffset(TextureInfo const& textureInfo, std::uint8_t mipIndex) noexcept;

    /*
        Returns the offset from the start of a buffer to an layer.
    */
    [[nodiscard]] std::uint64_t calculateLayerOffset(
        Dimensions baseDimensions,
        PixelFormat pixelFormat,
        std::uint8_t mipIndex,
        std::uint64_t layerCount,
        std::uint64_t layerIndex) noexcept;

    /*
        Returns the offset from the start of a buffer to an layer.
    */
    [[nodiscard]] std::uint64_t calculateLayerOffset(
        TextureInfo const& textureInfo,
        std::uint8_t mipIndex,
        std::uint64_t layerIndex) noexcept;

    /*
        Returns the size of a single 1D, 2D or 3D texture.

        Does not work for cubemaps.
    */
    [[nodiscard]] std::uint64_t calculateSingleImageSize(Dimensions dimensions, PixelFormat pixelFormat) noexcept;
}