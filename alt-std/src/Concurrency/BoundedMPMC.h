#pragma once

#ifndef __ALT_STD_STATIC_QUEUE_MPMC__
#define __ALT_STD_STATIC_QUEUE_MPMC__

#include "Backoff.h"

#include "../Platform/Platform.h"

#include "../Memory/MaybeUninit.h"

#include "../Utils/Num.h"
#include "../Utils/Optional.h"
#include "../Utils/NonCopyableMovable.h"

#ifdef ALT_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4324 /* structure was padded due to alignment specifier */)
#endif // ALT_COMPILER_MSVC

namespace Alt {

template <typename T>
class BoundedMPMC;

template <typename T, usize CAPACITY_IN_ELEMENTS_POW2>
class StaticBoundedMPMC;

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<BoundedMPMC<T>>
    : std::false_type
{};

template <typename T, usize CAPACITY_IN_ELEMENTS_POW2>
struct __is_trivially_reallocatable__<StaticBoundedMPMC<T, CAPACITY_IN_ELEMENTS_POW2>>
    : std::false_type
{};

} // namespace TypeTraits

/*
 * @brief Lock-free, bounded MPMC queue.
 * @tparam T The type of elements stored in the queue. Must be move-constructible or copy-constructible.
 * 
 * @note Based on: http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 * Fixed false overflow/underflow behavior on push/pop, added 'Len()' method
 *
 * Don't have copy/move constructors/assignments because not thread-safe. If need postponed
 * initialization wrap inside 'Optional'
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::BoundedMPMC<Alt::String> queue = Alt::BoundedMPMC<Alt::String>::Allocate() iferr_return;
 * 
 * queue.TryPushBack("Task"_salloc) iferr_return;
 * Alt::String taskName = queue.TryPopBack() iferr_return;
 * 
 * assert(taskName == "Task"_s);
 */
template <typename T>
class BoundedMPMC {

    static_assert(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>, "");

    using Self = BoundedMPMC;

    ALT_MAKE_CLASS_NON_COPYABLE(BoundedMPMC);

    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(BoundedMPMC);
    ALT_IFERR_INJECT_MOVE_CONSTRUCTOR(BoundedMPMC);

public:

    /**
     * @brief Allocates a new BoundedMPMC instance on the heap with a specified capacity.
     * 
     * @details The requested capacity is automatically rounded up to the nearest power of two.
     * 
     * @param capacity_ The desired maximum number of elements in the queue.
     * 
     * @return An Optional containing the successfully allocated BoundedMPMC, or Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BoundedMPMC<usize> queue = Alt::BoundedMPMC<usize>::Allocate(1024) iferr_return;
     * 
     * assert(queue.GetCapacity() == 1024);
     */
    ALT_NODISCARD static Optional<IfErrInjectionMove<BoundedMPMC>> Allocate(const usize capacity_) noexcept {
        iferr_scope;

        const usize capacity = Num::RoundUpToPowerOf2(capacity_);

        BoundedMPMC mpmc{};
        mpmc.m_CapacityMask = capacity - 1;

        mpmc.m_pCells = Memory::ChooseAlignedMalloc<Cell>(capacity) iferr_return Nullopt;

        for (usize i = 0; i < capacity; ++i) {

            Cell* pCell = Memory::ConstructDefaultObject(mpmc.m_pCells + i);
            pCell->m_Sequence = i;
        }

        return IfErrInjectionMove(std::move(mpmc));
    }

