#pragma once

#ifndef __ALT_STD_PLATFORM_ATOMICS_CORE__
#define __ALT_STD_PLATFORM_ATOMICS_CORE__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include <intrin.h>

#include "../../../TypeTraits.h"

#include "../../../Memory/Impl/MemoryPreInclude.h"
#include "../../../Utils/EnumClassBitFlagsImpl.h"

namespace Alt {
namespace _PlatformAtomicDetails {

/*
 * https://cppreference.com/cpp/atomic/memory_order
 *
 * enum class MemoryOrder : usize {
 * 	   Relaxed,
 * 	   Acquire,
 * 	   Release,
 * 	   AcqRel,
 * 	   SeqCst,
 * };
 */
enum class SupportedOperationsTrait : usize {

    None = 0,

    Store = static_cast<usize>(1) << 0,
    Load = static_cast<usize>(1) << 1,

    Exchange = static_cast<usize>(1) << 2,
    CompareExchange = static_cast<usize>(1) << 3,

    // Add, Sub, Inc, Dec
    Additive = static_cast<usize>(1) << 4,

    And = static_cast<usize>(1) << 5,
    Or = static_cast<usize>(1) << 6,
    Xor = static_cast<usize>(1) << 7,

    All = Store | Load | Exchange | CompareExchange | Additive | And | Or | Xor,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(SupportedOperationsTrait);

template <SupportedOperationsTrait TRAIT>
using SupportedOperations = std::integral_constant<SupportedOperationsTrait, TRAIT>;

template <typename T>
struct __supported_operations__
    : SupportedOperations<SupportedOperationsTrait::None>
{};

template <typename T>
constexpr static usize align_of_v = alignof(T);

template <typename T>
constexpr static SupportedOperationsTrait supported_operations_v = __supported_operations__<TypeTraits::clean_t<T>>::value;

template <typename T,
    std::enable_if_t<Num::Desc<T>::IsSigned()>* = nullptr>
ALT_FORCE_INLINE T Negate(const T val) noexcept {
    return -val;
}

template <typename T,
    std::enable_if_t<Num::Desc<T>::IsUnsigned()>* = nullptr>
ALT_FORCE_INLINE T Negate(const T val) noexcept {

    using S = typename Num::Desc<T>::Signed;

    const S s = Memory::Transmute<T, S>(val);
    return Memory::Transmute<S, T>(-s);
}

ALT_FORCE_INLINE void ThreadMemoryOrderBarrierAcquire() noexcept {

    _ReadWriteBarrier();
}

ALT_FORCE_INLINE void ThreadMemoryOrderBarrierRelease() noexcept {

    _ReadWriteBarrier();
}

ALT_FORCE_INLINE void ThreadMemoryOrderBarrierSeqCst() noexcept {

    _ReadWriteBarrier();

#ifdef ALT_PLATFORM_64BIT

    /*
     * MSVC - _InterlockedOr
     * Clang - _mm_mfence
     */
    __faststorefence();

#else

    volatile long _fence;
    _InterlockedOr(&_fence, 0);

#endif // ALT_PLATFORM_64BIT

    _ReadWriteBarrier();
}

} // namespace _PlatformAtomicDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_ATOMICS_CORE__