#pragma once

#ifndef __ALT_STD_BITSET__
#define __ALT_STD_BITSET__

#include "../String/Impl/FormatPreInclude.h"

#include "../Collections/Vector.h"

#include "BitSetIterator.h"

namespace Alt {

/**
 * @brief A bitset container used to store and manipulate a collection of bit flags.
 * @details BitSetT stores bits packed into words (of type Usize).
 * @tparam Usize The underlying integer type used to represent a word. Must be an unsigned scalar type.
 * @tparam MIN_BITS_CAPACITY_ON_ALLOCATE The minimum number of bits to allocate when the container is created.
 */
template <typename Usize, usize MIN_BITS_CAPACITY_ON_ALLOCATE = 16>
class BitSetT;

using BitSet = BitSetT<usize>;

namespace TypeTraits {

template <typename Usize, usize MIN_BITS_CAPACITY_ON_ALLOCATE>
struct __is_trivially_reallocatable__<BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>>
    : is_trivially_reallocatable_t<Vector<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>>
{};

} // namespace TypeTraits

template <typename Usize, usize MIN_BITS_CAPACITY_ON_ALLOCATE>
class BitSetT {
private:

    template <typename ...ArgsT>
    constexpr static bool is_populated_v = sizeof...(ArgsT) != 0;

    static_assert(std::is_scalar_v<Usize> && std::is_unsigned_v<Usize>, "");

    template <typename, usize>
    class Entry;

    template <typename, usize>
    class EntryConst;

public:

