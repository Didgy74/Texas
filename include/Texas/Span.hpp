#pragma once

#include <cstddef>

namespace Texas
{
	template<typename T>
	class Span
	{
	private:
		T* m_data = nullptr;
		std::size_t m_size = 0;

	public:
		[[nodiscard]] inline constexpr Span() noexcept {}

		[[nodiscard]] inline constexpr Span(T* data, std::size_t size) noexcept :
			m_data(data),
			m_size(size)
		{}

		inline constexpr operator Span<const T>() const noexcept;

		[[nodiscard]] inline constexpr T* data() const noexcept;

		[[nodiscard]] inline constexpr std::size_t size() const noexcept;
	};

	template<typename T>
	inline constexpr Span<T>::operator Span<const T>() const noexcept
	{
		return Span<const T>(m_data, m_size);
	}

	template<typename T>
	inline constexpr T* Span<T>::data() const noexcept
	{
		return m_data;
	}

	template<typename T>
	inline constexpr std::size_t Span<T>::size() const noexcept
	{
		return m_size;
	}
}