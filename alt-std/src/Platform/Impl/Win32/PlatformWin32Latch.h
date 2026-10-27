#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_LATCH__
#define __ALT_STD_PLATFORM_WIN32_LATCH__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32.h"

#include "../../../Utils/Optional.h"
#include "../../../Utils/NonCopyableMovable.h"

namespace Alt {
namespace _PlatformDetails {

class Latch;

} // namespace _PlatformDetails

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<_PlatformDetails::Latch>
    : std::false_type
{};

} // namespace TypeTraits

namespace _PlatformDetails {

/**
 * @brief A synchronization primitive that allows multiple threads to wait for a specific number of events.
 * 
 * @details The Latch maintains an internal atomic counter. Threads calling Dec() decrement the counter; 
 * when the counter reaches zero, the underlying semaphore is released, unblocking threads currently blocked
 * by Wait().
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::Latch latch = Alt::Latch::Create(2) iferr_return;
 * 
 * Alt::Thread::Allocate([&]() {
 * 
 *     latch.Dec();
 * 
 *     ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());
 * }).GetVal().Forget();
 * 
 * Alt::Thread::Allocate([&]() {
 * 
 *     latch.Dec();
 * 
 *     ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());
 * }).GetVal().Forget();
 * 
 * latch.Wait() iferr_panic_only_dev("poisoning or some system error");
 */
class Latch {
private:

    ALT_MAKE_CLASS_NON_COPYABLE(Latch);

    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(Latch);
    ALT_IFERR_INJECT_MOVE_CONSTRUCTOR(Latch);

public:

    /**
     * @brief Create a new instance of Latch.
     * @return An Optional containing the Latch instance, or Nullopt if the OS semaphore creation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Latch latch = Alt::Latch::Create() iferr_return;
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<IfErrInjectionMove<Latch>> Create(const usize cnt) noexcept {
        iferr_scope;

        ALT_ASSERT(cnt > 0);

        Latch latch{};
        latch.m_Cnt = cnt;
        latch.m_Handle = Platform::SemaphoreCreate(0, 1) iferr_return Nullopt;

        return IfErrInjectionMove(std::move(latch));
    }

    /**
     * @brief Decrements the latch counter and signal if the count reaches zero.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Latch latch = Alt::Latch::Create(1) iferr_return;
     * 
     * latch.Dec();
     */
    ALT_FORCE_INLINE void Dec() noexcept {
        iferr_scope;

        /*
         * We need AcqRel in case when not released below by 'SemaphoreRelease(...)'
         */
        ALT_MAYBE_UNUSED const usize cnt = _PlatformAtomicDetails::AtomicDecAcqRel(&m_Cnt);
        ALT_ASSERT(cnt != 0);

        if (cnt == 1) {

            Platform::SemaphoreRelease(m_Handle, 1) iferr_panic_only_dev("Should never fail. We release only 1 and repeated releases allowed. Or invalid usage because 'm_Handle' was closed.");
        }
    }

    /**
     * @brief Blocks the current thread until the latch is released.
     * @return True if the latch was signaled; false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Latch latch = Alt::Latch::Create(1) iferr_return;
     * 
     * latch.Dec();
     * 
     * latch.Wait() iferr_panic_only_dev("poisoning or some system error");
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Wait() const noexcept {
        iferr_scope;

        const Platform::WaitForSingleObjectResult result = Platform::WaitForSingleObject(m_Handle, __Win32__INFINITE)
            iferr_return false;

        return result == Platform::WaitForSingleObjectResult::Signaled;
    }

    /**
     * @brief Decrements and blocks the current thread until the latch is released.
     * @return True if the latch was signaled; false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Latch latch = Alt::Latch::Create(1) iferr_return;
     * 
     * latch.DecAndWait() iferr_panic_only_dev("poisoning or some system error");
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool DecAndWait() noexcept {

        Dec();

        return Wait();
    }

    /**
     * @brief Checks if the latch has been fully released.
     * @return True if the counter has reached zero, false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Latch latch = Alt::Latch::Create(1) iferr_return;
     * 
     * latch.DecAndWait() iferr_panic_only_dev("poisoning or some system error");
     * assert(latch.IsFinished()));
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsFinished() const noexcept {

        /*
         * Acquire because sync with 'SemaphoreRelease(...)' in 'Dec()'
         */
        return _PlatformAtomicDetails::AtomicLoadAcquire(&m_Cnt) == 0;
    }

    /**
     * @brief Resets the internal counter to reuse latch.
     * @details This method must only be called once the latch is already finished (count is zero).
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Latch latch = Alt::Latch::Create(1) iferr_return;
     * 
     * latch.DecAndWait() iferr_panic_only_dev("poisoning or some system error");
     * assert(latch.IsFinished()));
     * 
     * latch.Reset(1);
     * 
     * latch.DecAndWait() iferr_panic_only_dev("poisoning or some system error");
     * assert(latch.IsFinished()));
     */
    ALT_FORCE_INLINE void Reset(const usize cnt) noexcept {
        ALT_ASSERT(IsFinished());

        _PlatformAtomicDetails::AtomicStoreRelease(&m_Cnt, cnt);
    }

    ~Latch() noexcept {
        iferr_scope;

        if (m_Handle.handle == __Win32__INVALID_HANDLE_VALUE) {
            return;
        }

        Platform::SemaphoreClose(m_Handle) iferr_panic_only_dev("Just allow to fail. The system will close all handles on process exit.");
        m_Handle = Platform::Handle{};
    }

private:

    ALT_FORCE_INLINE Latch() noexcept = default;

    ALT_FORCE_INLINE Latch(Latch&& rhs) noexcept
        : m_Cnt(rhs.m_Cnt)
        , m_Handle(Exchange(rhs.m_Handle, Platform::Handle{}))
    {}

    ALT_FORCE_INLINE Latch& operator=(Latch&& rhs) noexcept {

        m_Cnt = rhs.m_Cnt;
        m_Handle = Exchange(rhs.m_Handle, Platform::Handle{});

        return *this;
    }

private:

    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) usize m_Cnt = 0;
    Platform::Handle m_Handle = Platform::Handle{};
};

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_LATCH__