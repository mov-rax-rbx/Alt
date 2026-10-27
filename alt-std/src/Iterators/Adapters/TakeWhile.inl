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
 *     .TakeWhile([](const usize val) {
 *         return val < 1;
 *     })
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 0);
 * assert(vec.GetSize() == 1);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT, typename FuncT>
class TakeWhileForwardTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class TakeWhileForwardTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = TakeWhile_<PrevIterT, FuncT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_IsFinished) {
            return Nullopt;
        }

        Optional<T> val = Self().m_Iter.Next();

        if ((Self().m_Pred)(*val)) {
            return val;
        }

        Self().m_IsFinished = true;
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

template <typename PrevIterT, typename FuncT>
class TakeWhile_ final

    : public _IterDetails::TraitMarker

    , public TakeWhileForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        TakeWhile_<PrevIterT, FuncT>
    >
{
public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
    >;

    ALT_FORCE_INLINE TakeWhile_(const PrevIterT& iter, FuncT&& pred) noexcept
        : m_Iter(iter)
        , m_Pred(std::forward<FuncT>(pred))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return 0;
    }

private:

    template <bool, typename, typename>
    friend class TakeWhileForwardTraitImpl;

    PrevIterT m_Iter;

    FuncT m_Pred;
    bool m_IsFinished = false;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt