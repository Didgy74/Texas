#pragma once

#include "Texas/Result.hpp"

namespace Texas
{
    /*
        Holds a Texas::Result, and an optional value.
        If .isSuccessful() returns true, the struct has a valid value.

        Accessing .value() is UB when .resultType() does not equal ResultType::Success.
    */
    template<typename T>
    class ResultValue
    {
    public:
        ResultValue() noexcept = default;
        ResultValue(ResultValue<T> const&) noexcept;
        ResultValue(ResultValue<T>&&) noexcept;
        ResultValue(T const&) noexcept;
        ResultValue(T&& data) noexcept;
        /*
            Passing in a Result with ResultType equal to Result::Success
            is undefined behavior.
        */
        ResultValue(Result result) noexcept;
        /*
            Passing in ResultType with value equal to Result::Success
            is undefined behavior.
        */
        ResultValue(ResultType resultType, char const* errorMessage) noexcept;
        ~ResultValue() noexcept;

        [[nodiscard]] ResultType resultType() const noexcept;
        [[nodiscard]] char const* errorMessage() const noexcept;

        /*
            Returns the stored value.

            Causes undefined behavior if:
             - .isSuccessful() returns false
        */
        [[nodiscard]] T& value() noexcept;

        /*
            Returns the stored value.

            Causes undefined behavior if:
             - .isSuccessful() returns false
        */
        [[nodiscard]] T const& value() const noexcept;

        /*
            Returns true if .resultType() returns ResultType::Success.
        */
        [[nodiscard]] bool isSuccessful() const noexcept;

        /*
            Does the same as .isSuccessful()
        */
        [[nodiscard]] operator bool() const noexcept;

        [[nodiscard]] Result toResult() const noexcept;

        [[nodiscard]] operator Result() const noexcept;

    private:
        Result m_result{ ResultType::UnknownError, nullptr };
        union
        {
            unsigned char m_valueBuffer = {};
            T m_value;
        };
    };

    template<typename T>
    ResultValue<T>::ResultValue(ResultValue<T> const& other) noexcept
    {
        m_result = other.m_result;
        if (m_result.isSuccessful())
        {
            new(m_value) T(other);
        }
    }

    template<typename T>
    ResultValue<T>::ResultValue(ResultValue<T>&& other) noexcept
    {
        m_result = other.m_result;
        if (other.m_result.isSuccessful())
        {
            new(m_value) T(static_cast<T&&>(other.m_value));
        }
    }

    template<typename T>
    ResultValue<T>::ResultValue(Result in) noexcept :
        m_result(in),
        m_valueBuffer()
    {
    }

    template<typename T>
    ResultValue<T>::ResultValue(ResultType resultType, const char* errorMessage) noexcept :
        m_result(resultType, errorMessage),
        m_valueBuffer()
    {
    }

    template<typename T>
    ResultValue<T>::ResultValue(T&& in) noexcept :
        m_result(ResultType::Success, nullptr),
        m_value(static_cast<T&&>(in))
    {
    }

    template<typename T>
    ResultValue<T>::~ResultValue() noexcept
    {
        if (isSuccessful())
            m_value.~T();
    }

    template<typename T>
    ResultType ResultValue<T>::resultType() const noexcept
    {
        return m_result.type();
    }

    template<typename T>
    char const* ResultValue<T>::errorMessage() const noexcept
    {
        return m_result.errorMessage();
    }

    /*
        Returns the loaded struct.

        Warning! Using this method when isSuccessful() returns false will result in undefined behavior.
    */
    template<typename T>
    T& ResultValue<T>::value() noexcept
    {
        return m_value;
    }

    template<typename T>
    T const& ResultValue<T>::value() const noexcept
    {
        return m_value;
    }

    template<typename T>
    bool ResultValue<T>::isSuccessful() const noexcept
    {
        return m_result.isSuccessful();
    }

    template<typename T>
    ResultValue<T>::operator bool() const noexcept
    {
        return isSuccessful();
    }

    template<typename T>
    Result ResultValue<T>::toResult() const noexcept
    {
        return m_result;
    }

    template<typename T>
    ResultValue<T>::operator Result() const noexcept
    {
        return toResult();
    }
}