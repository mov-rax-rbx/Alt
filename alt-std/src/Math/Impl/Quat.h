#pragma once

#ifndef __ALT_STD_MATH_QUAT__
#define __ALT_STD_MATH_QUAT__

#include "../../String/Impl/FormatPreInclude.h"

#include "MathPreInclude.h"

namespace Alt {
namespace Math {

template <typename T>
class Vec3;

template <typename T>
class Matrix44;

template <typename T>
class Quat {
public:

    using Type = T;

    ALT_FORCE_INLINE Quat() noexcept = default;

    ALT_FORCE_INLINE explicit Quat(const Vec3<T>& v, const T w) noexcept
        : quat{ v.x, v.y, v.z, w }
    {}

    ALT_FORCE_INLINE explicit Quat(const T x, const T y, const T z, const T w) noexcept
        : quat{ x, y, z, w }
    {}

    ALT_NODISCARD ALT_FORCE_INLINE static Quat Identity() noexcept {
        return Quat{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0) };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Quat FromRotate(Vec3<T> axis, const T angleRad) noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE static Quat FromVectors(Vec3<T> orig, Vec3<T> dst) noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE static Quat FromMatrix44(const Matrix44<T>& m) noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE Matrix44<T> ToMatrix44() const noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE T Dot(const Quat& q) const noexcept {
        return x * q.x + y * q.y + z * q.z + w * q.w;
    }

    ALT_NODISCARD ALT_FORCE_INLINE T Length() const noexcept {
        return Sqrt(Dot(*this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Quat Normalize() const noexcept {
        return *this * Rsqrt(Dot(*this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Quat Conjugate() const noexcept {
        return Quat{ -x, -y, -z, w };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Quat Inverse() const noexcept {
        return Conjugate() * SafeInv(Dot(*this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Quat Lerp(const Quat& dst, T t) const noexcept {
        return *this * (static_cast<T>(1.0) - t) + dst * t;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Quat Slerp(const Quat& dst, T t) const noexcept {

        const T cosTheta = quat[0] * dst[0] + quat[1] * dst[1] + quat[2] * dst[2] + quat[3] * dst[3];
        const T absCosTheta = Abs(cosTheta);

        T scale0;
        T scale1;

        // linear interpolation
        if (absCosTheta >= static_cast<T>(1.0) - Num::Desc<T>::Eps()) {

            scale0 = static_cast<T>(1.0) - t;
            scale1 = t;
        }
        else {

            const T theta = Acos(absCosTheta);
            const T sinTheta = Sin(theta);

            scale0 = Sin((static_cast<T>(1.0) - t) * theta) / sinTheta;
            scale1 = Sin((t * theta)) / sinTheta;
        }

        if (cosTheta < static_cast<T>(0.0)) {
            scale1 = -scale1;
        }

        return scale0 * *this + scale1 * dst;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> GetAxis() const noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE T GetAngle() const noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> GetEulerAnglesXYZ() const noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        return quat[i];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T operator[](const usize i) const noexcept {
        return quat[i];
    }

    ALT_FORCE_INLINE Quat operator*(const Quat& rhs) const noexcept {

        return Quat{
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
            w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
        };
    }

    ALT_FORCE_INLINE Vec3<T> operator*(const Vec3<T>& rhs) const noexcept;

    ALT_FORCE_INLINE Quat operator*(const T s) const noexcept {
        return Quat{ x * s, y * s, z * s, w * s };
    }

    ALT_FORCE_INLINE Quat& operator*=(const T s) noexcept {

        *this = *this * s;
        return *this;
    }

    ALT_FORCE_INLINE Quat operator/(const T s) const noexcept {

        return *this * SafeInv(s);
    }

    ALT_FORCE_INLINE Quat& operator/=(const T s) noexcept {

        *this = *this / s;
        return *this;
    }

    ALT_FORCE_INLINE Quat operator+(const Quat& rhs) const noexcept {
        return Quat{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }

    ALT_FORCE_INLINE Quat& operator+=(const Quat& rhs) noexcept {

        *this = *this + rhs;
        return *this;
    }

    ALT_FORCE_INLINE Quat operator-(const Quat& rhs) const noexcept {
        return Quat{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }

    ALT_FORCE_INLINE Quat& operator-=(const Quat& rhs) noexcept {

        *this = *this - rhs;
        return *this;
    }

    ALT_FORCE_INLINE friend Quat operator*(const T s, const Quat& q) noexcept {
        return q * s;
    }

public:

    union {

        T quat[4];

        struct { T x, y, z, w; };
    };
};

} // namespace Math
} // namespace Alt

#include "Quat.inl"

namespace Alt {
namespace Math {

using Quatf32 = Quat<f32>;
using Quatf64 = Quat<f64>;

} // namespace Math

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<Math::Quat<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Format specialization for Quat<T>.
 */
template <typename T>
class Format<Math::Quat<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Math::Quat<T>& val) noexcept {
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
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Math::Quat<T>& val) noexcept {
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
        return writer.Append("Quat{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f32) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f32) noexcept {
        return writer.Append("Quatf32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f64) noexcept {
        return 8;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f64) noexcept {
        return writer.Append("Quatf64{");
    }
};

} // namespace Alt

#endif // __ALT_STD_MATH_QUAT__