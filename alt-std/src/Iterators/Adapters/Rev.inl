#include "../IteratorTrait.h"

/*
 * Alt::IteratorForwardFromFunc iter{
 *     []() {
 *         static usize cnt = 0;
 *         return cnt++;
 *     }
 * };
 *
 * Alt::Vector<usize> vec0 = iter
 *     .Take(4)
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec0[0] == 0);
 * assert(vec0[1] == 1);
 * assert(vec0[2] == 2);
 * assert(vec0[3] == 3);
 *
 * Alt::Vector<usize> vec1 = vec0
 *     .GetIterator()
 *     .Map([](const usize* pVal) {
 *         return *pVal;
 *     })
 *     .Rev()
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec1[0] == 3);
 * assert(vec1[1] == 2);
 * assert(vec1[2] == 1);
 * assert(vec1[3] == 0);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT>
class RevForwardTraitImpl
{};

template <typename PrevIterT>
class RevForwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Rev_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {
        return Self().m_Iter.NextBack();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {
        return Self().m_Iter.AdvanceBack(advance);
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class RevBackwardTraitImpl
{};

template <typename PrevIterT>
class RevBackwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Rev_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {
        return Self().m_Iter.Next();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {
        return Self().m_Iter.Advance(advance);
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class RevExactSizeTraitImpl
{};

template <typename PrevIterT>
class RevExactSizeTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Rev_<PrevIterT>;

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
class Rev_ final

    : public _IterDetails::TraitMarker

    , public RevForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        PrevIterT
    >
    , public RevBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT
    >
    , public RevExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        Rev_<PrevIterT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Rev_<PrevIterT>
    >
{
public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorBackwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Rev_(const PrevIterT& iter) noexcept
        : m_Iter(iter)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_Iter.GetSizeHint();
    }

private:

    template <bool, typename>
    friend class RevForwardTraitImpl;

    template <bool, typename>
    friend class RevBackwardTraitImpl;

    template <bool, typename>
    friend class RevExactSizeTraitImpl;

    PrevIterT m_Iter;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt