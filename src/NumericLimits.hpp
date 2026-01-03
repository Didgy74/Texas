#pragma once

#include <cstdint>

namespace Texas::detail
{
	template<typename Integer_T>
	[[nodiscard]] constexpr Integer_T maxValue() noexcept = delete;

	template<>
	[[nodiscard]] constexpr std::uint32_t maxValue<std::uint32_t>() noexcept { return static_cast<std::uint32_t>(-1); }

	template<>
	[[nodiscard]] constexpr std::uint64_t maxValue<std::uint64_t>() noexcept { return static_cast<std::uint64_t>(-1); }

	// Temporary workaround
#if defined(__clang__) && defined(__apple_build_version__)
	template<>
	[[nodiscard]] constexpr std::size_t maxValue<std::size_t>() noexcept { return static_cast<std::size_t>(-1); }
#endif

}