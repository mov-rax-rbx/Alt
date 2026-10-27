#pragma once

#ifndef __ALT_STD_ITERATORS__
#define __ALT_STD_ITERATORS__

#include <iterator>

#include "../Adapters/Adapters.h"

/*
* const std::vector<usize> vec{ 0, 1, 2, 3, 4 };
* Alt::IteratorFromSTD stdIter{ vec.begin(), vec.end() };
*
* const std::vector<usize> vecFromIter = stdIter
*     .CloneIter()
*     .Map([](const usize* pVal) {
*         return *pVal;
*     })
*     .CollectOrDefault<std::vector<usize>>();
*
* assert(vec == vecFromIter);
*/
namespace Alt {
namespace _IterDetails {

template <typename SelfT, typename T, typename STDIterCategoryTagT>
class IteratorFromSTDImpl_;

template <typename SelfT, typename STDIterT>
using IteratorFromSTDImpl = std::conditional_t<
    std::is_same_v<typename std::iterator_traits<STDIterT>::iterator_category, std::input_iterator_tag>,
    IteratorFromSTDImpl_<
        SelfT,
        typename std::iterator_traits<STDIterT>::value_type,
        typename std::iterator_traits<STDIterT>::iterator_category
    >,
    IteratorFromSTDImpl_<
        SelfT,
        typename std::iterator_traits<STDIterT>::pointer,
        typename std::iterator_traits<STDIterT>::iterator_category
    >
>;

} // namespace _IterDetails

template <typename STDIterT>
class IteratorFromSTD final
    : public _IterDetails::TraitMarker
    , public _IterDetails::IteratorFromSTDImpl<IteratorFromSTD<STDIterT>, STDIterT>
{
public:

    using Super = _IterDetails::IteratorFromSTDImpl<IteratorFromSTD<STDIterT>, STDIterT>;

    using Type = typename Super::Type;
    using Traits = typename Super::Traits;

    ALT_FORCE_INLINE IteratorFromSTD(STDIterT begin, STDIterT end) noexcept
        : m_Begin(begin)
        , m_End(end)
    {}

    /*
     * IteratorBase
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {

        return GetSizeHintImpl<
            std::is_same_v<
                typename std::iterator_traits<STDIterT>::iterator_category, std::random_access_iterator_tag
            >
        >();
    }

private:

    template <bool IS_NEED_IMPL>
    ALT_FORCE_INLINE usize GetSizeHintImpl() const noexcept {
        return 0;
    }

    template <>
    ALT_FORCE_INLINE usize GetSizeHintImpl<true>() const noexcept {
        return m_End - m_Begin;
    }

private:

    template <typename, typename, typename>
    friend class _IterDetails::IteratorFromSTDImpl_;

private:

    STDIterT m_Begin;
    STDIterT m_End;
};

namespace _IterDetails {

template <typename SelfT, typename T>
class IteratorFromSTDImpl_<SelfT, T, std::input_iterator_tag>
    : public Adapters::IteratorForwardAdaptersImpl<true, SelfT>
{
public:

    using Type = T;
    using Traits = _IterDetails::type_list<IIteratorForwardTrait<T>>;

    /*
     * IIteratorForwardTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_Begin == Self().m_End) {

            return Nullopt;
        }

        T result = Self().m_Begin;
        Self().m_Begin += 1;

        return result;
    }

    /*
     * IIteratorForwardTrait
     */
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

template <typename SelfT, typename T>
class IteratorFromSTDImpl_<SelfT, T, std::forward_iterator_tag>
    : public Adapters::IteratorForwardAdaptersImpl<true, SelfT>
{
public:

    using Type = T;
    using Traits = _IterDetails::type_list<IIteratorForwardTrait<T>>;

    /*
     * IIteratorForwardTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_Begin == Self().m_End) {

            return Nullopt;
        }

        T result = &*Self().m_Begin;
        Self().m_Begin += 1;

        return result;
    }

    /*
     * IIteratorForwardTrait
     */
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

template <typename SelfT, typename T>
class IteratorFromSTDImpl_<SelfT, T, std::bidirectional_iterator_tag>
    : public Adapters::IteratorForwardAdaptersImpl<true, SelfT>
    , public Adapters::IteratorBackwardAdaptersImpl<true, SelfT>
{
public:

    using Type = T;
    using Traits = _IterDetails::type_list<IIteratorForwardTrait<T>, IIteratorBackwardTrait<T>>;

    /*
     * IIteratorForwardTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_Begin == Self().m_End) {

            return Nullopt;
        }

        T result = &*Self().m_Begin;
        Self().m_Begin += 1;

        return result;
    }

    /*
     * IIteratorForwardTrait
     */
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

    /*
     * IIteratorBackwardTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (Self().m_Begin == Self().m_End) {

            return Nullopt;
        }

        Self().m_End -= 1;
        T result = &*Self().m_End;

        return result;
    }

    /*
     * IIteratorBackwardTrait
     */
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

template <typename SelfT, typename T>
class IteratorFromSTDImpl_<SelfT, T, std::random_access_iterator_tag> 
    : public Adapters::IteratorForwardAdaptersImpl<true, SelfT>
    , public Adapters::IteratorBackwardAdaptersImpl<true, SelfT>
{
public:

    using Type = T;
    using Traits = _IterDetails::type_list<IIteratorForwardTrait<T>, IIteratorBackwardTrait<T>, IIteratorExactSizeTrait<T>>;

    /*
     * IIteratorForwardTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {

        if (Self().m_Begin == Self().m_End) {

            return Nullopt;
        }

        T result = &*Self().m_Begin;
        Self().m_Begin += 1;

        return result;
    }

    /*
     * IIteratorForwardTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(Self().GetSize(), advance_);
        Self().m_Begin += advance;

        return advance;
    }

    /*
     * IIteratorBackwardTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> NextBack() noexcept {

        if (Self().m_Begin == Self().m_End) {

            return Nullopt;
        }

        Self().m_End -= 1;
        T result = &*Self().m_End;

        return result;
    }

    /*
     * IIteratorBackwardTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(Self().GetSize(), advance_);
        Self().m_End -= advance;

        return advance;
    }

    /*
     * IIteratorExactSizeTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {
        return Self().m_End - Self().m_Begin;
    }

    /*
     * IIteratorExactSizeTrait
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return Self().m_Begin == Self().m_End;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

} // namespace _IterDetails
} // namespace Alt

#endif // __ALT_STD_ITERATORS__