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

	// Defines a range of bytes
	class ConstByteSpan
	{
	private:
		const std::byte* m_data = nullptr;
		std::size_t m_size = 0;

	public:
		[[nodiscard]] inline constexpr ConstByteSpan() noexcept {}

		[[nodiscard]] inline constexpr ConstByteSpan(const std::byte* data, std::size_t size) noexcept :
			m_data(data),
			m_size(size)
		{}

		[[nodiscard]] inline constexpr const std::byte* data() const noexcept;

		[[nodiscard]] inline constexpr std::size_t size() const noexcept;
	};

	inline constexpr const std::byte* ConstByteSpan::data() const noexcept
	{
		return m_data;
	}

	inline constexpr std::size_t ConstByteSpan::size() const noexcept
	{
		return m_size;
	}

	// Defines a range of bytes
	class ByteSpan
	{
	private:
		std::byte* m_data = nullptr;
		std::size_t m_size = 0;

	public:
		[[nodiscard]] inline constexpr ByteSpan() noexcept {}

		[[nodiscard]] inline constexpr ByteSpan(std::byte* data, std::size_t size) noexcept :
			m_data(data),
			m_size(size)
		{}

		[[nodiscard]] inline constexpr ConstByteSpan toConst() const;

		[[nodiscard]] inline constexpr operator ConstByteSpan() const;

		[[nodiscard]] inline constexpr std::byte* data() const noexcept;

		[[nodiscard]] inline constexpr std::size_t size() const noexcept;
	};

	inline constexpr ConstByteSpan ByteSpan::toConst() const
	{
		return ConstByteSpan(m_data, m_size);
	}

	inline constexpr ByteSpan::operator ConstByteSpan() const
	{
		return ConstByteSpan(m_data, m_size);
	}

	inline constexpr std::byte* ByteSpan::data() const noexcept
	{
		return m_data;
	}

	inline constexpr std::size_t ByteSpan::size() const noexcept
	{
		return m_size;
	}
}