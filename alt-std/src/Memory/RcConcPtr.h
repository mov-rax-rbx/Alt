#pragma once

#ifndef __ALT_STD_RC_CONC_PTR__
#define __ALT_STD_RC_CONC_PTR__

#include "../IfErr.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Memory/MaybeUninit.h"

#include "../Utils/Num.h"
#include "../Utils/Optional.h"
#include "../Utils/NonCopyableMovable.h"
#include "../Utils/EnumClassBitFlagsImpl.h"

#include "../Platform/Platform.h"
#include "../Concurrency/Backoff.h"

#ifdef ALT_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4324 /* structure was padded due to alignment specifier */)
#endif // ALT_COMPILER_MSVC

namespace Alt {

/**
 * @brief Configuration flags for Reference Counting behavior.
 * @details These flags determine if the RcConcPtr system supports weak pointers and 
 * performs arithmetic overflow checks during increment/decrement operations.
 */
enum class RcConcPtrTrait {

    None = 0,
    WeakPtr = 1 << 0, ///< Enables support for RcConcWeakPtr.
    OverflowCheck = 1 << 1, ///< Enables safety checks for counter overflows and disables constructor/assignment copy (use clonable trait with error check).

    All = WeakPtr | OverflowCheck,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(RcConcPtrTrait);

/**
 * @brief Default Rc Memory Layout Controller which inlines data and rc.
 * @tparam T The type of the object.
 * @tparam IsWeak Implementation of weak counter status (true if RcConcPtrTrait::WeakPtr is set).
 * @details This controller places the Strong and Weak counters in the same memory 
 * block as the data `T`.
 */
template <typename T, bool IS_WEAK_PTR>
struct RcConcIntrusiveMemoryLayoutController;

/**
 * @brief Default Rc Memory Layout Controller which keeps data and rc separately (2 alloctions).
 * @tparam T The type of the object.
 * @tparam IsWeak Implementation of weak counter status (true if RcConcPtrTrait::WeakPtr is set).
 * @details This controller separates the Reference Counter "Control Block" from 
 * the actual data `T`. This allows the count to be allocated in a different memory 
 * pool or region than the object itself.
 */
template <typename T, bool IS_WEAK_PTR>
struct RcControlBlockMemoryLayoutController;

/**
 * @brief Thread-safe smart pointer for managing objects with shared ownership via reference counting.
 * @tparam T The underlying type of the object.
 * @tparam TRAIT Configuration flags (e.g., WeakPtr, OverflowCheck).
 * @tparam RcConcMemoryLayoutController The strategy used to determine memory layout 
 * (e.g., RcConcIntrusiveMemoryLayoutController or RcControlBlockMemoryLayoutController, ...).
 * @example
 * 
 * iferr_scope;
 * 
 * RcConcPtr<Alt::String> ptr = RcConcPtr<Alt::String>::AllocateObject("Reference Counter"_salloc) iferr_return;
 * assert(*ptr == "Reference Counter");
 * 
 * RcConcPtr<Alt::String> ptr1 = ptr.Clone() iferr_return;
 * assert(ptr1 == ptr);
 * 
 * RcConcWeakPtr<Alt::String> weak = ptr.AsWeak() iferr_return;
 * RcConcPtr<Alt::String> ptr2 = weak.AsRc() iferr_return;
 * assert(ptr2 == ptr);
 */
template <
    typename T,
    RcConcPtrTrait TRAIT = RcConcPtrTrait::WeakPtr,
    typename RcConcMemoryLayoutController = RcConcIntrusiveMemoryLayoutController<T, BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::WeakPtr)>
>
class RcConcPtr;

/**
 * @brief Thread-safe weak reference to an object managed by RcConcPtr.
 * @tparam T The underlying type of the object.
 * @tparam TRAIT Configuration flags (e.g., WeakPtr).
 * @tparam RcConcMemoryLayoutController The layout strategy for memory allocation.
 * @example
 * 
 * iferr_scope;
 * 
 * RcConcWeakPtr<Alt::String> weak = rcPtr.AsWeak() iferr_return;
 * 
 * RcConcPtr<Alt::String> rcPtr = weak.AsRc() ifok {
 *     assert(rcPtr);
 * }
 */
template <
    typename T,
    RcConcPtrTrait TRAIT = RcConcPtrTrait::WeakPtr,
    typename RcConcMemoryLayoutController = RcConcIntrusiveMemoryLayoutController<T, BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::WeakPtr)>
>
class RcConcWeakPtr;

/*
 * To control the memory layout of Reference Counters, use the 'RcConcMemoryLayoutController' "interface".
 * By default available 2 implementations: 'Intrusive' (strong and weak counters inlined with data) and
 * 'ControlBlock' (strong and weak counters allocated separately from data).
 * 
 * In the case of 'Intrusive' - where some entities have counters inlined directly into the entity itself -
 * you must implement getters/setters to retrieve those counters.
 * 
 * If you wish to use custom allocators for entities or control blocks, you cnan extend respectively.
 * 
 * The most important invariant: Getters/setters MUST check object availability. Before 'AllocateStrongWeakRefAndSet(...)',
 * all MUST return 'Nullopt' because no data exists. After 'AllocateStrongWeakRefAndSet(...)', they are able to return valid
 * pointers and counter references. After 'Deallocate...(...)', internal counters are assigned default values; in
 * this case, getters/setters MUST return 'Nullopt'.
 * 
 * Lifecycle:
 * 
 * Before any 'AllocateStrongWeakRefAndSet(...)', we have default values and MUST return 'Nullopt' for getters/setters.
 * On 'AllocateStrongWeakRefAndSet(...)', we return a new 'StrongRef' and getters/setters use actual values.
 * 
 * Different paths for Non-Weak and Weak implementations:
 * 
 * Weak:
 *     On 'DeallocateObject(...)', the object is deallocated/destroyed and 'StrongRef' is assigned its default value, 
 *     so subsequent calls to 'GetObjectPtr()', 'GetStrongCounterRef()', or 'GetWeakCounterRef()' MUST return Nullopt or 0.
 * 
 *     On 'DeallocateWeakCounter(...)', the weak counter is deallocated and any other necessary objects are destroyed
 *     (final deallocation).
 * 
 * Non-Weak:
 *     On 'DeallocateStrongRef(...)', all data is deallocated, and 'StrongRef' is assigned its default value, 
 *     so subsequent calls to 'GetObjectPtr()', 'GetStrongCounterRef()' MUST return Nullopt or 0.
 */
template <typename T>
struct RcConcIntrusiveMemoryLayoutController<T, false> {

    struct StrongRefData;

    using StrongRef = StrongRefData;
    using StrongCounter = usize;
    using WeakCounter = Unit_t;

    struct StrongRefControlBlock {

        alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) StrongCounter strongCounter = 0;

        alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) T object;

        ALT_MAKE_CLASS_NON_COPYABLE(StrongRefControlBlock);
        ALT_MAKE_CLASS_NON_MOVABLE(StrongRefControlBlock);

        template <typename ...ArgsT>
        ALT_FORCE_INLINE StrongRefControlBlock(const StrongCounter strongCounter, ArgsT&&... args) noexcept
            : strongCounter(strongCounter)
            , object(std::forward<ArgsT>(args)...)
        {}
    };

    struct StrongRefData {

        StrongRefControlBlock* pStrongRef = nullptr;

        ALT_FORCE_INLINE StrongRefData() noexcept = default;

        ALT_FORCE_INLINE StrongRefData(StrongRefControlBlock* pStrongRef) noexcept
            : pStrongRef(pStrongRef)
        {}
    };

    template <typename ...ArgsT>
    ALT_FORCE_INLINE static Optional<StrongRef> AllocateStrongWeakRefAndSet(ArgsT&&... args) noexcept {
        iferr_scope;

        StrongRef strongRef{};
        strongRef.pStrongRef = Memory::AllocateObject<StrongRefControlBlock>(1, std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return strongRef;
    }

    ALT_FORCE_INLINE static void DeallocateStrongRef(StrongRef& strongRef) noexcept {

        Memory::DeallocateObject(strongRef.pStrongRef);
    }

    ALT_FORCE_INLINE static Optional<T*> GetObjectPtr(StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(&strongRef.pStrongRef->object) : Nullopt;
    }

    ALT_FORCE_INLINE static const Optional<T*> GetObjectPtr(const StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(&strongRef.pStrongRef->object) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<StrongCounter&> GetStrongCounterRef(StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(strongRef.pStrongRef->strongCounter) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<const StrongCounter&> GetStrongCounterRef(const StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(strongRef.pStrongRef->strongCounter) : Nullopt;
    }
};

template <typename T>
struct RcConcIntrusiveMemoryLayoutController<T, true> {

    struct StrongRefData;

    using StrongRef = StrongRefData;
    using StrongCounter = usize;
    using WeakCounter = usize;

    struct StrongRefControlBlock {

        alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) StrongCounter strongCounter = 0;
        alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) WeakCounter weakCounter = 0;

        alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) MaybeUninit<T> object;

        ALT_MAKE_CLASS_NON_COPYABLE(StrongRefControlBlock);
        ALT_MAKE_CLASS_NON_MOVABLE(StrongRefControlBlock);

        ALT_FORCE_INLINE StrongRefControlBlock() noexcept = default;

        ALT_FORCE_INLINE StrongRefControlBlock(const StrongCounter strongCounter, const WeakCounter weakCounter) noexcept
            : strongCounter(strongCounter)
            , weakCounter(weakCounter)
        {}
    };

    struct StrongRefData {

        StrongRefControlBlock* pStrongRef = nullptr;

        ALT_FORCE_INLINE StrongRefData() noexcept = default;

        ALT_FORCE_INLINE StrongRefData(StrongRefControlBlock* pStrongRef) noexcept
            : pStrongRef(pStrongRef)
        {}
    };

    template <typename ...ArgsT>
    ALT_FORCE_INLINE static Optional<StrongRef> AllocateStrongWeakRefAndSet(ArgsT&&... args) noexcept {
        iferr_scope;

        StrongRef strongRef{};
        strongRef.pStrongRef = Memory::AllocateObject<StrongRefControlBlock>(1, 1) iferr_return Nullopt;
        strongRef.pStrongRef->object.Construct(std::forward<ArgsT>(args)...);

        return strongRef;
    }

    ALT_FORCE_INLINE static void DeallocateObject(StrongRef& strongRef) noexcept {

        strongRef.pStrongRef->object.Destroy();
    }

    ALT_FORCE_INLINE static void DeallocateWeakCounter(StrongRef& strongRef) noexcept {

        Memory::DeallocateObject(strongRef.pStrongRef);
    }

    ALT_FORCE_INLINE static Optional<T*> GetObjectPtr(StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(strongRef.pStrongRef->object.GetPtr()) : Nullopt;
    }

    ALT_FORCE_INLINE static const Optional<T*> GetObjectPtr(const StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(strongRef.pStrongRef->object.GetPtr()) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<StrongCounter&> GetStrongCounterRef(StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(strongRef.pStrongRef->strongCounter) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<const StrongCounter&> GetStrongCounterRef(const StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(strongRef.pStrongRef->strongCounter) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<WeakCounter&> GetWeakCounterRef(StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(strongRef.pStrongRef->weakCounter) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<const WeakCounter&> GetWeakCounterRef(const StrongRef& strongRef) noexcept {
        return strongRef.pStrongRef ? MakeOptional(strongRef.pStrongRef->weakCounter) : Nullopt;
    }
};

template <typename T>
struct RcControlBlockMemoryLayoutController<T, false> : public RcConcIntrusiveMemoryLayoutController<T, false> {

    using Super = RcConcIntrusiveMemoryLayoutController<T, false>;
    using Super::Super;
};

template <typename T>
struct RcControlBlockMemoryLayoutController<T, true> {

    struct StrongRefData;

    using StrongRef = StrongRefData;
    using StrongCounter = usize;
    using WeakCounter = usize;

    struct StrongRefControlBlock {

        alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) StrongCounter strongCounter = 0;
        alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) WeakCounter weakCounter = 0;

        ALT_MAKE_CLASS_NON_COPYABLE(StrongRefControlBlock);
        ALT_MAKE_CLASS_NON_MOVABLE(StrongRefControlBlock);

        ALT_FORCE_INLINE StrongRefControlBlock() noexcept = default;

        ALT_FORCE_INLINE StrongRefControlBlock(const StrongCounter strongCounter, const WeakCounter weakCounter) noexcept
            : strongCounter(strongCounter)
            , weakCounter(weakCounter)
        {}
    };

    struct StrongRefData {

        T* pObject = nullptr;
        StrongRefControlBlock* pControlBlock = nullptr;

        ALT_FORCE_INLINE StrongRefData() noexcept = default;
    };

    template <typename ...ArgsT>
    ALT_FORCE_INLINE static Optional<StrongRef> AllocateStrongWeakRefAndSet(ArgsT&&... args) noexcept {

        StrongRef strongRef{};
        iferr_scope_on_err {

            if (strongRef.pObject) {

                Memory::DeallocateObject(strongRef.pObject);
            }

            if (strongRef.pControlBlock) {

                Memory::DeallocateObject(strongRef.pControlBlock);
            }
        };

        strongRef.pObject = Memory::AllocateObject<T>(std::forward<ArgsT>(args)...) iferr_return Nullopt;
        strongRef.pControlBlock = Memory::AllocateObject<StrongRefControlBlock>(1, 1) iferr_return Nullopt;

        return strongRef;
    }

    ALT_FORCE_INLINE static void DeallocateObject(StrongRef& strongRef) noexcept {

        Memory::DeallocateObject(strongRef.pObject);
    }

    ALT_FORCE_INLINE static void DeallocateWeakCounter(StrongRef& strongRef) noexcept {

        Memory::DeallocateObject(strongRef.pControlBlock);
    }

    ALT_FORCE_INLINE static Optional<T*> GetObjectPtr(StrongRef& strongRef) noexcept {
        return strongRef.pObject ? MakeOptional(strongRef.pObject) : Nullopt;
    }

    ALT_FORCE_INLINE static const Optional<T*> GetObjectPtr(const StrongRef& strongRef) noexcept {
        return strongRef.pObject ? MakeOptional(strongRef.pObject) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<StrongCounter&> GetStrongCounterRef(StrongRef& strongRef) noexcept {
        return strongRef.pControlBlock ? MakeOptional(strongRef.pControlBlock->strongCounter) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<const StrongCounter&> GetStrongCounterRef(const StrongRef& strongRef) noexcept {
        return strongRef.pControlBlock ? MakeOptional(strongRef.pControlBlock->strongCounter) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<WeakCounter&> GetWeakCounterRef(StrongRef& strongRef) noexcept {
        return strongRef.pControlBlock ? MakeOptional(strongRef.pControlBlock->weakCounter) : Nullopt;
    }

    ALT_FORCE_INLINE static Optional<const WeakCounter&> GetWeakCounterRef(const StrongRef& strongRef) noexcept {
        return strongRef.pControlBlock ? MakeOptional(strongRef.pControlBlock->weakCounter) : Nullopt;
    }
};

namespace TypeTraits {

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
struct __is_smart_pointer__<RcConcPtr<T, TRAIT, RcConcMemoryLayoutController>>
    : std::true_type
{};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
struct __is_trivially_reallocatable__<RcConcPtr<T, TRAIT, RcConcMemoryLayoutController>>
    : std::conjunction<
        is_trivially_reallocatable_t<RcConcMemoryLayoutController>,
        is_trivially_reallocatable_t<typename RcConcMemoryLayoutController::StrongRef>
    >
{};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
struct __is_trivially_reallocatable__<RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController>>
    : std::conjunction<
        is_trivially_reallocatable_t<RcConcMemoryLayoutController>,
        is_trivially_reallocatable_t<typename RcConcMemoryLayoutController::StrongRef>
    >
{};

} // namespace TypeTraits

/** \cond */
namespace _RcConcPtrDetails {

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController, bool IS_WEAK_PTR = BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::WeakPtr)>
class Ptr;

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class Ptr<T, TRAIT, RcConcMemoryLayoutController, false> : public RcConcMemoryLayoutController {
public:

    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;
    using StrongCounter = typename RcConcMemoryLayoutController::StrongCounter;

    ALT_FORCE_INLINE Ptr() noexcept = default;

    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcConcMemoryLayoutControllerT, RcConcMemoryLayoutController>>* = nullptr>
    ALT_FORCE_INLINE Ptr(StrongRef&& strongRef, RcConcMemoryLayoutControllerT&& controller) noexcept
        : RcConcMemoryLayoutController(std::forward<RcConcMemoryLayoutControllerT>(controller))
        , m_StrongRef(std::move(strongRef))
    {}

    ALT_FORCE_INLINE RcConcMemoryLayoutController& AsRcConcMemoryLayoutController() noexcept {
        return static_cast<RcConcMemoryLayoutController&>(*this);
    }

    ALT_FORCE_INLINE const RcConcMemoryLayoutController& AsRcConcMemoryLayoutController() const noexcept {
        return static_cast<const RcConcMemoryLayoutController&>(*this);
    }

    /**
     * 'IncStrongRef()' called on sharing between threads, so 'MemoryOrder::Relaxed' enought; for in thread sharing also 'MemoryOrder::Relaxed' enought.
     * 
     * https://www.boost.org/doc/libs/latest/libs/atomic/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_reference_counters.implementation
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IncStrongRef() noexcept {
        iferr_scope;

        StrongCounter* pStrongCounterRef = AsRcConcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return true;

        if constexpr (BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)) {

            StrongCounter strongCounterRef = Platform::AtomicLoad(pStrongCounterRef, Platform::MemoryOrder::Relaxed);

            if (strongCounterRef == Num::Desc<StrongCounter>::Max()) {
                return false;
            }

            Backoff backoff{};

            for (;;) {

                const StrongCounter strongCounterRefNew = Platform::AtomicCompareExchange(
                    pStrongCounterRef,
                    strongCounterRef,
                    strongCounterRef + 1,
                    Platform::MemoryOrder::Relaxed,
                    Platform::MemoryOrder::Relaxed
                );

                if (strongCounterRef == strongCounterRefNew) {
                    break;
                }

                strongCounterRef = strongCounterRefNew;
                if (strongCounterRef == Num::Desc<StrongCounter>::Max()) {
                    return false;
                }

                backoff.SpinLight();
            }
        }
        else {

            Platform::AtomicInc(pStrongCounterRef, Platform::MemoryOrder::Relaxed);
        }

        return true;
    }

    /**
     * Sync with 'IncStrongRef()' - this dec must be after, so 'MemoryOrder::Release'.
     */
    ALT_FORCE_INLINE void DecStrongRef() noexcept {
        iferr_scope;

        StrongCounter* pStrongCounterRef = AsRcConcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return;
        const StrongCounter strongCounterRef = Platform::AtomicDec(pStrongCounterRef, Platform::MemoryOrder::Release);

        if (strongCounterRef == 1) {

            // https://www.boost.org/doc/libs/latest/libs/atomic/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_reference_counters.implementation
            Platform::ThreadMemoryOrderBarrier(Platform::MemoryOrder::Acquire);

            AsRcConcMemoryLayoutController().DeallocateStrongRef(m_StrongRef);
        }

        // Decrementing only when outer object destroyed, so just reset 'StrongRef' to be robust for other 'DecStrongRef()'
        m_StrongRef = StrongRef{};
    }

    ALT_FORCE_INLINE T* Get() noexcept {
        iferr_scope;

        T* ptr = AsRcConcMemoryLayoutController().GetObjectPtr(m_StrongRef) iferr_return nullptr;
        return ptr;
    }

    ALT_FORCE_INLINE const T* Get() const noexcept {
        iferr_scope;

        const T* ptr = AsRcConcMemoryLayoutController().GetObjectPtr(m_StrongRef) iferr_return nullptr;
        return ptr;
    }

    /**
     * 'Relaxed' because we don't care at all
     */
    ALT_FORCE_INLINE usize GetStrongCount() const noexcept {
        iferr_scope;

        const StrongCounter* pStrongCounterRef = AsRcConcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return 0;
        return Platform::AtomicLoad(pStrongCounterRef, Platform::MemoryOrder::Relaxed);
    }

public:

    StrongRef m_StrongRef;
};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class Ptr<T, TRAIT, RcConcMemoryLayoutController, true> : public RcConcMemoryLayoutController {
public:

    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;
    using StrongCounter = typename RcConcMemoryLayoutController::StrongCounter;
    using WeakCounter = typename RcConcMemoryLayoutController::WeakCounter;

    ALT_FORCE_INLINE Ptr() noexcept = default;

    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcConcMemoryLayoutControllerT, RcConcMemoryLayoutController>>* = nullptr>
    ALT_FORCE_INLINE Ptr(StrongRef&& strongRef, RcConcMemoryLayoutControllerT&& controller) noexcept
        : RcConcMemoryLayoutController(std::forward<RcConcMemoryLayoutControllerT>(controller))
        , m_StrongRef(std::move(strongRef))
    {}

    ALT_FORCE_INLINE RcConcMemoryLayoutController& AsRcConcMemoryLayoutController() noexcept {
        return static_cast<RcConcMemoryLayoutController&>(*this);
    }

    ALT_FORCE_INLINE const RcConcMemoryLayoutController& AsRcConcMemoryLayoutController() const noexcept {
        return static_cast<const RcConcMemoryLayoutController&>(*this);
    }

    /**
     * 'IncStrongRef()' called on sharing between threads, so 'MemoryOrder::Relaxed' enought; for in thread sharing also 'MemoryOrder::Relaxed' enought.
     * 
     * https://www.boost.org/doc/libs/latest/libs/atomic/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_reference_counters.implementation
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IncStrongRef() noexcept {
        iferr_scope;

        StrongCounter* pStrongCounterRef = AsRcConcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return true;

        if constexpr (BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)) {

            StrongCounter strongCounterRef = Platform::AtomicLoad(pStrongCounterRef, Platform::MemoryOrder::Relaxed);

            if (strongCounterRef == Num::Desc<StrongCounter>::Max()) {
                return false;
            }

            Backoff backoff{};

            for (;;) {

                const StrongCounter strongCounterRefNew = Platform::AtomicCompareExchange(
                    pStrongCounterRef,
                    strongCounterRef,
                    strongCounterRef + 1,
                    Platform::MemoryOrder::Relaxed,
                    Platform::MemoryOrder::Relaxed
                );

                if (strongCounterRef == strongCounterRefNew) {
                    break;
                }

                strongCounterRef = strongCounterRefNew;
                if (strongCounterRef == Num::Desc<StrongCounter>::Max()) {
                    return false;
                }

                backoff.SpinLight();
            }
        }
        else {

            Platform::AtomicInc(pStrongCounterRef, Platform::MemoryOrder::Relaxed);
        }

        return true;
    }

    /**
     * Sync with 'IncStrongRef()' - this dec must be after, so 'MemoryOrder::Release'.
     * Sync with 'IncWeakRef()' + https://www.boost.org/doc/libs/latest/libs/atomic/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_reference_counters.implementation, so 'MemoryOrder::AcqRel'.
     */
    ALT_FORCE_INLINE void DecStrongRef() noexcept {
        iferr_scope;

        StrongCounter* pStrongCounterRef = AsRcConcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return;
        const StrongCounter strongCounterRef = Platform::AtomicDec(pStrongCounterRef, Platform::MemoryOrder::Release);

        if (strongCounterRef == 1) {

            WeakCounter* pWeakCounterRef = AsRcConcMemoryLayoutController().GetWeakCounterRef(m_StrongRef);
            ALT_ASSERT(pWeakCounterRef);

            const WeakCounter weakCounterRef = Platform::AtomicDec(pWeakCounterRef, Platform::MemoryOrder::AcqRel);

            AsRcConcMemoryLayoutController().DeallocateObject(m_StrongRef);

            if (weakCounterRef == 1) {

                AsRcConcMemoryLayoutController().DeallocateWeakCounter(m_StrongRef);
            }
        }

        // Decrementing only when object destroyed, so just reset 'StrongRef' to be robust for other 'DecStrongRef()'
        m_StrongRef = StrongRef{};
    }

    /**
     * 'IncWeakRef()' called on sharing between threads, so 'MemoryOrder::Relaxed' enought; for in thread sharing also 'MemoryOrder::Relaxed' enought.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IncWeakRef() noexcept {
        iferr_scope;

        WeakCounter* pWeakCounterRef = AsRcConcMemoryLayoutController().GetWeakCounterRef(m_StrongRef) iferr_return true;

        if constexpr (BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)) {

            WeakCounter weakCounterRef = Platform::AtomicLoad(pWeakCounterRef, Platform::MemoryOrder::Relaxed);
            if (weakCounterRef == Num::Desc<WeakCounter>::Max()) {
                return false;
            }

            Backoff backoff{};

            for (;;) {

                const WeakCounter weakCounterRefNew = Platform::AtomicCompareExchange(
                    pWeakCounterRef,
                    weakCounterRef,
                    weakCounterRef + 1,
                    Platform::MemoryOrder::Relaxed,
                    Platform::MemoryOrder::Relaxed
                );

                if (weakCounterRef == weakCounterRefNew) {
                    break;
                }

                weakCounterRef = weakCounterRefNew;
                if (weakCounterRef == Num::Desc<WeakCounter>::Max()) {
                    return false;
                }

                backoff.SpinLight();
            }
        }
        else {

            Platform::AtomicInc(pWeakCounterRef, Platform::MemoryOrder::Relaxed);
        }

        return true;
    }

    /**
     * Sync with 'IncWeakRef()' - this dec must be after, so 'MemoryOrder::Release'. Sync with 'DecStrongRef()'
     */
    ALT_FORCE_INLINE void DecWeakRef() noexcept {
        iferr_scope;

        WeakCounter* pWeakCounterRef = AsRcConcMemoryLayoutController().GetWeakCounterRef(m_StrongRef) iferr_return;
        const WeakCounter weakCounterRef = Platform::AtomicDec(pWeakCounterRef, Platform::MemoryOrder::Release);

        if (weakCounterRef == 1) {

            // https://www.boost.org/doc/libs/latest/libs/atomic/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_reference_counters.implementation
            Platform::ThreadMemoryOrderBarrier(Platform::MemoryOrder::Acquire);

            AsRcConcMemoryLayoutController().DeallocateWeakCounter(m_StrongRef);
        }

        // Decrementing only when weak object destroyed, so just reset 'StrongRef' to be robust for other 'DecWeakRef()'
        m_StrongRef = StrongRef{};
    }

    /**
     * Sync with 'DecStrongRef()', so 'MemoryOrder::Acquire'
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool TryMakeStrongRef() noexcept {
        iferr_scope;

        StrongCounter* pStrongCounterRef = AsRcConcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return false;

        StrongCounter strongCounterRef = Platform::AtomicLoad(pStrongCounterRef, Platform::MemoryOrder::Relaxed);

        if constexpr (BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)) {

            if (strongCounterRef == 0 || strongCounterRef == Num::Desc<StrongCounter>::Max()) {
                return false;
            }
        }
        else {

            if (strongCounterRef == 0) {
                return false;
            }
        }

        Backoff backoff{};

        for (;;) {

            const StrongCounter strongCounterRefNew = Platform::AtomicCompareExchange(
                pStrongCounterRef,
                strongCounterRef,
                strongCounterRef + 1,
                Platform::MemoryOrder::Acquire,
                Platform::MemoryOrder::Relaxed
            );

            if (strongCounterRef == strongCounterRefNew) {
                break;
            }

            strongCounterRef = strongCounterRefNew;

            if constexpr (BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)) {

                if (strongCounterRef == 0 || strongCounterRef == Num::Desc<StrongCounter>::Max()) {
                    return false;
                }
            }
            else {

                if (strongCounterRef == 0) {
                    return false;
                }
            }

            backoff.SpinLight();
        }

        return true;
    }

    ALT_FORCE_INLINE T* Get() noexcept {
        iferr_scope;

        T* ptr = AsRcConcMemoryLayoutController().GetObjectPtr(m_StrongRef) iferr_return nullptr;
        return ptr;
    }

    ALT_FORCE_INLINE const T* Get() const noexcept {
        iferr_scope;

        const T* ptr = AsRcConcMemoryLayoutController().GetObjectPtr(m_StrongRef) iferr_return nullptr;
        return ptr;
    }

    /**
     * 'Relaxed' because we don't care at all
     */
    ALT_FORCE_INLINE usize GetStrongCount() const noexcept {
        iferr_scope;

        const StrongCounter* pStrongCounterRef = AsRcConcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return 0;
        return Platform::AtomicLoad(pStrongCounterRef, Platform::MemoryOrder::Relaxed);
    }

    /**
     * 'Relaxed' because we don't care at all
     */
    ALT_FORCE_INLINE usize GetWeakCount() const noexcept {
        iferr_scope;

        const WeakCounter* pWeakCounterRef = AsRcConcMemoryLayoutController().GetWeakCounterRef(m_StrongRef) iferr_return 0;
        return Platform::AtomicLoad(pWeakCounterRef, Platform::MemoryOrder::Relaxed);
    }

public:

    StrongRef m_StrongRef;
};

struct Rule5Copy_t {};
constexpr static Rule5Copy_t Rule5Copy = Rule5Copy_t{};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcPtrResolverBase {
private:

    using Ptr = Ptr<T, TRAIT, RcConcMemoryLayoutController>;
    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;

public:

    ALT_FORCE_INLINE RcConcPtrResolverBase() noexcept = default;

    ALT_FORCE_INLINE RcConcPtrResolverBase(RcConcPtrResolverBase&& rhs) noexcept
        : m_Ptr(Exchange(rhs.m_Ptr.m_StrongRef, StrongRef{}), std::move(rhs.m_Ptr.AsRcConcMemoryLayoutController()))
    {}

    ALT_FORCE_INLINE RcConcPtrResolverBase& operator=(RcConcPtrResolverBase&& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return *this;
        }

        m_Ptr.DecStrongRef();

        m_Ptr.m_StrongRef = Exchange(rhs.m_Ptr.m_StrongRef, StrongRef{});
        m_Ptr.AsRcConcMemoryLayoutController() = std::move(rhs.m_Ptr.AsRcConcMemoryLayoutController());

        return *this;
    }

    ALT_FORCE_INLINE RcConcPtrResolverBase(std::nullptr_t) noexcept
    {}

    ALT_FORCE_INLINE RcConcPtrResolverBase& operator=(std::nullptr_t) noexcept {

        m_Ptr.DecStrongRef();

        m_Ptr.m_StrongRef = StrongRef{};

        return *this;
    }

protected:

    ALT_FORCE_INLINE RcConcPtrResolverBase(Ptr&& ptr) noexcept
        : m_Ptr(std::move(ptr))
    {}

    ALT_FORCE_INLINE RcConcPtrResolverBase(const Ptr& ptr) noexcept
        : m_Ptr(ptr)
    {}

    ALT_FORCE_INLINE RcConcPtrResolverBase(Rule5Copy_t, const RcConcPtrResolverBase& rhs) noexcept
        : m_Ptr(rhs.m_Ptr)
    {
        ALT_MAYBE_UNUSED const bool isOverflow = m_Ptr.IncStrongRef();
    }

    ALT_FORCE_INLINE bool Assign(Rule5Copy_t, const RcConcPtrResolverBase& rhs) noexcept {
        iferr_scope;

        if (static_cast<const void*>(this) == &rhs) {
            return true;
        }

        m_Ptr.DecStrongRef();

        m_Ptr.m_StrongRef = rhs.m_Ptr.m_StrongRef;
        m_Ptr.IncStrongRef() iferr {

            m_Ptr.m_StrongRef = StrongRef{};
            return false;
        }

        return true;
    }

protected:

    Ptr m_Ptr;
};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController, bool IS_WEAK_PTR = BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::WeakPtr)>
class RcConcPtrWeakResolver;

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcPtrWeakResolver<T, TRAIT, RcConcMemoryLayoutController, false> : public RcConcPtrResolverBase<T, TRAIT, RcConcMemoryLayoutController> {
private:

    using Super = RcConcPtrResolverBase<T, TRAIT, RcConcMemoryLayoutController>;

public:

    using Super::Super;
};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcPtrWeakResolver<T, TRAIT, RcConcMemoryLayoutController, true> : public RcConcPtrResolverBase<T, TRAIT, RcConcMemoryLayoutController> {
private:

    using Super = RcConcPtrResolverBase<T, TRAIT, RcConcMemoryLayoutController>;
    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;

public:

    using Super::Super;

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcConcPtr<T, TRAIT, RcConcMemoryLayoutController>> FromWeak(const RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController>& weak) noexcept {
        iferr_scope;

        RcConcPtr<T, TRAIT, RcConcMemoryLayoutController> result(weak.m_Ptr);
        result.m_Ptr.TryMakeStrongRef() iferr {

            result.m_Ptr.m_StrongRef = StrongRef{};
            return Nullopt;
        }

        return result;
    }

    template <typename U = T,
        std::enable_if_t<std::is_same_v<U, T> && BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Optional<RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController>> AsWeak() const noexcept {

        return RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController>::FromRc(static_cast<const RcConcPtr<T, TRAIT, RcConcMemoryLayoutController>&>(*this));
    }

    template <typename U = T,
        std::enable_if_t<std::is_same_v<U, T> && !BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController> AsWeak() const noexcept {

        return RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController>::FromRc(static_cast<const RcConcPtr<T, TRAIT, RcConcMemoryLayoutController>&>(*this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetWeakCount() const noexcept {

        return this->m_Ptr.GetWeakCount();
    }
};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController, bool IS_OVERFLOW_CHECK = BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)>
class RcConcPtrOverflowResolver;

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcPtrOverflowResolver<T, TRAIT, RcConcMemoryLayoutController, false> : public RcConcPtrWeakResolver<T, TRAIT, RcConcMemoryLayoutController> {
private:

    using Super = RcConcPtrWeakResolver<T, TRAIT, RcConcMemoryLayoutController>;
    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;

public:

    using Super::Super;

    ALT_FORCE_INLINE RcConcPtrOverflowResolver() noexcept = default;

    ALT_FORCE_INLINE RcConcPtrOverflowResolver(RcConcPtrOverflowResolver&&) noexcept = default;
    ALT_FORCE_INLINE RcConcPtrOverflowResolver& operator=(RcConcPtrOverflowResolver&&) noexcept = default;

    ALT_FORCE_INLINE RcConcPtrOverflowResolver(const RcConcPtrOverflowResolver& rhs) noexcept
        : Super(Rule5Copy, rhs)
    {}

    ALT_FORCE_INLINE RcConcPtrOverflowResolver& operator=(const RcConcPtrOverflowResolver& rhs) noexcept {

        this->Assign(Rule5Copy, rhs);
        return *this;
    }
};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcPtrOverflowResolver<T, TRAIT, RcConcMemoryLayoutController, true> : public RcConcPtrWeakResolver<T, TRAIT, RcConcMemoryLayoutController> {
private:

    using Super = RcConcPtrWeakResolver<T, TRAIT, RcConcMemoryLayoutController>;
    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;

public:

    using Super::Super;

    ALT_FORCE_INLINE RcConcPtrOverflowResolver() noexcept = default;

    ALT_FORCE_INLINE RcConcPtrOverflowResolver(RcConcPtrOverflowResolver&&) noexcept = default;
    ALT_FORCE_INLINE RcConcPtrOverflowResolver& operator=(RcConcPtrOverflowResolver&&) noexcept = default;

    /*
     * Use 'Clone()' to check overflow on increments
     */
    RcConcPtrOverflowResolver(const RcConcPtrOverflowResolver&) noexcept = delete;
    RcConcPtrOverflowResolver& operator=(const RcConcPtrOverflowResolver&) noexcept = delete;
};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcWeakPtrResolverBase {
private:

    using Ptr = _RcConcPtrDetails::Ptr<T, TRAIT, RcConcMemoryLayoutController>;
    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;

public:

    ALT_FORCE_INLINE RcConcWeakPtrResolverBase() noexcept = default;

    ALT_FORCE_INLINE RcConcWeakPtrResolverBase(RcConcWeakPtrResolverBase&& rhs) noexcept
        : m_Ptr(Exchange(rhs.m_Ptr.m_StrongRef, StrongRef{}), std::move(rhs.m_Ptr.AsRcConcMemoryLayoutController()))
    {}

    ALT_FORCE_INLINE RcConcWeakPtrResolverBase& operator=(RcConcWeakPtrResolverBase&& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return *this;
        }

        m_Ptr.DecWeakRef();

        m_Ptr.m_StrongRef = Exchange(rhs.m_Ptr.m_StrongRef, StrongRef{});
        m_Ptr.AsRcConcMemoryLayoutController() = std::move(rhs.m_Ptr.AsRcConcMemoryLayoutController());

        return *this;
    }

protected:

    ALT_FORCE_INLINE RcConcWeakPtrResolverBase(const Ptr& ptr) noexcept
        : m_Ptr(ptr)
    {}

    ALT_FORCE_INLINE RcConcWeakPtrResolverBase(Rule5Copy_t, const RcConcWeakPtrResolverBase& rhs) noexcept
        : m_Ptr(rhs.m_Ptr)
    {
        ALT_MAYBE_UNUSED const bool isOverflow = m_Ptr.IncWeakRef();
    }

    ALT_FORCE_INLINE bool Assign(Rule5Copy_t, const RcConcWeakPtrResolverBase& rhs) noexcept {
        iferr_scope;

        if (static_cast<const void*>(this) == &rhs) {
            return true;
        }

        m_Ptr.DecWeakRef();

        m_Ptr.m_StrongRef = rhs.m_Ptr.m_StrongRef;
        m_Ptr.IncWeakRef() iferr {

            m_Ptr.m_StrongRef = StrongRef{};
            return false;
        }

        return true;
    }

protected:

    Ptr m_Ptr;
};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController, bool IS_OVERFLOW_CHECK = BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)>
class RcConcWeakPtrOverflowResolver;

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcWeakPtrOverflowResolver<T, TRAIT, RcConcMemoryLayoutController, false> : public RcConcWeakPtrResolverBase<T, TRAIT, RcConcMemoryLayoutController> {
private:

    using Super = RcConcWeakPtrResolverBase<T, TRAIT, RcConcMemoryLayoutController>;

public:

    using Super::Super;

    ALT_NODISCARD ALT_FORCE_INLINE static RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController> FromRc(const RcConcPtr<T, TRAIT, RcConcMemoryLayoutController>& rcPtr) noexcept {

        RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController> weak(rcPtr.m_Ptr);
        ALT_MAYBE_UNUSED const bool isOverflow = weak.m_Ptr.IncWeakRef();

        return weak;
    }

    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver() noexcept = default;

    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver(RcConcWeakPtrOverflowResolver&&) noexcept = default;
    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver& operator=(RcConcWeakPtrOverflowResolver&&) noexcept = default;

    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver(const RcConcWeakPtrOverflowResolver& rhs) noexcept
        : Super(Rule5Copy, rhs)
    {}

    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver& operator=(const RcConcWeakPtrOverflowResolver& rhs) noexcept {

        this->Assign(Rule5Copy, rhs);
        return *this;
    }
};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcWeakPtrOverflowResolver<T, TRAIT, RcConcMemoryLayoutController, true> : public RcConcWeakPtrResolverBase<T, TRAIT, RcConcMemoryLayoutController> {
private:

    using Super = RcConcWeakPtrResolverBase<T, TRAIT, RcConcMemoryLayoutController>;
    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;

public:

    using Super::Super;

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController>> FromRc(const RcConcPtr<T, TRAIT, RcConcMemoryLayoutController>& rcPtr) noexcept {
        iferr_scope;

        RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController> weak(rcPtr.m_Ptr);
        weak.m_Ptr.IncWeakRef() iferr {

            weak.m_Ptr.m_StrongRef = StrongRef{};
            return Nullopt;
        }

        return weak;
    }

    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver() noexcept = default;

    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver(RcConcWeakPtrOverflowResolver&&) noexcept = default;
    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver& operator=(RcConcWeakPtrOverflowResolver&&) noexcept = default;

    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver(const RcConcWeakPtrOverflowResolver&) noexcept = delete;
    ALT_FORCE_INLINE RcConcWeakPtrOverflowResolver& operator=(const RcConcWeakPtrOverflowResolver&) noexcept = delete;
};

} // namespace _RcConcPtrDetails
/** \endcond */

/**
 * @brief Thread-safe weak reference to an object managed by RcConcPtr.
 * @tparam T The underlying type of the object.
 * @tparam TRAIT Configuration flags (e.g., WeakPtr).
 * @tparam RcConcMemoryLayoutController The layout strategy for memory allocation.
 * @example
 * 
 * iferr_scope;
 * 
 * RcConcWeakPtr<Alt::String> weak = rcPtr.AsWeak() iferr_return;
 * 
 * RcConcPtr<Alt::String> rcPtr = weak.AsRc() ifok {
 *     assert(rcPtr);
 * }
 */
template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcWeakPtr : public _RcConcPtrDetails::RcConcWeakPtrOverflowResolver<T, TRAIT, RcConcMemoryLayoutController> {
private:

    using Ptr = _RcConcPtrDetails::Ptr<T, TRAIT, RcConcMemoryLayoutController>;
    using Super = _RcConcPtrDetails::RcConcWeakPtrOverflowResolver<T, TRAIT, RcConcMemoryLayoutController>;

    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;

public:

    using Type = T;

    using RcPtr = RcConcPtr<T, TRAIT, RcConcMemoryLayoutController>;

    /**
     * @brief Returns the trait flags of the weak pointer.
     * @return The `RcConcPtrTrait` bitmask.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static RcConcPtrTrait GetTrait() noexcept {

        return TRAIT;
    }

    ALT_FORCE_INLINE RcConcWeakPtr() noexcept = default;

    ALT_FORCE_INLINE RcConcWeakPtr(const RcConcWeakPtr&) noexcept = default;
    ALT_FORCE_INLINE RcConcWeakPtr& operator=(const RcConcWeakPtr&) noexcept = default;

    ALT_FORCE_INLINE RcConcWeakPtr(RcConcWeakPtr&&) noexcept = default;
    ALT_FORCE_INLINE RcConcWeakPtr& operator=(RcConcWeakPtr&&) noexcept = default;

    ALT_FORCE_INLINE ~RcConcWeakPtr() noexcept {

        this->m_Ptr.DecWeakRef();
    }

    /**
     * Clonable Trait 
     * 
     * @brief Clones the weak pointer from another instance.
     * @param rhs The source weak pointer to copy from.
     * @return True if successful, false if counter overflow when 'RcConcPtrTrait::OverflowCheck' enabled.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool CloneFrom(const RcConcWeakPtr& rhs) noexcept {
        iferr_scope;

        this->Assign(_RcConcPtrDetails::Rule5Copy, rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool CloneConstructFrom(const RcConcWeakPtr& rhs) noexcept {

        iferr_scope_on_err {

            Memory::DestroyObject(*this);
        };

        Memory::ConstructDefaultObject<RcConcWeakPtr>(this);
        CloneFrom(rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<RcConcWeakPtr> Clone() const noexcept {
        iferr_scope;

        RcConcWeakPtr cloned;
        cloned.Assign(_RcConcPtrDetails::Rule5Copy, *this) iferr_return Nullopt;

        return MakeOptional(std::move(cloned));
    }

    /**
     * @brief Converts a weak pointer to a strong reference.
     * @details Attempt to increment the strong count and return an RcConcPtr. 
     * Returns Nullopt if the underlying object has already been deallocated.
     * @return An Optional containing the RcConcPtr, or Nullopt if the resource was deallocated
     * or counter overflow when 'RcConcPtrTrait::OverflowCheck' enabled.
     * @example
     * 
     * iferr_scope;
     * 
     * RcConcWeakPtr<Alt::String> weak = rcPtr.AsWeak() iferr_return;
     * 
     * RcConcPtr<Alt::String> rcPtr = weak.AsRc() ifok {
     *     assert(rcPtr);
     * }
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<RcPtr> AsRc() const noexcept {

        return RcPtr::FromWeak(*this);
    }

    /**
     * @brief Gets the current strong reference count.
     * @return The count of strong references.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetStrongCount() const noexcept {

        return this->m_Ptr.GetStrongCount();
    }

    /**
     * @brief Gets the current weak reference count.
     * 
     * @note Returns always >= 1, because first ever strong counter increments weak counter by 1
     *       this done to simplify managing of internal control block.
     * 
     * @return The count of weak references.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetWeakCount() const noexcept {

        return this->m_Ptr.GetWeakCount();
    }

private:

    template <typename, RcConcPtrTrait, typename, bool>
    friend class _RcConcPtrDetails::RcConcWeakPtrOverflowResolver;

    template <typename, RcConcPtrTrait, typename, bool>
    friend class _RcConcPtrDetails::RcConcPtrWeakResolver;

    ALT_FORCE_INLINE RcConcWeakPtr(const Ptr& ptr) noexcept
        : Super(ptr)
    {}
};

template <typename T, RcConcPtrTrait TRAIT, typename RcConcMemoryLayoutController>
class RcConcPtr
    : public _RcConcPtrDetails::RcConcPtrOverflowResolver<T, TRAIT, RcConcMemoryLayoutController>
{
private:

    using Ptr = _RcConcPtrDetails::Ptr<T, TRAIT, RcConcMemoryLayoutController>;
    using Super = _RcConcPtrDetails::RcConcPtrOverflowResolver<T, TRAIT, RcConcMemoryLayoutController>;

public:

    using Type = T;

    using StrongRef = typename RcConcMemoryLayoutController::StrongRef;
    using WeakPtr = RcConcWeakPtr<T, TRAIT, RcConcMemoryLayoutController>;

    /**
     * @brief Returns the trait flags of the weak pointer.
     * @return The `RcConcPtrTrait` bitmask.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static RcConcPtrTrait GetTrait() noexcept {

        return TRAIT;
    }

    /**
     * @brief Allocate object and rc (depends from 'RcConcMemoryLayoutController' implementation).
     * @tparam ArgsT... The types of the arguments used to construct the object T. 
     * @return Optional containing a valid RcConcPtr, or Nullopt if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * RcConcPtr<Alt::String> rcPtr = RcConcPtr<Alt::String>::AllocateObject("Reference Counter"_salloc) iferr_return;
     * assert(*rcPtr == "Reference Counter");
     */
    template <typename ...ArgsT,
        std::enable_if_t<std::is_constructible_v<T, ArgsT&&...> && std::is_default_constructible_v<RcConcMemoryLayoutController>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcConcPtr> AllocateObject(ArgsT&&... args) noexcept {
        iferr_scope;

        RcConcMemoryLayoutController controller{};
        StrongRef strongRef = controller.AllocateStrongWeakRefAndSet(std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return RcConcPtr::FromStrongRefIncAndRcConcMemoryLayoutController(std::move(strongRef), std::move(controller));
    }

    /**
     * @brief Allocate object and rc (depends from 'RcConcMemoryLayoutController' implementation).
     * @tparam RcConcMemoryLayoutControllerT The type of the controller (RcConcMemoryLayoutController must be contruction from RcConcMemoryLayoutControllerT).
     * @tparam ArgsT... The types of the arguments used to construct the object T. 
     * @param controller The RcConcMemoryLayoutControllerT controller instance.
     * @return Optional containing a valid RcConcPtr, or Nullopt if allocation failed.
     * @example
     * 
     * struct CustomRcConcMemoryLayoutController { ... };
     * using RcConcPtr = RcConcPtr<Alt::String, RcConcPtrTrait::All, CustomRcConcMemoryLayoutController>;
     * 
     * RcConcPtr rcPtr = RcConcPtr::AllocateObjectWithController("Reference Counter"_salloc) iferr_return;
     * assert(*rcPtr == "Reference Counter");
     */
    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController, typename ...ArgsT,
        std::enable_if_t<std::is_constructible_v<RcConcMemoryLayoutController, RcConcMemoryLayoutControllerT&&> && std::is_constructible_v<T, ArgsT&&...>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcConcPtr> AllocateObjectWithController(RcConcMemoryLayoutControllerT&& controller, ArgsT&&... args) noexcept {
        iferr_scope;

        StrongRef strongRef = controller.AllocateStrongWeakRefAndSet(std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return RcConcPtr::FromStrongRefIncAndRcConcMemoryLayoutController(std::move(strongRef), std::forward<RcConcMemoryLayoutControllerT>(controller));
    }

    /**
     * @brief Create an RcConcPtr from a 'StrongRef' and default 'RcConcMemoryLayoutController'.
     * @tparam RcConcMemoryLayoutControllerT The type of the controller (RcConcMemoryLayoutController must be contruction from RcConcMemoryLayoutControllerT).
     * @param strongRef A valid, allocated strong reference.
     * @return Optional containing a new RcConcPtr, or Nullopt if counter overflow when 'RcConcPtrTrait::OverflowCheck' enabled.
     * @example
     * 
     * Alt::RcConcIntrusiveMemoryLayoutController<Alt::String, true> rcMemLayoutController{};
     * Alt::RcConcIntrusiveMemoryLayoutController<Alt::String, true>::StrongRef* pStrongRef = rcMemLayoutController.AllocateStrongRef() iferr_return;
     * 
     * Alt::RcConcPtr<Alt::String> rcPtr = Alt::RcConcPtr<Alt::String>::CreateRcFromAllocated(std::move(*pStrongRef)) iferr_return;
     */
    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<
            std::is_same_v<RcConcMemoryLayoutControllerT, RcConcMemoryLayoutController> &&
            std::is_default_constructible_v<RcConcMemoryLayoutController> &&
            BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcConcPtr> CreateRcFromAllocated(StrongRef&& strongRef) noexcept {

        return RcConcPtr::FromStrongRefAndRcConcMemoryLayoutController(std::move(strongRef), RcConcMemoryLayoutController{});
    }

    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<
            std::is_same_v<RcConcMemoryLayoutControllerT, RcConcMemoryLayoutController> &&
            std::is_default_constructible_v<RcConcMemoryLayoutController> &&
            !BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static RcConcPtr CreateRcFromAllocated(StrongRef&& strongRef) noexcept {

        return RcConcPtr::FromStrongRefAndRcConcMemoryLayoutController(std::move(strongRef), RcConcMemoryLayoutController{});
    }

    /**
     * @brief Create an RcConcPtr from a 'StrongRef' and 'RcConcMemoryLayoutController'.
     * @tparam RcConcMemoryLayoutControllerT The type of the controller (RcConcMemoryLayoutController must be contruction from RcConcMemoryLayoutControllerT).
     * @param controller The RcConcMemoryLayoutControllerT controller instance.
     * @param strongRef A valid, allocated strong reference.
     * @return Optional containing a new RcConcPtr, or Nullopt if counter overflow when 'RcConcPtrTrait::OverflowCheck' enabled.
     * @example
     * 
     * Alt::RcConcIntrusiveMemoryLayoutController<Alt::String, true> rcMemLayoutController{};
     * Alt::RcConcIntrusiveMemoryLayoutController<Alt::String, true>::StrongRef* pStrongRef = rcMemLayoutController.AllocateStrongRef() iferr_return;
     * 
     * Alt::RcConcPtr<Alt::String> rcPtr = Alt::RcConcPtr<Alt::String>::CreateRcFromAllocatedWithController(std::move(*pStrongRef), std::move(rcMemLayoutController)) iferr_return;
     */
    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<
            std::is_constructible_v<RcConcMemoryLayoutController, RcConcMemoryLayoutControllerT&&> &&
            BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcConcPtr> CreateRcFromAllocatedWithController(RcConcMemoryLayoutControllerT&& controller, StrongRef&& strongRef) noexcept {

        return RcConcPtr::FromStrongRefAndRcConcMemoryLayoutController(std::move(strongRef), std::forward<RcConcMemoryLayoutControllerT>(controller));
    }

    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<
            std::is_constructible_v<RcConcMemoryLayoutController, RcConcMemoryLayoutControllerT&&> &&
            !BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static RcConcPtr CreateRcFromAllocatedWithController(RcConcMemoryLayoutControllerT&& controller, StrongRef&& strongRef) noexcept {

        return RcConcPtr::FromStrongRefAndRcConcMemoryLayoutController(std::move(strongRef), std::forward<RcConcMemoryLayoutControllerT>(controller));
    }

    ALT_FORCE_INLINE RcConcPtr() noexcept = default;

    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<
            std::is_same_v<RcConcMemoryLayoutControllerT, RcConcMemoryLayoutController> && std::is_default_constructible_v<RcConcMemoryLayoutControllerT>
        >* = nullptr>
    ALT_FORCE_INLINE RcConcPtr(StrongRef&& strongRef) noexcept
        : Super(std::move(strongRef), RcConcMemoryLayoutControllerT{})
    {}

    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcConcMemoryLayoutControllerT, RcConcMemoryLayoutController>>* = nullptr>
    ALT_FORCE_INLINE RcConcPtr(StrongRef&& strongRef, RcConcMemoryLayoutControllerT&& controller) noexcept
        : Super(std::move(strongRef), std::forward<RcConcMemoryLayoutControllerT>(controller))
    {}

    ALT_FORCE_INLINE RcConcPtr(const RcConcPtr&) noexcept = default;
    ALT_FORCE_INLINE RcConcPtr& operator=(const RcConcPtr&) noexcept = default;

    ALT_FORCE_INLINE RcConcPtr(RcConcPtr&&) noexcept = default;
    ALT_FORCE_INLINE RcConcPtr& operator=(RcConcPtr&&) noexcept = default;

    ALT_FORCE_INLINE RcConcPtr(std::nullptr_t) noexcept
        : Super(nullptr)
    {}

    ALT_FORCE_INLINE RcConcPtr& operator=(std::nullptr_t) noexcept {

        return Super::operator=(*this, nullptr);
    }

    ALT_FORCE_INLINE ~RcConcPtr() noexcept {

        this->m_Ptr.DecStrongRef();
    }

    /**
     * Clonable Trait 
     * 
     * @brief Clones the rc pointer from another instance.
     * @param rhs The source weak pointer to copy from.
     * @return True if successful, false if counter overflow when 'RcConcPtrTrait::OverflowCheck' enabled.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool CloneFrom(const RcConcPtr& rhs) noexcept {
        iferr_scope;

        this->Assign(_RcConcPtrDetails::Rule5Copy, rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool CloneConstructFrom(const RcConcPtr& rhs) noexcept {

        iferr_scope_on_err {

            Memory::DestroyObject(*this);
        };

        Memory::ConstructDefaultObject<RcConcPtr>(this);
        CloneFrom(rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<RcConcPtr> Clone() const noexcept {
        iferr_scope;

        RcConcPtr cloned;
        cloned.Assign(_RcConcPtrDetails::Rule5Copy, *this) iferr_return Nullopt;

        return MakeOptional(std::move(cloned));
    }

    /**
     * @brief Explicit conversion to bool.
     * @return True if the pointer is not nullptr.
     */
    ALT_FORCE_INLINE explicit operator bool() const noexcept {
        return this->m_Ptr.Get() != nullptr;
    }

    ALT_FORCE_INLINE T& operator*() noexcept {
        return *this->m_Ptr.Get();
    }

    ALT_FORCE_INLINE const T& operator*() const noexcept {
        return *this->m_Ptr.Get();
    }

    ALT_FORCE_INLINE T* operator->() noexcept {
        return this->m_Ptr.Get();
    }

    ALT_FORCE_INLINE const T* operator->() const noexcept {
        return this->m_Ptr.Get();
    }

    /**
     * @brief Returns the raw pointer to the underlying object.
     * @return The pointer to the data, or nullptr.
     */
    ALT_FORCE_INLINE T* Get() noexcept {
        return this->m_Ptr.Get();
    }

    ALT_FORCE_INLINE const T* Get() const noexcept {
        return this->m_Ptr.Get();
    }

    /**
     * @brief Replaces the object with a new one, decremention strong counter for old object.
     * @param ptr The new 'StrongRef'.
     */
    template <typename U = StrongRef,
        std::enable_if_t<std::is_same_v<U, StrongRef> && BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Reset(StrongRef&& strongRef = StrongRef{}) noexcept {
        iferr_scope;

        const T* ptr = this->m_Ptr.AsRcConcMemoryLayoutController()
            .GetObjectPtr(strongRef)
            .GetValueOr(nullptr);

        if (this->m_Ptr.Get() == ptr) {
            return true;
        }

        this->m_Ptr.DecStrongRef();

        this->m_Ptr.m_StrongRef = std::move(strongRef);
        this->m_Ptr.IncStrongRef() iferr {

            this->m_Ptr.m_StrongRef = StrongRef{};
            return false;
        }

        return true;
    }

    template <typename U = StrongRef,
        std::enable_if_t<std::is_same_v<U, StrongRef> && !BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)>* = nullptr>
    ALT_FORCE_INLINE bool Reset(StrongRef&& strongRef = StrongRef{}) noexcept {

        const T* ptr = this->m_Ptr.AsRcConcMemoryLayoutController()
            .GetObjectPtr(strongRef)
            .GetValueOr(nullptr);

        if (this->m_Ptr.Get() == ptr) {
            return true;
        }

        this->m_Ptr.DecStrongRef();

        this->m_Ptr.m_StrongRef = std::move(strongRef);
        ALT_MAYBE_UNUSED const bool isOverflow = this->m_Ptr.IncStrongRef();

        return true;
    }

    /**
     * @brief Gets the current strong reference count.
     * @return The count of strong references.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetStrongCount() const noexcept {

        return this->m_Ptr.GetStrongCount();
    }

    ALT_FORCE_INLINE bool operator==(const RcConcPtr& rhs) const noexcept {
        return this->m_Ptr.Get() == rhs.m_Ptr.Get();
    }

    ALT_FORCE_INLINE bool operator!=(const RcConcPtr& rhs) const noexcept {
        return !(*this == rhs);
    }

    ALT_FORCE_INLINE bool operator<(const RcConcPtr& rhs) const noexcept {
        return this->m_Ptr.Get() < rhs.m_Ptr.Get();
    }

    ALT_FORCE_INLINE bool operator>(const RcConcPtr& rhs) const noexcept {
        return rhs < *this;
    }

    ALT_FORCE_INLINE bool operator<=(const RcConcPtr& rhs) const noexcept {
        return !(*this > rhs);
    }

    ALT_FORCE_INLINE bool operator>=(const RcConcPtr& rhs) const noexcept {
        return !(*this < rhs);
    }

private:

    template <typename, RcConcPtrTrait, typename, bool>
    friend class _RcConcPtrDetails::RcConcPtrWeakResolver;

    template <typename, RcConcPtrTrait, typename, bool>
    friend class _RcConcPtrDetails::RcConcWeakPtrOverflowResolver;

    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcConcMemoryLayoutControllerT, RcConcMemoryLayoutController>>* = nullptr>
    static ALT_FORCE_INLINE Optional<RcConcPtr<T, TRAIT, RcConcMemoryLayoutControllerT>> FromStrongRefIncAndRcConcMemoryLayoutController(
        StrongRef&& strongRef, RcConcMemoryLayoutControllerT&& controller
    ) noexcept {

        Ptr ptr(std::move(strongRef), std::forward<RcConcMemoryLayoutControllerT>(controller));
        return RcConcPtr<T, TRAIT, RcConcMemoryLayoutControllerT>(std::move(ptr));
    }

    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcConcMemoryLayoutControllerT, RcConcMemoryLayoutController> && BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)>* = nullptr>
    static ALT_FORCE_INLINE Optional<RcConcPtr<T, TRAIT, RcConcMemoryLayoutControllerT>> FromStrongRefAndRcConcMemoryLayoutController(
        StrongRef&& strongRef, RcConcMemoryLayoutControllerT&& controller
    ) noexcept {

        iferr_scope;

        Ptr ptr(std::move(strongRef), std::forward<RcConcMemoryLayoutControllerT>(controller));
        ptr.IncStrongRef() iferr {

            ptr.m_Ptr.m_StrongRef = StrongRef{};
            return Nullopt;
        }

        return RcConcPtr<T, TRAIT, RcConcMemoryLayoutControllerT>(std::move(ptr));
    }

    template <typename RcConcMemoryLayoutControllerT = RcConcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcConcMemoryLayoutControllerT, RcConcMemoryLayoutController> && !BitFlag::IsSetEnumFlags(TRAIT, RcConcPtrTrait::OverflowCheck)>* = nullptr>
    static ALT_FORCE_INLINE RcConcPtr<T, TRAIT, RcConcMemoryLayoutControllerT> FromStrongRefAndRcConcMemoryLayoutController(
        StrongRef&& strongRef, RcConcMemoryLayoutControllerT&& controller
    ) noexcept {

        Ptr ptr(std::move(strongRef), std::forward<RcConcMemoryLayoutControllerT>(controller));
        ALT_MAYBE_UNUSED const bool isOverflow = ptr.IncStrongRef();

        return RcConcPtr<T, TRAIT, RcConcMemoryLayoutControllerT>(std::move(ptr));
    }

    ALT_FORCE_INLINE RcConcPtr(Ptr&& ptr) noexcept
        : Super(std::move(ptr))
    {}

    ALT_FORCE_INLINE RcConcPtr(const Ptr& ptr) noexcept
        : Super(ptr)
    {}
};

} // namespace Alt

#ifdef ALT_COMPILER_MSVC
#pragma warning(pop)
#endif // ALT_COMPILER_MSVC

#endif // __ALT_STD_RC_CONC_PTR__