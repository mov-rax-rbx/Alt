#pragma once

#ifndef __ALT_STD_VECTOR_ITERATOR__
#define __ALT_STD_VECTOR_ITERATOR__

#include "../TypeTraits.h"

#include "../Iterators/Iterator.h"

namespace Alt {

template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
class Vector;

template <typename T, usize SIZE, typename Usize>
class StaticVector;

/**
 * @brief An iterator that allows traversing the Vector.
 * @details This iterator maintains its own copy of the vector's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Vector object.
 * @tparam T The numeric type of the Vector elements.
 */
template <typename T>
class VectorIterator final : public Iterator<VectorIterator<T>, T&, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait> {
public:

    /**
     * @brief Constructs an iterator from a specific Vector.
     * @param self The Vector object to copy boundaries from.
     */
    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE VectorIterator(Vector<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
    {}

    /**
     * @brief Constructs an iterator from a specific StaticVector.
     * @param self The Vector object to copy boundaries from.
     */
    template <usize SIZE, typename Usize>
    ALT_FORCE_INLINE VectorIterator(StaticVector<T, SIZE, Usize>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
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
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::VectorIterator<usize> it = vec.GetIterator();
     * 
     * assert(*it.Next() == 0);
     * assert(*it.Next() == 1);
     */
    ALT_FORCE_INLINE Optional<T&> Next() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        T* pResult = m_pBegin;
        m_pBegin += 1;

        return *pResult;
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::VectorIterator<usize> it = vec.GetIterator();
     * 
     * const usize skipped = it.Advance(3);
     * 
     * assert(skipped == 3);
     * assert(it.GetSize() == 2);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pBegin += advance;

        return advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::VectorIterator<usize> it = vec.GetIterator();
     * 
     * assert(*it.NextBack() == 4);
     * assert(*it.NextBack() == 3);
     */
    ALT_FORCE_INLINE Optional<T&> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        m_pEnd -= 1;
        T* pResult = m_pEnd;

        return *pResult;
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::VectorIterator<usize> it = vec.GetIterator();
     * 
     * const usize skipped = it.AdvanceBack(1);
     * 
     * assert(skipped == 1);
     * assert(it.GetSize() == 4);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pEnd -= advance;

        return advance;
    }

    /**
     * @brief Returns the remaining size.
     * @return The number of elements left in the iterator's Vector.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    T* m_pBegin;
    T* m_pEnd;
};

/**
 * @brief An iterator that allows traversing the Vector.
 * @details This iterator maintains its own copy of the Vector's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Vector object.
 * @tparam T The numeric type of the Vector elements.
 */
template <typename T>
class VectorIteratorConst final : public Iterator<VectorIteratorConst<T>, const T&, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait> {
public:

    /**
     * @brief Constructs an iterator from a specific Vector.
     * @param self The Vector object to copy boundaries from.
     */
    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE VectorIteratorConst(const Vector<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
    {}

    /**
     * @brief Constructs an iterator from a specific StaticVector.
     * @param self The Vector object to copy boundaries from.
     */
    template <usize SIZE, typename Usize>
    ALT_FORCE_INLINE VectorIteratorConst(const StaticVector<T, SIZE, Usize>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
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
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::VectorIteratorConst<usize> it = vec.GetIterator();
     * 
     * assert(*it.Next() == 0);
     * assert(*it.Next() == 1);
     */
    ALT_FORCE_INLINE Optional<const T&> Next() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const T* pResult = m_pBegin;
        m_pBegin += 1;

        return *pResult;
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::VectorIteratorConst<usize> it = vec.GetIterator();
     * 
     * const usize skipped = it.Advance(3);
     * 
     * assert(skipped == 3);
     * assert(it.GetSize() == 2);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pBegin += advance;

        return advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::VectorIteratorConst<usize> it = vec.GetIterator();
     * 
     * assert(*it.NextBack() == 4);
     * assert(*it.NextBack() == 3);
     */
    ALT_FORCE_INLINE Optional<const T&> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        m_pEnd -= 1;
        const T* pResult = m_pEnd;

        return *pResult;
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::VectorIteratorConst<usize> it = vec.GetIterator();
     * 
     * const usize skipped = it.AdvanceBack(1);
     * 
     * assert(skipped == 1);
     * assert(it.GetSize() == 4);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pEnd -= advance;

        return advance;
    }

    /**
     * @brief Returns the remaining size.
     * @return The number of elements left in the iterator's Vector.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const T* m_pBegin;
    const T* m_pEnd;
};

} // namespace Alt

#endif // __ALT_STD_VECTOR_ITERATOR__