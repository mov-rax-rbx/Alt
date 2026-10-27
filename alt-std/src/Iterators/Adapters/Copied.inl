#include "../IteratorTrait.h"

/*
 * static usize s_CopyCnt = 0;
 *
 * class CopyCounter {
 * public:
 *
 *     CopyCounter(usize) noexcept {
 *     }
 *
 *     CopyCounter(const CopyCounter&) noexcept {
 *
 *         s_CopyCnt += 1;
 *     }
 *
 *     CopyCounter& operator=(const CopyCounter&) noexcept {
 *
 *         s_CopyCnt += 1;
 *     }
 *
 *     CopyCounter(CopyCounter&&) noexcept = default;
 *     CopyCounter& operator=(CopyCounter&&) noexcept = default;
 * };
 *
 * Alt::IteratorForwardFromFunc iter{
 *     []() {
 *         static usize cnt = 0;
 *         return CopyCounter{ cnt++ };
 *     }
 * };
 *
 * Alt::Vector<CopyCounter> vec = iter
 *     .Take(2)
 *     .Copied()
 *     .CollectOrDefault<Alt::Vector<CopyCounter>>();
 *
 * assert(s_CopyCnt == 2);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT>
class CopiedForwardTraitImpl
{};

template <typename PrevIterT>
class CopiedForwardTraitImpl<true, PrevIterT> {
public:

    using T = std::remove_reference_t<typename PrevIterT::Type>;
    using SelfT = Copied_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Optional<typename PrevIterT::Type> val = Self().m_Iter.Next()) {

            return val.GetValue();
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        return Self().m_Iter.Advance(advance);
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class CopiedBackwardTraitImpl
{};

template <typename PrevIterT>
class CopiedBackwardTraitImpl<true, PrevIterT> {
public:

    using T = std::remove_reference_t<typename PrevIterT::Type>;
    using SelfT = Copied_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (Optional<typename PrevIterT::Type> val = Self().m_Iter.NextBack()) {

            return val.GetValue();
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
class CopiedExactSizeTraitImpl
{};

template <typename PrevIterT>
class CopiedExactSizeTraitImpl<true, PrevIterT> {
public:

    using T = std::remove_reference_t<typename PrevIterT::Type>;
    using SelfT = Copied_<PrevIterT>;

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
class Copied_ final

    : public _IterDetails::TraitMarker

    , public CopiedForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT
    >
    , public CopiedBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        PrevIterT
    >
    , public CopiedExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Copied_<PrevIterT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        Copied_<PrevIterT>
    >
{
public:

    using Type = std::remove_reference_t<typename PrevIterT::Type>;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Copied_(const PrevIterT& iter) noexcept
        : m_Iter(iter)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_Iter.GetSizeHint();
    }

private:

    template <bool, typename>
    friend class CopiedForwardTraitImpl;

    template <bool, typename>
    friend class CopiedBackwardTraitImpl;

    template <bool, typename>
    friend class CopiedExactSizeTraitImpl;

    PrevIterT m_Iter;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt