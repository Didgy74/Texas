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
		inline constexpr Span() noexcept = default;
		inline constexpr Span(T* data, std::size_t size) noexcept;

		[[nodiscard]] inline constexpr T* data() const noexcept;
		[[nodiscard]] inline constexpr std::size_t size() const noexcept;

		[[nodiscard]] inline constexpr Span<T const> asConst() const noexcept;
		[[nodiscard]] inline constexpr operator Span<T const>() const noexcept;
	};

	template<typename T>
	inline constexpr Span<T>::Span(T* data, std::size_t size) noexcept :
		m_data(data),
		m_size(size)
	{
	}

	template<typename T>
	inline constexpr T* Span<T>::data() const noexcept { return m_data; }

	template<typename T>
	inline constexpr std::size_t Span<T>::size() const noexcept { return m_size; }

	template<typename T>
	inline constexpr Span<T const> Span<T>::asConst() const noexcept
	{
		return { m_data, m_size };
	}

	template<typename T>
	inline constexpr Span<T>::operator Span<T const>() const noexcept
	{
		return asConst();
	}

	template<typename T>
	class Span<T const>
	{
	private:
		T const* m_data = nullptr;
		std::size_t m_size = 0;

	public:
		inline constexpr Span() noexcept = default;
		inline constexpr Span(T const* data, std::size_t size) noexcept;

		[[nodiscard]] inline constexpr T const* data() const noexcept;
		[[nodiscard]] inline constexpr std::size_t size() const noexcept;
	};

	template<typename T>
	inline constexpr Span<T const>::Span(T const* data, std::size_t size) noexcept :
		m_data(data),
		m_size(size)
	{
	}

	template<typename T>
	inline constexpr T const* Span<T const>::data() const noexcept { return m_data; }

	template<typename T>
	inline constexpr std::size_t Span<T const>::size() const noexcept { return m_size; }

	// Defines a range of bytes
	using ByteSpan = Span<std::byte>;

	// Defines a range of immutable bytes
	using ConstByteSpan = Span<std::byte const>;

}