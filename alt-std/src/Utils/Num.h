#pragma once

#ifndef __ALT_STD_NUM__
#define __ALT_STD_NUM__

#include "../Utils/Impl/NumPreInclude.h"
#include "../Utils/Impl/u128.h"
#include "../Utils/Impl/s128.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Utils/Optional.h"

namespace Alt {
namespace _NumDetails {

constexpr static u64 DIGIT_COUNT_LOG_U32[32]{
    4294967296,  8589934582,  8589934582,  8589934582,  12884901788,
    12884901788, 12884901788, 17179868184, 17179868184, 17179868184,
    21474826480, 21474826480, 21474826480, 21474826480, 25769703776,
    25769703776, 25769703776, 30063771072, 30063771072, 30063771072,
    34349738368, 34349738368, 34349738368, 34349738368, 38554705664,
    38554705664, 38554705664, 41949672960, 41949672960, 41949672960,
    42949672960, 42949672960
};

constexpr static usize DIGIT_COUNT_DIGIT_COUNT_POW2[64 + 64] {

    // u64
    19, 19, 19, 19, 18, 18, 18, 17, 17, 17, 16, 16, 16,
    16, 15, 15, 15, 14, 14, 14, 13, 13, 13, 13, 12, 12,
    12, 11, 11, 11, 10, 10, 10, 10, 9,  9,  9,  8,  8,
    8,  7,  7,  7,  7,  6,  6,  6,  5,  5,  5,  4,  4,
    4,  4,  3,  3,  3,  2,  2,  2,  1,  1,  1,  1,

    // u128
    19, 18, 18, 18, 18, 17, 17, 17, 16, 16, 16, 15, 15,
    15, 15, 14, 14, 14, 13, 13, 13, 12, 12, 12, 12, 11,
    11, 11, 10, 10, 10, 9,  9,  9,  8,  8,  8,  8,  7,
    7,  7,  6,  6,  6,  5,  5,  5,  5,  4,  4,  4,  3,
    3,  3,  2,  2,  2,  2,  1,  1,  1,  0,  0,  0,
};

constexpr static u64 DIGIT_COUNT_LIMIT[64 + 64 * 2]{

    // u64
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

    // u128
    // low, high
    18446744073709551615ULL, 18446744073709551615ULL,
    687399551400673279ULL, 5421010862427522170ULL,
    687399551400673279ULL, 5421010862427522170ULL,
    687399551400673279ULL, 5421010862427522170ULL,
    687399551400673279ULL, 5421010862427522170ULL,
    68739955140067327ULL, 542101086242752217ULL,
    68739955140067327ULL, 542101086242752217ULL,
    68739955140067327ULL, 542101086242752217ULL,
    12919594847110692863ULL, 54210108624275221ULL,
    12919594847110692863ULL, 54210108624275221ULL,
    12919594847110692863ULL, 54210108624275221ULL,
    3136633892082024447ULL, 5421010862427522ULL,
    3136633892082024447ULL, 5421010862427522ULL,
    3136633892082024447ULL, 5421010862427522ULL,
    3136633892082024447ULL, 5421010862427522ULL,
    4003012203950112767ULL, 542101086242752ULL,
    4003012203950112767ULL, 542101086242752ULL,
    4003012203950112767ULL, 542101086242752ULL,
    4089650035136921599ULL, 54210108624275ULL,
    4089650035136921599ULL, 54210108624275ULL,
    4089650035136921599ULL, 54210108624275ULL,
    9632337040368467967ULL, 5421010862427ULL,
    9632337040368467967ULL, 5421010862427ULL,
    9632337040368467967ULL, 5421010862427ULL,
    9632337040368467967ULL, 5421010862427ULL,
    13875954555633532927ULL, 542101086242ULL,
    13875954555633532927ULL, 542101086242ULL,
    13875954555633532927ULL, 542101086242ULL,
    5076944270305263615ULL, 54210108624ULL,
    5076944270305263615ULL, 54210108624ULL,
    5076944270305263615ULL, 54210108624ULL,
    7886392056514347007ULL, 5421010862ULL,
    7886392056514347007ULL, 5421010862ULL,
    7886392056514347007ULL, 5421010862ULL,
    4477988020393345023ULL, 542101086ULL,
    4477988020393345023ULL, 542101086ULL,
    4477988020393345023ULL, 542101086ULL,
    4477988020393345023ULL, 542101086ULL,
    11515845246265065471ULL, 54210108ULL,
    11515845246265065471ULL, 54210108ULL,
    11515845246265065471ULL, 54210108ULL,
    15908979783594147839ULL, 5421010ULL,
    15908979783594147839ULL, 5421010ULL,
    15908979783594147839ULL, 5421010ULL,
    1590897978359414783ULL, 542101ULL,
    1590897978359414783ULL, 542101ULL,
    1590897978359414783ULL, 542101ULL,
    1590897978359414783ULL, 542101ULL,
    2003764205206896639ULL, 54210ULL,
    2003764205206896639ULL, 54210ULL,
    2003764205206896639ULL, 54210ULL,
    200376420520689663ULL, 5421ULL,
    200376420520689663ULL, 5421ULL,
    200376420520689663ULL, 5421ULL,
    1864712049423024127ULL, 542ULL,
    1864712049423024127ULL, 542ULL,
    1864712049423024127ULL, 542ULL,
    1864712049423024127ULL, 542ULL,
    3875820019684212735ULL, 54ULL,
    3875820019684212735ULL, 54ULL,
    3875820019684212735ULL, 54ULL,
    7766279631452241919ULL, 5ULL,
    7766279631452241919ULL, 5ULL,
    7766279631452241919ULL, 5ULL,
};

} // namespace _NumDetails

namespace Num {

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) < sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Popcount(const T x_) noexcept {

    using U = typename Desc<T>::Unsigned;

    const U x = Memory::Transmute<T, U>(x_);
    return Popcount(static_cast<u32>(x));
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) > sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Popcount(const T x) noexcept {
    return x.Popcount();
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsRightUnsafe(const T x_) noexcept {

    using U = typename Desc<T>::Unsigned;

    const U x = Memory::Transmute<T, U>(x_);
    return Count0BitsRightUnsafe(static_cast<u32>(x) | static_cast<u32>(1 << Desc<U>::Bits()));
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) > sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsRightUnsafe(const T x) noexcept {
    return x.Count0BitsRightUnsafe();
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsRight(const T x_) noexcept {

    using U = typename Desc<T>::Unsigned;

    const U x = Memory::Transmute<T, U>(x_);

    return Count0BitsRight(static_cast<u32>(x) | static_cast<u32>(1 << Desc<U>::Bits()));
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) > sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsRight(const T x) noexcept {
    return x.Count0BitsRight();
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsLeftUnsafe(const T x_) noexcept {

    using U = typename Desc<T>::Unsigned;

    const U x = Memory::Transmute<T, U>(x_);

    constexpr usize correction = Desc<u32>::Bits() - Desc<U>::Bits();
    return Count0BitsLeftUnsafe(static_cast<u32>(x)) - correction;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) > sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsLeftUnsafe(const T x) noexcept {
    return x.Count0BitsLeftUnsafe();
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) <= sizeof(u32))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsLeft(const T x_) noexcept {

    using U = typename Desc<T>::Unsigned;

    const U x = Memory::Transmute<T, U>(x_);

    constexpr usize correction = Desc<u32>::Bits() - Desc<U>::Bits();
    return Count0BitsLeft(static_cast<u32>(x)) - correction;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) > sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize Count0BitsLeft(const T x) noexcept {
    return x.Count0BitsLeft();
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && Desc<T>::IsUnsigned() && (sizeof(T) <= sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPowerOf2(const T num) noexcept {
    return num != 0 && (num & (num - 1)) == 0;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && Desc<T>::IsUnsigned() && (sizeof(T) > sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPowerOf2(const T num) noexcept {
    return num.IsPowerOf2();
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && Desc<T>::IsUnsigned() && (sizeof(T) <= sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPowerOf2PreserveZero(const T num) noexcept {
    return (num & (num - 1)) == 0;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && Desc<T>::IsUnsigned() && (sizeof(T) > sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPowerOf2PreserveZero(const T num) noexcept {
    return num.IsPowerOf2PreserveZero();
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr u32 RoundUpToPowerOf2(u32 num) noexcept {

    u32 log2 = 31 - static_cast<u32>(Count0BitsLeftUnsafe((num - 1) | 1));
    log2 = (log2 + 1) & 31;

    u32 result = static_cast<u32>(1) << log2;
    result = num > (static_cast<u32>(1) << 31) ? Desc<u32>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr u32 RoundUpToPowerOf2PreserveZero(u32 num) noexcept {

    u32 log2 = 31 - static_cast<u32>(Count0BitsLeftUnsafe((num - 1) | 1));
    log2 = (log2 + 1) & 31;

    u32 result = static_cast<u32>(1) << log2;
    result -= log2 == 0 ? 1 : 0;

    result = num > (static_cast<u32>(1) << 31) ? Desc<u32>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 RoundUpToPowerOf2(u64 num) noexcept {

    u64 log2 = 63 - static_cast<u64>(Count0BitsLeftUnsafe((num - 1) | 1));
    log2 = (log2 + 1) & 63;

    u64 result = static_cast<u64>(1) << log2;
    result = num > (static_cast<u64>(1) << 63) ? Desc<u64>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 RoundUpToPowerOf2PreserveZero(u64 num) noexcept {

    u64 log2 = 63 - static_cast<u64>(Count0BitsLeftUnsafe((num - 1) | 1));
    log2 = (log2 + 1) & 63;

    u64 result = static_cast<u64>(1) << log2;
    result -= log2 == 0 ? 1 : 0;

    result = num > (static_cast<u64>(1) << 63) ? Desc<u64>::Max() : result;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr u32 RoundDownToPowerOf2(const u32 num) noexcept {

    u32 result = static_cast<u32>(1) << (31 - Count0BitsLeftUnsafe(num | 1));
    result -= num == 0 ? 1 : 0;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 RoundDownToPowerOf2(const u64 num) noexcept {

    u64 result = static_cast<u64>(1) << (63 - Count0BitsLeftUnsafe(num | 1));
    result -= num == 0 ? 1 : 0;

    return result;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Log2(const T num) noexcept {

    return static_cast<T>(Desc<T>::Bits() - 1 - Count0BitsLeftUnsafe(num | 1));
}

// https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2025/01/07/include/digitcount.h
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize DigitCount(const u32 num) noexcept {

    const u64 result = (num + _NumDetails::DIGIT_COUNT_LOG_U32[Log2(num)]) >> 32;

    return static_cast<usize>(result);
}

// https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/2025/01/07/include/digitcount.h
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize DigitCount(const u64 num) noexcept {

    const usize log = Count0BitsLeftUnsafe(num | 1);

    const u64 limit = _NumDetails::DIGIT_COUNT_LIMIT[log];
    const usize digitCountPow2 = _NumDetails::DIGIT_COUNT_DIGIT_COUNT_POW2[log];

    return digitCountPow2 + (num > limit ? 1 : 0);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize DigitCount(const u128 num) noexcept {

    const bool isWide = num.GetHigh() != 0;
    const u64 hilo = isWide ? num.GetHigh() : num.GetLow();

    const usize log = Count0BitsLeftUnsafe(hilo | 1);
    const usize logLimits = log << (isWide ? 1 : 0);

    const usize offset = isWide ? 64 : 0;

    const u64* pLimits = _NumDetails::DIGIT_COUNT_LIMIT + logLimits + offset;
    const usize digitCountPow2 = _NumDetails::DIGIT_COUNT_DIGIT_COUNT_POW2[log + offset];

    usize result = digitCountPow2 + (num > u128::FromHighLow(isWide ? *(pLimits + 1) : 0, *pLimits) ? 1 : 0);
    result += isWide ? Desc<u64>::DigitCount() : 0;

    return result;
}

template <typename T,
    std::enable_if_t<Desc<T>::IsUnsigned()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize DigitCount(const T num) noexcept {

    return DigitCount(static_cast<u32>(num));
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckAdd(const T x, const U y_, T* pResult = nullptr) noexcept {

    const T y = static_cast<T>(y_);

    T result = 0;
    const bool isOverflow = _NumDetails::IsOverflowCheckAddImpl<T>::IsOverflowCheckAdd(x, y, result);

    if (pResult) {
        *pResult = result;
    }

    return isOverflow;
}

template <typename HeadT, typename ...TailT,
    std::enable_if_t<Desc<HeadT>::IsUnsigned() && std::conjunction_v<std::is_convertible<TailT, HeadT>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<HeadT> AddOverflowCheck(HeadT size, TailT... sizes) noexcept {

    HeadT total = size;
    const bool isOk = _NumDetails::AddOverflowCheckImpl{}.AddOverflowCheck(total, sizes...);

    return isOk ? MakeOptional(total) : Nullopt;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T AddWrap(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);

    T result = 0;
    ALT_MAYBE_UNUSED const bool isOverflow = _NumDetails::IsOverflowCheckAddImpl<T>::IsOverflowCheckAddWrap(x, y, result);

    return result;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T AddWrap(const T x, const U y_, const T wrap) noexcept {

    const T y = static_cast<T>(y_);

    T result = 0;
    ALT_MAYBE_UNUSED const bool isOverflow = _NumDetails::IsOverflowCheckAddImpl<T>::IsOverflowCheckAddWrap(x, y, result, wrap);

    return result;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsUnderflowCheckSub(const T x, const U y_, T* pResult = nullptr) noexcept {

    const T y = static_cast<T>(y_);

    T result = 0;
    const bool isUnderflow = _NumDetails::IsUnderflowCheckSubImpl<T>::IsUnderflowCheckSub(x, y, result);

    if (pResult) {
        *pResult = result;
    }

    return isUnderflow;
}

template <typename HeadT, typename ...TailT,
    std::enable_if_t<Desc<HeadT>::IsUnsigned() && std::conjunction_v<std::is_convertible<TailT, HeadT>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<HeadT> SubUnderflowCheck(HeadT size, TailT... sizes) noexcept {

    HeadT total = size;
    const bool isOk = _NumDetails::SubUnderflowCheckImpl{}.SubUnderflowCheck(total, sizes...);

    return isOk ? MakeOptional(total) : Nullopt;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T SubWrap(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);

    T result = 0;
    const bool isUnderflow = _NumDetails::IsUnderflowCheckSubImpl<T>::IsUnderflowCheckSub(x, y, result);

    return isUnderflow ? 0 : result;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverflowCheckMul(const T x, const U y_, T* pResult = nullptr) noexcept {

    const T y = static_cast<T>(y_);

    T result = 0;
    const bool isOverflow = _NumDetails::IsOverflowCheckMulImpl<T>::IsOverflowCheckMul(x, y, result);

    if (pResult) {
        *pResult = result;
    }

    return isOverflow;
}

template <typename HeadT, typename ...TailT,
    std::enable_if_t<Desc<HeadT>::IsUnsigned() && std::conjunction_v<std::is_convertible<TailT, HeadT>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<HeadT> MulOverflowCheck(HeadT size, TailT... sizes) noexcept {

    HeadT total = size;
    const bool isOk = _NumDetails::MulOverflowCheckImpl{}.MulOverflowCheck(total, sizes...);

    return isOk ? MakeOptional(total) : Nullopt;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T MulWrap(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);

    T result = 0;
    ALT_MAYBE_UNUSED const bool isOverflow = _NumDetails::IsOverflowCheckMulImpl<T>::IsOverflowCheckMulWrap(x, y, result);

    return result;
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T MulWrap(const T x, const U y_, const T wrap) noexcept {

    const T y = static_cast<T>(y_);

    T result = 0;
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
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsSignPositive(const FP floatx) noexcept {

    using U = typename Desc<FP>::Unsigned;

    const U x = Memory::Transmute<FP, U>(floatx);

    constexpr U BITS = static_cast<U>(Desc<U>::Bits());

    // Ignores 'nan' - OK
    return (x & (static_cast<U>(1) << (BITS - 1))) == 0;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) > sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsSignPositive(const T x) noexcept {
    return x.IsSignPositive();
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
    std::enable_if_t<TypeTraits::is_number_v<T> && (sizeof(T) > sizeof(u64))>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool Abs(const T x) noexcept {
    return x.Abs();
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
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<T> RoundUpOverflowCheck(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x += x == 0 ? 1 : 0;
    x = CeilDiv(x, round);

    return MulOverflowCheck(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T RoundUpWrap(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x += x == 0 ? 1 : 0;
    x = CeilDiv(x, round);

    return MulWrap(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T RoundUpWrap(T x, const U round_, const T wrap) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x += x == 0 ? 1 : 0;
    x = CeilDiv(x, round);

    return MulWrap(x, round, wrap);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<T> RoundUpOverflowCheckPreserveZero(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = CeilDiv(x, round);

    return MulOverflowCheck(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T RoundUpWrapPreserveZero(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = CeilDiv(x, round);

    return MulWrap(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T RoundUpWrapPreserveZero(T x, const U round_, const T wrap) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = CeilDiv(x, round);

    return MulWrap(x, round, wrap);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T RoundDown(T x, const U round_) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = x / round;

    return MulWrap(x, round);
}

template <typename T, typename U,
    std::enable_if_t<Desc<T>::IsUnsigned() && std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T RoundDown(T x, const U round_, const T wrap) noexcept {

    const T round = Max(static_cast<T>(round_), 1);
    x = x / round;

    return MulWrap(x, round, wrap);
}

template <typename S,
    std::enable_if_t<Desc<S>::IsSigned()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize DigitCount(const S num) noexcept {

    using U = typename Desc<S>::Unsigned;

    return DigitCount(IsSignPositive(num) ? Memory::Transmute<S, U>(num) : ~Memory::Transmute<S, U>(num) + 1);
}

} // namespace Num

using namespace Alt::Num;

} // namespace Alt

#endif // __ALT_STD_NUM__