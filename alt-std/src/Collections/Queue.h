#pragma once

#ifndef __ALT_STD_QUEUE__
#define __ALT_STD_QUEUE__

#include "../IfErr.h"
#include "../TypeTraits.h"

#include "../String/Impl/FormatPreInclude.h"

#include "../Memory/Memory.h"

#include "../Utils/Num.h"
#include "../Utils/Slice.h"
#include "../Utils/Optional.h"
#include "../Utils/ScopeDefer.h"

#include "QueueIterator.h"

namespace Alt {

template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE = 16>
class Queue;

namespace TypeTraits {

template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
struct __is_trivially_reallocatable__<Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>>
    : is_trivially_reallocatable_t<T*>
{};

} // namespace TypeTraits

/**
 * @brief A deque utilizing a contiguous memory block.
 * @details The Queue provides $O(1)$ insertion and removal from both the front and back - manages
 * a circular/sliding buffer window over a pre-allocated memory block.
 * 
 * @tparam T The type of elements stored in the queue.
 * @tparam MIN_ELEMENT_CAPACITY_ON_ALLOCATE The minimum capacity to reserve upon allocation.
 * Must be a power of two.
 */
template <typename T_, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
class Queue {
private:

    using T = TypeTraits::clean_t<T_>;

    template <typename U>
    using is_pushable_t = std::disjunction<TypeTraits::is_only_clonable_to_from_t<T, U>, std::is_constructible<T, U>>;

    template <typename U>
    constexpr static bool is_pushable_v = is_pushable_t<U>::value;

    template <typename ...ArgsT>
    constexpr static bool is_populated_v = sizeof...(ArgsT) != 0;

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

    static_assert(MIN_ELEMENT_CAPACITY_ON_ALLOCATE == 0 || Num::IsPowerOf2(MIN_ELEMENT_CAPACITY_ON_ALLOCATE), "");

public:

    using Type = T;

    ALT_FORCE_INLINE constexpr static usize GetMinElementCapacityOnAllocate() noexcept {

        return MIN_ELEMENT_CAPACITY_ON_ALLOCATE;
    }

