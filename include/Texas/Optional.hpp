#pragma once

#include "Texas/detail/Assert.hpp"

namespace Texas
{
	template<typename T>
	class Optional
	{
	public:
		inline constexpr Optional() noexcept;
		inline constexpr Optional(const T&) = delete;
		inline constexpr Optional(T&& in) noexcept;
		inline ~Optional() noexcept;

		[[nodiscard]] inline constexpr bool hasValue() const noexcept;

		[[nodiscard]] inline constexpr T& value() noexcept;

		[[nodiscard]] inline constexpr const T& value() const noexcept;

	private:
		bool m_hasValue = false;
		union
		{
			char m_maybeUnused{};
			T m_value;
		};
	};

	template<typename T>
	inline constexpr Optional<T>::Optional() noexcept
	{
	}

	template<typename T>
	inline constexpr Optional<T>::Optional(T&& in) noexcept :
		m_hasValue(true),
		m_value(static_cast<T&&>(in))
	{
	}

	template<typename T>
	inline Optional<T>::~Optional() noexcept
	{
		if (m_hasValue == true)
		{
			m_value.~T();
		}
	}

	template<typename T>
	inline constexpr bool Optional<T>::hasValue() const noexcept
	{
		return m_hasValue;
	}

	template<typename T>
	inline constexpr T& Optional<T>::value() noexcept
	{
		TEXAS_DETAIL_ASSERT_MSG(m_hasValue == true, "Attempted to access the value of a Texas::Optional<T> without a value.");
		return m_value;
	}

	template<typename T>
	inline constexpr const T& Optional<T>::value() const noexcept
	{
		TEXAS_DETAIL_ASSERT_MSG(m_hasValue == true, "Attempted to access the value of a Texas::Optional<T> without a value.");
		return m_value;
	}
}