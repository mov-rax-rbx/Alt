#pragma once

#ifndef __ALT_STD_RESULT__
#define __ALT_STD_RESULT__

#include "../TypeTraits.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Memory/Ref.h"
#include "../Memory/Ptr.h"
#include "../Memory/MaybeUninit.h"

#include "Optional.h"

namespace Alt {

/**
 * @brief A wrapper for an error type used within Result containers.
 * @tparam E The underlying error type to be wrapped.
 */
template <typename E>
struct ResultErr { TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<E> err; };

template <> struct ResultErr<void> {};
template <> struct ResultErr<const void> {};
template <> struct ResultErr<volatile void> {};
template <> struct ResultErr<const volatile void> {};

using Err_t = ResultErr<void>;
struct Ok_t {};

/**
 * @var Ok
 * @brief A global constant of type Ok_t representing the value.
 * @details Represents the 'Ok' state in all Result operations.
 * 
 * @example
 * 
 * Alt::Result<void, usize> result = Alt::Ok;
 */
constexpr static Ok_t Ok = Ok_t{} ;

/**
 * @var Err
 * @brief A global constant of type Err_t representing the error.
 * @details Represents the 'Error' state in all Result operations.
 * 
 * @example
 * 
 * Alt::Result<usize, void> result = Alt::Err;
 */
constexpr static Err_t Err = Err_t{};

template <typename T, typename E>
class Result;

/**
 * @brief Creates a successful Result containing a value constructed from the provided arguments.
 * @tparam T The success type. Must be constructible from ArgsT.
 * @tparam ArgsT The types of the arguments passed to the constructor.
 * @param args Arguments to forward to the constructor of T.
 * @return A Result<T, void> containing the constructed value.
 * @example
 * 
 * Alt::Result<usize, void> result = Alt::MakeResultOk<usize>(4);
 * assert(result.IsOk());
 * assert(*result == 4);
 */
template <typename T, typename ...ArgsT,
    std::enable_if_t<std::is_constructible_v<T, ArgsT&&...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Result<T, void> MakeResultOk(ArgsT&&... args) noexcept {
    return Result<T, void>(std::forward<ArgsT>(args)...);
}

template <typename T,
    std::enable_if_t<std::is_constructible_v<T, T&&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Result<T, void> MakeResultOk(T&& val) noexcept {
    return Result<T, void>(std::forward<T>(val));
}

/**
 * @brief Creates a failed Result containing an error constructed from the provided arguments.
 * @tparam T The error type. Must be constructible from ArgsT.
 * @tparam ArgsT The types of the arguments passed to the constructor.
 * @param args Arguments to forward to the constructor of T.
 * @return A Result<void, T> containing the constructed error.
 * @example
 * 
 * Alt::Result<void, usize> result = Alt::MakeResultErr<usize>(4);
 * assert(res.IsErr());
 * assert(res.GetErr() == 4);
 */
template <typename T, typename ...ArgsT,
    std::enable_if_t<std::is_constructible_v<T, ArgsT&&...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Result<void, T> MakeResultErr(ArgsT&&... args) noexcept {
    return Result<void, T>(std::forward<ArgsT>(args)...);
}

template <typename T,
    std::enable_if_t<std::is_constructible_v<T, T&&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Result<void, T> MakeResultErr(T&& val) noexcept {
    return Result<void, T>(std::forward<T>(val));
}

namespace TypeTraits {

template <typename T, typename E>
struct __is_result__<Result<T, E>>
    : std::true_type
{};

template <typename T, typename E>
struct __is_trivially_reallocatable__<Result<T, E>>
    : std::conditional_t<

        is_trivially_reallocatable_v<TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<T>> &&
        is_trivially_reallocatable_v<TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<E>>,

        std::true_type,
        std::false_type
    >
{};

} // namespace TypeTraits

/** \cond */
namespace _ResultDetails {

template <typename T>
using clean_t = TypeTraits::clean_t<T>;

enum class ResultLayoutTrait { Value, Ptr };
enum class TypeEntityTrait { Value, Pointer, AltSmartPtr, Reference, AltRef };

template <ResultLayoutTrait LAYOUT_TRAIT>
using ResultLayout = std::integral_constant<ResultLayoutTrait, LAYOUT_TRAIT>;

template <TypeEntityTrait TYPE_ENTITY>
using TypeEntity = std::integral_constant<TypeEntityTrait, TYPE_ENTITY>;

template <typename T, typename E>
struct result_layout_trait
    : std::conditional_t<

        (TypeTraits::is_unit_v<T> || TypeTraits::is_unit_v<E>) &&
        (std::is_pointer_v<T> || TypeTraits::is_smart_pointer_v<T> || std::is_reference_v<T> || TypeTraits::is_alt_ref_v<T> ||
            std::is_pointer_v<E> || TypeTraits::is_smart_pointer_v<E> || std::is_reference_v<E> || TypeTraits::is_alt_ref_v<E>),

        ResultLayout<ResultLayoutTrait::Ptr>,
        ResultLayout<ResultLayoutTrait::Value>
    >
{};

template <typename T, typename E>
constexpr static ResultLayoutTrait result_layout_trait_v = result_layout_trait<T, E>::value;

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

template <typename T, typename E, ResultLayoutTrait = result_layout_trait_v<T, E>>
struct ResultStorageCore;

template <typename T, typename E>
struct ResultStorageCore<T, E, ResultLayoutTrait::Value> {

    MaybeUninitOneOf<T, E> m_Val;
    bool m_IsOk = false;
};

template <typename T>
struct ResultStorageCore<T, Unit_t, ResultLayoutTrait::Ptr> {

    // yes, we want this
    MaybeUninit<unwrap_to_clean_ptr_t<T>> m_OkPtrVal;
};

template <typename E>
struct ResultStorageCore<Unit_t, E, ResultLayoutTrait::Ptr> {

    // yes, we want this
    MaybeUninit<unwrap_to_clean_ptr_t<E>> m_ErrPtrVal;
};

struct Rule5Copy_t {};
struct Rule5Move_t {};
struct InnerOk_t {};
struct InnerErr_t {};

constexpr static Rule5Copy_t Rule5Copy = Rule5Copy_t{};
constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};
constexpr static InnerOk_t InnerOk = InnerOk_t{};
constexpr static InnerErr_t InnerErr = InnerErr_t{};

template <
    typename SelfT,
    typename T,
    typename E,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_destructible_v<T, E>
>
struct ResultStorageDestruct;

template <typename SelfT, typename T, typename E>
struct ResultStorageDestruct<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ResultStorageCore<T, E>
{
    ResultStorageDestruct() noexcept = default;

    ResultStorageDestruct(const ResultStorageDestruct&) noexcept = default;
    ResultStorageDestruct& operator=(const ResultStorageDestruct&) noexcept = default;

    ResultStorageDestruct(ResultStorageDestruct&&) noexcept = default;
    ResultStorageDestruct& operator=(ResultStorageDestruct&&) noexcept = default;

