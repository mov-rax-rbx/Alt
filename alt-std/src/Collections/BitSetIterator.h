#pragma once

#ifndef __ALT_STD_BITSET_ITERATOR__
#define __ALT_STD_BITSET_ITERATOR__

#include "../TypeTraits.h"

#include "../Iterators/Iterator.h"

#include "../Utils/Range.h"

namespace Alt {

template <typename Usize, usize MIN_BITS_CAPACITY_ON_ALLOCATE>
class BitSetT;

template <usize SIZE_IN_BITS, typename Usize>
class StaticBitSet;

/**
 * @brief A standard iterator for the BitSet.
 * @tparam Usize The underlying bit-width type of the BitSet.
 */
template <typename Usize>
class BitSetIterator final : public Iterator<BitSetIterator<Usize>, bool, IIteratorForwardTrait, IIteratorExactSizeTrait> {
public:

    /**
     * @brief Constructor for a standard BitSet iterator.
     * @tparam MIN_BITS_CAPACITY_ON_ALLOCERATE The buffer constraint of the parent bitset.
     * @param self Reference to the source BitSetT.
     */
    template <usize MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE BitSetIterator(const BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>& self) noexcept
        : m_pBegin(self.m_Words.Data())
        , m_SizeBits(self.m_SizeBits)
    {}

    /**
     * @brief Constructor for a StaticBitSet iterator.
     * @tparam SIZE_IN_BITS The number of bits in the bitset.
     * @param self Reference to the source StaticBitSet.
     */
    template <usize SIZE_IN_BITS>
    ALT_FORCE_INLINE BitSetIterator(const StaticBitSet<SIZE_IN_BITS, Usize>& self) noexcept
        : m_pBegin(self.m_Words)
        , m_SizeBits(SIZE_IN_BITS)
    {}

    /**
     * @brief Returns the number of bits remaining to be iterated.
     * @return The remaining count of bits as a usize.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_SizeBits;
    }

    /**
     * @brief Advances the iterator by one bit and returns its state.
     * @return An Optional containing true if the current bit is set, false if not, or Nullopt if at end.
     */
    ALT_FORCE_INLINE Optional<bool> Next() noexcept {

        if (m_SizeBits == 0) {
            return Nullopt;
        }

        m_pBegin += m_BitIdx >> BITS_TO_WORDID_RIGHT_SHIFT;
        m_BitIdx = m_BitIdx & BITS_TO_BITIDX_IN_WORD_MASK;

        const bool result = ((*m_pBegin >> m_BitIdx) & 0b1) == 1;

        m_BitIdx += 1;
        m_SizeBits -= 1;

        return result;
    }

    /**
     * @brief Advances the iterator by a specified amount of bits.
     * @param advance_ The number of bits to jump forward.
     * @return The actual number of bits advanced (clamped by size).
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, m_SizeBits);

        m_SizeBits -= advance;

        const usize totalBits = m_BitIdx + advance;
        m_pBegin += totalBits >> BITS_TO_WORDID_RIGHT_SHIFT;
        m_BitIdx = totalBits & BITS_TO_BITIDX_IN_WORD_MASK;

        return advance;
    }

    /**
     * @brief Gets the remaining number of bits to be processed.
     * @return The count as a usize.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {

        return m_SizeBits;
    }

    /**
     * @brief Checks if there are any more bits to process.
     * @return True if the iterator is at the end of the data, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        return m_SizeBits == 0;
    }

private:

    constexpr static usize BITS = static_cast<usize>(Num::Desc<Usize>::Bits());
    static_assert(Num::IsPowerOf2(BITS), "");
    
    constexpr static usize BITS_TO_WORDID_RIGHT_SHIFT = Num::Count0BitsRightConstexpr(BITS);
    constexpr static usize BITS_TO_BITIDX_IN_WORD_MASK = BITS - 1;

    const Usize* m_pBegin;
    usize m_SizeBits;

    usize m_BitIdx = 0;
};

/**
 * @brief An iterator that skips over '0' bits to only return ranges of consecutive set '1' values.
 * @tparam Usize The underlying bit-width type of the BitSet.
 */
template <typename Usize>
class BitSetIteratorActiveRange final : public Iterator<BitSetIteratorActiveRange<Usize>, RangeUsize, IIteratorForwardTrait> {
private:

    using T = TypeTraits::select_larger_t<usize, Usize>;

public:

