#pragma once

#ifndef __ALT_STD_THREAD__
#define __ALT_STD_THREAD__

#include "../Defines.h"

#ifdef ALT_OS_WINDOWS
#include "../Platform/Impl/Win32/PlatformWin32Thread.h"
#endif // ALT_OS_WINDOWS

namespace Alt {

/**
 * @brief Represents and provides controls for OS thread.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::Thread thread = Alt::Thread::Create([]() noexcept {
 * 
 *     ALT_PRINTLN("Hello from another thread");
 * }) iferr_return;
 * 
 * thread.WaitCompletion() iferr_return;
 */
using Thread = _PlatformDetails::Thread;

} // namespace Alt

#endif // __ALT_STD_THREAD__