#pragma once

#ifndef __ALT_STD_S128__
#define __ALT_STD_S128__

#include "u128.h"

namespace Alt {
namespace Types {

/**
 * @brief A signed 128-bit integer type.
 * 
 * @details This class provides a implementation of a 128-bit signed integer, supporting 
 * basic arithmetic, bitwise operations, and comparison logic. It leverages native `__int128` 
 * where available while providing fallback implementations for other environments or when
 * custom 128-bit logic is requested via `ALT_USE_CUSTOM_128`.
 * 
 * @note For Clang use 'clang_rt.builtins-x86_64.lib'/'clang_rt.builtins-i386.lib' or define `ALT_USE_CUSTOM_128`.
 * 
 * @example
 * 
 * const s128 val         = 1000000000000000000ULL;
 * const s128 result = val + 500000000000000000ULL;
 * assert(result ==         1500000000000000000ULL);
 */
class alignas(16) s128 {
public:

    /**
     * @brief Returns the minimum representable value for a signed 128-bit integer.
     * @return The smallest possible s128 value.
     * @example
     * 
     * const s128 min = s128::Min(); // Alt::Desc<s128>::Min();
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 Min() noexcept {

        return s128(~Num::Desc<s64>::Max(), ~Num::Desc<u64>::Max());
    }

    /**
     * @brief Returns the maximum representable value for a signed 128-bit integer.
     * @return The largest possible s128 value.
     * @example
     * 
     * const s128 max = s128::Max(); // Alt::Desc<s128>::Max();
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 Max() noexcept {

        return s128(Num::Desc<s64>::Max(), Num::Desc<u64>::Max());
    }

    /**
     * @brief Constructs a 128-bit integer from a high and low component.
     * @tparam T The type of the low component; must be `u64`.
     * @param high The upper 64 bits (signed).
     * @param low The lower 64 bits (unsigned).
     * @return A new s128 instance.
     * @example
     * 
     * const s128 val = s128::FromHighLow(-1, 0xffffffffffffffffULL);
     * 
     * assert(val == Alt::Desc<s128>::Min());
     */
    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 FromHighLow(const typename Num::Desc<T>::Signed high, const T low) noexcept {

        return s128(high, low);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 FromHighLow(const ssize high, const usize low) noexcept {

        return s128(static_cast<s64>(high), static_cast<u64>(low));
    }

    /**
     * @brief Constructors for s128.
     */
    ALT_FORCE_INLINE constexpr s128() noexcept = default;

    ALT_FORCE_INLINE constexpr s128(const s128&) noexcept = default;
    ALT_FORCE_INLINE constexpr s128& operator=(const s128&) noexcept = default;

    ALT_FORCE_INLINE constexpr s128(s128&&) noexcept = default;
    ALT_FORCE_INLINE constexpr s128& operator=(s128&&) noexcept = default;

    ALT_FORCE_INLINE constexpr explicit s128(const u128& rhs) noexcept
        : m_Low(rhs.GetLow())
        , m_High(rhs.GetHigh())
    {}

    ALT_FORCE_INLINE constexpr s128(const ssize num) noexcept
#if !(!defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)))
        : m_Low(static_cast<u64>(num))
        , m_High(Num::IsSignPositive(num) ? 0 : ~static_cast<u64>(0))
#endif
    {
#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = static_cast<__int128>(num);

        *this = Memory::Transmute<__int128, s128>(x);
#endif
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_FORCE_INLINE constexpr explicit s128(const T num) noexcept
        : m_Low(num)
        , m_High(0)
    {}

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_FORCE_INLINE constexpr explicit s128(const T num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit s128(const u16 num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    ALT_FORCE_INLINE constexpr explicit s128(const u8 num) noexcept
        : m_Low(static_cast<u64>(num))
        , m_High(0)
    {}

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s64>>* = nullptr>
    ALT_FORCE_INLINE constexpr explicit s128(const T num) noexcept
#if !(!defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)))
        : m_Low(static_cast<u64>(num))
        , m_High(Num::IsSignPositive(num) ? 0 : ~static_cast<u64>(0))
#endif
    {
#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = static_cast<__int128>(num);

        *this = Memory::Transmute<__int128, s128>(x);
#endif
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s32>>* = nullptr>
    ALT_FORCE_INLINE constexpr explicit s128(const T num) noexcept
#if !(!defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)))
        : m_Low(static_cast<u64>(num))
        , m_High(Num::IsSignPositive(num) ? 0 : ~static_cast<u64>(0))
#endif
    {
#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = static_cast<__int128>(num);

        *this = Memory::Transmute<__int128, s128>(x);
#endif
    }

    ALT_FORCE_INLINE constexpr explicit s128(const s16 num) noexcept
#if !(!defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)))
        : m_Low(static_cast<u64>(num))
        , m_High(Num::IsSignPositive(num) ? 0 : ~static_cast<u64>(0))
#endif
    {
#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = static_cast<__int128>(num);

        *this = Memory::Transmute<__int128, s128>(x);
#endif
    }

    ALT_FORCE_INLINE constexpr explicit s128(const s8 num) noexcept
#if !(!defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)))
        : m_Low(static_cast<u64>(num))
        , m_High(Num::IsSignPositive(num) ? 0 : ~static_cast<u64>(0))
#endif
    {
#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = static_cast<__int128>(num);

        *this = Memory::Transmute<__int128, s128>(x);
#endif
    }

    ALT_FORCE_INLINE constexpr explicit s128(const f64 num_) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = static_cast<__int128>(num_);

        *this = Memory::Transmute<__int128, s128>(x);

#else

        constexpr u64 MANTISSA_SIZE = DBL_MANT_DIG - 1;
        constexpr u64 MANTISSA_MASK = (static_cast<u64>(1) << MANTISSA_SIZE) - 1;
        constexpr u64 EXP_MASK = (static_cast<u64>(1) << 11) - 1;
        constexpr u64 SIGN_MASK = static_cast<u64>(1) << (Num::Desc<f64>::Bits() - 1);

        const u64 num = Memory::Transmute<f64, u64>(num_);

        u64 exp = (num >> MANTISSA_SIZE) & EXP_MASK;

        if (exp < MANTISSA_SIZE + 1023) {

            static_assert(MANTISSA_SIZE <= 64, "");

            *this = s128(static_cast<u64>(num_));
            return;
        }

        u64 sign = num & SIGN_MASK;
        u64 mantissa = num & MANTISSA_MASK;

        exp -= 1023;

        const u64 highBit = static_cast<u64>(1) << MANTISSA_SIZE;
        mantissa |= exp == 0 ? 0 : highBit;

        exp -= MANTISSA_SIZE;

        *this = s128(static_cast<u64>(mantissa)) << static_cast<usize>(exp);
        *this = sign == 0 ? *this : -*this;

#endif
    }

    ALT_FORCE_INLINE constexpr explicit s128(const f32 num_) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = static_cast<__int128>(num_);

        *this = Memory::Transmute<__int128, s128>(x);

#else

        constexpr u32 MANTISSA_SIZE = FLT_MANT_DIG - 1;
        constexpr u32 MANTISSA_MASK = (static_cast<u32>(1) << MANTISSA_SIZE) - 1;
        constexpr u32 EXP_MASK = (static_cast<u32>(1) << 8) - 1;
        constexpr u32 SIGN_MASK = static_cast<u32>(1) << (Num::Desc<f32>::Bits() - 1);

        const u32 num = Memory::Transmute<f32, u32>(num_);

        u32 exp = (num >> MANTISSA_SIZE) & EXP_MASK;

        if (exp < MANTISSA_SIZE + 127) {

            static_assert(MANTISSA_SIZE <= 64, "");

            *this = s128(static_cast<u64>(num_));
            return;
        }

        u32 sign = num & SIGN_MASK;
        u32 mantissa = num & MANTISSA_MASK;

        exp -= 127;

        const u32 highBit = static_cast<u32>(1) << MANTISSA_SIZE;
        mantissa |= exp == 0 ? 0 : highBit;

        exp -= MANTISSA_SIZE;

        *this = s128(static_cast<u64>(mantissa)) << static_cast<usize>(exp);
        *this = sign == 0 ? *this : -*this;

#endif
    }

    /**
     * @brief Conversions for s128.
     */
    ALT_FORCE_INLINE constexpr explicit operator u128() noexcept {
        return u128::FromHighLow(m_High, m_Low);
    }

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

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return static_cast<f64>(x);

#else

        /**
         * IEEE 754 double-precision binary floating-point format: binary64
         * https://en.wikipedia.org/wiki/Double-precision_floating-point_format#IEEE_754_double-precision_binary_floating-point_format:_binary64
         */

        constexpr usize MANTISSA_SIZE = DBL_MANT_DIG - 1;
        constexpr u64 MANTISSA_MASK = (static_cast<u64>(1) << MANTISSA_SIZE) - 1;
        constexpr u64 SIGN_MASK = static_cast<u64>(1) << (Num::Desc<u64>::Bits() - 1);

        const u64 sign = m_High & SIGN_MASK;
        const u64 highUns = sign == 0 ? m_High : ~m_High + 1;

        if (highUns == 0) {

            const f64 result_ = static_cast<f64>(sign == 0 ? m_Low : ~m_Low + 1);
            const u64 result = Memory::Transmute<f64, u64>(result_) | sign;

            return Memory::Transmute<u64, f64>(result);
        }

        const usize zeroes = Num::Count0BitsLeft(highUns);
        const u32 pow2 = static_cast<u32>(127 - zeroes);

        // 2^exp-1023
        u64 exp = pow2 + 1023;

        // 2^exp-1023 * (1 + 2^MANTISSA_SIZE-i + ...)
        const u64 high = highUns << (zeroes + 1);

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

        const u64 result = (exp << MANTISSA_SIZE) | mantissa | sign;

        return Memory::Transmute<u64, f64>(result);

#endif
    }

    ALT_FORCE_INLINE constexpr explicit operator f32() const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return static_cast<f32>(x);

#else

        /**
         * IEEE 754 standard: binary32
         * https://en.wikipedia.org/wiki/Single-precision_floating-point_format#IEEE_754_standard:_binary32
         */

        constexpr usize MANTISSA_SIZE = FLT_MANT_DIG - 1;
        constexpr u32 MANTISSA_MASK = (static_cast<u32>(1) << MANTISSA_SIZE) - 1;
        constexpr u64 SIGN_MASK = static_cast<u64>(1) << (Num::Desc<u64>::Bits() - 1);

        const u64 sign = (m_High & SIGN_MASK) >> 32;
        const u64 highUns = sign == 0 ? m_High : ~m_High + 1;

        if (highUns == 0) {

            const f32 result_ = static_cast<f32>(sign == 0 ? m_Low : ~m_Low + 1);
            const u32 result = Memory::Transmute<f32, u32>(result_) | static_cast<u32>(sign);

            return Memory::Transmute<u32, f32>(result);
        }

        const usize zeroes = Num::Count0BitsLeft(highUns);
        const u32 pow2 = static_cast<u32>(127 - zeroes);

        // 2^exp-127
        u32 exp = pow2 + 127;

        // 2^exp-127 * (1 + 2^MANTISSA_SIZE-i + ...)
        const u64 high = highUns << (zeroes + 1);

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

        const u32 result = (exp << MANTISSA_SIZE) | mantissa | static_cast<u32>(sign);

        return Memory::Transmute<u32, f32>(result);

#endif
    }

    /**
     * @brief Performs addition of two s128 integers.
     * @param rhs The second s128 operand.
     * @return A new s128 result representing the sum.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s128 y = (s128)20;
     * 
     * const s128 z = x + y;
     * 
     * assert(z == 30);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128 operator+(const s128 rhs) const noexcept {
        return Add(rhs);
    }

    /**
     * @brief Performs addition of an s128 and a 64-bit integer.
     * @param rhs The second operand (s64).
     * @return A new s128 result representing the sum.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * const s128 z = x + y;
     * 
     * assert(z == 30);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128 operator+(const s64 rhs) const noexcept {
        return Add(rhs);
    }

    /**
     * @brief Adds an s128 value to itself in-place.
     * @param rhs The amount to add (s128).
     * @return A reference to this instance updated with the addition.
     * @example
     * 
     * s128 x = (s128)10;
     * const s128 y = (s128)20;
     * 
     * x += y;
     * 
     * assert(x == 30);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128& operator+=(const s128 rhs) noexcept {

        *this = Add(rhs);
        return *this;
    }

    /**
     * @brief Adds an s64 value to itself in-place.
     * @param rhs The amount to add (s64).
     * @return A reference to this instance updated with the addition.
     * @example
     * 
     * s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * x += y;
     * 
     * assert(x == 30);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128& operator+=(const s64 rhs) noexcept {

        *this = Add(rhs);
        return *this;
    }

    /**
     * @brief Adds a 64-bit integer to an s128 value.
     * @param lhs The first operand (s64).
     * @param rhs The second operand (s128).
     * @return A new s128 result representing the addition.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * const s128 z = y + x;
     * 
     * assert(z == 30);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr friend s128 operator+(const s64 lhs, const s128 rhs) noexcept {
        return rhs + lhs;
    }

    /**
     * @brief Performs subtraction of two s128 integers.
     * @param rhs The second s128 operand.
     * @return A new s128 result representing the subtraction.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s128 y = (s128)20;
     * 
     * const s128 z = x - y;
     * 
     * assert(z == -10);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128 operator-(const s128 rhs) const noexcept {
        return Sub(rhs);
    }

    /**
     * @brief Performs subtraction of an s128 and a 64-bit integer.
     * @param rhs The second operand (s64).
     * @return A new s128 result representing the subtraction.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * const s128 z = x - y;
     * 
     * assert(z == -10);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128 operator-(const s64 rhs) const noexcept {
        return Sub(rhs);
    }

    /**
     * @brief Subtracts an s128 value to itself in-place.
     * @param rhs The amount to subtract (s128).
     * @return A reference to this instance updated with the subtraction.
     * @example
     * 
     * s128 x = (s128)10;
     * const s128 y = (s128)20;
     * 
     * x -= y;
     * 
     * assert(x == -10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128& operator-=(const s128 rhs) noexcept {

        *this = Sub(rhs);
        return *this;
    }

    /**
     * @brief Subtracts an s64 value to itself in-place.
     * @param rhs The amount to subtract (s64).
     * @return A reference to this instance updated with the subtraction.
     * @example
     * 
     * s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * x -= y;
     * 
     * assert(x == -10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128& operator-=(const s64 rhs) noexcept {

        *this = Sub(rhs);
        return *this;
    }

    /**
     * @brief Subtracts a 64-bit integer to an s128 value.
     * @param lhs The first operand (s64).
     * @param rhs The second operand (s128).
     * @return A new s128 result representing the subtraction.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * const s128 z = y - x;
     * 
     * assert(z == 10);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr friend s128 operator-(const s64 lhs, const s128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        const __int128 result = lhs - y;

        return Memory::Transmute<__int128, s128>(result);

#else

        return s128(lhs) - rhs;

#endif
    }

    /**
     * @brief Multiplies of two s128 integers.
     * @param lhs The first operand (s128).
     * @param rhs The second operand (s128).
     * @return A new s128 result representing the multiplication.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s128 y = (s128)20;
     * 
     * const s128 z = y * x;
     * 
     * assert(z == 200);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128 operator*(const s128 rhs) const noexcept {
        return Mul(rhs);
    }

    /**
     * @brief Multiplies a 64-bit integer to an s128 value.
     * @param lhs The first operand (s64).
     * @param rhs The second operand (s128).
     * @return A new s128 result representing the multiplication.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * const s128 z = y * x;
     * 
     * assert(z == 200);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128 operator*(const s64 rhs) const noexcept {
        return Mul(rhs);
    }

    /**
     * @brief Multiplies an s128 value to itself in-place.
     * @param rhs The amount to multiply (s128).
     * @return A reference to this instance updated with the multiplication.
     * @example
     * 
     * s128 x = (s128)10;
     * const s128 y = (s128)20;
     * 
     * x *= y;
     * 
     * assert(x == 200);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128& operator*=(const s128 rhs) noexcept {

        *this = Mul(rhs);
        return *this;
    }

    /**
     * @brief Multiplies an s64 value to itself in-place.
     * @param rhs The amount to multiply (s64).
     * @return A reference to this instance updated with the multiplication.
     * @example
     * 
     * s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * x *= y;
     * 
     * assert(x == 200);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128& operator*=(const s64 rhs) noexcept {

        *this = Mul(rhs);
        return *this;
    }

    /**
     * @brief Multiplies a 64-bit integer to an s128 value.
     * @param lhs The first operand (s64).
     * @param rhs The second operand (s128).
     * @return A new s128 result representing the multiplication.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * const s128 z = y * x;
     * 
     * assert(z == 200);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr friend s128 operator*(const s64 lhs, const s128 rhs) noexcept {
        return rhs * lhs;
    }

    /**
     * @brief Performs divisions of two s128 integers.
     * @param rhs The divisor (s128).
     * @return A new s128 result representing the quotient.
     * @example
     * 
     * const s128 x = (s128)20;
     * const s128 y = (s128)10;
     * 
     * const s128 z = x / y;
     * 
     * assert(z == 2);
     * assert(x == 20);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr s128 operator/(const s128 rhs) const noexcept {
        return Div(rhs);
    }

    /**
     * @brief Performs divisions of two s128 integers.
     * @param rhs The divisor (s64).
     * @return A new s128 result representing the quotient.
     * @example
     * 
     * const s128 x = (s128)20;
     * const s64 y = 10;
     * 
     * const s128 z = x / y;
     * 
     * assert(z == 2);
     * assert(x == 20);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr s128 operator/(const s64 rhs) const noexcept {
        return Div(rhs);
    }

    /**
     * @brief Divides an s128 value to itself in-place.
     * @param rhs The divisor (s128).
     * @return A reference to this instance updated with the quotient.
     * @example
     * 
     * s128 x = (s128)20;
     * const s128 y = (s128)10;
     * 
     * x /= y;
     * 
     * assert(x == 2);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr s128& operator/=(const s128 rhs) noexcept {

        *this = Div(rhs);
        return *this;
    }

    /**
     * @brief Divides an s64 value to itself in-place.
     * @param rhs The divisor (s64).
     * @return A reference to this instance updated with the quotient.
     * @example
     * 
     * s128 x = (s128)20;
     * const s64 y = 10;
     * 
     * x /= y;
     * 
     * assert(x == 2);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr s128& operator/=(const s64 rhs) noexcept {

        *this = Div(rhs);
        return *this;
    }

    /**
     * @brief Divides a 64-bit integer to an s128 value.
     * @param lhs The dividend (s64).
     * @param rhs The divisor (s128).
     * @return A new s128 result representing the quotient.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * const s128 z = y / x;
     * 
     * assert(z == 2);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr friend s128 operator/(const s64 lhs, const s128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        const __int128 result = lhs / y;

        return Memory::Transmute<__int128, s128>(result);

#else

        return s128(lhs) / rhs;

#endif
    }

    /**
     * @brief Performs modulo of two s128 integers.
     * @param rhs The divisor (s128).
     * @return A new s128 result representing the remainder.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s128 y = (s128)20;
     * 
     * const s128 z = x % y;
     * 
     * assert(z == 0);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128 operator%(const s128 rhs) const noexcept {
        return Rem(rhs);
    }

    /**
     * @brief Performs modulo of an s128 and a 64-bit integer.
     * @param rhs The divisor (s64).
     * @return A new s128 result representing the remainder.
     * @example
     * 
     * const s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * const s128 z = x % y;
     * 
     * assert(z == 0);
     * assert(x == 10);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128 operator%(const s64 rhs) const noexcept {
        return Rem(rhs);
    }

    /**
     * @brief Performs modulo of two s128 integers.
     * @param rhs The divisor (s128).
     * @return A reference to this instance updated with the remainder.
     * @example
     * 
     * s128 x = (s128)10;
     * const s128 y = (s128)20;
     * 
     * x %= y;
     * 
     * assert(x == 0);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128& operator%=(const s128 rhs) noexcept {

        *this = Rem(rhs);
        return *this;
    }

    /**
     * @brief Performs modulo of an s128 and a 64-bit integer.
     * @param rhs The divisor (s64).
     * @return A reference to this instance updated with the remainder.
     * @example
     * 
     * s128 x = (s128)10;
     * const s64 y = 20;
     * 
     * x %= y;
     * 
     * assert(x == 0);
     * assert(y == 20);
     */
    ALT_FORCE_INLINE constexpr s128& operator%=(const s64 rhs) noexcept {

        *this = Rem(rhs);
        return *this;
    }

    /**
     * @brief Performs modulo a 64-bit integer and s128 value.
     * @param rhs The dividend (s64).
     * @param rhs The divisor (s128).
     * @return A new s128 result representing the remainder.
     * @example
     * 
     * const s128 x = (s128)20;
     * const s64 y = 10;
     * 
     * const s128 z = y % x;
     * 
     * assert(z == 0);
     * assert(x == 20);
     * assert(y == 10);
     */
    ALT_FORCE_INLINE constexpr friend s128 operator%(const s64 lhs, const s128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        const __int128 result = lhs % y;

        return Memory::Transmute<__int128, s128>(result);

#else

        return s128(lhs) % rhs;

#endif
    }

    /**
     * @brief Left bitwise shift.
     * @param shift The number of bits to shift left.
     * @return A new s128 result shifted left.
     * @example
     * 
     * const s128 x = (s128)1 << 104;
     */
    ALT_FORCE_INLINE constexpr s128 operator<<(const usize shift) const noexcept {
        return __shiftleft128__(*this, shift);
    }

    /**
     * @brief Left bitwise shift in-place.
     * @param shift The number of bits to shift left.
     * @return A reference to this instance shifted left.
     * @example
     * 
     * s128 x = 1;
     * x <<= 104;
     */
    ALT_FORCE_INLINE constexpr s128& operator<<=(const usize shift) noexcept {

        *this = __shiftleft128__(*this, shift);
        return *this;
    }

    /**
     * @brief Right bitwise shift.
     * @param shift The number of bits to shift right.
     * @return A new s128 result shifted right.
     * @example
     * 
     * const s128 x = Alt::Desc<s128>::Max() >> 104;
     */
    ALT_FORCE_INLINE constexpr s128 operator>>(const usize shift) const noexcept {
        return __shiftright128__(*this, shift);
    }

    /**
     * @brief Right bitwise shift in-place.
     * @param shift The number of bits to shift right.
     * @return A reference to this instance shifted right.
     * @example
     * 
     * s128 x = Alt::Desc<s128>::Max();
     * x >>= 104;
     */
    ALT_FORCE_INLINE constexpr s128& operator>>=(const usize shift) noexcept {

        *this = __shiftright128__(*this, shift);
        return *this;
    }

    /**
     * @brief Increments the value in-place.
     * @return A reference to this instance incremented by 1.
     */
    ALT_FORCE_INLINE constexpr s128& operator++() noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        __int128 x = Memory::Transmute<s128, __int128>(*this);

        *this = Memory::Transmute<__int128, s128>(++x);

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
    ALT_FORCE_INLINE constexpr s128 operator++(int) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(x++);

#else

        s128 res = *this;
        ++(*this);

        return res;

#endif
    }

    /**
     * @brief Decrements the value in-place.
     * @return A reference to this instance decremented by 1.
     */
    ALT_FORCE_INLINE constexpr s128& operator--() noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        __int128 x = Memory::Transmute<s128, __int128>(*this);

        *this = Memory::Transmute<__int128, s128>(--x);

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
    ALT_FORCE_INLINE constexpr s128 operator--(int) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(x--);

#else

        s128 res = *this;
        --(*this);

        return res;

#endif
    }

    /**
     * @brief Unary plus operator.
     * @return A copy of the current instance.
     */
    ALT_FORCE_INLINE constexpr s128 operator+() const noexcept {

        return *this;
    }

    /**
     * @brief Unary minus operator.
     * @details Negates the value represented by the s128 instance.
     * @return A new s128 result representing the negation.
     * @example
     * 
     * const s128 x = Alt::Desc<s128>::Max();
     * assert(-x == -Alt::Desc<s128>::Max());
     */
    ALT_FORCE_INLINE constexpr s128 operator-() const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(-x);

#else

        s128 res = ~*this;
        ++res;

        return res;

#endif
    }

    /**
     * @brief Bitwise AND operation.
     * @param rhs The second operand (s128).
     * @return A new s128 result of the bitwise AND.
     * @xample
     * 
     * const s128 x = Alt::Desc<s128>::Max();
     * assert((x & Alt::Desc<s128>::Max()) == Alt::Desc<s128>::Max());
     */
    ALT_FORCE_INLINE constexpr s128 operator&(const s128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);
        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        return Memory::Transmute<__int128, s128>(x & y);

#else

