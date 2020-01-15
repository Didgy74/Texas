#pragma once

#include "Texas/PixelFormat.hpp"
#include "Texas/Dimensions.hpp"
#include "Texas/TextureType.hpp"
#include "Texas/MetaData.hpp"
#include "Texas/Optional.hpp"

#include <cstdint>
#include <cstddef>

namespace Texas::Tools
{
    /*
        Returns 0 upon failure
    */
    [[nodiscard]] std::uint64_t calcSingleImageDataSize(Dimensions dimensions, PixelFormat pixelFormat) noexcept;
    [[nodiscard]] std::uint64_t calcSingleImageDataSize(const MetaData& metaData) noexcept;

    [[nodiscard]] std::uint64_t calcMaxMipLevelCount(Dimensions in);

    [[nodiscard]] Dimensions calcMipmapDimensions(Dimensions baseDimensions, std::uint64_t mipLevel);

    [[nodiscard]] std::uint64_t calcTotalSizeRequired(Dimensions baseDimensions, std::uint64_t arrayLayerCount, std::uint64_t mipLevelCount, PixelFormat pFormat);
    [[nodiscard]] std::uint64_t calcTotalSizeRequired(const MetaData& metaData);

    [[nodiscard]] Optional<std::size_t> calcMipOffset(
        Dimensions baseDimensions,
        PixelFormat pixelFormat,
        std::uint64_t arrayLayerCount,
        std::uint64_t mipLevelCount,
        std::uint64_t mipLevelIndex) noexcept;

    [[nodiscard]] Optional<std::size_t> calcMipOffset(const MetaData& metaData, std::uint64_t mipLevelIndex) noexcept;
}


