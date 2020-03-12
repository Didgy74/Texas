#pragma once

#include "Texas/PixelFormat.hpp"
#include "Texas/Dimensions.hpp"
#include "Texas/TextureType.hpp"
#include "Texas/TextureInfo.hpp"

#include <cstdint>

namespace Texas
{
    /*
        Returns the maximum amount of mip levels a texture with baseDimensions can hold.
    */
    [[nodiscard]] std::uint64_t calcMaxMipCount(Dimensions baseDimensions) noexcept;

    /*
        Returns the dimensions of a mip-level based on the baseDimensions.
    */
    [[nodiscard]] Dimensions calcMipDimensions(Dimensions baseDimensions, std::uint64_t mipLevelIndex) noexcept;

    /*
        Returns the total amount of memory an image will require.
    */
    [[nodiscard]] std::uint64_t calcTotalSize(
        Dimensions baseDimensions, 
        PixelFormat pixelFormat, 
        std::uint64_t mipLevelCount, 
        std::uint64_t arrayLayerCount) noexcept;

    /*
        Returns the total amount of memory an image will require.
    */
    [[nodiscard]] std::uint64_t calcTotalSize(TextureInfo const& textureInfo) noexcept;

    /*
        Returns the offset from the start of a buffer to a mip level.
    */
    [[nodiscard]] std::uint64_t calcMipOffset(
        Dimensions baseDimensions,
        PixelFormat pixelFormat,
        std::uint64_t arrayLayerCount,
        std::uint64_t mipLevelIndex) noexcept;

    /*
        Returns the offset from the start of a buffer to a mip level.
    */
    [[nodiscard]] std::uint64_t calcMipOffset(TextureInfo const& textureInfo, std::uint64_t mipLevelIndex) noexcept;

    /*
        Returns the offset from the start of a buffer to an array layer.
    */
    [[nodiscard]] std::uint64_t calcLayerOffset(
        Dimensions baseDimensions,
        PixelFormat pixelFormat,
        std::uint64_t mipLevelIndex,
        std::uint64_t arrayLayerCount,
        std::uint64_t arrayLayerIndex) noexcept;

    /*
        Returns the offset from the start of a buffer to an array layer.
    */
    [[nodiscard]] std::uint64_t calcLayerOffset(
        TextureInfo const& textureInfo,
        std::uint64_t mipLevelIndex,
        std::uint64_t arrayLayerIndex) noexcept;

    /*
        Returns the size of a single 1D, 2D or 3D texture.

        Does not work for cubemaps.
    */
    [[nodiscard]] std::uint64_t calcSingleImageSize(Dimensions dimensions, PixelFormat pixelFormat) noexcept;
}