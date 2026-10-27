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
 *     .Inspect([](const usize cnt) {
 *         ALT_PRINTLN("{}", cnt);
 *     })
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 0);
 * assert(vec[1] == 1);
 * assert(vec[2] == 2);
 * assert(vec[3] == 3);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT, typename FuncT>
class InspectForwardTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class InspectForwardTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Inspect_<PrevIterT, FuncT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Optional<T> val = Self().m_Iter.Next()) {

            (Self().m_Func)(val.GetValue());
            return val;
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
class InspectBackwardTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class InspectBackwardTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Inspect_<PrevIterT, FuncT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (Optional<T> val = Self().m_Iter.NextBack()) {

            (Self().m_Func)(val.GetValue());
            return val;
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

template <bool IS_NEED_IMPL, typename PrevIterT, typename FuncT>
class InspectExactSizeTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class InspectExactSizeTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Inspect_<PrevIterT, FuncT>;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {
        return Self().m_Iter.GetSize();
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return Self().m_Iter.IsEmpty();
    }

private:

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

template <typename PrevIterT, typename FuncT>
class Inspect_ final

    : public _IterDetails::TraitMarker

    , public InspectForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >
    , public InspectBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >
    , public InspectExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Inspect_<PrevIterT, FuncT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        Inspect_<PrevIterT, FuncT>
    >
{
public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Inspect_(const PrevIterT iter, FuncT&& func) noexcept
        : m_Iter(iter)
        , m_Func(std::move(func))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_Iter.GetSizeHint();
    }

private:

    template <bool, typename, typename>
    friend class InspectForwardTraitImpl;

    template <bool, typename, typename>
    friend class InspectBackwardTraitImpl;

    template <bool, typename, typename>
    friend class InspectExactSizeTraitImpl;

    PrevIterT m_Iter;
    FuncT m_Func;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt