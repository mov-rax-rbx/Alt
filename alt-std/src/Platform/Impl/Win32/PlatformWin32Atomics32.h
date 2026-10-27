#pragma once

#ifndef __ALT_STD_PLATFORM_ATOMICS32__
#define __ALT_STD_PLATFORM_ATOMICS32__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32AtomicsCore.h"

/*
 * @brief Atomic operations for u32/s32/f32
 */
namespace Alt {
namespace _PlatformAtomicDetails {

#ifdef ALT_PLATFORM_X86

template <>
struct __supported_operations__<u32>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

template <>
struct __supported_operations__<s32>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

template <>
struct __supported_operations__<f32>
    : SupportedOperations<
          SupportedOperationsTrait::Store
        | SupportedOperationsTrait::Load
        | SupportedOperationsTrait::Exchange
        | SupportedOperationsTrait::CompareExchange
    >
{};

/*
 * @brief Atomic store for u32
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    _InterlockedExchange(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    __int32 volatile* pAtomic = reinterpret_cast<__int32 volatile*>(pAtomic_);
    const __int32 value = Memory::Transmute<u32, __int32>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store32(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    __int32 volatile* pAtomic = reinterpret_cast<__int32 volatile*>(pAtomic_);
    const __int32 value = Memory::Transmute<u32, __int32>(value_);

    __iso_volatile_store32(pAtomic, value);
}

/*
 * @brief Atomic load for u32
 */
ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicLoadSeqCst(const u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    const __int32 volatile* pAtomic = reinterpret_cast<const __int32 volatile*>(pAtomic_);

    const __int32 result = __iso_volatile_load32(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int32, u32>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicLoadAcquire(const u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    const __int32 volatile* pAtomic = reinterpret_cast<const __int32 volatile*>(pAtomic_);

    const __int32 result = __iso_volatile_load32(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int32, u32>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicLoadRelaxed(const u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    const __int32 volatile* pAtomic = reinterpret_cast<const __int32 volatile*>(pAtomic_);

    const __int32 result = __iso_volatile_load32(pAtomic);

    return Memory::Transmute<__int32, u32>(result);
}

/*
 * @brief Atomic exchange for u32
 */
ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicExchangeSeqCst(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicExchangeAcqRel(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicExchangeAcquire(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicExchangeRelease(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicExchangeRelaxed(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchange(pAtomic, value));
}

/*
 * @brief Atomic CAS for u32
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeSeqCstSeqCst(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeSeqCstAcquire(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeSeqCstRelaxed(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeAcqRelSeqCst(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeAcqRelAcquire(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeAcqRelRelaxed(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeAcquireSeqCst(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeAcquireAcquire(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeAcquireRelaxed(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeReleaseSeqCst(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeReleaseAcquire(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeReleaseRelaxed(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeRelaxedSeqCst(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeRelaxedAcquire(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u32 AtomicCompareExchangeRelaxedRelaxed(u32* pAtomic_, const u32 compare_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);
    const long compare = Memory::Transmute<u32, long>(compare_);

    return Memory::Transmute<long, u32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for u32
 */
ALT_FORCE_INLINE u32 AtomicAddSeqCst(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicAddAcqRel(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicAddAcquire(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicAddRelease(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicAddRelaxed(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

/*
 * @brief Atomic subtraction for u32
 */
ALT_FORCE_INLINE u32 AtomicSubSeqCst(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(Negate(value_));

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicSubAcqRel(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(Negate(value_));

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicSubAcquire(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(Negate(value_));

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicSubRelease(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(Negate(value_));

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicSubRelaxed(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(Negate(value_));

    return Memory::Transmute<long, u32>(_InterlockedExchangeAdd(pAtomic, value));
}

/*
 * @brief Atomic increment for u32
 */
ALT_FORCE_INLINE u32 AtomicIncSeqCst(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedIncrement(pAtomic)) - 1;
}

ALT_FORCE_INLINE u32 AtomicIncAcqRel(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedIncrement(pAtomic)) - 1;
}

ALT_FORCE_INLINE u32 AtomicIncAcquire(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedIncrement(pAtomic)) - 1;
}

ALT_FORCE_INLINE u32 AtomicIncRelease(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedIncrement(pAtomic)) - 1;
}

ALT_FORCE_INLINE u32 AtomicIncRelaxed(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedIncrement(pAtomic)) - 1;
}

/*
 * @brief Atomic decrement for u32
 */
ALT_FORCE_INLINE u32 AtomicDecSeqCst(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedDecrement(pAtomic)) + 1;
}

ALT_FORCE_INLINE u32 AtomicDecAcqRel(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedDecrement(pAtomic)) + 1;
}

ALT_FORCE_INLINE u32 AtomicDecAcquire(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedDecrement(pAtomic)) + 1;
}

ALT_FORCE_INLINE u32 AtomicDecRelease(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedDecrement(pAtomic)) + 1;
}

ALT_FORCE_INLINE u32 AtomicDecRelaxed(u32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, u32>(_InterlockedDecrement(pAtomic)) + 1;
}

/*
 * @brief Atomic && for u32
 */
ALT_FORCE_INLINE u32 AtomicAndSeqCst(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedAnd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicAndAcqRel(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedAnd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicAndAcquire(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedAnd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicAndRelease(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedAnd(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicAndRelaxed(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedAnd(pAtomic, value));
}

/*
 * @brief Atomic || for u32
 */
ALT_FORCE_INLINE u32 AtomicOrSeqCst(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedOr(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicOrAcqRel(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedOr(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicOrAcquire(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedOr(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicOrRelease(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedOr(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicOrRelaxed(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedOr(pAtomic, value));
}

/*
 * @brief Atomic ^ for u32
 */
ALT_FORCE_INLINE u32 AtomicXorSeqCst(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedXor(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicXorAcqRel(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedXor(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicXorAcquire(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedXor(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicXorRelease(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedXor(pAtomic, value));
}

ALT_FORCE_INLINE u32 AtomicXorRelaxed(u32* pAtomic_, const u32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<u32, long>(value_);

    return Memory::Transmute<long, u32>(_InterlockedXor(pAtomic, value));
}

/*
 * @brief Atomic store for s32
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    _InterlockedExchange(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    __int32 volatile* pAtomic = reinterpret_cast<__int32 volatile*>(pAtomic_);
    const __int32 value = static_cast<__int32>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store32(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    __int32 volatile* pAtomic = reinterpret_cast<__int32 volatile*>(pAtomic_);
    const __int32 value = static_cast<__int32>(value_);

    __iso_volatile_store32(pAtomic, value);
}

/*
 * @brief Atomic load for s32
 */
ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicLoadSeqCst(const s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    const __int32 volatile* pAtomic = reinterpret_cast<const __int32 volatile*>(pAtomic_);

    const __int32 result = __iso_volatile_load32(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int32, s32>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicLoadAcquire(const s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    const __int32 volatile* pAtomic = reinterpret_cast<const __int32 volatile*>(pAtomic_);

    const __int32 result = __iso_volatile_load32(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int32, s32>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicLoadRelaxed(const s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    const __int32 volatile* pAtomic = reinterpret_cast<const __int32 volatile*>(pAtomic_);

    const __int32 result = __iso_volatile_load32(pAtomic);

    return Memory::Transmute<__int32, s32>(result);
}

/*
 * @brief Atomic exchange for s32
 */
ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicExchangeSeqCst(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicExchangeAcqRel(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicExchangeAcquire(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicExchangeRelease(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicExchangeRelaxed(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchange(pAtomic, value));
}

/*
 * @brief Atomic CAS for s32
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeSeqCstSeqCst(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeSeqCstAcquire(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeSeqCstRelaxed(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeAcqRelSeqCst(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeAcqRelAcquire(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeAcqRelRelaxed(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeAcquireSeqCst(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeAcquireAcquire(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeAcquireRelaxed(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeReleaseSeqCst(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeReleaseAcquire(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeReleaseRelaxed(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeRelaxedSeqCst(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeRelaxedAcquire(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s32 AtomicCompareExchangeRelaxedRelaxed(s32* pAtomic_, const s32 compare_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);
    const long compare = Memory::Transmute<s32, long>(compare_);

    return Memory::Transmute<long, s32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for s32
 */
ALT_FORCE_INLINE s32 AtomicAddSeqCst(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicAddAcqRel(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicAddAcquire(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicAddRelease(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicAddRelaxed(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

/*
 * @brief Atomic subtraction for s32
 */
ALT_FORCE_INLINE s32 AtomicSubSeqCst(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(Negate(value_));

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicSubAcqRel(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(Negate(value_));

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicSubAcquire(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(Negate(value_));

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicSubRelease(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(Negate(value_));

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicSubRelaxed(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(Negate(value_));

    return Memory::Transmute<long, s32>(_InterlockedExchangeAdd(pAtomic, value));
}

/*
 * @brief Atomic increment for s32
 */
ALT_FORCE_INLINE s32 AtomicIncSeqCst(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedIncrement(pAtomic)) - 1;
}

ALT_FORCE_INLINE s32 AtomicIncAcqRel(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedIncrement(pAtomic)) - 1;
}

ALT_FORCE_INLINE s32 AtomicIncAcquire(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedIncrement(pAtomic)) - 1;
}

ALT_FORCE_INLINE s32 AtomicIncRelease(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedIncrement(pAtomic)) - 1;
}

ALT_FORCE_INLINE s32 AtomicIncRelaxed(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedIncrement(pAtomic)) - 1;
}

/*
 * @brief Atomic decrement for s32
 */
ALT_FORCE_INLINE s32 AtomicDecSeqCst(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedDecrement(pAtomic)) + 1;
}

ALT_FORCE_INLINE s32 AtomicDecAcqRel(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedDecrement(pAtomic)) + 1;
}

ALT_FORCE_INLINE s32 AtomicDecAcquire(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedDecrement(pAtomic)) + 1;
}

ALT_FORCE_INLINE s32 AtomicDecRelease(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedDecrement(pAtomic)) + 1;
}

ALT_FORCE_INLINE s32 AtomicDecRelaxed(s32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);

    return Memory::Transmute<long, s32>(_InterlockedDecrement(pAtomic)) + 1;
}

/*
 * @brief Atomic && for s32
 */
ALT_FORCE_INLINE s32 AtomicAndSeqCst(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedAnd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicAndAcqRel(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedAnd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicAndAcquire(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedAnd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicAndRelease(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedAnd(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicAndRelaxed(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedAnd(pAtomic, value));
}

/*
 * @brief Atomic || for s32
 */
ALT_FORCE_INLINE s32 AtomicOrSeqCst(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedOr(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicOrAcqRel(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedOr(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicOrAcquire(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedOr(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicOrRelease(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedOr(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicOrRelaxed(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedOr(pAtomic, value));
}

/*
 * @brief Atomic ^ for s32
 */
ALT_FORCE_INLINE s32 AtomicXorSeqCst(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedXor(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicXorAcqRel(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedXor(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicXorAcquire(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedXor(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicXorRelease(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedXor(pAtomic, value));
}

ALT_FORCE_INLINE s32 AtomicXorRelaxed(s32* pAtomic_, const s32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<s32, long>(value_);

    return Memory::Transmute<long, s32>(_InterlockedXor(pAtomic, value));
}

/*
 * @brief Atomic store for f32
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(f32* pAtomic_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(f32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);

    _InterlockedExchange(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(f32* pAtomic_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(f32)) == 0);

    __int32 volatile* pAtomic = reinterpret_cast<__int32 volatile*>(pAtomic_);
    const __int32 value = Memory::Transmute<f32, __int32>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store32(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(f32* pAtomic_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(f32)) == 0);

    __int32 volatile* pAtomic = reinterpret_cast<__int32 volatile*>(pAtomic_);
    const __int32 value = Memory::Transmute<f32, __int32>(value_);

    __iso_volatile_store32(pAtomic, value);
}

/*
 * @brief Atomic load for f32
 */
ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicLoadSeqCst(const f32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    const __int32 volatile* pAtomic = reinterpret_cast<const __int32 volatile*>(pAtomic_);

    const __int32 result = __iso_volatile_load32(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int32, f32>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicLoadAcquire(const f32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    const __int32 volatile* pAtomic = reinterpret_cast<const __int32 volatile*>(pAtomic_);

    const __int32 result = __iso_volatile_load32(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int32, f32>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicLoadRelaxed(const f32* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    const __int32 volatile* pAtomic = reinterpret_cast<const __int32 volatile*>(pAtomic_);

    const __int32 result = __iso_volatile_load32(pAtomic);

    return Memory::Transmute<__int32, f32>(result);
}

/*
 * @brief Atomic exchange for f32
 */
ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicExchangeSeqCst(f32* pAtomic_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);

    return Memory::Transmute<long, f32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicExchangeAcqRel(f32* pAtomic_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);

    return Memory::Transmute<long, f32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicExchangeAcquire(f32* pAtomic_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);

    return Memory::Transmute<long, f32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicExchangeRelease(f32* pAtomic_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);

    return Memory::Transmute<long, f32>(_InterlockedExchange(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicExchangeRelaxed(f32* pAtomic_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);

    return Memory::Transmute<long, f32>(_InterlockedExchange(pAtomic, value));
}

/*
 * @brief Atomic CAS for f32
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeSeqCstSeqCst(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeSeqCstAcquire(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeSeqCstRelaxed(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeAcqRelSeqCst(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeAcqRelAcquire(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeAcqRelRelaxed(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeAcquireSeqCst(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeAcquireAcquire(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeAcquireRelaxed(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeReleaseSeqCst(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeReleaseAcquire(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeReleaseRelaxed(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeRelaxedSeqCst(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeRelaxedAcquire(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f32 AtomicCompareExchangeRelaxedRelaxed(f32* pAtomic_, const f32 compare_, const f32 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u32)) == 0);

    long volatile* pAtomic = reinterpret_cast<long volatile*>(pAtomic_);
    const long value = Memory::Transmute<f32, long>(value_);
    const long compare = Memory::Transmute<f32, long>(compare_);

    return Memory::Transmute<long, f32>(_InterlockedCompareExchange(pAtomic, value, compare));
}

#else

    #error provide implementation for specific platform

#endif // ALT_PLATFORM_X86

} // namespace _PlatformAtomicDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_ATOMICS32__