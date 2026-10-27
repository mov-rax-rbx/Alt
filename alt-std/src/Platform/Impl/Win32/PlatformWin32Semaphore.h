#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_SEMAPHORE__
#define __ALT_STD_PLATFORM_WIN32_SEMAPHORE__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32.h"

#include "../../../Utils/Optional.h"
#include "../../../Utils/NonCopyableMovable.h"

namespace Alt {
namespace _PlatformDetails {

class Semaphore;

} // namespace _PlatformDetails

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<_PlatformDetails::Semaphore>
    : std::false_type
{};

} // namespace TypeTraits

namespace _PlatformDetails {

/**
 * @brief Represents a semaphore for thread synchronization.
 * 
 * @details This class encapsulates the underlying Windows Kernel Semaphore handle, providing 
 * RAII-based lifecycle management. It allows threads to signal and wait on shared resources 
 * using platform-specific primitives while ensuring handles are safely released upon destruction.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::Semaphore semaphore = Alt::Semaphore::Create();
 * 
 * Alt::Thread::Allocate([&]() {
 *     iferr_scope;
 * 
 *     semaphore.Wait() iferr_panic_only_dev("poisoning or some system error");
 *     ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());
 * }).GetVal().Forget();
 * 
 * Alt::Thread::Allocate([&]() {
 * 
 *     semaphore.Wait() iferr_panic_only_dev("poisoning or some system error");
 *     ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());
 * }).GetVal().Forget();
 * 
 * semaphore.Signal(2) iferr_panic_only_dev("overflow numbers to signal - we can use only 'Semaphore::GetMax()' for total 'Wait()' and 'Signal(...)'");
 */
class Semaphore {
private:

    ALT_MAKE_CLASS_NON_COPYABLE(Semaphore);

    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(Semaphore);
    ALT_IFERR_INJECT_MOVE_CONSTRUCTOR(Semaphore);

public:

    /**
     * @brief Returns the maximum possible count for a semaphore.
     * @return The maximum value allowed by the platform's semaphore implementation.
     * @example
     * 
     * constexpr usize semMaxInUse = Alt::Semaphore::GetMax();
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize GetMax() noexcept {
        return Platform::SemaphoreGetMax();
    }

    /**
     * @brief Create a new instance of Semaphore.
     * @return An Optional containing the Semaphore instance, or Nullopt if the OS semaphore creation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Semaphore sem = Alt::Semaphore::Create() iferr_return;
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<IfErrInjectionMove<Semaphore>> Create(const usize numOfDeferredSignals = GetMax()) noexcept {
        iferr_scope;

        ALT_ASSERT(numOfDeferredSignals > 0 && numOfDeferredSignals <= GetMax());

        Semaphore sem{};
        sem.m_Handle = Platform::SemaphoreCreate(0, static_cast<u32>(numOfDeferredSignals)) iferr_return Nullopt;

        return IfErrInjectionMove(std::move(sem));
    }

    /**
     * @brief Blocks the current thread until the semaphore is signaled.
     * 
     * @details Performs a blocking wait operation waits indefinitely unless
     * interrupted by system events or logic errors.
     * 
     * @return True if the wait was successful and the semaphore was signaled; false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Semaphore sem = Alt::Semaphore::Create() iferr_return;
     * sem.Wait() iferr_panic_only_dev("poisoning or some system error");
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Wait() const noexcept {
        iferr_scope;

        const Platform::WaitForSingleObjectResult result = Platform::WaitForSingleObject(m_Handle, __Win32__INFINITE)
            iferr_return false;

        return result == Platform::WaitForSingleObjectResult::Signaled;
    }

    /**
     * @brief Signals the semaphore to release a specified number of counts.
     * 
     * @param numToSignal The number of units to release into the semaphore's counter. 
     * Must be greater than 0 and less than or equal to GetMax().
     * 
     * @return True if the signal operation was successful; false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Semaphore sem = Alt::Semaphore::Create();
     * 
     * Alt::Thread::Allocate([&]() {
     *     iferr_scope;
     * 
     *     sem.Wait() iferr_panic_only_dev("poisoning or some system error");
     * 
     *     ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());
     * 
     * }).GetVal().Forget();
     * 
     * sem.Signal(2) iferr_panic_only_dev("overflow numbers to signal - we can use only 'Semaphore::GetMax()' total 'Wait()' and 'Signal(...)'");
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Signal(const usize numToSignal) const noexcept {
        iferr_scope;

        /*
         * This is handled by 'SemaphoreRelease(...)' itself
         */
        // ALT_ASSERT(numToSignal != 0 && numToSignal <= GetMax());

        Platform::SemaphoreRelease(m_Handle, static_cast<u32>(numToSignal)) iferr_return false;

        return true;
    }

    ~Semaphore() noexcept {
        iferr_scope;

        if (m_Handle.handle == __Win32__INVALID_HANDLE_VALUE) {
            return;
        }

        Platform::SemaphoreClose(m_Handle) iferr_panic_only_dev("Just allow to fail. The system will close all handles on process exit.");
        m_Handle = Platform::Handle{};
    }

private:

    ALT_FORCE_INLINE Semaphore() noexcept = default;

    ALT_FORCE_INLINE Semaphore(Semaphore&& rhs) noexcept
        : m_Handle(Exchange(rhs.m_Handle, Platform::Handle{}))
    {}

    ALT_FORCE_INLINE Semaphore& operator=(Semaphore&& rhs) noexcept {

        m_Handle = Exchange(rhs.m_Handle, Platform::Handle{});

        return *this;
    }

private:

    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) Platform::Handle m_Handle = Platform::Handle{};
};

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_SEMAPHORE__