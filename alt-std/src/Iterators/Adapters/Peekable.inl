#include "../IteratorTrait.h"

/*
 * Alt::IteratorForwardFromFunc iter{
 *     []() {
 *         static usize cnt = 0;
 *         return cnt++;
 *     }
 * };
 *
 * auto it = iter
 *     .Take(2)
 *     .Peekable();
 *
 * assert(*it.Peek() == 0);
 * assert(*it.Peek() == 0);
 * assert(*it.Next() == 0);
 *
 * assert(*it.Peek() == 1);
 * assert(*it.Peek() == 1);
 * assert(*it.Next() == 1);
 *
 * assert(it.Next() == Alt::Nullopt);
 * assert(it.Peek() == Alt::Nullopt);
 * assert(it.Peek() == Alt::Nullopt);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT>
class PeekableForwardTraitImpl
{};

template <typename PrevIterT>
class PeekableForwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Peekable_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_Peeked) {

            return Exchange(Self().m_Peeked, Nullopt).GetValue();
        }

        return Self().m_Iter.Next();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        if (Self().m_Peeked) {

            Self().m_Peeked = Alt::Nullopt;

            if (advance > 1) {

                const usize size = Self().m_Iter.Advance(advance - 1);
                ALT_ASSERT(size < advance);

                return size + 1;
            }

            return advance;
        }

        return Self().m_Iter.Advance(advance);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const Optional<T>& Peek() noexcept {

        if (!Self().m_Peeked) {

            Self().m_Peeked = Next();
        }

        return Self().m_Peeked.GetValue();
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class PeekableBackwardTraitImpl
{};

template <typename PrevIterT>
class PeekableBackwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Peekable_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (Optional<T> result = Self().m_Iter.NextBack()) {
            return result;
        }

        if (Self().m_Peeked) {

            return Exchange(Self().m_Peeked, Nullopt).GetValue();
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {

        usize size = Self().m_Iter.AdvanceBack(advance);
        if (size != advance) {

            if (Self().m_Peeked) {

                Self().m_Peeked = Nullopt;
                return size + 1;
            }
        }

        return size;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class PeekableExactSizeTraitImpl
{};

template <typename PrevIterT>
class PeekableExactSizeTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Peekable_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {
        return Self().m_Iter.GetSize() + (Self().IsPeeked() ? 1 : 0);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return Self().m_Iter.IsEmpty() && !Self().IsPeeked();
    }

private:

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

template <typename PrevIterT>
class Peekable_ final

    : public _IterDetails::TraitMarker

    , public PeekableForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT
    >
    , public PeekableBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        PrevIterT
    >
    , public PeekableExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Peekable_<PrevIterT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        Peekable_<PrevIterT>
    >
{
public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Peekable_(const PrevIterT& iter) noexcept
        : m_Iter(iter)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_Iter.GetSizeHint() + (IsPeeked() ? 1 : 0);
    }

private:

    ALT_NODISCARD ALT_FORCE_INLINE bool IsPeeked() const noexcept {
        return m_Peeked && m_Peeked.GetValue();
    }

private:

    template <bool, typename>
    friend class PeekableForwardTraitImpl;

    template <bool, typename>
    friend class PeekableBackwardTraitImpl;

    template <bool, typename>
    friend class PeekableExactSizeTraitImpl;

    PrevIterT m_Iter;
    Optional<Optional<Type>> m_Peeked;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt