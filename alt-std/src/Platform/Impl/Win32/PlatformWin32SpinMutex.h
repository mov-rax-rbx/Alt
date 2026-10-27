#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_SPIN_MUTEX__
#define __ALT_STD_PLATFORM_WIN32_SPIN_MUTEX__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32.h"
#include "PlatformWin32Backoff.h"

#include "../../../Utils/NonCopyableMovable.h"

namespace Alt {
namespace _PlatformDetails {

class SpinMutex;

} // namespace _PlatformDetails

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<_PlatformDetails::SpinMutex>
    : std::false_type
{};

} // namespace TypeTraits

namespace _PlatformDetails {

/**
 * @brief A lightweight SpinLock for protecting small sections.
 * 
 * @note Prefer to use 'Mutex', but if want. Use to lock sections with small computations.
 * https://rigtorp.se/spinlock/
 * 
 * @example
 * 
 * Alt::SpinMutex mutex;
 * 
 * f32 stress = 0.0f;
 * 
 * ...
 * 
 * {
 *     mutex.Lock();
 *     defer{ mutex.Unlock(); };
 * 
 *     stress += dx;
 * }
 */
class SpinMutex {
private:

    ALT_MAKE_CLASS_NON_MOVABLE(SpinMutex);
    ALT_MAKE_CLASS_NON_COPYABLE(SpinMutex);

public:

    ALT_FORCE_INLINE SpinMutex() noexcept = default;

    /**
     * @brief Lock on the mutex.
     * @example
     * 
     * Alt::SpinMutex mutex;
     * 
     * mutex.Lock();
     * defer{ mutex.Unlock(); };
     */
    ALT_FORCE_INLINE void Lock() noexcept {

        if (_PlatformAtomicDetails::AtomicExchangeAcquire(&m_State, 1) == 0) {
            return;
        }

        Backoff backoff{};

        do {

            while (_PlatformAtomicDetails::AtomicLoadRelaxed(&m_State) == 1) {

                backoff.SpinLight();
            }

        } while (_PlatformAtomicDetails::AtomicExchangeAcquire(&m_State, 1) == 1);
    }

    /**
     * @brief Attempts lock on the mutex.
     * @return True if the lock was acquired successfully; false otherwise.
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

        if (_PlatformAtomicDetails::AtomicLoadRelaxed(&m_State) == 1) {
            return false;
        }

        return _PlatformAtomicDetails::AtomicExchangeAcquire(&m_State, 1) == 0;
    }

    /**
     * @brief Unlock of the mutex.
     * @example
     * 
     * {
     *     mutex.Lock();
     *     defer{ mutex.Unlock(); };
     * }
     * 
     * if (mutex.TryLock()) {
     *     defer{ mutex.Unlock(); };
     * }
     */
    ALT_FORCE_INLINE void Unlock() noexcept {

        _PlatformAtomicDetails::AtomicStoreRelease(&m_State, 0);
    }

private:

    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) u8 m_State = 0;
};

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_SPIN_MUTEX__