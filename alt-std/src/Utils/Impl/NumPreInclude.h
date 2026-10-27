#pragma once

#ifndef __ALT_STD_NUM_PRE_INCLUDE__
#define __ALT_STD_NUM_PRE_INCLUDE__

#include "../../TypeTraits.h"

#if defined(ALT_COMPILER_MSVC)

#include <intrin.h>
#endif // defined(ALT_COMPILER_MSVC)

namespace Alt {
namespace _NumDetails {

template <typename T>
struct type_to_bits : std::conditional_t<
    sizeof(T) == 1,
    std::integral_constant<usize, 8>,

    std::conditional_t<
        sizeof(T) == 2,
        std::integral_constant<usize, 16>,

        std::conditional_t<
            sizeof(T) == 4,
            std::integral_constant<usize, 32>,

            std::conditional_t<
                sizeof(T) == 8,
                std::integral_constant<usize, 64>,
                std::integral_constant<usize, 0>
            >
        >
    >
>
{};

template <typename T>
constexpr static usize type_to_bits_v = type_to_bits<T>::value;

} // namespace _NumDetails

namespace Num {

/**
 * @brief Provides description of properties for numeric types.
 * 
 * @details This template is specialized for various numbers (u8, s16, etc.) and 
 * floating-point (f32, f64) types to provide access to limits, size, and arithmetic
 * characteristics.
 * 
 * @tparam T The numeric type to describe (e.g., u32, f64).
 */
template <typename T>
struct Desc;

/**
 * @brief Description for u8.
 */
template <>
struct Desc<u8> {

    using Signed = s8;
    using Unsigned = u8;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u8 Min() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u8 Max() noexcept {
        return UINT8_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u8 Lowest() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u8 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u8 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u8 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u8 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u8 SignalingNaN() noexcept {
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
        return 3;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<u8>;
    }
};

/**
 * @brief Description for s8.
 */
template <>
struct Desc<s8> {

