#pragma once

namespace Texas
{
	template<typename T, typename U>
	class Pair
	{
	public:
		T m_a;
		U m_b;

		[[nodiscard]] inline constexpr T& a() noexcept;
		[[nodiscard]] inline constexpr const T& a() const noexcept;

		[[nodiscard]] inline constexpr U& b() noexcept;
		[[nodiscard]] inline constexpr const T& b() const noexcept;
	};

	template<typename T, typename U>
	[[nodiscard]] inline constexpr T& Pair<T, U>::a() noexcept
	{
		return m_a;
	}

	template<typename T, typename U>
	[[nodiscard]] inline constexpr const T& Pair<T, U>::a() const noexcept
	{
		return m_a;
	}

	template<typename T, typename U>
	[[nodiscard]] inline constexpr U& Pair<T, U>::b() noexcept
	{
		return m_b;
	}

	template<typename T, typename U>
	[[nodiscard]] inline constexpr const T& Pair<T, U>::b() const noexcept
	{
		return m_b;
	}
}