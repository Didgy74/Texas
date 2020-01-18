#pragma once

#include "Texas/PixelFormat.hpp"
#include "Texas/Dimensions.hpp"
#include "Texas/TextureType.hpp"
#include "Texas/MetaData.hpp"
#include "Texas/Optional.hpp"

#include <cstdint>
#include <cstddef>

namespace Texas
{
    [[nodiscard]] std::uint64_t calcMaxMipLevelCount(Dimensions baseDimensions) noexcept;

    [[nodiscard]] Dimensions calcMipmapDimensions(Dimensions baseDimensions, std::uint64_t mipLevelIndex) noexcept;

    /*
        Returns 0 upon failure
    */
    [[nodiscard]] std::uint64_t calcSingleImageDataSize(Dimensions dimensions, PixelFormat pixelFormat) noexcept;

    [[nodiscard]] std::uint64_t calcTotalSizeRequired(
        Dimensions baseDimensions, 
        PixelFormat pixelFormat, 
        std::uint64_t mipLevelCount, 
        std::uint64_t arrayLayerCount) noexcept;
    [[nodiscard]] std::uint64_t calcTotalSizeRequired(const MetaData& metaData) noexcept;

    [[nodiscard]] Optional<std::uint64_t> calcMipOffset(
        Dimensions baseDimensions,
        PixelFormat pixelFormat,
        std::uint64_t arrayLayerCount,
        std::uint64_t mipLevelCount,
        std::uint64_t mipLevelIndex) noexcept;

    [[nodiscard]] Optional<std::uint64_t> calcMipOffset(const MetaData& metaData, std::uint64_t mipLevelIndex) noexcept;

    [[nodiscard]] Optional<std::uint64_t> calcArrayLayerOffset(
        Dimensions baseDimensions,
        PixelFormat pixelFormat,
        std::uint64_t mipLevelCount,
        std::uint64_t mipLevelIndex,
        std::uint64_t arrayLayerCount,
        std::uint64_t arrayLayerIndex) noexcept;
    [[nodiscard]] Optional<std::uint64_t> calcArrayLayerOffset(
        const MetaData& metaData,
        std::uint64_t mipLevelIndex,
        std::uint64_t arrayLayerIndex) noexcept;
}