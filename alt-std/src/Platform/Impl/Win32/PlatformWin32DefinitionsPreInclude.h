#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_DEFINITIONS_PRE_INCLUDE__
#define __ALT_STD_PLATFORM_WIN32_DEFINITIONS_PRE_INCLUDE__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

namespace Alt {
namespace _PlatformDetails {

#if defined(_WIN64) || defined(_M_ALPHA)
#define __Win32__MEMORY_ALLOCATION_ALIGNMENT 16
#else
#define __Win32__MEMORY_ALLOCATION_ALIGNMENT 8
#endif

#define __Win32__HEAP_GENERATE_EXCEPTIONS   0x00000004
#define __Win32__HEAP_NO_SERIALIZE          0x00000001
#define __Win32__HEAP_ZERO_MEMORY           0x00000008
#define __Win32__HEAP_REALLOC_IN_PLACE_ONLY 0x00000010

#define __Win32__PAGE_NOACCESS           0x01    
#define __Win32__PAGE_READONLY           0x02    
#define __Win32__PAGE_READWRITE          0x04    
#define __Win32__PAGE_WRITECOPY          0x08    
#define __Win32__MEM_COMMIT                      0x00001000  
#define __Win32__MEM_RESERVE                     0x00002000  
#define __Win32__MEM_RESET                       0x00080000  
#define __Win32__MEM_DECOMMIT                    0x00004000  
#define __Win32__MEM_RELEASE                     0x00008000  

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_DEFINITIONS_PRE_INCLUDE__