#include "DualQuat.h"

#include "Vec3.h"
#include "Matrix44.h"

namespace Alt {
namespace Math {

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE DualQuat<T> DualQuat<T>::FromMatrix44(const Matrix44<T>& m) noexcept {

    return DualQuat::FromQuatAndTranslation(Quat<T>::FromMatrix44(m), m.GetTranslation());
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Matrix44<T> DualQuat<T>::ToMatrix44() const noexcept {

    Matrix44<T> mat = real.ToMatrix44();
    mat.SetTranslation(GetTranslation());

    return mat;
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> DualQuat<T>::operator*(const Vec3<T>& rhs) const noexcept {

    const Vec3<T> real_v3{ real.x, real.y, real.z };
    const Vec3<T> dual_v3{ dual.x, dual.y, dual.z };

    return (real_v3.Cross(real_v3.Cross(rhs) + rhs * real.w + dual_v3) + dual_v3 * real.w - real_v3 * dual.w) * static_cast<T>(2) + rhs;
}

} // namespace Math
} // namespace Alt
