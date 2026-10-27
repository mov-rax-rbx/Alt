#pragma once

#ifndef __ALT_STD_REF__
#define __ALT_STD_REF__

#include "../TypeTraits.h"

namespace Alt {

template <typename T>
class Ref;

namespace TypeTraits {

template <typename T>
struct __is_alt_ref__<Ref<T>>
    : std::true_type
{};

template <typename T>
struct __is_trivially_reallocatable__<Ref<T>>
    : is_trivially_reallocatable_t<T*>
{};

template <typename T, bool IS_REFERENCE = std::is_reference_v<T>>
struct __unwrap_reference_to_alt_ref_or_just_type__ {

    using Type = T;
};

template <typename T>
struct __unwrap_reference_to_alt_ref_or_just_type__<T, true> {

    using Type = Ref<std::remove_reference_t<T>>;
};

template <typename T>
using unwrap_reference_to_alt_ref_or_just_type_t =
    typename __unwrap_reference_to_alt_ref_or_just_type__<T>::Type;

template <typename T>
using remove_reference_or_alt_ref_t =
    std::conditional_t<
        std::is_reference_v<T>,
        std::remove_reference_t<T>,
        unwrap_reference_to_alt_ref_or_just_type_t<T>
    >;

} // namespace TypeTraits

namespace _RefDetails {

template <typename T>
class Ptr;

} // namespace _RefDetails

/**
 * @brief A non-owning reference wrapper for type T.
 * 
 * @details Provides a way to handle references with a syntax similar to references, 
 * while ensuring that the underlying object stored as pointer.
 * 
 * Velue construction binds reference to internal pointer.
 * Value assignemnt overwrites value inside reference.
 * 
 * Ref construction binds reference to internal pointer.
 * Ref assignment binds reference to internal pointer.
 * 
 * @tparam T The type of the object being referenced.
 * 
 * @example
 * 
 * usize value = 4;
 * Alt::Ref<usize> ref = value;
 * assert(ref == 4);
 * 
 * usize value2 = 48;
 * Alt::Ref<usize> ref2 = value2;
 * 
 * ref2 = value;
 * assert(ref2 == 4);
 */
template <typename T>
class Ref {
public:

    using Type = T;

    Ref(const _RefDetails::Ptr<T>&) noexcept = delete;
    Ref& operator=(const _RefDetails::Ptr<T>&) noexcept = delete;

