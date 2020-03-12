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

        [[nodiscard]] const TextureInfo& textureInfo() const;
        [[nodiscard]] FileFormat fileFormat() const;
        [[nodiscard]] TextureType textureType() const;
        [[nodiscard]] PixelFormat pixelFormat() const;
        [[nodiscard]] ChannelType channelType() const;
        [[nodiscard]] ColorSpace colorSpace() const;
        [[nodiscard]] Dimensions baseDimensions() const;
        [[nodiscard]] std::uint64_t mipLevelCount() const;
        [[nodiscard]] std::uint64_t arrayLayerCount() const;

        /*
            Returns the offset from the start the imagedata to the specified mip level index.

            Causes undefined behavior if: 
             - If mipLevelIndex is equal to or higher than .mipLevelCount().
        */
        [[nodiscard]] std::uint64_t mipOffset(std::uint64_t mipLevelIndex) const;

        /*
            Returns the size of the all the image-data at the specified mip level index.

            Causes undefined behavior if: 
             - If mipLevelIndex is equal to or higher than .mipLevelCount().
        */
        [[nodiscard]] std::uint64_t mipSize(std::uint64_t mipLevelIndex) const;

        /*
            Returns a pointer to the image-data at the specified mip level index.

            Causes undefined behavior if: 
             - If mipLevelIndex is equal to or higher than .mipLevelCount().
        */
        [[nodiscard]] const std::byte* mipData(std::uint64_t mipLevelIndex) const;

        /*
            Returns a span to the image-data of the specified mip level index.

            Causes undefined behavior if: 
            - If mipLevelIndex is equal to or higher than .mipLevelCount().
        */
        [[nodiscard]] ConstByteSpan mipSpan(std::uint64_t mipLevelIndex) const;

        /*
            Returns the offset from the start the imagedata to the specified array layer.

            Causes undefined behavior if: 
            - If mipLevelIndex is equal to or higher than .mipLevelCount().
            - If arrayLayerIndex is equal to or higher than .arrayLayerCount().
        */
        [[nodiscard]] std::uint64_t layerOffset(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const;

        /*
            Returns the size of the specified array layer at the specified mip level.

            Causes undefined behavior if: 
            - If mipLevelIndex is equal to or higher than .mipLevelCount().
            - If arrayLayerIndex is equal to or higher than .arrayLayerCount().
        */
        [[nodiscard]] std::uint64_t layerSize(std::uint64_t mipLevelIndex) const;

        /*
            Returns a pointer to the image-data at the specified array layer at the specified mip level.

            Causes undefined behavior if: 
             - If mipLevelIndex is equal to or higher than .mipLevelCount().
             - If arrayLayerIndex is equal to or higher than .arrayLayerCount().
        */
        [[nodiscard]] const std::byte* layerData(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const;

        /*
            Returns a span to the image-data of the specified array layer at the specified mip level.

            Causes undefined behavior if: 
            - If mipLevelIndex is equal to or higher than .mipLevelCount().
            - If arrayLayerIndex is equal to or higher than .arrayLayerCount().
        */
        [[nodiscard]] ConstByteSpan layerSpan(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const;

        /*
            Returns the pointer to the internal buffer of the Texture object.
        */
        [[nodiscard]] const std::byte* rawBufferData() const;

        /*
            Returns the size of the internal buffer of the Texture object.
        */
        [[nodiscard]] std::uint64_t totalDataSize() const;

        /*
            Returns a span to the internal buffer of the Texture object.
        */
        [[nodiscard]] ConstByteSpan rawBufferSpan() const;

    private:
        void deallocateInternalBuffer();

        TextureInfo m_textureInfo{};
        ByteSpan m_buffer = {};
        Allocator* m_allocator = nullptr;

        friend detail::PrivateAccessor;
    };
}