#pragma once

#ifndef __ALT_STD_STATIC_QUEUE__
#define __ALT_STD_STATIC_QUEUE__

#include "../IfErr.h"
#include "../TypeTraits.h"

#include "../Algorithms/Algorithms.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Memory/MaybeUninit.h"

#include "../String/Impl/FormatPreInclude.h"

#include "../Utils/Slice.h"
#include "../Utils/Optional.h"
#include "../Utils/ScopeDefer.h"

#include "QueueIterator.h"

namespace Alt {

template <typename T, usize SIZE>
class StaticQueue;

namespace TypeTraits {

template <typename T, usize SIZE>
struct __is_trivially_reallocatable__<StaticQueue<T, SIZE>>
    : is_trivially_reallocatable_t<T>
{};

} // namespace TypeTraits

/** \cond */
namespace _StaticQueueDetails {

struct Rule5Copy_t {};
struct Rule5Move_t {};

constexpr static Rule5Copy_t Rule5Copy = Rule5Copy_t{};
constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

template <typename T, usize SIZE, typename Usize>
class StaticQueueStorageCore {
public:

    ALT_FORCE_INLINE T* GetMemoryBeginImpl() noexcept {

        return m_Memory->GetPtr();
    }

    ALT_FORCE_INLINE const T* GetMemoryBeginImpl() const noexcept {

        return m_Memory->GetPtr();
    }

    ALT_FORCE_INLINE usize GetOffsetImpl() const noexcept {

        return static_cast<usize>(m_Offset);
    }

    ALT_FORCE_INLINE Usize& GetOffsetRefImpl() noexcept {

        return m_Offset;
    }

    ALT_FORCE_INLINE const Usize& GetOffsetRefImpl() const noexcept {

        return m_Offset;
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

    Usize m_Offset = 0;
    Usize m_Size = 0;
};

template <typename T, typename Usize>
class StaticQueueStorageCore<T, 0, Usize> {
public:

    ALT_FORCE_INLINE T* GetMemoryBeginImpl() noexcept {

        return nullptr;
    }

    ALT_FORCE_INLINE const T* GetMemoryBeginImpl() const noexcept {

        return nullptr;
    }

    ALT_FORCE_INLINE usize GetOffsetImpl() const noexcept {

        return static_cast<usize>(m_SomethingForRef);
    }

    ALT_FORCE_INLINE Usize& GetOffsetRefImpl() noexcept {

        return m_SomethingForRef;
    }

    ALT_FORCE_INLINE const Usize& GetOffsetRefImpl() const noexcept {

        return m_SomethingForRef;
    }

    ALT_FORCE_INLINE usize GetSizeImpl() const noexcept {

        return static_cast<usize>(m_SomethingForRef);
    }

    ALT_FORCE_INLINE Usize& GetSizeRefImpl() noexcept {

        return m_SomethingForRef;
    }

    ALT_FORCE_INLINE const Usize& GetSizeRefImpl() const noexcept {

        return m_SomethingForRef;
    }

private:

    Usize m_SomethingForRef = 0;
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    typename Usize,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_destructible_v<T>
>
struct StaticQueueStorageDestruct;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageDestruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticQueueStorageCore<T, SIZE, Usize>
{
    StaticQueueStorageDestruct() noexcept = default;

    StaticQueueStorageDestruct(const StaticQueueStorageDestruct&) noexcept = default;
    StaticQueueStorageDestruct& operator=(const StaticQueueStorageDestruct&) noexcept = default;

    StaticQueueStorageDestruct(StaticQueueStorageDestruct&&) noexcept = default;
    StaticQueueStorageDestruct& operator=(StaticQueueStorageDestruct&&) noexcept = default;

