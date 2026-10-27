#pragma once

#ifndef __ALT_STD_PLATFORM_ATOMICS128__
#define __ALT_STD_PLATFORM_ATOMICS128__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32AtomicsCore.h"

/*
 * @brief Atomic operations for u128/s128
 */
namespace Alt {
namespace _PlatformAtomicDetails {

#if defined(ALT_PLATFORM_X86_64)

/*
 * https://learn.microsoft.com/en-us/cpp/intrinsics/interlockedcompareexchange128?view=msvc-170
 * 
 * The _InterlockedCompareExchange128 intrinsic generates the cmpxchg16b instruction (with the lock prefix) to perform a 128-bit
 * locked compare and exchange. Early versions of AMD 64-bit hardware don't support this instruction. To check for hardware support
 * for the cmpxchg16b instruction, call the __cpuid intrinsic with InfoType=0x00000001 (standard function 1). Bit 13 of CPUInfo[2]
 * (ECX) is 1 if the instruction is supported.
 * 
 * If you run code that uses the intrinsic on hardware that doesn't support the cmpxchg16b instruction, the results are UNPREDICTABLE.
 * 
 * clang - add_compile_options(-mcx16)
 */

ALT_NODISCARD ALT_FORCE_INLINE bool IsAtomic128Supported() noexcept {

    int cpuInfo[4];
    __cpuid(cpuInfo, 0x00000001);

    // cmpxchg16b
    return ((cpuInfo[2] >> 13) & 0b1) == 1;
}

constexpr static usize _128_ALIGN_OF = 16;

/*
 * TODO: This will probably change in future
 */
struct alignas(_128_ALIGN_OF) u128 {

    u64 low;
    u64 high;

    ALT_NODISCARD ALT_FORCE_INLINE bool operator==(const u128& rhs) const noexcept { return low == rhs.low && high == rhs.high; }
};

struct alignas(_128_ALIGN_OF) s128 {

    s64 low;
    s64 high;

    ALT_NODISCARD ALT_FORCE_INLINE bool operator==(const s128& rhs) const noexcept { return low == rhs.low && high == rhs.high; }
};

/*
 * @brief Atomic load for u128
 */
ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicLoadSeqCst(const u128* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(const_cast<u128*>(pAtomic_));

    u128 compare{ 0, 0 };
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    _InterlockedCompareExchange128(pAtomic, 0, 0, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicLoadAcquire(const u128* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(const_cast<u128*>(pAtomic_));

    u128 compare{ 0, 0 };
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    _InterlockedCompareExchange128(pAtomic, 0, 0, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicLoadRelaxed(const u128* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(const_cast<u128*>(pAtomic_));

    u128 compare{ 0, 0 };
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    _InterlockedCompareExchange128(pAtomic, 0, 0, pCompare);

    return compare;
}

/*
 * @brief Atomic CAS for u128
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeSeqCstSeqCst(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeSeqCstAcquire(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeSeqCstRelaxed(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeAcqRelSeqCst(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeAcqRelAcquire(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeAcqRelRelaxed(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeAcquireSeqCst(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeAcquireAcquire(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeAcquireRelaxed(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeReleaseSeqCst(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeReleaseAcquire(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeReleaseRelaxed(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeRelaxedSeqCst(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeRelaxedAcquire(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE u128 AtomicCompareExchangeRelaxedRelaxed(u128* pAtomic_, const u128& value_, const u128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    u128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<u64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<u64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

/*
 * @brief Atomic load for s128
 */
ALT_FORCE_INLINE s128 AtomicLoadSeqCst(const s128* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(const_cast<s128*>(pAtomic_));

    s128 compare{ 0, 0 };
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    _InterlockedCompareExchange128(pAtomic, 0, 0, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicLoadAcquire(const s128* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(const_cast<s128*>(pAtomic_));

    s128 compare{ 0, 0 };
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    _InterlockedCompareExchange128(pAtomic, 0, 0, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicLoadRelaxed(const s128* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(const_cast<s128*>(pAtomic_));

    s128 compare{ 0, 0 };
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    _InterlockedCompareExchange128(pAtomic, 0, 0, pCompare);

    return compare;
}

/*
 * @brief Atomic CAS for s128
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeSeqCstSeqCst(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeSeqCstAcquire(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeSeqCstRelaxed(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeAcqRelSeqCst(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeAcqRelAcquire(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeAcqRelRelaxed(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeAcquireSeqCst(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeAcquireAcquire(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeAcquireRelaxed(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeReleaseSeqCst(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeReleaseAcquire(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeReleaseRelaxed(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare;
    compare.low = compare_.low;
    compare.high = compare_.high;

    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeRelaxedSeqCst(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeRelaxedAcquire(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

ALT_NODISCARD ALT_FORCE_INLINE s128 AtomicCompareExchangeRelaxedRelaxed(s128* pAtomic_, const s128& value_, const s128& compare_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, _128_ALIGN_OF) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    s128 compare = compare_;
    __int64* pCompare = reinterpret_cast<__int64*>(&compare.low);

    const __int64 low = Memory::Transmute<s64, __int64>(value_.low);
    const __int64 hight = Memory::Transmute<s64, __int64>(value_.high);

    _InterlockedCompareExchange128(pAtomic, hight, low, pCompare);

    return compare;
}

#elif defined(ALT_PLATFORM_X86)

#else

    #error provide implementation for specific platform

#endif

} // namespace _PlatformAtomicDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_ATOMICS128__