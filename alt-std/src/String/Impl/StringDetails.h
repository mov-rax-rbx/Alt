#pragma once

#ifndef __ALT_STD_STRING_DETAILS__
#define __ALT_STD_STRING_DETAILS__

#include "../../TypeTraits.h"
#include "../../Algorithms/Algorithms.h"

#include "../../Utils/Optional.h"

#include "../Impl/Utf/UtfCore.h"

#include "../Impl/Utf/Utf8.h"
#include "../Impl/Utf/Utf16.h"
#include "../Impl/Utf/Utf32.h"

namespace Alt {
namespace _StringDetails {

template <typename T>
struct __is_character__
    : std::false_type
{};

template <>
struct __is_character__<char>
    : std::true_type
{};

#ifdef __cpp_char8_t

template <>
struct __is_character__<char8_t>
    : std::true_type
{};
#endif // __cpp_char8_t

template <>
struct __is_character__<char16_t>
    : std::true_type
{};

template <>
struct __is_character__<char32_t>
    : std::true_type
{};

template <>
struct __is_character__<wchar_t>
    : std::true_type
{};

template <Utf::Encoding TYPE>
using UtfType = std::integral_constant<Utf::Encoding, TYPE>;

template <typename T>
struct __character_to_utf_type__
    : UtfType<Utf::Encoding::Unknown>
{};

template <>
struct __character_to_utf_type__<char>
    : UtfType<Utf::Encoding::Utf8>
{};

#ifdef __cpp_char8_t

template <>
struct __character_to_utf_type__<char8_t>
    : UtfType<Utf::Encoding::Utf8>
{};
#endif // __cpp_char8_t

template <>
struct __character_to_utf_type__<char16_t>
    : UtfType<Utf::Encoding::Utf16>
{};

template <>
struct __character_to_utf_type__<char32_t>
    : UtfType<Utf::Encoding::Utf32>
{};

#ifdef ALT_OS_WINDOWS

template <>
struct __character_to_utf_type__<wchar_t>
    : UtfType<Utf::Encoding::Utf16>
{};

#else

template <>
struct __character_to_utf_type__<wchar_t>
    : UtfType<Utf::Encoding::Utf32>
{};
#endif // ALT_OS_WINDOWS


template <typename T>
struct __is_character_compatible__
    : __is_character__<T>
{};

template <>
struct __is_character_compatible__<u8>
    : std::true_type
{};

template <>
struct __is_character_compatible__<u16>
    : std::true_type
{};

template <>
struct __is_character_compatible__<u32>
    : std::true_type
{};

template <typename T>
struct __character_compatible_to_utf_type__
    : __character_to_utf_type__<T>
{};

template <>
struct __character_compatible_to_utf_type__<u8>
    : UtfType<Utf::Encoding::Utf8>
{};

template <>
struct __character_compatible_to_utf_type__<u16>
    : UtfType<Utf::Encoding::Utf16>
{};

template <>
struct __character_compatible_to_utf_type__<u32>
    : UtfType<Utf::Encoding::Utf32>
{};

template <typename T>
struct __is_character_utf8__
    : std::false_type
{};

template <>
struct __is_character_utf8__<char>
    : std::true_type
{};

#ifdef __cpp_char8_t

template <>
struct __is_character_utf8__<char8_t>
    : std::true_type
{};
#endif // __cpp_char8_t

template <typename T>
struct __is_character_utf16__
    : std::false_type
{};

template <>
struct __is_character_utf16__<char16_t>
    : std::true_type
{};

#ifdef ALT_OS_WINDOWS

template <>
struct __is_character_utf16__<wchar_t>
    : std::true_type
{};
#endif // ALT_OS_WINDOWS

template <typename T>
struct __is_character_utf32__
    : std::false_type
{};

template <>
struct __is_character_utf32__<char32_t>
    : std::true_type
{};

#ifndef ALT_OS_WINDOWS

template <>
struct __is_character_utf32__<wchar_t>
    : std::true_type
{};
#endif // ALT_OS_WINDOWS

template <typename T>
struct __character_to_character_compatible_type__ {
    using type = void;
};

template <>
struct __character_to_character_compatible_type__<char> {
    using type = u8;
};

#ifdef __cpp_char8_t

template <>
struct __character_to_character_compatible_type__<char8_t> {
    using type = u8;
};
#endif // __cpp_char8_t

template <>
struct __character_to_character_compatible_type__<char16_t> {
    using type = u16;
};

#ifdef ALT_OS_WINDOWS

template <>
struct __character_to_character_compatible_type__<wchar_t> {
    using type = u16;
};
#else

template <>
struct __character_to_character_compatible_type__<wchar_t> {
    using type = u32;
};
#endif // ALT_OS_WINDOWS

template <>
struct __character_to_character_compatible_type__<char32_t> {
    using type = u32;
};

template <>
struct __character_to_character_compatible_type__<u8> {
    using type = u8;
};

template <>
struct __character_to_character_compatible_type__<u16> {
    using type = u16;
};

template <>
struct __character_to_character_compatible_type__<u32> {
    using type = u32;
};

template <typename T>
using character_to_utf_type_t = __character_to_utf_type__<TypeTraits::clean_t<T>>;

template <typename T>
constexpr static Utf::Encoding character_to_utf_type_v = character_to_utf_type_t<T>::value;

template <typename T>
using character_compatible_to_utf_type_t = __character_compatible_to_utf_type__<TypeTraits::clean_t<T>>;

template <typename T>
constexpr static Utf::Encoding character_compatible_to_utf_type_v = character_compatible_to_utf_type_t<T>::value;

template <usize>
class StringFormatWriter;

} // namespace _StringDetails

namespace TypeTraits {

template <typename T>
using is_character_t = _StringDetails::__is_character__<TypeTraits::clean_t<T>>;

template <typename T>
constexpr static bool is_character_v = is_character_t<T>::value;

template <typename T>
using is_compatible_with_character_t = _StringDetails::__is_character_compatible__<TypeTraits::clean_t<T>>;

template <typename T>
constexpr static bool is_compatible_with_character_v = is_compatible_with_character_t<T>::value;

template <typename T>
using is_character_utf8_t = _StringDetails::__is_character_utf8__<TypeTraits::clean_t<T>>;

template <typename T>
constexpr static bool is_character_utf8_v = is_character_utf8_t<T>::value;

template <typename T>
constexpr static bool is_compatible_with_character_utf8_v =
    _StringDetails::character_compatible_to_utf_type_v<T> == Utf::Encoding::Utf8;

template <typename T>
using is_character_utf16_t = _StringDetails::__is_character_utf16__<TypeTraits::clean_t<T>>;

template <typename T>
constexpr static bool is_character_utf16_v = is_character_utf16_t<T>::value;

template <typename T>
constexpr static bool is_compatible_with_character_utf16_v =
    _StringDetails::character_compatible_to_utf_type_v<T> == Utf::Encoding::Utf16;

template <typename T>
using is_character_utf32_t = _StringDetails::__is_character_utf32__<TypeTraits::clean_t<T>>;

template <typename T>
constexpr static bool is_character_utf32_v = is_character_utf32_t<T>::value;

template <typename T>
constexpr static bool is_compatible_with_character_utf32_v =
    _StringDetails::character_compatible_to_utf_type_v<T> == Utf::Encoding::Utf32;

template <typename T, typename U>
constexpr static bool is_character_have_same_encoding_v =
    _StringDetails::character_to_utf_type_v<T> == _StringDetails::character_to_utf_type_v<U>;

template <typename T, typename U>
constexpr static bool is_character_compatible_have_same_encoding_v =
    _StringDetails::character_compatible_to_utf_type_v<T> == _StringDetails::character_compatible_to_utf_type_v<U>;

template <typename T>
using character_to_character_compatible_type_t =
    preserve_qualifiers_t<T, typename _StringDetails::__character_to_character_compatible_type__<TypeTraits::clean_t<T>>::type>;

} // namespace TypeTraits

namespace _StringDetails {

template <typename T>
constexpr static bool is_character_v = TypeTraits::is_character_v<T>;

template <typename T>
constexpr static bool is_compatible_with_character_utf8_v = TypeTraits::is_compatible_with_character_utf8_v<T>;

template <typename T>
constexpr static bool is_compatible_with_character_utf16_v = TypeTraits::is_compatible_with_character_utf16_v<T>;

template <typename T>
constexpr static bool is_compatible_with_character_utf32_v = TypeTraits::is_compatible_with_character_utf32_v<T>;

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
ALT_NODISCARD ALT_FORCE_INLINE bool IsAscii(const chara utf32) noexcept {

    return utf32 <= 127;
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsAsciiWhitespace(const chara utf32) noexcept {

    return utf32 == ' ' || (utf32 >= 9 && utf32 <= 13); // \t, \n, \v, \f, \r
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsAsciiPrintable(const chara utf32) noexcept {

    return utf32 >= 32 && utf32 <= 126;
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsAsciiAlphabetLower(const chara utf32) noexcept {

    return utf32 >= 97 && utf32 <= 122; // [a..z]
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsAsciiAlphabetUpper(const chara utf32) noexcept {

    return utf32 >= 65 && utf32 <= 90; // [A..Z]
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsAsciiAlphabet(const chara utf32) noexcept {

    return IsAsciiAlphabetLower(utf32) || IsAsciiAlphabetUpper(utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsAsciiDigit(const chara utf32) noexcept {

    return utf32 >= 48 && utf32 <= 57; // [0..9]
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsAsciiHex(const chara utf32) noexcept {

    return IsAsciiDigit(utf32) || (utf32 >= 65 && utf32 <= 70) || (utf32 >= 97 && utf32 <= 102); // [0..9] [ABCDEF] [abcdef]
}

ALT_NODISCARD ALT_FORCE_INLINE chara AsciiToLower(const chara utf32) noexcept {

    return IsAsciiAlphabetUpper(utf32) ? utf32 + 32 : utf32;
}

ALT_NODISCARD ALT_FORCE_INLINE chara AsciiToUpper(const chara utf32) noexcept {

    return IsAsciiAlphabetLower(utf32) ? utf32 - 32 : utf32;
}

ALT_NODISCARD ALT_FORCE_INLINE usize CalculateCStrLen(const char* str) noexcept {
    ALT_ASSERT(str);

    return std::strlen(str);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE usize CalculateCStrLen(const char8_t* str) noexcept {
    ALT_ASSERT(str);

    static_assert(sizeof(char8_t) == sizeof(char), "");

    return std::strlen(reinterpret_cast<const char*>(str));
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE usize CalculateCStrLen(const char16_t* str) noexcept {
    ALT_ASSERT(str);

#ifdef ALT_OS_WINDOWS

    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    return std::wcslen(reinterpret_cast<const wchar_t*>(str));
#else

    const char16_t* pStr = str;
    while (*pStr != 0) {

        pStr += 1;
    }

    return static_cast<usize>(pStr - str);
#endif // ALT_OS_WINDOWS
}

ALT_NODISCARD ALT_FORCE_INLINE usize CalculateCStrLen(const chara* str) noexcept {
    ALT_ASSERT(str);

#ifdef ALT_OS_WINDOWS

    const chara* pStr = str;
    while (*pStr != 0) {

        pStr += 1;
    }

    return static_cast<usize>(pStr - str);
#else

    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return std::wcslen(reinterpret_cast<const wchar_t*>(str));
#endif // ALT_OS_WINDOWS
}

ALT_NODISCARD ALT_FORCE_INLINE usize CalculateCStrLen(const wchar_t* str) noexcept {
    ALT_ASSERT(str);

    return std::wcslen(str);
}

template <typename T,
    std::enable_if<is_character_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE usize CalculateSizeInBytes(const T* str) noexcept {

    return CalculateCStrLen(str) * sizeof(T);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE const T* TrimLeftImpl(const T* pBegin, const T* pEnd) noexcept {

    while (pBegin != pEnd && IsAsciiWhitespace(*pBegin)) {

        pBegin += 1;
    }

    return pBegin;
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE const T* TrimRightImpl(const T* pBegin, const T* pEnd) noexcept {

    if (pBegin == pEnd) {
        return pEnd;
    }

    bool isWhitespace = true;
    do {

        pEnd -= 1;
        isWhitespace = IsAsciiWhitespace(*pEnd);

    } while (pBegin != pEnd && isWhitespace);

    pEnd += isWhitespace ? 0 : 1;

    return pEnd;
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE const char* TrimLeft(const T* pBegin, const T* pEnd) noexcept {

    const u8* pResult = TrimLeftImpl(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd));
    return reinterpret_cast<const T*>(pResult);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE const char* TrimRight(const T* pBegin, const T* pEnd) noexcept {

    const u8* pResult = TrimRightImpl(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd));
    return reinterpret_cast<const T*>(pResult);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE const char* TrimLeft(const T* pBegin, const T* pEnd) noexcept {

    const u16* pResult = TrimLeftImpl(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd));
    return reinterpret_cast<const T*>(pResult);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE const char* TrimRight(const T* pBegin, const T* pEnd) noexcept {

    const u16* pResult = TrimRightImpl(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd));
    return reinterpret_cast<const T*>(pResult);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE const char* TrimLeft(const T* pBegin, const T* pEnd) noexcept {

    const u32* pResult = TrimLeftImpl(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd));
    return reinterpret_cast<const T*>(pResult);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE const char* TrimRight(const T* pBegin, const T* pEnd) noexcept {

    const u32* pResult = TrimRightImpl(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd));
    return reinterpret_cast<const T*>(pResult);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirstUtf8(
    const u8* pBegin, const u8* pEnd, const u8 (&utf8)[4], const usize utf8Size
) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf8Size > 0);

    bool isOk = true;
    bool isEq = true;

    const u8* pVal = pBegin;

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

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const chara utf32) noexcept {

    u8 utf8[4];
    const Utf::Result result = Utf::_32::Utf32ToUtf8IgnoreErrors(utf32, utf8);

    return FindFirstUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const u8 utf8) noexcept {

    const u8* pVal = Algorithms::FindFirst(pBegin, pEnd, utf8);

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const u8 (&utf8)[4]) noexcept {

    usize utf8Size = 1;
    utf8Size += utf8[1] != 0 ? 1 : 0;
    utf8Size += utf8[2] != 0 ? 1 : 0;
    utf8Size += utf8[3] != 0 ? 1 : 0;

    return FindFirstUtf8(pBegin, pEnd, utf8, utf8Size);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const char utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const char8_t utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    u8 utf8[4];
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

    return FindFirstUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const char16_t (&utf16_)[2]) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_[0]),
        static_cast<u16>(utf16_[1]),
    };

    u8 utf8[4];
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

    return FindFirstUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u8* pBegin, const u8* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return FindFirst(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindFirst(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return FindFirst(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), wchar);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLastUtf8(
    const u8* pBegin, const u8* pEnd, const u8 (&utf8)[4], const usize utf8Size
) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf8Size > 0);

    const u8* pValEnd = utf8Size <= static_cast<usize>(pEnd - pBegin)
        ? pEnd - utf8Size + 1
        : pEnd;

    bool isEq = true;
    bool isOk = false;

    do {

        const u8* pValEndOld = pValEnd;
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

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const chara utf32) noexcept {

    u8 utf8[4];
    const Utf::Result result = Utf::_32::Utf32ToUtf8IgnoreErrors(utf32, utf8);

    return FindLastUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const u8 utf8) noexcept {

    const u8* pVal = Algorithms::FindLast(pBegin, pEnd, utf8);

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const u8 (&utf8)[4]) noexcept {

    usize utf8Size = 1;
    utf8Size += utf8[1] != 0 ? 1 : 0;
    utf8Size += utf8[2] != 0 ? 1 : 0;
    utf8Size += utf8[3] != 0 ? 1 : 0;

    return FindLastUtf8(pBegin, pEnd, utf8, utf8Size);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const char utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const char8_t utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    u8 utf8[4];
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

    return FindLastUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const char16_t (&utf16_)[2]) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_[0]),
        static_cast<u16>(utf16_[1]),
    };

    u8 utf8[4];
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

    return FindLastUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u8* pBegin, const u8* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return FindLast(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindLast(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return FindLast(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), wchar);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirstUtf16(
    const u16* pBegin, const u16* pEnd, const u16 (&utf16)[2], const usize utf16Size
) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf16Size > 0);

    bool isOk = true;
    bool isEq = true;

    const u16* pVal = pBegin;

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

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const chara utf32) noexcept {

    u16 utf16[2];
    const Utf::Result result = Utf::_32::Utf32ToUtf16IgnoreErrors(utf32, utf16);

    return FindFirstUtf16(pBegin, pEnd, utf16, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const u8 utf8) noexcept {

    const u16* pVal = Algorithms::FindFirst(pBegin, pEnd, static_cast<u16>(utf8));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const u8 (&utf8)[4]) noexcept {

    u16 utf16[2];
    const Utf::Result result = Utf::_8::Utf8ToUtf16IgnoreErrors(utf8, utf16);

    return FindFirstUtf16(pBegin, pEnd, utf16, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const char utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const char8_t utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    return FindFirstUtf16(pBegin, pEnd, utf16, 1);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2];
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    usize utf16Size = 1;
    utf16Size += utf16[1] != 0 ? 1 : 0;

    return FindFirstUtf16(pBegin, pEnd, utf16, utf16Size);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u16* pBegin, const u16* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return FindFirst(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindFirst(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return FindFirst(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), wchar);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLastUtf16(
    const u16* pBegin, const u16* pEnd, const u16 (&utf16)[2], const usize utf16Size
) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf16Size > 0);

    const u16* pValEnd = utf16Size <= static_cast<usize>(pEnd - pBegin)
        ? pEnd - utf16Size + 1
        : pEnd;

    bool isEq = true;
    bool isOk = false;

    do {

        const u16* pValEndOld = pValEnd;
        pValEnd = Algorithms::FindLast(pBegin, pValEndOld, utf16[0]);

        isOk = pValEnd != pValEndOld;

        isEq = isOk;
        if (isOk && 1 < utf16Size) isEq = isEq && *(pValEnd + 1) == utf16[1];

    } while (isOk && !isEq);

    return isEq
        ? MakeOptional(pValEnd - pBegin)
        : Nullopt;
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const chara utf32) noexcept {

    u16 utf16[2];
    const Utf::Result result = Utf::_32::Utf32ToUtf16IgnoreErrors(utf32, utf16);

    return FindLastUtf16(pBegin, pEnd, utf16, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const u8 utf8) noexcept {

    const u16* pVal = Algorithms::FindLast(pBegin, pEnd, static_cast<u16>(utf8));

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const u8 (&utf8)[4]) noexcept {

    u16 utf16[2];
    const Utf::Result result = Utf::_8::Utf8ToUtf16IgnoreErrors(utf8, utf16);

    return FindLastUtf16(pBegin, pEnd, utf16, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const char utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const char8_t utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    return FindLastUtf16(pBegin, pEnd, utf16, 1);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2];
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    usize utf16Size = 1;
    utf16Size += utf16[1] != 0 ? 1 : 0;

    return FindLastUtf16(pBegin, pEnd, utf16, utf16Size);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u16* pBegin, const u16* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return FindLast(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindLast(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return FindLast(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), wchar);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirstUtf32(
    const u32* pBegin, const u32* pEnd, const u32 utf32
) noexcept {

    const u32* pVal = Algorithms::FindFirst(pBegin, pEnd, utf32);

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const chara utf32) noexcept {

    return FindFirstUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const u8 utf8) noexcept {

    return FindFirstUtf32(pBegin, pEnd, static_cast<u32>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const u8 (&utf8)[4]) noexcept {

    u32 utf32;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

    return FindFirstUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const char utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const char8_t utf8) noexcept {

    return FindFirst(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindFirst(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const char16_t utf16) noexcept {

    return FindFirstUtf32(pBegin, pEnd, static_cast<u32>(utf16));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2];
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    u32 utf32;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

    return FindFirstUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const u32* pBegin, const u32* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return FindFirst(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindFirst(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return FindFirst(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), wchar);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLastUtf32(
    const u32* pBegin, const u32* pEnd, const u32 utf32
) noexcept {

    const u32* pVal = Algorithms::FindLast(pBegin, pEnd, utf32);

    return pVal != pEnd
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const chara utf32) noexcept {

    return FindLastUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const u8 utf8) noexcept {

    return FindLastUtf32(pBegin, pEnd, static_cast<u32>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const u8 (&utf8)[4]) noexcept {

    u32 utf32;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

    return FindLastUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const char utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const char8_t utf8) noexcept {

    return FindLast(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return FindLast(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const char16_t utf16) noexcept {

    return FindLastUtf32(pBegin, pEnd, static_cast<u32>(utf16));
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2];
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    u32 utf32;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

    return FindLastUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const u32* pBegin, const u32* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return FindLast(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return FindLast(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return FindLast(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), wchar);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin, const T* pEnd, const T* pBytesBegin, const T* pBytesEnd) noexcept {

    const usize size = static_cast<usize>(pEnd - pBegin);
    const usize bytesSize = static_cast<usize>(pBytesEnd - pBytesBegin);

    if (bytesSize == 0) {
        return 0;
    }

    if (bytesSize > size) {
        return Nullopt;
    }

    const T* pBytesBeginPlusOne = pBytesBegin + 1;
    const usize bytesSizeMinusOne = bytesSize - 1;

    const T* pVal = pBegin;
    const T* pEndAlignedToSizeCheck = pEnd - bytesSizeMinusOne;

    bool isEq = false;
    bool isOk = true;

    do {

        pVal = Algorithms::FindFirst(pVal, pEnd, *pBytesBegin);

        isOk = pVal < pEndAlignedToSizeCheck;
        pVal += 1;

        isEq = isOk;
        isEq &= Algorithms::CompareEQ(pVal, pVal + (isOk ? bytesSizeMinusOne : 0), pBytesBeginPlusOne, pBytesEnd);

    } while (isOk && !isEq);

    pVal -= 1;

    return isEq
        ? MakeOptional(pVal - pBegin)
        : Nullopt;
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T> && is_compatible_with_character_utf8_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    return FindFirst(
        reinterpret_cast<const u8*>(pBegin0),
        reinterpret_cast<const u8*>(pEnd0),
        reinterpret_cast<const u8*>(pBegin1),
        reinterpret_cast<const u8*>(pEnd1)
    );
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T> && is_compatible_with_character_utf16_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    return FindFirst(
        reinterpret_cast<const u16*>(pBegin0),
        reinterpret_cast<const u16*>(pEnd0),
        reinterpret_cast<const u16*>(pBegin1),
        reinterpret_cast<const u16*>(pEnd1)
    );
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T> && is_compatible_with_character_utf32_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirst(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    return FindFirst(
        reinterpret_cast<const u32*>(pBegin0),
        reinterpret_cast<const u32*>(pEnd0),
        reinterpret_cast<const u32*>(pBegin1),
        reinterpret_cast<const u32*>(pEnd1)
    );
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin, const T* pEnd, const T* pBytesBegin, const T* pBytesEnd) noexcept {

    const usize size = static_cast<usize>(pEnd - pBegin);
    const usize bytesSize = static_cast<usize>(pBytesEnd - pBytesBegin);

    if (bytesSize == 0) {
        return 0;
    }

    if (bytesSize > size) {
        return Nullopt;
    }

    const T* pBytesBeginPlusOne = pBytesBegin + 1;
    const usize bytesSizeMinusOne = bytesSize - 1;

    const T* pValEnd = pEnd - bytesSizeMinusOne;

    bool isEq = false;
    bool isOk = true;

    do {

        const T* pValEndOld = pValEnd;
        pValEnd = Algorithms::FindLast(pBegin, pValEndOld, *pBytesBegin);

        isOk = pValEnd != pValEndOld;

        const T* pVal = pValEnd + 1;

        isEq = isOk;
        isEq &= Algorithms::CompareEQ(pVal, pVal + (isOk ? bytesSizeMinusOne : 0), pBytesBeginPlusOne, pBytesEnd);

    } while (isOk && !isEq);

    return isEq
        ? MakeOptional(pValEnd - pBegin)
        : Nullopt;
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T> && is_compatible_with_character_utf8_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    return FindLast(
        reinterpret_cast<const u8*>(pBegin0),
        reinterpret_cast<const u8*>(pEnd0),
        reinterpret_cast<const u8*>(pBegin1),
        reinterpret_cast<const u8*>(pEnd1)
    );
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T> && is_compatible_with_character_utf16_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    return FindLast(
        reinterpret_cast<const u16*>(pBegin0),
        reinterpret_cast<const u16*>(pEnd0),
        reinterpret_cast<const u16*>(pBegin1),
        reinterpret_cast<const u16*>(pEnd1)
    );
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T> && is_compatible_with_character_utf32_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindLast(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    return FindLast(
        reinterpret_cast<const u32*>(pBegin0),
        reinterpret_cast<const u32*>(pEnd0),
        reinterpret_cast<const u32*>(pBegin1),
        reinterpret_cast<const u32*>(pEnd1)
    );
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWithUtf8(const u8* pBegin, const u8* pEnd, const u8 (&utf8)[4], const usize utf8Size) noexcept {

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

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const chara utf32) noexcept {

    u8 utf8[4];
    const Utf::Result result = Utf::_32::Utf32ToUtf8IgnoreErrors(utf32, utf8);

    return StartWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const u8 utf8_) noexcept {

    const u8 utf8[4]{
        utf8_, 0, 0, 0
    };

    return StartWithUtf8(pBegin, pEnd, utf8, 1);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const u8 (&utf8)[4]) noexcept {

    usize utf8Size = 1;
    utf8Size += utf8[1] != 0 ? 1 : 0;
    utf8Size += utf8[2] != 0 ? 1 : 0;
    utf8Size += utf8[3] != 0 ? 1 : 0;

    return StartWithUtf8(pBegin, pEnd, utf8, utf8Size);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const char utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const char8_t utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    u8 utf8[4];
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

    return StartWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const char16_t (&utf16_)[2]) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_[0]),
        static_cast<u16>(utf16_[1]),
    };

    u8 utf8[4];
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

    return StartWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u8* pBegin, const u8* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return StartWith(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return StartWith(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return StartWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), wchar);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWithUtf16(const u16* pBegin, const u16* pEnd, const u16 (&utf16)[2], const usize utf16Size) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf16Size > 0);

    if (static_cast<usize>(pEnd - pBegin) < utf16Size) {
        return false;
    }

    bool isEq = *pBegin == utf16[0];
    if (1 < utf16Size) isEq = isEq && *(pBegin + 1) == utf16[1];

    return isEq;
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const chara utf32) noexcept {

    u16 utf16[2];
    const Utf::Result result = Utf::_32::Utf32ToUtf16IgnoreErrors(utf32, utf16);

    return StartWithUtf16(pBegin, pEnd, utf16, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const u8 utf8) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf8),
        0
    };

    return StartWithUtf16(pBegin, pEnd, utf16, 1);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const u8 (&utf8)[4]) noexcept {

    u16 utf16[2];
    const Utf::Result result = Utf::_8::Utf8ToUtf16IgnoreErrors(utf8, utf16);

    return StartWithUtf16(pBegin, pEnd, utf16, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const char utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const char8_t utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    return StartWithUtf16(pBegin, pEnd, utf16, 1);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2];
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    usize utf16Size = 1;
    utf16Size += utf16[1] != 0 ? 1 : 0;

    return StartWithUtf16(pBegin, pEnd, utf16, utf16Size);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u16* pBegin, const u16* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return StartWith(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return StartWith(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return StartWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), wchar);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWithUtf32(const u32* pBegin, const u32* pEnd, const u32 utf32) noexcept {

    return static_cast<usize>(pEnd - pBegin) == 0 ? false : *pBegin == utf32;
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const chara utf32) noexcept {

    return StartWithUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const u8 utf8) noexcept {

    return StartWithUtf32(pBegin, pEnd, static_cast<u32>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const u8 (&utf8)[4]) noexcept {

    u32 utf32;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

    return StartWithUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const char utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const char8_t utf8) noexcept {

    return StartWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return StartWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const char16_t utf16) noexcept {

    return StartWithUtf32(pBegin, pEnd, static_cast<u32>(utf16));
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2];
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    u32 utf32;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

    return StartWithUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const u32* pBegin, const u32* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return StartWith(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return StartWith(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return StartWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), wchar);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin, const T* pEnd, const T* pBytesBegin, const T* pBytesEnd) noexcept {

    const usize size = static_cast<usize>(pEnd - pBegin);
    const usize bytesSize = static_cast<usize>(pBytesEnd - pBytesBegin);

    if (bytesSize > size) {

        return false;
    }

    return Algorithms::CompareEQ(pBegin, pBegin + bytesSize, pBytesBegin, pBytesEnd);
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T> && is_compatible_with_character_utf8_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    return StartWith(
        reinterpret_cast<const u8*>(pBegin0),
        reinterpret_cast<const u8*>(pEnd0),
        reinterpret_cast<const u8*>(pBegin1),
        reinterpret_cast<const u8*>(pEnd1)
    );
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T> && is_compatible_with_character_utf16_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    return StartWith(
        reinterpret_cast<const u16*>(pBegin0),
        reinterpret_cast<const u16*>(pEnd0),
        reinterpret_cast<const u16*>(pBegin1),
        reinterpret_cast<const u16*>(pEnd1)
    );
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T> && is_compatible_with_character_utf32_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool StartWith(const T* pBegin0, const T* pEnd0, const U* pBegin1, const U* pEnd1) noexcept {

    return StartWith(
        reinterpret_cast<const u32*>(pBegin0),
        reinterpret_cast<const u32*>(pEnd0),
        reinterpret_cast<const u32*>(pBegin1),
        reinterpret_cast<const u32*>(pEnd1)
    );
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWithUtf8(const u8* pBegin, const u8* pEnd, const u8 (&utf8)[4], const usize utf8Size) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf8Size > 0);

    if (static_cast<usize>(pEnd - pBegin) < utf8Size) {
        return false;
    }

    const u8* pVal = pEnd - utf8Size;

    bool isEq = *pVal == utf8[0];
    if (1 < utf8Size) isEq = isEq && *(pVal + 1) == utf8[1];
    if (2 < utf8Size) isEq = isEq && *(pVal + 2) == utf8[2];
    if (3 < utf8Size) isEq = isEq && *(pVal + 3) == utf8[3];

    return isEq;
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const chara utf32) noexcept {

    u8 utf8[4];
    const Utf::Result result = Utf::_32::Utf32ToUtf8IgnoreErrors(utf32, utf8);

    return EndWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const u8 utf8_) noexcept {

    const u8 utf8[4]{
        utf8_, 0, 0, 0
    };

    return EndWithUtf8(pBegin, pEnd, utf8, 1);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const u8 (&utf8)[4]) noexcept {

    usize utf8Size = 1;
    utf8Size += utf8[1] != 0 ? 1 : 0;
    utf8Size += utf8[2] != 0 ? 1 : 0;
    utf8Size += utf8[3] != 0 ? 1 : 0;

    return EndWithUtf8(pBegin, pEnd, utf8, utf8Size);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const char utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const char8_t utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    u8 utf8[4];
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

    return EndWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const char16_t (&utf16_)[2]) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_[0]),
        static_cast<u16>(utf16_[1]),
    };

    u8 utf8[4];
    const Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

    return EndWithUtf8(pBegin, pEnd, utf8, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u8* pBegin, const u8* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return EndWith(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return EndWith(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return EndWith(reinterpret_cast<const u8*>(pBegin), reinterpret_cast<const u8*>(pEnd), wchar);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWithUtf16(const u16* pBegin, const u16* pEnd, const u16 (&utf16)[2], const usize utf16Size) noexcept {

    // if we call this function - mean we pass any charactrer, '\0' also character
    ALT_ASSERT(utf16Size > 0);

    if (static_cast<usize>(pEnd - pBegin) < utf16Size) {
        return false;
    }

    const u16* pVal = pEnd - utf16Size;

    bool isEq = *pVal == utf16[0];
    if (1 < utf16Size) isEq = isEq && *(pVal + 1) == utf16[1];

    return isEq;
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const chara utf32) noexcept {

    u16 utf16[2];
    const Utf::Result result = Utf::_32::Utf32ToUtf16IgnoreErrors(utf32, utf16);

    return EndWithUtf16(pBegin, pEnd, utf16, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const u8 utf8) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf8),
        0
    };

    return EndWithUtf16(pBegin, pEnd, utf16, 1);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const u8 (&utf8)[4]) noexcept {

    u16 utf16[2];
    const Utf::Result result = Utf::_8::Utf8ToUtf16IgnoreErrors(utf8, utf16);

    return EndWithUtf16(pBegin, pEnd, utf16, result.consumed);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const char utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const char8_t utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const char16_t utf16_) noexcept {

    const u16 utf16[2]{
        static_cast<u16>(utf16_),
        0
    };

    return EndWithUtf16(pBegin, pEnd, utf16, 1);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2];
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    usize utf16Size = 1;
    utf16Size += utf16[1] != 0 ? 1 : 0;

    return EndWithUtf16(pBegin, pEnd, utf16, utf16Size);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u16* pBegin, const u16* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return EndWith(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return EndWith(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return EndWith(reinterpret_cast<const u16*>(pBegin), reinterpret_cast<const u16*>(pEnd), wchar);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWithUtf32(const u32* pBegin, const u32* pEnd, const u32 utf32) noexcept {

    return static_cast<usize>(pEnd - pBegin) == 0 ? false : *(pEnd - 1) == utf32;
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const chara utf32) noexcept {

    return EndWithUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const u8 utf8) noexcept {

    return EndWithUtf32(pBegin, pEnd, static_cast<u32>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const u8 (&utf8)[4]) noexcept {

    u32 utf32;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

    return EndWithUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const char utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const char (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}

#ifdef __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const char8_t utf8) noexcept {

    return EndWith(pBegin, pEnd, static_cast<u8>(utf8));
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const char8_t (&utf8_)[4]) noexcept {

    u8 utf8[4];
    Memory::TransmuteInto<char8_t[4], u8[4]>(utf8_, utf8);

    return EndWith(pBegin, pEnd, utf8);
}
#endif // __cpp_char8_t

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const char16_t utf16) noexcept {

    return EndWithUtf32(pBegin, pEnd, static_cast<u32>(utf16));
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const char16_t (&utf16_)[2]) noexcept {

    u16 utf16[2];
    Memory::TransmuteInto<char16_t[2], u16[2]>(utf16_, utf16);

    u32 utf32;
    ALT_MAYBE_UNUSED const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

    return EndWithUtf32(pBegin, pEnd, utf32);
}

ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const u32* pBegin, const u32* pEnd, const wchar_t wchar) noexcept {

#ifdef ALT_OS_WINDOWS
    static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

    const char16_t utf16[2]{ static_cast<char16_t>(wchar), 0 };
    return EndWith(pBegin, pEnd, utf16);
#else
    static_assert(sizeof(wchar_t) == sizeof(chara), "");

    return EndWith(pBegin, pEnd, static_cast<chara>(wchar));
#endif // ALT_OS_WINDOWS
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const chara utf32) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf32);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const u8 utf8) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const u8 (&utf8)[4]) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char utf8) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char (&utf8)[4]) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

#ifdef __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char8_t utf8) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char8_t (&utf8)[4]) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf8);
}
#endif // __cpp_char8_t

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char16_t utf16) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const char16_t (&utf16)[2]) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), utf16);
}

template <typename T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const wchar_t wchar) noexcept {
    return EndWith(reinterpret_cast<const u32*>(pBegin), reinterpret_cast<const u32*>(pEnd), wchar);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const T* pBytesBegin, const T* pBytesEnd) noexcept {

    const usize size = static_cast<usize>(pEnd - pBegin);
    const usize bytesSize = static_cast<usize>(pBytesEnd - pBytesBegin);

    if (bytesSize > size) {

        return false;
    }

    return Algorithms::CompareEQ(pEnd - bytesSize, pEnd, pBytesBegin, pBytesEnd);
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf8_v<T> && is_compatible_with_character_utf8_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const U* pStrBegin, const U* pStrEnd) noexcept {

    return EndWith(
        reinterpret_cast<const u8*>(pBegin),
        reinterpret_cast<const u8*>(pEnd),
        reinterpret_cast<const u8*>(pStrBegin),
        reinterpret_cast<const u8*>(pStrEnd)
    );
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf16_v<T> && is_compatible_with_character_utf16_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const U* pStrBegin, const U* pStrEnd) noexcept {

    return EndWith(
        reinterpret_cast<const u16*>(pBegin),
        reinterpret_cast<const u16*>(pEnd),
        reinterpret_cast<const u16*>(pStrBegin),
        reinterpret_cast<const u16*>(pStrEnd)
    );
}

template <typename T, typename U = T,
    std::enable_if_t<is_compatible_with_character_utf32_v<T> && is_compatible_with_character_utf32_v<U>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE bool EndWith(const T* pBegin, const T* pEnd, const U* pStrBegin, const U* pStrEnd) noexcept {

    return EndWith(
        reinterpret_cast<const u32*>(pBegin),
        reinterpret_cast<const u32*>(pEnd),
        reinterpret_cast<const u32*>(pStrBegin),
        reinterpret_cast<const u32*>(pStrEnd)
    );
}

} // namespace _StringDetails
} // namespace Alt

#endif // __ALT_STD_STRING_DETAILS__