#pragma once

#ifndef __ALT_STD_UNION__
#define __ALT_STD_UNION__

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Memory/MaybeUninit.h"

#include "Optional.h"

namespace Alt {

/**
 * @brief A tag type representing a uninitialized state.
 * 
 * @example
 * 
 * Alt::Union<usize, f32> un = Alt::Noval;
 * assert(un.GetTypeIdx().IsNone());
 */
struct Noval_t {};
constexpr static Noval_t Noval = Noval_t{};

template <typename ...ArgsT>
class Union;

/** \cond */
namespace _UnionDetails {

template <typename T>
using unwrap_reference_to_alt_ref_or_just_type_t = typename TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<T>;

} // namespace _UnionDetails
/** \endcond */

namespace TypeTraits {

template <typename ...ArgsT>
struct __is_union__<Union<ArgsT...>>
    : std::true_type
{};

template <typename ...ArgsT>
struct __is_trivially_reallocatable__<Union<ArgsT...>>
    : std::conditional_t<
        std::conjunction_v<is_trivially_reallocatable_t<_UnionDetails::unwrap_reference_to_alt_ref_or_just_type_t<ArgsT>>...>,
        std::true_type,
        std::false_type
    >
{};

} // namespace TypeTraits

/** \cond */
namespace _UnionDetails {

template <typename T>
using clean_t = TypeTraits::clean_t<T>;

template <usize SIZE>
using count_to_type_t = TypeTraits::count_to_type_t<SIZE>;

template <usize ACC, typename T, typename ...TailT>
struct find_type_index {

    constexpr static usize value = ACC;
};

template <usize ACC, typename T, typename FirstT, typename ...TailT>
struct find_type_index<ACC, T, FirstT, TailT...>
    : std::conditional_t<
        std::is_same_v<clean_t<T>, clean_t<FirstT>>,
        std::integral_constant<usize, ACC>,
        find_type_index<ACC + 1, T, TailT...>
    >
{};

template <typename T>
using unwrap_void_to_noval_or_just_type_t = std::conditional_t<
    std::is_void_v<T>,
    Noval_t,
    T
>;

struct Rule5Copy_t {};
struct Rule5Move_t {};
struct InnerRef_t {};
struct InnerValue_t {};

constexpr static Rule5Copy_t Rule5Copy = Rule5Copy_t{};
constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};
constexpr static InnerRef_t InnerRef = InnerRef_t{};
constexpr static InnerValue_t InnerValue = InnerValue_t{};

template <typename ...ArgsT>
struct UnionCore {

    MaybeUninitOneOf<ArgsT...> m_Val;
};

template <>
struct UnionCore<> {
};

template <typename Usize, typename ...ArgsT>
struct UnionOperator;

template <typename Usize, typename FirstT, typename ...TailT>
struct UnionOperator<Usize, FirstT, TailT...> {

    constexpr static Usize INVALID_TYPE_IDX = find_type_index<0, Noval_t, FirstT, TailT...>::value;

    template <typename T>
    using is_same_as_first = std::is_same<clean_t<T>, clean_t<FirstT>>;

    template <typename T>
    constexpr static bool is_same_as_first_v = is_same_as_first<T>::value;

    template <typename T>
    using is_exist = std::disjunction<is_same_as_first<T>, std::disjunction<std::is_same<clean_t<T>, clean_t<TailT>>...>>;

    template <typename T>
    constexpr static bool is_exist_v = is_exist<T>::value;

    template <Usize ACC, typename T>
    constexpr static Usize FindTypeIndex() noexcept {
        return _UnionDetails::find_type_index<0, T, FirstT, TailT...>::value;
    }

    template <typename T, typename SelfT,
        std::enable_if_t<is_same_as_first_v<T>>* = nullptr>
    constexpr static const unwrap_reference_to_alt_ref_or_just_type_t<T>& Get(const SelfT& self) noexcept {
        return self.m_Val.template GetVal<unwrap_reference_to_alt_ref_or_just_type_t<T>>();
    }

    template <typename T, typename SelfT,
        std::enable_if_t<!is_same_as_first_v<T>>* = nullptr>
    constexpr static const unwrap_reference_to_alt_ref_or_just_type_t<T>& Get(const SelfT& self) noexcept {
        return UnionOperator<Usize, TailT...>::template Get<T>(self);
    }

    template <typename T, typename SelfT,
        std::enable_if_t<is_same_as_first_v<T>>* = nullptr>
    constexpr static unwrap_reference_to_alt_ref_or_just_type_t<T>& Get(SelfT& self) noexcept {
        return self.m_Val.template GetVal<unwrap_reference_to_alt_ref_or_just_type_t<T>>();
    }

    template <typename T, typename SelfT,
        std::enable_if_t<!is_same_as_first_v<T>>* = nullptr>
    constexpr static unwrap_reference_to_alt_ref_or_just_type_t<T>& Get(SelfT& self) noexcept {
        return UnionOperator<Usize, TailT...>::template Get<T>(self);
    }

    template <typename T, typename SelfT,
        std::enable_if_t<is_same_as_first_v<T>>* = nullptr>
    constexpr static unwrap_reference_to_alt_ref_or_just_type_t<T>&& Get(SelfT&& self) noexcept {
        return std::move(self.m_Val.template GetVal<unwrap_reference_to_alt_ref_or_just_type_t<T>>());
    }

    template <typename T, typename SelfT,
        std::enable_if_t<!is_same_as_first_v<T>>* = nullptr>
    constexpr static unwrap_reference_to_alt_ref_or_just_type_t<T>&& Get(SelfT&& self) noexcept {
        return UnionOperator<Usize, TailT...>::template Get<T>(std::move(self));
    }

    template <Usize ACC = 0, typename SelfT, typename ...ArgsT>
    static void ConstructCopy(SelfT& lhs, const UnionCore<ArgsT...>& rhs, const Usize typeIdx) noexcept {

        using T = unwrap_reference_to_alt_ref_or_just_type_t<FirstT>;

        if (ACC == typeIdx) {

            lhs.m_Val.template Construct<T>(rhs.m_Val.template GetVal<T>());
            return;
        }

        UnionOperator<Usize, TailT...>::template ConstructCopy<ACC + 1>(lhs, rhs, typeIdx);
    }

    template <Usize ACC = 0, typename SelfT, typename ...ArgsT>
    static void AssignCopy(SelfT& lhs, const UnionCore<ArgsT...>& rhs, const Usize typeIdx) noexcept {

        using T = unwrap_reference_to_alt_ref_or_just_type_t<FirstT>;

        if (ACC == typeIdx) {

            lhs.m_Val.template Assign<T>(rhs.m_Val.template GetVal<T>());
            return;
        }

        UnionOperator<Usize, TailT...>::template AssignCopy<ACC + 1>(lhs, rhs, typeIdx);
    }

    template <Usize ACC = 0, typename SelfT, typename ...ArgsT>
    static void ConstructMove(SelfT& lhs, UnionCore<ArgsT...>& rhs, const Usize typeIdx) noexcept {

        using T = unwrap_reference_to_alt_ref_or_just_type_t<FirstT>;

        if (ACC == typeIdx) {

            lhs.m_Val.template Construct<T>(std::move(rhs.m_Val.template GetVal<T>()));
            return;
        }

        UnionOperator<Usize, TailT...>::template ConstructMove<ACC + 1>(lhs, rhs, typeIdx);
    }

    template <Usize ACC = 0, typename SelfT, typename ...ArgsT>
    static void AssignMove(SelfT& lhs, UnionCore<ArgsT...>& rhs, const Usize typeIdx) noexcept {

        using T = unwrap_reference_to_alt_ref_or_just_type_t<FirstT>;

        if (ACC == typeIdx) {

            lhs.m_Val.template Assign<T>(std::move(rhs.m_Val.template GetVal<T>()));
            return;
        }

        UnionOperator<Usize, TailT...>::template AssignMove<ACC + 1>(lhs, rhs, typeIdx);
    }

    template <Usize ACC = 0, typename SelfT>
    static void Destroy(SelfT& self, const Usize typeIdx) noexcept {

        using T = unwrap_reference_to_alt_ref_or_just_type_t<FirstT>;

        if (ACC == typeIdx) {

            self.m_Val.template Destroy<T>();
            return;
        }

        return UnionOperator<Usize, TailT...>::template Destroy<ACC + 1>(self, typeIdx);
    }

    template <Usize ACC = 0, typename SelfT, typename ...ArgsT>
    ALT_NODISCARD static bool AssignCloneFrom(SelfT& lhs, const UnionCore<ArgsT...>& rhs, const Usize typeIdx) noexcept {

        using T = unwrap_reference_to_alt_ref_or_just_type_t<FirstT>;

        if (ACC == typeIdx) {

            return lhs.m_Val.template AssignCloneOrMove<T>(rhs.m_Val.template GetVal<T>());
        }

        return UnionOperator<Usize, TailT...>::template AssignCloneFrom<ACC + 1>(lhs, rhs, typeIdx);
    }

    template <Usize ACC = 0, typename SelfT, typename ...ArgsT>
    ALT_NODISCARD static bool ConstructCloneFrom(SelfT& lhs, const UnionCore<ArgsT...>& rhs, const Usize typeIdx) noexcept {

        using T = unwrap_reference_to_alt_ref_or_just_type_t<FirstT>;

        if (ACC == typeIdx) {

            return lhs.m_Val.template ConstructCloneOrMove<T>(rhs.m_Val.template GetVal<T>());
        }

        return UnionOperator<Usize, TailT...>::template ConstructCloneFrom<ACC + 1>(lhs, rhs, typeIdx);
    }
};

template <typename Usize>
struct UnionOperator<Usize> {

    constexpr static Usize INVALID_TYPE_IDX = 0;

    template <typename T>
    constexpr static bool is_same_as_first_v = false;

    template <typename T>
    using is_exist = std::false_type;

    template <typename T>
    constexpr static bool is_exist_v = is_exist<T>::value;

    template <Usize ACC, typename SelfT, typename ...ArgsT>
    static void ConstructCopy(SelfT&, const UnionCore<ArgsT...>&, const Usize) noexcept
    {}

    template <Usize ACC, typename SelfT, typename ...ArgsT>
    static void AssignCopy(SelfT&, const UnionCore<ArgsT...>&, const Usize) noexcept
    {}

    template <Usize ACC, typename SelfT, typename ...ArgsT>
    static void ConstructMove(SelfT&, UnionCore<ArgsT...>&, const Usize) noexcept
    {}

    template <Usize ACC, typename SelfT, typename ...ArgsT>
    static void AssignMove(SelfT&, UnionCore<ArgsT...>&, const Usize) noexcept
    {}

    template <Usize ACC = 0, typename SelfT>
    static void Destroy(SelfT&, const Usize) noexcept
    {}

    template <Usize ACC, typename SelfT, typename ...ArgsT>
    ALT_NODISCARD static bool AssignCloneFrom(SelfT&, const UnionCore<ArgsT...>&, const Usize) noexcept {

        return true;
    }

    template <Usize ACC, typename SelfT, typename ...ArgsT>
    ALT_NODISCARD static bool ConstructCloneFrom(SelfT&, const UnionCore<ArgsT...>&, const Usize) noexcept {

        return true;
    }
};

template <_MaybeUninitDetails::Rule5Trait RULE5_TRAIT, typename SelfT, typename ...ArgsT>
struct UnionStorageDestruct;

template <typename SelfT, typename ...ArgsT>
struct UnionStorageDestruct<_MaybeUninitDetails::Rule5Trait::Deleted, SelfT, ArgsT...>
    : UnionCore<ArgsT...>
{
    UnionStorageDestruct() noexcept = default;

    UnionStorageDestruct(const UnionStorageDestruct&) noexcept = default;
    UnionStorageDestruct& operator=(const UnionStorageDestruct&) noexcept = default;

    UnionStorageDestruct(UnionStorageDestruct&&) noexcept = default;
    UnionStorageDestruct& operator=(UnionStorageDestruct&&) noexcept = default;

    ~UnionStorageDestruct() noexcept = delete;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageDestruct<_MaybeUninitDetails::Rule5Trait::Trivial, SelfT, ArgsT...>
    : UnionCore<ArgsT...> 
{
    UnionStorageDestruct() noexcept = default;

    UnionStorageDestruct(const UnionStorageDestruct&) noexcept = default;
    UnionStorageDestruct& operator=(const UnionStorageDestruct&) noexcept = default;

    UnionStorageDestruct(UnionStorageDestruct&&) noexcept = default;
    UnionStorageDestruct& operator=(UnionStorageDestruct&&) noexcept = default;

    ~UnionStorageDestruct() noexcept = default;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageDestruct<_MaybeUninitDetails::Rule5Trait::NonTrivial, SelfT, ArgsT...>
    : UnionCore<ArgsT...>
{
    UnionStorageDestruct() noexcept = default;

    UnionStorageDestruct(const UnionStorageDestruct&) noexcept = default;
    UnionStorageDestruct& operator=(const UnionStorageDestruct&) noexcept = default;

    UnionStorageDestruct(UnionStorageDestruct&&) noexcept = default;
    UnionStorageDestruct& operator=(UnionStorageDestruct&&) noexcept = default;

    ~UnionStorageDestruct() noexcept {

        static_cast<SelfT*>(this)->Destroy();
    }
};

template <_MaybeUninitDetails::Rule5Trait RULE5_TRAIT, typename ...ArgsT>
struct UnionStorageAssignMove;

template <typename SelfT, typename ...ArgsT>
struct UnionStorageAssignMove<_MaybeUninitDetails::Rule5Trait::Deleted, SelfT, ArgsT...>
    : UnionStorageDestruct<_MaybeUninitDetails::rule5_destructible_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageAssignMove() noexcept = default;

    UnionStorageAssignMove(const UnionStorageAssignMove&) noexcept = default;
    UnionStorageAssignMove& operator=(const UnionStorageAssignMove&) noexcept = default;

    UnionStorageAssignMove(UnionStorageAssignMove&&) noexcept = default;
    UnionStorageAssignMove& operator=(UnionStorageAssignMove&&) noexcept = delete;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageAssignMove<_MaybeUninitDetails::Rule5Trait::Trivial, SelfT, ArgsT...>
    : UnionStorageDestruct<_MaybeUninitDetails::rule5_destructible_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageAssignMove() noexcept = default;

    UnionStorageAssignMove(const UnionStorageAssignMove&) noexcept = default;
    UnionStorageAssignMove& operator=(const UnionStorageAssignMove&) noexcept = default;

    UnionStorageAssignMove(UnionStorageAssignMove&&) noexcept = default;
    UnionStorageAssignMove& operator=(UnionStorageAssignMove&&) noexcept = default;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageAssignMove<_MaybeUninitDetails::Rule5Trait::NonTrivial, SelfT, ArgsT...>
    : UnionStorageDestruct<_MaybeUninitDetails::rule5_destructible_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageAssignMove() noexcept = default;

    UnionStorageAssignMove(const UnionStorageAssignMove&) noexcept = default;
    UnionStorageAssignMove& operator=(const UnionStorageAssignMove&) noexcept = default;

    UnionStorageAssignMove(UnionStorageAssignMove&&) noexcept = default;

    UnionStorageAssignMove& operator=(UnionStorageAssignMove&& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Move, static_cast<SelfT&>(rhs));
        return *this;
    }
};

template <_MaybeUninitDetails::Rule5Trait RULE5_TRAIT, typename ...ArgsT>
struct UnionStorageConstructMove;

template <typename SelfT, typename ...ArgsT>
struct UnionStorageConstructMove<_MaybeUninitDetails::Rule5Trait::Deleted, SelfT, ArgsT...>
    : UnionStorageAssignMove<_MaybeUninitDetails::rule5_move_assignable_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageConstructMove() noexcept = default;

    UnionStorageConstructMove(const UnionStorageConstructMove&) noexcept = default;
    UnionStorageConstructMove& operator=(const UnionStorageConstructMove&) noexcept = default;

    UnionStorageConstructMove(UnionStorageConstructMove&&) noexcept = delete;
    UnionStorageConstructMove& operator=(UnionStorageConstructMove&&) noexcept = default;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageConstructMove<_MaybeUninitDetails::Rule5Trait::Trivial, SelfT, ArgsT...>
    : UnionStorageAssignMove<_MaybeUninitDetails::rule5_move_assignable_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageConstructMove() noexcept = default;

    UnionStorageConstructMove(const UnionStorageConstructMove&) noexcept = default;
    UnionStorageConstructMove& operator=(const UnionStorageConstructMove&) noexcept = default;

    UnionStorageConstructMove(UnionStorageConstructMove&&) noexcept = default;
    UnionStorageConstructMove& operator=(UnionStorageConstructMove&&) noexcept = default;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageConstructMove<_MaybeUninitDetails::Rule5Trait::NonTrivial, SelfT, ArgsT...>
    : UnionStorageAssignMove<_MaybeUninitDetails::rule5_move_assignable_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageConstructMove() noexcept = default;

    UnionStorageConstructMove(const UnionStorageConstructMove&) noexcept = default;
    UnionStorageConstructMove& operator=(const UnionStorageConstructMove&) noexcept = default;

    UnionStorageConstructMove(UnionStorageConstructMove&& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Move, static_cast<SelfT&>(rhs));
    }

    UnionStorageConstructMove& operator=(UnionStorageConstructMove&&) noexcept = default;
};

template <_MaybeUninitDetails::Rule5Trait RULE5_TRAIT, typename ...ArgsT>
struct UnionStorageAssignCopy;

template <typename SelfT, typename ...ArgsT>
struct UnionStorageAssignCopy<_MaybeUninitDetails::Rule5Trait::Deleted, SelfT, ArgsT...>
    : UnionStorageConstructMove<_MaybeUninitDetails::rule5_move_constructible_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageAssignCopy() noexcept = default;

    UnionStorageAssignCopy(const UnionStorageAssignCopy&) noexcept = default;
    UnionStorageAssignCopy& operator=(const UnionStorageAssignCopy&) noexcept = delete;

    UnionStorageAssignCopy(UnionStorageAssignCopy&&) noexcept = default;
    UnionStorageAssignCopy& operator=(UnionStorageAssignCopy&&) noexcept = default;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageAssignCopy<_MaybeUninitDetails::Rule5Trait::Trivial, SelfT, ArgsT...>
    : UnionStorageConstructMove<_MaybeUninitDetails::rule5_move_constructible_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageAssignCopy() noexcept = default;

    UnionStorageAssignCopy(const UnionStorageAssignCopy&) noexcept = default;
    UnionStorageAssignCopy& operator=(const UnionStorageAssignCopy&) noexcept = default;

    UnionStorageAssignCopy(UnionStorageAssignCopy&&) noexcept = default;
    UnionStorageAssignCopy& operator=(UnionStorageAssignCopy&&) noexcept = default;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageAssignCopy<_MaybeUninitDetails::Rule5Trait::NonTrivial, SelfT, ArgsT...>
    : UnionStorageConstructMove<_MaybeUninitDetails::rule5_move_constructible_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageAssignCopy() noexcept = default;

    UnionStorageAssignCopy(const UnionStorageAssignCopy&) noexcept = default;

    UnionStorageAssignCopy& operator=(const UnionStorageAssignCopy& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Copy, static_cast<const SelfT&>(rhs));
        return *this;
    }

    UnionStorageAssignCopy(UnionStorageAssignCopy&&) noexcept = default;
    UnionStorageAssignCopy& operator=(UnionStorageAssignCopy&&) noexcept = default;
};

template <_MaybeUninitDetails::Rule5Trait RULE5_TRAIT, typename ...ArgsT>
struct UnionStorageConstructCopy;

template <typename SelfT, typename ...ArgsT>
struct UnionStorageConstructCopy<_MaybeUninitDetails::Rule5Trait::Deleted, SelfT, ArgsT...>
    : UnionStorageAssignCopy<_MaybeUninitDetails::rule5_copy_assignable_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageConstructCopy() noexcept = default;

    UnionStorageConstructCopy(const UnionStorageConstructCopy&) noexcept = delete;
    UnionStorageConstructCopy& operator=(const UnionStorageConstructCopy&) noexcept = default;

    UnionStorageConstructCopy(UnionStorageConstructCopy&&) noexcept = default;
    UnionStorageConstructCopy& operator=(UnionStorageConstructCopy&&) noexcept = default;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageConstructCopy<_MaybeUninitDetails::Rule5Trait::Trivial, SelfT, ArgsT...>
    : UnionStorageAssignCopy<_MaybeUninitDetails::rule5_copy_assignable_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageConstructCopy() noexcept = default;

    UnionStorageConstructCopy(const UnionStorageConstructCopy&) noexcept = default;
    UnionStorageConstructCopy& operator=(const UnionStorageConstructCopy&) noexcept = default;

    UnionStorageConstructCopy(UnionStorageConstructCopy&&) noexcept = default;
    UnionStorageConstructCopy& operator=(UnionStorageConstructCopy&&) noexcept = default;
};

