#include "../IteratorTrait.h"

#include "../../Utils/Pair.h"

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
 * Alt::Vector<Alt::Pair<usize, usize>> vec = iter0
 *     .Take(2)
 *     .Zip(iter1.Skip(1))
 *     .CollectOrDefault<Alt::Vector<Alt::Pair<usize, usize>>>();
 *
 * assert(vec[0].f == 0);
 * assert(vec[0].s == 1);
 *
 * assert(vec[1].f == 1);
 * assert(vec[1].s == 2);
 */
namespace Alt {
namespace _IterDetails {
namespace Adapters {

template <bool IS_NEED_IMPL, typename PrevIterT0, typename PrevIterT1>
class ZipForwardTraitImpl
{};

template <typename PrevIterT0, typename PrevIterT1>
class ZipForwardTraitImpl<true, PrevIterT0, PrevIterT1> {
public:

    using T0 = typename PrevIterT0::Type;
    using T1 = typename PrevIterT1::Type;
    using SelfT = Zip_<PrevIterT0, PrevIterT1>;

    using Type = Pair<T0, T1>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Type> Next() noexcept {

        Optional<T0> val0 = Self().m_Iter0.Next();
        Optional<T1> val1 = Self().m_Iter1.Next();

        if (val0 && val1) {

            return MakePair(
                val0.GetValue(),
                val1.GetValue()
            );
        }

        return Alt::Nullopt;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        usize size = 0;
        for (usize i = 0; i < advance; ++i) {

            const Optional<Type> val = Next();
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

template <bool IS_NEED_IMPL, typename PrevIterT0, typename PrevIterT1>
class ZipBackwardTraitImpl
{};

template <typename PrevIterT0, typename PrevIterT1>
class ZipBackwardTraitImpl<true, PrevIterT0, PrevIterT1> {
public:

    using T0 = typename PrevIterT0::Type;
    using T1 = typename PrevIterT1::Type;
    using SelfT = Zip_<PrevIterT0, PrevIterT1>;

    using Type = Pair<T0, T1>;

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Type> NextBack() noexcept {

        const usize size0 = Self().m_Iter0.GetSize();
        const usize size1 = Self().m_Iter1.GetSize();

        if (size0 != size1) {

            if (size0 > size1) {

                Self().m_Iter0.AdvanceBack(size0 - size1);
            }
            else {

                Self().m_Iter1.AdvanceBack(size1 - size0);
            }
        }

        Optional<T0> val0 = Self().m_Iter0.NextBack();
        Optional<T1> val1 = Self().m_Iter1.NextBack();

        return MakePair(
            std::move(val0.GetValue()),
            std::move(val1.GetValue())
        );
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {

        usize size = 0;
        for (usize i = 0; i < advance; ++i) {

            const Optional<Type> val = NextBack();
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

template <bool IS_NEED_IMPL, typename PrevIterT0, typename PrevIterT1>
class ZipExactSizeTraitImpl
{};

template <typename PrevIterT0, typename PrevIterT1>
class ZipExactSizeTraitImpl<true, PrevIterT0, PrevIterT1> {
public:

    using T0 = typename PrevIterT0::Type;
    using T1 = typename PrevIterT1::Type;
    using SelfT = Zip_<PrevIterT0, PrevIterT1>;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        return Num::Min(Self().m_Iter0.GetSize(), Self().m_Iter1.GetSize());
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return GetSize() == 0;
    }

private:

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

template <typename PrevIterT0, typename PrevIterT1>
class Zip_ final

    : public _IterDetails::TraitMarker

    , public ZipForwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT1>,
        PrevIterT0,
        PrevIterT1
    >
    , public ZipBackwardTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT1> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT1>,
        PrevIterT0,
        PrevIterT1
    >
    , public ZipExactSizeTraitImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT1>,
        PrevIterT0,
        PrevIterT1
    >

    , public IteratorForwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorForwardTrait, PrevIterT1>,
        Zip_<PrevIterT0, PrevIterT1>
    >
    , public IteratorBackwardAdaptersImpl<
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT0> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorBackwardTrait, PrevIterT1> &&
        _IterDetails::is_trait_exist_in_alt_iterator_v<IIteratorExactSizeTrait, PrevIterT1>,
        Zip_<PrevIterT0, PrevIterT1>
    >
{
public:

    using Type = Pair<typename PrevIterT0::Type, typename PrevIterT1::Type>;

    using Traits = _IterDetails::type_list<

          _IterDetails::return_if_exist_type_in_type_list_t<IIteratorForwardTrait<Type>, _IterDetails::type_list<PrevIterT0, PrevIterT1>, IIteratorForwardTrait>
        , _IterDetails::return_if_exist_type_in_type_list_t<IIteratorBackwardTrait<Type>, _IterDetails::type_list<PrevIterT0, PrevIterT1>, IIteratorBackwardTrait, IIteratorExactSizeTrait>
        , _IterDetails::return_if_exist_type_in_type_list_t<IIteratorExactSizeTrait<Type>, _IterDetails::type_list<PrevIterT0, PrevIterT1>, IIteratorExactSizeTrait>
    >;

    ALT_FORCE_INLINE Zip_(const PrevIterT0& iter0, const PrevIterT1& iter1) noexcept
        : m_Iter0(iter0)
        , m_Iter1(iter1)
    {
        ALT_ASSERT((void*)&m_Iter0 != (void*)&m_Iter1);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return Num::Min(m_Iter0.GetSizeHint(), m_Iter1.GetSizeHint());
    }

private:

    template <bool, typename, typename>
    friend class ZipForwardTraitImpl;

    template <bool, typename, typename>
    friend class ZipBackwardTraitImpl;

    template <bool, typename, typename>
    friend class ZipExactSizeTraitImpl;

    PrevIterT0 m_Iter0;
    PrevIterT1 m_Iter1;
};

} // namespace Adapters
} // namespace _IterDetails
} // namespace Alt