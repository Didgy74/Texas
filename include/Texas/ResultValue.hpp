#pragma once

#include "Texas/ResultType.hpp"
#include "Texas/Result.hpp"
#include "Texas/detail/Assert.hpp"

#include <cstdint>

namespace Texas
{
    namespace detail
    {
        class PrivateAccessor;
    }

    /*
        Holds a Result, and an optional value.
        If .resultType() equals ResultType::Successful, the struct has a valid value.
        Accessing .value() is UB when .resultType() does not equal ResultType::Success.
    */
    template<typename T>
    class ResultValue
    {
    public:
        ResultValue() = delete;
        ResultValue(const T&) = delete;
        ResultValue(T&& data) noexcept;

        ~ResultValue() noexcept;

        ResultType resultType() const noexcept;
        const char* errorMessage() const noexcept;

        /*
            Returns the loaded struct.

            Warning! Using this method when isSuccessful() returns false will result in undefined behavior.
        */
        T& value() noexcept;

        /*
            Returns the loaded struct.

            Warning! Using this method when isSuccessful() returns false will result in undefined behavior.
        */
        const T& value() const noexcept;

        /*
            Returns true if .resultType() returns ResultType::Success.
        */
        bool isSuccessful() const noexcept;

        /*
            Does the same as isSuccessful().
        */
        operator bool() const noexcept;

        Result toResult() const noexcept;

        operator Result() const noexcept;

    private:
        /*
            Note! The parameter "result" cannot be ResultType::Success.
            Passing ResultType::Success will mean either UB or will call abort through the Texas assert.
        */
        ResultValue(Result result) noexcept;

        /*
            Note! The parameter "result" cannot be ResultType::Success.
            Passing ResultType::Success will mean either UB or will call abort through the Texas assert.
        */
        ResultValue(ResultType resultType, const char* errorMessage) noexcept;

        Result m_result{ ResultType::Success, "" };
        union
        {
            std::uint8_t m_maybeUnused{};
            T m_value;
        };

        friend detail::PrivateAccessor;
    };

    template<typename T>
    ResultValue<T>::ResultValue(Result in) noexcept :
        m_result(in),
        m_maybeUnused(std::uint8_t())
    {
        TEXAS_DETAIL_ASSERT_MSG(m_result.resultType() != ResultType::Success, "Author error. Passed ResultType::Success and no value into LoadResult<T>.");
    }

    template<typename T>
    ResultValue<T>::ResultValue(ResultType resultType, const char* errorMessage) noexcept :
        m_result(resultType, errorMessage),
        m_maybeUnused(std::uint8_t())
    {
        TEXAS_DETAIL_ASSERT_MSG(m_result.resultType() != ResultType::Success, "Author error. Passed ResultType::Success and no value into LoadResult<T>.");
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
        return m_result.resultType();
    }

    template<typename T>
    const char* ResultValue<T>::errorMessage() const noexcept
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
        TEXAS_DETAIL_ASSERT_MSG(isSuccessful(), "Tried to access value of Texas::LoadResult<T> object without a value.");
        return m_value;
    }

    template<typename T>
    const T& ResultValue<T>::value() const noexcept
    {
        TEXAS_DETAIL_ASSERT_MSG(isSuccessful(), "Tried to access value of LoadResult<T> object without a value.");
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