#pragma once

#ifndef __ALT_STD_LATCH__
#define __ALT_STD_LATCH__

#include "../Defines.h"

#ifdef ALT_OS_WINDOWS
#include "../Platform/Impl/Win32/PlatformWin32Latch.h"
#endif // ALT_OS_WINDOWS

namespace Alt {

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
using Latch = _PlatformDetails::Latch;

} // namespace Alt

#endif // __ALT_STD_LATCH__