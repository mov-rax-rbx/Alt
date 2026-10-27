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
 *     .Take(8)
 *     .Step(2)
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 0);
 * assert(vec[1] == 2);
 * assert(vec[2] == 4);
 * assert(vec[3] == 6);
 * assert(vec.GetSize() == 4);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT>
class StepForwardTraitImpl
{};

template <typename PrevIterT>
class StepForwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Step_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_IsFirst) {

            Self().m_IsFirst = false;
            return Self().m_Iter.Next();
        }

        ALT_MAYBE_UNUSED usize advance = Self().m_Iter.Advance(Self().m_StepMinusOne);
        return Self().m_Iter.Next();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        return Self().m_IsFirst
            ? Self().m_Iter.Advance(advance * Self().m_Step)
            : Self().m_Iter.Advance(Num::SubWrap(advance * Self().m_Step, 1));
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class StepBackwardTraitImpl
{};

template <typename PrevIterT>
class StepBackwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Step_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        const usize size = Self().m_Iter.GetSize();
        const usize tail = size % Self().m_Step;

        usize advance = 0;

        if (Self().m_IsFirst) {

            if (tail != 0) {

                advance = tail - 1;
            }
            else {

                advance = Self().m_StepMinusOne;
            }
        }
        else {

            advance = tail;
        }

        Self().m_Iter.AdvanceBack(advance);
        return Self().m_Iter.NextBack();
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

template <bool IS_NEED_IMPL, typename PrevIterT>
class StepExactSizeTraitImpl
{};

template <typename PrevIterT>
class StepExactSizeTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Step_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        return Self().m_Step == 0
            ? Self().m_Iter.GetSize()
            : Self().m_Iter.GetSize() / Self().m_Step;
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
class Step_ final

    : public _IterDetails::TraitMarker

    , public StepForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT
    >
    , public StepBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >
    , public StepExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Step_<PrevIterT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        Step_<PrevIterT>
    >
{
public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait, IIteratorExactSizeTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Step_(const PrevIterT& iter, const usize step) noexcept
        : m_Iter(iter)
        , m_StepMinusOne(Num::SubWrap(step, 1))
        , m_Step(step)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {

        return m_Step == 0
            ? m_Iter.GetSizeHint()
            : m_Iter.GetSizeHint() / m_Step;
    }

private:

    template <bool, typename>
    friend class StepForwardTraitImpl;

    template <bool, typename>
    friend class StepBackwardTraitImpl;

    template <bool, typename>
    friend class StepExactSizeTraitImpl;

    PrevIterT m_Iter;

    usize m_StepMinusOne;
    usize m_Step;

    bool m_IsFirst = true;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt