#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>
#include <cassert>
#include <string>

namespace DTex
{
    struct Dimensions
    {
		using ValueType = uint32_t;
		static constexpr size_t dimensionCount = 3;

        uint32_t width;
	    uint32_t height;
	    uint32_t depth;

        constexpr uint32_t& operator[](size_t i);
        constexpr const uint32_t& operator[](size_t i) const;

        constexpr uint32_t& At(size_t i);
        constexpr const uint32_t& At(size_t i) const;
    };
}

inline constexpr uint32_t& DTex::Dimensions::operator[](size_t i) { return const_cast<uint32_t&>(std::as_const(*this)[i]); }
inline constexpr const uint32_t& DTex::Dimensions::operator[](size_t i) const
{
    switch (i)
    {
    case 0:
        return width;
    case 1:
        return height;
    case 2:
        return depth;
    default:
        return width;
    }
}

inline constexpr uint32_t& DTex::Dimensions::At(size_t i) { return const_cast<uint32_t&>(std::as_const(*this).At(i)); }
inline constexpr const uint32_t& DTex::Dimensions::At(size_t i) const
{
    assert(i >= 0 && i < 2);
    switch (i)
    {
    case 0:
        return width;
    case 1:
        return height;
    case 2:
        return depth;
    default:
        assert(false);
        return width;
    }
}