    ~StaticQueueStorageDestruct() noexcept = delete;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageDestruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticQueueStorageCore<T, SIZE, Usize>
{
    StaticQueueStorageDestruct() noexcept = default;

    StaticQueueStorageDestruct(const StaticQueueStorageDestruct&) noexcept = default;
    StaticQueueStorageDestruct& operator=(const StaticQueueStorageDestruct&) noexcept = default;

    StaticQueueStorageDestruct(StaticQueueStorageDestruct&&) noexcept = default;
    StaticQueueStorageDestruct& operator=(StaticQueueStorageDestruct&&) noexcept = default;

    ~StaticQueueStorageDestruct() noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageDestruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticQueueStorageCore<T, SIZE, Usize>
{
    StaticQueueStorageDestruct() noexcept = default;

    StaticQueueStorageDestruct(const StaticQueueStorageDestruct&) noexcept = default;
    StaticQueueStorageDestruct& operator=(const StaticQueueStorageDestruct&) noexcept = default;

    StaticQueueStorageDestruct(StaticQueueStorageDestruct&&) noexcept = default;
    StaticQueueStorageDestruct& operator=(StaticQueueStorageDestruct&&) noexcept = default;

    ~StaticQueueStorageDestruct() noexcept {

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
struct StaticQueueStorageMoveAssign;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageMoveAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticQueueStorageDestruct<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageMoveAssign() noexcept = default;

    StaticQueueStorageMoveAssign(const StaticQueueStorageMoveAssign&) noexcept = default;
    StaticQueueStorageMoveAssign& operator=(const StaticQueueStorageMoveAssign&) noexcept = default;

    StaticQueueStorageMoveAssign(StaticQueueStorageMoveAssign&&) noexcept = default;
    StaticQueueStorageMoveAssign& operator=(StaticQueueStorageMoveAssign&&) noexcept = delete;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageMoveAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticQueueStorageDestruct<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageMoveAssign() noexcept = default;

    StaticQueueStorageMoveAssign(const StaticQueueStorageMoveAssign&) noexcept = default;
    StaticQueueStorageMoveAssign& operator=(const StaticQueueStorageMoveAssign&) noexcept = default;

    StaticQueueStorageMoveAssign(StaticQueueStorageMoveAssign&&) noexcept = default;

    StaticQueueStorageMoveAssign& operator=(StaticQueueStorageMoveAssign&& rhs) noexcept {

        // we can do better - move only "active" elements
        static_cast<SelfT*>(this)->Assign(Rule5Move, static_cast<SelfT&>(rhs));
        return *this;
    }
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageMoveAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticQueueStorageDestruct<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageMoveAssign() noexcept = default;

    StaticQueueStorageMoveAssign(const StaticQueueStorageMoveAssign&) noexcept = default;
    StaticQueueStorageMoveAssign& operator=(const StaticQueueStorageMoveAssign&) noexcept = default;

    StaticQueueStorageMoveAssign(StaticQueueStorageMoveAssign&&) noexcept = default;

    StaticQueueStorageMoveAssign& operator=(StaticQueueStorageMoveAssign&& rhs) noexcept {

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
struct StaticQueueStorageMoveConstruct;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageMoveConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticQueueStorageMoveAssign<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageMoveConstruct() noexcept = default;

    StaticQueueStorageMoveConstruct(const StaticQueueStorageMoveConstruct&) noexcept = default;
    StaticQueueStorageMoveConstruct& operator=(const StaticQueueStorageMoveConstruct&) noexcept = default;

    StaticQueueStorageMoveConstruct(StaticQueueStorageMoveConstruct&&) noexcept = delete;
    StaticQueueStorageMoveConstruct& operator=(StaticQueueStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageMoveConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticQueueStorageMoveAssign<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageMoveConstruct() noexcept = default;

    StaticQueueStorageMoveConstruct(const StaticQueueStorageMoveConstruct&) noexcept = default;
    StaticQueueStorageMoveConstruct& operator=(const StaticQueueStorageMoveConstruct&) noexcept = default;

    StaticQueueStorageMoveConstruct(StaticQueueStorageMoveConstruct&& rhs) noexcept {

        // we can do better - move only "active" elements
        static_cast<SelfT*>(this)->Construct(Rule5Move, static_cast<SelfT&>(rhs));
    }

    StaticQueueStorageMoveConstruct& operator=(StaticQueueStorageMoveConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageMoveConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticQueueStorageMoveAssign<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageMoveConstruct() noexcept = default;

    StaticQueueStorageMoveConstruct(const StaticQueueStorageMoveConstruct&) noexcept = default;
    StaticQueueStorageMoveConstruct& operator=(const StaticQueueStorageMoveConstruct&) noexcept = default;

    StaticQueueStorageMoveConstruct(StaticQueueStorageMoveConstruct&& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Move, static_cast<SelfT&>(rhs));
    }

    StaticQueueStorageMoveConstruct& operator=(StaticQueueStorageMoveConstruct&&) noexcept = default;
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    typename Usize,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_assignable_v<T>
>
struct StaticQueueStorageCopyAssign;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageCopyAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticQueueStorageMoveConstruct<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageCopyAssign() noexcept = default;

    StaticQueueStorageCopyAssign(const StaticQueueStorageCopyAssign&) noexcept = default;
    StaticQueueStorageCopyAssign& operator=(const StaticQueueStorageCopyAssign&) noexcept = delete;

    StaticQueueStorageCopyAssign(StaticQueueStorageCopyAssign&&) noexcept = default;
    StaticQueueStorageCopyAssign& operator=(StaticQueueStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageCopyAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticQueueStorageMoveConstruct<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageCopyAssign() noexcept = default;

    StaticQueueStorageCopyAssign(const StaticQueueStorageCopyAssign&) noexcept = default;

    StaticQueueStorageCopyAssign& operator=(const StaticQueueStorageCopyAssign& rhs) noexcept {

        // we can do better - copy only "active" elements
        static_cast<SelfT*>(this)->Assign(Rule5Copy, static_cast<const SelfT&>(rhs));
        return *this;
    }

    StaticQueueStorageCopyAssign(StaticQueueStorageCopyAssign&&) noexcept = default;
    StaticQueueStorageCopyAssign& operator=(StaticQueueStorageCopyAssign&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageCopyAssign<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticQueueStorageMoveConstruct<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageCopyAssign() noexcept = default;

    StaticQueueStorageCopyAssign(const StaticQueueStorageCopyAssign&) noexcept = default;

    StaticQueueStorageCopyAssign& operator=(const StaticQueueStorageCopyAssign& rhs) noexcept {

        static_cast<SelfT*>(this)->Assign(Rule5Copy, static_cast<const SelfT&>(rhs));
        return *this;
    }

    StaticQueueStorageCopyAssign(StaticQueueStorageCopyAssign&&) noexcept = default;
    StaticQueueStorageCopyAssign& operator=(StaticQueueStorageCopyAssign&&) noexcept = default;
};

template <
    typename SelfT,
    typename T,
    usize SIZE,
    typename Usize,
    _MaybeUninitDetails::Rule5Trait RULE5_TRAIT = _MaybeUninitDetails::rule5_copy_constructible_v<T>
>
struct StaticQueueStorageCopyConstruct;

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageCopyConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Deleted>
    : StaticQueueStorageCopyAssign<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageCopyConstruct() noexcept = default;

    StaticQueueStorageCopyConstruct(const StaticQueueStorageCopyConstruct&) noexcept = delete;
    StaticQueueStorageCopyConstruct& operator=(const StaticQueueStorageCopyConstruct&) noexcept = default;

    StaticQueueStorageCopyConstruct(StaticQueueStorageCopyConstruct&&) noexcept = default;
    StaticQueueStorageCopyConstruct& operator=(StaticQueueStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageCopyConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::Trivial>
    : StaticQueueStorageCopyAssign<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageCopyConstruct() noexcept = default;

    StaticQueueStorageCopyConstruct(const StaticQueueStorageCopyConstruct& rhs) noexcept {

        // we can do better - copy only "active" elements
        static_cast<SelfT*>(this)->Construct(Rule5Copy, static_cast<const SelfT&>(rhs));
    }

    StaticQueueStorageCopyConstruct& operator=(const StaticQueueStorageCopyConstruct&) noexcept = default;

    StaticQueueStorageCopyConstruct(StaticQueueStorageCopyConstruct&&) noexcept = default;
    StaticQueueStorageCopyConstruct& operator=(StaticQueueStorageCopyConstruct&&) noexcept = default;
};

template <typename SelfT, typename T, usize SIZE, typename Usize>
struct StaticQueueStorageCopyConstruct<SelfT, T, SIZE, Usize, _MaybeUninitDetails::Rule5Trait::NonTrivial>
    : StaticQueueStorageCopyAssign<SelfT, T, SIZE, Usize>
{
    StaticQueueStorageCopyConstruct() noexcept = default;

    StaticQueueStorageCopyConstruct(const StaticQueueStorageCopyConstruct& rhs) noexcept {

        static_cast<SelfT*>(this)->Construct(Rule5Copy, static_cast<const SelfT&>(rhs));
    }

    StaticQueueStorageCopyConstruct& operator=(const StaticQueueStorageCopyConstruct&) noexcept = default;

    StaticQueueStorageCopyConstruct(StaticQueueStorageCopyConstruct&&) noexcept = default;
    StaticQueueStorageCopyConstruct& operator=(StaticQueueStorageCopyConstruct&&) noexcept = default;
};

template <typename T, usize SIZE, typename Usize>
class StaticQueueStorage
    : public StaticQueueStorageCopyConstruct<
        StaticQueueStorage<T, SIZE, Usize>,
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

    ALT_FORCE_INLINE StaticQueueStorage() noexcept = default;

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

    ALT_FORCE_INLINE Usize& GetOffsetRef() noexcept {

        return this->GetOffsetRefImpl();
    }

    ALT_FORCE_INLINE const Usize& GetOffsetRef() const noexcept {

        return this->GetOffsetRefImpl();
    }

    ALT_FORCE_INLINE usize GetOffset() const noexcept {

        return this->GetOffsetImpl();
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
    ALT_FORCE_INLINE void Construct(_StaticQueueDetails::Rule5Copy_t, const StaticQueueStorage<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        const usize offset = rhs.GetOffset();
        const usize size = rhs.GetSize();

        T* pQueueBegin0 = GetMemoryBegin();

        const T* pMemoryBegin1 = rhs.GetMemoryBegin();
        const T* pQueueBegin1 = pMemoryBegin1 + offset;

        const usize sizeBottomCapacity = static_cast<usize>(rhs.GetMemoryEnd() - pQueueBegin1);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);
        const usize sizeTop = size - sizeBottom;

        pQueueBegin0 = Memory::CopyConstructObjectsNonOverlap(pQueueBegin0, pQueueBegin1, pQueueBegin1 + sizeBottom);
        Memory::CopyConstructObjectsNonOverlap(pQueueBegin0, pMemoryBegin1, pMemoryBegin1 + sizeTop);

        GetSizeRef() = static_cast<Usize>(size);
    }

    template <typename U = T,
        std::enable_if_t<is_copy_constructible_v<U, SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Construct(_StaticQueueDetails::Rule5Copy_t, const StaticQueueStorage<U, SIZE, Usize>& rhs) noexcept {

        const usize offset = rhs.GetOffset();
        const usize size = rhs.GetSize();

        T* pMemoryBegin0 = GetMemoryBegin();
        T* pQueueBegin0 = pMemoryBegin0 + offset;

        const T* pMemoryBegin1 = rhs.GetMemoryBegin();
        const T* pQueueBegin1 = pMemoryBegin1 + offset;

        const usize sizeBottomCapacity = static_cast<usize>(rhs.GetMemoryEnd() - pQueueBegin1);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);
        const usize sizeTop = size - sizeBottom;

        Memory::CopyConstructObjectsNonOverlap(pQueueBegin0, pQueueBegin1, pQueueBegin1 + sizeBottom);
        Memory::CopyConstructObjectsNonOverlap(pMemoryBegin0, pMemoryBegin1, pMemoryBegin1 + sizeTop);

        GetOffsetRef() = static_cast<Usize>(offset);
        GetSizeRef() = static_cast<Usize>(size);
    }

    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<is_copy_assignable_v<U, RHS_SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Assign(_StaticQueueDetails::Rule5Copy_t, const StaticQueueStorage<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return;
        }

        const usize size0 = GetSize();
        const usize size1 = rhs.GetSize();

        const usize offset0 = GetOffset();
        const usize offset1 = rhs.GetOffset();

        T* pMemoryBegin0 = GetMemoryBegin();
        T* pQueueBegin0 = pMemoryBegin0 + offset0;

        const T* pMemoryBegin1 = rhs.GetMemoryBegin();
        const T* pQueueBegin1 = pMemoryBegin1 + offset1;

        const usize sizeBottom0Capacity = static_cast<usize>(GetMemoryEnd() - pQueueBegin0);
        const usize sizeBottom0 = Num::Min(sizeBottom0Capacity, size0);
        const usize sizeTop0 = size0 - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(rhs.GetMemoryEnd() - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(sizeBottom1Capacity, size1);
        const usize sizeTop1 = size1 - sizeBottom1;

        const usize sizeBottomToCopy = Num::Min(sizeBottom0, sizeBottom1);
        const usize sizeTopToCopy = Num::Min(sizeBottom0 - sizeBottomToCopy, sizeTop1);
        const usize sizeBottom0CapacityRemainder = sizeBottom0Capacity - sizeBottom0;
        const usize sizeBottomToConstruct = Num::Min(sizeBottom0CapacityRemainder, sizeBottom1 - sizeBottomToCopy);
        const usize sizeTopToConstruct = Num::Min(sizeBottom0CapacityRemainder - sizeBottomToConstruct, sizeTop1 - sizeTopToCopy);

        T* pDstBeginBottom = nullptr;
        T* pDstBeginTop = nullptr;

        const T* pSrcBeginBottom = pQueueBegin1;
        const T* pSrcBeginTop = pMemoryBegin1;

        {
            pDstBeginBottom = Memory::CopyObjectsNonOverlap(
                pQueueBegin0, pSrcBeginBottom, pSrcBeginBottom + sizeBottomToCopy
            );
            pSrcBeginBottom += sizeBottomToCopy;

            pDstBeginTop = Memory::CopyObjectsNonOverlap(
                pDstBeginBottom, pSrcBeginTop, pSrcBeginTop + sizeTopToCopy
            );
            pSrcBeginTop += sizeTopToCopy;

            Memory::DestroyObjects(pDstBeginTop, pQueueBegin0 + sizeBottom0);

            pDstBeginBottom = Memory::CopyConstructObjectsNonOverlap(
                pDstBeginTop, pSrcBeginBottom, pSrcBeginBottom + sizeBottomToConstruct
            );
            pSrcBeginBottom += sizeBottomToConstruct;

            pDstBeginTop = Memory::CopyConstructObjectsNonOverlap(
                pDstBeginBottom, pSrcBeginTop, pSrcBeginTop + sizeTopToConstruct
            );
            pSrcBeginTop += sizeTopToConstruct;
        }

        const usize sizeBottom1Remainder = sizeBottom1 - sizeBottomToCopy - sizeBottomToConstruct;
        const usize sizeTop1Remainder = sizeTop1 - sizeTopToCopy - sizeTopToConstruct;

        {
            const usize sizeBottomToCopy = Num::Min(sizeTop0, sizeBottom1Remainder);
            pDstBeginBottom = Memory::CopyObjectsNonOverlap(
                pMemoryBegin0, pSrcBeginBottom, pSrcBeginBottom + sizeBottomToCopy
            );
            pSrcBeginBottom += sizeBottomToCopy;

            const usize sizeTopToCopy = Num::Min(sizeTop0 - sizeBottomToCopy, sizeTop1Remainder);
            pDstBeginTop = Memory::CopyObjectsNonOverlap(
                pDstBeginBottom, pSrcBeginTop, pSrcBeginTop + sizeTopToCopy
            );
            pSrcBeginTop += sizeTopToCopy;

            Memory::DestroyObjects(pDstBeginTop, pMemoryBegin0 + sizeTop0);

            const usize sizeBottomToConstruct = sizeBottom1Remainder - sizeBottomToCopy;
            pDstBeginBottom = Memory::CopyConstructObjectsNonOverlap(
                pDstBeginTop, pSrcBeginBottom, pSrcBeginBottom + sizeBottomToConstruct
            );

            const usize sizeTopToConstruct = sizeTop1Remainder - sizeTopToCopy;
            pDstBeginTop = Memory::CopyConstructObjectsNonOverlap(
                pDstBeginBottom, pSrcBeginTop, pSrcBeginTop + sizeTopToConstruct
            );
        }

        GetSizeRef() = static_cast<Usize>(size1);
    }

    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<is_move_constructible_v<U, RHS_SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Construct(_StaticQueueDetails::Rule5Move_t, StaticQueueStorage<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        const usize offset = rhs.GetOffset();
        const usize size = rhs.GetSize();

        T* pQueueBegin0 = GetMemoryBegin();

        T* pMemoryBegin1 = rhs.GetMemoryBegin();
        T* pQueueBegin1 = pMemoryBegin1 + offset;

        const usize sizeBottomCapacity = static_cast<usize>(rhs.GetMemoryEnd() - pQueueBegin1);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);
        const usize sizeTop = size - sizeBottom;

        const Memory::MoveFromFirstElementResult<T> result = Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
            pQueueBegin0, pQueueBegin1, pQueueBegin1 + sizeBottom
        );

        Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
            result.pDstEnd, pMemoryBegin1, pMemoryBegin1 + sizeTop
        );

        GetSizeRef() = static_cast<Usize>(size);

        rhs.GetOffsetRef() = 0;
        rhs.GetSizeRef() = 0;
    }

    template <typename U = T,
        std::enable_if_t<is_move_constructible_v<U, SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Construct(_StaticQueueDetails::Rule5Move_t, StaticQueueStorage<U, SIZE, Usize>& rhs) noexcept {

        const usize offset = rhs.GetOffset();
        const usize size = rhs.GetSize();

        T* pMemoryBegin0 = GetMemoryBegin();
        T* pQueueBegin0 = pMemoryBegin0 + offset;

        T* pMemoryBegin1 = rhs.GetMemoryBegin();
        T* pQueueBegin1 = pMemoryBegin1 + offset;

        const usize sizeBottomCapacity = static_cast<usize>(rhs.GetMemoryEnd() - pQueueBegin1);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);
        const usize sizeTop = size - sizeBottom;

        Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(pQueueBegin0, pQueueBegin1, pQueueBegin1 + sizeBottom);
        Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(pMemoryBegin0, pMemoryBegin1, pMemoryBegin1 + sizeTop);

        GetOffsetRef() = static_cast<Usize>(offset);
        GetSizeRef() = static_cast<Usize>(size);

        rhs.GetOffsetRef() = 0;
        rhs.GetSizeRef() = 0;
    }

    template <typename U = T, usize RHS_SIZE = SIZE, typename RHSUsize = Usize,
        std::enable_if_t<is_move_assignable_v<U, RHS_SIZE>>* = nullptr>
    ALT_FORCE_INLINE void Assign(_StaticQueueDetails::Rule5Move_t, StaticQueueStorage<U, RHS_SIZE, RHSUsize>& rhs) noexcept {

        if (static_cast<const void*>(this) == &rhs) {
            return;
        }

        const usize size0 = GetSize();
        const usize size1 = rhs.GetSize();

        const usize offset0 = GetOffset();
        const usize offset1 = rhs.GetOffset();

        T* pMemoryBegin0 = GetMemoryBegin();
        T* pQueueBegin0 = pMemoryBegin0 + offset0;

        T* pMemoryBegin1 = rhs.GetMemoryBegin();
        T* pQueueBegin1 = pMemoryBegin1 + offset1;

        const usize sizeBottom0Capacity = static_cast<usize>(GetMemoryEnd() - pQueueBegin0);
        const usize sizeBottom0 = Num::Min(sizeBottom0Capacity, size0);
        const usize sizeTop0 = size0 - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(rhs.GetMemoryEnd() - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(sizeBottom1Capacity, size1);
        const usize sizeTop1 = size1 - sizeBottom1;

        Memory::MoveFromFirstElementResult<T> resultBottom;
        Memory::MoveFromFirstElementResult<T> resultTop;

        const usize sizeBottomToMove = Num::Min(sizeBottom0, sizeBottom1);
        const usize sizeTopToMove = Num::Min(sizeBottom0 - sizeBottomToMove, sizeTop1);
        const usize sizeBottom0CapacityRemainder = sizeBottom0Capacity - sizeBottom0;
        const usize sizeBottomToConstruct = Num::Min(sizeBottom0CapacityRemainder, sizeBottom1 - sizeBottomToMove);
        const usize sizeTopToConstruct = Num::Min(sizeBottom0CapacityRemainder - sizeBottomToConstruct, sizeTop1 - sizeTopToMove);

        {
            resultBottom = Memory::MoveDestructibleObjectsFromFirstElementNonOverlap(
                pQueueBegin0, pQueueBegin1, pQueueBegin1 + sizeBottomToMove
            );

            resultTop = Memory::MoveDestructibleObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, pMemoryBegin1, pMemoryBegin1 + sizeTopToMove
            );

            Memory::DestroyObjects(resultTop.pDstEnd, pQueueBegin0 + sizeBottom0);

            resultBottom = Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
                resultTop.pDstEnd, resultBottom.pSrcEnd, resultBottom.pSrcEnd + sizeBottomToConstruct
            );

            resultTop = Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
                resultBottom.pDstEnd, resultTop.pSrcEnd, resultTop.pSrcEnd + sizeTopToConstruct
            );
        }

        const usize sizeBottom1Remainder = sizeBottom1 - sizeBottomToMove - sizeBottomToConstruct;
        const usize sizeTop1Remainder = sizeTop1 - sizeTopToMove - sizeTopToConstruct;

        {
            const usize sizeBottomToMove = Num::Min(sizeTop0, sizeBottom1Remainder);
            resultBottom = Memory::MoveDestructibleObjectsFromFirstElementNonOverlap(
                pMemoryBegin0, resultBottom.pSrcEnd, resultBottom.pSrcEnd + sizeBottomToMove
            );

            const usize sizeTopToMove = Num::Min(sizeTop0 - sizeBottomToMove, sizeTop1Remainder);
            resultTop = Memory::MoveDestructibleObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, resultTop.pSrcEnd, resultTop.pSrcEnd + sizeTopToMove
            );

            Memory::DestroyObjects(resultTop.pDstEnd, pMemoryBegin0 + sizeTop0);

            const usize sizeBottomToConstruct = sizeBottom1Remainder - sizeBottomToMove;
            resultBottom = Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
                resultTop.pDstEnd, resultBottom.pSrcEnd, resultBottom.pSrcEnd + sizeBottomToConstruct
            );

            const usize sizeTopToConstruct = sizeTop1Remainder - sizeTopToMove;
            resultTop = Memory::MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
                resultBottom.pDstEnd, resultTop.pSrcEnd, resultTop.pSrcEnd + sizeTopToConstruct
            );
        }

        GetSizeRef() = static_cast<Usize>(size1);

        rhs.GetOffsetRef() = 0;
        rhs.GetSizeRef() = 0;
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        const usize size = GetSize();

        const usize offset = GetOffset();

        const T* pMemoryBegin = GetMemoryBegin();
        const T* pQueueBegin = pMemoryBegin + offset;

        if (size == 0) {
            return;
        }

        const usize sizeBottomCapacity = static_cast<usize>(GetMemoryEnd() - pQueueBegin);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);

        Memory::DestroyObjects(pQueueBegin, pQueueBegin + sizeBottom);
        Memory::DestroyObjects(pMemoryBegin, pMemoryBegin + size - sizeBottom);

        GetOffsetRef() = 0;
        GetSizeRef() = 0;
    }

private:

    template <typename, usize, typename>
    friend class StaticQueueStorage;
};

} // namespace _StaticQueueDetails
/** \endcond */

/**
 * @brief A fixed-capacity deque utilizing a contiguous memory block.
 * @details The Queue provides $O(1)$ insertion and removal from both the front and back - manages
 * a circular/sliding buffer window over a memory block.
 * 
 * @tparam T The type of elements stored in the queue.
 * @tparam SIZE The capacity of the static memory block.
 */
template <typename T_, usize SIZE>
class StaticQueue {
private:

    using T = TypeTraits::clean_t<T_>;
    using Usize = TypeTraits::count_to_type_t<SIZE>;

    using Storage = _StaticQueueDetails::StaticQueueStorage<T, SIZE, Usize>;

    template <typename U>
    using is_pushable_t = std::disjunction<TypeTraits::is_only_clonable_to_from_t<T, U>, std::is_constructible<T, U>>;

    template <typename U>
    constexpr static bool is_pushable_v = is_pushable_t<U>::value;

    template <typename ...ArgsT>
    constexpr static bool is_populated_v = sizeof...(ArgsT) != 0;

public:

    /**
     * @brief Creates a StaticQueue with a specified initial size.
     * @tparam SIZE_ The desired initial number of elements. Must be <= SIZE.
     * @tparam U The type of the value to use for initialization. Must be default constructible.
     * @return A StaticQueue initialized with SIZE_ elements.
     * @example
     * 
     * auto q = Alt::StaticQueue<usize, 10>::Create<5>();
     * assert(q.GetSize() == 5);
     * assert(q.GetCapacity() == 10);
     */
    template <usize SIZE_, typename U = T,
        std::enable_if_t<SIZE_ != 0 && SIZE_ <= SIZE && std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD static StaticQueue Create() noexcept {
        iferr_scope;

        StaticQueue result{};
        result.ResizeBack(SIZE_) iferr_panic_only_dev("checked by template - never panic");

        return result;
    }

    template <usize SIZE_, typename U = T,
        std::enable_if_t<SIZE_ == 0>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static StaticQueue Create() noexcept {

        return StaticQueue{};
    }

    /**
     * @brief Creates a StaticQueue with a specified initial size, where all elements are copies of val.
     * @tparam SIZE_ The desired initial number of elements. Must be <= SIZE.
     * @tparam U The type of the value. Must be constructible from U.
     * @param val The value used to populate the queue.
     * @return A StaticQueue initialized with SIZE_ copies of val.
     * @example
     * 
     * auto q = Alt::StaticQueue<usize, 10>::Create<5>(4);
     * assert(q.GetSize() == 5);
     * assert(q.GetCapacity() == 10);
     * 
     * assert(q[0] == 4);
     */
    template <usize SIZE_, typename U = T,
        std::enable_if_t<SIZE_ <= SIZE && (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)>* = nullptr>
    ALT_NODISCARD static StaticQueue Create(const U& val) noexcept {
        iferr_scope;

        StaticQueue result{};
        result.ResizeBack(SIZE_, val) iferr_panic_only_dev("checked by template - never panic");

        return result;
    }

    /**
     * @brief Creates an Optional StaticQueue with a specified size.
     * @tparam U The type of the value. Must be default constructible.
     * @param size The number of elements to initialize. Must be <= SIZE.
     * @return An Optional containing the StaticQueue if successful, or Nullopt if size > SIZE.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 10> q = Alt::StaticQueue<usize, 10>::Create(5) iferr_return;
     * assert(q.GetSize() == 5);
     * assert(q.GetCapacity() == 10);
     */
    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD static Optional<StaticQueue> Create(const usize size = 0) noexcept {
        iferr_scope;

        StaticQueue result{};
        result.ResizeBack(size) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Creates an Optional StaticQueue with a specified size and initial value.
     * @tparam U The type of the value. Must be constructible from U.
     * @param size The number of elements to initialize. Must be <= SIZE.
     * @param val The value to populate the queue with.
     * @return An Optional containing the StaticQueue if successful, or Nullopt if size > SIZE.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 10> q = Alt::StaticQueue<usize, 10>::Create(5, 4) iferr_return;
     * assert(q.GetSize() == 5);
     * assert(q.GetCapacity() == 10);
     * 
     * assert(q[0] == 4);
     */
    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD static Optional<StaticQueue> Create(const usize size, const U& val) noexcept {
        iferr_scope;

        StaticQueue result{};
        result.ResizeBack(size, val) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Creates a StaticQueue using a variadic list of arguments.
     * @tparam ArgsT The types of the arguments. Must be constructible to T.
     * @param args The arguments used to construct the elements.
     * @return A StaticQueue containing the provided arguments.
     * @example
     * 
     * Alt::StaticQueue<usize, 10> q = Alt::StaticQueue<usize, 10>::CreatePush(4, 5, 6, 7, 8);
     * assert(q.GetSize() == 5);
     * assert(q.GetCapacity() == 10);
     * 
     * assert(q[0] == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<(is_populated_v<ArgsT...> && sizeof...(ArgsT) <= SIZE) && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    ALT_NODISCARD static Optional<StaticQueue> CreatePush(ArgsT&&... args) noexcept {
        iferr_scope;

        StaticQueue self;
        PushBackVariadicImpl(self, std::forward<ArgsT>(args)...) iferr_return Nullopt;

        return self;
    }

    /**
     * @brief Default constructor. Creates an empty queue.
     * @example
     * 
     * Alt::StaticQueue<usize, 10> q;
     * assert(q.IsEmpty());
     * assert(q.GetCapacity() == 10);
     */
    ALT_FORCE_INLINE StaticQueue() noexcept = default;

    /**
     * @brief Creates a StaticQueue using a variadic list of arguments.
     * @tparam ArgsT The types of the arguments. Must be constructible to T.
     * @param args The arguments used to construct the elements.
     * @return A StaticQueue containing the provided arguments.
     * @example
     * 
     * Alt::StaticQueue<usize, 10> q{ 4, 5, 6, 7, 8 };
     * assert(q.GetSize() == 5);
     * assert(q.GetCapacity() == 10);
     * 
     * assert(q[0] == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<(sizeof...(ArgsT) <= SIZE) && std::conjunction_v<std::is_constructible<T, ArgsT&&>...>>* = nullptr>
    StaticQueue(ArgsT&&... args) noexcept {
        iferr_scope;

        PushBackVariadicImpl(*this, std::forward<ArgsT>(args)...) iferr_panic_only_dev("checked by template - never panic");
    }

    /**
     * @brief Copy constructor from another StaticQueue.
     * @tparam U The type of the source queue. Must be the same as T.
     * @tparam RHS_SIZE The capacity of the source queue. Must be <= SIZE.
     * @param rhs The source queue to copy from.
     * @example
     * 
     * Alt::StaticQueue<usize, 5> q0;
     * Alt::StaticQueue<usize, 10> q1 = q0; // q1.GetSize() >= q0.GetSize()
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<Storage::template is_copy_constructible_v<U, RHS_SIZE>>* = nullptr>
    StaticQueue(const StaticQueue<U, RHS_SIZE>& rhs) noexcept {

        m_Storage.Construct(_StaticQueueDetails::Rule5Copy, rhs.m_Storage);
    }

    /**
     * @brief Copy assignment operator.
     * @tparam U The type of the source queue. Must be the same as T.
     * @tparam RHS_SIZE The capacity of the source queue. Must be <= SIZE.
     * @param rhs The source queue to assign from.
     * @return A reference to this queue.
     * 
     * @example
     * 
     * Alt::StaticQueue<usize, 5> q0;
     * Alt::StaticQueue<usize, 10> q1;
     * 
     * q1 = q0; // q1.GetSize() >= q0.GetSize()
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<Storage::template is_copy_assignable_v<U, RHS_SIZE>>* = nullptr>
    StaticQueue& operator=(const StaticQueue<U, RHS_SIZE>& rhs) noexcept {

        m_Storage.Assign(_StaticQueueDetails::Rule5Copy, rhs.m_Storage);
        return *this;
    }

    /**
     * @brief Move constructor from another StaticQueue.
     * @tparam U The type of the source queue. Must be the same as T.
     * @tparam RHS_SIZE The capacity of the source queue. Must be <= SIZE.
     * @param rhs The source queue to move from.
     * 
     * @example
     * 
     * Alt::StaticQueue<usize, 5> q0;
     * Alt::StaticQueue<usize, 10> q1 = std::move(q0); // q1.GetSize() >= q0.GetSize()
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<Storage::template is_move_constructible_v<U, RHS_SIZE>>* = nullptr>
    StaticQueue(StaticQueue<U, RHS_SIZE>&& rhs) noexcept {

        m_Storage.Construct(_StaticQueueDetails::Rule5Move, rhs.m_Storage);
    }

    /**
     * @brief Move assignment operator.
     * @tparam U The type of the source queue. Must be the same as T.
     * @tparam RHS_SIZE The capacity of the source queue. Must be <= SIZE.
     * @param rhs The source queue to move from.
     * @return A reference to this queue.
     * 
     * @example
     * 
     * Alt::StaticQueue<usize, 5> q0;
     * Alt::StaticQueue<usize, 10> q1;
     * 
     * q1 = std::move(q0); // q1.GetSize() >= q0.GetSize()
     */
    template <typename U = T, usize RHS_SIZE = SIZE,
        std::enable_if_t<Storage::template is_move_assignable_v<U, RHS_SIZE>>* = nullptr>
    StaticQueue& operator=(StaticQueue<U, RHS_SIZE>&& rhs) noexcept {

        m_Storage.Assign(_StaticQueueDetails::Rule5Move, rhs.m_Storage);
        return *this;
    }

    /**
     * @brief Returns the number of elements currently in the queue.
     * @return The size of the queue.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {

        return m_Storage.GetSize();
    }

    /**
     * @brief Returns the maximum capacity of the queue.
     * @return The constant capacity (SIZE).
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetCapacity() const noexcept {

        return SIZE;
    }

    /**
     * @brief Checks if the queue contains no elements.
     * @return True if empty, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        return GetSize() == 0;
    }

    /**
     * @brief Checks if the queue contains at least one element.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept {

        return !IsEmpty();
    }

    /**
     * @brief Checks if the queue full - any push will return false.
     * @return True if full, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsFull() const noexcept {

        return GetSize() == SIZE;
    }

    /**
     * @brief Removes all elements from the queue.
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
     * Alt::StaticQueue<usize, 4> q = Alt::StaticQueue<usize, 4>::CreatePush(0, 2);
     * assert(q[0] == 0);
     * assert(q[1] == 2);
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize sizeBottom = static_cast<usize>(pMemoryEnd - pQueueBegin);
        return i < sizeBottom ? pQueueBegin[i] : pMemoryBegin[i - sizeBottom];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize i) const noexcept {
        ALT_ASSERT(i < GetSize());

        const T* pQueueBegin = GetQueueBegin();
        const T* pMemoryBegin = GetMemoryBegin();
        const T* pMemoryEnd = GetMemoryEnd();

        const usize sizeBottom = static_cast<usize>(pMemoryEnd - pQueueBegin);
        return i < sizeBottom ? pQueueBegin[i] : pMemoryBegin[i - sizeBottom];
    }

    /**
     * @brief Returns a reference to the first (front) element.
     * @return Reference to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetFirst() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *GetQueueBegin();
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetFirst() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *GetQueueBegin();
    }

    ALT_NODISCARD ALT_FORCE_INLINE T& GetFront() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *GetQueueBegin();
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetFront() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *GetQueueBegin();
    }

    /**
     * @brief Returns a reference to the last (back) element.
     * @return Reference to the last element.
     * @note Asserts that the qtor is not empty.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetLast() noexcept {
        ALT_ASSERT(!IsEmpty());

        return GetBack();
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetLast() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return GetBack();
    }

    ALT_NODISCARD ALT_FORCE_INLINE T& GetBack() noexcept {
        ALT_ASSERT(!IsEmpty());

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        return size <= sizeBottomCapacity ? pQueueBegin[size - 1] : pMemoryBegin[size - sizeBottomCapacity - 1];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetBack() const noexcept {
        ALT_ASSERT(!IsEmpty());

        const T* pQueueBegin = GetQueueBegin();
        const T* pMemoryBegin = GetMemoryBegin();
        const T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        return size <= sizeBottomCapacity ? pQueueBegin[size - 1] : pMemoryBegin[size - sizeBottomCapacity - 1];
    }

    /**
     * @brief Returns a mutable iterator for the queue.
     * @return An `QueueIterator`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE QueueIterator<T> GetIterator() noexcept {

        return QueueIterator<T>{ *this };
    }

    /**
     * @brief Returns a constant iterator for the queue.
     * @return An `QueueIteratorConst`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE QueueIteratorConst<T> GetIterator() const noexcept {

        return QueueIteratorConst<T>{ *this };
    }

    /**
     * Clonable Trait
     * 
     * @brief A deep copy current StaticQueue to another.
     * 
     * @details Return false when copy fails - the Queue contains
     * valid intermediate state of copying.
     * 
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q0 = Alt::StaticQueue<usize, 4>::CreatePush(4);
     * 
     * Alt::StaticQueue<usize, 2> q1;
     * q1.CloneFrom(q0) iferr_return;
     * 
     * assert(q1[0] == q0[0]);
     */
    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || (std::is_assignable_v<T&, const U&> && std::is_constructible_v<T, const U&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(StaticQueue<U, SIZE_>&& queue_) noexcept {

        if (static_cast<const void*>(this) == &queue_) {
            return true;
        }

        StaticQueue<U, SIZE_>& queue = queue_;
        return CloneFrom(queue);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || (std::is_assignable_v<T&, const U&> && std::is_constructible_v<T, const U&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const StaticQueue<U, SIZE_>& queue) noexcept {

        if (static_cast<const void*>(this) == &queue) {
            return true;
        }

        const usize size0 = GetSize();
        const usize size1 = queue.GetSize();

        T* pQueueBegin0 = GetQueueBegin();
        T* pMemoryBegin0 = GetMemoryBegin();
        T* pMemoryEnd0 = GetMemoryEnd();

        const T* pQueueBegin1 = queue.GetQueueBegin();
        const T* pMemoryBegin1 = queue.GetMemoryBegin();
        const T* pMemoryEnd1 = queue.GetMemoryEnd();

        if (size1 > GetCapacity()) {
            return false;
        }

        GetSizeRef() = 0;

        const usize sizeBottom0Capacity = static_cast<usize>(pMemoryEnd0 - pQueueBegin0);
        const usize sizeBottom0 = Num::Min(sizeBottom0Capacity, size0);
        const usize sizeTop0 = size0 - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(pMemoryEnd1 - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(sizeBottom1Capacity, size1);
        const usize sizeTop1 = size1 - sizeBottom1;

        Memory::CloneFromFirstElementResult<T> resultBottom;
        Memory::CloneFromFirstElementResult<T> resultTop;

        const usize sizeBottomToClone = Num::Min(sizeBottom0, sizeBottom1);
        const usize sizeTopToClone = Num::Min(sizeBottom0 - sizeBottomToClone, sizeTop1);
        const usize sizeBottom0CapacityRemainder = sizeBottom0Capacity - sizeBottom0;
        const usize sizeBottomToConstruct = Num::Min(sizeBottom0CapacityRemainder, sizeBottom1 - sizeBottomToClone);
        const usize sizeTopToConstruct = Num::Min(sizeBottom0CapacityRemainder - sizeBottomToConstruct, sizeTop1 - sizeTopToClone);

        {
            resultBottom = Memory::CloneObjectsFromFirstElementNonOverlap(
                pQueueBegin0,
                pQueueBegin1,
                pQueueBegin1 + sizeBottomToClone
            );

            GetSizeRef() += static_cast<Usize>(resultBottom.pDstObjectWhereCloneFailedOrEnd - pQueueBegin0);
            if (!resultBottom.isOk) {

                Memory::DestroyObjects(resultBottom.pDstObjectWhereCloneFailedOrEnd, pQueueBegin0 + sizeBottom0);
                Memory::DestroyObjects(pMemoryBegin0, pMemoryBegin0 + sizeTop0);

                return false;
            }

            resultTop = Memory::CloneObjectsFromFirstElementNonOverlap(
                resultBottom.pDstObjectWhereCloneFailedOrEnd,
                pMemoryBegin1,
                pMemoryBegin1 + sizeTopToClone
            );

            Memory::DestroyObjects(resultTop.pDstObjectWhereCloneFailedOrEnd, pQueueBegin0 + sizeBottom0);

            GetSizeRef() += static_cast<Usize>(resultTop.pDstObjectWhereCloneFailedOrEnd - resultBottom.pDstObjectWhereCloneFailedOrEnd);
            if (!resultTop.isOk) {

                Memory::DestroyObjects(pMemoryBegin0, pMemoryBegin0 + sizeTop0);

                return false;
            }

            resultBottom = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                resultTop.pDstObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd + sizeBottomToConstruct
            );

            GetSizeRef() += static_cast<Usize>(resultBottom.pDstObjectWhereCloneFailedOrEnd - resultTop.pDstObjectWhereCloneFailedOrEnd);
            if (!resultBottom.isOk) {

                Memory::DestroyObjects(pMemoryBegin0, pMemoryBegin0 + sizeTop0);

                return false;
            }

            resultTop = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                resultBottom.pDstObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd + sizeTopToConstruct
            );

            GetSizeRef() += static_cast<Usize>(resultTop.pDstObjectWhereCloneFailedOrEnd - resultBottom.pDstObjectWhereCloneFailedOrEnd);
            if (!resultTop.isOk) {

                Memory::DestroyObjects(pMemoryBegin0, pMemoryBegin0 + sizeTop0);

                return false;
            }
        }

        const usize sizeBottom1Remainder = sizeBottom1 - sizeBottomToClone - sizeBottomToConstruct;
        const usize sizeTop1Remainder = sizeTop1 - sizeTopToClone - sizeTopToConstruct;

        {
            const usize sizeBottomToClone = Num::Min(sizeTop0, sizeBottom1Remainder);
            resultBottom = Memory::CloneObjectsFromFirstElementNonOverlap(
                pMemoryBegin0,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd + sizeBottomToClone
            );

            GetSizeRef() += static_cast<Usize>(resultBottom.pDstObjectWhereCloneFailedOrEnd - pMemoryBegin0);
            if (!resultBottom.isOk) {

                Memory::DestroyObjects(resultBottom.pDstObjectWhereCloneFailedOrEnd, pMemoryBegin0 + sizeTop0);

                return false;
            }

            const usize sizeTopToClone = Num::Min(sizeTop0 - sizeBottomToClone, sizeTop1Remainder);
            resultTop = Memory::CloneObjectsFromFirstElementNonOverlap(
                resultBottom.pDstObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd + sizeTopToClone
            );

            Memory::DestroyObjects(resultTop.pDstObjectWhereCloneFailedOrEnd, pMemoryBegin0 + sizeTop0);

            GetSizeRef() += static_cast<Usize>(resultTop.pDstObjectWhereCloneFailedOrEnd - resultBottom.pDstObjectWhereCloneFailedOrEnd);
            if (!resultTop.isOk) {

                return false;
            }

            const usize sizeBottomToConstruct = sizeBottom1Remainder - sizeBottomToClone;
            resultBottom = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                resultTop.pDstObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd,
                resultBottom.pSrcObjectWhereCloneFailedOrEnd + sizeBottomToConstruct
            );

            GetSizeRef() += static_cast<Usize>(resultBottom.pDstObjectWhereCloneFailedOrEnd - resultTop.pDstObjectWhereCloneFailedOrEnd);
            if (!resultBottom.isOk) {

                return false;
            }

            const usize sizeTopToConstruct = sizeTop1Remainder - sizeTopToClone;
            resultTop = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                resultBottom.pDstObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd,
                resultTop.pSrcObjectWhereCloneFailedOrEnd + sizeTopToConstruct
            );

            GetSizeRef() += static_cast<Usize>(resultTop.pDstObjectWhereCloneFailedOrEnd - resultBottom.pDstObjectWhereCloneFailedOrEnd);
            if (!resultTop.isOk) {

                return false;
            }
        }

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const InnerU&> || (std::is_assignable_v<T&, const InnerU&> && std::is_constructible_v<T, const InnerU&>))
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(U&& rhs) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin, pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sliceSize = slice.GetSize();

        if (sliceSize > GetCapacity()) {
            return false;
        }

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        const usize sizeBottom = Num::Min(sizeBottomCapacity, size);

        const usize sizeBottomToClone = Num::Min(sliceSize, sizeBottom);
        Memory::CloneFromFirstElementResult<T> result = Memory::CloneObjectsFromFirstElementNonOverlap(
            pQueueBegin, slice.Data(), slice.Data() + sizeBottomToClone
        );

        Memory::DestroyObjects(result.pDstObjectWhereCloneFailedOrEnd, pQueueBegin + sizeBottom);

        GetSizeRef() = 0;
        GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueBegin);
        if (!result.isOk) {
            return false;
        }

        T* pDst = result.pDstObjectWhereCloneFailedOrEnd;
        const usize sizeBottomToConstruct = Num::Min(sliceSize, sizeBottomCapacity);
        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pDst, result.pSrcObjectWhereCloneFailedOrEnd, slice.Data() + sizeBottomToConstruct
        );

        GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pDst);
        if (!result.isOk) {
            return false;
        }

        const usize sizeTop = size - sizeBottom;
        const usize sizeTopToClone = Num::Min(sliceSize - sizeBottomToConstruct, sizeTop);

        result = Memory::CloneObjectsFromFirstElementNonOverlap(
            pMemoryBegin, result.pSrcObjectWhereCloneFailedOrEnd, result.pSrcObjectWhereCloneFailedOrEnd + sizeTopToClone
        );

        Memory::DestroyObjects(result.pDstObjectWhereCloneFailedOrEnd, pMemoryBegin + sizeTop);

        GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pMemoryBegin);
        if (!result.isOk) {
            return false;
        }

        pDst = result.pDstObjectWhereCloneFailedOrEnd;
        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pDst, result.pSrcObjectWhereCloneFailedOrEnd, slice.DataEnd()
        );

        GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pDst);

        return result.isOk;
    }

    ALT_NODISCARD bool CloneFrom(const T* pBegin, const usize size) noexcept {

        const Slice<const T> slice{ pBegin, size };
        return CloneFrom(slice);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const StaticQueue<U, SIZE_>& queue) noexcept {
        ALT_ASSERT(static_cast<const void*>(this) != &queue);

        iferr_scope_on_err {

            Memory::DestroyObject(*this);
        };

        Memory::ConstructDefaultObject<StaticQueue>(this);

        const usize size1 = queue.GetSize();

        T* pQueueBegin0 = GetQueueBegin();
        T* pMemoryBegin0 = GetMemoryBegin();

        const T* pQueueBegin1 = queue.GetQueueBegin();
        const T* pMemoryBegin1 = queue.GetMemoryBegin();
        const T* pMemoryEnd1 = queue.GetMemoryEnd();

        if (size1 > GetCapacity()) {
            iferr_return_return false;
        }

        const usize offset = static_cast<usize>(pQueueBegin1 - pMemoryBegin1);
        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd1 - pQueueBegin1);

        const usize sizeBottom = Num::Min(size1, sizeBottomCapacity);

        pQueueBegin0 = pMemoryBegin0 + offset;

        Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pQueueBegin0, pQueueBegin1, pQueueBegin1 + sizeBottom
        );

        if (!result.isOk) {
            iferr_return_return false;
        }

        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pMemoryBegin0, pMemoryBegin1, pMemoryBegin1 + size1 - sizeBottom
        );

        if (!result.isOk) {
            iferr_return_return false;
        }

        GetOffsetRef() = static_cast<Usize>(offset);
        GetSizeRef() = static_cast<Usize>(size1);

        return true;
    }

    ALT_NODISCARD Optional<StaticQueue> Clone() const noexcept {
        iferr_scope;

        MaybeUninit<StaticQueue> valueToConstruct;
        valueToConstruct.GetPtr()->CloneConstructFrom(*this) iferr_return Nullopt;

        defer {
            valueToConstruct.Destroy();
        };

        return std::move(valueToConstruct).GetVal();
    }

    /**
     * @brief A moves each element if current StaticQueue to another.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q0 = Alt::StaticQueue<usize, 4>::CreatePush(4);
     * 
     * Alt::StaticQueue<usize, 2> q1;
     * q1.MoveFrom(q0) iferr_return;
     * 
     * assert(q1[0] == q0[0]);
     */
    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool MoveFrom(StaticQueue<U, SIZE_>&& queue_) noexcept {

        if (static_cast<const void*>(this) == &queue_) {
            return true;
        }

        StaticQueue<U, SIZE_>& queue = queue_;
        return MoveFrom(queue);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_assignable_v<T&, U&&> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool MoveFrom(StaticQueue<U, SIZE_>& queue) noexcept {

        if (static_cast<const void*>(this) == &queue) {
            return true;
        }

        const usize size0 = GetSize();
        const usize size1 = queue.GetSize();

        T* pQueueBegin0 = GetQueueBegin();
        T* pMemoryBegin0 = GetMemoryBegin();
        T* pMemoryEnd0 = GetMemoryEnd();

        T* pQueueBegin1 = queue.GetQueueBegin();
        T* pMemoryBegin1 = queue.GetMemoryBegin();
        T* pMemoryEnd1 = queue.GetMemoryEnd();

        if (size1 > GetCapacity()) {
            return false;
        }

        const usize sizeBottom0Capacity = static_cast<usize>(pMemoryEnd0 - pQueueBegin0);
        const usize sizeBottom0 = Num::Min(sizeBottom0Capacity, size0);
        const usize sizeTop0 = size0 - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(pMemoryEnd1 - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(sizeBottom1Capacity, size1);
        const usize sizeTop1 = size1 - sizeBottom1;

        Memory::MoveFromFirstElementResult<T> resultBottom;
        Memory::MoveFromFirstElementResult<T> resultTop;

        const usize sizeBottomToMove = Num::Min(sizeBottom0, sizeBottom1);
        const usize sizeTopToMove = Num::Min(sizeBottom0 - sizeBottomToMove, sizeTop1);
        const usize sizeBottom0CapacityRemainder = sizeBottom0Capacity - sizeBottom0;
        const usize sizeBottomToConstruct = Num::Min(sizeBottom0CapacityRemainder, sizeBottom1 - sizeBottomToMove);
        const usize sizeTopToConstruct = Num::Min(sizeBottom0CapacityRemainder - sizeBottomToConstruct, sizeTop1 - sizeTopToMove);

        {
            resultBottom = Memory::MoveObjectsFromFirstElementNonOverlap(
                pQueueBegin0, pQueueBegin1, pQueueBegin1 + sizeBottomToMove
            );

            resultTop = Memory::MoveObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, pMemoryBegin1, pMemoryBegin1 + sizeTopToMove
            );

            Memory::DestroyObjects(resultTop.pDstEnd, pQueueBegin0 + sizeBottom0);

            resultBottom = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
                resultTop.pDstEnd, resultBottom.pSrcEnd, resultBottom.pSrcEnd + sizeBottomToConstruct
            );

            resultTop = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, resultTop.pSrcEnd, resultTop.pSrcEnd + sizeTopToConstruct
            );
        }

        const usize sizeBottom1Remainder = sizeBottom1 - sizeBottomToMove - sizeBottomToConstruct;
        const usize sizeTop1Remainder = sizeTop1 - sizeTopToMove - sizeTopToConstruct;

        {
            const usize sizeBottomToMove = Num::Min(sizeTop0, sizeBottom1Remainder);
            resultBottom = Memory::MoveObjectsFromFirstElementNonOverlap(
                pMemoryBegin0, resultBottom.pSrcEnd, resultBottom.pSrcEnd + sizeBottomToMove
            );

            const usize sizeTopToMove = Num::Min(sizeTop0 - sizeBottomToMove, sizeTop1Remainder);
            resultTop = Memory::MoveObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, resultTop.pSrcEnd, resultTop.pSrcEnd + sizeTopToMove
            );

            Memory::DestroyObjects(resultTop.pDstEnd, pMemoryBegin0 + sizeTop0);

            const usize sizeBottomToConstruct = sizeBottom1Remainder - sizeBottomToMove;
            resultBottom = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
                resultTop.pDstEnd, resultBottom.pSrcEnd, resultBottom.pSrcEnd + sizeBottomToConstruct
            );

            const usize sizeTopToConstruct = sizeTop1Remainder - sizeTopToMove;
            resultTop = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
                resultBottom.pDstEnd, resultTop.pSrcEnd, resultTop.pSrcEnd + sizeTopToConstruct
            );
        }

        GetSizeRef() = static_cast<Usize>(size1);

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&std::is_assignable_v<T&, InnerU&&> && std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool MoveFrom(U&& rhs) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin, pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sliceSize = slice.GetSize();

        if (sliceSize > GetCapacity()) {
            return false;
        }

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        const usize sizeBottom = Num::Min(sizeBottomCapacity, size);

        const usize sizeBottomToMove = Num::Min(sliceSize, sizeBottom);
        Memory::MoveFromFirstElementResult<T> result = Memory::MoveObjectsFromFirstElementNonOverlap(
            pQueueBegin, slice.Data(), slice.Data() + sizeBottomToMove
        );

        Memory::DestroyObjects(result.pDstEnd, pQueueBegin + sizeBottom);

        const usize sizeBottomToConstruct = Num::Min(sliceSize, sizeBottomCapacity);
        result = Memory::MoveConstructObjectsFromFirstElementNonOverlap(
            result.pDstEnd, result.pSrcEnd, slice.Data() + sizeBottomToConstruct
        );

        const usize sizeTop = size - sizeBottom;

        const usize sizeTopToMove = Num::Min(sliceSize - sizeBottomToConstruct, sizeTop);
        result = Memory::MoveObjectsFromFirstElementNonOverlap(
            pMemoryBegin, result.pSrcEnd, result.pSrcEnd + sizeTopToMove
        );

        Memory::DestroyObjects(result.pDstEnd, pMemoryBegin + sizeTop);

        Memory::MoveConstructObjectsFromFirstElementNonOverlap(
            result.pDstEnd, result.pDstEnd, slice.DataEnd()
        );

        GetSizeRef() = static_cast<Usize>(sliceSize);

        return true;
    }

    ALT_NODISCARD bool MoveFrom(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return MoveFrom(slice);
    }

    /**
     * @brief Append copy elements to the current StaticQueue back.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 1, 2, 3);
     * q.AppendBackClone(q.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(q[0] == q[4]);
     * assert(q[1] == q[5]);
     */
    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendBackClone(StaticQueue<U, SIZE_>&& queue_) noexcept {

        StaticQueue<U, SIZE_>& queue = queue_;
        return AppendBackClone(queue);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendBackClone(const StaticQueue<U, SIZE_>& queue) noexcept {

        T* pQueueBegin0 = GetQueueBegin();
        T* pMemoryBegin0 = GetMemoryBegin();
        T* pMemoryEnd0 = GetMemoryEnd();

        const T* pQueueBegin1 = queue.GetQueueBegin();
        const T* pMemoryBegin1 = queue.GetMemoryBegin();
        const T* pMemoryEnd1 = queue.GetMemoryEnd();

        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin0, pMemoryEnd0, pMemoryBegin1, pMemoryEnd1));

        const usize size0 = GetSize();
        const usize size1 = queue.GetSize();

        if (size1 == 0) {
            return true;
        }

        if (size1 > GetCapacity() - size0) {
            return false;
        }

        const usize sizeBottom0Capacity = static_cast<usize>(pMemoryEnd0 - pQueueBegin0);
        const usize sizeBottom0 = Num::Min(size0, sizeBottom0Capacity);

        const usize sizeBottom1Capacity = static_cast<usize>(pMemoryEnd1 - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);

        const usize sizeBottom = Num::Min(sizeBottom1, sizeBottom0);
        T* pQueueBottomBegin = pQueueBegin0 + sizeBottom0;
        T* pQueueTopBegin = pMemoryBegin0 + size0 - sizeBottom0;

        {
            const T* pBegin = pQueueBegin1;
            const T* pEnd = pQueueBegin1 + sizeBottom;

            Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                pQueueBottomBegin, pBegin, pEnd
            );

            GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueBottomBegin);
            pQueueBottomBegin = result.pDstObjectWhereCloneFailedOrEnd;

            if (!result.isOk) {
                return false;
            }

            result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                pQueueTopBegin, pEnd, pQueueBegin1 + sizeBottom1
            );

            GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueTopBegin);
            pQueueTopBegin = result.pDstObjectWhereCloneFailedOrEnd;

            if (!result.isOk) {
                return false;
            }
        }

        const usize sizeTop1 = size1 - sizeBottom1;
        const usize sizeBottomRemainder = Num::Min(sizeTop1, sizeBottom0 - sizeBottom);

        {
            const T* pBegin = pMemoryBegin1;
            const T* pEnd = pMemoryBegin1 + sizeBottomRemainder;

            Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                pQueueBottomBegin, pBegin, pEnd
            );

            GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueBottomBegin);
            pQueueBottomBegin = result.pDstObjectWhereCloneFailedOrEnd;

            if (!result.isOk) {
                return false;
            }

            result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
                pQueueTopBegin, pEnd, pBegin + sizeTop1
            );

            GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueTopBegin);
            pQueueTopBegin = result.pDstObjectWhereCloneFailedOrEnd;

            if (!result.isOk) {
                return false;
            }
        }

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const InnerU&> || std::is_constructible_v<T, const InnerU&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendBackClone(U&& rhs) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin, pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize sizeToAppend = slice.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        if (sizeToAppend > GetCapacity() - size) {
            return false;
        }

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);

        T* pQueueBottomBegin = pQueueBegin + sizeBottom;
        const usize sizeBottomAppend = Num::Min(sizeToAppend, sizeBottomCapacity - sizeBottom);
        Memory::CloneFromFirstElementResult<T> result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pQueueBottomBegin, slice.Data(), slice.Data() + sizeBottomAppend
        );

        GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueBottomBegin);

        if (!result.isOk) {
            return false;
        }

        T* pQueueTopBegin = pMemoryBegin + size - sizeBottom;
        result = Memory::CloneConstructObjectsFromFirstElementNonOverlap(
            pQueueTopBegin, slice.Data() + sizeBottomAppend, slice.DataEnd()
        );

        GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueTopBegin);

        return result.isOk;
    }

    ALT_NODISCARD bool AppendBackClone(const T* pBegin, const usize size) noexcept {

        const Slice<const T> slice{ pBegin, size };
        return AppendBackClone(slice);
    }

    /**
     * @brief Append copy elements to the current StaticQueue front.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 1, 2, 3);
     * q.AppendFrontClone(q.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(q[0] == q[4]);
     * assert(q[1] == q[5]);
     */
    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendFrontClone(StaticQueue<U, SIZE_>&& queue_) noexcept {

        StaticQueue<U, SIZE_>& queue = queue_;
        return AppendFrontClone(queue);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<
            std::is_same_v<U, T> &&
            (TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendFrontClone(const StaticQueue<U, SIZE_>& queue) noexcept {

        T* pQueueBegin0 = GetQueueBegin();
        T* pMemoryBegin0 = GetMemoryBegin();
        T* pMemoryEnd0 = GetMemoryEnd();

        const T* pQueueBegin1 = queue.GetQueueBegin();
        const T* pMemoryBegin1 = queue.GetMemoryBegin();
        const T* pMemoryEnd1 = queue.GetMemoryEnd();

        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin0, pMemoryEnd0, pMemoryBegin1, pMemoryEnd1));

        const usize size0 = GetSize();
        const usize size1 = queue.GetSize();

        if (size1 == 0) {
            return true;
        }

        if (size1 > GetCapacity() - size0) {
            return false;
        }

        const usize sizeTop0Capacity = static_cast<usize>(pQueueBegin0 - pMemoryBegin0);

        const usize sizeBottom1Capacity = static_cast<usize>(pMemoryEnd1 - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);
        const usize sizeTop1 = size1 - sizeBottom1;

        const usize sizeTop = Num::Min(sizeTop0Capacity, sizeTop1);
        T* pQueueBottomBegin = pQueueBegin0 - sizeTop;
        T* pQueueBottomEnd = pMemoryEnd0 - sizeTop1 + sizeTop;

        {
            const T* pEnd = pMemoryBegin1 + sizeTop1;
            const T* pBegin = pEnd - sizeTop;

            Memory::CloneFromLastElementResult<T> result = Memory::CloneConstructObjectsFromLastElementNonOverlap(
                pQueueBottomBegin, pBegin, pEnd
            );

            result.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
            GetSizeRef() += static_cast<Usize>(pEnd - result.pSrcObjectWhereCloneFailedOrBeginMinusOne);

            T* pQueueBeginTopNew = result.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

            if (!result.isOk) {

                GetOffsetRef() = static_cast<Usize>(pQueueBeginTopNew - pMemoryBegin0);
                return false;
            }

            result = Memory::CloneConstructObjectsFromLastElementNonOverlap(
                pQueueBottomEnd, pMemoryBegin1, pBegin
            );

            result.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
            GetSizeRef() += static_cast<Usize>(pBegin - result.pSrcObjectWhereCloneFailedOrBeginMinusOne);

            T* pQueueBeginBottomNew = result.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

            if (!result.isOk) {

                const T* pBegin = pQueueBeginBottomNew == pMemoryEnd0
                    ? pQueueBeginTopNew
                    : pQueueBeginBottomNew;

                GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin0);

                return false;
            }
        }

        const usize sizeTopRemainder = Num::Min(sizeBottom1, sizeTop0Capacity - sizeTop);
        pQueueBottomBegin -= sizeTopRemainder;
        pQueueBottomEnd -= sizeBottom1 - sizeTopRemainder;

        {
            const T* pEnd = pQueueBegin1 + sizeBottom1;
            const T* pBegin = pEnd - sizeTopRemainder;

            Memory::CloneFromLastElementResult<T> result = Memory::CloneConstructObjectsFromLastElementNonOverlap(
                pQueueBottomBegin, pBegin, pEnd
            );

            result.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
            GetSizeRef() += static_cast<Usize>(pEnd - result.pSrcObjectWhereCloneFailedOrBeginMinusOne);

            T* pQueueBeginTopNew = result.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

            if (!result.isOk) {

                GetOffsetRef() = static_cast<Usize>(pQueueBeginTopNew - pMemoryBegin0);
                return false;
            }

            result = Memory::CloneConstructObjectsFromLastElementNonOverlap(
                pQueueBottomEnd, pQueueBegin1, pBegin
            );

            result.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
            GetSizeRef() += static_cast<Usize>(pBegin - result.pSrcObjectWhereCloneFailedOrBeginMinusOne);

            T* pQueueBeginBottomNew = result.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

            if (!result.isOk) {

                T* pBegin = pQueueBeginBottomNew == pMemoryEnd0
                    ? pQueueBeginTopNew
                    : pQueueBeginBottomNew;

                GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin0);

                return false;
            }
        }

        T* pBegin = pQueueBottomEnd == pMemoryEnd0
            ? pQueueBottomBegin
            : pQueueBottomEnd;

        GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin0);

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            std::is_same_v<InnerU, T> && (TypeTraits::is_only_clonable_to_from_v<T, const InnerU&> || std::is_constructible_v<T, const InnerU&>)
        >* = nullptr>
    ALT_NODISCARD bool AppendFrontClone(U&& rhs) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin, pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sizeToAppend = slice.GetSize();

        if (sizeToAppend == 0) {
            return true;
        }

        if (sizeToAppend > GetCapacity() - size) {
            return false;
        }

        const usize sizeTopCapacity = static_cast<usize>(pQueueBegin - pMemoryBegin);
        const usize size = Num::Min(sizeToAppend, sizeTopCapacity);

        T* pQueueBeginTop = pQueueBegin - size;
        const Memory::CloneFromLastElementResult<T> resultTop = Memory::CloneConstructObjectsFromLastElementNonOverlap(
            pQueueBeginTop, slice.DataEnd() - size, slice.DataEnd()
        );

        resultTop.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
        GetSizeRef() += static_cast<Usize>(slice.DataEnd() - resultTop.pSrcObjectWhereCloneFailedOrBeginMinusOne);

        T* pQueueBeginTopNew = resultTop.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

        if (!resultTop.isOk) {

            pQueueBegin = pQueueBeginTopNew;
            return false;
        }

        T* pQueueBeginBottom = pMemoryEnd - sizeToAppend + size;
        const Memory::CloneFromLastElementResult<T> resultBottom = Memory::CloneConstructObjectsFromLastElementNonOverlap(
            pQueueBeginBottom, slice.Data(), slice.DataEnd() - size
        );

        resultBottom.pSrcObjectWhereCloneFailedOrBeginMinusOne += 1;
        GetSizeRef() += static_cast<Usize>(slice.DataEnd() - size - resultBottom.pSrcObjectWhereCloneFailedOrBeginMinusOne);

        T* pQueueBeginBottomNew = resultBottom.pDstObjectWhereCloneFailedOrBeginMinusOne + 1;

        T* pBegin = pQueueBeginBottomNew == pMemoryEnd
            ? pQueueBeginTopNew
            : pQueueBeginBottomNew;

        GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin);

        return resultBottom.isOk;
    }

    ALT_NODISCARD bool AppendFrontClone(const T* pBegin, const usize size) noexcept {

        const Slice<const T> slice{ pBegin, size };
        return AppendFrontClone(slice);
    }

    /**
     * @brief Append move elements to the current StaticQueue back.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<Alt::String, 8> q = Alt::StaticQueue<Alt::String, 8>::CreatePush("0"_salloc, "1"_salloc, "2"_salloc, "3"_salloc);
     * 
     * q.AppendBackMove(q.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(q[0] == "0");
     * assert(q[4] == "0");
     */
    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendBackMove(StaticQueue<U, SIZE_>&& queue_) noexcept {

        StaticQueue<U, SIZE_>& queue = queue_;
        return AppendBackMove(queue);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendBackMove(StaticQueue<U, SIZE_>& queue) noexcept {

        T* pQueueBegin0 = GetQueueBegin();
        T* pMemoryBegin0 = GetMemoryBegin();
        T* pMemoryEnd0 = GetMemoryEnd();

        T* pQueueBegin1 = queue.GetQueueBegin();
        T* pMemoryBegin1 = queue.GetMemoryBegin();
        T* pMemoryEnd1 = queue.GetMemoryEnd();

        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin0, pMemoryEnd0, pMemoryBegin1, pMemoryEnd1));

        const usize size0 = GetSize();
        const usize size1 = queue.GetSize();

        if (size1 == 0) {
            return true;
        }

        if (size1 > GetCapacity() - size0) {
            return false;
        }

        const usize sizeBottom0Capacity = static_cast<usize>(pMemoryEnd0 - pQueueBegin0);
        const usize sizeBottom0 = Num::Min(size0, sizeBottom0Capacity);

        const usize sizeBottom1Capacity = static_cast<usize>(pMemoryEnd1 - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);

        const usize sizeBottom = Num::Min(sizeBottom1, sizeBottom0);
        T* pQueueBottomBegin = pQueueBegin0 + sizeBottom0;
        T* pQueueTopBegin = pMemoryBegin0 + size0 - sizeBottom0;

        {
            T* pBegin = pQueueBegin1;
            T* pEnd = pQueueBegin1 + sizeBottom;

            Memory::MoveFromFirstElementResult<T> result = Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, pBegin, pEnd);
            pQueueBottomBegin = result.pDstEnd;

            result = Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueTopBegin, pEnd, pQueueBegin1 + sizeBottom1);
            pQueueTopBegin = result.pDstEnd;
        }

        const usize sizeTop1 = size1 - sizeBottom1;
        const usize sizeBottomRemainder = Num::Min(sizeTop1, sizeBottom0 - sizeBottom);

        {
            T* pBegin = pMemoryBegin1;
            T* pEnd = pMemoryBegin1 + sizeBottomRemainder;

            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, pBegin, pEnd);
            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueTopBegin, pEnd, pBegin + sizeTop1);
        }

        GetSizeRef() += static_cast<Usize>(size1);

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<InnerU>, U&&> &&
            std::is_same_v<InnerU, T> && std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool AppendBackMove(U&& rhs) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin, pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sizeToAppend = slice.GetSize();

        if (sizeToAppend == 0) {
            return true;
        }

        if (sizeToAppend > GetCapacity() - size) {
            return false;
        }

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);

        T* pQueueBottomBegin = pQueueBegin + sizeBottom;
        const usize sizeBottomAppend = Num::Min(sizeToAppend, sizeBottomCapacity - sizeBottom);
        Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, slice.Data(), slice.Data() + sizeBottomAppend);

        T* pQueueTopBegin = pMemoryBegin + size - sizeBottom;
        Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueTopBegin, slice.Data() + sizeBottomAppend, slice.DataEnd());

        GetSizeRef() += static_cast<Usize>(sizeToAppend);

        return true;
    }

    ALT_NODISCARD bool AppendBackMove(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return AppendBackMove(slice);
    }

    /**
     * @brief Append move elements to the current StaticQueue front.
     * @return An true if move success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<Alt::String, 8> q = Alt::StaticQueue<Alt::String, 8>::CreatePush("0"_salloc, "1"_salloc, "2"_salloc, "3"_salloc);
     * 
     * q.AppendFrontMove(q.Clone().GetValueOrDefault()) iferr_return;
     * 
     * assert(q[0] == "0");
     * assert(q[4] == "0");
     */
    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendFrontMove(StaticQueue<U, SIZE_>&& queue_) noexcept {

        StaticQueue<U, SIZE_>& queue = queue_;
        return AppendFrontMove(queue);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T> && std::is_constructible_v<T, U&&>>* = nullptr>
    ALT_NODISCARD bool AppendFrontMove(StaticQueue<U, SIZE_>& queue) noexcept {

        T* pQueueBegin0 = GetQueueBegin();
        T* pMemoryBegin0 = GetMemoryBegin();
        T* pMemoryEnd0 = GetMemoryEnd();

        T* pQueueBegin1 = queue.GetQueueBegin();
        T* pMemoryBegin1 = queue.GetMemoryBegin();
        T* pMemoryEnd1 = queue.GetMemoryEnd();

        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin0, pMemoryEnd0, pMemoryBegin1, pMemoryEnd1));

        const usize size0 = GetSize();
        const usize size1 = queue.GetSize();

        if (size1 == 0) {
            return true;
        }

        if (size1 > GetCapacity() - size0) {
            return false;
        }

        const usize sizeTop0Capacity = static_cast<usize>(pQueueBegin0 - pMemoryBegin0);

        const usize sizeBottom1Capacity = static_cast<usize>(pMemoryEnd1 - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);
        const usize sizeTop1 = size1 - sizeBottom1;

        const usize sizeTop = Num::Min(sizeTop0Capacity, sizeTop1);
        T* pQueueBottomBegin = pQueueBegin0 - sizeTop;
        T* pQueueBottomEnd = pMemoryEnd0 - sizeTop1 + sizeTop;

        {
            T* pEnd = pMemoryBegin1 + sizeTop1;
            T* pBegin = pEnd - sizeTop;

            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, pBegin, pEnd);
            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomEnd, pMemoryBegin1, pBegin);
        }

        const usize sizeTopRemainder = Num::Min(sizeBottom1, sizeTop0Capacity - sizeTop);
        pQueueBottomBegin -= sizeTopRemainder;
        pQueueBottomEnd -= sizeBottom1 - sizeTopRemainder;

        {
            T* pEnd = pQueueBegin1 + sizeBottom1;
            T* pBegin = pEnd - sizeTopRemainder;

            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomBegin, pBegin, pEnd);
            Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBottomEnd, pQueueBegin1, pBegin);
        }

        T* pBegin = pQueueBottomEnd == pMemoryEnd0
            ? pQueueBottomBegin
            : pQueueBottomEnd;

        GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin0);
        GetSizeRef() += static_cast<Usize>(size1);

        return true;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<InnerU>, U&&> &&
            std::is_same_v<InnerU, T> && std::is_constructible_v<T, InnerU&&>
        >* = nullptr>
    ALT_NODISCARD bool AppendFrontMove(U&& rhs) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        Slice<T> slice = rhs;
        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin, pMemoryEnd, slice.Data(), slice.DataEnd()));

        const usize size = GetSize();
        const usize sizeToAppend = slice.GetSize();

        if (sizeToAppend == 0) {
            return true;
        }

        if (sizeToAppend > GetCapacity() - size) {
            return false;
        }

        const usize sizeTopCapacity = static_cast<usize>(pQueueBegin - pMemoryBegin);

        const usize size = Num::Min(sizeToAppend, sizeTopCapacity);

        T* pQueueBeginTop = pQueueBegin - size;
        Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBeginTop, slice.DataEnd() - size, slice.DataEnd());

        T* pQueueBeginBottom = pMemoryEnd - sizeToAppend + size;
        Memory::MoveConstructObjectsFromFirstElementNonOverlap(pQueueBeginBottom, slice.Data(), slice.DataEnd() - size);

        T* pBegin = sizeToAppend <= sizeTopCapacity
            ? pQueueBeginTop
            : pQueueBeginBottom;

        GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin);
        GetSizeRef() += static_cast<Usize>(sizeToAppend);

        return true;
    }

    ALT_NODISCARD bool AppendFrontMove(T* pBegin, const usize size) noexcept {

        Slice<T> slice{ pBegin, size };
        return AppendFrontMove(slice);
    }

    /**
     * @brief Appends an element to the back of the StaticQueue.
     * @param val The value to push.
     * @return True if the element was successfully added, false if capacity exceeded.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q;
     * q.PushBack(0) iferr_return;
     * 
     * assert(q[0] == 0);
     */
    template <typename U = T,
        std::enable_if_t<is_pushable_v<U&&>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool PushBack(U&& val) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();

        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin, pMemoryEnd, &val, &val + 1));

        if (IsFull()) {
            return false;
        }

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        T* pEnd = size < sizeBottomCapacity ? pQueueBegin + size : pMemoryBegin + size - sizeBottomCapacity;

        T* pResult = Memory::CloneConstructOrMoveConstructObject(pEnd, std::forward<U>(val));

        const bool isOk = pResult != pEnd;
        GetSizeRef() += isOk ? 1 : 0;

        return isOk;
    }

    /**
     * @brief Variadic append an elements to the back of the queue.
     * @param ...vals The values to push.
     * @return True if the element was successfully added, false if capacity exceeded.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q;
     * q.PushBack(0, 1, 2, 3) iferr_return;
     * 
     * assert(q[0] == 0);
     * assert(q.GetSize() == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    ALT_NODISCARD bool PushBack(ArgsT&&... vals) noexcept {
        iferr_scope;

        const usize size = GetSize();

        constexpr usize sizeToPush = sizeof...(ArgsT);
        if (sizeToPush > GetCapacity() - size) {
            return false;
        }

        PushBackVariadicImpl(*this, std::forward<ArgsT>(vals)...) iferr_return false;

        return true;
    }

    /**
     * @brief Appends an element to the front of the StaticQueue.
     * @param val The value to push.
     * @return True if the element was successfully added, false if capacity exceeded.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q;
     * 
     * q.PushBack(1) iferr_return;
     * q.PushFront(0) iferr_return;
     * 
     * assert(q[0] == 0);
     */
    template <typename U = T,
        std::enable_if_t<is_pushable_v<U&&>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool PushFront(U&& val) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        ALT_ASSERT(!Memory::IsOverlap(pMemoryBegin, pMemoryEnd, &val, &val + 1));

        if (IsFull()) {
            return false;
        }

        const bool isNeedRecycle = pQueueBegin == pMemoryBegin;
        T* pBeginNew = isNeedRecycle ? pMemoryEnd : pQueueBegin;
        pBeginNew -= 1;

        T* pResult = Memory::CloneConstructOrMoveConstructObject(pBeginNew, std::forward<U>(val));

        const bool isOk = pResult != pBeginNew;

        T* pBegin = isOk ? pBeginNew : pQueueBegin;
        GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin);

        GetSizeRef() += isOk ? 1 : 0;

        return isOk;
    }

    /**
     * @brief Variadic append an elements to the front of the queue.
     * @param ...vals The values to push.
     * @return True if the element was successfully added, false if capacity exceeded.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q;
     * q.PushFront(0, 1, 2, 3) iferr_return;
     * 
     * assert(q[0] == 0);
     * assert(q.GetSize() == 4);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    ALT_NODISCARD bool PushFront(ArgsT&&... vals) noexcept {
        iferr_scope;

        const usize size = GetSize();

        constexpr usize sizeToPush = sizeof...(ArgsT);
        if (sizeToPush > GetCapacity() - size) {
            return false;
        }

        PushFrontVariadicImpl(*this, std::forward<ArgsT>(vals)...) iferr_return false;

        return true;
    }

    /**
     * @brief Removes and returns the last (back) element of the StaticQueue.
     * @return The element that was removed.
     * @note This method requires the StaticQueue to not be empty. Calling on an empty StaticQueue triggers an assertion.
     * @example
     * 
     * Alt::StaticQueue<usize, 4> q = Alt::StaticQueue<usize, 4>::CreatePush(0, 2, 4);
     * usize val = q.PopBack();
     * assert(val == 4);
     */
    ALT_FORCE_INLINE T PopBack() noexcept {
        ALT_ASSERT(!IsEmpty());

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();
        GetSizeRef() -= 1;

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        T* pEnd = size <= sizeBottomCapacity ? pQueueBegin + size : pMemoryBegin + size - sizeBottomCapacity;
        pEnd -= 1;

        return Memory::MoveDestructibleObject(*pEnd);
    }

    /**
     * @brief Removes and returns the first (front) element of the StaticQueue.
     * @return The element that was removed.
     * @note This method requires the StaticQueue to not be empty. Calling on an empty StaticQueue triggers an assertion.
     * @example
     * 
     * Alt::StaticQueue<usize, 4> q = Alt::StaticQueue<usize, 4>::CreatePush(0, 2, 4);
     * usize val = q.PopFront();
     * assert(val == 0);
     */
    ALT_FORCE_INLINE T PopFront() noexcept {
        ALT_ASSERT(!IsEmpty());

        T result = Memory::MoveDestructibleObject(*GetQueueBegin());

        GetSizeRef() -= 1;

        Usize& offset = GetOffsetRef();
        offset += 1;
        offset = offset == GetCapacity() ? 0 : offset;

        return result;
    }

    /**
     * @brief Fill or destroy elements in the queue back.
     * @tparam U The type of the value used for padding. Must be constructible to T.
     * @param size The new size of the queue.
     * @return True if the resize was successful, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q;
     * q.ResizeBack(3) iferr_return;
     * 
     * assert(q.GetSize() == 3);
     * assert(q.GetCapacity() == 4);
     * 
     * q.ResizeBackAddition(1) iferr_return;
     * 
     * assert(q.GetSize() == 4);
     */
    template <typename U = T, typename ...ArgsT,
        std::enable_if_t<
            is_populated_v<ArgsT...> &&
            std::is_default_constructible_v<U> &&
            std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>
        >* = nullptr>
    ALT_NODISCARD bool ResizeBackAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return ResizeBack(size);
    }

    /**
     * @brief Resizes the queue to a new size from back.
     * @tparam U The type of the value. Must be default constructible.
     * @param size The new size.
     * @return True if successful, false if size exceeds capacity.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q;
     * q.ResizeBack(3) iferr_return;
     * 
     * assert(q.GetSize() == 3);
     * assert(q.GetCapacity() == 4);
     * 
     * assert(q[0] == 0);
     */
    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD bool ResizeBack(const usize size) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize sizeQueue = GetSize();

        if (size < sizeQueue) {

            const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);

            const usize sizeBottom0 = Num::Min(sizeQueue, sizeBottomCapacity);
            const usize sizeBottom1 = Num::Min(size, sizeBottomCapacity);

            T* pQueueBottomBegin = pQueueBegin + sizeBottom1;
            T* pQueueBottomEnd = pQueueBegin + sizeBottom0;
            Memory::DestroyObjects(pQueueBottomBegin, pQueueBottomEnd);

            T* pQueueTopBegin = pMemoryBegin + size - sizeBottom1;
            T* pQueueTopEnd = pMemoryBegin + sizeQueue - sizeBottom0;
            Memory::DestroyObjects(pQueueTopBegin, pQueueTopEnd);
        }
        else if (size > sizeQueue) {

            if (size > GetCapacity() - sizeQueue) {
                return false;
            }

            const usize sizeOld = sizeQueue;
            const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);

            const usize sizeBottom0 = Num::Min(sizeOld, sizeBottomCapacity);
            const usize sizeBottom1 = Num::Min(size, sizeBottomCapacity);

            T* pQueueBottomBegin = pQueueBegin + sizeBottom0;
            T* pQueueBottomEnd = pQueueBegin + sizeBottom1;
            Memory::FillConstructDefaultObjects(pQueueBottomBegin, pQueueBottomEnd);

            T* pQueueTopBegin = pMemoryBegin + sizeOld - sizeBottom0;
            T* pQueueTopEnd = pMemoryBegin + size - sizeBottom1;
            Memory::FillConstructDefaultObjects(pQueueTopBegin, pQueueTopEnd);
        }

        GetSizeRef() = static_cast<Usize>(size);

        return true;
    }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD bool ResizeBack(const usize size, const U& val) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize sizeQueue = GetSize();

        if (size < sizeQueue) {

            const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);

            const usize sizeBottom0 = Num::Min(sizeQueue, sizeBottomCapacity);
            const usize sizeBottom1 = Num::Min(size, sizeBottomCapacity);

            T* pQueueBottomBegin = pQueueBegin + sizeBottom1;
            T* pQueueBottomEnd = pQueueBegin + sizeBottom0;
            Memory::DestroyObjects(pQueueBottomBegin, pQueueBottomEnd);

            T* pQueueTopBegin = pMemoryBegin + size - sizeBottom1;
            T* pQueueTopEnd = pMemoryBegin + sizeQueue - sizeBottom0;
            Memory::DestroyObjects(pQueueTopBegin, pQueueTopEnd);

            GetSizeRef() = static_cast<Usize>(size);
        }
        else if (size > sizeQueue) {

            if (size > GetCapacity() - sizeQueue) {
                return false;
            }

            const usize sizeOld = sizeQueue;
            const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);

            const usize sizeBottom0 = Num::Min(sizeOld, sizeBottomCapacity);
            const usize sizeBottom1 = Num::Min(size, sizeBottomCapacity);

            T* pQueueBottomBegin = pQueueBegin + sizeBottom0;
            T* pQueueBottomEnd = pQueueBegin + sizeBottom1;
            Memory::CloneFromFirstElementResult<T> result = Memory::FillCloneConstructObjectsFromFirstElement(
                pQueueBottomBegin, pQueueBottomEnd, val
            );

            GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueBottomBegin);

            if (!result.isOk) {
                return false;
            }

            T* pQueueTopBegin = pMemoryBegin + sizeOld - sizeBottom0;
            T* pQueueTopEnd = pMemoryBegin + size - sizeBottom1;
            result = Memory::FillCloneConstructObjectsFromFirstElement(pQueueTopBegin, pQueueTopEnd, val);

            GetSizeRef() += static_cast<Usize>(result.pDstObjectWhereCloneFailedOrEnd - pQueueTopBegin);

            return result.isOk;
        }

        return true;
    }

    /**
     * @brief Fill or destroy elements in the queue front.
     * @tparam U The type of the value used for padding. Must be constructible to T.
     * @param size The new size of the queue.
     * @return True if the resize was successful, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q;
     * q.ResizeBack(3) iferr_return;
     * 
     * assert(q.GetSize() == 3);
     * assert(q.GetCapacity() == 4);
     * 
     * q.ResizeFrontAddition(1) iferr_return;
     * 
     * assert(q.GetSize() == 4);
     */
    template <typename U = T, typename ...ArgsT,
        std::enable_if_t<
            is_populated_v<ArgsT...> &&
            std::is_default_constructible_v<U> &&
            std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>
        >* = nullptr>
    ALT_NODISCARD bool ResizeFrontAddition(ArgsT&&... sizeAdditions) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSize(), sizeAdditions...) iferr_return false;

        return ResizeFront(size);
    }

    /**
     * @brief Resizes the queue to a new size from front.
     * @tparam U The type of the value. Must be default constructible.
     * @param size The new size.
     * @return True if successful, false if size exceeds capacity.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 4> q;
     * q.ResizeFront(3) iferr_return;
     * 
     * assert(q.GetSize() == 3);
     * assert(q.GetCapacity() == 4);
     * 
     * assert(q[0] == 0);
     */
    template <typename U = T,
        std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
    ALT_NODISCARD bool ResizeFront(const usize size) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize sizeQueue = GetSize();

        if (size < sizeQueue) {

            const usize sizeToDestroy = sizeQueue - size;

            const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
            const usize sizeBottom = Num::Min(sizeToDestroy, sizeBottomCapacity);

            T* pQueueBeginBottom = pQueueBegin + sizeBottom;
            Memory::DestroyObjects(pQueueBegin, pQueueBeginBottom);

            T* pQueueBeginTop = pMemoryBegin + sizeToDestroy - sizeBottom;
            Memory::DestroyObjects(pMemoryBegin, pQueueBeginTop);

            T* pBegin = sizeToDestroy < sizeBottomCapacity
                ? pQueueBeginBottom
                : pQueueBeginTop;

            GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin);
        }
        else if (size > sizeQueue) {

            if (size > GetCapacity() - sizeQueue) {
                return false;
            }

            const usize sizeToConstruct = size - sizeQueue;

            const usize sizeTopCapacity = static_cast<usize>(pQueueBegin - pMemoryBegin);
            const usize sizeTop = Num::Min(sizeToConstruct, sizeTopCapacity);

            T* pQueueBeginTop = pQueueBegin - sizeTop;
            Memory::FillConstructDefaultObjects(pQueueBeginTop, pQueueBegin);

            T* pQueueBeginBottom = pMemoryEnd - sizeToConstruct + sizeTop;
            Memory::FillConstructDefaultObjects(pQueueBeginBottom, pMemoryEnd);

            T* pBegin = pQueueBeginBottom == pMemoryEnd
                ? pQueueBeginTop
                : pQueueBeginBottom;

            GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin);
        }

        GetSizeRef() = static_cast<Usize>(size);

        return true;
    }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<T, const U&> || std::is_constructible_v<T, const U&>>* = nullptr>
    ALT_NODISCARD bool ResizeFront(const usize size, const U& val) noexcept {

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize sizeQueue = GetSize();

        if (size < sizeQueue) {

            const usize sizeToDestroy = sizeQueue - size;

            const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
            const usize sizeBottom = Num::Min(sizeToDestroy, sizeBottomCapacity);

            T* pQueueBeginBottom = pQueueBegin + sizeBottom;
            Memory::DestroyObjects(pQueueBegin, pQueueBeginBottom);

            T* pQueueBeginTop = pMemoryBegin + sizeToDestroy - sizeBottom;
            Memory::DestroyObjects(pMemoryBegin, pQueueBeginTop);

            T* pBegin = sizeToDestroy < sizeBottomCapacity
                ? pQueueBeginBottom
                : pQueueBeginTop;

            GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin);
            GetSizeRef() = static_cast<Usize>(size);
        }
        else if (size > sizeQueue) {

            if (size > GetCapacity() - sizeQueue) {
                return false;
            }

            const usize sizeToConstruct = size - sizeQueue;

            const usize sizeTopCapacity = static_cast<usize>(pQueueBegin - pMemoryBegin);
            const usize sizeTop = Num::Min(sizeToConstruct, sizeTopCapacity);

            T* pQueueBeginTop = pQueueBegin - sizeTop;
            Memory::CloneFromLastElementResult<T> result = Memory::FillCloneConstructObjectsFromLastElement(
                pQueueBeginTop, pQueueBegin, val
            );

            result.pDstObjectWhereCloneFailedOrBeginMinusOne += 1;
            pQueueBeginTop = result.pDstObjectWhereCloneFailedOrBeginMinusOne;
            GetSizeRef() += static_cast<Usize>(pQueueBegin - pQueueBeginTop);

            if (!result.isOk) {

                GetOffsetRef() = static_cast<Usize>(pQueueBeginTop - pMemoryBegin);
                return false;
            }

            T* pQueueBeginBottom = pMemoryEnd - sizeToConstruct + sizeTop;
            result = Memory::FillCloneConstructObjectsFromLastElement(pQueueBeginBottom, pMemoryEnd, val);

            result.pDstObjectWhereCloneFailedOrBeginMinusOne += 1;
            pQueueBeginBottom = result.pDstObjectWhereCloneFailedOrBeginMinusOne;
            GetSizeRef() += static_cast<Usize>(pMemoryEnd - pQueueBeginBottom);

            T* pBegin = pQueueBeginBottom == pMemoryEnd
                ? pQueueBeginTop
                : pQueueBeginBottom;

            GetOffsetRef() = static_cast<Usize>(pBegin - pMemoryBegin);

            return result.isOk;
        }

        return true;
    }

    /**
     * @brief Removes the element at index `i` by swapping it with the last (back) element.
     * @param i The index of the element to remove.
     * @return The element that was removed.
     * @note This is an $O(1)$ operation but does not preserve the order of elements.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 2, 4, 8, 16);
     * const usize first = q.RemoveSwapBack(0);
     * 
     * assert(first == 0);
     * assert(q[0] == 16);
     */
    ALT_FORCE_INLINE T RemoveSwapLast(const usize i) noexcept {

        return RemoveSwapBack(i);
    }

    ALT_FORCE_INLINE T RemoveSwapBack(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();
        GetSizeRef() -= 1;

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);

        T* pCurrent = i < sizeBottomCapacity ? pQueueBegin + i : pMemoryBegin + i - sizeBottomCapacity;
        T* pEnd = size <= sizeBottomCapacity ? pQueueBegin + size : pMemoryBegin + size - sizeBottomCapacity;
        pEnd -= 1;

        if (pCurrent != pEnd) {

            Swap(*pCurrent, *pEnd);
        }

        return Memory::MoveDestructibleObject(*pEnd);
    }

    /**
     * @brief Removes the element at index `i` by swapping it with the first (front) element.
     * @param i The index of the element to remove.
     * @return The element that was removed.
     * @note This is an $O(1)$ operation but does not preserve the order of elements.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 2, 4, 8, 16);
     * const usize first = q.RemoveSwapFront(4);
     * 
     * assert(first == 16);
     * assert(q[0] == 0);
     */
    ALT_FORCE_INLINE T RemoveSwapFirst(const usize i) noexcept {

        return RemoveSwapFront(i);
    }

    ALT_FORCE_INLINE T RemoveSwapFront(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        GetSizeRef() -= 1;

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);

        T* pCurrent = i < sizeBottomCapacity ? pQueueBegin + i : pMemoryBegin + i - sizeBottomCapacity;
        T* pBegin = pQueueBegin;

        if (pCurrent != pBegin) {

            Swap(*pCurrent, *pBegin);
        }

        T result = Memory::MoveDestructibleObject(*pBegin);

        Usize& offset = GetOffsetRef();
        offset += 1;
        offset = offset == GetCapacity() ? 0 : offset;

        return result;
    }

    /**
     * @brief Removes the element at index `i` and shifts subsequent elements.
     * @param i The index of the element to remove.
     * @return The element that was removed.
     * @note This is an $O(n)$ operation.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticQueue<usize, 8> q = Alt::StaticQueue<usize, 8>::CreatePush(0, 2, 4, 8, 16);
     * const usize first = q.Remove(0);
     * 
     * assert(first == 0);
     * assert(q[0] == 2);
     */
    T Remove(const usize i) noexcept {
        ALT_ASSERT(i < GetSize());

        T* pQueueBegin = GetQueueBegin();
        T* pMemoryBegin = GetMemoryBegin();
        T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();
        GetSizeRef() -= 1;

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);

        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);
        const usize sizeTop = size - sizeBottom;

        const bool isBottom = i < sizeBottomCapacity;
        if (isBottom) {

            T result = Algorithms::RemoveShiftRight(pQueueBegin, pQueueBegin + sizeBottom, pQueueBegin + i);

            Usize& offset = GetOffsetRef();
            offset += 1;
            offset = offset == GetCapacity() ? 0 : offset;

            return result;
        }

        return Algorithms::Remove(pMemoryBegin, pMemoryBegin + sizeTop, pMemoryBegin + i - sizeBottom);
    }

    /**
     * @brief Operators for comparison with Queue.
     * @example
     *
     * Alt::Queue<usize, 1> q0{ 0, 1 };
     * Alt::Queue<usize, 1> q1 = q0.Clone() iferr_return;
     * 
     * assert(q0 == q1);
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator==(U&& rhs_) const noexcept {

        const T* pQueueBegin = GetQueueBegin();
        const T* pMemoryBegin = GetMemoryBegin();
        const T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();

        const Slice<const T> rhs = rhs_;

        if (size != rhs.GetSize()) {
            return false;
        }

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);

        const bool isEQ = Algorithms::CompareEQ(pQueueBegin, pQueueBegin + sizeBottom, rhs.Data(), rhs.Data() + sizeBottom);
        if (!isEQ) {
            return false;
        }

        return Algorithms::CompareEQ(pMemoryBegin, pMemoryBegin + size - sizeBottom, rhs.Data() + sizeBottom, rhs.DataEnd());
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator==(const StaticQueue<U, SIZE_>& queue) const noexcept {

        const T* pQueueBegin0 = GetQueueBegin();
        const T* pMemoryBegin0 = GetMemoryBegin();
        const T* pMemoryEnd0 = GetMemoryEnd();

        const T* pQueueBegin1 = queue.GetQueueBegin();
        const T* pMemoryBegin1 = queue.GetMemoryBegin();
        const T* pMemoryEnd1 = queue.GetMemoryEnd();

        const usize size0 = GetSize();
        const usize size1 = queue.GetSize();

        if (size0 != size1) {
            return false;
        }

        const usize sizeBottom0Capacity = static_cast<usize>(pMemoryEnd0 - pQueueBegin0);
        const usize sizeBottom0 = Num::Min(size0, sizeBottom0Capacity);
        const usize sizeTop0 = size0 - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(pMemoryEnd1 - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);
        const usize sizeTop1 = size1 - sizeBottom1;

        const usize sizeBottom = Num::Min(sizeBottom0, sizeBottom1);

        const T* pQueueBottom0Begin = pQueueBegin0 + sizeBottom;
        const T* pQueueBottom1Begin = pQueueBegin1 + sizeBottom;

        bool isEQ = Algorithms::CompareEQ(pQueueBegin0, pQueueBottom0Begin, pQueueBegin1, pQueueBottom1Begin);
        if (!isEQ) {
            return false;
        }

        usize sizeBottomRemainder = Num::Min(sizeBottom0 - sizeBottom, sizeTop1);
        const T* pQueueTop1Begin = pMemoryBegin1 + sizeBottomRemainder;

        isEQ = Algorithms::CompareEQ(pQueueBottom0Begin, pQueueBottom0Begin + sizeBottomRemainder, pMemoryBegin1, pQueueTop1Begin);
        if (!isEQ) {
            return false;
        }

        sizeBottomRemainder = Num::Min(sizeBottom1 - sizeBottom, sizeTop0);
        const T* pQueueTop0Begin = pMemoryBegin0 + sizeBottomRemainder;

        isEQ = Algorithms::CompareEQ(pMemoryBegin0, pQueueTop0Begin, pQueueBottom1Begin, pQueueBottom1Begin + sizeBottomRemainder);
        if (!isEQ) {
            return false;
        }

        return Algorithms::CompareEQ(pQueueTop0Begin, pMemoryBegin0 + sizeTop0, pQueueTop1Begin, pMemoryBegin1 + sizeTop1);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator!=(U&& rhs) const noexcept {

        return !(*this == rhs);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator!=(const StaticQueue<U, SIZE_>& queue) const noexcept {

        return !(*this == queue);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator<(U&& rhs_) const noexcept {

        const T* pQueueBegin = GetQueueBegin();
        const T* pMemoryBegin = GetMemoryBegin();
        const T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();

        const Slice<const T> rhs = rhs_;

        if (size != rhs.GetSize()) {
            return size < rhs.GetSize();
        }

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);

        const ssize cmp = Algorithms::Compare(pQueueBegin, pQueueBegin + sizeBottom, rhs.Data(), rhs.Data() + sizeBottom);

        switch (cmp) {
        case -1: {
            return true;
        }
        case 0: {
            break;
        }
        case 1: {
            return false;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        return Algorithms::CompareLT(pMemoryBegin, pMemoryBegin + size - sizeBottom, rhs.Data() + sizeBottom, rhs.DataEnd());
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator<(const StaticQueue<U, SIZE_>& queue) const noexcept {

        const T* pQueueBegin0 = GetQueueBegin();
        const T* pMemoryBegin0 = GetMemoryBegin();
        const T* pMemoryEnd0 = GetMemoryEnd();

        const T* pQueueBegin1 = queue.GetQueueBegin();
        const T* pMemoryBegin1 = queue.GetMemoryBegin();
        const T* pMemoryEnd1 = queue.GetMemoryEnd();

        const usize size0 = GetSize();
        const usize size1 = queue.GetSize();

        if (size0 != size1) {
            return size0 < size1;
        }

        const usize sizeBottom0Capacity = static_cast<usize>(pMemoryEnd0 - pQueueBegin0);
        const usize sizeBottom0 = Num::Min(size0, sizeBottom0Capacity);
        const usize sizeTop0 = size0 - sizeBottom0;

        const usize sizeBottom1Capacity = static_cast<usize>(pMemoryEnd1 - pQueueBegin1);
        const usize sizeBottom1 = Num::Min(size1, sizeBottom1Capacity);
        const usize sizeTop1 = size1 - sizeBottom1;

        const usize sizeBottom = Num::Min(sizeBottom0, sizeBottom1);

        const T* pQueueBottom0Begin = pQueueBegin0 + sizeBottom;
        const T* pQueueBottom1Begin = pQueueBegin1 + sizeBottom;
        ssize cmp = Algorithms::Compare(pQueueBegin0, pQueueBottom0Begin, pQueueBegin1, pQueueBottom1Begin);

        switch (cmp) {
        case -1: {
            return true;
        }
        case 0: {
            break;
        }
        case 1: {
            return false;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        usize sizeBottomRemainder = Num::Min(sizeBottom0 - sizeBottom, sizeTop1);
        const T* pQueueTop1Begin = pMemoryBegin1 + sizeBottomRemainder;

        cmp = Algorithms::Compare(pQueueBottom0Begin, pQueueBottom0Begin + sizeBottomRemainder, pMemoryBegin1, pQueueTop1Begin);

        switch (cmp) {
        case -1: {
            return true;
        }
        case 0: {
            break;
        }
        case 1: {
            return false;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        sizeBottomRemainder = Num::Min(sizeBottom1 - sizeBottom, sizeTop0);
        const T* pQueueTop0Begin = pMemoryBegin0 + sizeBottomRemainder;

        cmp = Algorithms::Compare(pMemoryBegin0, pQueueTop0Begin, pQueueBottom1Begin, pQueueBottom1Begin + sizeBottomRemainder);

        switch (cmp) {
        case -1: {
            return true;
        }
        case 0: {
            break;
        }
        case 1: {
            return false;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        return Algorithms::CompareLT(pQueueTop0Begin, pMemoryBegin0 + sizeTop0, pQueueTop1Begin, pMemoryBegin1 + sizeTop1);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator>(U&& rhs_) const noexcept {

        const T* pQueueBegin = GetQueueBegin();
        const T* pMemoryBegin = GetMemoryBegin();
        const T* pMemoryEnd = GetMemoryEnd();

        const usize size = GetSize();

        const Slice<const T> rhs = rhs_;

        if (size != rhs.GetSize()) {
            return size > rhs.GetSize();
        }

        const usize sizeBottomCapacity = static_cast<usize>(pMemoryEnd - pQueueBegin);
        const usize sizeBottom = Num::Min(size, sizeBottomCapacity);

        const ssize cmp = Algorithms::Compare(pQueueBegin, pQueueBegin + sizeBottom, rhs.Data(), rhs.Data() + sizeBottom);

        switch (cmp) {
        case -1: {
            return false;
        }
        case 0: {
            break;
        }
        case 1: {
            return true;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        return Algorithms::CompareGT(pMemoryBegin, pMemoryBegin + size - sizeBottom, rhs.Data() + sizeBottom, rhs.DataEnd());
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator>(const StaticQueue<U, SIZE_>& queue) const noexcept {

        return queue < *this;
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator<=(U&& rhs) const noexcept {

        return !(*this > rhs);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator<=(const StaticQueue<U, SIZE_>& queue) const noexcept {

        return !(*this > queue);
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && std::is_same_v<InnerU, T>>* = nullptr>
    ALT_NODISCARD bool operator>=(U&& rhs) const noexcept {

        return !(*this < rhs);
    }

    template <typename U = T, usize SIZE_ = SIZE,
        std::enable_if_t<std::is_same_v<U, T>>* = nullptr>
    ALT_NODISCARD bool operator>=(const StaticQueue<U, SIZE_>& queue) const noexcept {

        return !(*this < queue);
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

    ALT_FORCE_INLINE T* GetQueueBegin() noexcept {

        return m_Storage.GetMemoryBegin() + m_Storage.GetOffset();
    }

    ALT_FORCE_INLINE const T* GetQueueBegin() const noexcept {

        return m_Storage.GetMemoryBegin() + m_Storage.GetOffset();
    }

    ALT_FORCE_INLINE Usize& GetOffsetRef() noexcept {

        return m_Storage.GetOffsetRef();
    }

    ALT_FORCE_INLINE const Usize& GetOffsetRef() const noexcept {

        return m_Storage.GetOffsetRef();
    }

    ALT_FORCE_INLINE Usize& GetSizeRef() noexcept {

        return m_Storage.GetSizeRef();
    }

    ALT_FORCE_INLINE const Usize& GetSizeRef() const noexcept {

        return m_Storage.GetSizeRef();
    }

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE static bool PushBackVariadicImpl(StaticQueue& self, HeadT&& head, TailT&&... tail) noexcept {
        iferr_scope;

        self.PushBack(std::forward<HeadT>(head)) iferr_return false;
        return PushBackVariadicImpl(self, std::forward<TailT>(tail)...);
    }

    ALT_FORCE_INLINE static bool PushBackVariadicImpl(StaticQueue&) noexcept { return true; }

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE static bool PushFrontVariadicImpl(StaticQueue& self, HeadT&& head, TailT&&... tail) noexcept {
        iferr_scope;

        self.PushFront(std::forward<HeadT>(head)) iferr_return false;
        return PushFrontVariadicImpl(self, std::forward<TailT>(tail)...);
    }

    ALT_FORCE_INLINE static bool PushFrontVariadicImpl(StaticQueue&) noexcept { return true; }

private:

    template <typename, usize>
    friend class StaticQueue;

    template <typename T>
    friend class QueueIterator;

    template <typename T>
    friend class QueueIteratorConst;

    Storage m_Storage;
};

template <typename T, usize SIZE>
class IteratorCollector<StaticQueue<T, SIZE>> {
public:

    using SelfT = StaticQueue<T, SIZE>;

    template <typename U = T,
        std::enable_if_t<std::is_assignable_v<T&, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait>& iter) noexcept {
        iferr_scope;

        while (Optional<U> val = iter.Next()) {

            self.PushBack(std::move(val.GetValue())) iferr_return false;
        }

        return true;
    }
};

/**
 * @brief Format specialization for StaticQueue<T, SIZE>.
 */
