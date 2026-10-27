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
 *     .Take(6)
 *     .SkipWhile([](const usize val) -> bool {
 *         return val < 2;
 *     })
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 2);
 * assert(vec[1] == 3);
 * assert(vec[2] == 4);
 * assert(vec[3] == 5);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT, typename FuncT>
class SkipWhileForwardTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class SkipWhileForwardTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = SkipWhile_<PrevIterT, FuncT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_IsFirst) {

            while (Optional<T> val = Self().m_Iter.Next()) {

                if (!(Self().m_Pred)(*val)) {

                    Self().m_IsFirst = false;
                    return val;
                }
            }

            Self().m_IsFirst = false;
        }

        return Self().m_Iter.Next();
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

template <typename PrevIterT, typename FuncT>
class SkipWhile_ final

    : public _IterDetails::TraitMarker

    , public SkipWhileForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        SkipWhile_<PrevIterT, FuncT>
    >
{
public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
    >;

    ALT_FORCE_INLINE SkipWhile_(const PrevIterT& iter, FuncT&& pred) noexcept
        : m_Iter(iter)
        , m_Pred(std::forward<FuncT>(pred))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return 0;
    }

private:

    template <bool, typename, typename>
    friend class SkipWhileForwardTraitImpl;

    PrevIterT m_Iter;

    FuncT m_Pred;
    bool m_IsFirst = true;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt