#pragma once

#ifndef __ALT_STD_U128__
#define __ALT_STD_U128__

#include "NumPreInclude.h"

#include "../../Memory/Impl/MemoryPreInclude.h"

namespace Alt {
namespace Types {

class s128;

/**
 * @brief Represents a 128-bit unsigned integer.
 * 
 * @details Provides a implementation of 128-bit unsigned integer, supporting 
 * basic arithmetic, bitwise operations, and comparison logic. It leverages native `__int128` 
 * where available while providing fallback implementations for other environments or when
 * custom 128-bit logic is requested via `ALT_USE_CUSTOM_128`.
 * 
 * @note For Clang use 'clang_rt.builtins-x86_64.lib'/'clang_rt.builtins-i386.lib' or define `ALT_USE_CUSTOM_128`.
 * 
 * @example
 * 
 * const u128 val         = 1000000000000000000ULL;
 * const u128 result = val + 500000000000000000ULL;
 * assert(result ==         1500000000000000000ULL);
 */
class alignas(16) u128 {
public:

    /**
     * @brief Returns the minimum representable value for a unsigned 128-bit integer.
     * @return The smallest possible u128 value.
     * @example
     * 
     * const u128 min = u128::Min(); // Alt::Desc<u128>::Min();
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 Min() noexcept {

        return u128(Num::Desc<u64>::Min(), Num::Desc<u64>::Min());
    }

    /**
     * @brief Returns the maximum representable value for a unsigned 128-bit integer.
     * @return The largest possible u128 value.
     * @example
     * 
     * const u128 max = u128::Max(); // Alt::Desc<u128>::Max();
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 Max() noexcept {

        return u128(Num::Desc<u64>::Max(), Num::Desc<u64>::Max());
    }

    /**
     * @brief Constructs a 128-bit integer from a high and low component.
     * @tparam T The type of the low component; must be `u64`.
     * @param high The upper 64 bits (unsigned).
     * @param low The lower 64 bits (unsigned).
     * @return A new u128 instance.
     * @example
     * 
     * const u128 val = u128::FromHighLow(0xffffffffffffffffULL, 0xffffffffffffffffULL);
     * 
     * assert(val == Alt::Desc<u128>::Max());
     */
    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 FromHighLow(const T high, const T low) noexcept {

        return u128(high, low);
    }

    /**
     * @brief Constructors for u128.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 FromHighLow(const usize high, const usize low) noexcept {

        return u128(static_cast<u64>(high), static_cast<u64>(low));
    }

    ALT_FORCE_INLINE constexpr u128() noexcept = default;

    ALT_FORCE_INLINE constexpr u128(const u128&) noexcept = default;
    ALT_FORCE_INLINE constexpr u128& operator=(const u128&) noexcept = default;

    ALT_FORCE_INLINE constexpr u128(u128&&) noexcept = default;
    ALT_FORCE_INLINE constexpr u128& operator=(u128&&) noexcept = default;

    ALT_FORCE_INLINE constexpr u128(const usize num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit u128(const s128&) noexcept;

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_FORCE_INLINE constexpr explicit u128(const T num) noexcept
        : m_Low(num)
        , m_High(0)
    {}

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_FORCE_INLINE constexpr explicit u128(const T num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit u128(const u16 num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit u128(const u8 num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit u128(const s64 num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit u128(const s32 num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit u128(const s16 num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit u128(const s8 num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit u128(const f64 num_) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = static_cast<__uint128_t>(num_);

        *this = Memory::Transmute<__uint128_t, u128>(x);

#else

        constexpr u64 MANTISSA_SIZE = DBL_MANT_DIG - 1;
        constexpr u64 MANTISSA_MASK = (static_cast<u64>(1) << MANTISSA_SIZE) - 1;
        constexpr u64 EXP_MASK = (static_cast<u64>(1) << 11) - 1;

        const u64 num = Memory::Transmute<f64, u64>(num_);

        u64 exp = (num >> MANTISSA_SIZE) & EXP_MASK;

        if (exp < MANTISSA_SIZE + 1023) {

            static_assert(MANTISSA_SIZE <= 64, "");

            *this = u128(static_cast<u64>(num_));
            return;
        }

        u64 mantissa = num & MANTISSA_MASK;

        exp -= 1023;

        const u64 highBit = static_cast<u64>(1) << MANTISSA_SIZE;
        mantissa |= exp == 0 ? 0 : highBit;

        exp -= MANTISSA_SIZE;

        *this = u128(static_cast<u64>(mantissa)) << static_cast<usize>(exp);

#endif
    }

    ALT_FORCE_INLINE constexpr explicit u128(const f32 num_) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = static_cast<__uint128_t>(num_);

        *this = Memory::Transmute<__uint128_t, u128>(x);

#else

        constexpr u32 MANTISSA_SIZE = FLT_MANT_DIG - 1;
        constexpr u32 MANTISSA_MASK = (static_cast<u32>(1) << MANTISSA_SIZE) - 1;
        constexpr u32 EXP_MASK = (static_cast<u32>(1) << 8) - 1;

        const u32 num = Memory::Transmute<f32, u32>(num_);

        u32 exp = (num >> MANTISSA_SIZE) & EXP_MASK;

        if (exp < MANTISSA_SIZE + 127) {

            static_assert(MANTISSA_SIZE <= 64, "");

            *this = u128(static_cast<u64>(num_));
            return;
        }

        u32 mantissa = num & MANTISSA_MASK;

        exp -= 127;

        const u32 highBit = static_cast<u32>(1) << MANTISSA_SIZE;
        mantissa |= exp == 0 ? 0 : highBit;

        exp -= MANTISSA_SIZE;

        *this = u128(static_cast<u64>(mantissa)) << static_cast<usize>(exp);

#endif
    }

    /**
     * @brief Conversions for u128.
     */
    ALT_FORCE_INLINE constexpr explicit operator s128() noexcept;

    ALT_FORCE_INLINE constexpr explicit operator u64() const noexcept {
        return m_Low;
    }

    ALT_FORCE_INLINE constexpr explicit operator u32() const noexcept {
        return static_cast<u32>(m_Low);
    }

    ALT_FORCE_INLINE constexpr explicit operator u16() const noexcept {
        return static_cast<u16>(m_Low);
    }

    ALT_FORCE_INLINE constexpr explicit operator u8() const noexcept {
        return static_cast<u8>(m_Low);
    }

    ALT_FORCE_INLINE constexpr explicit operator s64() const noexcept {
        return static_cast<s64>(m_Low);
    }

    ALT_FORCE_INLINE constexpr explicit operator s32() const noexcept {
        return static_cast<s32>(m_Low);
    }

    ALT_FORCE_INLINE constexpr explicit operator s16() const noexcept {
        return static_cast<s16>(m_Low);
    }

    ALT_FORCE_INLINE constexpr explicit operator s8() const noexcept {
        return static_cast<s8>(m_Low);
    }

    ALT_FORCE_INLINE constexpr explicit operator f64() const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return static_cast<f64>(x);

#else

        /**
         * IEEE 754 double-precision binary floating-point format: binary64
         * https://en.wikipedia.org/wiki/Double-precision_floating-point_format#IEEE_754_double-precision_binary_floating-point_format:_binary64
         */

        constexpr usize MANTISSA_SIZE = DBL_MANT_DIG - 1;
        constexpr u64 MANTISSA_MASK = (static_cast<u64>(1) << MANTISSA_SIZE) - 1;

        if (m_High == 0) {
            return static_cast<f64>(m_Low);
        }

        const usize zeroes = Num::Count0BitsLeft(m_High);
        const u32 pow2 = static_cast<u32>(127 - zeroes);

        // 2^exp-1023
        u64 exp = pow2 + 1023;

        // 2^exp-1023 * (1 + 2^MANTISSA_SIZE-i + ...)
        const u64 high = m_High << (zeroes + 1);

        const usize shift = 64 - MANTISSA_SIZE;
        const u64 roundMask = (static_cast<u64>(1) << shift) - 1;
        const u64 roundHalf = static_cast<u64>(1) << (shift - 1);

        u64 mantissa = static_cast<u64>(high >> shift);

        const u64 roundBits = high & roundMask;

        // Round to Nearest, ties to even
        const bool isRoundUp = roundBits > roundHalf || (roundBits == roundHalf && (m_Low != 0 || (mantissa & 0b1) == 1));

        mantissa += isRoundUp ? 1 : 0;
        mantissa &= MANTISSA_MASK;

        const bool isMantissaOverflow = isRoundUp && mantissa == 0;

        exp += isMantissaOverflow ? 1 : 0;

        const u64 result = (exp << MANTISSA_SIZE) | mantissa;

        return Memory::Transmute<u64, f64>(result);

#endif
    }

    ALT_FORCE_INLINE constexpr explicit operator f32() const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return static_cast<f32>(x);

#else

        /**
         * IEEE 754 standard: binary32
         * https://en.wikipedia.org/wiki/Single-precision_floating-point_format#IEEE_754_standard:_binary32
         */

        constexpr usize MANTISSA_SIZE = FLT_MANT_DIG - 1;
        constexpr u32 MANTISSA_MASK = (static_cast<u32>(1) << MANTISSA_SIZE) - 1;

        if (m_High == 0) {
            return static_cast<f32>(m_Low);
        }

        const usize zeroes = Num::Count0BitsLeft(m_High);
        const u32 pow2 = static_cast<u32>(127 - zeroes);

        // 2^exp-127
        u32 exp = pow2 + 127;

        // 2^exp-127 * (1 + 2^MANTISSA_SIZE-i + ...)
        const u64 high = m_High << (zeroes + 1);

        const usize shift = 64 - MANTISSA_SIZE;
        const u64 roundMask = (static_cast<u64>(1) << shift) - 1;
        const u64 roundHalf = static_cast<u64>(1) << (shift - 1);

        u32 mantissa = static_cast<u32>(high >> shift);

        const u64 roundBits = high & roundMask;

        // Round to Nearest, ties to even
        const bool isRoundUp = roundBits > roundHalf || (roundBits == roundHalf && (m_Low != 0 || (mantissa & 0b1) == 1));

        mantissa += isRoundUp ? 1 : 0;
        mantissa &= MANTISSA_MASK;

        const bool isMantissaOverflow = isRoundUp && mantissa == 0;

        exp += isMantissaOverflow ? 1 : 0;

        const u32 result = (exp << MANTISSA_SIZE) | mantissa;

        return Memory::Transmute<u32, f32>(result);

#endif
    }

    /**
     * @brief Performs addition of two u128 integers.
     * @param rhs The second u128 operand.
     * @return A new u128 result representing the sum.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u128 y = (u128)20;
     * 
     * const u128 z = x + y;
     * 
     * assert(z == 30);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128 operator+(const u128 rhs) const noexcept {
        return Add(rhs);
    }

    /**
     * @brief Performs addition of an u128 and a 64-bit integer.
     * @param rhs The second operand (u64).
     * @return A new u128 result representing the sum.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * const u128 z = x + y;
     * 
     * assert(z == 30);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128 operator+(const u64 rhs) const noexcept {
        return Add(rhs);
    }

    /**
     * @brief Adds an u128 value to itself in-place.
     * @param rhs The amount to add (u128).
     * @return A reference to this instance updated with the addition.
     * @example
     * 
     * u128 x = (u128)10;
     * const u128 y = (u128)20;
     * 
     * x += y;
     * 
     * assert(x == 30);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128& operator+=(const u128 rhs) noexcept {

        *this = Add(rhs);
        return *this;
    }

    /**
     * @brief Adds an u64 value to itself in-place.
     * @param rhs The amount to add (u64).
     * @return A reference to this instance updated with the addition.
     * @example
     * 
     * u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * x += y;
     * 
     * assert(x == 30);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128& operator+=(const u64 rhs) noexcept {

        *this = Add(rhs);
        return *this;
    }

    /**
     * @brief Adds a 64-bit integer to an u128 value.
     * @param lhs The first operand (u64).
     * @param rhs The second operand (u128).
     * @return A new u128 result representing the addition.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * const u128 z = y + x;
     * 
     * assert(z == 30);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr friend u128 operator+(const u64 lhs, const u128 rhs) noexcept {
        return rhs + lhs;
    }

    /**
     * @brief Performs subtraction of two u128 integers.
     * @param rhs The second u128 operand.
     * @return A new u128 result representing the subtraction.
     * @example
     * 
     * const u128 x = (u128)20;
     * const u128 y = (u128)10;
     * 
     * const u128 z = x - y;
     * 
     * assert(z == 10);
     * assert(x == 20);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr u128 operator-(const u128 rhs) const noexcept {
        return Sub(rhs);
    }

    /**
     * @brief Performs subtraction of an u128 and a 64-bit integer.
     * @param rhs The second operand (u64).
     * @return A new u128 result representing the subtraction.
     * @example
     * 
     * const u128 x = (u128)20;
     * const u64 y = 10;
     * 
     * const u128 z = x - y;
     * 
     * assert(z == 10);
     * assert(x == 20);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr u128 operator-(const u64 rhs) const noexcept {
        return Sub(rhs);
    }

    /**
     * @brief Subtracts an u128 value to itself in-place.
     * @param rhs The amount to subtract (u128).
     * @return A reference to this instance updated with the subtraction.
     * @example
     * 
     * u128 x = (u128)20;
     * const u128 y = (u128)10;
     * 
     * x -= y;
     * 
     * assert(x == 10);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr u128& operator-=(const u128 rhs) noexcept {

        *this = Sub(rhs);
        return *this;
    }

    /**
     * @brief Subtracts an u64 value to itself in-place.
     * @param rhs The amount to subtract (u64).
     * @return A reference to this instance updated with the subtraction.
     * @example
     * 
     * u128 x = (u128)20;
     * const u64 y = 10;
     * 
     * x -= y;
     * 
     * assert(x == 10);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr u128& operator-=(const u64 rhs) noexcept {

        *this = Sub(rhs);
        return *this;
    }

    /**
     * @brief Subtracts a 64-bit integer to an u128 value.
     * @param lhs The first operand (u64).
     * @param rhs The second operand (u128).
     * @return A new u128 result representing the subtraction.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * const u128 z = y - x;
     * 
     * assert(z == 10);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr friend u128 operator-(const u64 lhs, const u128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        const __uint128_t result = lhs - y;

        return Memory::Transmute<__uint128_t, u128>(result);

#else

        return u128(lhs) - rhs;

#endif
    }

    /**
     * @brief Multiplies of two u128 integers.
     * @param lhs The first operand (u128).
     * @param rhs The second operand (u128).
     * @return A new u128 result representing the multiplication.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u128 y = (u128)20;
     * 
     * const u128 z = y * x;
     * 
     * assert(z == 200);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128 operator*(const u128 rhs) const noexcept {
        return Mul(rhs);
    }

    /**
     * @brief Multiplies a 64-bit integer to an u128 value.
     * @param lhs The first operand (u64).
     * @param rhs The second operand (u128).
     * @return A new u128 result representing the multiplication.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * const u128 z = y * x;
     * 
     * assert(z == 200);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128 operator*(const u64 rhs) const noexcept {
        return Mul(rhs);
    }

    /**
     * @brief Multiplies an u128 value to itself in-place.
     * @param rhs The amount to multiply (u128).
     * @return A reference to this instance updated with the multiplication.
     * @example
     * 
     * u128 x = (u128)10;
     * const u128 y = (u128)20;
     * 
     * x *= y;
     * 
     * assert(x == 200);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128& operator*=(const u128 rhs) noexcept {

        *this = Mul(rhs);
        return *this;
    }

    /**
     * @brief Multiplies an u64 value to itself in-place.
     * @param rhs The amount to multiply (u64).
     * @return A reference to this instance updated with the multiplication.
     * @example
     * 
     * u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * x *= y;
     * 
     * assert(x == 200);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128& operator*=(const u64 rhs) noexcept {

        *this = Mul(rhs);
        return *this;
    }

    /**
     * @brief Multiplies a 64-bit integer to an u128 value.
     * @param lhs The first operand (u64).
     * @param rhs The second operand (u128).
     * @return A new u128 result representing the multiplication.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * const u128 z = y * x;
     * 
     * assert(z == 200);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr friend u128 operator*(const u64 lhs, const u128 rhs) noexcept {
        return rhs * lhs;
    }

    /**
     * @brief Performs divisions of two u128 integers.
     * @param rhs The divisor (u128).
     * @return A new u128 result representing the quotient.
     * @example
     * 
     * const u128 x = (u128)20;
     * const u128 y = (u128)10;
     * 
     * const u128 z = x / y;
     * 
     * assert(z == 2);
     * assert(x == 20);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr u128 operator/(const u128 rhs) const noexcept {
        return Div(rhs);
    }

    /**
     * @brief Performs divisions of two u128 integers.
     * @param rhs The divisor (u64).
     * @return A new u128 result representing the quotient.
     * @example
     * 
     * const u128 x = (u128)20;
     * const u64 y = 10;
     * 
     * const u128 z = x / y;
     * 
     * assert(z == 2);
     * assert(x == 20);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr u128 operator/(const u64 rhs) const noexcept {
        return Div(rhs);
    }

    /**
     * @brief Divides an u128 value to itself in-place.
     * @param rhs The divisor (u128).
     * @return A reference to this instance updated with the quotient.
     * @example
     * 
     * u128 x = (u128)20;
     * const u128 y = (u128)10;
     * 
     * x /= y;
     * 
     * assert(x == 2);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr u128& operator/=(const u128 rhs) noexcept {

        *this = Div(rhs);
        return *this;
    }

    /**
     * @brief Divides an u64 value to itself in-place.
     * @param rhs The divisor (u64).
     * @return A reference to this instance updated with the quotient.
     * @example
     * 
     * u128 x = (u128)20;
     * const u64 y = 10;
     * 
     * x /= y;
     * 
     * assert(x == 2);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr u128& operator/=(const u64 rhs) noexcept {

        *this = Div(rhs);
        return *this;
    }

    /**
     * @brief Divides a 64-bit integer to an u128 value.
     * @param lhs The dividend (u64).
     * @param rhs The divisor (u128).
     * @return A new u128 result representing the quotient.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * const u128 z = y / x;
     * 
     * assert(z == 2);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr friend u128 operator/(const u64 lhs, const u128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        const __uint128_t result = lhs / y;

        return Memory::Transmute<__uint128_t, u128>(result);

#else

        return u128(lhs) / rhs;

#endif
    }

    /**
     * @brief Performs modulo of two u128 integers.
     * @param rhs The divisor (u128).
     * @return A new u128 result representing the remainder.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u128 y = (u128)20;
     * 
     * const u128 z = x % y;
     * 
     * assert(z == 0);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128 operator%(const u128 rhs) const noexcept {
        return Rem(rhs);
    }

    /**
     * @brief Performs modulo of an u128 and a 64-bit integer.
     * @param rhs The divisor (u64).
     * @return A new u128 result representing the remainder.
     * @example
     * 
     * const u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * const u128 z = x % y;
     * 
     * assert(z == 0);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128 operator%(const u64 rhs) const noexcept {
        return Rem(rhs);
    }

    /**
     * @brief Performs modulo of two u128 integers.
     * @param rhs The divisor (u128).
     * @return A reference to this instance updated with the remainder.
     * @example
     * 
     * u128 x = (u128)10;
     * const u128 y = (u128)20;
     * 
     * x %= y;
     * 
     * assert(x == 0);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128& operator%=(const u128 rhs) noexcept {

        *this = Rem(rhs);
        return *this;
    }

    /**
     * @brief Performs modulo of an u128 and a 64-bit integer.
     * @param rhs The divisor (u64).
     * @return A reference to this instance updated with the remainder.
     * @example
     * 
     * u128 x = (u128)10;
     * const u64 y = 20;
     * 
     * x %= y;
     * 
     * assert(x == 0);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr u128& operator%=(const u64 rhs) noexcept {

        *this = Rem(rhs);
        return *this;
    }

    /**
     * @brief Performs modulo a 64-bit integer and u128 value.
     * @param rhs The dividend (u64).
     * @param rhs The divisor (u128).
     * @return A new u128 result representing the remainder.
     * @example
     * 
     * const u128 x = (u128)20;
     * const u64 y = 10;
     * 
     * const u128 z = y % x;
     * 
     * assert(z == 0);
     * assert(x == 20);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr friend u128 operator%(const u64 lhs, const u128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        const __uint128_t result = lhs % y;

        return Memory::Transmute<__uint128_t, u128>(result);

#else

        return u128(lhs) % rhs;

#endif
    }

    /**
     * @brief Left bitwise shift.
     * @param shift The number of bits to shift left.
     * @return A new u128 result shifted left.
     * @example
     * 
     * const u128 x = (u128)1 << 104;
     */
    ALT_FORCE_INLINE constexpr u128 operator<<(const usize shift) const noexcept {
        return __shiftleft128__(*this, shift);
    }

    /**
     * @brief Left bitwise shift in-place.
     * @param shift The number of bits to shift left.
     * @return A reference to this instance shifted left.
     * @example
     * 
     * u128 x = 1;
     * x <<= 104;
     */
    ALT_FORCE_INLINE constexpr u128& operator<<=(const usize shift) noexcept {

        *this = __shiftleft128__(*this, shift);
        return *this;
    }

    /**
     * @brief Right bitwise shift.
     * @param shift The number of bits to shift right.
     * @return A new u128 result shifted right.
     * @example
     * 
     * const u128 x = Alt::Desc<u128>::Max() >> 104;
     */
    ALT_FORCE_INLINE constexpr u128 operator>>(const usize shift) const noexcept {
        return __shiftright128__(*this, shift);
    }

    /**
     * @brief Right bitwise shift in-place.
     * @param shift The number of bits to shift right.
     * @return A reference to this instance shifted right.
     * @example
     * 
     * u128 x = Alt::Desc<u128>::Max();
     * x >>= 104;
     */
    ALT_FORCE_INLINE constexpr u128& operator>>=(const usize shift) noexcept {

        *this = __shiftright128__(*this, shift);
        return *this;
    }

    /**
     * @brief Increments the value in-place.
     * @return A reference to this instance incremented by 1.
     */
    ALT_FORCE_INLINE constexpr u128& operator++() noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        *this = Memory::Transmute<__uint128_t, u128>(++x);

        return *this;
#else

        m_Low += 1;
        m_High += m_Low == 0 ? 1 : 0;

        return *this;

#endif
    }

    /**
     * @brief Post-increment operator.
     * @return A copy of the instance before incrementing.
     */
    ALT_FORCE_INLINE constexpr u128 operator++(int) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return Memory::Transmute<__uint128_t, u128>(x++);

#else

        u128 res = *this;
        ++(*this);

        return res;

#endif
    }

    /**
     * @brief Decrements the value in-place.
     * @return A reference to this instance decremented by 1.
     */
    ALT_FORCE_INLINE constexpr u128& operator--() noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        *this = Memory::Transmute<__uint128_t, u128>(--x);

        return *this;

#else

        m_Low -= 1;
        m_High -= m_Low == Num::Desc<u64>::Max() ? 1 : 0;

        return *this;

#endif
    }

    /**
     * @brief Post-decrement operator.
     * @return A copy of the instance before decrementing.
     */
    ALT_FORCE_INLINE constexpr u128 operator--(int) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return Memory::Transmute<__uint128_t, u128>(x--);

#else

        u128 res = *this;
        --(*this);

        return res;

