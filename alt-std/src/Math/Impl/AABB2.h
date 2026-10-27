#pragma once

#ifndef __ALT_STD_MATH_AABB2__
#define __ALT_STD_MATH_AABB2__

#include "../../String/Impl/FormatPreInclude.h"

#include "MathPreInclude.h"

#include "Vec2.h"

namespace Alt {
namespace Math {

template <typename T>
class AABB2 {
public:

    using Vec2 = Vec2<T>;

    ALT_FORCE_INLINE AABB2() noexcept
        : m_Min(Num::Desc<T>::Max())
        , m_Max(Num::Desc<T>::Lowest())
    {}

    ALT_FORCE_INLINE AABB2(const Vec2& min, const Vec2& max) noexcept
        : m_Min(min)
        , m_Max(max)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE static AABB2 Empty() noexcept {

        return AABB2{
            Vec2{ Num::Desc<T>::Max() },
            Vec2{ Num::Desc<T>::Lowest() },
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static AABB2 Zero() noexcept {

        return AABB2{
            Vec2{ static_cast<T>(0.0) },
            Vec2{ static_cast<T>(0.0) },
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE const Vec2& GetMin() const noexcept {
        return m_Min;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const Vec2& GetMax() const noexcept {
        return m_Max;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 GetCenter() const noexcept {
        return m_Max * static_cast<T>(0.5) + m_Min * static_cast<T>(0.5);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec2 GetSpan() const noexcept {
        return m_Max - m_Min;
    }

    ALT_FORCE_INLINE void Include(const Vec2& p) noexcept {

        m_Min.x = Min(m_Min.x, p.x);
        m_Min.y = Min(m_Min.y, p.y);

        m_Max.x = Max(m_Max.x, p.x);
        m_Max.y = Max(m_Max.y, p.y);
    }

    ALT_FORCE_INLINE AABB2 operator+(const Vec2& offset) const noexcept {
        return AABB2(m_Min + offset, m_Max + offset);
    }

    ALT_FORCE_INLINE AABB2& operator+=(const Vec2& offset) const noexcept {

        *this = AABB2(m_Min + offset, m_Max + offset);
        return *this;
    }

    ALT_FORCE_INLINE AABB2 operator-(const Vec2& offset) const noexcept {
        return AABB2(m_Min - offset, m_Max - offset);
    }

    ALT_FORCE_INLINE AABB2& operator-=(const Vec2& offset) const noexcept {

        *this = AABB2(m_Min - offset, m_Max - offset);
        return *this;
    }

    ALT_FORCE_INLINE void ExtendByOffset(const Vec2& offset) noexcept {

        m_Min -= offset;
        m_Max += offset;
    }

    ALT_FORCE_INLINE void ExtendByOffset_Max(const Vec2& offset) noexcept {

        m_Max += offset;
    }

private:

    Vec2 m_Min;
    Vec2 m_Max;
};

using AABB2f32 = AABB2<f32>;
using AABB2f64 = AABB2<f64>;

} // namespace Math

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<Math::AABB2<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Format specialization for AABB2<T>.
 */
template <typename T>
class Format<Math::AABB2<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Math::AABB2<T>& val) noexcept {
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
                EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMin().x) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMin().y) iferr_return Nullopt;
                result.size += estimated.size + 2;

                result.size += estimator.GetIndentEstimatedSize();
                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().x) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().y) iferr_return Nullopt;
                result.size += estimated.size + 2;
            }
            estimator.PopIndent();

            result.size += estimator.GetIndentEstimatedSize() + 1;

            return result;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            EstimateExactSizeResult result{};
            result.size = EstimateSizeTypeName(CleanU{}) + 1;

            EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMin().x) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMin().y) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().x) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().y) iferr_return Nullopt;
            result.size += estimated.size + 2;

            result.size += 1;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Math::AABB2<T>& val) noexcept {
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
                Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMin().x) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMin().y) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append('\n') iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().x) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().y) iferr_return false;
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

            Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMin().x) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMin().y) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().x) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().y) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            writer.Append('}') iferr_return false;

            return true;
        }

        return false;
    }

private:

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(U) noexcept {
        return 6;
    }

    template <typename U, typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, U) noexcept {
        return writer.Append("AABB2{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f32) noexcept {
        return 9;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f32) noexcept {
        return writer.Append("AABB2f32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f64) noexcept {
        return 9;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f64) noexcept {
        return writer.Append("AABB2f64{");
    }
};

} // namespace Alt

#endif // __ALT_STD_MATH_AABB2__