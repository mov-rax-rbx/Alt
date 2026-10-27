#include "../IteratorTrait.h"

/*
 * Alt::IteratorForwardFromFunc iter{
 *     []() {
 *
 *         static usize cnt = 0;
 *
 *         usize result = cnt;
 *         cnt += 1;
 *
 *         return result % 2 == 0
 *             ? Alt::MakeOptional(result)
 *             : Alt::Nullopt;
 *     }
 * };
 *
 * Alt::Vector<usize> vec = iter
 *     .Take(4)
 *     .Fuse()
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 0);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT>
class FuseForwardTraitImpl
{};

template <typename PrevIterT>
class FuseForwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Fuse_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_IsFinished) {
            return Nullopt;
        }

        if (Optional<T> val = Self().m_Iter.Next()) {
            return val;
        }

        Self().m_IsFinished = true;
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
};

template <bool IS_NEED_IMPL, typename PrevIterT>
class FuseBackwardTraitImpl
{};

template <typename PrevIterT>
class FuseBackwardTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Fuse_<PrevIterT>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (Self().m_IsFinished) {
            return Nullopt;
        }

        if (Optional<T> val = Self().m_Iter.NextBack()) {
            return val;
        }

        Self().m_IsFinished = true;
        return Nullopt;
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
class FuseExactSizeTraitImpl
{};

template <typename PrevIterT>
class FuseExactSizeTraitImpl<true, PrevIterT> {
public:

    using T = typename PrevIterT::Type;
    using SelfT = Fuse_<PrevIterT>;

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
class Fuse_ final

    : public _IterDetails::TraitMarker

    , public FuseForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        PrevIterT
    >
    , public FuseBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        PrevIterT
    >
    , public FuseExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT>,
        PrevIterT
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT>,
        Fuse_<PrevIterT>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT>,
        Fuse_<PrevIterT>
    >
{
public:

    using Type = typename PrevIterT::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorForwardTrait<Type>, PrevIterT, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorBackwardTrait<Type>, PrevIterT, IIteratorBackwardTrait>
        , _IterDetails::return_if_exist_trait_in_alt_iterator_t<IIteratorExactSizeTrait<Type>, PrevIterT, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Fuse_(const PrevIterT& iter) noexcept
        : m_Iter(iter)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_Iter.GetSizeHint();
    }

private:

    template <bool, typename>
    friend class FuseForwardTraitImpl;

    template <bool, typename>
    friend class FuseBackwardTraitImpl;

    template <bool, typename>
    friend class FuseExactSizeTraitImpl;

    PrevIterT m_Iter;

    bool m_IsFinished = false;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt