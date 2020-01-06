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
        explicit LoadResult(ResultType result, const char* errorMessage);
        LoadResult(const T&) = delete;
        explicit LoadResult(T&& data);

        ~LoadResult();

        ResultType resultType() const;
        const char* errorMessage() const;

        /*
            Returns the loaded struct.

            Warning! Using this method when isSuccessful() returns false will result in undefined behavior.
        */
        T& value();

        /*
            Returns the loaded struct.

            Warning! Using this method when isSuccessful() returns false will result in undefined behavior.
        */
        const T& value() const;

        /*
            Returns true if ResultType() returns ResultType::Success.
        */
        bool isSuccessful() const;

        /*
            Does the same as isSuccessful().
        */
        operator bool() const;

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
    LoadResult<T>::LoadResult(ResultType result, const char* errorMessage) :
        m_resultType(result),
        m_errorMessage(errorMessage),
        m_pad(std::uint8_t())
    {
    }

    template<typename T>
    LoadResult<T>::LoadResult(T&& in) :
        m_resultType(ResultType::Success),
        m_errorMessage(nullptr),
        m_value(static_cast<T&&>(in))
    {
    }

    template<typename T>
    LoadResult<T>::~LoadResult()
    {
        if (m_resultType == ResultType::Success)
            m_value.~T();
    }

    template<typename T>
    ResultType LoadResult<T>::resultType() const
    {
        return m_resultType;
    }

    template<typename T>
    const char* LoadResult<T>::errorMessage() const
    {
        return m_errorMessage;
    }

    /*
        Returns the loaded struct.

        Warning! Using this method when isSuccessful() returns false will result in undefined behavior.
    */
    template<typename T>
    T& LoadResult<T>::value()
    {
        return m_value;
    }

    template<typename T>
    const T& LoadResult<T>::value() const
    {
        return m_value;
    }

    template<typename T>
    bool LoadResult<T>::isSuccessful() const
    {
        return m_resultType == ResultType::Success;
    }

    template<typename T>
    LoadResult<T>::operator bool() const
    {
        return m_resultType == ResultType::Success;
    }
}