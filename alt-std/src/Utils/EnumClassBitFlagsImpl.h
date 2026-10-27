#pragma once

#ifndef __ALT_ENUM_CLASS_BIT_FLAGS_IMPL__
#define __ALT_ENUM_CLASS_BIT_FLAGS_IMPL__

#include "../TypeTraits.h"

/**
 * @brief Defines and implements bitwise operators for a specific enum class.
 * @details This macro pair allows an `enum class` to behave like a bitmask by 
 * injecting overloaded bitwise operators (`|`, `&`, `^`, `~`, `|=`, `&=`, `^=`) 
 * and logical negation (`!`) into the scope of the enum class.
 * 
 * @example
 * enum class Flags {
 *     F1   = 1 << 0,
 *     F2   = 1 << 1,
 *     F3   = 1 << 2,
 *     F4   = 1 << 3,
 *     F5   = 1 << 4,
 * };
 * ALT_ENUM_CLASS_BIT_FLAGS_IMPL(Flags);
 * 
 * Flags f = Flags::F1 | Flags::F2;
 * assert(Alt::BitFlag::IsSetEnumFlags(f, Flags::F1));
 * 
 * f |= Flags::F4;
 * assert(Alt::BitFlag::IsSetAnyEnumFlags(f, Flags::F4 | Flags::F5));
 * 
 * if (Alt::IsSetEnumFlags(flags, Flags::F4)) {
 * }
 * 
 * assert(Alt::BitFlag::IsSetAnyEnumFlags(f);
 */
namespace Alt {
namespace BitFlag {

template <typename Enumclass>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsSetAnyEnumFlags(Enumclass lhs) noexcept {
    return static_cast<std::underlying_type_t<Enumclass>>(lhs) != 0;
}

template <typename Enumclass>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsSetAnyEnumFlags(Enumclass lhs, Enumclass rhs) noexcept {
    return static_cast<std::underlying_type_t<Enumclass>>(lhs & rhs) != 0;
}

template <typename Enumclass>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsSetEnumFlags(Enumclass lhs, Enumclass rhs) noexcept {
    return (lhs & rhs) == rhs;
}

} // namespace BitFlag
} // namespace Alt

#define ALT_ENUM_CLASS_BIT_FLAGS_IN_CLASS_DECL(Enumclass_) \
    friend constexpr Enumclass_& operator|=(Enumclass_& lhs, Enumclass_ rhs) noexcept; \
    friend constexpr Enumclass_& operator&=(Enumclass_& lhs, Enumclass_ rhs) noexcept; \
    friend constexpr Enumclass_& operator^=(Enumclass_& lhs, Enumclass_ rhs) noexcept; \
    friend constexpr Enumclass_  operator| (Enumclass_ lhs, Enumclass_ rhs) noexcept;  \
    friend constexpr Enumclass_  operator& (Enumclass_ lhs, Enumclass_ rhs) noexcept;  \
    friend constexpr Enumclass_  operator^ (Enumclass_ lhs, Enumclass_ rhs) noexcept;  \
    friend constexpr Enumclass_  operator~ (Enumclass_ e) noexcept;                   \
    friend constexpr bool        operator! (Enumclass_ e) noexcept;

#ifdef ALT_COMPILER_CLANG
#define ALT_ENUM_CLASS_BIT_FLAGS_IMPL(Enumclass_) \
\
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Wunused-function\"") \
\
    ALT_FORCE_INLINE constexpr Enumclass_& operator|=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) | static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    ALT_FORCE_INLINE constexpr Enumclass_& operator&=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) & static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    ALT_FORCE_INLINE constexpr Enumclass_& operator^=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) ^ static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    ALT_FORCE_INLINE constexpr Enumclass_  operator| (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) | static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    ALT_FORCE_INLINE constexpr Enumclass_  operator& (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) & static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    ALT_FORCE_INLINE constexpr Enumclass_  operator^ (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) ^ static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    ALT_FORCE_INLINE constexpr Enumclass_  operator~ (Enumclass_ e) noexcept                    { return static_cast<Enumclass_>(~static_cast<std::underlying_type_t<Enumclass_>>(e)); }                                                               \
    ALT_FORCE_INLINE constexpr bool        operator! (Enumclass_ e) noexcept                    { return !static_cast<std::underlying_type_t<Enumclass_>>(e); }                                                                                        \
_Pragma("clang diagnostic pop")
#else

#define ALT_ENUM_CLASS_BIT_FLAGS_IMPL(Enumclass_) \
    ALT_FORCE_INLINE constexpr Enumclass_& operator|=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) | static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    ALT_FORCE_INLINE constexpr Enumclass_& operator&=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) & static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    ALT_FORCE_INLINE constexpr Enumclass_& operator^=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) ^ static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    ALT_FORCE_INLINE constexpr Enumclass_  operator| (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) | static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    ALT_FORCE_INLINE constexpr Enumclass_  operator& (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) & static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    ALT_FORCE_INLINE constexpr Enumclass_  operator^ (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) ^ static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    ALT_FORCE_INLINE constexpr Enumclass_  operator~ (Enumclass_ e) noexcept                    { return static_cast<Enumclass_>(~static_cast<std::underlying_type_t<Enumclass_>>(e)); }                                                               \
    ALT_FORCE_INLINE constexpr bool        operator! (Enumclass_ e) noexcept                    { return !static_cast<std::underlying_type_t<Enumclass_>>(e); }
#endif // ALT_COMPILER_CLANG

#ifdef ALT_COMPILER_CLANG
#define ALT_ENUM_CLASS_BIT_FLAGS_WITH_TEMPLATE_IMPL(Template_, Enumclass_) \
\
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Wunused-function\"") \
\
    Template_ ALT_FORCE_INLINE constexpr Enumclass_& operator|=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) | static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_& operator&=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) & static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_& operator^=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) ^ static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_  operator| (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) | static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_  operator& (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) & static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_  operator^ (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) ^ static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_  operator~ (Enumclass_ e) noexcept                                                { return static_cast<Enumclass_>(~static_cast<std::underlying_type_t<Enumclass_>>(e)); }                                                                  \
    Template_ ALT_FORCE_INLINE constexpr bool       operator! (Enumclass_ e) noexcept                                                                             { return !static_cast<std::underlying_type_t<Enumclass_>>(e); } \
_Pragma("clang diagnostic pop")
#else

#define ALT_ENUM_CLASS_BIT_FLAGS_WITH_TEMPLATE_IMPL(Template_, Enumclass_) \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_& operator|=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) | static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_& operator&=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) & static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_& operator^=(Enumclass_& lhs, Enumclass_ rhs) noexcept { return lhs = static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) ^ static_cast<std::underlying_type_t<Enumclass_>>(rhs)); } \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_  operator| (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) | static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_  operator& (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) & static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_  operator^ (Enumclass_ lhs, Enumclass_ rhs) noexcept  { return static_cast<Enumclass_>(static_cast<std::underlying_type_t<Enumclass_>>(lhs) ^ static_cast<std::underlying_type_t<Enumclass_>>(rhs)); }       \
    Template_ ALT_FORCE_INLINE constexpr Enumclass_  operator~ (Enumclass_ e) noexcept                                                { return static_cast<Enumclass_>(~static_cast<std::underlying_type_t<Enumclass_>>(e)); }                                                                  \
    Template_ ALT_FORCE_INLINE constexpr bool       operator! (Enumclass_ e) noexcept                                                                             { return !static_cast<std::underlying_type_t<Enumclass_>>(e); }
#endif // ALT_COMPILER_CLANG

#endif // __ALT_ENUM_CLASS_BIT_FLAGS_IMPL__