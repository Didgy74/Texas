#pragma once

#include "Texas/MetaData.hpp"
#include "Texas/Dimensions.hpp"

// Include detail headers
#include "Texas/detail/MemReqs_BackendData.hpp"
#include "Texas/detail/PrivateAccessor_Declaration.hpp"

#include <cstdint>
#include <cstddef>

namespace Texas
{
    /*
        Contains info on an image parsed from a buffer.

        This includes metadata and some fileformat-specific data for loading
        imagedata from said fileformat.
    */
    class MemReqs
    {
    public:
        [[nodiscard]] inline constexpr const MetaData& metaData() const noexcept;

        [[nodiscard]] inline constexpr Dimensions baseDimensions() const noexcept;

        [[nodiscard]] inline constexpr std::uint64_t memoryRequired() const noexcept;

        [[nodiscard]] inline constexpr std::uint64_t workingMemoryRequired() const noexcept;

        //[[nodiscard]] std::uint64_t mipOffset(std::uint64_t mipIndex) const noexcept;

    private:
        MemReqs() = default;

        MetaData m_metaData = {};
        std::uint64_t m_memoryRequired = 0;
        std::uint64_t m_workingMemoryRequired = 0;

        mutable detail::MemReqs_BackendData m_backendData{};

        friend class detail::PrivateAccessor;
    };

    inline constexpr const MetaData& MemReqs::metaData() const noexcept
    {
        return m_metaData;
    }

    inline constexpr Dimensions MemReqs::baseDimensions() const noexcept
    {
        return m_metaData.baseDimensions;
    }

    inline constexpr std::uint64_t MemReqs::memoryRequired() const noexcept
    {
        return m_memoryRequired;
    }

    inline constexpr std::uint64_t MemReqs::workingMemoryRequired() const noexcept
    {
        return m_workingMemoryRequired;
    }
}