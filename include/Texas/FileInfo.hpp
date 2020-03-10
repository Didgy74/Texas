#pragma once

#include "Texas/TextureInfo.hpp"
#include "Texas/Dimensions.hpp"

// Include detail headers
#include "Texas/detail/FileInfo_BackendData.hpp"
#include "Texas/detail/PrivateAccessor_Declaration.hpp"

#include <cstdint>
#include <cstddef>

namespace Texas
{
    /*
        Contains info on an image parsed from a buffer.
        This includes metadata and some fileformat-specific data for 
        loading imagedata from said fileformat.
    */
    class FileInfo
    {
    public:
        FileInfo() noexcept = default;

        [[nodiscard]] TextureInfo const & textureInfo() const noexcept;

        [[nodiscard]] Dimensions baseDimensions() const noexcept;

        [[nodiscard]] std::uint64_t memoryRequired() const noexcept;

        [[nodiscard]] std::uint64_t workingMemoryRequired() const noexcept;

    private:
        TextureInfo m_textureInfo = {};
        std::uint64_t m_memoryRequired = 0;
        std::uint64_t m_workingMemoryRequired = 0;

        mutable detail::FileInfo_BackendData m_backendData{};

        friend class detail::PrivateAccessor;
    };
}