    /**
     * @brief Returns the minimum capacity constraint for the BitSet.
     * @return The value of MIN_BITS_CAPACITY_ON_ALLOCATE.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize GetMinBitsCapacityOnAllocate() noexcept {

        return MIN_BITS_CAPACITY_ON_ALLOCATE;
    }

    /**
     * @brief Creates a BitSet with the minimum required capacity.
     * @return An Optional containing the BitSet, or Nullopt if allocation fails.
     * @example
     * 
     * Alt::BitSet bs = Alt::BitSet::Allocate();
     */
    ALT_NODISCARD static Optional<BitSetT> Allocate() noexcept {
        iferr_scope;

        BitSetT result{};
        result.Reserve(MIN_BITS_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Creates a BitSet and allocate at least the specified number of bits and an initial value for all bits.
     * @param sizeBits The total number of bits.
     * @param val The default bool state of every bit.
     * @return An Optional containing the BitSet, or Nullopt if allocation fails.
     * @example
     * 
     * Alt::BitSet bs = Alt::BitSet::Allocate(4);
     * 
     * assert(bs.GetSize() == 4);
     */
    ALT_NODISCARD static Optional<BitSetT> Allocate(const usize sizeBits, const bool val = false) noexcept {
        iferr_scope;

        BitSetT result{};

        if (sizeBits < MIN_BITS_CAPACITY_ON_ALLOCATE) {

            result.Reserve(MIN_BITS_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
        }

        result.Resize(sizeBits, val) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Creates a BitSet with pre-allocated capacity for at least the specified number of bits.
     * @param sizeBits The number of bits to reserve memory for (at least MIN_BITS_CAPACITY_ON_ALLOCERATE).
     * @return An Optional containing the BitSet, or Nullopt if allocation fails.
     * @example
     * 
     * Alt::BitSet bs = Alt::BitSet::AllocateCapacity(4);
     */
    ALT_NODISCARD static Optional<BitSetT> AllocateCapacity(const usize sizeBits) noexcept {
        iferr_scope;

        BitSetT result{};
        result.Reserve(Num::Max(sizeBits, MIN_BITS_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Creates a BitSet with exact memory allocation for at least the specified number of bits.
     * @param sizeBits The number of bits to reserve space for (at least MIN_BITS_CAPACITY_ON_ALLOCERATE).
     * @return An Optional containing the BitSet, or Nullopt if allocation fails.
     * @example
     * 
     * Alt::BitSet bs = Alt::BitSet::AllocateCapacityExact(4);
     */
    ALT_NODISCARD static Optional<BitSetT> AllocateCapacityExact(const usize sizeBits) noexcept {
        iferr_scope;

        BitSetT result{};
        result.ReserveExact(Num::Max(sizeBits, MIN_BITS_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        return result;
    }

    /**
     * Clonable Trait
     * 
     * @brief Clone from another BitSet.
     * @tparam U The underlying word type of the source bitset.
     * @param rhs The reference to the source BitSet.
     * @return True if copy succeeded, false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs0;
     * Alt::BitSet bs1 = Alt::BitSet::Allocate(4) iferr_return;
     * 
     * bs1.CloneFrom(bs0) iferr_return;
     * assert(bs0 == bs1);
     * 
     * bs1 = bs0.Clone() iferr_return;
     * assert(bs0 == bs1);
     */
    template <typename U = Usize, usize MIN_BITS_CAPACITY_ON_ALLOCATE_ = MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_NODISCARD bool CloneFrom(const BitSetT<U, MIN_BITS_CAPACITY_ON_ALLOCATE_>& rhs) noexcept {
        iferr_scope;

        m_Words.CloneFrom(rhs.m_Words) iferr_return false;
        m_SizeBits = rhs.m_SizeBits;

        return true;
    }

    template <typename U = Usize, usize MIN_BITS_CAPACITY_ON_ALLOCATE_ = MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_NODISCARD bool CloneConstructFrom(const BitSetT<U, MIN_BITS_CAPACITY_ON_ALLOCATE_>& rhs) noexcept {
        iferr_scope;

        ALT_ASSERT(static_cast<const void*>(this) != &rhs);

        m_Words.CloneConstructFrom(rhs.m_Words) iferr_return false;
        m_SizeBits = rhs.m_SizeBits;

        return true;
    }

    ALT_NODISCARD Optional<BitSetT> Clone() const noexcept {
        iferr_scope;

        MaybeUninit<BitSetT> valueToConstruct;
        valueToConstruct.GetPtr()->CloneConstructFrom(*this) iferr_return Nullopt;

        defer {
            valueToConstruct.Destroy();
        };

        return std::move(valueToConstruct).GetVal();
    }

    /**
     * @brief Resizes the BitSet.
     * @param sizeBits The new count of bits.
     * @param val The default bool state of every bit.
     * @return True if resize was successful, false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs;
     * bs.Resize(64, true) iferr_return;
     */
    ALT_NODISCARD bool Resize(const usize sizeBits, const bool val = false) noexcept {
        iferr_scope;

        if (sizeBits > m_SizeBits && m_SizeBits != 0) {

            const Usize word = m_Words[m_SizeBits >> BITS_TO_WORDID_RIGHT_SHIFT];

            m_Words[m_SizeBits >> BITS_TO_WORDID_RIGHT_SHIFT] = val
                ? word | ~((static_cast<Usize>(1) << (m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK)) - 1)
                : word &  ((static_cast<Usize>(1) << (m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK)) - 1)
            ;
        }

        const usize sizeNextWord = (sizeBits >> BITS_TO_WORDID_RIGHT_SHIFT) + ((sizeBits & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1);
        m_Words.Resize(sizeNextWord) iferr_return false;

        const usize sizeNextWordOld = (m_SizeBits >> BITS_TO_WORDID_RIGHT_SHIFT) + ((m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1);
        if (sizeNextWordOld < sizeNextWord) {

            std::memset(m_Words.Data() + sizeNextWordOld, val ? Num::Desc<s32>::Max() : 0, (sizeNextWord - sizeNextWordOld) * sizeof(Usize));
        }

        m_SizeBits = sizeBits;

        return true;
    }

    /**
     * @brief Reserves at least the specified bit capacity in the BitSet.
     * @param size The capacity to reserve in bits.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs;
     * bs.Reserve(256) iferr_return;
     * 
     * bs.Resize(64, true) iferr_return;
     */
    bool Reserve(const usize sizeBits) noexcept {

        const usize sizeNextWord = (sizeBits >> BITS_TO_WORDID_RIGHT_SHIFT) + ((sizeBits & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1);
        return m_Words.Reserve(sizeNextWord);
    }

    /**
     * @brief Additionally reserves at least the 'GetSize() + additional bit size' capacity in the BitSet.
     * @param size The additional capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed or overflow.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs;
     * bs.ReserveAdditional(256) iferr_return;
     * 
     * bs.Resize(64, true) iferr_return;
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>>* = nullptr>
    bool ReserveAdditional(ArgsT&&... sizeBitsAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeBitsAdditions...) iferr_return false;

        return Reserve(size);
    }

    /**
     * @brief Reserves the exact capacity of bits in the BitSet.
     * @param size The capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs;
     * 
     * bs.ReserveExact(64) iferr_return;
     */
    bool ReserveExact(const usize sizeBits) noexcept {

        const usize sizeNextWord = (sizeBits >> BITS_TO_WORDID_RIGHT_SHIFT) + ((sizeBits & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1);
        return m_Words.ReserveExact(sizeNextWord);
    }

    /**
     * @brief Reserves the exact capacity in the vector.
     * @param size The capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs;
     * 
     * bs.ReserveExactAddition(64) iferr_return;
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>>* = nullptr>
    bool ReserveExactAddition(ArgsT&&... sizeBitsAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeBitsAdditions...) iferr_return false;

        return ReserveExact(size);
    }

    /**
     * @brief Reduces the capacity of the BitSet to fit the current size.
     * @param bitsCapacityNew The new capacity.
     * @return True if successful, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs;
     * bs.Reserve(256) iferr_return;
     * 
     * bs.Resize(64) iferr_return;
     * bs.ShrinkToFit() iferr_return;
     */
    bool ShrinkToFit(const usize bitsCapacityNew = MIN_BITS_CAPACITY_ON_ALLOCATE) noexcept {

        const usize capacityNewWord = (bitsCapacityNew >> BITS_TO_WORDID_RIGHT_SHIFT) + ((bitsCapacityNew & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1);
        return m_Words.ShrinkToFit(capacityNewWord);
    }

    /**
     * @brief Removes all elements from the BitSet.
     * @details Does not deallocate the underlying memory.
     */
    void Clear() noexcept {

        m_Words.Clear();
    }

    /**
     * @brief Sets all bits in the BitSet.
     * @param val Set all bits to true if true, else false.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs;
     * bs.Resize(256, false) iferr_return;
     * 
     * bs.SetAll(true);
     */
    ALT_FORCE_INLINE void SetAll(const bool val = true) noexcept {

        std::memset(m_Words.Data(), val ? Num::Desc<s32>::Max() : 0, m_Words.GetSize() * sizeof(Usize));
    }

    /**
     * @brief Sets the state of a specific bit in the BitSet.
     * @param i The index of the bit to set.
     * @param val The value to assign to the bit.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs;
     * bs.Resize(256, false) iferr_return;
     * 
     * bs.Set(10, true);
     * 
     * assert(bs[10]);
     */
    ALT_FORCE_INLINE void Set(const usize i, const bool val = true) noexcept {

        if (val) {

            m_Words[i >> BITS_TO_WORDID_RIGHT_SHIFT] |= (static_cast<Usize>(1) << (i & BITS_TO_BITIDX_IN_WORD_MASK));
        }
        else {

            m_Words[i >> BITS_TO_WORDID_RIGHT_SHIFT] &= ~(static_cast<Usize>(1) << (i & BITS_TO_BITIDX_IN_WORD_MASK));
        }
    }

    /**
     * @brief Accesses the bit at the specified index.
     * @param i The index of the bit.
     * @return A reference to the bit.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs = Alt::BitSet::Allocate(4, true) iferr_return;
     * assert(bs.Get(0));
     * assert(bs[0]);
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Get(const usize i) const noexcept {
        ALT_ASSERT(i < GetSize());

        return ((m_Words[i >> BITS_TO_WORDID_RIGHT_SHIFT] >> (i & BITS_TO_BITIDX_IN_WORD_MASK)) & 0b1) == 1;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Entry<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE> operator[](const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        return Entry<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>{ *this, i };
    }

    ALT_NODISCARD ALT_FORCE_INLINE EntryConst<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE> operator[](const usize i) const noexcept {
        ALT_ASSERT(i < GetSize());

        return EntryConst<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>{ *this, i };
    }

    /**
     * @brief Find first set bit.
     * @param bitsOffset The offset in bits at whcch the search starts.
     * @return A index of bit which a 'true'.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs = Alt::BitSet::Allocate(4, true) iferr_return;
     * assert(bs.FindFirstSet());
     * assert(*bs.FindFirstSet() == 0);
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirstSet(const usize bitsOffset) const noexcept {
        ALT_ASSERT(bitsOffset <= GetSize());

        const usize wordOffset = bitsOffset >> BITS_TO_WORDID_RIGHT_SHIFT;
        const usize offset = bitsOffset & BITS_TO_BITIDX_IN_WORD_MASK;

        const Usize* pBegin = m_Words.Data() + wordOffset;
        const Usize* pEnd = m_Words.DataEnd();

        const Usize* pBeginOld = pBegin;

        pBegin += pBegin != pEnd && (*pBegin >> offset) == 0
            ? 1
            : 0;

        while (pBegin != pEnd && *pBegin == 0) {

            pBegin += 1;
        }

        if (pBegin == pEnd) {
            return Nullopt;
        }

        const usize correction = pBegin == pBeginOld ? offset : 0;
        const usize bitIdx = correction + (pBegin - m_Words.Data()) * BITS + Num::Count0BitsRight(*pBegin >> correction);

        return bitIdx < m_SizeBits
            ? MakeOptional(bitIdx)
            : Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirstSet() const noexcept {

        const Usize* pBegin = m_Words.Data();
        const Usize* pEnd = m_Words.DataEnd();

        while (pBegin != pEnd && *pBegin == 0) {

            pBegin += 1;
        }

        if (pBegin == pEnd) {
            return Nullopt;
        }

        const usize bitIdx = (pBegin - m_Words.Data()) * BITS + Num::Count0BitsRight(*pBegin);

        return bitIdx < m_SizeBits
            ? MakeOptional(bitIdx)
            : Nullopt;
    }

    /**
     * @brief Find first non-set bit.
     * @param bitsOffset The offset in bits at whcch the search starts.
     * @return A index of bit which a 'false'.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BitSet bs = Alt::BitSet::Allocate(4, true) iferr_return;
     * assert(bs.FindFirstNonSet() == Alt::Nullopt);
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirstNonSet(const usize bitsOffset) const noexcept {
        ALT_ASSERT(bitsOffset <= GetSize());

        constexpr static Usize WORD_MAX = Num::Desc<Usize>::Max();

        const usize wordOffset = bitsOffset >> BITS_TO_WORDID_RIGHT_SHIFT;
        const usize offset = bitsOffset & BITS_TO_BITIDX_IN_WORD_MASK;

        const Usize* pBegin = m_Words.Data() + wordOffset;
        const Usize* pEnd = m_Words.DataEnd();

        const Usize* pBeginOld = pBegin;

        pBegin += pBegin != pEnd && (*pBegin >> offset) == (WORD_MAX >> offset)
            ? 1
            : 0;

        while (pBegin != pEnd && *pBegin == WORD_MAX) {

            pBegin += 1;
        }

        if (pBegin == pEnd) {
            return Nullopt;
        }

        const usize correction = pBegin == pBeginOld ? offset : 0;
        const usize bitIdx = correction + (pBegin - m_Words.Data()) * BITS + Num::Count0BitsRight(~*pBegin >> correction);

        return bitIdx < m_SizeBits
            ? MakeOptional(bitIdx)
            : Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirstNonSet() const noexcept {

        constexpr static Usize WORD_MAX = Num::Desc<Usize>::Max();

        const Usize* pBegin = m_Words.Data();
        const Usize* pEnd = m_Words.DataEnd();

        while (pBegin != pEnd && *pBegin == WORD_MAX) {

            pBegin += 1;
        }

        if (pBegin == pEnd) {
            return Nullopt;
        }

        const usize bitIdx = (pBegin - m_Words.Data()) * BITS + Num::Count0BitsRight(~*pBegin);

        return bitIdx < m_SizeBits
            ? MakeOptional(bitIdx)
            : Nullopt;
    }

    /**
     * @brief Returns the number of bits currently in the BitSet.
     * @return The size of the BitSet.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        return m_SizeBits;
    }

    /**
     * @brief Returns the total number of bits the BitSet can hold before a reallocation is required.
     * @return The capacity of the BitSet.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetCapacity() const noexcept {

        return Num::MulWrap(m_Words.GetCapacity(), BITS);
    }

    /**
     * @brief Checks if the BitSet contains no bits.
     * @return True if empty, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        return m_SizeBits == 0;
    }

    /**
     * @brief Checks if the BitSet contains at least one bit.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept {

        return m_SizeBits != 0;
    }

    /**
     * @brief Operators for comparison with BitSet.
     * @example
     *
     * iferr_scope;
     * 
     * Alt::BitSet bs0 = Alt::BitSet::Allocate(66, true) iferr_return;
     * Alt::BitSet bs1 = bs0.Clone() iferr_return;
     * 
     * assert(bs0 == bs1);
     */
    template <typename U = Usize, usize MIN_BITS_CAPACITY_ON_ALLOCATE_ = MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_NODISCARD bool operator==(const BitSetT<U, MIN_BITS_CAPACITY_ON_ALLOCATE_>& rhs) const noexcept {

        if (m_SizeBits != rhs.m_SizeBits) {

            return false;
        }

        const usize sizeNextWord = (m_SizeBits >> BITS_TO_WORDID_RIGHT_SHIFT) + ((m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1);
        if (sizeNextWord > 1) {

            const bool isEq = Algorithms::CompareEQ(m_Words.Data(), m_Words.DataEnd() - 1, rhs.m_Words.Data(), rhs.m_Words.DataEnd() - 1);
            if (!isEq) {

                return false;
            }
        }

        const usize idx = BITS - (m_SizeBits & BITS_TO_BITIDX_IN_WORD_MASK);
        return (m_Words.GetLast() << idx) == (rhs.m_Words.GetLast() << idx);
    }

    template <typename U = Usize, usize MIN_BITS_CAPACITY_ON_ALLOCATE_ = MIN_BITS_CAPACITY_ON_ALLOCATE>
    ALT_NODISCARD bool operator!=(const BitSetT<U, MIN_BITS_CAPACITY_ON_ALLOCATE_>& rhs) const noexcept {

        return !(*this == rhs);
    }

    /**
     * @brief Returns a immutable iterator for all bits.
     * @return An `BitSetIterator`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE BitSetIterator<Usize> GetIterator() const noexcept {

        return BitSetIterator<Usize>{ *this };
    }

    /**
     * @brief Returns a immutable iterator only for ranges of '1' bits.
     * @return An `BitSetIteratorActiveRange`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE BitSetIteratorActiveRange<Usize> GetIteratorActiveRange() const noexcept {

        return BitSetIteratorActiveRange<Usize>{ *this };
    }

    ALT_NODISCARD ALT_FORCE_INLINE BitSetIteratorActiveRange<Usize> GetIteratorActiveRange(const usize bitOffset) const noexcept {
        ALT_ASSERT(bitOffset <= GetSize());

        return BitSetIteratorActiveRange<Usize>{ *this, bitOffset };
    }

    ALT_NODISCARD ALT_FORCE_INLINE BitSetIteratorActiveRange<Usize> GetIteratorActiveRange(const usize bitOffset, const usize bitSize) const noexcept {
        ALT_ASSERT(bitOffset <= GetSize() && bitSize <= GetSize() - bitOffset);

        return BitSetIteratorActiveRange<Usize>{ *this, bitOffset, bitSize };
    }

    /**
     * @brief Returns a immutable iterator only for ranges of '0' bits.
     * @return An `BitSetIteratorInactiveRange`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE BitSetIteratorInactiveRange<Usize> GetIteratorInactiveRange() const noexcept {

        return BitSetIteratorInactiveRange<Usize>{ *this };
    }

    ALT_NODISCARD ALT_FORCE_INLINE BitSetIteratorInactiveRange<Usize> GetIteratorInactiveRange(const usize bitOffset) const noexcept {
        ALT_ASSERT(bitOffset <= GetSize());

        return BitSetIteratorInactiveRange<Usize>{ *this, bitOffset };
    }

    ALT_NODISCARD ALT_FORCE_INLINE BitSetIteratorInactiveRange<Usize> GetIteratorInactiveRange( const usize bitOffset, const usize bitSize) const noexcept {
        ALT_ASSERT(bitOffset <= GetSize() && bitSize <= GetSize() - bitOffset);

        return BitSetIteratorInactiveRange<Usize>{ *this, bitOffset, bitSize };
    }

private:

    template <typename Usize_, usize MIN_BITS_CAPACITY_ON_ALLOCATE_>
    class Entry {
    public:

        ALT_FORCE_INLINE Entry(BitSetT<Usize_, MIN_BITS_CAPACITY_ON_ALLOCATE_>& bs, const usize idx) noexcept
            : m_pBitSet(&bs)
            , m_Idx(idx)
        {}

        ALT_FORCE_INLINE Entry& operator=(const bool val) noexcept {

            m_pBitSet->Set(m_Idx, val);
            return *this;
        }

        ALT_FORCE_INLINE operator bool() const noexcept {
            return m_pBitSet->Get(m_Idx);
        } 

    private:

        BitSetT<Usize_, MIN_BITS_CAPACITY_ON_ALLOCATE_>* m_pBitSet;
        usize m_Idx;
    };

    template <typename Usize_, usize MIN_BITS_CAPACITY_ON_ALLOCATE_>
    class EntryConst {
    public:

        ALT_FORCE_INLINE EntryConst(const BitSetT<Usize_, MIN_BITS_CAPACITY_ON_ALLOCATE_>& bs, const usize idx) noexcept
            : m_pBitSet(&bs)
            , m_Idx(idx)
        {}

        ALT_FORCE_INLINE operator bool() const noexcept {
            return m_pBitSet->Get(m_Idx);
        } 

    private:

        const BitSetT<Usize_, MIN_BITS_CAPACITY_ON_ALLOCATE_>* m_pBitSet;
        usize m_Idx;
    };

    template <typename, usize>
    friend class BitSetT;

    template <typename>
    friend class BitSetIterator;

    template <typename>
    friend class BitSetIteratorActiveRange;

    template <typename>
    friend class BitSetIteratorInactiveRange;

    constexpr static usize BITS = Num::Desc<Usize>::Bits();
    static_assert(Num::IsPowerOf2(BITS), "");

    constexpr static usize BITS_TO_WORDID_RIGHT_SHIFT = Num::Count0BitsRightConstexpr(BITS);
    constexpr static usize BITS_TO_BITIDX_IN_WORD_MASK = BITS - 1;

    constexpr static usize MIN_WORDS_CAPACITY_ON_ALLOCATE =
        MIN_BITS_CAPACITY_ON_ALLOCATE != 0
        ? Num::RoundUpToPowerOf2Constexpr(
            (MIN_BITS_CAPACITY_ON_ALLOCATE >> BITS_TO_WORDID_RIGHT_SHIFT) + ((MIN_BITS_CAPACITY_ON_ALLOCATE & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1)
        )
        : 0;

    Vector<Usize, MIN_WORDS_CAPACITY_ON_ALLOCATE> m_Words;
    usize m_SizeBits = 0;
};


/**
 * @brief Format specialization for BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>.
 */
template <typename Usize, usize MIN_BITS_CAPACITY_ON_ALLOCATE>
class Format<BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>> {
public:

    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, const Slice<const char> parameters, const BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>& vals
    ) noexcept {
        iferr_scope;

        bool isAllInOneLine = true;
        bool isBits = true;

        SplitIterator<const char> iter = parameters.GetSplitIterator(";");
        for (const Slice<const char> split : iter) {

            if (split == "inline"_s) {

                isAllInOneLine = true;
                continue;
            }

            if (split == "noinline"_s) {

                isAllInOneLine = false;
                continue;
            }

            if (split == "bits"_s) {

                isBits = true;
                continue;
            }

            if (split == "indices"_s) {

                isBits = false;
                continue;
            }

            return Nullopt;
        }

        if (isAllInOneLine) {

            EstimateExactSizeResult result{};
            result.size = 1;

            usize size = vals.GetSize();

            if (isBits) {

                // '0' or '1'
                result.size += size;

                // ', '
                result.size += Num::SubWrap(size, 1) * 2;
            }
            else {

                // Don't calculate precise size of numbers

                // ', '
                result.size += Num::SubWrap(size, 1) * 2;
            }

            result.size += 1;

            return result;
        }

        if (!isAllInOneLine) {

            EstimateExactSizeResult result{};
            result.size = 1;

            usize size = vals.GetSize();

            estimator.PushIndent();
            result.size += size * estimator.GetIndentEstimatedSize();

            if (isBits) {

                // '\n'
                result.size += size;

                // '0' or '1'
                result.size += size;

                // ','
                result.size += Num::SubWrap(size, 1);
            }
            else {

                // '\n'
                result.size += size;

                // Don't calculate precise size of numbers

                // ','
                result.size += Num::SubWrap(size, 1);
            }

            estimator.PopIndent();

            result.size += 2;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(
        FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const BitSetT<Usize, MIN_BITS_CAPACITY_ON_ALLOCATE>& vals
    ) noexcept {
        iferr_scope;

        bool isAllInOneLine = true;
        bool isBits = true;

        SplitIterator<const char> iter = parameters.GetSplitIterator(";");
        for (const Slice<const char> split : iter) {

            if (split == "inline"_s) {

                isAllInOneLine = true;
                continue;
            }

            if (split == "noinline"_s) {

                isAllInOneLine = false;
                continue;
            }

            if (split == "bits"_s) {

                isBits = true;
                continue;
            }

            if (split == "indices"_s) {

                isBits = false;
                continue;
            }

            return false;
        }

        if (isAllInOneLine) {

            writer.Append('[') iferr_return false;

            if (isBits) {

                auto iter = vals
                    .GetIterator()
                    .Enumerate();

                const Pair<usize, bool>& val = iter.Next() ifok {

                    if (val.s()) {
                        writer.Append('1') iferr_return false;
                    }
                    else {
                        writer.Append('0') iferr_return false;
                    }
                }

                for (const Pair<usize, bool>& val : iter) {

                    writer.Append(',') iferr_return false;
                    writer.Append(' ') iferr_return false;

                    if (val.s()) {
                        writer.Append('1') iferr_return false;
                    }
                    else {
                        writer.Append('0') iferr_return false;
                    }
                }
            }
            else {

                auto iter = vals
                    .GetIteratorActiveRange()
                    .Enumerate();

                const Pair<usize, RangeUsize>& val = iter.Next() ifok {

                    const RangeUsize range = val.s();
                    for (usize i = range.begin; i != range.end; ++i) {

                        if (i != range.begin) {

                            writer.Append(',') iferr_return false;
                            writer.Append(' ') iferr_return false;
                        }

                        Format<usize>::WriteAppend(writer, ""_s, i) iferr_return false;
                    }
                }

                for (const Pair<usize, RangeUsize>& val : iter) {

                    const RangeUsize range = val.s();
                    for (usize i = range.begin; i != range.end; ++i) {

                        writer.Append(',') iferr_return false;
                        writer.Append(' ') iferr_return false;

                        Format<usize>::WriteAppend(writer, ""_s, i) iferr_return false;
                    }
                }
            }

            writer.Append(']') iferr_return false;

            return true;
        }

        if (!isAllInOneLine) {

            writer.Append('[') iferr_return false;
            writer.PushIndent();

            if (isBits) {

                auto iter = vals
                    .GetIterator()
                    .Enumerate();

                const Pair<usize, bool>& val = iter.Next() ifok {

                    writer.Append('\n') iferr_return false;
                    writer.AppendIndent() iferr_return false;

                    if (val.s()) {
                        writer.Append('1') iferr_return false;
                    }
                    else {
                        writer.Append('0') iferr_return false;
                    }
                }

                for (const Pair<usize, bool>& val : iter) {

                    writer.Append(',') iferr_return false;
                    writer.Append('\n') iferr_return false;
                    writer.AppendIndent() iferr_return false;

                    if (val.s()) {
                        writer.Append('1') iferr_return false;
                    }
                    else {
                        writer.Append('0') iferr_return false;
                    }
                }
            }
            else {

                auto iter = vals
                    .GetIteratorActiveRange()
                    .Enumerate();

                const Pair<usize, RangeUsize>& val = iter.Next() ifok {

                    const RangeUsize range = val.s();
                    for (usize i = range.begin; i != range.end; ++i) {

                        if (i != range.begin) {

                            writer.Append(',') iferr_return false;
                        }

                        writer.Append('\n') iferr_return false;
                        writer.AppendIndent() iferr_return false;

                        Format<usize>::WriteAppend(writer, ""_s, i) iferr_return false;
                    }
                }

                for (const Pair<usize, RangeUsize>& val : iter) {

                    const RangeUsize range = val.s();
                    for (usize i = range.begin; i != range.end; ++i) {

                        writer.Append(',') iferr_return false;
                        writer.Append('\n') iferr_return false;
                        writer.AppendIndent() iferr_return false;

                        Format<usize>::WriteAppend(writer, ""_s, i) iferr_return false;
                    }
                }
            }

            writer.PopIndent();
            writer.Append('\n') iferr_return false;
            writer.Append(']') iferr_return false;

            return true;
        }

        return false;
    }
};

} // namespace Alt

#endif // __ALT_STD_BITSET__