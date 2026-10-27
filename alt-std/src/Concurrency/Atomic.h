#pragma once

#ifndef __ALT_STD_ATOMIC__
#define __ALT_STD_ATOMIC__

#include "../Defines.h"

#ifdef ALT_OS_WINDOWS
#include "../Platform/Impl/Win32/PlatformWin32.h"
#endif // ALT_OS_WINDOWS

#include "../Utils/NonCopyableMovable.h"

namespace Alt {

/*
 * https://cppreference.com/cpp/atomic/memory_order
 */
enum class MemoryOrder : usize {
    Relaxed = static_cast<usize>(Platform::MemoryOrder::Relaxed),
    Acquire = static_cast<usize>(Platform::MemoryOrder::Acquire),
    Release = static_cast<usize>(Platform::MemoryOrder::Release),
    AcqRel = static_cast<usize>(Platform::MemoryOrder::AcqRel),
    SeqCst = static_cast<usize>(Platform::MemoryOrder::SeqCst),
};

/**
 * @brief A atomic operations for a given type T.
 * @tparam T The underlying type of the atomic variable (e.g., u32, u64, ...).
 * @tparam IS_HARDWARE_SUPPORTED A compile-time bool indicating if the hardware supports standard atomic operations for type T.
 * @example
 * 
 * Alt::AtomicUsize counter = 0;
 * 
 * counter.FetchAdd(1, Alt::MemoryOrder::Relaxed);
 * assert(counter.Load() == 1);
 */
template <typename T, bool IS_HARDWARE_SUPPORTED = BitFlag::IsSetAnyEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>)>
class Atomic;

using AtomicBool = Atomic<bool>;

using AtomicU8 = Atomic<u8>;
using AtomicU16 = Atomic<u16>;
using AtomicU32 = Atomic<u32>;
using AtomicU64 = Atomic<u64>;

using AtomicS8 = Atomic<s8>;
using AtomicS16 = Atomic<s16>;
using AtomicS32 = Atomic<s32>;
using AtomicS64 = Atomic<s64>;

using AtomicUsize = Atomic<usize>;
using AtomicSsize = Atomic<ssize>;

/**
 * @brief Thread barrier based on the specified ordering.
 * @param order The 'MemoryOrder' to apply. Defaults to SeqCst.
 * @example
 * 
 * Alt::ThreadMemoryOrderBarrier(Alt::AtomicUsize::MemoryOrder::Release);
 */
ALT_FORCE_INLINE static void ThreadMemoryOrderBarrier(const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
    Platform::ThreadMemoryOrderBarrier(static_cast<Platform::MemoryOrder>(order));
}

/**
 * @brief A atomic operations for a given type T which has hardware support.
 * @tparam T The underlying type of the atomic variable (e.g., u32, u64, ...).
 */
template <typename T>
class Atomic<T, true> {
private:

