#pragma once

#include "Texas/PixelFormat.hpp"
#include "Texas/Dimensions.hpp"
#include "Texas/TextureType.hpp"
#include "Texas/TextureInfo.hpp"
#include "Texas/Optional.hpp"

#include <cstdint>
#include <cstddef>

namespace Texas
{
    [[nodiscard]] std::uint64_t calcMaxMipLevelCount(Dimensions baseDimensions) noexcept;

    [[nodiscard]] Optional<Dimensions> calcMipmapDimensions(Dimensions baseDimensions, std::uint64_t mipLevelIndex) noexcept;

    [[nodiscard]] std::uint64_t calcTotalSizeRequired(
        Dimensions baseDimensions, 
        PixelFormat pixelFormat, 
        std::uint64_t mipLevelCount, 
        std::uint64_t arrayLayerCount) noexcept;

    [[nodiscard]] std::uint64_t calcTotalSizeRequired(const TextureInfo& textureInfo) noexcept;

    [[nodiscard]] Optional<std::uint64_t> calcMipOffset(
        Dimensions baseDimensions,
        PixelFormat pixelFormat,
        std::uint64_t arrayLayerCount,
        std::uint64_t mipLevelCount,
        std::uint64_t mipLevelIndex) noexcept;

    [[nodiscard]] Optional<std::uint64_t> calcMipOffset(const TextureInfo& textureInfo, std::uint64_t mipLevelIndex) noexcept;

    [[nodiscard]] Optional<std::uint64_t> calcArrayLayerOffset(
        Dimensions baseDimensions,
        PixelFormat pixelFormat,
        std::uint64_t mipLevelCount,
        std::uint64_t mipLevelIndex,
        std::uint64_t arrayLayerCount,
        std::uint64_t arrayLayerIndex) noexcept;

    [[nodiscard]] Optional<std::uint64_t> calcArrayLayerOffset(
        const TextureInfo& textureInfo,
        std::uint64_t mipLevelIndex,
        std::uint64_t arrayLayerIndex) noexcept;

    [[nodiscard]] std::uint64_t calcArrayLayerSize(Dimensions dimensions, PixelFormat pixelFormat) noexcept;
}