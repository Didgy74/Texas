#pragma once

#include <cstdint>

namespace Texas::detail
{
	template<typename Integer_T>
	[[nodiscard]] constexpr Integer_T maxValue() = delete;

	template<>
	[[nodiscard]] constexpr std::uint32_t maxValue<std::uint32_t>() noexcept { return static_cast<std::uint32_t>(-1); }

	template<>
	[[nodiscard]] constexpr std::uint64_t maxValue<std::uint64_t>() noexcept { return static_cast<std::uint64_t>(-1); }
}