#pragma once

#ifndef __ALT_STD_STRING_ITERATOR__
#define __ALT_STD_STRING_ITERATOR__

#include "../Iterators/Iterator.h"

#include "Utf.h"

namespace Alt {

template <usize>
class StringUTF8;

template <usize, typename>
class StaticStringUTF8;

/**
 * @brief An iterator that allows traversing the String.
 * @details This iterator maintains its own copy of the String's boundaries. 
 * Advancing the iterator modifies its internal view, not the original String object.
 * @tparam T The numeric type of the String chara (UTF-32).
 */
class StringUTF8Iterator final : public Iterator<StringUTF8Iterator, chara, IIteratorForwardTrait, IIteratorBackwardTrait> {
public:

    /**
     * @brief Constructs an iterator from a specific String.
     * @param self The String object to copy boundaries from.
     */
    template <usize MIN_BYTES_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE StringUTF8Iterator(const StringUTF8<MIN_BYTES_CAPACITY_ON_ALLOCATE>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
    {}

    /**
     * @brief Constructs an iterator from a StaticString.
     * @param self The String object to copy boundaries from.
     */
    template <usize SIZE_IN_BYTES_MINUS_ONE, typename Usize>
    ALT_FORCE_INLINE StringUTF8Iterator(const StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE, Usize>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return 0 - mean no hint.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Retrieves the next chara (UTF-32) and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * Alt::String string = "St"_salloc;
     * Alt::StringIterator it = string.GetIterator();
     * 
     * assert(*it.Next() == U'S');
     * assert(*it.Next() == U't');
     * assert(it.Next() == Alt::Nullopt);
     */
    ALT_FORCE_INLINE Optional<chara> Next() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u8 utf8[4]{
            *m_pBegin,
            m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u8>(0),
            m_pBegin + 2 < m_pEnd ? *(m_pBegin + 2) : static_cast<u8>(0),
            m_pBegin + 3 < m_pEnd ? *(m_pBegin + 3) : static_cast<u8>(0),
        };

        u32 utf32;
        const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

        m_pBegin += result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Skips a specified number of charas (UTF-32) forward in the current view.
     * @param advance_ The number of charas (UTF-32) to skip.
     * @return The actual number of charas (UTF-32) successfully skipped.
     * @example
     * 
     * Alt::String string = "String"_salloc;
     * Alt::StringIterator it = string.GetIterator();
     * 
     * const usize skipped = it.Advance(3);
     * 
     * assert(skipped == 3);
     * assert(it.Count() == 3);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u8 utf8[4]{
                *m_pBegin,
                m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u8>(0),
                m_pBegin + 2 < m_pEnd ? *(m_pBegin + 2) : static_cast<u8>(0),
                m_pBegin + 3 < m_pEnd ? *(m_pBegin + 3) : static_cast<u8>(0),
            };

            const usize advanceBytes = Utf::_8::ForwardUtf8IgnoreErrors(utf8);

            m_pBegin += advanceBytes;
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Retrieves the lasts charas (UTF-32) and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * Alt::String string = "ng"_salloc;
     * Alt::StringIterator it = string.GetIterator();
     * 
     * assert(*it.NextBack() == 'g');
     * assert(*it.NextBack() == 'n');
     * assert(it.NextBack() == Alt::Nullopt);
     */
    ALT_FORCE_INLINE Optional<chara> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u8 utf8[4]{
            m_pBegin <= (m_pEnd - 4) ? *(m_pEnd - 4) : static_cast<u8>(0),
            m_pBegin <= (m_pEnd - 3) ? *(m_pEnd - 3) : static_cast<u8>(0),
            m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u8>(0),
            *(m_pEnd - 1),
        };

        u32 utf32;
        const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrorsBackward(utf8, utf32);

        m_pEnd -= result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Skips a specified number of charasa (UTF-32) backward in the current view.
     * @param advance_ The number of charasa (UTF-32) to skip.
     * @return The actual number of charasa (UTF-32) successfully skipped.
     * @example
     * 
     * Alt::String string = "String"_salloc iferr_return;
     * Alt::StringIterator it = string.GetIterator();
     * 
     * const usize skipped = it.AdvanceBack(1);
     * 
     * assert(skipped == 1);
     * assert(it.Count() == 5);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u8 utf8[4]{
                m_pBegin <= (m_pEnd - 4) ? *(m_pEnd - 4) : static_cast<u8>(0),
                m_pBegin <= (m_pEnd - 3) ? *(m_pEnd - 3) : static_cast<u8>(0),
                m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u8>(0),
                *(m_pEnd - 1),
            };

            const usize advanceBytes = Utf::_8::BackwardUtf8IgnoreErrors(utf8);

            m_pEnd -= advanceBytes;
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no charas (UTF-32) left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const u8* m_pBegin;
    const u8* m_pEnd;
};

} // namespace Alt

#endif // __ALT_STD_STRING_ITERATOR__