    ~ResultStorageDestruct() noexcept = delete;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageDestruct<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ResultStorageCore<T, E>
{
    ResultStorageDestruct() noexcept = default;

    ResultStorageDestruct(const ResultStorageDestruct&) noexcept = default;
    ResultStorageDestruct& operator=(const ResultStorageDestruct&) noexcept = default;

    ResultStorageDestruct(ResultStorageDestruct&&) noexcept = default;
    ResultStorageDestruct& operator=(ResultStorageDestruct&&) noexcept = default;

    ~ResultStorageDestruct() noexcept = default;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageDestruct<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ResultStorageCore<T, E>
{
    ResultStorageDestruct() noexcept = default;

    ResultStorageDestruct(const ResultStorageDestruct&) noexcept = default;
    ResultStorageDestruct& operator=(const ResultStorageDestruct&) noexcept = default;

    ResultStorageDestruct(ResultStorageDestruct&&) noexcept = default;
    ResultStorageDestruct& operator=(ResultStorageDestruct&&) noexcept = default;

    ~ResultStorageDestruct() noexcept {

        static_cast<SelfT*>(this)->Destroy();
    }
};

template <
    typename SelfT,
    typename T,
    typename E,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_assignable_v<T, E>
>
struct ResultStorageMoveAssign;

template <typename SelfT, typename T, typename E>
struct ResultStorageMoveAssign<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ResultStorageDestruct<SelfT, T, E>
{
    ResultStorageMoveAssign() noexcept = default;

    ResultStorageMoveAssign(const ResultStorageMoveAssign&) noexcept = default;
    ResultStorageMoveAssign& operator=(const ResultStorageMoveAssign&) noexcept = default;

    ResultStorageMoveAssign(ResultStorageMoveAssign&&) noexcept = default;
    ResultStorageMoveAssign& operator=(ResultStorageMoveAssign&&) noexcept = delete;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageMoveAssign<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ResultStorageDestruct<SelfT, T, E>
{
    ResultStorageMoveAssign() noexcept = default;

    ResultStorageMoveAssign(const ResultStorageMoveAssign&) noexcept = default;
    ResultStorageMoveAssign& operator=(const ResultStorageMoveAssign&) noexcept = default;

    ResultStorageMoveAssign(ResultStorageMoveAssign&&) noexcept = default;
    ResultStorageMoveAssign& operator=(ResultStorageMoveAssign&&) noexcept = default;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageMoveAssign<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ResultStorageDestruct<SelfT, T, E>
{
    ResultStorageMoveAssign() noexcept = default;

    ResultStorageMoveAssign(const ResultStorageMoveAssign&) noexcept = default;
    ResultStorageMoveAssign& operator=(const ResultStorageMoveAssign&) noexcept = default;

    ResultStorageMoveAssign(ResultStorageMoveAssign&&) noexcept = default;

    ResultStorageMoveAssign& operator=(ResultStorageMoveAssign&& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Move, static_cast<SelfT&>(rhs));
        return *this;
    }
};

template <
    typename SelfT,
    typename T,
    typename E,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_constructible_v<T, E>
>
struct ResultStorageMoveConstruct;

template <typename SelfT, typename T, typename E>
struct ResultStorageMoveConstruct<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ResultStorageMoveAssign<SelfT, T, E>
{
    ResultStorageMoveConstruct() noexcept = default;

    ResultStorageMoveConstruct(const ResultStorageMoveConstruct&) noexcept = default;
    ResultStorageMoveConstruct& operator=(const ResultStorageMoveConstruct&) noexcept = default;

    ResultStorageMoveConstruct(ResultStorageMoveConstruct&&) noexcept = delete;
    ResultStorageMoveConstruct& operator=(ResultStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageMoveConstruct<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ResultStorageMoveAssign<SelfT, T, E>
{
    ResultStorageMoveConstruct() noexcept = default;

    ResultStorageMoveConstruct(const ResultStorageMoveConstruct&) noexcept = default;
    ResultStorageMoveConstruct& operator=(const ResultStorageMoveConstruct&) noexcept = default;

    ResultStorageMoveConstruct(ResultStorageMoveConstruct&&) noexcept = default;
    ResultStorageMoveConstruct& operator=(ResultStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageMoveConstruct<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ResultStorageMoveAssign<SelfT, T, E>
{
    ResultStorageMoveConstruct() noexcept = default;

    ResultStorageMoveConstruct(const ResultStorageMoveConstruct&) noexcept = default;
    ResultStorageMoveConstruct& operator=(const ResultStorageMoveConstruct&) noexcept = default;

    ResultStorageMoveConstruct(ResultStorageMoveConstruct&& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Move, static_cast<SelfT&>(rhs));
    }

    ResultStorageMoveConstruct& operator=(ResultStorageMoveConstruct&&) noexcept = default;
};

template <
    typename SelfT,
    typename T,
    typename E,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_assignable_v<T, E>
>
struct ResultStorageCopyAssign;

template <typename SelfT, typename T, typename E>
struct ResultStorageCopyAssign<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ResultStorageMoveConstruct<SelfT, T, E>
{
    ResultStorageCopyAssign() noexcept = default;

    ResultStorageCopyAssign(const ResultStorageCopyAssign&) noexcept = default;
    ResultStorageCopyAssign& operator=(const ResultStorageCopyAssign&) noexcept = delete;

    ResultStorageCopyAssign(ResultStorageCopyAssign&&) noexcept = default;
    ResultStorageCopyAssign& operator=(ResultStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageCopyAssign<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ResultStorageMoveConstruct<SelfT, T, E>
{
    ResultStorageCopyAssign() noexcept = default;

    ResultStorageCopyAssign(const ResultStorageCopyAssign&) noexcept = default;
    ResultStorageCopyAssign& operator=(const ResultStorageCopyAssign&) noexcept = default;

    ResultStorageCopyAssign(ResultStorageCopyAssign&&) noexcept = default;
    ResultStorageCopyAssign& operator=(ResultStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageCopyAssign<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ResultStorageMoveConstruct<SelfT, T, E>
{
    ResultStorageCopyAssign() noexcept = default;

    ResultStorageCopyAssign(const ResultStorageCopyAssign&) noexcept = default;

    ResultStorageCopyAssign& operator=(const ResultStorageCopyAssign& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Copy, static_cast<const SelfT&>(rhs));
        return *this;
    }

    ResultStorageCopyAssign(ResultStorageCopyAssign&&) noexcept = default;
    ResultStorageCopyAssign& operator=(ResultStorageCopyAssign&&) noexcept = default;
};

template <
    typename SelfT,
    typename T,
    typename E,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_constructible_v<T, E>
>
struct ResultStorageCopyConstruct;

template <typename SelfT, typename T, typename E>
struct ResultStorageCopyConstruct<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ResultStorageCopyAssign<SelfT, T, E>
{
    ResultStorageCopyConstruct() noexcept = default;

    ResultStorageCopyConstruct(const ResultStorageCopyConstruct&) noexcept = delete;
    ResultStorageCopyConstruct& operator=(const ResultStorageCopyConstruct&) noexcept = default;

    ResultStorageCopyConstruct(ResultStorageCopyConstruct&&) noexcept = default;
    ResultStorageCopyConstruct& operator=(ResultStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageCopyConstruct<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ResultStorageCopyAssign<SelfT, T, E>
{
    ResultStorageCopyConstruct() noexcept = default;

    ResultStorageCopyConstruct(const ResultStorageCopyConstruct&) noexcept = default;
    ResultStorageCopyConstruct& operator=(const ResultStorageCopyConstruct&) noexcept = default;

    ResultStorageCopyConstruct(ResultStorageCopyConstruct&&) noexcept = default;
    ResultStorageCopyConstruct& operator=(ResultStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, typename E>
struct ResultStorageCopyConstruct<SelfT, T, E, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ResultStorageCopyAssign<SelfT, T, E>
{
    ResultStorageCopyConstruct() noexcept = default;

    ResultStorageCopyConstruct(const ResultStorageCopyConstruct& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Copy, static_cast<const SelfT&>(rhs));
    }

    ResultStorageCopyConstruct& operator=(const ResultStorageCopyConstruct&) noexcept = default;

    ResultStorageCopyConstruct(ResultStorageCopyConstruct&&) noexcept = default;
    ResultStorageCopyConstruct& operator=(ResultStorageCopyConstruct&&) noexcept = default;
};

template <typename T, typename E, ResultLayoutTrait = result_layout_trait_v<T, E>>
struct ResultStorage;

template <typename T, typename E>
struct ResultStorage<T, E, ResultLayoutTrait::Value>
    : ResultStorageCopyConstruct<ResultStorage<T, E>, T, E>
{
    using DerefOkT = unwrap_to_type_deref_t<T>;

    template <typename U>
    constexpr static bool is_constructible_inner_ok_v = std::is_constructible_v<T, U>;

    template <typename U>
    constexpr static bool is_assignable_inner_ok_v = std::is_assignable_v<T&, U> && std::is_constructible_v<T, U>;

    template <typename U>
    constexpr static bool is_constructible_inner_err_v = std::is_constructible_v<E, U>;

    template <typename U>
    constexpr static bool is_assignable_inner_err_v = std::is_assignable_v<E&, U> && std::is_constructible_v<E, U>;

    ResultStorage() noexcept = default;

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_unit_v<U>>* = nullptr>
    ResultStorage(Ok_t) noexcept {

        this->m_IsOk = true;
    }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_unit_v<U>>* = nullptr>
    void Assign(Ok_t) noexcept {

        if (!this->m_IsOk) {

            this->m_Val.template Destroy<E>();
            this->m_IsOk = true;
        }
    }

    template <typename U = E,
        std::enable_if_t<TypeTraits::is_unit_v<U>>* = nullptr>
    ResultStorage(Err_t) noexcept
    {}

    template <typename U = E,
        std::enable_if_t<TypeTraits::is_unit_v<U>>* = nullptr>
    void Assign(Err_t) noexcept {

        if (this->m_IsOk) {

            this->m_Val.template Destroy<T>();
            this->m_IsOk = false;
        }
    }

    template <typename U = T>
    void Construct(InnerOk_t, U&& val) noexcept {

        this->m_Val.template Construct<T>(std::forward<U>(val));
        this->m_IsOk = true;
    }

    template <typename U = T>
    void Assign(InnerOk_t, U&& rhs) noexcept {

        if (this->m_IsOk) {

            this->m_Val.template Assign<T>(std::forward<U>(rhs));
        }
        else {

            this->m_Val.template Destroy<E>();
            this->m_Val.template Construct<T>(std::forward<U>(rhs));

            this->m_IsOk = true;
        }
    }

    template <typename U = T>
    bool ConstructCloneOrMove(InnerOk_t, U&& val) noexcept {

        const bool isOk = this->m_Val.template ConstructCloneOrMove<T>(std::forward<U>(val));
        this->m_IsOk = isOk;

        return isOk;
    }

    template <typename U = E>
    void Construct(InnerErr_t, U&& val) noexcept {

        this->m_Val.template Construct<E>(std::forward<U>(val));
        this->m_IsOk = false;
    }

    template <typename U = E>
    void Assign(InnerErr_t, U&& rhs) noexcept {

        if (this->m_IsOk) {

            this->m_Val.template Destroy<T>();
            this->m_Val.template Construct<E>(std::forward<U>(rhs));

            this->m_IsOk = false;
        }
        else {

            this->m_Val.template Assign<E>(std::forward<U>(rhs));
        }
    }

    template <typename U = T>
    bool ConstructCloneOrMove(InnerErr_t, U&& val) noexcept {

        const bool isOk = this->m_Val.template ConstructCloneOrMove<E>(std::forward<U>(val));
        this->m_IsOk = false;

        return isOk;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    void Construct(Rule5Move_t, ResultStorage<T1, E1>& storage) noexcept {

        const bool isOk = storage.IsOk();
        if (isOk) {

            this->m_Val.template Construct<T>(std::move(storage.GetOk()));
        }
        else {

            this->m_Val.template Construct<E>(std::move(storage.GetErr()));
        }

        this->m_IsOk = isOk;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    void Construct(Rule5Move_t, ResultStorage<T1, E1>& storage) noexcept {

        const bool isOk = storage.IsOk();
        if (!isOk) {

            this->m_Val.template Construct<E>(std::move(storage.GetErr()));
        }

        this->m_IsOk = isOk;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && TypeTraits::is_unit_v<E1>>* = nullptr>
    void Construct(Rule5Move_t, ResultStorage<T1, E1>& storage) noexcept {

        const bool isOk = storage.IsOk();
        if (isOk) {

            this->m_Val.template Construct<T>(std::move(storage.GetOk()));
        }

        this->m_IsOk = isOk;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    void Construct(Rule5Copy_t, const ResultStorage<T1, E1>& storage) noexcept {

        const bool isOk = storage.IsOk();
        if (isOk) {

            this->m_Val.template Construct<T>(storage.GetOk());
        }
        else {

            this->m_Val.template Construct<E>(storage.GetErr());
        }

        this->m_IsOk = isOk;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    void Construct(Rule5Copy_t, const ResultStorage<T1, E1>& storage) noexcept {

        const bool isOk = storage.IsOk();
        if (!isOk) {

            this->m_Val.template Construct<E>(storage.GetErr());
        }

        this->m_IsOk = isOk;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && TypeTraits::is_unit_v<E1>>* = nullptr>
    void Construct(Rule5Copy_t, const ResultStorage<T1, E1>& storage) noexcept {

        const bool isOk = storage.IsOk();
        if (isOk) {

            this->m_Val.template Construct<T>(storage.GetOk());
        }

        this->m_IsOk = isOk;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    void Assign(Rule5Move_t, ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(InnerOk, std::move(storage.GetOk()));
        }
        else {

            Assign(InnerErr, std::move(storage.GetErr()));
        }
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    void Assign(Rule5Move_t, ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(Ok);
        }
        else {

            Assign(InnerErr, std::move(storage.GetErr()));
        }
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && TypeTraits::is_unit_v<E1>>* = nullptr>
    void Assign(Rule5Move_t, ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(InnerOk, std::move(storage.GetOk()));
        }
        else {

            Assign(Err);
        }
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    void Assign(Rule5Copy_t, const ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(InnerOk, storage.GetOk());
        }
        else {

            Assign(InnerErr, storage.GetErr());
        }
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    void Assign(Rule5Copy_t, const ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(Ok);
        }
        else {

            Assign(InnerErr, storage.GetErr());
        }
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && TypeTraits::is_unit_v<E1>>* = nullptr>
    void Assign(Rule5Copy_t, const ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(InnerOk, storage.GetOk());
        }
        else {

            Assign(Err);
        }
    }

    void Destroy() noexcept {

        if (this->m_IsOk) {

            this->m_Val.template Destroy<T>();
        }
        else {

            this->m_Val.template Destroy<E>();
        }
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    bool ConstructClone(const ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            return ConstructCloneOrMove(InnerOk, storage.GetOk());
        }
        else {

            return ConstructCloneOrMove(InnerErr, storage.GetErr());
        }
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    bool ConstructClone(const ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(Ok);
            return true;
        }
        else {

            return ConstructCloneOrMove(InnerErr, storage.GetErr());
        }
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && TypeTraits::is_unit_v<E1>>* = nullptr>
    bool ConstructClone(const ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            return ConstructCloneOrMove(InnerOk, storage.GetOk());
        }
        else {

            Assign(Err);
            return true;
        }
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    bool AssignClone(const ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            MaybeUninit<T> tmp;

            const bool isOk = tmp.ConstructCloneOrMove(storage.GetOk());
            if (!isOk) return false;

            Assign(InnerOk, std::move(tmp.GetVal()));

            tmp.Destroy();
        }
        else {

            MaybeUninit<E> tmp;

            const bool isOk = tmp.ConstructCloneOrMove(storage.GetErr());
            if (!isOk) return false;

            Assign(InnerErr, std::move(tmp.GetVal()));

            tmp.Destroy();
        }

        return true;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<TypeTraits::is_unit_v<T1> && !TypeTraits::is_unit_v<E1>>* = nullptr>
    bool AssignClone(const ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(Ok);
        }
        else {

            MaybeUninit<E> tmp;

            const bool isOk = tmp.ConstructCloneOrMove(storage.GetErr());
            if (!isOk) return false;

            Assign(InnerErr, std::move(tmp.GetVal()));

            tmp.Destroy();
        }

        return true;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<!TypeTraits::is_unit_v<T1> && TypeTraits::is_unit_v<E1>>* = nullptr>
    bool AssignClone(const ResultStorage<T1, E1>& storage) noexcept {

        if (storage.IsOk()) {

            MaybeUninit<T> tmp;

            const bool isOk = tmp.ConstructCloneOrMove(storage.GetOk());
            if (!isOk) return false;

            Assign(InnerOk, std::move(tmp.GetVal()));

            tmp.Destroy();
        }
        else {

            Assign(Err);
        }

        return true;
    }

    T& GetOk() & noexcept { return this->m_Val.template GetVal<T>(); }
    const T& GetOk() const & noexcept { return this->m_Val.template GetVal<T>(); }
    T&& GetOk() && noexcept { return std::move(this->m_Val.template GetVal<T>()); }
    const T&& GetOk() const && noexcept { return std::move(this->m_Val.template GetVal<T>()); }

    E& GetErr() & noexcept { return this->m_Val.template GetVal<E>(); }
    const E& GetErr() const & noexcept { return this->m_Val.template GetVal<E>(); }
    E&& GetErr() && noexcept { return std::move(this->m_Val.template GetVal<E>()); }
    const E&& GetErr() const && noexcept { return std::move(this->m_Val.template GetVal<E>()); }

    bool IsOk() const noexcept { return this->m_IsOk; }

    template <typename U = T,
        std::enable_if_t<is_layout_not_own_value_v<U>>* = nullptr>
    DerefOkT* Deref() noexcept { return &*this->m_Val.template GetVal<T>(); }

    template <typename U = T,
        std::enable_if_t<is_layout_own_value_v<U>>* = nullptr>
    DerefOkT* Deref() noexcept { return this->m_Val.template GetPtr<T>(); }

    template <typename U = T,
        std::enable_if_t<is_layout_not_own_value_v<U>>* = nullptr>
    const DerefOkT* Deref() const noexcept { return &*this->m_Val.template GetVal<T>(); }

    template <typename U = T,
        std::enable_if_t<is_layout_own_value_v<U>>* = nullptr>
    const DerefOkT* Deref() const noexcept { return this->m_Val.template GetPtr<T>(); }
};

template <typename T>
struct ResultStorage<T, Unit_t, ResultLayoutTrait::Ptr>
    : ResultStorageCopyConstruct<ResultStorage<T, Unit_t>, T, Unit_t>
{
    using DerefOkT = unwrap_to_type_deref_t<T>;

    template <typename U>
    constexpr static bool is_constructible_inner_ok_v = std::is_constructible_v<T, U>;

    template <typename U>
    constexpr static bool is_assignable_inner_ok_v = std::is_assignable_v<T&, U>;

    ResultStorage() noexcept = default;

    ResultStorage(Err_t) noexcept {

        this->m_OkPtrVal.Construct(nullptr);
    }

    void Assign(Err_t) noexcept {

        this->m_OkPtrVal.Assign(nullptr);
    }

    template <typename U = T>
    void Construct(InnerOk_t, U&& val) noexcept {

        this->m_OkPtrVal.Construct(std::forward<U>(val));
    }

    template <typename U = T>
    void Assign(InnerOk_t, U&& rhs) noexcept {

        this->m_OkPtrVal.Assign(std::forward<U>(rhs));
    }

    template <typename U = T>
    bool ConstructCloneOrMove(InnerOk_t, U&& val) noexcept {

        return this->m_OkPtrVal.ConstructCloneOrMove(std::forward<U>(val));
    }

    template <typename U = T>
    bool AssignCloneOrMove(InnerOk_t, U&& rhs) noexcept {

        return this->m_OkPtrVal.AssignCloneOrMove(std::forward<U>(rhs));
    }

    template <typename T1 = T>
    void Construct(Rule5Move_t, ResultStorage<T1, Unit_t>& storage) noexcept {

        if (storage.IsOk()) {

            this->m_OkPtrVal.Construct(std::move(storage.GetOk()));
        }
        else {

            this->m_OkPtrVal.Construct(nullptr);
        }
    }

    template <typename T1 = T>
    void Construct(Rule5Copy_t, const ResultStorage<T1, Unit_t>& storage) noexcept {

        if (storage.IsOk()) {

            this->m_OkPtrVal.Construct(storage.GetOk());
        }
        else {

            this->m_OkPtrVal.Construct(nullptr);
        }
    }

    template <typename T1 = T>
    void Assign(Rule5Move_t, ResultStorage<T1, Unit_t>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(InnerOk, std::move(storage.GetOk()));
        }
        else {

            Assign(Err);
        }
    }

    template <typename T1 = T>
    void Assign(Rule5Copy_t, const ResultStorage<T1, Unit_t>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(InnerOk, storage.GetOk());
        }
        else {

            Assign(Err);
        }
    }

    void Destroy() noexcept {

        this->m_OkPtrVal.Destroy();
    }

    template <typename T1 = T>
    bool ConstructClone(const ResultStorage<T1, Unit_t>& storage) noexcept {

        if (storage.IsOk()) {

            return ConstructCloneOrMove(InnerOk, storage.GetOk());
        }
        else {

            this->m_OkPtrVal.Construct(nullptr);
        }

        return true;
    }

    template <typename T1 = T>
    bool AssignClone(const ResultStorage<T1, Unit_t>& storage) noexcept {

        if (storage.IsOk()) {

            return AssignCloneOrMove(InnerOk, storage.GetOk());
        }
        else {

            Assign(Err);
        }

        return true;
    }

    T& GetOk() & noexcept { return this->m_OkPtrVal.GetVal(); }
    const T& GetOk() const & noexcept { return this->m_OkPtrVal.GetVal(); }
    T&& GetOk() && noexcept { return std::move(this->m_OkPtrVal.GetVal()); }
    const T&& GetOk() const && noexcept { return std::move(this->m_OkPtrVal.GetVal()); }

    bool IsOk() const noexcept { return this->m_OkPtrVal.GetVal() != nullptr; }

    template <typename U = T,
        std::enable_if_t<is_layout_own_value_v<U>>* = nullptr>
    DerefOkT* Deref() noexcept { return this->m_OkPtrVal.GetPtr(); }

    template <typename U = T,
        std::enable_if_t<is_layout_not_own_value_v<U>>* = nullptr>
    DerefOkT* Deref() noexcept { return &*this->m_OkPtrVal.GetVal(); }

    template <typename U = T,
        std::enable_if_t<is_layout_own_value_v<U>>* = nullptr>
    const DerefOkT* Deref() const noexcept { return this->m_OkPtrVal.GetPtr(); }

    template <typename U = T,
        std::enable_if_t<is_layout_not_own_value_v<U>>* = nullptr>
    const DerefOkT* Deref() const noexcept { return &*this->m_OkPtrVal.GetVal(); }
};

template <typename E>
struct ResultStorage<Unit_t, E, ResultLayoutTrait::Ptr>
    : ResultStorageCopyConstruct<ResultStorage<Unit_t, E>, Unit_t, E>
{
    using DerefOkT = Unit_t;

    template <typename U>
    constexpr static bool is_constructible_inner_err_v = std::is_constructible_v<E, U>;

    template <typename U>
    constexpr static bool is_assignable_inner_err_v = std::is_assignable_v<E&, U>;

    ResultStorage() noexcept = default;

    ResultStorage(Ok_t) noexcept {

        this->m_ErrPtrVal.Construct(nullptr);
    }

    void Assign(Ok_t) noexcept {

        this->m_ErrPtrVal.Assign(nullptr);
    }

    template <typename U = E>
    void Construct(InnerErr_t, U&& val) noexcept {

        this->m_ErrPtrVal.Construct(std::forward<U>(val));
    }

    template <typename U = E>
    void Assign(InnerErr_t, U&& rhs) noexcept {

        this->m_ErrPtrVal.Assign(std::forward<U>(rhs));
    }

    template <typename U = E>
    bool ConstructCloneOrMove(InnerErr_t, U&& val) noexcept {

        return this->m_ErrPtrVal.ConstructCloneOrMove(std::forward<U>(val));
    }
    
    template <typename U = E>
    bool AssignCloneOrMove(InnerErr_t, U&& rhs) noexcept {

        return this->m_ErrPtrVal.AssignCloneOrMove(std::forward<U>(rhs));
    }

    template <typename E1 = E>
    void Construct(Rule5Move_t, ResultStorage<Unit_t, E1>& storage) noexcept {

        if (storage.IsOk()) {

            this->m_ErrPtrVal.Construct(nullptr);
        }
        else {

            this->m_ErrPtrVal.Construct(std::move(storage.GetErr()));
        }
    }

    template <typename E1 = E>
    void Construct(Rule5Copy_t, const ResultStorage<Unit_t, E1>& storage) noexcept {

        if (storage.IsOk()) {

            this->m_ErrPtrVal.Construct(nullptr);
        }
        else {

            this->m_ErrPtrVal.Construct(storage.GetErr());
        }
    }

    template <typename E1 = E>
    void Assign(Rule5Move_t, ResultStorage<Unit_t, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(Ok);
        }
        else {

            Assign(InnerErr, std::move(storage.GetErr()));
        }
    }

    template <typename E1 = E>
    void Assign(Rule5Copy_t, const ResultStorage<Unit_t, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(Ok);
        }
        else {

            Assign(InnerErr, storage.GetErr());
        }
    }

    void Destroy() noexcept {

        this->m_ErrPtrVal.Destroy();
    }

    template <typename E1 = E>
    bool ConstructClone(const ResultStorage<Unit_t, E1>& storage) noexcept {

        if (storage.IsOk()) {

            this->m_ErrPtrVal.Construct(nullptr);
        }
        else {

            return ConstructCloneOrMove(InnerErr, storage.GetErr());
        }

        return true;
    }

    template <typename E1 = E>
    bool AssignClone(const ResultStorage<Unit_t, E1>& storage) noexcept {

        if (storage.IsOk()) {

            Assign(Ok);
        }
        else {

            return AssignCloneOrMove(InnerErr, storage.GetErr());
        }

        return true;
    }

    E& GetErr() & noexcept { return this->m_ErrPtrVal.GetVal(); }
    const E& GetErr() const & noexcept { return this->m_ErrPtrVal.GetVal(); }
    E&& GetErr() && noexcept { return std::move(this->m_ErrPtrVal.GetVal()); }
    const E&& GetErr() const && noexcept { return std::move(this->m_ErrPtrVal.GetVal()); }

    bool IsOk() const noexcept { return this->m_ErrPtrVal.GetVal() == nullptr; }
};

} // namespace _ResultDetails
/** \endcond */

/**
 * @brief A container representing either a successful value of type T or an error of type E.
 * @tparam T The type of the value contained in the successful state.
 * @tparam E The type of the error contained in the Err state.
 * @example
 * 
 * Alt::Result<usize, Alt::String> result = 4;
 * if (result) {
 *     usize value = *result;
 * }
 * 
 * Alt::String str = "Result"_salloc;
 * Alt::Result<Alt::String&, void> result = str;
 * 
 * assert(*result == "Result");
 * if (Alt::String* pStr = result) {
 *     assert(*pStr == "Result");
 * }
 */
template <typename T_, typename E_>
class Result {
private:

    using CleanT = TypeTraits::clean_t<T_>;
    using CleanE = TypeTraits::clean_t<E_>;

    using T = TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<CleanT>;
    using E = TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<CleanE>;

    using ResultStorage = _ResultDetails::ResultStorage<T, E>;

    using DerefOkT = typename ResultStorage::DerefOkT;

    template <typename U>
    constexpr static bool is_constructible_inner_ok_v = ResultStorage::template is_constructible_inner_ok_v<U>;

    template <typename U>
    constexpr static bool is_assignable_inner_ok_v = ResultStorage::template is_assignable_inner_ok_v<U>;

    template <typename U>
    constexpr static bool is_constructible_inner_err_v = ResultStorage::template is_constructible_inner_err_v<U>;

    template <typename U>
    constexpr static bool is_assignable_inner_err_v = ResultStorage::template is_assignable_inner_err_v<U>;

public:

    using OkT = T;
    using ErrT = E;

    /**
     * @brief Creates a Result containing the provided Ok value.
     * @tparam U The type of the value being passed.
     * @param val The value to wrap in the Result.
     * @return A Result instance representing the Ok state.
     * @example
     * 
     * Alt::Result<usize, usize> result = Alt::Result<usize, usize>::Ok(4);
     * assert(res.IsOk());
     */
    template <typename U = T,
        std::enable_if_t<is_constructible_inner_ok_v<U&&>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Result Ok(U&& val) noexcept {
        return Result(std::forward<U>(val));
    }

    /**
     * @brief Creates a failed Result containing the Err value.
     * @tparam U The type of the error being passed.
     * @param err The error value to wrap in the Result.
     * @return A Result instance representing the Err state.
     * @example
     * 
     * Alt::Result<usize, usize> result = Alt::Result<usize, usize>::Err(4);
     * assert(res.IsErr());
     */
    template <typename U = E,
        std::enable_if_t<is_constructible_inner_err_v<U&&>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Result Err(U&& err) noexcept {
        return Result(ResultErr<E>{ std::forward<U>(err) });
    }

    /**
     * @brief Constructors and Assignment Operators.
     * @details Handles construction.
     * @note The default constructor is deleted if the error type E is not default-constructible.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result;
     * assert(res.IsErr());
     */
    template <typename U = E,
        std::enable_if_t<!std::is_default_constructible_v<U>>* = nullptr>
    Result() noexcept = delete;

    template <typename U = E,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    Result() noexcept {

        m_Storage.Construct(_ResultDetails::InnerErr, E{});
    }

    /**
     * @brief Constructs a Result with a Ok value.
     * @tparam U The type of the value.
     * @param val The Ok value to wrap.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result(4);
     * assert(result.IsOk());
     */
    template <typename U = T,
        std::enable_if_t<is_constructible_inner_ok_v<U&&>>* = nullptr>
    Result(U&& val) noexcept {

        m_Storage.Construct(_ResultDetails::InnerOk, std::forward<U>(val));
    }

    /**
     * @brief Assigns a Ok value to the Result.
     * @tparam U The type of the value.
     * @param rhs The value to assign.
     * @return A reference to this Result instance.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result;
     * 
     * result = 4;
     * assert(result.IsOk());
     */
    template <typename U = T,
        std::enable_if_t<is_assignable_inner_ok_v<U&&>>* = nullptr>
    Result& operator=(U&& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::InnerOk, std::forward<U>(rhs));
        return *this;
    }

    /**
     * @brief Constructs a Result with an Err value.
     * @tparam U The type of the error. Must be constructible into E and not constructible into T.
     * @param val The error value to wrap.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result("Result"_salloc);
     * assert(result.IsErr());
     */
    template <typename U = E,
        std::enable_if_t<is_constructible_inner_err_v<U&&> && !is_constructible_inner_ok_v<U&&>>* = nullptr>
    Result(U&& val) noexcept {

        m_Storage.Construct(_ResultDetails::InnerErr, std::forward<U>(val));
    }

    /**
     * @brief Assigns an Err value to the Result.
     * @tparam U The type of the error. Must be assignable to E and not assignable to T.
     * @param rhs The error value to assign.
     * @return A reference to this Result instance.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result;
     * 
     * result = "Result"_salloc;
     * assert(result.IsErr());
     */
    template <typename U = E,
        std::enable_if_t<is_assignable_inner_err_v<U&&> && !is_assignable_inner_ok_v<U&&>>* = nullptr>
    Result& operator=(U&& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::InnerErr, std::forward<U>(rhs));
        return *this;
    }

    /**
     * @brief Constructs a Result from a 'ResultErr' wrapper.
     * @tparam U The error type.
     * @param err A 'ResultErr' object containing the Err value.
     * @example
     * 
     * Alt::Result<usize, usize> result = Alt::ResultErr<usize>{ 4 };
     * assert(result.IsErr());
     */
    template <typename U = E,
        std::enable_if_t<is_constructible_inner_err_v<U&&>>* = nullptr>
    Result(ResultErr<U>&& err) noexcept {

        m_Storage.Construct(_ResultDetails::InnerErr, std::move(err.err));
    }

    template <typename U = E,
        std::enable_if_t<is_constructible_inner_err_v<const U&>>* = nullptr>
    Result(const ResultErr<U>& err) noexcept {

        m_Storage.Construct(_ResultDetails::InnerErr, err.err);
    }

    /**
     * @brief Assigns a 'ResultErr' to the Result.
     * @tparam U The error type. Must be assignable to E and not assignable to T.
     * @param rhs A 'ResultErr' object.
     * @return A reference to this Result instance.
     * @example
     * 
     * Alt::Result<usize, usize> result;
     * 
     * result = Alt::ResultErr<usize>{ 4 };
     * assert(result.IsErr());
     */
    template <typename U = E,
        std::enable_if_t<is_assignable_inner_err_v<U&&>>* = nullptr>
    Result& operator=(ResultErr<U>&& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::InnerErr, std::move(rhs.err));
        return *this;
    }

    template <typename U = E,
        std::enable_if_t<is_assignable_inner_err_v<const U&>>* = nullptr>
    Result& operator=(const ResultErr<U>& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::InnerErr, rhs.err);
        return *this;
    }

    /**
     * @brief Constructs an Result from another Result with narrow cast.
     * @tparam U The type of the source Result.
     * @param val The source Result to move from.
     * @example
     * 
     * Alt::Result<usize, f32> result0 = 4;
     * Alt::Result<ssize, f32> result1 = result0;
     */
    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<
            !std::is_constructible_v<T, Result<T1, E1>&&> &&
            !std::is_constructible_v<E, Result<T1, E1>&&> &&
            is_constructible_inner_ok_v<T1&&> &&
            is_constructible_inner_err_v<E1&&>
        >* = nullptr>
    Result(Result<T1, E1>&& val) noexcept {

        m_Storage.Construct(_ResultDetails::Rule5Move, val.m_Storage);
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<
            !std::is_constructible_v<T, const Result<T1, E1>&> &&
            !std::is_constructible_v<E, const Result<T1, E1>&> &&
            is_constructible_inner_ok_v<const T1&> &&
            is_constructible_inner_err_v<const E1&>
        >* = nullptr>
    Result(const Result<T1, E1>& val) noexcept {

        m_Storage.Construct(_ResultDetails::Rule5Copy, val.m_Storage);
    }

    /**
     * @brief Assigns an Result from another Result with narrow cast.
     * @tparam U The type of the source Result.
     * @param rhs The source Result.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result0 = 4;
     * Alt::Result<ssize, Alt::String> result1 = 8;
     * result1 = std::move(result0);
     */
    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<
            !std::is_assignable_v<T&, Result<T1, E1>&&> &&
            !std::is_assignable_v<E&, Result<T1, E1>&&> &&
            is_assignable_inner_ok_v<T1&&> &&
            is_assignable_inner_err_v<E1&&>
        >* = nullptr>
    Result& operator=(Result<T1, E1>&& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::Rule5Move, rhs.m_Storage);
        return *this;
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<
            !std::is_assignable_v<T&, const Result<T1, E1>&> &&
            !std::is_assignable_v<E&, const Result<T1, E1>&> &&
            is_assignable_inner_ok_v<const T1&> &&
            is_assignable_inner_err_v<const E1&>
        >* = nullptr>
    Result& operator=(const Result<T1, E1>& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::Rule5Copy, rhs.m_Storage);
        return *this;
    }

    /**
     * @brief Clones the current Result.
     * 
     * @tparam U The type of the source Result.
     * @param val The destination Result to clone.
     * @return A bool, false when clone failed.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Result<Alt::Vector<usize>, usize> res0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
     * Alt::Result<Alt::Vector<usize>, usize> res1;
     * 
     * res1 = std::move(res0);
     * res0.CloneFrom(res1) iferr_return;
     * 
     * assert(*res0, *res1);
     */
    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<
            (TypeTraits::is_only_clonable_to_from_v<T1, const T&> || is_assignable_inner_ok_v<const T1&>) &&
            (TypeTraits::is_only_clonable_to_from_v<E1, const E&> || is_assignable_inner_err_v<const E1&>)
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const Result<T1, E1>& rhs) noexcept {

        return m_Storage.AssignClone(rhs.m_Storage);
    }

    template <typename T1 = T, typename E1 = E,
        std::enable_if_t<
            (TypeTraits::is_only_clonable_to_from_v<T1, const T&> || is_constructible_inner_ok_v<const T1&>) &&
            (TypeTraits::is_only_clonable_to_from_v<E1, const E&> || is_constructible_inner_err_v<const E1&>)
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const Result<T1, E1>& rhs) noexcept {

        return m_Storage.ConstructClone(rhs.m_Storage);
    }

    ALT_NODISCARD Optional<Result> Clone() const noexcept {

        struct Object {

            ~Object() noexcept {

                if (!isValid) {
                    return;
                }

                Result* pObject = reinterpret_cast<Result*>(memory);
                Memory::DestroyObject(pObject);

                isValid = false;
            }

            alignas(Result) u8 memory[sizeof(Result)];
            bool isValid = false;
        };

        Object object{};
        Result* pObject = reinterpret_cast<Result*>(object.memory);

        const bool isOk = pObject->CloneConstructFrom(*this);
        object.isValid = isOk;

        if (!isOk) {
            return Nullopt;
        }

        return MakeOptional(std::move(*pObject));
    }

    /**
     * @brief Equality comparison against an Ok state.
     * @param obj An Ok_t marker.
     * @return True if the Result is Ok, false otherwise.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = 4;
     * 
     * assert(result == Alt::Ok);
     */
    ALT_FORCE_INLINE bool operator==(Ok_t) const noexcept { return IsOk(); }
    ALT_FORCE_INLINE bool operator!=(Ok_t) const noexcept { return !IsOk(); }

    /**
     * @brief Equality comparison against an Err state.
     * @param obj An Err_t marker.
     * @return True if the Result is Err, false otherwise.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = "Result"_salloc;
     * 
     * assert(result == Alt::Err);
     */
    ALT_FORCE_INLINE bool operator==(Err_t) const noexcept { return !IsOk(); }
    ALT_FORCE_INLINE bool operator!=(Err_t) const noexcept { return IsOk(); }

    /**
     * @brief Dereferences the Result to access the Ok value.
     * @tparam U The deduced type of the underlying value.
     * @return A reference to the Ok value.
     * @details Behavior is undefined if the Result contains an error.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = 4;
     * assert(*result == 4);
     */
    template <typename U = DerefOkT>
    ALT_FORCE_INLINE U& operator*() & noexcept { ALT_ASSERT(m_Storage.IsOk()); return *m_Storage.Deref(); }

    template <typename U = DerefOkT>
    ALT_FORCE_INLINE const U& operator*() const & noexcept { ALT_ASSERT(m_Storage.IsOk()); return *m_Storage.Deref(); }

    template <typename U = DerefOkT>
    ALT_FORCE_INLINE U&& operator*() && noexcept { ALT_ASSERT(m_Storage.IsOk()); return std::move(*m_Storage.Deref()); }

    template <typename U = DerefOkT>
    ALT_FORCE_INLINE const U&& operator*() const && noexcept { ALT_ASSERT(m_Storage.IsOk()); return std::move(*m_Storage.Deref()); }

    /**
     * @brief Accesses the Ok value via member access.
     * @tparam U The deduced type of the underlying value.
     * @return A pointer to the Ok value.
     * @details Behavior is undefined if the Result contains an error.
     * @example
     * 
     * Alt::Result<Alt::String, usize> result = "Result"_salloc;
     * assert(result->size() == 6);
     */
    ALT_FORCE_INLINE DerefOkT* operator->() noexcept { ALT_ASSERT(m_Storage.IsOk()); return m_Storage.Deref(); }
    ALT_FORCE_INLINE const DerefOkT* operator->() const noexcept { ALT_ASSERT(m_Storage.IsOk()); return m_Storage.Deref(); }

    /**
     * @brief Accesses the Ok value throught pointer.
     * @tparam U The deduced type of the underlying value.
     * @return A pointer to the Ok value.
     * @example
     * 
     * Alt::Result<Alt::String, usize> result = 4;
     * 
     * Alt::String* pStr = result;
     * assert(pStr == nullptr);
     */
    template <typename U = T,
        std::enable_if_t<_ResultDetails::is_layout_not_own_value_v<U>>* = nullptr>
    ALT_FORCE_INLINE operator DerefOkT*() noexcept { return IsOk() ? m_Storage.Deref() : nullptr; }

    template <typename U = T,
        std::enable_if_t<_ResultDetails::is_layout_not_own_value_v<U>>* = nullptr>
    ALT_FORCE_INLINE operator const DerefOkT*() const noexcept { return IsOk() ? m_Storage.Deref() : nullptr; }

    /**
     * @brief Checks if the Result is in the Ok state.
     * @return True if Ok, false if Err.
     * @example
     * 
     * Alt::Result<Alt::String, usize> result = "Result"_salloc;
     * assert(result.IsOk());
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsOk() const noexcept { return m_Storage.IsOk(); }

    /**
     * @brief Checks if the Result is in the Err state.
     * @return True if Err, false if Ok.
     * @example
     * 
     * Alt::Result<Alt::String, usize> result = 4;
     * assert(result.IsErr());
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsErr() const noexcept { return !m_Storage.IsOk(); }

    /**
     * @brief Explicit conversion to boolean.
     * @return True if the Result is Ok, false if Err.
     * @example
     * 
     * Alt::Result<Alt::String, usize> result = "Result"_salloc;
     * assert(result);
     */
    ALT_FORCE_INLINE explicit operator bool() const noexcept { return IsOk(); }

    /**
     * @brief Retrieves the Ok value.
     * @return A reference to the Ok value.
     * @details Behavior is undefined if the Result contains an error.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = 4;
     * assert(result.GetOk() == 4);
     */
    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT>&, CleanT&>
        GetOk() & noexcept { ALT_ASSERT(m_Storage.IsOk()); return m_Storage.GetOk(); }

    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT>&, const CleanT&>
        GetOk() const & noexcept { ALT_ASSERT(m_Storage.IsOk()); return m_Storage.GetOk(); }

    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT>&, CleanT&&>
        GetOk() && noexcept { ALT_ASSERT(m_Storage.IsOk()); return std::move(m_Storage.GetOk()); }

    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT>&, const CleanT&&>
        GetOk() const && noexcept { ALT_ASSERT(m_Storage.IsOk()); return std::move(m_Storage.GetOk()); }

    /**
     * @brief Retrieves the Ok value or a fallback.
     * @tparam U The type of the fallback value. Must be convertible to T.
     * @param v The fallback value.
     * @return The Ok value if Ok, otherwise the fallback value.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = "Result"_salloc;
     * assert(result.GetOkOr(4) == 4);
     */
    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOr(U&& v) & noexcept { return m_Storage.IsOk() ? m_Storage.GetOk() : static_cast<T>(std::forward<U>(v)); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOr(U&& v) const & noexcept { return m_Storage.IsOk() ? m_Storage.GetOk() : static_cast<T>(std::forward<U>(v)); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOr(U&& v) && noexcept { return m_Storage.IsOk() ? std::move(m_Storage).GetOk() : static_cast<T>(std::forward<U>(v)); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOr(U&& v) const && noexcept { return m_Storage.IsOk() ? std::move(m_Storage).GetOk() : static_cast<T>(std::forward<U>(v)); }

    /**
     * @brief Retrieves the Ok value or a default-constructed value.
     * @tparam U The type to default construct. Must be default constructible.
     * @return The Ok value if Ok, otherwise a default-constructed U.
     * @example
     * 
     * Alt::Result<Alt::String, usize> result = 4;
     * assert(result.GetOkOrDefault() == "");
     */
    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrDefault() & noexcept { return GetOkOrExec([]() { return U{}; }); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrDefault() const & noexcept { return GetOkOrExec([]() { return U{}; }); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrDefault() && noexcept { return std::move(*this).GetOkOrExec([]() { return U{}; }); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrDefault() const && noexcept { return std::move(*this).GetOkOrExec([]() { return U{}; }); }

    /**
     * @brief Retrieves the Ok value or executes a function to generate a fallback.
     * @tparam FuncT A callable type.
     * @param func A function that returns a value convertible to T.
     * @return The Ok value if Ok, otherwise the result of func().
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = "Result"_salloc;
     * assert(res.GetOkOrExec([](){ return 4; }) == 4);
     */
    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrExec(FuncT&& func) & noexcept { return m_Storage.IsOk() ? m_Storage.GetOk() : static_cast<T>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrExec(FuncT&& func) const & noexcept { return m_Storage.IsOk() ? m_Storage.GetOk() : static_cast<T>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrExec(FuncT&& func) && noexcept { return m_Storage.IsOk() ? std::move(m_Storage).GetOk() : static_cast<T>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrExec(FuncT&& func) const && noexcept { return m_Storage.IsOk() ? std::move(m_Storage).GetOk() : static_cast<T>((func)()); }

    /**
     * @brief Map the Ok value using the function.
     * @tparam FuncT A callable type. Must return a type that is a Result.
     * @param func A function that takes T and returns a Result.
     * @return A new Result containing the mapped value or the original Ok value.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result0 = 4;
     * Alt::Result<usize, Alt::String> result1 = result0
     *     .Map([](const usize x) { return Alt::Result<usize, Alt::String>::Ok(x * 2); })
     *     .Map([](const usize x) { return Alt::Result<usize, Alt::String>::Ok(x + 4); })
     *     .Map([](const usize x) { return Alt::Result<usize, Alt::String>::Ok(x * 2); })
     * ;
     * assert(*result1 == 24);
     * 
     * Alt::Result<usize, Alt::String> result0 = "Result"_salloc;
     * Alt::Result<usize, Alt::String> result1 = result0
     *     .Map([](const usize x) { return Alt::Result<usize, Alt::String>::Ok(x * 2); })
     *     .Map([](const usize x) { return Alt::Result<usize, Alt::String>::Ok(x + 4); })
     *     .Map([](const usize x) { return Alt::Result<usize, Alt::String>::Ok(x * 2); })
     * ;
     * assert(result1.GetErr() == "Result"_salloc);
     */
    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanT&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, CleanT&> Map(FuncT&& func) & noexcept {

        if (IsOk()) {

            return (func)(GetOk());
        }

        return GetErr();
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanT&>>>* = nullptr>
    ALT_NODISCARD Result<std::invoke_result_t<FuncT, CleanT&>, CleanE> Map(FuncT&& func) & noexcept {

        if (IsOk()) {

            return (func)(GetOk());
        }

        return GetErr();
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanT&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const CleanT&> Map(FuncT&& func) const & noexcept {

        if (IsOk()) {

            return (func)(GetOk());
        }

        return GetErr();
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanT&>>>* = nullptr>
    ALT_NODISCARD Result<std::invoke_result_t<FuncT, const CleanT&>, CleanE> Map(FuncT&& func) const & noexcept {

        if (IsOk()) {

            return (func)(GetOk());
        }

        return GetErr();
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanT&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, CleanT&&> Map(FuncT&& func) && noexcept {

        if (IsOk()) {

            return (func)(std::move(GetOk()));
        }

        return std::move(*this).GetErr();
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanT&&>>>* = nullptr>
    ALT_NODISCARD Result<std::invoke_result_t<FuncT, CleanT&&>, CleanE> Map(FuncT&& func) && noexcept {

        if (IsOk()) {

            return (func)(std::move(GetOk()));
        }

        return std::move(*this).GetErr();
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanT&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const CleanT&&> Map(FuncT&& func) const && noexcept {

        if (IsOk()) {

            return (func)(std::move(GetOk()));
        }

        return std::move(*this).GetErr();
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanT&&>>>* = nullptr>
    ALT_NODISCARD Result<std::invoke_result_t<FuncT, const CleanT&&>, CleanE> Map(FuncT&& func) const && noexcept {

        if (IsOk()) {

            return (func)(std::move(GetOk()));
        }

        return std::move(*this).GetErr();
    }

    /**
     * @brief Retrieves the Err value.
     * @return A reference to the Err value.
     * @details Behavior is undefined if the Result contains an Ok value.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = "Result"_salloc;
     * assert(result.GetErr() == "Result");
     */
    template <typename U = CleanE>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanE>&, CleanE&>
        GetErr() & noexcept { ALT_ASSERT(!m_Storage.IsOk()); return m_Storage.GetErr(); }

    template <typename U = CleanE>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanE>&, const CleanE&>
        GetErr() const & noexcept { ALT_ASSERT(!m_Storage.IsOk()); return m_Storage.GetErr(); }

    template <typename U = CleanE>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanE>&, CleanE&&>
        GetErr() && noexcept { ALT_ASSERT(!m_Storage.IsOk()); return std::move(m_Storage.GetErr()); }

    template <typename U = CleanE>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanE>&, const CleanE&&>
        GetErr() const && noexcept { ALT_ASSERT(!m_Storage.IsOk()); return std::move(m_Storage.GetErr()); }

    /**
     * @brief Retrieves the Err value or a fallback.
     * @tparam U The type of the fallback value. Must be convertible to E.
     * @param v The fallback value.
     * @return The Err value if Err, otherwise the fallback value.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = 4;
     * assert(result.GetErrOr("Result"_salloc) == "Result");
     */
    template <typename U = CleanE,
        std::enable_if_t<std::is_convertible_v<U, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOr(U&& v) & noexcept { return !m_Storage.IsOk() ? m_Storage.GetErr() : static_cast<E>(std::forward<U>(v)); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_convertible_v<U, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOr(U&& v) const & noexcept { return !m_Storage.IsOk() ? m_Storage.GetErr() : static_cast<E>(std::forward<U>(v)); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_convertible_v<U, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOr(U&& v) && noexcept { return !m_Storage.IsOk() ? std::move(m_Storage).GetErr() : static_cast<E>(std::forward<U>(v)); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_convertible_v<U, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOr(U&& v) const && noexcept { return !m_Storage.IsOk() ? std::move(m_Storage).GetErr() : static_cast<E>(std::forward<U>(v)); }

    /**
     * @brief Retrieves the Err value or a default-constructed value.
     * @tparam U The type to default construct. Must be default constructible.
     * @return The Err value if Err, otherwise a default-constructed U.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = 4;
     * assert(result.GetErrOrDefault() == "");
     */
    template <typename U = CleanE,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrDefault() & noexcept { return GetErrOrExec([]() { return U{}; }); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrDefault() const & noexcept { return GetErrOrExec([]() { return U{}; }); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrDefault() && noexcept { return std::move(*this).GetErrOrExec([]() { return U{}; }); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrDefault() const && noexcept { return std::move(*this).GetErrOrExec([]() { return U{}; }); }

    /**
     * @brief Retrieves the Err value or executes a function to generate a fallback.
     * @tparam FuncT A callable type.
     * @param func A function that returns a value convertible to E.
     * @return The Err value if Err, otherwise the result of func().
     * @example
     * 
     * Alt::Result<usize, Alt::String> result = 4;
     * assert(result.GetErrOrExec([]() { return "Result"_salloc; }) == "Result");
     */
    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrExec(FuncT&& func) & noexcept { return !m_Storage.IsOk() ? m_Storage.GetErr() : static_cast<E>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrExec(FuncT&& func) const & noexcept { return !m_Storage.IsOk() ? m_Storage.GetErr() : static_cast<E>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrExec(FuncT&& func) && noexcept { return !m_Storage.IsOk() ? std::move(m_Storage).GetErr() : static_cast<E>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrExec(FuncT&& func) const && noexcept { return !m_Storage.IsOk() ? std::move(m_Storage).GetErr() : static_cast<E>((func)()); }

    /**
     * @brief Maps the error value using the function.
     * @tparam FuncT A callable type. Must return a type that is a Result.
     * @param func A function that takes E and returns a Result.
     * @return A new Result containing the mapped error or the original success.
     * @example
     * 
     * Alt::Result<usize, Alt::String> result0 = 4;
     * Alt::Result<usize, Alt::String> result1 = result0
     *     .MapErr([](const Alt::String&) { return Alt::Result<usize, Alt::String>::Err("Result"); })
     *     .MapErr([](const Alt::String&) { return Alt::Result<usize, Alt::String>::Err("Result"); })
     *     .MapErr([](const Alt::String&) { return Alt::Result<usize, Alt::String>::Err("Result"); })
     * ;
     * 
     * assert(*result1 == 4);
     * 
     * Alt::Result<usize, Alt::String> result0 = "Result";
     * Alt::Result<usize, Alt::String> result1 = result0
     *     .MapErr([](const Alt::String& x) { return Alt::Result<usize, Alt::String>::Err(x + x); })
     *     .MapErr([](const Alt::String& x) { return Alt::Result<usize, Alt::String>::Err(x + x); })
     *     .MapErr([](const Alt::String& x) { return Alt::Result<usize, Alt::String>::Err(x + x); })
     * ;
     * 
     * assert(result1.GetErr() == "ResultResultResultResultResultResultResultResult");
     */
    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanE&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, CleanE&> MapErr(FuncT&& func) & noexcept {

        if (IsOk()) {

            return GetOk();
        }

        return (func)(GetErr());
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanE&>>>* = nullptr>
    ALT_NODISCARD Result<CleanT, std::invoke_result_t<FuncT, CleanE&>> MapErr(FuncT&& func) & noexcept {

        if (IsOk()) {

            return GetOk();
        }

        return (func)(GetErr());
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanE&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const CleanE&> MapErr(FuncT&& func) const & noexcept {

        if (IsOk()) {

            return GetOk();
        }

        return (func)(GetErr());
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanE&>>>* = nullptr>
    ALT_NODISCARD Result<CleanT, std::invoke_result_t<FuncT, const CleanE&>> MapErr(FuncT&& func) const & noexcept {

        if (IsOk()) {

            return GetOk();
        }

        return (func)(GetErr());
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, E&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, E&&> MapErr(FuncT&& func) && noexcept {

        if (IsOk()) {

            return std::move(*this).GetOk();
        }

        return (func)(std::move(GetErr()));
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, E&&>>>* = nullptr>
    ALT_NODISCARD Result<CleanT, std::invoke_result_t<FuncT, E&&>> MapErr(FuncT&& func) && noexcept {

        if (IsOk()) {

            return std::move(*this).GetOk();
        }

        return (func)(std::move(GetErr()));
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, const E&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const E&&> MapErr(FuncT&& func) const && noexcept {

        if (IsOk()) {

            return std::move(*this).GetOk();
        }

        return (func)(std::move(GetErr()));
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, const E&&>>>* = nullptr>
    ALT_NODISCARD Result<CleanT, std::invoke_result_t<FuncT, const E&&>> MapErr(FuncT&& func) const && noexcept {

        if (IsOk()) {

            return std::move(*this).GetOk();
        }

        return (func)(std::move(GetErr()));
    }

private:

    template <typename, typename>
    friend class Result;

    ResultStorage m_Storage;
};

template <typename T_>
class Result<T_, void> {
private:

    using CleanT = TypeTraits::clean_t<T_>;

    using T = TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<CleanT>;

    using ResultStorage = _ResultDetails::ResultStorage<T, Unit_t>;

    using DerefOkT = typename ResultStorage::DerefOkT;
    
    template <typename U>
    constexpr static bool is_constructible_inner_ok_v = ResultStorage::template is_constructible_inner_ok_v<U>;

    template <typename U>
    constexpr static bool is_assignable_inner_ok_v = ResultStorage::template is_assignable_inner_ok_v<U>;

public:

    using OkT = T;
    using ErrT = void;

    /**
     * @brief Creates a Result containing the provided Ok value.
     * @tparam U The type of the value being passed.
     * @param val The value to wrap in the Result.
     * @return A Result instance representing the Ok state.
     * @example
     * 
     * Alt::Result<usize, void> result = Alt::Result<usize, void>::Ok(4);
     * assert(res.IsOk());
     */
    template <typename U = T,
        std::enable_if_t<is_constructible_inner_ok_v<U&&>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Result Ok(U&& val) noexcept {
        return Result(std::forward<U>(val));
    }

    /**
     * @brief Creates a failed Result containing the Err value.
     * @tparam U The type of the error being passed.
     * @param err The error value to wrap in the Result.
     * @return A Result instance representing the Err state.
     * @example
     * 
     * Alt::Result<usize, voide> result = Alt::Result<usize, void>::Err();
     * assert(res.IsErr());
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Result Err() noexcept {
        return Result(Alt::Err);
    }

    /**
     * @brief Constructors and Assignment Operators.
     * @details Handles construction.
     * @note The default constructor is deleted if the error type E is not default-constructible.
     * @example
     * 
     * Alt::Result<usize, void> result;
     * assert(res.IsErr());
     */
    ALT_FORCE_INLINE Result() noexcept
        : m_Storage(Alt::Err)
    {}

    /**
     * @brief Constructs a Result with an Err value.
     * @example
     * 
     * Alt::Result<usize, void> result(Alt::Err);
     * assert(result.IsErr());
     */
    ALT_FORCE_INLINE Result(Err_t) noexcept
        : m_Storage(Alt::Err)
    {}

    Result& operator=(Err_t) noexcept {

        m_Storage.Assign(Alt::Err);
        return *this;
    }

    /**
     * @brief Constructs a Result with a Ok value.
     * @tparam U The type of the value.
     * @param val The Ok value to wrap.
     * @example
     * 
     * Alt::Result<usize, void> result(4);
     * assert(result.IsOk());
     */
    template <typename U = T,
        std::enable_if_t<is_constructible_inner_ok_v<U&&>>* = nullptr>
    Result(U&& val) noexcept {

        m_Storage.Construct(_ResultDetails::InnerOk, std::forward<U>(val));
    }

    template <typename U = T,
        std::enable_if_t<is_assignable_inner_ok_v<U&&>>* = nullptr>
    Result& operator=(U&& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::InnerOk, std::forward<U>(rhs));
        return *this;
    }

    /**
     * @brief Constructs an Result from another Result with narrow cast.
     * @tparam U The type of the source Result.
     * @param val The source Result to move from.
     * @example
     * 
     * Alt::Result<usize, void> result0 = 4;
     * Alt::Result<ssize, void> result1 = result0;
     */
    template <typename T1 = T,
        std::enable_if_t<!std::is_constructible_v<T, Result<T1, void>&&> && is_constructible_inner_ok_v<T1&&>>* = nullptr>
    Result(Result<T1, void>&& val) noexcept {

        m_Storage.Construct(_ResultDetails::Rule5Move, val.m_Storage);
    }

    template <typename T1 = T,
        std::enable_if_t<!std::is_constructible_v<T, const Result<T1, void>&> && is_constructible_inner_ok_v<const T1&>>* = nullptr>
    Result(const Result<T1, void>& val) noexcept {

        m_Storage.Construct(_ResultDetails::Rule5Copy, val.m_Storage);
    }

    /**
     * @brief Assigns an Result from another Result with narrow cast.
     * @tparam U The type of the source Result.
     * @param rhs The source Result.
     * @example
     * 
     * Alt::Result<usize, void> result0 = 4;
     * Alt::Result<ssize, void> result1 = 8;
     * result1 = std::move(result0);
     */
    template <typename T1 = T,
        std::enable_if_t<!std::is_assignable_v<T&, Result<T1, void>&&> && is_assignable_inner_ok_v<T1&&>>* = nullptr>
    Result& operator=(Result<T1, void>&& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::Rule5Move, rhs.m_Storage);
        return *this;
    }

    template <typename T1 = T,
        std::enable_if_t<!std::is_assignable_v<T&, const Result<T1, void>&> && is_assignable_inner_ok_v<const T1&>>* = nullptr>
    Result& operator=(const Result<T1, void>& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::Rule5Copy, rhs.m_Storage);
        return *this;
    }

    /**
     * @brief Clones the current Result.
     * 
     * @tparam U The type of the source Result.
     * @param val The destination Result to clone.
     * @return A bool, false when clone failed.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Result<Alt::Vector<usize>, void> res0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
     * Alt::Result<Alt::Vector<usize>, void> res1;
     * 
     * res1 = std::move(res0);
     * res0.CloneFrom(res1) iferr_return;
     * 
     * assert(*res0, *res1);
     */
    template <typename T1 = T,
        std::enable_if_t<
            TypeTraits::is_only_clonable_to_from_v<T1, const T&> || is_assignable_inner_ok_v<const T1&>
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const Result<T1, void>& rhs) noexcept {

        return m_Storage.AssignClone(rhs.m_Storage);
    }

    template <typename T1 = T,
        std::enable_if_t<
            TypeTraits::is_only_clonable_to_from_v<T1, const T&> || is_constructible_inner_ok_v<const T1&>
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const Result<T1, void>& rhs) noexcept {

        return m_Storage.ConstructClone(rhs.m_Storage);
    }

    ALT_NODISCARD Optional<Result> Clone() const noexcept {

        struct Object {

            ~Object() noexcept {

                if (!isValid) {
                    return;
                }

                Result* pObject = reinterpret_cast<Result*>(memory);
                Memory::DestroyObject(pObject);

                isValid = false;
            }

            alignas(Result) u8 memory[sizeof(Result)];
            bool isValid = false;
        };

        Object object{};
        Result* pObject = reinterpret_cast<Result*>(object.memory);

        const bool isOk = pObject->CloneConstructFrom(*this);
        object.isValid = isOk;

        if (!isOk) {
            return Nullopt;
        }

        return MakeOptional(std::move(*pObject));
    }

    /**
     * @brief Dereferences the Result to access the Ok value.
     * @tparam U The deduced type of the underlying value.
     * @return A reference to the Ok value.
     * @details Behavior is undefined if the Result contains an error.
     * @example
     * 
     * Alt::Result<usize, void> result = 4;
     * assert(*result == 4);
     */
    template <typename U = DerefOkT>
    ALT_FORCE_INLINE U& operator*() & noexcept { ALT_ASSERT(m_Storage.IsOk()); return *m_Storage.Deref(); }

    template <typename U = DerefOkT>
    ALT_FORCE_INLINE const U& operator*() const & noexcept { ALT_ASSERT(m_Storage.IsOk()); return *m_Storage.Deref(); }

    template <typename U = DerefOkT>
    ALT_FORCE_INLINE U&& operator*() && noexcept { ALT_ASSERT(m_Storage.IsOk()); return std::move(*m_Storage.Deref()); }

    template <typename U = DerefOkT>
    ALT_FORCE_INLINE const U&& operator*() const && noexcept { ALT_ASSERT(m_Storage.IsOk()); return std::move(*m_Storage.Deref()); }

    /**
     * @brief Accesses the Ok value via member access.
     * @tparam U The deduced type of the underlying value.
     * @return A pointer to the Ok value.
     * @details Behavior is undefined if the Result contains an error.
     * @example
     * 
     * Alt::Result<Alt::String, void> result = "Result"_salloc;
     * assert(result->size() == 6);
     */
    ALT_FORCE_INLINE DerefOkT* operator->() noexcept { ALT_ASSERT(m_Storage.IsOk()); return m_Storage.Deref(); }
    ALT_FORCE_INLINE const DerefOkT* operator->() const noexcept { ALT_ASSERT(m_Storage.IsOk()); return m_Storage.Deref(); }

    /**
     * @brief Equality comparison against an Ok state.
     * @param obj An Ok_t marker.
     * @return True if the Result is Ok, false otherwise.
     * @example
     * 
     * Alt::Result<usize, void> result = 4;
     * 
     * assert(result == Alt::Ok);
     */
    ALT_FORCE_INLINE bool operator==(Ok_t) const noexcept { return IsOk(); }
    ALT_FORCE_INLINE bool operator!=(Ok_t) const noexcept { return !IsOk(); }

    /**
     * @brief Equality comparison against an Err state.
     * @param obj An Err_t marker.
     * @return True if the Result is Err, false otherwise.
     * @example
     * 
     * Alt::Result<usize, void> result;
     * 
     * assert(result == Alt::Err);
     */
    ALT_FORCE_INLINE bool operator==(Err_t) const noexcept { return !IsOk(); }
    ALT_FORCE_INLINE bool operator!=(Err_t) const noexcept { return IsOk(); }

    /**
     * @brief Accesses the Ok value throught pointer.
     * @tparam U The deduced type of the underlying value.
     * @return A pointer to the Ok value.
     * @example
     * 
     * Alt::Result<Alt::String, void> result = Alt::Err;
     * 
     * Alt::String* pStr = result;
     * assert(pStr == nullptr);
     */
    template <typename U = T,
        std::enable_if_t<_ResultDetails::is_layout_not_own_value_v<U>>* = nullptr>
    ALT_FORCE_INLINE operator DerefOkT*() noexcept { return m_Storage.Deref(); }

    template <typename U = T,
        std::enable_if_t<_ResultDetails::is_layout_not_own_value_v<U>>* = nullptr>
    ALT_FORCE_INLINE operator const DerefOkT*() const noexcept { return m_Storage.Deref(); }

    /**
     * @brief Checks if the Result is in the Ok state.
     * @return True if Ok, false if Err.
     * @example
     * 
     * Alt::Result<Alt::String, void> result = "Result"_salloc;
     * assert(result.IsOk());
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsOk() const noexcept { return m_Storage.IsOk(); }

    /**
     * @brief Checks if the Result is in the Err state.
     * @return True if Err, false if Ok.
     * @example
     * 
     * Alt::Result<Alt::String, void> result = Alt::Err;
     * assert(result.IsErr());
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsErr() const noexcept { return !m_Storage.IsOk(); }

    /**
     * @brief Explicit conversion to boolean.
     * @return True if the Result is Ok, false if Err.
     * @example
     * 
     * Alt::Result<Alt::String, void> result = "Result"_salloc;
     * assert(result);
     */
    ALT_NODISCARD ALT_FORCE_INLINE explicit operator bool() const noexcept { return IsOk(); }

    /**
     * @brief Retrieves the Ok value.
     * @return A reference to the Ok value.
     * @details Behavior is undefined if the Result contains an error.
     * @example
     * 
     * Alt::Result<usize, void> result = 4;
     * assert(result.GetOk() == 4);
     */
    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT>&, CleanT&>
        GetOk() & noexcept { ALT_ASSERT(m_Storage.IsOk()); return m_Storage.GetOk(); }

    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT>&, const CleanT&>
        GetOk() const & noexcept { ALT_ASSERT(m_Storage.IsOk()); return m_Storage.GetOk(); }

    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT>&, CleanT&&>
        GetOk() && noexcept { ALT_ASSERT(m_Storage.IsOk()); return std::move(m_Storage.GetOk()); }

    template <typename U = CleanT>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT>&, const CleanT&&>
        GetOk() const && noexcept { ALT_ASSERT(m_Storage.IsOk()); return std::move(m_Storage.GetOk()); }

    /**
     * @brief Retrieves the Ok value or a fallback.
     * @tparam U The type of the fallback value. Must be convertible to T.
     * @param v The fallback value.
     * @return The Ok value if Ok, otherwise the fallback value.
     * @example
     * 
     * Alt::Result<usize, void> result = Alt::Err;
     * assert(result.GetOkOr(4) == 4);
     */
    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOr(U&& v) & noexcept { return m_Storage.IsOk() ? m_Storage.GetOk() : static_cast<T>(std::forward<U>(v)); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOr(U&& v) const & noexcept { return m_Storage.IsOk() ? m_Storage.GetOk() : static_cast<T>(std::forward<U>(v)); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOr(U&& v) && noexcept { return m_Storage.IsOk() ? std::move(m_Storage).GetOk() : static_cast<T>(std::forward<U>(v)); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_convertible_v<U, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOr(U&& v) const && noexcept { return m_Storage.IsOk() ? std::move(m_Storage).GetOk() : static_cast<T>(std::forward<U>(v)); }

    /**
     * @brief Retrieves the Ok value or a default-constructed value.
     * @tparam U The type to default construct. Must be default constructible.
     * @return The Ok value if Ok, otherwise a default-constructed U.
     * @example
     * 
     * Alt::Result<Alt::String, void> result = Alt::Err;
     * assert(result.GetOkOrDefault() == "");
     */
    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrDefault() & noexcept { return GetOkOrExec([]() { return U{}; }); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrDefault() const & noexcept { return GetOkOrExec([]() { return U{}; }); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrDefault() && noexcept { return std::move(*this).GetOkOrExec([]() { return U{}; }); }

    template <typename U = CleanT,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrDefault() const && noexcept { return std::move(*this).GetOkOrExec([]() { return U{}; }); }

    /**
     * @brief Retrieves the Ok value or executes a function to generate a fallback.
     * @tparam FuncT A callable type.
     * @param func A function that returns a value convertible to T.
     * @return The Ok value if Ok, otherwise the result of func().
     * @example
     * 
     * Alt::Result<usize, void> result = Alt::Err;
     * assert(res.GetOkOrExec([](){ return 4; }) == 4);
     */
    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrExec(FuncT&& func) & noexcept { return m_Storage.IsOk() ? m_Storage.GetOk() : static_cast<T>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrExec(FuncT&& func) const & noexcept { return m_Storage.IsOk() ? m_Storage.GetOk() : static_cast<T>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrExec(FuncT&& func) && noexcept { return m_Storage.IsOk() ? std::move(m_Storage).GetOk() : static_cast<T>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanT GetOkOrExec(FuncT&& func) const && noexcept { return m_Storage.IsOk() ? std::move(m_Storage).GetOk() : static_cast<T>((func)()); }

    /**
     * @brief Map the Ok value using the function.
     * @tparam FuncT A callable type. Must return a type that is a Result.
     * @param func A function that takes T and returns a Result.
     * @return A new Result containing the mapped value or the original Ok value.
     * @example
     * 
     * Alt::Result<usize, void> result0 = 4;
     * Alt::Result<usize, void> result1 = result0
     *     .Map([](const usize x) { return Alt::Result<usize, void>::Ok(x * 2); })
     *     .Map([](const usize x) { return Alt::Result<usize, void>::Ok(x + 4); })
     *     .Map([](const usize x) { return Alt::Result<usize, void>::Ok(x * 2); })
     * ;
     * assert(*result1 == 24);
     * 
     * Alt::Result<usize, void> result0 = Alt::Err;
     * Alt::Result<usize, void> result1 = result0
     *     .Map([](const usize x) { return Alt::Result<usize, void>::Ok(x * 2); })
     *     .Map([](const usize x) { return Alt::Result<usize, void>::Ok(x + 4); })
     *     .Map([](const usize x) { return Alt::Result<usize, void>::Ok(x * 2); })
     * ;
     * assert(result1.IsErr());
     */
    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanT&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, CleanT&> Map(FuncT&& func) & noexcept {

        if (IsOk()) {

            return (func)(GetOk());
        }

        return Alt::Err;
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanT&>>>* = nullptr>
    ALT_NODISCARD Result<std::invoke_result_t<FuncT, CleanT&>, void> Map(FuncT&& func) & noexcept {

        if (IsOk()) {

            return (func)(GetOk());
        }

        return Alt::Err;
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanT&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const CleanT&> Map(FuncT&& func) const & noexcept {

        if (IsOk()) {

            return (func)(GetOk());
        }

        return Alt::Err;
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanT&>>>* = nullptr>
    ALT_NODISCARD Result<std::invoke_result_t<FuncT, const CleanT&>, void> Map(FuncT&& func) const & noexcept {

        if (IsOk()) {

            return (func)(GetOk());
        }

        return Alt::Err;
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanT&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, CleanT&&> Map(FuncT&& func) && noexcept {

        if (IsOk()) {

            return (func)(std::move(GetOk()));
        }

        return Alt::Err;
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanT&&>>>* = nullptr>
    ALT_NODISCARD Result<std::invoke_result_t<FuncT, CleanT&&>, void> Map(FuncT&& func) && noexcept {

        if (IsOk()) {

            return (func)(std::move(GetOk()));
        }

        return Alt::Err;
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanT&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const CleanT&&> Map(FuncT&& func) const && noexcept {

        if (IsOk()) {

            return (func)(std::move(GetOk()));
        }

        return Alt::Err;
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanT&&>>>* = nullptr>
    ALT_NODISCARD Result<std::invoke_result_t<FuncT, const CleanT&&>, void> Map(FuncT&& func) const && noexcept {

        if (IsOk()) {

            return (func)(std::move(GetOk()));
        }

        return Alt::Err;
    }

private:

    template <typename, typename>
    friend class Result;

    ResultStorage m_Storage;
};

template <typename E_>
class Result<void, E_> {
private:

    using CleanE = TypeTraits::clean_t<E_>;

    using E = TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<CleanE>;

    using ResultStorage = _ResultDetails::ResultStorage<Unit_t, E>;

    template <typename U>
    constexpr static bool is_constructible_inner_err_v = ResultStorage::template is_constructible_inner_err_v<U>;

    template <typename U>
    constexpr static bool is_assignable_inner_err_v = ResultStorage::template is_assignable_inner_err_v<U>;

public:

    using OkT = void;
    using ErrT = E;

    /**
     * @brief Creates a Result containing the provided Ok value.
     * @tparam U The type of the value being passed.
     * @param val The value to wrap in the Result.
     * @return A Result instance representing the Ok state.
     * @example
     * 
     * Alt::Result<void, usize> result = Alt::Result<void, usize>::Ok();
     * assert(res.IsOk());
     */
    ALT_NODISCARD ALT_FORCE_INLINE static Result Ok() noexcept {
        return Result(Alt::Ok);
    }

    /**
     * @brief Creates a failed Result containing the Err value.
     * @tparam U The type of the error being passed.
     * @param err The error value to wrap in the Result.
     * @return A Result instance representing the Err state.
     * @example
     * 
     * Alt::Result<void, usize> result = Alt::Result<usize, usize>::Err(4);
     * assert(res.IsErr());
     */
    template <typename U = E,
        std::enable_if_t<is_constructible_inner_err_v<U&&>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Result Err(E&& err) noexcept {
        return Result(std::forward<U>(err));
    }

    /**
     * @brief Constructors and Assignment Operators.
     * @details Handles construction.
     * @note The default constructor is deleted if the error type E is not default-constructible.
     * @example
     * 
     * Alt::Result<void, Alt::String> result;
     * assert(res.IsErr());
     */
    ALT_FORCE_INLINE Result() noexcept
        : m_Storage(Alt::Ok)
    {}

    /**
     * @brief Constructs a Result with a Ok value.
     * @example
     * 
     * Alt::Result<void, Alt::String> result = Alt::Ok;
     * assert(result.IsOk());
     */
    ALT_FORCE_INLINE Result(Ok_t) noexcept
        : m_Storage(Alt::Ok)
    {}

    Result& operator=(Ok_t) noexcept {

        m_Storage.Assign(Alt::Ok);
        return *this;
    }

    /**
     * @brief Constructs a Result with an Err value.
     * @tparam U The type of the error. Must be constructible into E and not constructible into T.
     * @param val The error value to wrap.
     * @example
     * 
     * Alt::Result<void, Alt::String> result("Result"_salloc);
     * assert(result.IsErr());
     */
    template <typename U = E,
        std::enable_if_t<is_constructible_inner_err_v<U&&>>* = nullptr>
    Result(U&& val) noexcept {

        m_Storage.Construct(_ResultDetails::InnerErr, std::forward<U>(val));
    }

    template <typename U = E,
        std::enable_if_t<is_assignable_inner_err_v<U&&>>* = nullptr>
    Result& operator=(U&& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::InnerErr, std::forward<U>(rhs));
        return *this;
    }

    /**
     * @brief Constructs a Result from a 'ResultErr' wrapper.
     * @tparam U The error type.
     * @param err A 'ResultErr' object containing the Err value.
     * @example
     * 
     * Alt::Result<void, usize> result = Alt::ResultErr<usize>{ 4 };
     * assert(result.IsErr());
     */
    template <typename U = E,
        std::enable_if_t<is_constructible_inner_err_v<U&&>>* = nullptr>
    Result(ResultErr<U>&& err) noexcept {

        m_Storage.Construct(_ResultDetails::InnerErr, std::move(err.err));
    }

    template <typename U = E,
        std::enable_if_t<is_constructible_inner_err_v<const U&>>* = nullptr>
    Result(const ResultErr<U>& err) noexcept {

        m_Storage.Construct(_ResultDetails::InnerErr, err.err);
    }

    template <typename U = E,
        std::enable_if_t<is_assignable_inner_err_v<U&&>>* = nullptr>
    Result& operator=(ResultErr<U>&& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::InnerErr, std::move(rhs.err));
        return *this;
    }

    template <typename U = E,
        std::enable_if_t<is_assignable_inner_err_v<const U&>>* = nullptr>
    Result& operator=(const ResultErr<U>& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::InnerErr, rhs.err);
        return *this;
    }

    /**
     * @brief Constructs an Result from another Result with narrow cast.
     * @tparam U The type of the source Result.
     * @param val The source Result to move from.
     * @example
     * 
     * Alt::Result<void, usize> result0 = 4;
     * Alt::Result<void, ssize> result1 = result0;
     */
    template <typename E1 = E,
        std::enable_if_t<!std::is_constructible_v<E, Result<void, E1>&&> && is_constructible_inner_err_v<E1&&>>* = nullptr>
    Result(Result<void, E1>&& val) noexcept {

        m_Storage.Construct(_ResultDetails::Rule5Move, val.m_Storage);
    }

    template <typename E1 = E,
        std::enable_if_t<!std::is_constructible_v<E, const Result<void, E1>&> && is_constructible_inner_err_v<const E1&>>* = nullptr>
    Result(const Result<void, E1>& val) noexcept {

        m_Storage.Construct(_ResultDetails::Rule5Copy, val.m_Storage);
    }

    /**
     * @brief Assigns an Result from another Result with narrow cast.
     * @tparam U The type of the source Result.
     * @param rhs The source Result.
     * @example
     * 
     * Alt::Result<void, usize> result0 = 4;
     * Alt::Result<void, ssize> result1 = 8;
     * result1 = std::move(result0);
     */
    template <typename E1 = E,
        std::enable_if_t<!std::is_assignable_v<E&, Result<void, E1>&&> && is_assignable_inner_err_v<E1&&>>* = nullptr>
    Result& operator=(Result<void, E1>&& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::Rule5Move, rhs.m_Storage);
        return *this;
    }

    template <typename E1 = E,
        std::enable_if_t<!std::is_assignable_v<E&, const Result<void, E1>&> && is_assignable_inner_err_v<const E1&>>* = nullptr>
    Result& operator=(const Result<void, E1>& rhs) noexcept {

        m_Storage.Assign(_ResultDetails::Rule5Copy, rhs.m_Storage);
        return *this;
    }

    /**
     * @brief Clones the current Result.
     * 
     * @tparam U The type of the source Result.
     * @param val The destination Result to clone.
     * @return A bool, false when clone failed.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Result<void, Alt::Vector<usize>> res0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
     * Alt::Result<void, Alt::Vector<usize>> res1;
     * 
     * res1 = std::move(res0);
     * res0.CloneFrom(res1) iferr_return;
     * 
     * assert(res0.GetErr(), res1.GetErr());
     */
    template <typename E1 = E,
        std::enable_if_t<
            TypeTraits::is_only_clonable_to_from_v<E1, const E&> || is_assignable_inner_err_v<const E1&>
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const Result<void, E1>& rhs) noexcept {

        return m_Storage.AssignClone(rhs.m_Storage);
    }

    template <typename E1 = E,
        std::enable_if_t<
            TypeTraits::is_only_clonable_to_from_v<E1, const E&> || is_constructible_inner_err_v<const E1&>
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const Result<void, E1>& rhs) noexcept {

        return m_Storage.ConstructClone(rhs.m_Storage);
    }

    ALT_NODISCARD Optional<Result> Clone() const noexcept {

        struct Object {

            ~Object() noexcept {

                if (!isValid) {
                    return;
                }

                Result* pObject = reinterpret_cast<Result*>(memory);
                Memory::DestroyObject(pObject);

                isValid = false;
            }

            alignas(Result) u8 memory[sizeof(Result)];
            bool isValid = false;
        };

        Object object{};
        Result* pObject = reinterpret_cast<Result*>(object.memory);

        const bool isOk = pObject->CloneConstructFrom(*this);
        object.isValid = isOk;

        if (!isOk) {
            return Nullopt;
        }

        return MakeOptional(std::move(*pObject));
    }

    /**
     * @brief Equality comparison against an Ok state.
     * @param obj An Ok_t marker.
     * @return True if the Result is Ok, false otherwise.
     * @example
     * 
     * Alt::Result<void, Alt::String> result = Alt::Ok;
     * 
     * assert(result == Alt::Ok);
     */
    ALT_FORCE_INLINE bool operator==(Ok_t) const noexcept { return IsOk(); }
    ALT_FORCE_INLINE bool operator!=(Ok_t) const noexcept { return !IsOk(); }

    /**
     * @brief Equality comparison against an Err state.
     * @param obj An Err_t marker.
     * @return True if the Result is Err, false otherwise.
     * @example
     * 
     * Alt::Result<void, Alt::String> result = "Result"_salloc;
     * 
     * assert(result == Alt::Err);
     */
    ALT_FORCE_INLINE bool operator==(Err_t) const noexcept { return !IsOk(); }
    ALT_FORCE_INLINE bool operator!=(Err_t) const noexcept { return IsOk(); }

    /**
     * @brief Checks if the Result is in the Ok state.
     * @return True if Ok, false if Err.
     * @example
     * 
     * Alt::Result<void, usize> result = Alt::Ok;
     * assert(result.IsOk());
     */
    ALT_FORCE_INLINE bool IsOk() const noexcept { return m_Storage.IsOk(); }

    /**
     * @brief Checks if the Result is in the Err state.
     * @return True if Err, false if Ok.
     * @example
     * 
     * Alt::Result<void, usize> result = 4;
     * assert(result.IsErr());
     */
    ALT_FORCE_INLINE bool IsErr() const noexcept { return !m_Storage.IsOk(); }

    /**
     * @brief Explicit conversion to boolean.
     * @return True if the Result is Ok, false if Err.
     * @example
     * 
     * Alt::Result<void, usize> result = Alt::Ok;
     * assert(result);
     */
    ALT_FORCE_INLINE explicit operator bool() const noexcept { return IsOk(); }

    /**
     * @brief Retrieves the Err value.
     * @return A reference to the Err value.
     * @details Behavior is undefined if the Result contains an Ok value.
     * @example
     * 
     * Alt::Result<void, Alt::String> result = "Result"_salloc;
     * assert(result.GetErr() == "Result");
     */
    template <typename U = CleanE>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanE>&, CleanE&>
        GetErr() & noexcept { ALT_ASSERT(!m_Storage.IsOk()); return m_Storage.GetErr(); }

    template <typename U = CleanE>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanE>&, const CleanE&>
        GetErr() const & noexcept { ALT_ASSERT(!m_Storage.IsOk()); return m_Storage.GetErr(); }

    template <typename U = CleanE>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanE>&, CleanE&&>
        GetErr() && noexcept { ALT_ASSERT(!m_Storage.IsOk()); return std::move(m_Storage.GetErr()); }

    template <typename U = CleanE>
    ALT_NODISCARD ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanE>&, const CleanE&&>
        GetErr() const && noexcept { ALT_ASSERT(!m_Storage.IsOk()); return std::move(m_Storage.GetErr()); }

    /**
     * @brief Retrieves the Err value or a fallback.
     * @tparam U The type of the fallback value. Must be convertible to E.
     * @param v The fallback value.
     * @return The Err value if Err, otherwise the fallback value.
     * @example
     * 
     * Alt::Result<void, Alt::String> result = Alt::Ok;
     * assert(result.GetErrOr("Result"_salloc) == "Result");
     */
    template <typename U = CleanE,
        std::enable_if_t<std::is_convertible_v<U, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOr(U&& v) & noexcept { return !m_Storage.IsOk() ? m_Storage.GetErr() : static_cast<E>(std::forward<U>(v)); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_convertible_v<U, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOr(U&& v) const & noexcept { return !m_Storage.IsOk() ? m_Storage.GetErr() : static_cast<E>(std::forward<U>(v)); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_convertible_v<U, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOr(U&& v) && noexcept { return !m_Storage.IsOk() ? std::move(m_Storage).GetErr() : static_cast<E>(std::forward<U>(v)); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_convertible_v<U, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOr(U&& v) const && noexcept { return !m_Storage.IsOk() ? std::move(m_Storage).GetErr() : static_cast<E>(std::forward<U>(v)); }

    /**
     * @brief Retrieves the Err value or a default-constructed value.
     * @tparam U The type to default construct. Must be default constructible.
     * @return The Err value if Err, otherwise a default-constructed U.
     * @example
     * 
     * Alt::Result<void, Alt::String> result = Alt::Ok;
     * assert(result.GetErrOrDefault() == "");
     */
    template <typename U = CleanE,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrDefault() & noexcept { return GetErrOrExec([]() { return U{}; }); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrDefault() const & noexcept { return GetErrOrExec([]() { return U{}; }); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrDefault() && noexcept { return std::move(*this).GetErrOrExec([]() { return U{}; }); }

    template <typename U = CleanE,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrDefault() const && noexcept { return std::move(*this).GetErrOrExec([]() { return U{}; }); }

    /**
     * @brief Retrieves the Err value or executes a function to generate a fallback.
     * @tparam FuncT A callable type.
     * @param func A function that returns a value convertible to E.
     * @return The Err value if Err, otherwise the result of func().
     * @example
     * 
     * Alt::Result<void, Alt::String> result = Alt::Ok;
     * assert(result.GetErrOrExec([]() { return "Result"_salloc; }) == "Result");
     */
    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrExec(FuncT&& func) & noexcept { return !m_Storage.IsOk() ? m_Storage.GetErr() : static_cast<E>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrExec(FuncT&& func) const & noexcept { return !m_Storage.IsOk() ? m_Storage.GetErr() : static_cast<E>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrExec(FuncT&& func) && noexcept { return !m_Storage.IsOk() ? std::move(m_Storage).GetErr() : static_cast<E>((func)()); }

    template <typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, CleanE>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE CleanE GetErrOrExec(FuncT&& func) const && noexcept { return !m_Storage.IsOk() ? std::move(m_Storage).GetErr() : static_cast<E>((func)()); }

    /**
     * @brief Maps the error value using the function.
     * @tparam FuncT A callable type. Must return a type that is a Result.
     * @param func A function that takes E and returns a Result.
     * @return A new Result containing the mapped error or the original success.
     * @example
     * 
     * Alt::Result<void, Alt::String> result0 = Alt::Ok;
     * Alt::Result<void, Alt::String> result1 = result0
     *     .MapErr([](const Alt::String&) { return Alt::Result<void, Alt::String>::Err("Result"); })
     *     .MapErr([](const Alt::String&) { return Alt::Result<void, Alt::String>::Err("Result"); })
     *     .MapErr([](const Alt::String&) { return Alt::Result<void, Alt::String>::Err("Result"); })
     * ;
     * 
     * assert(result1.IsOk());
     * 
     * Alt::Result<void, Alt::String> result0 = Alt::Ok;
     * Alt::Result<void, Alt::String> result1 = result0
     *     .MapErr([](const Alt::String& x) { return Alt::Result<void, Alt::String>::Err(x + x); })
     *     .MapErr([](const Alt::String& x) { return Alt::Result<void, Alt::String>::Err(x + x); })
     *     .MapErr([](const Alt::String& x) { return Alt::Result<void, Alt::String>::Err(x + x); })
     * ;
     * 
     * assert(result1.GetErr() == "ResultResultResultResultResultResultResultResult");
     */
    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanE&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, CleanE&> MapErr(FuncT&& func) & noexcept {

        if (IsOk()) {

            return Alt::Ok;
        }

        return (func)(GetErr());
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, CleanE&>>>* = nullptr>
    ALT_NODISCARD Result<void, std::invoke_result_t<FuncT, CleanE&>> MapErr(FuncT&& func) & noexcept {

        if (IsOk()) {

            return Alt::Ok;
        }

        return (func)(GetErr());
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanE&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const CleanE&> MapErr(FuncT&& func) const & noexcept {

        if (IsOk()) {

            return Alt::Ok;
        }

        return (func)(GetErr());
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, const CleanE&>>>* = nullptr>
    ALT_NODISCARD Result<void, std::invoke_result_t<FuncT, const CleanE&>> MapErr(FuncT&& func) const & noexcept {

        if (IsOk()) {

            return Alt::Ok;
        }

        return (func)(GetErr());
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, E&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, E&&> MapErr(FuncT&& func) && noexcept {

        if (IsOk()) {

            return Alt::Ok;
        }

        return (func)(std::move(GetErr()));
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, E&&>>>* = nullptr>
    ALT_NODISCARD Result<void, std::invoke_result_t<FuncT, E&&>> MapErr(FuncT&& func) && noexcept {

        if (IsOk()) {

            return Alt::Ok;
        }

        return (func)(std::move(GetErr()));
    }

    template <typename FuncT,
        std::enable_if_t<TypeTraits::is_result_v<std::invoke_result_t<FuncT, const E&&>>>* = nullptr>
    ALT_NODISCARD std::invoke_result_t<FuncT, const E&&> MapErr(FuncT&& func) const && noexcept {

        if (IsOk()) {

            return Alt::Ok;
        }

        return (func)(std::move(GetErr()));
    }

    template <typename FuncT,
        std::enable_if_t<!TypeTraits::is_result_v<std::invoke_result_t<FuncT, const E&&>>>* = nullptr>
    ALT_NODISCARD Result<void, std::invoke_result_t<FuncT, const E&&>> MapErr(FuncT&& func) const && noexcept {

        if (IsOk()) {

            return Alt::Ok;
        }

        return (func)(std::move(GetErr()));
    }

private:

    template <typename, typename>
    friend class Result;

    ResultStorage m_Storage;
};

template <>
class Result<void, void> {
public:

    using OkT = void;
    using ErrT = void;

    ALT_FORCE_INLINE Result() noexcept = default;

    ALT_FORCE_INLINE Result(Ok_t) noexcept
        : m_IsOk(true)
    {}

    ALT_FORCE_INLINE Result& operator=(Ok_t) noexcept {

        m_IsOk = true;
        return *this;
    }

    ALT_FORCE_INLINE Result(Err_t) noexcept
        : m_IsOk(false)
    {}

    ALT_FORCE_INLINE Result& operator=(Err_t) noexcept {

        m_IsOk = false;
        return *this;
    }

    ALT_FORCE_INLINE bool operator==(Ok_t) const noexcept { return IsOk(); }
    ALT_FORCE_INLINE bool operator!=(Ok_t) const noexcept { return !IsOk(); }

    ALT_FORCE_INLINE bool operator==(Err_t) const noexcept { return !IsOk(); }
    ALT_FORCE_INLINE bool operator!=(Err_t) const noexcept { return IsOk(); }

    ALT_FORCE_INLINE bool IsOk() const noexcept { return m_IsOk; }
    ALT_FORCE_INLINE bool IsErr() const noexcept { return !m_IsOk; }
    ALT_FORCE_INLINE explicit operator bool() const noexcept { return IsOk(); }

private:

    bool m_IsOk = false;
};

#define __ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(ForbiddenT) \
template <typename T> class Result<T, ForbiddenT>; \
template <typename E> class Result<ForbiddenT, E>;

__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(Ok_t)
__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(const Ok_t)
__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(volatile Ok_t)
__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(const volatile Ok_t)

__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(Err_t)
__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(const Err_t)
__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(volatile Err_t)
__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(const volatile Err_t)

__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(const void)
__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(volatile void)
__ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__(const volatile void)

#undef __ALT_STD_INTERNAL_IMPL_RESULT_FORBIDDEN__

} // namespace Alt

#endif // __ALT_STD_RESULT__