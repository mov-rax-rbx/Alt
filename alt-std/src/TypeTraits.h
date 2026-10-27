#pragma once

#ifndef __ALT_STD_TYPE_TRAITS__
#define __ALT_STD_TYPE_TRAITS__

#include <cstdlib>

#include <limits>
#include <type_traits>

#include "Types.h"
#include "Defines.h"

#include "Platform/Impl/PlatformPanicPreInclude.h"

#ifdef ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

using namespace Alt::Types;

#endif // ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#ifndef ALT_BREAKPOINT

#if defined(ALT_COMPILER_MSVC)
    #define ALT_BREAKPOINT __debugbreak()
#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)
    #define ALT_BREAKPOINT __builtin_debugtrap()
#else
    #define ALT_BREAKPOINT do {} while(0)
#endif

#endif // ALT_BREAKPOINT

#ifndef ALT_BREAKPOINT_DEV

#ifdef ALT_DEV
    #define ALT_BREAKPOINT_DEV ALT_BREAKPOINT
#else
    #define ALT_BREAKPOINT_DEV do {} while(0)
#endif // ALT_DEV

#endif // ALT_BREAKPOINT_DEV

#define ALT_PANIC_SILENT() \
do { \
    ALT_BREAKPOINT_DEV; \
    Alt::Platform::FastCrash(); \
} while(0)

#ifdef ALT_DEV
    #define ALT_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            Alt::_PlatformDetails::EPrintln_("{}:{}: {}", __FILE__, (Alt::usize)__LINE__, __ALT_STD_STRINGIFY__(expr)); \
            ALT_BREAKPOINT; \
            Alt::Platform::FastCrash(); \
        } \
    } while(0)
#else
    #define ALT_ASSERT(expr) do {} while(0)
#endif // ALT_DEV

#ifdef ALT_DEV

#if defined(ALT_COMPILER_MSVC)

    #define ALT_UNREACHABLE(...) \
    do { \
        ALT_PANIC_SILENT(); \
        __assume(0); \
    } while(0)
#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    #define ALT_UNREACHABLE(...) \
    do { \
        ALT_PANIC_SILENT(); \
        __builtin_unreachable(); \
    } while(0)
#else

    #error undefined compiler
#endif // defined(ALT_COMPILER_MSVC)
#else

#if defined(ALT_COMPILER_MSVC)

    #define ALT_UNREACHABLE(...) \
    do { \
        __assume(0); \
    } while(0)
#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    #define ALT_UNREACHABLE(...) \
    do { \
        __builtin_unreachable(); \
    } while(0)
#else

    #error undefined compiler
#endif // defined(ALT_COMPILER_MSVC)
#endif // ALT_DEV

#define __ALT_STD_STRINGIFY0__(x) #x
#define __ALT_STD_STRINGIFY__(x) __ALT_STD_STRINGIFY0__(x)

#define __ALT_STD_CONCAT_MACROS__0(x, y) x##y
#define __ALT_STD_CONCAT_MACROS__(x, y) __ALT_STD_CONCAT_MACROS__0(x, y)

namespace Alt {

using namespace Alt::Types;

template <typename T>
class Optional;

namespace TypeTraits {

template <typename T>
using clean_t = std::remove_cv_t<T>;

template <typename T0, typename T1>
using select_larger_t = std::conditional_t<(sizeof(T0) > sizeof(T1)), T0, T1>;

/*
 * Reallocatable Trait
 * 
 * https://en.cppreference.com/w/cpp/identifier_with_special_meaning/trivially_relocatable_if_eligible
 */
template <typename T>
struct __is_trivially_reallocatable__
    : std::conditional_t<
        std::is_trivially_copyable_v<T>,
        std::true_type,
        std::false_type
    >
{};

template <>
struct __is_trivially_reallocatable__<void>
    : std::true_type
{};

template <typename T>
using is_trivially_reallocatable_t = __is_trivially_reallocatable__<clean_t<T>>;

template <typename T>
constexpr static bool is_trivially_reallocatable_v = is_trivially_reallocatable_t<T>::value;

/*
 * Clonable Trait
 * 
 * ALT_NODISCARD bool CloneFrom(const T& rhs) noexcept {
 *     // implementation
 * }
 * 
 * // possible example of 'CloneConstructFrom(...)' (on fail the object must be in initial state - not initialized)
 * ALT_NODISCARD bool CloneConstructFrom(const T& rhs) noexcept {
 * 
 *     iferr_scope_on_err {
 * 
 *         Memory::DestroyObject(*this);
 *     };
 * 
 *     Memory::ConstructDefaultObject<T>(this);
 *     CloneFrom(rhs) iferr_return false;
 * 
 *     return true;
 * }
 * 
 * // possible example of 'Clone()'
 * ALT_NODISCARD Optional<T> Clone() const noexcept {
 *     iferr_scope;
 * 
 *     MaybeUninit<T> valueToConstruct;
 *     valueToConstruct.GetPtr()->CloneConstructFrom(*this) iferr_return Nullopt;
 * 
 *     defer {
 *         valueToConstruct.Destroy();
 *     };
 * 
 *     return std::move(valueToConstruct).GetVal();
 * }
 */
template <typename T, typename = void>
struct __has_clonable_method_clone_from__
    : std::false_type
{};

template <typename T>
struct __has_clonable_method_clone_from__<T, std::void_t<std::is_same<decltype(std::declval<T&>().CloneFrom(std::declval<const T&>())), bool>>>
    : std::true_type
{};

template <typename T, typename = void>
struct __has_clonable_method_clone_construct_from__
    : std::false_type
{};

template <typename T>
struct __has_clonable_method_clone_construct_from__<T, std::void_t<std::is_same<decltype(std::declval<T&>().CloneConstructFrom(std::declval<const T&>())), bool>>>
    : std::true_type
{};

template <typename T, typename = void>
struct __has_clonable_method_clone__
    : std::false_type
{};

template <typename T>
struct __has_clonable_method_clone__<T, std::void_t<std::is_same<decltype(std::declval<const T&>().Clone()), Optional<T>>>>
    : std::true_type
{};

template <typename T>
using is_clonable_t =
    std::conditional_t<
        __has_clonable_method_clone_from__<clean_t<T>>::value &&
        __has_clonable_method_clone_construct_from__<clean_t<T>>::value &&
        __has_clonable_method_clone__<clean_t<T>>::value,

