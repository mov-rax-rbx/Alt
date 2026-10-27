#pragma once

#ifndef __ALT_STD_BACKOFF__
#define __ALT_STD_BACKOFF__

#include "../Defines.h"

#ifdef ALT_OS_WINDOWS
#include "../Platform/Impl/Win32/PlatformWin32Backoff.h"
#endif // ALT_OS_WINDOWS

namespace Alt {

/**
 * @brief Provides an backoff strategy for synchronization primitives.
 * @example
 * 
 * Alt::Backoff backoff{};
 * 
 * while (Platform::AtomicLoadRelaxed(&state) != State::Ready) {
 *     backoff.SpinLight();
 * }
 */
using Backoff = _PlatformDetails::Backoff;

} // namespace Alt

#endif // __ALT_STD_BACKOFF__