    ALT_MAKE_CLASS_NON_MOVABLE(Atomic);
    ALT_MAKE_CLASS_NON_COPYABLE(Atomic);

public:

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedStore() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Store);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedLoad() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Load);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedExchange() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Exchange);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedCompareExchange() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::CompareExchange);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedAdditive() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Additive);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedAnd() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::And);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedOr() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Or);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedXor() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Xor);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedAll() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::All);
    }

    ALT_FORCE_INLINE Atomic() noexcept = default;

    /**
     * @brief Constructs an atomic with a specified initial value.
     * @param val The initial value to store in the atomic variable.
     * @example
     * 
     * Alt::AtomicUsize val = 4;
     */
    ALT_FORCE_INLINE Atomic(const T val) noexcept
        : m_Value(val)
    {}

    /**
     * @brief Stores a value.
     * @param val The new value to store.
     * @param order The memory ordering constraint. Default SeqCst.
     * @example
     * 
     * Alt::AtomicUsize val = 0;
     * val.Store(10);
     */
    template <typename U,
        std::enable_if_t<
            std::is_convertible_v<U, T> &&
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::Store)
        >* = nullptr>
    ALT_FORCE_INLINE void Store(const U val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicStore(&m_Value, val, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief Loads a value.
     * @param val The new value to store.
     * @param order The memory ordering constraint. Default SeqCst.
     * @example
     * 
     * Alt::AtomicUsize val = 0;
     * assert(val.Load() == 0);
     */
    template <typename U = T,
        std::enable_if_t<
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::Load)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T Load(const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicLoad(&m_Value, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief Exchanges the current value with a new value.
     * @param val The new value to exchange.
     * @param order The memory ordering constraint. Default SeqCst.
     * @example
     * 
     * Alt::AtomicUsize val = 0;
     * 
     * assert(val.Exchange(4) == 0);
     * assert(val.Load() == 4);
     */
    template <typename U = T,
        std::enable_if_t<
            std::is_convertible_v<U, T> &&
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::Exchange)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T Exchange(const U val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicExchange(&m_Value, val, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief CAS - compare current value to compare and on equality exchange the current value with a new value.
     * @param comapre The new value to exchange.
     * @param val The new value to exchange.
     * @param order The memory ordering constraint. Default SeqCst.
     * @example
     * 
     * Alt::AtomicUsize val = 0;
     * 
     * assert(val.CompareExchange(1, 4) != 1);
     * assert(val.CompareExchange(0, 4) == 0);
     * assert(val.Load() == 4);
     */
    template <typename U = T, typename U0 = T, typename U1 = T,
        std::enable_if_t<
            std::is_convertible_v<U0, T> &&
            std::is_convertible_v<U1, T> &&
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::CompareExchange)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T CompareExchange(
        const U0 compare,
        const U1 val,
        const MemoryOrder successOrder = MemoryOrder::SeqCst,
        const MemoryOrder failureOrder = MemoryOrder::SeqCst
    ) noexcept {
        return Platform::AtomicCompareExchange(&m_Value, compare, val, static_cast<Platform::MemoryOrder>(successOrder), static_cast<Platform::MemoryOrder>(failureOrder));
    }

    /**
     * @brief Exchange add.
     * @param val The value to add.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the addition.
     * @example
     * 
     * Alt::AtomicUsize val = 0;
     * 
     * assert(val.FetchAdd(4) == 0);
     * assert(val.Load() == 4);
     */
    template <typename U = T,
        std::enable_if_t<
            std::is_convertible_v<U, T> &&
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::Additive)
        >* = nullptr>
    ALT_FORCE_INLINE T FetchAdd(const U val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicAdd(&m_Value, val, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief Exchange sub.
     * @param val The value to sub.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the subtraction.
     * @example
     * 
     * Alt::AtomicUsize val = 4;
     * 
     * assert(val.FetchSub(2) == 4);
     * assert(val.Load() == 2);
     */
    template <typename U = T,
        std::enable_if_t<
            std::is_convertible_v<U, T> &&
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::Additive)
        >* = nullptr>
    ALT_FORCE_INLINE T FetchSub(const U val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicSub(&m_Value, val, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief Exchange increment.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the increment.
     * @example
     * 
     * Alt::AtomicUsize val = 0;
     * 
     * assert(val.FetchInc() == 0);
     * assert(val.Load() == 1);
     */
    template <typename U = T,
        std::enable_if_t<
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::Additive)
        >* = nullptr>
    ALT_FORCE_INLINE T FetchInc(const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicInc(&m_Value, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief Exchange decrement.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the decrement.
     * @example
     * 
     * Alt::AtomicUsize val = 4;
     * 
     * assert(val.FetchDec() == 4);
     * assert(val.Load() == 3);
     */
    template <typename U = T,
        std::enable_if_t<
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::Additive)
        >* = nullptr>
    ALT_FORCE_INLINE T FetchDec(const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicDec(&m_Value, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief Exchange and.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the and.
     * @example
     * 
     * Alt::AtomicUsize val = 4;
     * 
     * assert(val.FetchAnd(1) == 4);
     * assert(val.Load() == 0);
     */
    template <typename U = T,
        std::enable_if_t<
            std::is_convertible_v<U, T> &&
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::And)
        >* = nullptr>
    ALT_FORCE_INLINE T FetchAnd(const U val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicAnd(&m_Value, val, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief Exchange or.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the or.
     * @example
     * 
     * Alt::AtomicUsize val = 4;
     * 
     * assert(val.FetchOr(1) == 4);
     * assert(val.Load() == 5);
     */
    template <typename U = T,
        std::enable_if_t<
            std::is_convertible_v<U, T> &&
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::Or)
        >* = nullptr>
    ALT_FORCE_INLINE T FetchOr(const U val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicOr(&m_Value, val, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief Exchange xor.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the xor.
     * @example
     * 
     * Alt::AtomicUsize val = 4;
     * 
     * assert(val.FetchXor(4) == 4);
     * assert(val.Load() == 0);
     */
    template <typename U = T,
        std::enable_if_t<
            std::is_convertible_v<U, T> &&
            BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<U>, _PlatformAtomicDetails::SupportedOperationsTrait::Xor)
        >* = nullptr>
    ALT_FORCE_INLINE T FetchXor(const U val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicXor(&m_Value, val, static_cast<Platform::MemoryOrder>(order));
    }

private:

    alignas(_PlatformAtomicDetails::align_of_v<T>) T m_Value = T{};
};

template <>
class Atomic<bool, false> {
private:

    using T = u8;

public:

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedStore() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Store);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedLoad() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Load);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedExchange() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Exchange);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedCompareExchange() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::CompareExchange);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedAdditive() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedAnd() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::And);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedOr() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Or);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedXor() noexcept {
        return BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Xor);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedAll() noexcept {
        return false;
    }

    ALT_FORCE_INLINE Atomic() noexcept = default;

    /**
     * @brief Constructs an atomic with a specified initial value.
     * @param val The initial value to store in the atomic variable.
     * @example
     * 
     * Alt::AtomicBool val = true;
     */
    ALT_FORCE_INLINE Atomic(const bool val) noexcept
        : m_Value(val ? 1 : 0)
    {}

    /**
     * @brief Stores a value.
     * @param val The new value to store.
     * @param order The memory ordering constraint. Default SeqCst.
     * @example
     * 
     * Alt::AtomicBool val = false;
     * val.Store(true);
     */
    ALT_FORCE_INLINE void Store(const bool val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicStore(&m_Value, val ? 1 : 0, static_cast<Platform::MemoryOrder>(order));
    }

    /**
     * @brief Loads a value.
     * @param val The new value to store.
     * @param order The memory ordering constraint. Default SeqCst.
     * @example
     * 
     * Alt::AtomicBool val = false;
     * assert(val.Load() == false);
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Load(const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicLoad(&m_Value, static_cast<Platform::MemoryOrder>(order)) == 1;
    }

    /**
     * @brief Exchanges the current value with a new value.
     * @param val The new value to exchange.
     * @param order The memory ordering constraint. Default SeqCst.
     * @example
     * 
     * Alt::AtomicBool val = false;
     * 
     * assert(val.Exchange(true) == false);
     * assert(val.Load() == true);
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Exchange(const bool val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicExchange(&m_Value, val ? 1 : 0, static_cast<Platform::MemoryOrder>(order)) == 1;
    }

    /**
     * @brief CAS - compare current value to compare and on equality exchange the current value with a new value.
     * @param comapre The new value to exchange.
     * @param val The new value to exchange.
     * @param order The memory ordering constraint. Default SeqCst.
     * @example
     * 
     * Alt::AtomicBool val = false;
     * 
     * assert(val.CompareExchange(true, true) != true);
     * assert(val.CompareExchange(false, true) == false);
     * assert(val.Load() == true);
     */
    ALT_NODISCARD ALT_FORCE_INLINE T CompareExchange(
        const bool compare,
        const bool val,
        const MemoryOrder successOrder = MemoryOrder::SeqCst,
        const MemoryOrder failureOrder = MemoryOrder::SeqCst
    ) noexcept {
        return Platform::AtomicCompareExchange(&m_Value, compare ? 1 : 0, val ? 1 : 0, static_cast<Platform::MemoryOrder>(successOrder), static_cast<Platform::MemoryOrder>(failureOrder)) == 1;
    }

    /**
     * @brief Exchange and.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the and.
     * @example
     * 
     * Alt::AtomicBool val = true;
     * 
     * assert(val.FetchAnd(false) == true);
     * assert(val.Load() == false);
     */
    ALT_FORCE_INLINE T FetchAnd(const bool val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicAnd(&m_Value, val ? 1 : 0, static_cast<Platform::MemoryOrder>(order)) == 1;
    }

    /**
     * @brief Exchange or.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the or.
     * @example
     * 
     * Alt::AtomicBool val = false;
     * 
     * assert(val.FetchOr(true) == true);
     * assert(val.Load() == true);
     */
    ALT_FORCE_INLINE T FetchOr(const bool val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicOr(&m_Value, val ? 1 : 0, static_cast<Platform::MemoryOrder>(order)) == 1;
    }

    /**
     * @brief Exchange xor.
     * @param order The memory ordering constraint. Defaults to SeqCst.
     * @return The original value before the xor.
     * @example
     * 
     * Alt::AtomicBool val = true;
     * 
     * assert(val.FetchXor(true) == true);
     * assert(val.Load() == false);
     */
    ALT_FORCE_INLINE T FetchXor(const bool val, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {
        return Platform::AtomicXor(&m_Value, val ? 1 : 0, static_cast<Platform::MemoryOrder>(order)) == 1;
    }

private:

    alignas(_PlatformAtomicDetails::align_of_v<T>) T m_Value = T{};
};

template <typename T>
class Atomic<T, false> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedStore() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedLoad() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedExchange() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedCompareExchange() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedAdditive() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedAnd() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedOr() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedXor() noexcept {
        return false;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static bool IsHardwareSuppotedAll() noexcept {
        return false;
    }

    ALT_FORCE_INLINE Atomic() noexcept = default;
};

} // namespace Alt

#endif // __ALT_STD_ATOMIC__