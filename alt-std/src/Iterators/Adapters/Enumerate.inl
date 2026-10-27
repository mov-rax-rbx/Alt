#include "../IteratorTrait.h"

#include "../../Utils/Pair.h"

/*
* Alt::IteratorForwardFromFunc iter{
*     []() {
*         return static_cast<usize>(0);
*     }
* };
*
* Alt::Vector<Alt::Pair<usize, usize>> vec = iter
*     .Take(4)
*     .Enumerate()
*     .CollectOrDefault<Alt::Vector<Alt::Pair<usize, usize>>>();
*
* assert(vec[0].f == 0);
* assert(vec[0].s == 0);
*
* assert(vec[1].f == 1);
* assert(vec[1].s == 0);
*
* assert(vec[2].f == 2);
* assert(vec[2].s == 0);
*
* assert(vec[3].f == 3);
* assert(vec[3].s == 0);
*/
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT>
class EnumerateForwardTraitImpl
{};

template <typename PrevIterT>
class EnumerateForwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Enumerate_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Pair<usize, T>> Next() noexcept {

        if (Optional<T> val = Self().m_Iter.Next()) {

            const usize i = Self().m_Cnt;
            Self().m_Cnt += 1;

            return MakePair(i, val.GetValue());
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        const usize size = Self().m_Iter.Advance(advance);
        Self().m_Cnt += size;

        return size;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class EnumerateBackwardTraitImpl
{};

template <typename PrevIterT>
class EnumerateBackwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Enumerate_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Pair<usize, T>> NextBack() noexcept {

        if (Optional<T> val = Self().m_Iter.NextBack()) {

            const usize cnt = Self().m_Cnt;
            return MakePair(cnt + Self().GetSize(), std::move(val.GetValue()));
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {

        return Self().m_Iter.AdvanceBack(advance);
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class EnumerateExactSizeTraitImpl
{};

template <typename PrevIterT>
class EnumerateExactSizeTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Enumerate_<PrevIterT>;

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

template <typename PrevIterT>
class Enumerate_ final

    : public _IterDetails::TraitMarker

    , public EnumerateForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT
    >
    , public EnumerateBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >
    , public EnumerateExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Enumerate_<PrevIterT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        Enumerate_<PrevIterT>
    >
{
public:

    using Type = Pair<usize, typename PrevIterT::Type>;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait, IIteratorExactSizeTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Enumerate_(const PrevIterT& iter) noexcept
        : m_Iter(iter)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_Iter.GetSizeHint();
    }

private:

    template <bool, typename>
    friend class EnumerateForwardTraitImpl;

    template <bool, typename>
    friend class EnumerateBackwardTraitImpl;

    template <bool, typename>
    friend class EnumerateExactSizeTraitImpl;

    PrevIterT m_Iter;
    usize m_Cnt = 0;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt