#pragma once

#ifndef __ALT_STD_PLATFORM_PANIC_PRE_INCLUDE__
#define __ALT_STD_PLATFORM_PANIC_PRE_INCLUDE__

// POSIX EXIT_SUCCESS and EXIT_FAILURE
// WINDOWS ERROR_SUCCESS and ERROR_INVALID_FUNCTION
#define ALT_EXIT_CODE_SUCCESS 0
#define ALT_EXIT_CODE_FAILURE 1

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-inline"
#endif // ALT_COMPILER_CLANG

namespace Alt {
namespace _PlatformDetails {

template <typename ...ArgsT>
[[noreturn]] inline void Panic_(const char* fmt, const ArgsT&... args) noexcept;

template <typename T, typename ...ArgsT>
inline void EPrintln_(T&& fmt, const ArgsT&... args) noexcept;

} // namespace _PlatformDetails

namespace Platform {

[[noreturn]] ALT_FORCE_INLINE void FastCrash() noexcept;

} // namespace Platform
} // namespace Alt

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic pop
#endif // ALT_COMPILER_CLANG

#endif // __ALT_STD_PLATFORM_PANIC_PRE_INCLUDE__