template <typename SelfT, typename ...ArgsT>
struct UnionStorageConstructCopy<_MaybeUninitDetails::Rule5Trait::NonTrivial, SelfT, ArgsT...>
    : UnionStorageAssignCopy<_MaybeUninitDetails::rule5_copy_assignable_v<ArgsT...>, SelfT, ArgsT...>
{
    UnionStorageConstructCopy() noexcept = default;

    UnionStorageConstructCopy(const UnionStorageConstructCopy& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Copy, static_cast<const SelfT&>(rhs));
    }

    UnionStorageConstructCopy& operator=(const UnionStorageConstructCopy&) noexcept = default;

    UnionStorageConstructCopy(UnionStorageConstructCopy&&) noexcept = default;
    UnionStorageConstructCopy& operator=(UnionStorageConstructCopy&&) noexcept = default;
};

template <typename UnionOperator, typename Usize, typename ...ArgsT>
class UnionStorage
    : public UnionStorageConstructCopy<
        _MaybeUninitDetails::rule5_copy_constructible_v<ArgsT...>,
        UnionStorage<UnionOperator, Usize, ArgsT...>,
        ArgsT...
    >
{
public:

    UnionStorage() noexcept = default;

    UnionStorage(Noval_t) noexcept
    {}

    constexpr static usize MaxInnerValueSizeOf() noexcept {

        return sizeof(UnionCore<ArgsT...>);
    }

    constexpr static usize MaxInnerValueAlignOf() noexcept {

        return alignof(UnionCore<ArgsT...>);
    }

    void Assign(Noval_t) noexcept {

        UnionOperator::Destroy(*this, m_TypeIdx);
        m_TypeIdx = UnionOperator::INVALID_TYPE_IDX;
    }

    usize GetTypeIdx() const noexcept {

        return static_cast<usize>(m_TypeIdx);
    }

    template <usize TYPE_IDX, typename T>
    void Construct(InnerRef_t, const T& val) noexcept {

        Memory::ConstructObject(UnionOperator::template Get<T>(*this), val);
        m_TypeIdx = static_cast<Usize>(TYPE_IDX);
    }

    template <usize TYPE_IDX, typename T>
    void Construct(InnerValue_t, T&& val) noexcept {

        Memory::ConstructObject(UnionOperator::template Get<T>(*this), std::forward<T>(val));
        m_TypeIdx = static_cast<Usize>(TYPE_IDX);
    }

    template <usize TYPE_IDX, typename T>
    void Assign(InnerRef_t, const T& val) noexcept {

        const Usize typeIdx = static_cast<Usize>(TYPE_IDX);
        if (m_TypeIdx == typeIdx) {

            UnionOperator::template Get<T>(*this) = val;
        }
        else {

            UnionOperator::Destroy(*this, m_TypeIdx);

            Memory::ConstructObject(UnionOperator::template Get<T>(*this), val);
            m_TypeIdx = typeIdx;
        }
    }

    template <usize TYPE_IDX, typename T>
    void Assign(InnerValue_t, T&& val) noexcept {

        const Usize typeIdx = static_cast<Usize>(TYPE_IDX);
        if (m_TypeIdx == typeIdx) {

            UnionOperator::template Get<T>(*this) = std::forward<T>(val);
        }
        else {

            UnionOperator::Destroy(*this, m_TypeIdx);

            Memory::ConstructObject(UnionOperator::template Get<T>(*this), std::forward<T>(val));
            m_TypeIdx = typeIdx;
        }
    }

    template <typename UnionOperatorU, typename ...ArgsU>
    void Construct(Rule5Copy_t, const UnionStorage<UnionOperatorU, Usize, ArgsU...>& rhs) noexcept {

        if (rhs.m_TypeIdx == UnionOperator::INVALID_TYPE_IDX) {
            return;
        }

        UnionOperator::ConstructCopy(*this, rhs, rhs.m_TypeIdx);
        m_TypeIdx = rhs.m_TypeIdx;
    }

    template <typename UnionOperatorU, typename ...ArgsU>
    void Assign(Rule5Copy_t, const UnionStorage<UnionOperatorU, Usize, ArgsU...>& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return;
        }

        if (m_TypeIdx == rhs.m_TypeIdx) {

            UnionOperator::AssignCopy(*this, rhs, rhs.m_TypeIdx);
            return;
        }

        UnionOperator::Destroy(*this, m_TypeIdx);

        UnionOperator::ConstructCopy(*this, rhs, rhs.m_TypeIdx);
        m_TypeIdx = rhs.m_TypeIdx;
    }

    template <typename UnionOperatorU, typename ...ArgsU>
    void Construct(Rule5Move_t, UnionStorage<UnionOperatorU, Usize, ArgsU...>& rhs) noexcept {

        if (rhs.m_TypeIdx == UnionOperator::INVALID_TYPE_IDX) {
            return;
        }

        UnionOperator::ConstructMove(*this, rhs, rhs.m_TypeIdx);
        m_TypeIdx = rhs.m_TypeIdx;
    }

    template <typename UnionOperatorU, typename ...ArgsU>
    void Assign(Rule5Move_t, UnionStorage<UnionOperatorU, Usize, ArgsU...>& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return;
        }

        if (m_TypeIdx == rhs.m_TypeIdx) {

            UnionOperator::AssignMove(*this, rhs, rhs.m_TypeIdx);
            return;
        }

        UnionOperator::Destroy(*this, m_TypeIdx);

        UnionOperator::ConstructMove(*this, rhs, rhs.m_TypeIdx);
        m_TypeIdx = rhs.m_TypeIdx;
    }

    void Destroy() noexcept {

        UnionOperator::Destroy(*this, m_TypeIdx);
        m_TypeIdx = UnionOperator::INVALID_TYPE_IDX;
    }

    template <typename UnionOperatorU, typename ...ArgsU>
    bool ConstructCloneFrom(const UnionStorage<UnionOperatorU, Usize, ArgsU...>& rhs) noexcept {

        const bool isOk = UnionOperator::ConstructCloneFrom(*this, rhs, rhs.m_TypeIdx);
        m_TypeIdx = isOk ? rhs.m_TypeIdx : UnionOperator::INVALID_TYPE_IDX;

        return isOk;
    }

    template <typename UnionOperatorU, typename ...ArgsU>
    bool CloneFrom(const UnionStorage<UnionOperatorU, Usize, ArgsU...>& rhs) noexcept {

        if (m_TypeIdx == rhs.m_TypeIdx) {

            return UnionOperator::AssignCloneFrom(*this, rhs, rhs.m_TypeIdx);
        }

        const bool isOk = UnionOperator::ConstructCloneFrom(*this, rhs, rhs.m_TypeIdx);
        m_TypeIdx = isOk ? rhs.m_TypeIdx : UnionOperator::INVALID_TYPE_IDX;

        return isOk;
    }

