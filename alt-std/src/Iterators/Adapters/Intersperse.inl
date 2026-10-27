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
*     .Intersperse([]() {
*         return 4;
*     })
*     .CollectOrDefault<Alt::Vector<usize>>();
*
* assert(vec[0] == 0);
* assert(vec[1] == 4);
* assert(vec[2] == 1);
* assert(vec[3] == 4);
* assert(vec[4] == 2);
* assert(vec[5] == 4);
* assert(vec[6] == 3);
*/
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT, typename FuncT>
class IntersperseForwardTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class IntersperseForwardTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Intersperse_<PrevIterT, FuncT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (m_IsIntersperseForwardTurn) {

            m_IsIntersperseForwardTurn = false;
            return static_cast<T>((Self().m_Func)());
        }

        if (Optional<T> val = Self().m_Iter.Next()) {

            m_IsIntersperseForwardTurn = true;
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

private:

    bool m_IsIntersperseForwardTurn = false;
};

template <bool IS_NEED_IMPL, typename PrevIterT, typename FuncT>
class IntersperseExactSizeTraitImpl
{};

template <typename PrevIterT, typename FuncT>
class IntersperseExactSizeTraitImpl<true, PrevIterT, FuncT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Intersperse_<PrevIterT, FuncT>;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        const usize size = Self().m_Iter.GetSize();
        return size + Num::SubWrap(size, 1);
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
class Intersperse_ final

    : public _IterDetails::TraitMarker

    , public IntersperseForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >
    , public IntersperseExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT,
        FuncT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Intersperse_<PrevIterT, FuncT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        Intersperse_<PrevIterT, FuncT>
    >
{
public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Intersperse_(const PrevIterT iter, FuncT&& func) noexcept
        : m_Iter(iter)
        , m_Func(std::move(func))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {

        const usize sizeHint = m_Iter.GetSizeHint();
        return sizeHint + Num::SubWrap(sizeHint, 1);
    }

private:

    template <bool, typename, typename>
    friend class IntersperseForwardTraitImpl;

    template <bool, typename, typename>
    friend class IntersperseBackwardTraitImpl;

    template <bool, typename, typename>
    friend class IntersperseExactSizeTraitImpl;

    PrevIterT m_Iter;
    FuncT m_Func;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt