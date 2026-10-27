#pragma once

#ifndef __ALT_STD_MAYBE_UNINIT__
#define __ALT_STD_MAYBE_UNINIT__

#include <new>

#include "../TypeTraits.h"

namespace Alt {

/**
 * @brief Represents a void type.
 * @details Used internaly, adds ability to return `void` and `void&`. Also 
 * marker for `MaybeUninit` to skip implementation code related to `Unit_t`.
 */
struct Unit_t {};

namespace TypeTraits {

template <typename T>
using unwrap_void_to_unit_or_just_type_t = std::conditional_t<std::is_void_v<T>, Unit_t, T>;

} // namespace TypeTraits

/**
 * @brief A data storage for uninitialized memory of type T.
 * @details This class allows to control uninitialized memory for an objects without immediately 
 * invoking its constructor. The object's lifetime must be managed manually via `Construct` and `Destroy`.
 * @tparam T The type of the object held in the uninitialized memory.
 * @example
 * 
 * Alt::MaybeUninit<usize> opt;
 * opt.Construct(42);
 * 
 * assert(opt.GetVal() == 42);
 * opt.Destroy();
 * 
 * {
 *     Alt::MaybeUninit<Alt::Vector<usize>> opt;
 *     opt.Construct(Alt::Vector<usize>::Allocate().GetValueOrDefault());
 * 
 *     // opt.Destroy();
 * }
 * // Leak!
 */
template <typename T>
class MaybeUninit;

/**
 * @brief A data storage that allows to hold one of type passed in template like union.
 * @details This union-like structure allocates enough space to hold the largest type in the 
 * template parameter pack and aligns to the largest alignment requirement. It provides 
 * manual lifetime management for the active type.
 * @tparam ArgsT... A variadic list of possible types the container can hold.
 * @example
 * 
 * Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;
 * 
 * opt.Construct<Alt::String>("MaybeUninit"_salloc);
 * assert(storage.GetVal<Alt::String>() == "MaybeUninit");
 * 
 * storage.Destroy<Alt::String>();
 * 
 * storage.Construct<Alt::Vector<usize>>(Alt::Vector<usize>::Allocate(2, 4).GetValueOrDefault());
 * assert(storage.GetVal<Alt::Vector<usize>>()[0] == 4);
 * 
 * storage.Destroy<Alt::Vector<usize>>();
 */
template <typename ...ArgsT>
class MaybeUninitOneOf;

/** \cond */
namespace _MaybeUninitDetails {

template <typename T>
using clean_t = TypeTraits::clean_t<T>;

} // namespace _MaybeUninitDetails
/** \endcond */

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<MaybeUninit<T>>
    : is_trivially_reallocatable_t<T>
{};

template <typename ...ArgsT>
struct __is_trivially_reallocatable__<MaybeUninitOneOf<ArgsT...>>
    : std::conjunction<is_trivially_reallocatable_t<ArgsT>...>
{};

template <typename T>
using unwrap_void_to_unit_or_just_type_t = std::conditional_t<
    std::is_void_v<T>,
    Unit_t,
    T
>;

template <typename T>
constexpr static bool is_unit_v = std::is_same_v<_MaybeUninitDetails::clean_t<T>, Unit_t>;

} // namespace TypeTraits

/** \cond */
namespace _MaybeUninitDetails {

/*
* When using 'MaybeUninit', we need to specify 'Rule5' correctly.
* Use the following templates as a reference for implementation.
* 
template <typename T>
class TemplateStorageCore {
protected:

    T m_Val;
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_destructible_v<T>>
struct TemplateStorageDestruct;

template <typename SelfT, typename T>
struct TemplateStorageDestruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted> : TemplateStorageCore<SelfT, T> {

    TemplateStorageDestruct() noexcept = default;

    TemplateStorageDestruct(const TemplateStorageDestruct&) noexcept = default;
    TemplateStorageDestruct& operator=(const TemplateStorageDestruct&) noexcept = default;

    TemplateStorageDestruct(TemplateStorageDestruct&&) noexcept = default;
    TemplateStorageDestruct& operator=(TemplateStorageDestruct&&) noexcept = default;

    ~TemplateStorageDestruct() noexcept = delete;
};

template <typename SelfT, typename T>
struct TemplateStorageDestruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial> : TemplateStorageCore<SelfT, T> {

    TemplateStorageDestruct() noexcept = default;

    TemplateStorageDestruct(const TemplateStorageDestruct&) noexcept = default;
    TemplateStorageDestruct& operator=(const TemplateStorageDestruct&) noexcept = default;

    TemplateStorageDestruct(TemplateStorageDestruct&&) noexcept = default;
    TemplateStorageDestruct& operator=(TemplateStorageDestruct&&) noexcept = default;

    ~TemplateStorageDestruct() noexcept = default;
};

template <typename SelfT, typename T>
struct TemplateStorageDestruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial> : TemplateStorageCore<SelfT, T> {

    TemplateStorageDestruct() noexcept = default;

    TemplateStorageDestruct(const TemplateStorageDestruct&) noexcept = default;
    TemplateStorageDestruct& operator=(const TemplateStorageDestruct&) noexcept = default;

    TemplateStorageDestruct(TemplateStorageDestruct&&) noexcept = default;
    TemplateStorageDestruct& operator=(TemplateStorageDestruct&&) noexcept = default;

    ~TemplateStorageDestruct() noexcept {

        static_cast<SelfT*>(this)->Destroy();
    }
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_assignable_v<T>>
struct TemplateStorageMoveAssign;

template <typename SelfT, typename T>
struct TemplateStorageMoveAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted> : TemplateStorageDestruct<SelfT, T> {

    TemplateStorageMoveAssign() noexcept = default;

    TemplateStorageMoveAssign(const TemplateStorageMoveAssign&) noexcept = default;
    TemplateStorageMoveAssign& operator=(const TemplateStorageMoveAssign&) noexcept = default;

    TemplateStorageMoveAssign(TemplateStorageMoveAssign&&) noexcept = default;
    TemplateStorageMoveAssign& operator=(TemplateStorageMoveAssign&&) noexcept = delete;
};

template <typename SelfT, typename T>
struct TemplateStorageMoveAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial> : TemplateStorageDestruct<SelfT, T> {

    TemplateStorageMoveAssign() noexcept = default;

    TemplateStorageMoveAssign(const TemplateStorageMoveAssign&) noexcept = default;
    TemplateStorageMoveAssign& operator=(const TemplateStorageMoveAssign&) noexcept = default;

    TemplateStorageMoveAssign(TemplateStorageMoveAssign&&) noexcept = default;
    TemplateStorageMoveAssign& operator=(TemplateStorageMoveAssign&&) noexcept = default;
};

template <typename SelfT, typename T>
struct TemplateStorageMoveAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial> : TemplateStorageDestruct<SelfT, T> {

    TemplateStorageMoveAssign() noexcept = default;

    TemplateStorageMoveAssign(const TemplateStorageMoveAssign&) noexcept = default;
    TemplateStorageMoveAssign& operator=(const TemplateStorageMoveAssign&) noexcept = default;

    TemplateStorageMoveAssign(TemplateStorageMoveAssign&&) noexcept = default;

    TemplateStorageMoveAssign& operator=(TemplateStorageMoveAssign&& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Move, static_cast<SelfT&>(rhs));
        return *this;
    }
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_constructible_v<T>>
struct TemplateStorageMoveConstruct;

template <typename SelfT, typename T>
struct TemplateStorageMoveConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted> : TemplateStorageMoveAssign<SelfT, T> {

    TemplateStorageMoveConstruct() noexcept = default;

    TemplateStorageMoveConstruct(const TemplateStorageMoveConstruct&) noexcept = default;
    TemplateStorageMoveConstruct& operator=(const TemplateStorageMoveConstruct&) noexcept = default;

    TemplateStorageMoveConstruct(TemplateStorageMoveConstruct&&) noexcept = delete;
    TemplateStorageMoveConstruct& operator=(TemplateStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T>
struct TemplateStorageMoveConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial> : TemplateStorageMoveAssign<SelfT, T> {

    TemplateStorageMoveConstruct() noexcept = default;

    TemplateStorageMoveConstruct(const TemplateStorageMoveConstruct&) noexcept = default;
    TemplateStorageMoveConstruct& operator=(const TemplateStorageMoveConstruct&) noexcept = default;

    TemplateStorageMoveConstruct(TemplateStorageMoveConstruct&&) noexcept = default;
    TemplateStorageMoveConstruct& operator=(TemplateStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T>
struct TemplateStorageMoveConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial> : TemplateStorageMoveAssign<SelfT, T> {

    TemplateStorageMoveConstruct() noexcept = default;

    TemplateStorageMoveConstruct(const TemplateStorageMoveConstruct&) noexcept = default;
    TemplateStorageMoveConstruct& operator=(const TemplateStorageMoveConstruct&) noexcept = default;

    TemplateStorageMoveConstruct(TemplateStorageMoveConstruct&& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Move, static_cast<SelfT&>(rhs));
    }

    TemplateStorageMoveConstruct& operator=(TemplateStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_assignable_v<T>>
struct TemplateStorageCopyAssign;

template <typename SelfT, typename T>
struct TemplateStorageCopyAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted>
    : TemplateStorageMoveConstruct<T>
{
    TemplateStorageCopyAssign() noexcept = default;

    TemplateStorageCopyAssign(const TemplateStorageCopyAssign&) noexcept = default;
    TemplateStorageCopyAssign& operator=(const TemplateStorageCopyAssign&) noexcept = delete;

    TemplateStorageCopyAssign(TemplateStorageCopyAssign&&) noexcept = default;
    TemplateStorageCopyAssign& operator=(TemplateStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T>
struct TemplateStorageCopyAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial> : TemplateStorageMoveConstruct<SelfT, T> {

    TemplateStorageCopyAssign() noexcept = default;

    TemplateStorageCopyAssign(const TemplateStorageCopyAssign&) noexcept = default;
    TemplateStorageCopyAssign& operator=(const TemplateStorageCopyAssign&) noexcept = default;

    TemplateStorageCopyAssign(TemplateStorageCopyAssign&&) noexcept = default;
    TemplateStorageCopyAssign& operator=(TemplateStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T>
struct TemplateStorageCopyAssign<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial> : TemplateStorageMoveConstruct<SelfT, T> {

    TemplateStorageCopyAssign() noexcept = default;

    TemplateStorageCopyAssign(const TemplateStorageCopyAssign&) noexcept = default;

    TemplateStorageCopyAssign& operator=(const TemplateStorageCopyAssign& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Copy, static_cast<const SelfT&>(rhs));
        return *this;
    }

    TemplateStorageCopyAssign(TemplateStorageCopyAssign&&) noexcept = default;
    TemplateStorageCopyAssign& operator=(TemplateStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_constructible_v<T>>
struct TemplateStorageCopyConstruct;

template <typename SelfT, typename T>
struct TemplateStorageCopyConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Deleted> : TemplateStorageCopyAssign<SelfT, T> {

    TemplateStorageCopyConstruct() noexcept = default;

    TemplateStorageCopyConstruct(const TemplateStorageCopyConstruct&) noexcept = delete;
    TemplateStorageCopyConstruct& operator=(const TemplateStorageCopyConstruct&) noexcept = default;

    TemplateStorageCopyConstruct(TemplateStorageCopyConstruct&&) noexcept = default;
    TemplateStorageCopyConstruct& operator=(TemplateStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T>
struct TemplateStorageCopyConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::Trivial> : TemplateStorageCopyAssign<SelfT, T> {

    TemplateStorageCopyConstruct() noexcept = default;

    TemplateStorageCopyConstruct(const TemplateStorageCopyConstruct&) noexcept = default;
    TemplateStorageCopyConstruct& operator=(const TemplateStorageCopyConstruct&) noexcept = default;

    TemplateStorageCopyConstruct(TemplateStorageCopyConstruct&&) noexcept = default;
    TemplateStorageCopyConstruct& operator=(TemplateStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T>
struct TemplateStorageCopyConstruct<SelfT, T, _MaybeUninitDetails::Rule5Trait::NonTrivial> : TemplateStorageCopyAssign<SelfT, T> {

    TemplateStorageCopyConstruct() noexcept = default;

    TemplateStorageCopyConstruct(const TemplateStorageCopyConstruct& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Copy, static_cast<const SelfT&>(rhs));
    }

    TemplateStorageCopyConstruct& operator=(const TemplateStorageCopyConstruct&) noexcept = default;

    TemplateStorageCopyConstruct(TemplateStorageCopyConstruct&&) noexcept = default;
    TemplateStorageCopyConstruct& operator=(TemplateStorageCopyConstruct&&) noexcept = default;
};
*/
enum class Rule5Trait { Deleted, Trivial, NonTrivial };

template <Rule5Trait TRAIT>
using Rule5 = std::integral_constant<Rule5Trait, TRAIT>;

template <typename ...ArgsT>
constexpr static Rule5Trait rule5_constructible_v =
    std::conditional_t<
        std::disjunction_v<std::negation<std::is_default_constructible<ArgsT>>...>,
        Rule5<Rule5Trait::Deleted>,
        std::conditional_t<
            std::conjunction_v<std::is_trivially_default_constructible<ArgsT>...>,
            Rule5<Rule5Trait::Trivial>,
            Rule5<Rule5Trait::NonTrivial>
        >
    >::value;

template <typename ...ArgsT>
constexpr static Rule5Trait rule5_destructible_v =
    std::conditional_t<
        std::disjunction_v<std::negation<std::is_destructible<ArgsT>>...>,
        Rule5<Rule5Trait::Deleted>,
        std::conditional_t<
            std::conjunction_v<std::is_trivially_destructible<ArgsT>...>,
            Rule5<Rule5Trait::Trivial>,
            Rule5<Rule5Trait::NonTrivial>
        >
    >::value;

template <typename ...ArgsT>
constexpr static Rule5Trait rule5_move_constructible_v =
    std::conditional_t<
        std::disjunction_v<std::negation<std::is_move_constructible<ArgsT>>...>,
        Rule5<Rule5Trait::Deleted>,
        std::conditional_t<
            std::conjunction_v<std::is_trivially_move_constructible<ArgsT>...>,
            Rule5<Rule5Trait::Trivial>,
            Rule5<Rule5Trait::NonTrivial>
        >
    >::value;

template <typename ...ArgsT>
constexpr static Rule5Trait rule5_move_assignable_v =
    std::conditional_t<