    /**
     * @brief Allocates a Queue of a specific size, filled with a default value.
     * @param size The number of elements to initialize.
     * @return An Alt::Optional containing the Queue, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::Allocate(4) iferr_return;
     * 
     * assert(q.GetCapacity() == Alt::Queue<usize>::GetMinElementCapacityOnAllocate());
     * 
     * assert(q.GetSize() == 4);
     * assert(q[0] == 0);
     * assert(q[1] == 0);
     */
    ALT_NODISCARD static Optional<Queue> Allocate() noexcept {
        iferr_scope;

        Queue result{};
        result.Reserve(MIN_ELEMENT_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;

        return result;
    }

    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD static Optional<Queue> Allocate(const usize size) noexcept {
        iferr_scope;

        Queue result{};

        if (size < MIN_ELEMENT_CAPACITY_ON_ALLOCATE) {

            result.Reserve(MIN_ELEMENT_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
        }

        result.ResizeBack(size) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a Queue of a specific size, filled with a provided value.
     * @tparam U The type of the value. Must be constructible to T.
     * @param size The number of elements to initialize.
     * @param val The value used to fill the queue.
     * @return An Alt::Optional containing the Queue, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::Allocate(4, 44) iferr_return;
     * assert(q.GetSize() == 4);
     * assert(q[0] == 44);
     * assert(q[1] == 44);
     */
    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD static Optional<Queue> Allocate(const usize size, const U& val) noexcept {
        iferr_scope;

        Queue result{};

        if (size < MIN_ELEMENT_CAPACITY_ON_ALLOCATE) {

            result.Reserve(MIN_ELEMENT_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
        }

        result.ResizeBack(size, val) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a Queue with a pre-defined capacity.
     * @param size The desired capacity.
     * @return An Alt::Optional containing the Queue, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocateCapacity(44) iferr_return;
     * assert(q.GetCapacity() >= 44);
     */
    ALT_NODISCARD static Optional<Queue> AllocateCapacity(const usize size) noexcept {
        iferr_scope;

        Queue result{};
        result.Reserve(Num::Max(size, MIN_ELEMENT_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a Queue with a exact pre-defined capacity.
     * @param size The desired capacity.
     * @return An Alt::Optional containing the Queue, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocateCapacityExact(44) iferr_return;
     * assert(q.GetCapacity() == 44);
     */
    ALT_NODISCARD static Optional<Queue> AllocateCapacityExact(const usize size) noexcept {
        iferr_scope;

        Queue result{};
        result.ReserveExact(Num::Max(size, MIN_ELEMENT_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a Queue initialized with a variadic list of arguments.
     * @tparam ArgsT... The types of the arguments to be pushed back into the queue.
     * @param args The values to initialize the queue with.
     * @return An Alt::Optional containing the initialized Queue, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(1, 2, 3, 4) iferr_return;
     * assert(q.GetValue().GetSize() == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    ALT_NODISCARD static Optional<Queue> AllocatePush(ArgsT&&... args) noexcept {
        iferr_scope;

        constexpr usize size = sizeof...(ArgsT);

        Queue result{};
        result.Reserve(Num::Max(size, MIN_ELEMENT_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        PushBackVariadicImpl(result, std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return result;
    }

    ALT_FORCE_INLINE Queue() noexcept = default;

    Queue(const Queue&) noexcept = delete;
    Queue& operator=(const Queue&) noexcept = delete;

    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    Queue(Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& rhs) noexcept
        : Queue(Rule5Move, rhs)
    {}

    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    Queue& operator=(Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~Queue() noexcept {

        Destroy();
    }

    /**
     * @brief Returns the number of elements currently in the queue.
     * @return The size of the queue.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        return m_Size;
    }

    /**
     * @brief Returns the total number of elements the queue can hold before a reallocation is required.
     * @return The capacity of the queue.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetCapacity() const noexcept {

        return static_cast<usize>(m_pMemoryEnd - m_pMemoryBegin);
    }

    /**
     * @brief Checks if the queue contains no elements.
     * @return True if empty, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        return m_Size == 0;
    }

    /**
     * @brief Checks if the queue contains at least one element.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept {

        return m_Size != 0;
    }

    /**
     * @brief Removes all elements from the queue.
     * @details Does not deallocate the underlying memory, but destroys all existing objects.
     */
    void Clear() noexcept {

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);

        Memory::DestroyObjects(m_pQueueBegin, m_pQueueBegin + sizeBottom);
        Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryBegin + m_Size - sizeBottom);

        m_pQueueBegin = m_pMemoryBegin;
        m_Size = 0;
    }

    /**
     * @brief Accesses the element at the specified index.
     * @param i The index of the element.
     * @return A reference to the element.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * assert(q[0] == 0);
     * assert(q[4] == 16);
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        const usize sizeBottom = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        return i < sizeBottom ? m_pQueueBegin[i] : m_pMemoryBegin[i - sizeBottom];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize i) const noexcept {
        ALT_ASSERT(i < GetSize());

        const usize sizeBottom = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        return i < sizeBottom ? m_pQueueBegin[i] : m_pMemoryBegin[i - sizeBottom];
    }

    /**
     * @brief Returns a reference to the first (front) element.
     * @return Reference to the first element.
     * @note Asserts that the queue is not empty.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetFirst() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pQueueBegin;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetFirst() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pQueueBegin;
    }

    ALT_NODISCARD ALT_FORCE_INLINE T& GetFront() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pQueueBegin;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetFront() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pQueueBegin;
    }

    /**
     * @brief Returns a reference to the last (back) element.
     * @return Reference to the last element.
     * @note Asserts that the queue is not empty.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetLast() noexcept {
        ALT_ASSERT(!IsEmpty());

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        return m_Size <= sizeBottomCapacity ? m_pQueueBegin[m_Size - 1] : m_pMemoryBegin[m_Size - sizeBottomCapacity - 1];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetLast() const noexcept {
        ALT_ASSERT(!IsEmpty());

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        return m_Size <= sizeBottomCapacity ? m_pQueueBegin[m_Size - 1] : m_pMemoryBegin[m_Size - sizeBottomCapacity - 1];
    }

    ALT_NODISCARD ALT_FORCE_INLINE T& GetBack() noexcept {
        ALT_ASSERT(!IsEmpty());

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        return m_Size <= sizeBottomCapacity ? m_pQueueBegin[m_Size - 1] : m_pMemoryBegin[m_Size - sizeBottomCapacity - 1];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetBack() const noexcept {
        ALT_ASSERT(!IsEmpty());

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        return m_Size <= sizeBottomCapacity ? m_pQueueBegin[m_Size - 1] : m_pMemoryBegin[m_Size - sizeBottomCapacity - 1];
    }

    /**
     * @brief Returns a mutable iterator for the queue.
     * @return An `QueueIterator`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE QueueIterator<T> GetIterator() noexcept {

        return QueueIterator<T>{ *this };
    }

    /**
     * @brief Returns a constant iterator for the queue.
     * @return An `QueueIteratorConst`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE QueueIteratorConst<T> GetIterator() const noexcept {

        return QueueIteratorConst<T>{ *this };
    }

    /**
     * Clonable Trait
     * 
     * @brief A deep copy current queue to another.
     * 
     * @details Return false when allocation or copy fails. On allocation there
     * are nothing changed in internal state, but on copy the Queue contains
     * valid intermediate state of copying.
     * 
     * @return An true if copy success, or false if allocation or copy fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q0 = Alt::Queue<usize>::AllocatePush(4) iferr_return;
     * 
     * Alt::Queue<usize> q1;
     * q1.CloneFrom(q0) iferr_return;
     * 
     * assert(q1[0] == q0[0]);
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || (std::is_assignable_v<T&, const U&> && std::is_constructible_v<T, const U&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& queue_) noexcept {

        if (static_cast<const void*>(this) == &queue_) {
            return true;
        }

        Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue = queue_;
        return CloneFrom(queue);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || (std::is_assignable_v<T&, const U&> && std::is_constructible_v<T, const U&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) noexcept {
        iferr_scope;

        if (static_cast<const void*>(this) == &queue) {
            return true;
        }

        const usize size0 = m_Size;
        const usize size1 = queue.m_Size;

        Reserve(size1) iferr_return false;

        m_Size = 0;

        const usize sizeBottom0Capacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom0 = Num::Min(sizeBottom0Capacity, size0);
        const usize sizeTop0 = size0 - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(queue.m_pMemoryEnd - queue.m_pQueueBegin);
        const usize sizeBottom1 = Num::Min(sizeBottom1Capacity, size1);
        const usize sizeTop1 = size1 - sizeBottom1;

        Memory::CloneFromFirstElementResult<T> resultBottom;
        Memory::CloneFromFirstElementResult<T> resultTop;

        const usize sizeBottomToClone = Num::Min(sizeBottom0, sizeBottom1);
        const usize sizeTopToClone = Num::Min(sizeBottom0 - sizeBottomToClone, sizeTop1);
        const usize sizeBottom0CapacityRemainder = sizeBottom0Capacity - sizeBottom0;
        const usize sizeBottomToConstruct = Num::Min(sizeBottom0CapacityRemainder, sizeBottom1 - sizeBottomToClone);
        const usize sizeTopToConstruct = Num::Min(sizeBottom0CapacityRemainder - sizeBottomToConstruct, sizeTop1 - sizeTopToClone);

        {
            resultBottom = Memory::CloneObjectsFromFirstElementNonOverlap(
                m_pQueueBegin,
                queue.m_pQueueBegin,
                queue.m_pQueueBegin + sizeBottomToClone
            );

            m_Size += static_cast<usize>(resultBottom.pDstObjectWhereCloneFailedOrEnd - m_pQueueBegin);
            if (!resultBottom.isOk) {

                Memory::DestroyObjects(resultBottom.pDstObjectWhereCloneFailedOrEnd, m_pQueueBegin + sizeBottom0);
                Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryBegin + sizeTop0);

                return false;
            }

            resultTop = Memory::CloneObjectsFromFirstElementNonOverlap(
                resultBottom.pDstObjectWhereCloneFailedOrEnd,
                queue.m_pMemoryBegin,
                queue.m_pMemoryBegin + sizeTopToClone
            );

            Memory::DestroyObjects(resultTop.pDstObjectWhereCloneFailedOrEnd, m_pQueueBegin + sizeBottom0);

            m_Size += static_cast<usize>(resultTop.pDstObjectWhereCloneFailedOrEnd - resultBottom.pDstObjectWhereCloneFailedOrEnd);
            if (!resultTop.isOk) {

                Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryBegin + sizeTop0);

                return false;
            }

            resultBottom = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                resultTop.pDstObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd + sizeBottomToConstruct
            );

            m_Size += static_cast<usize>(resultBottom.pDstObjectWhereCloneFailedOrEnd - resultTop.pDstObjectWhereCloneFailedOrEnd);
            if (!resultBottom.isOk) {

                Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryBegin + sizeTop0);

                return false;
            }

            resultTop = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                resultBottom.pDstObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd + sizeTopToConstruct
            );

            m_Size += static_cast<usize>(resultTop.pDstObjectWhereCloneFailedOrEnd - resultBottom.pDstObjectWhereCloneFailedOrEnd);
            if (!resultTop.isOk) {

                Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryBegin + sizeTop0);

                return false;
            }
        }

        const usize sizeBottom1Remainder = sizeBottom1 - sizeBottomToClone - sizeBottomToConstruct;
        const usize sizeTop1Remainder = sizeTop1 - sizeTopToClone - sizeTopToConstruct;

        {
            const usize sizeBottomToClone = Num::Min(sizeTop0, sizeBottom1Remainder);
            resultBottom = Memory::CloneObjectsFromFirstElementNonOverlap(
                m_pMemoryBegin,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd + sizeBottomToClone
            );

            m_Size += static_cast<usize>(resultBottom.pDstObjectWhereCloneFailedOrEnd - m_pMemoryBegin);
            if (!resultBottom.isOk) {

                Memory::DestroyObjects(resultBottom.pDstObjectWhereCloneFailedOrEnd, m_pMemoryBegin + sizeTop0);

                return false;
            }

            const usize sizeTopToClone = Num::Min(sizeTop0 - sizeBottomToClone, sizeTop1Remainder);
            resultTop = Memory::CloneObjectsFromFirstElementNonOverlap(
                resultBottom.pDstObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd + sizeTopToClone
            );

            Memory::DestroyObjects(resultTop.pDstObjectWhereCloneFailedOrEnd, m_pMemoryBegin + sizeTop0);

            m_Size += static_cast<usize>(resultTop.pDstObjectWhereCloneFailedOrEnd - resultBottom.pDstObjectWhereCloneFailedOrEnd);
            if (!resultTop.isOk) {

                return false;
            }

            const usize sizeBottomToConstruct = sizeBottom1Remainder - sizeBottomToClone;
            resultBottom = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                resultTop.pDstObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd + sizeBottomToConstruct
            );

            m_Size += static_cast<usize>(resultBottom.pDstObjectWhereCloneFailedOrEnd - resultTop.pDstObjectWhereCloneFailedOrEnd);
            if (!resultBottom.isOk) {

                return false;
            }

            const usize sizeTopToConstruct = sizeTop1Remainder - sizeTopToClone;
            resultTop = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                resultBottom.pDstObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd + sizeTopToConstruct
            );

            m_Size += static_cast<usize>(resultTop.pDstObjectWhereCloneFailedOrEnd - resultBottom.pDstObjectWhereCloneFailedOrEnd);
            if (!resultTop.isOk) {

                return false;
            }
        }

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const InnerU&> || (std::is_assignable_v<T&, const InnerU&> && std::is_constructible_v<T, const InnerU&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(U&& rhs) noexcept {
        iferr_scope;

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sliceSize = slice.GetSize();

        Reserve(sliceSize) iferr_return false;

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom = Num::Min(sizeBottomCapacity, size);

        const usize sizeBottomToClone = Num::Min(sliceSize, sizeBottom);
        Memory::CloneFromFirstElementResult<T> result = Memory::CloneObjectsFromFirstElementNonOverlap(
            m_pQueueBegin, slice.Data(), slice.Data() + sizeBottomToClone
        );

        Memory::DestroyObjects(result.pDstObjectWhereCloneFailedOrEnd, m_pQueueBegin + sizeBottom);

        m_Size = 0;
        m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - m_pQueueBegin);
        if (!result.isOk) {
            return false;
        }

        T* pDst = result.pDstObjectWhereCloneFailedOrEnd;
        const usize sizeBottomToConstruct = Num::Min(sliceSize, sizeBottomCapacity);
        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pDst, result.pSrcObjectWhereCloneFailedOrEnd, slice.Data() + sizeBottomToConstruct
        );

        m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pDst);
        if (!result.isOk) {
            return false;
        }

        const usize sizeTop = size - sizeBottom;
        const usize sizeTopToClone = Num::Min(sliceSize - sizeBottomToConstruct, sizeTop);

        result = Memory::CloneObjectsFromFirstElementNonOverlap(
            m_pMemoryBegin, result.pSrcObjectWhereCloneFailedOrEnd, result.pSrcObjectWhereCloneFailedOrEnd + sizeTopToClone
        );

        Memory::DestroyObjects(result.pDstObjectWhereCloneFailedOrEnd, m_pMemoryBegin + sizeTop);

        m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - m_pMemoryBegin);
        if (!result.isOk) {
            return false;
        }

        pDst = result.pDstObjectWhereCloneFailedOrEnd;
        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pDst, result.pSrcObjectWhereCloneFailedOrEnd, slice.DataEnd()
        );

        m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pDst);

        return result.isOk;
    }

    ALT_NODISCARD bool CloneFrom(const T* pBegin, const usize size) noexcept {

        const Slice<const T> slice{ pBegin, size };
        return CloneFrom(slice);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) noexcept {
        ALT_ASSERT(static_cast<const void*>(this) != &queue);

        iferr_scope_on_err {

            Memory::DestroyObject(*this);
        };

        Memory::ConstructDefaultObject<Queue>(this);

        Reserve(queue.GetSize()) iferr_return false;

        const usize offset = static_cast<usize>(queue.m_pQueueBegin - queue.m_pMemoryBegin);
        const usize sizeBottomCapacity = static_cast<usize>(queue.m_pMemoryEnd - queue.m_pQueueBegin);

        const usize sizeBottom = Num::Min(queue.m_Size, sizeBottomCapacity);

        m_pQueueBegin = m_pMemoryBegin + offset;

        Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            m_pQueueBegin, queue.m_pQueueBegin, queue.m_pQueueBegin + sizeBottom
        );

        if (!result.isOk) {
            iferr_return_return false;
        }

        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            m_pMemoryBegin, queue.m_pMemoryBegin, queue.m_pMemoryBegin + queue.m_Size - sizeBottom
        );

        if (!result.isOk) {
            iferr_return_return false;
        }

        m_Size = queue.m_Size;

        return true;
    }

    ALT_NODISCARD Optional<Queue> Clone() const noexcept {
        iferr_scope;

        MaybeUninit<Queue> valueToConstruct;
        valueToConstruct.GetPtr()->CloneConstructFrom(*this) iferr_return Nullopt;

        defer {
            valueToConstruct.Destroy();
        };

        return std::move(valueToConstruct).GetVal();
    }

    /**
     * @brief A moves eaach element if current queue to another.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q0 = Alt::Queue<usize>::AllocatePush(4) iferr_return;
     * 
     * Alt::Queue<usize> q1;
     * q1.MoveFrom(q0) iferr_return;
     * 
     * assert(q1[0] == 4);
     * assert(!q1.IsEmpty()); // size still valid - just move elements
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool MoveFrom(Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& queue_) noexcept {

        if (static_cast<const void*>(this) == &queue_) {
            return true;
        }

        Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue = queue_;
        return MoveFrom(queue);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool MoveFrom(Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) noexcept {
        iferr_scope;

        if (static_cast<const void*>(this) == &queue) {
            return true;
        }

        const usize size0 = m_Size;
        const usize size1 = queue.m_Size;

        Reserve(size1) iferr_return false;

        const usize sizeBottom0Capacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom0 = Num::Min(sizeBottom0Capacity, size0);
        const usize sizeTop0 = size0 - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(queue.m_pMemoryEnd - queue.m_pQueueBegin);
        const usize sizeBottom1 = Num::Min(sizeBottom1Capacity, size1);
        const usize sizeTop1 = size1 - sizeBottom1;

        Memory::MoveFromFirstElementResult<T> resultBottom;
        Memory::MoveFromFirstElementResult<T> resultTop;

        const usize sizeBottomToMove = Num::Min(sizeBottom0, sizeBottom1);
        const usize sizeTopToMove = Num::Min(sizeBottom0 - sizeBottomToMove, sizeTop1);
        const usize sizeBottom0CapacityRemainder = sizeBottom0Capacity - sizeBottom0;
        const usize sizeBottomToConstruct = Num::Min(sizeBottom0CapacityRemainder, sizeBottom1 - sizeBottomToMove);
        const usize sizeTopToConstruct = Num::Min(sizeBottom0CapacityRemainder - sizeBottomToConstruct, sizeTop1 - sizeTopToMove);

        {
            resultBottom = Memory::MoveObjectsFromFirstElementNonOverlap(
                m_pQueueBegin, queue.m_pQueueBegin, queue.m_pQueueBegin + sizeBottomToMove
            );

            resultTop = Memory::MoveObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, queue.m_pMemoryBegin, queue.m_pMemoryBegin + sizeTopToMove
            );

            Memory::DestroyObjects(resultTop.pDstEnd, m_pQueueBegin + sizeBottom0);

            resultBottom = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
                resultTop.pDstEnd, resultBottom.pSrcEnd, resultBottom.pSrcEnd + sizeBottomToConstruct
            );

            resultTop = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, resultTop.pSrcEnd, resultTop.pSrcEnd + sizeTopToConstruct
            );
        }

        const usize sizeBottom1Remainder = sizeBottom1 - sizeBottomToMove - sizeBottomToConstruct;
        const usize sizeTop1Remainder = sizeTop1 - sizeTopToMove - sizeTopToConstruct;

        {
            const usize sizeBottomToMove = Num::Min(sizeTop0, sizeBottom1Remainder);
            resultBottom = Memory::MoveObjectsFromFirstElementNonOverlap(
                m_pMemoryBegin, resultBottom.pSrcEnd, resultBottom.pSrcEnd + sizeBottomToMove
            );

            const usize sizeTopToMove = Num::Min(sizeTop0 - sizeBottomToMove, sizeTop1Remainder);
            resultTop = Memory::MoveObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, resultTop.pSrcEnd, resultTop.pSrcEnd + sizeTopToMove
            );

            Memory::DestroyObjects(resultTop.pDstEnd, m_pMemoryBegin + sizeTop0);

            const usize sizeBottomToConstruct = sizeBottom1Remainder - sizeBottomToMove;
            resultBottom = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
                resultTop.pDstEnd, resultBottom.pSrcEnd, resultBottom.pSrcEnd + sizeBottomToConstruct
            );

            const usize sizeTopToConstruct = sizeTop1Remainder - sizeTopToMove;
            resultTop = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, resultTop.pSrcEnd, resultTop.pSrcEnd + sizeTopToConstruct
            );
        }

        m_Size = size1;

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&std::is_assignable_v<T&, InnerU&&> && std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool MoveFrom(U&& rhs) noexcept {
        iferr_scope;

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sliceSize = slice.GetSize();

        Reserve(sliceSize) iferr_return false;

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom = Num::Min(sizeBottomCapacity, size);

        const usize sizeBottomToMove = Num::Min(sliceSize, sizeBottom);
        Memory::MoveFromFirstElementResult<T> result = Memory::MoveObjectsFromFirstElementNonOverlap(
            m_pQueueBegin, slice.Data(), slice.Data() + sizeBottomToMove
        );

        Memory::DestroyObjects(result.pDstEnd, m_pQueueBegin + sizeBottom);

        const usize sizeBottomToConstruct = Num::Min(sliceSize, sizeBottomCapacity);
        result = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
            result.pDstEnd, result.pSrcEnd, slice.Data() + sizeBottomToConstruct
        );

        const usize sizeTop = size - sizeBottom;

        const usize sizeTopToMove = Num::Min(sliceSize - sizeBottomToConstruct, sizeTop);
        result = Memory::MoveObjectsFromFirstElementNonOverlap(
            m_pMemoryBegin, result.pSrcEnd, result.pSrcEnd + sizeTopToMove
        );

        Memory::DestroyObjects(result.pDstEnd, m_pMemoryBegin + sizeTop);

        Memory::MoveConstructObjectsFromFirstElementNonOverlap(
            result.pDstEnd, result.pDstEnd, slice.DataEnd()
        );

        m_Size = sliceSize;

        return true;
    }

    ALT_NODISCARD bool MoveFrom(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return MoveFrom(slice);
    }

    /**
     * @brief Append to back copy elements to the current queue end.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3) iferr_return;
     * q.AppendBackClone(q.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(q[0] == q[4]);
     * assert(q[1] == q[5]);
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendBackClone(Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& queue_) noexcept {

        Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue = queue_;
        return AppendBackClone(queue);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendBackClone(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) noexcept {
        iferr_scope;

        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, queue.m_pMemoryBegin, queue.m_pMemoryEnd));

        const usize size1 = queue.m_Size;
        if (size1 == 0) {
            return true;
        }

        ReserveAddition(size1) iferr_return false;

        const usize size0 = m_Size;
        const usize sizeBottom0Capacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom0 = Num::Min(size0, sizeBottom0Capacity);

        const usize sizeBottom1Capacity = static_cast<usize>(queue.m_pMemoryEnd - queue.m_pQueueBegin);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);

        const usize sizeBottom = Num::Min(sizeBottom1, sizeBottom0);
        T* pQueueBottomBegin = m_pQueueBegin + sizeBottom0;
        T* pQueueTopBegin = m_pMemoryBegin + size0 - sizeBottom0;

        {
            T* pBegin = queue.m_pQueueBegin;
            T* pEnd = queue.m_pQueueBegin + sizeBottom;

            Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                pQueueBottomBegin, pBegin, pEnd
            );

            m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueBottomBegin);
            pQueueBottomBegin = result.pDstObjectWhereCloneFailedOrEnd;

            if (!result.isOk) {
                return false;
            }

            result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                pQueueTopBegin, pEnd, queue.m_pQueueBegin + sizeBottom1
            );

            m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueTopBegin);
            pQueueTopBegin = result.pDstObjectWhereCloneFailedOrEnd;

            if (!result.isOk) {
                return false;
            }
        }

        const usize sizeTop1 = size1 - sizeBottom1;
        const usize sizeBottomRemainder = Num::Min(sizeTop1, sizeBottom0 - sizeBottom);

        {
            T* pBegin = queue.m_pMemoryBegin;
            T* pEnd = queue.m_pMemoryBegin + sizeBottomRemainder;

            Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                pQueueBottomBegin, pBegin, pEnd
            );

            m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueBottomBegin);
            pQueueBottomBegin = result.pDstObjectWhereCloneFailedOrEnd;

            if (!result.isOk) {
                return false;
            }

            result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                pQueueTopBegin, pEnd, pBegin + sizeTop1
            );

            m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueTopBegin);
            pQueueTopBegin = result.pDstObjectWhereCloneFailedOrEnd;

            if (!result.isOk) {
                return false;
            }
        }

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> && (TypeTraits::is_only_clonable_to_from_v<T, const InnerU&> || std::is_constructible_v<T, const InnerU&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendBackClone(U&& rhs) noexcept {
        iferr_scope;

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize sizeToAppend = slice.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        ReserveAddition(sizeToAppend) iferr_return false;

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);

        T* pQueueBottomBegin = m_pQueueBegin + sizeBottom;
        const usize sizeBottomAppend = Num::Min(sizeToAppend, sizeBottomCapacity - sizeBottom);
        Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pQueueBottomBegin, slice.Data(), slice.Data() + sizeBottomAppend
        );

        m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueBottomBegin);

        if (!result.isOk) {
            return false;
        }

        T* pQueueTopBegin = m_pMemoryBegin + m_Size - sizeBottom;
        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pQueueTopBegin, slice.Data() + sizeBottomAppend, slice.DataEnd()
        );

        m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueTopBegin);

        return result.isOk;
    }

    ALT_NODISCARD bool AppendBackClone(const T* pBegin, const usize size) noexcept {

        const Slice<const T> slice{ pBegin, size };
        return AppendBackClone(slice);
    }

    /**
     * @brief Append to front copy elements to the current queue end.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3) iferr_return;
     * q.AppendFrontClone(q.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(q[0] == q[4]);
     * assert(q[1] == q[5]);
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendFrontClone(Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& queue_) noexcept {

        Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue = queue_;
        return AppendFrontClone(queue);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendFrontClone(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) noexcept {
        iferr_scope;

        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, queue.m_pMemoryBegin, queue.m_pMemoryEnd));

        const usize size1 = queue.m_Size;
        if (size1 == 0) {
            return true;
        }

        ReserveAddition(size1) iferr_return false;

        const usize sizeTop0Capacity = static_cast<usize>(m_pQueueBegin - m_pMemoryBegin);

        const usize sizeBottom1Capacity = static_cast<usize>(queue.m_pMemoryEnd - queue.m_pQueueBegin);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);
        const usize sizeTop1 = size1 - sizeBottom1;

        const usize sizeTop = Num::Min(sizeTop0Capacity, sizeTop1);
        T* pQueueBottomBegin = m_pQueueBegin - sizeTop;
        T* pQueueBottomEnd = m_pMemoryEnd - sizeTop1 + sizeTop;

        {
            T* pEnd = queue.m_pMemoryBegin + sizeTop1;
            T* pBegin = pEnd - sizeTop;

            Memory::CloneFromLastElementResult<T> result = Memory::CloneConstructObjectsFromLastElementNonOverlap(
                pQueueBottomBegin, pBegin, pEnd
            );

            result.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
            m_Size += static_cast<usize>(pEnd - result.pSrcObjectWhereCloneFailedOrBeginMinusOne);

            T* pQueueBeginTopNew = result.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

            if (!result.isOk) {

                m_pQueueBegin = pQueueBeginTopNew;
                return false;
            }

            result = Memory::CloneConstructObjectsFromLastElementNonOverlap(
                pQueueBottomEnd, queue.m_pMemoryBegin, pBegin
            );

            result.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
            m_Size += static_cast<usize>(pBegin - result.pSrcObjectWhereCloneFailedOrBeginMinusOne);

            T* pQueueBeginBottomNew = result.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

            if (!result.isOk) {

                m_pQueueBegin = pQueueBeginBottomNew == m_pMemoryEnd
                    ? pQueueBeginTopNew
                    : pQueueBeginBottomNew;

                return false;
            }
        }

        const usize sizeTopRemainder = Num::Min(sizeBottom1, sizeTop0Capacity - sizeTop);
        pQueueBottomBegin -= sizeTopRemainder;
        pQueueBottomEnd -= sizeBottom1 - sizeTopRemainder;

        {
            T* pEnd = queue.m_pQueueBegin + sizeBottom1;
            T* pBegin = pEnd - sizeTopRemainder;

            Memory::CloneFromLastElementResult<T> result = Memory::CloneConstructObjectsFromLastElementNonOverlap(
                pQueueBottomBegin, pBegin, pEnd
            );

            result.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
            m_Size += static_cast<usize>(pEnd - result.pSrcObjectWhereCloneFailedOrBeginMinusOne);

            T* pQueueBeginTopNew = result.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

            if (!result.isOk) {

                m_pQueueBegin = pQueueBeginTopNew;
                return false;
            }

            result = Memory::CloneConstructObjectsFromLastElementNonOverlap(
                pQueueBottomEnd, queue.m_pQueueBegin, pBegin
            );

            result.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
            m_Size += static_cast<usize>(pBegin - result.pSrcObjectWhereCloneFailedOrBeginMinusOne);

            T* pQueueBeginBottomNew = result.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

            if (!result.isOk) {

                m_pQueueBegin = pQueueBeginBottomNew == m_pMemoryEnd
                    ? pQueueBeginTopNew
                    : pQueueBeginBottomNew;

                return false;
            }
        }

        m_pQueueBegin = pQueueBottomEnd == m_pMemoryEnd
            ? pQueueBottomBegin
            : pQueueBottomEnd;

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> && (TypeTraits::is_only_clonable_to_from_v<T, const InnerU&> || std::is_constructible_v<T, const InnerU&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendFrontClone(U&& rhs) noexcept {
        iferr_scope;

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize sizeToAppend = slice.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        ReserveAddition(sizeToAppend) iferr_return false;

        const usize sizeTopCapacity = static_cast<usize>(m_pQueueBegin - m_pMemoryBegin);
        const usize size = Num::Min(sizeToAppend, sizeTopCapacity);

        T* pQueueBeginTop = m_pQueueBegin - size;
        const Memory::CloneFromLastElementResult<T> resultTop = Memory::CloneConstructObjectsFromLastElementNonOverlap(
            pQueueBeginTop, slice.DataEnd() - size, slice.DataEnd()
        );

        resultTop.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
        m_Size += static_cast<usize>(slice.DataEnd() - resultTop.pSrcObjectWhereCloneFailedOrBeginMinusOne);

        T* pQueueBeginTopNew = resultTop.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

        if (!resultTop.isOk) {

            m_pQueueBegin = pQueueBeginTopNew;
            return false;
        }

        T* pQueueBeginBottom = m_pMemoryEnd - sizeToAppend + size;
        const Memory::CloneFromLastElementResult<T> resultBottom = Memory::CloneConstructObjectsFromLastElementNonOverlap(
            pQueueBeginBottom, slice.Data(), slice.DataEnd() - size
        );

        resultBottom.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
        m_Size += static_cast<usize>(slice.DataEnd() - size - resultBottom.pSrcObjectWhereCloneFailedOrBeginMinusOne);

        T* pQueueBeginBottomNew = resultBottom.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

        m_pQueueBegin = pQueueBeginBottomNew == m_pMemoryEnd
            ? pQueueBeginTopNew
            : pQueueBeginBottomNew;

        return resultBottom.isOk;
    }

    ALT_NODISCARD bool AppendFrontClone(const T* pBegin, const usize size) noexcept {

        const Slice<const T> slice{ pBegin, size };
        return AppendFrontClone(slice);
    }

    /**
     * @brief Append to back move elements to the current queue end.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<Alt::String> q = Alt::Queue<Alt::String>::AllocatePush("0"_salloc, "1"_salloc, "2"_salloc, "3"_salloc) iferr_return;
     * q.AppendBackMove(q.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(q[0] == "0");
     * assert(q[4] == "0");
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendBackMove(Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& queue_) noexcept {

        Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue = queue_;
        return AppendBackMove(queue);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendBackMove(Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) noexcept {
        iferr_scope;

        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, queue.m_pMemoryBegin, queue.m_pMemoryEnd));

        const usize size1 = queue.m_Size;
        if (size1 == 0) {
            return true;
        }

        ReserveAddition(size1) iferr_return false;

        const usize size0 = m_Size;
        const usize sizeBottom0Capacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom0 = Num::Min(size0, sizeBottom0Capacity);

        const usize sizeBottom1Capacity = static_cast<usize>(queue.m_pMemoryEnd - queue.m_pQueueBegin);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);

        const usize sizeBottom = Num::Min(sizeBottom1, sizeBottom0);
        T* pQueueBottomBegin = m_pQueueBegin + sizeBottom0;
        T* pQueueTopBegin = m_pMemoryBegin + size0 - sizeBottom0;

        {
            T* pBegin = queue.m_pQueueBegin;
            T* pEnd = queue.m_pQueueBegin + sizeBottom;

            Memory::MoveFromFirstElementResult<T> result = Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, pBegin, pEnd);
            pQueueBottomBegin = result.pDstEnd;

            result = Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueTopBegin, pEnd, queue.m_pQueueBegin + sizeBottom1);
            pQueueTopBegin = result.pDstEnd;
        }

        const usize sizeTop1 = size1 - sizeBottom1;
        const usize sizeBottomRemainder = Num::Min(sizeTop1, sizeBottom0 - sizeBottom);

        {
            T* pBegin = queue.m_pMemoryBegin;
            T* pEnd = queue.m_pMemoryBegin + sizeBottomRemainder;

            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, pBegin, pEnd);
            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueTopBegin, pEnd, pBegin + sizeTop1);
        }

        m_Size += size1;

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<InnerU>, U&&> &&
            std::is_same_v<InnerU, T> && std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool AppendBackMove(U&& rhs) noexcept {
        iferr_scope;

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize sizeToAppend = slice.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        ReserveAddition(sizeToAppend) iferr_return false;

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);

        T* pQueueBottomBegin = m_pQueueBegin + sizeBottom;
        const usize sizeBottomAppend = Num::Min(sizeToAppend, sizeBottomCapacity - sizeBottom);
        Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, slice.Data(), slice.Data() + sizeBottomAppend);

        T* pQueueTopBegin = m_pMemoryBegin + m_Size - sizeBottom;
        Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueTopBegin, slice.Data() + sizeBottomAppend, slice.DataEnd());

        m_Size += sizeToAppend;

        return true;
    }

    ALT_NODISCARD bool AppendBackMove(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return AppendBackMove(slice);
    }

    /**
     * @brief Append to front move elements to the current queue end.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<Alt::String> q = Alt::Queue<Alt::String>::AllocatePush("0"_salloc, "1"_salloc, "2"_salloc, "3"_salloc) iferr_return;
     * q.AppendFrontMove(q.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(q[0] == "0");
     * assert(q[4] == "0");
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendFrontMove(Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& queue_) noexcept {

        Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue = queue_;
        return AppendFrontMove(queue);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendFrontMove(Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) noexcept {
        iferr_scope;

        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, queue.m_pMemoryBegin, queue.m_pMemoryEnd));

        const usize size1 = queue.m_Size;
        if (size1 == 0) {
            return true;
        }

        ReserveAddition(size1) iferr_return false;

        const usize sizeTop0Capacity = static_cast<usize>(m_pQueueBegin - m_pMemoryBegin);

        const usize sizeBottom1Capacity = static_cast<usize>(queue.m_pMemoryEnd - queue.m_pQueueBegin);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);
        const usize sizeTop1 = size1 - sizeBottom1;

        const usize sizeTop = Num::Min(sizeTop0Capacity, sizeTop1);
        T* pQueueBottomBegin = m_pQueueBegin - sizeTop;
        T* pQueueBottomEnd = m_pMemoryEnd - sizeTop1 + sizeTop;

        {
            T* pEnd = queue.m_pMemoryBegin + sizeTop1;
            T* pBegin = pEnd - sizeTop;

            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, pBegin, pEnd);
            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomEnd, queue.m_pMemoryBegin, pBegin);
        }

        const usize sizeTopRemainder = Num::Min(sizeBottom1, sizeTop0Capacity - sizeTop);
        pQueueBottomBegin -= sizeTopRemainder;
        pQueueBottomEnd -= sizeBottom1 - sizeTopRemainder;

        {
            T* pEnd = queue.m_pQueueBegin + sizeBottom1;
            T* pBegin = pEnd - sizeTopRemainder;

            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, pBegin, pEnd);
            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomEnd, queue.m_pQueueBegin, pBegin);
        }

        m_pQueueBegin = pQueueBottomEnd == m_pMemoryEnd
            ? pQueueBottomBegin
            : pQueueBottomEnd;

        m_Size += size1;

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<InnerU>, U&&> &&
            std::is_same_v<InnerU, T> && std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool AppendFrontMove(U&& rhs) noexcept {
        iferr_scope;

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize sizeToAppend = slice.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        ReserveAddition(sizeToAppend) iferr_return false;

        const usize sizeTopCapacity = static_cast<usize>(m_pQueueBegin - m_pMemoryBegin);

        const usize size = Num::Min(sizeToAppend, sizeTopCapacity);

        T* pQueueBeginTop = m_pQueueBegin - size;
        Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBeginTop, slice.DataEnd() - size, slice.DataEnd());

        T* pQueueBeginBottom = m_pMemoryEnd - sizeToAppend + size;
        Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBeginBottom, slice.Data(), slice.DataEnd() - size);

        m_pQueueBegin = sizeToAppend <= sizeTopCapacity
            ? pQueueBeginTop
            : pQueueBeginBottom;

        m_Size += sizeToAppend;

        return true;
    }

    ALT_NODISCARD bool AppendFrontMove(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return AppendFrontMove(slice);
    }

    /**
     * @brief Appends an element to the back of the queue.
     * @param val The value to push.
     * @return True if the element was successfully added, false if memory allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q;
     * q.PushBack(0) iferr_return;
     * 
     * assert(q[0] == 0);
     */
    template <typename U = T,
        std::enable_if_t<is_pushable_v<U&&>>* = nullptr>
    ALT_FORCE_INLINE bool PushBack(U&& val) noexcept {
        iferr_scope;

        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, &val, &val + 1));

        ReserveAddition(1) iferr_return false;

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        T* pEnd = m_Size < sizeBottomCapacity ? m_pQueueBegin + m_Size : m_pMemoryBegin + m_Size - sizeBottomCapacity;

        T* pResult = Memory::CloneConstructOrMoveConstructObject(pEnd, std::forward<U>(val));

        const bool isOk = pResult != pEnd;
        m_Size += isOk ? 1 : 0;

        return isOk;
    }

    /**
     * @brief Variadic append an elements to the end of the queue.
     * @param ...vals The values to push.
     * @return True if the element was successfully added, false if memory allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q;
     * q.PushBack(0, 1, 2, 3) iferr_return;
     * 
     * assert(q[0] == 0);
     * assert(q.GetSize() == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    bool PushBack(ArgsT&&... vals) noexcept {
        iferr_scope;

        constexpr usize sizeToPush = sizeof...(ArgsT);
        ReserveAddition(sizeToPush) iferr_return false;

        PushBackVariadicImpl(*this, std::forward<ArgsT>(vals)...) iferr_return false;

        return true;
    }

    /**
     * @brief Appends an element to the front of the queue.
     * @param val The value to push.
     * @return True if the element was successfully added, false if memory allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q;
     * q.PushFront(0) iferr_return;
     * q.PushFront(1) iferr_return;
     * 
     * assert(q[0] == 1);
     * assert(q[1] == 0);
     */
    template <typename U = T,
        std::enable_if_t<is_pushable_v<U&&>>* = nullptr>
    ALT_FORCE_INLINE bool PushFront(U&& val) noexcept {
        iferr_scope;

        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, &val, &val + 1));

        ReserveAddition(1) iferr_return false;

        const bool isNeedRecycle = m_pQueueBegin == m_pMemoryBegin;
        T* pBeginNew = isNeedRecycle ? m_pMemoryEnd : m_pQueueBegin;
        pBeginNew -= 1;

        T* pResult = Memory::CloneConstructOrMoveConstructObject(pBeginNew, std::forward<U>(val));

        const bool isOk = pResult != pBeginNew;
        m_pQueueBegin = isOk ? pBeginNew : m_pQueueBegin;
        m_Size += isOk ? 1 : 0;

        return isOk;
    }

    /**
     * @brief Variadic append an elements to the front of the queue.
     * @param ...vals The values to push.
     * @return True if the element was successfully added, false if memory allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q;
     * q.PushFront(0, 1) iferr_return;
     * 
     * assert(q[0] == 1);
     * assert(q[1] == 0);
     * assert(q.GetSize() == 2);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    bool PushFront(ArgsT&&... vals) noexcept {
        iferr_scope;

        constexpr usize sizeToPush = sizeof...(ArgsT);
        ReserveAddition(sizeToPush) iferr_return false;

        PushFrontVariadicImpl(*this, std::forward<ArgsT>(vals)...) iferr_return false;

        return true;
    }

    /**
     * @brief Removes and returns the back element of the queue.
     * @return The element that was removed.
     * @note This method requires the queue to not be empty. Calling on an empty queue triggers an assertion.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 2, 4) iferr_return;
     * usize val = q.Pop();
     * assert(val == 4);
     */
    ALT_FORCE_INLINE T PopBack() noexcept {
        ALT_ASSERT(!IsEmpty());

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        T* pEnd = m_Size <= sizeBottomCapacity ? m_pQueueBegin + m_Size : m_pMemoryBegin + m_Size - sizeBottomCapacity;
        pEnd -= 1;

        m_Size -= 1;

        return Memory::MoveDestructibleObject(*pEnd);
    }

    /**
     * @brief Removes and returns the front element of the queue.
     * @return The element that was removed.
     * @note This method requires the queue to not be empty. Calling on an empty queue triggers an assertion.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 2, 4) iferr_return;
     * usize val = q.PopFront();
     * assert(val == 0);
     */
    ALT_FORCE_INLINE T PopFront() noexcept {
        ALT_ASSERT(!IsEmpty());

        T result = Memory::MoveDestructibleObject(*m_pQueueBegin);

        m_pQueueBegin += 1;
        m_pQueueBegin = m_pQueueBegin == m_pMemoryEnd ? m_pMemoryBegin : m_pQueueBegin;

        m_Size -= 1;

        return result;
    }

    /**
     * @brief Allocate, or fill or destroy elements in the queue starting from back.
     * @tparam U The type of the value used for padding. Must be constructible to T.
     * @param size The new size of the queue.
     * @param val (Optional) The value to use for initializing new elements.
     * @return True if the resize was successful, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 2, 4) iferr_return;
     *
     * q.ResizeBack(0) iferr_return;
     * assert(q.IsEmpty());
     * 
     * q.ResizeBack(1, 4);
     * assert(q[0] == 4);
     *
     * q.ResizeBack(2, 8);
     * assert(q[0] == 4);
     * assert(q[1] == 8);
     */
    template <typename U = T, typename ...ArgsT,
        std::enable_if_t<
            is_populated_v<ArgsT...> &&
            std::is_default_constructible_v<U> &&
            std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>
        >* = nullptr>
    bool ResizeBackAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return ResizeBack(size);
    }

    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD bool ResizeBack(const usize size) noexcept {
        iferr_scope;

        if (size < m_Size) {

            const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

            const usize sizeBottom0 = Num::Min(m_Size, sizeBottomCapacity);
            const usize sizeBottom1 = Num::Min(size, sizeBottomCapacity);

            T* pQueueBottomBegin = m_pQueueBegin + sizeBottom1;
            T* pQueueBottomEnd = m_pQueueBegin + sizeBottom0;
            Memory::DestroyObjects(pQueueBottomBegin, pQueueBottomEnd);

            T* pQueueTopBegin = m_pMemoryBegin + size - sizeBottom1;
            T* pQueueTopEnd = m_pMemoryBegin + m_Size - sizeBottom0;
            Memory::DestroyObjects(pQueueTopBegin, pQueueTopEnd);
        }
        else if (size > m_Size) {

            Reserve(size) iferr_return false;

            const usize sizeOld = m_Size;
            const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

            const usize sizeBottom0 = Num::Min(sizeOld, sizeBottomCapacity);
            const usize sizeBottom1 = Num::Min(size, sizeBottomCapacity);

            T* pQueueBottomBegin = m_pQueueBegin + sizeBottom0;
            T* pQueueBottomEnd = m_pQueueBegin + sizeBottom1;
            Memory::FillConstructDefaultObjects(pQueueBottomBegin, pQueueBottomEnd);

            T* pQueueTopBegin = m_pMemoryBegin + sizeOld - sizeBottom0;
            T* pQueueTopEnd = m_pMemoryBegin + size - sizeBottom1;
            Memory::FillConstructDefaultObjects(pQueueTopBegin, pQueueTopEnd);
        }

        m_Size = size;

        return true;
    }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD bool ResizeBack(const usize size, const U& val) noexcept {
        iferr_scope;

        if (size < m_Size) {

            const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

            const usize sizeBottom0 = Num::Min(m_Size, sizeBottomCapacity);
            const usize sizeBottom1 = Num::Min(size, sizeBottomCapacity);

            T* pQueueBottomBegin = m_pQueueBegin + sizeBottom1;
            T* pQueueBottomEnd = m_pQueueBegin + sizeBottom0;
            Memory::DestroyObjects(pQueueBottomBegin, pQueueBottomEnd);

            T* pQueueTopBegin = m_pMemoryBegin + size - sizeBottom1;
            T* pQueueTopEnd = m_pMemoryBegin + m_Size - sizeBottom0;
            Memory::DestroyObjects(pQueueTopBegin, pQueueTopEnd);

            m_Size = size;
        }
        else if (size > m_Size) {

            Reserve(size) iferr_return false;

            const usize sizeOld = m_Size;
            const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

            const usize sizeBottom0 = Num::Min(sizeOld, sizeBottomCapacity);
            const usize sizeBottom1 = Num::Min(size, sizeBottomCapacity);

            T* pQueueBottomBegin = m_pQueueBegin + sizeBottom0;
            T* pQueueBottomEnd = m_pQueueBegin + sizeBottom1;
            Memory::CloneFromFirstElementResult<T> result = Memory::FillCloneConstructObjectsFromFirstElement(
                pQueueBottomBegin, pQueueBottomEnd, val
            );

            m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueBottomBegin);

            if (!result.isOk) {
                return false;
            }

            T* pQueueTopBegin = m_pMemoryBegin + sizeOld - sizeBottom0;
            T* pQueueTopEnd = m_pMemoryBegin + size - sizeBottom1;
            result = Memory::FillCloneConstructObjectsFromFirstElement(pQueueTopBegin, pQueueTopEnd, val);

            m_Size += static_cast<usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueTopBegin);

            return result.isOk;
        }

        return true;
    }

    /**
     * @brief Allocate, or fill or destroy elements in the queue starting from front.
     * @tparam U The type of the value used for padding. Must be constructible to T.
     * @param size The new size of the queue.
     * @param val (Optional) The value to use for initializing new elements.
     * @return True if the resize was successful, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 2, 4) iferr_return;
     *
     * q.ResizeFront(0) iferr_return;
     * assert(q.IsEmpty());
     * 
     * q.ResizeFront(1, 4);
     * assert(q[0] == 4);
     *
     * q.ResizeFront(2, 8);
     * assert(q[0] == 8);
     * assert(q[1] == 4);
     */
    template <typename U = T, typename ...ArgsT,
        std::enable_if_t<
            is_populated_v<ArgsT...> &&
            std::is_default_constructible_v<U> &&
            std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>
        >* = nullptr>
    bool ResizeFrontAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return ResizeFront(size);
    }

    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD bool ResizeFront(const usize size) noexcept {
        iferr_scope;

        if (size < m_Size) {

            const usize sizeToDestroy = m_Size - size;

            const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
            const usize sizeBottom = Num::Min(sizeToDestroy, sizeBottomCapacity);

            T* pQueueBeginBottom = m_pQueueBegin + sizeBottom;
            Memory::DestroyObjects(m_pQueueBegin, pQueueBeginBottom);

            T* pQueueBeginTop = m_pMemoryBegin + sizeToDestroy - sizeBottom;
            Memory::DestroyObjects(m_pMemoryBegin, pQueueBeginTop);

            m_pQueueBegin = sizeToDestroy < sizeBottomCapacity
                ? pQueueBeginBottom
                : pQueueBeginTop;
        }
        else if (size > m_Size) {

            Reserve(size) iferr_return false;

            const usize sizeToConstruct = size - m_Size;

            const usize sizeTopCapacity = static_cast<usize>(m_pQueueBegin - m_pMemoryBegin);
            const usize sizeTop = Num::Min(sizeToConstruct, sizeTopCapacity);

            T* pQueueBeginTop = m_pQueueBegin - sizeTop;
            Memory::FillConstructDefaultObjects(pQueueBeginTop, m_pQueueBegin);

            T* pQueueBeginBottom = m_pMemoryEnd - sizeToConstruct + sizeTop;
            Memory::FillConstructDefaultObjects(pQueueBeginBottom, m_pMemoryEnd);

            m_pQueueBegin = pQueueBeginBottom == m_pMemoryEnd
                ? pQueueBeginTop
                : pQueueBeginBottom;
        }

        m_Size = size;

        return true;
    }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD bool ResizeFront(const usize size, const U& val) noexcept {
        iferr_scope;

        if (size < m_Size) {

            const usize sizeToDestroy = m_Size - size;

            const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
            const usize sizeBottom = Num::Min(sizeToDestroy, sizeBottomCapacity);

            T* pQueueBeginBottom = m_pQueueBegin + sizeBottom;
            Memory::DestroyObjects(m_pQueueBegin, pQueueBeginBottom);

            T* pQueueBeginTop = m_pMemoryBegin + sizeToDestroy - sizeBottom;
            Memory::DestroyObjects(m_pMemoryBegin, pQueueBeginTop);

            m_pQueueBegin = sizeToDestroy < sizeBottomCapacity
                ? pQueueBeginBottom
                : pQueueBeginTop;

            m_Size = size;
        }
        else if (size > m_Size) {

            Reserve(size) iferr_return false;

            const usize sizeToConstruct = size - m_Size;

            const usize sizeTopCapacity = static_cast<usize>(m_pQueueBegin - m_pMemoryBegin);
            const usize sizeTop = Num::Min(sizeToConstruct, sizeTopCapacity);

            T* pQueueBeginTop = m_pQueueBegin - sizeTop;
            Memory::CloneFromLastElementResult<T> result = Memory::FillCloneConstructObjectsFromLastElement(
                pQueueBeginTop, m_pQueueBegin, val
            );

            result.pDstObjectWhereCloneFailedOrBeginMinusOne += 1;
            pQueueBeginTop = result.pDstObjectWhereCloneFailedOrBeginMinusOne;
            m_Size += static_cast<usize>(m_pQueueBegin - pQueueBeginTop);

            if (!result.isOk) {

                m_pQueueBegin = pQueueBeginTop;
                return false;
            }

            T* pQueueBeginBottom = m_pMemoryEnd - sizeToConstruct + sizeTop;
            result = Memory::FillCloneConstructObjectsFromLastElement(pQueueBeginBottom, m_pMemoryEnd, val);

            result.pDstObjectWhereCloneFailedOrBeginMinusOne += 1;
            pQueueBeginBottom = result.pDstObjectWhereCloneFailedOrBeginMinusOne;
            m_Size += static_cast<usize>(m_pMemoryEnd - pQueueBeginBottom);

            m_pQueueBegin = pQueueBeginBottom == m_pMemoryEnd
                ? pQueueBeginTop
                : pQueueBeginBottom;

            return result.isOk;
        }

        return true;
    }

    /**
     * @brief Reserves at least the specified capacity in the queue.
     * @param size The capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q;
     * 
     * const usize capacityOld = q.GetCapacity();
     * q.Reserve(1'024);
     * const usize capacity = q.GetCapacity();
     * 
     * assert(capacityOld < capacity);
     */
    bool Reserve(const usize size) noexcept {

        if (size <= GetCapacity()) {
            return true;
        }

        return ReallocateToNextGrow(size);
    }

    /**
     * @brief Additionally reserves at least the 'GetSize() + additional size' capacity in the queue.
     * @param size The additional capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed or overflow.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q;
     * 
     * const usize capacityOld = q.GetCapacity();
     * q.ReserveAddition(1);
     * const usize capacity = q.GetCapacity();
     * 
     * assert(capacityOld < capacity);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>>* = nullptr>
    bool ReserveAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return Reserve(size);
    }

    /**
     * @brief Reserves the exact capacity in the queue.
     * @param size The capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize, 0> q;
     * 
     * q.ReserveExact(9);
     * assert(q.GetCapacity() == 9);
     */
    bool ReserveExact(const usize size) noexcept {

        if (size <= GetCapacity()) {
            return true;
        }

        return Reallocate(size);
    }

    /**
     * @brief Additionaly reserves the exact 'GetSize() + additional size' capacity in the queue.
     * @param size The additional capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize, 0> q;
     * 
     * q.ReserveExact(9);
     * q.ReserveExactAddition(7);
     * assert(q.GetCapacity() == 16);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>>* = nullptr>
    bool ReserveExactAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return ReserveExact(size);
    }

    /**
     * @brief Reduces the capacity of the queue to fit the current size.
     * @param capacityNew The new capacity.
     * @return True if successful, false if allocation failed or we can't
     * reallocate without massive moving.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize, 0> q = Alt::Queue<usize>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * 
     * const usize capacityOld = q.GetCapacity();
     * q.ShrinkToFit();
     * const usize capacity = q.GetCapacity();
     * 
     * assert(capacityOld > capacity);
     */
    bool ShrinkToFit(const usize capacityNew = MIN_ELEMENT_CAPACITY_ON_ALLOCATE) noexcept {
        iferr_scope;

        if (GetCapacity() <= capacityNew) {

            return true;
        }

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize size = GetSize();

        if (size > sizeBottomCapacity) {

            return false;
        }

        usize offset = static_cast<usize>(m_pQueueBegin - m_pMemoryBegin);
        usize capacity = Num::Max(offset + size, capacityNew);

        T* pMemory = nullptr;
        if (capacity == 0) {

            Memory::ChooseAlignedFree<T>(m_pMemoryBegin);
        }
        else {

            const Memory::ReallocBits bits = size == GetCapacity()
                ? Memory::ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace
                : Memory::ReallocBits::None;

            pMemory = Memory::ChooseAlignedRealloc<T>(
                m_pMemoryBegin,
                size,
                capacity,
                Memory::ReallocBits::InPlace | bits
            ) iferr {

                capacity = Num::Max(size, capacityNew);

                pMemory = Memory::ChooseAlignedMalloc<T>(capacity) iferr_return false;
                Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(pMemory, m_pQueueBegin, m_pQueueBegin + size);
                Memory::ChooseAlignedFree<T>(m_pMemoryBegin);

                offset = 0;
            }
        }

        m_pMemoryBegin = pMemory;
        m_pMemoryEnd = pMemory + capacity;
        m_pQueueBegin = pMemory + offset;

        return true;
    }

    /**
     * @brief Removes the element at index `i` by swapping it with the last element.
     * @param i The index of the element to remove.
     * @return The element that was removed.
     * @note This is an $O(1)$ operation but does not preserve the order of elements.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * const usize first = q.RemoveSwapBack(0);
     * 
     * assert(first == 0);
     * assert(q.GetFront() == 16);
     */
    ALT_FORCE_INLINE T RemoveSwapLast(const usize i) noexcept {

        return RemoveSwapBack(i);
    }

    ALT_FORCE_INLINE T RemoveSwapBack(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

        T* pCurrent = i < sizeBottomCapacity ? m_pQueueBegin + i : m_pMemoryBegin + i - sizeBottomCapacity;
        T* pEnd = m_Size <= sizeBottomCapacity ? m_pQueueBegin + m_Size : m_pMemoryBegin + m_Size - sizeBottomCapacity;
        pEnd -= 1;

        if (pCurrent != pEnd) {

            Swap(*pCurrent, *pEnd);
        }

        m_Size -= 1;

        return Memory::MoveDestructibleObject(*pEnd);
    }

    /**
     * @brief Removes the element at index `i` by swapping it with the first element.
     * @param i The index of the element to remove.
     * @return The element that was removed.
     * @note This is an $O(1)$ operation but does not preserve the order of elements.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * const usize first = q.RemoveSwapFront(4);
     * 
     * assert(first == 16);
     * assert(q.GetBack() == 0);
     */
    ALT_FORCE_INLINE T RemoveSwapFirst(const usize i) noexcept {

        return RemoveSwapFront(i);
    }

    ALT_FORCE_INLINE T RemoveSwapFront(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

        T* pCurrent = i < sizeBottomCapacity ? m_pQueueBegin + i : m_pMemoryBegin + i - sizeBottomCapacity;
        T* pBegin = m_pQueueBegin;

        if (pCurrent != pBegin) {

            Swap(*pCurrent, *pBegin);
        }

        T result = Memory::MoveDestructibleObject(*pBegin);

        m_pQueueBegin += 1;
        m_pQueueBegin = m_pQueueBegin == m_pMemoryEnd ? m_pMemoryBegin : m_pQueueBegin;

        m_Size -= 1;

        return result;
    }

    /**
     * @brief Removes the element at index `i` and shifts subsequent elements.
     * @param i The index of the element to remove.
     * @return The element that was removed.
     * @note This is an $O(n)$ operation.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Queue<usize> q = Alt::Queue<usize>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * const usize first = q.Remove(0);
     * 
     * assert(first == 0);
     * assert(q[0] == 2);
     */
    T Remove(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

        const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);
        const usize sizeTop = m_Size - sizeBottom;

        m_Size -= 1;

        const bool isBottom = i < sizeBottomCapacity;
        if (isBottom) {

            T result = Algorithms::RemoveShiftRight(m_pQueueBegin, m_pQueueBegin + sizeBottom, m_pQueueBegin + i);

            m_pQueueBegin += 1;
            m_pQueueBegin = m_pQueueBegin == m_pMemoryEnd ? m_pMemoryBegin : m_pQueueBegin;

            return result;
        }

        return Algorithms::Remove(m_pMemoryBegin, m_pMemoryBegin + sizeTop, m_pMemoryBegin + i - sizeBottom);
    }

    /**
     * @brief Operators for comparison with Queue or Slice.
     * @example
     *
     * iferr_scope;
     * 
     * Alt::Queue<usize> q0 = Alt::Queue<usize>::AllocatePush(0, 1) iferr_return;
     * Alt::Queue<usize> q1 = q0.Clone() iferr_return;
     * 
     * assert(q0 == q1);
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator==(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;

        if (m_Size != rhs.GetSize()) {
            return false;
        }

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);

        const bool isEQ = Algorithms::CompareEQ(m_pQueueBegin, m_pQueueBegin + sizeBottom, rhs.Data(), rhs.Data() + sizeBottom);
        if (!isEQ) {
            return false;
        }

        return Algorithms::CompareEQ(m_pMemoryBegin, m_pMemoryBegin + m_Size - sizeBottom, rhs.Data() + sizeBottom, rhs.DataEnd());
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator==(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) const noexcept {

        if (m_Size != queue.m_Size) {
            return false;
        }

        const usize sizeBottom0Capacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom0 = Num::Min(m_Size, sizeBottom0Capacity);
        const usize sizeTop0 = m_Size - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(queue.m_pMemoryEnd - queue.m_pQueueBegin);
        const usize sizeBottom1 = Num::Min(queue.m_Size, sizeBottom1Capacity);
        const usize sizeTop1 = queue.m_Size - sizeBottom1;

        const usize sizeBottom = Num::Min(sizeBottom0, sizeBottom1);

        const T* pQueueBottom0Begin = m_pQueueBegin + sizeBottom;
        const T* pQueueBottom1Begin = queue.m_pQueueBegin + sizeBottom;

        bool isEQ = Algorithms::CompareEQ(m_pQueueBegin, pQueueBottom0Begin, queue.m_pQueueBegin, pQueueBottom1Begin);
        if (!isEQ) {
            return false;
        }

        usize sizeBottomRemainder = Num::Min(sizeBottom0 - sizeBottom, sizeTop1);
        const T* pQueueTop1Begin = queue.m_pMemoryBegin + sizeBottomRemainder;

        isEQ = Algorithms::CompareEQ(pQueueBottom0Begin, pQueueBottom0Begin + sizeBottomRemainder, queue.m_pMemoryBegin, pQueueTop1Begin);
        if (!isEQ) {
            return false;
        }

        sizeBottomRemainder = Num::Min(sizeBottom1 - sizeBottom, sizeTop0);
        const T* pQueueTop0Begin = m_pMemoryBegin + sizeBottomRemainder;

        isEQ = Algorithms::CompareEQ(m_pMemoryBegin, pQueueTop0Begin, pQueueBottom1Begin, pQueueBottom1Begin + sizeBottomRemainder);
        if (!isEQ) {
            return false;
        }

        return Algorithms::CompareEQ(pQueueTop0Begin, m_pMemoryBegin + sizeTop0, pQueueTop1Begin, queue.m_pMemoryBegin + sizeTop1);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator!=(U&& rhs) const noexcept {

        return !(*this == rhs);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator!=(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) const noexcept {

        return !(*this == queue);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator<(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;

        if (m_Size != rhs.GetSize()) {
            return m_Size < rhs.GetSize();
        }

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);

        const ssize cmp = Algorithms::Compare(m_pQueueBegin, m_pQueueBegin + sizeBottom, rhs.Data(), rhs.Data() + sizeBottom);

        switch (cmp) {
        case -1: {
            return true;
        }
        case 0: {
            break;
        }
        case 1: {
            return false;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        return Algorithms::CompareLT(m_pMemoryBegin, m_pMemoryBegin + m_Size - sizeBottom, rhs.Data() + sizeBottom, rhs.DataEnd());
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator<(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) const noexcept {

        if (m_Size != queue.m_Size) {
            return m_Size < queue.m_Size;
        }

        const usize sizeBottom0Capacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom0 = Num::Min(m_Size, sizeBottom0Capacity);
        const usize sizeTop0 = m_Size - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(queue.m_pMemoryEnd - queue.m_pQueueBegin);
        const usize sizeBottom1 = Num::Min(queue.m_Size, sizeBottom1Capacity);
        const usize sizeTop1 = queue.m_Size - sizeBottom1;

        const usize sizeBottom = Num::Min(sizeBottom0, sizeBottom1);

        const T* pQueueBottom0Begin = m_pQueueBegin + sizeBottom;
        const T* pQueueBottom1Begin = queue.m_pQueueBegin + sizeBottom;
        ssize cmp = Algorithms::Compare(m_pQueueBegin, pQueueBottom0Begin, queue.m_pQueueBegin, pQueueBottom1Begin);

        switch (cmp) {
        case -1: {
            return true;
        }
        case 0: {
            break;
        }
        case 1: {
            return false;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        usize sizeBottomRemainder = Num::Min(sizeBottom0 - sizeBottom, sizeTop1);
        const T* pQueueTop1Begin = queue.m_pMemoryBegin + sizeBottomRemainder;

        cmp = Algorithms::Compare(pQueueBottom0Begin, pQueueBottom0Begin + sizeBottomRemainder, queue.m_pMemoryBegin, pQueueTop1Begin);

        switch (cmp) {
        case -1: {
            return true;
        }
        case 0: {
            break;
        }
        case 1: {
            return false;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        sizeBottomRemainder = Num::Min(sizeBottom1 - sizeBottom, sizeTop0);
        const T* pQueueTop0Begin = m_pMemoryBegin + sizeBottomRemainder;

        cmp = Algorithms::Compare(m_pMemoryBegin, pQueueTop0Begin, pQueueBottom1Begin, pQueueBottom1Begin + sizeBottomRemainder);

        switch (cmp) {
        case -1: {
            return true;
        }
        case 0: {
            break;
        }
        case 1: {
            return false;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        return Algorithms::CompareLT(pQueueTop0Begin, m_pMemoryBegin + sizeTop0, pQueueTop1Begin, queue.m_pMemoryBegin + sizeTop1);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator>(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;

        if (m_Size != rhs.GetSize()) {
            return m_Size > rhs.GetSize();
        }

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);

        const ssize cmp = Algorithms::Compare(m_pQueueBegin, m_pQueueBegin + sizeBottom, rhs.Data(), rhs.Data() + sizeBottom);

        switch (cmp) {
        case -1: {
            return false;
        }
        case 0: {
            break;
        }
        case 1: {
            return true;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        return Algorithms::CompareGT(m_pMemoryBegin, m_pMemoryBegin + m_Size - sizeBottom, rhs.Data() + sizeBottom, rhs.DataEnd());
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator>(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) const noexcept {

        return queue < *this;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator<=(U&& rhs) const noexcept {

        return !(*this > rhs);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator<=(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) const noexcept {

        return !(*this > queue);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator>=(U&& rhs) const noexcept {

        return !(*this < rhs);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator>=(const Queue<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& queue) const noexcept {

        return !(*this < queue);
    }

private:

    ALT_FORCE_INLINE static usize GetNextSize(const usize size) noexcept {

        return Num::Max(Num::RoundUpToPowerOf2(size), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);
    }

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE static bool PushBackVariadicImpl(Queue& self, HeadT&& head, TailT&&... tail) noexcept {
        iferr_scope;

        self.PushBack(std::forward<HeadT>(head)) iferr_return false;
        return PushBackVariadicImpl(self, std::forward<TailT>(tail)...);
    }

    ALT_FORCE_INLINE static bool PushBackVariadicImpl(Queue&) noexcept { return true; }

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE static bool PushFrontVariadicImpl(Queue& self, HeadT&& head, TailT&&... tail) noexcept {
        iferr_scope;

        self.PushFront(std::forward<HeadT>(head)) iferr_return false;
        return PushFrontVariadicImpl(self, std::forward<TailT>(tail)...);
    }

    ALT_FORCE_INLINE static bool PushFrontVariadicImpl(Queue&) noexcept { return true; }

    ALT_FORCE_INLINE bool ReallocateToNextGrow(const usize size) noexcept {

        const usize sizeNext = GetNextSize(size);
        return Reallocate(sizeNext, Num::Max(size, Num::Desc<usize>::Max() / sizeof(T)));
    }

    ALT_FORCE_INLINE bool Reallocate(const usize size, const usize wrapSize = Num::Desc<usize>::Max() / sizeof(T)) noexcept {
        iferr_scope;

        ALT_ASSERT(size > GetCapacity());

        const usize sizeBottomOld = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

        const Memory::ReallocBits bits = m_Size == GetCapacity()
            ? Memory::ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace
            : Memory::ReallocBits::None;

        const usize offset = static_cast<usize>(m_pQueueBegin - m_pMemoryBegin);

        Memory::MallocWrapResult<T> result = Memory::ChooseAlignedReallocWrap<T>(
            m_pMemoryBegin,
            m_Size,
            size,
            wrapSize,
            Memory::ReallocBits::InPlace | bits
        ) ifok {

            m_pMemoryBegin = result.pMemory;
            m_pMemoryEnd = result.pMemory + result.size;
            m_pQueueBegin = result.pMemory + offset;

            if (m_Size > sizeBottomOld) {

                const usize sizeTopOld = m_Size - sizeBottomOld;
                const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);

                const usize size0 = Num::Min(sizeTopOld, sizeBottomCapacity - sizeBottomOld);
                Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(m_pQueueBegin + sizeBottomOld, m_pMemoryBegin, m_pMemoryBegin + size0);

                if (TypeTraits::is_trivially_reallocatable_v<T>) {

                    const usize sizeInPlaceMove = sizeTopOld - size0;
                    if (sizeInPlaceMove <= size0) {

                        std::memcpy(m_pMemoryBegin, m_pMemoryBegin + size0, sizeInPlaceMove * sizeof(T));
                    }
                    else {

                        std::memmove(m_pMemoryBegin, m_pMemoryBegin + size0, sizeInPlaceMove * sizeof(T));
                    }
                }
                else {

                    const usize sizeInPlaceMove = sizeTopOld - size0;
                    if (sizeInPlaceMove <= size0) {

                        Memory::MoveDestructibleConstructObjectsFromFirstElementNonOverlap(m_pMemoryBegin, m_pMemoryBegin + size0, m_pMemoryBegin + sizeTopOld);
                    }
                    else {

                        Memory::MoveDestructibleConstructObjectsFromFirstElement(m_pMemoryBegin, m_pMemoryBegin + size0, m_pMemoryBegin + sizeTopOld);
                    }
                }
            }
        }
        else {

            result = Memory::ChooseAlignedMallocWrap<T>(size, wrapSize) iferr_return false;

            const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
            const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);

            Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(result.pMemory, m_pQueueBegin, m_pQueueBegin + sizeBottom);
            Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(result.pMemory + sizeBottomCapacity, m_pMemoryBegin, m_pMemoryBegin + m_Size - sizeBottom);

            Memory::ChooseAlignedFree<T>(m_pMemoryBegin);

            m_pMemoryBegin = result.pMemory;
            m_pMemoryEnd = result.pMemory + result.size;
            m_pQueueBegin = result.pMemory;
        }

        return true;
    }

    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE Queue(Rule5Move_t, Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& rhs) noexcept
        : m_pMemoryBegin(Exchange(rhs.m_pMemoryBegin, nullptr))
        , m_pMemoryEnd(Exchange(rhs.m_pMemoryEnd, nullptr))
        , m_pQueueBegin(Exchange(rhs.m_pQueueBegin, nullptr))
        , m_Size(Exchange(rhs.m_Size, 0))
    {}

    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE void Assign(Rule5Move_t, Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& rhs) noexcept {

        if (static_cast<void*>(this) == &rhs) {
            return;
        }

        if (m_pMemoryBegin) {

            const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
            const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);

            Memory::DestroyObjects(m_pQueueBegin, m_pQueueBegin + sizeBottom);
            Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryBegin + m_Size - sizeBottom);

            Memory::ChooseAlignedFree<T>(m_pMemoryBegin);
        }

        m_pMemoryBegin = Exchange(rhs.m_pMemoryBegin, nullptr);
        m_pMemoryEnd = Exchange(rhs.m_pMemoryEnd, nullptr);
        m_pQueueBegin = Exchange(rhs.m_pQueueBegin, nullptr);
        m_Size = Exchange(rhs.m_Size, 0);
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        if (!m_pMemoryBegin) {
            return;
        }

        const usize sizeBottomCapacity = static_cast<usize>(m_pMemoryEnd - m_pQueueBegin);
        const usize sizeBottom = Num::Min(m_Size, sizeBottomCapacity);

        Memory::DestroyObjects(m_pQueueBegin, m_pQueueBegin + sizeBottom);
        Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryBegin + m_Size - sizeBottom);

        Memory::ChooseAlignedFree<T>(m_pMemoryBegin);

        m_pMemoryBegin = nullptr;
        m_pMemoryEnd = nullptr;
        m_pQueueBegin = nullptr;
        m_Size = 0;
    }

private:

    template <typename T, usize>
    friend class Queue;

    template <typename T>
    friend class QueueIterator;

    template <typename T>
    friend class QueueIteratorConst;

    /*
     *                        m_Size 
     *                      v        v 
     * [....................xxxxxxxxxx.....]
     *  ^                   ^              ^
     *  m_pMemoryBegin      m_pQueueBegin  m_pMemoryEnd
     * 
     * m_Size
     *     v                v
     * [xxxx................xxxxxxxxxxxxxxx]
     *  ^                   ^              ^
     *  m_pMemoryBegin      m_pQueueBegin  m_pMemoryEnd
     */

    T* m_pMemoryBegin = nullptr;
    T* m_pMemoryEnd = nullptr;

    T* m_pQueueBegin = nullptr;
    usize m_Size = 0;
};

template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
class IteratorCollector<Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>> {
public:

    using SelfT = Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>;

    template <typename U = T,
        std::enable_if_t<std::is_assignable_v<T&, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait>& iter) noexcept {
        iferr_scope;

        const usize sizeHint = iter.GetSizeHint();
        self.ReserveAddition(sizeHint) iferr_return false;

        while (Optional<U> val = iter.Next()) {

            self.PushBack(std::move(val.GetValue())) iferr_return false;
        }

        return true;
    }
};

/**
 * @brief Format specialization for Queue<T>.
 */
template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
class Format<Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, Slice<const char> parameters, const Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>& vals
    ) noexcept {
        iferr_scope;
        
        using CleanU = _FormatDetails::clean_t<U>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "noinline"_s) {

            EstimateExactSizeResult result{};
            result.size = 2;

            if (vals.IsEmpty()) {
                return result;
            }

            estimator.PushIndent();

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const U& val : iter) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val) iferr_return Nullopt;

                result.size += estimator.GetIndentEstimatedSize() + estimated.size;
                result.size += 2;
            }

            if (vals.IsPopulated()) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, vals.GetLast()) iferr_return Nullopt;

                result.size += estimator.GetIndentEstimatedSize() + estimated.size;
                result.size += 1;
            }

            estimator.PopIndent();

            result.size += estimator.GetIndentEstimatedSize() + 1;

            return result;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            EstimateExactSizeResult result{};
            result.size = 1;

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const U& val : iter) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val) iferr_return Nullopt;

                result.size += estimated.size;
                result.size += 2;
            }

            if (vals.IsPopulated()) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, vals.GetLast()) iferr_return Nullopt;

                result.size += estimated.size;
            }

            result.size += 1;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(
        FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Queue<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>& vals
    ) noexcept {
        iferr_scope;

        using CleanT = _FormatDetails::clean_t<T>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "noinline"_s) {

            writer.Append('[') iferr_return false;

            if (vals.IsEmpty()) {

                writer.Append(']') iferr_return false;
                return true;
            }

            writer.Append('\n') iferr_return false;
            writer.PushIndent();

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const T& val : iter) {

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append('\n') iferr_return false;
            }

            if (vals.IsPopulated()) {

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, vals.GetLast()) iferr_return false;
                writer.Append('\n') iferr_return false;
            }

            writer.PopIndent();

            writer.AppendIndent() iferr_return false;
            writer.Append(']') iferr_return false;

            return true;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            writer.Append('[') iferr_return false;

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const T& val : iter) {

                Format<T>::WriteAppend(writer, parametersNext, val) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;
            }

            if (vals.IsPopulated()) {

                Format<CleanT>::WriteAppend(writer, parametersNext, vals.GetLast()) iferr_return false;
            }

            writer.Append(']') iferr_return false;

            return true;
        }

        return false;
    }
};

} // namespace Alt

#endif // __ALT_STD_QUEUE__