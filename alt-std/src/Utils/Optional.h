#pragma once

#ifndef __ALT_STD_OPTIONAL__
#define __ALT_STD_OPTIONAL__

#include "../TypeTraits.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Memory/Ref.h"
#include "../Memory/Ptr.h"
#include "../Memory/MaybeUninit.h"

namespace Alt {

/**
 * @struct Nullopt_t
 * @brief A tag type representing an 'nullopt' state.
 * @details Used primarily for initializing Optional objects to an 'nullopt' state and for comparison.
 * 
 * @example
 * 
 * Alt::Optional<usize> opt = Alt::Nullopt;
 * if (opt == Alt::Nullopt) {
 * }
 */
struct Nullopt_t {};

/**
 * @var Nullopt
 * @brief A global constant of type Nullopt_t representing the absence of a value.
 * @details Represents the 'nullopt' state in all Optional operations.
 * 
 * @example
 * 
 * Alt::Optional<usize> opt = Alt::MakeOptional(4);
 * opt = Alt::Nullopt;
 */
constexpr static Nullopt_t Nullopt = Nullopt_t{};

template <typename T>
class Optional;

/**
 * @brief Factory function to create an Optional containing a value.
 * @tparam T The type of the target Optional.
 * @tparam ArgsT The types of the arguments to be passed to the constructor.
 * @param args The arguments used to construct the value of type T.
 * @return A new Optional<T> instance populated with the constructed value.
 * 
 * @example
 * 
 * Alt::Optional<usize> opt0 = Alt::MakeOptional<usize>(4);
 * Alt::Optional<Alt::String> opt1 = Alt::MakeOptional<Alt::String>("Optional");
 * Alt::Optional<Alt::Pair<usize, usize>> opt2 = Alt::MakeOptional<Alt::Pair<usize, usize>>(4, 8);
 */
template <typename T, typename ...ArgsT,
    std::enable_if_t<std::is_constructible_v<T, ArgsT&&...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<T> MakeOptional(ArgsT&&... args) noexcept {
    return Optional<T>(T(std::forward<ArgsT>(args)...));
}

template <typename T,
    std::enable_if_t<std::is_constructible_v<T, T&&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<T> MakeOptional(T&& val) noexcept {
    return Optional<T>(std::forward<T>(val));
}

namespace TypeTraits {

template <typename T>
struct __is_optional__<Optional<T>>
    : std::true_type
{};

template <typename T>
struct __is_trivially_reallocatable__<Optional<T>>
    : is_trivially_reallocatable_t<TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<T>>
{};

} // namespace TypeTraits

/** \cond */
namespace _OptionalDetails {

template <typename T>
using clean_t = TypeTraits::clean_t<T>;

enum class OptionalLayoutTrait { Value, Ptr };
enum class TypeEntityTrait { Value, Pointer, AltSmartPtr, Reference, AltRef };

template <OptionalLayoutTrait LAYOUT_TRAIT>
using OptionalLayout = std::integral_constant<OptionalLayoutTrait, LAYOUT_TRAIT>;

template <TypeEntityTrait TYPE_ENTITY>
using TypeEntity = std::integral_constant<TypeEntityTrait, TYPE_ENTITY>;

template <typename T>
struct optional_layout_trait
    : std::conditional_t<
        std::is_pointer_v<T> || TypeTraits::is_smart_pointer_v<T> ||
            std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T>,
        OptionalLayout<OptionalLayoutTrait::Ptr>,
        OptionalLayout<OptionalLayoutTrait::Value>
    >
{};

template <typename T>
constexpr static OptionalLayoutTrait optional_layout_trait_v = optional_layout_trait<clean_t<T>>::value;

template <typename T>
struct type_entity_trait
    : std::conditional_t<
        std::is_pointer_v<T>,
        TypeEntity<TypeEntityTrait::Pointer>,

        std::conditional_t<
            TypeTraits::is_smart_pointer_v<T>,
            TypeEntity<TypeEntityTrait::AltSmartPtr>,

            std::conditional_t<
                std::is_reference_v<T>,
                TypeEntity<TypeEntityTrait::Reference>,

                std::conditional_t<
                    TypeTraits::is_alt_ref_v<T>,
                    TypeEntity<TypeEntityTrait::AltRef>,

                    TypeEntity<TypeEntityTrait::Value>
                >
            >
        >
    >
{};

template <typename T>
constexpr static TypeEntityTrait type_entity_trait_v = type_entity_trait<clean_t<T>>::value;

template <typename T>
constexpr static bool is_layout_not_own_value_v =
    type_entity_trait_v<T> == TypeEntityTrait::Pointer ||
    type_entity_trait_v<T> == TypeEntityTrait::Reference ||
    type_entity_trait_v<T> == TypeEntityTrait::AltRef
;

template <typename T>
constexpr static bool is_layout_own_value_v = !is_layout_not_own_value_v<T>;

template <typename T, TypeEntityTrait TYPE_ENTITY_TRAIT = type_entity_trait_v<T>>
struct unwrap_to_clean_ptr;

template <typename T>
struct unwrap_to_clean_ptr<T, TypeEntityTrait::Value> {

    using Type = T*;
};

template <typename T>
struct unwrap_to_clean_ptr<T, TypeEntityTrait::Pointer> {

    using Type = T;
};

template <typename T>
struct unwrap_to_clean_ptr<T, TypeEntityTrait::AltSmartPtr> {

    using Type = T;
};

template <typename T>
struct unwrap_to_clean_ptr<T, TypeEntityTrait::Reference> {