        std::disjunction_v<std::negation<std::is_move_assignable<ArgsT>>...> ||
        std::disjunction_v<std::negation<std::is_move_constructible<ArgsT>>...> ||
        std::disjunction_v<std::negation<std::is_destructible<ArgsT>>...>,

        Rule5<Rule5Trait::Deleted>,
        std::conditional_t<

            std::conjunction_v<std::is_trivially_move_assignable<ArgsT>...> &&
            std::conjunction_v<std::is_trivially_move_constructible<ArgsT>...> &&
            std::conjunction_v<std::is_trivially_destructible<ArgsT>...>,

            Rule5<Rule5Trait::Trivial>,
            Rule5<Rule5Trait::NonTrivial>
        >
    >::value;

template <typename ...ArgsT>
constexpr static Rule5Trait rule5_copy_constructible_v =
    std::conditional_t<
        std::disjunction_v<std::negation<std::is_copy_constructible<ArgsT>>...>,
        Rule5<Rule5Trait::Deleted>,
        std::conditional_t<
            std::conjunction_v<std::is_trivially_copy_constructible<ArgsT>...>,
            Rule5<Rule5Trait::Trivial>,
            Rule5<Rule5Trait::NonTrivial>
        >
    >::value;

template <typename ...ArgsT>
constexpr static Rule5Trait rule5_copy_assignable_v =
    std::conditional_t<

        std::disjunction_v<std::negation<std::is_copy_assignable<ArgsT>>...> ||
        std::disjunction_v<std::negation<std::is_copy_constructible<ArgsT>>...> ||
        std::disjunction_v<std::negation<std::is_destructible<ArgsT>>...>,

        Rule5<Rule5Trait::Deleted>,
        std::conditional_t<

            std::conjunction_v<std::is_trivially_copy_assignable<ArgsT>...> &&
            std::conjunction_v<std::is_trivially_copy_constructible<ArgsT>...> &&
            std::conjunction_v<std::is_trivially_destructible<ArgsT>...>,

            Rule5<Rule5Trait::Trivial>,
            Rule5<Rule5Trait::NonTrivial>
        >
    >::value;

template <typename T>
using unwrap_void_to_unit_or_just_type_t = TypeTraits::unwrap_void_to_unit_or_just_type_t<T>;

template <typename T>
constexpr static bool is_unit_v = TypeTraits::is_unit_v<T>;

template <typename T, typename U>
constexpr inline T Max(const T x, const U y_) noexcept {

    const T y{ static_cast<T>(y_) };
    return x > y ? x : y;
}

template <typename T>
struct align_of {
    constexpr static usize value = alignof(T);
};

template <typename T>
constexpr static usize align_of_v = align_of<T>::value;

template <typename T>
struct size_of {
    constexpr static usize value = sizeof(T);
};

template <typename T>
constexpr static usize size_of_v = size_of<T>::value;

template <typename ...ArgsT>
struct max_align_of_variadic;

template <typename HeadT, typename ...TailT>
struct max_align_of_variadic<HeadT, TailT...> {
    constexpr static usize value = Max(align_of_v<HeadT>, max_align_of_variadic<TailT...>::value);
};

template <typename T>
struct max_align_of_variadic<T> {
    constexpr static usize value = align_of_v<T>;
};

template <typename ...ArgsT>
constexpr static usize max_align_of_variadic_v = max_align_of_variadic<ArgsT...>::value;

template <typename ...ArgsT>
struct max_size_of_variadic;

template <typename HeadT, typename ...TailT>
struct max_size_of_variadic<HeadT, TailT...> {
    constexpr static usize value = Max(size_of_v<HeadT>, max_size_of_variadic<TailT...>::value);
};

template <typename T>
struct max_size_of_variadic<T> {
    constexpr static usize value = size_of_v<T>;
};

template <typename ...ArgsT>
constexpr static usize max_size_of_variadic_v = max_size_of_variadic<ArgsT...>::value;

template <Rule5Trait CONSTRUCT_TRAIT, Rule5Trait DESTRUCT_TRAIT, typename ...ArgsT>
union __MaybeUninitOneOfMarker;

template <Rule5Trait CONSTRUCT_TRAIT, Rule5Trait DESTRUCT_TRAIT>
union __MaybeUninitOneOfMarker<CONSTRUCT_TRAIT, DESTRUCT_TRAIT>
{};

template <typename HeadT, typename ...TailT>
union __MaybeUninitOneOfMarker<Rule5Trait::Deleted, Rule5Trait::Deleted, HeadT, TailT...> {

    __MaybeUninitOneOfMarker() noexcept
    {}

    ~__MaybeUninitOneOfMarker() noexcept = delete;

    union {

        HeadT __marker0;
        __MaybeUninitOneOfMarker<Rule5Trait::Deleted, Rule5Trait::Deleted, TailT...> __marker1;
    };
};

template <typename HeadT, typename ...TailT>
union __MaybeUninitOneOfMarker<Rule5Trait::Deleted, Rule5Trait::Trivial, HeadT, TailT...> {

    __MaybeUninitOneOfMarker() noexcept
    {}

    ~__MaybeUninitOneOfMarker() noexcept = default;

    union {

        HeadT __marker0;
        __MaybeUninitOneOfMarker<Rule5Trait::Deleted, Rule5Trait::Trivial, TailT...> __marker1;
    };
};

template <typename HeadT, typename ...TailT>
union __MaybeUninitOneOfMarker<Rule5Trait::Deleted, Rule5Trait::NonTrivial, HeadT, TailT...> {

    __MaybeUninitOneOfMarker() noexcept
    {}

    ~__MaybeUninitOneOfMarker() noexcept
    {}

    union {

        HeadT __marker0;
        __MaybeUninitOneOfMarker<Rule5Trait::Deleted, Rule5Trait::NonTrivial, TailT...> __marker1;
    };
};

template <typename HeadT, typename ...TailT>
union __MaybeUninitOneOfMarker<Rule5Trait::Trivial, Rule5Trait::Deleted, HeadT, TailT...> {

    __MaybeUninitOneOfMarker() noexcept = default;

    ~__MaybeUninitOneOfMarker() noexcept = delete;

    union {

        HeadT __marker0;
        __MaybeUninitOneOfMarker<Rule5Trait::Trivial, Rule5Trait::Deleted, TailT...> __marker1;
    };
};

template <typename HeadT, typename ...TailT>
union __MaybeUninitOneOfMarker<Rule5Trait::Trivial, Rule5Trait::Trivial, HeadT, TailT...> {

    __MaybeUninitOneOfMarker() noexcept = default;

    ~__MaybeUninitOneOfMarker() noexcept = default;

    union {

        HeadT __marker0;
        __MaybeUninitOneOfMarker<Rule5Trait::Trivial, Rule5Trait::Trivial, TailT...> __marker1;
    };
};

template <typename HeadT, typename ...TailT>
union __MaybeUninitOneOfMarker<Rule5Trait::Trivial, Rule5Trait::NonTrivial, HeadT, TailT...> {

    __MaybeUninitOneOfMarker() noexcept = default;

    ~__MaybeUninitOneOfMarker() noexcept
    {}

    union {

        HeadT __marker0;
        __MaybeUninitOneOfMarker<Rule5Trait::Trivial, Rule5Trait::NonTrivial, TailT...> __marker1;
    };
};

template <typename HeadT, typename ...TailT>
union __MaybeUninitOneOfMarker<Rule5Trait::NonTrivial, Rule5Trait::Deleted, HeadT, TailT...> {

    __MaybeUninitOneOfMarker() noexcept
    {}

    ~__MaybeUninitOneOfMarker() noexcept = delete;

    union {

        HeadT __marker0;
        __MaybeUninitOneOfMarker<Rule5Trait::NonTrivial, Rule5Trait::Deleted, TailT...> __marker1;
    };
};

template <typename HeadT, typename ...TailT>
union __MaybeUninitOneOfMarker<Rule5Trait::NonTrivial, Rule5Trait::Trivial, HeadT, TailT...> {

    __MaybeUninitOneOfMarker() noexcept
    {}

    ~__MaybeUninitOneOfMarker() noexcept = default;

    union {

        HeadT __marker0;
        __MaybeUninitOneOfMarker<Rule5Trait::NonTrivial, Rule5Trait::Trivial, TailT...> __marker1;
    };
};

template <typename HeadT, typename ...TailT>
union __MaybeUninitOneOfMarker<Rule5Trait::NonTrivial, Rule5Trait::NonTrivial, HeadT, TailT...> {

    __MaybeUninitOneOfMarker() noexcept
    {}

    ~__MaybeUninitOneOfMarker() noexcept
    {}

    union {

        HeadT __marker0;
        __MaybeUninitOneOfMarker<Rule5Trait::NonTrivial, Rule5Trait::NonTrivial, TailT...> __marker1;
    };
};

template <Rule5Trait CONSTRUCT_TRAIT, Rule5Trait DESTRUCT_TRAIT, typename ...ArgsT>
union MaybeUninitOneOf;

template <typename ...ArgsT>
union MaybeUninitOneOf<Rule5Trait::Deleted, Rule5Trait::Deleted, ArgsT...> {

    constexpr static usize size_of = _MaybeUninitDetails::max_size_of_variadic_v<ArgsT...>;
    constexpr static usize align_of = _MaybeUninitDetails::max_align_of_variadic_v<ArgsT...>;

    MaybeUninitOneOf() noexcept
    {}

    ~MaybeUninitOneOf() noexcept = delete;

    union {

        alignas(align_of) u8 m_Memory[sizeof(size_of)];
        __MaybeUninitOneOfMarker<Rule5Trait::Deleted, Rule5Trait::Deleted, ArgsT...> __marker;
    };
};

template <typename ...ArgsT>
union MaybeUninitOneOf<Rule5Trait::Deleted, Rule5Trait::Trivial, ArgsT...> {

    constexpr static usize size_of = _MaybeUninitDetails::max_size_of_variadic_v<ArgsT...>;
    constexpr static usize align_of = _MaybeUninitDetails::max_align_of_variadic_v<ArgsT...>;

    MaybeUninitOneOf() noexcept
    {}

    ~MaybeUninitOneOf() noexcept = default;

    union {

