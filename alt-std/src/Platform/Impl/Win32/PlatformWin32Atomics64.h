#pragma once

#ifndef __ALT_STD_PLATFORM_ATOMICS64__
#define __ALT_STD_PLATFORM_ATOMICS64__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32AtomicsCore.h"

/*
 * @brief Atomic operations for u64/s64/f64
 */
namespace Alt {
namespace _PlatformAtomicDetails {

#if defined(ALT_PLATFORM_X86_64)

template <>
struct __supported_operations__<u64>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

template <>
struct __supported_operations__<s64>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

template <>
struct __supported_operations__<f64>
    : SupportedOperations<
          SupportedOperationsTrait::Store
        | SupportedOperationsTrait::Load
        | SupportedOperationsTrait::Exchange
        | SupportedOperationsTrait::CompareExchange
    >
{};

/*
 * @brief Atomic store for u64
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    _InterlockedExchange64(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store64(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    __iso_volatile_store64(pAtomic, value);
}

/*
 * @brief Atomic load for u64
 */
ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicLoadSeqCst(const u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, u64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicLoadAcquire(const u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, u64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicLoadRelaxed(const u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    return Memory::Transmute<__int64, u64>(result);
}

/*
 * @brief Atomic exchange for u64
 */
ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));
}

/*
 * @brief Atomic CAS for u64
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeSeqCstSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeSeqCstAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeSeqCstRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcqRelSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcqRelAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcqRelRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcquireSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcquireAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcquireRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeReleaseSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeReleaseAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeReleaseRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeRelaxedSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeRelaxedAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeRelaxedRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for u64
 */
ALT_FORCE_INLINE u64 AtomicAddSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicAddAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicAddAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicAddRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicAddRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

/*
 * @brief Atomic subtraction for u64
 */
ALT_FORCE_INLINE u64 AtomicSubSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicSubAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicSubAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicSubRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicSubRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));
}

/*
 * @brief Atomic increment for u64
 */
ALT_FORCE_INLINE u64 AtomicIncSeqCst(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;
}

ALT_FORCE_INLINE u64 AtomicIncAcqRel(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;
}

ALT_FORCE_INLINE u64 AtomicIncAcquire(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;
}

ALT_FORCE_INLINE u64 AtomicIncRelease(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;
}

ALT_FORCE_INLINE u64 AtomicIncRelaxed(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;
}

/*
 * @brief Atomic decrement for u64
 */
ALT_FORCE_INLINE u64 AtomicDecSeqCst(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;
}

ALT_FORCE_INLINE u64 AtomicDecAcqRel(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;
}

ALT_FORCE_INLINE u64 AtomicDecAcquire(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;
}

ALT_FORCE_INLINE u64 AtomicDecRelease(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;
}

ALT_FORCE_INLINE u64 AtomicDecRelaxed(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;
}

/*
 * @brief Atomic && for u64
 */
ALT_FORCE_INLINE u64 AtomicAndSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicAndAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicAndAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicAndRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicAndRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));
}

/*
 * @brief Atomic || for u64
 */
ALT_FORCE_INLINE u64 AtomicOrSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicOrAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicOrAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicOrRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicOrRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));
}

/*
 * @brief Atomic ^ for u64
 */
ALT_FORCE_INLINE u64 AtomicXorSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicXorAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicXorAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicXorRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));
}

ALT_FORCE_INLINE u64 AtomicXorRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));
}

/*
 * @brief Atomic store for s64
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    _InterlockedExchange64(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = static_cast<__int64>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store64(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = static_cast<__int64>(value_);

    __iso_volatile_store64(pAtomic, value);
}

/*
 * @brief Atomic load for s64
 */
ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicLoadSeqCst(const s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, s64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicLoadAcquire(const s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, s64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicLoadRelaxed(const s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    return Memory::Transmute<__int64, s64>(result);
}

/*
 * @brief Atomic exchange for s64
 */
ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));
}

/*
 * @brief Atomic CAS for s64
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeSeqCstSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeSeqCstAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeSeqCstRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcqRelSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcqRelAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcqRelRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcquireSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcquireAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcquireRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeReleaseSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeReleaseAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeReleaseRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeRelaxedSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeRelaxedAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeRelaxedRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for s64
 */
ALT_FORCE_INLINE s64 AtomicAddSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicAddAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicAddAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicAddRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicAddRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

/*
 * @brief Atomic subtraction for s64
 */
ALT_FORCE_INLINE s64 AtomicSubSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicSubAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicSubAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicSubRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicSubRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));
}

/*
 * @brief Atomic increment for s64
 */
ALT_FORCE_INLINE s64 AtomicIncSeqCst(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;
}

ALT_FORCE_INLINE s64 AtomicIncAcqRel(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;
}

ALT_FORCE_INLINE s64 AtomicIncAcquire(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;
}

ALT_FORCE_INLINE s64 AtomicIncRelease(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;
}

ALT_FORCE_INLINE s64 AtomicIncRelaxed(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;
}

/*
 * @brief Atomic decrement for s64
 */
ALT_FORCE_INLINE s64 AtomicDecSeqCst(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;
}

ALT_FORCE_INLINE s64 AtomicDecAcqRel(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;
}

ALT_FORCE_INLINE s64 AtomicDecAcquire(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;
}

ALT_FORCE_INLINE s64 AtomicDecRelease(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;
}

ALT_FORCE_INLINE s64 AtomicDecRelaxed(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;
}

/*
 * @brief Atomic && for s64
 */
ALT_FORCE_INLINE s64 AtomicAndSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicAndAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicAndAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicAndRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicAndRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));
}

/*
 * @brief Atomic || for s64
 */
ALT_FORCE_INLINE s64 AtomicOrSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicOrAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicOrAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicOrRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicOrRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));
}

/*
 * @brief Atomic ^ for s64
 */
ALT_FORCE_INLINE s64 AtomicXorSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicXorAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicXorAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicXorRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));
}

ALT_FORCE_INLINE s64 AtomicXorRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));
}

/*
 * @brief Atomic store for f64
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(f64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    _InterlockedExchange64(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(f64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store64(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(f64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    __iso_volatile_store64(pAtomic, value);
}

/*
 * @brief Atomic load for f64
 */
ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicLoadSeqCst(const f64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, f64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicLoadAcquire(const f64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, f64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicLoadRelaxed(const f64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    return Memory::Transmute<__int64, f64>(result);
}

/*
 * @brief Atomic exchange for f64
 */
ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeSeqCst(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeAcqRel(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeAcquire(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeRelease(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeRelaxed(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));
}

/*
 * @brief Atomic CAS for f64
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeSeqCstSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeSeqCstAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeSeqCstRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcqRelSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcqRelAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcqRelRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcquireSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcquireAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcquireRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeReleaseSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeReleaseAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeReleaseRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeRelaxedSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeRelaxedAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeRelaxedRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

#elif defined(ALT_PLATFORM_X86)

template <>
struct __supported_operations__<u64>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

template <>
struct __supported_operations__<s64>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

template <>
struct __supported_operations__<f64>
    : SupportedOperations<
          SupportedOperationsTrait::Store
        | SupportedOperationsTrait::Load
        | SupportedOperationsTrait::Exchange
        | SupportedOperationsTrait::CompareExchange
    >
{};

/*
 * @brief Atomic store for u64
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    _interlockedexchange64(pAtomic, value);

#elif defined(ALT_COMPILER_CLANG)

    _InterlockedExchange64(pAtomic, value);

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE void AtomicStoreRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store64(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

    __iso_volatile_store64(pAtomic, value);
}

/*
 * @brief Atomic load for u64
 */
ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicLoadSeqCst(const u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, u64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicLoadAcquire(const u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, u64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicLoadRelaxed(const u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    return Memory::Transmute<__int64, u64>(result);
}

/*
 * @brief Atomic exchange for u64
 */
ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicExchangeRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic CAS for u64
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeSeqCstSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeSeqCstAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeSeqCstRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcqRelSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcqRelAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcqRelRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcquireSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcquireAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeAcquireRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeReleaseSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeReleaseAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeReleaseRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeRelaxedSeqCst(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeRelaxedAcquire(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u64 AtomicCompareExchangeRelaxedRelaxed(u64* pAtomic_, const u64 compare_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);
    const __int64 compare = Memory::Transmute<u64, __int64>(compare_);

    return Memory::Transmute<__int64, u64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for u64
 */
ALT_FORCE_INLINE u64 AtomicAddSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicAddAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicAddAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicAddRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicAddRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic subtraction for u64
 */
ALT_FORCE_INLINE u64 AtomicSubSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicSubAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicSubAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicSubRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicSubRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic increment for u64
 */
ALT_FORCE_INLINE u64 AtomicIncSeqCst(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicIncAcqRel(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicIncAcquire(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicIncRelease(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicIncRelaxed(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic decrement for u64
 */
ALT_FORCE_INLINE u64 AtomicDecSeqCst(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicDecAcqRel(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicDecAcquire(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicDecRelease(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicDecRelaxed(u64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic && for u64
 */
ALT_FORCE_INLINE u64 AtomicAndSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicAndAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicAndAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicAndRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicAndRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic || for u64
 */
ALT_FORCE_INLINE u64 AtomicOrSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicOrAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicOrAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicOrRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicOrRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic ^ for u64
 */
ALT_FORCE_INLINE u64 AtomicXorSeqCst(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicXorAcqRel(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicXorAcquire(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicXorRelease(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE u64 AtomicXorRelaxed(u64* pAtomic_, const u64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<u64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, u64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, u64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic store for s64
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    _interlockedexchange64(pAtomic, value);

#elif defined(ALT_COMPILER_CLANG)

    _InterlockedExchange64(pAtomic, value);

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE void AtomicStoreRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = static_cast<__int64>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store64(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = static_cast<__int64>(value_);

    __iso_volatile_store64(pAtomic, value);
}

/*
 * @brief Atomic load for s64
 */
ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicLoadSeqCst(const s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, s64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicLoadAcquire(const s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, s64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicLoadRelaxed(const s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    return Memory::Transmute<__int64, s64>(result);
}

/*
 * @brief Atomic exchange for s64
 */
ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicExchangeRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic CAS for s64
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeSeqCstSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeSeqCstAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeSeqCstRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcqRelSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcqRelAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcqRelRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcquireSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcquireAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeAcquireRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeReleaseSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeReleaseAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeReleaseRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeRelaxedSeqCst(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeRelaxedAcquire(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s64 AtomicCompareExchangeRelaxedRelaxed(s64* pAtomic_, const s64 compare_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);
    const __int64 compare = Memory::Transmute<s64, __int64>(compare_);

    return Memory::Transmute<__int64, s64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for s64
 */
ALT_FORCE_INLINE s64 AtomicAddSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicAddAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicAddAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicAddRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicAddRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic subtraction for s64
 */
ALT_FORCE_INLINE s64 AtomicSubSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicSubAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicSubAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicSubRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicSubRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(Negate(value_));

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedexchangeadd64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedExchangeAdd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic increment for s64
 */
ALT_FORCE_INLINE s64 AtomicIncSeqCst(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicIncAcqRel(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicIncAcquire(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicIncRelease(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicIncRelaxed(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedincrement64(pAtomic)) - 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedIncrement64(pAtomic)) - 1;

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic decrement for s64
 */
ALT_FORCE_INLINE s64 AtomicDecSeqCst(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicDecAcqRel(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicDecAcquire(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicDecRelease(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicDecRelaxed(s64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockeddecrement64(pAtomic)) + 1;

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedDecrement64(pAtomic)) + 1;

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic && for s64
 */
ALT_FORCE_INLINE s64 AtomicAndSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicAndAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicAndAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicAndRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicAndRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedand64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedAnd64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic || for s64
 */
ALT_FORCE_INLINE s64 AtomicOrSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicOrAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicOrAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicOrRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicOrRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedOr64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic ^ for s64
 */
ALT_FORCE_INLINE s64 AtomicXorSeqCst(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicXorAcqRel(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicXorAcquire(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicXorRelease(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE s64 AtomicXorRelaxed(s64* pAtomic_, const s64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<s64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, s64>(_interlockedxor64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, s64>(_InterlockedXor64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic store for f64
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(f64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    _interlockedexchange64(pAtomic, value);

#elif defined(ALT_COMPILER_CLANG)

    _InterlockedExchange64(pAtomic, value);

#else
    #error provide implementation for specific compiler
#endif
}

ALT_FORCE_INLINE void AtomicStoreRelease(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(f64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store64(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(f64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

    __iso_volatile_store64(pAtomic, value);
}

/*
 * @brief Atomic load for f64
 */
ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicLoadSeqCst(const f64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, f64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicLoadAcquire(const f64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int64, f64>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicLoadRelaxed(const f64* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    const __int64 volatile* pAtomic = reinterpret_cast<const __int64 volatile*>(pAtomic_);

    const __int64 result = __iso_volatile_load64(pAtomic);

    return Memory::Transmute<__int64, f64>(result);
}

/*
 * @brief Atomic exchange for f64
 */
ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeSeqCst(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, f64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeAcqRel(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, f64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeAcquire(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, f64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeRelease(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, f64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicExchangeRelaxed(f64* pAtomic_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);

#if defined(ALT_COMPILER_MSVC)

    return Memory::Transmute<__int64, f64>(_interlockedexchange64(pAtomic, value));

#elif defined(ALT_COMPILER_CLANG)

    return Memory::Transmute<__int64, f64>(_InterlockedExchange64(pAtomic, value));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @brief Atomic CAS for f64
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeSeqCstSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeSeqCstAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeSeqCstRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcqRelSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcqRelAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcqRelRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcquireSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcquireAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeAcquireRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeReleaseSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeReleaseAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeReleaseRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeRelaxedSeqCst(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeRelaxedAcquire(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE f64 AtomicCompareExchangeRelaxedRelaxed(f64* pAtomic_, const f64 compare_, const f64 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u64)) == 0);

    __int64 volatile* pAtomic = reinterpret_cast<__int64 volatile*>(pAtomic_);
    const __int64 value = Memory::Transmute<f64, __int64>(value_);
    const __int64 compare = Memory::Transmute<f64, __int64>(compare_);

    return Memory::Transmute<__int64, f64>(_InterlockedCompareExchange64(pAtomic, value, compare));
}

#else

    #error provide implementation for specific platform

#endif // ALT_PLATFORM_X86_64

} // namespace _PlatformAtomicDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_ATOMICS64__