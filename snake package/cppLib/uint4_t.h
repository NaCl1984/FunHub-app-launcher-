#pragma once

#include <cstdint>
#include <type_traits>

struct alignas(1) uint4_t {
private:
    uint8_t m_val = 0;

    template <typename T>
    static constexpr uint8_t clamp(T v) noexcept {
        return static_cast<uint8_t>(v) & 0x0F;
    }

public:
    // Конструкторы
    constexpr uint4_t() noexcept = default;

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr uint4_t(T v) noexcept : m_val(clamp(v)) {}

    constexpr uint4_t(const uint4_t&) noexcept = default;
    constexpr uint4_t& operator=(const uint4_t&) noexcept = default;

    constexpr operator uint8_t() const noexcept { return m_val; }
    // constexpr operator int() const noexcept { return static_cast<int>(m_val); }

    // Оператор присваивания для любых чисел
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    constexpr uint4_t& operator=(T v) noexcept {
        m_val = clamp(v);
        return *this;
    }

    // Инкремент / Декремент с кольцевым обнулением (0..15)
    constexpr uint4_t& operator++() noexcept { m_val = clamp(m_val + 1); return *this; }
    constexpr uint4_t operator++(int) noexcept { uint4_t tmp = *this; ++(*this); return tmp; }
    constexpr uint4_t& operator--() noexcept { m_val = clamp(m_val - 1); return *this; }
    constexpr uint4_t operator--(int) noexcept { uint4_t tmp = *this; --(*this); return tmp; }

    // Составное присваивание (поддерживает int, uint8_t, uint4_t и т.д.)
    template <typename T>
    constexpr uint4_t& operator+=(T rhs) noexcept { m_val = clamp(m_val + rhs); return *this; }

    template <typename T>
    constexpr uint4_t& operator-=(T rhs) noexcept { m_val = clamp(m_val - rhs); return *this; }

    template <typename T>
    constexpr uint4_t& operator*=(T rhs) noexcept { m_val = clamp(m_val * rhs); return *this; }

    template <typename T>
    constexpr uint4_t& operator/=(T rhs) noexcept { if (rhs != 0) m_val = clamp(m_val / rhs); return *this; }

    template <typename T>
    constexpr uint4_t& operator%=(T rhs) noexcept { if (rhs != 0) m_val = clamp(m_val % rhs); return *this; }

    template <typename T>
    constexpr uint4_t& operator&=(T rhs) noexcept { m_val = clamp(m_val & rhs); return *this; }

    template <typename T>
    constexpr uint4_t& operator|=(T rhs) noexcept { m_val = clamp(m_val | rhs); return *this; }

    template <typename T>
    constexpr uint4_t& operator^=(T rhs) noexcept { m_val = clamp(m_val ^ rhs); return *this; }

    template <typename T>
    constexpr uint4_t& operator<<=(T rhs) noexcept { m_val = clamp(m_val << rhs); return *this; }

    template <typename T>
    constexpr uint4_t& operator>>=(T rhs) noexcept { m_val = clamp(m_val >> rhs); return *this; }
};

// Проверка размера во время компиляции
static_assert(sizeof(uint4_t) == 1, "uint4_t must take exactly 1 byte of memory!");