    /**
     * @brief Attempts append element to the back of the queue.
     * @tparam U The type of data to push. Must be constructible to T.
     * @param data The value to be moved into the queue.
     * @return True if the push was successful, false if the queue is full (overflow).
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BoundedMPMC<usize> queue = Alt::BoundedMPMC<usize>::Allocate(1024) iferr_return;
     * queue.TryPushBack(44) iferr_return;
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool TryPushBack(T&& data) noexcept {

        return TryPushBackMove(data);
    }

    /**
     * @brief Attempts append element to the back of the queue.
     * 
     * @note If push successful function return true and data moved;
     *       otherwise returns false and data in correct state.
     * 
     * @tparam U The type of data to push. Must be constructible to T.
     * @param data The value to be moved into the queue.
     * @return True if the push was successful, false if the queue is full (overflow).
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool TryPushBackMove(T& data) noexcept {

        Backoff backoff{};

        Cell* pCell;
        usize seq;

        const usize mask = m_CapacityMask;

        for (;;) {

            const usize pushPos = Platform::AtomicLoad(&m_PushPos, Platform::MemoryOrder::Relaxed);

            const usize idx = pushPos & mask;
            pCell = m_pCells + idx;

            seq = Platform::AtomicLoad(&pCell->m_Sequence, Platform::MemoryOrder::Acquire);

            if (seq == pushPos) {

                if (Platform::AtomicCompareExchange(&m_PushPos, pushPos, pushPos + 1, Platform::MemoryOrder::SeqCst, Platform::MemoryOrder::Relaxed) != pushPos) {

                    /**
                     * Contention
                     */
                    backoff.SpinLight();
                }
                else {

                    break;
                }
            }
            /**
             * Overflow?
             */
            else if (seq + mask == pushPos) {

                const usize popPos = Platform::AtomicLoad(&m_PushPos, Platform::MemoryOrder::SeqCst);

                if (pushPos == popPos) {

                    /**
                     * Yes, overflow!
                     */
                    return false;
                }

                /**
                 * Maybe no, contention with 'Pop()'?
                 */
                backoff.SpinLight();
            }
            else {

                /**
                 * Another thread something doing with current Cell lets wait
                 */
                backoff.SpinHeavy();
            }
        }

        pCell->m_Data.Construct(std::move(data));
        Platform::AtomicStore(&pCell->m_Sequence, seq + 1, Platform::MemoryOrder::Release);

        return true;
    }

    /**
     * @brief Attempts remove and returns the front element of the queue.
     * @return An Optional containing the popped value, or Nullopt if the queue is empty.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::BoundedMPMC<usize> queue = Alt::BoundedMPMC<usize>::Allocate(1024) iferr_return;
     * 
     * queue.TryPushBack(44) iferr_return;
     * const usize val = queue.TryPopFront() iferr_return;
     * 
     * assert(val == 44);
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> TryPopFront() noexcept {

        Backoff backoff{};

        Cell* pCell;
        usize seq;

        const usize mask = m_CapacityMask;

        for (;;) {

            const usize popPos = Platform::AtomicLoad(&m_PopPos, Platform::MemoryOrder::Relaxed);

            const usize idx = popPos & mask;
            pCell = m_pCells + idx;

            seq = Platform::AtomicLoad(&pCell->m_Sequence, Platform::MemoryOrder::Acquire);

            if (seq == popPos + 1) {

                if (Platform::AtomicCompareExchange(&m_PopPos, popPos, popPos + 1, Platform::MemoryOrder::SeqCst, Platform::MemoryOrder::Relaxed) != popPos) {

                    /**
                     * Contention
                     */
                    backoff.SpinLight();
                }
                else {

                    break;
                }
            }
            /**
             * Empty?
             */
            else if (seq == popPos) {

                const usize pushPos = Platform::AtomicLoad(&m_PopPos, Platform::MemoryOrder::SeqCst);

                if (pushPos == popPos) {

                    /**
                     * Yes, empty!
                     */
                    return Nullopt;
                }

                /**
                 * Maybe no, contention with 'Push()'?
                 */
                backoff.SpinLight();
            }
            else {

                /**
                 * Another thread something doing with current Cell lets wait
                 */
                backoff.SpinHeavy();
            }
        }

        T result = Memory::MoveDestructibleObject(pCell->m_Data.GetVal());
        Platform::AtomicStore(&pCell->m_Sequence, seq + mask, Platform::MemoryOrder::Release);

        return result;
    }

    /**
     * @brief Returns the number of elements currently in the queue.
     * @return The size of the queue.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        const usize popPos = Platform::AtomicLoad(&m_PopPos, Platform::MemoryOrder::SeqCst);
        const usize pushPos = Platform::AtomicLoad(&m_PushPos, Platform::MemoryOrder::SeqCst);

        return pushPos - popPos;
    }

    /**
     * @brief Returns the total number of elements the queue can hold.
     * @return The capacity of the queue.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetCapacity() const noexcept {
        return m_CapacityMask + 1;
    }

    /**
     * @brief Checks if the queue contains no elements.
     * @return True if empty, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        const usize popPos = Platform::AtomicLoad(&m_PopPos, Platform::MemoryOrder::SeqCst);
        const usize pushPos = Platform::AtomicLoad(&m_PushPos, Platform::MemoryOrder::SeqCst);

        return pushPos == popPos;
    }

    /**
     * @brief Checks if the queue contains at least one element.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept {
        return !IsEmpty();
    }

    /**
     * @brief Removes all elements from the queue.
     * @details Destroys all existing objects.
     */
    void Clear() noexcept {

        while (ALT_MAYBE_UNUSED Optional<T> data = TryPopFront())
        {}
    }

    /**
     * @brief Destructor
     * @details Non thread safe - the destruction must be after main queue usage interruption
     */
    ~BoundedMPMC() noexcept {

        if (!m_pCells) {
            return;
        }

        while (m_PopPos != m_PushPos) {

            ALT_ASSERT(m_pCells[m_PopPos & m_CapacityMask].m_Sequence == m_PopPos + 1);

            m_pCells[m_PopPos & m_CapacityMask].m_Data.Destroy();
            m_PopPos += 1;
        }

        Memory::ChooseAlignedFree<Cell>(m_pCells);
        m_pCells = nullptr;
    }

private:

    ALT_FORCE_INLINE BoundedMPMC() noexcept = default;

    ALT_FORCE_INLINE BoundedMPMC(BoundedMPMC&& rhs) noexcept
        : m_CapacityMask(rhs.m_CapacityMask)
        , m_pCells(Exchange(rhs.m_pCells, nullptr))
        , m_PushPos(rhs.m_PushPos)
        , m_PopPos(rhs.m_PopPos)
    {}

    ALT_FORCE_INLINE BoundedMPMC& operator=(BoundedMPMC&& rhs) noexcept {

        m_CapacityMask = rhs.m_CapacityMask;
        m_pCells = Exchange(rhs.m_pCells, nullptr);
        m_PushPos = rhs.m_PushPos;
        m_PopPos = rhs.m_PopPos;

        return *this;
    }

private:

    // Separate Cells from each other
    struct alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) Cell {

        MaybeUninit<T> m_Data;
        usize m_Sequence;
    };

    // Put in one cache - together
    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) usize m_CapacityMask = 0;
    Cell* m_pCells = nullptr;

    // Separate caches to reduce contention between 'Push()' and 'Pop()'
    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) usize m_PushPos = 0;
    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) usize m_PopPos = 0;
};

template <typename T, usize CAPACITY_IN_ELEMENTS_POW2>
class StaticBoundedMPMC {

    static_assert(Num::IsPowerOf2(CAPACITY_IN_ELEMENTS_POW2), "");
    static_assert(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>, "");

    using Self = StaticBoundedMPMC;

    ALT_MAKE_CLASS_NON_COPYABLE(StaticBoundedMPMC);

public:

    /**
     * @brief Creates a new BoundedMPMC instance.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticBoundedMPMC<usize, 1024> queue{};
     */
    ALT_NODISCARD StaticBoundedMPMC() noexcept {

        const usize capacity = CAPACITY_IN_ELEMENTS_POW2;

        m_CapacityMask = capacity - 1;

        for (usize i = 0; i < capacity; ++i) {

            Cell* pCell = m_Cells + i;
            pCell->m_Sequence = i;
        }
    }

    /**
     * @brief Attempts append element to the back of the queue.
     * @tparam U The type of data to push. Must be constructible to T.
     * @param data The value to be moved into the queue.
     * @return True if the push was successful, false if the queue is full (overflow).
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StatiacBoundedMPMC<usize, 1024> queue;
     * queue.TryPushBack(44) iferr_return;
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool TryPushBack(T&& data) noexcept {

        return TryPushBackMove(data);
    }

    /**
     * @brief Attempts append element to the back of the queue.
     * 
     * @note If push successful function return true and data moved;
     *       otherwise returns false and data in correct state.
     * 
     * @tparam U The type of data to push. Must be constructible to T.
     * @param data The value to be moved into the queue.
     * @return True if the push was successful, false if the queue is full (overflow).
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool TryPushBackMove(T& data) noexcept {

        Backoff backoff{};

        Cell* pCell;
        usize seq;

        const usize mask = m_CapacityMask;

        for (;;) {

            const usize pushPos = Platform::AtomicLoad(&m_PushPos, Platform::MemoryOrder::Relaxed);

            const usize idx = pushPos & mask;
            pCell = m_Cells + idx;

            seq = Platform::AtomicLoad(&pCell->m_Sequence, Platform::MemoryOrder::Acquire);

            if (seq == pushPos) {

                if (Platform::AtomicCompareExchange(&m_PushPos, pushPos, pushPos + 1, Platform::MemoryOrder::SeqCst, Platform::MemoryOrder::Relaxed) != pushPos) {

                    /**
                     * Contention
                     */
                    backoff.SpinLight();
                }
                else {

                    break;
                }
            }
            /**
             * Overflow?
             */
            else if (seq + mask == pushPos) {

                const usize popPos = Platform::AtomicLoad(&m_PushPos, Platform::MemoryOrder::SeqCst);

                if (pushPos == popPos) {

                    /**
                     * Yes, overflow!
                     */
                    return false;
                }

                /**
                 * Maybe no, contention with 'Pop()'?
                 */
                backoff.SpinLight();
            }
            else {

                /**
                 * Another thread something doing with current Cell lets wait
                 */
                backoff.SpinHeavy();
            }
        }

        pCell->m_Data.Construct(std::move(data));
        Platform::AtomicStore(&pCell->m_Sequence, seq + 1, Platform::MemoryOrder::Release);

        return true;
    }

    /**
     * @brief Attempts remove and returns the front element of the queue.
     * @return An Optional containing the popped value, or Nullopt if the queue is empty.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticBoundedMPMC<usize, 1024> queue{};
     * 
     * queue.TryPushBack(44) iferr_return;
     * const usize val = queue.TryPopFront() iferr_return;
     * 
     * assert(val == 44);
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> TryPopFront() noexcept {

        Backoff backoff{};

        Cell* pCell;
        usize seq;

        const usize mask = m_CapacityMask;

        for (;;) {

            const usize popPos = Platform::AtomicLoad(&m_PopPos, Platform::MemoryOrder::Relaxed);

            const usize idx = popPos & mask;
            pCell = m_Cells + idx;

            seq = Platform::AtomicLoad(&pCell->m_Sequence, Platform::MemoryOrder::Acquire);

            if (seq == popPos + 1) {

                if (Platform::AtomicCompareExchange(&m_PopPos, popPos, popPos + 1, Platform::MemoryOrder::SeqCst, Platform::MemoryOrder::Relaxed) != popPos) {

                    /**
                     * Contention
                     */
                    backoff.SpinLight();
                }
                else {

                    break;
                }
            }
            /**
             * Empty?
             */
            else if (seq == popPos) {

                const usize pushPos = Platform::AtomicLoad(&m_PopPos, Platform::MemoryOrder::SeqCst);

                if (pushPos == popPos) {

                    /**
                     * Yes, empty!
                     */
                    return Nullopt;
                }

                /**
                 * Maybe no, contention with 'Push()'?
                 */
                backoff.SpinLight();
            }
            else {

                /**
                 * Another thread something doing with current Cell lets wait
                 */
                backoff.SpinHeavy();
            }
        }

        T result = Memory::MoveDestructibleObject(pCell->m_Data.GetVal());
        Platform::AtomicStore(&pCell->m_Sequence, seq + mask, Platform::MemoryOrder::Release);

        return result;
    }

    /**
     * @brief Returns the number of elements currently in the queue.
     * @return The size of the queue.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        const usize popPos = Platform::AtomicLoad(&m_PopPos, Platform::MemoryOrder::SeqCst);
        const usize pushPos = Platform::AtomicLoad(&m_PushPos, Platform::MemoryOrder::SeqCst);

        return pushPos - popPos;
    }

    /**
     * @brief Returns the total number of elements the queue can hold.
     * @return The capacity of the queue.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetCapacity() const noexcept {
        return m_CapacityMask + 1;
    }

    /**
     * @brief Checks if the queue contains no elements.
     * @return True if empty, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        const usize popPos = Platform::AtomicLoad(&m_PopPos, Platform::MemoryOrder::SeqCst);
        const usize pushPos = Platform::AtomicLoad(&m_PushPos, Platform::MemoryOrder::SeqCst);

        return pushPos == popPos;
    }

    /**
     * @brief Checks if the queue contains at least one element.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept {
        return !IsEmpty();
    }

    /**
     * @brief Removes all elements from the queue.
     * @details Destroys all existing objects.
     */
    void Clear() noexcept {

        while (ALT_MAYBE_UNUSED Optional<T> data = TryPopFront())
        {}
    }

    /**
     * @brief Destructor
     * @details Non thread safe - the destruction must be after main queue usage interruption
     */
    ~StaticBoundedMPMC() noexcept {

        while (m_PopPos != m_PushPos) {

            ALT_ASSERT(m_Cells[m_PopPos & m_CapacityMask].m_Sequence == m_PopPos + 1);

            m_Cells[m_PopPos & m_CapacityMask].m_Data.Destroy();
            m_PopPos += 1;
        }

        m_PushPos = m_PopPos;
    }

private:

    ALT_FORCE_INLINE StaticBoundedMPMC(StaticBoundedMPMC&& rhs) noexcept
        : m_CapacityMask(rhs.m_CapacityMask)
        , m_PushPos(rhs.m_PushPos)
        , m_PopPos(rhs.m_PopPos)
    {
        MoveCellsFrom(rhs);
    }

    ALT_FORCE_INLINE StaticBoundedMPMC& operator=(StaticBoundedMPMC&& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return *this;
        }

        m_CapacityMask = rhs.m_CapacityMask;
        m_PushPos = rhs.m_PushPos;
        m_PopPos = rhs.m_PopPos;

        MoveCellsFrom(rhs);

        return *this;
    }

    ALT_FORCE_INLINE void MoveCellsFrom(StaticBoundedMPMC&& rhs) noexcept {

        usize pushPos = m_PushPos;
        usize popPos = m_PopPos;

        pushPos &= m_CapacityMask;
        popPos &= m_CapacityMask;

        if (pushPos >= popPos) {

            std::memcpy(m_Cells, rhs.m_Cells, popPos * sizeof(Cell));
            std::memcpy(m_Cells + pushPos, rhs.m_Cells + pushPos, (m_CapacityMask + 1 - pushPos) * sizeof(Cell));

            for (usize i = popPos; i < pushPos; ++i) {

                m_Cells[i].Construct(Memory::MoveDestructibleObject(rhs.m_Cells[i].m_Data.GetVal()));
                m_Cells[i].m_Sequence = rhs.m_Cells[i].m_Sequence;
            }
        }
        else {

            std::memcpy(m_Cells + pushPos, rhs.m_Cells + pushPos, (popPos - pushPos) * sizeof(Cell));

            for (usize i = 0; i < pushPos; ++i) {

                m_Cells[i].Construct(Memory::MoveDestructibleObject(rhs.m_Cells[i].m_Data.GetVal()));
                m_Cells[i].m_Sequence = rhs.m_Cells[i].m_Sequence;
            }

            for (usize i = popPos; i <= m_CapacityMask; ++i) {

                m_Cells[i].Construct(Memory::MoveDestructibleObject(rhs.m_Cells[i].m_Data.GetVal()));
                m_Cells[i].m_Sequence = rhs.m_Cells[i].m_Sequence;
            }
        }
    }

private:

    // Separate Cells from each other
    struct alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) Cell {

        MaybeUninit<T> m_Data;
        usize m_Sequence;
    };

    // Put in one cache - together
    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) usize m_CapacityMask = 0;
    Cell m_Cells[CAPACITY_IN_ELEMENTS_POW2]{};

    // Separate caches to reduce contention between 'Push()' and 'Pop()'
    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) usize m_PushPos = 0;
    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) usize m_PopPos = 0;
};

} // namespace Alt

#ifdef ALT_COMPILER_MSVC
#pragma warning(pop)
#endif // ALT_COMPILER_MSVC

#endif // __ALT_STD_STATIC_QUEUE_MPMC__