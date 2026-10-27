#pragma once

#ifndef __ALT_STD_MATH_POINT3__
#define __ALT_STD_MATH_POINT3__

#include "../../String/Impl/FormatPreInclude.h"

#include "MathPreInclude.h"

#include "Vec3.h"

namespace Alt {
namespace Math {

template <typename T>
class Point3 : public Vec3<T> {
public:

    using Vec3<T>::Vec3;

    ALT_FORCE_INLINE Point3(const Vec3<T>& vec) noexcept
        : Vec3<T>(vec)
    {}
};

using Point3f32 = Point3<f32>;
using Point3f64 = Point3<f64>;

} // namespace Math

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<Math::Point3<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Format specialization for Point3<T>.
 */
template <typename T>
class Format<Math::Point3<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Math::Point3<T>& val) noexcept {
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
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Math::Point3<T>& val) noexcept {
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
        return 7;
    }

    template <typename U, typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, U) noexcept {
        return writer.Append("Point3{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u8) noexcept {
        return 9;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u8) noexcept {
        return writer.Append("Point3u8{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u16) noexcept {
        return 10;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u16) noexcept {
        return writer.Append("Point3u16{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u32) noexcept {
        return 10;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u32) noexcept {
        return writer.Append("Point3u32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(u64) noexcept {
        return 10;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, u64) noexcept {
        return writer.Append("Point3u64{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s8) noexcept {
        return 9;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s8) noexcept {
        return writer.Append("Point3s8{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s16) noexcept {
        return 10;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s16) noexcept {
        return writer.Append("Point3s16{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s32) noexcept {
        return 10;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s32) noexcept {
        return writer.Append("Point3s32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(s64) noexcept {
        return 10;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, s64) noexcept {
        return writer.Append("Point3s64{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f32) noexcept {
        return 10;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f32) noexcept {
        return writer.Append("Point3f32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f64) noexcept {
        return 10;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f64) noexcept {
        return writer.Append("Point3f64{");
    }
};

} // namespace Alt

#endif // __ALT_STD_MATH_POINT3__