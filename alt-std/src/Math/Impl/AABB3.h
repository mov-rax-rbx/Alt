#pragma once

#ifndef __ALT_STD_MATH_AABB3__
#define __ALT_STD_MATH_AABB3__

#include "../../String/Impl/FormatPreInclude.h"

#include "MathPreInclude.h"

#include "Vec3.h"
#include "Matrix44.h"

namespace Alt {
namespace Math {

template <typename T>
class AABB3 {
public:

    using Vec3 = Vec3<T>;

    ALT_FORCE_INLINE AABB3() noexcept
        : m_Min(Num::Desc<T>::Max())
        , m_Max(Num::Desc<T>::Lowest())
    {}

    ALT_FORCE_INLINE AABB3(const Vec3& min, const Vec3& max) noexcept
        : m_Min(min)
        , m_Max(max)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE static AABB3 Empty() noexcept {

        return AABB3{
            Vec3{ Num::Desc<T>::Max() },
            Vec3{ Num::Desc<T>::Lowest() },
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE static AABB3 Zero() noexcept {

        return AABB3{
            Vec3{ static_cast<T>(0.0) },
            Vec3{ static_cast<T>(0.0) },
        };
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        AABB3 empty = AABB3::Empty();
        return m_Min.IsEqFuzzy(empty.m_Min) && m_Max.IsEqFuzzy(empty.m_Max);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsDegen() const noexcept {

        return
            m_Min.x > m_Max.x ||
            m_Min.y > m_Max.y ||
            m_Min.z > m_Max.z;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsZero() const noexcept {

        const AABB3 zero = AABB3::Zero();
        return m_Min.IsEqFuzzy(zero.m_Min) && m_Max.IsEqFuzzy(zero.m_Max);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const Vec3& operator[](const usize i) const noexcept {
        return i == 0 ? m_Min : m_Max;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const Vec3& GetMin() const noexcept {
        return m_Min;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const Vec3& GetMax() const noexcept {
        return m_Max;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 GetCenter() const noexcept {
        return m_Max * static_cast<T>(0.5) + m_Min * static_cast<T>(0.5);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 GetExtent() const noexcept {
        return (m_Max - m_Min) * static_cast<T>(0.5);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 GetSpan() const noexcept {
        return m_Max - m_Min;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetLongestAxis() const noexcept {

        const Vec3 span = GetSpan();
        if (span.x > span.y && span.x > span.z) {

            return 0;
        }
        else {

            return span.y > span.z ? 1 : 2;
        }
    }

    ALT_NODISCARD ALT_FORCE_INLINE T GetVolume() const noexcept {

        const Vec3 e = GetSpan();
        return e.x * e.y * e.z;
    }

    ALT_NODISCARD ALT_FORCE_INLINE T GetSurfaceArea() const noexcept {

        const Vec3 e = GetSpan();
        return static_cast<T>(2) * (e.x * e.y + e.x * e.z + e.y * e.z);
    }

    /*
     * return point by direction
     */
    ALT_NODISCARD ALT_FORCE_INLINE Vec3 GetSupport(const Vec3& dir) const noexcept {

        return Vec3{
            dir.x >= static_cast<T>(0.0) ? m_Max.x : m_Min.x,
            dir.y >= static_cast<T>(0.0) ? m_Max.y : m_Min.y,
            dir.z >= static_cast<T>(0.0) ? m_Max.z : m_Min.z,
        };
    }

    ALT_FORCE_INLINE void Include(const AABB3& aabb) noexcept {

        m_Min[0] = Min(m_Min[0], aabb.m_Min[0]);
        m_Min[1] = Min(m_Min[1], aabb.m_Min[1]);
        m_Min[2] = Min(m_Min[2], aabb.m_Min[2]);

        m_Max[0] = Max(m_Max[0], aabb.m_Max[0]);
        m_Max[1] = Max(m_Max[1], aabb.m_Max[1]);
        m_Max[2] = Max(m_Max[2], aabb.m_Max[2]);
    }

    ALT_FORCE_INLINE void Include(const Vec3& p) noexcept {

        m_Min[0] = Min(m_Min[0], p[0]);
        m_Min[1] = Min(m_Min[1], p[1]);
        m_Min[2] = Min(m_Min[2], p[2]);

        m_Max[0] = Max(m_Max[0], p[0]);
        m_Max[1] = Max(m_Max[1], p[1]);
        m_Max[2] = Max(m_Max[2], p[2]);
    }

    ALT_FORCE_INLINE AABB3 operator+(const Vec3& offset) const noexcept {
        return AABB3(m_Min + offset, m_Max + offset);
    }

    ALT_FORCE_INLINE AABB3& operator+=(const Vec3& offset) const noexcept {

        *this = AABB3(m_Min + offset, m_Max + offset);
        return *this;
    }

    ALT_FORCE_INLINE AABB3 operator-(const Vec3& offset) const noexcept {
        return AABB3(m_Min - offset, m_Max - offset);
    }

    ALT_FORCE_INLINE AABB3& operator-=(const Vec3& offset) const noexcept {

        *this = AABB3(m_Min - offset, m_Max - offset);
        return *this;
    }

    ALT_FORCE_INLINE void ExtendByOffset(const Vec3& offset) noexcept {

        m_Min -= offset;
        m_Max += offset;
    }

    ALT_FORCE_INLINE void ExtendByOffset_Max(const Vec3& offset) noexcept {

        m_Max += offset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE AABB3 Transformed(const Matrix44<T>& mat44) const noexcept {

        Vec3 newMin = mat44.GetTranslation();
        Vec3 newMax = mat44.GetTranslation();

        for (usize i = 0; i < 3; ++i) {
            const Vec3 col = Vec3(mat44.GetCol(i));

            const Vec3 a = col * m_Min[i];
            const Vec3 b = col * m_Max[i];

            newMin.x += Min(a.x, b.x);
            newMin.y += Min(a.y, b.y);
            newMin.z += Min(a.z, b.z);

            newMax.x += Max(a.x, b.x);
            newMax.y += Max(a.y, b.y);
            newMax.z += Max(a.z, b.z);
        }

        return AABB3(newMin, newMax);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IntersectsAABB(const AABB3& aabb) const noexcept {

        return !(
            aabb.m_Min.x > m_Max.x || aabb.m_Max.x < m_Min.x ||
            aabb.m_Min.y > m_Max.y || aabb.m_Max.y < m_Min.y ||
            aabb.m_Min.z > m_Max.z || aabb.m_Max.z < m_Min.z
        );
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IntersectsSphere(const Vec3& p, const T radius) const noexcept {
        return IntersectsSphereSq(p, radius * radius);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IntersectsSphereSq(const Vec3& p, const T radiusSq) const noexcept {

        const Vec3 cl = ClosestPoint(p);
        return (cl - p).LengthSq() <= radiusSq;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool Contains(const Vec3& p) const noexcept {

        return
            p.x >= m_Min.x && p.y >= m_Min.y && p.z >= m_Min.z &&
            p.x <= m_Max.x && p.y <= m_Max.y && p.z <= m_Max.z;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Vec3 ClosestPoint(const Vec3& p) const noexcept {

        return Vec3{
            Min(Max(p.x, m_Min.x), m_Max.x),
            Min(Max(p.y, m_Min.y), m_Max.y),
            Min(Max(p.z, m_Min.z), m_Max.z),
        };
    }

private:

    Vec3 m_Min;
    Vec3 m_Max;
};

using AABB3f32 = AABB3<f32>;
using AABB3f64 = AABB3<f64>;

} // namespace Math

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<Math::AABB3<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Format specialization for AABB3<T>.
 */
template <typename T>
class Format<Math::AABB3<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Math::AABB3<T>& val) noexcept {
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

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMin().z) iferr_return Nullopt;
                result.size += estimated.size + 2;

                result.size += estimator.GetIndentEstimatedSize();
                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().x) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().y) iferr_return Nullopt;
                result.size += estimated.size + 2;

                estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().z) iferr_return Nullopt;
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

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMin().z) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().x) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().y) iferr_return Nullopt;
            result.size += estimated.size + 2;

            estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val.GetMax().z) iferr_return Nullopt;
            result.size += estimated.size + 2;

            result.size += 1;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Math::AABB3<T>& val) noexcept {
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
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMin().z) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append('\n') iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().x) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().y) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;

                Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().z) iferr_return false;
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

            Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMin().z) iferr_return false;
            writer.Append(';') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().x) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().y) iferr_return false;
            writer.Append(',') iferr_return false;
            writer.Append(' ') iferr_return false;

            Format<CleanT>::WriteAppend(writer, parametersNext, val.GetMax().z) iferr_return false;
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
        return 6;
    }

    template <typename U, typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, U) noexcept {
        return writer.Append("AABB3{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f32) noexcept {
        return 9;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f32) noexcept {
        return writer.Append("AABB3f32{");
    }

    ALT_NODISCARD ALT_FORCE_INLINE static usize EstimateSizeTypeName(f64) noexcept {
        return 9;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriterAppendTypeName(FormatWriter<FormatWriterT>& writer, f64) noexcept {
        return writer.Append("AABB3f64{");
    }
};

} // namespace Alt

#endif // __ALT_STD_MATH_AABB3__