#endif
    }

    /**
     * @brief Unary plus operator.
     * @return A copy of the current instance.
     */
    ALT_FORCE_INLINE constexpr u128 operator+() const noexcept {
        return *this;
    }

    /**
     * @brief Bitwise AND operation.
     * @param rhs The second operand (u128).
     * @return A new u128 result of the bitwise AND.
     * @xample
     * 
     * const u128 x = Alt::Desc<u128>::Max();
     * assert((x & Alt::Desc<u128>::Max()) == Alt::Desc<u128>::Max());
     */
    ALT_FORCE_INLINE constexpr u128 operator&(const u128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        return Memory::Transmute<__uint128_t, u128>(x & y);

#else

        return u128(m_High & rhs.m_High, m_Low & rhs.m_Low);

#endif
    }

    /**
     * @brief Bitwise AND in-place.
     * @param rhs The second operand (u128).
     * @return A reference to this instance updated with the result of the bitwise AND.
     * @xample
     * 
     * u128 x = Alt::Desc<u128>::Max();
     * x &= Alt::Desc<u128>::Max();
     * 
     * assert(x == Alt::Desc<u128>::Max());
     */
    ALT_FORCE_INLINE constexpr u128& operator&=(const u128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        *this = Memory::Transmute<__uint128_t, u128>(x & y);

        return *this;

#else

        m_Low &= rhs.m_Low;
        m_High &= rhs.m_High;

        return *this;

#endif
    }

    /**
     * @brief Bitwise AND with a 64-bit integer.
     * @param rhs The second operand (u64).
     * @return A new u128 result of the bitwise AND.
     * @example
     * 
     * const u128 x = Alt::Desc<u128>::Max();
     * assert((x & 0) == 0);
     */
    ALT_FORCE_INLINE constexpr u128 operator&(const u64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return Memory::Transmute<__uint128_t, u128>(x & rhs);

#else

        return u128(m_Low & rhs);

#endif
    }

    /**
     * @brief Bitwise AND in-place with a 64-bit integer.
     * @param rhs The second operand (u64).
     * @return A reference to this instance updated with the result of the bitwise AND.
     * @xample
     * 
     * u128 x = Alt::Desc<u128>::Max();
     * x &= 0;
     * 
     * assert(x == 0);
     */
    ALT_FORCE_INLINE constexpr u128& operator&=(const u64 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        *this = Memory::Transmute<__uint128_t, u128>(x & rhs);

        return *this;

#else

        m_Low &= rhs;
        m_High = 0;

        return *this;

#endif
    }

    /**
     * @brief Bitwise AND of a 64-bit integer and an u128.
     * @param lhs The first operand (u64).
     * @param rhs The second operand (u128).
     * @return A new u128 result of the bitwise AND.
     * @xample
     * 
     * u64 x = Alt::Desc<u64>::Max();
     * x &= Alt::Desc<u128>::Max();
     * 
     * assert(x == Alt::Desc<u64>::Max());
     */
    ALT_FORCE_INLINE constexpr friend u128 operator&(const u64 lhs, const u128 rhs) noexcept {
        return rhs & lhs;
    }

    /**
     * @brief Bitwise OR operation.
     * @param rhs The second operand (u128).
     * @return A new u128 result of the bitwise OR.
     * @xample
     * 
     * const u128 x = Alt::Desc<u128>::Max();
     * assert((x | 0) == Alt::Desc<u128>::Max());
     */
    ALT_FORCE_INLINE constexpr u128 operator|(const u128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        return Memory::Transmute<__uint128_t, u128>(x | y);

#else

        return u128(m_High | rhs.m_High, m_Low | rhs.m_Low);

#endif
    }

    /**
     * @brief Bitwise OR in-place.
     * @param rhs The second operand (u128).
     * @return A reference to this instance updated with the result of the bitwise OR.
     * @xample
     * 
     * u128 x = 0;
     * x |= Alt::Desc<u128>::Max();
     * 
     * assert(x == Alt::Desc<u128>::Max());
     */
    ALT_FORCE_INLINE constexpr u128& operator|=(const u128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        *this = Memory::Transmute<__uint128_t, u128>(x | y);

        return *this;

#else

        m_Low |= rhs.m_Low;
        m_High |= rhs.m_High;

        return *this;

#endif
    }

    /**
     * @brief Bitwise OR with a 64-bit integer.
     * @param rhs The second operand (u64).
     * @return A new u128 result of the bitwise OR.
     * @example
     * 
     * const u128 x = Alt::Desc<u128>::Max();
     * assert((x | 0) == Alt::Desc<u128>::Max());
     */
    ALT_FORCE_INLINE constexpr u128 operator|(const u64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return Memory::Transmute<__uint128_t, u128>(x | rhs);

#else

        return u128(m_High, m_Low | rhs);

#endif
    }

    /**
     * @brief Bitwise OR in-place with a 64-bit integer.
     * @param rhs The second operand (u64).
     * @return A reference to this instance updated with the result of the bitwise OR.
     * @xample
     * 
     * u128 x = Alt::Desc<u128>::Max();
     * x |= 0;
     * 
     * assert(x == Alt::Desc<u128>::Max());
     */
    ALT_FORCE_INLINE constexpr u128& operator|=(const u64 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        *this = Memory::Transmute<__uint128_t, u128>(x | rhs);

        return *this;

#else

        m_Low |= rhs;

        return *this;

#endif
    }

    /**
     * @brief Bitwise OR of a 64-bit integer and an u128.
     * @param lhs The first operand (u64).
     * @param rhs The second operand (u128).
     * @return A new u128 result of the bitwise OR.
     * @xample
     * 
     * u64 x = 0;
     * x |= Alt::Desc<u128>::Max();
     * 
     * assert(x == Alt::Desc<u64>::Max());
     */
    ALT_FORCE_INLINE constexpr friend u128 operator|(const u64 lhs, const u128 rhs) noexcept {
        return rhs | lhs;
    }

    /**
     * @brief Bitwise XOR operation.
     * @param rhs The second operand (u128).
     * @return A new u128 result of the bitwise XOR.
     * @xample
     * 
     * const u128 x = Alt::Desc<u128>::Max();
     * assert((x ^ Alt::Desc<u128>::Max()) == 0);
     */
    ALT_FORCE_INLINE constexpr u128 operator^(const u128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        return Memory::Transmute<__uint128_t, u128>(x ^ y);

#else

        return u128(m_High ^ rhs.m_High, m_Low ^ rhs.m_Low);

#endif
    }

    /**
     * @brief Bitwise XOR in-place.
     * @param rhs The second operand (u128).
     * @return A reference to this instance updated with the result of the bitwise XOR.
     * @xample
     * 
     * u128 x = Alt::Desc<u128>::Max();
     * x ^= Alt::Desc<u128>::Max();
     * 
     * assert(x == 0);
     */
    ALT_FORCE_INLINE constexpr u128& operator^=(const u128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        *this = Memory::Transmute<__uint128_t, u128>(x ^ y);

        return *this;

#else

        m_Low ^= rhs.m_Low;
        m_High ^= rhs.m_High;

        return *this;

#endif
    }

    /**
     * @brief Bitwise XOR with a 64-bit integer.
     * @param rhs The second operand (u64).
     * @return A new u128 result of the bitwise XOR.
     * @example
     * 
     * const u128 x = Alt::Desc<u128>::Max();
     * assert((x ^ 0) == Alt::Desc<u128>::Max());
     */
    ALT_FORCE_INLINE constexpr u128 operator^(const u64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return Memory::Transmute<__uint128_t, u128>(x ^ rhs);

#else

        return u128(m_High, m_Low ^ rhs);

#endif
    }

    /**
     * @brief Bitwise XOR in-place with a 64-bit integer.
     * @param rhs The second operand (u64).
     * @return A reference to this instance updated with the result of the bitwise XOR.
     * @xample
     * 
     * u128 x = Alt::Desc<u128>::Max();
     * x ^= 0;
     * 
     * assert(x == Alt::Desc<u128>::Max());
     */
    ALT_FORCE_INLINE constexpr u128& operator^=(const u64 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        *this = Memory::Transmute<__uint128_t, u128>(x ^ rhs);

        return *this;

#else

        m_Low ^= rhs;

        return *this;

#endif
    }

    /**
     * @brief Bitwise XOR of a 64-bit integer and an u128.
     * @param lhs The first operand (u64).
     * @param rhs The second operand (u128).
     * @return A new u128 result of the bitwise XOR.
     * @xample
     * 
     * u64 x = Alt::Desc<u64>::Max();
     * x ^= Alt::Desc<u128>::Max();
     * 
     * assert(x == 0);
     */
    ALT_FORCE_INLINE constexpr friend u128 operator^(const u64 lhs, const u128 rhs) noexcept {
        return rhs ^ lhs;
    }

    /**
     * @brief Bitwise NOT operation.
     * @details Negates all bits of the signed 128-bit integer.
     * @return A new u128 result representing the bitwise negation.
     * @example
     * 
     * u128 x = Alt::Desc<u128>::Max();
     * x = ~x;
     * 
     * assert(x == Alt::Desc<u128>::Min());
     */
    ALT_FORCE_INLINE constexpr u128 operator~() const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return Memory::Transmute<__uint128_t, u128>(~x);

#else

        return u128(~m_High, ~m_Low);

