#pragma once

#ifndef __ALT_STD_MEMORY_PRE_INCLUDE__
#define __ALT_STD_MEMORY_PRE_INCLUDE__

#include <new>

#include "../../TypeTraits.h"

#include "../../Utils/Impl/NumPreInclude.h"
#include "../../Platform/Impl/PlatformPreInclude.h"

namespace Alt {
namespace Memory {

enum class MallocBits : usize {

    None = 0,

    FillZero = static_cast<usize>(Platform::MemoryAllocateBytesBits::FillZero),
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(MallocBits);

enum class ReallocBits : usize {

    None = 0,

    FillZero = static_cast<usize>(Platform::MemoryReallocateBytesBits::FillZero),
    InPlace = static_cast<usize>(Platform::MemoryReallocateBytesBits::InPlace),

    /*
     * If specified means we are using internal system routine for reallocation and fully 'memcpy' the whole
     * old region on grow - used as optimization hint (for example for shrink operation can reduce code generation,
     * applied on is_trivially_reallocatable_v<T> types).
     * 
     * The reallocation can be performed even if 'InPlace' bit specified.
     */
    AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace = static_cast<usize>(1) << Num::Desc<u16>::Bits(),
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(ReallocBits);

namespace _MemoryDetails {

template <typename T, bool IS_WITH_OVERLFOW_CHECK = (sizeof(T) > 1)>
struct MallocImpl;

template <typename T, bool IS_TRIVIALLY_REALLOCATABLE = TypeTraits::is_trivially_reallocatable_v<T>>
struct ReallocImpl;

template <typename T, bool IS_WITH_OVERLFOW_CHECK = (sizeof(T) > 1)>
struct AlignedMallocImpl;

template <typename T, bool IS_TRIVIALLY_REALLOCATABLE = TypeTraits::is_trivially_reallocatable_v<T>>
struct AlignedReallocImpl;

} // namespace _MemoryDetails

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize MallocDefaultAlignment() noexcept {
    return Platform::MemoryDefaultAllocationAlignment();
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T* Malloc(const MallocBits bits = MallocBits::None) noexcept {
    return _MemoryDetails::MallocImpl<T>::Malloc(bits);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T* Malloc(const usize cnt, const MallocBits bits = MallocBits::None) noexcept {
    return _MemoryDetails::MallocImpl<T>::Malloc(cnt, bits);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T* Realloc(T* pMemoryOld, const usize cntOld, const usize cntNew, const ReallocBits bits = ReallocBits::None) noexcept {
    return _MemoryDetails::MallocImpl<T>::Realloc(pMemoryOld, cntOld, cntNew, bits);
}

ALT_FORCE_INLINE void Free(void* ptr) noexcept {
    Platform::MemoryDeallocateBytes(ptr);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T* AlignedMalloc(const MallocBits bits = MallocBits::None) noexcept {
    return _MemoryDetails::AlignedMallocImpl<T>::AlignedMalloc(bits);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T* AlignedMalloc(const usize cnt, const MallocBits bits = MallocBits::None) noexcept {
    return _MemoryDetails::AlignedMallocImpl<T>::AlignedMalloc(cnt, bits);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T* AlignedRealloc(T* pMemoryOld, const usize cntOld, const usize cntNew, const ReallocBits bits = ReallocBits::None) noexcept {
    return _MemoryDetails::AlignedMallocImpl<T>::AlignedRealloc(pMemoryOld, cntOld, cntNew, bits);
}

ALT_FORCE_INLINE void AlignedFree(void* ptr) noexcept {
    Platform::MemoryAlignedDeallocateBytes(ptr);
}

/*
 * 'Choose...Malloc...(...)' - version in which we choose between default malloc and aligned malloc
 * based on type alignment. Malloc by default has alignment, so for better code generation we can
 * use 'Choose...Malloc...(...)' version if exact type known on deallocation and need to use <= MallocDefaultAlignment().
 */
template <typename T,
    std::enable_if_t<(alignof(T) <= MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* ChooseAlignedMalloc(const MallocBits bits = MallocBits::None) noexcept {
    return _MemoryDetails::MallocImpl<T>::Malloc(bits);
}

template <typename T,
    std::enable_if_t<(alignof(T) > MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* ChooseAlignedMalloc(const MallocBits bits = MallocBits::None) noexcept {
    return _MemoryDetails::AlignedMallocImpl<T>::AlignedMalloc(bits);
}

template <typename T,
    std::enable_if_t<(alignof(T) <= MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* ChooseAlignedMalloc(const usize cnt, const MallocBits bits = MallocBits::None) noexcept {
    return _MemoryDetails::MallocImpl<T>::Malloc(cnt, bits);
}

template <typename T,
    std::enable_if_t<(alignof(T) > MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* ChooseAlignedMalloc(const usize cnt, const MallocBits bits = MallocBits::None) noexcept {
    return _MemoryDetails::AlignedMallocImpl<T>::AlignedMalloc(cnt, bits);
}

template <typename T,
    std::enable_if_t<(alignof(T) <= MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* ChooseAlignedRealloc(T* pMemoryOld, const usize cntOld, const usize cntNew, const ReallocBits bits = ReallocBits::None) noexcept {
    return _MemoryDetails::MallocImpl<T>::Realloc(pMemoryOld, cntOld, cntNew, bits);
}

template <typename T,
    std::enable_if_t<(alignof(T) > MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* ChooseAlignedRealloc(T* pMemoryOld, const usize cntOld, const usize cntNew, const ReallocBits bits = ReallocBits::None) noexcept {
    return _MemoryDetails::AlignedMallocImpl<T>::AlignedRealloc(pMemoryOld, cntOld, cntNew, bits);
}

template <typename T,
    std::enable_if_t<(alignof(T) <= MallocDefaultAlignment())>* = nullptr>
ALT_FORCE_INLINE void ChooseAlignedFree(void* ptr) noexcept {
    Platform::MemoryDeallocateBytes(ptr);
}

template <typename T,
    std::enable_if_t<(alignof(T) > MallocDefaultAlignment())>* = nullptr>
ALT_FORCE_INLINE void ChooseAlignedFree(void* ptr) noexcept {
    Platform::MemoryAlignedDeallocateBytes(ptr);
}

ALT_NODISCARD ALT_FORCE_INLINE usize Align(const void* ptr_, const usize alignment) noexcept {
    ALT_ASSERT(alignment != 0 && (alignment & (alignment - 1)) == 0);

    const uptr ptr = reinterpret_cast<uptr>(ptr_);
    return static_cast<usize>(((ptr + (alignment - 1)) & ~(alignment - 1)) - ptr);
}

ALT_FORCE_INLINE void* AlignPtr(void* ptr_, const usize alignment) noexcept {
    ALT_ASSERT(alignment != 0 && (alignment & (alignment - 1)) == 0);

    const uptr ptr = reinterpret_cast<uptr>(ptr_);
    return reinterpret_cast<void*>((ptr + (alignment - 1)) & ~(alignment - 1));
}

ALT_FORCE_INLINE const void* AlignPtr(const void* ptr_, const usize alignment) noexcept {
    ALT_ASSERT(alignment != 0 && (alignment & (alignment - 1)) == 0);

    const uptr ptr = reinterpret_cast<uptr>(ptr_);
    return reinterpret_cast<const void*>((ptr + (alignment - 1)) & ~(alignment - 1));
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsOverlap(
    const void* pBegin0, const void* pEnd0, const void* pBegin1, const void* pEnd1
) noexcept {

    return !(pEnd1 <= pBegin0 || pBegin1 >= pEnd0);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 RoundUpToPageSizeWrap(const u32 sizeInBytes) noexcept {
    return Platform::MemoryRoundUpToPageSizeWrap(sizeInBytes);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 RoundUpToPageSizeWrap(const u32 sizeInBytes, const u32 wrap) noexcept {
    return Platform::MemoryRoundUpToPageSizeWrap(sizeInBytes, wrap);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 RoundUpToPageSizeWrap(const u64 sizeInBytes) noexcept {
    return Platform::MemoryRoundUpToPageSizeWrap(sizeInBytes);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 RoundUpToPageSizeWrap(const u64 sizeInBytes, const u64 wrap) noexcept {
    return Platform::MemoryRoundUpToPageSizeWrap(sizeInBytes, wrap);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 RoundUpToPageSizeWrapPreserveZero(const u32 sizeInBytes) noexcept {
    return Platform::MemoryRoundUpToPageSizeWrapPreserveZero(sizeInBytes);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 RoundUpToPageSizeWrapPreserveZero(const u32 sizeInBytes, const u32 wrap) noexcept {
    return Platform::MemoryRoundUpToPageSizeWrapPreserveZero(sizeInBytes, wrap);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 RoundUpToPageSizeWrapPreserveZero(const u64 sizeInBytes) noexcept {
    return Platform::MemoryRoundUpToPageSizeWrapPreserveZero(sizeInBytes);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 RoundUpToPageSizeWrapPreserveZero(const u64 sizeInBytes, const u64 wrap) noexcept {
    return Platform::MemoryRoundUpToPageSizeWrapPreserveZero(sizeInBytes, wrap);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 RoundDownToPageSize(const u32 sizeInBytes) noexcept {
    return Platform::MemoryRoundDownToPageSize(sizeInBytes);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 RoundDownToPageSize(const u64 sizeInBytes) noexcept {
    return Platform::MemoryRoundDownToPageSize(sizeInBytes);
}

template <typename FromT, typename ToT,
    std::enable_if_t<
        std::is_trivially_copyable_v<FromT> &&
        std::is_trivially_copyable_v<ToT> &&
        sizeof(FromT) == sizeof(ToT)
    >* = nullptr
>
ALT_NODISCARD ALT_FORCE_INLINE ToT Transmute(const FromT& from) noexcept {

    ToT to;
    std::memcpy(&to, &from, sizeof(ToT));

    return to;
}

template <typename FromT, typename ToT,
    std::enable_if_t<
        std::is_trivially_copyable_v<FromT> &&
        std::is_trivially_copyable_v<ToT> &&
        sizeof(FromT) == sizeof(ToT)
    >* = nullptr
>
ALT_FORCE_INLINE void TransmuteInto(const FromT& from, ToT& to) noexcept {

    std::memcpy(&to, &from, sizeof(ToT));
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && sizeof(T) == 2>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T ByteSwap(const T num_) noexcept {

    using U = typename Num::Desc<T>::Unsigned;
    const U num = Transmute<T, U>(num_);

#if defined(ALT_COMPILER_MSVC)

    return Transmute<U, T>(_byteswap_ushort(num));

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return Transmute<U, T>(__builtin_bswap16(num));

#else
    #error provide implementation for specific compiler
#endif
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && sizeof(T) == 4>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T ByteSwap(const T num_) noexcept {

    using U = typename Num::Desc<T>::Unsigned;
    const U num = Transmute<T, U>(num_);

#if defined(ALT_COMPILER_MSVC)

    return Transmute<U, T>(_byteswap_ulong(num));

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return Transmute<U, T>(__builtin_bswap32(num));

#else
    #error provide implementation for specific compiler
#endif
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && sizeof(T) == 8>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T ByteSwap(const T num_) noexcept {

    using U = typename Num::Desc<T>::Unsigned;
    const U num = Transmute<T, U>(num_);

#if defined(ALT_COMPILER_MSVC)

    return Transmute<U, T>(_byteswap_uint64(num));

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return Transmute<U, T>(__builtin_bswap64(num));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * @example
 * 
 * const usize littleEndianX = ByteSwapIf(x, ALT_PLATFORM_BIG_ENDIAN_BOOL)
 */
template <typename T,
    std::enable_if_t<std::is_scalar_v<T> && sizeof(T) >= 2 && sizeof(T) <= 8>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T ByteSwapIf(const T num, const bool cond) noexcept {

    return cond ? ByteSwap(num) : num;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize KB(const usize kb) noexcept {
    return kb * 1'024;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize MB(const usize mb) noexcept {
    return KB(mb * 1'024);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GB(const usize gb) noexcept {
    return MB(gb * 1'024);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize TB(const usize tb) noexcept {
    return GB(tb * 1'024);
}

template <typename T, typename ...ArgsT,
    std::enable_if_t<std::is_constructible_v<T, ArgsT&&...>>* = nullptr>
ALT_FORCE_INLINE T* ConstructObject(T* ptr, ArgsT&&... val) noexcept {

    new (ptr) T(std::forward<ArgsT>(val)...);
    return std::launder(ptr);
}

template <typename T, typename U = T,
    std::enable_if_t<std::is_constructible_v<T, U&&>>* = nullptr>
ALT_FORCE_INLINE T* ConstructObject(T* ptr, U&& val) noexcept {

    new (ptr) T(std::forward<U>(val));
    return std::launder(ptr);
}

template <typename T, typename ...ArgsT,
    std::enable_if_t<std::is_constructible_v<T, ArgsT&&...>>* = nullptr>
ALT_FORCE_INLINE T* ConstructObject(void* ptr, ArgsT&&... val) noexcept {

    return ConstructObject(reinterpret_cast<T*>(ptr), std::forward<ArgsT>(val)...);
}

template <typename T, typename U = T,
    std::enable_if_t<std::is_constructible_v<T, U&&>>* = nullptr>
ALT_FORCE_INLINE T* ConstructObject(void* ptr, U&& val) noexcept {

    return ConstructObject(reinterpret_cast<T*>(ptr), std::forward<U>(val));
}

template <typename T, typename ...ArgsT,
    std::enable_if_t<std::is_constructible_v<T, ArgsT&&...>>* = nullptr>
ALT_FORCE_INLINE T* ConstructObject(T& obj, ArgsT&&... val) noexcept {

    return ConstructObject<T>(&obj, std::forward<ArgsT>(val)...);
}

template <typename T, typename U = T,
    std::enable_if_t<std::is_constructible_v<T, U&&>>* = nullptr>
ALT_FORCE_INLINE T* ConstructObject(T& obj, U&& val) noexcept {

    return ConstructObject<T>(&obj, std::forward<U>(val));
}

template <typename T,
    std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
ALT_FORCE_INLINE T* ConstructDefaultObject(T* ptr) noexcept {

    new (ptr) T{};
    return std::launder(ptr);
}

template <typename T,
    std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr>
ALT_FORCE_INLINE T* ConstructDefaultObject(void* ptr) noexcept {

    return ConstructDefaultObject(static_cast<T*>(ptr));
}

template <typename T>
struct CloneFromFirstElementResult {

    T* pDstObjectWhereCloneFailedOrEnd = nullptr;
    const T* pSrcObjectWhereCloneFailedOrEnd = nullptr;

    bool isOk = true;
};

template <typename T>
struct CloneFromLastElementResult {

    T* pDstObjectWhereCloneFailedOrBeginMinusOne = nullptr;
    const T* pSrcObjectWhereCloneFailedOrBeginMinusOne = nullptr;

    bool isOk = true;
};

template <typename T>
struct MoveFromFirstElementResult {

    T* pDstEnd = nullptr;
    T* pSrcEnd = nullptr;
};

template <typename T>
struct MoveFromLastElementResult {

    T* pDstBeginMinusOne = nullptr;
    T* pSrcBeginMinusOne = nullptr;
};

template <typename DstT, typename SrcT,
    std::enable_if_t<std::is_constructible_v<DstT, SrcT&&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE DstT* CloneConstructOrMoveConstructObject(DstT* ptr_, SrcT&& val) noexcept {

    DstT* ptr = ConstructObject(ptr_, std::forward<SrcT>(val));
    return ptr + 1;
}

template <typename DstT, typename SrcT,
    std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<DstT, const SrcT&> && !std::is_constructible_v<DstT, const SrcT&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE DstT* CloneConstructOrMoveConstructObject(DstT* ptr, const SrcT& val) noexcept {

    const bool isOk = ptr->CloneConstructFrom(val);

    ptr += isOk ? 1 : 0;
    return ptr;
}

template <typename DstT, typename SrcT,
    std::enable_if_t<std::is_constructible_v<DstT, SrcT&&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE DstT* CloneConstructOrMoveConstructObject(void* ptr_, SrcT&& val) noexcept {

    DstT* ptr = ConstructObject<DstT>(ptr_, std::forward<SrcT>(val));
    return ptr + 1;
}

template <typename DstT, typename SrcT,
    std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<DstT, const SrcT&> && !std::is_constructible_v<DstT, const SrcT&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE DstT* CloneConstructOrMoveConstructObject(void* ptr, const SrcT& val) noexcept {

    return CloneConstructOrMoveConstructObject(static_cast<DstT*>(ptr), std::forward<SrcT>(val));
}

template <typename DstT, typename SrcT,
    std::enable_if_t<std::is_constructible_v<DstT, SrcT&&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE DstT* CloneConstructOrMoveConstructObject(DstT& obj, SrcT&& val) noexcept {

    DstT* ptr = ConstructObject(obj, std::forward<SrcT>(val));
    return ptr + 1;
}

template <typename DstT, typename SrcT,
    std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<DstT, const SrcT&> && !std::is_constructible_v<DstT, const SrcT&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE DstT* CloneConstructOrMoveConstructObject(DstT& obj, const SrcT& val) noexcept {

    return CloneConstructOrMoveConstructObject<DstT>(&obj, val);
}

template <typename DstT, typename SrcT,
    std::enable_if_t<std::is_assignable_v<DstT&, SrcT&&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE DstT* CloneOrMoveObject(DstT& obj, SrcT&& val) noexcept {

    obj = std::forward<SrcT>(val);
    return &obj + 1;
}

template <typename DstT, typename SrcT,
    std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<DstT, const SrcT&> && !std::is_assignable_v<DstT&, const SrcT&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE DstT* CloneOrMoveObject(DstT& obj, const SrcT& val) noexcept {

    const bool isOk = obj.CloneFrom(val);

    DstT* ptr = &obj;
    ptr += isOk ? 1 : 0;
    return ptr;
}

template <typename T>
ALT_FORCE_INLINE void DestroyObject(T& obj) noexcept;

template <typename T,
    std::enable_if_t<!std::is_array_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* AllocateObject() noexcept {

    T* pMemory = ChooseAlignedMalloc<T>();
    if (!pMemory) {
        return nullptr;
    }

    ConstructDefaultObject(pMemory);

    return pMemory;
}

template <typename T, typename ...ArgsT,
    std::enable_if_t<!std::is_array_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* AllocateObject(ArgsT&&... args) noexcept {

    T* pMemory = ChooseAlignedMalloc<T>();
    if (!pMemory) {
        return nullptr;
    }

    return ConstructObject(pMemory, std::forward<ArgsT>(args)...);
}

template <typename T,
    std::enable_if_t<!std::is_array_v<T>>* = nullptr>
ALT_FORCE_INLINE void DeallocateObject(T*& pObj) noexcept {

    if (!pObj) {
        return;
    }

    DestroyObject(*pObj);

    ChooseAlignedFree<T>(pObj);
    pObj = nullptr;
}

template <typename T,
    std::enable_if_t<!std::is_array_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* AllocateArrayObject(const usize cnt) noexcept {

    T* pMemory = ChooseAlignedMalloc<T>(cnt);
    if (!pMemory) {
        return nullptr;
    }

    ConstructDefaultObjects(pMemory, pMemory + cnt);

    return pMemory;
}

template <typename T, typename ...ArgsT,
    std::enable_if_t<!std::is_array_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* AllocateArrayObject(const usize cnt, ArgsT&&... args) noexcept {

    T* pMemory = ChooseAlignedMalloc<T>(cnt);
    if (!pMemory) {
        return nullptr;
    }

    const T* pEnd = pMemory + cnt;
    ConstructObjectsFromFirstElement(pMemory, pMemory + cnt, std::forward<ArgsT>(args)...);

    return pMemory;
}

template <typename T, typename ...ArgsT,
    std::enable_if_t<!std::is_array_v<T>>* = nullptr>
ALT_FORCE_INLINE void DeallocateArrayObject(T*& pMemory, const usize cnt) noexcept {

    if (!pMemory) {
        return;
    }

    DestroyObjects(pMemory, pMemory + cnt);

    ChooseAlignedFree<T>(pMemory);
    pMemory = nullptr;
}

namespace _MemoryDetails {

/*
 * `Alt` assumes that move construction/assign is a cheap operation which
 * never fails. Same for destructors - there are exists cases where destructors
 * can fail, but this should be handled separately maybe with some global state
 * or do not rely on RAII (we can't deal with this because want RAII and disabled
 * exceptions).
 * 
 * Copy can fail. Assumed that have 3 types of copy:
 *     1. Trivial - simple, never fail, just use memcpy/memmove.
 *     2. Copy from user generated copy constructor/assign - as we do not using
 *        exceptions this operations on fail panics. In `Alt` such copy constructor/assign
 *        implemented only if they are trivial or underlying type implements such
 *        operations and we do not add additional possible errors on this stage
 *        (for simplicity) - itself `Alt` only implements move constructor/assign.
 *     3. `Clonable Trait` - the best to handle copying. For more info see. `is_clonable` in TypeTraits.
 *        This is best way to work with copy because we can handle errors and explicitly
 *        demonstrate that this is copy which is heavy and can fail. Clonable Trait has 3
 *        methods `bool CloneFrom(...)`, `bool CloneConstructFrom(...)` and `Optional<T> Clone()`.
 *        When `bool CloneConstructFrom(...)` failed the object still not constructed.
 */
enum class CopyTrait {

    MemcpyOrMemmove,
    CppCopy,
    AltClone,
};

template <CopyTrait TRAIT>
using Copy = std::integral_constant<CopyTrait, TRAIT>;

enum class MoveTrait {

    MemcpyOrMemmove,
    MoveByElements,
};

template <MoveTrait TRAIT>
using Move = std::integral_constant<MoveTrait, TRAIT>;

enum class FillDefaultTrait {

    Memset,
    DefaultByElement,
};

template <FillDefaultTrait TRAIT>
using FillDefault = std::integral_constant<FillDefaultTrait, TRAIT>;

enum class FillTrait {

    Memset,
    CppCopy,
    AltClone,
};

template <FillTrait TRAIT>
using Fill = std::integral_constant<FillTrait, TRAIT>;

template <typename T>
struct CloneResult {

    T* pDstFailedOrEndOrBeginMinusOne = nullptr;
    const T* pSrcFailedOrEndOrBeginMinusOne = nullptr;
};

template <typename T>
struct MoveResult {

    T* pDstFailedOrEndOrBeginMinusOne = nullptr;
    T* pSrcFailedOrEndOrBeginMinusOne = nullptr;
};

template <typename T, bool IS_TRIVIALLY_DESTRUCTIBLE = std::is_trivially_destructible_v<T>>
struct DestroyObjectImpl;

template <typename T, bool IS_TRIVIALLY_DESTRUCTIBLE = std::is_trivially_destructible_v<T>>
struct DestroyObjectsImpl;

template <typename T>
constexpr static FillTrait fill_construct_clone_objects_copy_v =
    std::conditional_t<
        std::is_trivially_constructible_v<T> && std::is_trivially_copyable_v<T> && std::is_scalar_v<T> && sizeof(T) == sizeof(u8),
        Fill<FillTrait::Memset>,
        Fill<FillTrait::CppCopy>
    >::value;

template <typename T>
constexpr static FillTrait fill_construct_clone_objects_clone_v =
    std::conditional_t<

        std::is_trivially_constructible_v<T> && std::is_trivially_copyable_v<T> && std::is_scalar_v<T> && sizeof(T) == sizeof(u8),
        Fill<FillTrait::Memset>,

        std::conditional_t<
            TypeTraits::is_clonable_v<T>,
            Fill<FillTrait::AltClone>,
            Fill<FillTrait::CppCopy>
        >
    >::value;

template <
    typename T,
    FillDefaultTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_copyable_v<T> && std::is_scalar_v<T>,
            FillDefault<FillDefaultTrait::Memset>,
            FillDefault<FillDefaultTrait::DefaultByElement>
        >::value
>
struct FillConstructDefaultObjectsImpl;

template <typename T, FillTrait TRAIT>
struct FillCloneConstructObjectsFromFirstElementImpl;

template <typename T, FillTrait TRAIT>
struct FillCloneConstructObjectsFromLastElementImpl;

template <typename T>
constexpr static FillTrait fill_clone_objects_copy_v =
    std::conditional_t<
        std::is_trivially_copyable_v<T> && std::is_scalar_v<T> && sizeof(T) == sizeof(u8),
        Fill<FillTrait::Memset>,
        Fill<FillTrait::CppCopy>
    >::value;

template <typename T>
constexpr static FillTrait fill_clone_objects_clone_v =
    std::conditional_t<

        std::is_trivially_copyable_v<T> && std::is_scalar_v<T> && sizeof(T) == sizeof(u8),
        Fill<FillTrait::Memset>,

        std::conditional_t<
            TypeTraits::is_clonable_v<T>,
            Fill<FillTrait::AltClone>,
            Fill<FillTrait::CppCopy>
        >
    >::value;

template <
    typename T,
    FillDefaultTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_copyable_v<T> && std::is_scalar_v<T>,
            FillDefault<FillDefaultTrait::Memset>,
            FillDefault<FillDefaultTrait::DefaultByElement>
        >::value
>
struct FillDefaultObjectsImpl;

template <typename T, FillTrait TRAIT>
struct FillCloneObjectsFromFirstElementImpl;

template <typename T, FillTrait TRAIT>
struct FillCloneObjectsFromLastElementImpl;

template <typename T>
constexpr static CopyTrait clone_objects_copy_v =
    std::conditional_t<
        std::is_trivially_copyable_v<T>,
        Copy<CopyTrait::MemcpyOrMemmove>,
        Copy<CopyTrait::CppCopy>
    >::value;

template <typename T>
constexpr static CopyTrait clone_objects_clone_v =
    std::conditional_t<

        std::is_trivially_copyable_v<T>,
        Copy<CopyTrait::MemcpyOrMemmove>,

        std::conditional_t<
            TypeTraits::is_clonable_v<T>,
            Copy<CopyTrait::AltClone>,
            Copy<CopyTrait::CppCopy>
        >
    >::value;

template <typename T, CopyTrait TRAIT>
struct CloneObjectsFromFirstElementNonOverlapImpl;

template <typename T, CopyTrait TRAIT>
struct CloneObjectsFromLastElementNonOverlapImpl;

template <typename T, CopyTrait TRAIT>
struct CloneObjectsFromFirstElementImpl;

template <typename T, CopyTrait TRAIT>
struct CloneObjectsFromLastElementImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_move_assignable_v<T> && std::is_trivially_copyable_v<T>,
            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveObjectsFromFirstElementNonOverlapImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_move_assignable_v<T> && std::is_trivially_copyable_v<T>,
            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveObjectsFromLastElementNonOverlapImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_move_assignable_v<T> && std::is_trivially_copyable_v<T>,
            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveObjectsFromFirstElementImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_move_assignable_v<T> && std::is_trivially_copyable_v<T>,
            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveObjectsFromLastElementImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<

            std::is_trivially_move_assignable_v<T> &&
            std::is_trivially_copyable_v<T> &&
            std::is_trivially_destructible_v<T>,

            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveDestructibleObjectsFromFirstElementNonOverlapImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<

            std::is_trivially_move_assignable_v<T> &&
            std::is_trivially_copyable_v<T> &&
            std::is_trivially_destructible_v<T>,

            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveDestructibleObjectsFromLastElementNonOverlapImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<

            std::is_trivially_move_assignable_v<T> &&
            std::is_trivially_copyable_v<T> &&
            std::is_trivially_destructible_v<T>,

            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveDestructibleObjectsFromFirstElementImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<

            std::is_trivially_move_assignable_v<T> &&
            std::is_trivially_copyable_v<T> &&
            std::is_trivially_destructible_v<T>,

            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveDestructibleObjectsFromLastElementImpl;

template <typename T>
constexpr static CopyTrait construct_clone_objects_copy_v =
    std::conditional_t<
        std::is_trivially_copy_constructible_v<T> && std::is_trivially_copyable_v<T>,
        Copy<CopyTrait::MemcpyOrMemmove>,
        Copy<CopyTrait::CppCopy>
    >::value;

template <typename T>
constexpr static CopyTrait construct_clone_objects_clone_v =
    std::conditional_t<

        std::is_trivially_copy_constructible_v<T> && std::is_trivially_copyable_v<T>,
        Copy<CopyTrait::MemcpyOrMemmove>,

        std::conditional_t<
            TypeTraits::is_clonable_v<T>,
            Copy<CopyTrait::AltClone>,
            Copy<CopyTrait::CppCopy>
        >
    >::value;

template <typename T, CopyTrait TRAIT>
struct CloneConstructObjectsFromFirstElementNonOverlapImpl;

template <typename T, CopyTrait TRAIT>
struct CloneConstructObjectsFromLastElementNonOverlapImpl;

template <typename T, CopyTrait TRAIT>
struct CloneConstructObjectsFromFirstElementImpl;

template <typename T, CopyTrait TRAIT>
struct CloneConstructObjectsFromLastElementImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_move_constructible_v<T> && std::is_trivially_copyable_v<T>,
            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveConstructObjectsFromFirstElementNonOverlapImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_move_constructible_v<T> && std::is_trivially_copyable_v<T>,
            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveConstructObjectsFromLastElementNonOverlapImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_move_constructible_v<T> && std::is_trivially_copyable_v<T>,
            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveConstructObjectsFromFirstElementImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<
            std::is_trivially_move_constructible_v<T> && std::is_trivially_copyable_v<T>,
            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveConstructObjectsFromLastElementImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<

            std::is_trivially_move_constructible_v<T> &&
            std::is_trivially_copyable_v<T> &&
            std::is_trivially_destructible_v<T>,

            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveDestructibleConstructObjectsFromFirstElementNonOverlapImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<

            std::is_trivially_move_constructible_v<T> &&
            std::is_trivially_copyable_v<T> &&
            std::is_trivially_destructible_v<T>,

            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveDestructibleConstructObjectsFromLastElementNonOverlapImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<

            std::is_trivially_move_constructible_v<T> &&
            std::is_trivially_copyable_v<T> &&
            std::is_trivially_destructible_v<T>,

            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveDestructibleConstructObjectsFromFirstElementImpl;

template <
    typename T,
    MoveTrait TRAIT = 
        std::conditional_t<

            std::is_trivially_move_constructible_v<T> &&
            std::is_trivially_copyable_v<T> &&
            std::is_trivially_destructible_v<T>,

            Move<MoveTrait::MemcpyOrMemmove>,
            Move<MoveTrait::MoveByElements>
        >::value
>
struct MoveDestructibleConstructObjectsFromLastElementImpl;

template <typename T, bool IS_TRIVIALLY_REALLOCATABLE = TypeTraits::is_trivially_reallocatable_v<T>>
struct MemcpyOrMoveDestructibleConstructObjectsNonOverlapImpl;

} // namespace _MemoryDetails

template <typename T>
ALT_FORCE_INLINE void DestroyObject(T& obj) noexcept {

    _MemoryDetails::DestroyObjectImpl<T>::Destroy(obj);
}

template <typename T>
ALT_FORCE_INLINE void DestroyObjects(T* __restrict pBegin, T* __restrict pEnd) noexcept {

    _MemoryDetails::DestroyObjectsImpl<T>::Destroy(pBegin, pEnd);
}

template <typename T>
ALT_FORCE_INLINE void FillConstructDefaultObjects(T* __restrict pBegin, T* __restrict pEnd) noexcept {

    _MemoryDetails::FillConstructDefaultObjectsImpl<T>::FillConstructDefault(pBegin, pEnd);
}

template <typename DstT, typename SrcT,
    std::enable_if_t<std::is_constructible_v<DstT, const SrcT&>>* = nullptr>
ALT_FORCE_INLINE CloneFromFirstElementResult<DstT> FillCopyConstructObjects(
    DstT* __restrict pDstBegin, DstT* __restrict pDstEnd, const SrcT& val
) noexcept {

    DstT* pResult = _MemoryDetails::FillCloneConstructObjectsFromFirstElementImpl<
        DstT,
        _MemoryDetails::fill_construct_clone_objects_copy_v<DstT>
    >::FillCloneConstruct(pDstBegin, pDstEnd, val);

    CloneFromFirstElementResult<DstT> result;
    result.pDstObjectWhereCloneFailedOrEnd = pResult;
    result.pSrcObjectWhereCloneFailedOrEnd = reinterpret_cast<const DstT*>(&val + 1);
    result.isOk = true;

    return result;
}

template <typename DstT, typename SrcT,
    std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<DstT, const SrcT&> || std::is_constructible_v<DstT, const SrcT&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromFirstElementResult<DstT> FillCloneConstructObjectsFromFirstElement(
    DstT* __restrict pDstBegin, DstT* __restrict pDstEnd, const SrcT& val
) noexcept {

    DstT* pResult = _MemoryDetails::FillCloneConstructObjectsFromFirstElementImpl<
        DstT,
        _MemoryDetails::fill_construct_clone_objects_clone_v<DstT>
    >::FillCloneConstruct(pDstBegin, pDstEnd, val);

    CloneFromFirstElementResult<DstT> result;
    result.pDstObjectWhereCloneFailedOrEnd = pResult;
    result.pSrcObjectWhereCloneFailedOrEnd = reinterpret_cast<const DstT*>(&val + 1);
    result.isOk = pResult == pDstEnd;

    return result;
}

template <typename DstT, typename SrcT,
    std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<DstT, const SrcT&> || std::is_constructible_v<DstT, const SrcT&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromLastElementResult<DstT> FillCloneConstructObjectsFromLastElement(
    DstT* __restrict pDstBegin, DstT* __restrict pDstEnd, const SrcT& val
) noexcept {

    DstT* pResult = _MemoryDetails::FillCloneConstructObjectsFromLastElementImpl<
        DstT,
        _MemoryDetails::fill_construct_clone_objects_clone_v<DstT>
    >::FillCloneConstruct(pDstBegin, pDstEnd, val);

    CloneFromLastElementResult<DstT> result;
    result.pDstObjectWhereCloneFailedOrBeginMinusOne = pResult;
    result.pSrcObjectWhereCloneFailedOrBeginMinusOne = reinterpret_cast<const DstT*>(&val - 1);
    result.isOk = pResult + 1 == pDstBegin;

    return result;
}

template <typename T,
    std::enable_if_t<std::is_default_constructible_v<T> && std::is_assignable_v<T&, T&&>>* = nullptr>
ALT_FORCE_INLINE void FillDefaultObjects(T* __restrict pDstBegin, T* __restrict pDstEnd) noexcept {

    _MemoryDetails::FillDefaultObjectsImpl<T>::FillDefault(pDstBegin, pDstEnd);
}

template <typename DstT, typename SrcT,
    std::enable_if_t<std::is_assignable_v<DstT&, const SrcT&>>* = nullptr>
ALT_FORCE_INLINE DstT* FillCopyObjects(
    DstT* __restrict pDstBegin, DstT* __restrict pDstEnd, const SrcT& val
) noexcept {

    return _MemoryDetails::FillCloneObjectsFromFirstElementImpl<
        DstT,
        _MemoryDetails::fill_clone_objects_copy_v<DstT>
    >::FillClone(pDstBegin, pDstEnd, val);
}

template <typename DstT, typename SrcT,
    std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<DstT, const SrcT&> || std::is_assignable_v<DstT, const SrcT&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromFirstElementResult<DstT> FillCloneObjectsFromFirstElement(
    DstT* __restrict pDstBegin, DstT* __restrict pDstEnd, const SrcT& val
) noexcept {

    DstT* pResult = _MemoryDetails::FillCloneObjectsFromFirstElementImpl<
        DstT,
        _MemoryDetails::fill_clone_objects_clone_v<DstT>
    >::FillClone(pDstBegin, pDstEnd, val);

    CloneFromFirstElementResult<DstT> result;
    result.pDstObjectWhereCloneFailedOrEnd = pResult;
    result.pSrcObjectWhereCloneFailedOrEnd = reinterpret_cast<const DstT*>(&val + 1);
    result.isOk = pResult == pDstEnd;

    return result;
}

template <typename DstT, typename SrcT,
    std::enable_if_t<TypeTraits::is_only_clonable_to_from_v<DstT, const SrcT&> || std::is_assignable_v<DstT, const SrcT&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromLastElementResult<DstT> FillCloneObjectsFromLastElement(
    DstT* __restrict pDstBegin, DstT* __restrict pDstEnd, const SrcT& val
) noexcept {

    DstT* pResult = _MemoryDetails::FillCloneObjectsFromLastElementImpl<
        DstT,
        _MemoryDetails::fill_clone_objects_clone_v<DstT>
    >::FillClone(pDstBegin, pDstEnd, val);

    CloneFromLastElementResult<DstT> result;
    result.pDstObjectWhereCloneFailedOrBeginMinusOne = pResult;
    result.pSrcObjectWhereCloneFailedOrBeginMinusOne = reinterpret_cast<const DstT*>(&val - 1);
    result.isOk = pResult + 1 == pDstBegin;

    return result;
}

template <typename T,
    std::enable_if_t<std::is_assignable_v<T&, const T&>>* = nullptr>
ALT_FORCE_INLINE T* CopyObjectsNonOverlap(T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::CloneResult<T> result = _MemoryDetails::CloneObjectsFromFirstElementNonOverlapImpl<
        T,
        _MemoryDetails::clone_objects_copy_v<T>
    >::Clone(pDstBegin, pSrcBegin, pSrcEnd);

    return result.pDstFailedOrEndOrBeginMinusOne;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_clonable_v<T> || std::is_assignable_v<T&, const T&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromFirstElementResult<T> CloneObjectsFromFirstElementNonOverlap(
    T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneObjectsFromFirstElementNonOverlapImpl<
        T,
        _MemoryDetails::clone_objects_clone_v<T>
    >::Clone(pDstBegin, pSrcBegin, pSrcEnd);

    CloneFromFirstElementResult<T> result;
    result.pDstObjectWhereCloneFailedOrEnd = cloneResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcObjectWhereCloneFailedOrEnd = cloneResult.pSrcFailedOrEndOrBeginMinusOne;
    result.isOk = cloneResult.pSrcFailedOrEndOrBeginMinusOne == pSrcEnd;

    return result;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_clonable_v<T> || std::is_assignable_v<T&, const T&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromLastElementResult<T> CloneObjectsFromLastElementNonOverlap(
    T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneObjectsFromLastElementNonOverlapImpl<
        T,
        _MemoryDetails::clone_objects_clone_v<T>
    >::Clone(pDstBegin, pSrcBegin, pSrcEnd);

    CloneFromLastElementResult<T> result;
    result.pDstObjectWhereCloneFailedOrBeginMinusOne = cloneResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcObjectWhereCloneFailedOrBeginMinusOne = cloneResult.pSrcFailedOrEndOrBeginMinusOne;
    result.isOk = cloneResult.pSrcFailedOrEndOrBeginMinusOne + 1 == pSrcBegin;

    return result;
}

template <typename T,
    std::enable_if_t<std::is_assignable_v<T&, const T&>>* = nullptr>
ALT_FORCE_INLINE T* CopyObjectsFromFirstElement(T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd) noexcept {
    ALT_ASSERT(pSrcBegin >= pDstBegin);

    _MemoryDetails::CloneResult<T> result = _MemoryDetails::CloneObjectsFromFirstElementImpl<
        T,
        _MemoryDetails::clone_objects_copy_v<T>
    >::Clone(pDstBegin, pSrcBegin, pSrcEnd);

    return result.pDstFailedOrEndOrBeginMinusOne;
}

template <typename T,
    std::enable_if_t<std::is_assignable_v<T&, const T&>>* = nullptr>
ALT_FORCE_INLINE T* CopyObjectsFromLastElement(T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd) noexcept {
    ALT_ASSERT(pSrcBegin <= pDstBegin);

    _MemoryDetails::CloneResult<T> result = _MemoryDetails::CloneObjectsFromLastElementImpl<
        T,
        _MemoryDetails::clone_objects_copy_v<T>
    >::Clone(pDstBegin, pSrcBegin, pSrcEnd);

    return result.pDstFailedOrEndOrBeginMinusOne;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_clonable_v<T> || std::is_assignable_v<T&, const T&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromFirstElementResult<T> CloneObjectsFromFirstElement(
    T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin >= pDstBegin);

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneObjectsFromFirstElementImpl<
        T,
        _MemoryDetails::clone_objects_clone_v<T>
    >::Clone(pDstBegin, pSrcBegin, pSrcEnd);

    CloneFromFirstElementResult<T> result;
    result.pDstObjectWhereCloneFailedOrEnd = cloneResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcObjectWhereCloneFailedOrEnd = cloneResult.pSrcFailedOrEndOrBeginMinusOne;
    result.isOk = cloneResult.pSrcFailedOrEndOrBeginMinusOne == pSrcEnd;

    return result;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_clonable_v<T> || std::is_assignable_v<T&, const T&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromLastElementResult<T> CloneObjectsFromLastElement(
    T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin <= pDstBegin);

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneObjectsFromLastElementImpl<
        T,
        _MemoryDetails::clone_objects_clone_v<T>
    >::Clone(pDstBegin, pSrcBegin, pSrcEnd);

    CloneFromLastElementResult<T> result;
    result.pDstObjectWhereCloneFailedOrBeginMinusOne = cloneResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcObjectWhereCloneFailedOrBeginMinusOne = cloneResult.pSrcFailedOrEndOrBeginMinusOne;
    result.isOk = cloneResult.pSrcFailedOrEndOrBeginMinusOne + 1 == pSrcBegin;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromFirstElementResult<T> MoveObjectsFromFirstElementNonOverlap(
    T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveObjectsFromFirstElementNonOverlapImpl<
        T
    >::Move(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromFirstElementResult<T> result;
    result.pDstEnd = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcEnd = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromLastElementResult<T> MoveObjectsFromLastElementNonOverlap(
    T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveObjectsFromLastElementNonOverlapImpl<
        T
    >::Move(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromLastElementResult<T> result;
    result.pDstBeginMinusOne = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcBeginMinusOne = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromFirstElementResult<T> MoveObjectsFromFirstElement(
    T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin >= pDstBegin);

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveObjectsFromFirstElementImpl<
        T
    >::Move(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromFirstElementResult<T> result;
    result.pDstEnd = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcEnd = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromLastElementResult<T> MoveObjectsFromLastElement(
    T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin <= pDstBegin);

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveObjectsFromLastElementImpl<
        T
    >::Move(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromLastElementResult<T> result;
    result.pDstBeginMinusOne = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcBeginMinusOne = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromFirstElementResult<T> MoveDestructibleObjectsFromFirstElementNonOverlap(
    T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveDestructibleObjectsFromFirstElementNonOverlapImpl<
        T
    >::Move(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromFirstElementResult<T> result;
    result.pDstEnd = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcEnd = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromLastElementResult<T> MoveDestructibleObjectsFromLastElementNonOverlap(
    T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveDestructibleObjectsFromLastElementNonOverlapImpl<
        T
    >::Move(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromLastElementResult<T> result;
    result.pDstBeginMinusOne = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcBeginMinusOne = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromFirstElementResult<T> MoveDestructibleObjectsFromFirstElement(
    T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin >= pDstBegin);

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveDestructibleObjectsFromFirstElementImpl<
        T
    >::Move(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromFirstElementResult<T> result;
    result.pDstEnd = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcEnd = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromLastElementResult<T> MoveDestructibleObjectsFromLastElement(
    T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin <= pDstBegin);

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveDestructibleObjectsFromLastElementImpl<
        T
    >::Move(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromLastElementResult<T> result;
    result.pDstBeginMinusOne = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcBeginMinusOne = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T,
    std::enable_if_t<std::is_constructible_v<T, const T&>>* = nullptr>
ALT_FORCE_INLINE T* CopyConstructObjectsNonOverlap(
    T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::CloneResult<T> result = _MemoryDetails::CloneConstructObjectsFromFirstElementNonOverlapImpl<
        T,
        _MemoryDetails::construct_clone_objects_copy_v<T>
    >::CloneConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    return result.pDstFailedOrEndOrBeginMinusOne;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_clonable_v<T> || std::is_constructible_v<T, const T&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromFirstElementResult<T> CloneConstructObjectsFromFirstElementNonOverlap(
    T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneConstructObjectsFromFirstElementNonOverlapImpl<
        T,
        _MemoryDetails::construct_clone_objects_clone_v<T>
    >::CloneConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    CloneFromFirstElementResult<T> result;
    result.pDstObjectWhereCloneFailedOrEnd = cloneResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcObjectWhereCloneFailedOrEnd = cloneResult.pSrcFailedOrEndOrBeginMinusOne;
    result.isOk = cloneResult.pSrcFailedOrEndOrBeginMinusOne == pSrcEnd;

    return result;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_clonable_v<T> || std::is_constructible_v<T, const T&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromLastElementResult<T> CloneConstructObjectsFromLastElementNonOverlap(
    T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneConstructObjectsFromLastElementNonOverlapImpl<
        T,
        _MemoryDetails::construct_clone_objects_clone_v<T>
    >::CloneConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    CloneFromLastElementResult<T> result;
    result.pDstObjectWhereCloneFailedOrBeginMinusOne = cloneResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcObjectWhereCloneFailedOrBeginMinusOne = cloneResult.pSrcFailedOrEndOrBeginMinusOne;
    result.isOk = cloneResult.pSrcFailedOrEndOrBeginMinusOne + 1 == pSrcBegin;

    return result;
}

template <typename T,
    std::enable_if_t<std::is_constructible_v<T, const T&>>* = nullptr>
ALT_FORCE_INLINE T* CopyConstructObjectsFromFirstElement(
    T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin >= pDstBegin);

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneConstructObjectsFromFirstElementImpl<
        T,
        _MemoryDetails::construct_clone_objects_copy_v<T>
    >::CloneConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    return cloneResult.pDstFailedOrEndOrBeginMinusOne;
}

template <typename T,
    std::enable_if_t<std::is_constructible_v<T, const T&>>* = nullptr>
ALT_FORCE_INLINE T* CopyConstructObjectsFromLastElement(
    T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin <= pDstBegin);

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneConstructObjectsFromLastElementImpl<
        T,
        _MemoryDetails::construct_clone_objects_copy_v<T>
    >::CloneConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    return cloneResult.pDstFailedOrEndOrBeginMinusOne;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_clonable_v<T> || std::is_constructible_v<T, const T&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromFirstElementResult<T> CloneConstructObjectsFromFirstElement(
    T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin >= pDstBegin);

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneConstructObjectsFromFirstElementImpl<
        T,
        _MemoryDetails::construct_clone_objects_clone_v<T>
    >::CloneConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    CloneFromFirstElementResult<T> result;
    result.pDstObjectWhereCloneFailedOrEnd = cloneResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcObjectWhereCloneFailedOrEnd = cloneResult.pSrcFailedOrEndOrBeginMinusOne;
    result.isOk = cloneResult.pSrcFailedOrEndOrBeginMinusOne == pSrcEnd;

    return result;
}

template <typename T,
    std::enable_if_t<TypeTraits::is_clonable_v<T> || std::is_constructible_v<T, const T&>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE CloneFromLastElementResult<T> CloneConstructObjectsFromLastElement(
    T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin <= pDstBegin);

    _MemoryDetails::CloneResult<T> cloneResult = _MemoryDetails::CloneConstructObjectsFromLastElementImpl<
        T,
        _MemoryDetails::construct_clone_objects_clone_v<T>
    >::CloneConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    CloneFromLastElementResult<T> result;
    result.pDstObjectWhereCloneFailedOrEnd = cloneResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcObjectWhereCloneFailedOrEnd = cloneResult.pSrcFailedOrEndOrBeginMinusOne;
    result.isOk = cloneResult.pSrcFailedOrEndOrBeginMinusOne + 1 == pSrcBegin;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromFirstElementResult<T> MoveConstructObjectsFromFirstElementNonOverlap(
    T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveConstructObjectsFromFirstElementNonOverlapImpl<
        T
    >::MoveConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromFirstElementResult<T> result;
    result.pDstEnd = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcEnd = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromLastElementResult<T> MoveConstructObjectsFromLastElementNonOverlap(
    T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveConstructObjectsFromLastElementNonOverlapImpl<
        T
    >::MoveConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromLastElementResult<T> result;
    result.pDstBeginMinusOne = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcBeginMinusOne = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromFirstElementResult<T> MoveConstructObjectsFromFirstElement(
    T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin >= pDstBegin);

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveConstructObjectsFromFirstElementImpl<
        T
    >::MoveConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromFirstElementResult<T> result;
    result.pDstEnd = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcEnd = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromLastElementResult<T> MoveConstructObjectsFromLastElement(
    T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin <= pDstBegin);

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveConstructObjectsFromLastElementImpl<
        T
    >::MoveConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromLastElementResult<T> result;
    result.pDstBeginMinusOne = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcBeginMinusOne = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromFirstElementResult<T> MoveDestructibleConstructObjectsFromFirstElementNonOverlap(
    T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveDestructibleConstructObjectsFromFirstElementNonOverlapImpl<
        T
    >::MoveDestructibleConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromFirstElementResult<T> result;
    result.pDstEnd = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcEnd = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromLastElementResult<T> MoveDestructibleConstructObjectsFromLastElementNonOverlap(
    T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveDestructibleConstructObjectsFromLastElementNonOverlapImpl<
        T
    >::MoveDestructibleConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromLastElementResult<T> result;
    result.pDstBeginMinusOne = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcBeginMinusOne = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromFirstElementResult<T> MoveDestructibleConstructObjectsFromFirstElement(
    T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin >= pDstBegin);

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveDestructibleConstructObjectsFromFirstElementImpl<
        T
    >::MoveDestructibleConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromFirstElementResult<T> result;
    result.pDstEnd = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcEnd = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromLastElementResult<T> MoveDestructibleConstructObjectsFromLastElement(
    T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(pSrcBegin <= pDstBegin);

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MoveDestructibleConstructObjectsFromLastElementImpl<
        T
    >::MoveDestructibleConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromLastElementResult<T> result;
    result.pDstBeginMinusOne = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcBeginMinusOne = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE MoveFromFirstElementResult<T> MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
    T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
) noexcept {
    ALT_ASSERT(!IsOverlap(pSrcBegin, pSrcEnd, pDstBegin, pDstBegin + (pSrcEnd - pSrcBegin)));

    _MemoryDetails::MoveResult<T> moveResult = _MemoryDetails::MemcpyOrMoveDestructibleConstructObjectsNonOverlapImpl<
        T
    >::MoveDestructibleConstruct(pDstBegin, pSrcBegin, pSrcEnd);

    MoveFromFirstElementResult<T> result;
    result.pDstEnd = moveResult.pDstFailedOrEndOrBeginMinusOne;
    result.pSrcEnd = moveResult.pSrcFailedOrEndOrBeginMinusOne;

    return result;
}

template <typename T>
ALT_FORCE_INLINE T MoveDestructibleObject(T& obj) noexcept {

    T result = std::move(obj);
    DestroyObject(obj);

    return result;
}

namespace _MemoryDetails {

template <typename T>
struct MallocWrapResult {

    T* pMemory = nullptr;
    usize size = 0;
};

template <typename T>
struct MallocImpl<T, true> {

    ALT_NODISCARD ALT_FORCE_INLINE static T* Malloc(const MallocBits bits) noexcept {

        return reinterpret_cast<T*>(Platform::MemoryAllocateBytes(sizeof(T), static_cast<Platform::MemoryAllocateBytesBits>(bits)));
    }

    ALT_NODISCARD ALT_FORCE_INLINE static T* Malloc(usize cnt, const MallocBits bits) noexcept {

        if (_NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMul(cnt, sizeof(T), cnt)) {
            return nullptr;
        }

        return reinterpret_cast<T*>(Platform::MemoryAllocateBytes(cnt, static_cast<Platform::MemoryAllocateBytesBits>(bits)));
    }

    ALT_NODISCARD ALT_FORCE_INLINE static MallocWrapResult<T> MallocWrap(usize cnt, usize wrapCnt, const MallocBits bits) noexcept {

        const bool isOverflowWrap = _NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMulWrap(
            wrapCnt, sizeof(T), wrapCnt, Num::Desc<usize>::Max()
        );

        const bool isOverflow = _NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMulWrap(
            cnt, sizeof(T), cnt, wrapCnt
        );

        if (isOverflowWrap && isOverflow) {

            return MallocWrapResult<T>{};
        }

        MallocWrapResult<T> result{};
        result.pMemory = reinterpret_cast<T*>(Platform::MemoryAllocateBytes(cnt, static_cast<Platform::MemoryAllocateBytesBits>(bits)));
        result.size = cnt / sizeof(T);

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static T* Realloc(void* pMemoryOld, const usize cntOld, usize cntNew, const ReallocBits bits) noexcept {

        if (_NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMul(cntNew, sizeof(T), cntNew)) {
            return nullptr;
        }

        return ReallocImpl<T>::Realloc(pMemoryOld, sizeof(T) * cntOld, cntNew, bits);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static MallocWrapResult<T> ReallocWrap(void* pMemoryOld, const usize cntOld, usize cntNew, usize wrapCnt, const ReallocBits bits) noexcept {

        const bool isOverflowWrap = _NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMulWrap(
            wrapCnt, sizeof(T), wrapCnt, Num::Desc<usize>::Max()
        );

        const bool isOverflow = _NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMulWrap(
            cntNew, sizeof(T), cntNew, wrapCnt
        );

        if (isOverflowWrap && isOverflow) {

            return MallocWrapResult<T>{};
        }

        MallocWrapResult<T> result{};
        result.pMemory = ReallocImpl<T>::Realloc(pMemoryOld, sizeof(T) * cntOld, cntNew, bits);
        result.size = cntNew / sizeof(T);

        return result;
    }
};

template <typename T>
struct MallocImpl<T, false> {

    ALT_NODISCARD ALT_FORCE_INLINE static T* Malloc(const MallocBits bits) noexcept {

        return reinterpret_cast<T*>(Platform::MemoryAllocateBytes(sizeof(T), static_cast<Platform::MemoryAllocateBytesBits>(bits)));
    }

    ALT_NODISCARD ALT_FORCE_INLINE static T* Malloc(const usize cnt, const MallocBits bits) noexcept {

        return reinterpret_cast<T*>(Platform::MemoryAllocateBytes(sizeof(T) * cnt, static_cast<Platform::MemoryAllocateBytesBits>(bits)));
    }

    ALT_NODISCARD ALT_FORCE_INLINE static MallocWrapResult<T> MallocWrap(const usize cnt, const usize, const MallocBits bits) noexcept {

        MallocWrapResult<T> result{};
        result.pMemory = reinterpret_cast<T*>(Platform::MemoryAllocateBytes(sizeof(T) * cnt, static_cast<Platform::MemoryAllocateBytesBits>(bits)));
        result.size = cnt;

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static T* Realloc(void* pMemoryOld, const usize cntOld, const usize cntNew, const ReallocBits bits) noexcept {

        return ReallocImpl<T>::Realloc(pMemoryOld, sizeof(T) * cntOld, sizeof(T) * cntNew, bits);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static MallocWrapResult<T> ReallocWrap(void* pMemoryOld, const usize cntOld, const usize cntNew, const usize, const ReallocBits bits) noexcept {

        MallocWrapResult<T> result{};
        result.pMemory = ReallocImpl<T>::Realloc(pMemoryOld, sizeof(T) * cntOld, sizeof(T) * cntNew, bits);
        result.size = cntNew;

        return result;
    }
};

template <typename T>
struct ReallocImpl<T, true> {

    ALT_NODISCARD ALT_FORCE_INLINE static T* Realloc(
        void* pMemoryOld, const usize sizeInBytesOld, const usize sizeInBytesNew, ReallocBits bits
    ) noexcept {

        /*
         * This function doesn't support memory shrink case for objects which should be destroyed by destructor - we operate
         * only on raw memory bytes (moving on reallocation is exception - sometime we can't move objects only using 'memcpy',
         * see. 'is_trivially_reallocatable_v<T>')
         */
        ALT_ASSERT(sizeInBytesOld <= sizeInBytesNew || std::is_trivially_destructible_v<T>);

        if (BitFlag::IsSetEnumFlags(bits, ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace)) {

            bits &= ~ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace;
            u8* pMemory = Platform::MemoryReallocateBytes(pMemoryOld, sizeInBytesNew, static_cast<Platform::MemoryReallocateBytesBits>(bits));

            return reinterpret_cast<T*>(pMemory);
        }

        u8* pMemory = Platform::MemoryReallocateBytes(
            pMemoryOld,
            sizeInBytesNew,
            static_cast<Platform::MemoryReallocateBytesBits>(bits) | Platform::MemoryReallocateBytesBits::InPlace
        );

        if (pMemory) {
            return reinterpret_cast<T*>(pMemory);
        }

        if (BitFlag::IsSetEnumFlags(bits, ReallocBits::InPlace)) {
            return nullptr;
        }

        pMemory = Platform::MemoryAllocateBytes(sizeInBytesNew, static_cast<Platform::MemoryAllocateBytesBits>(bits));
        if (!pMemory) {
            return nullptr;
        }

        MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
            reinterpret_cast<T*>(pMemory),
            reinterpret_cast<T*>(pMemoryOld),
            reinterpret_cast<T*>(static_cast<u8*>(pMemoryOld) + sizeInBytesOld)
        );

        Platform::MemoryDeallocateBytes(pMemoryOld);

        return reinterpret_cast<T*>(pMemory);
    }
};

template <typename T>
struct ReallocImpl<T, false> {

    ALT_NODISCARD ALT_FORCE_INLINE static T* Realloc(
        void* pMemoryOld, const usize sizeInBytesOld, const usize sizeInBytesNew, const ReallocBits bits
    ) noexcept {

        /*
         * This function doesn't support memory shrink case for objects which should be destroyed by destructor - we operate
         * only on raw memory bytes (moving on reallocation is exception - sometime we can't move objects only using 'memcpy',
         * see. 'is_trivially_reallocatable_v<T>')
         */
        ALT_ASSERT(sizeInBytesOld <= sizeInBytesNew || std::is_trivially_destructible_v<T>);

        u8* pMemory = Platform::MemoryReallocateBytes(
            pMemoryOld,
            sizeInBytesNew,
            static_cast<Platform::MemoryReallocateBytesBits>(bits) | Platform::MemoryReallocateBytesBits::InPlace
        );

        if (pMemory) {
            return reinterpret_cast<T*>(pMemory);
        }

        if (BitFlag::IsSetEnumFlags(bits, ReallocBits::InPlace)) {
            return nullptr;
        }

        pMemory = Platform::MemoryAllocateBytes(sizeInBytesNew, static_cast<Platform::MemoryAllocateBytesBits>(bits));
        if (!pMemory) {
            return nullptr;
        }

        MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
            reinterpret_cast<T*>(pMemory),
            reinterpret_cast<T*>(pMemoryOld),
            reinterpret_cast<T*>(static_cast<u8*>(pMemoryOld) + sizeInBytesOld)
        );

        Platform::MemoryDeallocateBytes(pMemoryOld);

        return reinterpret_cast<T*>(pMemory);
    }
};

template <typename T>
struct AlignedMallocImpl<T, true> {

    ALT_NODISCARD ALT_FORCE_INLINE static T* AlignedMalloc(const MallocBits bits) noexcept {

        u8* pMemory = Platform::MemoryAlignedAllocateBytes(sizeof(T), alignof(T), static_cast<Platform::MemoryAllocateBytesBits>(bits));

        return reinterpret_cast<T*>(pMemory);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static T* AlignedMalloc(usize cnt, const MallocBits bits) noexcept {

        if (_NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMul(cnt, sizeof(T), cnt)) {
            return nullptr;
        }

        u8* pMemory = Platform::MemoryAlignedAllocateBytes(cnt, alignof(T), static_cast<Platform::MemoryAllocateBytesBits>(bits));

        return reinterpret_cast<T*>(pMemory);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static MallocWrapResult<T> AlignedMallocWrap(usize cnt, usize wrapCnt, const MallocBits bits) noexcept {

        const bool isOverflowWrap = _NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMulWrap(
            wrapCnt, sizeof(T), wrapCnt, Num::Desc<usize>::Max()
        );

        const bool isOverflow = _NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMulWrap(
            cnt, sizeof(T), cnt, wrapCnt
        );

        if (isOverflowWrap && isOverflow) {

            return MallocWrapResult<T>{};
        }

        u8* pMemory = Platform::MemoryAlignedAllocateBytes(cnt, alignof(T), static_cast<Platform::MemoryAllocateBytesBits>(bits));

        MallocWrapResult<T> result{};
        result.pMemory = reinterpret_cast<T*>(pMemory);
        result.size = cnt / sizeof(T);

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static T* AlignedRealloc(
        void* pMemoryOld, const usize cntOld, usize cntNew, const ReallocBits bits
    ) noexcept {

        if (_NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMul(cntNew, sizeof(T), cntNew)) {
            return nullptr;
        }

        return AlignedReallocImpl<T>::Realloc(pMemoryOld, sizeof(T) * cntOld, cntNew, bits);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static MallocWrapResult<T> AlignedReallocWrap(
        void* pMemoryOld, const usize cntOld, usize cntNew, usize wrapCnt, const ReallocBits bits
    ) noexcept {

        const bool isOverflowWrap = _NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMulWrap(
            wrapCnt, sizeof(T), wrapCnt, Num::Desc<usize>::Max()
        );

        const bool isOverflow = _NumDetails::IsOverflowCheckMulImpl<usize>::IsOverflowCheckMulWrap(
            cntNew, sizeof(T), cntNew, wrapCnt
        );

        if (isOverflowWrap && isOverflow) {

            return MallocWrapResult<T>{};
        }

        MallocWrapResult<T> result{};
        result.pMemory = AlignedReallocImpl<T>::Realloc(pMemoryOld, sizeof(T) * cntOld, cntNew, bits);
        result.size = cntNew / sizeof(T);

        return result;
    }
};

template <typename T>
struct AlignedMallocImpl<T, false> {

    ALT_NODISCARD ALT_FORCE_INLINE static T* AlignedMalloc(const MallocBits bits) noexcept {

        u8* pMemory = Platform::MemoryAlignedAllocateBytes(sizeof(T), alignof(T), static_cast<Platform::MemoryAllocateBytesBits>(bits));

        return reinterpret_cast<T*>(pMemory);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static T* AlignedMalloc(const usize cnt, const MallocBits bits) noexcept {

        u8* pMemory = Platform::MemoryAlignedAllocateBytes(sizeof(T) * cnt, alignof(T), static_cast<Platform::MemoryAllocateBytesBits>(bits));

        return reinterpret_cast<T*>(pMemory);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static MallocWrapResult<T> AlignedMallocWrap(usize cnt, const usize, const MallocBits bits) noexcept {

        u8* pMemory = Platform::MemoryAlignedAllocateBytes(sizeof(T) * cnt, alignof(T), static_cast<Platform::MemoryAllocateBytesBits>(bits));

        MallocWrapResult<T> result{};
        result.pMemory = reinterpret_cast<T*>(pMemory);
        result.size = cnt;

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static T* AlignedRealloc(
        void* pMemoryOld, const usize cntOld, const usize cntNew, const ReallocBits bits
    ) noexcept {

        return AlignedReallocImpl<T>::Realloc(pMemoryOld, sizeof(T) * cntOld, sizeof(T) * cntNew, bits);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static MallocWrapResult<T> AlignedReallocWrap(
        void* pMemoryOld, const usize cntOld, const usize cntNew, const usize, const ReallocBits bits
    ) noexcept {

        MallocWrapResult<T> result{};
        result.pMemory = AlignedReallocImpl<T>::Realloc(pMemoryOld, sizeof(T) * cntOld, sizeof(T) * cntNew, bits);
        result.size = cntNew;

        return result;
    }
};

template <typename T, bool IS_TRIVIALLY_REALLOCATABLE>
struct AlignedReallocImpl {

    ALT_NODISCARD ALT_FORCE_INLINE static T* Realloc(
        void* pMemoryOld, const usize sizeInBytesOld, const usize sizeInBytesNew, const ReallocBits bits
    ) noexcept {

        /*
         * This function doesn't support memory shrink case for objects which should be destroyed by destructor - we operate
         * only on raw memory bytes (moving on reallocation is exception - sometime we can't move objects only using 'memcpy',
         * see. 'is_trivially_reallocatable_v<T>')
         */
        ALT_ASSERT(sizeInBytesOld <= sizeInBytesNew || std::is_trivially_destructible_v<T>);

        u8* pMemory = Platform::MemoryReallocateBytes(
            pMemoryOld,
            sizeInBytesNew,
            static_cast<Platform::MemoryReallocateBytesBits>(bits) | Platform::MemoryReallocateBytesBits::InPlace
        );

        if (pMemory) {
            return reinterpret_cast<T*>(pMemory);
        }

        if (BitFlag::IsSetEnumFlags(bits, ReallocBits::InPlace)) {
            return nullptr;
        }

        pMemory = Platform::MemoryAlignedAllocateBytes(
            sizeInBytesNew,
            alignof(T),
            static_cast<Platform::MemoryAllocateBytesBits>(bits)
        );

        if (!pMemory) {
            return nullptr;
        }

        MemcpyOrMoveDestructibleConstructObjectsNonOverlap(
            reinterpret_cast<T*>(pMemory),
            reinterpret_cast<T*>(pMemoryOld),
            reinterpret_cast<T*>(static_cast<u8*>(pMemoryOld) + sizeInBytesOld)
        );

        Platform::MemoryAlignedDeallocateBytes(pMemoryOld);

        return reinterpret_cast<T*>(pMemory);
    }
};

template <typename T>
struct DestroyObjectImpl<T, true> {

    ALT_FORCE_INLINE static void Destroy(T&) noexcept {}
};

template <typename T>
struct DestroyObjectImpl<T, false> {
public:

    ALT_FORCE_INLINE static void Destroy(T& obj) noexcept {

        DestroyImpl(obj);
    }

private:

    template <typename U>
    ALT_FORCE_INLINE static void DestroyImpl(U& obj) noexcept {
        obj.~U();
    }
};

template <typename T>
struct DestroyObjectsImpl<T, true> {

    ALT_FORCE_INLINE static void Destroy(T* __restrict, T* __restrict) noexcept {}
};

template <typename T>
struct DestroyObjectsImpl<T, false> {

    ALT_FORCE_INLINE static void Destroy(T* __restrict pBegin, T* __restrict pEnd) noexcept {

        for (T* ptr = pBegin; ptr != pEnd; ++ptr) {

            DestroyObject(*ptr);
        }
    }
};

template <typename T>
struct FillConstructDefaultObjectsImpl<T, FillDefaultTrait::Memset> {

    ALT_FORCE_INLINE static void FillConstructDefault(T* __restrict pBegin, T* __restrict pEnd) noexcept {

        std::memset(pBegin, 0, (pEnd - pBegin) * sizeof(T));
    }
};

template <typename T>
struct FillConstructDefaultObjectsImpl<T, FillDefaultTrait::DefaultByElement> {

    ALT_FORCE_INLINE static void FillConstructDefault(T* __restrict pBegin, T* __restrict pEnd) noexcept {

        for (T* ptr = pBegin; ptr != pEnd; ++ptr) {

            ConstructDefaultObject(ptr);
        }
    }
};

template <typename T>
struct FillCloneConstructObjectsFromFirstElementImpl<T, FillTrait::Memset> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillCloneConstruct(T* __restrict pBegin, T* __restrict pEnd, const U& val_) noexcept {

        const s32 val = static_cast<s32>(val_);
        std::memset(pBegin, val, (pEnd - pBegin) * sizeof(T));

        return pEnd;
    }
};

template <typename T>
struct FillCloneConstructObjectsFromFirstElementImpl<T, FillTrait::CppCopy> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillCloneConstruct(T* __restrict pBegin, T* __restrict pEnd, const U& val) noexcept {

        for (T* ptr = pBegin; ptr != pEnd; ++ptr) {

            ConstructObject<T>(ptr, val);
        }

        return pEnd;
    }
};

template <typename T>
struct FillCloneConstructObjectsFromFirstElementImpl<T, FillTrait::AltClone> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillCloneConstruct(T* __restrict pBegin, T* __restrict pEnd, const U& val) noexcept {

        T* ptr = pBegin;

        for (; ptr != pEnd; ++ptr) {

            if (!ptr->CloneConstructFrom(val)) {

                return ptr;
            }
        }

        return pEnd;
    }
};

template <typename T>
struct FillCloneConstructObjectsFromLastElementImpl<T, FillTrait::Memset> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillCloneConstruct(T* __restrict pBegin, T* __restrict pEnd, const U& val_) noexcept {

        const s32 val = static_cast<s32>(val_);
        std::memset(pBegin, val, (pEnd - pBegin) * sizeof(T));

        return pBegin - 1;
    }
};

template <typename T>
struct FillCloneConstructObjectsFromLastElementImpl<T, FillTrait::CppCopy> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillCloneConstruct(T* __restrict pBegin, T* __restrict pEnd, const U& val) noexcept {

        for (T* ptr = pBegin; ptr != pEnd; ++ptr) {

            ConstructObject<T>(ptr, val);
        }

        return pBegin - 1;
    }
};

template <typename T>
struct FillCloneConstructObjectsFromLastElementImpl<T, FillTrait::AltClone> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillCloneConstruct(T* __restrict pBegin, T* __restrict pEnd, const U& val) noexcept {

        T* ptr = pEnd;

        while (ptr != pBegin) {

            ptr -= 1;

            if (!ptr->CloneConstructFrom(val)) {

                return ptr;
            }
        }

        return pBegin - 1;
    }
};

template <typename T>
struct FillCloneObjectsFromFirstElementImpl<T, FillTrait::Memset> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillClone(T* __restrict pBegin, T* __restrict pEnd, const U& val_) noexcept {

        const s32 val = static_cast<s32>(val_);
        std::memset(pBegin, val, (pEnd - pBegin) * sizeof(T));

        return pEnd;
    }
};

template <typename T>
struct FillCloneObjectsFromFirstElementImpl<T, FillTrait::CppCopy> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillClone(T* __restrict pBegin, T* __restrict pEnd, const U& val) noexcept {

        T* ptr = pBegin;
        for (; ptr != pEnd; ++ptr) {

            *ptr = val;
        }

        return pEnd;
    }
};

template <typename T>
struct FillCloneObjectsFromFirstElementImpl<T, FillTrait::AltClone> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillClone(T* __restrict pBegin, T* __restrict pEnd, const U& val) noexcept {

        T* ptr = pBegin;
        for (; ptr != pEnd; ++ptr) {

            if (!ptr->CloneFrom(val)) {

                return ptr;
            }
        }

        return pEnd;
    }
};

template <typename T>
struct FillCloneObjectsFromLastElementImpl<T, FillTrait::Memset> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillClone(T* __restrict pBegin, T* __restrict pEnd, const U& val_) noexcept {

        const s32 val = static_cast<s32>(val_);
        std::memset(pBegin, val, (pEnd - pBegin) * sizeof(T));

        return pBegin - 1;
    }
};

template <typename T>
struct FillCloneObjectsFromLastElementImpl<T, FillTrait::CppCopy> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillClone(T* __restrict pBegin, T* __restrict pEnd, const U& val) noexcept {

        T* ptr = pBegin;
        for (; ptr != pEnd; ++ptr) {

            *ptr = val;
        }

        return pBegin - 1;
    }
};

template <typename T>
struct FillCloneObjectsFromLastElementImpl<T, FillTrait::AltClone> {

    template <typename U>
    ALT_NODISCARD ALT_FORCE_INLINE static T* FillClone(T* __restrict pBegin, T* __restrict pEnd, const U& val) noexcept {

        T* ptr = pEnd;
        while (ptr != pBegin) {

            ptr -= 1;

            if (!ptr->CloneFrom(val)) {

                return ptr;
            }
        }

        return pBegin - 1;
    }
};

template <typename T>
struct FillDefaultObjectsImpl<T, FillDefaultTrait::Memset> {

    ALT_FORCE_INLINE static void FillDefault(T* __restrict pBegin, T* __restrict pEnd) noexcept {

        std::memset(pBegin, 0, (pEnd - pBegin) * sizeof(T));
    }
};

template <typename T>
struct FillDefaultObjectsImpl<T, FillDefaultTrait::DefaultByElement> {

    ALT_FORCE_INLINE static void FillDefault(T* __restrict pBegin, T* __restrict pEnd) noexcept {

        for (T* ptr = pBegin; ptr != pEnd; ++ptr) {

            *ptr = T{};
        }
    }
};

template <typename T>
struct CloneObjectsFromFirstElementNonOverlapImpl<T, CopyTrait::MemcpyOrMemmove> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromFirstElementNonOverlapImpl<T, CopyTrait::CppCopy> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            *dst = *src;
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromFirstElementNonOverlapImpl<T, CopyTrait::AltClone> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            if (!dst->CloneFrom(*src)) {

                CloneResult<T> result;
                result.pDstFailedOrEndOrBeginMinusOne = dst;
                result.pSrcFailedOrEndOrBeginMinusOne = src;

                return result;
            }
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromLastElementNonOverlapImpl<T, CopyTrait::MemcpyOrMemmove> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromLastElementNonOverlapImpl<T, CopyTrait::CppCopy> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            *dst = *src;
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromLastElementNonOverlapImpl<T, CopyTrait::AltClone> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        const T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            if (!dst->CloneFrom(*src)) {

                CloneResult<T> result;
                result.pDstFailedOrEndOrBeginMinusOne = dst;
                result.pSrcFailedOrEndOrBeginMinusOne = src;

                return result;
            }
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromFirstElementImpl<T, CopyTrait::MemcpyOrMemmove> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromFirstElementImpl<T, CopyTrait::CppCopy> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            *dst = *src;
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromFirstElementImpl<T, CopyTrait::AltClone> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            if (!dst->CloneFrom(*src)) {

                CloneResult<T> result;
                result.pDstFailedOrEndOrBeginMinusOne = dst;
                result.pSrcFailedOrEndOrBeginMinusOne = src;

                return result;
            }
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromLastElementImpl<T, CopyTrait::MemcpyOrMemmove> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromLastElementImpl<T, CopyTrait::CppCopy> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        const T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            *dst = *src;
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct CloneObjectsFromLastElementImpl<T, CopyTrait::AltClone> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> Clone(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        const T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            if (!dst->CloneFrom(*src)) {

                CloneResult<T> result;
                result.pDstFailedOrEndOrBeginMinusOne = dst;
                result.pSrcFailedOrEndOrBeginMinusOne = src;

                return result;
            }
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct MoveObjectsFromFirstElementNonOverlapImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveObjectsFromFirstElementNonOverlapImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            *dst = std::move(*src);
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct MoveObjectsFromLastElementNonOverlapImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveObjectsFromLastElementNonOverlapImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            *dst = std::move(*src);
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveObjectsFromFirstElementImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveObjectsFromFirstElementImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            *dst = std::move(*src);
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct MoveObjectsFromLastElementImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveObjectsFromLastElementImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            *dst = std::move(*src);
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct MoveDestructibleObjectsFromFirstElementNonOverlapImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveDestructibleObjectsFromFirstElementNonOverlapImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            *dst = MoveDestructibleObject(*src);
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct MoveDestructibleObjectsFromLastElementNonOverlapImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveDestructibleObjectsFromLastElementNonOverlapImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            *dst = MoveDestructibleObject(*src);
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveDestructibleObjectsFromFirstElementImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveDestructibleObjectsFromFirstElementImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            *dst = MoveDestructibleObject(*src);
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct MoveDestructibleObjectsFromLastElementImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveDestructibleObjectsFromLastElementImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> Move(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            *dst = MoveDestructibleObject(*src);
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromFirstElementNonOverlapImpl<T, CopyTrait::MemcpyOrMemmove> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromFirstElementNonOverlapImpl<T, CopyTrait::CppCopy> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            ConstructObject(dst, *src);
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromFirstElementNonOverlapImpl<T, CopyTrait::AltClone> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            if (!dst->CloneConstructFrom(*src)) {

                CloneResult<T> result;
                result.pDstFailedOrEndOrBeginMinusOne = dst;
                result.pSrcFailedOrEndOrBeginMinusOne = src;

                return result;
            }
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromLastElementNonOverlapImpl<T, CopyTrait::MemcpyOrMemmove> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromLastElementNonOverlapImpl<T, CopyTrait::CppCopy> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            ConstructObject(dst, *src);
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromLastElementNonOverlapImpl<T, CopyTrait::AltClone> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* __restrict pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        const T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            if (!dst->CloneConstructFrom(*src)) {

                CloneResult<T> result;
                result.pDstFailedOrEndOrBeginMinusOne = dst;
                result.pSrcFailedOrEndOrBeginMinusOne = src;

                return result;
            }
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromFirstElementImpl<T, CopyTrait::MemcpyOrMemmove> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromFirstElementImpl<T, CopyTrait::CppCopy> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            ConstructObject(dst, *src);
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromFirstElementImpl<T, CopyTrait::AltClone> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            if (!dst->CloneConstructFrom(*src)) {

                CloneResult<T> result;
                result.pDstFailedOrEndOrBeginMinusOne = dst;
                result.pSrcFailedOrEndOrBeginMinusOne = src;

                return result;
            }
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromLastElementImpl<T, CopyTrait::MemcpyOrMemmove> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, (pSrcEnd - pSrcBegin) * sizeof(T));

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromLastElementImpl<T, CopyTrait::CppCopy> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        const T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            ConstructObject(dst, *src);
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct CloneConstructObjectsFromLastElementImpl<T, CopyTrait::AltClone> {

    ALT_NODISCARD ALT_FORCE_INLINE static CloneResult<T> CloneConstruct(
        T* pDstBegin, const T* __restrict pSrcBegin, const T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        const T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            if (!dst->CloneConstructFrom(*src)) {

                CloneResult<T> result;
                result.pDstFailedOrEndOrBeginMinusOne = dst;
                result.pSrcFailedOrEndOrBeginMinusOne = src;

                return result;
            }
        }

        CloneResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct MoveConstructObjectsFromFirstElementNonOverlapImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> MoveConstruct(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveConstructObjectsFromFirstElementNonOverlapImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> MoveConstruct(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            ConstructObject(dst, std::move(*src));
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveConstructObjectsFromLastElementNonOverlapImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> MoveConstruct(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveConstructObjectsFromLastElementNonOverlapImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> MoveConstruct(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            ConstructObject(dst, std::move(*src));
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveConstructObjectsFromFirstElementImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> MoveConstruct(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveConstructObjectsFromFirstElementImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> MoveConstruct(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            ConstructObject(dst, std::move(*src));
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct MoveConstructObjectsFromLastElementImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> MoveConstruct(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveConstructObjectsFromLastElementImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> MoveConstruct(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            ConstructObject(dst, std::move(*src));
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct MoveDestructibleConstructObjectsFromFirstElementNonOverlapImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(
        T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveDestructibleConstructObjectsFromFirstElementNonOverlapImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(
        T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
    ) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            ConstructObject(dst, MoveDestructibleObject(*src));
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveDestructibleConstructObjectsFromLastElementNonOverlapImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(
        T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
    ) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveDestructibleConstructObjectsFromLastElementNonOverlapImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(
        T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd
    ) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            ConstructObject(dst, MoveDestructibleObject(*src));
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveDestructibleConstructObjectsFromFirstElementImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MoveDestructibleConstructObjectsFromFirstElementImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        T* src = pSrcBegin;
        T* dst = pDstBegin;

        for (; src != pSrcEnd; ++src, ++dst) {

            ConstructObject(dst, MoveDestructibleObject(*src));
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst;
        result.pSrcFailedOrEndOrBeginMinusOne = src;

        return result;
    }
};

template <typename T>
struct MoveDestructibleConstructObjectsFromLastElementImpl<T, MoveTrait::MemcpyOrMemmove> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memmove(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcBegin - 1;

        return result;
    }
};

template <typename T>
struct MoveDestructibleConstructObjectsFromLastElementImpl<T, MoveTrait::MoveByElements> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(T* pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        T* src = pSrcEnd;
        T* dst = pDstBegin + size;

        while (src != pSrcBegin) {

            src -= 1;
            dst -= 1;

            ConstructObject(dst, MoveDestructibleObject(*src));
        }

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = dst - 1;
        result.pSrcFailedOrEndOrBeginMinusOne = src - 1;

        return result;
    }
};

template <typename T>
struct MemcpyOrMoveDestructibleConstructObjectsNonOverlapImpl<T, true> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        const usize size = static_cast<usize>(pSrcEnd - pSrcBegin);

        std::memcpy(pDstBegin, pSrcBegin, size * sizeof(T));

        MoveResult<T> result;
        result.pDstFailedOrEndOrBeginMinusOne = pDstBegin + size;
        result.pSrcFailedOrEndOrBeginMinusOne = pSrcEnd;

        return result;
    }
};

template <typename T>
struct MemcpyOrMoveDestructibleConstructObjectsNonOverlapImpl<T, false> {

    ALT_FORCE_INLINE static MoveResult<T> MoveDestructibleConstruct(T* __restrict pDstBegin, T* __restrict pSrcBegin, T* __restrict pSrcEnd) noexcept {

        return MoveDestructibleConstructObjectsFromFirstElementNonOverlapImpl<T>::MoveDestructibleConstruct(pDstBegin, pSrcBegin, pSrcEnd);
    }
};

} // namespace _MemoryDetails

} // namespace Memory
} // namespace Alt

#endif // __ALT_STD_MEMORY_PRE_INCLUDE__