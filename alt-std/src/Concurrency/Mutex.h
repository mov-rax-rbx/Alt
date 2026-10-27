#pragma once

#ifndef __ALT_STD_MUTEX__
#define __ALT_STD_MUTEX__

#include "../Defines.h"

#ifdef ALT_OS_WINDOWS
#include "../Platform/Impl/Win32/PlatformWin32Mutex.h"
#endif // ALT_OS_WINDOWS

namespace Alt {

/**
 * @brief A synchronization primitive Mutex for thread safety.
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
using Mutex = _PlatformDetails::Mutex;

} // namespace Alt

#endif // __ALT_STD_MUTEX__