#pragma once

namespace Texas
{
	class Result
	{
	public:
		inline constexpr Result(ResultType resultType, const char* errorMessage) :
			m_resultType(resultType),
			m_errorMessage(errorMessage)
		{
		}

		[[nodiscard]] inline constexpr ResultType resultType() const noexcept
		{
			return m_resultType;
		}

		[[nodiscard]] inline constexpr const char* errorMessage() const noexcept
		{
			return m_errorMessage;
		}

		[[nodiscard]] inline constexpr bool isSuccessful() const noexcept
		{
			return m_resultType == ResultType::Success;
		}

		inline constexpr operator bool() const noexcept
		{
			return m_resultType == ResultType::Success;
		}

	private:
		ResultType m_resultType;
		const char* m_errorMessage;
	};
}