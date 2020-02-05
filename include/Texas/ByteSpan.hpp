#pragma once

#include <cstddef>

namespace Texas
{
    template<typename T>
    class Span
    {
    private:
        T const* m_data = nullptr;
        std::size_t m_size = 0;

    public:
        inline constexpr Span() noexcept = default;
        inline constexpr Span(T const* data, std::size_t size) noexcept;

        [[nodiscard]] inline constexpr T const* data() const noexcept;
        [[nodiscard]] inline constexpr std::size_t size() const noexcept;
    };

    template<typename T>
    inline constexpr Span<T>::Span(T const* data, std::size_t size) noexcept :
        m_data(data),
        m_size(size)
    {
    }

    template<typename T>
    inline constexpr T const* Span<T>::data() const noexcept { return m_data; }

    template<typename T>
    inline constexpr std::size_t Span<T>::size() const noexcept { return m_size; }

    // Defines a range of bytes
    class ConstByteSpan
    {
    private:
        const std::byte* m_data = nullptr;
        std::size_t m_size = 0;

    public:
        inline constexpr ConstByteSpan() noexcept {}

        inline constexpr ConstByteSpan(std::byte const* data, std::size_t size) noexcept :
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
        inline constexpr ByteSpan() noexcept {}

        inline constexpr ByteSpan(std::byte* data, std::size_t size) noexcept :
            m_data(data),
            m_size(size)
        {}

        [[nodiscard]] inline constexpr ConstByteSpan toConstSpan() const;

        [[nodiscard]] inline constexpr operator ConstByteSpan() const;

        [[nodiscard]] inline constexpr std::byte* data() const noexcept;

        [[nodiscard]] inline constexpr std::size_t size() const noexcept;
    };

    inline constexpr ConstByteSpan ByteSpan::toConstSpan() const
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