    using Signed = s8;
    using Unsigned = u8;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s8 Min() noexcept {
        return INT8_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s8 Max() noexcept {
        return INT8_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s8 Lowest() noexcept {
        return INT8_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s8 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s8 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s8 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s8 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s8 SignalingNaN() noexcept {
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
        return 3;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<s8>;
    }
};

/**
 * @brief Description for u16.
 */
template <>
struct Desc<u16> {

    using Signed = s16;
    using Unsigned = u16;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u16 Min() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u16 Max() noexcept {
        return UINT16_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u16 Lowest() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u16 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u16 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u16 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u16 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u16 SignalingNaN() noexcept {
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
        return 5;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<u16>;
    }
};

/**
 * @brief Description for s16.
 */
template <>
struct Desc<s16> {

    using Signed = s16;
    using Unsigned = u16;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s16 Min() noexcept {
        return INT16_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s16 Max() noexcept {
        return INT16_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s16 Lowest() noexcept {
        return INT16_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s16 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s16 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s16 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s16 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s16 SignalingNaN() noexcept {
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
        return 5;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<s16>;
    }
};

/**
 * @brief Description for u32.
 */
template <>
struct Desc<u32> {

    using Signed = s32;
    using Unsigned = u32;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u32 Min() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u32 Max() noexcept {
        return UINT32_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u32 Lowest() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u32 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u32 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u32 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u32 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u32 SignalingNaN() noexcept {
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
        return 10;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<u32>;
    }
};

/**
 * @brief Description for s32.
 */
template <>
struct Desc<s32> {

    using Signed = s32;
    using Unsigned = u32;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s32 Min() noexcept {
        return INT32_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s32 Max() noexcept {
        return INT32_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s32 Lowest() noexcept {
        return INT32_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s32 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s32 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s32 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s32 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s32 SignalingNaN() noexcept {
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
        return 10;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<s32>;
    }
};

/**
 * @brief Description for u64.
 */
template <>
struct Desc<u64> {

    using Signed = s64;
    using Unsigned = u64;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 Min() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 Max() noexcept {
        return UINT64_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 Lowest() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 SignalingNaN() noexcept {
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
        return 20;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<u64>;
    }
};

/**
 * @brief Description for s64.
 */
template <>
struct Desc<s64> {

    using Signed = s64;
    using Unsigned = u64;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s64 Min() noexcept {
        return INT64_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s64 Max() noexcept {
        return INT64_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s64 Lowest() noexcept {
        return INT64_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s64 Eps() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s64 DenormMin() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s64 Inf() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s64 QuietNaN() noexcept {
        return 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static s64 SignalingNaN() noexcept {
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
        return 19;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<s64>;
    }
};

/**
 * @brief Description for f32.
 */
template <>
struct Desc<f32> {

    using Signed = s32;
    using Unsigned = u32;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f32 Min() noexcept {
        return FLT_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f32 Max() noexcept {
        return FLT_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f32 Lowest() noexcept {
        return -FLT_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f32 Eps() noexcept {
        return FLT_EPSILON;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f32 DenormMin() noexcept {
        return FLT_TRUE_MIN;
    }

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnan-infinity-disabled"
#endif// ALT_COMPILER_CLANG

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f32 Inf() noexcept {
        return __builtin_huge_valf();
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f32 QuietNaN() noexcept {
        return __builtin_nanf("0");
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f32 SignalingNaN() noexcept {
        return __builtin_nansf("1");
    }

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic pop
#endif// ALT_COMPILER_CLANG

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsUnsigned() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsSigned() noexcept {
        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsFloat() noexcept {
        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize DigitCount() noexcept {
        return FLT_DECIMAL_DIG;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<f32>;
    }
};

/**
 * @brief Description for f64.
 */
template <>
struct Desc<f64> {

    using Signed = s64;
    using Unsigned = u64;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f64 Min() noexcept {
        return DBL_MIN;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f64 Max() noexcept {
        return DBL_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f64 Lowest() noexcept {
        return -DBL_MAX;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f64 Eps() noexcept {
        return DBL_EPSILON;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f64 DenormMin() noexcept {
        return DBL_TRUE_MIN;
    }

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnan-infinity-disabled"
#endif// ALT_COMPILER_CLANG

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f64 Inf() noexcept {
        return __builtin_huge_val();
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f64 QuietNaN() noexcept {
        return __builtin_nan("0");
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static f64 SignalingNaN() noexcept {
        return __builtin_nans("1");
    }

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic pop
#endif// ALT_COMPILER_CLANG

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsUnsigned() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsSigned() noexcept {
        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsFloat() noexcept {
        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize DigitCount() noexcept {
        return DBL_DECIMAL_DIG;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize Bits() noexcept {
        return _NumDetails::type_to_bits_v<f64>;
    }
};

} // namespace Num

namespace _NumDetails {

template <typename T>
struct IsOverflowCheckAddImpl;

template <>
struct IsOverflowCheckAddImpl<u8> {

    ALT_FORCE_INLINE static bool IsOverflowCheckAdd(const u8 x, const u8 y, u8& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        return _addcarry_u8(0, x, y, &res) != 0;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_add_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckAddWrap(const u8 x, const u8 y, u8& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        u8 res_;
        const bool isOverflow = _addcarry_u8(0, x, y, &res_) != 0;
        res = isOverflow ? Num::Desc<u8>::Max() : res_;

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u8 res_;
        const bool isOverflow = __builtin_add_overflow(x, y, &res_);
        res = isOverflow ? Num::Desc<u8>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckAddWrap(const u8 x, const u8 y, u8& res, const u8 wrap) noexcept {

#if defined(ALT_COMPILER_MSVC)

        u8 res_;
        bool isOverflow = _addcarry_u8(0, x, y, &res_) != 0;
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u8 res_;
        bool isOverflow = __builtin_add_overflow(x, y, &res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <>
struct IsOverflowCheckAddImpl<u16> {

    ALT_FORCE_INLINE static bool IsOverflowCheckAdd(const u16 x, const u16 y, u16& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        return _addcarry_u16(0, x, y, &res) != 0;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_add_overflow(x, y, &res);

#else

        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckAddWrap(const u16 x, const u16 y, u16& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        u16 res_;
        const bool isOverflow = _addcarry_u16(0, x, y, &res_) != 0;
        res = isOverflow ? Num::Desc<u16>::Max() : res_;

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u16 res_;
        const bool isOverflow = __builtin_add_overflow(x, y, &res_);
        res = isOverflow ? Num::Desc<u16>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckAddWrap(const u16 x, const u16 y, u16& res, const u16 wrap) noexcept {

#if defined(ALT_COMPILER_MSVC)

        u16 res_;
        bool isOverflow = _addcarry_u16(0, x, y, &res_) != 0;
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u16 res_;
        bool isOverflow = __builtin_add_overflow(x, y, &res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <>
struct IsOverflowCheckAddImpl<u32> {

    ALT_FORCE_INLINE static bool IsOverflowCheckAdd(const u32 x, const u32 y, u32& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        return _addcarry_u32(0, x, y, &res) != 0;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_add_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckAddWrap(const u32 x, const u32 y, u32& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        u32 res_;
        const bool isOverflow = _addcarry_u32(0, x, y, &res_) != 0;
        res = isOverflow ? Num::Desc<u32>::Max() : res_;

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u32 res_;
        const bool isOverflow = __builtin_add_overflow(x, y, &res_);
        res = isOverflow ? Num::Desc<u32>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckAddWrap(const u32 x, const u32 y, u32& res, const u32 wrap) noexcept {

#if defined(ALT_COMPILER_MSVC)

        u32 res_;
        bool isOverflow = _addcarry_u32(0, x, y, &res_) != 0;
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u32 res_;
        bool isOverflow = __builtin_add_overflow(x, y, &res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <>
struct IsOverflowCheckAddImpl<u64> {

    ALT_FORCE_INLINE static bool IsOverflowCheckAdd(const u64 x, const u64 y, u64& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        return _addcarry_u64(0, x, y, &res) != 0;

#elif defined(ALT_PLATFORM_X86)

        res = x + y;
        const bool isOverflow = res < x;

        return isOverflow;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_add_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckAddWrap(const u64 x, const u64 y, u64& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        u64 res_;
        const bool isOverflow = _addcarry_u64(0, x, y, &res_) != 0;
        res = isOverflow ? Num::Desc<u64>::Max() : res_;

        return isOverflow;

#elif defined(ALT_PLATFORM_X86)

        const u64 res_ = x + y;
        const bool isOverflow = res_ < x;
        res = isOverflow ? Num::Desc<u64>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u64 res_;
        const bool isOverflow = __builtin_add_overflow(x, y, &res_);
        res = isOverflow ? Num::Desc<u64>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckAddWrap(const u64 x, const u64 y, u64& res, const u64 wrap) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        u64 res_;
        bool isOverflow = _addcarry_u64(0, x, y, &res_) != 0;
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#elif defined(ALT_PLATFORM_X86)

        const u64 res_ = x + y;
        const bool isOverflow = res_ < x || res_ > wrap;
        res = isOverflow ? Num::Desc<u64>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u64 res_;
        bool isOverflow = __builtin_add_overflow(x, y, &res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <typename T>
struct IsUnderflowCheckSubImpl;

template <>
struct IsUnderflowCheckSubImpl<u8> {

    ALT_FORCE_INLINE static bool IsUnderflowCheckSub(const u8 x, const u8 y, u8& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        return _subborrow_u8(0, x, y, &res) != 0;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_sub_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <>
struct IsUnderflowCheckSubImpl<u16> {

    ALT_FORCE_INLINE static bool IsUnderflowCheckSub(const u16 x, const u16 y, u16& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        return _subborrow_u16(0, x, y, &res) != 0;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_sub_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <>
struct IsUnderflowCheckSubImpl<u32> {

    ALT_FORCE_INLINE static bool IsUnderflowCheckSub(const u32 x, const u32 y, u32& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        return _subborrow_u32(0, x, y, &res) != 0;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_sub_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <>
struct IsUnderflowCheckSubImpl<u64> {

    ALT_FORCE_INLINE static bool IsUnderflowCheckSub(const u64 x, const u64 y, u64& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        return _subborrow_u64(0, x, y, &res) != 0;

#elif defined(ALT_PLATFORM_X86)

        res = x - y;
        const bool isUnderflow = res > x;

        return isUnderflow;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_sub_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <typename T>
struct IsOverflowCheckMulImpl;

template <>
struct IsOverflowCheckMulImpl<u8> {

    ALT_FORCE_INLINE static bool IsOverflowCheckMul(const u8 x, const u8 y, u8& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        const u16 res_ = static_cast<u16>(x) * static_cast<u16>(y);
        res = static_cast<u8>(res_);

        return res_ > static_cast<u16>(Num::Desc<u8>::Max());

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_mul_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckMulWrap(const u8 x, const u8 y, u8& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        const u16 res_ = static_cast<u16>(x) * static_cast<u16>(y);

        const bool isOverflow = res_ > static_cast<u16>(Num::Desc<u8>::Max());
        res = isOverflow ? Num::Desc<u8>::Max() : static_cast<u8>(res_);

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u8 res_;
        const bool isOverflow = __builtin_mul_overflow(x, y, &res_);
        res = isOverflow ? Num::Desc<u8>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckMulWrap(const u8 x, const u8 y, u8& res, const u8 wrap) noexcept {

#if defined(ALT_COMPILER_MSVC)

        const u16 res_ = static_cast<u16>(x) * static_cast<u16>(y);

        const bool isOverflow = res_ > static_cast<u16>(wrap);
        res = isOverflow ? wrap : static_cast<u8>(res_);

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u8 res_;
        bool isOverflow = __builtin_mul_overflow(x, y, &res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <>
struct IsOverflowCheckMulImpl<u16> {

    ALT_FORCE_INLINE static bool IsOverflowCheckMul(const u16 x, const u16 y, u16& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        const u32 res_ = static_cast<u32>(x) * static_cast<u32>(y);
        res = static_cast<u16>(res_);

        return res_ > static_cast<u32>(Num::Desc<u16>::Max());

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_mul_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckMulWrap(const u16 x, const u16 y, u16& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        const u32 res_ = static_cast<u32>(x) * static_cast<u32>(y);

        const bool isOverflow = res_ > static_cast<u32>(Num::Desc<u16>::Max());
        res = isOverflow ? Num::Desc<u16>::Max() : static_cast<u16>(res_);

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u16 res_;
        const bool isOverflow = __builtin_mul_overflow(x, y, &res_);
        res = isOverflow ? Num::Desc<u16>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckMulWrap(const u16 x, const u16 y, u16& res, const u16 wrap) noexcept {

#if defined(ALT_COMPILER_MSVC)

        const u32 res_ = static_cast<u32>(x) * static_cast<u32>(y);

        const bool isOverflow = res_ > static_cast<u32>(wrap);
        res = isOverflow ? wrap : static_cast<u16>(res_);

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u16 res_;
        bool isOverflow = __builtin_mul_overflow(x, y, &res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <>
struct IsOverflowCheckMulImpl<u32> {

    ALT_FORCE_INLINE static bool IsOverflowCheckMul(const u32 x, const u32 y, u32& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        const u64 res_ = __emulu(x, y);
        res = static_cast<u32>(res_);

        return res_ > static_cast<u64>(Num::Desc<u32>::Max());

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_mul_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckMulWrap(const u32 x, const u32 y, u32& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

        const u64 res_ = __emulu(x, y);

        const bool isOverflow = res_ > static_cast<u64>(Num::Desc<u32>::Max());
        res = isOverflow ? Num::Desc<u32>::Max() : static_cast<u32>(res_);

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u32 res_;
        const bool isOverflow = __builtin_mul_overflow(x, y, &res_);
        res = isOverflow ? Num::Desc<u32>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckMulWrap(const u32 x, const u32 y, u32& res, const u32 wrap) noexcept {

#if defined(ALT_COMPILER_MSVC)

        const u64 res_ = __emulu(x, y);

        const bool isOverflow = res_ > static_cast<u64>(wrap);
        res = isOverflow ? wrap : static_cast<u32>(res_);

        return isOverflow;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u32 res_;
        bool isOverflow = __builtin_mul_overflow(x, y, &res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

template <>
struct IsOverflowCheckMulImpl<u64> {

    ALT_FORCE_INLINE static bool IsOverflowCheckMul(const u64 x, const u64 y, u64& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        u64 hi;
        res = _umul128(x, y, &hi);
        return hi != 0;

#elif defined(ALT_PLATFORM_X86)

        const u64 ad = __emulu(static_cast<u32>(x >> 32), static_cast<u32>(y));
        const u64 bd = __emulu(static_cast<u32>(x), static_cast<u32>(y));
        const u64 adbc = ad + __emulu(static_cast<u32>(x), static_cast<u32>(y >> 32));

        const u64 lo = bd + (adbc << 32);

        u64 carry = adbc >> 32;
        carry += adbc < ad ? 1 : 0;
        carry += lo < bd ? 1 : 0;

        const bool isOverflow = ((x >> 32) != 0 && (y >> 32) != 0) || carry != 0;

        res = lo;

        return isOverflow;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        return __builtin_mul_overflow(x, y, &res);

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckMulWrap(const u64 x, const u64 y, u64& res) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        u64 hi;
        const u64 res_ = _umul128(x, y, &hi);
        const bool isOverflow = hi != 0;

        res = isOverflow ? Num::Desc<u64>::Max() : static_cast<u64>(res_);

        return isOverflow;

#elif defined(ALT_PLATFORM_X86)

        const u64 ad = __emulu(static_cast<u32>(x >> 32), static_cast<u32>(y));
        const u64 bd = __emulu(static_cast<u32>(x), static_cast<u32>(y));
        const u64 adbc = ad + __emulu(static_cast<u32>(x), static_cast<u32>(y >> 32));

        const u64 lo = bd + (adbc << 32);

        u64 carry = adbc >> 32;
        carry += adbc < ad ? 1 : 0;
        carry += lo < bd ? 1 : 0;

        const bool isOverflow = ((x >> 32) != 0 && (y >> 32) != 0) || carry != 0;

        res = isOverflow ? Num::Desc<u64>::Max() : lo;

        return isOverflow;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u64 res_;
        const bool isOverflow = __builtin_mul_overflow(x, y, &res_);

        res = isOverflow ? Num::Desc<u64>::Max() : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }

    ALT_FORCE_INLINE static bool IsOverflowCheckMulWrap(const u64 x, const u64 y, u64& res, const u64 wrap) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

        u64 hi;
        const u64 res_ = _umul128(x, y, &hi);
        const bool isOverflow = hi != 0 || res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#elif defined(ALT_PLATFORM_X86)

        const u64 ad = __emulu(static_cast<u32>(x >> 32), static_cast<u32>(y));
        const u64 bd = __emulu(static_cast<u32>(x), static_cast<u32>(y));
        const u64 adbc = ad + __emulu(static_cast<u32>(x), static_cast<u32>(y >> 32));

        const u64 lo = bd + (adbc << 32);

        u64 carry = adbc >> 32;
        carry += adbc < ad ? 1 : 0;
        carry += lo < bd ? 1 : 0;

        bool isOverflow = ((x >> 32) != 0 && (y >> 32) != 0) || carry != 0;
        isOverflow |= lo > wrap;

        res = isOverflow ? wrap : lo;

        return isOverflow;

#else
        #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

        u64 res_;
        bool isOverflow = __builtin_mul_overflow(x, y, &res_);
        isOverflow |= res_ > wrap;

        res = isOverflow ? wrap : res_;

        return isOverflow;

#else
        #error provide implementation for specific compiler/platform
#endif
    }
};

class AddOverflowCheckImpl {
public:

    template <typename Usize, typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE bool AddOverflowCheck(Usize& total, HeadT size_, TailT... sizes) noexcept {

        const Usize size = static_cast<Usize>(size_);
        m_IsOverflow |= IsOverflowCheckAddImpl<Usize>::IsOverflowCheckAdd(total, size, total);

        return AddOverflowCheck(total, sizes...);
    }

    template <typename Usize>
    ALT_FORCE_INLINE bool AddOverflowCheck(Usize&) noexcept { return !m_IsOverflow; }

private:

    bool m_IsOverflow = false;
};

class SubUnderflowCheckImpl {
public:

    template <typename Usize, typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE bool SubUnderflowCheck(Usize& total, HeadT size_, TailT... sizes) noexcept {

        const Usize size = static_cast<Usize>(size_);
        m_IsUnderflow |= IsUnderflowCheckSubImpl<Usize>::IsUnderflowCheckSub(total, size, total);

        return SubUnderflowCheck(total, sizes...);
    }

    template <typename Usize>
    ALT_FORCE_INLINE bool SubUnderflowCheck(Usize&) noexcept { return !m_IsUnderflow; }

private:

    bool m_IsUnderflow = false;
};

class MulOverflowCheckImpl {
public:

    template <typename Usize, typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE bool MulOverflowCheck(Usize& total, HeadT size_, TailT... sizes) noexcept {

        const Usize size = static_cast<Usize>(size_);
        m_IsOverflow |= IsOverflowCheckMulImpl<Usize>::IsOverflowCheckMul(total, size, total);

        return MulOverflowCheck(total, sizes...);
    }

    template <typename Usize>
    ALT_FORCE_INLINE bool MulOverflowCheck(Usize&) noexcept { return !m_IsOverflow; }

private:

    bool m_IsOverflow = false;
};

} // namespace _NumDetails

namespace TypeTraits {

template <usize SIZE>
using count_to_type_t = std::conditional_t<
    SIZE < Num::Desc<u8>::Max(),
    u8,

    std::conditional_t<
        SIZE < Num::Desc<u16>::Max(),
        u16,

        std::conditional_t<
            SIZE < Num::Desc<u32>::Max(),
            u32,

            std::conditional_t<
                SIZE < Num::Desc<u64>::Max(),
                u64,
                void
            >
        >
    >
>;

} // namespace TypeTraits
} // namespace Alt

#endif // __ALT_STD_NUM_PRE_INCLUDE__