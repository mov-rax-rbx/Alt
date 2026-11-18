#pragma once

#ifndef __ALT_STD_STRING_DETAILS_PRE_INCLUDE__
#define __ALT_STD_STRING_DETAILS_PRE_INCLUDE__

#include "../../TypeTraits.h"

#include "../Impl/Utf/UtfCore.h"

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
} // namespace Alt

#endif // __ALT_STD_STRING_DETAILS_PRE_INCLUDE__