#include "Vec3.h"
#include "Vec4.h"
#include "Quat.h"

namespace Alt {
namespace Math {

template <typename T>
ALT_FORCE_INLINE Vec4<T>::Vec4(const Vec3<T>& vec3, const T v4) noexcept
    : vec{ vec3.x, vec3.y, vec3.z, v4 }
{}

template <typename T>
ALT_FORCE_INLINE Vec4<T>::Vec4(const Quat<T>& quat) noexcept
    : vec{ quat.x, quat.y, quat.z, quat.w }
{}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::xyzw() const noexcept {
    return *this;
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::xywz() const noexcept {
    return Vec4<T>(x, y, w, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::xzyw() const noexcept {
    return Vec4<T>(x, z, y, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::xzwy() const noexcept {
    return Vec4<T>(x, z, w, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::xwzy() const noexcept {
    return Vec4<T>(x, w, z, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::xwyz() const noexcept {
    return Vec4<T>(x, w, y, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::yxzw() const noexcept {
    return Vec4<T>(y, x, z, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::yxwz() const noexcept {
    return Vec4<T>(y, x, w, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::yzxw() const noexcept {
    return Vec4<T>(y, z, x, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::yzwx() const noexcept {
    return Vec4<T>(y, z, w, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::ywxz() const noexcept {
    return Vec4<T>(y, w, x, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::ywzx() const noexcept {
    return Vec4<T>(y, w, z, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::zxyw() const noexcept {
    return Vec4<T>(z, x, y, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::zxwy() const noexcept {
    return Vec4<T>(z, x, w, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::zyxw() const noexcept {
    return Vec4<T>(z, y, x, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::zywx() const noexcept {
    return Vec4<T>(z, y, w, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::zwxy() const noexcept {
    return Vec4<T>(z, w, x, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::zwyx() const noexcept {
    return Vec4<T>(z, w, y, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::wxyz() const noexcept {
    return Vec4<T>(w, x, y, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::wxzy() const noexcept {
    return Vec4<T>(w, x, z, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::wyxz() const noexcept {
    return Vec4<T>(w, y, x, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::wyzx() const noexcept {
    return Vec4<T>(w, y, z, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::wzxy() const noexcept {
    return Vec4<T>(w, z, x, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec4<T> Vec4<T>::wzyx() const noexcept {
    return Vec4<T>(w, z, y, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::xyz() const noexcept {
    return Vec3<T>(x, y, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::xzy() const noexcept {
    return Vec3<T>(x, z, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::yxz() const noexcept {
    return Vec3<T>(y, x, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::yzx() const noexcept {
    return Vec3<T>(y, z, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::zxy() const noexcept {
    return Vec3<T>(z, x, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::zyx() const noexcept {
    return Vec3<T>(z, y, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::xyw() const noexcept {
    return Vec3<T>(x, y, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::xwy() const noexcept {
    return Vec3<T>(x, w, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::yxw() const noexcept {
    return Vec3<T>(y, x, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::ywx() const noexcept {
    return Vec3<T>(y, w, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::wxy() const noexcept {
    return Vec3<T>(w, x, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::wyx() const noexcept {
    return Vec3<T>(w, y, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::xwz() const noexcept {
    return Vec3<T>(x, w, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::xzw() const noexcept {
    return Vec3<T>(x, z, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::wxz() const noexcept {
    return Vec3<T>(w, x, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::wzx() const noexcept {
    return Vec3<T>(w, z, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::zxw() const noexcept {
    return Vec3<T>(z, x, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::zwx() const noexcept {
    return Vec3<T>(z, w, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::wyz() const noexcept {
    return Vec3<T>(w, y, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::wzy() const noexcept {
    return Vec3<T>(w, z, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::ywz() const noexcept {
    return Vec3<T>(y, w, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::yzw() const noexcept {
    return Vec3<T>(y, z, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::zwy() const noexcept {
    return Vec3<T>(z, w, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec4<T>::zyw() const noexcept {
    return Vec3<T>(z, y, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::xy() const noexcept {
    return Vec2<T>(x, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::xz() const noexcept {
    return Vec2<T>(x, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::yx() const noexcept {
    return Vec2<T>(y, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::yz() const noexcept {
    return Vec2<T>(y, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::zx() const noexcept {
    return Vec2<T>(z, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::zy() const noexcept {
    return Vec2<T>(z, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::xw() const noexcept {
    return Vec2<T>(x, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::wx() const noexcept {
    return Vec2<T>(w, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::wz() const noexcept {
    return Vec2<T>(w, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::zw() const noexcept {
    return Vec2<T>(z, w);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::wy() const noexcept {
    return Vec2<T>(w, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec4<T>::yw() const noexcept {
    return Vec2<T>(y, w);
}

} // namespace Math
} // namespace Alt