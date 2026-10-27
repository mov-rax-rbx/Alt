#include "../IteratorTrait.h"

/*
 * Alt::IteratorForwardFromFunc iter{
 *     []() {
 *         static usize cnt = 0;
 *         return cnt++;
 *     }
 * };
 *
 * Alt::Vector<usize> vec = iter
 *     .Take(4)
 *     .Filter([](const usize val) -> bool {
 *         return val % 2 == 0;
 *     })
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 0);
 * assert(vec[1] == 2);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT, typename FuncT>
class FilterForwardTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class FilterForwardTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Filter_<PrevIterT, FuncT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        while (Optional<T> val = Self().m_Iter.Next()) {

            if ((Self().m_Pred)(*val)) {
                return val;
            }
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        usize size = 0;
        for (usize i = 0; i < advance; ++i) {

            const Optional<T> val = Next();
            if (!val) {
                return size;
            }

            size += 1;
        }

        return size;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT, typename FuncT>
class FilterBackwardTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class FilterBackwardTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Filter_<PrevIterT, FuncT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        while (Optional<T> val = Self().m_Iter.NextBack()) {

            if ((Self().m_Pred)(*val)) {
                return val;
            }
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {

        usize size = 0;
        for (usize i = 0; i < advance; ++i) {

            const Optional<T> val = NextBack();
            if (!val) {
                return size;
            }

            size += 1;
        }

        return size;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <typename PrevIterT, typename FuncT>
class Filter_ final

    : public _IterDetails::TraitMarker

    , public FilterForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >
    , public FilterBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Filter_<PrevIterT, FuncT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        Filter_<PrevIterT, FuncT>
    >
{
public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait>
    >;

    ALT_FORCE_INLINE Filter_(const PrevIterT& iter, FuncT&& pred) noexcept
        : m_Iter(iter)
        , m_Pred(std::move(pred))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return 0;
    }

private:

    template <bool, typename, typename>
    friend class FilterForwardTraitImpl;

    template <bool, typename, typename>
    friend class FilterBackwardTraitImpl;

    template <bool, typename, typename>
    friend class FilterExactSizeTraitImpl;

    PrevIterT m_Iter;
    FuncT m_Pred;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt