#pragma once

#ifndef __ALT_STD_RANGE__
#define __ALT_STD_RANGE__

#include "../TypeTraits.h"

#include "../Iterators/Iterator.h"

namespace Alt {

template <typename T>
struct Range;

template <typename T>
class RangeIterator;

/** @brief Aliases for Range. */
using RangeUsize = Range<usize>;
using RangeU64 = Range<u64>;
using RangeU32 = Range<u32>;
using RangeU16 = Range<u16>;
using RangeU8 = Range<u8>;

using RangeSsize = Range<ssize>;
using RangeS64 = Range<s64>;
using RangeS32 = Range<s32>;
using RangeS16 = Range<s16>;
using RangeS8 = Range<s8>;

using RangeF32 = Range<f32>;
using RangeF64 = Range<f64>;

using RangeUptr = Range<uptr>;
using RangeSptr = Range<sptr>;

namespace TypeTraits {

template <typename T>
struct __is_range__<Range<T>>
    : std::true_type
{};

template <typename T>
struct __is_trivially_reallocatable__<Range<T>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/**
 * @brief Represents a contiguous range of values defined by a start and end point.
 * @tparam T The numeric type used to define the range boundaries.
 * 
 * @example
 * 
 * Alt::RangeUsize range0{ 0, 5 };
 * assert(range0.Size() == 5);
 * assert(!range0.IsEmpty());
 * 
 * Alt::Rangef32 range1{ 10.0f, 10.0f };
 * assert(range1.IsEmpty());
 * assert(range1.Size() == 0);
 */
template <typename T>
struct Range {

    T begin = static_cast<T>(0);
    T end = static_cast<T>(0);

    /**
     * @brief Default empty range constructor.
     */
    ALT_FORCE_INLINE Range() noexcept = default;

    /**
     * @brief Range constructor from [begin, end).
     */
    ALT_FORCE_INLINE Range(const T& begin, const T& end) noexcept
        : begin(begin)
        , end(end)
    {}

    /**
     * @brief Returns the number of elements contained within the range.
     * @return The count of elements as a usize.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize Size() const noexcept {
        return static_cast<usize>(end - begin);
    }

    /**
     * @brief Checks if the range contains no elements.
     * @return True if the range is empty (begin == end), false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return end == begin;
    }

    /**
     * @brief Creates a new iterator for the current range.
     * @return An `RangeIterator` object initialized to the start of the range.
     */
    ALT_NODISCARD ALT_FORCE_INLINE RangeIterator<T> GetIterator() const noexcept {

        return RangeIterator<T>{ *this };
    }
};

/**
 * @brief An iterator that allows traversing the range.
 * @details This iterator maintains its own copy of the range's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Range object.
 * @tparam T The numeric type of the range elements.
 */
template <typename T>
class RangeIterator final : public Iterator<RangeIterator<T>, T, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait> {
public:

    /**
     * @brief Constructs an iterator from a specific range.
     * @param range The Range object to copy boundaries from.
     */
    ALT_FORCE_INLINE RangeIterator(const Range<T>& range) noexcept
        : m_Range(range)
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return The remaining size of the iterator's current view.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {

        return GetSize();
    }

    /**
     * @brief Retrieves the next element and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * Alt::RangeUsize range{ 0, 2 };
     * Alt::RangeUsizeIterator it = range.GetIterator();
     * 
     * assert(*it.Next() == 0);
     * assert(*it.Next() == 1);
     * assert(it.Next() == Alt::Nullopt);
     */
    ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (m_Range.begin == m_Range.end) {

            return Nullopt;
        }

        T result = m_Range.begin;
        m_Range.begin += static_cast<T>(1);

        return result;
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::RangeUsize range{ 0, 10 };
     * Alt::RangeUsizeIterator it = range.GetIterator();
     * 
     * const usize skipped = it.Advance(5);
     * 
     * assert(skipped == 5);
     * assert(it.GetSize() == 5);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, m_Range.Size());

        ALT_ASSERT(advance <= static_cast<usize>(Num::Desc<T>::Max()));
        m_Range.begin += static_cast<T>(advance);

        return advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * Alt::RangeUsize range{ 0, 2 };
     * Alt::RangeUsizeIterator it = range.GetIterator();
     * 
     * assert(*it.NextBack() == 1);
     * assert(*it.NextBack() == 0);
     * assert(it.NextBack() == Alt::Nullopt);
     */
    ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (m_Range.begin == m_Range.end) {

            return Nullopt;
        }

        m_Range.end -= static_cast<T>(1);
        T result = m_Range.end;

        return result;
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::RangeUsize range{ 0, 10 };
     * Alt::RangeUsizeIterator it = range.GetIterator();
     * 
     * const usize skipped = it.AdvanceBack(3);
     * assert(skipped == 3);
     * assert(it.GetSize() == 7);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, m_Range.Size());

        ALT_ASSERT(advance <= static_cast<usize>(Num::Desc<T>::Max()));
        m_Range.end -= static_cast<T>(advance);

        return advance;
    }

    /**
     * @brief Returns the remaining size.
     * @return The number of elements left in the iterator's range.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_Range.Size();
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_Range.IsEmpty();
    }

private:

    Range<T> m_Range;
};

} // namespace Alt

#endif // __ALT_STD_RANGE__