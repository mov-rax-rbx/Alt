#pragma once

#ifndef __ALT_STD_RC_PTR__
#define __ALT_STD_RC_PTR__

#include "../IfErr.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Memory/MaybeUninit.h"

#include "../Utils/Num.h"
#include "../Utils/Optional.h"
#include "../Utils/NonCopyableMovable.h"
#include "../Utils/EnumClassBitFlagsImpl.h"

namespace Alt {

/**
 * @brief Configuration flags for Reference Counting behavior.
 * @details These flags determine if the RcPtr system supports weak pointers and 
 * performs arithmetic overflow checks during increment/decrement operations.
 */
enum class RcPtrTrait {

    None = 0,
    WeakPtr = 1 << 0, ///< Enables support for RcWeakPtr.
    OverflowCheck = 1 << 1, ///< Enables safety checks for counter overflows and disables constructor/assignment copy (use clonable trait with error check).

    All = WeakPtr | OverflowCheck,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(RcPtrTrait);

/**
 * @brief Default Rc Memory Layout Controller which inlines data and rc.
 * @tparam T The type of the object.
 * @tparam IsWeak Implementation of weak counter status (true if RcPtrTrait::WeakPtr is set).
 * @details This controller places the Strong and Weak counters in the same memory 
 * block as the data `T`.
 */
template <typename T, bool IS_WEAK_PTR>
struct RcIntrusiveMemoryLayoutController;

/**
 * @brief Default Rc Memory Layout Controller which keeps data and rc separately (2 alloctions).
 * @tparam T The type of the object.
 * @tparam IsWeak Implementation of weak counter status (true if RcPtrTrait::WeakPtr is set).
 * @details This controller separates the Reference Counter "Control Block" from 
 * the actual data `T`. This allows the count to be allocated in a different memory 
 * pool or region than the object itself.
 */
template <typename T, bool IS_WEAK_PTR>
struct RcControlBlockMemoryLayoutController;

/**
 * @brief Smart pointer for managing objects with shared ownership via reference counting.
 * @tparam T The underlying type of the object.
 * @tparam TRAIT Configuration flags (e.g., WeakPtr, OverflowCheck).
 * @tparam RcMemoryLayoutController The strategy used to determine memory layout 
 * (e.g., RcIntrusiveMemoryLayoutController or RcControlBlockMemoryLayoutController, ...).
 * @example
 * 
 * iferr_scope;
 * 
 * RcPtr<Alt::String> ptr = RcPtr<Alt::String>::AllocateObject("Reference Counter"_salloc) iferr_return;
 * assert(*ptr == "Reference Counter");
 * 
 * RcPtr<Alt::String> ptr1 = ptr.Clone() iferr_return;
 * assert(ptr1 == ptr);
 * 
 * RcWeakPtr<Alt::String> weak = ptr.AsWeak() iferr_return;
 * RcPtr<Alt::String> ptr2 = weak.AsRc() iferr_return;
 * assert(ptr2 == ptr);
 */
template <
    typename T,
    RcPtrTrait TRAIT = RcPtrTrait::WeakPtr,
    typename RcMemoryLayoutController = RcIntrusiveMemoryLayoutController<T, BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::WeakPtr)>
>
class RcPtr;

/**
 * @brief Weak reference to an object managed by RcPtr.
 * @tparam T The underlying type of the object.
 * @tparam TRAIT Configuration flags (e.g., WeakPtr).
 * @tparam RcMemoryLayoutController The layout strategy for memory allocation.
 * @example
 * 
 * iferr_scope;
 * 
 * RcWeakPtr<Alt::String> weak = rcPtr.AsWeak() iferr_return;
 * 
 * RcPtr<Alt::String> rcPtr = weak.AsRc() ifok {
 *     assert(rcPtr);
 * }
 */
template <
    typename T,
    RcPtrTrait TRAIT = RcPtrTrait::WeakPtr,
    typename RcMemoryLayoutController = RcIntrusiveMemoryLayoutController<T, BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::WeakPtr)>
>
class RcWeakPtr;

/*
 * To control the memory layout of Reference Counters, use the 'RcMemoryLayoutController' "interface".
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
struct RcIntrusiveMemoryLayoutController<T, false> {

    struct StrongRefData;

    using StrongRef = StrongRefData;
    using StrongCounter = usize;
    using WeakCounter = Unit_t;

    struct StrongRefControlBlock {

        StrongCounter strongCounter = 0;

        T object;

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
struct RcIntrusiveMemoryLayoutController<T, true> {

    struct StrongRefData;

    using StrongRef = StrongRefData;
    using StrongCounter = usize;
    using WeakCounter = usize;

    struct StrongRefControlBlock {

        StrongCounter strongCounter = 0;
        WeakCounter weakCounter = 0;

        MaybeUninit<T> object;

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
struct RcControlBlockMemoryLayoutController<T, false> : public RcIntrusiveMemoryLayoutController<T, false> {

    using Super = RcIntrusiveMemoryLayoutController<T, false>;
    using Super::Super;
};

template <typename T>
struct RcControlBlockMemoryLayoutController<T, true> {

    struct StrongRefData;

    using StrongRef = StrongRefData;
    using StrongCounter = usize;
    using WeakCounter = usize;

    struct StrongRefControlBlock {

        StrongCounter strongCounter = 0;
        WeakCounter weakCounter = 0;

        ALT_MAKE_CLASS_NON_COPYABLE(StrongRefControlBlock);
        ALT_MAKE_CLASS_NON_MOVABLE(StrongRefControlBlock);

        ALT_FORCE_INLINE StrongRefControlBlock() noexcept = default;

        ALT_FORCE_INLINE StrongRefControlBlock(const StrongCounter strongCounter, const StrongCounter weakCounter) noexcept
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

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
struct __is_smart_pointer__<RcPtr<T, TRAIT, RcMemoryLayoutController>>
    : std::true_type
{};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
struct __is_trivially_reallocatable__<RcPtr<T, TRAIT, RcMemoryLayoutController>>
    : std::conjunction<
        is_trivially_reallocatable_t<RcMemoryLayoutController>,
        is_trivially_reallocatable_t<typename RcMemoryLayoutController::StrongRef>
    >
{};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
struct __is_trivially_reallocatable__<RcWeakPtr<T, TRAIT, RcMemoryLayoutController>>
    : std::conjunction<
        is_trivially_reallocatable_t<RcMemoryLayoutController>,
        is_trivially_reallocatable_t<typename RcMemoryLayoutController::StrongRef>
    >
{};

} // namespace TypeTraits

/** \cond */
namespace _RcPtrDetails {

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController, bool IS_WEAK_PTR = BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::WeakPtr)>
class Ptr;

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class Ptr<T, TRAIT, RcMemoryLayoutController, false> : public RcMemoryLayoutController {
public:

    using StrongRef = typename RcMemoryLayoutController::StrongRef;
    using StrongCounter = typename RcMemoryLayoutController::StrongCounter;

    ALT_FORCE_INLINE Ptr() noexcept = default;

    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcMemoryLayoutControllerT, RcMemoryLayoutController>>* = nullptr>
    ALT_FORCE_INLINE Ptr(StrongRef&& strongRef, RcMemoryLayoutControllerT&& controller) noexcept
        : RcMemoryLayoutController(std::forward<RcMemoryLayoutControllerT>(controller))
        , m_StrongRef(std::move(strongRef))
    {}

    ALT_FORCE_INLINE RcMemoryLayoutController& AsRcMemoryLayoutController() noexcept {
        return static_cast<RcMemoryLayoutController&>(*this);
    }

    ALT_FORCE_INLINE const RcMemoryLayoutController& AsRcMemoryLayoutController() const noexcept {
        return static_cast<const RcMemoryLayoutController&>(*this);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IncStrongRef() noexcept {
        iferr_scope;

        StrongCounter* pStrongCounterRef = AsRcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return true;

        if constexpr (BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)) {

            const StrongCounter strongCounterRef = Num::AddOverflowCheck(*pStrongCounterRef, 1) iferr_return false;
            *pStrongCounterRef = strongCounterRef;
        }
        else {

            *pStrongCounterRef += 1;
        }

        return true;
    }

    ALT_FORCE_INLINE void DecStrongRef() noexcept {
        iferr_scope;

        StrongCounter* pStrongCounterRef = AsRcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return;
        *pStrongCounterRef -= 1;

        if (*pStrongCounterRef == 0) {

            AsRcMemoryLayoutController().DeallocateStrongRef(m_StrongRef);
        }

        // Decrementing only when outer object destroyed, so just reset 'StrongRef' to be robust for other 'DecStrongRef()'
        m_StrongRef = StrongRef{};
    }

    ALT_FORCE_INLINE T* Get() noexcept {
        iferr_scope;

        T* ptr = AsRcMemoryLayoutController().GetObjectPtr(m_StrongRef) iferr_return nullptr;
        return ptr;
    }

    ALT_FORCE_INLINE const T* Get() const noexcept {
        iferr_scope;

        const T* ptr = AsRcMemoryLayoutController().GetObjectPtr(m_StrongRef) iferr_return nullptr;
        return ptr;
    }

    ALT_FORCE_INLINE usize GetStrongCount() const noexcept {
        iferr_scope;

        const StrongCounter* pStrongCounterRef = AsRcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return 0;
        return *pStrongCounterRef;
    }

public:

    StrongRef m_StrongRef;
};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class Ptr<T, TRAIT, RcMemoryLayoutController, true> : public RcMemoryLayoutController {
public:

    using StrongRef = typename RcMemoryLayoutController::StrongRef;
    using StrongCounter = typename RcMemoryLayoutController::StrongCounter;
    using WeakCounter = typename RcMemoryLayoutController::WeakCounter;

    ALT_FORCE_INLINE Ptr() noexcept = default;

    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcMemoryLayoutControllerT, RcMemoryLayoutController>>* = nullptr>
    ALT_FORCE_INLINE Ptr(StrongRef&& strongRef, RcMemoryLayoutControllerT&& controller) noexcept
        : RcMemoryLayoutController(std::forward<RcMemoryLayoutControllerT>(controller))
        , m_StrongRef(std::move(strongRef))
    {}

    ALT_FORCE_INLINE RcMemoryLayoutController& AsRcMemoryLayoutController() noexcept {
        return static_cast<RcMemoryLayoutController&>(*this);
    }

    ALT_FORCE_INLINE const RcMemoryLayoutController& AsRcMemoryLayoutController() const noexcept {
        return static_cast<const RcMemoryLayoutController&>(*this);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IncStrongRef() noexcept {
        iferr_scope;

        StrongCounter* pStrongCounterRef = AsRcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return true;

        if constexpr (BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)) {

            const StrongCounter strongCounterRef = Num::AddOverflowCheck(*pStrongCounterRef, 1) iferr_return false;
            *pStrongCounterRef = strongCounterRef;
        }
        else {

            *pStrongCounterRef += 1;
        }

        return true;
    }

    ALT_FORCE_INLINE void DecStrongRef() noexcept {
        iferr_scope;

        StrongCounter* pStrongCounterRef = AsRcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return;
        *pStrongCounterRef -= 1;

        if (*pStrongCounterRef == 0) {

            WeakCounter* pWeakCounterRef = AsRcMemoryLayoutController().GetWeakCounterRef(m_StrongRef);
            ALT_ASSERT(pWeakCounterRef);

            *pWeakCounterRef -= 1;

            AsRcMemoryLayoutController().DeallocateObject(m_StrongRef);

            if (*pWeakCounterRef == 0) {

                AsRcMemoryLayoutController().DeallocateWeakCounter(m_StrongRef);
            }
        }

        // Decrementing only when object destroyed, so just reset 'StrongRef' to be robust for other 'DecStrongRef()'
        m_StrongRef = StrongRef{};
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IncWeakRef() noexcept {
        iferr_scope;

        WeakCounter* pWeakCounterRef = AsRcMemoryLayoutController().GetWeakCounterRef(m_StrongRef) iferr_return true;

        if constexpr (BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)) {

            const WeakCounter weakCounterRef = Num::AddOverflowCheck(*pWeakCounterRef, 1) iferr_return false;
            *pWeakCounterRef = weakCounterRef;
        }
        else {

            *pWeakCounterRef += 1;
        }

        return true;
    }

    ALT_FORCE_INLINE void DecWeakRef() noexcept {
        iferr_scope;

        WeakCounter* pWeakCounterRef = AsRcMemoryLayoutController().GetWeakCounterRef(m_StrongRef) iferr_return;
        *pWeakCounterRef -= 1;

        if (*pWeakCounterRef == 0) {

            AsRcMemoryLayoutController().DeallocateWeakCounter(m_StrongRef);
        }

        // Decrementing only when weak object destroyed, so just reset 'StrongRef' to be robust for other 'DecWeakRef()'
        m_StrongRef = StrongRef{};
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool TryMakeStrongRef() noexcept {

        if (GetStrongCount() == 0) {
            return false;
        }

        return IncStrongRef();
    }

    ALT_FORCE_INLINE T* Get() noexcept {
        iferr_scope;

        T* ptr = AsRcMemoryLayoutController().GetObjectPtr(m_StrongRef) iferr_return nullptr;
        return ptr;
    }

    ALT_FORCE_INLINE const T* Get() const noexcept {
        iferr_scope;

        const T* ptr = AsRcMemoryLayoutController().GetObjectPtr(m_StrongRef) iferr_return nullptr;
        return ptr;
    }

    ALT_FORCE_INLINE usize GetStrongCount() const noexcept {
        iferr_scope;

        const StrongCounter* pStrongCounterRef = AsRcMemoryLayoutController().GetStrongCounterRef(m_StrongRef) iferr_return 0;
        return *pStrongCounterRef;
    }

    ALT_FORCE_INLINE usize GetWeakCount() const noexcept {
        iferr_scope;

        const WeakCounter* pWeakCounterRef = AsRcMemoryLayoutController().GetWeakCounterRef(m_StrongRef) iferr_return 0;
        return *pWeakCounterRef;
    }

public:

    StrongRef m_StrongRef;
};

struct Rule5Copy_t {};
constexpr static Rule5Copy_t Rule5Copy = Rule5Copy_t{};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcPtrResolverBase {
private:

    using Ptr = Ptr<T, TRAIT, RcMemoryLayoutController>;
    using StrongRef = typename RcMemoryLayoutController::StrongRef;

public:

    ALT_FORCE_INLINE RcPtrResolverBase() noexcept = default;

    ALT_FORCE_INLINE RcPtrResolverBase(RcPtrResolverBase&& rhs) noexcept
        : m_Ptr(Exchange(rhs.m_Ptr.m_StrongRef, StrongRef{}), std::move(rhs.m_Ptr.AsRcMemoryLayoutController()))
    {}

    ALT_FORCE_INLINE RcPtrResolverBase& operator=(RcPtrResolverBase&& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return *this;
        }

        m_Ptr.DecStrongRef();

        m_Ptr.m_StrongRef = Exchange(rhs.m_Ptr.m_StrongRef, StrongRef{});
        m_Ptr.AsRcMemoryLayoutController() = std::move(rhs.m_Ptr.AsRcMemoryLayoutController());

        return *this;
    }

    ALT_FORCE_INLINE RcPtrResolverBase(std::nullptr_t) noexcept
    {}

    ALT_FORCE_INLINE RcPtrResolverBase& operator=(std::nullptr_t) noexcept {

        m_Ptr.DecStrongRef();

        m_Ptr.m_StrongRef = StrongRef{};

        return *this;
    }

protected:

    ALT_FORCE_INLINE RcPtrResolverBase(Ptr&& ptr) noexcept
        : m_Ptr(std::move(ptr))
    {}

    ALT_FORCE_INLINE RcPtrResolverBase(const Ptr& ptr) noexcept
        : m_Ptr(ptr)
    {}

    ALT_FORCE_INLINE RcPtrResolverBase(Rule5Copy_t, const RcPtrResolverBase& rhs) noexcept
        : m_Ptr(rhs.m_Ptr)
    {
        ALT_MAYBE_UNUSED const bool isOverflow = m_Ptr.IncStrongRef();
    }

    ALT_FORCE_INLINE bool Assign(Rule5Copy_t, const RcPtrResolverBase& rhs) noexcept {
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

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController, bool IS_WEAK_PTR = BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::WeakPtr)>
class RcPtrWeakResolver;

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcPtrWeakResolver<T, TRAIT, RcMemoryLayoutController, false> : public RcPtrResolverBase<T, TRAIT, RcMemoryLayoutController> {
private:

    using Super = RcPtrResolverBase<T, TRAIT, RcMemoryLayoutController>;

public:

    using Super::Super;
};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcPtrWeakResolver<T, TRAIT, RcMemoryLayoutController, true> : public RcPtrResolverBase<T, TRAIT, RcMemoryLayoutController> {
private:

    using Super = RcPtrResolverBase<T, TRAIT, RcMemoryLayoutController>;
    using StrongRef = typename RcMemoryLayoutController::StrongRef;

public:

    using Super::Super;

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcPtr<T, TRAIT, RcMemoryLayoutController>> FromWeak(const RcWeakPtr<T, TRAIT, RcMemoryLayoutController>& weak) noexcept {
        iferr_scope;

        RcPtr<T, TRAIT, RcMemoryLayoutController> result(weak.m_Ptr);
        result.m_Ptr.TryMakeStrongRef() iferr {

            result.m_Ptr.m_StrongRef = StrongRef{};
            return Nullopt;
        }

        return result;
    }

    template <typename U = T,
        std::enable_if_t<std::is_same_v<U, T> && BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Optional<RcWeakPtr<T, TRAIT, RcMemoryLayoutController>> AsWeak() const noexcept {

        return RcWeakPtr<T, TRAIT, RcMemoryLayoutController>::FromRc(static_cast<const RcPtr<T, TRAIT, RcMemoryLayoutController>&>(*this));
    }

    template <typename U = T,
        std::enable_if_t<std::is_same_v<U, T> && !BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE RcWeakPtr<T, TRAIT, RcMemoryLayoutController> AsWeak() const noexcept {

        return RcWeakPtr<T, TRAIT, RcMemoryLayoutController>::FromRc(static_cast<const RcPtr<T, TRAIT, RcMemoryLayoutController>&>(*this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetWeakCount() const noexcept {

        return this->m_Ptr.GetWeakCount();
    }
};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController, bool IS_OVERFLOW_CHECK = BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)>
class RcPtrOverflowResolver;

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcPtrOverflowResolver<T, TRAIT, RcMemoryLayoutController, false> : public RcPtrWeakResolver<T, TRAIT, RcMemoryLayoutController> {
private:

    using Super = RcPtrWeakResolver<T, TRAIT, RcMemoryLayoutController>;
    using StrongRef = typename RcMemoryLayoutController::StrongRef;

public:

    using Super::Super;

    ALT_FORCE_INLINE RcPtrOverflowResolver() noexcept = default;

    ALT_FORCE_INLINE RcPtrOverflowResolver(RcPtrOverflowResolver&&) noexcept = default;
    ALT_FORCE_INLINE RcPtrOverflowResolver& operator=(RcPtrOverflowResolver&&) noexcept = default;

    ALT_FORCE_INLINE RcPtrOverflowResolver(const RcPtrOverflowResolver& rhs) noexcept
        : Super(Rule5Copy, rhs)
    {}

    ALT_FORCE_INLINE RcPtrOverflowResolver& operator=(const RcPtrOverflowResolver& rhs) noexcept {

        this->Assign(Rule5Copy, rhs);
        return *this;
    }
};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcPtrOverflowResolver<T, TRAIT, RcMemoryLayoutController, true> : public RcPtrWeakResolver<T, TRAIT, RcMemoryLayoutController> {
private:

    using Super = RcPtrWeakResolver<T, TRAIT, RcMemoryLayoutController>;
    using StrongRef = typename RcMemoryLayoutController::StrongRef;

public:

    using Super::Super;

    ALT_FORCE_INLINE RcPtrOverflowResolver() noexcept = default;

    ALT_FORCE_INLINE RcPtrOverflowResolver(RcPtrOverflowResolver&&) noexcept = default;
    ALT_FORCE_INLINE RcPtrOverflowResolver& operator=(RcPtrOverflowResolver&&) noexcept = default;

    /*
     * Use 'Clone()' to check overflow on increments
     */
    RcPtrOverflowResolver(const RcPtrOverflowResolver&) noexcept = delete;
    RcPtrOverflowResolver& operator=(const RcPtrOverflowResolver&) noexcept = delete;
};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcWeakPtrResolverBase {
private:

    using Ptr = _RcPtrDetails::Ptr<T, TRAIT, RcMemoryLayoutController>;
    using StrongRef = typename RcMemoryLayoutController::StrongRef;

public:

    ALT_FORCE_INLINE RcWeakPtrResolverBase() noexcept = default;

    ALT_FORCE_INLINE RcWeakPtrResolverBase(RcWeakPtrResolverBase&& rhs) noexcept
        : m_Ptr(Exchange(rhs.m_Ptr.m_StrongRef, StrongRef{}), std::move(rhs.m_Ptr.AsRcMemoryLayoutController()))
    {}

    ALT_FORCE_INLINE RcWeakPtrResolverBase& operator=(RcWeakPtrResolverBase&& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return *this;
        }

        m_Ptr.DecWeakRef();

        m_Ptr.m_StrongRef = Exchange(rhs.m_Ptr.m_StrongRef, StrongRef{});
        m_Ptr.AsRcMemoryLayoutController() = std::move(rhs.m_Ptr.AsRcMemoryLayoutController());

        return *this;
    }

protected:

    ALT_FORCE_INLINE RcWeakPtrResolverBase(const Ptr& ptr) noexcept
        : m_Ptr(ptr)
    {}

    ALT_FORCE_INLINE RcWeakPtrResolverBase(Rule5Copy_t, const RcWeakPtrResolverBase& rhs) noexcept
        : m_Ptr(rhs.m_Ptr)
    {
        ALT_MAYBE_UNUSED const bool isOverflow = m_Ptr.IncWeakRef();
    }

    ALT_FORCE_INLINE bool Assign(Rule5Copy_t, const RcWeakPtrResolverBase& rhs) noexcept {
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

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController, bool IS_OVERFLOW_CHECK = BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)>
class RcWeakPtrOverflowResolver;

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcWeakPtrOverflowResolver<T, TRAIT, RcMemoryLayoutController, false> : public RcWeakPtrResolverBase<T, TRAIT, RcMemoryLayoutController> {
private:

    using Super = RcWeakPtrResolverBase<T, TRAIT, RcMemoryLayoutController>;

public:

    using Super::Super;

    ALT_NODISCARD ALT_FORCE_INLINE static RcWeakPtr<T, TRAIT, RcMemoryLayoutController> FromRc(const RcPtr<T, TRAIT, RcMemoryLayoutController>& rcPtr) noexcept {

        RcWeakPtr<T, TRAIT, RcMemoryLayoutController> weak(rcPtr.m_Ptr);
        ALT_MAYBE_UNUSED const bool isOverflow = weak.m_Ptr.IncWeakRef();

        return weak;
    }

    ALT_FORCE_INLINE RcWeakPtrOverflowResolver() noexcept = default;

    ALT_FORCE_INLINE RcWeakPtrOverflowResolver(RcWeakPtrOverflowResolver&&) noexcept = default;
    ALT_FORCE_INLINE RcWeakPtrOverflowResolver& operator=(RcWeakPtrOverflowResolver&&) noexcept = default;

    ALT_FORCE_INLINE RcWeakPtrOverflowResolver(const RcWeakPtrOverflowResolver& rhs) noexcept
        : Super(Rule5Copy, rhs)
    {}

    ALT_FORCE_INLINE RcWeakPtrOverflowResolver& operator=(const RcWeakPtrOverflowResolver& rhs) noexcept {

        this->Assign(Rule5Copy, rhs);
        return *this;
    }
};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcWeakPtrOverflowResolver<T, TRAIT, RcMemoryLayoutController, true> : public RcWeakPtrResolverBase<T, TRAIT, RcMemoryLayoutController> {
private:

    using Super = RcWeakPtrResolverBase<T, TRAIT, RcMemoryLayoutController>;
    using StrongRef = typename RcMemoryLayoutController::StrongRef;

public:

    using Super::Super;

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcWeakPtr<T, TRAIT, RcMemoryLayoutController>> FromRc(const RcPtr<T, TRAIT, RcMemoryLayoutController>& rcPtr) noexcept {
        iferr_scope;

        RcWeakPtr<T, TRAIT, RcMemoryLayoutController> weak(rcPtr.m_Ptr);
        weak.m_Ptr.IncWeakRef() iferr {

            weak.m_Ptr.m_StrongRef = StrongRef{};
            return Nullopt;
        }

        return weak;
    }

    ALT_FORCE_INLINE RcWeakPtrOverflowResolver() noexcept = default;

    ALT_FORCE_INLINE RcWeakPtrOverflowResolver(RcWeakPtrOverflowResolver&&) noexcept = default;
    ALT_FORCE_INLINE RcWeakPtrOverflowResolver& operator=(RcWeakPtrOverflowResolver&&) noexcept = default;

    ALT_FORCE_INLINE RcWeakPtrOverflowResolver(const RcWeakPtrOverflowResolver&) noexcept = delete;
    ALT_FORCE_INLINE RcWeakPtrOverflowResolver& operator=(const RcWeakPtrOverflowResolver&) noexcept = delete;
};

} // namespace _RcPtrDetails
/** \endcond */

/**
 * @brief Weak reference to an object managed by RcPtr.
 * @tparam T The underlying type of the object.
 * @tparam TRAIT Configuration flags (e.g., WeakPtr).
 * @tparam RcMemoryLayoutController The layout strategy for memory allocation.
 * @example
 * 
 * iferr_scope;
 * 
 * RcWeakPtr<Alt::String> weak = rcPtr.AsWeak() iferr_return;
 * 
 * RcPtr<Alt::String> rcPtr = weak.AsRc() ifok {
 *     assert(rcPtr);
 * }
 */
template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcWeakPtr : public _RcPtrDetails::RcWeakPtrOverflowResolver<T, TRAIT, RcMemoryLayoutController> {
private:

    using Ptr = _RcPtrDetails::Ptr<T, TRAIT, RcMemoryLayoutController>;
    using Super = _RcPtrDetails::RcWeakPtrOverflowResolver<T, TRAIT, RcMemoryLayoutController>;

    using StrongRef = typename RcMemoryLayoutController::StrongRef;

public:

    using Type = T;

    using RcPtr = RcPtr<T, TRAIT, RcMemoryLayoutController>;

    /**
     * @brief Returns the trait flags of the weak pointer.
     * @return The `RcPtrTrait` bitmask.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static RcPtrTrait GetTrait() noexcept {

        return TRAIT;
    }

    ALT_FORCE_INLINE RcWeakPtr() noexcept = default;

    ALT_FORCE_INLINE RcWeakPtr(const RcWeakPtr&) noexcept = default;
    ALT_FORCE_INLINE RcWeakPtr& operator=(const RcWeakPtr&) noexcept = default;

    ALT_FORCE_INLINE RcWeakPtr(RcWeakPtr&&) noexcept = default;
    ALT_FORCE_INLINE RcWeakPtr& operator=(RcWeakPtr&&) noexcept = default;

    ALT_FORCE_INLINE ~RcWeakPtr() noexcept {

        this->m_Ptr.DecWeakRef();
    }

    /**
     * Clonable Trait 
     * 
     * @brief Clones the weak pointer from another instance.
     * @param rhs The source weak pointer to copy from.
     * @return True if successful, false if counter overflow when 'RcPtrTrait::OverflowCheck' enabled.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool CloneFrom(const RcWeakPtr& rhs) noexcept {
        iferr_scope;

        this->Assign(_RcPtrDetails::Rule5Copy, rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool CloneConstructFrom(const RcWeakPtr& rhs) noexcept {

        iferr_scope_on_err {

            Memory::DestroyObject(*this);
        };

        Memory::ConstructDefaultObject<RcWeakPtr>(this);
        CloneFrom(rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<RcWeakPtr> Clone() const noexcept {
        iferr_scope;

        RcWeakPtr cloned;
        cloned.Assign(_RcPtrDetails::Rule5Copy, *this) iferr_return Nullopt;

        return MakeOptional(std::move(cloned));
    }

    /**
     * @brief Converts a weak pointer to a strong reference.
     * @details Attempt to increment the strong count and return an RcPtr. 
     * Returns Nullopt if the underlying object has already been deallocated.
     * @return An Optional containing the RcPtr, or Nullopt if the resource was deallocated
     * or counter overflow when 'RcPtrTrait::OverflowCheck' enabled.
     * @example
     * 
     * iferr_scope;
     * 
     * RcWeakPtr<Alt::String> weak = rcPtr.AsWeak() iferr_return;
     * 
     * RcPtr<Alt::String> rcPtr = weak.AsRc() ifok {
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
     *       this done to simplify managing of internal control block and to be compatible with 'RcConcPtr'.
     * 
     * @return The count of weak references.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetWeakCount() const noexcept {

        return this->m_Ptr.GetWeakCount();
    }

private:

    template <typename, RcPtrTrait, typename, bool>
    friend class _RcPtrDetails::RcWeakPtrOverflowResolver;

    template <typename, RcPtrTrait, typename, bool>
    friend class _RcPtrDetails::RcPtrWeakResolver;

    ALT_FORCE_INLINE RcWeakPtr(const Ptr& ptr) noexcept
        : Super(ptr)
    {}
};

template <typename T, RcPtrTrait TRAIT, typename RcMemoryLayoutController>
class RcPtr
    : public _RcPtrDetails::RcPtrOverflowResolver<T, TRAIT, RcMemoryLayoutController>
{
private:

    using Ptr = _RcPtrDetails::Ptr<T, TRAIT, RcMemoryLayoutController>;
    using Super = _RcPtrDetails::RcPtrOverflowResolver<T, TRAIT, RcMemoryLayoutController>;

public:

    using Type = T;

    using StrongRef = typename RcMemoryLayoutController::StrongRef;
    using WeakPtr = RcWeakPtr<T, TRAIT, RcMemoryLayoutController>;

    /**
     * @brief Returns the trait flags of the weak pointer.
     * @return The `RcPtrTrait` bitmask.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static RcPtrTrait GetTrait() noexcept {

        return TRAIT;
    }

    /**
     * @brief Allocate object and rc (depends from 'RcMemoryLayoutController' implementation).
     * @tparam ArgsT... The types of the arguments used to construct the object T. 
     * @return Optional containing a valid RcPtr, or Nullopt if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * RcPtr<Alt::String> rcPtr = RcPtr<Alt::String>::AllocateObject("Reference Counter"_salloc) iferr_return;
     * assert(*rcPtr == "Reference Counter");
     */
    template <typename ...ArgsT,
        std::enable_if_t<std::is_constructible_v<T, ArgsT&&...> && std::is_default_constructible_v<RcMemoryLayoutController>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcPtr> AllocateObject(ArgsT&&... args) noexcept {
        iferr_scope;

        RcMemoryLayoutController controller{};
        StrongRef strongRef = controller.AllocateStrongWeakRefAndSet(std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return RcPtr::FromStrongRefIncAndRcMemoryLayoutController(std::move(strongRef), std::move(controller));
    }

    /**
     * @brief Allocate object and rc (depends from 'RcMemoryLayoutController' implementation).
     * @tparam RcMemoryLayoutControllerT The type of the controller (RcMemoryLayoutController must be contruction from RcMemoryLayoutControllerT).
     * @tparam ArgsT... The types of the arguments used to construct the object T. 
     * @param controller The RcMemoryLayoutControllerT controller instance.
     * @return Optional containing a valid RcPtr, or Nullopt if allocation failed.
     * @example
     * 
     * struct CustomRcMemoryLayoutController { ... };
     * using RcPtr = RcPtr<Alt::String, RcPtrTrait::All, CustomRcMemoryLayoutController>;
     * 
     * RcPtr rcPtr = RcPtr::AllocateObjectWithController("Reference Counter"_salloc) iferr_return;
     * assert(*rcPtr == "Reference Counter");
     */
    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController, typename ...ArgsT,
        std::enable_if_t<std::is_constructible_v<RcMemoryLayoutController, RcMemoryLayoutControllerT&&> && std::is_constructible_v<T, ArgsT&&...>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcPtr> AllocateObjectWithController(RcMemoryLayoutControllerT&& controller, ArgsT&&... args) noexcept {
        iferr_scope;

        StrongRef strongRef = controller.AllocateStrongWeakRefAndSet(std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return RcPtr::FromStrongRefIncAndRcMemoryLayoutController(std::move(strongRef), std::forward<RcMemoryLayoutControllerT>(controller));
    }

    /**
     * @brief Create an RcPtr from a 'StrongRef' and default 'RcMemoryLayoutController'.
     * @tparam RcMemoryLayoutControllerT The type of the controller (RcMemoryLayoutController must be contruction from RcMemoryLayoutControllerT).
     * @param strongRef A valid, allocated strong reference.
     * @return Optional containing a new RcPtr, or Nullopt if counter overflow when 'RcPtrTrait::OverflowCheck' enabled.
     * @example
     * 
     * Alt::RcIntrusiveMemoryLayoutController<Alt::String, true> rcMemLayoutController{};
     * Alt::RcIntrusiveMemoryLayoutController<Alt::String, true>::StrongRef* pStrongRef = rcMemLayoutController.AllocateStrongRef() iferr_return;
     * 
     * Alt::RcPtr<Alt::String> rcPtr = Alt::RcPtr<Alt::String>::CreateRcFromAllocated(std::move(*pStrongRef)) iferr_return;
     */
    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<
            std::is_same_v<RcMemoryLayoutControllerT, RcMemoryLayoutController> &&
            std::is_default_constructible_v<RcMemoryLayoutController> &&
            BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcPtr> CreateRcFromAllocated(StrongRef&& strongRef) noexcept {

        return RcPtr::FromStrongRefAndRcMemoryLayoutController(std::move(strongRef), RcMemoryLayoutController{});
    }

    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<
            std::is_same_v<RcMemoryLayoutControllerT, RcMemoryLayoutController> &&
            std::is_default_constructible_v<RcMemoryLayoutController> &&
            !BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static RcPtr CreateRcFromAllocated(StrongRef&& strongRef) noexcept {

        return RcPtr::FromStrongRefAndRcMemoryLayoutController(std::move(strongRef), RcMemoryLayoutController{});
    }

    /**
     * @brief Create an RcPtr from a 'StrongRef' and 'RcMemoryLayoutController'.
     * @tparam RcMemoryLayoutControllerT The type of the controller (RcMemoryLayoutController must be contruction from RcMemoryLayoutControllerT).
     * @param controller The RcMemoryLayoutControllerT controller instance.
     * @param strongRef A valid, allocated strong reference.
     * @return Optional containing a new RcPtr, or Nullopt if counter overflow when 'RcPtrTrait::OverflowCheck' enabled.
     * @example
     * 
     * Alt::RcIntrusiveMemoryLayoutController<Alt::String, true> rcMemLayoutController{};
     * Alt::RcIntrusiveMemoryLayoutController<Alt::String, true>::StrongRef* pStrongRef = rcMemLayoutController.AllocateStrongRef() iferr_return;
     * 
     * Alt::RcPtr<Alt::String> rcPtr = Alt::RcPtr<Alt::String>::CreateRcFromAllocatedWithController(std::move(*pStrongRef), std::move(rcMemLayoutController)) iferr_return;
     */
    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<
            std::is_constructible_v<RcMemoryLayoutController, RcMemoryLayoutControllerT&&> &&
            BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<RcPtr> CreateRcFromAllocatedWithController(RcMemoryLayoutControllerT&& controller, StrongRef&& strongRef) noexcept {

        return RcPtr::FromStrongRefAndRcMemoryLayoutController(std::move(strongRef), std::forward<RcMemoryLayoutControllerT>(controller));
    }

    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<
            std::is_constructible_v<RcMemoryLayoutController, RcMemoryLayoutControllerT&&> &&
            !BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static RcPtr CreateRcFromAllocatedWithController(RcMemoryLayoutControllerT&& controller, StrongRef&& strongRef) noexcept {

        return RcPtr::FromStrongRefAndRcMemoryLayoutController(std::move(strongRef), std::forward<RcMemoryLayoutControllerT>(controller));
    }

    ALT_FORCE_INLINE RcPtr() noexcept = default;

    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<
            std::is_same_v<RcMemoryLayoutControllerT, RcMemoryLayoutController> && std::is_default_constructible_v<RcMemoryLayoutControllerT>
        >* = nullptr>
    ALT_FORCE_INLINE RcPtr(StrongRef&& strongRef) noexcept
        : Super(std::move(strongRef), RcMemoryLayoutControllerT{})
    {}

    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcMemoryLayoutControllerT, RcMemoryLayoutController>>* = nullptr>
    ALT_FORCE_INLINE RcPtr(StrongRef&& strongRef, RcMemoryLayoutControllerT&& controller) noexcept
        : Super(std::move(strongRef), std::forward<RcMemoryLayoutControllerT>(controller))
    {}

    ALT_FORCE_INLINE RcPtr(const RcPtr&) noexcept = default;
    ALT_FORCE_INLINE RcPtr& operator=(const RcPtr&) noexcept = default;

    ALT_FORCE_INLINE RcPtr(RcPtr&&) noexcept = default;
    ALT_FORCE_INLINE RcPtr& operator=(RcPtr&&) noexcept = default;

    ALT_FORCE_INLINE RcPtr(std::nullptr_t) noexcept
        : Super(nullptr)
    {}

    ALT_FORCE_INLINE RcPtr& operator=(std::nullptr_t) noexcept {

        return Super::operator=(*this, nullptr);
    }

    ALT_FORCE_INLINE ~RcPtr() noexcept {

        this->m_Ptr.DecStrongRef();
    }

    /**
     * Clonable Trait 
     * 
     * @brief Clones the rc pointer from another instance.
     * @param rhs The source weak pointer to copy from.
     * @return True if successful, false if counter overflow when 'RcPtrTrait::OverflowCheck' enabled.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool CloneFrom(const RcPtr& rhs) noexcept {
        iferr_scope;

        this->Assign(_RcPtrDetails::Rule5Copy, rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool CloneConstructFrom(const RcPtr& rhs) noexcept {

        iferr_scope_on_err {

            Memory::DestroyObject(*this);
        };

        Memory::ConstructDefaultObject<RcPtr>(this);
        CloneFrom(rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<RcPtr> Clone() const noexcept {
        iferr_scope;

        RcPtr cloned;
        cloned.Assign(_RcPtrDetails::Rule5Copy, *this) iferr_return Nullopt;

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
        std::enable_if_t<std::is_same_v<U, StrongRef> && BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Reset(StrongRef&& strongRef = StrongRef{}) noexcept {
        iferr_scope;

        const T* ptr = this->m_Ptr.AsRcMemoryLayoutController()
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
        std::enable_if_t<std::is_same_v<U, StrongRef> && !BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)>* = nullptr>
    ALT_FORCE_INLINE bool Reset(StrongRef&& strongRef = StrongRef{}) noexcept {

        const T* ptr = this->m_Ptr.AsRcMemoryLayoutController()
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

    ALT_FORCE_INLINE bool operator==(const RcPtr& rhs) const noexcept {
        return this->m_Ptr.Get() == rhs.m_Ptr.Get();
    }

    ALT_FORCE_INLINE bool operator!=(const RcPtr& rhs) const noexcept {
        return !(*this == rhs);
    }

    ALT_FORCE_INLINE bool operator<(const RcPtr& rhs) const noexcept {
        return this->m_Ptr.Get() < rhs.m_Ptr.Get();
    }

    ALT_FORCE_INLINE bool operator>(const RcPtr& rhs) const noexcept {
        return rhs < *this;
    }

    ALT_FORCE_INLINE bool operator<=(const RcPtr& rhs) const noexcept {
        return !(*this > rhs);
    }

    ALT_FORCE_INLINE bool operator>=(const RcPtr& rhs) const noexcept {
        return !(*this < rhs);
    }

private:

    template <typename, RcPtrTrait, typename, bool>
    friend class _RcPtrDetails::RcPtrWeakResolver;

    template <typename, RcPtrTrait, typename, bool>
    friend class _RcPtrDetails::RcWeakPtrOverflowResolver;

    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcMemoryLayoutControllerT, RcMemoryLayoutController>>* = nullptr>
    static ALT_FORCE_INLINE Optional<RcPtr<T, TRAIT, RcMemoryLayoutControllerT>> FromStrongRefIncAndRcMemoryLayoutController(
        StrongRef&& strongRef, RcMemoryLayoutControllerT&& controller
    ) noexcept {

        Ptr ptr(std::move(strongRef), std::forward<RcMemoryLayoutControllerT>(controller));
        return RcPtr<T, TRAIT, RcMemoryLayoutControllerT>(std::move(ptr));
    }

    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcMemoryLayoutControllerT, RcMemoryLayoutController> && BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)>* = nullptr>
    static ALT_FORCE_INLINE Optional<RcPtr<T, TRAIT, RcMemoryLayoutControllerT>> FromStrongRefAndRcMemoryLayoutController(
        StrongRef&& strongRef, RcMemoryLayoutControllerT&& controller
    ) noexcept {

        iferr_scope;

        Ptr ptr(std::move(strongRef), std::forward<RcMemoryLayoutControllerT>(controller));
        ptr.IncStrongRef() iferr_return Nullopt;

        return RcPtr<T, TRAIT, RcMemoryLayoutControllerT>(std::move(ptr));
    }

    template <typename RcMemoryLayoutControllerT = RcMemoryLayoutController,
        std::enable_if_t<std::is_same_v<RcMemoryLayoutControllerT, RcMemoryLayoutController> && !BitFlag::IsSetEnumFlags(TRAIT, RcPtrTrait::OverflowCheck)>* = nullptr>
    static ALT_FORCE_INLINE RcPtr<T, TRAIT, RcMemoryLayoutControllerT> FromStrongRefAndRcMemoryLayoutController(
        StrongRef&& strongRef, RcMemoryLayoutControllerT&& controller
    ) noexcept {

        Ptr ptr(std::move(strongRef), std::forward<RcMemoryLayoutControllerT>(controller));
        ALT_MAYBE_UNUSED const bool isOverflow = ptr.IncStrongRef();

        return RcPtr<T, TRAIT, RcMemoryLayoutControllerT>(std::move(ptr));
    }

    ALT_FORCE_INLINE RcPtr(Ptr&& ptr) noexcept
        : Super(std::move(ptr))
    {}

    ALT_FORCE_INLINE RcPtr(const Ptr& ptr) noexcept
        : Super(ptr)
    {}
};

} // namespace Alt

#endif // __ALT_STD_RC_PTR__