private:

    template <typename, typename, typename ...>
    friend class UnionStorage;

    Usize m_TypeIdx = UnionOperator::INVALID_TYPE_IDX;
};

} // namespace _UnionDetails
/** \endcond */

/**
 * @brief A type-safe tag union.
 * 
 * @tparam ArgsT The list of types that the Union is permitted to hold.
 * 
 * @example
 * 
 * Alt::Union<s32, f32, Alt::String> un;
 * un = 10;
 * un = Alt::String("Union");
 * 
 * if (Alt::String* s = un.Is<Alt::String>()) {
 *     assert(*s == "Union");
 * }
 * 
 * un = Alt::Noval;
 * assert(un.GetTypeIdx().IsNone());
 */
template <typename ...ArgsT>
class Union {
private:

    constexpr static usize ARGS_COUNT = sizeof...(ArgsT);

    using Usize = _UnionDetails::count_to_type_t<ARGS_COUNT>;

    template <typename T>
    using clean_t =
        _UnionDetails::unwrap_reference_to_alt_ref_or_just_type_t<
            _UnionDetails::unwrap_void_to_noval_or_just_type_t<_UnionDetails::clean_t<T>>>;

    using UnionOperator = _UnionDetails::UnionOperator<Usize, ArgsT...>;
    using Storage = _UnionDetails::UnionStorage<UnionOperator, Usize, clean_t<ArgsT>...>;

    template <typename T>
    using is_exist = typename UnionOperator::template is_exist<T>;

    template <typename T>
    constexpr static bool is_exist_v = UnionOperator::template is_exist_v<T>;

    constexpr static usize INVALID_TYPE_IDX = static_cast<usize>(UnionOperator::INVALID_TYPE_IDX);

    static_assert(!std::is_same_v<Usize, void>, "");
    static_assert(ARGS_COUNT < Num::Desc<Usize>::Max(), "");

public:

    /**
     * @brief Returns the total number of types.
     * 
     * @return The number of types in the template parameter pack.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize GetNumTypes() noexcept {

        return ARGS_COUNT;
    }

    /**
     * @brief Retrieves the compile-time index of a specific type.
     * 
     * @tparam T The type to look up. Must be one of the types in the Union.
     * @return The index of the type.
     */
    template <typename T,
        std::enable_if_t<is_exist_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize GetTypeIdx() noexcept {

        return static_cast<usize>(UnionOperator::template FindTypeIndex<0, T>());
    }

    /**
     * @brief Retrieves the compile-time index of the Noval state.
     * 
     * @return The specialized index representing the Noval state.
     */
    template <typename T,
        std::enable_if_t<!is_exist_v<T> && std::is_same_v<T, Noval_t>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize GetTypeIdx() noexcept {

        return INVALID_TYPE_IDX;
    }

