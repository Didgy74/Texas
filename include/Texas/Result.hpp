#pragma once

#include <Texas/ResultType.hpp>

namespace Texas
{
	/*
		Struct that contains a ResultType and an error message.

		Primarily used for error-handling.
	*/
	class Result
	{
	public:
		constexpr Result() noexcept = default;

		constexpr Result(
			ResultType resultType, 
			char const* errorMessage) noexcept;

		[[nodiscard]] constexpr ResultType type() const noexcept;
		[[nodiscard]] constexpr char const* errorMessage() const noexcept;

		[[nodiscard]] constexpr bool isSuccessful() const noexcept;

		constexpr operator bool() const noexcept;

	private:
		ResultType m_type = ResultType::UnknownError;
		char const* m_errorMessage = nullptr;
	};

	constexpr Result::Result(
		ResultType resultType, 
		char const* errorMessage) noexcept :
			m_type(resultType),
			m_errorMessage(errorMessage)
	{
	}

	constexpr ResultType Result::type() const noexcept
	{
		return m_type;
	}

	constexpr char const* Result::errorMessage() const noexcept
	{
		return m_errorMessage;
	}

	constexpr bool Result::isSuccessful() const noexcept
	{
		return m_type == ResultType::Success;
	}

	constexpr Result::operator bool() const noexcept
	{
		return m_type == ResultType::Success;
	}

	constexpr Result successResult = { ResultType::Success, nullptr };
}