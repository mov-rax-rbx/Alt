#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_MUTEX__
#define __ALT_STD_PLATFORM_WIN32_MUTEX__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32.h"

#include "../../../Utils/Optional.h"
#include "../../../Utils/NonCopyableMovable.h"

namespace Alt {
namespace _PlatformDetails {

class Mutex;

} // namespace _PlatformDetails

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<_PlatformDetails::Mutex>
    : std::false_type
{};

} // namespace TypeTraits

namespace _PlatformDetails {

/**
 * @brief A synchronization primitive Mutex for thread safety.
 * 
 * "Futexes Are Tricky" Ulrich Drepper Red Hat, Inc.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::Mutex result = Alt::Mutex::Create() iferr_return;
 * 
 * mutex.Lock() iferr_panic_only_dev("poisoning or some system error");
 * defer{ mutex.Unlock(); };
 * 
 * // use some resource
 */
class Mutex {
private:

    ALT_MAKE_CLASS_NON_COPYABLE(Mutex);

    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(Mutex);
    ALT_IFERR_INJECT_MOVE_CONSTRUCTOR(Mutex);

public:

    /**
     * @brief Create a new instance of Mutex.
     * @return An Optional containing the Mutex instance, or Nullopt if the OS semaphore creation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Mutex mutex = Alt::Mutex::Create() iferr_return;
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<IfErrInjectionMove<Mutex>> Create() noexcept {
        iferr_scope;

        Mutex mutex{};
        mutex.m_Sem = Platform::SemaphoreCreate(0, 1) iferr_return Nullopt;

        return IfErrInjectionMove(std::move(mutex));
    }

    /**
     * @brief Lock on the mutex.
     * 
     * @details This call blocks the current thread until it can acquire exclusive ownership of the mutex.
     * 
     * @note Returns indication of successfull lock, if lock failed mean poissoned mutex - other thread
     * was subnormaly terminated and mutex still locked, this is lead to the deadlock.
     * Similar to the https://doc.rust-lang.org/std/sync/poison/struct.Mutex.html#poisoning
     * 
     * @return True if the lock was successfully acquired; false if the mutex is poisoned or a system error occurred.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Mutex mutex = Alt::Mutex::Create() iferr_return;
     * 
     * mutex.Lock() iferr_panic_only_dev("poisoning or some system error");
     * defer{ mutex.Unlock(); };
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Lock() noexcept {
        iferr_scope;

        u8 state = 0;

        if (_PlatformAtomicDetails::AtomicCompareExchangeAcquireRelaxed(&m_State, 0, 1) == 0) {
            return true;
        }

        if (state != 2) {
            state = _PlatformAtomicDetails::AtomicExchangeRelease(&m_State, 2);
        }

        while (state != 0) {

            //u8 compare = 2;
            //ALT_MAYBE_UNUSED const Platform::WaitOnAdressResult result = Platform::WaitOnAdress(&m_State, &compare, sizeof(u8), __Win32__INFINITE)
            //	iferr_return false;

            const Platform::WaitForSingleObjectResult result = Platform::WaitForSingleObject(m_Sem, __Win32__INFINITE)
                iferr_return false;

            if (result == Platform::WaitForSingleObjectResult::Abandoned) {
                // Poisoining
                return false;
            }

            state = _PlatformAtomicDetails::AtomicExchangeRelease(&m_State, 2);
        }

        return true;
    }

    /**
     * @brief Attempts lock on the mutex.
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

        return _PlatformAtomicDetails::AtomicCompareExchangeAcquireRelaxed(&m_State, 0, 1) == 0;
    }

    /**
     * @brief Unlock of the mutex.
     * @example
     * 
     * {
     *     mutex.Lock() iferr_panic_only_dev("poisoning or some system error");
     *     defer{ mutex.Unlock(); };
     * }
     * 
     * if (mutex.TryLock()) {
     *     defer{ mutex.Unlock(); };
     * }
     */
    ALT_FORCE_INLINE void Unlock() noexcept {
        iferr_scope;

        if (_PlatformAtomicDetails::AtomicDecRelease(&m_State) != 1) {

            _PlatformAtomicDetails::AtomicStoreRelaxed(&m_State, 0);

            //Platform::WakeByAddressSingle(&m_State) iferr_panic_only_dev("not supported");

            Platform::SemaphoreRelease(m_Sem, 1) iferr_panic_only_dev("Should never fail. We release only 1 and repeated releases allowed. Or invalid usage because 'm_Handle' was closed.");
        }
    }

    ~Mutex() noexcept {
        iferr_scope;

        if (m_Sem.handle == __Win32__INVALID_HANDLE_VALUE) {
            return;
        }

        Platform::SemaphoreClose(m_Sem) iferr_panic_only_dev("Just allow to fail. The system will close all handles on process exit.");
        m_Sem = Platform::Handle{};
    }

private:

    ALT_FORCE_INLINE Mutex() noexcept = default;

    ALT_FORCE_INLINE Mutex(Mutex&& rhs) noexcept
        : m_State(rhs.m_State)
        , m_Sem(Exchange(rhs.m_Sem, Platform::Handle{}))
    {}

    ALT_FORCE_INLINE Mutex& operator=(Mutex&& rhs) noexcept {

        m_State = rhs.m_State;
        m_Sem = Exchange(rhs.m_Sem, Platform::Handle{});

        return *this;
    }

private:

    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) u8 m_State = 0;
    Platform::Handle m_Sem = Platform::Handle{};
};

//class alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) Mutex {
//private:
//
//	ALT_MAKE_CLASS_NON_COPYABLE(Mutex);
//	
//	ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(Mutex);
//	ALT_IFERR_INJECT_MOVE_CONSTRUCTOR(Mutex);
//
//public:
//
//	ALT_NODISCARD ALT_FORCE_INLINE static Optional<IfErrInjectionMove<Mutex>> Create() noexcept {
//
//		Mutex mutex{};
//		mutex.m_CritSect = Platform::CriticalSectionCreate();
//		mutex.m_IsInitialized = true;
//
//		return IfErrInjectionMove<Mutex>(std::move(mutex));
//	}
//
//	ALT_FORCE_INLINE bool Lock() noexcept {
//
//		Platform::CriticalSectionEnter(m_CritSect);
//
//		return false;
//	}
//
//	ALT_NODISCARD ALT_FORCE_INLINE bool TryLock() noexcept {
//
//		return Platform::CriticalSectionTryEnter(m_CritSect);
//	}
//
//	ALT_FORCE_INLINE bool Unlock() noexcept {
//
//		Platform::CriticalSectionLeave(m_CritSect);
//
//		return true;
//	}
//
//	~Mutex() noexcept {
//
//		if (!m_IsInitialized) {
//			return;
//		}
//
//		Platform::CriticalSectionDelete(m_CritSect);
//		m_IsInitialized = false;
//	}
//
//private:
//
//	ALT_FORCE_INLINE Mutex() noexcept = default;
//
//	ALT_FORCE_INLINE Mutex(Mutex&& rhs) noexcept
//		: m_CritSect(rhs.m_CritSect)
//		, m_IsInitialized(Exchange(rhs.m_IsInitialized, false))
//	{}
//
//	ALT_FORCE_INLINE Mutex& operator=(Mutex&& rhs) noexcept {
//
//		m_CritSect = rhs.m_CritSect;
//		m_IsInitialized = Exchange(rhs.m_IsInitialized, false);
//
//		return *this;
//	}
//
//private:
//
//	Platform::CriticalSection m_CritSect = Platform::CriticalSection{};
//	bool m_IsInitialized = false;
//};

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_MUTEX__