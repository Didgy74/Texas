#pragma once

#include "Texas/ResultType.hpp"

namespace Texas
{
    /*
        Small struct that contains a ResultType and an error message.
    */
    class Result
    {
    public:
        inline constexpr Result() = default;

        inline constexpr Result(ResultType resultType, char const* errorMessage) :
            m_resultType(resultType),
            m_errorMessage(errorMessage)
        {
        }

        [[nodiscard]] inline constexpr ResultType resultType() const noexcept
        {
            return m_resultType;
        }

        [[nodiscard]] inline constexpr char const* errorMessage() const noexcept
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
        ResultType m_resultType = ResultType::Success;
        char const* m_errorMessage = nullptr;
    };
}