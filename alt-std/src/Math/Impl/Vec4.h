#pragma once

#ifndef __ALT_STD_MATH_VEC4__
#define __ALT_STD_MATH_VEC4__

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
class Vec2;

template <typename T>
class Vec3;

template <typename T>
class Quat;

template <typename T>
class Vec4 {
public:

    using Type = T;

    ALT_FORCE_INLINE Vec4() noexcept
        : vec{ static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0) }
    {}

    ALT_FORCE_INLINE Vec4(const T val) noexcept
        : vec{ val, val, val, val }
    {}

    ALT_FORCE_INLINE explicit Vec4(const T v0, const T v1, const T v2, const T v3) noexcept
        : vec{ v0, v1, v2, v3 }
    {}

    ALT_FORCE_INLINE explicit Vec4(const Vec3<T>& vec3, T v4) noexcept;
    ALT_FORCE_INLINE explicit Vec4(const Quat<T>& quat) noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE T Dot(const Vec4& vec4) const noexcept {
        return x * vec4.x + y * vec4.y + z * vec4.z + w * vec4.w;
    }

    ALT_NODISCARD ALT_FORCE_INLINE T Dot3(const Vec4& vec4) const noexcept {
        return x * vec4.x + y * vec4.y + z * vec4.z;
    }

    ALT_NODISCARD ALT_FORCE_INLINE T Dot3(const Vec3<T>& vec3) const noexcept {
        return x * vec3.x + y * vec3.y + z * vec3.z;
    }

    ALT_NODISCARD ALT_FORCE_INLINE T Length() const noexcept {
        return Sqrt(Dot(*this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE T LengthSq() const noexcept {
        return Dot(*this);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsUnit() const noexcept {
        return Math::IsEqFuzzy(Length(), static_cast<T>(1));
    }

    ALT_NODISCARD ALT_FORCE_INLINE int CompareFuzzy(const Vec4& rhs, const T eps = Num::Desc<T>::Eps()) const noexcept {

        if (Math::IsEqFuzzy(vec[0], rhs[0], eps)) {

            if (Math::IsEqFuzzy(vec[1], rhs[1], eps)) {

                if (Math::IsEqFuzzy(vec[2], rhs[2], eps)) {

                    if (Math::IsEqFuzzy(vec[3], rhs[3], eps)) {

                        return 0;
                    }

                    return vec[3] < rhs[3] ? -1 : 1;
                }

                return vec[2] < rhs[2] ? -1 : 1;
            }

            return vec[1] < rhs[1] ? -1 : 1;
        }

        return vec[0] < rhs[0] ? -1 : 1;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEqFuzzy(const Vec4& rhs, const T eps = Num::Desc<T>::Eps()) const noexcept {

        return
            Math::IsEqFuzzy(vec[0], rhs[0], eps) &&
            Math::IsEqFuzzy(vec[1], rhs[1], eps) &&
            Math::IsEqFuzzy(vec[2], rhs[2], eps) &&
            Math::IsEqFuzzy(vec[3], rhs[3], eps);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Normalize() const noexcept {
        return *this * Rsqrt(LengthSq());
    }

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE Vec4<U> Cast() const noexcept {
        return Vec4<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(w));
    }

    ALT_NODISCARD ALT_FORCE_INLINE T MinFetch() const noexcept {
        return Num::Min(x, y, z, w);
    }

    ALT_NODISCARD ALT_FORCE_INLINE T MaxFetch() const noexcept {
        return Num::Max(x, y, z, w);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Min(const T vecx, const T vecy, const T vecz, const T vecw) const noexcept {
        return Vec4(Num::Min(x, vecx), Num::Min(y, vecy), Num::Min(z, vecz), Num::Min(w, vecw));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Min(const Vec4& vec) const noexcept {
        return Vec4(Num::Min(x, vec.x), Num::Min(y, vec.y), Num::Min(z, vec.z), Num::Min(w, vec.w));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Max(const T vecx, const T vecy, const T vecz, const T vecw) const noexcept {
        return Vec4(Num::Max(x, vecx), Num::Max(y, vecy), Num::Max(z, vecz), Num::Max(w, vecw));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Max(const Vec4& vec) const noexcept {
        return Vec4(Num::Max(x, vec.x), Num::Max(y, vec.y), Num::Max(z, vec.z), Num::Max(w, vec.w));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Clamp(const Vec4& min, const Vec4& max) const noexcept {
        return Min(max).Max(min);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Abs() const noexcept {
        return Vec4(Num::Abs(x), Num::Abs(y), Num::Abs(z), Num::Abs(w));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Floor() const noexcept {
        return Vec4(Math::Floor(x), Math::Floor(y), Math::Floor(z), Math::Floor(w));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Ceil() const noexcept {
        return Vec4(Math::Ceil(x), Math::Ceil(y), Math::Ceil(z), Math::Ceil(w));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Trunc() const noexcept {
        return Vec4(Math::Trunc(x), Math::Trunc(y), Math::Trunc(z), Math::Trunc(w));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Frac() const noexcept {
        return Vec4(Math::Frac(x), Math::Frac(y), Math::Frac(z), Math::Frac(w));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Round() const noexcept {
        return Vec4(Math::Round(x), Math::Round(y), Math::Round(z), Math::Round(w));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xyzw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xywz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xzyw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xzwy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xwzy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xwyz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yxzw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yxwz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yzxw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yzwx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 ywxz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 ywzx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zxyw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zxwy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zyxw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zywx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zwxy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zwyx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wxyz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wxzy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wyxz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wyzx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wzxy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wzyx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> xyz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> xzy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> yxz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> yzx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> zxy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> zyx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> xyw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> xwy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> yxw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> ywx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> wxy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> wyx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> xwz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> xzw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> wxz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> wzx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> zxw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> zwx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> wyz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> wzy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> ywz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> yzw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> zwy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> zyw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> xy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> xz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> yx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> yz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> zx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> zy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> xw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> wx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> wz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> zw() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> wy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> yw() const noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 RotateX(const T angleRad) const noexcept {

        const T cos = Cos(angleRad);
        const T sin = Sin(angleRad);

        return Vec4(x, y * cos - z * sin, y * sin + z * cos, w);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 RotateY(const T angleRad) const noexcept {

        const T cos = Cos(angleRad);
        const T sin = Sin(angleRad);

        return Vec4(x * cos + z * sin, y, -x * sin + z * cos, w);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4 RotateZ(const T angleRad) const noexcept {

        const T cos = Cos(angleRad);
        const T sin = Sin(angleRad);

        return Vec4(x * cos - y * sin, x * sin + y * cos, z, w);
    }

    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        return vec[i];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize i) const noexcept {
        return vec[i];
    }

    ALT_FORCE_INLINE Vec4 operator*(const T s) const noexcept {
        return Vec4(x * s, y * s, z * s, w * s);
    }

    ALT_FORCE_INLINE Vec4 operator*(const Vec4& rhs) const noexcept {
        return Vec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
    }

    ALT_FORCE_INLINE Vec4& operator*=(const T s) noexcept {

        *this = Vec4(x * s, y * s, z * s, w * s);
        return *this;
    }

    ALT_FORCE_INLINE Vec4& operator*=(const Vec4& rhs) noexcept {

        *this = Vec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
        return *this;
    }

    ALT_FORCE_INLINE Vec4 operator/(const T s) const noexcept {

        const T inv = SafeInv(s);
        return Vec4(x * inv, y * inv, z * inv, w * inv);
    }

    ALT_FORCE_INLINE Vec4 operator/(const Vec4& rhs) const noexcept {
        return Vec4(x * SafeInv(rhs.x), y * SafeInv(rhs.y), z * SafeInv(rhs.z), w * SafeInv(rhs.w));
    }

    ALT_FORCE_INLINE Vec4& operator/=(const T s) noexcept {

        const T inv = SafeInv(s);
        *this = Vec4(x * inv, y * inv, z * inv, w * inv);

        return *this;
    }

    ALT_FORCE_INLINE Vec4& operator/=(const Vec4& rhs) noexcept {

        *this = Vec4(x * SafeInv(rhs.x), y * SafeInv(rhs.y), z * SafeInv(rhs.z), w * SafeInv(rhs.w));
        return *this;
    }

    ALT_FORCE_INLINE Vec4 operator+(const T s) const noexcept {
        return Vec4(x + s, y + s, z + s, w + s);
    }

    ALT_FORCE_INLINE Vec4 operator+(const Vec4& rhs) const noexcept {
        return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    ALT_FORCE_INLINE Vec4& operator+=(const T s) noexcept {

        *this = Vec4(x + s, y + s, z + s, w + s);
        return *this;
    }

    ALT_FORCE_INLINE Vec4& operator+=(const Vec4& rhs) noexcept {

        *this = Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
        return *this;
    }

    ALT_FORCE_INLINE Vec4 operator-(const T s) const noexcept {
        return Vec4(x - s, y - s, z - s, w - s);
    }

    ALT_FORCE_INLINE Vec4 operator-(const Vec4& rhs) const noexcept {
        return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }

    ALT_FORCE_INLINE Vec4& operator-=(const T s) noexcept {

        *this = Vec4(x - s, y - s, z - s, w - s);
        return *this;
    }

    ALT_FORCE_INLINE Vec4& operator-=(const Vec4& rhs) noexcept {

        *this = Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
        return *this;
    }

    ALT_FORCE_INLINE Vec4 operator-() const noexcept {
        return Vec4(-x, -y, -z, -w);
    }

    ALT_FORCE_INLINE friend Vec4 operator*(const T s, const Vec4& vec4) noexcept {
        return Vec4(s * vec4.x, s * vec4.y, s * vec4.z, s * vec4.w);
    }

    ALT_FORCE_INLINE friend Vec4 operator+(const T s, const Vec4& vec4) noexcept {
        return Vec4(s + vec4.x, s + vec4.y, s + vec4.z, s + vec4.w);
    }

    ALT_FORCE_INLINE friend Vec4 operator-(const T s, const Vec4& vec4) noexcept {
        return Vec4(s - vec4.x, s - vec4.y, s - vec4.z, s - vec4.w);
    }

public:

    union {

        T vec[4];

        struct { T x, y, z, w; };
    };
};

#define __ALT_STD_INTERNAL_MATH_IMPL_VEC4_UNSIGNED__(type) \
template <> \
class Vec4<type> { \
public: \
\
    using Type = type; \
\
    ALT_FORCE_INLINE Vec4() noexcept \
        : vec{ static_cast<type>(0), static_cast<type>(0), static_cast<type>(0), static_cast<type>(0) } \
    {} \
\
    ALT_FORCE_INLINE Vec4(const type val) noexcept \
        : vec{ val, val, val, val } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec4(const type v0, const type v1, const type v2, const type v3) noexcept \
        : vec{ v0, v1, v2, v3 } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec4(const Vec3<type>& vec3, type v4) noexcept; \
\
    template <typename U> \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4<U> Cast() const noexcept { \
        return Vec4<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MinFetch() const noexcept { \
        return Num::Min(x, y, z, w); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MaxFetch() const noexcept { \
        return Num::Max(x, y, z, w); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Min(const type vecx, const type vecy, const type vecz, const type vecw) const noexcept { \
        return Vec4(Num::Min(x, vecx), Num::Min(y, vecy), Num::Min(z, vecz), Num::Min(w, vecw)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Min(const Vec4& vec) const noexcept { \
        return Vec4(Num::Min(x, vec.x), Num::Min(y, vec.y), Num::Min(z, vec.z), Num::Min(w, vec.w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Max(const type vecx, const type vecy, const type vecz, const type vecw) const noexcept { \
        return Vec4(Num::Max(x, vecx), Num::Max(y, vecy), Num::Max(z, vecz), Num::Max(w, vecw)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Max(const Vec4& vec) const noexcept { \
        return Vec4(Num::Max(x, vec.x), Num::Max(y, vec.y), Num::Max(z, vec.z), Num::Max(w, vec.w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Clamp(const Vec4& min, const Vec4& max) const noexcept { \
        return Min(max).Max(min); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 AddWrap(const type s) const noexcept { \
        return Vec4(Num::AddWrap(x, s), Num::AddWrap(y, s), Num::AddWrap(z, s), Num::AddWrap(w, s)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 AddWrap(const Vec4& rhs) const noexcept { \
        return Vec4(Num::AddWrap(x, rhs.x), Num::AddWrap(y, rhs.y), Num::AddWrap(z, rhs.z), Num::AddWrap(w, rhs.w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 SubWrap(const type s) const noexcept { \
        return Vec4(Num::SubWrap(x, s), Num::SubWrap(y, s), Num::SubWrap(z, s), Num::SubWrap(w, s)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 SubWrap(const Vec4& rhs) const noexcept { \
        return Vec4(Num::SubWrap(x, rhs.x), Num::SubWrap(y, rhs.y), Num::SubWrap(z, rhs.z), Num::SubWrap(w, rhs.w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 MulWrap(const type s) const noexcept { \
        return Vec4(Num::MulWrap(x, s), Num::MulWrap(y, s), Num::MulWrap(z, s), Num::MulWrap(w, s)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 MulWrap(const Vec4& rhs) const noexcept { \
        return Vec4(Num::MulWrap(x, rhs.x), Num::MulWrap(y, rhs.y), Num::MulWrap(z, rhs.z), Num::MulWrap(w, rhs.w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xyzw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xywz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xzyw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xzwy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xwzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xwyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yxzw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yxwz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yzxw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yzwx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 ywxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 ywzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zxyw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zxwy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zyxw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zywx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zwxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zwyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wxyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wxzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wyxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wyzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wzxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wzyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> yxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> yzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xyw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xwy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> yxw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> ywx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xwz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xzw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zxw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zwx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> ywz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> yzw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zwy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zyw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> wx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> wz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> wy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yw() const noexcept; \
\
    ALT_NODISCARD ALT_FORCE_INLINE type& operator[](const usize i) noexcept { \
        return vec[i]; \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE const type& operator[](const usize i) const noexcept { \
        return vec[i]; \
    } \
\
    ALT_FORCE_INLINE Vec4 operator*(const type s) const noexcept { \
        return Vec4(x * s, y * s, z * s, w * s); \
    } \
\
    ALT_FORCE_INLINE Vec4 operator*(const Vec4& rhs) const noexcept { \
        return Vec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); \
    } \
\
    ALT_FORCE_INLINE Vec4& operator*=(const type s) noexcept { \
\
        *this = Vec4(x * s, y * s, z * s, w * s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4& operator*=(const Vec4& rhs) noexcept { \
\
        *this = Vec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4 operator/(const type s) const noexcept { \
        return Vec4(x / s, y / s, z / s, w / s); \
    } \
\
    ALT_FORCE_INLINE Vec4 operator/(const Vec4& rhs) const noexcept { \
        return Vec4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); \
    } \
\
    ALT_FORCE_INLINE Vec4& operator/=(const type s) noexcept { \
\
        *this = Vec4(x / s, y / s, z / s, w / s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4& operator/=(const Vec4& rhs) noexcept { \
\
        *this = Vec4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4 operator+(const type s) const noexcept { \
        return Vec4(x + s, y + s, z + s, w + s); \
    } \
\
    ALT_FORCE_INLINE Vec4 operator+(const Vec4& rhs) const noexcept { \
        return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); \
    } \
\
    ALT_FORCE_INLINE Vec4& operator+=(const type s) noexcept { \
\
        *this = Vec4(x + s, y + s, z + s, w + s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4& operator+=(const Vec4& rhs) noexcept { \
\
        *this = Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4 operator-(const type s) const noexcept { \
        return Vec4(x - s, y - s, z - s, w - s); \
    } \
\
    ALT_FORCE_INLINE Vec4 operator-(const Vec4& rhs) const noexcept { \
        return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); \
    } \
\
    ALT_FORCE_INLINE Vec4& operator-=(const type s) noexcept { \
\
        *this = Vec4(x - s, y - s, z - s, w - s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4& operator-=(const Vec4& rhs) noexcept { \
\
        *this = Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE friend Vec4 operator*(const type s, const Vec4& vec4) noexcept { \
        return Vec4(s * vec4.x, s * vec4.y, s * vec4.z, s * vec4.w); \
    } \
\
    ALT_FORCE_INLINE friend Vec4 operator+(const type s, const Vec4& vec4) noexcept { \
        return Vec4(s + vec4.x, s + vec4.y, s + vec4.z, s + vec4.w); \
    } \
\
    ALT_FORCE_INLINE friend Vec4 operator-(const type s, const Vec4& vec4) noexcept { \
        return Vec4(s - vec4.x, s - vec4.y, s - vec4.z, s - vec4.w); \
    } \
\
public: \
\
    union { \
\
        type vec[4]; \
\
        struct { type x, y, z, w; }; \
    }; \
}; \

#define __ALT_STD_INTERNAL_MATH_IMPL_VEC4_SIGNED__(type) \
template <> \
class Vec4<type> { \
public: \
\
    using Type = type; \
\
    ALT_FORCE_INLINE Vec4() noexcept \
        : vec{ static_cast<type>(0), static_cast<type>(0), static_cast<type>(0), static_cast<type>(0) } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec4(const type v0, const type v1, const type v2, const type v3) noexcept \
        : vec{ v0, v1, v2, v3 } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec4(const Vec3<type>& vec3, type v4) noexcept; \
\
    template <typename U> \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4<U> Cast() const noexcept { \
        return Vec4<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MinFetch() const noexcept { \
        return Num::Min(x, y, z, w); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MaxFetch() const noexcept { \
        return Num::Max(x, y, z, w); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Min(const type vecx, const type vecy, const type vecz, const type vecw) const noexcept { \
        return Vec4(Num::Min(x, vecx), Num::Min(y, vecy), Num::Min(z, vecz), Num::Min(w, vecw)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Min(const Vec4& vec) const noexcept { \
        return Vec4(Num::Min(x, vec.x), Num::Min(y, vec.y), Num::Min(z, vec.z), Num::Min(w, vec.w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Max(const type vecx, const type vecy, const type vecz, const type vecw) const noexcept { \
        return Vec4(Num::Max(x, vecx), Num::Max(y, vecy), Num::Max(z, vecz), Num::Max(w, vecw)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Max(const Vec4& vec) const noexcept { \
        return Vec4(Num::Max(x, vec.x), Num::Max(y, vec.y), Num::Max(z, vec.z), Num::Max(w, vec.w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Clamp(const Vec4& min, const Vec4& max) const noexcept { \
        return Min(max).Max(min); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 Abs() const noexcept { \
        return Vec4(Num::Abs(x), Num::Abs(y), Num::Abs(z), Num::Abs(w)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xyzw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xywz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xzyw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xzwy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xwzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 xwyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yxzw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yxwz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yzxw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 yzwx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 ywxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 ywzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zxyw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zxwy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zyxw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zywx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zwxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 zwyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wxyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wxzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wyxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wyzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wzxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec4 wzyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> yxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> yzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xyw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xwy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> yxw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> ywx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xwz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> xzw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zxw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zwx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> wzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> ywz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> yzw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zwy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<type> zyw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> wx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> wz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zw() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> wy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yw() const noexcept; \
\
    ALT_NODISCARD ALT_FORCE_INLINE type& operator[](const usize i) noexcept { \
        return vec[i]; \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE const type& operator[](const usize i) const noexcept { \
        return vec[i]; \
    } \
\
    ALT_FORCE_INLINE Vec4 operator*(const type s) const noexcept { \
        return Vec4(x * s, y * s, z * s, w * s); \
    } \
\
    ALT_FORCE_INLINE Vec4 operator*(const Vec4& rhs) const noexcept { \
        return Vec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); \
    } \
\
    ALT_FORCE_INLINE Vec4& operator*=(const type s) noexcept { \
\
        *this = Vec4(x * s, y * s, z * s, w * s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4& operator*=(const Vec4& rhs) noexcept { \
\
        *this = Vec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4 operator/(const type s) const noexcept { \
        return Vec4(x / s, y / s, z / s, w / s); \
    } \
\
    ALT_FORCE_INLINE Vec4 operator/(const Vec4& rhs) const noexcept { \
        return Vec4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); \
    } \
\
    ALT_FORCE_INLINE Vec4& operator/=(const type s) noexcept { \
\
        *this = Vec4(x / s, y / s, z / s, w / s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4& operator/=(const Vec4& rhs) noexcept { \
\
        *this = Vec4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4 operator+(const type s) const noexcept { \
        return Vec4(x + s, y + s, z + s, w + s); \
    } \
\
    ALT_FORCE_INLINE Vec4 operator+(const Vec4& rhs) const noexcept { \
        return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); \
    } \
\
    ALT_FORCE_INLINE Vec4& operator+=(const type s) noexcept { \
\
        *this = Vec4(x + s, y + s, z + s, w + s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4& operator+=(const Vec4& rhs) noexcept { \
\
        *this = Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4 operator-(const type s) const noexcept { \
        return Vec4(x - s, y - s, z - s, w - s); \
    } \
\
    ALT_FORCE_INLINE Vec4 operator-(const Vec4& rhs) const noexcept { \
        return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); \
    } \
\
    ALT_FORCE_INLINE Vec4& operator-=(const type s) noexcept { \
\
        *this = Vec4(x - s, y - s, z - s, w - s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4& operator-=(const Vec4& rhs) noexcept { \
\
        *this = Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec4 operator-() const noexcept { \
        return Vec4(-x, -y, -z, -w); \
    } \
\
    ALT_FORCE_INLINE friend Vec4 operator*(const type s, const Vec4& vec4) noexcept { \
        return Vec4(s * vec4.x, s * vec4.y, s * vec4.z, s * vec4.w); \
    } \
\
    ALT_FORCE_INLINE friend Vec4 operator+(const type s, const Vec4& vec4) noexcept { \
        return Vec4(s + vec4.x, s + vec4.y, s + vec4.z, s + vec4.w); \
    } \
\
    ALT_FORCE_INLINE friend Vec4 operator-(const type s, const Vec4& vec4) noexcept { \
        return Vec4(s - vec4.x, s - vec4.y, s - vec4.z, s - vec4.w); \
    } \
\
public: \
\
    union { \
\
        type vec[4]; \
\
        struct { type x, y, z, w; }; \
    }; \
};

} // namespace Math
} // namespace Alt

#include "Vec4.inl"

namespace Alt {
namespace Math {

__ALT_STD_INTERNAL_MATH_IMPL_VEC4_UNSIGNED__(u8)
__ALT_STD_INTERNAL_MATH_IMPL_VEC4_UNSIGNED__(u16)
__ALT_STD_INTERNAL_MATH_IMPL_VEC4_UNSIGNED__(u32)
__ALT_STD_INTERNAL_MATH_IMPL_VEC4_UNSIGNED__(u64)

__ALT_STD_INTERNAL_MATH_IMPL_VEC4_SIGNED__(s8)
__ALT_STD_INTERNAL_MATH_IMPL_VEC4_SIGNED__(s16)
__ALT_STD_INTERNAL_MATH_IMPL_VEC4_SIGNED__(s32)
__ALT_STD_INTERNAL_MATH_IMPL_VEC4_SIGNED__(s64)

#undef __ALT_STD_INTERNAL_MATH_IMPL_VEC4_UNSIGNED__
#undef __ALT_STD_INTERNAL_MATH_IMPL_VEC4_SIGNED__

using Vec4u8 = Vec4<u8>;
using Vec4u16 = Vec4<u16>;
using Vec4u32 = Vec4<u32>;
using Vec4u64 = Vec4<u64>;
using Vec4usize = Vec4<usize>;

using Vec4s8 = Vec4<s8>;
using Vec4s16 = Vec4<s16>;
using Vec4s32 = Vec4<s32>;
using Vec4s64 = Vec4<s64>;
using Vec4ssize = Vec4<ssize>;

using Vec4f32 = Vec4<f32>;
using Vec4f64 = Vec4<f64>;

} // namespace Math

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<Math::Vec4<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Format specialization for Vec4<T>.
 */
template <typename T>
class Format<Math::Vec4<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Math::Vec4<T>& val) noexcept {
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
                result.size += estimated.size + 2;

                result.size += estimator.GetIndentEstimatedSize();
                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.z) iferr_return Nullopt;
                result.size += estimated.size + 2;

                result.size += estimator.GetIndentEstimatedSize();
                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.w) iferr_return Nullopt;
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
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.z) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.w) iferr_return Nullopt;
            result.size += estimated.size;

            result.size += 2;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Math::Vec4<T>& val) noexcept {
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
                writer.Append(',') iferr_return false;
                writer.Append('\n') iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val.z) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append('\n') iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val.w) iferr_return false;
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
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.z) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.w) iferr_return false;

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
        return writer.Append("Vec4{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u8) noexcept {
        return 7;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u8) noexcept {
        return writer.Append("Vec4u8{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u16) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u16) noexcept {
        return writer.Append("Vec4u16{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u32) noexcept {
        return writer.Append("Vec4u32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u64) noexcept {
        return writer.Append("Vec4u64{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s8) noexcept {
        return 7;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s8) noexcept {
        return writer.Append("Vec4s8{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s16) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s16) noexcept {
        return writer.Append("Vec4s16{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s32) noexcept {
        return writer.Append("Vec4s32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s64) noexcept {
        return writer.Append("Vec4s64{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f32) noexcept {
        return writer.Append("Vec4f32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f64) noexcept {
        return writer.Append("Vec4f64{");
    }
};

} // namespace Alt

#ifdef ALT_COMPILER_MSVC
#pragma warning(pop)
#endif // ALT_COMPILER_MSVC

#endif // __ALT_STD_MATH_VEC4__