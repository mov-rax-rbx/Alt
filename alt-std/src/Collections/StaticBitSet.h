#pragma once

#ifndef __ALT_STD_STATIC_BITSET__
#define __ALT_STD_STATIC_BITSET__

#include "../String/Impl/FormatPreInclude.h"

#include "BitSetIterator.h"

namespace Alt {

/**
 * @brief A bitset container used to store and manipulate a collection of bit flags.
 * @details StaticBitSet stores bits packed into words (of type Usize).
 * @tparam SIZE_IN_BITS The number of bits.
 * @tparam Usize The underlying integer type used to represent a word. Must be an unsigned scalar type.
 */
template <usize SIZE_IN_BITS, typename Usize>
class StaticBitSet;

namespace TypeTraits {

template <usize SIZE_IN_BITS, typename Usize>
struct __is_trivially_reallocatable__<StaticBitSet<SIZE_IN_BITS, Usize>>
    : std::true_type
{};

} // namespace TypeTraits

namespace _StaticBitSetDetails {

template <usize SIZE_IN_BITS>
using size_in_bits_to_type_t = std::conditional_t<
    SIZE_IN_BITS <= Num::Desc<u8>::Bits(),
    u8,

    std::conditional_t<
        SIZE_IN_BITS <= Num::Desc<u16>::Bits(),
        u16,

        std::conditional_t<
            SIZE_IN_BITS <= Num::Desc<u32>::Bits(),
            u32,

            std::conditional_t<
                SIZE_IN_BITS <= Num::Desc<u64>::Bits(),
                u64,
                umax
            >
        >
    >
>;

} // namespace _StaticBitSetDetails

template <usize SIZE_IN_BITS, typename Usize = _StaticBitSetDetails::size_in_bits_to_type_t<SIZE_IN_BITS>>
class StaticBitSet {
private:

    static_assert(std::is_scalar_v<Usize> && std::is_unsigned_v<Usize>, "");

    template <usize, typename>
    class Entry;

    template <usize, typename>
    class EntryConst;

public:

    /**
     * @brief Creates a StaticBitSet with the minimum required bit size.
     * @return An StaticBitSet.
     * @example
     * 
     * Alt::StaticBitSet<64> bs = Alt::StaticBitSet<64>::Create(true);
     */
    ALT_NODISCARD ALT_FORCE_INLINE static StaticBitSet Create(const bool val = false) noexcept {

        return StaticBitSet{ val };
    }

    ALT_FORCE_INLINE StaticBitSet(const bool val = false) noexcept {

        std::memset(m_Words, val ? Num::Desc<s32>::Max() : 0, SIZE * sizeof(Usize));
    }

    /**
     * @brief Sets all bits in the StaticBitSet.
     * @param val Set all bits to true if true, else false.
     * @example
     * 
     * Alt::StaticBitSet<64> bs;
     * bs.SetAll(true);
     */
    ALT_FORCE_INLINE void SetAll(const bool val = true) noexcept {

        std::memset(m_Words, val ? Num::Desc<s32>::Max() : 0, SIZE * sizeof(Usize));
    }

    /**
     * @brief Sets the state of a specific bit in the StaticBitSet.
     * @param i The index of the bit to set.
     * @param val The value to assign to the bit.
     * @example
     * 
     * Alt::StaticBitSet<64> bs;
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
     * Alt::StaticBitSet<64> bs{ true };
     * 
     * assert(bs.Get(0));
     * assert(bs[0]);
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Get(const usize i) const noexcept {
        ALT_ASSERT(i < GetSize());

        return ((m_Words[i >> BITS_TO_WORDID_RIGHT_SHIFT] >> (i & BITS_TO_BITIDX_IN_WORD_MASK)) & 0b1) == 1;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Entry<SIZE_IN_BITS, Usize> operator[](const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        return Entry<SIZE_IN_BITS, Usize>{ *this, i };
    }

    ALT_NODISCARD ALT_FORCE_INLINE EntryConst<SIZE_IN_BITS, Usize> operator[](const usize i) const noexcept {
        ALT_ASSERT(i < GetSize());

        return EntryConst<SIZE_IN_BITS, Usize>{ *this, i };
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

        const Usize* pBegin = Data() + wordOffset;
        const Usize* pEnd = DataEnd();

        pBegin += pBegin != pEnd && (*pBegin >> offset) == 0
            ? 1
            : 0;

        while (pBegin != pEnd && *pBegin == 0) {

            pBegin += 1;
        }

        if (pBegin == pEnd) {
            return Nullopt;
        }

        const usize bitIdx = (pBegin - Data()) * DIGITS + Num::Count0BitsRight(*pBegin);

        return bitIdx < GetSize()
            ? MakeOptional(bitIdx)
            : Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirstSet() const noexcept {

        const Usize* pBegin = Data();
        const Usize* pEnd = DataEnd();

        while (pBegin != pEnd && *pBegin == 0) {

            pBegin += 1;
        }

        if (pBegin == pEnd) {
            return Nullopt;
        }

        const usize bitIdx = (pBegin - Data()) * DIGITS + Num::Count0BitsRight(*pBegin);

        return bitIdx < GetSize()
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

        const Usize* pBegin = Data() + wordOffset;
        const Usize* pEnd = DataEnd();

        pBegin += pBegin != pEnd && (*pBegin >> offset) == (WORD_MAX >> offset)
            ? 1
            : 0;

        while (pBegin != pEnd && *pBegin == WORD_MAX) {

            pBegin += 1;
        }

        if (pBegin == pEnd) {
            return Nullopt;
        }

        const usize bitIdx = (pBegin - Data()) * DIGITS + Num::Count0BitsRight(~*pBegin);

        return bitIdx < GetSize()
            ? MakeOptional(bitIdx)
            : Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FindFirstNonSet() const noexcept {

        constexpr static Usize WORD_MAX = Num::Desc<Usize>::Max();

        const Usize* pBegin = Data();
        const Usize* pEnd = DataEnd();

        while (pBegin != pEnd && *pBegin == WORD_MAX) {

            pBegin += 1;
        }

        if (pBegin == pEnd) {
            return Nullopt;
        }

        const usize bitIdx = (pBegin - Data()) * DIGITS + Num::Count0BitsRight(~*pBegin);

        return bitIdx < GetSize()
            ? MakeOptional(bitIdx)
            : Nullopt;
    }

    /**
     * @brief Returns the number of bits currently in the StaticBitSet.
     * @return The size of the StaticBitSet.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetSize() const noexcept {

        return SIZE_IN_BITS;
    }

    /**
     * @brief Operators for comparison with BitSet.
     * @example
     *
     * iferr_scope;
     * 
     * Alt::StaticBitSet<64> bs0{ false };
     * Alt::StaticBitSet<24> bs1{ true };
     * 
     * assert(bs0 != bs1);
     */
    template <usize RHS_SIZE_IN_BITS = SIZE_IN_BITS, typename U = Usize,
        std::enable_if_t<RHS_SIZE_IN_BITS == SIZE_IN_BITS>* = nullptr>
    ALT_NODISCARD bool operator==(const StaticBitSet<RHS_SIZE_IN_BITS, U>& rhs) const noexcept {

        if (SIZE > 1) {

            const bool isEq = Algorithms::CompareEQ(m_Words, m_Words + SIZE - 1, rhs.m_Words, rhs.m_Words + SIZE - 1);
            if (!isEq) {

                return false;
            }
        }

        const usize idx = DIGITS - (SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK);
        return (m_Words[SIZE - 1] << idx) == (rhs.m_Words[SIZE - 1] << idx);
    }

