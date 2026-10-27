#pragma once

#ifndef __ALT_STD_VECTOR__
#define __ALT_STD_VECTOR__

#include "../IfErr.h"
#include "../TypeTraits.h"

#include "../Algorithms/Algorithms.h"

#include "../Memory/Memory.h"

#include "../Utils/Num.h"
#include "../Utils/Slice.h"
#include "../Utils/Optional.h"
#include "../Utils/ScopeDefer.h"

#include "VectorIterator.h"

namespace Alt {

template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE = 16>
class Vector;

namespace TypeTraits {

template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
struct __is_trivially_reallocatable__<Vector<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>>
    : is_trivially_reallocatable_t<T*>
{};

} // namespace TypeTraits

/**
 * @brief A dynamic array container that manages a contiguous block of memory.
 * @details Memory is automatically reallocated using a power of two growth strategy.
 * 
 * @tparam T_ The type of the elements stored in the vector.
 * @tparam MIN_ELEMENT_CAPACITY_ON_ALLOCATE The initial capacity when allocating. Must be a power of 2.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocateCapacity(2);
 * vec.Push(10) iferr_return;
 * vec.Push(20) iferr_return;
 * 
 * assert(vec.GetSize() == 2);
 * assert(vec[0] == 10);
 */
template <typename T_, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
class Vector {
private:

    using T = TypeTraits::clean_t<T_>;

    template <typename U>
    using is_pushable_t = std::disjunction<TypeTraits::is_only_clonable_to_from_t<T, U>, std::is_constructible<T, U>>;

    template <typename U>
    constexpr static bool is_pushable_v = is_pushable_t<U>::value;

    template <typename ...ArgsT>
    constexpr static bool is_populated_v = sizeof...(ArgsT) != 0;

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

    static_assert(MIN_ELEMENT_CAPACITY_ON_ALLOCATE == 0 || Num::IsPowerOf2(MIN_ELEMENT_CAPACITY_ON_ALLOCATE), "");

public:

    using Type = T;

    ALT_FORCE_INLINE constexpr static usize GetMinElementCapacityOnAllocate() noexcept {

        return MIN_ELEMENT_CAPACITY_ON_ALLOCATE;
    }