template <typename T, usize SIZE>
class Format<StaticQueue<T, SIZE>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U> >* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, Slice<const char> parameters, const StaticQueue<T, SIZE>& vals
    ) noexcept {
        iferr_scope;
        
        using CleanU = _FormatDetails::clean_t<U>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "noinline"_s) {

            EstimateExactSizeResult result{};
            result.size = 2;

            if (vals.IsEmpty()) {
                return result;
            }

            estimator.PushIndent();

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const U& val : iter) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val) iferr_return Nullopt;

                result.size += estimator.GetIndentEstimatedSize() + estimated.size;
                result.size += 2;
            }

            if (vals.IsPopulated()) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, vals.GetLast()) iferr_return Nullopt;

                result.size += estimator.GetIndentEstimatedSize() + estimated.size;
                result.size += 1;
            }

            estimator.PopIndent();

            result.size += estimator.GetIndentEstimatedSize() + 1;

            return result;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            EstimateExactSizeResult result{};
            result.size = 1;

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const U& val : iter) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val) iferr_return Nullopt;

                result.size += estimated.size;
                result.size += 2;
            }

            if (vals.IsPopulated()) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, vals.GetLast()) iferr_return Nullopt;

                result.size += estimated.size;
            }

            result.size += 1;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(
        FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const StaticQueue<T, SIZE>& vals
    ) noexcept {
        iferr_scope;

        using CleanT = _FormatDetails::clean_t<T>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "noinline"_s) {

            writer.Append('[') iferr_return false;

            if (vals.IsEmpty()) {

                writer.Append(']') iferr_return false;
                return true;
            }

            writer.Append('\n') iferr_return false;
            writer.PushIndent();

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const T& val : iter) {

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append('\n') iferr_return false;
            }

            if (vals.IsPopulated()) {

                writer.AppendIndent() iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, vals.GetLast()) iferr_return false;
                writer.Append('\n') iferr_return false;
            }

            writer.PopIndent();

            writer.AppendIndent() iferr_return false;
            writer.Append(']') iferr_return false;

            return true;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            writer.Append('[') iferr_return false;

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const T& val : iter) {

                Format<T>::WriteAppend(writer, parametersNext, val) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;
            }

            if (vals.IsPopulated()) {

                Format<CleanT>::WriteAppend(writer, parametersNext, vals.GetLast()) iferr_return false;
            }

            writer.Append(']') iferr_return false;

            return true;
        }

        return false;
    }
};

} // namespace Alt

#endif // __ALT_STD_STATIC_QUEUE__