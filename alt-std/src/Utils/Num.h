#pragma once

#ifndef __ALT_STD_NUM__
#define __ALT_STD_NUM__

#include "../Utils/Impl/NumPreInclude.h"
#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Utils/Optional.h"

namespace Alt {
namespace Num {

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize PopcountConstexpr(u32 x) noexcept {

    x -= (x >> 1) & 0x55555555u;
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0F0F0F0Fu;
    x *= 0x01010101u;

    return static_cast<usize>(x >> 24);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize PopcountConstexpr(u64 x) noexcept {

    x -= (x >> 1) & 0x5555555555555555ull;
    x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0Full;
    x *= 0x0101010101010101ull;

    return static_cast<usize>(x >> 56);
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize PopcountConstexpr(const T x) noexcept {

    return PopcountConstexpr(static_cast<u32>(x));
}

ALT_NODISCARD ALT_FORCE_INLINE usize Popcount(const u32 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

    return static_cast<usize>(__popcnt(num));

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return static_cast<usize>(__builtin_popcount(num));

#else
    #error provide implementation for specific compiler/platform
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE usize Popcount(const u64 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

    return static_cast<usize>(__popcnt64(num));

#elif defined(ALT_PLATFORM_X86)

    return static_cast<usize>(__popcnt(static_cast<u32>(num)) + __popcnt(static_cast<u32>(num >> 32)));

#else
    #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return static_cast<usize>(__builtin_popcountll(num));

#else
    #error provide implementation for specific compiler/platform
#endif
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE usize Popcount(const T x) noexcept {

    return Popcount(static_cast<u32>(x));
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsRightConstexpr(u32 x) noexcept {

    usize n = 31;

    x &= ~x + 1;
    if ((x & 0x0000FFFF) != 0) n -= 16;
    if ((x & 0x00FF00FF) != 0) n -= 8;
    if ((x & 0x0F0F0F0F) != 0) n -= 4;
    if ((x & 0x33333333) != 0) n -= 2;
    if ((x & 0x55555555) != 0) n -= 1;
    if (x == 0) { n += 1; }

    return n;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsRightConstexpr(u64 x) noexcept {

    usize n = 63;

    x &= ~x + 1;
    if ((x & 0x00000000FFFFFFFF) != 0) n -= 32;
    if ((x & 0x0000FFFF0000FFFF) != 0) n -= 16;
    if ((x & 0x00FF00FF00FF00FF) != 0) n -= 8;
    if ((x & 0x0F0F0F0F0F0F0F0F) != 0) n -= 4;
    if ((x & 0x3333333333333333) != 0) n -= 2;
    if ((x & 0x5555555555555555) != 0) n -= 1;
    if (x == 0) { n += 1; }

    return n;
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsRightConstexpr(const T x) noexcept {

    return Count0BitsRightConstexpr(static_cast<u32>(x) | static_cast<u32>(1 << Desc<T>::Bits()));
}

ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsRightUnsafe(const u32 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

    unsigned long result;
    ALT_MAYBE_UNUSED const u8 isNonZero = _BitScanForward(&result, num);
    return static_cast<usize>(result);

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return static_cast<usize>(__builtin_ctzl(num));

#else
    #error provide implementation for specific compiler/platform
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsRightUnsafe(const u64 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

    unsigned long result;
    ALT_MAYBE_UNUSED const u8 isNonZero = _BitScanForward64(&result, num);
    return static_cast<usize>(result);

#elif defined(ALT_PLATFORM_X86)

    unsigned long result0;
    const u8 isNonZero0 = _BitScanForward(&result0, static_cast<u32>(num));

    unsigned long result1;
    ALT_MAYBE_UNUSED const u8 isNonZero1 = _BitScanForward(&result1, static_cast<u32>(num >> 32));

    result0 = isNonZero0 == 0 ? 32 : result0;
    result1 = isNonZero0 == 0 ? result1 : 0;

    return static_cast<usize>(result1 + result0);

#else
    #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return static_cast<usize>(__builtin_ctzll(num));

#else
    #error provide implementation for specific compiler/platform
#endif
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsRightUnsafe(const T x) noexcept {

    return Count0BitsRightUnsafe(static_cast<u32>(x) | static_cast<u32>(1 << Desc<T>::Bits()));
}

ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsRight(const u32 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

    unsigned long result;
    ALT_MAYBE_UNUSED const u8 isNonZero = _BitScanForward(&result, num);
    return num != 0 ? static_cast<usize>(result) : 32;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return num != 0
        ? static_cast<usize>(__builtin_ctzl(num))
        : 32;

#else
    #error provide implementation for specific compiler/platform
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsRight(const u64 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

    unsigned long result;
    ALT_MAYBE_UNUSED const u8 isNonZero = _BitScanForward64(&result, num);
    return num != 0 ? static_cast<usize>(result) : 64;

#elif defined(ALT_PLATFORM_X86)

    unsigned long result0;
    const u8 isNonZero0 = _BitScanForward(&result0, static_cast<u32>(num));

    unsigned long result1;
    ALT_MAYBE_UNUSED const u8 isNonZero1 = _BitScanForward(&result1, static_cast<u32>(num >> 32));

    result0 = isNonZero0 == 0 ? 32 : result0;
    result1 = isNonZero0 == 0 ? result1 : 0;

    const usize result = static_cast<usize>(result1 + result0);

    return num != 0 ? result : 64;

#else
    #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return num != 0
        ? static_cast<usize>(__builtin_ctzll(num))
        : 64;

#else
    #error provide implementation for specific compiler/platform
#endif
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsRight(const T x) noexcept {

    return Count0BitsRight(static_cast<u32>(x) | static_cast<u32>(1 << Desc<T>::Bits()));
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsLeftConstexpr(u32 x) noexcept {

    usize zeroes = 28;

    if ((x >> 16) != 0) {
        zeroes -= 16;
        x >>= 16;
    }

    if ((x >> 8) != 0) {
        zeroes -= 8;
        x >>= 8;
    }

    if ((x >> 4) != 0) {
        zeroes -= 4;
        x >>= 4;
    }

    return "\4\3\2\2\1\1\1\1\0\0\0\0\0\0\0"[x] + zeroes;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsLeftConstexpr(u64 x) noexcept {

    usize zeroes = 60;

    if (x >> 32) {
        zeroes -= 32;
        x >>= 32;
    }

    if (x >> 16) {
        zeroes -= 16;
        x >>= 16;
    }

    if (x >> 8) {
        zeroes -= 8;
        x >>= 8;
    }

    if (x >> 4) {
        zeroes -= 4;
        x >>= 4;
    }

    return "\4\3\2\2\1\1\1\1\0\0\0\0\0\0\0"[x] + zeroes;
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsLeftConstexpr(const T x) noexcept {

    const usize correction = Desc<u32>::Bits() - Desc<T>::Bits();
    return Count0BitsLeftConstexpr(static_cast<u32>(x)) - correction;
}

ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsLeftUnsafe(const u32 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

    unsigned long result;
    ALT_MAYBE_UNUSED const u8 isNonZero = _BitScanReverse(&result, num);
    return 31 - static_cast<usize>(result);

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return static_cast<usize>(__builtin_clzl(num));

#else
    #error provide implementation for specific compiler/platform
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsLeftUnsafe(const u64 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

    unsigned long result;
    ALT_MAYBE_UNUSED const u8 isNonZero = _BitScanReverse64(&result, num);
    return 63 - static_cast<usize>(result);

#elif defined(ALT_PLATFORM_X86)

    unsigned long result0;
    ALT_MAYBE_UNUSED const u8 isNonZero0 = _BitScanReverse(&result0, static_cast<u32>(num));
    result0 = 63 - result0;

    unsigned long result1;
    const u8 isNonZero1 = _BitScanReverse(&result1, static_cast<u32>(num >> 32));
    result1 = 31 - result1;

    const usize result = isNonZero1 == 0 ? static_cast<usize>(result0) : static_cast<usize>(result1);

    return result;

#else
    #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return static_cast<usize>(__builtin_clzll(num));

#else
    #error provide implementation for specific compiler/platform
#endif
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsLeftUnsafe(const T x) noexcept {

    constexpr static usize correction = Desc<u32>::Bits() - Desc<T>::Bits();
    return Count0BitsLeftUnsafe(static_cast<u32>(x)) - correction;
}

ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsLeft(const u32 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

    unsigned long result;
    ALT_MAYBE_UNUSED const u8 isNonZero = _BitScanReverse(&result, num);
    return num != 0 ? 31 - static_cast<usize>(result) : 32;

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return num != 0
        ? static_cast<usize>(__builtin_clzl(num))
        : 32;

#else
    #error provide implementation for specific compiler/platform
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsLeft(const u64 num) noexcept {

#if defined(ALT_COMPILER_MSVC)

#if defined(ALT_PLATFORM_X86_64)

    unsigned long result;
    ALT_MAYBE_UNUSED const u8 isNonZero = _BitScanReverse64(&result, num);
    return num != 0 ? 63 - static_cast<usize>(result) : 64;

#elif defined(ALT_PLATFORM_X86)

    unsigned long result0;
    ALT_MAYBE_UNUSED const u8 isNonZero0 = _BitScanReverse(&result0, static_cast<u32>(num));
    result0 = 63 - result0;

    unsigned long result1;
    const u8 isNonZero1 = _BitScanReverse(&result1, static_cast<u32>(num >> 32));
    result1 = 31 - result1;

    const usize result = isNonZero1 == 0 ? static_cast<usize>(result0) : static_cast<usize>(result1);

    return num != 0 ? result : 64;

#else
    #error provide implementation for specific platform
#endif

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return num != 0
        ? static_cast<usize>(__builtin_clzll(num))
        : 64;

#else
    #error provide implementation for specific compiler/platform
#endif
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE usize Count0BitsLeft(const T x) noexcept {

    constexpr static usize correction = Desc<u32>::Bits() - Desc<T>::Bits();
    return Count0BitsLeft(static_cast<u32>(x)) - correction;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPowerOf2(const usize num) noexcept {
    return num != 0 && (num & (num - 1)) == 0;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPowerOf2PreserveZero(const usize num) noexcept {
    return (num & (num - 1)) == 0;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr u32 RoundUpToPowerOf2Constexpr(u32 num) noexcept {

    u32 log2 = 31 - static_cast<u32>(Count0BitsLeftConstexpr((num - 1) | 1));
    log2 = (log2 + 1) & 31;

    u32 result = static_cast<u32>(1) << log2;
    result = num > (static_cast<u32>(1) << 31) ? Desc<u32>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 RoundUpToPowerOf2Constexpr(u64 num) noexcept {

    u64 log2 = 63 - static_cast<u64>(Count0BitsLeftConstexpr((num - 1) | 1));
    log2 = (log2 + 1) & 63;

    u64 result = static_cast<u64>(1) << log2;
    result = num > (static_cast<u64>(1) << 63) ? Desc<u64>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE u32 RoundUpToPowerOf2(u32 num) noexcept {

    u32 log2 = 31 - static_cast<u32>(Count0BitsLeftUnsafe((num - 1) | 1));
    log2 = (log2 + 1) & 31;

    u32 result = static_cast<u32>(1) << log2;
    result = num > (static_cast<u32>(1) << 31) ? Desc<u32>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE u32 RoundUpToPowerOf2PreserveZero(u32 num) noexcept {

    u32 log2 = 31 - static_cast<u32>(Count0BitsLeftUnsafe((num - 1) | 1));
    log2 = (log2 + 1) & 31;

    u32 result = static_cast<u32>(1) << log2;
    result -= log2 == 0 ? 1 : 0;

    result = num > (static_cast<u32>(1) << 31) ? Desc<u32>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE u64 RoundUpToPowerOf2(u64 num) noexcept {

    u64 log2 = 63 - static_cast<u64>(Count0BitsLeftUnsafe((num - 1) | 1));
    log2 = (log2 + 1) & 63;

    u64 result = static_cast<u64>(1) << log2;
    result = num > (static_cast<u64>(1) << 63) ? Desc<u64>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE u64 RoundUpToPowerOf2PreserveZero(u64 num) noexcept {

    u64 log2 = 63 - static_cast<u64>(Count0BitsLeftUnsafe((num - 1) | 1));
    log2 = (log2 + 1) & 63;

    u64 result = static_cast<u64>(1) << log2;
    result -= log2 == 0 ? 1 : 0;

    result = num > (static_cast<u64>(1) << 63) ? Desc<u64>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE u32 RoundDownToPowerOf2(const u32 num) noexcept {

    u32 result = static_cast<u32>(1) << (31 - Count0BitsLeftUnsafe(num | 1));
    result -= num == 0 ? 1 : 0;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE u64 RoundDownToPowerOf2(const u64 num) noexcept {

    u64 result = static_cast<u64>(1) << (63 - Count0BitsLeftUnsafe(num | 1));
    result -= num == 0 ? 1 : 0;

    return result;
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Log2Constexpr(const T num) noexcept {

    return static_cast<T>(Desc<T>::Bits() - 1 - Count0BitsLeftConstexpr(num));
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T Log2(const T num) noexcept {

    return static_cast<T>(Desc<T>::Bits() - 1 - Count0BitsLeftUnsafe(num | 1));
}

// https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2025/01/07/include/digitcount.h
ALT_NODISCARD ALT_FORCE_INLINE usize DigitCount(const u32 num) noexcept {

    constexpr static u64 table[]{
        4294967296,  8589934582,  8589934582,  8589934582,  12884901788,
        12884901788, 12884901788, 17179868184, 17179868184, 17179868184,
        21474826480, 21474826480, 21474826480, 21474826480, 25769703776,
        25769703776, 25769703776, 30063771072, 30063771072, 30063771072,
        34349738368, 34349738368, 34349738368, 34349738368, 38554705664,
        38554705664, 38554705664, 41949672960, 41949672960, 41949672960,
        42949672960, 42949672960
    };

    const u64 result = (num + table[Log2(num)]) >> 32;
    return static_cast<usize>(result);
}

// https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2025/01/07/include/digitcount.h
ALT_NODISCARD ALT_FORCE_INLINE usize DigitCount(const u64 num) noexcept {

    constexpr static u64 digits[65] {
        19, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16,
        16, 15, 15, 15, 14, 14, 14, 13, 13, 13, 13, 12, 12,
        12, 11, 11, 11, 10, 10, 10, 10, 9,  9,  9,  8,  8,
        8,  7,  7,  7,  7,  6,  6,  6,  5,  5,  5,  4,  4,
        4,  4,  3,  3,  3,  2,  2,  2,  1,  1,  1,  1,  1
    };

    constexpr static u64 table[65]{
        9999999999999999999ULL,
        9999999999999999999ULL,
        9999999999999999999ULL,
        9999999999999999999ULL,
        999999999999999999ULL,
        999999999999999999ULL,
        999999999999999999ULL,
        99999999999999999ULL,
        99999999999999999ULL,
        99999999999999999ULL,
        9999999999999999ULL,
        9999999999999999ULL,
        9999999999999999ULL,
        9999999999999999ULL,
        999999999999999ULL,
        999999999999999ULL,
        999999999999999ULL,
        99999999999999ULL,
        99999999999999ULL,
        99999999999999ULL,
        9999999999999ULL,
        9999999999999ULL,
        9999999999999ULL,
        9999999999999ULL,
        999999999999ULL,
        999999999999ULL,
        999999999999ULL,
        99999999999ULL,
        99999999999ULL,
        99999999999ULL,
        9999999999ULL,
        9999999999ULL,
        9999999999ULL,
        9999999999ULL,
        999999999ULL,
        999999999ULL,
        999999999ULL,
        99999999ULL,
        99999999ULL,
        99999999ULL,
        9999999ULL,
        9999999ULL,
        9999999ULL,
        9999999ULL,
        999999ULL,
        999999ULL,
        999999ULL,
        99999ULL,
        99999ULL,
        99999ULL,
        9999ULL,
        9999ULL,
        9999ULL,
        9999ULL,
        999ULL,
        999ULL,
        999ULL,
        99ULL,
        99ULL,
        99ULL,
        9ULL,
        9ULL,
        9ULL,
        9ULL,
        0ULL
    };

    const u64 log = Count0BitsLeftUnsafe(num | 1);
    const u64 low = table[log];
    const u64 high = digits[log];
    return static_cast<usize>((num > low) + high);
}

template <typename T,
    std::enable_if_t<Desc<T>::IsUnsigned()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE usize DigitCount(const T num) noexcept {

    return DigitCount(static_cast<u32>(num));
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool IsOverflowCheckAdd(const T x, const U y_, T* pResult = nullptr) noexcept {

    const T y = static_cast<T>(y_);

    T result;
    const bool isOverflow = _NumDetails::IsOverflowCheckAddImpl<T>::IsOverflowCheckAdd(x, y, result);

    if (pResult) {
        *pResult = result;
    }

    return isOverflow;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_constructible_v<T, U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckAddConstexpr(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);
    return x > Desc<T>::Max() - y;
}

template <typename HeadT, typename ...TailT,
    std::enable_if_t<Desc<HeadT>::IsUnsigned() && std::conjunction_v<std::is_convertible<TailT, HeadT>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<HeadT> AddOverflowCheck(HeadT size, TailT... sizes) noexcept {

    HeadT total = size;
    const bool isOk = _NumDetails::AddOverflowCheckImpl{}.AddOverflowCheck(total, sizes...);

    return isOk ? MakeOptional(total) : Nullopt;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T AddWrap(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);

    T result;
    ALT_MAYBE_UNUSED const bool isOverflow = _NumDetails::IsOverflowCheckAddImpl<T>::IsOverflowCheckAddWrap(x, y, result);

    return result;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T AddWrap(const T x, const U y_, const T wrap) noexcept {

    const T y = static_cast<T>(y_);

    T result;
    ALT_MAYBE_UNUSED const bool isOverflow = _NumDetails::IsOverflowCheckAddImpl<T>::IsOverflowCheckAddWrap(x, y, result, wrap);

    return result;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool IsUnderflowCheckSub(const T x, const U y_, T* pResult = nullptr) noexcept {

    const T y = static_cast<T>(y_);

    T result;
    const bool isUnderflow = _NumDetails::IsUnderflowCheckSubImpl<T>::IsUnderflowCheckSub(x, y, result);

    if (pResult) {
        *pResult = result;
    }

    return isUnderflow;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_constructible_v<T, U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsUnderflowCheckSubConstexpr(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);
    return x < y;
}

template <typename HeadT, typename ...TailT,
    std::enable_if_t<Desc<HeadT>::IsUnsigned() && std::conjunction_v<std::is_convertible<TailT, HeadT>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<HeadT> SubUnderflowCheck(HeadT size, TailT... sizes) noexcept {

    HeadT total = size;
    const bool isOk = _NumDetails::SubUnderflowCheckImpl{}.SubUnderflowCheck(total, sizes...);

    return isOk ? MakeOptional(total) : Nullopt;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T SubWrap(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);

    T result;
    const bool isUnderflow = _NumDetails::IsUnderflowCheckSubImpl<T>::IsUnderflowCheckSub(x, y, result);

    return isUnderflow ? 0 : result;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool IsOverflowCheckMul(const T x, const U y_, T* pResult = nullptr) noexcept {

    const T y = static_cast<T>(y_);

    T result;
    const bool isOverflow = _NumDetails::IsOverflowCheckMulImpl<T>::IsOverflowCheckMul(x, y, result);

    if (pResult) {
        *pResult = result;
    }

    return isOverflow;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckMulConstexpr(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);
    return x > Desc<T>::Max() / y;
}

template <typename HeadT, typename ...TailT,
    std::enable_if_t<Desc<HeadT>::IsUnsigned() && std::conjunction_v<std::is_convertible<TailT, HeadT>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<HeadT> MulOverflowCheck(HeadT size, TailT... sizes) noexcept {

    HeadT total = size;
    const bool isOk = _NumDetails::MulOverflowCheckImpl{}.MulOverflowCheck(total, sizes...);

    return isOk ? MakeOptional(total) : Nullopt;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T MulWrap(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);

    T result;
    ALT_MAYBE_UNUSED const bool isOverflow = _NumDetails::IsOverflowCheckMulImpl<T>::IsOverflowCheckMulWrap(x, y, result);

    return result;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T MulWrap(const T x, const U y_, const T wrap) noexcept {

    const T y = static_cast<T>(y_);

    T result;
    ALT_MAYBE_UNUSED const bool isOverflow = _NumDetails::IsOverflowCheckMulImpl<T>::IsOverflowCheckMulWrap(x, y, result, wrap);

    return result;
}

template <typename S,
    std::enable_if_t<Desc<S>::IsSigned()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr typename Desc<S>::Unsigned ConvertSignedToUnsignedOrdered(const S signedx) noexcept {

    using U = typename Desc<S>::Unsigned;
    const U x = Memory::Transmute<S, U>(signedx);

    constexpr U BITS = static_cast<U>(Desc<U>::Bits());
    return x ^ (static_cast<U>(1) << (BITS - 1));
}

template <typename U,
    std::enable_if_t<Desc<U>::IsUnsigned()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr typename Desc<U>::Signed ConvertUnsignedToSignedOrdered(const U unsignedx) noexcept {

    constexpr U BITS = static_cast<U>(Desc<U>::Bits());
    const U result = unsignedx ^ (static_cast<U>(1) << (BITS - 1));

    using S = typename Desc<U>::Signed;
    return Memory::Transmute<U, S>(result);
}

template <typename FP,
    std::enable_if_t<Desc<FP>::IsFloat()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr typename Desc<FP>::Unsigned ConvertFloatToUnsignedOrdered(const FP floatx) noexcept {

    using S = typename Desc<FP>::Signed;
    using U = typename Desc<FP>::Unsigned;

    const U x = Memory::Transmute<FP, U>(floatx);

    constexpr U BITS = static_cast<U>(Desc<U>::Bits());
    return x ^ (-static_cast<S>(x >> (BITS - 1)) | (static_cast<U>(1) << (BITS - 1)));
}

template <typename FP,
    std::enable_if_t<Desc<FP>::IsFloat()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr typename Desc<FP>::Signed ConvertFloatToSignedOrdered(const FP floatx) noexcept {

    using S = typename Desc<FP>::Signed;

    const S x = Memory::Transmute<FP, S>(floatx);

    constexpr S BITS = static_cast<S>(Desc<S>::Bits());
    return x ^ -static_cast<S>(x >> (BITS - 1));
}

template <typename FP,
    std::enable_if_t<Desc<FP>::IsFloat()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool IsSignPositive(const FP floatx) noexcept {

    using U = typename Desc<FP>::Unsigned;

    const U x = Memory::Transmute<FP, U>(floatx);

    constexpr U BITS = static_cast<U>(Desc<U>::Bits());

    // Ignores 'nan' - OK
    return (x & (static_cast<U>(1) << (BITS - 1))) == 0;
}

template <typename T,
    std::enable_if_t<Desc<T>::IsSigned() && !Desc<T>::IsFloat()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsSignPositive(const T x) noexcept {
    return x >= 0;
}

template <typename FP,
    std::enable_if_t<Desc<FP>::IsFloat()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr FP Abs(const FP floatx) noexcept {

    using U = typename Desc<FP>::Unsigned;

    const U x = Memory::Transmute<FP, U>(floatx);

    constexpr U BITS = static_cast<U>(Desc<U>::Bits());

    // Don't care about 'nan'
    const U result = x & ~(static_cast<U>(1) << (BITS - 1));

    return Memory::Transmute<U, FP>(result);
}

template <typename T,
    std::enable_if_t<Desc<T>::IsSigned() && !Desc<T>::IsFloat()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Abs(const T x) noexcept {
    return x < 0 ? -x : x;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Mod(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);
    return x % y;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsSigned() && !Desc<T>::IsFloat() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Mod(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);

    T res = x % y;
    res += res != 0 && (x < 0) != (y < 0) ? y : 0;

    return res;
}

template <typename T, typename U,
    std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Max(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);
    return x > y ? x : y;
}

template <typename T, typename U, typename ...ArgsU,
    std::enable_if_t<std::conjunction_v<std::is_convertible<U, T>, std::is_convertible<ArgsU, T>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Max(const T x, const U y_, const ArgsU... args) noexcept {

    const T y = static_cast<T>(y_);
    return x > y ? Max(x, args...) : Max(y, args...);
}

template <typename T, typename U,
    std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Min(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);
    return x < y ? x : y;
}

template <typename T, typename U, typename ...ArgsU,
    std::enable_if_t<std::conjunction_v<std::is_convertible<U, T>, std::is_convertible<ArgsU, T>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Min(const T x, const U y_, const ArgsU... args) noexcept {

    const T y = static_cast<T>(y_);
    return x < y ? Min(x, args...) : Min(y, args...);
}

template <typename T0, typename T1, typename T2,
    std::enable_if_t<std::is_convertible_v<T1, T0> && std::is_convertible_v<T2, T0>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T0 Clamp(const T0 val, const T1 from, const T2 to) noexcept {
    return Min(Max(val, static_cast<T0>(from)), static_cast<T0>(to));
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T CeilDiv(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);
    return x / y + (x % y != 0 ? 1 : 0);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<T> RoundUpOverflowCheck(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x += x == 0 ? 1 : 0;
    x = CeilDiv(x, round);

    return MulOverflowCheck(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T RoundUpWrap(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x += x == 0 ? 1 : 0;
    x = CeilDiv(x, round);

    return MulWrap(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T RoundUpWrap(T x, const U round_, const T wrap) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x += x == 0 ? 1 : 0;
    x = CeilDiv(x, round);

    return MulWrap(x, round, wrap);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<T> RoundUpOverflowCheckPreserveZero(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = CeilDiv(x, round);

    return MulOverflowCheck(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T RoundUpWrapPreserveZero(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = CeilDiv(x, round);

    return MulWrap(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T RoundUpWrapPreserveZero(T x, const U round_, const T wrap) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = CeilDiv(x, round);

    return MulWrap(x, round, wrap);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T RoundDown(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = x / round;

    return MulWrap(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T RoundDown(T x, const U round_, const T wrap) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = x / round;

    return MulWrap(x, round, wrap);
}

template <typename S,
    std::enable_if_t<Desc<S>::IsSigned()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE usize DigitCount(const S num) noexcept {

    using U = typename Desc<S>::Unsigned;

    return DigitCount(static_cast<U>(Abs(num)));
}

} // namespace Num
} // namespace Alt

#endif // __ALT_STD_NUM__