        return s128(m_High & rhs.m_High, m_Low & rhs.m_Low);

#endif
    }

    /**
     * @brief Bitwise AND in-place.
     * @param rhs The second operand (s128).
     * @return A reference to this instance updated with the result of the bitwise AND.
     * @xample
     * 
     * s128 x = Alt::Desc<s128>::Max();
     * x &= Alt::Desc<s128>::Max();
     * 
     * assert(x == Alt::Desc<s128>::Max());
     */
    ALT_FORCE_INLINE constexpr s128& operator&=(const s128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);
        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        *this = Memory::Transmute<s128, __int128>(x & y);

        return *this;
#else

        m_Low &= rhs.m_Low;
        m_High &= rhs.m_High;

        return *this;

#endif
    }

    /**
     * @brief Bitwise AND with a 64-bit integer.
     * @param rhs The second operand (s64).
     * @return A new s128 result of the bitwise AND.
     * @example
     * 
     * const s128 x = Alt::Desc<s128>::Max();
     * assert((x & 0) == 0);
     */
    ALT_FORCE_INLINE constexpr s128 operator&(const s64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(x & rhs);

#else

        return s128(Num::IsSignPositive(rhs) ? 0 : m_High, m_Low & static_cast<u64>(rhs));

#endif
    }

    /**
     * @brief Bitwise AND in-place with a 64-bit integer.
     * @param rhs The second operand (s64).
     * @return A reference to this instance updated with the result of the bitwise AND.
     * @xample
     * 
     * s128 x = Alt::Desc<s128>::Max();
     * x &= 0;
     * 
     * assert(x == 0);
     */
    ALT_FORCE_INLINE constexpr s128& operator&=(const s64 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        *this = Memory::Transmute<s128, __int128>(x & rhs);

        return *this;
#else

        m_Low &= static_cast<u64>(rhs);
        m_High = Num::IsSignPositive(rhs) ? 0 : m_High;

        return *this;

#endif
    }

    /**
     * @brief Bitwise AND of a 64-bit integer and an s128.
     * @param lhs The first operand (s64).
     * @param rhs The second operand (s128).
     * @return A new s128 result of the bitwise AND.
     * @xample
     * 
     * s64 x = Alt::Desc<s64>::Max();
     * x &= Alt::Desc<s128>::Max();
     * 
     * assert(x == Alt::Desc<s64>::Max());
     */
    ALT_FORCE_INLINE constexpr friend s128 operator&(const s64 lhs, const s128 rhs) noexcept {
        return rhs & lhs;
    }

    /**
     * @brief Bitwise OR operation.
     * @param rhs The second operand (s128).
     * @return A new s128 result of the bitwise OR.
     * @xample
     * 
     * const s128 x = Alt::Desc<s128>::Max();
     * assert((x | 0) == Alt::Desc<s128>::Max());
     */
    ALT_FORCE_INLINE constexpr s128 operator|(const s128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);
        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        return Memory::Transmute<__int128, s128>(x | y);

