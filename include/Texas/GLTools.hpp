#pragma once


#include <Texas/ChannelType.hpp>
#include <Texas/ColorSpace.hpp>
#include <Texas/TextureInfo.hpp>
#include <Texas/TextureType.hpp>
#include <Texas/PixelFormat.hpp>

#include <cstdint>

namespace Texas
{
    [[nodiscard]] PixelFormat GLToPixelFormat(std::uint32_t GLInternalFormat) noexcept;
    [[nodiscard]] ColorSpace GLToColorSpace(std::uint32_t GLInternalFormat) noexcept;
    [[nodiscard]] ChannelType GLToChannelType(std::uint32_t GLInternalFormat) noexcept;

    [[nodiscard]] std::uint32_t toGLTarget(TextureType texType) noexcept;
    [[nodiscard]] std::uint32_t toGLType(PixelFormat pFormat, ChannelType chType) noexcept;
    [[nodiscard]] std::uint32_t toGLTypeSize(PixelFormat pFormat) noexcept;
    [[nodiscard]] std::uint32_t toGLFormat(PixelFormat pFormat) noexcept;
    [[nodiscard]] std::uint32_t toGLInternalFormat(
      PixelFormat pFormat, 
      ColorSpace cSpace, 
      ChannelType chType) noexcept;
    [[nodiscard]] std::uint32_t toGLInternalFormat(TextureInfo const& texInfo) noexcept;
    [[nodiscard]] std::uint32_t toGLBaseInternalFormat(PixelFormat pFormat) noexcept;
}