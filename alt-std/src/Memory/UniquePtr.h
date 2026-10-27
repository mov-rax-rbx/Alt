#pragma once

#ifndef __ALT_STD_UNIQUE_PTR__
#define __ALT_STD_UNIQUE_PTR__

#include "../Memory/Impl/MemoryPreInclude.h"

namespace Alt {

template <typename T, typename Deleter>
class UniquePtr;

namespace TypeTraits {

template <typename T, typename Deleter>
struct __is_smart_pointer__<UniquePtr<T, Deleter>>
    : std::true_type
{};

template <typename T, typename Deleter>
struct __is_trivially_reallocatable__<UniquePtr<T, Deleter>>
    : std::conjunction<is_trivially_reallocatable_t<T*>, is_trivially_reallocatable_t<Deleter>>
{};

} // namespace TypeTraits

/**
 * @brief Default deleter for single objects.
 * @details Uses the standard 'DeallocateObject(...)' to free memory paired with 'AllocateObject(...)'.
 */
struct UniquePtrDefaultDeleter {

    template <typename T>
    ALT_FORCE_INLINE void operator()(T* ptr) const noexcept {
        Memory::DeallocateObject(ptr);
    }
};

/**
 * @brief Default deleter for arrays of a fixed size.
 * @tparam SIZE The number of elements in the array to be deallocated paired with 'AllocateArrayObject(...)'.
 */
template <usize SIZE>
struct UniquePtrDefaultArrayDeleter {

    template <typename T>
    ALT_FORCE_INLINE void operator()(T* ptr) const noexcept {
        Memory::DeallocateArrayObject(ptr, SIZE);
    }
};

/**
 * @brief A smart pointer that provides exclusive ownership of a dynamically allocated object.
 * @tparam T The type of the object being managed.
 * @tparam Deleter The functor type used to destroy the object. Must be move_constructible and move_assignable.
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::UniquePtr<usize> ptr = Alt::UniquePtr<usize>::AllocateObject(4) iferr_return;
 * assert(*ptr == 4);
 * 
 * // Alt::UniquePtr<usize> ptrCopy = ptr; // compilation error
 * Alt::UniquePtr<usize> ptrMove = std::move(ptr);
 * assert(ptrMove && !ptr);
 * 
 * struct DeleterCustom { void operator()(usize* ptr) const noexcept { Alt::Memory::DeallocateObject(ptr); } };
 * Alt::UniquePtr<usize> ptr0 = Alt::UniquePtr<usize, Custom>::AllocateObjectWithDeleter(Custom{}, 4);
 * Alt::UniquePtr<usize> ptr1 = Alt::UniquePtr<usize, Custom>::AllocateObject(4);
 */
template <typename T, typename Deleter = UniquePtrDefaultDeleter>
class UniquePtr {
private:

    static_assert(std::is_move_constructible_v<Deleter>, "");
    static_assert(std::is_move_assignable_v<Deleter>, "");

public:

    using Type = T;

    /**
     * @brief Allocates and constructs a new object of type T.
     * @tparam ArgsT Variadic pack of arguments to pass to the constructor of T.
     * @return A UniquePtr containing the newly allocated object.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::UniquePtr<Alt::String> ptr = Alt::UniquePtr<Alt::String>::AllocateObject("UniquePtr"_salloc);
     * assert(*ptr == "UniquePtr");
     */
    template <typename ...ArgsT,
        std::enable_if_t<std::is_constructible_v<T, ArgsT&&...> && std::is_default_constructible_v<Deleter>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<UniquePtr<T, Deleter>> AllocateObject(ArgsT&&... args) noexcept {

        return UniquePtr<T, Deleter>(Memory::AllocateObject<T>(std::forward<ArgsT>(args)...));
    }

    /**
     * @brief Allocates a new object using a specific custom deleter.
     * @tparam DeleterT The type of the deleter to use.
     * @tparam ArgsT Variadic pack of arguments to pass to the constructor of T.
     * @return A UniquePtr containing the newly allocated object.
     * @example
     * 
     * iferr_scope;
     * 
     * struct DeleterCustom { void operator()(usize* ptr) const noexcept { Alt::Memory::DeallocateObject(ptr); } };
     * Alt::UniquePtr<usize> ptr0 = Alt::UniquePtr<usize, Custom>::AllocateObjectWithDeleter(Custom{}, 4);
     */
    template <typename DeleterT = Deleter, typename ...ArgsT,
        std::enable_if_t<std::is_constructible_v<Deleter, DeleterT&&> && std::is_constructible_v<T, ArgsT&&...>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<UniquePtr<T, Deleter>> AllocateObjectWithDeleter(DeleterT&& deleter, ArgsT&&... args) noexcept {

        return UniquePtr<T, Deleter>(Memory::AllocateObject<T>(std::forward<ArgsT>(args)...), std::forward<DeleterT>(deleter));
    }

    /**
     * @brief Default constructor for an empty UniquePtr.
     */
    ALT_FORCE_INLINE UniquePtr() noexcept = default;

    /**
     * @brief Constructor wrapping an existing raw pointer.
     * @param ptr The raw pointer.
     */
    template <typename DeleterT = Deleter,
        std::enable_if_t<std::is_same_v<DeleterT, Deleter> && std::is_default_constructible_v<DeleterT>>* = nullptr>
    ALT_FORCE_INLINE UniquePtr(Type* ptr) noexcept
        : m_Ptr(ptr, Deleter{})
    {}

    /**
     * @brief Constructor wrapping a raw pointer with a specific deleter.
     * 
     * @param ptr The raw pointer to wrap.
     * @param deleter A custom deleter instance.
     */
    ALT_FORCE_INLINE UniquePtr(Type* ptr, Deleter&& deleter) noexcept
        : m_Ptr(ptr, std::move(deleter))
    {}

