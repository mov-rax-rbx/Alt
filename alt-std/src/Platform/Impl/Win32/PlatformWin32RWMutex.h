#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_RWMUTEX__
#define __ALT_STD_PLATFORM_WIN32_RWMUTEX__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32.h"

#include "../../../Utils/Optional.h"
#include "../../../Utils/NonCopyableMovable.h"

namespace Alt {
namespace _PlatformDetails {

class RWMutex;

} // namespace _PlatformDetails

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<_PlatformDetails::RWMutex>
    : std::false_type
{};

} // namespace TypeTraits

namespace _PlatformDetails {

/**
 * @brief Provides a platform-specific implementation of a Read/Write Mutex.
 * 
 * @details This class wraps the native Windows SRWLock functionality, providing thread synchronization
 * capabilities that allow multiple concurrent readers or a single exclusive writer.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::RWMutex mutex = Alt::RWMutex::Create() iferr_return;
 * 
 * {
 *     mutex.Lock() iferr_panic_only_dev("for actual implementation never return 'false', for other possible implementations can indicate poisoning or some system error");
 *     defer{ mutex.Unlock(); };
 * 
 *     // mutate some resource
 * }
 * 
 * {
 *     mutex.LockRead() iferr_panic_only_dev("for actual implementation never return 'false', for other possible implementations can indicate poisoning or some system error");
 *     defer{ mutex.UnlockRead(); };
 * 
 *     // read from some resource
 * }
 */
class RWMutex {
private:

    ALT_MAKE_CLASS_NON_COPYABLE(RWMutex);

    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(RWMutex);
    ALT_IFERR_INJECT_MOVE_CONSTRUCTOR(RWMutex);

public:

    /**
     * @brief Create a new instance of RWMutex.
     * 
     * @details Validates if the underlying platform supports Reader-Writer Mutexes (Windows Vista or later)
     * before attempting to instantiate. Returns an Optional containing the wrapped mutex or Nullopt on failure.
     * 
     * @return An Optional containing the RWMutex instance, or Nullopt if the OS version is incompatible.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::RWMutex mutex = Alt::RWMutex::Create() iferr_return;
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<IfErrInjectionMove<RWMutex>> Create() noexcept {

        /*
         * >= Windows Vista  or >= Windows Server 2008
         */
        if (!Platform::IsRWMutexSupported()) {
            return Nullopt;
        }

        return IfErrInjectionMove(RWMutex{});
    }

    /**
     * @brief Read/Write lock on the mutex.
     * @details This call blocks the current thread until it can acquire exclusive ownership of the mutex.
     * @return For current implementation always returned true.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::RWMutex mutex = Alt::RWMutex::Create() iferr_return;
     * 
     * mutex.Lock() iferr_panic_only_dev("for actual implementation never return 'false', for other possible implementations can indicate poisoning or some system error");
     * defer{ mutex.Unlock(); };
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Lock() noexcept {

        Platform::RWMutexLock(m_Mutex);

        return true;
    }

    /**
     * @brief Read lock on the mutex.
     * 
     * @details This call blocks the current thread until it can acquire shared ownership, allowing other 
     * readers to hold the lock simultaneously but blocking exclusive writers.
     * 
     * @return True upon successful acquisition (this method is blocking).
     * 
     * @example
     * 
     * Alt::RWMutex mutex = Alt::RWMutex::Create() iferr_return;
     * 
     * mutex.LockRead() iferr_panic_only_dev("for actual implementation never return 'false', for other possible implementations can indicate poisoning or some system error");
     * defer{ mutex.UnlockRead(); };
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool LockRead() noexcept {

        Platform::RWMutexLockRead(m_Mutex);

        return true;
    }

    /**
     * @brief Attempts read/write lock on the mutex.
     * 
     * @details Immediately returns success if the mutex is available, or failure if another thread
     * holds either a shared or exclusive lock.
     * 
     * @return True if the lock was acquired successfully; false otherwise.
     * 
     * @example
     * 
     * if (mutex.TryLock()) {
     *     defer{ mutex.Unlock(); };
     * }
     * 
     * while (!mutex.TryLock()) {
     * }
     * 
     * defer{ mutex.Unlock(); };
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool TryLock() noexcept {

        return Platform::RWMutexTryLock(m_Mutex);
    }

    /**
     * @brief Attempts read lock on the mutex.
     * 
     * @details Immediately returns success if the mutex is available for reading, or failure if
     * an exclusive writer currently holds the lock.
     * 
     * @return True if the lock was acquired successfully; false otherwise.
     * 
     * @example
     * 
     * if (mutex.TryLockRead()) {
     *     defer{ mutex.UnlockRead(); };
     * }
     * 
     * while (!mutex.TryLockRead()) {
     * }
     * 
     * defer{ mutex.UnlockRead(); };
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool TryLockRead() noexcept {

        return Platform::RWMutexTryLockRead(m_Mutex);
    }

    /**
     * @brief Read/Write unlock of the mutex.
     * @details Should only be called if the lock was previously acquired via Lock() or TryLock().
     * @example
     * 
     * {
     *     mutex.Lock() iferr_panic_only_dev("for actual implementation never return 'false', for other possible implementations can indicate poisoning or some system error");
     *     defer{ mutex.Unlock(); };
     * }
     * 
     * if (mutex.TryLock()) {
     *     defer{ mutex.Unlock(); };
     * }
     */
    ALT_FORCE_INLINE void Unlock() noexcept {

        Platform::RWMutexUnlock(m_Mutex);
    }

    /**
     * @brief Read unlock of the mutex.
     * @details Should only be called if the lock was previously acquired via LockRead() or TryLockRead().
     * @example
     * 
     * {
     *     mutex.LockRead() iferr_panic_only_dev("for actual implementation never return 'false', for other possible implementations can indicate poisoning or some system error");
     *     defer{ mutex.UnlockRead(); };
     * }
     * 
     * if (mutex.TryLockRead()) {
     *     defer{ mutex.UnlockRead(); };
     * }
     */
    ALT_FORCE_INLINE void UnlockRead() noexcept {

        Platform::RWMutexUnlockRead(m_Mutex);
    }

private:

    ALT_FORCE_INLINE RWMutex() noexcept = default;

    ALT_FORCE_INLINE RWMutex(RWMutex&&) noexcept = default;
    ALT_FORCE_INLINE RWMutex& operator=(RWMutex&&) noexcept = default;

private:

    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) Platform::RWMutex m_Mutex = Platform::RWMutex{};
};

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_RWMUTEX__