        alignas(align_of) u8 m_Memory[sizeof(size_of)];
        __MaybeUninitOneOfMarker<Rule5Trait::Deleted, Rule5Trait::Trivial, ArgsT...> __marker;
    };
};

template <typename ...ArgsT>
union MaybeUninitOneOf<Rule5Trait::Deleted, Rule5Trait::NonTrivial, ArgsT...> {

    constexpr static usize size_of = _MaybeUninitDetails::max_size_of_variadic_v<ArgsT...>;
    constexpr static usize align_of = _MaybeUninitDetails::max_align_of_variadic_v<ArgsT...>;

    MaybeUninitOneOf() noexcept
    {}

    ~MaybeUninitOneOf() noexcept
    {}

    union {

        alignas(align_of) u8 m_Memory[sizeof(size_of)];
        __MaybeUninitOneOfMarker<Rule5Trait::Deleted, Rule5Trait::NonTrivial, ArgsT...> __marker;
    };
};

template <typename ...ArgsT>
union MaybeUninitOneOf<Rule5Trait::Trivial, Rule5Trait::Deleted, ArgsT...> {

    constexpr static usize size_of = _MaybeUninitDetails::max_size_of_variadic_v<ArgsT...>;
    constexpr static usize align_of = _MaybeUninitDetails::max_align_of_variadic_v<ArgsT...>;

    MaybeUninitOneOf() noexcept = default;

    ~MaybeUninitOneOf() noexcept = delete;

    union {

        alignas(align_of) u8 m_Memory[sizeof(size_of)];
        __MaybeUninitOneOfMarker<Rule5Trait::Trivial, Rule5Trait::Deleted, ArgsT...> __marker;
    };
};

template <typename ...ArgsT>
union MaybeUninitOneOf<Rule5Trait::Trivial, Rule5Trait::Trivial, ArgsT...> {

    constexpr static usize size_of = _MaybeUninitDetails::max_size_of_variadic_v<ArgsT...>;
    constexpr static usize align_of = _MaybeUninitDetails::max_align_of_variadic_v<ArgsT...>;

    MaybeUninitOneOf() noexcept = default;

    ~MaybeUninitOneOf() noexcept = default;

    union {

        alignas(align_of) u8 m_Memory[sizeof(size_of)];
        __MaybeUninitOneOfMarker<Rule5Trait::Trivial, Rule5Trait::Trivial, ArgsT...> __marker;
    };
};

template <typename ...ArgsT>
union MaybeUninitOneOf<Rule5Trait::Trivial, Rule5Trait::NonTrivial, ArgsT...> {

    constexpr static usize size_of = _MaybeUninitDetails::max_size_of_variadic_v<ArgsT...>;
    constexpr static usize align_of = _MaybeUninitDetails::max_align_of_variadic_v<ArgsT...>;

    MaybeUninitOneOf() noexcept = default;

    ~MaybeUninitOneOf() noexcept
    {}

    union {

        alignas(align_of) u8 m_Memory[sizeof(size_of)];
        __MaybeUninitOneOfMarker<Rule5Trait::Trivial, Rule5Trait::NonTrivial, ArgsT...> __marker;
    };
};

template <typename ...ArgsT>
union MaybeUninitOneOf<Rule5Trait::NonTrivial, Rule5Trait::Deleted, ArgsT...> {

    constexpr static usize size_of = _MaybeUninitDetails::max_size_of_variadic_v<ArgsT...>;
    constexpr static usize align_of = _MaybeUninitDetails::max_align_of_variadic_v<ArgsT...>;

    MaybeUninitOneOf() noexcept
    {}

    ~MaybeUninitOneOf() noexcept = delete;

    union {

        alignas(align_of) u8 m_Memory[sizeof(size_of)];
        __MaybeUninitOneOfMarker<Rule5Trait::NonTrivial, Rule5Trait::Deleted, ArgsT...> __marker;
    };
};

template <typename ...ArgsT>
union MaybeUninitOneOf<Rule5Trait::NonTrivial, Rule5Trait::Trivial, ArgsT...> {

    constexpr static usize size_of = _MaybeUninitDetails::max_size_of_variadic_v<ArgsT...>;
    constexpr static usize align_of = _MaybeUninitDetails::max_align_of_variadic_v<ArgsT...>;

    MaybeUninitOneOf() noexcept
    {}

    ~MaybeUninitOneOf() noexcept = default;

    union {

        alignas(align_of) u8 m_Memory[sizeof(size_of)];
        __MaybeUninitOneOfMarker<Rule5Trait::NonTrivial, Rule5Trait::Trivial, ArgsT...> __marker;
    };
};

template <typename ...ArgsT>
union MaybeUninitOneOf<Rule5Trait::NonTrivial, Rule5Trait::NonTrivial, ArgsT...> {

    constexpr static usize size_of = _MaybeUninitDetails::max_size_of_variadic_v<ArgsT...>;
    constexpr static usize align_of = _MaybeUninitDetails::max_align_of_variadic_v<ArgsT...>;

    MaybeUninitOneOf() noexcept
    {}

    ~MaybeUninitOneOf() noexcept
    {}

    union {

        alignas(align_of) u8 m_Memory[sizeof(size_of)];
        __MaybeUninitOneOfMarker<Rule5Trait::NonTrivial, Rule5Trait::NonTrivial, ArgsT...> __marker;
    };
};

} // namespace _MaybeUninitDetails
/** \endcond */

/**
 * @brief A data storage for uninitialized memory of type T.
 * @details This class allows to control uninitialized memory for an objects without immediately 
 * invoking its constructor. The object's lifetime must be managed manually via `Construct` and `Destroy`.
 * @tparam T The type of the object held in the uninitialized memory.
 * @example
 * 
 * Alt::MaybeUninit<usize> opt;
 * opt.Construct(42);
 * 
 * assert(opt.GetVal() == 42);
 * opt.Destroy();
 * 
 * {
 *     Alt::MaybeUninit<Alt::Vector<usize>> opt;
 *     opt.Construct(Alt::Vector<usize>::Allocate().GetValueOrDefault());
 * 
 *     // opt.Destroy();
 * }
 * // Leak!
 */
template <typename T_>
class MaybeUninit {
private:

    template <typename T>
    using clean_t = _MaybeUninitDetails::unwrap_void_to_unit_or_just_type_t<_MaybeUninitDetails::clean_t<T>>;

    using T = clean_t<T_>;

    using Storage = _MaybeUninitDetails::MaybeUninitOneOf<
        _MaybeUninitDetails::rule5_constructible_v<T>,
        _MaybeUninitDetails::rule5_destructible_v<T>,
        T
    >;

    template <typename T>
    constexpr static bool is_unit_v = _MaybeUninitDetails::is_unit_v<T>;

public:

    /**
     * @brief Construct internal object.
     * @tparam U The type of the input value. Must be constructible into T.
     * @param val The value used to construct the object in memory.
     * @example
     * 
     * Alt::MaybeUninit<Alt::String> opt;
     * 
     * opt.Construct("MaybeUninit"_salloc);
     * assert(opt.GetVal() == "MaybeUninit");
     * 
     * // Leak!
     */
    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_FORCE_INLINE void Construct(U&& val) noexcept {

        Memory::ConstructObject<T>(m_Storage.m_Memory, std::forward<U>(val));
    }

    template <typename T,
        std::enable_if_t<!is_unit_v<T> && std::is_default_constructible_v<T>>* = nullptr>
    ALT_FORCE_INLINE void Construct() noexcept {

        Memory::ConstructDefaultObject<T>(m_Storage.m_Memory);
    }

    /**
     * @brief Construct internal object for clonable objects.
     * @tparam U The type of the input value. Must be constructible into T.
     * @param val The value used to construct the object in memory.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::MaybeUninit<Alt::String> opt;
     * 
     * Alt::String str = "MaybeUninitClonable"_salloc;
     * 
     * opt.Construct(str); // compilation error
     * opt.ConstructCloneOrMove(str) iferr_return; // can return false if 'CloneConstructFrom(...)' failed
     * 
     * assert(opt.GetVal() == str);
     * 
     * // Leak!
     */
    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && (TypeTraits::is_only_clonable_to_from_v<T, U&&> || std::is_constructible_v<T, U&&>)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool ConstructCloneOrMove(U&& val) noexcept {

        const T* pResult = Memory::CloneConstructOrMoveConstructObject(reinterpret_cast<T*>(m_Storage.m_Memory), std::forward<U>(val));
        return pResult != static_cast<const void*>(m_Storage.m_Memory);
    }

    /**
     * @brief Assign to the internal object.
     * @tparam U The type of the input value. Must be convertible to T.
     * @param val The value to assign to the existing object.
     * @example
     * 
     * Alt::MaybeUninit<Alt::String> opt;
     * opt.Construct("Construct"_salloc);
     * 
     * opt.Assign("MaybeUninit"_salloc);
     * assert(opt.GetVal() == "MaybeUninit");
     * 
     * // Leak!
     */
    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && std::is_assignable_v<T&, U&&>>* = nullptr>
    ALT_FORCE_INLINE void Assign(U&& val) noexcept {

        T* ptr = GetPtr();
        *ptr = std::forward<U>(val);
    }

    /**
     * @brief Assign to the internal object for clonable objects.
     * @tparam U The type of the input value. Must be convertible to T.
     * @param val The value to assign to the existing object.
     * @example
     * 
     * Alt::MaybeUninit<Alt::String> opt;
     * opt.Construct("Construct"_salloc);
     * 
     * Alt::String str = "MaybeUninitClonable"_salloc;
     * 
     * opt.Assign(str); // compilation error
     * opt.AssignCloneOrMove(str) iferr_return; // can return false if 'CloneFrom(...)' failed
     * 
     * assert(opt.GetVal() == "MaybeUninitClonable");
     * 
     * // Leak!
     */
    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && (TypeTraits::is_only_clonable_to_from_v<T, U&&> || std::is_assignable_v<T&, U&&>)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool AssignCloneOrMove(U&& val) noexcept {

        T* ptr = GetPtr();
        const T* pResult = Memory::CloneOrMoveObject(*ptr, std::forward<U>(val));

        return pResult != ptr;
    }

    /**
     * @brief Destroy the internal object.
     * @example
     * 
     * Alt::MaybeUninit<Alt::String> opt;
     * opt.Construct("Construct"_salloc);
     * 
     * opt.Assign("MaybeUninit"_salloc);
     * assert(opt.GetVal() == "MaybeUninit");
     * 
     * opt.Destroy(); // call destructor!
     */
    template <typename U = T,
        std::enable_if_t<!is_unit_v<U>>* = nullptr>
    ALT_FORCE_INLINE void Destroy() noexcept {

        Memory::DestroyObject(GetVal());
    }

    /**
     * @brief Accesses the object.
     * @return A reference to the stored object.
     * @example
     * 
     * Alt::MaybeUninit<Alt::String> opt;
     * opt.Construct("MaybeUninit"_salloc);
     * 
     * Alt::String& val = opt.GetVal();
     * assert(val == "MaybeUninit");
     * 
     * val = "Assign"_salloc;
     * assert(val == "Assign");
     * 
     * opt.Assign("Another Assign"_salloc);
     * assert(val == "Another Assign");
     * 
     * opt.Destroy();
     */
    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T& GetVal() & noexcept { return *GetPtr(); }

    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const T& GetVal() const & noexcept { return *GetPtr(); }

    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T&& GetVal() && noexcept { return std::move(*GetPtr()); }

    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const T&& GetVal() const && noexcept { return std::move(*GetPtr()); }

    /**
     * @brief Accesses to the pointer of object.
     * @return A pointer to the stored object.
     * @example
     * 
     * Alt::MaybeUninit<Alt::String> opt;
     * opt.Construct("MaybeUninit"_salloc);
     * 
     * Alt::String* pVal = opt.GetPtr();
     * assert(*pVal == "MaybeUninit");
     * 
     * *pVal = "Assign"_salloc;
     * assert(*pVal == "Assign");
     * 
     * opt.Destroy();
     */
    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T* GetPtr() noexcept { return std::launder(reinterpret_cast<T*>(m_Storage.m_Memory)); }

    template <typename U = T,
        std::enable_if_t<!is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const T* GetPtr() const noexcept { return std::launder(reinterpret_cast<const T*>(m_Storage.m_Memory)); }

    /*
     * `Unit_t` case (just do nothing, from getters just return underlying pointer for
     * compatibility with other methods). This is not necessary compiler should optimize
     * any empty `Unit_t` struct.
     */
    template <typename U = T,
        std::enable_if_t<is_unit_v<U>>* = nullptr>
    ALT_FORCE_INLINE void Construct(U&&) noexcept {}

    template <typename U = T,
        std::enable_if_t<is_unit_v<U>>* = nullptr>
    ALT_FORCE_INLINE bool ConstructCloneOrMove(U&&) noexcept { return true; }

    template <typename U = T,
        std::enable_if_t<is_unit_v<U>>* = nullptr>
    ALT_FORCE_INLINE void Assign(U&&) noexcept {}

    template <typename U = T,
        std::enable_if_t<is_unit_v<U>>* = nullptr>
    ALT_FORCE_INLINE bool AssignCloneOrMove(U&&) noexcept { return true; }

    template <typename U = T,
        std::enable_if_t<is_unit_v<U>>* = nullptr>
    ALT_FORCE_INLINE void Destroy() noexcept {}

    template <typename U = T,
        std::enable_if_t<is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T& GetVal() & noexcept { return *reinterpret_cast<T*>(m_Storage.m_Memory); }

    template <typename U = T,
        std::enable_if_t<is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const T& GetVal() const & noexcept { return *reinterpret_cast<const T*>(m_Storage.m_Memory); }

    template <typename U = T,
        std::enable_if_t<is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T&& GetVal() && noexcept { return Unit_t{}; }

    template <typename U = T,
        std::enable_if_t<is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const T&& GetVal() const && noexcept { return Unit_t{}; }

    template <typename U = T,
        std::enable_if_t<is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T* GetPtr() noexcept { return reinterpret_cast<T*>(m_Storage.m_Memory); }

    template <typename U = T,
        std::enable_if_t<is_unit_v<U> && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const T* GetPtr() const noexcept { return reinterpret_cast<const T*>(m_Storage.m_Memory); }

private:

    Storage m_Storage;
};

/**
 * @brief A data storage that allows to hold one of type passed in template like union.
 * @details This union-like structure allocates enough space to hold the largest type in the 
 * template parameter pack and aligns to the largest alignment requirement. It provides 
 * manual lifetime management for the active type.
 * @tparam ArgsT... A variadic list of possible types the container can hold.
 * @example
 * 
 * Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;
 * 
 * opt.Construct<Alt::String>("MaybeUninit"_salloc);
 * assert(storage.GetVal<Alt::String>() == "MaybeUninit");
 * 
 * storage.Destroy<Alt::String>();
 * 
 * storage.Construct<Alt::Vector<usize>>(Alt::Vector<usize>::Allocate(2, 4).GetValueOrDefault());
 * assert(storage.GetVal<Alt::Vector<usize>>()[0] == 4);
 * 
 * storage.Destroy<Alt::Vector<usize>>();
 */
template <typename ...ArgsT>
class MaybeUninitOneOf {
private:

    template <typename T>
    using clean_t = _MaybeUninitDetails::unwrap_void_to_unit_or_just_type_t<_MaybeUninitDetails::clean_t<T>>;

    using Storage = _MaybeUninitDetails::MaybeUninitOneOf<
        _MaybeUninitDetails::rule5_constructible_v<clean_t<ArgsT>...>,
        _MaybeUninitDetails::rule5_destructible_v<clean_t<ArgsT>...>,
        clean_t<ArgsT>...
    >;

    template <typename T>
    constexpr static bool is_unit_v = _MaybeUninitDetails::is_unit_v<T>;

    template <typename T>
    constexpr static bool is_compatible_v =
        (_MaybeUninitDetails::size_of_v<clean_t<T>> <= Storage::size_of) && (_MaybeUninitDetails::align_of_v<clean_t<T>> <= Storage::align_of);

public:

    /**
     * @brief Construct internal object.
     * @tparam T The type to construct. Must be compatible with the memory size/alignment and constructible from U.
     * @tparam U The type of the input value. Must be constructible into T.
     * @example
     * 
     * Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;
     * 
     * opt.Construct<Alt::String>("MaybeUninit"_salloc);
     * assert(opt.GetVal<Alt::String>() == "MaybeUninit");
     * 
     * // Leak!
     */
    template <typename T, typename U = T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_FORCE_INLINE void Construct(U&& val) noexcept {

        Memory::ConstructObject<T>(m_Storage.m_Memory, std::forward<U>(val));
    }

    template <typename T,
        std::enable_if_t<!is_unit_v<T> && std::is_default_constructible_v<T>>* = nullptr>
    ALT_FORCE_INLINE void Construct() noexcept {

        Memory::ConstructDefaultObject<T>(m_Storage.m_Memory);
    }

    /**
     * @brief Construct internal object for clonable objects.
     * @tparam T The type to construct. Must be compatible with the memory size/alignment and constructible from U.
     * @tparam U The type of the input value. Must be constructible into T.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;
     * 
     * Alt::String str = "MaybeUninitClonable"_salloc;
     * 
     * opt.Construct<Alt::String>(str); // compilation error
     * opt.ConstructCloneOrMove<Alt::String>(str) iferr_return; // can return false if 'CloneConstructFrom(...)' failed
     * 
     * assert(opt.GetVal<Alt::String>() == "MaybeUninitClonable");
     * 
     * // Leak!
     */
    template <typename T, typename U = T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T> && (TypeTraits::is_only_clonable_to_from_v<T, U&&> || std::is_constructible_v<T, U&&>)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool ConstructCloneOrMove(U&& val) noexcept {

        const T* pResult = Memory::CloneConstructOrMoveConstructObject(reinterpret_cast<T*>(m_Storage.m_Memory), std::forward<U>(val));
        return pResult != static_cast<const void*>(m_Storage.m_Memory);
    }

    /**
     * @brief Assign to the internal object.
     * @tparam T The type being assigned. Must be compatible with the memory size/alignment.
     * @tparam U The type of the input value. Must be convertible to T.
     * @param val The value to assign.
     * @example
     * 
     * Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;
     * opt.Construct<Alt::String>("Construct"_salloc);
     * 
     * opt.Assign<Alt::String>("MaybeUninit"_salloc);
     * assert(opt.GetVal<Alt::String>() == "MaybeUninit");
     * 
     * // Leak!
     */
    template <typename T, typename U = T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T> && std::is_assignable_v<T&, U&&>>* = nullptr>
    ALT_FORCE_INLINE void Assign(U&& val) noexcept {

        T* ptr = GetPtr<T>();
        *ptr = std::forward<U>(val);
    }

    /**
     * @brief Assign to the internal object for clonable objects.
     * @tparam T The type being assigned. Must be compatible with the memory size/alignment.
     * @tparam U The type of the input value. Must be convertible to T.
     * @param val The value to assign.
     * @example
     * 
     * Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;
     * opt.Construct<Alt::String>("Construct"_salloc);
     * 
     * Alt::String str = "MaybeUninitClonable"_salloc;
     * 
     * opt.Assign<Alt::String>(str); // compilation error
     * opt.AssignCloneOrMove<Alt::String>(str) iferr_return; // can return false if 'CloneFrom(...)' failed
     * 
     * assert(opt.GetVal<Alt::String>() == "MaybeUninitClonable");
     * 
     * // Leak!
     */
    template <typename T, typename U = T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T> && (TypeTraits::is_only_clonable_to_from_v<T, U&&> || std::is_assignable_v<T&, U&&>)>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool AssignCloneOrMove(U&& val) noexcept {

        T* ptr = GetPtr<T>();
        const T* pResult = Memory::CloneOrMoveObject(*ptr, std::forward<U>(val));

        return pResult != ptr;
    }

    /**
     * @brief Destroy the internal object.
     * @tparam T The type of the object to destroy. Must be compatible with the memory size/alignment.
     * @example
     * 
     * Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;
     * opt.Construct<Alt::String>("Construct"_salloc);
     * 
     * opt.Assign<Alt::String>("MaybeUninit"_salloc);
     * assert(opt.GetVal<Alt::String>() == "MaybeUninit");
     * 
     * opt.Destroy<Alt::String>(); // call destructor!
     */
    template <typename T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T>>* = nullptr>
    ALT_FORCE_INLINE void Destroy() noexcept {

        Memory::DestroyObject(GetVal<T>());
    }

    /**
     * @brief Accesses the object.
     * @tparam T The type of the object to destroy. Must be compatible with the memory size/alignment.
     * @return A reference to the stored object.
     * @example
     * 
     * Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;
     * opt.Construct<Alt::Vector<usize>>(Alt::Vector<usize>::Allocate(2, 4).GetValueOrDefault());
     * 
     * Alt::Vector<usize>& val = opt.GetVal<Alt::Vector<usize>>();
     * assert(val[0] == 4);
     * 
     * val[0] = 1;
     * assert(val[0] == 1);
     * 
     * opt.Assign<Alt::Vector<usize>>(Alt::Vector<usize>::AllocatePush(0, 1, 2, 4).GetValueOrDefault());
     * assert(val[1] == 1);
     * 
     * opt.Destroy<Alt::Vector<usize>>();
     */
    template <typename T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T& GetVal() & noexcept { return *GetPtr<T>(); }

    template <typename T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const T& GetVal() const & noexcept { return *GetPtr<T>(); }

    template <typename T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T&& GetVal() && noexcept { return std::move(*GetPtr<T>()); }

    template <typename T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const T&& GetVal() const && noexcept { return std::move(*GetPtr<T>()); }

    /**
     * @brief Accesses to the pointer of object.
     * @tparam T The type of the object to destroy. Must be compatible with the memory size/alignment.
     * @return A pointer to the stored object.
     * @example
     * 
     * Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;
     * opt.Construct<Alt::Vector<usize>>(Alt::Vector<usize>::Allocate(2, 4).GetValueOrDefault());
     * 
     * Alt::Vector<usize>* pVal = opt.GetPtr<Alt::Vector<usize>>();
     * assert((*pVal)[0] == 4);
     * 
     * (*pVal)[0] = 1;
     * assert((*pVal)[0] == 1);
     * 
     * opt.Assign<Alt::Vector<usize>>(Alt::Vector<usize>::AllocatePush(0, 1, 2, 4).GetValueOrDefault());
     * assert((*pVal)[1] == 1);
     * 
     * opt.Destroy<Alt::Vector<usize>>();
     */
    template <typename T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE T* GetPtr() noexcept { return std::launder(reinterpret_cast<T*>(m_Storage.m_Memory)); }

    template <typename T,
        std::enable_if_t<!is_unit_v<T> && is_compatible_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const T* GetPtr() const noexcept { return std::launder(reinterpret_cast<const T*>(m_Storage.m_Memory)); }

    /*
     * `Unit_t` case (just do nothing, from getters just return underlying pointer for
     * compatibility with other methods). This is not necessary - compiler should optimize
     * any empty `Unit_t` struct.
     */
    template <typename T, typename U = T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_FORCE_INLINE void Construct(U&&) noexcept {}

    template <typename T, typename U = T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_FORCE_INLINE bool ConstructCloneOrMove(U&&) noexcept { return true; }

    template <typename T, typename U = T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_FORCE_INLINE void Assign(U&&) noexcept {}

    template <typename T, typename U = T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_FORCE_INLINE bool AssignCloneOrMove(U&&) noexcept { return true; }

    template <typename T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_FORCE_INLINE void Destroy() noexcept {}

    template <typename T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE clean_t<T>& GetVal() & noexcept { return *reinterpret_cast<clean_t<T>*>(m_Storage.m_Memory); }

    template <typename T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const clean_t<T>& GetVal() const & noexcept { return *reinterpret_cast<const clean_t<T>*>(m_Storage.m_Memory); }

    template <typename T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE clean_t<T>&& GetVal() && noexcept { return Unit_t{}; }

    template <typename T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const clean_t<T>&& GetVal() const && noexcept { return Unit_t{}; }

    template <typename T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE clean_t<T>* GetPtr() noexcept { return reinterpret_cast<clean_t<T>*>(m_Storage.m_Memory); }

    template <typename T,
        std::enable_if_t<is_unit_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE const clean_t<T>* GetPtr() const noexcept { return reinterpret_cast<const clean_t<T>*>(m_Storage.m_Memory); }

private:

    Storage m_Storage;
};

/*
 * Case with empty oneof - just use `Unit_t`
 */
template <>
class MaybeUninitOneOf<> : public MaybeUninitOneOf<Unit_t>
{};

} // namespace Alt

#endif // __ALT_STD_MAYBE_UNINIT__