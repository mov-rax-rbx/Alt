#pragma once

#ifndef __ALT_STD_SEMAPHORE_TIMELINE__
#define __ALT_STD_SEMAPHORE_TIMELINE__

#include "../Defines.h"

#ifdef ALT_OS_WINDOWS
#include "../Platform/Impl/Win32/PlatformWin32SemaphoreTimeline.h"
#endif // ALT_OS_WINDOWS

namespace Alt {

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
using SemaphoreTimeline = _PlatformDetails::SemaphoreTimeline;

} // namespace Alt

#endif // __ALT_STD_SEMAPHORE_TIMELINE__