    using Type = _RefDetails::Ptr<std::remove_reference_t<T>>;
};

template <typename T>
struct unwrap_to_clean_ptr<T, TypeEntityTrait::AltRef> {

    using Type = _RefDetails::Ptr<typename T::Type>;
};

template <typename T>
using unwrap_to_clean_ptr_t = typename unwrap_to_clean_ptr<T>::Type;

template <typename T, TypeEntityTrait TYPE_ENTITY_TRAIT = type_entity_trait_v<T>>
struct unwrap_to_type_deref;

template <typename T>
struct unwrap_to_type_deref<T, TypeEntityTrait::Value> {

    using Type = T;
};

template <typename T>
struct unwrap_to_type_deref<T, TypeEntityTrait::Pointer> {

    using Type = std::remove_pointer_t<T>;
};

template <typename T>
struct unwrap_to_type_deref<T, TypeEntityTrait::AltSmartPtr> {

    using Type = T;
};

template <typename T>
struct unwrap_to_type_deref<T, TypeEntityTrait::Reference> {

    using Type = std::remove_reference_t<T>;
};

template <typename T>
struct unwrap_to_type_deref<T, TypeEntityTrait::AltRef> {

    using Type = typename T::Type;
};

template <typename T>
using unwrap_to_type_deref_t = typename unwrap_to_type_deref<T>::Type;

struct Rule5Copy_t {};
struct Rule5Move_t {};
struct InnerValue_t {};

constexpr static Rule5Copy_t Rule5Copy = Rule5Copy_t{};
constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};
constexpr static InnerValue_t InnerValue = InnerValue_t{};

template <typename T, OptionalLayoutTrait = optional_layout_trait_v<T>>
struct OptionalStorageCore;

template <typename T>
struct OptionalStorageCore<T, OptionalLayoutTrait::Value> {

    MaybeUninit<T> m_Val;
    bool m_IsVal = false;
};

template <typename T>
struct OptionalStorageCore<T, OptionalLayoutTrait::Ptr> {

    // yes, we want 'MaybeUninit', because 'T' can be smart pointer (some struct with data)
    MaybeUninit<unwrap_to_clean_ptr_t<T>> m_PtrVal;
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_destructible_v<T>>
struct OptionalStorageDestruct;

template <typename SelfT, typename T>
struct OptionalStorageDestruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted>
    : OptionalStorageCore<T>
{
    OptionalStorageDestruct() noexcept = default;

    OptionalStorageDestruct(const OptionalStorageDestruct&) noexcept = default;
    OptionalStorageDestruct& operator=(const OptionalStorageDestruct&) noexcept = default;

    OptionalStorageDestruct(OptionalStorageDestruct&&) noexcept = default;
    OptionalStorageDestruct& operator=(OptionalStorageDestruct&&) noexcept = default;

    ~OptionalStorageDestruct() noexcept = delete;
};

template <typename SelfT, typename T>
struct OptionalStorageDestruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial>
    : OptionalStorageCore<T>
{
    OptionalStorageDestruct() noexcept = default;

    OptionalStorageDestruct(const OptionalStorageDestruct&) noexcept = default;
    OptionalStorageDestruct& operator=(const OptionalStorageDestruct&) noexcept = default;

    OptionalStorageDestruct(OptionalStorageDestruct&&) noexcept = default;
    OptionalStorageDestruct& operator=(OptionalStorageDestruct&&) noexcept = default;

    ~OptionalStorageDestruct() noexcept = default;
};

template <typename SelfT, typename T>
struct OptionalStorageDestruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : OptionalStorageCore<T>
{
    OptionalStorageDestruct() noexcept = default;

    OptionalStorageDestruct(const OptionalStorageDestruct&) noexcept = default;
    OptionalStorageDestruct& operator=(const OptionalStorageDestruct&) noexcept = default;

    OptionalStorageDestruct(OptionalStorageDestruct&&) noexcept = default;
    OptionalStorageDestruct& operator=(OptionalStorageDestruct&&) noexcept = default;

    ~OptionalStorageDestruct() noexcept {

        static_cast<SelfT*>(this)->Destroy();
    }
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_assignable_v<T>>
struct OptionalStorageMoveAssign;

template <typename SelfT, typename T>
struct OptionalStorageMoveAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted>
    : OptionalStorageDestruct<SelfT, T>
{
    OptionalStorageMoveAssign() noexcept = default;

    OptionalStorageMoveAssign(const OptionalStorageMoveAssign&) noexcept = default;
    OptionalStorageMoveAssign& operator=(const OptionalStorageMoveAssign&) noexcept = default;

    OptionalStorageMoveAssign(OptionalStorageMoveAssign&&) noexcept = default;
    OptionalStorageMoveAssign& operator=(OptionalStorageMoveAssign&&) noexcept = delete;
};

template <typename SelfT, typename T>
struct OptionalStorageMoveAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial>
    : OptionalStorageDestruct<SelfT, T>
{
    OptionalStorageMoveAssign() noexcept = default;

    OptionalStorageMoveAssign(const OptionalStorageMoveAssign&) noexcept = default;
    OptionalStorageMoveAssign& operator=(const OptionalStorageMoveAssign&) noexcept = default;

    OptionalStorageMoveAssign(OptionalStorageMoveAssign&&) noexcept = default;
    OptionalStorageMoveAssign& operator=(OptionalStorageMoveAssign&&) noexcept = default;
};

template <typename SelfT, typename T>
struct OptionalStorageMoveAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : OptionalStorageDestruct<SelfT, T>
{
    OptionalStorageMoveAssign() noexcept = default;

    OptionalStorageMoveAssign(const OptionalStorageMoveAssign&) noexcept = default;
    OptionalStorageMoveAssign& operator=(const OptionalStorageMoveAssign&) noexcept = default;

    OptionalStorageMoveAssign(OptionalStorageMoveAssign&&) noexcept = default;

    OptionalStorageMoveAssign& operator=(OptionalStorageMoveAssign&& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Move, static_cast<SelfT&>(rhs));
        return *this;
    }
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_constructible_v<T>>
struct OptionalStorageMoveConstruct;

template <typename SelfT, typename T>
struct OptionalStorageMoveConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted>
    : OptionalStorageMoveAssign<SelfT, T>
{
    OptionalStorageMoveConstruct() noexcept = default;

    OptionalStorageMoveConstruct(const OptionalStorageMoveConstruct&) noexcept = default;
    OptionalStorageMoveConstruct& operator=(const OptionalStorageMoveConstruct&) noexcept = default;

    OptionalStorageMoveConstruct(OptionalStorageMoveConstruct&&) noexcept = delete;
    OptionalStorageMoveConstruct& operator=(OptionalStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T>
struct OptionalStorageMoveConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial>
    : OptionalStorageMoveAssign<SelfT, T>
{
    OptionalStorageMoveConstruct() noexcept = default;

    OptionalStorageMoveConstruct(const OptionalStorageMoveConstruct&) noexcept = default;
    OptionalStorageMoveConstruct& operator=(const OptionalStorageMoveConstruct&) noexcept = default;

    OptionalStorageMoveConstruct(OptionalStorageMoveConstruct&&) noexcept = default;
    OptionalStorageMoveConstruct& operator=(OptionalStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T>
struct OptionalStorageMoveConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : OptionalStorageMoveAssign<SelfT, T>
{
    OptionalStorageMoveConstruct() noexcept = default;

    OptionalStorageMoveConstruct(const OptionalStorageMoveConstruct&) noexcept = default;
    OptionalStorageMoveConstruct& operator=(const OptionalStorageMoveConstruct&) noexcept = default;

    OptionalStorageMoveConstruct(OptionalStorageMoveConstruct&& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Move, static_cast<SelfT&>(rhs));
    }

    OptionalStorageMoveConstruct& operator=(OptionalStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_assignable_v<T>>
struct OptionalStorageCopyAssign;

template <typename SelfT, typename T>
struct OptionalStorageCopyAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted>
    : OptionalStorageMoveConstruct<SelfT, T>
{
    OptionalStorageCopyAssign() noexcept = default;

    OptionalStorageCopyAssign(const OptionalStorageCopyAssign&) noexcept = default;
    OptionalStorageCopyAssign& operator=(const OptionalStorageCopyAssign&) noexcept = delete;

    OptionalStorageCopyAssign(OptionalStorageCopyAssign&&) noexcept = default;
    OptionalStorageCopyAssign& operator=(OptionalStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T>
struct OptionalStorageCopyAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial>
    : OptionalStorageMoveConstruct<SelfT, T>
{
    OptionalStorageCopyAssign() noexcept = default;

    OptionalStorageCopyAssign(const OptionalStorageCopyAssign&) noexcept = default;
    OptionalStorageCopyAssign& operator=(const OptionalStorageCopyAssign&) noexcept = default;

    OptionalStorageCopyAssign(OptionalStorageCopyAssign&&) noexcept = default;
    OptionalStorageCopyAssign& operator=(OptionalStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T>
struct OptionalStorageCopyAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : OptionalStorageMoveConstruct<SelfT, T>
{
    OptionalStorageCopyAssign() noexcept = default;

    OptionalStorageCopyAssign(const OptionalStorageCopyAssign&) noexcept = default;

    OptionalStorageCopyAssign& operator=(const OptionalStorageCopyAssign& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Copy, static_cast<const SelfT&>(rhs));
        return *this;
    }

    OptionalStorageCopyAssign(OptionalStorageCopyAssign&&) noexcept = default;
    OptionalStorageCopyAssign& operator=(OptionalStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_constructible_v<T>>
struct OptionalStorageCopyConstruct;

template <typename SelfT, typename T>
struct OptionalStorageCopyConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted>
    : OptionalStorageCopyAssign<SelfT, T>
{
    OptionalStorageCopyConstruct() noexcept = default;

    OptionalStorageCopyConstruct(const OptionalStorageCopyConstruct&) noexcept = delete;
    OptionalStorageCopyConstruct& operator=(const OptionalStorageCopyConstruct&) noexcept = default;

    OptionalStorageCopyConstruct(OptionalStorageCopyConstruct&&) noexcept = default;
    OptionalStorageCopyConstruct& operator=(OptionalStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T>
struct OptionalStorageCopyConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial>
    : OptionalStorageCopyAssign<SelfT, T>
{
    OptionalStorageCopyConstruct() noexcept = default;

    OptionalStorageCopyConstruct(const OptionalStorageCopyConstruct&) noexcept = default;
    OptionalStorageCopyConstruct& operator=(const OptionalStorageCopyConstruct&) noexcept = default;

    OptionalStorageCopyConstruct(OptionalStorageCopyConstruct&&) noexcept = default;
    OptionalStorageCopyConstruct& operator=(OptionalStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T>
struct OptionalStorageCopyConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : OptionalStorageCopyAssign<SelfT, T>
{
    OptionalStorageCopyConstruct() noexcept = default;

    OptionalStorageCopyConstruct(const OptionalStorageCopyConstruct& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Copy, static_cast<const SelfT&>(rhs));
    }

    OptionalStorageCopyConstruct& operator=(const OptionalStorageCopyConstruct&) noexcept = default;

    OptionalStorageCopyConstruct(OptionalStorageCopyConstruct&&) noexcept = default;
    OptionalStorageCopyConstruct& operator=(OptionalStorageCopyConstruct&&) noexcept = default;
};

template <typename T, OptionalLayoutTrait = optional_layout_trait_v<T>>
struct OptionalStorage;

template <typename T>
struct OptionalStorage<T, OptionalLayoutTrait::Value>
    : OptionalStorageCopyConstruct<OptionalStorage<T>, T>
{
    using DerefT = unwrap_to_type_deref_t<T>;

    template <typename U>
    constexpr static bool is_constructible_inner_value_v = std::is_constructible_v<T, U>;

    template <typename U>
    constexpr static bool is_assignable_inner_value_v = std::is_assignable_v<T&, U> && std::is_constructible_v<T, U>;

    OptionalStorage() noexcept = default;

    OptionalStorage(Nullopt_t) noexcept
    {}

    void Assign(Nullopt_t) noexcept {

        if (this->m_IsVal) {

            this->m_Val.Destroy();
            this->m_IsVal = false;
        }
    }

    template <typename U = T>
    void Construct(InnerValue_t, U&& val) noexcept {

        this->m_Val.Construct(std::forward<U>(val));
        this->m_IsVal = true;
    }

    template <typename U = T>
    void Assign(InnerValue_t, U&& rhs) noexcept {

        if (this->m_IsVal) {

            this->m_Val.Assign(std::forward<U>(rhs));
        }
        else {

            this->m_Val.Construct(std::forward<U>(rhs));
            this->m_IsVal = true;
        }
    }

    template <typename U = T>
    bool ConstructCloneOrMove(InnerValue_t, U&& val) noexcept {

        const bool isOk = this->m_Val.ConstructCloneOrMove(std::forward<U>(val));
        this->m_IsVal = isOk;

        return isOk;
    }

    template <typename U = T>
    bool AssignCloneOrMove(InnerValue_t, U&& rhs) noexcept {

        bool isOk;
        if (this->m_IsVal) {

            isOk = this->m_Val.AssignCloneOrMove(std::forward<U>(rhs));
        }
        else {

            isOk = this->m_Val.ConstructCloneOrMove(std::forward<U>(rhs));
            this->m_IsVal = isOk;
        }

        return isOk;
    }

    template <typename U = T>
    void Construct(Rule5Move_t, OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            this->m_Val.Construct(std::move(storage.GetVal()));
            this->m_IsVal = true;
        }
    }

    template <typename U = T>
    void Construct(Rule5Copy_t, const OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            this->m_Val.Construct(storage.GetVal());
            this->m_IsVal = true;
        }
    }

    template <typename U = T>
    void Assign(Rule5Move_t, OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            Assign(InnerValue, std::move(storage.GetVal()));
        }
        else {

            Assign(Nullopt);
        }
    }

    template <typename U = T>
    void Assign(Rule5Copy_t, const OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            Assign(InnerValue, storage.GetVal());
        }
        else {

            Assign(Nullopt);
        }
    }
    
    void Destroy() noexcept {

        Assign(Nullopt);
    }

    template <typename U = T>
    bool ConstructClone(const OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            return ConstructCloneOrMove(InnerValue, storage.GetVal());
        }

        return true;
    }

    template <typename U = T>
    bool AssignClone(const OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            return AssignCloneOrMove(InnerValue, storage.GetVal());
        }
        else {

            Assign(Nullopt);
        }

        return true;
    }

    T& GetVal() & noexcept { return this->m_Val.GetVal(); }
    const T& GetVal() const & noexcept { return this->m_Val.GetVal(); }
    T&& GetVal() && noexcept { return std::move(this->m_Val.GetVal()); }
    const T&& GetVal() const && noexcept { return std::move(this->m_Val.GetVal()); }

    bool IsVal() const noexcept { return this->m_IsVal; }

    DerefT* Deref() noexcept { return this->m_Val.GetPtr(); }
    const DerefT* Deref() const noexcept { return this->m_Val.GetPtr(); }
};

template <typename T>
struct OptionalStorage<T, OptionalLayoutTrait::Ptr>
    : OptionalStorageCopyConstruct<OptionalStorage<T>, T>
{
    using DerefT = unwrap_to_type_deref_t<T>;

    template <typename U>
    constexpr static bool is_constructible_inner_value_v = std::is_constructible_v<T, U>;

    template <typename U>
    constexpr static bool is_assignable_inner_value_v = std::is_assignable_v<T&, U>;

    OptionalStorage() noexcept = default;

    OptionalStorage(Nullopt_t) noexcept {

        this->m_PtrVal.Construct(nullptr);
    }

    void Assign(Nullopt_t) noexcept {

        this->m_PtrVal.Assign(nullptr);
    }

    template <typename U = T>
    void Construct(InnerValue_t, U&& val) noexcept {

        this->m_PtrVal.Construct(std::forward<U>(val));
    }

    template <typename U = T>
    void Assign(InnerValue_t, U&& rhs) noexcept {

        this->m_PtrVal.Assign(std::forward<U>(rhs));
    }

    template <typename U = T>
    bool ConstructCloneOrMove(InnerValue_t, U&& val) noexcept {

        return this->m_PtrVal.ConstructCloneOrMove(std::forward<U>(val));
    }

    template <typename U = T>
    bool AssignCloneOrMove(InnerValue_t, U&& rhs) noexcept {

        return this->m_PtrVal.AssignCloneOrMove(std::forward<U>(rhs));
    }

    template <typename U = T>
    void Construct(Rule5Move_t, OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            this->m_PtrVal.Construct(std::move(storage.GetVal()));
        }
        else {

            this->m_PtrVal.Construct(nullptr);
        }
    }

    template <typename U = T>
    void Construct(Rule5Copy_t, const OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            this->m_PtrVal.Construct(storage.GetVal());
        }
        else {

            this->m_PtrVal.Construct(nullptr);
        }
    }

    template <typename U = T>
    void Assign(Rule5Move_t, OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            Assign(InnerValue, std::move(storage.GetVal()));
        }
        else {

            Assign(Nullopt);
        }
    }

    template <typename U = T>
    void Assign(Rule5Copy_t, const OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            Assign(InnerValue, storage.GetVal());
        }
        else {

            Assign(Nullopt);
        }
    }

    void Destroy() noexcept {

        this->m_PtrVal.Destroy();
    }

    template <typename U = T>
    bool ConstructClone(const OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            return ConstructCloneOrMove(InnerValue, storage.GetVal());
        }
        else {

            this->m_PtrVal.Construct(nullptr);
        }

        return true;
    }

    template <typename U = T>
    bool AssignClone(const OptionalStorage<U>& storage) noexcept {

        if (storage.IsVal()) {

            return AssignCloneOrMove(InnerValue, storage.GetVal());
        }
        else {

            Assign(Nullopt);
        }

        return true;
    }

    T& GetVal() & noexcept { return this->m_PtrVal.GetVal(); }
    const T& GetVal() const & noexcept { return this->m_PtrVal.GetVal(); }
    T&& GetVal() && noexcept { return std::move(this->m_PtrVal.GetVal()); }
    const T&& GetVal() const && noexcept { return std::move(this->m_PtrVal.GetVal()); }

    bool IsVal() const noexcept { return this->m_PtrVal.GetVal() != nullptr; }

    template <typename U = T,
        std::enable_if_t<is_layout_own_value_v<U>>* = nullptr>
    DerefT* Deref() noexcept { return this->m_PtrVal.GetPtr(); }

    template <typename U = T,
        std::enable_if_t<is_layout_not_own_value_v<U>>* = nullptr>
    DerefT* Deref() noexcept { return &*this->m_PtrVal.GetVal(); }

    template <typename U = T,
        std::enable_if_t<is_layout_own_value_v<U>>* = nullptr>
    const DerefT* Deref() const noexcept { return this->m_PtrVal.GetPtr(); }

    template <typename U = T,
        std::enable_if_t<is_layout_not_own_value_v<U>>* = nullptr>
    const DerefT* Deref() const noexcept { return &*this->m_PtrVal.GetVal(); }
};

} // namespace _OptionalDetails
/** \endcond */

/**
 * @class Optional<T>
 * @brief Type-safe container for a value that may or may not exist.
 * 
 * @details This class is optimized to use the most efficient memory layout (Value or Pointer) 
 * based on the type T. It provides monadic operations like `Map` and safe access via `GetValueOr`.
 * 
 * @tparam T The type of the value contained within the Optional.
 * 
 * @example
 * 
 * Alt::Optional<usize> opt = 4;
 * if (opt) {
 *     usize value = *opt;
 * }
 */
template <typename T_>
class Optional {
private:

    using CleanT = TypeTraits::clean_t<T_>;

    using T = TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<CleanT>;

    using OptionalStorage = _OptionalDetails::OptionalStorage<T>;

    using DerefT = typename OptionalStorage::DerefT;

    template <typename U>
    constexpr static bool is_constructible_inner_value_v = OptionalStorage::template is_constructible_inner_value_v<U>;

    template <typename U>
    constexpr static bool is_assignable_inner_value_v = OptionalStorage::template is_assignable_inner_value_v<U>;

public:

    using Type = T;

    /**
     * @brief Creates an Optional containing a valid value.
     * @tparam U The type of the value to wrap.
     * @param val The value to be wrapped.
     * @return An Optional<T> populated with the provided value.
     * @example
     * 
     * auto opt = Alt::Optional<usize>::Ok(4);
     * auto optStr = Alt::Optional<Alt::String>::Ok("Optional");
     */
    template <typename U = T,
        std::enable_if_t<is_constructible_inner_value_v<U&&>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional Ok(U&& val) noexcept {
        return Optional(std::forward<U>(val));
    }

    /**
     * @brief Creates an 'nullopt' Optional.
     * @return An Alt::Nullopt instance.
     * @example
     * 
     * Alt::Optional<usize> opt = Alt::Optional<usize>::Null();
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Optional Null() noexcept {
        return Nullopt;
    }

    /**
     * @brief Default constructor.
     * @details Initializes the container in an 'nullopt' state.
     * @example
     * 
     * Alt::Optional<usize> opt;
     */
    ALT_FORCE_INLINE Optional() noexcept
        : m_Storage(Nullopt)
    {}

    /**
     * @brief Constructs an 'nullopt' Optional using the Nullopt tag.
     * @param val The Nullopt_t tag.
     * @example
     * 
     * Alt::Optional<usize> opt{ Alt::Nullopt };
     * Alt::Optional<usize> opt = Alt::Nullopt;
     */
    ALT_FORCE_INLINE Optional(Nullopt_t) noexcept
        : m_Storage(Nullopt)
    {}

    Optional& operator=(Nullopt_t) noexcept {

        m_Storage.Assign(Nullopt);
        return *this;
    }

    /**
     * @brief Constructs an Optional from a raw value.
     * @tparam U The type of the input value.
     * @param val The value to wrap.
     * @example
     * 
     * Alt::Optional<usize> opt(4);
     */
    template <typename U = T,
        std::enable_if_t<is_constructible_inner_value_v<U&&>>* = nullptr>
    Optional(U&& val) noexcept {

        m_Storage.Construct(_OptionalDetails::InnerValue, std::forward<U>(val));
    }

    /**
     * @brief Assigns a raw value to the Optional.
     * @tparam U The type of the value.
     * @param rhs The value to store.
     * @example
     * 
     * Alt::Optional<usize> opt;
     * opt = 20;
     */
    template <typename U = T,
        std::enable_if_t<is_assignable_inner_value_v<U&&>>* = nullptr>
    Optional& operator=(U&& rhs) noexcept {

        m_Storage.Assign(_OptionalDetails::InnerValue, std::forward<U>(rhs));
        return *this;
    }

    /**
     * @brief Constructs an Optional from another Optional with narrow cast.
     * @tparam U The type of the source Optional.
     * @param val The source Optional to move from.
     * @example
     * 
     * Alt::Optional<usize> opt0 = 4;
     * Alt::Optional<ssize> opt1 = source;
     */
    template <typename U = T,
        std::enable_if_t<!std::is_constructible_v<T, Optional<U>> && is_constructible_inner_value_v<U&&>>* = nullptr>
    Optional(Optional<U>&& val) noexcept {

        m_Storage.Construct(_OptionalDetails::Rule5Move, val.m_Storage);
    }

    template <typename U = T,
        std::enable_if_t<!std::is_constructible_v<T, Optional<U>> && is_constructible_inner_value_v<const U&>>* = nullptr>
    Optional(const Optional<U>& val) noexcept {

        m_Storage.Construct(_OptionalDetails::Rule5Copy, val.m_Storage);
    }

    /**
     * @brief Assigns an Optional from another Optional with narrow cast.
     * @tparam U The type of the source Optional.
     * @param rhs The source Optional.
     * @example
     * 
     * Alt::Optional<usize> opt0 = 4;
     * Alt::Optional<ssize> opt1 = 8;
     * opt1 = std::move(opt0);
     */
    template <typename U = T,
        std::enable_if_t<!std::is_assignable_v<T&, Optional<U>&&> && is_assignable_inner_value_v<U&&>>* = nullptr>
    Optional& operator=(Optional<U>&& rhs) noexcept {

        m_Storage.Assign(_OptionalDetails::Rule5Move, rhs.m_Storage);
        return *this;
    }

    template <typename U = T,
        std::enable_if_t<!std::is_assignable_v<T&, const Optional<U>&> && is_assignable_inner_value_v<const U&>>* = nullptr>
    Optional& operator=(const Optional<U>& rhs) noexcept {

        m_Storage.Assign(_OptionalDetails::Rule5Copy, rhs.m_Storage);
        return *this;
    }

    /**
     * Clonable Trait
     * 
     * @brief Clones the current Optional.
     * 
     * @tparam U The type of the source Optional.
     * @param val The destination Optional to clone.
     * @return A bool, false when clone failed.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Optional<Alt::Vector<usize>> opt0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
     * Alt::Optional<Alt::Vector<usize>> opt1;
     * 
     * opt1 = std::move(opt0);
     * opt0.CloneFrom(opt1) iferr_return;
     * 
     * assert(*opt0, *opt1);
     */
    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<U, T> || is_assignable_inner_value_v<const U&>>* = nullptr>
    ALT_NODISCARD bool CloneFrom(const Optional<U>& rhs) noexcept {

        return m_Storage.AssignClone(rhs.m_Storage);
    }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<U, T> || is_constructible_inner_value_v<const U&>>* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const Optional<U>& rhs) noexcept {

        return m_Storage.ConstructClone(rhs.m_Storage);
    }

    ALT_NODISCARD Optional<Optional> Clone() const noexcept {

        struct Object {

            ~Object() noexcept {

                if (!isValid) {
                    return;
                }

                Optional* pObject = reinterpret_cast<Optional*>(memory);
                Memory::DestroyObject(pObject);

                isValid = false;
            }

            alignas(Optional) u8 memory[sizeof(Optional)];
            bool isValid = false;
        };

        Object object{};
        Optional* pObject = reinterpret_cast<Optional*>(object.memory);

        const bool isOk = pObject->CloneConstructFrom(*this);
        object.isValid = isOk;

        if (!isOk) {
            return Nullopt;
        }

        return MakeOptional(std::move(*pObject));
    }

    /**
     * @brief Checks if the Optional is 'nullopt'.
     * @return true if the Optional is 'nullopt' (matches Nullopt), false otherwise.
     * @example
     * 
     * if (opt == Alt::Nullopt) {}
     */
    ALT_FORCE_INLINE bool operator==(Nullopt_t) const noexcept { return !IsOk(); }
    ALT_FORCE_INLINE bool operator!=(Nullopt_t) const noexcept { return IsOk(); }

    /**
     * @brief Dereferences the Optional to access the value.
     * @return T& or const T& or T&& (depending on the overload).
     * @example
     * 
     * usize val = *opt;
     */
    template <typename U = DerefT>
    ALT_FORCE_INLINE U& operator*() & noexcept { ALT_ASSERT(m_Storage.IsVal()); return *m_Storage.Deref(); }

    template <typename U = DerefT>
    ALT_FORCE_INLINE const U& operator*() const & noexcept { ALT_ASSERT(m_Storage.IsVal()); return *m_Storage.Deref(); }

    template <typename U = DerefT>
    ALT_FORCE_INLINE U&& operator*() && noexcept { ALT_ASSERT(m_Storage.IsVal()); return std::move(*m_Storage.Deref()); }

    template <typename U = DerefT>
    ALT_FORCE_INLINE const U&& operator*() const && noexcept { ALT_ASSERT(m_Storage.IsVal()); return std::move(*m_Storage.Deref()); }

    /**
     * @brief Accesses members of the contained object.
     * @example
     * 
     * Alt::Optional<SomeClass> opt = SomeClass{};
     * if (opt) opt->SomeMethod();
     */
    ALT_FORCE_INLINE DerefT* operator->() noexcept { ALT_ASSERT(m_Storage.IsVal()); return m_Storage.Deref(); }
    ALT_FORCE_INLINE const DerefT* operator->() const noexcept { ALT_ASSERT(m_Storage.IsVal()); return m_Storage.Deref(); }

    /**
     * @brief Implicit conversion to a pointer type.
     * @return A pointer to the value, or nullptr if empty.
     * @example
     * 
     * Alt::Optional<SomeClass*> opt = nullptr;
     * if (SomeClass* pSomeClass = opt) pSomeClass->SomeMethod();
     * 
     * Alt::Optional<SomeClass&> opt = Alt::Nullopt;
     * if (SomeClass* pSomeClass = opt) pSomeClass->SomeMethod();
     */
    template <typename U = T,
        std::enable_if_t<_OptionalDetails::is_layout_not_own_value_v<U>>* = nullptr>
    ALT_FORCE_INLINE operator DerefT*() noexcept { return m_Storage.Deref(); }

    template <typename U = T,
        std::enable_if_t<_OptionalDetails::is_layout_not_own_value_v<U>>* = nullptr>
    ALT_FORCE_INLINE operator const DerefT*() const noexcept { return m_Storage.Deref(); }

    /**
    * @brief Checks if the Optional contains a valid value.
    * @return true if present, false if empty.
    * @example
    * 
    * if (opt.IsOk()) {}
    */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsOk() const noexcept { return m_Storage.IsVal(); }

    /**
     * @brief Checks if the Optional contains a valid value.
     * @return true if present, false if empty.
     * @example
     * 
     * if (opt.IsPopulated()) {}
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept { return m_Storage.IsVal(); }

    /**
     * @brief Checks if the Optional is 'nullopt'.
     * @return true if empty, false if a value is present.
     * @example
     * 
     * if (opt.IsNone()) {}
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsNone() const noexcept { return !m_Storage.IsVal(); }

    /**
     * @brief Checks if the Optional contains a valid value.
     * @return true if present, false if empty.
     * @example
     * 
     * if (opt) {}
     */
    ALT_FORCE_INLINE explicit operator bool() const noexcept { return IsOk(); }

    /**
     * @brief Accesses the contained value.
     * @return The contained value (T&, const T&, or T&&).
     * @example
     * 
     * SomeClass& val = opt.GetValue();
     * const SomeClass& val = opt.GetValue();
     * SomeClass val = opt.GetValue();
     * SomeClass val = std::move(opt.GetValue());
     */
    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT>&, CleanT&>
        GetValue() & noexcept { ALT_ASSERT(m_Storage.IsVal()); return m_Storage.GetVal(); }

    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT>&, const CleanT&>
        GetValue() const & noexcept { ALT_ASSERT(m_Storage.IsVal()); return m_Storage.GetVal(); }

    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT>&, CleanT&&>
        GetValue() && noexcept { ALT_ASSERT(m_Storage.IsVal()); return std::move(m_Storage.GetVal()); }

    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT>&, const CleanT&&>
        GetValue() const && noexcept { ALT_ASSERT(m_Storage.IsVal()); return std::move(m_Storage.GetVal()); }

    /**
     * @brief Returns the contained value or a fallback value.
     * @tparam U The type of the fallback value. Must be convertible to T.
     * @param v The value to return if the Optional is 'nullopt'.
     * @return The contained value if present, otherwise the provided value.
     * @example
     * 
     * Alt::Optional<usize> opt = Alt::Nullopt;
     * usize val = opt.GetValueOr(0);
     * assert(val == 0);
     * 
     * Alt::Optional<Alt::String> optStr = Alt::Nullopt;
     * Alt::String str = optStr.GetValueOr("Empty");
     * assert(str == "Empty");
     */
    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOr(U&& v) & noexcept { return m_Storage.IsVal() ? m_Storage.GetVal() : static_cast<T>(std::forward<U>(v)); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOr(U&& v) const & noexcept { return m_Storage.IsVal() ? m_Storage.GetVal() : static_cast<T>(std::forward<U>(v)); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOr(U&& v) && noexcept { return m_Storage.IsVal() ? std::move(m_Storage).GetVal() : static_cast<T>(std::forward<U>(v)); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOr(U&& v) const && noexcept { return m_Storage.IsVal() ? std::move(m_Storage).GetVal() : static_cast<T>(std::forward<U>(v)); }

    /**
     * @brief Returns the contained value or a default-constructed value.
     * @tparam U The type used to create the default value. Defaults to T. Must be default constructable.
     * @return The contained value if present, otherwise a default-constructed value of type U.
     * @example
     * 
     * Alt::Optional<usize> opt = Alt::Nullopt;
     * usize val = opt.GetValueOrDefault();
     * assert(val == usize{});
     * 
     * Alt::Optional<Alt::String> optStr = Alt::Nullopt;
     * Alt::String str = optStr.GetValueOrDefault();
     * assert(str == "");
     */
    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOrDefault() & noexcept { return GetValueOrExec([]() { return U{}; }); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOrDefault() const & noexcept { return GetValueOrExec([]() { return U{}; }); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOrDefault() && noexcept { return std::move(*this).GetValueOrExec([]() { return U{}; }); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOrDefault() const && noexcept { return std::move(*this).GetValueOrExec([]() { return U{}; }); }

    /**
     * @brief Returns the contained value or the result of executing a callable.
     * @tparam FuncT The type of the callable.
     * @param func A callable that returns a value convertible to T.
     * @return The contained value if present, otherwise the result of calling func().
     * @example
     * 
     * Alt::Optional<usize> opt = Alt::Nullopt;
     * usize val = opt.GetValueOrExec([]() { return 4; });
     * assert(val == 4);
     * 
     * Alt::Optional<Alt::String> optStr = Alt::Nullopt;
     * Alt::String str = optStr.GetValueOrExec([]() { return "Empty"; });
     * assert(str == "Empty");
     */
    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOrExec(FuncT&& func) & noexcept { return m_Storage.IsVal() ? m_Storage.GetVal() : static_cast<T>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOrExec(FuncT&& func) const & noexcept { return m_Storage.IsVal() ? m_Storage.GetVal() : static_cast<T>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOrExec(FuncT&& func) && noexcept { return m_Storage.IsVal() ? std::move(m_Storage).GetVal() : static_cast<T>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetValueOrExec(FuncT&& func) const && noexcept { return m_Storage.IsVal() ? std::move(m_Storage).GetVal() : static_cast<T>((func)()); }

    /**
     * @brief Maps the contained value using a provided function.
     * @details If the Optional is 'nullopt' - returns a Nullopt. If the function returns a value, 
     * it wraps it in a new Optional. If the function returns an Optional, it flattens the result.
     * @tparam FuncT The callable type.
     * @param func The map function.
     * @return A new Optional containing the new value.
     * @example
     * 
     * Alt::Optional<usize> opt = 10;
     * Alt::Optional<usize> result = opt
     *		.Map([](const usize x) { return x * 2; })
     *		.Map([](const usize x) { return x + 4; })
     *		.Map([](const usize x) { return x * 2; })
     * ;
     * 
     * assert(*result == 48);
     *
     * Alt::Optional<Alt::String> optStr = opt.Map([](const usize x) {
     *		return std::to_string(x);
     * });
     * 
     * Alt::Optional<int> optNested = opt.Map([](const usize x) {
     *		return Alt::MakeOptional(x * 2);
     * });
     */
    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_optional_v<std::invoke_result_t<FuncT, CleanT&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, CleanT&> Map(FuncT&& func) & noexcept {

        return IsOk()
            ? (func)(GetValue())
            : Nullopt;
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_optional_v<std::invoke_result_t<FuncT, CleanT&>>>* = nullptr>
    ALT_NODISCARD Optional<std::invoke_result_t<FuncT, CleanT&>> Map(FuncT&& func) & noexcept {

        return IsOk()
            ? MakeOptional((func)(GetValue()))
            : Nullopt;
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_optional_v<std::invoke_result_t<FuncT, const CleanT&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const CleanT&> Map(FuncT&& func) const & noexcept {

        return IsOk()
            ? (func)(GetValue())
            : Nullopt;
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_optional_v<std::invoke_result_t<FuncT, const CleanT&>>>* = nullptr>
    ALT_NODISCARD Optional<std::invoke_result_t<FuncT, const CleanT&>> Map(FuncT&& func) const & noexcept {

        return IsOk()
            ? MakeOptional((func)(GetValue()))
            : Nullopt;
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_optional_v<std::invoke_result_t<FuncT, CleanT&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, CleanT&&> Map(FuncT&& func) && noexcept {

        return IsOk()
            ? (func)(std::move(GetValue()))
            : Nullopt;
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_optional_v<std::invoke_result_t<FuncT, CleanT&&>>>* = nullptr>
    ALT_NODISCARD Optional<std::invoke_result_t<FuncT, CleanT&&>> Map(FuncT&& func) && noexcept {

        return IsOk()
            ? MakeOptional((func)(std::move(GetValue())))
            : Nullopt;
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_optional_v<std::invoke_result_t<FuncT, const CleanT&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const CleanT&&> Map(FuncT&& func) const && noexcept {

        return IsOk()
            ? (func)(std::move(GetValue()))
            : Nullopt;
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_optional_v<std::invoke_result_t<FuncT, const CleanT&&>>>* = nullptr>
    ALT_NODISCARD Optional<std::invoke_result_t<FuncT, const CleanT&&>> Map(FuncT&& func) const && noexcept {

        return IsOk()
            ? MakeOptional((func)(std::move(GetValue())))
            : Nullopt;
    }

private:

    template <typename>
    friend class Optional;

    OptionalStorage m_Storage;
};

#define __ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__(T) \
template <> class Optional<T> { \
public: \
\
    using Type = T; \
\
    ALT_FORCE_INLINE static Optional Null() noexcept { return Nullopt; } \
    ALT_FORCE_INLINE Optional() noexcept = default; \
    ALT_FORCE_INLINE Optional(Nullopt_t) noexcept {} \
};

__ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__(void)
__ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__(const void)
__ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__(volatile void)
__ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__(const volatile void)

__ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__(Nullopt_t)
__ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__(const Nullopt_t)
__ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__(volatile Nullopt_t)
__ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__(const volatile Nullopt_t)

#undef __ALT_STD_INTERNAL_IMPL_OPTIONAL_VOID__

} // namespace Alt

#endif // __ALT_STD_OPTIONAL__