#pragma once

#ifndef __ALT_STD_PLATFORM_ATOMICS16__
#define __ALT_STD_PLATFORM_ATOMICS16__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32AtomicsCore.h"

/*
 * @brief Atomic operations for u16/s16
 */
namespace Alt {
namespace _PlatformAtomicDetails {

#ifdef ALT_PLATFORM_X86

template <>
struct __supported_operations__<u16>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

template <>
struct __supported_operations__<s16>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

/*
 * @brief Atomic store for u16
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    _InterlockedExchange16(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    __int16 volatile* pAtomic = reinterpret_cast<__int16 volatile*>(pAtomic_);
    const __int16 value = Memory::Transmute<u16, __int16>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store16(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    __int16 volatile* pAtomic = reinterpret_cast<__int16 volatile*>(pAtomic_);
    const __int16 value = Memory::Transmute<u16, __int16>(value_);

    __iso_volatile_store16(pAtomic, value);
}

/*
 * @brief Atomic load for u16
 */
ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicLoadSeqCst(const u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    const __int16 volatile* pAtomic = reinterpret_cast<const __int16 volatile*>(pAtomic_);

    const __int16 result = __iso_volatile_load16(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int16, u16>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicLoadAcquire(const u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    const __int16 volatile* pAtomic = reinterpret_cast<const __int16 volatile*>(pAtomic_);

    const __int16 result = __iso_volatile_load16(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int16, u16>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicLoadRelaxed(const u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    const __int16 volatile* pAtomic = reinterpret_cast<const __int16 volatile*>(pAtomic_);

    const __int16 result = __iso_volatile_load16(pAtomic);

    return Memory::Transmute<__int16, u16>(result);
}

/*
 * @brief Atomic exchange for u16
 */
ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicExchangeSeqCst(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchange16(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicExchangeAcqRel(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchange16(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicExchangeAcquire(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchange16(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicExchangeRelease(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchange16(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicExchangeRelaxed(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchange16(pAtomic, value));
}

/*
 * @brief Atomic CAS for u16
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeSeqCstSeqCst(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeSeqCstAcquire(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeSeqCstRelaxed(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeAcqRelSeqCst(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeAcqRelAcquire(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeAcqRelRelaxed(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeAcquireSeqCst(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeAcquireAcquire(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeAcquireRelaxed(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeReleaseSeqCst(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeReleaseAcquire(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeReleaseRelaxed(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeRelaxedSeqCst(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeRelaxedAcquire(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u16 AtomicCompareExchangeRelaxedRelaxed(u16* pAtomic_, const u16 compare_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);
    const short compare = Memory::Transmute<u16, short>(compare_);

    return Memory::Transmute<short, u16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for u16
 */
ALT_FORCE_INLINE u16 AtomicAddSeqCst(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicAddAcqRel(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicAddAcquire(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicAddRelease(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicAddRelaxed(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

/*
 * @brief Atomic subtraction for u16
 */
ALT_FORCE_INLINE u16 AtomicSubSeqCst(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(Negate(value_));

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicSubAcqRel(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(Negate(value_));

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicSubAcquire(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(Negate(value_));

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicSubRelease(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(Negate(value_));

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicSubRelaxed(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(Negate(value_));

    return Memory::Transmute<short, u16>(_InterlockedExchangeAdd16(pAtomic, value));
}

/*
 * @brief Atomic increment for u16
 */
ALT_FORCE_INLINE u16 AtomicIncSeqCst(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedIncrement16(pAtomic)) - 1;
}

ALT_FORCE_INLINE u16 AtomicIncAcqRel(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedIncrement16(pAtomic)) - 1;
}

ALT_FORCE_INLINE u16 AtomicIncAcquire(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedIncrement16(pAtomic)) - 1;
}

ALT_FORCE_INLINE u16 AtomicIncRelease(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedIncrement16(pAtomic)) - 1;
}

ALT_FORCE_INLINE u16 AtomicIncRelaxed(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedIncrement16(pAtomic)) - 1;
}

/*
 * @brief Atomic decrement for u16
 */
ALT_FORCE_INLINE u16 AtomicDecSeqCst(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedDecrement16(pAtomic)) + 1;
}

ALT_FORCE_INLINE u16 AtomicDecAcqRel(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedDecrement16(pAtomic)) + 1;
}

ALT_FORCE_INLINE u16 AtomicDecAcquire(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedDecrement16(pAtomic)) + 1;
}

ALT_FORCE_INLINE u16 AtomicDecRelease(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedDecrement16(pAtomic)) + 1;
}

ALT_FORCE_INLINE u16 AtomicDecRelaxed(u16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, u16>(_InterlockedDecrement16(pAtomic)) + 1;
}

/*
 * @brief Atomic && for u16
 */
ALT_FORCE_INLINE u16 AtomicAndSeqCst(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedAnd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicAndAcqRel(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedAnd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicAndAcquire(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedAnd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicAndRelease(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedAnd16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicAndRelaxed(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedAnd16(pAtomic, value));
}

/*
 * @brief Atomic || for u16
 */
ALT_FORCE_INLINE u16 AtomicOrSeqCst(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedOr16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicOrAcqRel(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedOr16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicOrAcquire(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedOr16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicOrRelease(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedOr16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicOrRelaxed(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedOr16(pAtomic, value));
}

/*
 * @brief Atomic ^ for u16
 */
ALT_FORCE_INLINE u16 AtomicXorSeqCst(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedXor16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicXorAcqRel(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedXor16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicXorAcquire(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedXor16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicXorRelease(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedXor16(pAtomic, value));
}

ALT_FORCE_INLINE u16 AtomicXorRelaxed(u16* pAtomic_, const u16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<u16, short>(value_);

    return Memory::Transmute<short, u16>(_InterlockedXor16(pAtomic, value));
}

/*
 * @brief Atomic store for s16
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    _InterlockedExchange16(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    __int16 volatile* pAtomic = reinterpret_cast<__int16 volatile*>(pAtomic_);
    const __int16 value = Memory::Transmute<s16, __int16>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store16(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    __int16 volatile* pAtomic = reinterpret_cast<__int16 volatile*>(pAtomic_);
    const __int16 value = Memory::Transmute<s16, __int16>(value_);

    __iso_volatile_store16(pAtomic, value);
}

/*
 * @brief Atomic load for s16
 */
ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicLoadSeqCst(const s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    const __int16 volatile* pAtomic = reinterpret_cast<const __int16 volatile*>(pAtomic_);

    const __int16 result = __iso_volatile_load16(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int16, s16>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicLoadAcquire(const s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    const __int16 volatile* pAtomic = reinterpret_cast<const __int16 volatile*>(pAtomic_);

    const __int16 result = __iso_volatile_load16(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int16, s16>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicLoadRelaxed(const s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    const __int16 volatile* pAtomic = reinterpret_cast<const __int16 volatile*>(pAtomic_);

    const __int16 result = __iso_volatile_load16(pAtomic);

    return Memory::Transmute<__int16, s16>(result);
}

/*
 * @brief Atomic exchange for s16
 */
ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicExchangeSeqCst(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchange16(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicExchangeAcqRel(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchange16(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicExchangeAcquire(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchange16(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicExchangeRelease(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchange16(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicExchangeRelaxed(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchange16(pAtomic, value));
}

/*
 * @brief Atomic CAS for s16
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeSeqCstSeqCst(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeSeqCstAcquire(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeSeqCstRelaxed(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeAcqRelSeqCst(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeAcqRelAcquire(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeAcqRelRelaxed(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeAcquireSeqCst(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeAcquireAcquire(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeAcquireRelaxed(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeReleaseSeqCst(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeReleaseAcquire(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeReleaseRelaxed(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeRelaxedSeqCst(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeRelaxedAcquire(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s16 AtomicCompareExchangeRelaxedRelaxed(s16* pAtomic_, const s16 compare_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);
    const short compare = Memory::Transmute<s16, short>(compare_);

    return Memory::Transmute<short, s16>(_InterlockedCompareExchange16(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for s16
 */
ALT_FORCE_INLINE s16 AtomicAddSeqCst(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicAddAcqRel(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicAddAcquire(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicAddRelease(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicAddRelaxed(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

/*
 * @brief Atomic subtraction for s16
 */
ALT_FORCE_INLINE s16 AtomicSubSeqCst(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(Negate(value_));

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicSubAcqRel(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(Negate(value_));

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicSubAcquire(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(Negate(value_));

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicSubRelease(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(Negate(value_));

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicSubRelaxed(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(Negate(value_));

    return Memory::Transmute<short, s16>(_InterlockedExchangeAdd16(pAtomic, value));
}

/*
 * @brief Atomic increment for s16
 */
ALT_FORCE_INLINE s16 AtomicIncSeqCst(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedIncrement16(pAtomic)) - 1;
}

ALT_FORCE_INLINE s16 AtomicIncAcqRel(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedIncrement16(pAtomic)) - 1;
}

ALT_FORCE_INLINE s16 AtomicIncAcquire(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedIncrement16(pAtomic)) - 1;
}

ALT_FORCE_INLINE s16 AtomicIncRelease(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedIncrement16(pAtomic)) - 1;
}

ALT_FORCE_INLINE s16 AtomicIncRelaxed(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedIncrement16(pAtomic)) - 1;
}

/*
 * @brief Atomic decrement for s16
 */
ALT_FORCE_INLINE s16 AtomicDecSeqCst(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedDecrement16(pAtomic)) + 1;
}

ALT_FORCE_INLINE s16 AtomicDecAcqRel(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedDecrement16(pAtomic)) + 1;
}

ALT_FORCE_INLINE s16 AtomicDecAcquire(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedDecrement16(pAtomic)) + 1;
}

ALT_FORCE_INLINE s16 AtomicDecRelease(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedDecrement16(pAtomic)) + 1;
}

ALT_FORCE_INLINE s16 AtomicDecRelaxed(s16* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);

    return Memory::Transmute<short, s16>(_InterlockedDecrement16(pAtomic)) + 1;
}

/*
 * @brief Atomic && for s16
 */
ALT_FORCE_INLINE s16 AtomicAndSeqCst(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedAnd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicAndAcqRel(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedAnd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicAndAcquire(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedAnd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicAndRelease(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedAnd16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicAndRelaxed(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedAnd16(pAtomic, value));
}

/*
 * @brief Atomic || for s16
 */
ALT_FORCE_INLINE s16 AtomicOrSeqCst(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedOr16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicOrAcqRel(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedOr16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicOrAcquire(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedOr16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicOrRelease(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedOr16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicOrRelaxed(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedOr16(pAtomic, value));
}

/*
 * @brief Atomic ^ for s16
 */
ALT_FORCE_INLINE s16 AtomicXorSeqCst(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedXor16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicXorAcqRel(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedXor16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicXorAcquire(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedXor16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicXorRelease(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedXor16(pAtomic, value));
}

ALT_FORCE_INLINE s16 AtomicXorRelaxed(s16* pAtomic_, const s16 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s16)) == 0);

    short volatile* pAtomic = reinterpret_cast<short volatile*>(pAtomic_);
    const short value = Memory::Transmute<s16, short>(value_);

    return Memory::Transmute<short, s16>(_InterlockedXor16(pAtomic, value));
}

#else

    #error provide implementation for specific platform

#endif // ALT_PLATFORM_X86

} // namespace _PlatformAtomicDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_ATOMICS16__