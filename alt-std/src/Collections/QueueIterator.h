#pragma once

#ifndef __ALT_STD_QUEUE_ITERATOR__
#define __ALT_STD_QUEUE_ITERATOR__

#include "../TypeTraits.h"

#include "../Iterators/Iterator.h"

namespace Alt {

template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
class Queue;

template <typename T, usize SIZE>
class StaticQueue;

/**
 * @brief An iterator that allows traversing the Queue.
 * @details This iterator maintains its own copy of the queue's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Queue object.
 * @tparam T The numeric type of the Queue elements.
 */
template <typename T>
class QueueIterator final : public Iterator<QueueIterator<T>, T&, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait> {
public:

    /**
     * @brief Constructs an iterator from a specific Queue.
     * @param self The Queue object to copy boundaries from.
     */
    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE QueueIterator(Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>& self) noexcept
        : m_pMemoryBegin(self.m_pMemoryBegin)
        , m_pMemoryEnd(self.m_pMemoryEnd)
        , m_pQueueBegin(self.m_pQueueBegin)
        , m_Size(self.m_Size)
    {}

    /**
     * @brief Constructs an iterator from a specific StaticQueue.
     * @param self The StaticQueue object to copy boundaries from.
     */
    template <usize SIZE>
    ALT_FORCE_INLINE QueueIterator(StaticQueue<T, SIZE>& self) noexcept
        : m_pMemoryBegin(self.GetMemoryBegin())
        , m_pMemoryEnd(self.GetMemoryEnd())
        , m_pQueueBegin(self.GetQueueBegin())
        , m_Size(self.GetSize())
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
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::Create(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::QueueIterator<usize> it = q.GetIterator();
     * 
     * assert(*it.Next() == 0);
     * assert(*it.Next() == 1);
     */
    ALT_FORCE_INLINE Optional<T&> Next() noexcept {

        if (m_Size == 0) {
            return Nullopt;
        }

        m_pQueueBegin = m_pQueueBegin == m_pMemoryEnd ? m_pMemoryBegin : m_pQueueBegin;

        T* pResult = m_pQueueBegin;
        m_pQueueBegin += 1;
        m_Size -= 1;

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
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::QueueIterator<usize> it = q.GetIterator();
     * 
     * const usize skipped = it.Advance(3);
     * 
     * assert(skipped == 3);
     * assert(it.GetSize() == 2);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, m_Size);

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        if (advance > sizeBottomCapacity) {

            m_pQueueBegin = m_pMemoryBegin + advance - sizeBottomCapacity;
        }
        else {

            m_pQueueBegin += advance;
        }

        m_Size -= advance;

        return advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::QueueIterator<usize> it = q.GetIterator();
     * 
     * assert(*it.NextBack() == 4);
     * assert(*it.NextBack() == 3);
     */
    ALT_FORCE_INLINE Optional<T&> NextBack() noexcept {

        if (m_Size == 0) {
            return Nullopt;
        }

        m_Size -= 1;

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

        const usize offset = m_Size < sizeBottomCapacity ? m_Size : m_Size - sizeBottomCapacity;
        T* pMemory = m_Size < sizeBottomCapacity ? m_pQueueBegin : m_pMemoryBegin;

        return pMemory[offset];
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::QueueIterator<usize> it = q.GetIterator();
     * 
     * const usize skipped = it.AdvanceBack(1);
     * 
     * assert(skipped == 1);
     * assert(it.GetSize() == 4);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, m_Size);
        m_Size -= advance;

        return advance;
    }

    /**
     * @brief Returns the remaining size.
     * @return The number of elements left in the iterator's Queue.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_Size;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_Size == 0;
    }

private:

    T* m_pMemoryBegin;
    T* m_pMemoryEnd;

    T* m_pQueueBegin;
    usize m_Size;
};

/**
 * @brief An iterator that allows traversing the Queue.
 * @details This iterator maintains its own copy of the queue's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Queue object.
 * @tparam T The numeric type of the Queue elements.
 */
template <typename T>
class QueueIteratorConst final : public Iterator<QueueIteratorConst<T>, const T&, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait> {
public:

    /**
     * @brief Constructs an iterator from a specific Queue.
     * @param self The Queue object to copy boundaries from.
     */
    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE QueueIteratorConst(const Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>& self) noexcept
        : m_pMemoryBegin(self.m_pMemoryBegin)
        , m_pMemoryEnd(self.m_pMemoryEnd)
        , m_pQueueBegin(self.m_pQueueBegin)
        , m_Size(self.m_Size)
    {}

    /**
     * @brief Constructs an iterator from a specific StaticQueue.
     * @param self The StaticQueue object to copy boundaries from.
     */
    template <usize SIZE>
    ALT_FORCE_INLINE QueueIteratorConst(StaticQueue<T, SIZE>& self) noexcept
        : m_pMemoryBegin(self.GetMemoryBegin())
        , m_pMemoryEnd(self.GetMemoryEnd())
        , m_pQueueBegin(self.GetQueueBegin())
        , m_Size(self.GetSize())
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
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::QueueIteratorConst<usize> it = q.GetIterator();
     * 
     * assert(*it.Next() == 0);
     * assert(*it.Next() == 1);
     */
    ALT_FORCE_INLINE Optional<const T&> Next() noexcept {

        if (m_Size == 0) {
            return Nullopt;
        }

        m_pQueueBegin = m_pQueueBegin == m_pMemoryEnd ? m_pMemoryBegin : m_pQueueBegin;

        const T* pResult = m_pQueueBegin;
        m_pQueueBegin += 1;
        m_Size -= 1;

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
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::QueueIteratorConst<usize> it = q.GetIterator();
     * 
     * const usize skipped = it.Advance(3);
     * 
     * assert(skipped == 3);
     * assert(it.GetSize() == 2);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, m_Size);
        m_Size -= advance;

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        if (advance > sizeBottomCapacity) {

            m_pQueueBegin = m_pMemoryBegin + advance - sizeBottomCapacity;
        }
        else {

            m_pQueueBegin += advance;
        }

        return advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::QueueIteratorConst<usize> it = q.GetIterator();
     * 
     * assert(*it.NextBack() == 4);
     * assert(*it.NextBack() == 3);
     */
    ALT_FORCE_INLINE Optional<const T&> NextBack() noexcept {

        if (m_Size == 0) {
            return Nullopt;
        }

        m_Size -= 1;

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

        const usize offset = m_Size < sizeBottomCapacity ? m_Size : m_Size - sizeBottomCapacity;
        const T* pMemory = m_Size < sizeBottomCapacity ? m_pQueueBegin : m_pMemoryBegin;

        return pMemory[offset];
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3, 4) iferr_return;
     * // Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 1, 2, 3, 4) iferr_return;
     * 
     * Alt::QueueIteratorConst<usize> it = q.GetIterator();
     * 
     * const usize skipped = it.AdvanceBack(1);
     * 
     * assert(skipped == 1);
     * assert(it.GetSize() == 4);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, m_Size);
        m_Size -= advance;

        return advance;
    }

    /**
     * @brief Returns the remaining size.
     * @return The number of elements left in the iterator's Queue.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_Size;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_Size == 0;
    }

private:

    const T* m_pMemoryBegin;
    const T* m_pMemoryEnd;

    const T* m_pQueueBegin;
    usize m_Size;
};

} // namespace Alt

#endif // __ALT_STD_QUEUE_ITERATOR__