    /**
     * @brief Allocates a Vector of a specific size, filled with a default value.
     * @param size The number of elements to initialize.
     * @return An Alt::Optional containing the Vector, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::Allocate(4) iferr_return;
     * assert(vec.GetSize() == 4);
     * assert(vec[0] == 0);
     * assert(vec[1] == 0);
     */
    ALT_NODISCARD static Optional<Vector> Allocate() noexcept {
        iferr_scope;

        Vector result{};
        result.Reserve(MIN_ELEMENT_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;

        return result;
    }

    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD static Optional<Vector> Allocate(const usize size) noexcept {
        iferr_scope;

        Vector result{};

        if (size < MIN_ELEMENT_CAPACITY_ON_ALLOCATE) {

            result.Reserve(MIN_ELEMENT_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
        }

        result.Resize(size) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a Vector of a specific size, filled with a provided value.
     * @tparam U The type of the value. Must be constructible to T.
     * @param size The number of elements to initialize.
     * @param val The value used to fill the vector.
     * @return An Alt::Optional containing the Vector, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::Allocate(4, 44) iferr_return;
     * assert(vec.GetSize() == 4);
     * assert(vec[0] == 44);
     * assert(vec[1] == 44);
     */
    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD static Optional<Vector> Allocate(const usize size, const U& val) noexcept {
        iferr_scope;

        Vector result{};

        if (size < MIN_ELEMENT_CAPACITY_ON_ALLOCATE) {

            result.Reserve(MIN_ELEMENT_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
        }

        result.Resize(size, val) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a Vector with a pre-defined capacity.
     * @param size The desired capacity.
     * @return An Alt::Optional containing the Vector, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocateCapacity(44) iferr_return;
     * assert(vec.GetCapacity() >= 44);
     */
    ALT_NODISCARD static Optional<Vector> AllocateCapacity(const usize size) noexcept {
        iferr_scope;

        Vector result{};
        result.Reserve(Num::Max(size, MIN_ELEMENT_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a Vector with a exact pre-defined capacity.
     * @param size The desired capacity.
     * @return An Alt::Optional containing the Vector, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocateCapacityExact(44) iferr_return;
     * assert(vec.GetCapacity() == 44);
     */
    ALT_NODISCARD static Optional<Vector> AllocateCapacityExact(const usize size) noexcept {
        iferr_scope;

        Vector result{};
        result.ReserveExact(Num::Max(size, MIN_ELEMENT_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a Vector initialized with a variadic list of arguments.
     * @tparam ArgsT... The types of the arguments to be pushed into the vector.
     * @param args The values to initialize the vector with.
     * @return An Alt::Optional containing the initialized Vector, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(1, 2, 3, 4) iferr_return;
     * assert(vec.GetValue().GetSize() == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    ALT_NODISCARD static Optional<Vector> AllocatePush(ArgsT&&... args) noexcept {
        iferr_scope;

        constexpr usize size = sizeof...(ArgsT);

        Vector result{};
        result.Reserve(Num::Max(size, MIN_ELEMENT_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        PushVariadicImpl(result, std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return result;
    }

    ALT_FORCE_INLINE Vector() noexcept = default;

    Vector(const Vector&) noexcept = delete;
    Vector& operator=(const Vector&) noexcept = delete;

    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    Vector(Vector<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& rhs) noexcept
        : Vector(Rule5Move, rhs)
    {}

    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    Vector& operator=(Vector<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~Vector() noexcept {

        Destroy();
    }

    /**
     * @brief Returns the number of elements currently in the vector.
     * @return The size of the vector.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        return m_pMemoryEnd - m_pMemoryBegin;
    }

    /**
     * @brief Returns the total number of elements the vector can hold before a reallocation is required.
     * @return The capacity of the vector.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetCapacity() const noexcept {

        return m_pCapacityEnd - m_pMemoryBegin;
    }

    /**
     * @brief Checks if the vector contains no elements.
     * @return True if empty, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        return m_pMemoryEnd == m_pMemoryBegin;
    }

    /**
     * @brief Checks if the vector contains at least one element.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept {

        return m_pMemoryEnd != m_pMemoryBegin;
    }

    /**
     * @brief Removes all elements from the vector.
     * @details Does not deallocate the underlying memory, but destroys all existing objects.
     */
    void Clear() noexcept {

        Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryEnd);
        m_pMemoryEnd = m_pMemoryBegin;
    }

    /**
     * @brief Accesses the element at the specified index.
     * @param i The index of the element.
     * @return A reference to the element.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * assert(vec[0] == 0);
     * assert(vec[4] == 16);
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        return m_pMemoryBegin[i];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize i) const noexcept {
        ALT_ASSERT(i < GetSize());

        return m_pMemoryBegin[i];
    }

    /**
     * @brief Returns a reference to the first element.
     * @return Reference to the first element.
     * @note Asserts that the vector is not empty.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetFirst() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pMemoryBegin;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetFirst() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pMemoryBegin;
    }

    /**
     * @brief Returns a reference to the last element.
     * @return Reference to the last element.
     * @note Asserts that the vector is not empty.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetLast() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(m_pMemoryEnd - 1);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetLast() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(m_pMemoryEnd - 1);
    }

    /**
     * @brief Returns a pointer to the beginning.
     * @return Pointer to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T* Data() noexcept {
        return m_pMemoryBegin;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T* Data() const noexcept {
        return m_pMemoryBegin;
    }

    /**
     * @brief Returns a raw pointer to the end of the vector.
     * @return Pointer to the next after last element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T* DataEnd() noexcept {

        return m_pMemoryEnd;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T* DataEnd() const noexcept {

        return m_pMemoryEnd;
    }

    /**
     * @brief Returns a mutable iterator for the vector.
     * @return An `VectorIterator`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE VectorIterator<T> GetIterator() noexcept {

        return VectorIterator<T>{ *this };
    }

    /**
     * @brief Returns a constant iterator for the vector.
     * @return An `VectorIteratorConst`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE VectorIteratorConst<T> GetIterator() const noexcept {

        return VectorIteratorConst<T>{ *this };
    }

    /**
     * Clonable Trait
     * 
     * @brief A deep copy current vector to another.
     * 
     * @details Return false when allocation or copy fails. On allocation there
     * are nothing changed in internal state, but on copy the Vector contains
     * valid intermediate state of copying.
     * 
     * @return An true if copy success, or false if allocation or copy fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec0 = Alt::Vector<usize>::AllocatePush(4) iferr_return;
     * 
     * Alt::Vector<usize> vec1;
     * vec1.CloneFrom(vec0) iferr_return;
     * 
     * assert(vec1[0] == vec0[0]);
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || (std::is_assignable_v<T&, const U&> && std::is_constructible_v<T, const U&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(Vector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& vector) noexcept {

        if (static_cast<const void*>(this) == &vector) {
            return true;
        }

        const Slice<const T> slice = vector;
        return CloneFrom(slice);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || (std::is_assignable_v<T&, const U&> && std::is_constructible_v<T, const U&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const Vector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& vector) noexcept {

        if (static_cast<const void*>(this) == &vector) {
            return true;
        }

        const Slice<const T> slice = vector;
        return CloneFrom(slice);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const InnerU&> || (std::is_assignable_v<T&, const InnerU&> && std::is_constructible_v<T, const InnerU&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(U&& rhs) noexcept {
        iferr_scope;

        const Slice<const T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sliceSize = slice.GetSize();

        Reserve(sliceSize) iferr_return false;

        const usize sizeToClone = Num::Min(sliceSize, size);

        Memory::CloneFromFirstElementResult<T> result = Memory::CloneObjectsFromFirstElementNonOverlap(
            m_pMemoryBegin, slice.Data(), slice.Data() + sizeToClone
        );

        Memory::DestroyObjects(result.pDstObjectWhereCloneFailedOrEnd, m_pMemoryEnd);

        if (!result.isOk) {

            m_pMemoryEnd = result.pDstObjectWhereCloneFailedOrEnd;
            return false;
        }

        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            result.pDstObjectWhereCloneFailedOrEnd, slice.Data() + sizeToClone, slice.DataEnd()
        );

        m_pMemoryEnd = result.pDstObjectWhereCloneFailedOrEnd;

        return result.isOk;
    }

    ALT_NODISCARD bool CloneFrom(const T* pBegin, const usize size) noexcept {

        const Slice<const T> slice{ pBegin, size };
        return CloneFrom(slice);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const Vector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& rhs) noexcept {
        ALT_ASSERT(static_cast<const void*>(this) != &rhs);

        iferr_scope_on_err {

            Memory::DestroyObject(*this);
        };

        Memory::ConstructDefaultObject<Vector>(this);
        AppendClone(rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD Optional<Vector> Clone() const noexcept {
        iferr_scope;

        MaybeUninit<Vector> valueToConstruct;
        valueToConstruct.GetPtr()->CloneConstructFrom(*this) iferr_return Nullopt;

        defer {
            valueToConstruct.Destroy();
        };

        return std::move(valueToConstruct).GetVal();
    }

    /**
     * @brief A moves eaach element if current vector to another.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec0 = Alt::Vector<usize>::AllocatePush(4) iferr_return;
     * 
     * Alt::Vector<usize> vec1;
     * vec1.MoveFrom(vec0) iferr_return;
     * 
     * assert(vec1[0] == 4);
     * assert(!vec1.IsEmpty()); // size still valid - just move elements
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool MoveFrom(Vector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& vector) noexcept {

        if (static_cast<const void*>(this) == &vector) {
            return true;
        }

        Slice<T> slice = vector;
        return MoveFrom(slice);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool MoveFrom(Vector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& vector) noexcept {

        if (static_cast<const void*>(this) == &vector) {
            return true;
        }

        Slice<T> slice = vector;
        return MoveFrom(slice);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&std::is_assignable_v<T&, InnerU&&> && std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool MoveFrom(U&& rhs) noexcept {
        iferr_scope;

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sliceSize = slice.GetSize();

        Reserve(sliceSize) iferr_return false;

        const usize sizeToMove = Num::Min(sliceSize, size);

        Memory::MoveFromFirstElementResult<T> result = Memory::MoveObjectsFromFirstElementNonOverlap(
            m_pMemoryBegin, slice.Data(), slice.Data() + sizeToMove
        );

        Memory::DestroyObjects(result.pDstEnd, m_pMemoryEnd);

        result = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
            result.pDstEnd, slice.Data() + sizeToMove, slice.DataEnd()
        );

        m_pMemoryEnd = result.pDstEnd;

        return true;
    }

    ALT_NODISCARD bool MoveFrom(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return MoveFrom(slice);
    }

    /**
     * @brief Append copy elements to the current vector end.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3) iferr_return;
     * vec.AppendClone(vec.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(vec[0] == vec[4]);
     * assert(vec[1] == vec[5]);
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendClone(Vector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& vector) noexcept {

        const Slice<const T> slice = vector;
        return AppendClone(slice);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendClone(const Vector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& vector) noexcept {

        const Slice<const T> slice = vector;
        return AppendClone(slice);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> && (TypeTraits::is_only_clonable_to_from_v<T, const InnerU&> || std::is_constructible_v<T, const InnerU&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendClone(U&& rhs) noexcept {
        iferr_scope;

        const Slice<const T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize sizeToAppend = slice.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        ReserveAddition(sizeToAppend) iferr_return false;

        const Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            m_pMemoryEnd, slice.Data(), slice.DataEnd()
        );

        m_pMemoryEnd = result.pDstObjectWhereCloneFailedOrEnd;

        return result.isOk;
    }

    ALT_NODISCARD bool AppendClone(const T* pBegin, const usize size) noexcept {

        const Slice<const T> slice{ pBegin, size };
        return AppendClone(slice);
    }

    /**
     * @brief Append move elements to the current vector end.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<Alt::String> vec = Alt::Vector<Alt::String>::AllocatePush("0"_salloc, "1"_salloc, "2"_salloc, "3"_salloc) iferr_return;
     * vec.AppendMove(vec.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(vec[0] == "0");
     * assert(vec[4] == "0");
     */
    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendMove(Vector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>&& vector) noexcept {

        Slice<T> slice = vector;
        return AppendMove(slice);
    }

    template <typename U = T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendMove(Vector<U, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& vector) noexcept {

        Slice<T> slice = vector;
        return AppendMove(slice);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<InnerU>, U&&> &&
            std::is_same_v<InnerU, T> && std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool AppendMove(U&& rhs) noexcept {
        iferr_scope;

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize sizeToAppend = slice.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        ReserveAddition(sizeToAppend) iferr_return false;

        const Memory::MoveFromFirstElementResult<T> result = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
            m_pMemoryEnd, slice.Data(), slice.DataEnd()
        );

        m_pMemoryEnd = result.pDstEnd;

        return true;
    }

    ALT_NODISCARD bool AppendMove(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return AppendMove(slice);
    }

    /**
     * @brief Appends an element to the end of the vector.
     * @param val The value to push.
     * @return True if the element was successfully added, false if memory allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec;
     * vec.Push(0) iferr_return;
     * 
     * assert(vec[0] == 0);
     */
    template <typename U = T,
        std::enable_if_t<is_pushable_v<U&&>>* = nullptr>
    ALT_FORCE_INLINE bool Push(U&& val) noexcept {
        iferr_scope;

        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryEnd, &val, &val + 1));

        ReserveAddition(1) iferr_return false;

        T* pResult = Memory::CloneConstructOrMoveConstructObject(m_pMemoryEnd, std::forward<U>(val));

        const bool isOk = pResult != m_pMemoryEnd;
        m_pMemoryEnd = pResult;

        return isOk;
    }

    /**
     * @brief Variadic append an elements to the end of the vector.
     * @param ...vals The values to push.
     * @return True if the element was successfully added, false if memory allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec;
     * vec.Push(0, 1, 2, 3) iferr_return;
     * 
     * assert(vec[0] == 0);
     * assert(vec.GetSize() == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    bool Push(ArgsT&&... vals) noexcept {
        iferr_scope;

        constexpr usize sizeToPush = sizeof...(ArgsT);
        ReserveAddition(sizeToPush) iferr_return false;

        PushVariadicImpl(*this, std::forward<ArgsT>(vals)...) iferr_return false;

        return true;
    }

    /**
     * @brief Removes and returns the last element of the vector.
     * @return The element that was removed.
     * @note This method requires the vector to not be empty. Calling on an empty vector triggers an assertion.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 2, 4) iferr_return;
     * usize val = vec.Pop();
     * assert(val == 4);
     */
    ALT_FORCE_INLINE T Pop() noexcept {
        ALT_ASSERT(!IsEmpty());

        m_pMemoryEnd -= 1;
        return Memory::MoveDestructibleObject(*m_pMemoryEnd);
    }

    /**
     * @brief Inserts an element at the specified index.
     * @tparam U The type of the value. Must be convertible to T.
     * @param i The index at which to insert the element.
     * @param val The value to insert.
     * @return True if the insertion was successful, false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 2, 4) iferr_return;
     * vec.Insert(1, 10) iferr_return;
     * 
     * assert(vec[1] == 10);
     * assert(vec[2] == 2);
     */
    template <typename U = T,
        std::enable_if_t<(TypeTraits::is_only_clonable_to_from_v<T, U&&> || (std::is_assignable_v<T&, U&&> && std::is_constructible_v<T, U&&>)) && std::is_default_constructible_v<T>>* = nullptr>
    ALT_NODISCARD bool Insert(const usize i, U&& val) noexcept {

        return InsertImpl(i, std::forward<U>(val));
    }

    /**
     * @brief Inserts an elements from iterator at the specified index.
     * @param i The index at which to insert the elements.
     * @param iterator The iterator to insert.
     * @return True if the insertion was successful, false otherwise.
     * 
     * @details Return false when allocation or fails. On allocation there
     * are nothing changed in internal state, and the state of Vector stay
     * valid as before 'InsertIterator...()' call. The clone version not
     * implemented because of complex state restoring on clone error.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<Alt::String> vec0 = Alt::Vector<Alt::String>::AllocatePush("str0"_salloc, "str1"_salloc) iferr_return;
     * 
     * auto iter = vec0.GetIterator()
     *     .Map([](const Alt::String& str) {
     *	       return str.Clone().GetValueOrDefault();
     *     });
     * 
     * Alt::Vector<Alt::String> vec1;
     * vec1.InsertIteratorMove(0, iter) iferr_return;
     * 
     * assert(vec1[0] == "str0"_s);
     * assert(vec1[1] == "str1"_s);
     */
    ALT_NODISCARD bool InsertIteratorMove(const usize i, IteratorInterface<T, IIteratorForwardTrait, IIteratorExactSizeTrait> iterator) noexcept {

        return InsertIteratorMoveImpl(i, iterator);
    }

    ALT_NODISCARD bool InsertIteratorMove(const usize i, IteratorInterface<T&, IIteratorForwardTrait, IIteratorExactSizeTrait> iterator) noexcept {

        return InsertIteratorMoveImpl(i, iterator);
    }

    /**
     * @brief Allocate, or fill or destroy elements in the vector.
     * @tparam U The type of the value used for padding. Must be constructible to T.
     * @param size The new size of the vector.
     * @param val (Optional) The value to use for initializing new elements.
     * @return True if the resize was successful, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 2, 4) iferr_return;
     *
     * vec.Resize(0) iferr_return;
     * assert(vec.IsEmpty());
     * 
     * vec.Resize(1, 4);
     * assert(vec[0] == 4);
     */
    template <typename U = T, typename ...ArgsT,
        std::enable_if_t<
            is_populated_v<ArgsT...> &&
            std::is_default_constructible_v<U> &&
            std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>
        >* = nullptr>
    bool ResizeAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return Resize(size);
    }

    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD bool Resize(const usize size) noexcept {
        iferr_scope;

        const usize sizeOld = GetSize();
        if (size < sizeOld) {

            T* pSizeEndNew = m_pMemoryBegin + size;
            Memory::DestroyObjects(pSizeEndNew, m_pMemoryEnd);

            m_pMemoryEnd = pSizeEndNew;
        }
        else if (size > sizeOld) {

            Reserve(size) iferr_return false;

            Memory::FillConstructDefaultObjects(m_pMemoryEnd, m_pMemoryBegin + size);
            m_pMemoryEnd = m_pMemoryBegin + size;
        }

        return true;
    }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD bool Resize(const usize size, const U& val) noexcept {
        iferr_scope;

        const usize sizeOld = GetSize();
        if (size < sizeOld) {

            T* pNewSizeEnd = m_pMemoryBegin + size;
            Memory::DestroyObjects(pNewSizeEnd, m_pMemoryEnd);

            m_pMemoryEnd = pNewSizeEnd;

            return true;
        }
        else if (size > sizeOld) {

            Reserve(size) iferr_return false;

            const Memory::CloneFromFirstElementResult<T> result = Memory::FillCloneConstructObjectsFromFirstElement(
                m_pMemoryEnd, m_pMemoryBegin + size, val
            );

            m_pMemoryEnd = result.pDstObjectWhereCloneFailedOrEnd;

            return result.isOk;
        }

        return true;
    }

    /**
     * @brief Reserves at least the specified capacity in the vector.
     * @param size The capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec;
     * 
     * const usize capacityOld = vec.GetCapacity();
     * vec.Reserve(1'024);
     * const usize capacity = vec.GetCapacity();
     * 
     * assert(capacityOld < capacity);
     */
    bool Reserve(const usize size) noexcept {

        if (size <= GetCapacity()) {
            return true;
        }

        return ReallocateToNextGrow(size);
    }

    /**
     * @brief Additionally reserves at least the 'GetSize() + additional size' capacity in the vector.
     * @param size The additional capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed or overflow.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec;
     * 
     * const usize capacityOld = vec.GetCapacity();
     * vec.ReserveAddition(1);
     * const usize capacity = vec.GetCapacity();
     * 
     * assert(capacityOld < capacity);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>>* = nullptr>
    bool ReserveAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return Reserve(size);
    }

    /**
     * @brief Reserves the exact capacity in the vector.
     * @param size The capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize, 0> vec;
     * 
     * vec.ReserveExact(9);
     * assert(vec.GetCapacity() == 9);
     */
    bool ReserveExact(const usize size) noexcept {

        if (size <= GetCapacity()) {
            return true;
        }

        return Reallocate(size);
    }

    /**
     * @brief Additionaly reserves the exact 'GetSize() + additional size' capacity in the vector.
     * @param size The additional capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize, 0> vec;
     * 
     * vec.ReserveExact(9);
     * vec.ReserveExactAddition(7);
     * assert(vec.GetCapacity() == 16);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>>* = nullptr>
    bool ReserveExactAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return ReserveExact(size);
    }

    /**
     * @brief Reduces the capacity of the vector to fit the current size.
     * @param capacityNew The new capacity.
     * @return True if successful, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize, 0> vec = Alt::Vector<usize>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * 
     * const usize capacityOld = vec.GetCapacity();
     * vec.ShrinkToFit();
     * const usize capacity = vec.GetCapacity();
     * 
     * assert(capacityOld > capacity);
     */
    bool ShrinkToFit(const usize capacityNew = MIN_ELEMENT_CAPACITY_ON_ALLOCATE) noexcept {
        iferr_scope;

        if (GetCapacity() <= capacityNew) {

            return true;
        }

        const usize size = GetSize();
        const usize capacity = Num::Max(size, capacityNew);

        T* pMemory = nullptr;
        if (capacity == 0) {

            Memory::ChooseAlignedFree<T>(m_pMemoryBegin);
        }
        else {

            pMemory = Memory::ChooseAlignedRealloc<T>(
                m_pMemoryBegin,
                size,
                capacity,
                Memory::ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace
            ) iferr_return false;
        }

        m_pMemoryBegin = pMemory;
        m_pMemoryEnd = pMemory + size;
        m_pCapacityEnd = pMemory + capacity;

        return true;
    }

    /**
     * @brief Removes the element at index `i` by swapping it with the last element.
     * @param i The index of the element to remove.
     * @return The element that was removed.
     * @note This is an $O(1)$ operation but does not preserve the order of elements.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * const usize first = vec.RemoveSwapLast(0);
     * 
     * assert(first == 0);
     * assert(vec[0] == 16);
     */
    ALT_FORCE_INLINE T RemoveSwapLast(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        T result = Algorithms::RemoveSwapLast(m_pMemoryBegin, m_pMemoryEnd, m_pMemoryBegin + i);
        m_pMemoryEnd -= 1;

        return result;
    }

    /**
     * @brief Removes the element at index `i` and shifts subsequent elements forward.
     * @param i The index of the element to remove.
     * @return The element that was removed.
     * @note This is an $O(n)$ operation.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * const usize first = vec.Remove(0);
     * 
     * assert(first == 0);
     * assert(vec[0] == 2);
     */
    T Remove(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        T result = Algorithms::Remove(m_pMemoryBegin, m_pMemoryEnd, m_pMemoryBegin + i);
        m_pMemoryEnd -= 1;

        return result;
    }

    /**
     * @brief A binary search algorithm.
     * @return Returns pointer to actual value or 'DataEnd()' for next match for upper bound case.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(1, 1, 1, 4, 4, 4) iferr_return;
     * 
     * const usize* pBS0 = vec.BinarySearchLowerBound(4);
     * assert(pBS0 == &vec[3]);
     * 
     * const usize* pBS1 = vec.BinarySearchUpperBound(4);
     * assert(pBS1 == vec.DataEnd());
     * 
     * const usize* pBS2 = vec.BinarySearchLowerBound(100);
     * assert(pBS2 == vec.DataEnd());
     * 
     * const usize* pBS3 = vec.BinarySearchUpperBound(100);
     * assert(pBS3 == vec.DataEnd());
     */
    template <typename FuncT>
    ALT_NODISCARD T* BinarySearch(FuncT&& cmp) noexcept {

        return Algorithms::BinarySearch(m_pMemoryBegin, m_pMemoryEnd, std::forward<FuncT>(cmp));
    }

    template <typename FuncT>
    ALT_NODISCARD const T* BinarySearch(FuncT&& cmp) const noexcept {

        return Algorithms::BinarySearch(m_pMemoryBegin, m_pMemoryEnd, std::forward<FuncT>(cmp));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD T* BinarySearchLowerBound(U&& x) noexcept {

        return Algorithms::BinarySearchLowerBound(m_pMemoryBegin, m_pMemoryEnd, std::forward<U>(x));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD const T* BinarySearchLowerBound(U&& x) const noexcept {

        return Algorithms::BinarySearchLowerBound(m_pMemoryBegin, m_pMemoryEnd, std::forward<U>(x));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD T* BinarySearchUpperBound(U&& x) noexcept {

        return Algorithms::BinarySearchUpperBound(m_pMemoryBegin, m_pMemoryEnd, std::forward<U>(x));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD const T* BinarySearchUpperBound(U&& x) const noexcept {

        return Algorithms::BinarySearchUpperBound(m_pMemoryBegin, m_pMemoryEnd, std::forward<U>(x));
    }

    /**
     * @brief In place sorting.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(3, 1, 2, 0) iferr_return;
     * 
     * vec.Sort();
     * 
     * assert(vec[0] == 0);
     * assert(vec[1] == 1);
     * assert(vec[2] == 2);
     * assert(vec[3] == 3);
     */
    template <typename FuncT>
    void Sort(FuncT&& cmp) noexcept {

        return Algorithms::Sort(m_pMemoryBegin, m_pMemoryEnd, std::forward<FuncT>(cmp));
    }

    void Sort() noexcept {

        return Algorithms::Sort(m_pMemoryBegin, m_pMemoryEnd);
    }

    /**
     * @brief Operators for comparison with Vector.
     * @example
     *
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
     * Alt::Vector<usize> vec1 = vec0.Clone() iferr_return;
     * 
     * assert(vec0 == vec1);
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator==(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareEQ(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator!=(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareNE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator<(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareLT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator<=(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareLE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator>(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareGT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator>=(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareGE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

private:

    ALT_FORCE_INLINE static usize GetNextSize(const usize size) noexcept {

        return Num::Max(Num::RoundUpToPowerOf2(size), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);
    }

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE static bool PushVariadicImpl(Vector& self, HeadT&& head, TailT&&... tail) noexcept {
        iferr_scope;

        self.Push(std::forward<HeadT>(head)) iferr_return false;
        return PushVariadicImpl(self, std::forward<TailT>(tail)...);
    }

    ALT_FORCE_INLINE static bool PushVariadicImpl(Vector&) noexcept { return true; }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> && !std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_FORCE_INLINE bool InsertImpl(const usize i, const U& val_) noexcept {
        iferr_scope;

        U val = val_.Clone() iferr_return false;
        return InsertImpl(i, std::move(val));
    }

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_FORCE_INLINE bool InsertImpl(const usize i, U&& val) noexcept {
        iferr_scope;

        const usize size = GetSize();

        if (i >= size) {

            if (i >= GetCapacity()) {

                const usize sizeGrow = Num::AddOverflowCheck(i, 1) iferr_return false;
                ReallocateToNextGrow(sizeGrow) iferr_return false;
            }

            T* pNewSizeEndMinusOne = m_pMemoryBegin + i;

            Memory::FillConstructDefaultObjects(m_pMemoryEnd, pNewSizeEndMinusOne);
            Memory::ConstructObject(pNewSizeEndMinusOne, std::forward<U>(val));

            m_pMemoryEnd = pNewSizeEndMinusOne + 1;
        }
        else {

            const usize sizeNew = Num::AddOverflowCheck(size, 1) iferr_return false;

            if (sizeNew > GetCapacity()) {

                const usize nextSize = GetNextSize(sizeNew);
                ALT_ASSERT(nextSize > GetCapacity());

                constexpr usize wrapSizeMaxPossible = Num::Desc<usize>::Max() / sizeof(T);

                const Memory::ReallocBits bits = size == GetCapacity()
                    ? Memory::ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace
                    : Memory::ReallocBits::None;

                Memory::MallocWrapResult<T> result = Memory::ChooseAlignedReallocWrap<T>(
                    m_pMemoryBegin,
                    size,
                    nextSize,
                    Num::Max(sizeNew, wrapSizeMaxPossible),
                    Memory::ReallocBits::InPlace | bits
                ) ifok {

                    m_pMemoryBegin = result.pMemory;
                    m_pMemoryEnd = result.pMemory + size;
                    m_pCapacityEnd = result.pMemory + result.size;

                    goto insert_object_label;
                }
                else {

                    result = Memory::ChooseAlignedMallocWrap<T>(nextSize, Num::Max(sizeNew, wrapSizeMaxPossible)) iferr_return false;

                    Memory::MoveFromFirstElementResult<T> moveResult = Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
                        result.pMemory, m_pMemoryBegin, m_pMemoryBegin + i
                    );

                    Memory::ConstructObject(moveResult.pDstEnd, std::forward<U>(val));
                    Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(moveResult.pDstEnd + 1, m_pMemoryBegin + i, m_pMemoryEnd);

                    Memory::ChooseAlignedFree<T>(m_pMemoryBegin);

                    m_pMemoryBegin = result.pMemory;
                    m_pMemoryEnd = result.pMemory + sizeNew;
                    m_pCapacityEnd = result.pMemory + result.size;
                }
            }
            else {

insert_object_label:
                T* pMemoryInsert = m_pMemoryBegin + i;

                if (TypeTraits::is_trivially_reallocatable_v<T>) {

                    std::memmove(pMemoryInsert + 1, pMemoryInsert, (m_pMemoryEnd - pMemoryInsert) * sizeof(T));
                    Memory::ConstructObject(pMemoryInsert, std::forward<U>(val));
                }
                else {

                    T* pLast = m_pMemoryEnd - 1;

                    Memory::ConstructObject(m_pMemoryEnd, std::move(*pLast));
                    Memory::MoveObjectsFromLastElement(pMemoryInsert + 1, pMemoryInsert, pLast);
                    *pMemoryInsert = std::forward<U>(val);
                }

                m_pMemoryEnd += 1;
            }
        }

        return true;
    }

    template <typename U>
    bool InsertIteratorMoveImpl(const usize i, IteratorInterface<U, IIteratorForwardTrait, IIteratorExactSizeTrait> iterator) noexcept {
        iferr_scope;

        const usize sizeToInsert = iterator.GetSize();
        if (sizeToInsert == 0) {
            return true;
        }

        const usize size = GetSize();
        const usize iEnd = Num::AddOverflowCheck(i, sizeToInsert) iferr_return false;

        if (i >= size) {

            if (iEnd > GetCapacity()) {

                ReallocateToNextGrow(iEnd) iferr_return false;
            }

            T* pNewSizeEndMinusOne = m_pMemoryBegin + i;
            Memory::FillConstructDefaultObjects(m_pMemoryEnd, pNewSizeEndMinusOne);

            while (Optional<U> val = iterator.Next()) {

                Memory::ConstructObject(pNewSizeEndMinusOne, std::move(val.GetValue()));
                pNewSizeEndMinusOne += 1;
            }

            m_pMemoryEnd = pNewSizeEndMinusOne;
        }
        else {

            const usize sizeNew = Num::AddOverflowCheck(size, sizeToInsert) iferr_return false;

            if (sizeNew > GetCapacity()) {

                const usize nextSize = GetNextSize(sizeNew);
                ALT_ASSERT(nextSize > GetCapacity());

                constexpr usize wrapSizeMaxPossible = Num::Desc<usize>::Max() / sizeof(T);

                const Memory::ReallocBits bits = size == GetCapacity()
                    ? Memory::ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace
                    : Memory::ReallocBits::None;

                Memory::MallocWrapResult<T> result = Memory::ChooseAlignedReallocWrap<T>(
                    m_pMemoryBegin,
                    size,
                    nextSize,
                    Num::Max(sizeNew, wrapSizeMaxPossible),
                    Memory::ReallocBits::InPlace | bits
                ) ifok {

                    m_pMemoryBegin = result.pMemory;
                    m_pMemoryEnd = result.pMemory + size;
                    m_pCapacityEnd = result.pMemory + result.size;

                    goto insert_objects_label;
                }
                else {

                    const Memory::MallocWrapResult<T> result = Memory::ChooseAlignedMallocWrap<T>(nextSize, Num::Max(sizeNew, wrapSizeMaxPossible))
                        iferr_return false;

                    Memory::MoveFromFirstElementResult<T> moveResult = Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
                        result.pMemory, m_pMemoryBegin, m_pMemoryBegin + i
                    );

                    while (Optional<U> val = iterator.Next()) {

                        Memory::ConstructObject(moveResult.pDstEnd, std::move(val.GetValue()));
                        moveResult.pDstEnd += 1;
                    }

                    Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(moveResult.pDstEnd, m_pMemoryBegin + i, m_pMemoryEnd);

                    Memory::ChooseAlignedFree<T>(m_pMemoryBegin);

                    m_pMemoryBegin = result.pMemory;
                    m_pMemoryEnd = result.pMemory + sizeNew;
                    m_pCapacityEnd = result.pMemory + result.size;
                }
            }
            else {

insert_objects_label:
                T* pMemoryInsert = m_pMemoryBegin + i;

                if (TypeTraits::is_trivially_reallocatable_v<T>) {

                    std::memmove(pMemoryInsert + sizeToInsert, pMemoryInsert, (m_pMemoryEnd - pMemoryInsert) * sizeof(T));

                    while (Optional<U> val = iterator.Next()) {

                        Memory::ConstructObject(pMemoryInsert, std::move(val.GetValue()));
                        pMemoryInsert += 1;
                    }
                }
                else {

                    if (pMemoryInsert + sizeToInsert <= m_pMemoryEnd) {

                        Memory::MoveConstructObjectsFromFirstElementNonOverlap(m_pMemoryEnd, m_pMemoryEnd - sizeToInsert, m_pMemoryEnd);

                        if (pMemoryInsert + sizeToInsert >= m_pMemoryEnd - sizeToInsert) {

                            Memory::MoveObjectsFromFirstElementNonOverlap(pMemoryInsert + sizeToInsert, pMemoryInsert, m_pMemoryEnd - sizeToInsert);
                        }
                        else {

                            Memory::MoveObjectsFromLastElement(pMemoryInsert + sizeToInsert, pMemoryInsert, m_pMemoryEnd - sizeToInsert);
                        }
                    }
                    else {

                        Memory::MoveConstructObjectsFromFirstElementNonOverlap(pMemoryInsert + sizeToInsert, pMemoryInsert, m_pMemoryEnd);
                    }

                    while (Optional<U> val = iterator.Next()) {

                        *pMemoryInsert = std::move(val.GetValue());
                        pMemoryInsert += 1;

                        if (pMemoryInsert == m_pMemoryEnd) {
                            break;
                        }
                    }

                    while (Optional<U> val = iterator.Next()) {

                        Memory::ConstructObject(pMemoryInsert, std::move(val.GetValue()));
                        pMemoryInsert += 1;
                    }
                }

                m_pMemoryEnd += sizeToInsert;
            }
        }

        return true;
    }

    ALT_FORCE_INLINE bool ReallocateToNextGrow(const usize size) noexcept {

        const usize sizeNext = GetNextSize(size);
        return Reallocate(sizeNext, Num::Max(size, Num::Desc<usize>::Max() / sizeof(T)));
    }

    ALT_FORCE_INLINE bool Reallocate(const usize size, const usize wrapSize = Num::Desc<usize>::Max() / sizeof(T)) noexcept {
        iferr_scope;

        ALT_ASSERT(size > GetCapacity());

        const usize sizeOld = GetSize();

        const Memory::ReallocBits reallocBits = sizeOld == GetCapacity()
            ? Memory::ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace
            : Memory::ReallocBits::None;

        const Memory::MallocWrapResult<T> result = Memory::ChooseAlignedReallocWrap<T>(
            m_pMemoryBegin,
            sizeOld,
            size,
            wrapSize,
            reallocBits
        ) iferr_return false;

        m_pMemoryBegin = result.pMemory;
        m_pMemoryEnd = result.pMemory + sizeOld;
        m_pCapacityEnd = result.pMemory + result.size;

        return true;
    }

    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE Vector(Rule5Move_t, Vector<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& rhs) noexcept
        : m_pMemoryBegin(Exchange(rhs.m_pMemoryBegin, nullptr))
        , m_pMemoryEnd(Exchange(rhs.m_pMemoryEnd, nullptr))
        , m_pCapacityEnd(Exchange(rhs.m_pCapacityEnd, nullptr))
    {}

    template <usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE_ = MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE void Assign(Rule5Move_t, Vector<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE_>& rhs) noexcept {

        if (static_cast<void*>(this) == &rhs) {
            return;
        }

        if (m_pMemoryBegin) {

            Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryEnd);
            Memory::ChooseAlignedFree<T>(m_pMemoryBegin);
        }

        m_pMemoryBegin = Exchange(rhs.m_pMemoryBegin, nullptr);
        m_pMemoryEnd = Exchange(rhs.m_pMemoryEnd, nullptr);
        m_pCapacityEnd = Exchange(rhs.m_pCapacityEnd, nullptr);
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        if (!m_pMemoryBegin) {
            return;
        }

        Memory::DestroyObjects(m_pMemoryBegin, m_pMemoryEnd);
        Memory::ChooseAlignedFree<T>(m_pMemoryBegin);

        m_pMemoryBegin = nullptr;
        m_pMemoryEnd = nullptr;
        m_pCapacityEnd = nullptr;
    }

private:

    template <typename T, usize>
    friend class Vector;

    T* m_pMemoryBegin = nullptr;

    T* m_pMemoryEnd = nullptr;
    T* m_pCapacityEnd = nullptr;
};

template <typename T, usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE>
class IteratorCollector<Vector<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>> {
public:

    using SelfT = Vector<T, MIN_ELEMENT_CAPACITY_ON_ALLOCATE>;

    template <typename U = T,
        std::enable_if_t<std::is_assignable_v<T&, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait>& iter) noexcept {
        iferr_scope;

        const usize sizeHint = iter.GetSizeHint();
        self.ReserveAddition(sizeHint) iferr_return false;

        while (Optional<U> val = iter.Next()) {

            self.Push(std::move(val.GetValue())) iferr_return false;
        }

        return true;
    }
};

} // namespace Alt

#endif // __ALT_STD_VECTOR__