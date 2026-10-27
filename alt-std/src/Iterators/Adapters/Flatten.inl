#include "../IteratorTrait.h"

/*
 * Alt::IteratorForwardFromFunc iter{
 *     []() {
 *
 *         static Alt::Vector<usize> vec = Alt::Vector<usize>::Allocate(0, 1)
 *             .Take();
 *
 *         return vec.GetIterator();
 *     }
 * };
 *
 * Alt::Vector<usize> vec = iter
 *     .Take(2)
 *     .Flatten()
 *     .Map([](const usize* pVal) {
 *         return *pVal;
 *     })
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 0);
 * assert(vec[1] == 1);
 *
 * assert(vec[2] == 0);
 * assert(vec[3] == 1);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT>
class FlattenForwardTraitImpl
{};

template <typename PrevIterT>
class FlattenForwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type::Type;
    using SelfT = Flatten_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        for (;;) {

            if (!Self().m_InnerForwardIter) {

                Self().m_InnerForwardIter = Self().m_Iter.Next();
                if (!Self().m_InnerForwardIter) {

                    Self().m_InnerForwardIter = Exchange(Self().m_InnerBackwardIter, Nullopt);
                    if (!Self().m_InnerForwardIter) {

                        return Nullopt;
                    }
                }
            }

            if (Optional<T> val = Self().m_InnerForwardIter->Next()) {

                return val;
            }

            Self().m_InnerForwardIter = Nullopt;
        }
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        usize size = 0;
        if (Self().m_InnerForwardIter) {

            size = Self().m_InnerForwardIter->Advance(advance);
            ALT_ASSERT(size <= advance);

            if (size == advance) {
                return size;
            }

            Self().m_InnerForwardIter = Nullopt;
        }

        while (Optional<typename PrevIterT::Type> iter = Self().m_Iter.Next()) {

            size += iter->Advance(advance - size);
            ALT_ASSERT(size <= advance);

            if (size == advance) {
                return size;
            }
        }

        if (Self().m_InnerBackwardIter) {

            size += Self().m_InnerBackwardIter->Advance(advance - size);
            ALT_ASSERT(size <= advance);

            if (size == advance) {
                return size;
            }

            Self().m_InnerBackwardIter = Nullopt;
        }

        return size;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class FlattenBackwardTraitImpl
{};

template <typename PrevIterT>
class FlattenBackwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type::Type;
    using SelfT = Flatten_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        for (;;) {

            if (!Self().m_InnerBackwardIter) {

                Self().m_InnerBackwardIter = Self().m_Iter.NextBack();
                if (!Self().m_InnerBackwardIter) {

                    Self().m_InnerBackwardIter = Exchange(Self().m_InnerForwardIter, Nullopt);
                    if (!Self().m_InnerBackwardIter) {

                        return Nullopt;
                    }
                }
            }

            if (Optional<T> val = Self().m_InnerBackwardIter->NextBack()) {

                return val;
            }

            Self().m_InnerBackwardIter = Nullopt;
        }
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {

        usize size = 0;

        if (Self().m_InnerBackwardIter) {

            size = Self().m_InnerBackwardIter->AdvanceBack(advance);

            if (size == advance) {
                return size;
            }

            Self().m_InnerBackwardIter = Nullopt;
        }

        while (Optional<typename PrevIterT::Type> iter = Self().m_Iter.NextBack()) {

            size += iter->AdvanceBack(advance - size);
            if (size == advance) {
                return size;
            }
        }

        if (Self().m_InnerForwardIter) {

            size += Self().m_InnerForwardIter->AdvanceBack(advance - size);

            if (size == advance) {
                return size;
            }

            Self().m_InnerForwardIter = Nullopt;
        }

        return size;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <typename PrevIterT>
class Flatten_ final

    : public _IterDetails::TraitMarker

    , public FlattenForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT
    >
    , public FlattenBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        PrevIterT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Flatten_<PrevIterT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        Flatten_<PrevIterT>
    >
{
public:

    using Type = typename PrevIterT::Type::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait>
    >;

    ALT_FORCE_INLINE Flatten_(const PrevIterT& iter) noexcept
        : m_Iter(iter)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return 0;
    }

private:

    template <bool, typename>
    friend class FlattenForwardTraitImpl;

    template <bool, typename>
    friend class FlattenBackwardTraitImpl;

    PrevIterT m_Iter;

    Optional<typename PrevIterT::Type> m_InnerForwardIter;
    Optional<typename PrevIterT::Type> m_InnerBackwardIter;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt