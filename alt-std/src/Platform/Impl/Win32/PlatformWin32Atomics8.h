#pragma once

#ifndef __ALT_STD_PLATFORM_ATOMICS8__
#define __ALT_STD_PLATFORM_ATOMICS8__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32AtomicsCore.h"

/*
 * @brief Atomic operations for u8/s8
 */
namespace Alt {
namespace _PlatformAtomicDetails {

#ifdef ALT_PLATFORM_X86

template <>
struct __supported_operations__<u8>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

template <>
struct __supported_operations__<s8>
    : SupportedOperations<SupportedOperationsTrait::All>
{};

/*
 * @brief Atomic store for u8
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    _InterlockedExchange8(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    __int8 volatile* pAtomic = reinterpret_cast<__int8 volatile*>(pAtomic_);
    const __int8 value = Memory::Transmute<u8, __int8>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store8(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    __int8 volatile* pAtomic = reinterpret_cast<__int8 volatile*>(pAtomic_);
    const __int8 value = Memory::Transmute<u8, __int8>(value_);

    __iso_volatile_store8(pAtomic, value);
}

/*
 * @brief Atomic load for u8
 */
ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicLoadSeqCst(const u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    const __int8 volatile* pAtomic = reinterpret_cast<const __int8 volatile*>(pAtomic_);

    const __int8 result = __iso_volatile_load8(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int8, u8>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicLoadAcquire(const u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    const __int8 volatile* pAtomic = reinterpret_cast<const __int8 volatile*>(pAtomic_);

    const __int8 result = __iso_volatile_load8(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int8, u8>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicLoadRelaxed(const u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    const __int8 volatile* pAtomic = reinterpret_cast<const __int8 volatile*>(pAtomic_);

    const __int8 result = __iso_volatile_load8(pAtomic);

    return Memory::Transmute<__int8, u8>(result);
}

/*
 * @brief Atomic exchange for u8
 */
ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicExchangeSeqCst(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchange8(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicExchangeAcqRel(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchange8(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicExchangeAcquire(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchange8(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicExchangeRelease(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchange8(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicExchangeRelaxed(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchange8(pAtomic, value));
}

/*
 * @brief Atomic CAS for u8
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeSeqCstSeqCst(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeSeqCstAcquire(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeSeqCstRelaxed(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeAcqRelSeqCst(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeAcqRelAcquire(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeAcqRelRelaxed(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeAcquireSeqCst(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeAcquireAcquire(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeAcquireRelaxed(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeReleaseSeqCst(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeReleaseAcquire(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeReleaseRelaxed(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeRelaxedSeqCst(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeRelaxedAcquire(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE u8 AtomicCompareExchangeRelaxedRelaxed(u8* pAtomic_, const u8 compare_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);
    const char compare = Memory::Transmute<u8, char>(compare_);

    return Memory::Transmute<char, u8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for u8
 */
ALT_FORCE_INLINE u8 AtomicAddSeqCst(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicAddAcqRel(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicAddAcquire(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicAddRelease(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicAddRelaxed(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

/*
 * @brief Atomic subtraction for u8
 */
ALT_FORCE_INLINE u8 AtomicSubSeqCst(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(value_));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicSubAcqRel(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(value_));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicSubAcquire(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(value_));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicSubRelease(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(value_));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicSubRelaxed(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(value_));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

/*
 * @brief Atomic increment for u8
 */
ALT_FORCE_INLINE u8 AtomicIncSeqCst(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

ALT_FORCE_INLINE u8 AtomicIncAcqRel(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

ALT_FORCE_INLINE u8 AtomicIncAcquire(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

ALT_FORCE_INLINE u8 AtomicIncRelease(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

ALT_FORCE_INLINE u8 AtomicIncRelaxed(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

/*
 * @brief Atomic decrement for u8
 */
ALT_FORCE_INLINE u8 AtomicDecSeqCst(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(static_cast<u8>(1)));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicDecAcqRel(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(static_cast<u8>(1)));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicDecAcquire(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(static_cast<u8>(1)));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicDecRelease(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(static_cast<u8>(1)));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicDecRelaxed(u8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(Negate(static_cast<u8>(1)));

    return Memory::Transmute<char, u8>(_InterlockedExchangeAdd8(pAtomic, value));
}

/*
 * @brief Atomic && for u8
 */
ALT_FORCE_INLINE u8 AtomicAndSeqCst(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedAnd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicAndAcqRel(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedAnd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicAndAcquire(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedAnd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicAndRelease(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedAnd8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicAndRelaxed(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedAnd8(pAtomic, value));
}

/*
 * @brief Atomic || for u8
 */
ALT_FORCE_INLINE u8 AtomicOrSeqCst(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedOr8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicOrAcqRel(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedOr8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicOrAcquire(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedOr8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicOrRelease(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedOr8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicOrRelaxed(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedOr8(pAtomic, value));
}

/*
 * @brief Atomic ^ for u8
 */
ALT_FORCE_INLINE u8 AtomicXorSeqCst(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedXor8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicXorAcqRel(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedXor8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicXorAcquire(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedXor8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicXorRelease(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedXor8(pAtomic, value));
}

ALT_FORCE_INLINE u8 AtomicXorRelaxed(u8* pAtomic_, const u8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(u8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<u8, char>(value_);

    return Memory::Transmute<char, u8>(_InterlockedXor8(pAtomic, value));
}

/*
 * @brief Atomic store for s8
 */
ALT_FORCE_INLINE void AtomicStoreSeqCst(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    _InterlockedExchange8(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelease(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    __int8 volatile* pAtomic = reinterpret_cast<__int8 volatile*>(pAtomic_);
    const __int8 value = Memory::Transmute<s8, __int8>(value_);

    _ReadWriteBarrier();

    __iso_volatile_store8(pAtomic, value);
}

ALT_FORCE_INLINE void AtomicStoreRelaxed(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    __int8 volatile* pAtomic = reinterpret_cast<__int8 volatile*>(pAtomic_);
    const __int8 value = Memory::Transmute<s8, __int8>(value_);

    __iso_volatile_store8(pAtomic, value);
}

/*
 * @brief Atomic load for s8
 */
ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicLoadSeqCst(const s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    const __int8 volatile* pAtomic = reinterpret_cast<const __int8 volatile*>(pAtomic_);

    const __int8 result = __iso_volatile_load8(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int8, s8>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicLoadAcquire(const s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    const __int8 volatile* pAtomic = reinterpret_cast<const __int8 volatile*>(pAtomic_);

    const __int8 result = __iso_volatile_load8(pAtomic);

    _ReadWriteBarrier();

    return Memory::Transmute<__int8, s8>(result);
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicLoadRelaxed(const s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    const __int8 volatile* pAtomic = reinterpret_cast<const __int8 volatile*>(pAtomic_);

    const __int8 result = __iso_volatile_load8(pAtomic);

    return Memory::Transmute<__int8, s8>(result);
}

/*
 * @brief Atomic exchange for s8
 */
ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicExchangeSeqCst(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchange8(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicExchangeAcqRel(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchange8(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicExchangeAcquire(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchange8(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicExchangeRelease(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchange8(pAtomic, value));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicExchangeRelaxed(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchange8(pAtomic, value));
}

/*
 * @brief Atomic CAS for s8
 *
 *                            failure
 *                            vvvvvv
 * AtomicCompareExchangeSeqCstSeqCst(...)
 *                      ^^^^^^
 *                      success
 */
ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeSeqCstSeqCst(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeSeqCstAcquire(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeSeqCstRelaxed(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeAcqRelSeqCst(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeAcqRelAcquire(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeAcqRelRelaxed(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeAcquireSeqCst(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeAcquireAcquire(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeAcquireRelaxed(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeReleaseSeqCst(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeReleaseAcquire(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeReleaseRelaxed(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeRelaxedSeqCst(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeRelaxedAcquire(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

ALT_NODISCARD ALT_FORCE_INLINE s8 AtomicCompareExchangeRelaxedRelaxed(s8* pAtomic_, const s8 compare_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);
    const char compare = Memory::Transmute<s8, char>(compare_);

    return Memory::Transmute<char, s8>(_InterlockedCompareExchange8(pAtomic, value, compare));
}

/*
 * @brief Atomic addition for s8
 */
ALT_FORCE_INLINE s8 AtomicAddSeqCst(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicAddAcqRel(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicAddAcquire(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicAddRelease(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicAddRelaxed(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

/*
 * @brief Atomic subtraction for s8
 */
ALT_FORCE_INLINE s8 AtomicSubSeqCst(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(value_));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicSubAcqRel(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(value_));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicSubAcquire(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(value_));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicSubRelease(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(value_));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicSubRelaxed(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(value_));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

/*
 * @brief Atomic increment for s8
 */
ALT_FORCE_INLINE s8 AtomicIncSeqCst(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

ALT_FORCE_INLINE s8 AtomicIncAcqRel(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

ALT_FORCE_INLINE s8 AtomicIncAcquire(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

ALT_FORCE_INLINE s8 AtomicIncRelease(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

ALT_FORCE_INLINE s8 AtomicIncRelaxed(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, 1));
}

/*
 * @brief Atomic decrement for s8
 */
ALT_FORCE_INLINE s8 AtomicDecSeqCst(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(static_cast<s8>(1)));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicDecAcqRel(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(static_cast<s8>(1)));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicDecAcquire(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(static_cast<s8>(1)));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicDecRelease(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(static_cast<s8>(1)));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicDecRelaxed(s8* pAtomic_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(Negate(static_cast<s8>(1)));

    return Memory::Transmute<char, s8>(_InterlockedExchangeAdd8(pAtomic, value));
}

/*
 * @brief Atomic && for s8
 */
ALT_FORCE_INLINE s8 AtomicAndSeqCst(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedAnd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicAndAcqRel(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedAnd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicAndAcquire(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedAnd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicAndRelease(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedAnd8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicAndRelaxed(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedAnd8(pAtomic, value));
}

/*
 * @brief Atomic || for s8
 */
ALT_FORCE_INLINE s8 AtomicOrSeqCst(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedOr8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicOrAcqRel(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedOr8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicOrAcquire(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedOr8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicOrRelease(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedOr8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicOrRelaxed(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedOr8(pAtomic, value));
}

/*
 * @brief Atomic ^ for s8
 */
ALT_FORCE_INLINE s8 AtomicXorSeqCst(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedXor8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicXorAcqRel(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedXor8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicXorAcquire(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedXor8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicXorRelease(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedXor8(pAtomic, value));
}

ALT_FORCE_INLINE s8 AtomicXorRelaxed(s8* pAtomic_, const s8 value_) noexcept {

    ALT_ASSERT(Memory::Align(pAtomic_, sizeof(s8)) == 0);

    char volatile* pAtomic = reinterpret_cast<char volatile*>(pAtomic_);
    const char value = Memory::Transmute<s8, char>(value_);

    return Memory::Transmute<char, s8>(_InterlockedXor8(pAtomic, value));
}

#else

    #error provide implementation for specific platform

#endif // ALT_PLATFORM_X86

} // namespace _PlatformAtomicDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_ATOMICS8__