    template <usize RHS_SIZE_IN_BITS = SIZE_IN_BITS, typename U = Usize,
        std::enable_if_t<RHS_SIZE_IN_BITS != SIZE_IN_BITS>* = nullptr>
    ALT_NODISCARD bool operator==(const StaticBitSet<RHS_SIZE_IN_BITS, U>& rhs) const noexcept {

        return false;
    }

    template <usize RHS_SIZE_IN_BITS = SIZE_IN_BITS, typename U = Usize>
    ALT_NODISCARD bool operator!=(const StaticBitSet<RHS_SIZE_IN_BITS, U>& rhs) const noexcept {

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
        ALT_ASSERT(bitOffset <= GetSize() && bitSize < GetSize() - bitOffset);

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

    ALT_NODISCARD ALT_FORCE_INLINE BitSetIteratorInactiveRange<Usize> GetIteratorInactiveRange(const usize bitOffset, const usize bitSize) const noexcept {
        ALT_ASSERT(bitOffset <= GetSize() && bitSize < GetSize() - bitOffset);

        return BitSetIteratorInactiveRange<Usize>{ *this, bitOffset, bitSize };
    }

private:

    ALT_FORCE_INLINE Usize* Data() noexcept { return m_Words; }
    ALT_FORCE_INLINE const Usize* Data() const noexcept { return m_Words; }

    ALT_FORCE_INLINE Usize* DataEnd() noexcept { return m_Words + SIZE; }
    ALT_FORCE_INLINE const Usize* DataEnd() const noexcept { return m_Words + SIZE; }

    template <usize SIZE_IN_BITS_, typename Usize_>
    class Entry {
    public:

        ALT_FORCE_INLINE Entry(StaticBitSet<SIZE_IN_BITS_, Usize_>& bs, const usize idx) noexcept
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

        StaticBitSet<SIZE_IN_BITS_, Usize_>* m_pBitSet;
        usize m_Idx;
    };

    template <usize SIZE_IN_BITS_, typename Usize_>
    class EntryConst {
    public:

        ALT_FORCE_INLINE EntryConst(const StaticBitSet<SIZE_IN_BITS_, Usize_>& bs, const usize idx) noexcept
            : m_pBitSet(&bs)
            , m_Idx(idx)
        {}

        ALT_FORCE_INLINE operator bool() const noexcept {
            return m_pBitSet->Get(m_Idx);
        } 

    private:

        const StaticBitSet<SIZE_IN_BITS_, Usize_>* m_pBitSet;
        usize m_Idx;
    };

private:

    template <usize, typename>
    friend class StaticBitSet;

    template <typename>
    friend class BitSetIterator;

    template <typename>
    friend class BitSetIteratorActiveRange;

    template <typename>
    friend class BitSetIteratorInactiveRange;

    constexpr static usize DIGITS = Num::Desc<Usize>::Bits();
    static_assert(Num::IsPowerOf2(DIGITS), "");

    constexpr static usize BITS_TO_WORDID_RIGHT_SHIFT = Num::Count0BitsRightConstexpr(DIGITS);
    constexpr static usize BITS_TO_BITIDX_IN_WORD_MASK = DIGITS - 1;

    constexpr static usize SIZE =
        Num::Max((SIZE_IN_BITS >> BITS_TO_WORDID_RIGHT_SHIFT) + ((SIZE_IN_BITS & BITS_TO_BITIDX_IN_WORD_MASK) == 0 ? 0 : 1), 1);

    Usize m_Words[SIZE];
};

/**
 * @brief Format specialization for StaticBitSet<SIZE_IN_BITS, Usize>.
 */
template <usize SIZE_IN_BITS, typename Usize>
class Format<StaticBitSet<SIZE_IN_BITS, Usize>> {
public:

    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, const Slice<const char> parameters, const StaticBitSet<SIZE_IN_BITS, Usize>& vals
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
        FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const StaticBitSet<SIZE_IN_BITS, Usize>& vals
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

#endif // __ALT_STD_STATIC_BITSET__