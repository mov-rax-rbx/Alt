#include "Vec3.h"
#include "Vec4.h"

namespace Alt {
namespace Math {

template <typename T>
ALT_FORCE_INLINE Vec3<T>::Vec3(const Vec4<T>& vec4) noexcept
    : vec{ vec4.x, vec4.y, vec4.z }
{}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec3<T>::xyz() const noexcept {
    return *this;
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec3<T>::xzy() const noexcept {
    return Vec3<T>(x, z, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec3<T>::yxz() const noexcept {
    return Vec3<T>(y, x, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec3<T>::yzx() const noexcept {
    return Vec3<T>(y, z, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec3<T>::zxy() const noexcept {
    return Vec3<T>(z, x, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> Vec3<T>::zyx() const noexcept {
    return Vec3<T>(z, y, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec3<T>::xy() const noexcept {
    return Vec2(x, y);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec3<T>::xz() const noexcept {
    return Vec2(x, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec3<T>::yx() const noexcept {
    return Vec2(y, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec3<T>::yz() const noexcept {
    return Vec2(y, z);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec3<T>::zx() const noexcept {
    return Vec2(z, x);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec2<T> Vec3<T>::zy() const noexcept {
    return Vec2(z, y);
}

} // namespace Math
} // namespace Alt