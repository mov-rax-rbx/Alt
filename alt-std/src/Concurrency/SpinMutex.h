#pragma once

#ifndef __ALT_STD_SPIN_MUTEX__
#define __ALT_STD_SPIN_MUTEX__

#include "../Defines.h"

#ifdef ALT_OS_WINDOWS
#include "../Platform/Impl/Win32/PlatformWin32SpinMutex.h"
#endif // ALT_OS_WINDOWS

namespace Alt {

/**
 * @brief A SpinLock for protecting small sections.
 * @note Prefer to use 'Mutex', but if want. Use to lock sections with small computations.
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
using SpinMutex = _PlatformDetails::SpinMutex;

} // namespace Alt

#endif // __ALT_STD_SPIN_MUTEX__