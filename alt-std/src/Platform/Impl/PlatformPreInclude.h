#pragma once

#ifndef __ALT_STD_PLATFORM_PREINCLUDE__
#define __ALT_STD_PLATFORM_PREINCLUDE__

#include "../../TypeTraits.h"

#include "PlatformPanicPreInclude.h"

#include "../../Utils/EnumClassBitFlagsImpl.h"

#ifdef ALT_OS_WINDOWS
#include "Win32/PlatformWin32DefinitionsPreInclude.h"
#else
#error provide implementation for specific platform
#endif

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundefined-inline"
#endif // ALT_COMPILER_CLANG

namespace Alt {
namespace Platform {

#ifdef ALT_OS_WINDOWS

enum class MemoryAllocateBytesBits : usize {

    None = 0,

    FillZero = __Win32__HEAP_ZERO_MEMORY,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(MemoryAllocateBytesBits);

enum class MemoryReallocateBytesBits : usize {

    None = 0,

    FillZero = __Win32__HEAP_ZERO_MEMORY,
    InPlace = __Win32__HEAP_REALLOC_IN_PLACE_ONLY,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(MemoryReallocateBytesBits);

enum class VirtualMemoryAllocateBytesBits : usize {

    Commit = __Win32__MEM_COMMIT,
    Reserve = __Win32__MEM_RESERVE,

    Reset = __Win32__MEM_RESET,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(VirtualMemoryAllocateBytesBits);

enum class VirtualMemoryAccessBits : usize {

    None = __Win32__PAGE_NOACCESS,

    Read = __Win32__PAGE_READONLY,
    ReadWrite = __Win32__PAGE_READWRITE,

    CopyOnWrite = __Win32__PAGE_WRITECOPY,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(VirtualMemoryAccessBits);

enum class VirtualMemoryDeallocateBytesBits : usize {

    Decommit = __Win32__MEM_DECOMMIT,
    Release = __Win32__MEM_RELEASE,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(VirtualMemoryDeallocateBytesBits);

#else

#error provide implementation for specific platform
#endif

/**
 * @brief Returns true if the current system's byte order is little-endian.
 */
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsLittleEndian() noexcept {

#if defined(ALT_PLATFORM_LITTLE_ENDIAN)

    return true;
#elif defined(ALT_PLATFORM_BIG_ENDIAN)

    return false;
#else
    static_assert(false, "");
#endif
}

/**
 * @brief Returns the default alignment for memory allocations on the current platform.
 */
ALT_NODISCARD ALT_FORCE_INLINE constexpr usize MemoryDefaultAllocationAlignment() noexcept {

#ifdef ALT_OS_WINDOWS
    return __Win32__MEMORY_ALLOCATION_ALIGNMENT;
#else
    #error provide implementation for specific platform
#endif
}

/**
 * @brief Returns some address for seed.
 */
ALT_NODISCARD ALT_FORCE_INLINE uptr GetSeedAddress() noexcept;

/**
 * @brief Provides the address of the instruction in the calling function that will be executed after control returns to the caller.
 */
ALT_NODISCARD ALT_FORCE_INLINE uptr GetReturnAddress() noexcept;

/**
 * @brief Returns number of bytes per page for Virtual Memory.
 */
ALT_NODISCARD ALT_FORCE_INLINE usize GetPageSizeInBytes() noexcept;

/**
 * @brief Same as 'Alt::Num::RoundUpWrap(size, Alt::Platform::GetPageSizeInBytes())', but optimized.
 */
ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundUpToPageSizeWrap(u32 sizeInBytes) noexcept;
ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundUpToPageSizeWrap(u32 sizeInBytes, const u32 wrap) noexcept;

ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundUpToPageSizeWrap(u64 sizeInBytes) noexcept;
ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundUpToPageSizeWrap(u64 sizeInBytes, const u64 wrap) noexcept;

/**
 * @brief Same as 'Alt::Num::RoundUpWrapPreserveZero(size, Alt::Platform::GetPageSizeInBytes())', but optimized.
 */
ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundUpToPageSizeWrapPreserveZero(const u32 sizeInBytes) noexcept;
ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundUpToPageSizeWrapPreserveZero(const u32 sizeInBytes, const u32 wrap) noexcept;

ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundUpToPageSizeWrapPreserveZero(const u64 sizeInBytes) noexcept;
ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundUpToPageSizeWrapPreserveZero(const u64 sizeInBytes, const u64 wrap) noexcept;

/**
 * @brief Same as 'Alt::Num::RoundDownTo(size, Alt::Platform::GetPageSizeInBytes())', but optimized.
 */
ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundDownToPageSize(const u32 sizeInBytes) noexcept;
ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundDownToPageSize(const u64 sizeInBytes) noexcept;

/**
 * @brief Allocates a block of memory of the specified size.
 */
ALT_NODISCARD ALT_FORCE_INLINE u8* MemoryAllocateBytes(const usize sizeInBytes, const MemoryAllocateBytesBits bits) noexcept;

/**
 * @brief Reallocates a block of memory of the specified size.
 */
ALT_NODISCARD ALT_FORCE_INLINE u8* MemoryReallocateBytes(void* pMemoryOld, const usize sizeInBytes, const MemoryReallocateBytesBits bits) noexcept;

/**
 * @brief Deallocates a allocated block of memory by 'MemoryAllocateBytes(...)'.
 */
ALT_FORCE_INLINE void MemoryDeallocateBytes(void* pMemory) noexcept;

/**
 * @brief Allocates a block of memory with specific alignment constraints.
 */
ALT_NODISCARD ALT_FORCE_INLINE u8* MemoryAlignedAllocateBytes(const usize sizeInBytes, const usize alignment, const MemoryAllocateBytesBits bits) noexcept;

/**
 * @brief Deallocates a allocated block of memory by 'MemoryAlignedAllocateBytes(...)'.
 */
ALT_FORCE_INLINE void MemoryAlignedDeallocateBytes(void* pMemory) noexcept;

/**
 * @brief Allocates a page of the specified size.
 */
ALT_NODISCARD ALT_FORCE_INLINE u8* VirtualMemoryAllocateBytes(void* pMemory, const usize sizeInBytes, const Platform::VirtualMemoryAllocateBytesBits bits, const Platform::VirtualMemoryAccessBits accessBits) noexcept;

/**
 * @brief Deallocates a allocated page of memory by 'VirtualMemoryDeallocateBytes(...)'.
 */
ALT_FORCE_INLINE void VirtualMemoryDeallocateBytes(void* pMemory, const usize sizeInBytes, const Platform::VirtualMemoryDeallocateBytesBits bits) noexcept;

/*
 * @brief Return numbers of ticks since first initialization of `GetTicks...()` call
 */
ALT_NODISCARD ALT_FORCE_INLINE u64 GetTicks() noexcept;

} // namespace Platform
} // namespace Alt

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic pop
#endif // ALT_COMPILER_CLANG

#endif // __ALT_STD_PLATFORM_PREINCLUDE__