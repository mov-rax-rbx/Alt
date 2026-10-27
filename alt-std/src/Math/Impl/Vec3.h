#pragma once

#ifndef __ALT_STD_MATH_VEC3__
#define __ALT_STD_MATH_VEC3__

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
class Vec4;

template <typename T>
class Vec3 {
public:

    using Type = T;

    ALT_FORCE_INLINE Vec3() noexcept
        : vec{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0) }
    {}

    ALT_FORCE_INLINE explicit Vec3(const T v) noexcept
        : vec{ v, v, v }
    {}

    ALT_FORCE_INLINE explicit Vec3(const T v0, const T v1, const T v2) noexcept
        : vec{ v0, v1, v2 }
    {}

    ALT_FORCE_INLINE explicit Vec3(const Vec4<T>& vec4) noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE T Dot(const Vec3& vec3) const noexcept {
        return x * vec3.x + y * vec3.y + z * vec3.z;
    }

    ALT_NODISCARD ALT_FORCE_INLINE T Length() const noexcept {
        return Sqrt(this->Dot(*this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE T LengthSq() const noexcept {
        return this->Dot(*this);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsUnit() const noexcept {
        return Math::IsEqFuzzy(Length(), static_cast<T>(1.0));
    }

    ALT_NODISCARD ALT_FORCE_INLINE ssize CompareFuzzy(const Vec3& rhs, const T eps = Num::Desc<T>::Eps()) const noexcept {

        if (Math::IsEqFuzzy(vec[0], rhs[0], eps)) {

            if (Math::IsEqFuzzy(vec[1], rhs[1], eps)) {

                if (Math::IsEqFuzzy(vec[2], rhs[2], eps)) {

                    return 0;
                }

                return vec[2] < rhs[2] ? -1 : 1;
            }

            return vec[1] < rhs[1] ? -1 : 1;
        }

        return vec[0] < rhs[0] ? -1 : 1;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEqFuzzy(const Vec3& rhs, const T eps = Num::Desc<T>::Eps()) const noexcept {

        return
            Math::IsEqFuzzy(vec[0], rhs[0], eps) &&
            Math::IsEqFuzzy(vec[1], rhs[1], eps) &&
            Math::IsEqFuzzy(vec[2], rhs[2], eps);
    }

    struct RightUpPerpResult {
        Vec3<T> up;
        Vec3<T> right;
    };
    ALT_NODISCARD ALT_FORCE_INLINE RightUpPerpResult CalculateRightUpPerp() const noexcept {

        RightUpPerpResult result;

        const Vec3 up{ static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0) };
        const Vec3 right{ static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0) };

        const Vec3 self = Normalize();

        if (Math::IsEqFuzzy(Num::Abs(self.Dot(up)), static_cast<T>(1.0))) {

            result.up = right.Cross(self).Normalize();
            result.right = self.Cross(result.up).Normalize();
        }
        else {

            result.right = self.Cross(up).Normalize();
            result.up = result.right.Cross(self).Normalize();
        }

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Cross(const Vec3& rhs) const noexcept {

        return Vec3{
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x,
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 FrictionVec(const Vec3& normal) const noexcept {
        ALT_ASSERT(normal.IsUnit());

        return this->Dot(normal) * normal - *this;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Reflect(const Vec3& dir) const noexcept {
        ALT_ASSERT(dir.IsUnit());

        return *this - static_cast<T>(2.0) * this->Dot(dir) * dir;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Refract(const Vec3& dir, const T eta) const noexcept {
        ALT_ASSERT(IsUnit());
        ALT_ASSERT(dir.IsUnit());

        const T dot = this->Dot(dir);
        const T k = static_cast<T>(1.0) - eta * eta * (static_cast<T>(1.0) - dot * dot);

        return k < static_cast<T>(0.0)
            ? Vec3{ static_cast<T>(0.0) }
            : eta * *this - (eta * dot + Sqrt(k)) * dir;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Normalize() const noexcept {
        return *this * Rsqrt(LengthSq());
    }

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<U> Cast() const noexcept {
        return Vec3<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
    }

    ALT_NODISCARD ALT_FORCE_INLINE T MinFetch() const noexcept {
        return Num::Min(x, y, z);
    }

    ALT_NODISCARD ALT_FORCE_INLINE T MaxFetch() const noexcept {
        return Num::Max(x, y, z);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Min(const T rhsx, const T rhsy, const T rhsz) const noexcept {
        return Vec3(Num::Min(x, rhsx), Num::Min(y, rhsy), Num::Min(z, rhsz));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Min(const Vec3& rhs) const noexcept {
        return Vec3(Num::Min(x, rhs.x), Num::Min(y, rhs.y), Num::Min(z, rhs.z));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Max(const T rhsx, const T rhsy, const T rhsz) const noexcept {
        return Vec3(Num::Max(x, rhsx), Num::Max(y, rhsy), Num::Max(z, rhsz));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Max(const Vec3& rhs) const noexcept {
        return Vec3(Num::Max(x, rhs.x), Num::Max(y, rhs.y), Num::Max(z, rhs.z));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Clamp(const Vec3& min, const Vec3& max) const noexcept {
        return Min(max).Max(min);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Abs() const noexcept {
        return Vec3(Num::Abs(x), Num::Abs(y), Num::Abs(z));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Floor() const noexcept {
        return Vec3(Math::Floor(x), Math::Floor(y), Math::Floor(z));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Ceil() const noexcept {
        return Vec3(Math::Ceil(x), Math::Ceil(y), Math::Ceil(z));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Trunc() const noexcept {
        return Vec3(Math::Trunc(x), Math::Trunc(y), Math::Trunc(z));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Frac() const noexcept {
        return Vec3(Math::Frac(x), Math::Frac(y), Math::Frac(z));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Round() const noexcept {
        return Vec3(Math::Round(x), Math::Round(y), Math::Round(z));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 xyz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 xzy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 yxz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 yzx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 zxy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 zyx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> xy() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> xz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> yx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> yz() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> zx() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> zy() const noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Lerp(const Vec3& dst, const T t) const noexcept {
        return *this * (static_cast<T>(1.0) - t) + dst * t;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 RotateX(const T angleRad) const noexcept {

        const T cos = Cos(angleRad);
        const T sin = Sin(angleRad);

        return Vec3{
            x, y * cos - z * sin, y * sin + z * cos,
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 RotateY(const T angleRad) const noexcept {

        const T cos = Cos(angleRad);
        const T sin = Sin(angleRad);

        return Vec3{
            x * cos + z * sin, y, -x * sin + z * cos,
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 RotateZ(const T angleRad) const noexcept {

        const T cos = Cos(angleRad);
        const T sin = Sin(angleRad);

        return Vec3{
            x * cos - y * sin, x * sin + y * cos, z,
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 PerpNormalized() const noexcept {

        if (Num::Abs(x) > Num::Abs(y)) {

            const T invLen = Rsqrt(x * x + z * z);
            return Vec3(z, static_cast<T>(0.0), -x) * invLen;
        }
        else {

            const T invLen = Rsqrt(y * y + z * z);
            return Vec3(static_cast<T>(0.0), z, -y) * invLen;
        }
    }

    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        return vec[i];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize i) const noexcept {
        return vec[i];
    }

    ALT_FORCE_INLINE Vec3 operator*(const T s) const noexcept {
        return Vec3(x * s, y * s, z * s);
    }

    ALT_FORCE_INLINE Vec3 operator*(const Vec3& rhs) const noexcept {
        return Vec3(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    ALT_FORCE_INLINE Vec3& operator*=(const T s) noexcept {

        *this = Vec3(x * s, y * s, z * s);
        return *this;
    }

    ALT_FORCE_INLINE Vec3& operator*=(const Vec3& rhs) noexcept {

        *this = Vec3(x * rhs.x, y * rhs.y, z * rhs.z);
        return *this;
    }

    ALT_FORCE_INLINE Vec3 operator/(const T s) const noexcept {

        const T inv = SafeInv(s);
        return Vec3(x * inv, y * inv, z * inv);
    }

    ALT_FORCE_INLINE Vec3 operator/(const Vec3& rhs) const noexcept {
        return Vec3(x * SafeInv(rhs.x), y * SafeInv(rhs.y), z * SafeInv(rhs.z));
    }

    ALT_FORCE_INLINE Vec3& operator/=(const T s) noexcept {

        const T inv = SafeInv(s);
        *this = Vec3(x * inv, y * inv, z * inv);

        return *this;
    }

    ALT_FORCE_INLINE Vec3& operator/=(const Vec3& rhs) noexcept {

        *this = Vec3(x * SafeInv(rhs.x), y * SafeInv(rhs.y), z * SafeInv(rhs.z));
        return *this;
    }

    ALT_FORCE_INLINE Vec3 operator+(const T s) const noexcept {
        return Vec3(x + s, y + s, z + s);
    }

    ALT_FORCE_INLINE Vec3 operator+(const Vec3& rhs) const noexcept {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    ALT_FORCE_INLINE Vec3& operator+=(const T s) noexcept {

        *this = Vec3(x + s, y + s, z + s);
        return *this;
    }

    ALT_FORCE_INLINE Vec3& operator+=(const Vec3& rhs) noexcept {

        *this = Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
        return *this;
    }

    ALT_FORCE_INLINE Vec3 operator-(const T s) const noexcept {
        return Vec3(x - s, y - s, z - s);
    }

    ALT_FORCE_INLINE Vec3 operator-(const Vec3& rhs) const noexcept {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    ALT_FORCE_INLINE Vec3& operator-=(const T s) noexcept {

        *this = Vec3(x - s, y - s, z - s);
        return *this;
    }

    ALT_FORCE_INLINE Vec3& operator-=(const Vec3& rhs) noexcept {

        *this = Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
        return *this;
    }

    ALT_FORCE_INLINE Vec3 operator-() const noexcept {
        return Vec3(-x, -y, -z);
    }

    ALT_FORCE_INLINE friend Vec3 operator*(const T s, const Vec3& vec3) noexcept {
        return Vec3(s * vec3.x, s * vec3.y, s * vec3.z);
    }

    ALT_FORCE_INLINE friend Vec3 operator+(const T s, const Vec3& vec3) noexcept {
        return Vec3(s + vec3.x, s + vec3.y, s + vec3.z);
    }

    ALT_FORCE_INLINE friend Vec3 operator-(const T s, const Vec3& vec3) noexcept {
        return Vec3(s - vec3.x, s - vec3.y, s - vec3.z);
    }

public:

    union {

        T vec[3];

        struct { T x, y, z; };
    };
};

#define __ALT_STD_INTERNAL_MATH_IMPL_VEC3_UNSIGNED__(type) \
template <> \
class Vec3<type> { \
public: \
\
    using Type = type; \
\
    ALT_FORCE_INLINE Vec3() noexcept \
        : vec{ static_cast<type>(0), static_cast<type>(0), static_cast<type>(0) } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec3(const type v) noexcept \
        : vec{ v, v, v } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec3(const type v0, const type v1, const type v2) noexcept \
        : vec{ v0, v1, v2 } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec3(const Vec4<type>& vec4) noexcept; \
\
    template <typename U> \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<U> Cast() const noexcept { \
        return Vec3<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MinFetch() const noexcept { \
        return Num::Min(x, y, z); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MaxFetch() const noexcept { \
        return Num::Max(x, y, z); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Min(const type rhsx, const type rhsy, const type rhsz) const noexcept { \
        return Vec3(Num::Min(x, rhsx), Num::Min(y, rhsy), Num::Min(z, rhsz)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Min(const Vec3& rhs) const noexcept { \
        return Vec3(Num::Min(x, rhs.x), Num::Min(y, rhs.y), Num::Min(z, rhs.z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Max(const type rhsx, const type rhsy, const type rhsz) const noexcept { \
        return Vec3(Num::Max(x, rhsx), Num::Max(y, rhsy), Num::Max(z, rhsz)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Max(const Vec3& rhs) const noexcept { \
        return Vec3(Num::Max(x, rhs.x), Num::Max(y, rhs.y), Num::Max(z, rhs.z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Clamp(const Vec3& min, const Vec3& max) const noexcept { \
        return Min(max).Max(min); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 AddWrap(const Vec3& rhs) const noexcept { \
        return Vec3(Num::AddWrap(x, rhs.x), Num::AddWrap(y, rhs.y), Num::AddWrap(z, rhs.z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 SubWrap(const Vec3& rhs) const noexcept { \
        return Vec3(Num::SubWrap(x, rhs.x), Num::SubWrap(y, rhs.y), Num::SubWrap(z, rhs.z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 MulWrap(const Vec3& rhs) const noexcept { \
        return Vec3(Num::MulWrap(x, rhs.x), Num::MulWrap(y, rhs.y), Num::MulWrap(z, rhs.z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 xyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 xzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 yxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 yzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 zxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 zyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zy() const noexcept; \
\
    ALT_NODISCARD ALT_FORCE_INLINE type& operator[](const usize i) noexcept { \
        return vec[i]; \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE const type& operator[](const usize i) const noexcept { \
        return vec[i]; \
    } \
\
    ALT_FORCE_INLINE Vec3 operator*(const type s) const noexcept { \
        return Vec3(x * s, y * s, z * s); \
    } \
\
    ALT_FORCE_INLINE Vec3 operator*(const Vec3& rhs) const noexcept { \
        return Vec3(x * rhs.x, y * rhs.y, z * rhs.z); \
    } \
\
    ALT_FORCE_INLINE Vec3& operator*=(const type s) noexcept { \
\
        *this = Vec3(x * s, y * s, z * s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3& operator*=(const Vec3& rhs) noexcept { \
\
        *this = Vec3(x * rhs.x, y * rhs.y, z * rhs.z); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3 operator/(const type s) const noexcept { \
        return Vec3(x / s, y / s, z / s); \
    } \
\
    ALT_FORCE_INLINE Vec3 operator/(const Vec3& rhs) const noexcept { \
        return Vec3(x / rhs.x, y / rhs.y, z / rhs.z); \
    } \
\
    ALT_FORCE_INLINE Vec3& operator/=(const type s) noexcept { \
\
        *this = Vec3(x / s, y / s, z / s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3& operator/=(const Vec3& rhs) noexcept { \
\
        *this = Vec3(x / rhs.x, y / rhs.y, z / rhs.z); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3 operator+(const type s) const noexcept { \
        return Vec3(x + s, y + s, z + s); \
    } \
\
    ALT_FORCE_INLINE Vec3 operator+(const Vec3& rhs) const noexcept { \
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); \
    } \
\
    ALT_FORCE_INLINE Vec3& operator+=(const type s) noexcept { \
\
        *this = Vec3(x + s, y + s, z + s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3& operator+=(const Vec3& rhs) noexcept { \
\
        *this = Vec3(x + rhs.x, y + rhs.y, z + rhs.z); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3 operator-(const type s) const noexcept { \
        return Vec3(x - s, y - s, z - s); \
    } \
\
    ALT_FORCE_INLINE Vec3 operator-(const Vec3& rhs) const noexcept { \
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); \
    } \
\
    ALT_FORCE_INLINE Vec3& operator-=(const type s) noexcept { \
\
        *this = Vec3(x - s, y - s, z - s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3& operator-=(const Vec3& rhs) noexcept { \
\
        *this = Vec3(x - rhs.x, y - rhs.y, z - rhs.z); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE friend Vec3 operator*(const type s, const Vec3& vec3) noexcept { \
        return Vec3(s * vec3.x, s * vec3.y, s * vec3.z); \
    } \
\
    ALT_FORCE_INLINE friend Vec3 operator+(const type s, const Vec3& vec3) noexcept { \
        return Vec3(s + vec3.x, s + vec3.y, s + vec3.z); \
    } \
\
    ALT_FORCE_INLINE friend Vec3 operator-(const type s, const Vec3& vec3) noexcept { \
        return Vec3(s - vec3.x, s - vec3.y, s - vec3.z); \
    } \
\
public: \
\
    union { \
\
        type vec[3]; \
\
        struct { type x, y, z; }; \
    }; \
};

#define __ALT_STD_INTERNAL_MATH_IMPL_VEC3_SIGNED__(type) \
template <> \
class Vec3<type> { \
public: \
\
    using Type = type; \
\
    ALT_FORCE_INLINE Vec3() noexcept \
        : vec{ static_cast<type>(0), static_cast<type>(0), static_cast<type>(0) } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec3(const type v) noexcept \
        : vec{ v, v, v } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec3(const type v0, const type v1, const type v2) noexcept \
        : vec{ v0, v1, v2 } \
    {} \
\
    ALT_FORCE_INLINE explicit Vec3(const Vec4<type>& vec4) noexcept; \
\
    template <typename U> \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3<U> Cast() const noexcept { \
        return Vec3<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MinFetch() const noexcept { \
        return Num::Min(x, y, z); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type MaxFetch() const noexcept { \
        return Num::Max(x, y, z); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Min(const type rhsx, const type rhsy, const type rhsz) const noexcept { \
        return Vec3(Num::Min(x, rhsx), Num::Min(y, rhsy), Num::Min(z, rhsz)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Min(const Vec3& rhs) const noexcept { \
        return Vec3(Num::Min(x, rhs.x), Num::Min(y, rhs.y), Num::Min(z, rhs.z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Max(const type rhsx, const type rhsy, const type rhsz) const noexcept { \
        return Vec3(Num::Max(x, rhsx), Num::Max(y, rhsy), Num::Max(z, rhsz)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Max(const Vec3& rhs) const noexcept { \
        return Vec3(Num::Max(x, rhs.x), Num::Max(y, rhs.y), Num::Max(z, rhs.z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Clamp(const Vec3& min, const Vec3& max) const noexcept { \
        return Min(max).Max(min); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 Abs() const noexcept { \
        return Vec3(Num::Abs(x), Num::Abs(y), Num::Abs(z)); \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 xyz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 xzy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 yxz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 yzx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 zxy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 zyx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xy() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> xz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> yz() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zx() const noexcept; \
    ALT_NODISCARD ALT_FORCE_INLINE Vec2<type> zy() const noexcept; \
\
    ALT_NODISCARD ALT_FORCE_INLINE const type& operator[](const usize i) const noexcept { \
        return vec[i]; \
    } \
\
    ALT_NODISCARD ALT_FORCE_INLINE type& operator[](const usize i) noexcept { \
        return vec[i]; \
    } \
\
    ALT_FORCE_INLINE Vec3 operator*(const type s) const noexcept { \
        return Vec3(x * s, y * s, z * s); \
    } \
\
    ALT_FORCE_INLINE Vec3 operator*(const Vec3& rhs) const noexcept { \
        return Vec3(x * rhs.x, y * rhs.y, z * rhs.z); \
    } \
\
    ALT_FORCE_INLINE Vec3& operator*=(const type s) noexcept { \
\
        *this = Vec3(x * s, y * s, z * s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3& operator*=(const Vec3& rhs) noexcept { \
\
        *this = Vec3(x * rhs.x, y * rhs.y, z * rhs.z); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3 operator/(const type s) const noexcept { \
        return Vec3(x / s, y / s, z / s); \
    } \
\
    ALT_FORCE_INLINE Vec3 operator/(const Vec3& rhs) const noexcept { \
        return Vec3(x / rhs.x, y / rhs.y, z / rhs.z); \
    } \
\
    ALT_FORCE_INLINE Vec3& operator/=(const type s) noexcept { \
\
        *this = Vec3(x / s, y / s, z / s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3& operator/=(const Vec3& rhs) noexcept { \
\
        *this = Vec3(x / rhs.x, y / rhs.y, z / rhs.z); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3 operator+(const type s) const noexcept { \
        return Vec3(x + s, y + s, z + s); \
    } \
\
    ALT_FORCE_INLINE Vec3 operator+(const Vec3& rhs) const noexcept { \
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); \
    } \
\
    ALT_FORCE_INLINE Vec3& operator+=(const type s) noexcept { \
\
        *this = Vec3(x + s, y + s, z + s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3& operator+=(const Vec3& rhs) noexcept { \
\
        *this = Vec3(x + rhs.x, y + rhs.y, z + rhs.z); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3 operator-(const type s) const noexcept { \
        return Vec3(x - s, y - s, z - s); \
    } \
\
    ALT_FORCE_INLINE Vec3 operator-(const Vec3& rhs) const noexcept { \
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); \
    } \
\
    ALT_FORCE_INLINE Vec3& operator-=(const type s) noexcept { \
\
        *this = Vec3(x - s, y - s, z - s); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3& operator-=(const Vec3& rhs) noexcept { \
\
        *this = Vec3(x - rhs.x, y - rhs.y, z - rhs.z); \
        return *this; \
    } \
\
    ALT_FORCE_INLINE Vec3 operator-() const noexcept { \
        return Vec3(-x, -y, -z); \
    } \
\
    ALT_FORCE_INLINE friend Vec3 operator*(const type s, const Vec3& vec3) noexcept { \
        return Vec3(s * vec3.x, s * vec3.y, s * vec3.z); \
    } \
\
    ALT_FORCE_INLINE friend Vec3 operator+(const type s, const Vec3& vec3) noexcept { \
        return Vec3(s + vec3.x, s + vec3.y, s + vec3.z); \
    } \
\
    ALT_FORCE_INLINE friend Vec3 operator-(const type s, const Vec3& vec3) noexcept { \
        return Vec3(s - vec3.x, s - vec3.y, s - vec3.z); \
    } \
\
public: \
\
    union { \
\
        type vec[3]; \
\
        struct { type x, y, z; }; \
    }; \
};

} // namespace Math
} // namespace Alt

#include "Vec3.inl"

namespace Alt {
namespace Math {

__ALT_STD_INTERNAL_MATH_IMPL_VEC3_UNSIGNED__(u8)
__ALT_STD_INTERNAL_MATH_IMPL_VEC3_UNSIGNED__(u16)
__ALT_STD_INTERNAL_MATH_IMPL_VEC3_UNSIGNED__(u32)
__ALT_STD_INTERNAL_MATH_IMPL_VEC3_UNSIGNED__(u64)

__ALT_STD_INTERNAL_MATH_IMPL_VEC3_SIGNED__(s8)
__ALT_STD_INTERNAL_MATH_IMPL_VEC3_SIGNED__(s16)
__ALT_STD_INTERNAL_MATH_IMPL_VEC3_SIGNED__(s32)
__ALT_STD_INTERNAL_MATH_IMPL_VEC3_SIGNED__(s64)

#undef __ALT_STD_INTERNAL_MATH_IMPL_VEC3_UNSIGNED__
#undef __ALT_STD_INTERNAL_MATH_IMPL_VEC3_SIGNED__

using Vec3u8 = Vec3<u8>;
using Vec3u16 = Vec3<u16>;
using Vec3u32 = Vec3<u32>;
using Vec3u64 = Vec3<u64>;
using Vec3usize = Vec3<usize>;

using Vec3s8 = Vec3<s8>;
using Vec3s16 = Vec3<s16>;
using Vec3s32 = Vec3<s32>;
using Vec3s64 = Vec3<s64>;
using Vec3ssize = Vec3<ssize>;

using Vec3f32 = Vec3<f32>;
using Vec3f64 = Vec3<f64>;

} // namespace Math

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<Math::Vec3<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Format specialization for Vec3<T>.
 */
template <typename T>
class Format<Math::Vec3<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Math::Vec3<T>& val) noexcept {
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
            result.size += estimated.size;

            result.size += 2;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Math::Vec3<T>& val) noexcept {
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
        return writer.Append("Vec3{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u8) noexcept {
        return 7;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u8) noexcept {
        return writer.Append("Vec3u8{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u16) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u16) noexcept {
        return writer.Append("Vec3u16{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u32) noexcept {
        return writer.Append("Vec3u32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u64) noexcept {
        return writer.Append("Vec3u64{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s8) noexcept {
        return 7;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s8) noexcept {
        return writer.Append("Vec3s8{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s16) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s16) noexcept {
        return writer.Append("Vec3s16{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s32) noexcept {
        return writer.Append("Vec3s32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s64) noexcept {
        return writer.Append("Vec3s64{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f32) noexcept {
        return writer.Append("Vec3f32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f64) noexcept {
        return writer.Append("Vec3f64{");
    }
};

} // namespace Alt

#ifdef ALT_COMPILER_MSVC
#pragma warning(pop)
#endif // ALT_COMPILER_MSVC

#endif // __ALT_STD_MATH_VEC3__