    UniquePtr(const UniquePtr&) noexcept = delete;
    UniquePtr& operator=(const UniquePtr&) noexcept = delete;

    ALT_FORCE_INLINE UniquePtr(UniquePtr&& ptr) noexcept
        : m_Ptr(Exchange(ptr.m_Ptr.ptr, nullptr), std::move(ptr.m_Ptr.AsDeleter()))
    {}

    ALT_FORCE_INLINE UniquePtr& operator=(UniquePtr&& ptr) noexcept {

        if (this == &ptr) {
            return *this;
        }

        Reset(ptr.Release());
        m_Ptr.AsDeleter() = std::move(ptr.m_Ptr.AsDeleter());

        return *this;
    }

    template <typename U,
        std::enable_if_t<std::is_base_of_v<T, U>>* = nullptr>
    ALT_FORCE_INLINE UniquePtr(UniquePtr<U, Deleter>&& ptr) noexcept
        : m_Ptr(static_cast<T*>(Exchange(ptr.m_Ptr.ptr, nullptr)), std::move(ptr.m_Ptr.AsDeleter()))
    {}

    template <typename U,
        std::enable_if_t<std::is_base_of_v<T, U>>* = nullptr>
    ALT_FORCE_INLINE UniquePtr& operator=(UniquePtr<U, Deleter>&& ptr) noexcept {

        if (this == &ptr) {
            return *this;
        }

        Reset(static_cast<T*>(ptr.Release()));
        m_Ptr.AsDeleter() = std::move(ptr.m_Ptr.AsDeleter());

        return *this;
    }

    ALT_FORCE_INLINE ~UniquePtr() noexcept {

        if (!m_Ptr.ptr) {
            return;
        }

        m_Ptr.AsDeleter()(m_Ptr.ptr);
        m_Ptr.ptr = nullptr;
    }

    ALT_FORCE_INLINE UniquePtr(std::nullptr_t) noexcept
    {}

    ALT_FORCE_INLINE UniquePtr& operator=(std::nullptr_t) noexcept {

        if (!m_Ptr.ptr) {
            return *this;
        }

        m_Ptr.AsDeleter()(m_Ptr.ptr);
        m_Ptr.ptr = nullptr;

        return *this;
    }

    /**
     * @brief Checks if the pointer is not nullptr.
     * @return True if a underlying ptr != nullptr, false otherwise.
     */
    ALT_FORCE_INLINE explicit operator bool() const noexcept {
        return m_Ptr.ptr != nullptr;
    }

    /**
     * @brief Dereference operators.
     */
    ALT_FORCE_INLINE Type& operator*() noexcept {
        return *m_Ptr.ptr;
    }

    ALT_FORCE_INLINE const Type& operator*() const noexcept {
        return *m_Ptr.ptr;
    }

    ALT_FORCE_INLINE Type* operator->() noexcept {
        return m_Ptr.ptr;
    }

    ALT_FORCE_INLINE const Type* operator->() const noexcept {
        return m_Ptr.ptr;
    }

    /**
     * @brief Returns the raw pointer.
     */
    ALT_FORCE_INLINE Type* Get() noexcept {
        return m_Ptr.ptr;
    }

    ALT_FORCE_INLINE const Type* Get() const noexcept {
        return m_Ptr.ptr;
    }

    /**
     * @brief Releases ownership of the managed object.
     * @details Sets the internal pointer to nullptr and returns the original raw pointer.
     * @return The released raw pointer.
     */
    ALT_FORCE_INLINE Type* Release() noexcept {

        return Exchange(m_Ptr.ptr, nullptr);
    }

    /**
     * @brief Replaces the object with a new one, deallocates old object.
     * @param ptr The new raw pointer.
     */
    ALT_FORCE_INLINE void Reset(Type* ptr = nullptr) noexcept {

        if (m_Ptr.ptr == ptr) {
            return;
        }

        if (m_Ptr.ptr) {
            m_Ptr.AsDeleter()(m_Ptr.ptr);
        }

        m_Ptr.ptr = ptr;
    }
    
    /**
     * @brief Comparison operators for underlying pointer.
     * @details These compare the raw memory addresses of the underlying pointers.
     */
    ALT_FORCE_INLINE bool operator==(const UniquePtr& rhs) const noexcept {
        return m_Ptr.ptr == rhs.m_Ptr.ptr;
    }

    ALT_FORCE_INLINE bool operator!=(const UniquePtr& rhs) const noexcept {
        return !(*this == rhs);
    }

    ALT_FORCE_INLINE bool operator<(const UniquePtr& rhs) const noexcept {
        return m_Ptr.ptr < rhs.m_Ptr.ptr;
    }

    ALT_FORCE_INLINE bool operator>(const UniquePtr& rhs) const noexcept {
        return rhs < *this;
    }

    ALT_FORCE_INLINE bool operator<=(const UniquePtr& rhs) const noexcept {
        return !(*this > rhs);
    }

    ALT_FORCE_INLINE bool operator>=(const UniquePtr& rhs) const noexcept {
        return !(*this < rhs);
    }

private:

    template <typename, typename>
    friend class UniquePtr;

    struct Ptr : public Deleter {

        ALT_FORCE_INLINE Ptr() noexcept = default;

        ALT_FORCE_INLINE Ptr(Type* ptr, Deleter&& deleter) noexcept
            : Deleter(std::move(deleter))
            , ptr(ptr)
        {}

        ALT_FORCE_INLINE Deleter& AsDeleter() noexcept {
            return static_cast<Deleter&>(*this);
        }

        Type* ptr = nullptr;
    };

    Ptr m_Ptr;
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter>;

} // namespace Alt

#endif // __ALT_STD_UNIQUE_PTR__