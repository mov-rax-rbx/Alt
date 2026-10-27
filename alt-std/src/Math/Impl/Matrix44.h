#pragma once

#ifndef __ALT_STD_MATH_MATRIX44__
#define __ALT_STD_MATH_MATRIX44__

#include "../../String/Impl/FormatPreInclude.h"

#include "../../Memory/Memory.h"
#include "../../Utils/Optional.h"

#include "MathPreInclude.h"

#include "Vec3.h"
#include "Vec4.h"
#include "Quat.h"
#include "Point3.h"

namespace Alt {
namespace Math {

template <typename T>
class Quat;

/*
 * Column Major
 */
template <typename T>
class Matrix44 {
public:

    using Type = T;

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 Identity() noexcept {

        return Matrix44{
            static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 FromTranslation(const Vec3<T>& vec3) noexcept {

        return Matrix44{
            static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0),
            vec3.x,              vec3.y,              vec3.z,              static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 FromTranslation(const T x, const T y, const T z) noexcept {

        return Matrix44{
            static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0),
            x,                   y,                   z,                   static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 FromScale(T scale) noexcept {

        return Matrix44{
            scale,               static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), scale,               static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), scale,               static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 FromScale(const Vec3<T> scale) noexcept {

        return Matrix44{
            scale.x,           static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), scale.y,           static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), scale.z,           static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 FromScale(const T x, const T y, const T z) noexcept {

        return Matrix44{
            x,                   static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), y,                   static_cast<T>(0.0), static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), z,                   static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 FromRotate(Vec3<T> axis, const T angleRad) noexcept {

        axis = axis.Normalize();

        const T cos = Cos(angleRad);
        const T sin = Sin(angleRad);

        const Vec3<T> coef{ (static_cast<T>(1.0) - cos) * axis };

        return Matrix44{

            cos + coef[0] * axis[0],
            coef[0] * axis[1] + sin * axis[2],
            coef[0] * axis[2] - sin * axis[1],
            static_cast<T>(0.0),

            coef[1] * axis[0] - sin * axis[2],
            cos + coef[1] * axis[1],
            coef[1] * axis[2] + sin * axis[0],
            static_cast<T>(0.0),

            coef[2] * axis[0] + sin * axis[1],
            coef[2] * axis[1] - sin * axis[0],
            cos + coef[2] * axis[2],
            static_cast<T>(0.0),

            static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44<T> FromVectors(Vec3<T> orig, Vec3<T> dest) noexcept {

        orig = orig.Normalize();
        dest = dest.Normalize();

        const T c = orig.Dot(dest);

        const bool isInOppositeDir = static_cast<T>(1.0) + c <= Num::Desc<T>::Eps();
        if (isInOppositeDir) {

            const Vec3<T> axis = orig.PerpNormalized();

            return Matrix44{
                axis.x * axis.x - (static_cast<T>(1.0) - axis.x * axis.x), axis.x * axis.y * static_cast<T>(2.0), axis.x * axis.z * static_cast<T>(2.0), static_cast<T>(0.0),
                axis.x * axis.y * static_cast<T>(2.0), axis.y * axis.y - (static_cast<T>(1.0) - axis.y * axis.y), axis.y * axis.z * static_cast<T>(2.0), static_cast<T>(0.0),
                axis.x * axis.z * static_cast<T>(2.0), axis.y * axis.z * static_cast<T>(2.0), axis.z * axis.z - (static_cast<T>(1.0) - axis.z * axis.z), static_cast<T>(0.0),
                static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0),
            };
        }

        const Vec3<T> axis = orig.Cross(dest);
        const T k = static_cast<T>(1.0) / (static_cast<T>(1.0) + c);

        return Matrix44{
            axis.x * axis.x * k + c, axis.x * axis.y * k + axis.z, axis.x * axis.z * k - axis.y, static_cast<T>(0.0),
            axis.y * axis.x * k - axis.z, axis.y * axis.y * k + c, axis.y * axis.z * k + axis.x, static_cast<T>(0.0),
            axis.z * axis.x * k + axis.y, axis.z * axis.y * k - axis.x, axis.z * axis.z * k + c, static_cast<T>(0.0),
            static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44<T> FromQuat(const Quat<T>& quat) noexcept {

        return quat.ToMatrix44();
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44<T> FromEulerAngles(const Vec3<T>& angle) noexcept {

        const T c1 = Cos(-angle.x);
        const T c2 = Cos(-angle.y);
        const T c3 = Cos(-angle.z);

        const T s1 = Sin(-angle.x);
        const T s2 = Sin(-angle.y);
        const T s3 = Sin(-angle.z);

        Matrix44<T> res;
        res(0, 0) = c2 * c3;
        res(1, 0) = -c1 * s3 + s1 * s2 * c3;
        res(2, 0) = s1 * s3 + c1 * s2 * c3;
        res(3, 0) = static_cast<T>(0.0);

        res(0, 1) = c2 * s3;
        res(1, 1) = c1 * c3 + s1 * s2 * s3;
        res(2, 1) = -s1 * c3 + c1 * s2 * s3;
        res(3, 1) = static_cast<T>(0.0);

        res(0, 2) = -s2;
        res(1, 2) = s1 * c2;
        res(2, 2) = c1 * c2;
        res(3, 2) = static_cast<T>(0.0);

        res(0, 3) = static_cast<T>(0.0);
        res(1, 3) = static_cast<T>(0.0);
        res(2, 3) = static_cast<T>(0.0);
        res(3, 3) = static_cast<T>(1.0);

        return res;
    }

    /*
     * Right Hended we revert Z axis 
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 Ortho_LH_Z__1_1(const T left, const T right, const T bottom, const T top, const T zNear, const T zFar) noexcept {

        return Matrix44{
            static_cast<T>(2.0) / (right - left), static_cast<T>(0.0),                  static_cast<T>(0.0),                   static_cast<T>(0.0),
            static_cast<T>(0.0),                  static_cast<T>(2.0) / (top - bottom), static_cast<T>(0.0),                   static_cast<T>(0.0),
            static_cast<T>(0.0),                  static_cast<T>(0.0),                  static_cast<T>(2.0) / (zFar - zNear),  static_cast<T>(0.0),
            -(right + left) / (right - left),     -(top + bottom) / (top - bottom),     -(zFar + zNear) / (zFar - zNear),      static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 Ortho_RH_Z__1_1(const T left, const T right, const T bottom, const T top, const T zNear, const T zFar) noexcept {

        return Matrix44{
            static_cast<T>(2.0) / (right - left), static_cast<T>(0.0),                  static_cast<T>(0.0),                   static_cast<T>(0.0),
            static_cast<T>(0.0),                  static_cast<T>(2.0) / (top - bottom), static_cast<T>(0.0),                   static_cast<T>(0.0),
            static_cast<T>(0.0),                  static_cast<T>(0.0),                  -static_cast<T>(2.0) / (zFar - zNear), static_cast<T>(0.0),
            -(right + left) / (right - left),     -(top + bottom) / (top - bottom),     -(zFar + zNear) / (zFar - zNear),      static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 Ortho_LH_Z_0_1(const T left, const T right, const T bottom, const T top, const T zNear, const T zFar) noexcept {

        return Matrix44{
            static_cast<T>(2.0) / (right - left), static_cast<T>(0.0),                  static_cast<T>(0.0),                   static_cast<T>(0.0),
            static_cast<T>(0.0),                  static_cast<T>(2.0) / (top - bottom), static_cast<T>(0.0),                   static_cast<T>(0.0),
            static_cast<T>(0.0),                  static_cast<T>(0.0),                  static_cast<T>(1.0) / (zFar - zNear),  static_cast<T>(0.0),
            -(right + left) / (right - left),     -(top + bottom) / (top - bottom),     -zNear / (zFar - zNear),               static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 Ortho_RH_Z_0_1(const T left, const T right, const T bottom, const T top, const T zNear, const T zFar) noexcept {

        return Matrix44{
            static_cast<T>(2.0) / (right - left), static_cast<T>(0.0),                  static_cast<T>(0.0),                   static_cast<T>(0.0),
            static_cast<T>(0.0),                  static_cast<T>(2.0) / (top - bottom), static_cast<T>(0.0),                   static_cast<T>(0.0),
            static_cast<T>(0.0),                  static_cast<T>(0.0),                  -static_cast<T>(1.0) / (zFar - zNear), static_cast<T>(0.0),
            -(right + left) / (right - left),     -(top + bottom) / (top - bottom),     -zNear / (zFar - zNear),               static_cast<T>(1.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 Perspective_LH_Z__1_1(const T fovYRad, const T aspectXY, const T zNear, const T zFar) noexcept {

        const T invHalfTan = SafeInv(Tan(fovYRad * static_cast<T>(0.5)));

        return Matrix44{
            invHalfTan * SafeInv(aspectXY), static_cast<T>(0.0), static_cast<T>(0.0),                                    static_cast<T>(0.0),
            static_cast<T>(0.0),            invHalfTan,          static_cast<T>(0.0),                                    static_cast<T>(0.0),
            static_cast<T>(0.0),            static_cast<T>(0.0), (zFar + zNear) / (zFar - zNear),                        static_cast<T>(1.0),
            static_cast<T>(0.0),            static_cast<T>(0.0), -(static_cast<T>(2.0) * zFar * zNear) / (zFar - zNear), static_cast<T>(0.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 Perspective_RH_Z__1_1(const T fovYRad, const T aspectXY, const T zNear, const T zFar) noexcept {

        const T invHalfTan = SafeInv(Tan(fovYRad * static_cast<T>(0.5)));

        return Matrix44{
            invHalfTan * SafeInv(aspectXY), static_cast<T>(0.0), static_cast<T>(0.0),                                    static_cast<T>(0.0),
            static_cast<T>(0.0),            invHalfTan,          static_cast<T>(0.0),                                    static_cast<T>(0.0),
            static_cast<T>(0.0),            static_cast<T>(0.0), -(zFar + zNear) / (zFar - zNear),                       static_cast<T>(-1.0),
            static_cast<T>(0.0),            static_cast<T>(0.0), -(static_cast<T>(2.0) * zFar * zNear) / (zFar - zNear), static_cast<T>(0.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 Perspective_LH_Z_0_1(const T fovYRad, const T aspectXY, const T zNear, const T zFar) noexcept {

        const T invHalfTan = SafeInv(Tan(fovYRad * static_cast<T>(0.5)));

        return Matrix44{
            invHalfTan * SafeInv(aspectXY), static_cast<T>(0.0), static_cast<T>(0.0),              static_cast<T>(0.0),
            static_cast<T>(0.0),            invHalfTan,          static_cast<T>(0.0),              static_cast<T>(0.0),
            static_cast<T>(0.0),            static_cast<T>(0.0), zFar / (zFar - zNear),            static_cast<T>(1.0),
            static_cast<T>(0.0),            static_cast<T>(0.0), -(zFar * zNear) / (zFar - zNear), static_cast<T>(0.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 Perspective_RH_Z_0_1(const T fovYRad, const T aspectXY, const T zNear, const T zFar) noexcept {

        const T invHalfTan = SafeInv(Tan(fovYRad * static_cast<T>(0.5)));

        return Matrix44{
            invHalfTan * SafeInv(aspectXY), static_cast<T>(0.0), static_cast<T>(0.0),              static_cast<T>(0.0),
            static_cast<T>(0.0),            invHalfTan,          static_cast<T>(0.0),              static_cast<T>(0.0),
            static_cast<T>(0.0),            static_cast<T>(0.0), -zFar / (zFar - zNear),           static_cast<T>(-1.0),
            static_cast<T>(0.0),            static_cast<T>(0.0), -(zFar * zNear) / (zFar - zNear), static_cast<T>(0.0),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 LookAt_RH(const Vec3<T>& eye, const Vec3<T>& center, const Vec3<T>& up) noexcept {

        const Vec3<T> f = (center - eye).Normalize();
        const Vec3<T> r = f.Cross(up).Normalize();
        const Vec3<T> u = r.Cross(f);

        return Matrix44{
            r.x,                  u.x,                  -f.x,                static_cast<T>(0.0),
            r.y,                  u.y,                  -f.y,                static_cast<T>(0.0),
            r.z,                  u.z,                  -f.z,                static_cast<T>(0.0),
            -Vec3<T>(r).Dot(eye), -Vec3<T>(u).Dot(eye), Vec3<T>(f).Dot(eye), static_cast<T>(1.0)
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Matrix44 LookAtByRight_RH(const Vec3<T>& eye, const Vec3<T>& center, const Vec3<T>& right) noexcept {

        const Vec3<T> f = (center - eye).Normalize();
        const Vec3<T> u = right.Cross(f).Normalize();
        const Vec3<T> r = f.Cross(u);

        return Matrix44{
            r.x,                  u.x,                  -f.x,                static_cast<T>(0.0),
            r.y,                  u.y,                  -f.y,                static_cast<T>(0.0),
            r.z,                  u.z,                  -f.z,                static_cast<T>(0.0),
            -Vec3<T>(r).Dot(eye), -Vec3<T>(u).Dot(eye), Vec3<T>(f).Dot(eye), static_cast<T>(1.0)
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Matrix44() = default;

    ALT_NODISCARD ALT_FORCE_INLINE Matrix44(
        const T m00, const T m01, const T m02, const T m03,
        const T m10, const T m11, const T m12, const T m13,
        const T m20, const T m21, const T m22, const T m23,
        const T m30, const T m31, const T m32, const T m33
    ) noexcept {

        m_Matrix[0][0] = m00; m_Matrix[0][1] = m01; m_Matrix[0][2] = m02; m_Matrix[0][3] = m03;
        m_Matrix[1][0] = m10; m_Matrix[1][1] = m11; m_Matrix[1][2] = m12; m_Matrix[1][3] = m13;
        m_Matrix[2][0] = m20; m_Matrix[2][1] = m21; m_Matrix[2][2] = m22; m_Matrix[2][3] = m23;
        m_Matrix[3][0] = m30; m_Matrix[3][1] = m31; m_Matrix[3][2] = m32; m_Matrix[3][3] = m33;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Matrix44(const Vec4<T>& c1, const Vec4<T>& c2, const Vec4<T>& c3, const Vec4<T>& c4) noexcept {

        m_Matrix[0][0] = c1.x;
        m_Matrix[0][1] = c1.y;
        m_Matrix[0][2] = c1.z;
        m_Matrix[0][3] = c1.w;

        m_Matrix[1][0] = c2.x;
        m_Matrix[1][1] = c2.y;
        m_Matrix[1][2] = c2.z;
        m_Matrix[1][3] = c2.w;

        m_Matrix[2][0] = c3.x;
        m_Matrix[2][1] = c3.y;
        m_Matrix[2][2] = c3.z;
        m_Matrix[2][3] = c3.w;

        m_Matrix[3][0] = c4.x;
        m_Matrix[3][1] = c4.y;
        m_Matrix[3][2] = c4.z;
        m_Matrix[3][3] = c4.w;
    }

    ALT_FORCE_INLINE Matrix44(Matrix44&& mat) noexcept {

        const T* ptr = &mat.m_Matrix[0][0];
        Memory::CopyObjectsNonOverlap(&m_Matrix[0][0], ptr, ptr + 16);
    }

    ALT_FORCE_INLINE Matrix44& operator=(Matrix44&& mat) noexcept {

        if (this == &mat) {
            return *this;
        }

        const T* ptr = &mat.m_Matrix[0][0];
        Memory::CopyObjectsNonOverlap(&m_Matrix[0][0], ptr, ptr + 16);

        return *this;
    }

    ALT_FORCE_INLINE Matrix44(const Matrix44& mat) noexcept {

        const T* ptr = &mat.m_Matrix[0][0];
        Memory::CopyObjectsNonOverlap(&m_Matrix[0][0], ptr, ptr + 16);
    }

    ALT_FORCE_INLINE Matrix44& operator=(const Matrix44& mat) noexcept {

        if (this == &mat) {
            return *this;
        }

        const T* ptr = &mat.m_Matrix[0][0];
        Memory::CopyObjectsNonOverlap(&m_Matrix[0][0], ptr, ptr + 16);
        return *this;
    }

    ALT_NODISCARD ALT_FORCE_INLINE T Det() const noexcept {

        const T A2323 = m_Matrix[2][2] * m_Matrix[3][3] - m_Matrix[2][3] * m_Matrix[3][2];
        const T A1323 = m_Matrix[2][1] * m_Matrix[3][3] - m_Matrix[2][3] * m_Matrix[3][1];
        const T A1223 = m_Matrix[2][1] * m_Matrix[3][2] - m_Matrix[2][2] * m_Matrix[3][1];
        const T A0323 = m_Matrix[2][0] * m_Matrix[3][3] - m_Matrix[2][3] * m_Matrix[3][0];
        const T A0223 = m_Matrix[2][0] * m_Matrix[3][2] - m_Matrix[2][2] * m_Matrix[3][0];
        const T A0123 = m_Matrix[2][0] * m_Matrix[3][1] - m_Matrix[2][1] * m_Matrix[3][0];

        const T det =
            + m_Matrix[0][0] * (m_Matrix[1][1] * A2323 - m_Matrix[1][2] * A1323 + m_Matrix[1][3] * A1223)
            - m_Matrix[0][1] * (m_Matrix[1][0] * A2323 - m_Matrix[1][2] * A0323 + m_Matrix[1][3] * A0223)
            + m_Matrix[0][2] * (m_Matrix[1][0] * A1323 - m_Matrix[1][1] * A0323 + m_Matrix[1][3] * A0123)
            - m_Matrix[0][3] * (m_Matrix[1][0] * A1223 - m_Matrix[1][1] * A0223 + m_Matrix[1][2] * A0123);

        return det;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Matrix44> Inverse() const noexcept {

        const T A2323 = m_Matrix[2][2] * m_Matrix[3][3] - m_Matrix[2][3] * m_Matrix[3][2];
        const T A1323 = m_Matrix[2][1] * m_Matrix[3][3] - m_Matrix[2][3] * m_Matrix[3][1];
        const T A1223 = m_Matrix[2][1] * m_Matrix[3][2] - m_Matrix[2][2] * m_Matrix[3][1];
        const T A0323 = m_Matrix[2][0] * m_Matrix[3][3] - m_Matrix[2][3] * m_Matrix[3][0];
        const T A0223 = m_Matrix[2][0] * m_Matrix[3][2] - m_Matrix[2][2] * m_Matrix[3][0];
        const T A0123 = m_Matrix[2][0] * m_Matrix[3][1] - m_Matrix[2][1] * m_Matrix[3][0];
        const T A2313 = m_Matrix[1][2] * m_Matrix[3][3] - m_Matrix[1][3] * m_Matrix[3][2];
        const T A1313 = m_Matrix[1][1] * m_Matrix[3][3] - m_Matrix[1][3] * m_Matrix[3][1];
        const T A1213 = m_Matrix[1][1] * m_Matrix[3][2] - m_Matrix[1][2] * m_Matrix[3][1];
        const T A2312 = m_Matrix[1][2] * m_Matrix[2][3] - m_Matrix[1][3] * m_Matrix[2][2];
        const T A1312 = m_Matrix[1][1] * m_Matrix[2][3] - m_Matrix[1][3] * m_Matrix[2][1];
        const T A1212 = m_Matrix[1][1] * m_Matrix[2][2] - m_Matrix[1][2] * m_Matrix[2][1];
        const T A0313 = m_Matrix[1][0] * m_Matrix[3][3] - m_Matrix[1][3] * m_Matrix[3][0];
        const T A0213 = m_Matrix[1][0] * m_Matrix[3][2] - m_Matrix[1][2] * m_Matrix[3][0];
        const T A0312 = m_Matrix[1][0] * m_Matrix[2][3] - m_Matrix[1][3] * m_Matrix[2][0];
        const T A0212 = m_Matrix[1][0] * m_Matrix[2][2] - m_Matrix[1][2] * m_Matrix[2][0];
        const T A0113 = m_Matrix[1][0] * m_Matrix[3][1] - m_Matrix[1][1] * m_Matrix[3][0];
        const T A0112 = m_Matrix[1][0] * m_Matrix[2][1] - m_Matrix[1][1] * m_Matrix[2][0];

        const T det =
            + m_Matrix[0][0] * (m_Matrix[1][1] * A2323 - m_Matrix[1][2] * A1323 + m_Matrix[1][3] * A1223)
            - m_Matrix[0][1] * (m_Matrix[1][0] * A2323 - m_Matrix[1][2] * A0323 + m_Matrix[1][3] * A0223)
            + m_Matrix[0][2] * (m_Matrix[1][0] * A1323 - m_Matrix[1][1] * A0323 + m_Matrix[1][3] * A0123)
            - m_Matrix[0][3] * (m_Matrix[1][0] * A1223 - m_Matrix[1][1] * A0223 + m_Matrix[1][2] * A0123);

        if (Abs(det) <= Num::Desc<T>::Eps()) {
            return Nullopt;
        }

        const T invDet = static_cast<T>(1.0) / det;

        return Matrix44{
            invDet *  (m_Matrix[1][1] * A2323 - m_Matrix[1][2] * A1323 + m_Matrix[1][3] * A1223),
            invDet * -(m_Matrix[0][1] * A2323 - m_Matrix[0][2] * A1323 + m_Matrix[0][3] * A1223),
            invDet *  (m_Matrix[0][1] * A2313 - m_Matrix[0][2] * A1313 + m_Matrix[0][3] * A1213),
            invDet * -(m_Matrix[0][1] * A2312 - m_Matrix[0][2] * A1312 + m_Matrix[0][3] * A1212),
            invDet * -(m_Matrix[1][0] * A2323 - m_Matrix[1][2] * A0323 + m_Matrix[1][3] * A0223),
            invDet *  (m_Matrix[0][0] * A2323 - m_Matrix[0][2] * A0323 + m_Matrix[0][3] * A0223),
            invDet * -(m_Matrix[0][0] * A2313 - m_Matrix[0][2] * A0313 + m_Matrix[0][3] * A0213),
            invDet *  (m_Matrix[0][0] * A2312 - m_Matrix[0][2] * A0312 + m_Matrix[0][3] * A0212),
            invDet *  (m_Matrix[1][0] * A1323 - m_Matrix[1][1] * A0323 + m_Matrix[1][3] * A0123),
            invDet * -(m_Matrix[0][0] * A1323 - m_Matrix[0][1] * A0323 + m_Matrix[0][3] * A0123),
            invDet *  (m_Matrix[0][0] * A1313 - m_Matrix[0][1] * A0313 + m_Matrix[0][3] * A0113),
            invDet * -(m_Matrix[0][0] * A1312 - m_Matrix[0][1] * A0312 + m_Matrix[0][3] * A0112),
            invDet * -(m_Matrix[1][0] * A1223 - m_Matrix[1][1] * A0223 + m_Matrix[1][2] * A0123),
            invDet *  (m_Matrix[0][0] * A1223 - m_Matrix[0][1] * A0223 + m_Matrix[0][2] * A0123),
            invDet * -(m_Matrix[0][0] * A1213 - m_Matrix[0][1] * A0213 + m_Matrix[0][2] * A0113),
            invDet *  (m_Matrix[0][0] * A1212 - m_Matrix[0][1] * A0212 + m_Matrix[0][2] * A0112),
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Matrix44 Transpose() const noexcept {

        Matrix44 res;

        res.m_Matrix[0][0] = m_Matrix[0][0];
        res.m_Matrix[0][1] = m_Matrix[1][0];
        res.m_Matrix[0][2] = m_Matrix[2][0];
        res.m_Matrix[0][3] = m_Matrix[3][0];
        res.m_Matrix[1][0] = m_Matrix[0][1];
        res.m_Matrix[1][1] = m_Matrix[1][1];
        res.m_Matrix[1][2] = m_Matrix[2][1];
        res.m_Matrix[1][3] = m_Matrix[3][1];
        res.m_Matrix[2][0] = m_Matrix[0][2];
        res.m_Matrix[2][1] = m_Matrix[1][2];
        res.m_Matrix[2][2] = m_Matrix[2][2];
        res.m_Matrix[2][3] = m_Matrix[3][2];
        res.m_Matrix[3][0] = m_Matrix[0][3];
        res.m_Matrix[3][1] = m_Matrix[1][3];
        res.m_Matrix[3][2] = m_Matrix[2][3];
        res.m_Matrix[3][3] = m_Matrix[3][3];

        return res;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Matrix44<T> ConvertToMatrixForNormals() const noexcept {
        return Transpose(Inverse(*this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> GetEulerAnglesXYZ() const noexcept {

        return Vec3<T>{
            Atan2(m_Matrix[2][1], m_Matrix[2][2]),
            Asin(-m_Matrix[2][0]),
            Atan2(m_Matrix[1][0], m_Matrix[0][0])
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> GetTranslation() const noexcept {

        return Vec3<T>{ GetCol(3) };
    }

    ALT_FORCE_INLINE void SetTranslation(const Vec3<T>& translation) noexcept {

        m_Matrix[3][0] = translation[0];
        m_Matrix[3][1] = translation[1];
        m_Matrix[3][2] = translation[2];
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> GetCol(const usize i) const noexcept {

        return Vec4<T>{ m_Matrix[i][0], m_Matrix[i][1], m_Matrix[i][2], m_Matrix[i][3] };
    }

    ALT_NODISCARD ALT_FORCE_INLINE T* GetPtr() noexcept {

        return &m_Matrix[0][0];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T* GetPtr() const noexcept {

        return &m_Matrix[0][0];
    }

    ALT_FORCE_INLINE Matrix44 operator*(const Matrix44& rhs) const noexcept {

        Matrix44 res;
        const Matrix44 lhsTransp = Transpose();

        res.m_Matrix[0][0] =
            lhsTransp.m_Matrix[0][0] * rhs.m_Matrix[0][0] +
            lhsTransp.m_Matrix[0][1] * rhs.m_Matrix[0][1] +
            lhsTransp.m_Matrix[0][2] * rhs.m_Matrix[0][2] +
            lhsTransp.m_Matrix[0][3] * rhs.m_Matrix[0][3];

        res.m_Matrix[0][1] =
            lhsTransp.m_Matrix[0][0] * rhs.m_Matrix[1][0] +
            lhsTransp.m_Matrix[0][1] * rhs.m_Matrix[1][1] +
            lhsTransp.m_Matrix[0][2] * rhs.m_Matrix[1][2] +
            lhsTransp.m_Matrix[0][3] * rhs.m_Matrix[1][3];

        res.m_Matrix[0][2] =
            lhsTransp.m_Matrix[0][0] * rhs.m_Matrix[2][0] +
            lhsTransp.m_Matrix[0][1] * rhs.m_Matrix[2][1] +
            lhsTransp.m_Matrix[0][2] * rhs.m_Matrix[2][2] +
            lhsTransp.m_Matrix[0][3] * rhs.m_Matrix[2][3];

        res.m_Matrix[0][3] =
            lhsTransp.m_Matrix[0][0] * rhs.m_Matrix[3][0] +
            lhsTransp.m_Matrix[0][1] * rhs.m_Matrix[3][1] +
            lhsTransp.m_Matrix[0][2] * rhs.m_Matrix[3][2] +
            lhsTransp.m_Matrix[0][3] * rhs.m_Matrix[3][3];

        res.m_Matrix[1][0] =
            lhsTransp.m_Matrix[1][0] * rhs.m_Matrix[0][0] +
            lhsTransp.m_Matrix[1][1] * rhs.m_Matrix[0][1] +
            lhsTransp.m_Matrix[1][2] * rhs.m_Matrix[0][2] +
            lhsTransp.m_Matrix[1][3] * rhs.m_Matrix[0][3];

        res.m_Matrix[1][1] =
            lhsTransp.m_Matrix[1][0] * rhs.m_Matrix[1][0] +
            lhsTransp.m_Matrix[1][1] * rhs.m_Matrix[1][1] +
            lhsTransp.m_Matrix[1][2] * rhs.m_Matrix[1][2] +
            lhsTransp.m_Matrix[1][3] * rhs.m_Matrix[1][3];

        res.m_Matrix[1][2] =
            lhsTransp.m_Matrix[1][0] * rhs.m_Matrix[2][0] +
            lhsTransp.m_Matrix[1][1] * rhs.m_Matrix[2][1] +
            lhsTransp.m_Matrix[1][2] * rhs.m_Matrix[2][2] +
            lhsTransp.m_Matrix[1][3] * rhs.m_Matrix[2][3];

        res.m_Matrix[1][3] =
            lhsTransp.m_Matrix[1][0] * rhs.m_Matrix[3][0] +
            lhsTransp.m_Matrix[1][1] * rhs.m_Matrix[3][1] +
            lhsTransp.m_Matrix[1][2] * rhs.m_Matrix[3][2] +
            lhsTransp.m_Matrix[1][3] * rhs.m_Matrix[3][3];

        res.m_Matrix[2][0] =
            lhsTransp.m_Matrix[2][0] * rhs.m_Matrix[0][0] +
            lhsTransp.m_Matrix[2][1] * rhs.m_Matrix[0][1] +
            lhsTransp.m_Matrix[2][2] * rhs.m_Matrix[0][2] +
            lhsTransp.m_Matrix[2][3] * rhs.m_Matrix[0][3];

        res.m_Matrix[2][1] =
            lhsTransp.m_Matrix[2][0] * rhs.m_Matrix[1][0] +
            lhsTransp.m_Matrix[2][1] * rhs.m_Matrix[1][1] +
            lhsTransp.m_Matrix[2][2] * rhs.m_Matrix[1][2] +
            lhsTransp.m_Matrix[2][3] * rhs.m_Matrix[1][3];

        res.m_Matrix[2][2] =
            lhsTransp.m_Matrix[2][0] * rhs.m_Matrix[2][0] +
            lhsTransp.m_Matrix[2][1] * rhs.m_Matrix[2][1] +
            lhsTransp.m_Matrix[2][2] * rhs.m_Matrix[2][2] +
            lhsTransp.m_Matrix[2][3] * rhs.m_Matrix[2][3];

        res.m_Matrix[2][3] =
            lhsTransp.m_Matrix[2][0] * rhs.m_Matrix[3][0] +
            lhsTransp.m_Matrix[2][1] * rhs.m_Matrix[3][1] +
            lhsTransp.m_Matrix[2][2] * rhs.m_Matrix[3][2] +
            lhsTransp.m_Matrix[2][3] * rhs.m_Matrix[3][3];

        res.m_Matrix[3][0] =
            lhsTransp.m_Matrix[3][0] * rhs.m_Matrix[0][0] +
            lhsTransp.m_Matrix[3][1] * rhs.m_Matrix[0][1] +
            lhsTransp.m_Matrix[3][2] * rhs.m_Matrix[0][2] +
            lhsTransp.m_Matrix[3][3] * rhs.m_Matrix[0][3];

        res.m_Matrix[3][1] =
            lhsTransp.m_Matrix[3][0] * rhs.m_Matrix[1][0] +
            lhsTransp.m_Matrix[3][1] * rhs.m_Matrix[1][1] +
            lhsTransp.m_Matrix[3][2] * rhs.m_Matrix[1][2] +
            lhsTransp.m_Matrix[3][3] * rhs.m_Matrix[1][3];

        res.m_Matrix[3][2] =
            lhsTransp.m_Matrix[3][0] * rhs.m_Matrix[2][0] +
            lhsTransp.m_Matrix[3][1] * rhs.m_Matrix[2][1] +
            lhsTransp.m_Matrix[3][2] * rhs.m_Matrix[2][2] +
            lhsTransp.m_Matrix[3][3] * rhs.m_Matrix[2][3];

        res.m_Matrix[3][3] =
            lhsTransp.m_Matrix[3][0] * rhs.m_Matrix[3][0] +
            lhsTransp.m_Matrix[3][1] * rhs.m_Matrix[3][1] +
            lhsTransp.m_Matrix[3][2] * rhs.m_Matrix[3][2] +
            lhsTransp.m_Matrix[3][3] * rhs.m_Matrix[3][3];

        return res.Transpose();
    }

    ALT_FORCE_INLINE Vec4<T> operator*(const Vec4<T>& vec4) const noexcept {

        return Vec4<T>{
            m_Matrix[0][0] * vec4[0] + m_Matrix[0][1] * vec4[1] + m_Matrix[0][2] * vec4[2] + m_Matrix[0][3] * vec4[3],
            m_Matrix[1][0] * vec4[0] + m_Matrix[1][1] * vec4[1] + m_Matrix[1][2] * vec4[2] + m_Matrix[1][3] * vec4[3],
            m_Matrix[2][0] * vec4[0] + m_Matrix[2][1] * vec4[1] + m_Matrix[2][2] * vec4[2] + m_Matrix[2][3] * vec4[3],
            m_Matrix[3][0] * vec4[0] + m_Matrix[3][1] * vec4[1] + m_Matrix[3][2] * vec4[2] + m_Matrix[3][3] * vec4[3],
        };
    }

    ALT_FORCE_INLINE Vec3<T> operator*(const Vec3<T>& vec3) const noexcept {

        return Vec3<T>{
            m_Matrix[0][0] * vec3[0] + m_Matrix[0][1] * vec3[1] + m_Matrix[0][2] * vec3[2],
            m_Matrix[1][0] * vec3[0] + m_Matrix[1][1] * vec3[1] + m_Matrix[1][2] * vec3[2],
            m_Matrix[2][0] * vec3[0] + m_Matrix[2][1] * vec3[1] + m_Matrix[2][2] * vec3[2],
        };
    }

    ALT_FORCE_INLINE Point3<T> operator*(const Point3<T>& vec3) const noexcept {

        return Point3<T>{
            m_Matrix[0][0] * vec3[0] + m_Matrix[0][1] * vec3[1] + m_Matrix[0][2] * vec3[2] + m_Matrix[0][3] * vec3[3],
            m_Matrix[1][0] * vec3[0] + m_Matrix[1][1] * vec3[1] + m_Matrix[1][2] * vec3[2] + m_Matrix[1][3] * vec3[3],
            m_Matrix[2][0] * vec3[0] + m_Matrix[2][1] * vec3[1] + m_Matrix[2][2] * vec3[2] + m_Matrix[2][3] * vec3[3],
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE T& operator()(const usize iRow, const usize iCol) noexcept {

        return m_Matrix[iCol][iRow];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator()(const usize iRow, const usize iCol) const noexcept {

        return m_Matrix[iCol][iRow];
    }

private:

    T m_Matrix[4][4];
};

using Matrix44f32 = Matrix44<f32>;
using Matrix44f64 = Matrix44<f64>;

} // namespace Math

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<Math::Matrix44<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Format specialization for Matrix44<T>.
 */
template <typename T>
class Format<Math::Matrix44<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Math::Matrix44<T>& val) noexcept {
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
                EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(0, 0)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(0, 1)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(0, 2)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(0, 3)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                result.size += estimator.GetIndentEstimatedSize();
                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(1, 0)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(1, 1)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(1, 2)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(1, 3)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                result.size += estimator.GetIndentEstimatedSize();
                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(2, 0)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(2, 1)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(2, 2)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(2, 3)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                result.size += estimator.GetIndentEstimatedSize();
                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(3, 0)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(3, 1)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(3, 2)) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(3, 3)) iferr_return Nullopt;
                result.size += estimated.size + 2;
            }
            estimator.PopIndent();

            result.size += estimator.GetIndentEstimatedSize() + 1;

            return result;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            EstimateExactSizeResult result{};
            result.size = EstimateSizeTypeName(CleanU{}) + 1;

            EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(0, 0)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(0, 1)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(0, 2)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(0, 3)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(1, 0)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(1, 1)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(1, 2)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(1, 3)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(2, 0)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(2, 1)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(2, 2)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(2, 3)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(3, 0)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(3, 1)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(3, 2)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val(3, 3)) iferr_return Nullopt;
            result.size += estimated.size + 2;

            result.size += 1;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Math::Matrix44<T>& val) noexcept {
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
                Format<CleanT>::WriteAppend(writer, parametersNext, val(0, 0)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(0, 1)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(0, 2)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(0, 3)) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append('\n') iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val(1, 0)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(1, 1)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(1, 2)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(1, 3)) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append('\n') iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val(2, 0)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(2, 1)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(2, 2)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(2, 3)) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append('\n') iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val(3, 0)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(3, 1)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(3, 2)) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val(3, 3)) iferr_return false;
                writer.Append(';') iferr_return false;
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

            Format<CleanT>::WriteAppend(writer, parametersNext, val(0, 0)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(0, 1)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(0, 2)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(0, 3)) iferr_return false;
            writer.Append(';') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(1, 0)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(1, 1)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(1, 2)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(1, 3)) iferr_return false;
            writer.Append(';') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(2, 0)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(2, 1)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(2, 2)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(2, 3)) iferr_return false;
            writer.Append(';') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(3, 0)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(3, 1)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(3, 2)) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val(3, 3)) iferr_return false;
            writer.Append(';') iferr_return false;
            writer.Append(' ') iferr_return false;

            writer.Append('}') iferr_return false;

            return true;
        }

        return false;
    }

private:

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(U) noexcept {
        return 9;
    }

    template <typename U, typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, U) noexcept {
        return writer.Append("Matrix44{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f32) noexcept {
        return 12;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f32) noexcept {
        return writer.Append("Matrix44f32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f64) noexcept {
        return 12;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f64) noexcept {
        return writer.Append("Matrix44f64{");
    }
};

} // namespace Alt

#endif // __ALT_STD_MATH_MATRIX44__