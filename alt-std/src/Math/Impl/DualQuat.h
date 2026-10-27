#pragma once

#ifndef __ALT_STD_MATH_DUAL_QUAT__
#define __ALT_STD_MATH_DUAL_QUAT__

#include "../../String/Impl/FormatPreInclude.h"

#include "MathPreInclude.h"

#include "Quat.h"

#ifdef ALT_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4201 /* nonstandard extension used: nameless struct/union */)
#pragma warning(disable: 4458 /* declaration of '...' hides class member */)
#endif // ALT_COMPILER_MSVC

namespace Alt {
namespace Math {

template <typename T>
class Vec3;

template <typename T>
class Matrix44;

template <typename T>
class DualQuat {
public:

    using Type = T;

    ALT_FORCE_INLINE DualQuat() noexcept = default;

    ALT_FORCE_INLINE explicit DualQuat(const Quat<T>& real) noexcept
        : real{ real }
    {}

    ALT_NODISCARD ALT_FORCE_INLINE static DualQuat Identity() noexcept {

        DualQuat dualQuat;
        dualQuat.real = Quat<T>::Identity();
        dualQuat.dual = Quat<T>{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0) };

        return dualQuat;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static DualQuat FromQuatAndTranslation(const Quat<T>& quat, const Vec3<T>& translation) noexcept {

        DualQuat dualQuat;

        dualQuat.real = quat;
        dualQuat.dual = Quat<T>{
            static_cast<T>( 0.5) * (translation.x * quat.w + translation.y * quat.z - translation.z * quat.y),
            static_cast<T>( 0.5) * (-translation.x * quat.z + translation.y * quat.w + translation.z * quat.x),
            static_cast<T>( 0.5) * (translation.x * quat.y - translation.y * quat.x + translation.z * quat.w),
            static_cast<T>(-0.5) * (translation.x * quat.x + translation.y * quat.y + translation.z * quat.z),
        };

        return dualQuat;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static DualQuat FromMatrix44(const Matrix44<T>& m) noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE Matrix44<T> ToMatrix44() const noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE DualQuat Normalize() const noexcept {
        return *this * Rsqrt(real.LengthSq());
    }

    ALT_NODISCARD ALT_FORCE_INLINE DualQuat Conjugate() const noexcept {

        DualQuat self;
        self.real = real.Conjugate();
        self.dual = dual.Conjugate();

        return self;
    }

    ALT_NODISCARD ALT_FORCE_INLINE DualQuat Inverse() const noexcept {

        const DualQuat self = Conjugate();

        DualQuat result;
        result.real = self.real;
        result.dual = self.dual + (self.real * (-static_cast<T>(2) * self.real.Dot(self.dual)));

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE DualQuat Lerp(const Quat<T>& dst, T t) const noexcept {

        const T k = real.Dot(dst.real) < static_cast<T>(0.0) ? -t : t;
        return *this * (static_cast<T>(1) - t) + dst * k;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3<T> GetTranslation() const noexcept {

        const Quat<T> quatReal = real.Conjugate();
        const Quat<T> quatDual = static_cast<T>(2.0) * dual;

        return Vec3<T>{
            quatDual.w * quatReal.x + quatDual.x * quatReal.w + quatDual.y * quatReal.z - quatDual.z * quatReal.y,
            quatDual.w * quatReal.y + quatDual.y * quatReal.w + quatDual.z * quatReal.x - quatDual.x * quatReal.z,
            quatDual.w * quatReal.z + quatDual.z * quatReal.w + quatDual.x * quatReal.y - quatDual.y * quatReal.x,
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        return dualQuat[i];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T operator[](const usize i) const noexcept {
        return dualQuat[i];
    }

    ALT_FORCE_INLINE DualQuat operator*(const DualQuat& rhs) const noexcept {

        DualQuat dualQuat;
        dualQuat.real = real * rhs.real;
        dualQuat.dual = real * rhs.dual + dual * rhs.real;

        return dualQuat;
    }

    ALT_FORCE_INLINE Vec3<T> operator*(const Vec3<T>& rhs) const noexcept;

    ALT_FORCE_INLINE DualQuat operator*(const T s) const noexcept {

        DualQuat dualQuat;
        dualQuat.real = real * s;
        dualQuat.dual = dual * s;

        return dualQuat;
    }

    ALT_FORCE_INLINE DualQuat operator/(const T s) const noexcept {
        return *this * SafeInv(s);
    }

    ALT_FORCE_INLINE DualQuat operator+(const DualQuat& rhs) const noexcept {

        DualQuat dualQuat;
        dualQuat.real = real + rhs.real;
        dualQuat.dual = dual + rhs.dual;

        return dualQuat;
    }

    ALT_FORCE_INLINE DualQuat operator-(const DualQuat& rhs) const noexcept {

        DualQuat dualQuat;
        dualQuat.real = real - rhs.real;
        dualQuat.dual = dual - rhs.dual;

        return dualQuat;
    }

    ALT_FORCE_INLINE friend DualQuat operator*(const T s, const DualQuat& q) noexcept {
        return q * s;
    }

public:

    static_assert((sizeof(Quat<T>) * 2) / sizeof(T) == 8, "");

    union {

        struct {

            Quat<T> real;
            Quat<T> dual;
        };

        T dualQuat[8];
    };
};

} // namespace Math
} // namespace Alt

#include "DualQuat.inl"

namespace Alt {
namespace Math {

using DualQuatf32 = DualQuat<f32>;
using DualQuatf64 = DualQuat<f64>;

} // namespace Math

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<Math::DualQuat<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Format specialization for DualQuat<T>.
 */
template <typename T>
class Format<Math::DualQuat<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Math::DualQuat<T>& val) noexcept {
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
                EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.real.x) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.real.y) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.real.z) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.real.w) iferr_return Nullopt;
                result.size += estimated.size + 2;

                result.size += estimator.GetIndentEstimatedSize();
                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.dual.x) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.dual.y) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.dual.z) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.dual.w) iferr_return Nullopt;
                result.size += estimated.size + 2;
            }
            estimator.PopIndent();

            result.size += estimator.GetIndentEstimatedSize() + 1;

            return result;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            EstimateExactSizeResult result{};
            result.size = EstimateSizeTypeName(CleanU{}) + 1;

            EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.real.x) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.real.y) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.real.z) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.real.w) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.dual.x) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.dual.y) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.dual.z) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.dual.w) iferr_return Nullopt;
            result.size += estimated.size + 2;

            result.size += 1;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Math::DualQuat<T>& val) noexcept {
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
                Format<CleanT>::WriteAppend(writer, parametersNext, val.real.x) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.real.y) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.real.z) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.real.w) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append('\n') iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val.dual.x) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.dual.y) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.dual.z) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.dual.w) iferr_return false;
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

            Format<CleanT>::WriteAppend(writer, parametersNext, val.real.x) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.real.y) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.real.z) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.real.w) iferr_return false;
            writer.Append(';') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.dual.x) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.dual.y) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.dual.z) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.dual.w) iferr_return false;
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
        return writer.Append("DualQuat{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f32) noexcept {
        return 12;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f32) noexcept {
        return writer.Append("DualQuatf32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f64) noexcept {
        return 12;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f64) noexcept {
        return writer.Append("DualQuatf64{");
    }
};

} // namespace Alt

#ifdef ALT_COMPILER_MSVC
#pragma warning(pop)
#endif // ALT_COMPILER_MSVC

#endif // __ALT_STD_MATH_DUAL_QUAT__