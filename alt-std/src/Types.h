#pragma once

#ifndef __ALT_STD_TYPES__
#define __ALT_STD_TYPES__

#include <cstdint>

#include "Defines.h"

namespace Alt {
namespace Types {

using usize = size_t;
using umax = uintmax_t;
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

using ssize = ptrdiff_t;
using smax = intmax_t;
using s64 = int64_t;
using s32 = int32_t;
using s16 = int16_t;
using s8 = int8_t;

using uptr = uintptr_t;
using sptr = intptr_t;

using f32 = float;
using f64 = double;

// UTF-32 character
using chara = char32_t;

} // namespace Types

namespace TypeTraits {

template <typename T>
struct __is_number__
    : std::false_type
{};

template <>
struct __is_number__<Types::u64>
    : std::true_type
{};

template <>
struct __is_number__<Types::u32>
    : std::true_type
{};

template <>
struct __is_number__<Types::u16>
    : std::true_type
{};

template <>
struct __is_number__<Types::u8>
    : std::true_type
{};

template <>
struct __is_number__<Types::s64>
    : std::true_type
{};

template <>
struct __is_number__<Types::s32>
    : std::true_type
{};

template <>
struct __is_number__<Types::s16>
    : std::true_type
{};

template <>
struct __is_number__<Types::s8>
    : std::true_type
{};

/**
 * Redifinitions
 */
//template <>
//struct __is_number__<Types::usize>
//    : std::true_type
//{};
//
//template <>
//struct __is_number__<Types::ssize>
//    : std::true_type
//{};
//
//template <>
//struct __is_number__<Types::umax>
//    : std::true_type
//{};
//
//template <>
//struct __is_number__<Types::smax>
//    : std::true_type
//{};
//
//template <>
//struct __is_number__<Types::uptr>
//    : std::true_type
//{};
//
//template <>
//struct __is_number__<Types::sptr>
//    : std::true_type
//{};

template <>
struct __is_number__<Types::f32>
    : std::true_type
{};

template <>
struct __is_number__<Types::f64>
    : std::true_type
{};

} // namespace TypeTraits

} // namespace Alt

#endif // __ALT_STD_TYPES__