    ALT_FORCE_INLINE Ref(Ref&&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(Ref&&) noexcept = default;

    ALT_FORCE_INLINE Ref(const Ref&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(const Ref&) noexcept = default;

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T*, U*>>* = nullptr>
    Ref(U&&) noexcept = delete;

    /**
     * @brief Constructs a Ref from a value of type U.
     * 
     * @tparam U The type of the value. Must be such that T* is constructible from U*.
     * @param val The value to wrap.
     * @example
     * 
     * usize val = 4;
     * Alt::Ref<usize> ref = val;
     * 
     * assert(ref == val);
     */
    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T*, U*>>* = nullptr>
    ALT_FORCE_INLINE Ref(U& val) noexcept
        : m_Ptr(&val)
    {}

    /**
     * @brief Assigns a value of type U to the underlying object.
     * 
     * @tparam U The type of the value. Must be convertible to T.
     * @param val The value to assign.
     * @return A reference to this Ref instance.
     * @example
     * 
     * usize x = 4;
     * 
     * Alt::Ref<usize> ref = x;
     * 
     * ref = 8; 
     * assert(x == 8);
     */
    template <typename U = T,
        std::enable_if_t<std::is_assignable_v<T&, U&&>>* = nullptr>
    ALT_FORCE_INLINE Ref& operator=(U&& val) noexcept {

        *m_Ptr = std::forward<U>(val);
        return *this;
    }

    /**
     * @brief Constructs a Ref from another Ref.
     * 
     * @tparam U The type of the source Ref. Must be such that T* is constructible from U*.
     * @param ref The source reference to wrap.
     * @example
     * 
     * usize x = 4;
     * 
     * Alt::Ref<usize> source = x;
     * Alt::Ref<usize> target = source;
     * 
     * assert(&target.GetRef() == &x);
     */
    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T*, U*>>* = nullptr>
    ALT_FORCE_INLINE Ref(const Ref<U>& ref) noexcept
        : m_Ptr(&ref.GetRef())
    {}

    /**
     * @brief Assignes Ref to another Ref.
     * 
     * @tparam U The type of the source Ref. Must be such that T* is convertible from U*.
     * @param ref The source reference to point to.
     * @return A reference to this Ref instance.
     * @example
     * 
     * usize x = 0;
     * usize y = 1;
     * 
     * Alt::Ref<usize> ref = x;
     * Alt::Ref<usize> other = y;
     * 
     * ref = other;
     * assert(&ref.GetRef() == &y);
     */
    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U*, T*>>* = nullptr>
    ALT_FORCE_INLINE Ref& operator=(const Ref<U>& ref) noexcept {

        m_Ptr = &ref.GetRef();
        return *this;
    }

    /**
     * @brief Implicit conversion to reference.
     * 
     * @return A reference to the underlying object.
     * @example
     * 
     * usize x = 4;
     * 
     * Alt::Ref<usize> ref = x;
     * 
     * usize& val = ref; 
     * assert(val == 4);
     */
    ALT_FORCE_INLINE operator T&() noexcept {
        return *m_Ptr;
    }

    ALT_FORCE_INLINE operator const T&() const noexcept {
        return *m_Ptr;
    }

    /**
     * @brief Accesses the underlying object via dereferencing.
     * 
     * @return A reference to the underlying object.
     * @example
     * 
     * usize x = 4;
     * Alt::Ref<usize> ref = x;
     * assert(ref == 4);
     * assert(*ref == 4);
     */
    ALT_FORCE_INLINE T& operator*() noexcept { return *m_Ptr; }
    ALT_FORCE_INLINE const T& operator*() const noexcept { return *m_Ptr; }

    /**
     * @brief Accesses members of the underlying object.
     * 
     * @return A pointer to the underlying object.
     * @example
     * 
     * Alt::String str = "Ref";
     * Alt::Ref<Alt::String> ref = str;
     * 
     * assert(ref->size() == 3);
     */
    ALT_FORCE_INLINE T* operator->() noexcept { return m_Ptr; }
    ALT_FORCE_INLINE const T* operator->() const noexcept { return m_Ptr; }

    /**
     * @brief Retrieves the underlying object as a reference.
     * 
     * @return A reference to the underlying object.
     * @example
     * 
     * usize x = 4;
     * Alt::Ref<usize> ref = x;
     * 
     * assert(ref == 4);
     * assert(*ref == 4);
     * assert(ref.GetRef() == 4);
     * assert(&ref.GetRef() == &x);
     */
    ALT_FORCE_INLINE T& GetRef() noexcept {
        return *m_Ptr;
    }

    ALT_FORCE_INLINE const T& GetRef() const noexcept {
        return *m_Ptr;
    }

private:

    template <typename>
    friend class _RefDetails::Ptr;

    Ref(T* ptr) noexcept
        : m_Ptr(ptr)
    {}

private:

    T* m_Ptr;
};

/**
* Needs for 'Alt::_RefDetails::Ptr<void>', as 'Ptr' inherited from 'Alt::Ref'
*/
template <>
class Ref<void> {
public:

    using Type = void;

    Ref(const _RefDetails::Ptr<void>&) noexcept = delete;
    Ref& operator=(const _RefDetails::Ptr<void>&) noexcept = delete;

    ALT_FORCE_INLINE Ref(Ref&&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(Ref&&) noexcept = default;

    ALT_FORCE_INLINE Ref(const Ref&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(const Ref&) noexcept = default;

    ALT_FORCE_INLINE void* operator->() noexcept { return m_Ptr; }
    ALT_FORCE_INLINE const void* operator->() const noexcept { return m_Ptr; }

private:

    template <typename>
    friend class _RefDetails::Ptr;

    Ref(void* ptr) noexcept
        : m_Ptr(ptr)
    {}

private:

    void* m_Ptr;
};

template <>
class Ref<const void> {
public:

    using Type = const void;

    Ref(const _RefDetails::Ptr<const void>&) noexcept = delete;
    Ref& operator=(const _RefDetails::Ptr<const void>&) noexcept = delete;

    ALT_FORCE_INLINE Ref(Ref&&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(Ref&&) noexcept = default;

    ALT_FORCE_INLINE Ref(const Ref&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(const Ref&) noexcept = default;

    ALT_FORCE_INLINE const void* operator->() const noexcept { return m_Ptr; }

private:

    template <typename>
    friend class _RefDetails::Ptr;

    Ref(const void* ptr) noexcept
        : m_Ptr(ptr)
    {}

private:

    const void* m_Ptr;
};

template <>
class Ref<volatile void> {
public:

    using Type = volatile void;

    Ref(const _RefDetails::Ptr<volatile void>&) noexcept = delete;
    Ref& operator=(const _RefDetails::Ptr<volatile void>&) noexcept = delete;

    ALT_FORCE_INLINE Ref(Ref&&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(Ref&&) noexcept = default;

    ALT_FORCE_INLINE Ref(const Ref&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(const Ref&) noexcept = default;

    ALT_FORCE_INLINE volatile void* operator->() noexcept { return m_Ptr; }
    ALT_FORCE_INLINE const volatile void* operator->() const noexcept { return m_Ptr; }

private:

    template <typename>
    friend class _RefDetails::Ptr;

    Ref(volatile void* ptr) noexcept
        : m_Ptr(ptr)
    {}

private:

    volatile void* m_Ptr;
};

template <>
class Ref<const volatile void> {
public:

    using Type = const volatile void;

    Ref(const _RefDetails::Ptr<const volatile void>&) noexcept = delete;
    Ref& operator=(const _RefDetails::Ptr<const volatile void>&) noexcept = delete;

    ALT_FORCE_INLINE Ref(Ref&&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(Ref&&) noexcept = default;

    ALT_FORCE_INLINE Ref(const Ref&) noexcept = default;
    ALT_FORCE_INLINE Ref& operator=(const Ref&) noexcept = default;

    ALT_FORCE_INLINE const volatile void* operator->() noexcept { return m_Ptr; }

private:

    template <typename>
    friend class _RefDetails::Ptr;

    Ref(const volatile void* ptr) noexcept
        : m_Ptr(ptr)
    {}

private:

    const volatile void* m_Ptr;
};

} // namespace Alt

#endif // __ALT_STD_REF__