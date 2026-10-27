#pragma once

#ifndef __ALT_STD_ITERATOR__
#define __ALT_STD_ITERATOR__

#include "IteratorTrait.h"
#include "IteratorInterface.h"

#include "IteratorCollector.h"
#include "Adapters/Adapters.h"

#include "STD/STDIterator.h"

/*
 * class Iterator final : public Alt::Iterator<Iterator, T*, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait> {
 * public:
 * 
 *     Iterator(Vector& self) noexcept
 *         : m_pBegin(self.m_pMemory)
 *         , m_pEnd(self.m_pSizeEnd)
 *     {}
 * 
 *     // IteratorBase
 *     usize GetSizeHint() const noexcept {
 *         return GetSize(); // can be just - return 0;
 *     }
 * 
 *     // IIteratorForwardTrait
 *     Optional<T*> Next() noexcept {
 * 
 *         if (m_pBegin == m_pEnd) {
 *             return Nullopt;
 *         }
 * 
 *         T* pResult = m_pBegin;
 *         m_pBegin += 1;
 * 
 *         return pResult;
 *     }
 * 
 *     // IIteratorForwardTrait
 *     usize Advance(const usize advance_) noexcept {
 * 
 *         const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
 *         m_pBegin += advance;
 * 
 *         return advance;
 *     }
 * 
 *     // IIteratorBackwardTrait
 *     Optional<T*> NextBack() noexcept {
 * 
 *         if (m_pBegin == m_pEnd) {
 *             return Nullopt;
 *         }
 * 
 *         m_pEnd -= 1;
 *         T* pResult = m_pEnd;
 * 
 *         return pResult;
 *     }
 * 
 *     // IIteratorBackwardTrait
 *     usize AdvanceBack(const usize advance_) noexcept {
 * 
 *         const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
 *         m_pEnd -= advance;
 * 
 *         return advance;
 *     }
 * 
 *     // IIteratorExactSizeTrait
 *     usize GetSize() const noexcept {
 *         return m_pEnd - m_pBegin;
 *     }
 * 
 *     // IIteratorExactSizeTrait
 *     bool IsEmpty() const noexcept {
 *         return m_pEnd == m_pBegin;
 *     }
 * 
 * private:
 * 
 *     T* m_pBegin;
 *     T* m_pEnd;
 * };
 */
namespace Alt {
namespace _IterDetails {

template <typename SelfT, typename FuncT, bool IS_OPTIONAL_RETURN>
class IteratorForwardFromFuncImpl;

template <typename SelfT, typename FuncT, bool IS_OPTIONAL_RETURN>
class IteratorBackwardFromFuncImpl;

} // namespace _IterDetails

template <typename SelfT, typename T, template <typename> typename ...TraitsT>
class Iterator
    : public _IterDetails::TraitMarker
    , public _IterDetails::Adapters::IteratorForwardAdaptersImpl<_IterDetails::is_trait_exist_in_variadic_v<T, IIteratorForwardTrait, TraitsT...>, SelfT>
    , public _IterDetails::Adapters::IteratorBackwardAdaptersImpl<_IterDetails::is_trait_exist_in_variadic_v<T, IIteratorBackwardTrait, TraitsT...>, SelfT>
{
public:

    using Type = T;
    using Traits = _IterDetails::type_list<TraitsT<T>...>;

    /*
     * IteratorBase
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return Self().GetSizeHint();
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

template <typename SelfT, typename T>
class Iterator<SelfT, T>;

/*
 * Alt::IteratorForwardFromFunc iter{
 *     []() {
 *         static usize cnt = 0;
 *         return cnt++;
 *     }
 * };
 * 
 * Alt::Vector<usize> vec = iter
 *     .Take(4)
 *     .CollectOrDefault<Alt::Vector>();
 * 
 * assert(vec[0] == 0);
 * assert(vec[1] == 1);
 * assert(vec[2] == 2);
 * assert(vec[3] == 3);
 */
template <typename FuncT>
class IteratorForwardFromFunc final
    : public _IterDetails::TraitMarker
    , public _IterDetails::IteratorForwardFromFuncImpl<IteratorForwardFromFunc<FuncT>, FuncT, TypeTraits::is_optional_v<std::invoke_result_t<FuncT>>>
{
public:

    using Super = _IterDetails::IteratorForwardFromFuncImpl<IteratorForwardFromFunc<FuncT>, FuncT, TypeTraits::is_optional_v<std::invoke_result_t<FuncT>>>;

    using Type = typename Super::Type;
    using Traits = typename Super::Traits;

    ALT_FORCE_INLINE IteratorForwardFromFunc(FuncT func) noexcept
        : Super(func)
    {}

    /*
    * IteratorBase
    */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return 0;
    }
};

/*
 * Alt::IteratorBackwardFromFunc iter{
 *     []() {
 *         static usize cnt = 4;
 *         return --cnt;
 *     }
 * };
 * 
 * Alt::Vector<usize> vec = iter
 *     .Rev() // bacause 'Rev()' conveerts 'IIteratorForwardTrait' -> 'IIteratorBackwardTrait' and 'IIteratorBackwardTrait' -> 'IIteratorForwardTrait'
 *     .Take(4)
 *     .CollectOrDefault<Alt::Vector>();
 * 
 * assert(vec[0] == 3);
 * assert(vec[1] == 2);
 * assert(vec[2] == 1);
 * assert(vec[3] == 0);
 */
template <typename FuncT>
class IteratorBackwardFromFunc final
    : public _IterDetails::TraitMarker
    , public _IterDetails::IteratorBackwardFromFuncImpl<IteratorBackwardFromFunc<FuncT>, FuncT, TypeTraits::is_optional_v<std::invoke_result_t<FuncT>>>
{
public:

    using Super = _IterDetails::IteratorBackwardFromFuncImpl<IteratorBackwardFromFunc<FuncT>, FuncT, TypeTraits::is_optional_v<std::invoke_result_t<FuncT>>>;

    using Type = typename Super::Type;
    using Traits = typename Super::Traits;

    ALT_FORCE_INLINE IteratorBackwardFromFunc(FuncT func) noexcept
        : Super(func)
    {}

    /*
     * IteratorBase
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return 0;
    }
};

namespace _IterDetails {

template <typename SelfT, typename FuncT>
class IteratorForwardFromFuncImpl<SelfT, FuncT, true>
    : public _IterDetails::Adapters::IteratorForwardAdaptersImpl<true, SelfT>
{
public:

    using Type = typename std::invoke_result_t<FuncT>::Type;
    using Traits = _IterDetails::type_list<IIteratorForwardTrait<Type>>;

    template <typename U = FuncT>
    ALT_FORCE_INLINE IteratorForwardFromFuncImpl(U&& func) noexcept
        : m_Func(std::forward<U>(func))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Type> Next() noexcept {
        return (m_Func)();
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

    FuncT m_Func;
};

template <typename SelfT, typename FuncT>
class IteratorForwardFromFuncImpl<SelfT, FuncT, false>
    : public _IterDetails::Adapters::IteratorForwardAdaptersImpl<true, SelfT>
{
public:

    using Type = typename std::invoke_result_t<FuncT>;
    using Traits = _IterDetails::type_list<IIteratorForwardTrait<Type>>;

    template <typename U = FuncT>
    ALT_FORCE_INLINE IteratorForwardFromFuncImpl(U&& func) noexcept
        : m_Func(std::forward<U>(func))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Type> Next() noexcept {
        return MakeOptional((m_Func)());
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

    FuncT m_Func;
};

template <typename SelfT, typename FuncT>
class IteratorBackwardFromFuncImpl<SelfT, FuncT, true>
    : public _IterDetails::Adapters::IteratorBackwardAdaptersImpl<true, SelfT>
{
public:

    using Type = typename std::invoke_result_t<FuncT>::Type;
    using Traits = _IterDetails::type_list<IIteratorBackwardTrait<Type>>;

    template <typename U = FuncT>
    ALT_FORCE_INLINE IteratorBackwardFromFuncImpl(U&& func) noexcept
        : m_Func(std::forward<U>(func))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Type> NextBack() noexcept {
        return (m_Func)();
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

    FuncT m_Func;
};

template <typename SelfT, typename FuncT>
class IteratorBackwardFromFuncImpl<SelfT, FuncT, false>
    : public _IterDetails::Adapters::IteratorBackwardAdaptersImpl<true, SelfT>
{
public:

    using Type = typename std::invoke_result_t<FuncT>;
    using Traits = _IterDetails::type_list<IIteratorBackwardTrait<Type>>;

    template <typename U = FuncT>
    ALT_FORCE_INLINE IteratorBackwardFromFuncImpl(U&& func) noexcept
        : m_Func(std::forward<U>(func))
    {}

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Type> NextBack() noexcept {
        return MakeOptional((m_Func)());
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

    FuncT m_Func;
};

} // namespace _IterDetails
} // namespace Alt

#endif // __ALT_STD_ITERATOR__