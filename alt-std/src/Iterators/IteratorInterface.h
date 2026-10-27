#pragma once

#ifndef __ALT_STD_ITERATOR_INTERFACE__
#define __ALT_STD_ITERATOR_INTERFACE__

#include "IteratorTrait.h"

#include "Adapters/Adapters.h"

#include "../Utils/Func.h"

/*
 * Example of usage:
 * 
 *     void Append(Alt::Vector<T>& vec, Alt::IteratorInterfaceRef<T*, Alt::IIteratorForwardTrait> iter) {
 * 
 *         iter.ForEach([&vec](T* pVal) {
 * 
 *             vec.Push(std::move(*pVal));
 *         });
 *     }
 * 
 *     template <typename IterT>
 *     void Append(Vector<T>& vec, IteratorInterface<IterT> iter) {
 * 
 *         iter.ForEach([&vec](T* pVal) {
 * 
 *             vec.Push(std::move(*pVal));
 *         });
 *     }
 */
namespace Alt {

template <typename T, template <typename> typename ...TraitsT>
class IteratorInterface;

namespace _IterDetails {

template <typename SelfT, typename T, template <typename> typename TraitsT>
class IteratorInterfaceImpl;

template <typename T>
struct is_alt_iterator_interface
    : std::false_type
{};

template <typename T, template <typename> typename ...TraitsT>
struct is_alt_iterator_interface<IteratorInterface<T, TraitsT...>>
    : std::true_type
{};

template <typename T>
constexpr static bool is_alt_iterator_interface_v = is_alt_iterator_interface<T>::value;

} // namespace _IterDetails 

template <typename T, template <typename> typename ...TraitsT>
class IteratorInterface final
    : public _IterDetails::IteratorInterfaceImpl<IteratorInterface<T, TraitsT...>, T, TraitsT>...
{
public:

    using Type = T;
    using Traits = _IterDetails::type_list<TraitsT<T>...>;

    template <typename IterT,
        std::enable_if_t<!_IterDetails::is_alt_iterator_interface_v<IterT> && std::is_same_v<typename IterT::Type, T>>* = nullptr>
    ALT_FORCE_INLINE IteratorInterface(IterT& iter) noexcept
        : _IterDetails::IteratorInterfaceImpl<IteratorInterface, T, TraitsT>(iter)...
    {
        m_pIterObj = reinterpret_cast<void*>(&iter);
        m_pFuncGetSizeHint = [](void* pIterObj) noexcept -> usize {
            return reinterpret_cast<const IterT*>(pIterObj)->GetSizeHint();
        };
    }

    template <template <typename> typename ...TraitsU,
        std::enable_if_t<_IterDetails::count_intersect_type_list_v<_IterDetails::type_list<TraitsU<T>...>, Traits> == sizeof...(TraitsT)>* = nullptr>
    ALT_FORCE_INLINE IteratorInterface(IteratorInterface<T, TraitsU...> iter) noexcept
        : _IterDetails::IteratorInterfaceImpl<IteratorInterface, T, TraitsT>(iter)...
        , m_pIterObj(iter.m_pIterObj)
        , m_pFuncGetSizeHint(iter.m_pFuncGetSizeHint)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return (*m_pFuncGetSizeHint)(m_pIterObj);
    }

private:

    ALT_FORCE_INLINE void* GetIterObjPtr() noexcept {
        return m_pIterObj;
    }

    ALT_FORCE_INLINE const void* GetIterObjPtr() const noexcept {
        return m_pIterObj;
    }

private:

    template <typename, template <typename> typename ...TraitsU>
    friend class IteratorInterface;

    template <typename, typename, template <typename> typename>
    friend class _IterDetails::IteratorInterfaceImpl;

    void* m_pIterObj;
    FuncPtr<usize, void*> m_pFuncGetSizeHint;
};

template <typename T>
class IteratorInterface<T>
{};

namespace _IterDetails {

template <typename SelfT, typename T>
class IteratorInterfaceImpl<SelfT, T, IIteratorForwardTrait>
    : public Adapters::IteratorForwardAdaptersImpl<true, SelfT>
{
public:

    template <typename IterT>
    ALT_FORCE_INLINE IteratorInterfaceImpl(IterT&) noexcept {

        m_pFuncNext = [](void* pSelf_) noexcept -> Optional<T> {

            SelfT* pSelf = reinterpret_cast<SelfT*>(pSelf_);
            return reinterpret_cast<IterT*>(pSelf->GetIterObjPtr())->Next();
        };

        m_pFuncAdvance = [](void* pSelf_, const usize advance) noexcept -> usize {

            SelfT* pSelf = reinterpret_cast<SelfT*>(pSelf_);
            return reinterpret_cast<IterT*>(pSelf->GetIterObjPtr())->Advance(advance);
        };
    }

    template <typename SelfU>
    ALT_FORCE_INLINE IteratorInterfaceImpl(IteratorInterfaceImpl<SelfU, T, IIteratorForwardTrait> iter) noexcept
        : m_pFuncNext(iter.m_pFuncNext)
        , m_pFuncAdvance(iter.m_pFuncAdvance)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Next() noexcept {
        return (*m_pFuncNext)(static_cast<SelfT*>(this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {
        return (*m_pFuncAdvance)(static_cast<SelfT*>(this), advance);
    }

private:

    template <typename, typename, template <typename> typename>
    friend class IteratorInterfaceImpl;

    FuncPtr<Optional<T>, void*> m_pFuncNext;
    FuncPtr<usize, void*, usize> m_pFuncAdvance;
};

template <typename SelfT, typename T>
class IteratorInterfaceImpl<SelfT, T, IIteratorBackwardTrait>
    : public Adapters::IteratorBackwardAdaptersImpl<true, SelfT>
{
public:

    template <typename IterT>
    ALT_FORCE_INLINE IteratorInterfaceImpl(IterT&) noexcept {

        m_pFuncNextBack = [](void* pSelf_) noexcept -> Optional<T> {

            SelfT* pSelf = reinterpret_cast<SelfT*>(pSelf_);
            return reinterpret_cast<IterT*>(pSelf->GetIterObjPtr())->NextBack();
        };

        m_pFuncAdvanceBack = [](void* pSelf_, const usize advance) noexcept -> usize {

            SelfT* pSelf = reinterpret_cast<SelfT*>(pSelf_);
            return reinterpret_cast<IterT*>(pSelf->GetIterObjPtr())->AdvanceBack(advance);
        };
    }

    template <typename SelfU>
    ALT_FORCE_INLINE IteratorInterfaceImpl(IteratorInterfaceImpl<SelfU, T, IIteratorBackwardTrait> iter) noexcept
        : m_pFuncNextBack(iter.m_pFuncNextBack)
        , m_pFuncAdvanceBack(iter.m_pFuncAdvanceBack)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE Optional<T> Back() noexcept {
        return (*m_pFuncNextBack)(static_cast<SelfT*>(this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {
        return (*m_pFuncAdvanceBack)(static_cast<SelfT*>(this), advance);
    }

private:

    template <typename, typename, template <typename> typename>
    friend class IteratorInterfaceImpl;

    FuncPtr<Optional<T>, void*> m_pFuncNextBack;
    FuncPtr<usize, void*, usize> m_pFuncAdvanceBack;
};

template <typename SelfT, typename T>
class IteratorInterfaceImpl<SelfT, T, IIteratorExactSizeTrait> {
public:

    template <typename IterT>
    ALT_FORCE_INLINE IteratorInterfaceImpl(IterT&) noexcept {

        m_pFuncGetSize = [](const void* pSelf_) noexcept -> usize {

            const SelfT* pSelf = reinterpret_cast<const SelfT*>(pSelf_);
            return reinterpret_cast<const IterT*>(pSelf->GetIterObjPtr())->GetSize();
        };

        m_pFuncIsEmpty = [](const void* pSelf_) noexcept -> bool {

            const SelfT* pSelf = reinterpret_cast<const SelfT*>(pSelf_);
            return reinterpret_cast<const IterT*>(pSelf->GetIterObjPtr())->IsEmpty();
        };
    }

    template <typename SelfU>
    ALT_FORCE_INLINE IteratorInterfaceImpl(IteratorInterfaceImpl<SelfU, T, IIteratorExactSizeTrait> iter) noexcept
        : m_pFuncGetSize(iter.m_pFuncGetSize)
        , m_pFuncIsEmpty(iter.m_pFuncIsEmpty)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {
        return (*m_pFuncGetSize)(static_cast<const SelfT*>(this));
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return (*m_pFuncIsEmpty)(static_cast<const SelfT*>(this));
    }

private:

    template <typename, typename, template <typename> typename>
    friend class IteratorInterfaceImpl;

    FuncPtr<usize, const void*> m_pFuncGetSize;
    FuncPtr<bool, const void*> m_pFuncIsEmpty;
};

} // namespace _IterDetails
} // namespace Alt

#endif // __ALT_STD_ITERATOR_INTERFACE__