#endif
    }

    /**
     * @brief Comparison operators.
     */
    ALT_FORCE_INLINE constexpr bool operator==(const u128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<u128, __uint128_t>(*this) == Memory::Transmute<u128, __uint128_t>(rhs);

#else

        return m_High == rhs.m_High && m_Low == rhs.m_Low;

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator!=(const u128 rhs) const noexcept {
        return !(*this == rhs);
    }

    ALT_FORCE_INLINE constexpr bool operator<(const u128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<u128, __uint128_t>(*this) < Memory::Transmute<u128, __uint128_t>(rhs);

#else

        return m_High < rhs.m_High || (m_High <= rhs.m_High && m_Low < rhs.m_Low);

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator>(const u128 rhs) const noexcept {
        return rhs < *this;
    }

    ALT_FORCE_INLINE constexpr bool operator<=(const u128 rhs) const noexcept {
        return !(rhs < *this);
    }

    ALT_FORCE_INLINE constexpr bool operator>=(const u128 rhs) const noexcept {
        return !(*this < rhs);
    }

    ALT_FORCE_INLINE constexpr bool operator==(const u64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<u128, __uint128_t>(*this) == rhs;

#else

        return m_High == 0 && m_Low == rhs;

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator!=(const u64 rhs) const noexcept {
        return !(*this == rhs);
    }

    ALT_FORCE_INLINE constexpr bool operator<(const u64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<u128, __uint128_t>(*this) < rhs;

#else

        return m_High == 0 && m_Low < rhs;

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator>(const u64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<u128, __uint128_t>(*this) > rhs;

#else

        return m_High != 0 || m_Low > rhs;

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator<=(const u64 rhs) const noexcept {
        return !(*this > rhs);
    }

    ALT_FORCE_INLINE constexpr bool operator>=(const u64 rhs) const noexcept {
        return !(*this < rhs);
    }

    ALT_FORCE_INLINE constexpr friend bool operator==(const u64 lhs, const u128 rhs) noexcept {
        return rhs == lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator!=(const u64 lhs, const u128 rhs) noexcept {
        return rhs != lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator<(const u64 lhs, const u128 rhs) noexcept {
        return rhs > lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator>(const u64 lhs, const u128 rhs) noexcept {
        return rhs < lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator<=(const u64 lhs, const u128 rhs) noexcept {
        return rhs >= lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator>=(const u64 lhs, const u128 rhs) noexcept {
        return rhs <= lhs;
    }

    /**
     * @brief Adds an u128 to itself.
     * @param rhs The second operand (u128).
     * @return A new u128 result representing the sum.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Add(const u128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        return Memory::Transmute<__uint128_t, u128>(x + y);

#else

        u128 result;

        result.m_Low = __uadd128__(m_Low, rhs.m_Low, result.m_High);
        result.m_High += m_High + rhs.m_High;

        return result;

#endif
    }

    /**
     * @brief Adds an u64.
     * @param rhs The second operand (u64).
     * @return A new u128 result representing the sum.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Add(const u64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return Memory::Transmute<__uint128_t, u128>(x + rhs);

#else

        u128 result;

        result.m_Low = __uadd128__(m_Low, rhs, result.m_High);
        result.m_High += m_High;

        return result;

#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckAdd(const u128 rhs, u128* pResult = nullptr) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        __uint128_t res = 0;
        const bool isOverflow = __builtin_add_overflow(x, y, &res);

        if (pResult) {
            *pResult = Memory::Transmute<__uint128_t, u128>(res);
        }

        return isOverflow;

#else

        u128 result;

        u64 hi = 0;
        result.m_Low = __uadd128__(m_Low, rhs.m_Low, hi);
        result.m_High = __uadd128Carry__(m_High, rhs.m_High, hi, hi);

        const bool isOverflow = hi != 0;

        if (pResult) {
            *pResult = result;
        }

        return isOverflow;

#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckAdd(const u64 rhs, u128* pResult = nullptr) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        __uint128_t res = 0;
        const bool isOverflow = __builtin_add_overflow(x, rhs, &res);

        if (pResult) {
            *pResult = Memory::Transmute<__uint128_t, u128>(res);
        }

        return isOverflow;

#else

        u128 result;

        u64 hi = 0;
        result.m_Low = __uadd128__(m_Low, rhs, hi);
        result.m_High = __uadd128__(m_High, hi, hi);

        const bool isOverflow = hi != 0;

        if (pResult) {
            *pResult = result;
        }

        return isOverflow;

#endif
    }

    /**
     * @brief Subtracts an u128 from itself.
     * @param rhs The second operand (u128).
     * @return A new u128 result representing the difference.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Sub(const u128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        return Memory::Transmute<__uint128_t, u128>(x - y);

#else

        u128 result;

        u64 hi = 0;
        result.m_Low = __sub128__(m_Low, rhs.m_Low, hi);
        result.m_High = m_High - rhs.m_High - hi;

        return result;

#endif
    }

    /**
     * @brief Subtracts an u64 from itself.
     * @param rhs The second operand (u64).
     * @return A new u128 result representing the difference.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Sub(const u64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return Memory::Transmute<__uint128_t, u128>(x - rhs);

#else

        u128 result;

        u64 hi = 0;
        result.m_Low = __sub128__(m_Low, rhs, hi);
        result.m_High = m_High - hi;

        return result;

#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckSub(const u128 rhs, u128* pResult = nullptr) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        __uint128_t res = 0;
        const bool isOverflow = __builtin_sub_overflow(x, y, &res);

        if (pResult) {
            *pResult = Memory::Transmute<__uint128_t, u128>(res);
        }

        return isOverflow;

#else

        u128 result;

        u64 hi = 0;
        result.m_Low = __sub128__(m_Low, rhs.m_Low, hi);
        result.m_High = __sub128Carry__(m_High, rhs.m_High, hi, hi);

        const bool isOverflow = hi != 0;

        if (pResult) {
            *pResult = result;
        }

        return isOverflow;

#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckSub(const u64 rhs, u128* pResult = nullptr) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        __uint128_t res = 0;
        const bool isOverflow = __builtin_sub_overflow(x, rhs, &res);

        if (pResult) {
            *pResult = Memory::Transmute<__uint128_t, u128>(res);
        }

        return isOverflow;

#else

        u128 result;

        u64 hi = 0;
        result.m_Low = __sub128__(m_Low, rhs, hi);
        result.m_High = __sub128__(m_High, hi, hi);

        const bool isOverflow = hi != 0;

        if (pResult) {
            *pResult = result;
        }

        return isOverflow;

#endif
    }

    /**
     * @brief Multiplies itself by an u128.
     * @param rhs The second operand (u128).
     * @return A new u128 result representing the product.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Mul(const u128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        return Memory::Transmute<__uint128_t, u128>(x * y);

#else

        u128 result;
        result.m_Low = __umul128__(m_Low, rhs.m_Low, result.m_High);

        result.m_High += m_High * rhs.m_Low;
        result.m_High += m_Low * rhs.m_High;

        return result;

#endif
    }

    /**
     * @brief Multiplies itself by an u64.
     * @param rhs The second operand (u64).
     * @return A new u128 result representing the product.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Mul(const u64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        return Memory::Transmute<__uint128_t, u128>(x * rhs);

#else

        u128 result;
        result.m_Low = __umul128__(m_Low, rhs, result.m_High);

        result.m_High += m_High * rhs;

        return result;

#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckMul(const u128 rhs, u128* pResult = nullptr) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);
        const __uint128_t y = Memory::Transmute<u128, __uint128_t>(rhs);

        __uint128_t res = 0;
        const bool isOverflow = __builtin_mul_overflow(x, y, &res);

        if (pResult) {
            *pResult = Memory::Transmute<__uint128_t, u128>(res);
        }

        return isOverflow;

#else

        u128 result;
        result.m_Low = __umul128__(m_Low, rhs.m_Low, result.m_High);

        bool isOverflow = false;

        const u64 high0 = __umul128OverflowOr__(m_High, rhs.m_Low, isOverflow);
        const u64 high1 = __umul128OverflowOr__(m_Low, rhs.m_High, isOverflow);

        u64 hi = 0;
        result.m_High = __uadd128__(result.m_High, high0, hi);
        isOverflow |= hi != 0;

        result.m_High = __uadd128__(result.m_High, high1, hi);
        isOverflow |= hi != 0;

        isOverflow |= m_High != 0 && rhs.m_High != 0;

        if (pResult) {
            *pResult = result;
        }

        return isOverflow;

#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckMul(const u64 rhs, u128* pResult = nullptr) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __uint128_t x = Memory::Transmute<u128, __uint128_t>(*this);

        __uint128_t res = 0;
        const bool isOverflow = __builtin_mul_overflow(x, rhs, &res);

        if (pResult) {
            *pResult = Memory::Transmute<__uint128_t, u128>(res);
        }

        return isOverflow;

#else

        u128 result;
        result.m_Low = __umul128__(m_Low, rhs, result.m_High);

        bool isOverflow = false;

        const u64 high = __umul128OverflowOr__(m_High, rhs, isOverflow);

        u64 hi = 0;
        result.m_High = __uadd128__(result.m_High, high, hi);
        isOverflow |= hi != 0;

        if (pResult) {
            *pResult = result;
        }

        return isOverflow;

#endif
    }

    /**
     * @brief Division of the instance by another u128.
     * @param rhs The divisor (u128).
     * @return A new u128 result representing the quotient.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Div(const u128 rhs) const noexcept {

        u128 rem;
        return __udiv128__(*this, rhs, rem);
    }

    /**
     * @brief Modulo operation of the instance by another u128.
     * @param rhs The divisor (u128).
     * @return A new u128 result representing the remainder.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Rem(const u128 rhs) const noexcept {

        u128 rem;
        ALT_MAYBE_UNUSED const u128 div = __udiv128__(*this, rhs, rem);

        return rem;
    }

    /**
     * @brief Division of the instance by an u64.
     * @param rhs The divisor (u64).
     * @return A new u128 result representing the quotient.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Div(const u64 rhs) const noexcept {

        u128 rem;
        return __udiv128__(*this, rhs, rem);
    }

    /**
     * @brief Modulo operation of the instance by an u64.
     * @param rhs The divisor (u64).
     * @return A new u128 result representing the remainder.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 Rem(const u64 rhs) const noexcept {

        u128 rem;
        ALT_MAYBE_UNUSED const u128 div = __udiv128__(*this, rhs, rem);

        return rem;
    }

    /**
     * @brief Division and modulo.
     * @param rhs The divisor (u128).
     * @param rem Output parameter for the remainder.
     * @return A new u128 result representing the quotient.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 DivRem(const u128 rhs, u128& rem) const noexcept {

        return __udiv128__(*this, rhs, rem);
    }

    /**
     * @brief Division and modulo with an u64.
     * @param rhs The divisor (u64).
     * @param rem Output parameter for the remainder.
     * @return A new u128 result representing the quotient.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 DivRem(const u64 rhs, u128& rem) const noexcept {

        return __udiv128__(*this, rhs, rem);
    }

    /**
     * @brief Counts the number of set bits (population count).
     * @return The total number of bits set to 1 in the 128-bit integer.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Popcount() const noexcept {

        return Num::Popcount(m_Low) + Num::Popcount(m_High);
    }

    /**
     * @brief Counts trailing zeros.
     * @details This method has UB on 0.
     * @return The number of trailing zero bits.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsRightUnsafe() const noexcept {

        const usize result0 = Num::Count0BitsRightUnsafe(m_Low);

        usize result1 = Num::Count0BitsRightUnsafe(m_High);
        result1 += 64;

        return m_Low == 0 ? result1 : result0;
    }

    /**
     * @brief Counts trailing zeros.
     * @return The count of trailing zero bits.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsRight() const noexcept {

        const usize result0 = Num::Count0BitsRightUnsafe(m_Low);

        usize result1 = Num::Count0BitsRightUnsafe(m_High);
        result1 += 64;

        const usize result = m_Low == 0 ? result1 : result0;

        return *this != 0 ? result : 128;
    }

    /**
     * @brief Counts leading zeros.
     * @details This method has UB on 0.
     * @return The number of leading zero bits.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsLeftUnsafe() const noexcept {

        usize result0 = Num::Count0BitsLeftUnsafe(m_Low);
        result0 += 64;

        const usize result1 = Num::Count0BitsLeftUnsafe(m_High);

        return m_High == 0 ? result0 : result1;
    }

    /**
     * @brief Counts leading zeros.
     * @return The number of leading zero bits.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsLeft() const noexcept {

        usize result0 = Num::Count0BitsLeftUnsafe(m_Low);
        result0 += 64;

        const usize result1 = Num::Count0BitsLeftUnsafe(m_High);

        const usize result = m_High == 0 ? result0 : result1;

        return *this != 0 ? result : 128;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPowerOf2() const noexcept {

        u128 thisMinusOne = *this;
        --thisMinusOne;

        return *this != 0 && (*this & thisMinusOne) == 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPowerOf2PreserveZero() const noexcept {

        u128 thisMinusOne = *this;
        --thisMinusOne;

        return (*this & thisMinusOne) == 0;
    }

    /**
     * @brief Swaps the byte order of the 128-bit integer.
     * @return A new s128 instance with swapped bytes.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u128 ByteSwap() const noexcept {
        return u128(Memory::ByteSwap(m_Low), Memory::ByteSwap(m_High));
    }

    /**
     * @brief Gets the low 64 bits of the value.
     * @return The low component as a u64.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 GetLow() const noexcept {
        return m_Low;
    }

    /**
     * @brief Gets the high 64 bits of the value.
     * @return The high component as a s64.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 GetHigh() const noexcept {
        return m_High;
    }

private:

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 __uadd128Carry__(const u64 x, const u64 y, const u64 carry, u64& hi) noexcept {

        ALT_IF_IS_CONSTANT_EVALUATED {

            const u64 res = (carry != 0) + x + y;
            const u64 overflow = (x & y) ^ ((x ^ y) & ~res);

            hi = overflow >> 63;

            return res;
        }

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        u64 res = 0;
        hi = _addcarry_u64(static_cast<u8>(carry), x, y, &res);

        return res;

#elif defined(ALT_PLATFORM_X86)

        const u64 res = (carry != 0) + x + y;
        const u64 overflow = (x & y) ^ ((x ^ y) & ~res);

        hi = overflow >> 63;

        return res;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_addcll(x, y, carry, &hi);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 __uadd128__(const u64 x, const u64 y, u64& hi) noexcept {

        ALT_IF_IS_CONSTANT_EVALUATED {

            const u64 res = x + y;
            const bool isOverflow = res < x;

            hi = isOverflow ? 1 : 0;

            return res;
        }

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        u64 res = 0;
        hi = _addcarry_u64(0, x, y, &res);

        return res;

#elif defined(ALT_PLATFORM_X86)

        const u64 res = x + y;
        const bool isOverflow = res < x;

        hi = isOverflow ? 1 : 0;

        return res;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_addcll(x, y, 0, &hi);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 __sub128Carry__(const u64 x, const u64 y, const u64 carry, u64& hi) noexcept {

        ALT_IF_IS_CONSTANT_EVALUATED {

            const u64 res = x - y - (carry != 0);
            const u64 overflow = (res & y) ^ ((res ^ y) & ~x);

            hi = overflow >> 63;

            return res;
        }

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        u64 res = 0;
        hi = _subborrow_u64(static_cast<u8>(carry), x, y, &res);

        return res;

#elif defined(ALT_PLATFORM_X86)

        const u64 res = x - y - (carry != 0);
        const u64 overflow = (res & y) ^ ((res ^ y) & ~x);

        hi = overflow >> 63;

        return res;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_subcll(x, y, carry, &hi);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 __sub128__(const u64 x, const u64 y, u64& hi) noexcept {

        ALT_IF_IS_CONSTANT_EVALUATED {

            const u64 res = x - y;
            const bool isUnderflow = res > x;

            hi = isUnderflow ? 1 : 0;

            return res;
        }

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        u64 res = 0;
        hi = _subborrow_u64(0, x, y, &res);

        return res;

#elif defined(ALT_PLATFORM_X86)

        const u64 res = x - y;
        const bool isUnderflow = res > x;

        hi = isUnderflow ? 1 : 0;

        return res;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_subcll(x, y, 0, &hi);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 __umul128__(const u64 x, const u64 y, u64& hi) noexcept {

        ALT_IF_IS_CONSTANT_EVALUATED {

            constexpr u64 mask32 = static_cast<u64>(Num::Desc<u32>::Max());

            const u64 a = (x >> 32) & mask32;
            const u64 b = x & mask32;
            const u64 c = (y >> 32) & mask32;
            const u64 d = y & mask32;

            const u64 ad = a * d;
            const u64 bd = b * d;
            const u64 adbc = ad + b * c;

            const u64 lo = bd + (adbc << 32);

            u64 carry = adbc < ad ? 1 : 0;
            carry <<= 32;
            carry += adbc >> 32;
            carry += lo < bd ? 1 : 0;

            hi = a * c + carry;

            return lo;
        }

#if defined(ALT_PLATFORM_X86_64)

        return _umul128(x, y, &hi);

#elif defined(ALT_PLATFORM_X86)

        const u32 a = static_cast<u32>(x >> 32);
        const u32 b = static_cast<u32>(x);
        const u32 c = static_cast<u32>(y >> 32);
        const u32 d = static_cast<u32>(y);

        const u64 ad = __emulu(a, d);
        const u64 bd = __emulu(b, d);
        const u64 adbc = ad + __emulu(b, c);

        const u64 lo = bd + (adbc << 32);

        u64 carry = adbc < ad ? 1 : 0;
        carry <<= 32;
        carry += adbc >> 32;
        carry += lo < bd ? 1 : 0;

        hi = __emulu(a, c) + carry;

        return lo;

#else
        #error provide implementation for specific platform
#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 __umul128OverflowOr__(const u64 x, const u64 y, bool& isOverflow) noexcept {

        u64 res = 0;
        isOverflow |= _NumDetails::IsOverflowCheckMulImpl<u64>::IsOverflowCheckMul(x, y, res);

        return res;
    }

    /**
     * "Improved division by invariant integers" Niels M¨oller and Torbj¨orn Granlund
     */

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 umullo(const u64 a, const u64 b) noexcept {

        return a * b;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 umulhi(const u64 a, const u64 b) noexcept {

        u64 hi = 0;
        ALT_MAYBE_UNUSED const u64 lo = __umul128__(a, b, hi);
        return hi;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 ReciprocalWord_2by1(const u64 d) noexcept {

        const u64 d0 = d & 0b1;
        const u64 d9 = d >> 55;
        const u64 d40 = (d >> 24) + 1;
        const u64 d63 = (d >> 1) + d0;

        const u64 v0 = ((static_cast<u64>(1) << 19) - (static_cast<u64>(3) << 8)) / d9;

        const u64 v1 = (v0 << 11) - (umullo(umullo(v0, v0), d40) >> 40) - 1;
        const u64 v2 = (v1 << 13) + (umullo(v1, (static_cast<u64>(1) << 60) - umullo(v1, d40)) >> 47);

        const u64 e = ((u128(1) << 96) - umullo(v2, d63) + (v2 >> 1) * d0).GetLow();

        const u64 v3 = (v2 << 31) + (umulhi(v2, e) >> 1);

        const u64 x = v3 + 1;
        const u64 hi = x != 0 ? umulhi(x, d) : d;
        const u64 v4 = v3 - hi - d;

        return v4;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 ReciprocalWord_2by1_Hardware(const u64 d) noexcept {

        ALT_IF_IS_CONSTANT_EVALUATED {

            return ReciprocalWord_2by1(d);
        }

#if defined(ALT_PLATFORM_X86_64) && defined(ALT_COMPILER_MSVC)

        u64 rem = 0;
        return _udiv128(Num::Desc<u64>::Max() - d, Num::Desc<u64>::Max(), d, &rem);

#elif defined(ALT_PLATFORM_X86_32) || defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return ReciprocalWord_2by1(d);

#else
    #error provide implementation for specific platform/compiler
#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 ReciprocalWord_3by2(const u128 d) noexcept {

        u64 v = ReciprocalWord_2by1_Hardware(d.m_High);

        u64 p = umullo(d.m_High, v);
        p += d.m_Low;

        if (p < d.m_Low) {

            v -= 1;

            const bool isCorrectionNeed = p >= d.m_High;

            v -= isCorrectionNeed ? 1 : 0;
            p -= isCorrectionNeed ? d.m_High : 0;

            p -= d.m_High;
        }

        u128 t = u128(v) * d.m_Low;

        p += t.m_High;

        bool isCorrectionNeed = p < t.m_High;
        v -= isCorrectionNeed ? 1 : 0;

        isCorrectionNeed = u128(p, t.m_Low) >= d;
        v -= isCorrectionNeed ? 1 : 0;

        return v;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 __div2by1__(const u128 u, const u64 d, const u64 v, u64& rem) noexcept {

        u128 q = u128(v) * u.m_High;

        q += u;
        q.m_High += 1;

        u64 r = u.m_Low - q.m_High * d;

        const bool isCorrectionNeed = r > q.m_Low;

        q.m_High -= isCorrectionNeed ? 1 : 0;
        r += isCorrectionNeed ? d : 0;

        if (r >= d) {

            q.m_High += 1;
            r -= d;
        }

        rem = r;

        return q.m_High;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 __div3by2__(const u64 u2, const u128 u, const u128 d, const u64 v, u128& rem) noexcept {

        u128 q = u128(v) * u2;
        q += u128(u2, u.m_High);

        const u64 r1 = u.m_High - q.m_High * d.m_High;

        u128 t = u128(d.m_Low) * q.m_High;
        u128 r = u128(r1, u.m_Low) - t - d;

        q.m_High += 1;

        const bool isCorrectionNeed = r.m_High >= q.m_Low;
        q.m_High -= isCorrectionNeed ? 1 : 0;
        r += isCorrectionNeed ? d : 0;

        if (r >= d) {

            q.m_High += 1;
            r -= d;
        }

        rem = r;

        return q.m_High;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 __udiv128__(const u128 x, const u128 y, u128& rem) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__)

        const __uint128_t xx = Memory::Transmute<u128, __uint128_t>(x);
        const __uint128_t yy = Memory::Transmute<u128, __uint128_t>(y);

        const __uint128_t result = xx / yy;
        const __uint128_t rem_ = xx % yy;

        rem = Memory::Transmute<__uint128_t, u128>(rem_);
        return Memory::Transmute<__uint128_t, u128>(result);

#else

        if (y.m_High == 0) {

            return __udiv128__(x, y.m_Low, rem);
        }

        const usize shift = Num::Count0BitsLeftUnsafe(y.m_High);
        const u128 d = y << shift;

        const u64 v = ReciprocalWord_3by2(d);

        u128 result;

        const u64 high = (u128(x.m_High) << shift).m_High;
        result.m_Low = __div3by2__(high, x << shift, d, v, rem);

        rem >>= shift;

        return result;

#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 __udiv128__(const u128 x, const u64 y, u128& rem) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__)

        const __uint128_t xx = Memory::Transmute<u128, __uint128_t>(x);

        const __uint128_t result = xx / y;
        const __uint128_t rem_ = xx % y;

        rem = Memory::Transmute<__uint128_t, u128>(rem_);
        return Memory::Transmute<__uint128_t, u128>(result);

#else

        ALT_IF_IS_CONSTANT_EVALUATED {

            const usize shift = Num::Count0BitsLeftUnsafe(y);
            const u64 d = y << shift;

            const u64 v = ReciprocalWord_2by1(d);

            u128 result;

            u64 high = 0;
            result.m_High = __div2by1__(u128(x.m_High) << shift, d, v, high);
            result.m_Low = __div2by1__(u128(high >> shift, x.m_Low) << shift, d, v, rem.m_Low);

            rem.m_Low >>= shift;
            rem.m_High = 0;

            return result;
        }

#if defined(ALT_PLATFORM_X86_64) && defined(ALT_COMPILER_MSVC)

        u128 result;

        result.m_High = x.m_High / y;
        const u64 high = x.m_High % y;

        result.m_Low = _udiv128(high, x.m_Low, y, &rem.m_Low);

        rem.m_High = 0;

        return result;

#elif defined(ALT_PLATFORM_X86_32) || defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        const usize shift = Num::Count0BitsLeftUnsafe(y);
        const u64 d = y << shift;

        const u64 v = ReciprocalWord_2by1(d);

        u128 result;

        u64 high = 0;
        result.m_High = __div2by1__(u128(x.m_High) << shift, d, v, high);
        result.m_Low = __div2by1__(u128(high >> shift, x.m_Low) << shift, d, v, rem.m_Low);

        rem.m_Low >>= shift;
        rem.m_High = 0;

        return result;

#else
    #error provide implementation for specific platform/compiler
#endif
#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 __shiftleft128__(const u128 x, const usize shift) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__)

        const __uint128_t xx = Memory::Transmute<u128, __uint128_t>(x);

        return Memory::Transmute<__uint128_t, u128>(xx << shift);

#else

        ALT_IF_IS_CONSTANT_EVALUATED {

            u128 result;

            if (shift >= 64) {

                result.m_High = x.m_Low << (shift - 64);
            }
            else {

                result.m_Low = x.m_Low << shift;
                result.m_High = (x.m_High << shift) | (x.m_Low >> (64 - shift));
            }

            return result;
        }

#if defined(ALT_PLATFORM_X86_64)

        u128 result;

        if (shift >= 64) {

            result.m_High = x.m_Low << (shift - 64);
        }
        else {

            result.m_Low = x.m_Low << shift;
            result.m_High = __shiftleft128(x.m_Low, x.m_High, static_cast<u8>(shift));
        }

        return result;

#elif defined(ALT_PLATFORM_X86_32)

        u128 result;

        if (shift >= 64) {

            result.m_High = x.m_Low << (shift - 64);
        }
        else {

            result.m_Low = x.m_Low << shift;
            result.m_High = (x.m_High << shift) | (x.m_Low >> (64 - shift));
        }

        return result;

#else
    #error provide implementation for specific platform
#endif
#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 __shiftright128__(const u128 x, const usize shift) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__)

        const __uint128_t xx = Memory::Transmute<u128, __uint128_t>(x);

        return Memory::Transmute<__uint128_t, u128>(xx >> shift);

#else

        ALT_IF_IS_CONSTANT_EVALUATED {

            u128 result;

            if (shift >= 64) {

                result.m_Low = x.m_High >> (shift - 64);
            }
            else {

                result.m_Low = (x.m_Low >> shift) | (x.m_High << (64 - shift));
                result.m_High = x.m_High >> shift;
            }

            return result;
        }

#if defined(ALT_PLATFORM_X86_64)

        u128 result;

        if (shift >= 64) {

            result.m_Low = x.m_High >> (shift - 64);
        }
        else {

            result.m_Low = __shiftright128(x.m_Low, x.m_High, static_cast<u8>(shift));
            result.m_High = x.m_High >> shift;
        }

        return result;

#elif defined(ALT_PLATFORM_X86_32)

        u128 result;

        if (shift >= 64) {

            result.m_Low = x.m_High >> (shift - 64);
        }
        else {

            result.m_Low = (x.m_Low >> shift) | (x.m_High << (64 - shift));
            result.m_High = x.m_High >> shift;
        }

        return result;

#else
    #error provide implementation for specific platform
#endif
#endif
    }

    ALT_FORCE_INLINE constexpr u128(const u64 hi, const u64 lo) noexcept
        : m_Low(lo)
        , m_High(hi)
    {}

private:

#ifdef ALT_PLATFORM_LITTLE_ENDIAN

    u64 m_Low = 0;
    u64 m_High = 0;

#else

    u64 m_High = 0;
    u64 m_Low = 0;

#endif
};

} // namespace Types

namespace TypeTraits {

template <>
struct __is_number__<Types::u128>
    : std::true_type
{};

template <>
struct __is_trivially_reallocatable__<Types::u128>
    : std::true_type
{};

} // namespace TypeTraits

namespace Num {

/**
 * @brief Num::Description for u128.
 */
template <>
struct Desc<u128> {

    using Signed = s128;
    using Unsigned = u128;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 Min() noexcept {
        return u128::Min();
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 Max() noexcept {
        return u128::Max();
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 Lowest() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u128 SignalingNaN() noexcept {
        return 0;
    }
    
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsUnsigned() noexcept {
        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsSigned() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsFloat() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize DigitCount() noexcept {
        return 39;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<u128>;
    }
};

} // namespace Num

namespace _NumDetails {

template <>
struct IsOverflowCheckAddImpl<u128> {

    template <typename T>
    ALT_FORCE_INLINE constexpr static bool IsOverflowCheckAdd(const u128 x, const T y, u128& res) noexcept {

        return __IsOverflowCheckAdd__(x, y, res);
    }

    template <typename T>
    ALT_FORCE_INLINE constexpr static bool IsOverflowCheckAddWrap(const u128 x, const T y, u128& res) noexcept {

        u128 res_ = 0;
        const bool isOverflow = __IsOverflowCheckAdd__(x, y, res_);
        res = isOverflow ? Num::Desc<u128>::Max() : res_;

        return isOverflow;
    }

    template <typename T>
    ALT_FORCE_INLINE constexpr static bool IsOverflowCheckAddWrap(const u128 x, const T y, u128& res, const u128 wrap) noexcept {

        u128 res_ = 0;
        bool isOverflow = __IsOverflowCheckAdd__(x, y, res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;
    }

private:

    template <typename T,
        std::enable_if_t<std::is_convertible_v<T, u64> && (sizeof(T) <= sizeof(u64))>* = nullptr>
    ALT_FORCE_INLINE constexpr static bool __IsOverflowCheckAdd__(const u128 x, const T y_, u128& res) noexcept {

        const u64 y = static_cast<u64>(y_);

        return x.IsOverflowCheckAdd(y, &res);
    }

    ALT_FORCE_INLINE constexpr static bool __IsOverflowCheckAdd__(const u128 x, const u128 y, u128& res) noexcept {

        return x.IsOverflowCheckAdd(y, &res);
    }
};

template <>
struct IsUnderflowCheckSubImpl<u128> {

    template <typename T>
    ALT_FORCE_INLINE constexpr static bool IsUnderflowCheckSub(const u128 x, const T y, u128& res) noexcept {

        return __IsOverflowCheckSub__(x, y, res);
    }

private:

    template <typename T,
        std::enable_if_t<std::is_convertible_v<T, u64> && (sizeof(T) <= sizeof(u64))>* = nullptr>
    ALT_FORCE_INLINE constexpr static bool __IsOverflowCheckSub__(const u128 x, const T y_, u128& res) noexcept {

        const u64 y = static_cast<u64>(y_);

        return x.IsOverflowCheckSub(y, &res);
    }

    ALT_FORCE_INLINE constexpr static bool __IsOverflowCheckSub__(const u128 x, const u128 y, u128& res) noexcept {

        return x.IsOverflowCheckSub(y, &res);
    }
};

template <>
struct IsOverflowCheckMulImpl<u128> {

    template <typename T>
    ALT_FORCE_INLINE constexpr static bool IsOverflowCheckMul(const u128 x, const T y, u128& res) noexcept {

        return __IsOverflowCheckMul__(x, y, res);
    }

    template <typename T>
    ALT_FORCE_INLINE constexpr static bool IsOverflowCheckMulWrap(const u128 x, const T y, u128& res) noexcept {

        u128 res_ = 0;
        const bool isOverflow = __IsOverflowCheckMul__(x, y, res_);

        res = isOverflow ? Num::Desc<u128>::Max() : res_;

        return isOverflow;
    }

    template <typename T>
    ALT_FORCE_INLINE constexpr static bool IsOverflowCheckMulWrap(const u128 x, const T y, u128& res, const u128 wrap) noexcept {

        u128 res_ = 0;
        bool isOverflow = __IsOverflowCheckMul__(x, y, res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;
    }

private:

    template <typename T,
        std::enable_if_t<std::is_convertible_v<T, u64> && (sizeof(T) <= sizeof(u64))>* = nullptr>
    ALT_FORCE_INLINE constexpr static bool __IsOverflowCheckMul__(const u128 x, const T y_, u128& res) noexcept {

        const u64 y = static_cast<u64>(y_);

        return x.IsOverflowCheckMul(y, &res);
    }

    ALT_FORCE_INLINE constexpr static bool __IsOverflowCheckMul__(const u128 x, const u128 y, u128& res) noexcept {

        return x.IsOverflowCheckMul(y, &res);
    }
};

} // namespace _NumDetails
} // namespace Alt

#include "u128.inl"

#endif // __ALT_STD_U128__