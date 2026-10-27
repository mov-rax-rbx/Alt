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
 *     .Map([](const usize cnt) {
 *         return cnt + 1;
 *     })
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 1);
 * assert(vec[1] == 2);
 * assert(vec[2] == 3);
 * assert(vec[3] == 4);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT, typename FuncT>
class MapForwardTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class MapForwardTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Map_<PrevIterT, FuncT>;

    using RetT = std::invoke_result_t<FuncT, T>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<RetT> Next() noexcept {

        if (Optional<T> val = Self().m_Iter.Next()) {

            return (Self().m_Func)(std::move(val.GetValue()));
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        usize size = 0;
        for (usize i = 0; i < advance; ++i) {

            const Optional<RetT> val = Next();
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
class MapBackwardTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class MapBackwardTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Map_<PrevIterT, FuncT>;

    using RetT = std::invoke_result_t<FuncT, T>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<RetT> NextBack() noexcept {

        if (Optional<T> val = Self().m_Iter.NextBack()) {

            return (Self().m_Func)(std::move(val.GetValue()));
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {

        usize size = 0;
        for (usize i = 0; i < advance; ++i) {

            const Optional<RetT> val = NextBack();
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
class MapExactSizeTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class MapExactSizeTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Map_<PrevIterT, FuncT>;

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
class Map_ final

    : public _IterDetails::TraitMarker

    , public MapForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >
    , public MapBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >
    , public MapExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Map_<PrevIterT, FuncT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        Map_<PrevIterT, FuncT>
    >
{
public:

    using Type = std::invoke_result_t<FuncT, typename PrevIterT::Type>;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Map_(const PrevIterT iter, FuncT&& func) noexcept
        : m_Iter(iter)
        , m_Func(std::move(func))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_Iter.GetSizeHint();
    }

private:

    template <bool, typename, typename>
    friend class MapForwardTraitImpl;

    template <bool, typename, typename>
    friend class MapBackwardTraitImpl;

    template <bool, typename, typename>
    friend class MapExactSizeTraitImpl;

    PrevIterT m_Iter;
    FuncT m_Func;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt