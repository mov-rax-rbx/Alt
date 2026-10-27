#pragma once

#ifndef __ALT_STD_ITERATOR_CORE_IMPLEMENTATION__
#define __ALT_STD_ITERATOR_CORE_IMPLEMENTATION__

#include "../../TypeTraits.h"

namespace Alt {
namespace _IterDetails {

class TraitMarker
{};

struct Null
{};

template <typename ...ArgsT>
struct type_list;

template <typename HeadT, typename ...TailT>
struct type_list<HeadT, TailT...> {
    using Type = HeadT;
    using Rest = type_list<TailT...>;
};

template <typename HeadT>
struct type_list<HeadT> {
    using Type = HeadT;
    using Rest = type_list<>;
};

template <>
struct type_list<Null> {
    using Type = Null;
    using Rest = Null;
};

template <>
struct type_list<> {
    using Type = Null;
    using Rest = Null;
};

template <typename TypeListT>
constexpr static bool is_empty_type_list_v = std::is_same_v<TypeListT, type_list<>>;

template <typename T, typename TypeListT>
struct is_type_exist_in_type_list
    : public std::conditional_t<
        is_empty_type_list_v<TypeListT>,
        std::false_type,
        std::conditional_t<
            std::is_same_v<T, typename TypeListT::Type>,
            std::true_type,
            is_type_exist_in_type_list<T, typename TypeListT::Rest>
        >
    >
{};

template <typename RetT, typename ItersTypeListT, template <typename> typename ...TraitsT>
struct return_if_exist_type_in_type_list {
    using type = std::conditional_t<
        is_empty_type_list_v<ItersTypeListT>,
        void,
        std::conditional_t<
            std::conjunction_v<is_type_exist_in_type_list<TraitsT<typename ItersTypeListT::Type::Type>, typename ItersTypeListT::Type::Traits>...>,
            RetT,
            return_if_exist_type_in_type_list<RetT, typename ItersTypeListT::Rest, TraitsT...>
        >
    >;
};

template <typename TypeListT0, typename TypeListT1>
struct count_intersect_type_list {

    constexpr static usize value = std::conditional_t<
        is_empty_type_list_v<TypeListT0>,
        std::integral_constant<usize, 0>,
        std::integral_constant<usize, (is_type_exist_in_type_list<typename TypeListT0::Type, TypeListT1>::value ? 1 : 0) + count_intersect_type_list<typename TypeListT0::Rest, TypeListT1>::value>
    >::value;
};

template <typename TypeListT1>
struct count_intersect_type_list<Null, TypeListT1> {

    constexpr static usize value = 0;
};

template <typename T>
constexpr static bool is_trait_alt_iterator_v = std::is_base_of_v<TraitMarker, T>;

template <typename T, template <typename> typename TraitT, template <typename> typename ...TraitsVariadicT>
constexpr static bool is_trait_exist_in_variadic_v = std::disjunction_v<std::is_same<TraitT<T>, TraitsVariadicT<T>>...>;

template <template <typename> typename TraitT, typename IterT>
constexpr static bool is_trait_exist_in_alt_iterator_v = is_type_exist_in_type_list<TraitT<typename IterT::Type>, typename IterT::Traits>::value;

template <typename TypeListT0, typename TypeListT1>
constexpr static bool count_intersect_type_list_v = count_intersect_type_list<TypeListT0, TypeListT1>::value;

template <typename RetT, typename ItersTypeListT, template <typename> typename ...TraitsT>
using return_if_exist_type_in_type_list_t = typename return_if_exist_type_in_type_list<RetT, ItersTypeListT, TraitsT...>::type;

template <typename RetT, typename IterT, template <typename> typename ...TraitsT>
using return_if_exist_trait_in_alt_iterator_t =  typename return_if_exist_type_in_type_list<RetT, type_list<IterT>, TraitsT...>::type;

} // namespace _IterDetails
} // namespace Alt

#endif // __ALT_STD_ITERATOR_CORE_IMPLEMENTATION__