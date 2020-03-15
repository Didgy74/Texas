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
            m_type(resultType),
            m_errorMessage(errorMessage)
        {
        }

        [[nodiscard]] inline constexpr ResultType type() const noexcept
        {
            return m_type;
        }

        [[nodiscard]] inline constexpr char const* errorMessage() const noexcept
        {
            return m_errorMessage;
        }

        [[nodiscard]] inline constexpr bool isSuccessful() const noexcept
        {
            return m_type == ResultType::Success;
        }

        inline constexpr operator bool() const noexcept
        {
            return m_type == ResultType::Success;
        }

    private:
        ResultType m_type = ResultType::UnknownError;
        char const* m_errorMessage = nullptr;
    };
}