#else

        return s128(m_High | rhs.m_High, m_Low | rhs.m_Low);

#endif
    }

    /**
     * @brief Bitwise OR in-place.
     * @param rhs The second operand (s128).
     * @return A reference to this instance updated with the result of the bitwise OR.
     * @xample
     * 
     * s128 x = 0;
     * x |= Alt::Desc<s128>::Max();
     * 
     * assert(x == Alt::Desc<s128>::Max());
     */
    ALT_FORCE_INLINE constexpr s128& operator|=(const s128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);
        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        *this = Memory::Transmute<s128, __int128>(x | y);

        return *this;
#else

        m_Low |= rhs.m_Low;
        m_High |= rhs.m_High;

        return *this;

#endif
    }

    /**
     * @brief Bitwise OR with a 64-bit integer.
     * @param rhs The second operand (s64).
     * @return A new s128 result of the bitwise OR.
     * @example
     * 
     * const s128 x = Alt::Desc<s128>::Max();
     * assert((x | 0) == Alt::Desc<s128>::Max());
     */
    ALT_FORCE_INLINE constexpr s128 operator|(const s64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(x | rhs);

#else

        return s128(Num::IsSignPositive(rhs) ? m_High : ~static_cast<s64>(0), m_Low | static_cast<u64>(rhs));

#endif
    }

    /**
     * @brief Bitwise OR in-place with a 64-bit integer.
     * @param rhs The second operand (s64).
     * @return A reference to this instance updated with the result of the bitwise OR.
     * @xample
     * 
     * s128 x = Alt::Desc<s128>::Max();
     * x |= 0;
     * 
     * assert(x == Alt::Desc<s128>::Max());
     */
    ALT_FORCE_INLINE constexpr s128& operator|=(const s64 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        *this = Memory::Transmute<s128, __int128>(x | rhs);

        return *this;

#else

        m_Low |= static_cast<u64>(rhs);
        m_High = Num::IsSignPositive(rhs) ? m_High : ~static_cast<s64>(0);

        return *this;

#endif
    }

    /**
     * @brief Bitwise OR of a 64-bit integer and an s128.
     * @param lhs The first operand (s64).
     * @param rhs The second operand (s128).
     * @return A new s128 result of the bitwise OR.
     * @xample
     * 
     * s64 x = 0;
     * x |= Alt::Desc<s128>::Max();
     * 
     * assert(x == Alt::Desc<s64>::Max());
     */
    ALT_FORCE_INLINE constexpr friend s128 operator|(const s64 lhs, const s128 rhs) noexcept {
        return rhs | lhs;
    }

    /**
     * @brief Bitwise XOR operation.
     * @param rhs The second operand (s128).
     * @return A new s128 result of the bitwise XOR.
     * @xample
     * 
     * const s128 x = Alt::Desc<s128>::Max();
     * assert((x ^ Alt::Desc<s128>::Max()) == 0);
     */
    ALT_FORCE_INLINE constexpr s128 operator^(const s128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);
        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        return Memory::Transmute<__int128, s128>(x ^ y);