        std::true_type,
        std::false_type
    >;

template <typename T>
constexpr static bool is_clonable_v = is_clonable_t<T>::value;

template <typename T, typename U, typename U_>
struct __is_only_clonable_to_from__
    : std::conjunction<

        is_clonable_t<U>,

        /*
         * We can't use 'is_same' check, because 'template' parametrization can differ,
         * so assume that 'clone' possible for the same type combinations as 'move'.
         */
        //std::is_same<U, T>,
        std::is_convertible<U, T>,
        std::is_constructible<T, U>,

        /*
         * For move behavior we want to use just simple assignment
         */
        std::negation<std::is_rvalue_reference<U_>>
    >
{};

template <typename T, typename U>
using is_only_clonable_to_from_t = __is_only_clonable_to_from__<clean_t<std::remove_reference_t<T>>, clean_t<std::remove_reference_t<U>>, U>;

template <typename T, typename U>
constexpr static bool is_only_clonable_to_from_v = is_only_clonable_to_from_t<T, U>::value;

template <typename T, typename U>
struct is_explicitly_convertible {

    constexpr static bool value = std::is_constructible_v<U, T> && !std::is_convertible_v<T, U>;
};

template <typename U, typename T>
constexpr static bool is_explicitly_convertible_v = is_explicitly_convertible<U, T>::value;

template <typename T, typename U>
struct __preseprve_qualifiers__ {

    using type = U;
};

template <typename T, typename U>
struct __preseprve_qualifiers__<const T, U> {

    using type = const U;
};

template <typename T, typename U>
struct __preseprve_qualifiers__<volatile T, U> {

    using type = volatile U;
};

template <typename T, typename U>
struct __preseprve_qualifiers__<const volatile T, U> {

    using type = const volatile U;
};

template <typename T, typename U>
using preserve_qualifiers_t = typename __preseprve_qualifiers__<T, U>::type;

template <typename T>
struct __is_alt_ref__
    : std::false_type
{};

template <typename T>
using is_alt_ref_t = __is_alt_ref__<clean_t<T>>;

template <typename T>
constexpr static bool is_alt_ref_v = is_alt_ref_t<T>::value;

template <typename T>
struct __is_smart_pointer__
    : std::false_type
{};

template <typename T>
using is_smart_pointer_t = __is_smart_pointer__<clean_t<T>>;

template <typename T>
constexpr static bool is_smart_pointer_v = is_smart_pointer_t<T>::value;

template <typename T>
struct __is_pair__
    : std::false_type
{};

template <typename T>
using is_pair_t = __is_pair__<clean_t<T>>;

template <typename T>
constexpr static bool is_pair_v = is_pair_t<T>::value;

template <typename T>
struct __is_optional__
    : std::false_type
{};

template <typename T>
using is_optional_t = __is_optional__<clean_t<T>>;

template <typename T>
constexpr static bool is_optional_v = is_optional_t<T>::value;

template <typename T>
struct __is_result__
    : std::false_type
{};

template <typename T>
using is_result_t = __is_result__<clean_t<T>>;

template <typename T>
constexpr static bool is_result_v = is_result_t<T>::value;

template <typename T>
struct __is_union__
    : std::false_type
{};

template <typename T>
using is_union_t = __is_union__<clean_t<T>>;

template <typename T>
constexpr static bool is_union_v = is_union_t<T>::value;

template <typename T>
struct __is_range__
    : std::false_type
{};

template <typename T>
using is_range_t = __is_range__<clean_t<T>>;

template <typename T>
constexpr static bool is_range_v = is_range_t<T>::value;

template <typename T>
struct __is_string__
    : std::false_type
{};

template <typename T>
using is_string_t = __is_string__<clean_t<T>>;

template <typename T>
constexpr static bool is_string_v = is_string_t<T>::value;

} // namespace TypeTraits

template <typename T, usize N>
ALT_FORCE_INLINE constexpr usize ArraySize(T(&)[N]) noexcept {
    return N;
}

template <typename T, typename U = T>
ALT_FORCE_INLINE constexpr T Exchange(T& x, U&& y)
    noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_assignable_v<T&, U>)
{
    T old = std::move(x);
    x = std::forward<U>(y);
    return old;
}

template <typename T>
ALT_FORCE_INLINE constexpr void Swap(T& x, T& y)
    noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>)
{
    T tmp = std::move(x);
    x = std::move(y);
    y = std::move(tmp);
}

} // namespace Alt

#endif // __ALT_STD_TYPE_TRAITS__