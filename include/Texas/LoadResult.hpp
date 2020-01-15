#pragma once

#include "Texas/ResultType.hpp"

#include <cstdint>

namespace Texas
{
    namespace detail
    {
        class PrivateAccessor;
    }

    template<typename T>
    class LoadResult
    {
    public:
        LoadResult() = delete;
        LoadResult(ResultType result, const char* errorMessage) noexcept;
        LoadResult(const T&) = delete;
        LoadResult(T&& data) noexcept;

        ~LoadResult() noexcept;

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
            Returns true if ResultType() returns ResultType::Success.
        */
        bool isSuccessful() const noexcept;

        /*
            Does the same as isSuccessful().
        */
        operator bool() const noexcept;

    private:
        ResultType m_resultType = ResultType::Success;
        const char* m_errorMessage = nullptr;
        union
        {
            std::uint8_t m_pad{};
            T m_value;
        };
    };

    template<typename T>
    LoadResult<T>::LoadResult(ResultType result, const char* errorMessage) noexcept :
        m_resultType(result),
        m_errorMessage(errorMessage),
        m_pad(std::uint8_t())
    {
    }

    template<typename T>
    LoadResult<T>::LoadResult(T&& in) noexcept :
        m_resultType(ResultType::Success),
        m_errorMessage(nullptr),
        m_value(static_cast<T&&>(in))
    {
    }

    template<typename T>
    LoadResult<T>::~LoadResult() noexcept
    {
        if (m_resultType == ResultType::Success)
            m_value.~T();
    }

    template<typename T>
    ResultType LoadResult<T>::resultType() const noexcept
    {
        return m_resultType;
    }

    template<typename T>
    const char* LoadResult<T>::errorMessage() const noexcept
    {
        return m_errorMessage;
    }

    /*
        Returns the loaded struct.

        Warning! Using this method when isSuccessful() returns false will result in undefined behavior.
    */
    template<typename T>
    T& LoadResult<T>::value() noexcept
    {
        return m_value;
    }

    template<typename T>
    const T& LoadResult<T>::value() const noexcept
    {
        return m_value;
    }

    template<typename T>
    bool LoadResult<T>::isSuccessful() const noexcept
    {
        return m_resultType == ResultType::Success;
    }

    template<typename T>
    LoadResult<T>::operator bool() const noexcept
    {
        return m_resultType == ResultType::Success;
    }
}