#else

        return s128(m_High ^ rhs.m_High, m_Low ^ rhs.m_Low);

#endif
    }

    /**
     * @brief Bitwise XOR in-place.
     * @param rhs The second operand (s128).
     * @return A reference to this instance updated with the result of the bitwise XOR.
     * @xample
     * 
     * s128 x = Alt::Desc<s128>::Max();
     * x ^= Alt::Desc<s128>::Max();
     * 
     * assert(x == 0);
     */
    ALT_FORCE_INLINE constexpr s128& operator^=(const s128 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);
        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        *this = Memory::Transmute<s128, __int128>(x ^ y);

        return *this;

#else

        m_Low ^= rhs.m_Low;
        m_High ^= rhs.m_High;

        return *this;

#endif
    }

    /**
     * @brief Bitwise XOR with a 64-bit integer.
     * @param rhs The second operand (s64).
     * @return A new s128 result of the bitwise XOR.
     * @example
     * 
     * const s128 x = Alt::Desc<s128>::Max();
     * assert((x ^ 0) == Alt::Desc<s128>::Max());
     */
    ALT_FORCE_INLINE constexpr s128 operator^(const s64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(x ^ rhs);

#else

        return s128(Num::IsSignPositive(rhs) ? m_High : ~m_High, m_Low ^ static_cast<u64>(rhs));

#endif
    }

    /**
     * @brief Bitwise XOR in-place with a 64-bit integer.
     * @param rhs The second operand (s64).
     * @return A reference to this instance updated with the result of the bitwise XOR.
     * @xample
     * 
     * s128 x = Alt::Desc<s128>::Max();
     * x ^= 0;
     * 
     * assert(x == Alt::Desc<s128>::Max());
     */
    ALT_FORCE_INLINE constexpr s128& operator^=(const s64 rhs) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        *this = Memory::Transmute<s128, __int128>(x ^ rhs);

        return *this;