    /**
     * @brief Returns the compile-time sizeof of inner value (without type index).
     * 
     * @return The sizeof of inner value.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize MaxInnerValueSizeOf() noexcept {

        return Storage::MaxInnerValueSizeOf();
    }

    /**
     * @brief Returns the compile-time alignof of inner value (without type index).
     * 
     * @return The alignof of inner value.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize MaxInnerValueAlignOf() noexcept {

        return Storage::MaxInnerValueAlignOf();
    }

    /**
     * @brief Default constructor.
     * 
     * Initializes the union in an Noval state.
     */
    ALT_FORCE_INLINE Union() noexcept = default;

    /**
     * @brief Constructor for the Noval state.
     * 
     * @param val The Noval tag.
     */
    ALT_FORCE_INLINE Union(Noval_t) noexcept
        : m_Storage(Noval)
    {}

    /**
     * @brief Assignment to the Noval state.
     * 
     * Destroys the currently active object and sets the type index to invalid.
     * 
     * @param val The Noval tag.
     * @return A reference to this Union.
     */
    Union& operator=(Noval_t) noexcept {

        m_Storage.Assign(Noval);
        return *this;
    }

    /**
     * @brief Constructor that initializes the union with a value.
     * 
     * @tparam T The type of the value. Must be present in the Union's type pack.
     * @param val The value to store.
     * @example
     * 
     * Alt::Union<s32, f32> un(4.5f);
     */
    template <typename T,
        std::enable_if_t<is_exist_v<T> && std::is_constructible_v<T, T&&>>* = nullptr>
    Union(T&& val) noexcept {

        m_Storage.template Construct<GetTypeIdx<T>()>(_UnionDetails::InnerValue, std::forward<T>(val));
    }

    template <typename T,
        std::enable_if_t<is_exist_v<T> && !is_exist_v<T&> && std::is_constructible_v<T, const T&>>* = nullptr>
    Union(const T& val) noexcept {

        m_Storage.template Construct<GetTypeIdx<T>()>(_UnionDetails::InnerRef, val);
    }

    /**
     * @brief Assignment operator.
     * 
     * @tparam T The type of the value.
     * @param val The value to assign.
     * @return A reference to this Union.
     * @example
     * 
     * Alt::Union<s32, f32> un = Alt::Noval;
     * un = 4.5f;
     */
    template <typename T,
        std::enable_if_t<is_exist_v<T> && std::is_assignable_v<T, T&&>>* = nullptr>
    Union& operator=(T&& val) noexcept {

        m_Storage.template Assign<GetTypeIdx<T>()>(_UnionDetails::InnerValue, std::forward<T>(val));
        return *this;
    }

    template <typename T,
        std::enable_if_t<is_exist_v<T> && !is_exist_v<T&> && std::is_assignable_v<T, const T&>>* = nullptr>
    Union& operator=(const T& val) noexcept {

        m_Storage.template Assign<GetTypeIdx<T>()>(_UnionDetails::InnerRef, val);
        return *this;
    }

    /**
     * @brief Constructs a union from another compatible Union.
     * 
     * @tparam ArgsU The types of the source Union. Must be constructible to ArgsT.
     * @param un The source union to copy or move from.
     * @return A reference to this Union.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Union<Alt::Vector<usize, 0>> un0 = Alt::Vector<usize, 0>::AllocatePush(0, 1) iferr_return;
     * Alt::Union<Alt::Vector<usize, 1>> un1 = std::move(un0);
     * //Alt::Union<Alt::Vector<usize, 1>> un1;
     * //un1 = std::move(un0);
     * 
     * const bool isEmpty0 = un0.Is<Alt::Vector<usize, 0>>()->IsEmpty();
     * const bool isEmpty1 = un1.Is<Alt::Vector<usize, 1>>()->IsEmpty();
     * 
     * assert(isEmpty0);
     * assert(!isEmpty1);
     */
    template <typename ...ArgsU,
        std::enable_if_t<!is_exist_v<Union<ArgsU...>> && std::conjunction_v<std::is_constructible<ArgsT, ArgsU&&>...>>* = nullptr>
    Union(Union<ArgsU...>&& un) noexcept {

        m_Storage.Construct(_UnionDetails::Rule5Move, un.m_Storage);
    }

    template <typename ...ArgsU,
        std::enable_if_t<!is_exist_v<Union<ArgsU...>> && std::conjunction_v<std::is_constructible<ArgsT, const ArgsU&>...>>* = nullptr>
    Union(const Union<ArgsU...>& un) noexcept {

        m_Storage.Construct(_UnionDetails::Rule5Copy, un.m_Storage);
    }

    template <typename ...ArgsU,
        std::enable_if_t<
            !is_exist_v<Union<ArgsU...>> &&
            std::conjunction_v<std::conjunction<std::is_assignable<ArgsT&, ArgsU&&>, std::is_constructible<ArgsT, ArgsU&&>>...>
        >* = nullptr>
    Union& operator=(Union<ArgsU...>&& un) noexcept {

        m_Storage.Assign(_UnionDetails::Rule5Move, un.m_Storage);
        return *this;
    }

    template <typename ...ArgsU,
        std::enable_if_t<
            !is_exist_v<Union<ArgsU...>> &&
            std::conjunction_v<std::conjunction<std::is_assignable<ArgsT&, const ArgsU&>, std::is_constructible<ArgsT, const ArgsU&>>...>
        >* = nullptr>
    Union& operator=(const Union<ArgsU...>& un) noexcept {

        m_Storage.Assign(_UnionDetails::Rule5Copy, un.m_Storage);
        return *this;
    }

