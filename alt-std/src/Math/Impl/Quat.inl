#include "Quat.h"
#include "Vec3.h"
#include "Matrix44.h"

namespace Alt {
namespace Math {

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Quat<T> Quat<T>::FromRotate(Vec3<T> axis, const T angleRad) noexcept {

    axis = axis.Normalize();

    const T sin = Sin(angleRad * static_cast<T>(0.5));
    const T cos = Cos(angleRad * static_cast<T>(0.5));

    return Quat{ axis.x * sin, axis.y * sin, axis.z * sin, cos };
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Quat<T> Quat<T>::FromVectors(Vec3<T> orig, Vec3<T> dst) noexcept {

    orig = orig.Normalize();
    dst = dst.Normalize();

    T cosTheta = orig.Dot(dst);
    T w = static_cast<T>(1.0) + cosTheta;

    if (w >= Num::Desc<T>::Eps()) {
        return Normalize(Quat{ orig.Cross(dst), w });
    }
    else {
        return Quat{ orig.PerpNormalized(), static_cast<T>(0.0) };
    }
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Quat<T>::GetAxis() const noexcept {

    const T a = static_cast<T>(1.0) - w * w;
    if (a <= static_cast<T>(0.0)) {
        return Vec3{ 0, 0, 1 };
    }

    const T inv = static_cast<T>(1.0) / a;
    return Vec3{ x * inv, y * inv, z * inv };
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T Quat<T>::GetAngle() const noexcept {

    if (Abs(w) > Cos(static_cast<T>(0.5))) {

        const T a = Asin(Sqrt(x * x + y * y + z * z)) * static_cast<T>(2.0);
        if (w < static_cast<T>(0.0)) {
            return static_cast<T>(ALT_MATH_PI) * static_cast<T>(2.0) - a;
        }

        return a;
    }

    return Acos(w) * static_cast<T>(2.0);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Quat<T>::GetEulerAnglesXYZ() const noexcept {

    const Quat& q = *this;

    T pitch = static_cast<T>(0.0);
    {
        const T y = static_cast<T>(2.0) * (q.y * q.z + q.w * q.x);
        const T x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

        if (Abs(x) < Num::Desc<T>::Eps() && Abs(y) < Num::Desc<T>::Eps()) {
            pitch = static_cast<T>(2.0) * Atan2(q.x, q.w);
        }
        else {
            pitch = Atan2(y, x);
        }
    }

    const T yaw = Asin(Clamp(-static_cast<T>(2.0) * (q.x * q.z - q.w * q.y), static_cast<T>(-1), static_cast<T>(1)));

    T roll = static_cast<T>(0.0);
    {
        const T y = static_cast<T>(2.0) * (q.x * q.y + q.w * q.z);
        const T x = q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z;

        if (Abs(x) < Num::Desc<T>::Eps() && Abs(y) < Num::Desc<T>::Eps()) {
            roll = static_cast<T>(0.0);
        }
        else {
            roll = Atan2(y, x);
        }
    }

    return Vec3<T>{ pitch, yaw, roll };
}

template <typename T>
ALT_FORCE_INLINE Vec3<T> Quat<T>::operator*(const Vec3<T>& rhs) const noexcept {

    const Vec3<T> quatVec{ x, y, z };
    const Vec3<T> a = quatVec.Cross(rhs);
    const Vec3<T> b = quatVec.Cross(a);

    return rhs + (a * w + b) * static_cast<T>(2.0);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Quat<T> Quat<T>::FromMatrix44(const Matrix44<T>& m) noexcept {

    const T tr = m(0, 0) + m(1, 1) + m(2, 2);

    if (tr >= static_cast<T>(0.0)) {

        const T s = Sqrt(tr + static_cast<T>(1.0));
        const T is = static_cast<T>(0.5) / s;

        return Quat{
            static_cast<T>((m(2, 1) - m(1, 2)) * is),
            static_cast<T>((m(0, 2) - m(2, 0)) * is),
            static_cast<T>((m(1, 0) - m(0, 1)) * is),
            static_cast<T>(static_cast<T>(0.5) * s)
        };
    }

    usize i = 0;
    if (m(1, 1) > m(0, 0)) i = 1;
    if (m(2, 2) > m(i, i)) i = 2;

    if (i == 0) {

        const T s = Sqrt(m(0, 0) - (m(1, 1) + m(2, 2)) + static_cast<T>(1.0));
        const T is = static_cast<T>(0.5) / s;
        return Quat{
            static_cast<T>(static_cast<T>(0.5) * s),
            static_cast<T>((m(0, 1) + m(1, 0)) * is),
            static_cast<T>((m(2, 0) + m(0, 2)) * is),
            static_cast<T>((m(2, 1) - m(1, 2)) * is)
        };
    }

    if (i == 1) {

        const T s = Sqrt(m(1, 1) - (m(2, 2) + m(0, 0)) + static_cast<T>(1.0));
        const T is = static_cast<T>(0.5) / s;
        return Quat{
            static_cast<T>((m(0, 1) + m(1, 0)) * is),
            static_cast<T>(static_cast<T>(0.5) * s),
            static_cast<T>((m(1, 2) + m(2, 1)) * is),
            static_cast<T>((m(0, 2) - m(2, 0)) * is)
        };
    }

    // ALT_ASSERT(i == 2);

    const T s = Sqrt(m(2, 2) - (m(0, 0) + m(1, 1)) + static_cast<T>(1.0));
    const T is = static_cast<T>(0.5) / s;
    return Quat{
        static_cast<T>((m(2, 0) + m(0, 2)) * is),
        static_cast<T>((m(1, 2) + m(2, 1)) * is),
        static_cast<T>(static_cast<T>(0.5) * s),
        static_cast<T>((m(1, 0) - m(0, 1)) * is)
    };
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Matrix44<T> Quat<T>::ToMatrix44() const noexcept {

    const T tx = x + x;
    const T ty = y + y;
    const T tz = z + z;

    const T xx = tx * x;
    const T yy = ty * y;
    const T zz = tz * z;
    const T xy = tx * y;
    const T xz = tx * z;
    const T xw = tx * w;
    const T yz = ty * z;
    const T yw = ty * w;
    const T zw = tz * w;

    return Matrix44{
        (static_cast<T>(1.0) - yy) - zz, xy + zw, xz - yw, static_cast<T>(0.0),
        xy - zw, (static_cast<T>(1.0) - zz) - xx, yz + xw, static_cast<T>(0.0),
        xz + yw, yz - xw, (static_cast<T>(1.0) - xx) - yy, static_cast<T>(0.0),
        static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0),
    };
}

} // namespace Math
} // namespace Alt
