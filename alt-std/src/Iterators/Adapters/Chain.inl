#include "../IteratorTrait.h"

/*
 * Alt::IteratorForwardFromFunc iter0{
 *     []() {
 *         static usize cnt = 0;
 *         return cnt++;
 *     }
 * };
 *
 * Alt::IteratorForwardFromFunc iter1{
 *     []() {
 *         static usize cnt = 0;
 *         return cnt++;
 *     }
 * };
 *
 * Alt::Vector<usize> vec = iter0
 *     .Take(2)
 *     .Chain(iter1.Take(2))
 *     .CollectOrDefault<Alt::Vector<usize>>();
 *
 * assert(vec[0] == 0);
 * assert(vec[1] == 1);
 * assert(vec[2] == 0);
 * assert(vec[3] == 1);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT0, typename PrevIterT1>
class ChainForwardTraitImpl
{};

template <typename PrevIterT0, typename PrevIterT1>
class ChainForwardTraitImpl<true, PrevIterT0, PrevIterT1> {
public:

    using T = typename PrevIterT0::Type;
    using SelfT = Chain_<PrevIterT0, PrevIterT1>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_Iter0) {

            if (Optional<T> val = Self().m_Iter0->Next()) {
                return val;
            }

            Self().m_Iter0 = Nullopt;
        }

        if (Self().m_Iter1) {

            if (Optional<T> val = Self().m_Iter1->Next()) {
                return MakeOptional(static_cast<T>(val.GetValue()));
            }

            Self().m_Iter1 = Nullopt;
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        usize size = 0;

        if (Self().m_Iter0) {

            size = Self().m_Iter0->Advance(advance);

            if (size == advance) {
                return size;
            }

            Self().m_Iter0 = Nullopt;
        }

        if (Self().m_Iter1) {

            size += Self().m_Iter1->Advance(advance - size);
            if (size != advance) {

                Self().m_Iter1 = Nullopt;
            }
        }

        return size;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT0, typename PrevIterT1>
class ChainBackwardTraitImpl
{};

template <typename PrevIterT0, typename PrevIterT1>
class ChainBackwardTraitImpl<true, PrevIterT0, PrevIterT1> {
public:

    using T = typename PrevIterT0::Type;
    using SelfT = Chain_<PrevIterT0, PrevIterT1>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (Self().m_Iter1) {

            if (Optional<T> val = Self().m_Iter1->NextBack()) {
                return MakeOptional(static_cast<T>(val.GetValue()));
            }

            Self().m_Iter1 = Nullopt;
        }

        if (Self().m_Iter0) {

            if (Optional<T> val = Self().m_Iter0->NextBack()) {
                return val;
            }

            Self().m_Iter0 = Nullopt;
        }

        return Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {

        usize size = 0;

        if (Self().m_Iter1) {

            size += Self().m_Iter1->AdvanceBack(advance);
            if (size != advance) {

                Self().m_Iter1 = Nullopt;
            }
        }

        if (Self().m_Iter0) {

            size += Self().m_Iter0->Advance(advance - size);

            if (size == advance) {
                return size;
            }

            Self().m_Iter0 = Nullopt;
        }

        return size;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename PrevIterT0, typename PrevIterT1>
class ChainExactSizeTraitImpl
{};

template <typename PrevIterT0, typename PrevIterT1>
class ChainExactSizeTraitImpl<true, PrevIterT0, PrevIterT1> {
public:

    using T = typename PrevIterT0::Type;
    using SelfT = Chain_<PrevIterT0, PrevIterT1>;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        const usize size0 = Self().m_Iter0
            ? Self().m_Iter0->GetSize()
            : 0;

        const usize size1 = Self().m_Iter1
            ? Self().m_Iter1->GetSize()
            : 0;

        return size0 + size1;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        const bool isEmpty0 = Self().m_Iter0
            ? Self().m_Iter0->IsEmpty()
            : true;

        const bool isEmpty1 = Self().m_Iter1
            ? Self().m_Iter1->IsEmpty()
            : true;

        return isEmpty0 && isEmpty1;
    }

private:

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

template <typename PrevIterT0, typename PrevIterT1>
class Chain_ final

    : public _IterDetails::TraitMarker

    , public ChainForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT1>,
        PrevIterT0,
        PrevIterT1
    >
    , public ChainBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT1>,
        PrevIterT0,
        PrevIterT1
    >
    , public ChainExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT1>,
        PrevIterT0,
        PrevIterT1
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT1>,
        Chain_<PrevIterT0, PrevIterT1>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT1>,
        Chain_<PrevIterT0, PrevIterT1>
    >
{
public:

    using Type = typename PrevIterT0::Type;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_type_in_type_list_t<IIteratorForwardTrait<Type>, _IterDetails::type_list<PrevIterT0, PrevIterT1>, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_type_in_type_list_t<IIteratorBackwardTrait<Type>, _IterDetails::type_list<PrevIterT0, PrevIterT1>, IIteratorBackwardTrait>
        , _IterDetails::return_if_exist_type_in_type_list_t<IIteratorExactSizeTrait<Type>, _IterDetails::type_list<PrevIterT0, PrevIterT1>, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Chain_(const PrevIterT0& iter0, const PrevIterT1& iter1) noexcept
        : m_Iter0(iter0)
        , m_Iter1(iter1)
    {
        ALT_ASSERT((void*)&m_Iter0 != (void*)&m_Iter1);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_Iter0->GetSizeHint() + m_Iter1->GetSizeHint();
    }

private:

    template <bool, typename, typename>
    friend class ChainForwardTraitImpl;

    template <bool, typename, typename>
    friend class ChainBackwardTraitImpl;

    template <bool, typename, typename>
    friend class ChainExactSizeTraitImpl;

    Optional<PrevIterT0> m_Iter0;
    Optional<PrevIterT1> m_Iter1;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt