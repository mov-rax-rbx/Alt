#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_SEMAPHORE_TIMELINE__
#define __ALT_STD_PLATFORM_WIN32_SEMAPHORE_TIMELINE__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32.h"

#include "../../../Utils/Optional.h"
#include "../../../Utils/NonCopyableMovable.h"

namespace Alt {
namespace _PlatformDetails {

class SemaphoreTimeline;

} // namespace _PlatformDetails

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<_PlatformDetails::SemaphoreTimeline>
    : std::false_type
{};

} // namespace TypeTraits

namespace _PlatformDetails {

/**
 * @brief Represents a timeline semaphore for thread synchronization.
 * 
 * @details This class implements a semaphore where progress is tracked via monotonically increasing 
 * values a "timeline".
 * 
 * Similar to https://docs.vulkan.org/samples/latest/samples/extensions/timeline_semaphore/README.html
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::SemaphoreTimeline semaphore = Alt::SemaphoreTimeline::Create();
 * 
 * Alt::Thread::Allocate([&]() {
 *     iferr_scope;
 * 
 *     semaphore.Wait(0) iferr_panic_only_dev("poisoning or some system error");
 *     ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());
 * }).GetVal().Forget();
 * 
 * Alt::Thread::Allocate([&]() {
 * 
 *     semaphore.Wait(1) iferr_panic_only_dev("poisoning or some system error");
 *     ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());
 * }).GetVal().Forget();
 * 
 * // semaphore.Signal(0) iferr_panic_only_dev("for actual implementation never return 'false', for other possible implementations can be some error");
 * // semaphore.Signal(1) iferr_panic_only_dev("ignore");
 * 
 * // or just signal 2 timeline
 * semaphore.Signal(2); iferr_panic_only_dev("ignore");
 */
class SemaphoreTimeline {
private:

    ALT_MAKE_CLASS_NON_COPYABLE(SemaphoreTimeline);

    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(SemaphoreTimeline);
    ALT_IFERR_INJECT_MOVE_CONSTRUCTOR(SemaphoreTimeline);

public:

    /**
     * @brief Returns the maximum timeline length. This value need to control overflow behaviour.
     * @return The timeline length.
     * @example
     * 
     * const u64 maxLen = Alt::SemaphoreTimeline::GetTimelineLength();
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static u64 GetTimelineLength() noexcept {
        return static_cast<u64>(Num::Desc<u32>::Max());
    }

    /**
     * @brief Create a new instance of SemaphoreTimeline.
     * @return An Optional containing the SemaphoreTimeline instance, or Nullopt if the OS version is incompatible.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::SemaphoreTimeline sem = Alt::SemaphoreTimeline::Create() iferr_return;
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<IfErrInjectionMove<SemaphoreTimeline>> Create(const u64 initVal = Num::Desc<u64>::Max()) noexcept {

        /*
         * >= Windows 8 or >= Windows Server 2012
         */
        if (!Platform::IsWaitWakeOnAdressSupported()) {
            return Nullopt;
        }

        SemaphoreTimeline sem{};
        sem.m_Value = initVal;

        return IfErrInjectionMove(std::move(sem));
    }

    /**
     * @brief Blocks the current thread until the semaphore is signaled. If value alraddy in
     * timeline '<' - return without waiting.
     * 
     * @param value The target point in the timeline to wait for.
     * @return True if the wait was successful and the semaphore was signaled; false otherwise.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::SemaphoreTimeline sem = Alt::SemaphoreTimeline::Create() iferr_return;
     * sem.Wait(4) iferr_panic_only_dev("poisoning or some system error");
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Wait(const u64 value) const noexcept {
        iferr_scope;

        u64 valueSignaled = _PlatformAtomicDetails::AtomicLoadAcquire(&m_Value);

        /*
         * Check if we in Timeline Range - already signaled
         */
        if (value + GetTimelineLength() >= valueSignaled && value <= valueSignaled) {
            return true;
        }

        for (;;) {

            ALT_MAYBE_UNUSED const Platform::WaitOnAdressResult result = Platform::WaitOnAdress(&m_Value, &valueSignaled, sizeof(u64), __Win32__INFINITE)
                iferr_return false;

            /*
             * relaxed because after 'WaitOnAdress(...)'
             */
            valueSignaled = _PlatformAtomicDetails::AtomicLoadRelaxed(&m_Value);

            /*
             * Check if we in Timeline Range - so our wake
             */
            if (value + GetTimelineLength() >= valueSignaled && value <= valueSignaled) {
                return true;
            }
        }
    }

    /**
     * @brief Signals the semaphore before value timeline. If value alreaddy in
     * timeline '<' - return immediately without signaling.
     * 
     * @param value The target point in the timeline to wait for.
     * @return True if the signal operation was successful; false otherwise.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::SemaphoreTimeline sem = Alt::SemaphoreTimeline::Create();
     * 
     * Alt::Thread::Allocate([&]() {
     *     iferr_scope;
     * 
     *     sem.Wait(4) iferr_panic_only_dev("poisoning or some system error");
     * 
     *     ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());
     * 
     * }).GetVal().Forget();
     * 
     * sem.Signal(4) iferr_panic_only_dev("for actual implementation never return 'false', for other possible implementations can be some error");
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Signal(const u64 value) noexcept {
        iferr_scope;

        u64 valueOld0 = _PlatformAtomicDetails::AtomicLoadAcquire(&m_Value);

        /*
         * Signal value which have been signaled - just repeat
         */
        if (valueOld0 == value) {

            Platform::WakeByAddressAll(&m_Value) iferr_panic_only_dev("not supported; never panic, because handled on 'Create(...)'");
            return true;
        }

        /*
         * Check if we in Timeline Range - just skip already signaled
         */
        if (value + GetTimelineLength() >= valueOld0 && value < valueOld0) {
            return true;
        }

        for (;;) {

            /*
             * Try to update value
             */
            const u64 valueOld1 = _PlatformAtomicDetails::AtomicCompareExchangeAcqRelRelaxed(&m_Value, valueOld0, value);

            /*
             * Successfuly updated - just wake all
             */
            if (valueOld0 == valueOld1) {

                Platform::WakeByAddressAll(&m_Value) iferr_panic_only_dev("not supported; never panic, because handled on 'Create(...)'");
                return true;
            }

            /*
             * Check if we in Timeline Range - we already have signaled by some other call
             */
            if (value + GetTimelineLength() >= valueOld1 && value < valueOld1) {
                return true;
            }

            /*
             * Try continue update to max timeline value
             */
            valueOld0 = valueOld1;
        }
    }

private:

    ALT_FORCE_INLINE SemaphoreTimeline() noexcept = default;

    ALT_FORCE_INLINE SemaphoreTimeline(SemaphoreTimeline&&) noexcept = default;
    ALT_FORCE_INLINE SemaphoreTimeline& operator=(SemaphoreTimeline&&) noexcept = default;

private:

    alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) u64 m_Value = Num::Desc<u64>::Max();
};

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_SEMAPHORE_TIMELINE__