    /**
     * Clonable Trait
     * 
     * @brief Clones the current union state into a compatible destination union.
     * 
     * @tparam ArgsU The types of the destination Union. Must be clonable from the current type pack.
     * @param un The destination union where the state will be cloned.
     * @return True if the cloning was successful, false otherwise.
     * 
     * @example
     * 
     * Alt::Union<Alt::Vector<usize>> un0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
     * Alt::Union<Alt::Vector<usize>> un1;
     * un1.CloneFrom(un0) iferr_return;
     * 
     * const bool isEq = *un0.Is<Alt::Vector<usize>>() == *un1.Is<Alt::Vector<usize>>();
     * assert(isEq);
     */
    template <typename ...ArgsU,
        std::enable_if_t<
            std::conjunction_v<
                std::disjunction<
                    TypeTraits::is_only_clonable_to_from_t<ArgsT, const ArgsU&>,
                    std::conjunction<
                        std::is_assignable<ArgsT&, const ArgsU&>,
                        std::is_constructible<ArgsT, const ArgsU&>
                    >
                >...
            >
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const Union<ArgsU...>& rhs) noexcept {

        return m_Storage.CloneFrom(rhs.m_Storage);
    }

    template <typename ...ArgsU,
        std::enable_if_t<
            std::conjunction_v<
                std::disjunction<
                    TypeTraits::is_only_clonable_to_from_t<ArgsT, const ArgsU&>,
                    std::is_constructible<ArgsT, const ArgsU&>
                >...
            >
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const Union<ArgsU...>& rhs) noexcept {

        return m_Storage.ConstructCloneFrom(rhs.m_Storage);
    }

    ALT_NODISCARD Alt::Optional<Union> Clone() const noexcept {

        struct Object {

            ~Object() noexcept {

                if (!isValid) {
                    return;
                }

                Union* pObject = reinterpret_cast<Union*>(memory);
                Memory::DestroyObject(pObject);

                isValid = false;
            }

            alignas(Union) u8 memory[sizeof(Union)];
            bool isValid = false;
        };

        Object object{};
        Union* pObject = reinterpret_cast<Union*>(object.memory);

        const bool isOk = pObject->CloneConstructFrom(*this);
        object.isValid = isOk;

        if (!isOk) {
            return Nullopt;
        }

        return MakeOptional(std::move(*pObject));
    }

    /**
     * @brief Retrieves the index of the currently active type.
     * 
     * @return An `Optional` containing the index, or `Nullopt` if Noval.
     * @example
     * 
     * Alt::Union<s32, f32> un = 4;
     * assert(un.GetTypeIdx().IsPopulated());
     * assert(*un.GetTypeIdx(), Alt::Union<s32, f32>::GetTypeIdx<s32>());
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> GetTypeIdx() const noexcept {

        return m_Storage.GetTypeIdx() != INVALID_TYPE_IDX
            ? MakeOptional(m_Storage.GetTypeIdx())
            : Nullopt;
    }

    /**
     * @brief Checks if the union currently contains the specified type.
     * 
     * @tparam T The type to check.
     * @return A pointer to the underlying value if the type matches, otherwise `nullptr`.
     * @example
     * 
     * Alt::Union<s32, f32&> un = 4;
     * 
     * if (s32* pVal = un.Is<s32>()) {
     *     assert(*pVal == 4);
     * }
     * 
     * f32 val = 4.5f;
     * un = val;
     * 
     * if (f32* pVal = un.Is<f32&>()) {
     *     assert(*pVal == val);
     * }
     */
    template <typename T,
        std::enable_if_t<is_exist_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE std::remove_reference_t<T>* Is() noexcept {

        using TypeValue = std::remove_reference_t<T>;

        if (m_Storage.GetTypeIdx() == GetTypeIdx<T>()) {

            T& val = UnionOperator::template Get<T>(m_Storage);
            return static_cast<TypeValue*>(&val);
        }

        return nullptr;
    }

    template <typename T,
        std::enable_if_t<is_exist_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const std::remove_reference_t<T>* Is() const noexcept {

        using TypeValue = std::remove_reference_t<T>;

        if (m_Storage.GetTypeIdx() == GetTypeIdx<T>()) {

            const T& val = UnionOperator::template Get<T>(m_Storage);
            return static_cast<const TypeValue*>(&val);
        }

        return nullptr;
    }

    /**
     * @brief Returns the underlying index of the active type.
     * 
     * @return The underlying index of actual type.
     * @example
     * 
     * Alt::Union<s32, f32> un;
     * assert(un.GetUnderlyingTypeIdx(), Alt::Union<s32, f32>::GetTypeIdx<Alt::Noval_t>())
     * 
     * un = 4.5f;
     * assert(un.Is<f32>())
     * assert(un.GetUnderlyingTypeIdx(), Alt::Union<s32, f32>::GetTypeIdx<f32>())
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetUnderlyingTypeIdx() const noexcept {

        return m_Storage.GetTypeIdx();
    }

private:

    template <typename...>
    friend class Union;

    Storage m_Storage;
};

} // namespace Alt

#endif // __ALT_STD_UNION__