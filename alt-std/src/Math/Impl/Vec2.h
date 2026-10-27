#pragma once

#ifndef __ALT_STD_MATH_VEC2__
#define __ALT_STD_MATH_VEC2__

#include "../../String/Impl/FormatPreInclude.h"

#include "MathPreInclude.h"

#ifdef ALT_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4201 /* nonstandard extension used: nameless struct/union */)
#pragma warning(disable: 4458 /* declaration of '...' hides class member */)
#endif // ALT_COMPILER_MSVC

namespace Alt {
namespace Math {

template <typename T>
class Vec2 {
public:

    using Type = T;

    ALT_FORCE_INLINE Vec2() noexcept
        : vec{ static_cast<T>(0), static_cast<T>(0) }
    {}

    ALT_FORCE_INLINE explicit Vec2(const T v) noexcept
        : vec{ v, v }
    {}

    ALT_FORCE_INLINE explicit Vec2(const T v0, const T v1) noexcept
        : vec{ v0, v1 }
    {}

    ALT_NODISCARD ALT_FORCE_INLINE T Dot(const Vec2& vec2) const noexcept {
        return x * vec2.x + y * vec2.y;
    }

    ALT_NODISCARD ALT_FORCE_INLINE T Length() const noexcept {
        return Sqrt(Dot(*this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE T LengthSq() const noexcept {
        return Dot(*this);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsUnit() const noexcept {
        return Math::IsEqFuzzy(Length(), static_cast<T>(1.0));
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEqFuzzy(const Vec2& vec2, const T eps = Num::Desc<T>::Eps()) const noexcept {

        return
            Math::IsEqFuzzy(vec[0], vec2[0], eps) &&
            Math::IsEqFuzzy(vec[1], vec2[1], eps);
    }

    ALT_NODISCARD ALT_FORCE_INLINE T Cross(const Vec2& vec2) const noexcept {
        return x * vec2.y - y * vec2.x;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Normalize() const noexcept {
        return *this * Rsqrt(LengthSq());
    }

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<U> Cast() const noexcept {
        return Vec2<U>(static_cast<U>(x), static_cast<U>(y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE T MinFetch() const noexcept {
        return Num::Min(x, y);
    }

    ALT_NODISCARD ALT_FORCE_INLINE T MaxFetch() const noexcept {
        return Num::Max(x, y);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Min(const T vecx, const T vecy) const noexcept {
        return Vec2(Num::Min(x, vecx), Num::Min(y, vecy));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Min(const Vec2& vec) const noexcept {
        return Vec2(Num::Min(x, vec.x), Num::Min(y, vec.y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Max(const T vecx, const T vecy) const noexcept {
        return Vec2(Num::Max(x, vecx), Num::Max(y, vecy));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Max(const Vec2& vec) const noexcept {
        return Vec2(Num::Max(x, vec.x), Num::Max(y, vec.y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Clamp(const Vec2& min, const Vec2& max) const noexcept {
        return Min(max).Max(min);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Abs() const noexcept {
        return Vec2(Num::Abs(x), Num::Abs(y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Floor() const noexcept {
        return Vec2(Math::Floor(x), Math::Floor(y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Ceil() const noexcept {
        return Vec2(Math::Ceil(x), Math::Ceil(y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Trunc() const noexcept {
        return Vec2(Math::Trunc(x), Math::Trunc(y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Frac() const noexcept {
        return Vec2(Math::Frac(x), Math::Frac(y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Round() const noexcept {
        return Vec2(Math::Round(x), Math::Round(y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 xy() const noexcept {
        return *this;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 yx() const noexcept {
        return Vec2(y, x);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Lerp(const Vec2& dst, const T t) const noexcept {
        return *this * (static_cast<T>(1.0) - t) + dst * t;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 RotateZ(const T angleRad) const noexcept {

        const T cos = Cos(angleRad);
        const T sin = Sin(angleRad);

        return Vec2{
            x * cos - y * sin, x * sin + y * cos,
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 PerpNormalizedVec2() const noexcept {

        return Vec2(-y, x).Normalize();
    }

    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        return vec[i];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize i) const noexcept {
        return vec[i];
    }

    ALT_FORCE_INLINE Vec2 operator*(const T s) const noexcept {
        return Vec2(x * s, y * s);
    }

    ALT_FORCE_INLINE Vec2 operator*(const Vec2& rhs) const noexcept {
        return Vec2(x * rhs.x, y * rhs.y);
    }

    ALT_FORCE_INLINE Vec2& operator*=(const T s) noexcept {

        *this = Vec2(x * s, y * s);
        return *this;
    }

    ALT_FORCE_INLINE Vec2& operator*=(const Vec2& rhs) noexcept {

        *this = Vec2(x * rhs.x, y * rhs.y);
        return *this;
    }

    ALT_FORCE_INLINE Vec2 operator/(const T s) const noexcept {

        const T inv = SafeInv(s);
        return Vec2(x * inv, y * inv);
    }

    ALT_FORCE_INLINE Vec2 operator/(const Vec2& rhs) const noexcept {
        return Vec2(x * SafeInv(rhs.x), y * SafeInv(rhs.y));
    }

    ALT_FORCE_INLINE Vec2& operator/=(const T s) noexcept {

        const T inv = SafeInv(s);
        *this = Vec2(x * inv, y * inv);

        return *this;
    }

    ALT_FORCE_INLINE Vec2& operator/=(const Vec2& rhs) noexcept {

        *this = Vec2(x * SafeInv(rhs.x), y * SafeInv(rhs.y));
        return *this;
    }

    ALT_FORCE_INLINE Vec2 operator+(const T s) const noexcept {
        return Vec2(x + s, y + s);
    }

    ALT_FORCE_INLINE Vec2 operator+(const Vec2& rhs) const noexcept {
        return Vec2(x + rhs.x, y + rhs.y);
    }

    ALT_FORCE_INLINE Vec2& operator+=(const T s) noexcept {

        *this = Vec2(x + s, y + s);
        return *this;
    }

    ALT_FORCE_INLINE Vec2& operator+=(const Vec2& rhs) noexcept {

        *this = Vec2(x + rhs.x, y + rhs.y);
        return *this;
    }

    ALT_FORCE_INLINE Vec2 operator-(const T s) const noexcept {
        return Vec2(x - s, y - s);
    }

    ALT_FORCE_INLINE Vec2 operator-(const Vec2& rhs) const noexcept {
        return Vec2(x - rhs.x, y - rhs.y);
    }

    ALT_FORCE_INLINE Vec2& operator-=(const T s) noexcept {

        *this = Vec2(x - s, y - s);
        return *this;
    }

    ALT_FORCE_INLINE Vec2& operator-=(const Vec2& rhs) noexcept {

        *this = Vec2(x - rhs.x, y - rhs.y);
        return *this;
    }

    ALT_FORCE_INLINE Vec2 operator-() const noexcept {
        return Vec2(-x, -y);
    }

    ALT_FORCE_INLINE friend Vec2 operator*(const T s, const Vec2& vec2) noexcept {
        return Vec2(s * vec2.x, s * vec2.y);
    }

    ALT_FORCE_INLINE friend Vec2 operator+(const T s, const Vec2& vec2) noexcept {
        return Vec2(s + vec2.x, s + vec2.y);
    }

    ALT_FORCE_INLINE friend Vec2 operator-(const T s, const Vec2& vec2) noexcept {
        return Vec2(s - vec2.x, s - vec2.y);
    }

public:

    union {

        T vec[2];

        struct { T x, y; };
    };
};

#define __ALT_STD_INTERNAL_MATH_IMPL_VEC2_UNSIGNED__(type) \
template <> \
class Vec2<type> { \
public: \
\
    using Type = type; \
\
    ALT_FORCE_INLINE Vec2() noexcept \
        : vec{ static_cast<type>(0), static_cast<type>(0) } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec2(const type v) noexcept \
        : vec{ v, v } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec2(const type v0, const type v1) noexcept \
        : vec{ v0, v1 } \
    {} \
\
    template <typename U> \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<U> Cast() const noexcept { \
        return Vec2<U>(static_cast<U>(x), static_cast<U>(y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MinFetch() const noexcept { \
        return Num::Min(x, y); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MaxFetch() const noexcept { \
        return Num::Max(x, y); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Min(const type vecx, const type vecy) const noexcept { \
        return Vec2(Num::Min(x, vecx), Num::Min(y, vecy)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Min(const Vec2& vec) const noexcept { \
        return Vec2(Num::Min(x, vec.x), Num::Min(y, vec.y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Max(const type vecx, const type vecy) const noexcept { \
        return Vec2(Num::Max(x, vecx), Num::Max(y, vecy)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Max(const Vec2& vec) const noexcept { \
        return Vec2(Num::Max(x, vec.x), Num::Max(y, vec.y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Clamp(const Vec2& min, const Vec2& max) const noexcept { \
        return Min(max).Max(min); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 AddWrap(const type s) const noexcept { \
        return Vec2(Num::AddWrap(x, s), Num::AddWrap(y, s)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 AddWrap(const Vec2& rhs) const noexcept { \
        return Vec2(Num::AddWrap(x, rhs.x), Num::AddWrap(y, rhs.y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 SubWrap(const type s) const noexcept { \
        return Vec2(Num::SubWrap(x, s), Num::SubWrap(y, s)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 SubWrap(const Vec2& rhs) const noexcept { \
        return Vec2(Num::SubWrap(x, rhs.x), Num::SubWrap(y, rhs.y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 MulWrap(const type s) const noexcept { \
        return Vec2(Num::MulWrap(x, s), Num::MulWrap(y, s)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 MulWrap(const Vec2& rhs) const noexcept { \
        return Vec2(Num::MulWrap(x, rhs.x), Num::MulWrap(y, rhs.y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 xy() const noexcept { \
        return *this; \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 yx() const noexcept { \
        return Vec2(y, x); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type& operator[](const usize i) noexcept { \
        return vec[i]; \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE const type& operator[](const usize i) const noexcept { \
        return vec[i]; \
    } \
\
    ALT_FORCE_INLINE Vec2 operator*(const type s) const noexcept { \
        return Vec2(x * s, y * s); \
    } \
\
    ALT_FORCE_INLINE Vec2 operator*(const Vec2& rhs) const noexcept { \
        return Vec2(x * rhs.x, y * rhs.y); \
    } \
\
    ALT_FORCE_INLINE Vec2& operator*=(const Vec2& rhs) noexcept { \
\
        *this = Vec2(x * rhs.x, y * rhs.y); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2& operator*=(const type s) noexcept { \
\
        *this = Vec2(x * s, y * s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2 operator/(const type s) const noexcept { \
        return Vec2(x / s, y / s); \
    } \
\
    ALT_FORCE_INLINE Vec2 operator/(const Vec2& rhs) const noexcept { \
        return Vec2(x / rhs.x, y / rhs.y); \
    } \
\
    ALT_FORCE_INLINE Vec2& operator/=(const Vec2& rhs) noexcept { \
\
        *this = Vec2(x / rhs.x, y / rhs.y); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2& operator/=(const type s) noexcept { \
\
        *this = Vec2(x / s, y / s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2 operator+(const Vec2& rhs) const noexcept { \
        return Vec2(x + rhs.x, y + rhs.y); \
    } \
\
    ALT_FORCE_INLINE Vec2 operator+(const type s) const noexcept { \
        return Vec2(x + s, y + s); \
    } \
\
    ALT_FORCE_INLINE Vec2& operator+=(const Vec2& rhs) noexcept { \
\
        *this = Vec2(x + rhs.x, y + rhs.y); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2& operator+=(const type s) noexcept { \
\
        *this = Vec2(x + s, y + s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2 operator-(const Vec2& rhs) const noexcept { \
        return Vec2(x - rhs.x, y - rhs.y); \
    } \
\
    ALT_FORCE_INLINE Vec2 operator-(const type s) const noexcept { \
        return Vec2(x - s, y - s); \
    } \
\
    ALT_FORCE_INLINE Vec2& operator-=(const Vec2& rhs) noexcept { \
\
        *this = Vec2(x - rhs.x, y - rhs.y); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2& operator-=(const type s) noexcept { \
\
        *this = Vec2(x - s, y - s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE friend Vec2 operator*(const type s, const Vec2& vec2) noexcept { \
        return Vec2(s * vec2.x, s * vec2.y); \
    } \
\
    ALT_FORCE_INLINE friend Vec2 operator+(const type s, const Vec2& vec2) noexcept { \
        return Vec2(s + vec2.x, s + vec2.y); \
    } \
\
    ALT_FORCE_INLINE friend Vec2 operator-(const type s, const Vec2& vec2) noexcept { \
        return Vec2(s - vec2.x, s - vec2.y); \
    } \
\
public: \
\
    union { \
\
        type vec[2]; \
\
        struct { type x, y; }; \
    }; \
};

#define __ALT_STD_INTERNAL_MATH_IMPL_VEC2_SIGNED__(type) \
template <> \
class Vec2<type> { \
public: \
\
    using Type = type; \
\
    ALT_FORCE_INLINE Vec2() noexcept \
        : vec{ static_cast<type>(0), static_cast<type>(0) } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec2(const type v) noexcept \
        : vec{ v, v } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec2(const type v0, const type v1) noexcept \
        : vec{ v0, v1 } \
    {} \
\
    template <typename U> \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<U> Cast() const noexcept { \
        return Vec2<U>(static_cast<U>(x), static_cast<U>(y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MinFetch() const noexcept { \
        return Num::Min(x, y); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MaxFetch() const noexcept { \
        return Num::Max(x, y); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Min(const type vecx, const type vecy) const noexcept { \
        return Vec2(Num::Min(x, vecx), Num::Min(y, vecy)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Min(const Vec2& vec) const noexcept { \
        return Vec2(Num::Min(x, vec.x), Num::Min(y, vec.y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Max(const type vecx, const type vecy) const noexcept { \
        return Vec2(Num::Max(x, vecx), Num::Max(y, vecy)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Max(const Vec2& vec) const noexcept { \
        return Vec2(Num::Max(x, vec.x), Num::Max(y, vec.y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Clamp(const Vec2& min, const Vec2& max) const noexcept { \
        return Min(max).Max(min); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 Abs() const noexcept {  \
        return Vec2(Num::Abs(x), Num::Abs(y)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 xy() const noexcept { \
        return *this; \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec2 yx() const noexcept { \
        return Vec2(y, x); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type& operator[](const usize i) noexcept { \
        return vec[i]; \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE const type& operator[](const usize i) const noexcept { \
        return vec[i]; \
    } \
\
    ALT_FORCE_INLINE Vec2 operator*(const type s) const noexcept { \
        return Vec2(x * s, y * s); \
    } \
\
    ALT_FORCE_INLINE Vec2 operator*(const Vec2& rhs) const noexcept { \
        return Vec2(x * rhs.x, y * rhs.y); \
    } \
\
    ALT_FORCE_INLINE Vec2& operator*=(const Vec2& rhs) noexcept { \
\
        *this = Vec2(x * rhs.x, y * rhs.y); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2& operator*=(const type s) noexcept { \
\
        *this = Vec2(x * s, y * s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2 operator/(const type s) const noexcept { \
        return Vec2(x / s, y / s); \
    } \
\
    ALT_FORCE_INLINE Vec2 operator/(const Vec2& rhs) const noexcept { \
        return Vec2(x / rhs.x, y / rhs.y); \
    } \
\
    ALT_FORCE_INLINE Vec2& operator/=(const Vec2& rhs) noexcept { \
\
        *this = Vec2(x / rhs.x, y / rhs.y); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2& operator/=(const type s) noexcept { \
\
        *this = Vec2(x / s, y / s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2 operator+(const Vec2& rhs) const noexcept { \
        return Vec2(x + rhs.x, y + rhs.y); \
    } \
\
    ALT_FORCE_INLINE Vec2 operator+(const type s) const noexcept { \
        return Vec2(x + s, y + s); \
    } \
\
    ALT_FORCE_INLINE Vec2& operator+=(const Vec2& rhs) noexcept { \
\
        *this = Vec2(x + rhs.x, y + rhs.y); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2& operator+=(const type s) noexcept { \
\
        *this = Vec2(x + s, y + s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2 operator-(const Vec2& rhs) const noexcept { \
        return Vec2(x - rhs.x, y - rhs.y); \
    } \
\
    ALT_FORCE_INLINE Vec2 operator-(const type s) const noexcept { \
        return Vec2(x - s, y - s); \
    } \
\
    ALT_FORCE_INLINE Vec2& operator-=(const Vec2& rhs) noexcept { \
\
        *this = Vec2(x - rhs.x, y - rhs.y); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2& operator-=(const type s) noexcept { \
\
        *this = Vec2(x - s, y - s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec2 operator-() const noexcept { \
        return Vec2(-x, -y); \
    } \
\
    ALT_FORCE_INLINE friend Vec2 operator*(const type s, const Vec2& vec2) noexcept { \
        return Vec2(s * vec2.x, s * vec2.y); \
    } \
\
    ALT_FORCE_INLINE friend Vec2 operator+(const type s, const Vec2& vec2) noexcept { \
        return Vec2(s + vec2.x, s + vec2.y); \
    } \
\
    ALT_FORCE_INLINE friend Vec2 operator-(const type s, const Vec2& vec2) noexcept { \
        return Vec2(s - vec2.x, s - vec2.y); \
    } \
\
public: \
\
    union { \
\
        type vec[2]; \
\
        struct { type x, y; }; \
    }; \
};

__ALT_STD_INTERNAL_MATH_IMPL_VEC2_UNSIGNED__(u8)
__ALT_STD_INTERNAL_MATH_IMPL_VEC2_UNSIGNED__(u16)
__ALT_STD_INTERNAL_MATH_IMPL_VEC2_UNSIGNED__(u32)
__ALT_STD_INTERNAL_MATH_IMPL_VEC2_UNSIGNED__(u64)

__ALT_STD_INTERNAL_MATH_IMPL_VEC2_SIGNED__(s8)
__ALT_STD_INTERNAL_MATH_IMPL_VEC2_SIGNED__(s16)
__ALT_STD_INTERNAL_MATH_IMPL_VEC2_SIGNED__(s32)
__ALT_STD_INTERNAL_MATH_IMPL_VEC2_SIGNED__(s64)

#undef __ALT_STD_INTERNAL_MATH_IMPL_VEC2_UNSIGNED__
#undef __ALT_STD_INTERNAL_MATH_IMPL_VEC2_SIGNED__

using Vec2u8 = Vec2<u8>;
using Vec2u16 = Vec2<u16>;
using Vec2u32 = Vec2<u32>;
using Vec2u64 = Vec2<u64>;
using Vec2usize = Vec2<usize>;

using Vec2s8 = Vec2<s8>;
using Vec2s16 = Vec2<s16>;
using Vec2s32 = Vec2<s32>;
using Vec2s64 = Vec2<s64>;
using Vec2ssize = Vec2<ssize>;

using Vec2f32 = Vec2<f32>;
using Vec2f64 = Vec2<f64>;

} // namespace Math

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<Math::Vec2<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Format specialization for Vec2<T>.
 */
template <typename T>
class Format<Math::Vec2<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Math::Vec2<T>& val) noexcept {
        iferr_scope;

        using CleanU = _FormatDetails::clean_t<U>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "noinline"_s) {

            EstimateExactSizeResult result{};
            result.size = EstimateSizeTypeName(CleanU{}) + 1;

            estimator.PushIndent();
            {
                result.size += estimator.GetIndentEstimatedSize();
                EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.x) iferr_return Nullopt;
                result.size += estimated.size + 2;

                result.size += estimator.GetIndentEstimatedSize();
                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.y) iferr_return Nullopt;
                result.size += estimated.size + 1;
            }
            estimator.PopIndent();

            result.size += estimator.GetIndentEstimatedSize() + 1;

            return result;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            EstimateExactSizeResult result{};
            result.size = EstimateSizeTypeName(CleanU{}) + 1;

            EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.x) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.y) iferr_return Nullopt;
            result.size += estimated.size;

            result.size += 2;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Math::Vec2<T>& val) noexcept {
        iferr_scope;

        using CleanT = _FormatDetails::clean_t<T>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "noinline"_s) {

            WriterAppendTypeName(writer, CleanT{}) iferr_return false;
            writer.Append('\n') iferr_return false;

            writer.PushIndent();
            {
                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val.x) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append('\n') iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val.y) iferr_return false;
                writer.Append('\n') iferr_return false;
            }
            writer.PopIndent();

            writer.AppendIndent() iferr_return false;
            writer.Append('}') iferr_return false;

            return true;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            WriterAppendTypeName(writer, CleanT{}) iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.x) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.y) iferr_return false;

            writer.Append(' ') iferr_return false;
            writer.Append('}') iferr_return false;

            return true;
        }

        return false;
    }

private:

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(U) noexcept {
        return 5;
    }

    template <typename U, typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, U) noexcept {
        return writer.Append("Vec2{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u8) noexcept {
        return 7;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u8) noexcept {
        return writer.Append("Vec2u8{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u16) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u16) noexcept {
        return writer.Append("Vec2u16{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u32) noexcept {
        return writer.Append("Vec2u32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u64) noexcept {
        return writer.Append("Vec2u64{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s8) noexcept {
        return 7;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s8) noexcept {
        return writer.Append("Vec2s8{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s16) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s16) noexcept {
        return writer.Append("Vec2s16{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s32) noexcept {
        return writer.Append("Vec2s32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s64) noexcept {
        return writer.Append("Vec2s64{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f32) noexcept {
        return writer.Append("Vec2f32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f64) noexcept {
        return writer.Append("Vec2f64{");
    }
};

} // namespace Alt

#ifdef ALT_COMPILER_MSVC
#pragma warning(pop)
#endif // ALT_COMPILER_MSVC

#endif // __ALT_STD_MATH_VEC2__