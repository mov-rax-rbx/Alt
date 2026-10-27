#pragma once

#ifndef __ALT_STD_SEMAPHORE__
#define __ALT_STD_SEMAPHORE__

#include "../Defines.h"

#ifdef ALT_OS_WINDOWS
#include "../Platform/Impl/Win32/PlatformWin32Semaphore.h"
#endif // ALT_OS_WINDOWS

namespace Alt {

/**
 * @brief Represents a semaphore for thread synchronization.
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
using Semaphore = _PlatformDetails::Semaphore;

} // namespace Alt

#endif // __ALT_STD_SEMAPHORE__