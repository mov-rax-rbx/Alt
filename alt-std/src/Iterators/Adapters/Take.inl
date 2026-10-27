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
 *     .Take(1)
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 0);
 * assert(vec.GetSize() == 1);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT>
class TakeForwardTraitImpl
{};

template <typename PrevIterT>
class TakeForwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Take_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_TakeCnt == 0) {
            return Nullopt;
        }

        Self().m_TakeCnt -= 1;
        return Self().m_Iter.Next();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, Self().m_TakeCnt);
        Self().m_TakeCnt -= advance;

        return Self().m_Iter.Advance(advance);
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class TakeBackwardTraitImpl {
public:

    ALT_FORCE_INLINE TakeBackwardTraitImpl(const PrevIterT&, const usize) noexcept
    {}
};

template <typename PrevIterT>
class TakeBackwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Take_<PrevIterT>;

    ALT_FORCE_INLINE TakeBackwardTraitImpl(const PrevIterT& iter, const usize takeCnt) noexcept
        : m_SkipN(Num::SubWrap(iter.GetSize(), takeCnt))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (m_SkipN != 0) {

            Self().m_Iter.AdvanceBack(m_SkipN);
            m_SkipN = 0;
        }

        return Self().m_Iter.NextBack();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {

        if (m_SkipN != 0) {

            Self().m_Iter.AdvanceBack(m_SkipN);
            m_SkipN = 0;
        }

        return Self().m_Iter.AdvanceBack(advance);
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }

private:

    usize m_SkipN;
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class TakeExactSizeTraitImpl
{};

template <typename PrevIterT>
class TakeExactSizeTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Take_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        return Num::Min(Self().m_TakeCnt, Self().m_Iter.GetSize());
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return GetSize() == 0;
    }

private:

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

template <typename PrevIterT>
class Take_ final

    : public _IterDetails::TraitMarker

    , public TakeForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT
    >
    , public TakeBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >
    , public TakeExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Take_<PrevIterT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        Take_<PrevIterT>
    >
{
private:

    using SuperTakeBackwardTraitImpl = TakeBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >;

public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait, IIteratorExactSizeTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Take_(const PrevIterT& iter, const usize takeCnt) noexcept
        : SuperTakeBackwardTraitImpl(iter, takeCnt)
        , m_Iter(iter)
        , m_TakeCnt(takeCnt)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return Num::Min(m_Iter.GetSizeHint(), m_TakeCnt);
    }

private:

    template <bool, typename>
    friend class TakeForwardTraitImpl;

    template <bool, typename>
    friend class TakeBackwardTraitImpl;

    template <bool, typename>
    friend class TakeExactSizeTraitImpl;

    PrevIterT m_Iter;
    usize m_TakeCnt;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt