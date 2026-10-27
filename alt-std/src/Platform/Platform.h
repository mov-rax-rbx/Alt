#pragma once

#ifndef __ALT_STD_PLATFORM__
#define __ALT_STD_PLATFORM__

#include "../Defines.h"

/*
 * For manual Platform layer initialization see. 'ALT_USE_MANUAL_PLATFORM_INSTANCE_INITIALIZATION'
 */

#ifdef ALT_OS_WINDOWS

#include "Impl/Win32/PlatformWin32.h"
#else // ALT_OS_WINDOWS

#error provide implementation for specific platform
#endif

#endif // __ALT_STD_PLATFORM__