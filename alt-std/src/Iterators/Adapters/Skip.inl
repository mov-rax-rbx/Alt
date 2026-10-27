#include "../IteratorTrait.h"

#include "../../Utils/Num.h"

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
 *     .Skip(2)
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

template <bool IS_NEED_IMPL, typename PrevIterT>
class SkipForwardTraitImpl
{};

template <typename PrevIterT>
class SkipForwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Skip_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_SkipN != 0) {

            ALT_MAYBE_UNUSED usize advance = Self().m_Iter.Advance(Self().m_SkipN);
            Self().m_SkipN = 0;
        }

        Self().OnAdvance(1);
        return Self().m_Iter.Next();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        if (Self().m_SkipN != 0) {

            Self().m_Iter.Advance(Self().m_SkipN);
            Self().m_SkipN = 0;
        }

        Self().OnAdvance(advance);
        return Self().m_Iter.Advance(advance);
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class SkipBackwardTraitImpl {
public:

    ALT_FORCE_INLINE SkipBackwardTraitImpl(const PrevIterT&, const usize) noexcept
    {}

    ALT_FORCE_INLINE void OnAdvance(const usize) noexcept {
    }
};

template <typename PrevIterT>
class SkipBackwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Skip_<PrevIterT>;

    ALT_FORCE_INLINE SkipBackwardTraitImpl(const PrevIterT& iter, const usize skipN) noexcept
        : m_TakeCnt(Num::SubWrap(iter.GetSize(), skipN))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (m_TakeCnt == 0) {
            return Nullopt;
        }

        m_TakeCnt -= 1;
        return Self().m_Iter.NextBack();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, Self().m_TakeCnt);
        Self().m_TakeCnt -= advance;

        return Self().m_Iter.AdvanceBack(advance);
    }

    ALT_FORCE_INLINE void OnAdvance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, Self().m_TakeCnt);
        Self().m_TakeCnt -= advance;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }

private:

    usize m_TakeCnt;
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class SkipExactSizeTraitImpl
{};

template <typename PrevIterT>
class SkipExactSizeTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Skip_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        return Num::SubWrap(Self().m_Iter.GetSize(), Self().m_SkipN);
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
class Skip_ final

    : public _IterDetails::TraitMarker

    , public SkipForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT
    >
    , public SkipBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >
    , public SkipExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Skip_<PrevIterT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        Skip_<PrevIterT>
    >
{
private:

    using SuperSkipBackwardTraitImpl = SkipBackwardTraitImpl<
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

    ALT_FORCE_INLINE Skip_(const PrevIterT& iter, const usize skipN) noexcept
        : SuperSkipBackwardTraitImpl(iter, skipN)
        , m_Iter(iter)
        , m_SkipN(skipN)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return Num::SubWrap(m_Iter.GetSizeHint(), m_SkipN);
    }

private:

    template <bool, typename>
    friend class SkipForwardTraitImpl;

    template <bool, typename>
    friend class SkipBackwardTraitImpl;

    template <bool, typename>
    friend class SkipExactSizeTraitImpl;

    PrevIterT m_Iter;
    usize m_SkipN;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt