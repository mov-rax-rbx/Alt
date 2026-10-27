#pragma once

#ifndef __ALT_STD_STATIC_VECTOR__
#define __ALT_STD_STATIC_VECTOR__

#include "../IfErr.h"
#include "../TypeTraits.h"

#include "../Algorithms/Algorithms.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Memory/MaybeUninit.h"

#include "../Utils/Slice.h"
#include "../Utils/Optional.h"
#include "../Utils/ScopeDefer.h"

#include "VectorIterator.h"

namespace Alt {

/**
 * @brief A fixed-capacity vector (no allocations) that stores up to a specified number of elements.
 * @details This container manages memory statically on the stack or within the object's footprint.
 *          Unlike a standard vector, it does not perform heap allocations. It supports two modes:
 *          a dynamic size mode (up to SIZE - `StaticVector`) and a fixed size mode (exactly SIZE - `Array`).
 * 
 * @tparam T The type of the elements stored in the vector.
 * @tparam SIZE The maximum capacity of the vector.
 * @tparam Usize The type used for size and indexing. Defaults to TypeTraits::count_to_type_t<SIZE>.
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::StaticVector<usize, 5> vec0;
 * vec0.Push(10) iferr_return;
 * assert(vec0.GetSize() == 1);
 * 
 * Alt::StaticVector<f32, 3> vec1;
 * vec1.Push(1.5f, 2.5f);
 * assert(vec1.GetSize() == 2);
 * 
 * Alt::Array<usize. 2> arr;
 * arr[0] = 1;
 * assert(arr[0] == 1);
 * assert(arr.GetSize() == 2);
 */
template <typename T, usize SIZE, typename Usize = TypeTraits::count_to_type_t<SIZE>>
class StaticVector;

template <typename T, usize SIZE>
using Array = StaticVector<T, SIZE, Unit_t>;

template <typename T>
using ArrayIterator = VectorIterator<T>;

template <typename T>
using ArrayIteratorConst = VectorIteratorConst<T>;

/**
 * @brief Creates a Array from a variadic list of arguments.
 * @tparam HeadT The type of the first element.
 * @tparam TailT The types of the subsequent elements.
 * @param val The first element to be included in the array.
 * @param args The remaining elements to be included in the array.
 * @return A Array containing the provided elements, with a fixed capacity 
 *         equal to the total number of arguments passed.
 * 
 * @example
 * 
 * auto array = Alt::MakeArray(0, 1, 2, 3);
 * assert(array.GetSize() == 4);
 * assert(array.GetCapacity() == 4);
 * 
 * assert(array[0] == 0);
 * assert(array[1] == 1);
 */
template <typename HeadT, typename ...TailT,
    std::enable_if_t<std::conjunction_v<std::is_constructible<HeadT, TailT>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Array<HeadT, 1 + sizeof...(TailT)> MakeArray(HeadT val, TailT&&... args) noexcept {

    auto result = Array<HeadT, 1 + sizeof...(TailT)>::CreatePush(std::forward<HeadT>(val), static_cast<HeadT>(std::forward<TailT>(args))...);
    ALT_ASSERT(result);

    return std::move(result).GetValue();
}

template <typename HeadT, typename ...TailT,
    std::enable_if_t<
        std::conjunction_v<TypeTraits::is_only_clonable_to_from_t<HeadT, TailT>...> &&
        std::disjunction_v<std::negation<std::is_constructible<HeadT, TailT>>...>
    >* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Optional<Array<HeadT, 1 + sizeof...(TailT)>> MakeArray(HeadT val, TailT&&... args) noexcept {

    return Array<HeadT, 1 + sizeof...(TailT)>::CreatePush(std::forward<HeadT>(val), static_cast<HeadT>(std::forward<TailT>(args))...);
}

namespace TypeTraits {

template <typename T, usize SIZE, typename Usize>
struct __is_trivially_reallocatable__<StaticVector<T, SIZE, Usize>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/** \cond */
namespace _StaticVectorDetails {

struct Rule5Copy_t {};
struct Rule5Move_t {};

constexpr static Rule5Copy_t Rule5Copy = Rule5Copy_t{};
constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

template <typename T, typename U>
using is_pushable_t = std::disjunction<TypeTraits::is_only_clonable_to_from_t<T, U>, std::is_constructible<T, U>>;

template <typename T, usize SIZE, typename Usize>
class StaticVectorStorageCore {
public:

    ALT_FORCE_INLINE T* GetMemoryBeginImpl() noexcept {

        return m_Memory->GetPtr();
    }

    ALT_FORCE_INLINE const T* GetMemoryBeginImpl() const noexcept {

        return m_Memory->GetPtr();
    }

    ALT_FORCE_INLINE usize GetSizeImpl() const noexcept {

        return static_cast<usize>(m_Size);
    }

    ALT_FORCE_INLINE Usize& GetSizeRefImpl() noexcept {

        return m_Size;
    }

    ALT_FORCE_INLINE const Usize& GetSizeRefImpl() const noexcept {

        return m_Size;
    }

private:

    MaybeUninit<T> m_Memory[SIZE];
    Usize m_Size = 0;
};

template <typename T, typename Usize>
class StaticVectorStorageCore<T, 0, Usize> {
public:

    ALT_FORCE_INLINE T* GetMemoryBeginImpl() noexcept {

        return nullptr;
    }

    ALT_FORCE_INLINE const T* GetMemoryBeginImpl() const noexcept {

        return nullptr;
    }

    ALT_FORCE_INLINE usize GetSizeImpl() const noexcept {

        return static_cast<usize>(m_Size);
    }

    ALT_FORCE_INLINE Usize& GetSizeRefImpl() noexcept {

        return m_Size;
    }

    ALT_FORCE_INLINE const Usize& GetSizeRefImpl() const noexcept {

        return m_Size;
    }

private:

    Usize m_Size = 0;
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    typename Usize,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_destructible_v<T>
>
struct StaticVectorStorageDestruct;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageDestruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticVectorStorageCore<T, SIZE, Usize>
{
    StaticVectorStorageDestruct() noexcept = default;

    StaticVectorStorageDestruct(const StaticVectorStorageDestruct&) noexcept = default;
    StaticVectorStorageDestruct& operator=(const StaticVectorStorageDestruct&) noexcept = default;

    StaticVectorStorageDestruct(StaticVectorStorageDestruct&&) noexcept = default;
    StaticVectorStorageDestruct& operator=(StaticVectorStorageDestruct&&) noexcept = default;

    ~StaticVectorStorageDestruct() noexcept = delete;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageDestruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticVectorStorageCore<T, SIZE, Usize>
{
    StaticVectorStorageDestruct() noexcept = default;

    StaticVectorStorageDestruct(const StaticVectorStorageDestruct&) noexcept = default;
    StaticVectorStorageDestruct& operator=(const StaticVectorStorageDestruct&) noexcept = default;

    StaticVectorStorageDestruct(StaticVectorStorageDestruct&&) noexcept = default;
    StaticVectorStorageDestruct& operator=(StaticVectorStorageDestruct&&) noexcept = default;

    ~StaticVectorStorageDestruct() noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageDestruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticVectorStorageCore<T, SIZE, Usize>
{
    StaticVectorStorageDestruct() noexcept = default;

    StaticVectorStorageDestruct(const StaticVectorStorageDestruct&) noexcept = default;
    StaticVectorStorageDestruct& operator=(const StaticVectorStorageDestruct&) noexcept = default;

    StaticVectorStorageDestruct(StaticVectorStorageDestruct&&) noexcept = default;
    StaticVectorStorageDestruct& operator=(StaticVectorStorageDestruct&&) noexcept = default;

    ~StaticVectorStorageDestruct() noexcept {

        static_cast<SelfT*>(this)->Destroy();
    }
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    typename Usize,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_assignable_v<T>
>
struct StaticVectorStorageMoveAssign;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageMoveAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticVectorStorageDestruct<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageMoveAssign() noexcept = default;

    StaticVectorStorageMoveAssign(const StaticVectorStorageMoveAssign&) noexcept = default;
    StaticVectorStorageMoveAssign& operator=(const StaticVectorStorageMoveAssign&) noexcept = default;

    StaticVectorStorageMoveAssign(StaticVectorStorageMoveAssign&&) noexcept = default;
    StaticVectorStorageMoveAssign& operator=(StaticVectorStorageMoveAssign&&) noexcept = delete;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageMoveAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticVectorStorageDestruct<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageMoveAssign() noexcept = default;

    StaticVectorStorageMoveAssign(const StaticVectorStorageMoveAssign&) noexcept = default;
    StaticVectorStorageMoveAssign& operator=(const StaticVectorStorageMoveAssign&) noexcept = default;

    StaticVectorStorageMoveAssign(StaticVectorStorageMoveAssign&&) noexcept = default;

    StaticVectorStorageMoveAssign& operator=(StaticVectorStorageMoveAssign&& rhs) noexcept {

        // we can do better - move only "active" elements
        static_cast<SelfT*>(this)->Assign(Rule5Move, static_cast<SelfT&>(rhs));
        return *this;
    }
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageMoveAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticVectorStorageDestruct<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageMoveAssign() noexcept = default;

    StaticVectorStorageMoveAssign(const StaticVectorStorageMoveAssign&) noexcept = default;
    StaticVectorStorageMoveAssign& operator=(const StaticVectorStorageMoveAssign&) noexcept = default;

    StaticVectorStorageMoveAssign(StaticVectorStorageMoveAssign&&) noexcept = default;

    StaticVectorStorageMoveAssign& operator=(StaticVectorStorageMoveAssign&& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Move, static_cast<SelfT&>(rhs));
        return *this;
    }
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    typename Usize,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_constructible_v<T>
>
struct StaticVectorStorageMoveConstruct;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageMoveConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticVectorStorageMoveAssign<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageMoveConstruct() noexcept = default;

    StaticVectorStorageMoveConstruct(const StaticVectorStorageMoveConstruct&) noexcept = default;
    StaticVectorStorageMoveConstruct& operator=(const StaticVectorStorageMoveConstruct&) noexcept = default;

    StaticVectorStorageMoveConstruct(StaticVectorStorageMoveConstruct&&) noexcept = delete;
    StaticVectorStorageMoveConstruct& operator=(StaticVectorStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageMoveConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticVectorStorageMoveAssign<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageMoveConstruct() noexcept = default;

    StaticVectorStorageMoveConstruct(const StaticVectorStorageMoveConstruct&) noexcept = default;
    StaticVectorStorageMoveConstruct& operator=(const StaticVectorStorageMoveConstruct&) noexcept = default;

    StaticVectorStorageMoveConstruct(StaticVectorStorageMoveConstruct&& rhs) noexcept {

        // we can do better - move only "active" elements
        static_cast<SelfT*>(this)->Construct(Rule5Move, static_cast<SelfT&>(rhs));
    }

    StaticVectorStorageMoveConstruct& operator=(StaticVectorStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageMoveConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticVectorStorageMoveAssign<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageMoveConstruct() noexcept = default;

    StaticVectorStorageMoveConstruct(const StaticVectorStorageMoveConstruct&) noexcept = default;
    StaticVectorStorageMoveConstruct& operator=(const StaticVectorStorageMoveConstruct&) noexcept = default;

    StaticVectorStorageMoveConstruct(StaticVectorStorageMoveConstruct&& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Move, static_cast<SelfT&>(rhs));
    }

    StaticVectorStorageMoveConstruct& operator=(StaticVectorStorageMoveConstruct&&) noexcept = default;
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    typename Usize,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_assignable_v<T>
>
struct StaticVectorStorageCopyAssign;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageCopyAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticVectorStorageMoveConstruct<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageCopyAssign() noexcept = default;

    StaticVectorStorageCopyAssign(const StaticVectorStorageCopyAssign&) noexcept = default;
    StaticVectorStorageCopyAssign& operator=(const StaticVectorStorageCopyAssign&) noexcept = delete;

    StaticVectorStorageCopyAssign(StaticVectorStorageCopyAssign&&) noexcept = default;
    StaticVectorStorageCopyAssign& operator=(StaticVectorStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageCopyAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticVectorStorageMoveConstruct<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageCopyAssign() noexcept = default;

    StaticVectorStorageCopyAssign(const StaticVectorStorageCopyAssign&) noexcept = default;

    StaticVectorStorageCopyAssign& operator=(const StaticVectorStorageCopyAssign& rhs) noexcept {

        // we can do better - copy only "active" elements
        static_cast<SelfT*>(this)->Assign(Rule5Copy, static_cast<const SelfT&>(rhs));
        return *this;
    }

    StaticVectorStorageCopyAssign(StaticVectorStorageCopyAssign&&) noexcept = default;
    StaticVectorStorageCopyAssign& operator=(StaticVectorStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageCopyAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticVectorStorageMoveConstruct<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageCopyAssign() noexcept = default;

    StaticVectorStorageCopyAssign(const StaticVectorStorageCopyAssign&) noexcept = default;

    StaticVectorStorageCopyAssign& operator=(const StaticVectorStorageCopyAssign& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Copy, static_cast<const SelfT&>(rhs));
        return *this;
    }

    StaticVectorStorageCopyAssign(StaticVectorStorageCopyAssign&&) noexcept = default;
    StaticVectorStorageCopyAssign& operator=(StaticVectorStorageCopyAssign&&) noexcept = default;
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    typename Usize,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_constructible_v<T>
>
struct StaticVectorStorageCopyConstruct;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageCopyConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticVectorStorageCopyAssign<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageCopyConstruct() noexcept = default;

    StaticVectorStorageCopyConstruct(const StaticVectorStorageCopyConstruct&) noexcept = delete;
    StaticVectorStorageCopyConstruct& operator=(const StaticVectorStorageCopyConstruct&) noexcept = default;

    StaticVectorStorageCopyConstruct(StaticVectorStorageCopyConstruct&&) noexcept = default;
    StaticVectorStorageCopyConstruct& operator=(StaticVectorStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageCopyConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticVectorStorageCopyAssign<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageCopyConstruct() noexcept = default;

    StaticVectorStorageCopyConstruct(const StaticVectorStorageCopyConstruct& rhs) noexcept {

        // we can do better - copy only "active" elements
        static_cast<SelfT*>(this)->Construct(Rule5Copy, static_cast<const SelfT&>(rhs));
    }

    StaticVectorStorageCopyConstruct& operator=(const StaticVectorStorageCopyConstruct&) noexcept = default;

    StaticVectorStorageCopyConstruct(StaticVectorStorageCopyConstruct&&) noexcept = default;
    StaticVectorStorageCopyConstruct& operator=(StaticVectorStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticVectorStorageCopyConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticVectorStorageCopyAssign<SelfT, T, SIZE, Usize>
{
    StaticVectorStorageCopyConstruct() noexcept = default;

    StaticVectorStorageCopyConstruct(const StaticVectorStorageCopyConstruct& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Copy, static_cast<const SelfT&>(rhs));
    }

    StaticVectorStorageCopyConstruct& operator=(const StaticVectorStorageCopyConstruct&) noexcept = default;

    StaticVectorStorageCopyConstruct(StaticVectorStorageCopyConstruct&&) noexcept = default;
    StaticVectorStorageCopyConstruct& operator=(StaticVectorStorageCopyConstruct&&) noexcept = default;
};

template <typename T, usize SIZE, typename Usize>
class StaticVectorStorage
    : public StaticVectorStorageCopyConstruct<
        StaticVectorStorage<T, SIZE, Usize>,
        T,
        SIZE,
        Usize
    >
{
public:

    template <typename U, usize RHS_SIZE = SIZE>
    constexpr static bool is_copy_constructible_v =
        std::is_same_v<U, T> &&
        std::is_constructible_v<T, const U&> &&
        RHS_SIZE <= SIZE
    ;

    template <typename U, usize RHS_SIZE = SIZE>
    constexpr static bool is_move_constructible_v =
        std::is_same_v<U, T> &&
        (TypeTraits::is_trivially_reallocatable_v<U> || (std::is_constructible_v<T, U&&> && std::is_destructible_v<U>)) &&
        RHS_SIZE <= SIZE
    ;

    template <typename U, usize RHS_SIZE = SIZE>
    constexpr static bool is_copy_assignable_v =
        std::is_same_v<U, T> &&
        std::is_assignable_v<T&, const U&> &&
        std::is_constructible_v<T, const U&> &&
        std::is_destructible_v<U> &&
        RHS_SIZE <= SIZE
    ;

    template <typename U, usize RHS_SIZE = SIZE>
    constexpr static bool is_move_assignable_v =
        std::is_same_v<U, T> &&
        std::is_assignable_v<T&, U&&> && std::is_destructible_v<U> &&
        (TypeTraits::is_trivially_reallocatable_v<U> || std::is_constructible_v<T, U&&>) &&
        RHS_SIZE <= SIZE
    ;

    ALT_FORCE_INLINE StaticVectorStorage() noexcept = default;

    ALT_FORCE_INLINE T* GetMemoryBegin() noexcept {

        return this->GetMemoryBeginImpl();
    }

    ALT_FORCE_INLINE const T* GetMemoryBegin() const noexcept {

        return this->GetMemoryBeginImpl();
    }

    ALT_FORCE_INLINE T* GetMemoryEnd() noexcept {

        return this->GetMemoryBeginImpl() + GetSize();
    }

    ALT_FORCE_INLINE const T* GetMemoryEnd() const noexcept {

        return this->GetMemoryBeginImpl() + GetSize();
    }

    ALT_FORCE_INLINE Usize& GetSizeRef() noexcept {

        return this->GetSizeRefImpl();
    }

    ALT_FORCE_INLINE const Usize& GetSizeRef() const noexcept {

        return this->GetSizeRefImpl();
    }

    ALT_FORCE_INLINE usize GetSize() const noexcept {

        return this->GetSizeImpl();
    }

    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<is_copy_constructible_v<U, RHS_SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Construct(_StaticVectorDetails::Rule5Copy_t, const StaticVectorStorage<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        const T* pCopyEnd = Memory::CopyConstructObjectsNonOverlap(GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryEnd());
        GetSizeRef() = static_cast<Usize>(pCopyEnd - GetMemoryBegin());
    }

    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<is_copy_assignable_v<U, RHS_SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Assign(_StaticVectorDetails::Rule5Copy_t, const StaticVectorStorage<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return;
        }

        const usize size = GetSize();
        const usize rhsSize = rhs.GetSize();

        const usize sizeToCopy = Num::Min(size, rhsSize);

        T* pCopyEnd = Memory::CopyObjectsNonOverlap(GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryBegin() + sizeToCopy);
        Memory::DestroyObjects(pCopyEnd, GetMemoryEnd());
        Memory::CopyConstructObjectsNonOverlap(pCopyEnd, rhs.GetMemoryBegin() + sizeToCopy, rhs.GetMemoryEnd());

        GetSizeRef() = static_cast<Usize>(rhsSize);
    }

    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<is_move_constructible_v<U, RHS_SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Construct(_StaticVectorDetails::Rule5Move_t, StaticVectorStorage<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryEnd());

        const RHSUsize rhsSize = Exchange(rhs.GetSizeRef(), static_cast<RHSUsize>(0));
        GetSizeRef() = static_cast<Usize>(rhsSize);
    }

    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<is_move_assignable_v<U, RHS_SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Assign(_StaticVectorDetails::Rule5Move_t, StaticVectorStorage<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return;
        }

        const usize size = GetSize();
        const usize rhsSize = rhs.GetSize();

        const usize sizeToMove = Num::Min(size, rhsSize);

        Memory::MoveFromFirstElementResult<T> result = Memory::MoveDestructibleObjectsFromFirstElementNonOverlap(
            GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryBegin() + sizeToMove
        );

        Memory::DestroyObjects(result.pDstEnd, GetMemoryEnd());
        Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(result.pDstEnd, rhs.GetMemoryBegin() + sizeToMove, rhs.GetMemoryEnd());

        GetSizeRef() = static_cast<Usize>(Exchange(rhs.GetSizeRef(), 0));
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        Memory::DestroyObjects(GetMemoryBegin(), GetMemoryEnd());
        GetSizeRef() = 0;
    }

private:

    template <typename, usize, typename>
    friend class StaticVectorStorage;
};

template <typename T, usize SIZE>
class ArrayStorageCore {
public:

    using CArray = T[SIZE];

    ALT_FORCE_INLINE T* GetMemoryBeginImpl() noexcept {

        return m_Memory.GetVal();
    }

    ALT_FORCE_INLINE const T* GetMemoryBeginImpl() const noexcept {

        return m_Memory.GetVal();
    }

    ALT_FORCE_INLINE operator CArray&() noexcept {
        return m_Memory.GetVal();
    }

    ALT_FORCE_INLINE operator const CArray&() const noexcept {
        return m_Memory.GetVal();
    }

private:

    MaybeUninit<T[SIZE]> m_Memory;
};

template <typename T>
class ArrayStorageCore<T, 0> {
public:

    ALT_FORCE_INLINE T* GetMemoryBeginImpl() noexcept {

        return nullptr;
    }

    ALT_FORCE_INLINE const T* GetMemoryBeginImpl() const noexcept {

        return nullptr;
    }
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_destructible_v<T>
>
struct ArrayStorageDestruct;

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageDestruct<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ArrayStorageCore<T, SIZE>
{
    ArrayStorageDestruct() noexcept = default;

    ArrayStorageDestruct(const ArrayStorageDestruct&) noexcept = default;
    ArrayStorageDestruct& operator=(const ArrayStorageDestruct&) noexcept = default;

    ArrayStorageDestruct(ArrayStorageDestruct&&) noexcept = default;
    ArrayStorageDestruct& operator=(ArrayStorageDestruct&&) noexcept = default;

    ~ArrayStorageDestruct() noexcept = delete;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageDestruct<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ArrayStorageCore<T, SIZE>
{
    ArrayStorageDestruct() noexcept = default;

    ArrayStorageDestruct(const ArrayStorageDestruct&) noexcept = default;
    ArrayStorageDestruct& operator=(const ArrayStorageDestruct&) noexcept = default;

    ArrayStorageDestruct(ArrayStorageDestruct&&) noexcept = default;
    ArrayStorageDestruct& operator=(ArrayStorageDestruct&&) noexcept = default;

    ~ArrayStorageDestruct() noexcept = default;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageDestruct<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ArrayStorageCore<T, SIZE>
{
    ArrayStorageDestruct() noexcept = default;

    ArrayStorageDestruct(const ArrayStorageDestruct&) noexcept = default;
    ArrayStorageDestruct& operator=(const ArrayStorageDestruct&) noexcept = default;

    ArrayStorageDestruct(ArrayStorageDestruct&&) noexcept = default;
    ArrayStorageDestruct& operator=(ArrayStorageDestruct&&) noexcept = default;

    ~ArrayStorageDestruct() noexcept {

        static_cast<SelfT*>(this)->Destroy();
    }
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_assignable_v<T>
>
struct ArrayStorageMoveAssign;

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageMoveAssign<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ArrayStorageDestruct<SelfT, T, SIZE>
{
    ArrayStorageMoveAssign() noexcept = default;

    ArrayStorageMoveAssign(const ArrayStorageMoveAssign&) noexcept = default;
    ArrayStorageMoveAssign& operator=(const ArrayStorageMoveAssign&) noexcept = default;

    ArrayStorageMoveAssign(ArrayStorageMoveAssign&&) noexcept = default;
    ArrayStorageMoveAssign& operator=(ArrayStorageMoveAssign&&) noexcept = delete;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageMoveAssign<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ArrayStorageDestruct<SelfT, T, SIZE>
{
    ArrayStorageMoveAssign() noexcept = default;

    ArrayStorageMoveAssign(const ArrayStorageMoveAssign&) noexcept = default;
    ArrayStorageMoveAssign& operator=(const ArrayStorageMoveAssign&) noexcept = default;

    ArrayStorageMoveAssign(ArrayStorageMoveAssign&&) noexcept = default;
    ArrayStorageMoveAssign& operator=(ArrayStorageMoveAssign&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageMoveAssign<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ArrayStorageDestruct<SelfT, T, SIZE>
{
    ArrayStorageMoveAssign() noexcept = default;

    ArrayStorageMoveAssign(const ArrayStorageMoveAssign&) noexcept = default;
    ArrayStorageMoveAssign& operator=(const ArrayStorageMoveAssign&) noexcept = default;

    ArrayStorageMoveAssign(ArrayStorageMoveAssign&&) noexcept = default;

    ArrayStorageMoveAssign& operator=(ArrayStorageMoveAssign&& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Move, static_cast<SelfT&>(rhs));
        return *this;
    }
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_move_constructible_v<T>
>
struct ArrayStorageMoveConstruct;

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageMoveConstruct<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ArrayStorageMoveAssign<SelfT, T, SIZE>
{
    ArrayStorageMoveConstruct() noexcept = default;

    ArrayStorageMoveConstruct(const ArrayStorageMoveConstruct&) noexcept = default;
    ArrayStorageMoveConstruct& operator=(const ArrayStorageMoveConstruct&) noexcept = default;

    ArrayStorageMoveConstruct(ArrayStorageMoveConstruct&&) noexcept = delete;
    ArrayStorageMoveConstruct& operator=(ArrayStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageMoveConstruct<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ArrayStorageMoveAssign<SelfT, T, SIZE>
{
    ArrayStorageMoveConstruct() noexcept = default;

    ArrayStorageMoveConstruct(const ArrayStorageMoveConstruct&) noexcept = default;
    ArrayStorageMoveConstruct& operator=(const ArrayStorageMoveConstruct&) noexcept = default;

    ArrayStorageMoveConstruct(ArrayStorageMoveConstruct&&) noexcept = default;
    ArrayStorageMoveConstruct& operator=(ArrayStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageMoveConstruct<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ArrayStorageMoveAssign<SelfT, T, SIZE>
{
    ArrayStorageMoveConstruct() noexcept = default;

    ArrayStorageMoveConstruct(const ArrayStorageMoveConstruct&) noexcept = default;
    ArrayStorageMoveConstruct& operator=(const ArrayStorageMoveConstruct&) noexcept = default;

    ArrayStorageMoveConstruct(ArrayStorageMoveConstruct&& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Move, static_cast<SelfT&>(rhs));
    }

    ArrayStorageMoveConstruct& operator=(ArrayStorageMoveConstruct&&) noexcept = default;
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_assignable_v<T>
>
struct ArrayStorageCopyAssign;

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageCopyAssign<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ArrayStorageMoveConstruct<SelfT, T, SIZE>
{
    ArrayStorageCopyAssign() noexcept = default;

    ArrayStorageCopyAssign(const ArrayStorageCopyAssign&) noexcept = default;
    ArrayStorageCopyAssign& operator=(const ArrayStorageCopyAssign&) noexcept = delete;

    ArrayStorageCopyAssign(ArrayStorageCopyAssign&&) noexcept = default;
    ArrayStorageCopyAssign& operator=(ArrayStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageCopyAssign<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ArrayStorageMoveConstruct<SelfT, T, SIZE>
{
    ArrayStorageCopyAssign() noexcept = default;

    ArrayStorageCopyAssign(const ArrayStorageCopyAssign&) noexcept = default;
    ArrayStorageCopyAssign& operator=(const ArrayStorageCopyAssign&) noexcept = default;

    ArrayStorageCopyAssign(ArrayStorageCopyAssign&&) noexcept = default;
    ArrayStorageCopyAssign& operator=(ArrayStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageCopyAssign<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ArrayStorageMoveConstruct<SelfT, T, SIZE>
{
    ArrayStorageCopyAssign() noexcept = default;

    ArrayStorageCopyAssign(const ArrayStorageCopyAssign&) noexcept = default;

    ArrayStorageCopyAssign& operator=(const ArrayStorageCopyAssign& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Copy, static_cast<const SelfT&>(rhs));
        return *this;
    }

    ArrayStorageCopyAssign(ArrayStorageCopyAssign&&) noexcept = default;
    ArrayStorageCopyAssign& operator=(ArrayStorageCopyAssign&&) noexcept = default;
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_constructible_v<T>
>
struct ArrayStorageCopyConstruct;

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageCopyConstruct<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Deleted>
    : ArrayStorageCopyAssign<SelfT, T, SIZE>
{
    ArrayStorageCopyConstruct() noexcept = default;

    ArrayStorageCopyConstruct(const ArrayStorageCopyConstruct&) noexcept = delete;
    ArrayStorageCopyConstruct& operator=(const ArrayStorageCopyConstruct&) noexcept = default;

    ArrayStorageCopyConstruct(ArrayStorageCopyConstruct&&) noexcept = default;
    ArrayStorageCopyConstruct& operator=(ArrayStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageCopyConstruct<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::Trivial>
    : ArrayStorageCopyAssign<SelfT, T, SIZE>
{
    ArrayStorageCopyConstruct() noexcept = default;

    ArrayStorageCopyConstruct(const ArrayStorageCopyConstruct&) noexcept = default;
    ArrayStorageCopyConstruct& operator=(const ArrayStorageCopyConstruct&) noexcept = default;

    ArrayStorageCopyConstruct(ArrayStorageCopyConstruct&&) noexcept = default;
    ArrayStorageCopyConstruct& operator=(ArrayStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE>
struct ArrayStorageCopyConstruct<SelfT, T, SIZE, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : ArrayStorageCopyAssign<SelfT, T, SIZE>
{
    ArrayStorageCopyConstruct() noexcept = default;

    ArrayStorageCopyConstruct(const ArrayStorageCopyConstruct& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Copy, static_cast<const SelfT&>(rhs));
    }

    ArrayStorageCopyConstruct& operator=(const ArrayStorageCopyConstruct&) noexcept = default;

    ArrayStorageCopyConstruct(ArrayStorageCopyConstruct&&) noexcept = default;
    ArrayStorageCopyConstruct& operator=(ArrayStorageCopyConstruct&&) noexcept = default;
};

template <typename T, usize SIZE>
class ArrayStorage
    : public ArrayStorageCopyConstruct<
        ArrayStorage<T, SIZE>,
        T,
        SIZE
    >
{
public:

    template <typename U, usize RHS_SIZE>
    constexpr static bool is_copy_constructible_v =
        std::is_same_v<U, T> && std::is_constructible_v<T, const U&> && RHS_SIZE <= SIZE
    ;

    template <typename U, usize RHS_SIZE>
    constexpr static bool is_move_constructible_v =
        std::is_same_v<U, T> && std::is_constructible_v<T, U&&> && RHS_SIZE <= SIZE
    ;

    template <typename U, usize RHS_SIZE>
    constexpr static bool is_copy_assignable_v =
        std::is_same_v<U, T> && std::is_assignable_v<T&, const U&> && RHS_SIZE <= SIZE
    ;

    template <typename U, usize RHS_SIZE>
    constexpr static bool is_move_assignable_v =
        std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && RHS_SIZE <= SIZE
    ;

    ALT_FORCE_INLINE T* GetMemoryBegin() noexcept {

        return this->GetMemoryBeginImpl();
    }

    ALT_FORCE_INLINE const T* GetMemoryBegin() const noexcept {

        return this->GetMemoryBeginImpl();
    }

    ALT_FORCE_INLINE T* GetMemoryEnd() noexcept {

        return this->GetMemoryBeginImpl() + SIZE;
    }

    ALT_FORCE_INLINE const T* GetMemoryEnd() const noexcept {

        return this->GetMemoryBeginImpl() + SIZE;
    }

    template <typename U = T,
        std::enable_if_t<is_copy_constructible_v<U, SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Construct(Rule5Copy_t, const ArrayStorage<U, SIZE>& rhs) noexcept {

        ALT_MAYBE_UNUSED const T* pCopyPossibleEnd = Memory::CopyConstructObjectsNonOverlap(
            GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryEnd()
        );
    }

    template <typename U = T,
        std::enable_if_t<is_copy_assignable_v<U, SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Assign(Rule5Copy_t, const ArrayStorage<U, SIZE>& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        ALT_MAYBE_UNUSED const T* pCopyPossibleEnd = Memory::CopyObjectsNonOverlap(
            GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryEnd()
        );
    }

    template <typename U = T,
        std::enable_if_t<is_move_constructible_v<U, SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Construct(Rule5Move_t, ArrayStorage<U, SIZE>& rhs) noexcept {

        Memory::MoveConstructObjectsFromFirstElementNonOverlap(GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryEnd());
    }

    template <typename U = T,
        std::enable_if_t<is_move_assignable_v<U, SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Assign(Rule5Move_t, ArrayStorage<U, SIZE>& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        Memory::MoveObjectsFromFirstElementNonOverlap(GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryEnd());
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        Memory::DestroyObjects(GetMemoryBegin(), GetMemoryEnd());
    }

private:

    template <typename, usize>
    friend class ArrayStorage;
};

} // namespace _StaticVectorDetails
/** \endcond */

/**
 * @brief A fixed-capacity vector that stores up to a specified number of elements.
 * @details This container manages memory statically on the stack or within the object's footprint.
 *          Unlike a standard vector, it does not perform heap allocations.
 * 
 * @tparam T The type of the elements stored in the vector.
 * @tparam SIZE The maximum capacity of the vector.
 * @tparam Usize The type used for size and indexing. Defaults to TypeTraits::count_to_type_t<SIZE>.
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::StaticVector<usize, 5> vec0;
 * vec0.Push(10) iferr_return;
 * assert(vec0.GetSize() == 1);
 * 
 * Alt::StaticVector<f32, 3> vec1;
 * vec1.Push(1.5f, 2.5f) iferr_return;
 * assert(vec1.GetSize() == 2);
 */
template <typename T_, usize SIZE, typename Usize_>
class StaticVector {
private:

    using Self = StaticVector<T_, SIZE, Usize_>;

    using T = TypeTraits::clean_t<T_>;
    using Usize = TypeTraits::clean_t<Usize_>;

    using Storage = _StaticVectorDetails::StaticVectorStorage<T, SIZE, Usize>;

    template <typename U>
    using is_pushable_t = _StaticVectorDetails::is_pushable_t<T, U>;

    template <typename U>
    constexpr static bool is_pushable_v = is_pushable_t<U>::value;

    template <typename ...ArgsT>
    constexpr static bool is_populated_v = sizeof...(ArgsT) != 0;

    static_assert(sizeof(Usize) >= sizeof(TypeTraits::count_to_type_t<SIZE>), "");

public:

    using Type = T;

    /**
     * @brief Creates a StaticVector with a specified initial size.
     * @tparam SIZE_ The desired initial number of elements. Must be <= SIZE.
     * @tparam U The type of the value to use for initialization. Must be default constructible.
     * @return A StaticVector initialized with SIZE_ elements.
     * @example
     * 
     * auto vec = Alt::StaticVector<usize, 10>::Create<5>();
     * assert(vec.GetSize() == 5);
     * assert(vec.GetCapacity() == 10);
     */
    template <usize SIZE_, typename U = T,
        std::enable_if_t<SIZE_ != 0 && SIZE_ <= SIZE && std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD static StaticVector Create() noexcept {

        StaticVector result{};
        ALT_MAYBE_UNUSED const bool isOk = result.Resize(SIZE_);

        // checked by template - never panic
        ALT_ASSERT(isOk);

        return result;
    }

    template <usize SIZE_, typename U = T,
        std::enable_if_t<SIZE_ == 0>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static StaticVector Create() noexcept {

        return StaticVector{};
    }

    /**
     * @brief Creates a StaticVector with a specified initial size, where all elements are copies of val.
     * @tparam SIZE_ The desired initial number of elements. Must be <= SIZE.
     * @tparam U The type of the value. Must be constructible from U.
     * @param val The value used to populate the vector.
     * @return A StaticVector initialized with SIZE_ copies of val.
     * @example
     * 
     * auto vec = Alt::StaticVector<usize, 10>::Create<5>(4);
     * assert(vec.GetSize() == 5);
     * assert(vec.GetCapacity() == 10);
     * 
     * assert(vec[0] == 4);
     */
    template <usize SIZE_, typename U = T,
        std::enable_if_t<SIZE_ <= SIZE && (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)>* = nullptr>
    ALT_NODISCARD static StaticVector Create(const U& val) noexcept {

        StaticVector result{};
        ALT_MAYBE_UNUSED const bool isOk = result.Resize(SIZE_, val);

        // checked by template - never panic
        ALT_ASSERT(isOk);

        return result;
    }

    /**
     * @brief Creates an Optional StaticVector with a specified size.
     * @tparam U The type of the value. Must be default constructible.
     * @param size The number of elements to initialize. Must be <= SIZE.
     * @return An Optional containing the StaticVector if successful, or Nullopt if size > SIZE.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 10> vec = Alt::StaticVector<usize, 10>::Create(5) iferr_return;
     * assert(vec.GetSize() == 5);
     * assert(vec.GetCapacity() == 10);
     */
    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD static Optional<StaticVector> Create(const usize size = 0) noexcept {
        iferr_scope;

        StaticVector result{};
        result.Resize(size) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Creates an Optional StaticVector with a specified size and initial value.
     * @tparam U The type of the value. Must be constructible from U.
     * @param size The number of elements to initialize. Must be <= SIZE.
     * @param val The value to populate the vector with.
     * @return An Optional containing the StaticVector if successful, or Nullopt if size > SIZE.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 10> vec = Alt::StaticVector<usize, 10>::Create(5, 4) iferr_return;
     * assert(vec.GetSize() == 5);
     * assert(vec.GetCapacity() == 10);
     * 
     * assert(vec[0] == 4);
     */
    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD static Optional<StaticVector> Create(const usize size, const U& val) noexcept {
        iferr_scope;

        StaticVector result{};
        result.Resize(size, val) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Creates a StaticVector using a variadic list of arguments.
     * @tparam ArgsT The types of the arguments. Must be constructible to T.
     * @param args The arguments used to construct the elements.
     * @return A StaticVector containing the provided arguments.
     * @example
     * 
     * Alt::StaticVector<usize, 10> vec = Alt::StaticVector<usize, 10>::CreatePush(4, 5, 6, 7, 8);
     * assert(vec.GetSize() == 5);
     * assert(vec.GetCapacity() == 10);
     * 
     * assert(vec[0] == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<(is_populated_v<ArgsT...> && sizeof...(ArgsT) <= SIZE) && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    ALT_NODISCARD static Optional<StaticVector> CreatePush(ArgsT&&... args) noexcept {
        iferr_scope;

        StaticVector self;
        PushVariadicImpl(self, std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return self;
    }

    /**
     * @brief Default constructor. Creates an empty vector.
     * @example
     * 
     * Alt::StaticVector<usize, 10> vec;
     * assert(vec.IsEmpty());
     * assert(vec.GetCapacity() == 10);
     */
    ALT_FORCE_INLINE StaticVector() noexcept = default;

    /**
     * @brief Creates a StaticVector using a variadic list of arguments.
     * @tparam ArgsT The types of the arguments. Must be constructible to T.
     * @param args The arguments used to construct the elements.
     * @return A StaticVector containing the provided arguments.
     * @example
     * 
     * Alt::StaticVector<usize, 10> vec{ 4, 5, 6, 7, 8 };
     * assert(vec.GetSize() == 5);
     * assert(vec.GetCapacity() == 10);
     * 
     * assert(vec[0] == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<(sizeof...(ArgsT) <= SIZE) && std::conjunction_v<std::is_constructible<T, ArgsT&&>...>>* = nullptr>
    StaticVector(ArgsT&&... args) noexcept {

        ALT_MAYBE_UNUSED const bool isOk = PushVariadicImpl(*this, std::forward<ArgsT>(args)...);

        // checked by template - never panic
        ALT_ASSERT(isOk);
    }

    /**
     * @brief Copy constructor from another StaticVector.
     * @tparam U The type of the source vector. Must be the same as T.
     * @tparam RHS_SIZE The capacity of the source vector. Must be <= SIZE.
     * @param rhs The source vector to copy from.
     * @example
     * 
     * Alt::StaticVector<usize, 5> vec0;
     * Alt::StaticVector<usize, 10> vec1 = vec0; // vec1.GetSize() >= vec0.GetSize()
     */
    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<Storage::template is_copy_constructible_v<U, RHS_SIZE>>* = nullptr>
    StaticVector(const StaticVector<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        m_Storage.Construct(_StaticVectorDetails::Rule5Copy, rhs.m_Storage);
    }

    /**
     * @brief Copy assignment operator.
     * @tparam U The type of the source vector. Must be the same as T.
     * @tparam RHS_SIZE The capacity of the source vector. Must be <= SIZE.
     * @param rhs The source vector to assign from.
     * @return A reference to this vector.
     * 
     * @example
     * 
     * Alt::StaticVector<usize, 5> vec0;
     * Alt::StaticVector<usize, 10> vec1;
     * 
     * vec1 = vec0; // vec1.GetSize() >= vec0.GetSize()
     */
    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<Storage::template is_copy_assignable_v<U, RHS_SIZE>>* = nullptr>
    StaticVector& operator=(const StaticVector<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        m_Storage.Assign(_StaticVectorDetails::Rule5Copy, rhs.m_Storage);
        return *this;
    }

    /**
     * @brief Move constructor from another StaticVector.
     * @tparam U The type of the source vector. Must be the same as T.
     * @tparam RHS_SIZE The capacity of the source vector. Must be <= SIZE.
     * @param rhs The source vector to move from.
     * 
     * @example
     * 
     * Alt::StaticVector<usize, 5> vec0;
     * Alt::StaticVector<usize, 10> vec1 = std::move(vec0); // vec1.GetSize() >= vec0.GetSize()
     */
    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<Storage::template is_move_constructible_v<U, RHS_SIZE>>* = nullptr>
    StaticVector(StaticVector<U, RHS_SIZE, RHSUsize>&& rhs) noexcept {

        m_Storage.Construct(_StaticVectorDetails::Rule5Move, rhs.m_Storage);
    }

    /**
     * @brief Move assignment operator.
     * @tparam U The type of the source vector. Must be the same as T.
     * @tparam RHS_SIZE The capacity of the source vector. Must be <= SIZE.
     * @param rhs The source vector to move from.
     * @return A reference to this vector.
     * 
     * @example
     * 
     * Alt::StaticVector<usize, 5> vec0;
     * Alt::StaticVector<usize, 10> vec1;
     * 
     * vec1 = std::move(vec0); // vec1.GetSize() >= vec0.GetSize()
     */
    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<Storage::template is_move_assignable_v<U, RHS_SIZE>>* = nullptr>
    StaticVector& operator=(StaticVector<U, RHS_SIZE, RHSUsize>&& rhs) noexcept {

        m_Storage.Assign(_StaticVectorDetails::Rule5Move, rhs.m_Storage);
        return *this;
    }

    /**
     * @brief Returns the number of elements currently in the vector.
     * @return The size of the vector.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        return m_Storage.GetSize();
    }

    /**
     * @brief Returns the maximum capacity of the vector.
     * @return The constant capacity (SIZE).
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetCapacity() const noexcept {

        return SIZE;
    }

    /**
     * @brief Checks if the vector contains no elements.
     * @return True if empty, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        return GetSize() == 0;
    }

    /**
     * @brief Checks if the vector contains at least one element.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept {

        return !IsEmpty();
    }

    /**
     * @brief Checks if the vector full - any push will return false.
     * @return True if full, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsFull() const noexcept {

        return GetSize() == SIZE;
    }

    /**
     * @brief Removes all elements from the vector.
     * @details Destroys all existing objects.
     */
    void Clear() noexcept {

        m_Storage.Destroy();
    }

    /**
     * @brief Accesses the element at the specified index.
     * @param i The index of the element.
     * @return A reference to the element.
     * @example
     * 
     * Alt::StaticVector<usize, 4> vec = Alt::StaticVector<usize, 4>::CreatePush(0, 2);
     * assert(vec[0] == 0);
     * assert(vec[1] == 2);
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        return *(GetMemoryBegin() + i);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize i) const noexcept {
        ALT_ASSERT(i < GetSize());

        return *(GetMemoryBegin() + i);
    }

    /**
     * @brief Returns a reference to the first element.
     * @return Reference to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetFirst() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *GetMemoryBegin();
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetFirst() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *GetMemoryBegin();
    }

    /**
     * @brief Returns a reference to the last element.
     * @return Reference to the last element.
     * @note Asserts that the vector is not empty.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetLast() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(GetMemoryEnd() - 1);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetLast() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(GetMemoryEnd() - 1);
    }

    /**
     * @brief Returns a pointer to the beginning.
     * @return Pointer to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T* Data() noexcept {

        return GetMemoryBegin();
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T* Data() const noexcept {

        return GetMemoryBegin();
    }

    /**
     * @brief Returns a raw pointer to the end of the vector.
     * @return Pointer to the next after last element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T* DataEnd() noexcept {

        return GetMemoryEnd();
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T* DataEnd() const noexcept {

        return GetMemoryEnd();
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
     * @brief A deep copy current StaticVector to another.
     * 
     * @details Return false when copy fails - the Vector contains
     * valid intermediate state of copying.
     * 
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 4> vec0 = Alt::StaticVector<usize, 4>::AllocatePush(4) iferr_return;
     * 
     * Alt::StaticVector<usize, 2> vec1;
     * vec1.CloneFrom(vec0) iferr_return;
     * 
     * assert(vec1[0] == vec0[0]);
     */
    template <typename U = T, usize RHS_SIZE, typename RHSUsize,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || (std::is_assignable_v<T&, const U&> && std::is_constructible_v<T, const U&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(StaticVector<U, RHS_SIZE, RHSUsize>&& vector) noexcept {

        if (static_cast<const void*>(this) == &vector) {
            return true;
        }

        const Slice<const T> slice = vector;
        return CloneFrom(slice);
    }

    template <typename U = T, usize RHS_SIZE, typename RHSUsize,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || (std::is_assignable_v<T&, const U&> && std::is_constructible_v<T, const U&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const StaticVector<U, RHS_SIZE, RHSUsize>& vector) noexcept {

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

        const Slice<const T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(GetMemoryBegin(), GetMemoryEnd(), slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sliceSize = slice.GetSize();

        if (sliceSize > GetCapacity()) {
            return false;
        }

        const usize sizeToClone = Num::Min(size, sliceSize);

        Memory::CloneFromFirstElementResult<T> result = Memory::CloneObjectsFromFirstElementNonOverlap(
            GetMemoryBegin(), slice.Data(), slice.Data() + sizeToClone
        );

        Memory::DestroyObjects(result.pDstObjectWhereCloneFailedOrEnd, GetMemoryEnd());

        if (!result.isOk) {

            GetSizeRef() = static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - GetMemoryBegin());
            return false;
        }

        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            result.pDstObjectWhereCloneFailedOrEnd, slice.Data() + sizeToClone, slice.DataEnd()
        );

        GetSizeRef() = static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - GetMemoryBegin());

        return result.isOk;
    }

    ALT_NODISCARD bool CloneFrom(const T* pBegin, const usize size) noexcept {

        const Slice<const T> slice{ pBegin, size };
        return CloneFrom(slice);
    }

    template <typename U = T, usize RHS_SIZE, typename RHSUsize,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const StaticVector<U, RHS_SIZE, RHSUsize>& rhs) noexcept {
        ALT_ASSERT(static_cast<const void*>(this) != &rhs);

        const usize rhsSize = rhs.GetSize();

        if (rhsSize > GetCapacity()) {
            return false;
        }

        const Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            GetMemoryBegin(), rhs.Data(), rhs.DataEnd()
        );

        if (!result.isOk) {

            Memory::DestroyObjects(GetMemoryBegin(), result.pDstObjectWhereCloneFailedOrEnd);
            return false;
        }

        GetSizeRef() = static_cast<Usize>(rhsSize);
        return true;
    }

    ALT_NODISCARD Optional<StaticVector> Clone() const noexcept {
        iferr_scope;

        MaybeUninit<StaticVector> valueToConstruct;
        valueToConstruct.GetPtr()->CloneConstructFrom(*this) iferr_return Nullopt;

        defer {
            valueToConstruct.Destroy();
        };

        return std::move(valueToConstruct).GetVal();
    }

    /**
     * @brief A moves each element if current StaticVector to another.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 4> vec0 = Alt::StaticVector<usize, 4>::AllocatePush(4) iferr_return;
     * 
     * Alt::StaticVector<usize, 2> vec1;
     * vec1.MoveFrom(vec0) iferr_return;
     * 
     * assert(vec1[0] == vec0[0]);
     */
    template <typename U = T, usize RHS_SIZE, typename RHSUsize,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && std::is_constructible_v<T, U&&>>* = nullptr>
    bool MoveFrom(StaticVector<U, RHS_SIZE, RHSUsize>&& vector) noexcept {

        if (static_cast<const void*>(this) == &vector) {
            return true;
        }

        Slice<T> slice = vector;
        return MoveFrom(slice);
    }

    template <typename U = T, usize RHS_SIZE, typename RHSUsize,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && std::is_constructible_v<T, U&&>>* = nullptr>
    bool MoveFrom(StaticVector<U, RHS_SIZE, RHSUsize>& vector) noexcept {

        if (static_cast<const void*>(this) == &vector) {
            return true;
        }

        Slice<T> slice = vector;
        return MoveFrom(slice);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&
            std::is_assignable_v<T&, InnerU&&> && std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool MoveFrom(U&& rhs) noexcept {

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(GetMemoryBegin(), GetMemoryEnd(), slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sliceSize = slice.GetSize();

        if (sliceSize > GetCapacity()) {
            return false;
        }

        const usize sizeToMove = Num::Min(size, sliceSize);

        Memory::MoveFromFirstElementResult<T> result = Memory::MoveObjectsFromFirstElementNonOverlap(
            GetMemoryBegin(), slice.Data(), slice.Data() + sizeToMove
        );

        Memory::DestroyObjects(result.pDstEnd, GetMemoryEnd());

        Memory::MoveConstructObjectsFromFirstElementNonOverlap(result.pDstEnd, slice.Data() + sizeToMove, slice.DataEnd());

        GetSizeRef() = static_cast<Usize>(sliceSize);

        return true;
    }

    ALT_NODISCARD bool MoveFrom(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return MoveFrom(slice);
    }

    /**
     * @brief Append copy elements to the current StaticVector end.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::AllocatePush(0, 1, 2, 3) iferr_return;
     * vec.AppendClone(vec.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(vec[0] == vec[4]);
     * assert(vec[1] == vec[5]);
     */
    template <typename U = T, usize RHS_SIZE, typename RHSUsize,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendClone(StaticVector<U, RHS_SIZE, RHSUsize>&& vector) noexcept {

        const Slice<const T> slice = vector;
        return AppendClone(slice);
    }

    template <typename U = T, usize RHS_SIZE, typename RHSUsize,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendClone(const StaticVector<U, RHS_SIZE, RHSUsize>& vector) noexcept {

        const Slice<const T> slice = vector;
        return AppendClone(slice);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const InnerU&> || std::is_constructible_v<T, const InnerU&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendClone(U&& rhs) noexcept {
        iferr_scope;

        const Slice<const T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(GetMemoryBegin(), GetMemoryEnd(), slice.Data(), slice.DataEnd()));

        const usize sizeToAppend = slice.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        const usize size = Num::AddOverflowCheck(GetSize(), sizeToAppend) iferr_return false;
        if (size > GetCapacity()) {
            return false;
        }

        const Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            GetMemoryEnd(), slice.Data(), slice.DataEnd()
        );

        GetSizeRef() = static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - GetMemoryBegin());

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
     * Alt::StaticVector<Alt::String, 8> vec = Alt::StaticVector<Alt::String, 8>::AllocatePush("0"_salloc, "1"_salloc, "2"_salloc, "3"_salloc)
     *     iferr_return;
     * 
     * vec.AppendMove(vec.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(vec[0] == "0");
     * assert(vec[4] == "0");
     */
    template <typename U = T, usize RHS_SIZE, typename RHSUsize,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendMove(StaticVector<U, RHS_SIZE, RHSUsize>&& vector) noexcept {

        Slice<T> slice = vector;
        return AppendMove(slice);
    }

    template <typename U = T, usize RHS_SIZE, typename RHSUsize,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendMove(StaticVector<U, RHS_SIZE, RHSUsize>& vector) noexcept {

        Slice<T> slice = vector;
        return AppendMove(slice);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&
            std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool AppendMove(U&& rhs) noexcept {
        iferr_scope;

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(GetMemoryBegin(), GetMemoryEnd(), slice.Data(), slice.DataEnd()));

        const usize sizeToAppend = slice.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        const usize sizeNew = Num::AddOverflowCheck(GetSize(), sizeToAppend) iferr_return false;
        if (sizeNew > GetCapacity()) {
            return false;
        }

        Memory::MoveConstructObjectsFromFirstElementNonOverlap(GetMemoryEnd(), slice.Data(), slice.DataEnd());
        GetSizeRef() = static_cast<Usize>(sizeNew);

        return true;
    }

    ALT_NODISCARD bool AppendMove(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return AppendMove(slice);
    }

    /**
     * @brief Appends an element to the end of the StaticVector.
     * @param val The value to push.
     * @return True if the element was successfully added, false if capacity exceeded.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 4> vec;
     * vec.Push(0) iferr_return;
     * 
     * assert(vec[0] == 0);
     */
    template <typename U = T,
        std::enable_if_t<is_pushable_v<U&&>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Push(U&& val) noexcept {

        if (IsFull()) {

            return false;
        }

        T* pResult = Memory::CloneConstructOrMoveConstructObject(GetMemoryEnd(), std::forward<U>(val));

        const bool isOk = pResult != GetMemoryEnd();
        GetSizeRef() += isOk ? 1 : 0;

        return isOk;
    }

    /**
     * @brief Variadic append an elements to the end of the vector.
     * @param ...vals The values to push.
     * @return True if the element was successfully added, false if capacity exceeded.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 4> vec;
     * vec.Push(0, 1, 2, 3) iferr_return;
     * 
     * assert(vec[0] == 0);
     * assert(vec.GetSize() == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    ALT_NODISCARD bool Push(ArgsT&&... vals) noexcept {
        iferr_scope;

        constexpr usize sizeToPush = sizeof...(ArgsT);

        const usize sizeNew = Num::AddOverflowCheck(GetSize(), sizeToPush) iferr_return false;
        if (sizeNew > GetCapacity()) {
            return false;
        }

        PushVariadicImpl(*this, std::forward<ArgsT>(vals)...) iferr_return false;

        return true;
    }

    /**
     * @brief Removes and returns the last element of the StaticVector.
     * @return The element that was removed.
     * @note This method requires the StaticVector to not be empty. Calling on an empty StaticVector triggers an assertion.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 4> vec = Alt::StaticVector<usize, 4>::AllocatePush(0, 2, 4) iferr_return;
     * usize val = vec.Pop();
     * assert(val == 4);
     */
    ALT_FORCE_INLINE T Pop() noexcept {
        ALT_ASSERT(!IsEmpty());

        GetSizeRef() -= 1;
        return Memory::MoveDestructibleObject(*GetMemoryEnd());
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
     * Alt::StaticVector<usize, 4> vec = Alt::StaticVector<usize, 4>::AllocatePush(0, 2, 4) iferr_return;
     * vec.Insert(1, 10) iferr_return;
     * 
     * assert(vec[1] == 10);
     * assert(vec[2] == 2);
     */
    template <typename U = T,
        std::enable_if_t<
            std::is_default_constructible_v<T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, U&&> || std::is_constructible_v<T, U&&>)
        >* = nullptr>
    ALT_NODISCARD bool Insert(const usize i, U&& val) noexcept {

        return InsertImpl(i, std::forward<U>(val));
    }

    /**
     * @brief Fill or destroy elements in the vector.
     * @tparam U The type of the value used for padding. Must be constructible to T.
     * @param size The new size of the vector.
     * @return True if the resize was successful, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 4> vec;
     * vec.Resize(3) iferr_return;
     * 
     * assert(vec.GetSize() == 3);
     * assert(vec.GetCapacity() == 4);
     * 
     * vec.ResizeAddition(1) iferr_return;
     * 
     * assert(vec.GetSize() == 4);
     */
    template <typename U = T, typename ...ArgsT,
        std::enable_if_t<
            is_populated_v<ArgsT...> &&
            std::is_default_constructible_v<U> &&
            std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>
        >* = nullptr>
    ALT_NODISCARD bool ResizeAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return Resize(size);
    }

    /**
     * @brief Resizes the vector to a new size.
     * @tparam U The type of the value. Must be default constructible.
     * @param size The new size.
     * @return True if successful, false if size exceeds capacity.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 4> vec;
     * vec.Resize(3) iferr_return;
     * 
     * assert(vec.GetSize() == 3);
     * assert(vec.GetCapacity() == 4);
     * 
     * assert(vec[0] == 0);
     */
    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD bool Resize(const usize size) noexcept {

        const usize sizeOld = GetSize();
        if (size < sizeOld) {

            Memory::DestroyObjects(GetMemoryBegin() + size, GetMemoryEnd());

            GetSizeRef() = static_cast<Usize>(size);
        }
        else if (size > sizeOld) {

            if (size > GetCapacity()) {

                return false;
            }

            Memory::FillConstructDefaultObjects(GetMemoryEnd(), GetMemoryBegin() + size);

            GetSizeRef() = static_cast<Usize>(size);
        }

        return true;
    }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD bool Resize(const usize size, const U& val) noexcept {

        const usize sizeOld = GetSize();
        if (size < sizeOld) {

            Memory::DestroyObjects(GetMemoryBegin() + size, GetMemoryEnd());

            GetSizeRef() = static_cast<Usize>(size);
        }
        else if (size > sizeOld) {

            if (size > GetCapacity()) {

                return false;
            }

            const Memory::CloneFromFirstElementResult<T> result = Memory::FillCloneConstructObjectsFromFirstElement(
                GetMemoryEnd(), GetMemoryBegin() + size, val
            );

            GetSizeRef() = static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - GetMemoryBegin());

            return result.isOk;
        }

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
     * Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * const usize first = vec.RemoveSwapLast(0);
     * 
     * assert(first == 0);
     * assert(vec[0] == 16);
     */
    T RemoveSwapLast(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        T result = Algorithms::RemoveSwapLast(GetMemoryBegin(), GetMemoryEnd(), GetMemoryBegin() + i);
        GetSizeRef() -= 1;

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
     * Alt::StaticVector<usize, 8> vec = Alt::StaticVector<usize, 8>::AllocatePush(0, 2, 4, 8, 16) iferr_return;
     * const usize first = vec.Remove(0);
     * 
     * assert(first == 0);
     * assert(vec[0] == 2);
     */
    T Remove(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        T result = Algorithms::Remove(GetMemoryBegin(), GetMemoryEnd(), GetMemoryBegin() + i);
        GetSizeRef() -= 1;

        return result;
    }

    /**
     * @brief A binary search algorithm.
     * @return Returns pointer to actual value or 'DataEnd()' for next match for upper bound case.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticVector<usize, 6> vec = Alt::StaticVector<usize, 6>::AllocatePush(1, 1, 1, 4, 4, 4) iferr_return;
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

        return Algorithms::BinarySearch(GetMemoryBegin(), GetMemoryEnd(), std::forward<FuncT>(cmp));
    }

    template <typename FuncT>
    ALT_NODISCARD const T* BinarySearch(FuncT&& cmp) const noexcept {

        return Algorithms::BinarySearch(GetMemoryBegin(), GetMemoryEnd(), std::forward<FuncT>(cmp));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD T* BinarySearchLowerBound(U&& x) noexcept {

        return Algorithms::BinarySearchLowerBound(GetMemoryBegin(), GetMemoryEnd(), std::forward<U>(x));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD const T* BinarySearchLowerBound(U&& x) const noexcept {

        return Algorithms::BinarySearchLowerBound(GetMemoryBegin(), GetMemoryEnd(), std::forward<U>(x));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD T* BinarySearchUpperBound(U&& x) noexcept {

        return Algorithms::BinarySearchUpperBound(GetMemoryBegin(), GetMemoryEnd(), std::forward<U>(x));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD const T* BinarySearchUpperBound(U&& x) const noexcept {

        return Algorithms::BinarySearchUpperBound(GetMemoryBegin(), GetMemoryEnd(), std::forward<U>(x));
    }

    /**
     * @brief In place sorting.
     * @example
     * 
     * Alt::StaticVector<usize, 4> vec{ 3, 1, 2, 0 };
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

        return Algorithms::Sort(GetMemoryBegin(), GetMemoryEnd(), std::forward<FuncT>(cmp));
    }

    void Sort() noexcept {

        return Algorithms::Sort(GetMemoryBegin(), GetMemoryEnd());
    }

    /**
     * @brief Operators for comparison with StaticVector.
     * @example
     *
     * Alt::StaticVector<usize, 1> vec0{ 0, 1 };
     * Alt::StaticVector<usize, 1> vec1 = vec0.Clone() iferr_return;
     * 
     * assert(vec0 == vec1);
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator==(U&& rhs_) const noexcept {

        Slice<const T> rhs = rhs_;
        return Algorithms::CompareEQ(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator!=(U&& rhs_) const noexcept {

        Slice<const T> rhs = rhs_;
        return Algorithms::CompareNE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator<(U&& rhs_) const noexcept {

        Slice<const T> rhs = rhs_;
        return Algorithms::CompareLT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator<=(U&& rhs_) const noexcept {

        Slice<const T> rhs = rhs_;
        return Algorithms::CompareLE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator>(U&& rhs_) const noexcept {

        Slice<const T> rhs = rhs_;
        return Algorithms::CompareGT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator>=(U&& rhs_) const noexcept {

        Slice<const T> rhs = rhs_;
        return Algorithms::CompareGE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

private:

    template <typename U = Usize,
        std::enable_if_t<std::is_same_v<U, usize>>* = nullptr>
    ALT_FORCE_INLINE static bool IsCanOverflowSizeInc(const U x) noexcept {

        return Num::IsOverflowCheckAdd(x, 1);
    }

    template <typename U = Usize,
        std::enable_if_t<!std::is_same_v<U, usize>>* = nullptr>
    ALT_FORCE_INLINE static bool IsCanOverflowSizeInc(const U) noexcept {

        return false;
    }

    ALT_FORCE_INLINE T* GetMemoryBegin() noexcept {

        return m_Storage.GetMemoryBegin();
    }

    ALT_FORCE_INLINE const T* GetMemoryBegin() const noexcept {

        return m_Storage.GetMemoryBegin();
    }

    ALT_FORCE_INLINE T* GetMemoryEnd() noexcept {

        return m_Storage.GetMemoryEnd();
    }

    ALT_FORCE_INLINE const T* GetMemoryEnd() const noexcept {

        return m_Storage.GetMemoryEnd();
    }

    ALT_FORCE_INLINE Usize& GetSizeRef() noexcept {

        return m_Storage.GetSizeRef();
    }

    ALT_FORCE_INLINE const Usize& GetSizeRef() const noexcept {

        return m_Storage.GetSizeRef();
    }

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE static bool PushVariadicImpl(StaticVector& self, HeadT&& head, TailT&&... tail) noexcept {
        iferr_scope;

        self.Push(std::forward<HeadT>(head)) iferr_return false;
        return PushVariadicImpl(self, std::forward<TailT>(tail)...);
    }

    ALT_FORCE_INLINE static bool PushVariadicImpl(StaticVector&) noexcept { return true; }

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

        const usize size = GetSize();

        if (i >= size) {

            if (i >= GetCapacity()) {

                return false;
            }

            T* pNewSizeEndMinusOne = GetMemoryBegin() + i;

            Memory::FillConstructDefaultObjects(GetMemoryBegin() + size, pNewSizeEndMinusOne);
            Memory::ConstructObject(pNewSizeEndMinusOne, std::forward<U>(val));

            GetSizeRef() = static_cast<Usize>(i + 1);
        }
        else {

            if (IsCanOverflowSizeInc(size) || size + 1 > GetCapacity()) {
                return false;
            }

            T* pMemoryInsertConstruct = GetMemoryBegin() + i;

            if (TypeTraits::is_trivially_reallocatable_v<T>) {

                std::memmove(pMemoryInsertConstruct + 1, pMemoryInsertConstruct, (size - i) * sizeof(T));
                Memory::ConstructObject(pMemoryInsertConstruct, std::forward<U>(val));
            }
            else {

                T* pLast = GetMemoryEnd() - 1;

                Memory::ConstructObject(GetMemoryEnd(), std::move(*pLast));
                Memory::MoveObjectsFromLastElement(pMemoryInsertConstruct + 1, pMemoryInsertConstruct, pLast);
                *pMemoryInsertConstruct = std::forward<U>(val);
            }

            GetSizeRef() += 1;
        }

        return true;
    }

private:

    template <typename, usize, typename>
    friend class StaticVector;

    Storage m_Storage;
};

/**
 * @brief A Array that always contains exactly SIZE elements.
 * @details Unlike the primary template, this specialization allocates and
 *          initializes all SIZE elements immediately upon construction.
 *          The size of this vector cannot be changed.
 * 
 * @tparam T The type of the elements stored in the vector.
 * @tparam SIZE The exact number of elements this vector will always hold.
 * @example
 * 
 * Alt::Array<usize, 4> vec{ 1, 2, 3 };
 * assert(vec.GetSize() == 4);
 * assert(vec.GetCapacity() == 4);
 * 
 * assert(vec[0] == 1);
 * assert(vec[3] == 0);
 */
template <typename T_, usize SIZE>
class StaticVector<T_, SIZE, Unit_t> {
private:

    using T = TypeTraits::clean_t<T_>;

    using Storage = _StaticVectorDetails::ArrayStorage<T, SIZE>;

    template <typename U>
    using is_pushable_t = _StaticVectorDetails::is_pushable_t<T, U>;

    template <typename U>
    constexpr static bool is_pushable_v = is_pushable_t<U>::value;

    struct Unitialized_t {};
    constexpr static Unitialized_t Unitialized = Unitialized_t{};

public:

    using Type = T;
    using CArray = T_[SIZE];

    /**
     * @brief Creates a Array with all elements default-initialized.
     * @tparam U The type of the value. Must be default constructible.
     * @return A StaticVector of size SIZE.
     * @example
     * 
     * auto vec = Alt::Array<usize, 4>::Create();
     * assert(vec.GetSize() == 4);
     */
    template <typename U = T,
        std::enable_if_t<SIZE != 0 && std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD static StaticVector Create() noexcept {

        return StaticVector{};
    }

    template <typename U = T,
        std::enable_if_t<SIZE == 0 && std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD static StaticVector Create() noexcept {

        return StaticVector{};
    }

    /**
     * @brief Creates a Array where all elements are copies of val.
     * @tparam U The type of the value. Must be constructible from T.
     * @param val The value used to initialize all SIZE elements.
     * @return A StaticVector of size SIZE populated with val.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * auto vec = Alt::Array<usize, 4>::Create(10) iferr_return;
     * assert(vec[0] == 10);
     * assert(vec[1] == 10);
     */
    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD static Optional<StaticVector> Create(const U& val) noexcept {

        StaticVector result{ Unitialized };

        const Memory::CloneFromFirstElementResult<T> cloneResult = Memory::FillCloneConstructObjectsFromFirstElement(
            result.GetMemoryBegin(), result.GetMemoryEnd(), val
        );

        return cloneResult.isOk
            ? MakeOptional(std::move(result))
            : Nullopt;
    }

    /**
     * @brief Creates a Array from a variadic list of arguments.
     * @tparam ArgsT The types of the arguments. Must be constructible to T.
     * @param args The arguments used to initialize the elements. 
     *             If fewer than SIZE arguments are provided, the remaining elements are default-initialized.
     * @return A StaticVector populated with the provided arguments.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Array<usize, 4> vec = Alt::Array<usize, 4>::CreatePush(4, 1) iferr_return;
     * assert(vec[0] == 4);
     * assert(vec[1] == 1);
     * assert(vec[2] == 0); // Default initialized
     */
    template <typename ...ArgsT,
        std::enable_if_t<(sizeof...(ArgsT) == SIZE) && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    ALT_NODISCARD static Optional<StaticVector> CreatePush(ArgsT&&... args) noexcept {
        iferr_scope;

        StaticVector result{ Unitialized };
        ConstructVariadicImpl(result, 0, std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return result;
    }

    template <typename ...ArgsT,
        std::enable_if_t<
            std::conjunction_v<is_pushable_t<ArgsT&&>...> &&
            ((sizeof...(ArgsT) < SIZE) && std::is_default_constructible_v<T>)
        >* = nullptr>
    ALT_NODISCARD static Optional<StaticVector> CreatePush(ArgsT&&... args) noexcept {
        iferr_scope;

        const usize size = sizeof...(ArgsT);

        StaticVector result{ Unitialized };
        ConstructVariadicImpl(result, 0, std::forward<ArgsT>(args)...) iferr_return Nullopt;

        Memory::FillConstructDefaultObjects(result.GetMemoryBegin() + size, result.GetMemoryEnd());

        return result;
    }

    /**
     * @brief Constructs a Array.
     * @tparam ArgsT The types of the arguments. Must be constructible to T.
     * @param args The arguments to initialize the vector elements.
     * @example
     * 
     * Alt::Array<usize, 4> vec;
     * assert(vec[0] == 0);
     * assert(vec[1] == 0);
     */
    template <typename U = T,
        std::enable_if_t<SIZE != 0 && std::is_default_constructible_v<U>>* = nullptr>
    StaticVector() noexcept
        : StaticVector(Unitialized)
    {
        Memory::FillConstructDefaultObjects(GetMemoryBegin(), GetMemoryEnd());
    }

    template <typename U = T,
        std::enable_if_t<SIZE == 0 && std::is_same_v<U, T>>* = nullptr>
    StaticVector() noexcept
        : StaticVector(Unitialized)
    {}

    /**
     * @brief Constructs a Array from a variadic list of arguments.
     * @tparam ArgsT The types of the arguments. Must be constructible to T.
     * @param args The arguments to initialize N elements.
     * 
     * @example
     * 
     * Alt::Array<usize, 2> vec{ 4, 1 };
     * assert(vec[0] == 4);
     * assert(vec[1] == 1);
     * assert(vec[2] == 0); // Default initialized
     */
    template <typename ...ArgsT,
        std::enable_if_t<
            (sizeof...(ArgsT) == SIZE) &&
            std::conjunction_v<std::is_constructible<T, ArgsT&&>...>
        >* = nullptr>
    StaticVector(ArgsT&&... args) noexcept
        : StaticVector(Unitialized)
    {
        ALT_MAYBE_UNUSED const bool isOk = ConstructVariadicImpl(*this, 0, std::forward<ArgsT>(args)...);

        // checked by template - never panic
        ALT_ASSERT(isOk);
    }

    template <typename ...ArgsT,
        std::enable_if_t<
            std::conjunction_v<std::is_constructible<T, ArgsT&&>...> &&
            ((sizeof...(ArgsT) < SIZE) && std::is_default_constructible_v<T>)
        >* = nullptr>
    StaticVector(ArgsT&&... args) noexcept
        : StaticVector(Unitialized)
    {
        const usize size = sizeof...(ArgsT);

        ALT_MAYBE_UNUSED const bool isOk = ConstructVariadicImpl(*this, 0, std::forward<ArgsT>(args)...);

        // checked by template - never panic
        ALT_ASSERT(isOk);

        Memory::FillConstructDefaultObjects(GetMemoryBegin() + size, GetMemoryEnd());
    }

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T, U&&>>* = nullptr>
    StaticVector(U&& val) noexcept
        : StaticVector(Unitialized)
    {
        Memory::FillCopyConstructObjects(GetMemoryBegin(), GetMemoryEnd(), val);
    }

    /**
     * @brief Copy constructor from another Array.
     * @tparam U The type of the source vector. Must be the same as T.
     * @param rhs The source vector to copy from.
     * @example
     * 
     * Alt::Array<usize, 5> vec0;
     * Alt::Array<usize, 10> vec1 = vec0;
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<Storage::template is_copy_constructible_v<U, RHS_SIZE>>* = nullptr>
    StaticVector(const Array<U, SIZE>& rhs) noexcept
        : StaticVector(Unitialized)
    {
        m_Storage.Construct(_StaticVectorDetails::Rule5Copy, rhs.m_Storage);
    }

    /**
     * @brief Copy assignment operator.
     * @tparam U The type of the source vector. Must be the same as T.
     * @param rhs The source vector to assign from.
     * @return A reference to this vector.
     * @example
     * 
     * Alt::Array<usize, 5> vec0;
     * Alt::Array<usize, 10> vec1;
     * 
     * vec1 = vec0;
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<Storage::template is_copy_assignable_v<U, RHS_SIZE>>* = nullptr>
    StaticVector& operator=(const Array<U, SIZE>& rhs) noexcept {

        m_Storage.Assign(_StaticVectorDetails::Rule5Copy, rhs.m_Storage);
        return *this;
    }

    /**
     * @brief Move constructor from another Array.
     * @tparam U The type of the source vector. Must be the same as T.
     * @param rhs The source vector to move from.
     * @example
     * 
     * Alt::Array<usize, 5> vec0;
     * Alt::Array<usize, 10> vec1 = std::move(vec0);
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<Storage::template is_move_constructible_v<U, RHS_SIZE>>* = nullptr>
    StaticVector(Array<U, SIZE>&& rhs) noexcept
        : StaticVector(Unitialized)
    {
        m_Storage.Construct(_StaticVectorDetails::Rule5Move, rhs.m_Storage);
    }

    /**
     * @brief Move assignment operator.
     * @tparam U The type of the source vector. Must be the same as T.
     * @param rhs The source vector to move from.
     * @return A reference to this vector.
     * @example
     * 
     * Alt::Array<usize, 5> vec0;
     * Alt::Array<usize, 10> vec1;
     * 
     * vec1 = std::move(vec0);
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<Storage::template is_move_assignable_v<U, RHS_SIZE>>* = nullptr>
    StaticVector& operator=(Array<U, SIZE>&& rhs) noexcept {

        m_Storage.Assign(_StaticVectorDetails::Rule5Move, rhs.m_Storage);
        return *this;
    }

    /**
     * @brief Copy constructor from C array.
     * @tparam RHS_SIZE The length of C array. Must be <= SIZE.
     * @param rhs The C array from copied.
     * @return A C array.
     * @example
     * 
     * usize arr[4]{ 0, 1, 2, 1 };
     * 
     * Alt::Array<usize, 5> vec = arr;
     * assert(vec[0] == arr[0]);
     * assert(vec[1] == arr[1]);
     */
    template <usize RHS_SIZE = SIZE,
        std::enable_if_t<
            (RHS_SIZE == SIZE || (RHS_SIZE < SIZE && std::is_default_constructible_v<T>)) &&
            (RHS_SIZE == 0 || std::is_constructible_v<T, const T&>)
        >* = nullptr>
    StaticVector(const T (&rhs)[RHS_SIZE]) noexcept
        : StaticVector(Unitialized)
    {
        Memory::CopyConstructObjectsNonOverlap(GetMemoryBegin(), rhs, rhs + RHS_SIZE);
        Memory::FillConstructDefaultObjects(GetMemoryBegin() + RHS_SIZE, GetMemoryEnd());
    }

    template <usize RHS_SIZE = SIZE,
        std::enable_if_t<
            (RHS_SIZE == SIZE || (RHS_SIZE < SIZE && std::is_default_constructible_v<T>)) &&
            (RHS_SIZE == 0 || std::is_assignable_v<T&, const T&>)
        >* = nullptr>
    StaticVector& operator=(const T (&rhs)[RHS_SIZE]) noexcept {

        CloneFrom(rhs);
        return *this;
    }

    /**
     * @brief Returns the fixed size of the vector.
     * @return The constant SIZE.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetSize() const noexcept {

        return SIZE;
    }

    /**
     * @brief Returns the fixed capacity of the vector.
     * @return The constant SIZE.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetCapacity() const noexcept {

        return SIZE;
    }

    /**
     * @brief Checks if the vector is empty.
     * @return True if SIZE is 0, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsEmpty() const noexcept {

        return SIZE == 0;
    }

    /**
     * @brief Checks if the vector contains elements.
     * @return True if SIZE is greater than 0.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPopulated() const noexcept {

        return SIZE != 0;
    }

    /**
     * @brief Accesses the element at the specified index.
     * @param i The index of the element.
     * @return A reference to the element.
     * @example
     * 
     * Alt::Array<usize, 4> vec{ 0, 2 };
     * assert(vec[0] == 0);
     * assert(vec[1] == 2);
     */

#ifndef ALT_PLATFORM_32BIT

    /*
     * On x86 platform conflicts with `operator CArray&()`
     */

    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        return *(GetMemoryBegin() + i);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize i) const noexcept {
        ALT_ASSERT(i < GetSize());

        return *(GetMemoryBegin() + i);
    }
#endif // !ALT_PLATFORM_32BIT

    /**
     * @brief Return reference to underlying c arrays.
     * @return A reference to c array.
     * @example
     * 
     * void func(usize (&arr)[4]) {}
     * 
     * Alt::Array<usize, 4> vec{ 0, 2 };
     * func(vec);
     */
    ALT_FORCE_INLINE operator CArray&() noexcept {
        return m_Storage;
    }

    ALT_FORCE_INLINE operator const CArray&() const noexcept {
        return m_Storage;
    }

    /**
     * @brief Returns a reference to the first element.
     * @return Reference to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetFirst() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *GetMemoryBegin();
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetFirst() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *GetMemoryBegin();
    }

    /**
     * @brief Returns a reference to the last element.
     * @return Reference to the last element.
     * @note Asserts that the vector is not empty.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetLast() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(GetMemoryEnd() - 1);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetLast() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(GetMemoryEnd() - 1);
    }

    /**
     * @brief Returns a pointer to the beginning.
     * @return Pointer to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T* Data() noexcept {

        return GetMemoryBegin();
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T* Data() const noexcept {

        return GetMemoryBegin();
    }

    /**
     * @brief Returns a raw pointer to the end of the vector.
     * @return Pointer to the next after last element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T* DataEnd() noexcept {

        return GetMemoryEnd();
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T* DataEnd() const noexcept {

        return GetMemoryEnd();
    }

    /**
     * @brief Returns a mutable iterator for the vector.
     * @return An `VectorIterator`/`ArrayIterator`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE ArrayIterator<T> GetIterator() noexcept {

        return ArrayIterator<T>{ *this };
    }

    /**
     * @brief Returns a constant iterator for the vector.
     * @return An `VectorIteratorConst`/`ArrayIteratorConst`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE ArrayIteratorConst<T> GetIterator() const noexcept {

        return ArrayIteratorConst<T>{ *this };
    }

    /**
     * Clonable Trait
     * 
     * @brief A deep copy current Array to another.
     * 
     * @details Return false when copy fails - the Array contains
     * valid intermediate state of copying.
     * 
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Array<usize, 4> vec0 = Alt::Array<usize, 4>::AllocatePush(4) iferr_return;
     * 
     * Alt::Array<usize, 2> vec1;
     * vec1.CloneFrom(vec0) iferr_return;
     * 
     * assert(vec1[0] == vec0[0]);
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_assignable_v<T&, const U&>) &&
            RHS_SIZE <= SIZE
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const Array<U, RHS_SIZE>& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return true;
        }

        const Memory::CloneFromFirstElementResult<T> result = Memory::CloneObjectsFromFirstElementNonOverlap(
            GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryEnd()
        );

        Memory::FillDefaultObjects(result.pDstObjectWhereCloneFailedOrEnd, GetMemoryEnd());

        return result.isOk;
    }

    template <usize RHS_SIZE = SIZE,
        std::enable_if_t<
            (TypeTraits::is_only_clonable_to_from_v<T, const T&> || std::is_assignable_v<T&, const T&>) &&
            RHS_SIZE <= SIZE
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const T (&rhs)[RHS_SIZE]) noexcept {
        ALT_ASSERT(!Memory::IsOverlap(GetMemoryBegin(), GetMemoryEnd(), rhs, rhs + RHS_SIZE));

        const Memory::CloneFromFirstElementResult<T> result = Memory::CloneObjectsFromFirstElementNonOverlap(
            GetMemoryBegin(), rhs, rhs + RHS_SIZE
        );

        Memory::FillDefaultObjects(result.pDstObjectWhereCloneFailedOrEnd, GetMemoryEnd());

        return result.isOk;
    }

    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>) &&
            RHS_SIZE <= SIZE
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const Array<U, RHS_SIZE>& rhs) noexcept {
        ALT_ASSERT(static_cast<const void*>(this) != &rhs);

        const Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryEnd()
        );

        if (!result.isOk) {

            Memory::DestroyObjects(GetMemoryBegin(), result.pDstObjectWhereCloneFailedOrEnd);

            return false;
        }

        Memory::FillConstructDefaultObjects(result.pDstObjectWhereCloneFailedOrEnd, GetMemoryEnd());

        return true;
    }

    ALT_NODISCARD Optional<StaticVector> Clone() const noexcept {
        iferr_scope;

        MaybeUninit<StaticVector> valueToConstruct;
        valueToConstruct.GetPtr()->CloneConstructFrom(*this) iferr_return Nullopt;

        defer {
            valueToConstruct.Destroy();
        };

        return std::move(valueToConstruct).GetVal();
    }

    /**
     * @brief A moves each element if current Array to another.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Array<usize, 4> vec0 = Alt::Array<usize, 4>::AllocatePush(4) iferr_return;
     * 
     * Alt::Array<usize, 2> vec1;
     * vec1.MoveFrom(vec0) iferr_return;
     * 
     * assert(vec1[0] == vec0[0]);
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && RHS_SIZE <= SIZE>* = nullptr>
    bool MoveFrom(Array<U, RHS_SIZE>&& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return true;
        }

        return MoveFrom(rhs);
    }

    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && RHS_SIZE <= SIZE>* = nullptr>
    bool MoveFrom(Array<U, RHS_SIZE>& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return true;
        }

        Memory::MoveObjectsFromFirstElementNonOverlap(GetMemoryBegin(), rhs.GetMemoryBegin(), rhs.GetMemoryEnd());
        Memory::FillDefaultObjects(GetMemoryBegin() + RHS_SIZE, GetMemoryEnd());

        return true;
    }

    template <usize RHS_SIZE = SIZE,
        std::enable_if_t<std::is_assignable_v<T&, T&&> && RHS_SIZE <= SIZE>* = nullptr>
    bool MoveFrom(T (&rhs)[RHS_SIZE]) noexcept {
        ALT_ASSERT(!Memory::IsOverlap(GetMemoryBegin(), GetMemoryEnd(), rhs, rhs + RHS_SIZE));

        Memory::MoveObjectsFromFirstElementNonOverlap(GetMemoryBegin(), rhs, rhs + RHS_SIZE);
        Memory::FillDefaultObjects(GetMemoryBegin() + RHS_SIZE, GetMemoryEnd());

        return true;
    }

    /**
     * @brief A binary search algorithm.
     * @return Returns pointer to actual value or 'DataEnd()' for next match for upper bound case.
     * @example
     * 
     * Alt::StaticArray<usize> vec{ 1, 1, 1, 4, 4, 4 };
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

        return Algorithms::BinarySearch(GetMemoryBegin(), GetMemoryEnd(), std::forward<FuncT>(cmp));
    }

    template <typename FuncT>
    ALT_NODISCARD const T* BinarySearch(FuncT&& cmp) const noexcept {

        return Algorithms::BinarySearch(GetMemoryBegin(), GetMemoryEnd(), std::forward<FuncT>(cmp));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD T* BinarySearchLowerBound(U&& x) noexcept {

        return Algorithms::BinarySearchLowerBound(GetMemoryBegin(), GetMemoryEnd(), std::forward<U>(x));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD const T* BinarySearchLowerBound(U&& x) const noexcept {

        return Algorithms::BinarySearchLowerBound(GetMemoryBegin(), GetMemoryEnd(), std::forward<U>(x));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD T* BinarySearchUpperBound(U&& x) noexcept {

        return Algorithms::BinarySearchUpperBound(GetMemoryBegin(), GetMemoryEnd(), std::forward<U>(x));
    }

    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
    ALT_NODISCARD const T* BinarySearchUpperBound(U&& x) const noexcept {

        return Algorithms::BinarySearchUpperBound(GetMemoryBegin(), GetMemoryEnd(), std::forward<U>(x));
    }

    /**
     * @brief In place sorting.
     * @example
     * 
     * Alt::Array<usize, 4> vec{ 3, 1, 2, 0 };
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

        return Algorithms::Sort(GetMemoryBegin(), GetMemoryEnd(), std::forward<FuncT>(cmp));
    }

    void Sort() noexcept {

        return Algorithms::Sort(GetMemoryBegin(), GetMemoryEnd());
    }

    /**
     * @brief Operators for comparison with StaticStatic.
     * @example
     *
     * Alt::StaticStatic<usize, 2> vec0{ 0, 1 };
     * Alt::StaticStatic<usize, 2> vec1 = vec0;
     * 
     * assert(vec0 == vec1);
     */
    template <usize RHS_SIZE,
        std::enable_if_t<SIZE == RHS_SIZE>* = nullptr>
    ALT_NODISCARD bool operator==(const Array<T, RHS_SIZE>& rhs) const noexcept {

        return Algorithms::CompareEQ(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <usize RHS_SIZE,
        std::enable_if_t<SIZE != RHS_SIZE>* = nullptr>
    ALT_NODISCARD bool operator==(const Array<T, RHS_SIZE>&) const noexcept {

        return false;
    }

    template <usize RHS_SIZE,
        std::enable_if_t<SIZE == RHS_SIZE>* = nullptr>
    ALT_NODISCARD bool operator!=(const Array<T, RHS_SIZE>& rhs) const noexcept {

        return Algorithms::CompareNE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <usize RHS_SIZE,
        std::enable_if_t<SIZE != RHS_SIZE>* = nullptr>
    ALT_NODISCARD bool operator!=(const Array<T, RHS_SIZE>&) const noexcept {

        return true;
    }

    template <usize RHS_SIZE,
        std::enable_if_t<SIZE == RHS_SIZE>* = nullptr>
    ALT_NODISCARD bool operator<(const Array<T, RHS_SIZE>& rhs) const noexcept {

        return Algorithms::CompareLT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <usize RHS_SIZE,
        std::enable_if_t<(SIZE < RHS_SIZE)>* = nullptr>
    ALT_NODISCARD bool operator<(const Array<T, RHS_SIZE>&) const noexcept {

        return true;
    }

    template <usize RHS_SIZE,
        std::enable_if_t<(SIZE > RHS_SIZE)>* = nullptr>
    ALT_NODISCARD bool operator<(const Array<T, RHS_SIZE>&) const noexcept {

        return false;
    }

    template <usize RHS_SIZE,
        std::enable_if_t<SIZE == RHS_SIZE>* = nullptr>
    ALT_NODISCARD bool operator<=(const Array<T, RHS_SIZE>& rhs) const noexcept {

        return Algorithms::CompareLE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <usize RHS_SIZE,
        std::enable_if_t<(SIZE < RHS_SIZE)>* = nullptr>
    ALT_NODISCARD bool operator<=(const Array<T, RHS_SIZE>&) const noexcept {

        return true;
    }

    template <usize RHS_SIZE,
        std::enable_if_t<(SIZE > RHS_SIZE)>* = nullptr>
    ALT_NODISCARD bool operator<=(const Array<T, RHS_SIZE>&) const noexcept {

        return false;
    }

    template <usize RHS_SIZE,
        std::enable_if_t<SIZE == RHS_SIZE>* = nullptr>
    ALT_NODISCARD bool operator>(const Array<T, RHS_SIZE>& rhs) const noexcept {

        return Algorithms::CompareGT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <usize RHS_SIZE,
        std::enable_if_t<(SIZE < RHS_SIZE)>* = nullptr>
    ALT_NODISCARD bool operator>(const Array<T, RHS_SIZE>&) const noexcept {

        return false;
    }

    template <usize RHS_SIZE,
        std::enable_if_t<(SIZE > RHS_SIZE)>* = nullptr>
    ALT_NODISCARD bool operator>(const Array<T, RHS_SIZE>&) const noexcept {

        return true;
    }

    template <usize RHS_SIZE,
        std::enable_if_t<SIZE == RHS_SIZE>* = nullptr>
    ALT_NODISCARD bool operator>=(const Array<T, RHS_SIZE>& rhs) const noexcept {

        return Algorithms::CompareGE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <usize RHS_SIZE,
        std::enable_if_t<(SIZE < RHS_SIZE)>* = nullptr>
    ALT_NODISCARD bool operator>=(const Array<T, RHS_SIZE>&) const noexcept {

        return false;
    }

    template <usize RHS_SIZE,
        std::enable_if_t<(SIZE > RHS_SIZE)>* = nullptr>
    ALT_NODISCARD bool operator>=(const Array<T, RHS_SIZE>&) const noexcept {

        return true;
    }

private:

    ALT_FORCE_INLINE StaticVector(Unitialized_t) noexcept
    {}

    ALT_FORCE_INLINE T* GetMemoryBegin() noexcept {

        return m_Storage.GetMemoryBegin();
    }

    ALT_FORCE_INLINE const T* GetMemoryBegin() const noexcept {

        return m_Storage.GetMemoryBegin();
    }

    ALT_FORCE_INLINE T* GetMemoryEnd() noexcept {

        return m_Storage.GetMemoryEnd();
    }

    ALT_FORCE_INLINE const T* GetMemoryEnd() const noexcept {

        return m_Storage.GetMemoryEnd();
    }

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE static bool ConstructVariadicImpl(StaticVector& self, const usize i, HeadT&& head, TailT&&... tail) noexcept {

        T* pDst = self.GetMemoryBegin() + i;
        T* pResult = Memory::CloneConstructOrMoveConstructObject(pDst, std::forward<HeadT>(head));

        if (pResult == pDst) {
            return false;
        }

        return ConstructVariadicImpl(self, i + 1, std::forward<TailT>(tail)...);
    }

    ALT_FORCE_INLINE static bool ConstructVariadicImpl(StaticVector&, const usize) noexcept { return true; }

private:

    template <typename, usize, typename>
    friend class StaticVector;

    Storage m_Storage;
};

#define __ALT_STD_INTERNAL_IMPL_STATICVECTOR_FORBIDDEN__(ForbiddenT) \
template <typename T_, usize SIZE> class StaticVector<T_, SIZE, ForbiddenT>;

__ALT_STD_INTERNAL_IMPL_STATICVECTOR_FORBIDDEN__(const void);
__ALT_STD_INTERNAL_IMPL_STATICVECTOR_FORBIDDEN__(volatile void);
__ALT_STD_INTERNAL_IMPL_STATICVECTOR_FORBIDDEN__(const volatile void);

#undef __ALT_STD_INTERNAL_IMPL_STATICVECTOR_FORBIDDEN__

template <typename T, usize SIZE, typename Usize>
class IteratorCollector<StaticVector<T, SIZE, Usize>> {
public:

    using SelfT = StaticVector<T, SIZE, Usize>;

    template <typename U = T,
        std::enable_if_t<std::is_assignable_v<T&, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait> iter) noexcept {
        iferr_scope;

        while (Optional<U> val = iter.Next()) {

            self.Push(std::move(val.GetValue())) iferr_return false;
        }

        return true;
    }
};

template <typename T, usize SIZE>
class IteratorCollector<StaticVector<T, SIZE, void>> {
public:

    using SelfT = Array<T, SIZE>;

    template <typename U = T,
        std::enable_if_t<std::is_assignable_v<T&, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait> iter) noexcept {
        iferr_scope;

        usize i = 0;
        while (Optional<U> val = iter.Next()) {

            if (i >= self.GetSize()) {
                return false;
            }

            self[i] = std::move(val.GetValue());
            i += 1;
        }

        Memory::FillDefaultObjects(self.Data() + i, self.DataEnd());

        return true;
    }
};

} // namespace Alt

#endif // __ALT_STD_STATIC_VECTOR__