    /**
     * @brief Constructor for an active range iterator.
     * @tparam MIN_BITS_CAPACITY_ON_ALLOCERATE The buffer constraint of the parent bitset.
     * @param self Reference to the source BitSetT.
     */
    template <usize MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE BitSetIteratorActiveRange(const BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>& self) noexcept
        : m_pBegin(self.m_Words.Data())
        , m_pEnd(self.m_Words.DataEnd())
        , m_MaskLast(
            (self.m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << (self.m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {}

    template <usize MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE BitSetIteratorActiveRange(const BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>& self, const usize bitOffset) noexcept
        : m_pBegin(self.m_Words.Data() + (bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT))
        , m_pEnd(self.m_Words.DataEnd())
        , m_WordID(bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT)
        , m_WordShiftAcc(bitOffset & BITS_TO_BITIDX_IN_WORD_MASK)
        , m_MaskLast(
            (self.m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << (self.m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {
        if (m_pBegin == m_pEnd || m_WordShiftAcc == 0) {
            return;
        }

        const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

        m_WordAcc = static_cast<T>(*m_pBegin) & mask;
        m_WordAcc >>= m_WordShiftAcc;

        m_pBegin += bitOffset == self.m_SizeBits ? 1 : 0;
        m_WordID += bitOffset == self.m_SizeBits ? 1 : 0;
    }

    template <usize MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE BitSetIteratorActiveRange(const BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>& self, const usize bitOffset, const usize bitSize) noexcept
        : m_pBegin(self.m_Words.Data() + (bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT))
        , m_pEnd(self.m_Words.Data() + ((bitOffset + bitSize) >> BITS_TO_WORDID_RIGHT_SHIFT) + (((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1))
        , m_WordID(bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT)
        , m_WordShiftAcc(bitOffset & BITS_TO_BITIDX_IN_WORD_MASK)
        , m_MaskLast(
            ((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << ((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {
        if (m_pBegin == m_pEnd || m_WordShiftAcc == 0) {
            return;
        }

        const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

        m_WordAcc = static_cast<T>(*m_pBegin) & mask;
        m_WordAcc >>= m_WordShiftAcc;

        m_pBegin += bitSize == 0 ? 1 : 0;
        m_WordID += bitSize == 0 ? 1 : 0;
    }

    /**
     * @brief Constructor for an active range iterator.
     * @tparam SIZE_IN_BITS The number of bits in the bitset.
     * @param self Reference to the source StaticBitSet.
     */
    template <usize SIZE_IN_BITS>
    ALT_FORCE_INLINE BitSetIteratorActiveRange(const StaticBitSet<SIZE_IN_BITS, Usize>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
        , m_MaskLast(
            (SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << (SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {}

    template <usize SIZE_IN_BITS>
    ALT_FORCE_INLINE BitSetIteratorActiveRange(const StaticBitSet<SIZE_IN_BITS, Usize>& self, const usize bitOffset) noexcept
        : m_pBegin(self.Data() + (bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT))
        , m_pEnd(self.DataEnd())
        , m_WordID(bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT)
        , m_WordShiftAcc(bitOffset & BITS_TO_BITIDX_IN_WORD_MASK)
        , m_MaskLast(
            (SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << (SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {
        if (m_pBegin == m_pEnd || m_WordShiftAcc == 0) {

            return;
        }

        const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

        m_WordAcc = static_cast<T>(*(self.Data() + m_WordID)) & mask;
        m_WordAcc >>= m_WordShiftAcc;

        m_pBegin += bitOffset == SIZE_IN_BITS ? 1 : 0;
        m_WordID += bitOffset == SIZE_IN_BITS ? 1 : 0;
    }

    template <usize SIZE_IN_BITS>
    ALT_FORCE_INLINE BitSetIteratorActiveRange(const StaticBitSet<SIZE_IN_BITS, Usize>& self, const usize bitOffset, const usize bitSize) noexcept
        : m_pBegin(self.Data() + (bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT))
        , m_pEnd(self.Data() + ((bitOffset + bitSize) >> BITS_TO_WORDID_RIGHT_SHIFT) + (((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1))
        , m_WordID(bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT)
        , m_WordShiftAcc(bitOffset & BITS_TO_BITIDX_IN_WORD_MASK)
        , m_MaskLast(
            ((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << ((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {
        if (m_pBegin == m_pEnd || m_WordShiftAcc == 0) {

            return;
        }

        const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

        m_WordAcc = static_cast<T>(*(self.Data() + m_WordID)) & mask;
        m_WordAcc >>= m_WordShiftAcc;

        m_pBegin += bitSize == 0 ? 1 : 0;
        m_WordID += bitSize == 0 ? 1 : 0;
    }

    /**
     * @brief Returns the next contiguous range of set bits.
     * @return An Optional containing a RangeUsize object, or Nullopt if no more active ranges exist.
     */
    ALT_FORCE_INLINE Optional<RangeUsize> Next() noexcept {

        if (m_WordAcc == 0) {

            if (m_pBegin == m_pEnd) {
                return Nullopt;
            }

            const Usize* pEnd = m_pEnd - 1;
            while (*m_pBegin == 0 && m_pBegin != pEnd) {

                m_pBegin += 1;
                m_WordID += 1;
            }

            const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

            m_WordAcc = static_cast<T>(*m_pBegin) & mask;
            m_WordShiftAcc = 0;

            if (m_WordAcc == 0) {
                return Nullopt;
            }
        }

        const bool isLast = m_pBegin + 1 == m_pEnd;
        T mask = isLast ? m_MaskLast : Num::Desc<T>::Max();

        T word = m_WordAcc;
        T wordShiftAcc = m_WordShiftAcc;
        T idx = static_cast<T>(Num::Count0BitsRight(word));

        wordShiftAcc += idx;

        T guardMask0 = idx == BITS ? 0 : Num::Desc<T>::Max();
        const T guardMask1 = wordShiftAcc == BITS ? 0 : Num::Desc<T>::Max();

        word = (word >> idx) & guardMask0;
        mask = (mask >> wordShiftAcc) & guardMask1;

        const usize begin = (m_WordID << BITS_TO_WORDID_RIGHT_SHIFT) + wordShiftAcc;
        usize end = begin;

        if (!isLast && word == (WORD_MAX & mask)) {

            m_pBegin += 1;
            m_WordID += 1;

            const Usize* pEnd = m_pEnd - 1;
            while (m_pBegin != pEnd && *m_pBegin == static_cast<Usize>(WORD_MAX)) {

                m_pBegin += 1;
                m_WordID += 1;
            }

            mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

            word = static_cast<T>(*m_pBegin) & mask;
            end = m_WordID << BITS_TO_WORDID_RIGHT_SHIFT;

            wordShiftAcc = 0;
        }

        idx = static_cast<T>(Num::Count0BitsRight(~word));
        end += idx;

        guardMask0 = idx == BITS ? 0 : Num::Desc<T>::Max();
        word = (word >> idx) & guardMask0;

        wordShiftAcc += idx;

        m_pBegin += word == 0 ? 1 : 0;
        m_WordID += word == 0 ? 1 : 0;

        m_WordAcc = word;
        m_WordShiftAcc = wordShiftAcc;

        return RangeUsize{ begin, end };
    }

    /**
     * @brief Advances the iterator by a specified amount of active ranges.
     * @param advance_ The number of regions to advance.
     * @return The actual number of regions advanced (clamped by size).
     */
    ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        if (advance == 0) {
            return 0;
        }

        usize result = 0;
        while (Optional<RangeUsize> range = Next()) {

            result += 1;
            if (result == advance) {
                break;
            }
        }

        return result;
    }

private:

    constexpr static T BITS = static_cast<T>(Num::Desc<Usize>::Bits());
    static_assert(Num::IsPowerOf2(BITS), "");

    constexpr static T BITS_TO_WORDID_RIGHT_SHIFT = static_cast<T>(Num::Count0BitsRightConstexpr(BITS));
    constexpr static T BITS_TO_BITIDX_IN_WORD_MASK = BITS - 1;

    constexpr static T WORD_MAX = static_cast<T>(Num::Desc<Usize>::Max());

    const Usize* m_pBegin;
    const Usize* m_pEnd;

    usize m_WordID = 0;
    T m_WordAcc = 0;
    T m_WordShiftAcc = 0;
    T m_MaskLast;
};

/**
 * @brief An iterator that skips over '1' bits to only return ranges of consecutive '0' values.
 * @tparam Usize The underlying bit-width type of the BitSet.
 */
template <typename Usize>
class BitSetIteratorInactiveRange final : public Iterator<BitSetIteratorInactiveRange<Usize>, RangeUsize, IIteratorForwardTrait> {
private:

    using T = TypeTraits::select_larger_t<usize, Usize>;

public:

    /**
     * @brief Constructor for an inactive range iterator.
     * @tparam MIN_BITS_CAPACITY_ON_ALLOCERATE The buffer constraint of the parent bitset.
     * @param self Reference to the source BitSetT.
     */
    template <usize MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE BitSetIteratorInactiveRange(const BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>& self) noexcept
        : m_pBegin(self.m_Words.Data())
        , m_pEnd(self.m_Words.DataEnd())
        , m_MaskLast(
            (self.m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << (self.m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {}

    template <usize MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE BitSetIteratorInactiveRange(const BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>& self, const usize bitOffset) noexcept
        : m_pBegin(self.m_Words.Data() + (bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT))
        , m_pEnd(self.m_Words.DataEnd())
        , m_WordID(bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT)
        , m_WordShiftAcc(bitOffset & BITS_TO_BITIDX_IN_WORD_MASK)
        , m_MaskLast(
            (self.m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << (self.m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {
        if (m_pBegin == m_pEnd || m_WordShiftAcc == 0) {

            return;
        }

        const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

        m_WordAcc = ~static_cast<T>(*(self.m_Words.Data() + m_WordID)) & mask;
        m_WordAcc >>= m_WordShiftAcc;

        m_pBegin += bitOffset == self.m_SizeBits ? 1 : 0;
    }

    template <usize MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE BitSetIteratorInactiveRange(const BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>& self, const usize bitOffset, const usize bitSize) noexcept
        : m_pBegin(self.m_Words.Data() + (bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT))
        , m_pEnd(self.m_Words.Data() + ((bitOffset + bitSize) >> BITS_TO_WORDID_RIGHT_SHIFT) + (((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1))
        , m_WordID(bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT)
        , m_WordShiftAcc(bitOffset & BITS_TO_BITIDX_IN_WORD_MASK)
        , m_MaskLast(
            ((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << ((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {
        if (m_pBegin == m_pEnd || m_WordShiftAcc == 0) {

            return;
        }

        const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

        m_WordAcc = ~static_cast<T>(*(self.m_Words.Data() + m_WordID)) & mask;
        m_WordAcc >>= m_WordShiftAcc;

        m_pBegin += bitOffset == (bitOffset + bitSize) ? 1 : 0;
    }

    /**
     * @brief Constructor for an inactive range iterator.
     * @tparam SIZE_IN_BITS The number of bits in the bitset.
     * @param self Reference to the source StaticBitSet.
     */
    template <usize SIZE_IN_BITS>
    ALT_FORCE_INLINE BitSetIteratorInactiveRange(const StaticBitSet<SIZE_IN_BITS, Usize>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
        , m_MaskLast(
            (SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << (SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {}

    template <usize SIZE_IN_BITS>
    ALT_FORCE_INLINE BitSetIteratorInactiveRange(const StaticBitSet<SIZE_IN_BITS, Usize>& self, const usize bitOffset) noexcept
        : m_pBegin(self.Data() + (bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT))
        , m_pEnd(self.DataEnd())
        , m_WordID(bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT)
        , m_WordShiftAcc(bitOffset & BITS_TO_BITIDX_IN_WORD_MASK)
        , m_MaskLast(
            (SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << (SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {
        if (m_pBegin == m_pEnd || m_WordShiftAcc == 0) {

            return;
        }

        const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

        m_WordAcc = ~static_cast<T>(*(self.Data() + m_WordID)) & mask;
        m_WordAcc >>= m_WordShiftAcc;

        m_pBegin += bitOffset == SIZE_IN_BITS ? 1 : 0;
    }

    template <usize SIZE_IN_BITS>
    ALT_FORCE_INLINE BitSetIteratorInactiveRange(const StaticBitSet<SIZE_IN_BITS, Usize>& self, const usize bitOffset, const usize bitSize) noexcept
        : m_pBegin(self.Data() + (bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT))
        , m_pEnd(self.Data() + ((bitOffset + bitSize) >> BITS_TO_WORDID_RIGHT_SHIFT) + (((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1))
        , m_WordID(bitOffset >> BITS_TO_WORDID_RIGHT_SHIFT)
        , m_WordShiftAcc(bitOffset & BITS_TO_BITIDX_IN_WORD_MASK)
        , m_MaskLast(
            ((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK) == 0
                ? Num::Desc<T>::Max()
                : (static_cast<T>(1) << ((bitOffset + bitSize) & BITS_TO_BITIDX_IN_WORD_MASK)) - 1
        )
    {
        if (m_pBegin == m_pEnd || m_WordShiftAcc == 0) {

            return;
        }

        const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

        m_WordAcc = ~static_cast<T>(*(self.Data() + m_WordID)) & mask;
        m_WordAcc >>= m_WordShiftAcc;

        m_pBegin += bitOffset == (bitOffset + bitSize) ? 1 : 0;
    }

    /**
     * @brief Returns the next consecutive range of unset bits.
     * @return An Optional containing a RangeUsize object, or Nullopt if no more inactive ranges exist.
     */
    ALT_FORCE_INLINE Optional<RangeUsize> Next() noexcept {

        if (m_WordAcc == 0) {

            if (m_pBegin == m_pEnd) {
                return Nullopt;
            }

            const Usize* pEnd = m_pEnd - 1;
            while (*m_pBegin == static_cast<Usize>(WORD_MAX) && m_pBegin != pEnd) {

                m_pBegin += 1;
                m_WordID += 1;
            }

            const T mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

            m_WordAcc = ~static_cast<T>(*m_pBegin) & mask;
            m_WordShiftAcc = 0;

            if (m_WordAcc == 0) {
                return Nullopt;
            }
        }

        const bool isLast = m_pBegin + 1 == m_pEnd;
        T mask = isLast ? m_MaskLast : Num::Desc<T>::Max();

        T word = m_WordAcc;
        T wordShiftAcc = m_WordShiftAcc;
        T idx = static_cast<T>(Num::Count0BitsRight(word));

        wordShiftAcc += idx;

        T guardMask0 = idx == BITS ? 0 : Num::Desc<T>::Max();
        const T guardMask1 = wordShiftAcc == BITS ? 0 : Num::Desc<T>::Max();

        word = (word >> idx) & guardMask0;
        mask = (mask >> wordShiftAcc) & guardMask1;

        const usize begin = (m_WordID << BITS_TO_WORDID_RIGHT_SHIFT) + wordShiftAcc;
        usize end = begin;

        if (!isLast && word == (WORD_MAX & mask)) {

            m_pBegin += 1;
            m_WordID += 1;

            const Usize* pEnd = m_pEnd - 1;
            while (m_pBegin != pEnd && *m_pBegin == 0) {

                m_pBegin += 1;
                m_WordID += 1;
            }

            mask = m_pBegin + 1 == m_pEnd ? m_MaskLast : Num::Desc<T>::Max();

            word = ~static_cast<T>(*m_pBegin) & mask;
            end = m_WordID << BITS_TO_WORDID_RIGHT_SHIFT;

            wordShiftAcc = 0;
        }

        idx = Num::Count0BitsRight(~word);
        end += idx;

        guardMask0 = idx == BITS ? 0 : Num::Desc<T>::Max();
        word = (word >> idx) & guardMask0;

        wordShiftAcc += idx;

        m_pBegin += word == 0 ? 1 : 0;
        m_WordID += word == 0 ? 1 : 0;

        m_WordAcc = word;
        m_WordShiftAcc = wordShiftAcc;

        return RangeUsize{ begin, end };
    }

    /**
     * @brief Advances the iterator by a specified amount of inactive ranges.
     * @param advance_ The number of regions to advance.
     * @return The actual number of regions advanced (clamped by size).
     */
    ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        if (advance == 0) {
            return 0;
        }

        usize result = 0;
        while (Optional<RangeUsize> range = Next()) {

            result += 1;
            if (result == advance) {
                break;
            }
        }

        return result;
    }

private:

    constexpr static T BITS = static_cast<T>(Num::Desc<Usize>::Bits());
    static_assert(Num::IsPowerOf2(BITS), "");

    constexpr static T BITS_TO_WORDID_RIGHT_SHIFT = static_cast<T>(Num::Count0BitsRightConstexpr(BITS));
    constexpr static T BITS_TO_BITIDX_IN_WORD_MASK = BITS - 1;

    constexpr static T WORD_MAX = static_cast<T>(Num::Desc<Usize>::Max());

    const Usize* m_pBegin;
    const Usize* m_pEnd;

    usize m_WordID = 0;
    T m_WordAcc = 0;
    T m_WordShiftAcc = 0;
    T m_MaskLast;
};

} // namespace Alt

#endif // __ALT_STD_BITSET_ITERATOR__