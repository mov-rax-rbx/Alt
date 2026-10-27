#pragma once

#ifndef __ALT_STD_PTR__
#define __ALT_STD_PTR__

#include "Ref.h"

namespace Alt {

namespace _RefDetails {

template <typename T>
class Ptr;

} // namespace _RefDetails

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<_RefDetails::Ptr<T>>
    : is_trivially_reallocatable_t<T*>
{};

} // namespace TypeTraits

namespace _RefDetails {

/** \cond */
namespace PtrDetails {

template <typename T, bool IS_VOID = std::is_void_v<T>>
struct DeleteOperatorTReference {

    operator T&() noexcept = delete;
    operator const T&() const noexcept = delete;
};

template <typename T>
struct DeleteOperatorTReference<T, true>
{};

} // namespace PtrDetails
/** \endcond */

/**
 * @brief A pointer implementation designed for types inheriting from `Alt::Ref`.
 * 
 * @details Unsafe, because inherited from `Alt::Ref` and if 'Alt::_RefDetails::Ptr' is nullptr this breaks `Alt::Ref` invariant.
 * Used to store reference as "optional reference" (pointer) and achive 'Alt::Ref' behavior (combination of reference and `MaybeUninit`).
 * 
 * @tparam T The type of the object.
 * 
 * Alt::String val;
 * 
 * Alt::Ref<Alt::String> ref0 = val;
 * 
 * Alt::_RefDetails::Ptr<Alt::String> ptr0 = ref0;
 * const Alt::_RefDetails::Ptr<Alt::String> ptr1 = ref0;
 * Alt::_RefDetails::Ptr<const Alt::String> ptr2 = ref0;
 * 
 * Alt::_RefDetails::Ptr<Alt::String> ptr3 = &val;
 * //Alt::_RefDetails::Ptr<Alt::String> ptr4 = val; // allowed, because we want to shadow `Ptr` with `Ref` in storages like `Optional`
 * 
 * ptr2 = ref0;
 * ptr0 = ptr1;
 * ptr0 = ref0;
 * 
 * //ref0 = ptr0; // compilation error
 * 
 * Alt::Ref<Alt::String> ref1 = *ptr0;
 * 
 * ptr0 = val; // allowed - just change pointer
 * //ptr0 = ""; // compilation error
 */
template <typename T>
class Ptr
    : public Ref<T>
    , public PtrDetails::DeleteOperatorTReference<T>
{
public:

    using Type = T;

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T*, U*>>* = nullptr>
    Ptr(U&&) noexcept = delete;

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U*, T*>>* = nullptr>
    Ptr& operator=(U&&) noexcept = delete;

    ALT_FORCE_INLINE Ptr() noexcept
        : Ref<T>(nullptr)
    {}

    ALT_FORCE_INLINE Ptr(T* ptr) noexcept
        : Ref<T>(ptr)
    {}

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T*, U*>>* = nullptr>
    ALT_FORCE_INLINE Ptr(U& rhs) noexcept
        : Ref<T>(rhs)
    {}

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U*, T*>>* = nullptr>
    ALT_FORCE_INLINE Ptr& operator=(U& rhs) noexcept {

        this->m_Ptr = &rhs;
        return *this;
    }

    ALT_FORCE_INLINE Ptr& operator=(T* ptr) noexcept {

        this->m_Ptr = ptr;
        return *this;
    }

    ALT_FORCE_INLINE Ptr(Ptr&&) noexcept = default;
    ALT_FORCE_INLINE Ptr& operator=(Ptr&&) noexcept = default;

    ALT_FORCE_INLINE Ptr(const Ptr&) noexcept = default;
    ALT_FORCE_INLINE Ptr& operator=(const Ptr&) noexcept = default;

    ALT_FORCE_INLINE bool operator==(const T* ptr) const noexcept { return this->m_Ptr == ptr; }
    ALT_FORCE_INLINE bool operator!=(const T* ptr) const noexcept { return this->m_Ptr != ptr; }

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T*, U*>>* = nullptr>
    ALT_FORCE_INLINE Ptr(const Ptr<U>& ptr) noexcept
        : Ref<T>(ptr.GetPtr())
    {}

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U*, T*>>* = nullptr>
    ALT_FORCE_INLINE Ptr& operator=(const Ptr<U>& ptr) noexcept {

        *this = ptr.GetPtr();
        return *this;
    }

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T*, U*>>* = nullptr>
    ALT_FORCE_INLINE Ptr(const Ref<U>& ref) noexcept
        : Ref<T>(ref)
    {}

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U*, T*>>* = nullptr>
    ALT_FORCE_INLINE Ptr& operator=(const Ref<U>& ref) noexcept {

        static_cast<Ref<T>*>(this)->operator=(ref);
        return *this;
    }

    ALT_FORCE_INLINE operator T*() noexcept {
        return this->m_Ptr;
    }

    ALT_FORCE_INLINE operator const T*() const noexcept {
        return this->m_Ptr;
    }

    ALT_FORCE_INLINE T* GetPtr() noexcept {
        return this->m_Ptr;
    }

    ALT_FORCE_INLINE const T* GetPtr() const noexcept {
        return this->m_Ptr;
    }
};

} // namespace _RefDetails
} // namespace Alt

#endif // __ALT_STD_PTR__