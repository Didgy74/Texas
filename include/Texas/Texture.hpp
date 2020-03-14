#pragma once

#include "Texas/TextureInfo.hpp"
#include "Texas/Allocator.hpp"
#include "Texas/Span.hpp"

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
        Texture(Texture const&) = delete;
        Texture(Texture&&) noexcept;

        Texture& operator=(Texture const&) = delete;
        Texture& operator=(Texture&&) noexcept;

        ~Texture();

        [[nodiscard]] TextureInfo const& textureInfo() const;
        [[nodiscard]] FileFormat fileFormat() const;
        [[nodiscard]] TextureType textureType() const;
        [[nodiscard]] PixelFormat pixelFormat() const;
        [[nodiscard]] ChannelType channelType() const;
        [[nodiscard]] ColorSpace colorSpace() const;
        [[nodiscard]] Dimensions baseDimensions() const;
        [[nodiscard]] std::uint8_t mipCount() const;
        [[nodiscard]] std::uint64_t layerCount() const;

        /*
            Returns the offset from the start of the imagedata to the specified mip index.

            Causes undefined behavior if: 
             - If mipIndex is equal to or higher than .mipCount().
        */
        [[nodiscard]] std::uint64_t mipOffset(std::uint8_t mipIndex) const;

        /*
            Returns a span to the imagedata of the specified mip index.

            Causes undefined behavior if: 
            - If mipIndex is equal to or higher than .mipCount().
        */
        [[nodiscard]] ConstByteSpan mipSpan(std::uint8_t mipIndex) const;

        /*
            Returns the offset from the start the imagedata to the specified layer.

            Causes undefined behavior if: 
            - If mipIndex is equal to or higher than .mipCount().
            - If layerIndex is equal to or higher than .layerCount().
        */
        [[nodiscard]] std::uint64_t layerOffset(std::uint8_t mipIndex, std::uint64_t layerIndex) const;

        /*
            Returns a span to the image-data of the specified layer at the specified mip.

            Causes undefined behavior if: 
            - If mipIndex is equal to or higher than .mipCount().
            - If layerIndex is equal to or higher than .layerCount().
        */
        [[nodiscard]] ConstByteSpan layerSpan(std::uint8_t mipIndex, std::uint64_t layerIndex) const;

        /*
            Returns a span to the internal buffer of the Texture object.
        */
        [[nodiscard]] ConstByteSpan rawBufferSpan() const;

    private:
        [[nodiscard]] std::uint64_t mipSize(std::uint8_t mipIndex) const;
        [[nodiscard]] std::byte const* mipData(std::uint8_t mipIndex) const;
        [[nodiscard]] std::uint64_t layerSize(std::uint8_t mipIndex) const;
        [[nodiscard]] std::byte const* layerData(std::uint8_t mipIndex, std::uint64_t layerIndex) const;
        [[nodiscard]] std::byte const* rawBufferData() const;
        [[nodiscard]] std::uint64_t totalDataSize() const;

        void deallocateInternalBuffer();

        TextureInfo m_textureInfo{};
        ByteSpan m_buffer = {};
        Allocator* m_allocator = nullptr;

        friend detail::PrivateAccessor;
    };
}