#else

        m_Low ^= static_cast<u64>(rhs);
        m_High = Num::IsSignPositive(rhs) ? m_High : ~m_High;

        return *this;

#endif
    }

    /**
     * @brief Bitwise XOR of a 64-bit integer and an s128.
     * @param lhs The first operand (s64).
     * @param rhs The second operand (s128).
     * @return A new s128 result of the bitwise XOR.
     * @xample
     * 
     * s64 x = Alt::Desc<s64>::Max();
     * x ^= Alt::Desc<s128>::Max();
     * 
     * assert(x == 0);
     */
    ALT_FORCE_INLINE constexpr friend s128 operator^(const s64 lhs, const s128 rhs) noexcept {
        return rhs ^ lhs;
    }

    /**
     * @brief Bitwise NOT operation.
     * @details Negates all bits of the signed 128-bit integer.
     * @return A new s128 result representing the bitwise negation.
     * @example
     * 
     * s128 x = Alt::Desc<s128>::Max();
     * x = ~x;
     * 
     * assert(x == Alt::Desc<s128>::Min());
     */
    ALT_FORCE_INLINE constexpr s128 operator~() const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(~x);

#else

        return s128(~m_High, ~m_Low);

#endif
    }

    /**
     * @brief Comparison operators.
     */
    ALT_FORCE_INLINE constexpr bool operator==(const s128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<s128, __int128>(*this) == Memory::Transmute<s128, __int128>(rhs);

#else

        return m_High == rhs.m_High && m_Low == rhs.m_Low;

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator!=(const s128 rhs) const noexcept {
        return !(*this == rhs);
    }

    ALT_FORCE_INLINE constexpr bool operator<(const s128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<s128, __int128>(*this) < Memory::Transmute<s128, __int128>(rhs);

#else

        return
            static_cast<s64>(m_High) < static_cast<s64>(rhs.m_High) ||
            (static_cast<s64>(m_High) <= static_cast<s64>(rhs.m_High) && m_Low < rhs.m_Low);

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator>(const s128 rhs) const noexcept {
        return rhs < *this;
    }

    ALT_FORCE_INLINE constexpr bool operator<=(const s128 rhs) const noexcept {
        return !(rhs < *this);
    }

    ALT_FORCE_INLINE constexpr bool operator>=(const s128 rhs) const noexcept {
        return !(*this < rhs);
    }

    ALT_FORCE_INLINE constexpr bool operator==(const s64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<s128, __int128>(*this) == rhs;

#else

        return *this == s128(rhs);

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator!=(const s64 rhs) const noexcept {
        return !(*this == rhs);
    }

    ALT_FORCE_INLINE constexpr bool operator<(const s64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<s128, __int128>(*this) < rhs;

#else

        return *this < s128(rhs);

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator>(const s64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        return Memory::Transmute<s128, __int128>(*this) > rhs;

#else

        return *this > s128(rhs);

#endif
    }

    ALT_FORCE_INLINE constexpr bool operator<=(const s64 rhs) const noexcept {
        return !(*this > rhs);
    }

    ALT_FORCE_INLINE constexpr bool operator>=(const s64 rhs) const noexcept {
        return !(*this < rhs);
    }

    ALT_FORCE_INLINE constexpr friend bool operator==(const s64 lhs, const s128 rhs) noexcept {
        return rhs == lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator!=(const s64 lhs, const s128 rhs) noexcept {
        return rhs != lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator<(const s64 lhs, const s128 rhs) noexcept {
        return rhs > lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator>(const s64 lhs, const s128 rhs) noexcept {
        return rhs < lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator<=(const s64 lhs, const s128 rhs) noexcept {
        return rhs >= lhs;
    }

    ALT_FORCE_INLINE constexpr friend bool operator>=(const s64 lhs, const s128 rhs) noexcept {
        return rhs <= lhs;
    }

    /**
     * @brief Adds an s128 to itself.
     * @param rhs The second operand (s128).
     * @return A new s128 result representing the sum.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Add(const s128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);
        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        return Memory::Transmute<__int128, s128>(x + y);

#else

        s128 result;

        result.m_Low = __uadd128__(m_Low, rhs.m_Low, result.m_High);
        result.m_High += m_High + rhs.m_High;

        return result;

#endif
    }

    /**
     * @brief Adds an s64.
     * @param rhs The second operand (s64).
     * @return A new s128 result representing the sum.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Add(const s64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(x + rhs);

#else

        return Add(s128(rhs));

#endif
    }

    /**
     * @brief Subtracts an s128 from itself.
     * @param rhs The second operand (s128).
     * @return A new s128 result representing the difference.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Sub(const s128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);
        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        return Memory::Transmute<__int128, s128>(x - y);

#else

        s128 result;

        u64 hi = 0;
        result.m_Low = __sub128__(m_Low, rhs.m_Low, hi);
        result.m_High = m_High - rhs.m_High - hi;

        return result;

#endif
    }

    /**
     * @brief Subtracts an s64 from itself.
     * @param rhs The second operand (s64).
     * @return A new s128 result representing the difference.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Sub(const s64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(x - rhs);

#else

        return Sub(s128(rhs));

#endif
    }

    /**
     * @brief Multiplies itself by an s128.
     * @param rhs The second operand (s128).
     * @return A new s128 result representing the product.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Mul(const s128 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);
        const __int128 y = Memory::Transmute<s128, __int128>(rhs);

        return Memory::Transmute<__int128, s128>(x * y);

#else

        s128 result;
        result.m_Low = __umul128__(m_Low, rhs.m_Low, result.m_High);

        result.m_High += m_High * rhs.m_Low;
        result.m_High += m_Low * rhs.m_High;

        return result;

#endif
    }

    /**
     * @brief Multiplies itself by an s64.
     * @param rhs The second operand (s64).
     * @return A new s128 result representing the product.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Mul(const s64 rhs) const noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__) && (defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC))

        const __int128 x = Memory::Transmute<s128, __int128>(*this);

        return Memory::Transmute<__int128, s128>(x * rhs);

#else

        return Mul(s128(rhs));

#endif
    }

    /**
     * @brief Division of the instance by another s128.
     * @param rhs The divisor (s128).
     * @return A new s128 result representing the quotient.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Div(const s128 rhs) const noexcept {

        s128 rem;
        return __sdiv128__(*this, rhs, rem);
    }

    /**
     * @brief Modulo operation of the instance by another s128.
     * @param rhs The divisor (s128).
     * @return A new s128 result representing the remainder.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Rem(const s128 rhs) const noexcept {

        s128 rem;
        ALT_MAYBE_UNUSED const s128 div = __sdiv128__(*this, rhs, rem);

        return rem;
    }

    /**
     * @brief Division of the instance by an s64.
     * @param rhs The divisor (s64).
     * @return A new s128 result representing the quotient.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Div(const s64 rhs) const noexcept {

        s128 rem;
        return __sdiv128__(*this, rhs, rem);
    }

    /**
     * @brief Modulo operation of the instance by an s64.
     * @param rhs The divisor (s64).
     * @return A new s128 result representing the remainder.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Rem(const s64 rhs) const noexcept {

        s128 rem;
        ALT_MAYBE_UNUSED const s128 div = __sdiv128__(*this, rhs, rem);

        return rem;
    }

    /**
     * @brief Division and modulo.
     * @param rhs The divisor (s128).
     * @param rem Output parameter for the remainder.
     * @return A new s128 result representing the quotient.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 DivRem(const s128 rhs, s128& rem) const noexcept {

        return __sdiv128__(*this, rhs, rem);
    }

    /**
     * @brief Division and modulo with an s64.
     * @param rhs The divisor (s64).
     * @param rem Output parameter for the remainder.
     * @return A new s128 result representing the quotient.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 DivRem(const s64 rhs, s128& rem) const noexcept {

        return __sdiv128__(*this, rhs, rem);
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

    /**
     * @brief Swaps the byte order of the 128-bit integer.
     * @return A new s128 instance with swapped bytes.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 ByteSwap() const noexcept {
        return s128(Memory::ByteSwap(m_Low), Memory::ByteSwap(m_High));
    }

    /**
     * @brief Checks if the value is positive.
     * @return True if the sign bit of the high component indicates a positive value.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsSignPositive() const noexcept {
        return Num::IsSignPositive(Memory::Transmute<u64, s64>(m_High));
    }

    /**
     * @brief Returns the absolute value of the integer.
     * @return A new s128 instance representing $|x|$.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s128 Abs() const noexcept {
        return IsSignPositive() ? *this : -*this;
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
    ALT_NODISCARD ALT_FORCE_INLINE constexpr s64 GetHigh() const noexcept {
        return Memory::Transmute<u64, s64>(m_High);
    }

private:

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

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 __sdiv128__(const s128 x_, const s128 y_, s128& rem_) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__)

        const __int128 x = Memory::Transmute<s128, __int128>(x_);
        const __int128 y = Memory::Transmute<s128, __int128>(y_);

        const __int128 result = x / y;
        const __int128 rem = x % y;

        rem_ = Memory::Transmute<__int128, s128>(rem);
        return Memory::Transmute<__int128, s128>(result);

#else

        const bool isPlusX = x_.IsSignPositive();
        const bool isPlusY = y_.IsSignPositive();

        u128 x = Memory::Transmute<s128, u128>(x_);
        x = isPlusX ? x : ++(~x);

        u128 y = Memory::Transmute<s128, u128>(y_);
        y = isPlusY ? y : ~y + 1;

        u128 rem;
        x = x.DivRem(y, rem);

        s128 result = Memory::Transmute<u128, s128>(x);
        result = isPlusX == isPlusY ? result : -result;

        rem_ = Memory::Transmute<u128, s128>(rem);

        return result;

#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 __sdiv128__(const s128 x_, const s64 y_, s128& rem_) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__)

        const __int128 x = Memory::Transmute<s128, __int128>(x_);

        const __int128 result = x / y_;
        const __int128 rem = x % y_;

        rem_ = Memory::Transmute<__int128, s128>(rem);
        return Memory::Transmute<__int128, s128>(result);

#else

        const bool isPlusX = x_.IsSignPositive();
        const bool isPlusY = Num::IsSignPositive(y_);

        u128 x = Memory::Transmute<s128, u128>(x_);
        x = isPlusX ? x : ++(~x);

        u64 y = Memory::Transmute<s64, u64>(y_);
        y = isPlusY ? y : ~y + 1;

        u128 rem;
        x = x.DivRem(y, rem);

        s128 result = Memory::Transmute<u128, s128>(x);
        result = isPlusX == isPlusY ? result : -result;

        rem_ = Memory::Transmute<u128, s128>(rem);

        return result;

#endif
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 __shiftleft128__(const s128 x, const usize shift) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__)

        const __int128 xx = Memory::Transmute<s128, __int128>(x);

        return Memory::Transmute<__int128, s128>(xx << shift);

#else

        ALT_IF_IS_CONSTANT_EVALUATED {

            s128 result;

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

        s128 result;

        if (shift >= 64) {

            result.m_High = x.m_Low << (shift - 64);
        }
        else {

            result.m_Low = x.m_Low << shift;
            result.m_High = __shiftleft128(x.m_Low, x.m_High, static_cast<u8>(shift));
        }

        return result;

#elif defined(ALT_PLATFORM_X86_32)

        s128 result;

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

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 __shiftright128__(const s128 x, const usize shift) noexcept {

#if !defined(ALT_USE_CUSTOM_128) && defined(__SIZEOF_INT128__)

        const __int128 xx = Memory::Transmute<s128, __int128>(x);

        return Memory::Transmute<__int128, s128>(xx >> shift);

#else

        ALT_IF_IS_CONSTANT_EVALUATED {

            s128 result;

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

        s128 result;

        if (shift >= 64) {

            result.m_Low = x.m_High >> (shift - 64);
        }
        else {

            result.m_Low = __shiftright128(x.m_Low, x.m_High, static_cast<u8>(shift));
            result.m_High = x.m_High >> shift;
        }

        return result;

#elif defined(ALT_PLATFORM_X86_32)

        s128 result;

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

    ALT_FORCE_INLINE constexpr s128(const s64 hi, const u64 lo) noexcept
        : m_Low(lo)
        , m_High(Memory::Transmute<s64, u64>(hi))
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
struct __is_number__<Types::s128>
    : std::true_type
{};

template <>
struct __is_trivially_reallocatable__<Types::s128>
    : std::true_type
{};

} // namespace TypeTraits

namespace Num {

/**
 * @brief Num::Description for s128.
 */
template <>
struct Desc<s128> {

    using Signed = s128;
    using Unsigned = u128;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 Min() noexcept {
        return s128::Min();
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 Max() noexcept {
        return s128::Max();
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 Lowest() noexcept {
        return s128::Min();
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s128 SignalingNaN() noexcept {
        return 0;
    }
    
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsUnsigned() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsSigned() noexcept {
        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsFloat() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize DigitCount() noexcept {
        return 39;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<s128>;
    }
};

} // namespace Num
} // namespace Alt

#endif // __ALT_STD_S128__