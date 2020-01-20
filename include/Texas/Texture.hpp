#pragma once

#include "Texas/TextureInfo.hpp"
#include "Texas/Optional.hpp"
#include "Texas/Allocator.hpp"
#include "Texas/ByteSpan.hpp"

// Include detail headers
#include "Texas/detail/PrivateAccessor_Declaration.hpp"

#include <cstddef>
#include <cstdint>

namespace Texas
{
    /*
        Represents the entirety of a file loaded with Texas.
        Contains both metadata and the actual imagedata.

        Running any methods on a Texture whose contents have been extracted through move-semantics is UB.
    */
    class Texture
    {
    public:
        Texture() = default;
        Texture(const Texture&) = delete;
        Texture(Texture&&) noexcept;

        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) noexcept;

        ~Texture();

        [[nodiscard]] const TextureInfo& textureInfo() const noexcept;
        [[nodiscard]] FileFormat fileFormat() const noexcept;
        [[nodiscard]] TextureType textureType() const noexcept;
        [[nodiscard]] PixelFormat pixelFormat() const noexcept;
        [[nodiscard]] ChannelType channelType() const noexcept;
        [[nodiscard]] ColorSpace colorSpace() const noexcept;
        [[nodiscard]] Dimensions baseDimensions() const noexcept;
        [[nodiscard]] std::uint64_t mipLevelCount() const noexcept;
        [[nodiscard]] std::uint64_t arrayLayerCount() const noexcept;

        /*
            Returns the offset from the start the imagedata to the specified mip level index.

            Returns a non-value optional if: 
             - If mipLevelIndex is equal to or higher than .mipLevelCount().
        */
        [[nodiscard]] Optional<std::uint64_t> mipOffset(std::uint64_t mipLevelIndex) const noexcept;

        /*
            Returns the size of the all the image-data at the specified mip level index.

            Returns a non-value optional if:
             - If mipLevelIndex is equal to or higher than .mipLevelCount().
        */
        [[nodiscard]] Optional<std::uint64_t> mipSize(std::uint64_t mipLevelIndex) const noexcept;

        /*
            Returns a pointer to the image-data at the specified mip level index.

            Returns a non-value Optional if:
             - If mipLevelIndex is equal to or higher than .mipLevelCount().
        */
        [[nodiscard]] Optional<const std::byte*> mipData(std::uint64_t mipLevelIndex) const noexcept;

        /*
            Returns a span to the image-data of the specified mip level index.

            Returns a nullptr, zero-size span if:
            - If mipLevelIndex is equal to or higher than .mipLevelCount().
        */
        [[nodiscard]] Optional<ConstByteSpan> mipSpan(std::uint64_t mipLevelIndex) const noexcept;

        /*
            Returns the offset from the start the imagedata to the specified array layer.

            Returns a non-value optional if:
            - If mipLevelIndex is equal to or higher than .mipLevelCount().
            - If arrayLayerIndex is equal to or higher than .arrayLayerCount().
        */
        [[nodiscard]] Optional<std::uint64_t> arrayLayerOffset(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept;

        /*
            Returns the size of the specified array layer at the specified mip level.

            Returns a non-value optional if:
            - If mipLevelIndex is equal to or higher than .mipLevelCount().
            - If arrayLayerIndex is equal to or higher than .arrayLayerCount().
        */
        [[nodiscard]] Optional<std::uint64_t> arrayLayerSize(std::uint64_t mipLevelIndex) const noexcept;

        /*
            Returns a pointer to the image-data at the specified array layer at the specified mip level.

            Returns a non-value optional if:
             - If mipLevelIndex is equal to or higher than .mipLevelCount().
             - If arrayLayerIndex is equal to or higher than .arrayLayerCount().
        */
        [[nodiscard]] Optional<const std::byte*> arrayLayerData(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept;

        /*
            Returns a span to the image-data of the specified array layer at the specified mip level.

            Returns a nullptr, zero-size span if:
            - If mipLevelIndex is equal to or higher than .mipLevelCount().
            - If arrayLayerIndex is equal to or higher than .arrayLayerCount().
        */
        [[nodiscard]] Optional<ConstByteSpan> arrayLayerSpan(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept;

        /*
            Returns the pointer to the internal buffer of the Texture object.

            Returns a nullptr if the Texture object is invalid.
        */
        [[nodiscard]] const std::byte* rawBufferData() const noexcept;

        /*
            Returns the size of the internal buffer of the Texture object.
        */
        [[nodiscard]] std::uint64_t totalDataSize() const noexcept;

        /*
            Returns a span to the internal buffer of the Texture object.
        */
        [[nodiscard]] ConstByteSpan rawBufferSpan() const noexcept;

    private:
        void deallocateInternalBuffer();

        TextureInfo m_textureInfo{};
        ByteSpan m_buffer = {};
        Allocator* m_allocator = nullptr;

        friend detail::PrivateAccessor;
    };
}