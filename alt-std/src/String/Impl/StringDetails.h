#pragma once

#ifndef __ALT_STD_STRING_DETAILS__
#define __ALT_STD_STRING_DETAILS__

#include "StringDetailsPreInclude.h"

#include "../../Algorithms/Algorithms.h"

#include "../../Utils/Optional.h"

#include "../Impl/Utf/Utf8.h"
#include "../Impl/Utf/Utf16.h"
#include "../Impl/Utf/Utf32.h"

namespace Alt {
namespace _StringDetails {

template <usize>
class StringFormatWriter;

template <typename T>
constexpr static bool is_character_v = TypeTraits::is_character_v<T>;

template <typename T>
constexpr static bool is_compatible_with_character_utf8_v = TypeTraits::is_compatible_with_character_utf8_v<T>;

template <typename T>
constexpr static bool is_compatible_with_character_utf16_v = TypeTraits::is_compatible_with_character_utf16_v<T>;

template <typename T>
constexpr static bool is_compatible_with_character_utf32_v = TypeTraits::is_compatible_with_character_utf32_v<T>;

template <typename T>
constexpr static bool is_compatible_with_character_v = TypeTraits::is_compatible_with_character_v<T>;

/**
 * @brief Helpers to handle ascii
 * 
 * https://cppreference.com/cpp/language/ascii
 * 
 * Printable ASCII [32..126]:
 *   ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?
 * @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \ ] ^ _
 * ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~
 * 
 */
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsAscii(const chara utf32) noexcept {

    return utf32 <= 127;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsAsciiWhitespace(const chara utf32) noexcept {

    return utf32 == ' ' || (utf32 >= 9 && utf32 <= 13); // \t, \n, \v, \f, \r
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsAsciiPrintable(const chara utf32) noexcept {

    return utf32 >= 32 && utf32 <= 126;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsAsciiAlphabetLower(const chara utf32) noexcept {

    return utf32 >= 97 && utf32 <= 122; // [a..z]
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsAsciiAlphabetUpper(const chara utf32) noexcept {

    return utf32 >= 65 && utf32 <= 90; // [A..Z]
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsAsciiAlphabet(const chara utf32) noexcept {

    return IsAsciiAlphabetLower(utf32) || IsAsciiAlphabetUpper(utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsAsciiDigit(const chara utf32) noexcept {

    return utf32 >= 48 && utf32 <= 57; // [0..9]
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsAsciiHex(const chara utf32) noexcept {

    return IsAsciiDigit(utf32) || (utf32 >= 65 && utf32 <= 70) || (utf32 >= 97 && utf32 <= 102); // [0..9] [ABCDEF] [abcdef]
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr chara AsciiToLower(const chara utf32) noexcept {

    return IsAsciiAlphabetUpper(utf32) ? utf32 + 32 : utf32;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr chara AsciiToUpper(const chara utf32) noexcept {

    return IsAsciiAlphabetLower(utf32) ? utf32 - 32 : utf32;
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize CalculateCStrLenConstexpr(const T* str) noexcept {
    ALT_ASSERT(str);

    const T* pStr = str;
    while (*pStr != 0) {

        pStr += 1;
    }

    return static_cast<usize>(pStr - str);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize CalculateCStrLen(const char* str) noexcept {
    ALT_ASSERT(str);

    ALT_IF_IS_CONSTANT_EVALUATED {

        return CalculateCStrLenConstexpr(str);
    }

    return std::strlen(str);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize CalculateCStrLen(const char8_t* str) noexcept {
    ALT_ASSERT(str);

    static_assert(sizeof(char8_t) == sizeof(char), "");

    ALT_IF_IS_CONSTANT_EVALUATED {

        return CalculateCStrLenConstexpr(str);
    }

    return std::strlen(reinterpret_cast<const char*>(str));
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize CalculateCStrLen(const char16_t* str) noexcept {
    ALT_ASSERT(str);

    ALT_IF_IS_CONSTANT_EVALUATED {

        return CalculateCStrLenConstexpr(str);
    }

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return std::wcslen(reinterpret_cast<const wchar_t*>(str));

#else

    return CalculateCStrLenConstexpr(str);

#endif // ALT_OS_WINDOWS
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize CalculateCStrLen(const chara* str) noexcept {
    ALT_ASSERT(str);

    ALT_IF_IS_CONSTANT_EVALUATED {

        return CalculateCStrLenConstexpr(str);
    }

#ifdef ALT_OS_WINDOWS

    return CalculateCStrLenConstexpr(str);

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return std::wcslen(reinterpret_cast<const wchar_t*>(str));

#endif // ALT_OS_WINDOWS
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize CalculateCStrLen(const wchar_t* str) noexcept {
    ALT_ASSERT(str);

    ALT_IF_IS_CONSTANT_EVALUATED {

        return CalculateCStrLenConstexpr(str);
    }

    return std::wcslen(str);
}

template <typename T,
    std::enable_if<is_character_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize CalculateSizeInBytes(const T* str) noexcept {

    return CalculateCStrLen(str) * sizeof(T);
}

template <typename T, typename U,
    std::enable_if_t<is_compatible_with_character_v<T> && is_compatible_with_character_v<U> && TypeTraits::is_character_compatible_have_same_encoding_v<T, U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool CompareEQ(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    ALT_IF_IS_CONSTANT_EVALUATED {

        const usize size0 = static_cast<usize>(pEnd0 - pBegin0);
        const usize size1 = static_cast<usize>(pEnd1 - pBegin1);

        if (size0 != size1) {
            return false;
        }

        const T* pVal0 = pBegin0;
        const U* pVal1 = pBegin1;

        const T* pVal0End = pEnd0;

        bool isEq = true;

        while (isEq && pVal0 != pVal0End) {

            isEq = *pVal0 == static_cast<U>(*pVal1);

            pVal0 += 1;
            pVal1 += 1;
        }

        return isEq;
    }

    return Algorithms::CompareEQ(pBegin0, pEnd0, reinterpret_cast<const T*>(pBegin1), reinterpret_cast<const T*>(pEnd1));
}

template <typename T, typename U,
    std::enable_if_t<is_compatible_with_character_v<T> && is_compatible_with_character_v<U> && TypeTraits::is_character_compatible_have_same_encoding_v<T, U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr ssize Compare(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    ALT_IF_IS_CONSTANT_EVALUATED {

        const usize size0 = static_cast<usize>(pEnd0 - pBegin0);
        const usize size1 = static_cast<usize>(pEnd1 - pBegin1);

        if (size0 < size1) {
            return -1;
        }

        if (size0 > size1) {
            return 1;
        }

        const T* pVal0 = pBegin0;
        const U* pVal1 = pBegin1;

        const T* pVal0End = pEnd0;

        bool isEq = true;

        while (isEq && pVal0 != pVal0End) {

            isEq = *pVal0 == static_cast<T>(*pVal1);

            pVal0 += 1;
            pVal1 += 1;
        }

        if (isEq) {
            return 0;
        }

        pVal0 -= 1;
        pVal1 -= 1;

        return *pVal0 < static_cast<T>(*pVal1) ? -1 : 1;
    }

    return Algorithms::Compare(pBegin0, pEnd0, reinterpret_cast<const T*>(pBegin1), reinterpret_cast<const T*>(pEnd1));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr const T* TrimLeft(const T* pBegin, const T* pEnd) noexcept {

    while (pBegin != pEnd && IsAsciiWhitespace(static_cast<chara>(*pBegin))) {

        pBegin += 1;
    }

    return pBegin;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr const T* TrimRight(const T* pBegin, const T* pEnd) noexcept {

    if (pBegin == pEnd) {
        return pEnd;
    }

    bool isWhitespace = true;
    do {

        pEnd -= 1;
        isWhitespace = IsAsciiWhitespace(static_cast<chara>(*pEnd));

    } while (pBegin != pEnd && isWhitespace);

    pEnd += isWhitespace ? 0 : 1;

    return pEnd;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirstUtf8(
    const T* pBegin, const T* pEnd, const T (&utf8)[4], const usize utf8Size
) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf8Size > 0);

    bool isOk = true;
    bool isEq = true;

    const T* pVal = pBegin;

    do {

        pVal = Algorithms::FindFirst(pVal, pEnd, utf8[0]);
        isOk = utf8Size <= static_cast<usize>(pEnd - pVal);

        isEq = isOk;
        if (isOk && 1 < utf8Size) isEq = isEq && *(pVal + 1) == utf8[1];
        if (isOk && 2 < utf8Size) isEq = isEq && *(pVal + 2) == utf8[2];
        if (isOk && 3 < utf8Size) isEq = isEq && *(pVal + 3) == utf8[3];

        pVal += 1;

    } while (isOk && !isEq);

    pVal -= 1;

    return isEq
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_32::Utf32ToUtf8IgnoreErrors(utf32, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return FindFirstUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    const T* pVal = Algorithms::FindFirst(pBegin, pEnd, static_cast<T>(utf8));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 (&utf8_)[4]) noexcept {

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    usize utf8Size = 1;
    utf8Size += utf8[1] != 0 ? 1 : 0;
    utf8Size += utf8[2] != 0 ? 1 : 0;
    utf8Size += utf8[3] != 0 ? 1 : 0;

    return FindFirstUtf8(pBegin, pEnd, utf8, utf8Size);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return FindFirstUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_[0]),
        static_cast<u16>(utf16_[1]),
    };

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return FindFirstUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return FindFirst(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindFirst(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLastUtf8(
    const T* pBegin, const T* pEnd, const T (&utf8)[4], const usize utf8Size
) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf8Size > 0);

    const T* pValEnd = utf8Size <= static_cast<usize>(pEnd - pBegin)
        ? pEnd - utf8Size + 1
        : pEnd;

    bool isEq = true;
    bool isOk = false;

    do {

        const T* pValEndOld = pValEnd;
        pValEnd = Algorithms::FindLast(pBegin, pValEndOld, utf8[0]);

        isOk = pValEnd != pValEndOld;

        isEq = isOk;
        if (isOk && 1 < utf8Size) isEq = isEq && *(pValEnd + 1) == utf8[1];
        if (isOk && 2 < utf8Size) isEq = isEq && *(pValEnd + 2) == utf8[2];
        if (isOk && 3 < utf8Size) isEq = isEq && *(pValEnd + 3) == utf8[3];

    } while (isOk && !isEq);

    return isEq
        ? MakeOptional(pValEnd - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_32::Utf32ToUtf8IgnoreErrors(utf32, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return FindLastUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    const T* pVal = Algorithms::FindLast(pBegin, pEnd, static_cast<T>(utf8));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 (&utf8_)[4]) noexcept {

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    usize utf8Size = 1;
    utf8Size += utf8[1] != 0 ? 1 : 0;
    utf8Size += utf8[2] != 0 ? 1 : 0;
    utf8Size += utf8[3] != 0 ? 1 : 0;

    return FindLastUtf8(pBegin, pEnd, utf8, utf8Size);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return FindLastUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_[0]),
        static_cast<u16>(utf16_[1]),
    };

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return FindLastUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return FindLast(pBegin, pEnd, static_cast<char16_t>(wchar));
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindLast(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirstUtf16(
    const T* pBegin, const T* pEnd, const T (&utf16)[2], const usize utf16Size
) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf16Size > 0);

    bool isOk = true;
    bool isEq = true;

    const T* pVal = pBegin;

    do {

        pVal = Algorithms::FindFirst(pVal, pEnd, utf16[0]);
        isOk = utf16Size <= static_cast<usize>(pEnd - pVal);

        isEq = isOk;
        if (isOk && 1 < utf16Size) isEq = isEq && *(pVal + 1) == utf16[1];

        pVal += 1;

    } while (isOk && !isEq);

    pVal -= 1;

    return isEq
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    u16 utf16_[2]{};
    const Utf::Result result = Utf::_32::Utf32ToUtf16IgnoreErrors(utf32, utf16_);

    T utf16[2]{};
    Memory::TransmuteInto<u16[2], T[2]>(utf16_, utf16);

    return FindFirstUtf16(pBegin, pEnd, utf16, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    const T* pVal = Algorithms::FindFirst(pBegin, pEnd, static_cast<T>(utf8));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {

    u16 utf16_[2]{};
    const Utf::Result result = Utf::_8::Utf8ToUtf16IgnoreErrors(utf8, utf16_);

    T utf16[2]{};
    Memory::TransmuteInto<u16[2], T[2]>(utf16_, utf16);

    return FindFirstUtf16(pBegin, pEnd, utf16, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {

    const T* pVal = Algorithms::FindFirst(pBegin, pEnd, static_cast<T>(utf16));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2]{};
    Memory::TransmuteInto<char16_t[2], T[2]>(utf16_, utf16);

    usize utf16Size = 1;
    utf16Size += utf16[1] != 0 ? 1 : 0;

    return FindFirstUtf16(pBegin, pEnd, utf16, utf16Size);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return FindFirst(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindFirst(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLastUtf16(
    const T* pBegin, const T* pEnd, const T (&utf16)[2], const usize utf16Size
) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf16Size > 0);

    const T* pValEnd = utf16Size <= static_cast<usize>(pEnd - pBegin)
        ? pEnd - utf16Size + 1
        : pEnd;

    bool isEq = true;
    bool isOk = false;

    do {

        const T* pValEndOld = pValEnd;
        pValEnd = Algorithms::FindLast(pBegin, pValEndOld, utf16[0]);

        isOk = pValEnd != pValEndOld;

        isEq = isOk;
        if (isOk && 1 < utf16Size) isEq = isEq && *(pValEnd + 1) == utf16[1];

    } while (isOk && !isEq);

    return isEq
        ? MakeOptional(pValEnd - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    u16 utf16_[2]{};
    const Utf::Result result = Utf::_32::Utf32ToUtf16IgnoreErrors(utf32, utf16_);

    T utf16[2]{};
    Memory::TransmuteInto<u16[2], T[2]>(utf16_, utf16);

    return FindLastUtf16(pBegin, pEnd, utf16, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    const T* pVal = Algorithms::FindLast(pBegin, pEnd, static_cast<T>(utf8));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {

    u16 utf16_[2]{};
    const Utf::Result result = Utf::_8::Utf8ToUtf16IgnoreErrors(utf8, utf16_);

    T utf16[2]{};
    Memory::TransmuteInto<u16[2], T[2]>(utf16_, utf16);

    return FindLastUtf16(pBegin, pEnd, utf16, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {

    const T* pVal = Algorithms::FindLast(pBegin, pEnd, static_cast<T>(utf16));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    T utf16[2]{};
    Memory::TransmuteInto<char16_t[2], T[2]>(utf16_, utf16);

    usize utf16Size = 1;
    utf16Size += utf16[1] != 0 ? 1 : 0;

    return FindLastUtf16(pBegin, pEnd, utf16, utf16Size);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return FindLast(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindLast(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirstUtf32(
    const T* pBegin, const T* pEnd, const T utf32
) noexcept {

    const T* pVal = Algorithms::FindFirst(pBegin, pEnd, static_cast<T>(utf32));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    return FindFirstUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    return FindFirstUtf32(pBegin, pEnd, static_cast<T>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {

    u32 utf32 = 0;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

    return FindFirstUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {

    return FindFirstUtf32(pBegin, pEnd, static_cast<T>(utf16));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2]{};
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    u32 utf32 = 0;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

    return FindFirstUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return FindFirst(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindFirst(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLastUtf32(
    const T* pBegin, const T* pEnd, const T utf32
) noexcept {

    const T* pVal = Algorithms::FindLast(pBegin, pEnd, static_cast<T>(utf32));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    return FindLastUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    return FindLastUtf32(pBegin, pEnd, static_cast<T>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {

    u32 utf32 = 0;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

    return FindLastUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {

    return FindLastUtf32(pBegin, pEnd, static_cast<T>(utf16));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2]{};
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    u32 utf32 = 0;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

    return FindLastUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return FindLast(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindLast(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T, typename U,
    std::enable_if_t<is_compatible_with_character_v<T> && is_compatible_with_character_v<U> && TypeTraits::is_character_compatible_have_same_encoding_v<T, U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const U* pBytesBegin, const U* pBytesEnd) noexcept {

    const usize size = static_cast<usize>(pEnd - pBegin);
    const usize bytesSize = static_cast<usize>(pBytesEnd - pBytesBegin);

    if (bytesSize == 0) {
        return 0;
    }

    if (bytesSize > size) {
        return Nullopt;
    }

    const U* pBytesBeginPlusOne = pBytesBegin + 1;
    const usize bytesSizeMinusOne = bytesSize - 1;

    const T* pVal = pBegin;
    const T* pEndAlignedToSizeCheck = pEnd - bytesSizeMinusOne;

    bool isEq = false;
    bool isOk = true;

    do {

        pVal = Algorithms::FindFirst(pVal, pEnd, static_cast<T>(*pBytesBegin));

        isOk = pVal < pEndAlignedToSizeCheck;
        pVal += 1;

        isEq = isOk;
        isEq &= CompareEQ(pVal, pVal + (isOk ? bytesSizeMinusOne : 0), pBytesBeginPlusOne, pBytesEnd);

    } while (isOk && !isEq);

    pVal -= 1;

    return isEq
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T, typename U,
    std::enable_if_t<is_compatible_with_character_v<T> && is_compatible_with_character_v<U> && TypeTraits::is_character_compatible_have_same_encoding_v<T, U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<usize> FindLast(const T* pBegin, const T* pEnd, const U* pBytesBegin, const U* pBytesEnd) noexcept {

    const usize size = static_cast<usize>(pEnd - pBegin);
    const usize bytesSize = static_cast<usize>(pBytesEnd - pBytesBegin);

    if (bytesSize == 0) {
        return 0;
    }

    if (bytesSize > size) {
        return Nullopt;
    }

    const U* pBytesBeginPlusOne = pBytesBegin + 1;
    const usize bytesSizeMinusOne = bytesSize - 1;

    const T* pValEnd = pEnd - bytesSizeMinusOne;

    bool isEq = false;
    bool isOk = true;

    do {

        const T* pValEndOld = pValEnd;
        pValEnd = Algorithms::FindLast(pBegin, pValEndOld, static_cast<T>(*pBytesBegin));

        isOk = pValEnd != pValEndOld;

        const T* pVal = pValEnd + 1;

        isEq = isOk;
        isEq &= CompareEQ(pVal, pVal + (isOk ? bytesSizeMinusOne : 0), pBytesBeginPlusOne, pBytesEnd);

    } while (isOk && !isEq);

    return isEq
        ? MakeOptional(pValEnd - pBegin)
        : Nullopt;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWithUtf8(const T* pBegin, const T* pEnd, const T (&utf8)[4], const usize utf8Size) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf8Size > 0);

    if (static_cast<usize>(pEnd - pBegin) < utf8Size) {
        return false;
    }

    bool isEq = *pBegin == utf8[0];
    if (1 < utf8Size) isEq = isEq && *(pBegin + 1) == utf8[1];
    if (2 < utf8Size) isEq = isEq && *(pBegin + 2) == utf8[2];
    if (3 < utf8Size) isEq = isEq && *(pBegin + 3) == utf8[3];

    return isEq;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_32::Utf32ToUtf8IgnoreErrors(utf32, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return StartWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const u8 utf8_) noexcept {

    const T utf8[4]{
        static_cast<T>(utf8_),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
    };

    return StartWithUtf8(pBegin, pEnd, utf8, 1);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const u8 (&utf8_)[4]) noexcept {

    usize utf8Size = 1;
    utf8Size += utf8_[1] != 0 ? 1 : 0;
    utf8Size += utf8_[2] != 0 ? 1 : 0;
    utf8Size += utf8_[3] != 0 ? 1 : 0;

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return StartWithUtf8(pBegin, pEnd, utf8, utf8Size);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return StartWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_[0]),
        static_cast<u16>(utf16_[1]),
    };

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return StartWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return StartWith(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return StartWith(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWithUtf16(const T* pBegin, const T* pEnd, const T (&utf16)[2], const usize utf16Size) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf16Size > 0);

    if (static_cast<usize>(pEnd - pBegin) < utf16Size) {
        return false;
    }

    bool isEq = *pBegin == utf16[0];
    if (1 < utf16Size) isEq = isEq && *(pBegin + 1) == utf16[1];

    return isEq;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    u16 utf16_[2]{};
    const Utf::Result result = Utf::_32::Utf32ToUtf16IgnoreErrors(utf32, utf16_);

    T utf16[2]{};
    Memory::TransmuteInto<u16[2], T[2]>(utf16_, utf16);

    return StartWithUtf16(pBegin, pEnd, utf16, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    const T utf16[2]{
        static_cast<T>(utf8),
        0
    };

    return StartWithUtf16(pBegin, pEnd, utf16, 1);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {

    u16 utf16_[2]{};
    const Utf::Result result = Utf::_8::Utf8ToUtf16IgnoreErrors(utf8, utf16_);

    T utf16[2]{};
    Memory::TransmuteInto<u16[2], T[2]>(utf16_, utf16);

    return StartWithUtf16(pBegin, pEnd, utf16, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char16_t utf16_) noexcept {

    const T utf16[2]{
        static_cast<T>(utf16_),
        0
    };

    return StartWithUtf16(pBegin, pEnd, utf16, 1);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    T utf16[2]{};
    Memory::TransmuteInto<char16_t[2], T[2]>(utf16_, utf16);

    usize utf16Size = 1;
    utf16Size += utf16[1] != 0 ? 1 : 0;

    return StartWithUtf16(pBegin, pEnd, utf16, utf16Size);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return StartWith(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return StartWith(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWithUtf32(const T* pBegin, const T* pEnd, const T utf32) noexcept {

    return pBegin == pEnd ? false : *pBegin == utf32;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    return StartWithUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    return StartWithUtf32(pBegin, pEnd, static_cast<T>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {

    u32 utf32 = 0;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

    return StartWithUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return StartWithUtf32(pBegin, pEnd, static_cast<T>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {

    return StartWithUtf32(pBegin, pEnd, static_cast<T>(utf16));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2]{};
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    u32 utf32 = 0;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

    return StartWithUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return StartWith(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return StartWith(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWithUtf8(const T* pBegin, const T* pEnd, const T (&utf8)[4], const usize utf8Size) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf8Size > 0);

    if (static_cast<usize>(pEnd - pBegin) < utf8Size) {
        return false;
    }

    const T* pVal = pEnd - utf8Size;

    bool isEq = *pVal == utf8[0];
    if (1 < utf8Size) isEq = isEq && *(pVal + 1) == utf8[1];
    if (2 < utf8Size) isEq = isEq && *(pVal + 2) == utf8[2];
    if (3 < utf8Size) isEq = isEq && *(pVal + 3) == utf8[3];

    return isEq;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_32::Utf32ToUtf8IgnoreErrors(utf32, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return EndWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const u8 utf8_) noexcept {

    const T utf8[4]{
        static_cast<T>(utf8_),
        0,
        0,
        0
    };

    return EndWithUtf8(pBegin, pEnd, utf8, 1);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const u8 (&utf8_)[4]) noexcept {

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    usize utf8Size = 1;
    utf8Size += utf8[1] != 0 ? 1 : 0;
    utf8Size += utf8[2] != 0 ? 1 : 0;
    utf8Size += utf8[3] != 0 ? 1 : 0;

    return EndWithUtf8(pBegin, pEnd, utf8, utf8Size);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return EndWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_[0]),
        static_cast<u16>(utf16_[1]),
    };

    u8 utf8_[4]{};
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8_);

    T utf8[4]{};
    Memory::TransmuteInto<u8[4], T[4]>(utf8_, utf8);

    return EndWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return EndWith(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return EndWith(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWithUtf16(const T* pBegin, const T* pEnd, const T (&utf16)[2], const usize utf16Size) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf16Size > 0);

    if (static_cast<usize>(pEnd - pBegin) < utf16Size) {
        return false;
    }

    const T* pVal = pEnd - utf16Size;

    bool isEq = *pVal == utf16[0];
    if (1 < utf16Size) isEq = isEq && *(pVal + 1) == utf16[1];

    return isEq;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    u16 utf16_[2]{};
    const Utf::Result result = Utf::_32::Utf32ToUtf16IgnoreErrors(utf32, utf16_);

    T utf16[2]{};
    Memory::TransmuteInto<u16[2], T[2]>(utf16_, utf16);

    return EndWithUtf16(pBegin, pEnd, utf16, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    const T utf16[2]{
        static_cast<T>(utf8),
        0
    };

    return EndWithUtf16(pBegin, pEnd, utf16, 1);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {

    u16 utf16_[2]{};
    const Utf::Result result = Utf::_8::Utf8ToUtf16IgnoreErrors(utf8, utf16_);

    T utf16[2]{};
    Memory::TransmuteInto<u16[2], T[2]>(utf16_, utf16);

    return EndWithUtf16(pBegin, pEnd, utf16, result.consumed);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char16_t utf16_) noexcept {

    const T utf16[2]{
        static_cast<T>(utf16_),
        0
    };

    return EndWithUtf16(pBegin, pEnd, utf16, 1);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    T utf16[2]{};
    Memory::TransmuteInto<char16_t[2], T[2]>(utf16_, utf16);

    usize utf16Size = 1;
    utf16Size += utf16[1] != 0 ? 1 : 0;

    return EndWithUtf16(pBegin, pEnd, utf16, utf16Size);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return EndWith(pBegin, pEnd, static_cast<char16_t>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return EndWith(pBegin, pEnd, static_cast<chara>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWithUtf32(const T* pBegin, const T* pEnd, const T utf32) noexcept {

    return pBegin == pEnd ? false : *(pEnd - 1) == utf32;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {

    return EndWithUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {

    return EndWithUtf32(pBegin, pEnd, static_cast<T>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {

    u32 utf32 = 0;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

    return EndWithUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4]{};
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {

    return EndWithUtf32(pBegin, pEnd, static_cast<T>(utf16));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2]{};
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    u32 utf32 = 0;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

    return EndWithUtf32(pBegin, pEnd, static_cast<T>(utf32));
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return EndWithUtf32(pBegin, pEnd, static_cast<T>(wchar));

#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return EndWithUtf32(pBegin, pEnd, static_cast<T>(wchar));

#endif // ALT_OS_WINDOWS
}

template <typename T, typename U,
    std::enable_if_t<is_compatible_with_character_v<T> && is_compatible_with_character_v<U> && TypeTraits::is_character_compatible_have_same_encoding_v<T, U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool StartWith(const T* pBegin, const T* pEnd, const U* pBytesBegin, const U* pBytesEnd) noexcept {

    const usize size = static_cast<usize>(pEnd - pBegin);
    const usize bytesSize = static_cast<usize>(pBytesEnd - pBytesBegin);

    if (bytesSize > size) {

        return false;
    }

    return CompareEQ(pBegin, pBegin + bytesSize, pBytesBegin, pBytesEnd);
}

template <typename T, typename U,
    std::enable_if_t<is_compatible_with_character_v<T> && is_compatible_with_character_v<U> && TypeTraits::is_character_compatible_have_same_encoding_v<T, U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool EndWith(const T* pBegin, const T* pEnd, const U* pBytesBegin, const U* pBytesEnd) noexcept {

    const usize size = static_cast<usize>(pEnd - pBegin);
    const usize bytesSize = static_cast<usize>(pBytesEnd - pBytesBegin);

    if (bytesSize > size) {

        return false;
    }

    return CompareEQ(pEnd - bytesSize, pEnd, pBytesBegin, pBytesEnd);
}

} // namespace _StringDetails
} // namespace Alt

#endif // __ALT_STD_STRING_DETAILS__