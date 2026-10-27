#pragma once

#ifndef __ALT_STD_RWMUTEX__
#define __ALT_STD_RWMUTEX__

#include "../Defines.h"

#ifdef ALT_OS_WINDOWS
#include "../Platform/Impl/Win32/PlatformWin32RWMutex.h"
#endif // ALT_OS_WINDOWS

namespace Alt {


/**
 * @brief Read/Write Mutex.
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::RWMutex mutex = Alt::RWMutex::Create() iferr_return;
 * 
 * {
 *     mutex.Lock() iferr_panic("poisoning or some system error");
 *     defer{ mutex.Unlock(); };
 * 
 *     // mutate some resource
 * }
 * 
 * {
 *     mutex.LockRead() iferr_panic("poisoning or some system error");
 *     defer{ mutex.UnlockRead(); };
 * 
 *     // read from some resource
 * }
 */
using RWMutex = _PlatformDetails::RWMutex;

} // namespace Alt

#endif // __ALT_STD_RWMUTEX__