#pragma once

#ifndef __ALT_STD_DEFINES__
#define __ALT_STD_DEFINES__

/*
 * 1. Available customization by actual defines:
 * 
 *    - `ALT_USE_MANUAL_PLATFORM_INSTANCE_INITIALIZATION` - set pointer for 'PlatformInstance' manually (without lazy static initialization stuff)
 *    - `ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE` - use scalar types like 'usize', 'ssize', 'f32', ... in global namespace (without 'Alt::usize', 'Alt::ssize', 'Alt::f32', ...)
 *    - `ALT_USE_MATH_TYPES_IN_GLOBAL_NAMESPACE` - use math types like 'Vec3f32', 'Matrix44f32', 'Quatf32', ... in global namespace (without 'Alt::Vec3f32', 'Alt::Matrix44f32', 'Alt::Quatf32', ...)
 *    - `ALT_USE_CUSTOM_FAST_MATH` - use approximations for some math function
 *    - `ALT_USE_UTF_NON_BRANCHLESS` - use non-branchless Utf encoding/decoding (by default branchless)
 *    - `ALT_USE_FORMAT_WITHOUT_SIZE_ESTIMATION` - disables size estimation and reservation for formatting (has 2 stages: calculate estimated size and reserve; write to buffer;)
 * 
 * 2. Helper defines:
 * 
 *    - ALT_COMPILER_XXXX - compiler
 *    - ALT_OS_XXXX - operating system
 *    - ALT_PLATFORM_XXXX - platform
 *    - ALT_DEV - development build
 *    - ALT_DEV_BOOL - same as 'ALT_DEV' but contains 'bool' value
 * 
 *    - ALT_NODISCARD
 *    - ALT_FORCE_INLINE
 *    - ALT_NOINLINE
 *    - ALT_MAYBE_UNUSED
 *    - ALT_FALLTHROUGH
 */

#if defined(_MSC_VER) && !defined(__clang__)
    #define ALT_COMPILER_MSVC
#elif defined(__clang__)
    #define ALT_COMPILER_CLANG
#elif defined(__GNUC__)
    #define ALT_COMPILER_GCC
#else
    #error undefined compiler
#endif

#if defined(_WIN32)
    #define ALT_OS_WINDOWS
#else
    #error undefined OS
#endif // _WIN32

#if defined(_M_X64) || defined(__x86_64__)
    #define ALT_PLATFORM_X86
    #define ALT_PLATFORM_64BIT
    #define ALT_PLATFORM_X86_64
#elif defined(_M_IX86) || defined(__i386__)
    #define ALT_PLATFORM_X86
    #define ALT_PLATFORM_32BIT
    #define ALT_PLATFORM_X86_32
#else
    #error undefined platform
#endif

#ifdef ALT_PLATFORM_32BIT
    #define ALT_PLATFORM_32BIT_BOOL true
#else
    #define ALT_PLATFORM_32BIT_BOOL false
#endif // ALT_PLATFORM_32BIT

#ifdef ALT_PLATFORM_64BIT
    #define ALT_PLATFORM_64BIT_BOOL true
#else
    #define ALT_PLATFORM_64BIT_BOOL false
#endif // ALT_PLATFORM_64BIT

#ifdef ALT_PLATFORM_X86
    #ifndef ALT_PLATFORM_LITTLE_ENDIAN
        #define ALT_PLATFORM_LITTLE_ENDIAN
    #endif // ALT_PLATFORM_LITTLE_ENDIAN
#else
    #error provide implementation for specific platform
#endif // ALT_PLATFORM_X86

#ifdef ALT_PLATFORM_LITTLE_ENDIAN
    #define ALT_PLATFORM_LITTLE_ENDIAN_BOOL true
#else
    #define ALT_PLATFORM_LITTLE_ENDIAN_BOOL false
#endif

#ifdef ALT_PLATFORM_BIG_ENDIAN
    #define ALT_PLATFORM_BIG_ENDIAN_BOOL true
#else
    #define ALT_PLATFORM_BIG_ENDIAN_BOOL false
#endif

#ifdef ALT_PLATFORM_X86
    // https://cppreference.com/cpp/thread/hardware_destructive_interference_size
    #define ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD 64
#else
    #error provide implementation for specific platform
#endif // ALT_PLATFORM_X86

#ifndef ALT_DEV
#ifndef NDEBUG
    #define ALT_DEV
#endif // NDEBUG
#endif // ALT_DEV

#ifdef ALT_DEV
    #define ALT_DEV_BOOL true
#else
    #define ALT_DEV_BOOL false
#endif // ALT_DEV

#ifndef ALT_NODISCARD

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    #define ALT_NODISCARD [[nodiscard]]
#else
    #if __cplusplus >= 201703L
        #define ALT_NODISCARD [[nodiscard]]
    #else
        #define ALT_NODISCARD
    #endif
#endif

#endif // ALT_NODISCARD

#ifndef ALT_FORCE_INLINE

#if defined(ALT_COMPILER_MSVC)
    #define ALT_FORCE_INLINE __forceinline
#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)
    #define ALT_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define ALT_FORCE_INLINE inline
#endif

#endif // ALT_FORCE_INLINE

#ifndef ALT_NOINLINE

#if defined(ALT_COMPILER_MSVC)
    #define ALT_NOINLINE __declspec(noinline)
#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)
    #define ALT_NOINLINE __attribute__((noinline))
#else
    #define ALT_NOINLINE
#endif

#endif // ALT_NOINLINE

#ifndef ALT_MAYBE_UNUSED
    #define ALT_MAYBE_UNUSED [[maybe_unused]]
#endif // ALT_MAYBE_UNUSED

#ifndef ALT_FALLTHROUGH
    #define ALT_FALLTHROUGH [[fallthrough]]
#endif // ALT_FALLTHROUGH

#endif // __ALT_STD_DEFINES__