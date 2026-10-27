#pragma once

#ifndef __ALT_STD_ALGORITHMS__
#define __ALT_STD_ALGORITHMS__

#include "../TypeTraits.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "Impl/xxHashConstexpr-0.8.3.h"
#include "Impl/blqs.h"

namespace Alt {

#define ALT_HASH_CONST(strSlice_) Alt::Algorithms::HashConstexpr(strSlice_.Data(), strSlice_.GetSize());
#define ALT_HASH_SEED_CONST(strSlice_, seed) Alt::Algorithms::HashConstexpr(strSlice_.Data(), strSlice_.GetSize(), seed);

#define ALT_HASH32_CONST(strSlice_) Alt::Algorithms::Hash32Constexpr(strSlice_.Data(), strSlice_.GetSize());
#define ALT_HASH32_SEED_CONST(strSlice_, seed) Alt::Algorithms::Hash32Constexpr(strSlice_.Data(), strSlice_.GetSize(), seed);

#define ALT_HASH64_CONST(strSlice_) Alt::Algorithms::Hash64Constexpr(strSlice_.Data(), strSlice_.GetSize());
#define ALT_HASH64_SEED_CONST(strSlice_, seed) Alt::Algorithms::Hash64Constexpr(strSlice_.Data(), strSlice_.GetSize(), seed);

namespace Algorithms {
namespace _AlgorithmsDetails {

enum class PlatformTrait {

    x32,
    x64,
};

template <PlatformTrait TRAIT>
using Platform = std::integral_constant<PlatformTrait, TRAIT>;

template <
    typename T = void,
    PlatformTrait PLATFORM =
        std::conditional_t<
            sizeof(usize) == sizeof(u64),
            Platform<PlatformTrait::x64>,
            Platform<PlatformTrait::x32>
        >::value
>
struct HashImpl;

template <
    typename T = void,
    PlatformTrait PLATFORM =
        std::conditional_t<
            sizeof(usize) == sizeof(u64),
            Platform<PlatformTrait::x64>,
            Platform<PlatformTrait::x32>
        >::value
>
struct HashConstexprImpl;

template <typename T>
struct is_allow_memcmp_comparison
    : std::conditional_t<

        std::has_unique_object_representations_v<T> &&
        std::is_scalar_v<T> &&
        std::is_unsigned_v<T> &&
        sizeof(T) == sizeof(u8),

        std::true_type,
        std::false_type
    >
{};

template <typename T>
constexpr static bool is_allow_memcmp_comparison_v = is_allow_memcmp_comparison<TypeTraits::clean_t<T>>::value;

template <typename T>
struct is_allow_memcmp_comparison_eq
    : std::conditional_t<

        std::has_unique_object_representations_v<T> &&
        std::is_scalar_v<T>,

        std::true_type,
        std::false_type
    >
{};

template <typename T>
constexpr static bool is_allow_memcmp_comparison_eq_v = is_allow_memcmp_comparison_eq<TypeTraits::clean_t<T>>::value;

template <typename T>
struct is_allow_wmemcmp_comparison
    : std::conditional_t<

        std::has_unique_object_representations_v<T> &&
        std::is_scalar_v<T> &&
        std::is_unsigned_v<T> &&
        sizeof(T) == sizeof(wchar_t),

        std::true_type,
        std::false_type
    >
{};

template <typename T>
constexpr static bool is_allow_wmemcmp_comparison_v = is_allow_wmemcmp_comparison<TypeTraits::clean_t<T>>::value;

enum class ComparisonTrait {

    Memcmp,
    Wmemcmp,
    Operator,
};

template <ComparisonTrait TRAIT>
using Comparison = std::integral_constant<ComparisonTrait, TRAIT>;

template <
    typename T,
    ComparisonTrait TRAIT =
        std::conditional_t<

            is_allow_memcmp_comparison_v<T>,
            Comparison<ComparisonTrait::Memcmp>,

            std::conditional_t<

                is_allow_wmemcmp_comparison_v<T>,
                Comparison<ComparisonTrait::Wmemcmp>,

                Comparison<ComparisonTrait::Operator>
            >
        >::value
>
struct CompareImpl;

template <
    typename T,
    ComparisonTrait TRAIT =
        std::conditional_t<
            is_allow_memcmp_comparison_eq_v<T>,
            Comparison<ComparisonTrait::Memcmp>,
            Comparison<ComparisonTrait::Operator>
        >::value
>
struct CompareEQImpl;

enum class FindTrait {

    Memchr,
    Wmemchr,
    Operator,
};

template <FindTrait TRAIT>
using Find = std::integral_constant<FindTrait, TRAIT>;

template <
    typename T,
    FindTrait TRAIT = std::conditional_t<

        sizeof(T) == sizeof(u8),
        Find<FindTrait::Memchr>,

        std::conditional_t<

            sizeof(T) == sizeof(wchar_t),
            Find<FindTrait::Wmemchr>,

            Find<FindTrait::Operator>
        >
    >::value
>
struct FindFirstTraitImpl;

} // namespace _AlgorithmsDetails

template <typename T = void>
ALT_NODISCARD ALT_FORCE_INLINE usize Hash(const void* pBegin, const void* pEnd, const usize seed = 0) noexcept {

    return _AlgorithmsDetails::HashImpl<T>::Hash(pBegin, pEnd, seed);
}

template <typename T = void>
ALT_NODISCARD ALT_FORCE_INLINE u32 Hash32(const void* pBegin, const void* pEnd, const u32 seed = 0) noexcept {

    return _AlgorithmsDetails::HashImpl<T, _AlgorithmsDetails::PlatformTrait::x32>::Hash(pBegin, pEnd, seed);
}

template <typename T = void>
ALT_NODISCARD ALT_FORCE_INLINE u64 Hash64(const void* pBegin, const void* pEnd, const u64 seed = 0) noexcept {

    return _AlgorithmsDetails::HashImpl<T, _AlgorithmsDetails::PlatformTrait::x64>::Hash(pBegin, pEnd, seed);
}

template <typename T = void,
    std::enable_if_t<sizeof(T) == 1>* = nullptr>
ALT_NODISCARD ALT_NOINLINE constexpr usize HashConstexpr(const T* pBegin, const usize size, const usize seed = 0) noexcept {

    return _AlgorithmsDetails::HashConstexprImpl<T>::Hash(pBegin, size, seed);
}

template <typename T = void, usize N,
    std::enable_if_t<sizeof(T) == 1>* = nullptr>
ALT_NODISCARD ALT_NOINLINE constexpr usize HashConstexpr(const T (&str)[N], const usize seed = 0) noexcept {

    return _AlgorithmsDetails::HashConstexprImpl<T>::Hash(str, N, seed);
}

template <typename T = void,
    std::enable_if_t<sizeof(T) == 1>* = nullptr>
ALT_NODISCARD ALT_NOINLINE constexpr u32 Hash32Constexpr(const T* pBegin, const usize size, const u32 seed = 0) noexcept {

    return _AlgorithmsDetails::HashConstexprImpl<T, _AlgorithmsDetails::PlatformTrait::x32>::Hash(pBegin, size, seed);
}

template <typename T = void, usize N,
    std::enable_if_t<sizeof(T) == 1>* = nullptr>
ALT_NODISCARD ALT_NOINLINE constexpr u32 Hash32Constexpr(const T (&str)[N], const u32 seed = 0) noexcept {

    return _AlgorithmsDetails::HashConstexprImpl<T, _AlgorithmsDetails::PlatformTrait::x32>::Hash(str, N, seed);
}

template <typename T = void,
    std::enable_if_t<sizeof(T) == 1>* = nullptr>
ALT_NODISCARD ALT_NOINLINE constexpr u64 Hash64Constexpr(const T* pBegin, const usize size, const u64 seed = 0) noexcept {

    return _AlgorithmsDetails::HashConstexprImpl<T, _AlgorithmsDetails::PlatformTrait::x64>::Hash(pBegin, size, seed);
}

template <typename T = void, usize N,
    std::enable_if_t<sizeof(T) == 1>* = nullptr>
ALT_NODISCARD ALT_NOINLINE constexpr u64 Hash64Constexpr(const T (&str)[N], const u64 seed = 0) noexcept {

    return _AlgorithmsDetails::HashConstexprImpl<T, _AlgorithmsDetails::PlatformTrait::x64>::Hash(str, N, seed);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE ssize Compare(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

    return _AlgorithmsDetails::CompareImpl<T>::Compare(pBegin0, pEnd0, pBegin1, pEnd1);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE bool CompareEQ(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

    return _AlgorithmsDetails::CompareEQImpl<T>::CompareEQ(pBegin0, pEnd0, pBegin1, pEnd1);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE bool CompareNE(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

    return !CompareEQ(pBegin0, pEnd0, pBegin1, pEnd1);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE bool CompareLT(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

    return Compare(pBegin0, pEnd0, pBegin1, pEnd1) == -1;
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE bool CompareGT(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

    return CompareLT(pBegin1, pEnd1, pBegin0, pEnd0);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE bool CompareLE(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

    return !CompareGT(pBegin0, pEnd0, pBegin1, pEnd1);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE bool CompareGE(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

    return !CompareLT(pBegin0, pEnd0, pBegin1, pEnd1);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T Remove(ALT_MAYBE_UNUSED T* pBegin, T* pEnd, T* pValToRemove) noexcept {
    ALT_ASSERT(pValToRemove >= pBegin && pValToRemove < pEnd);

    if (pValToRemove + 1 == pEnd) {

        return Memory::MoveDestructibleObject(*pValToRemove);
    }

    if (TypeTraits::is_trivially_reallocatable_v<T>) {

        alignas(T) u8 memory[sizeof(T)];
        std::memcpy(memory, pValToRemove, sizeof(T));
        std::memmove(pValToRemove, pValToRemove + 1, (pEnd - (pValToRemove + 1)) * sizeof(T));

        std::memcpy(pEnd - 1, memory, sizeof(T));
        return Memory::MoveDestructibleObject(*(pEnd - 1));
    }
    else {

        T valToRemove = std::move(*pValToRemove);
        Memory::MoveObjectsFromFirstElement(pValToRemove, pValToRemove + 1, pEnd);

        Memory::DestroyObject(*(pEnd - 1));
        return valToRemove;
    }
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T RemoveShiftRight(T* pBegin, ALT_MAYBE_UNUSED T* pEnd, T* pValToRemove) noexcept {
    ALT_ASSERT(pValToRemove >= pBegin && pValToRemove < pEnd);

    if (pValToRemove == pBegin) {

        return Memory::MoveDestructibleObject(*pValToRemove);
    }

    if (TypeTraits::is_trivially_reallocatable_v<T>) {

        alignas(T) u8 memory[sizeof(T)];
        std::memcpy(memory, pValToRemove, sizeof(T));
        std::memmove(pBegin + 1, pBegin, (pValToRemove - pBegin) * sizeof(T));

        std::memcpy(pBegin, memory, sizeof(T));
        return Memory::MoveDestructibleObject(*pBegin);
    }
    else {

        T valToRemove = std::move(*pValToRemove);
        Memory::MoveObjectsFromLastElement(pBegin + 1, pBegin, pValToRemove);

        Memory::DestroyObject(*pBegin);
        return valToRemove;
    }
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE T RemoveSwapLast(ALT_MAYBE_UNUSED T* pBegin, T* pEnd, T* pValToRemove) noexcept {
    ALT_ASSERT(pValToRemove >= pBegin && pValToRemove < pEnd);

    T* pLast = pEnd - 1;
    if (pValToRemove != pLast) {

        Swap(*pValToRemove, *pLast);
    }

    return Memory::MoveDestructibleObject(*pLast);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE const T* FindFirst(const T* pBegin, const T* pEnd, const T& val) noexcept {

    return _AlgorithmsDetails::FindFirstTraitImpl<T>::Find(pBegin, pEnd, val);
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE const T* FindLast(const T* pBegin, const T* pEnd, const T& val) noexcept {

    bool isEq = false;
    const T* pVal = pEnd;

    while (!isEq && pVal != pBegin) {

        pVal -= 1;
        isEq = *pVal == val;
    }

    return isEq ? pVal : pEnd;
}

#ifdef ALT_COMPILER_MSVC
#include <xmmintrin.h>
#define __ALT_STD_INTERNAL_PREFETCH__(ptr) _mm_prefetch(reinterpret_cast<const char*>(ptr), _MM_HINT_T0) 
#else
#define __ALT_STD_INTERNAL_PREFETCH__(ptr) __builtin_prefetch((ptr), 0, 3)
#endif

#ifndef ALT_COMPILER_CLANG
// not a CLANG/LLVM compiler
#elif !(defined(i386) || defined(__386) || defined(__i386) ||                  \
        defined(__i386__) || defined(i486) || defined(__i486) ||               \
        defined(__i486__) || defined(i586) | defined(__i586) ||                \
        defined(__i586__) || defined(i686) || defined(__i686) ||               \
        defined(__i686__) || defined(_M_IX86) || defined(_X86_) ||             \
        defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__) ||    \
        defined(__x86_64) || defined(__x86_64__) || defined(__amd64__) ||      \
        defined(__amd64) || defined(_M_X64) || defined(_M_AMD64) ||            \
        defined(__IA32__) || defined(__INTEL__))
// not a x86/IA32 target
#elif __clang_major__ < 5
// none needed for CLANG < 5
#else
// __asm() workaround
#define __ALT_STD_INTERNAL_CLANG_X86CMOV_WORKAROUND__(size, flag)                  \
  do                                                                           \
    __asm __volatile(""                                                        \
                     : "+r"(size)                                              \
                     : "r" /* the `b` constraint is more suitable here, but    \
                              cause CLANG to allocate and push/pop an one more \
                              register, so using the `r` which avoids this. */ \
                     (flag));                                                  \
  while (0)
#endif

#ifndef __ALT_STD_INTERNAL_CLANG_X86CMOV_WORKAROUND__
#define __ALT_STD_INTERNAL_CLANG_X86CMOV_WORKAROUND__(size, flag)                  \
  do                                                                           \
    /* nope for non-clang or non-x86 */;                                       \
  while (0)
#endif /* __ALT_STD_INTERNAL_CLANG_X86CMOV_WORKAROUND__ */

/*
 * @brief A binary search algorithm.
 * @return Returns pointer to actual value or 'pEnd' for next match for upper bound case.
 * @example
 * 
 * // lower bound
 * const T* pResult = Alt::Algorithms::BinarySearch(pBegin, pEnd, [&x](const T& y) {
 *     return x > y;
 * });
 *
 * // upper bound
 * const T* pResult = Alt::Algorithms::BinarySearch(pBegin, pEnd, [&x](const T& y) {
 *     return x >= y;
 * });
 */
template <typename T, typename FuncT>
ALT_NODISCARD ALT_FORCE_INLINE T* BinarySearch(T* pBegin, T* pEnd, FuncT&& cmp) noexcept {

    T* pVal = pBegin;
    usize len = static_cast<usize>(pEnd - pBegin);

    while (len > 1) {

        const usize half = len >> 1;
        len -= half;

        __ALT_STD_INTERNAL_PREFETCH__(pVal + (len >> 1) - 1);
        __ALT_STD_INTERNAL_PREFETCH__(pVal + half + (len >> 1) - 1);

        const bool isGreater = (cmp)(pVal[half - 1]);
        __ALT_STD_INTERNAL_CLANG_X86CMOV_WORKAROUND__(len, isGreater);
        pVal += isGreater ? half : 0;
    }

    pVal += len != 0 ? (cmp)(*pVal) : 0;

    return pVal;
}

template <typename T, typename U,
    std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* BinarySearchLowerBound(T* pBegin, T* pEnd, U&& x) noexcept {

    return BinarySearch(pBegin, pEnd, [&x](const T& y) noexcept -> bool {
        return x > y;
    });
}

template <typename T, typename U,
    std::enable_if_t<std::is_convertible_v<U, T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T* BinarySearchUpperBound(T* pBegin, T* pEnd, U&& x) noexcept {

    return BinarySearch(pBegin, pEnd, [&x](const T& y) noexcept -> bool {
        return x >= y;
    });
}

#undef __ALT_STD_INTERNAL_PREFETCH__
#undef __ALT_STD_INTERNAL_CLANG_X86CMOV_WORKAROUND__

template <typename T, typename FuncT>
ALT_FORCE_INLINE void Sort(T* pBegin, T* pEnd, FuncT&& cmp) noexcept {

    _SortDetails::sort(pBegin, pEnd, cmp);
}

template <typename T>
ALT_FORCE_INLINE void Sort(T* pBegin, T* pEnd) noexcept {

    _SortDetails::sort(pBegin, pEnd, [](const T& x, const T& y) noexcept { return x < y; });
}

namespace _AlgorithmsDetails {

template <typename T>
struct HashImpl<T, PlatformTrait::x32> {

    ALT_FORCE_INLINE static u32 Hash(const void* pBegin, const void* pEnd, const u32 seed) noexcept {

        const usize size = static_cast<usize>(static_cast<const u8*>(pEnd) - static_cast<const u8*>(pBegin));
        const XXH32_hash_t hash = XXH32(pBegin, size, seed);
        return static_cast<u32>(hash);
    }
};

template <typename T>
struct HashImpl<T, PlatformTrait::x64> {

    ALT_FORCE_INLINE static u64 Hash(const void* pBegin, const void* pEnd, const u64 seed) noexcept {

        const usize size = static_cast<usize>(static_cast<const u8*>(pEnd) - static_cast<const u8*>(pBegin));
        const XXH64_hash_t hash = XXH3_64bits_withSeed(pBegin, size, seed);
        return static_cast<u64>(hash);
    }
};

template <typename T>
struct HashConstexprImpl<T, PlatformTrait::x32> {

    ALT_FORCE_INLINE constexpr static u32 Hash(const T* pBegin, const usize size, const u32 seed) noexcept {

        const XXH32_hash_t hash = Algorithms::_HashDetails::XXH32Constexpr(pBegin, size, seed);
        return static_cast<u32>(hash);
    }
};

template <typename T>
struct HashConstexprImpl<T, PlatformTrait::x64> {

    ALT_FORCE_INLINE constexpr static u64 Hash(const T* pBegin, const usize size, const u64 seed) noexcept {

        const XXH64_hash_t hash = Algorithms::_HashDetails::XXH3_64bits_withSeedConstexpr(pBegin, size, seed);
        return static_cast<u64>(hash);
    }
};

template <typename T>
struct CompareImpl<T, ComparisonTrait::Memcmp> {

    ALT_FORCE_INLINE static ssize Compare(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

        const usize size0 = static_cast<usize>(pEnd0 - pBegin0);
        const usize size1 = static_cast<usize>(pEnd1 - pBegin1);

        if (size0 < size1) {
            return -1;
        }

        if (size0 > size1) {
            return 1;
        }

        if (size0 * sizeof(T) <= 8 * sizeof(usize)) {

            const T* pVal0 = pBegin0;
            const T* pVal1 = pBegin1;

            const T* pVal0End = pEnd0;

            bool isEq = true;

            while (isEq && pVal0 != pVal0End) {

                isEq = *pVal0 == *pVal1;

                pVal0 += 1;
                pVal1 += 1;
            }

            if (isEq) {
                return 0;
            }

            pVal0 -= 1;
            pVal1 -= 1;

            return *pVal0 < *pVal1 ? -1 : 1;
        }

        const s32 result = std::memcmp(pBegin0, pBegin1, size0 * sizeof(T));

        if (result == 0) {
            return 0;
        }

        return result < 0 ? -1 : 1;
    }
};

template <typename T>
struct CompareImpl<T, ComparisonTrait::Wmemcmp> {

    ALT_FORCE_INLINE static ssize Compare(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

        const usize size0 = static_cast<usize>(pEnd0 - pBegin0);
        const usize size1 = static_cast<usize>(pEnd1 - pBegin1);

        if (size0 < size1) {
            return -1;
        }

        if (size0 > size1) {
            return 1;
        }

        if (size0 * sizeof(T) <= 8 * sizeof(usize)) {

            const T* pVal0 = pBegin0;
            const T* pVal1 = pBegin1;

            const T* pVal0End = pEnd0;

            bool isEq = true;

            while (isEq && pVal0 != pVal0End) {

                isEq = *pVal0 == *pVal1;

                pVal0 += 1;
                pVal1 += 1;
            }

            if (isEq) {
                return 0;
            }

            pVal0 -= 1;
            pVal1 -= 1;

            return *pVal0 < *pVal1 ? -1 : 1;
        }

        const s32 result =
            std::wmemcmp(
                reinterpret_cast<const wchar_t*>(pBegin0),
                reinterpret_cast<const wchar_t*>(pBegin1),
                size0 * (sizeof(T) / sizeof(wchar_t))
            );

        if (result == 0) {
            return 0;
        }

        return result < 0 ? -1 : 1;
    }
};

template <typename T>
struct CompareImpl<T, ComparisonTrait::Operator> {

    ALT_FORCE_INLINE static ssize Compare(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

        const usize size0 = static_cast<usize>(pEnd0 - pBegin0);
        const usize size1 = static_cast<usize>(pEnd1 - pBegin1);

        if (size0 < size1) {
            return -1;
        }

        if (size0 > size1) {
            return 1;
        }

        const T* pVal0 = pBegin0;
        const T* pVal1 = pBegin1;

        const T* pVal0End = pEnd0;

        bool isEq = true;

        while (isEq && pVal0 != pVal0End) {

            isEq = *pVal0 == *pVal1;

            pVal0 += 1;
            pVal1 += 1;
        }

        if (isEq) {
            return 0;
        }

        pVal0 -= 1;
        pVal1 -= 1;

        return *pVal0 < *pVal1 ? -1 : 1;
    }
};

template <typename T>
struct CompareEQImpl<T, ComparisonTrait::Memcmp> {

    ALT_FORCE_INLINE static bool CompareEQ(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

        const usize size0 = static_cast<usize>(pEnd0 - pBegin0);
        const usize size1 = static_cast<usize>(pEnd1 - pBegin1);

        if (size0 != size1) {
            return false;
        }

        if (size0 * sizeof(T) <= 8 * sizeof(usize)) {

            const T* pVal0 = pBegin0;
            const T* pVal1 = pBegin1;

            const T* pVal0End = pEnd0;

            bool isEq = true;

            while (isEq && pVal0 != pVal0End) {

                isEq = *pVal0 == *pVal1;

                pVal0 += 1;
                pVal1 += 1;
            }

            return isEq;
        }

        return std::memcmp(pBegin0, pBegin1, size0 * sizeof(T)) == 0;
    }
};

template <typename T>
struct CompareEQImpl<T, ComparisonTrait::Operator> {

    ALT_FORCE_INLINE static bool CompareEQ(const T* pBegin0, const T* pEnd0, const T* pBegin1, const T* pEnd1) noexcept {

        const usize size0 = static_cast<usize>(pEnd0 - pBegin0);
        const usize size1 = static_cast<usize>(pEnd1 - pBegin1);

        if (size0 != size1) {
            return false;
        }

        const T* pVal0 = pBegin0;
        const T* pVal1 = pBegin1;

        const T* pVal0End = pEnd0;

        bool isEq = true;

        while (isEq && pVal0 != pVal0End) {

            isEq = *pVal0 == *pVal1;

            pVal0 += 1;
            pVal1 += 1;
        }

        return isEq;
    }
};

template <typename T>
struct FindFirstTraitImpl<T, FindTrait::Memchr> {

    ALT_FORCE_INLINE static const T* Find(const T* pBegin, const T* pEnd, const T& val_) noexcept {

        if ((pEnd - pBegin) * sizeof(T) <= 8 * sizeof(usize)) {

            bool isEq = false;
            const T* pVal = pBegin;

            while (!isEq && pVal != pEnd) {

                isEq = *pVal == val_;
                pVal += 1;
            }

            return isEq ? pVal - 1 : pVal;
        }

        const s32 val = static_cast<s32>(val_);
        const void* pResult = std::memchr(pBegin, val, (pEnd - pBegin) * sizeof(T));

        return pResult
            ? reinterpret_cast<const T*>(pResult)
            : pEnd;
    }
};

template <typename T>
struct FindFirstTraitImpl<T, FindTrait::Wmemchr> {

    ALT_FORCE_INLINE static const T* Find(const T* pBegin, const T* pEnd, const T& val_) noexcept {

        if ((pEnd - pBegin) * sizeof(T) <= 8 * sizeof(usize)) {

            bool isEq = false;
            const T* pVal = pBegin;

            while (!isEq && pVal != pEnd) {

                isEq = *pVal == val_;
                pVal += 1;
            }

            return isEq ? pVal - 1 : pVal;
        }

        const wchar_t val = static_cast<wchar_t>(val_);
        const wchar_t* pResult = std::wmemchr(
            reinterpret_cast<const wchar_t*>(pBegin),
            val,
            (pEnd - pBegin) * (sizeof(T) / sizeof(wchar_t))
        );

        return pResult
            ? reinterpret_cast<const T*>(pResult)
            : pEnd;
    }
};

template <typename T>
struct FindFirstTraitImpl<T, FindTrait::Operator> {

    ALT_FORCE_INLINE static const T* Find(const T* pBegin, const T* pEnd, const T& val) noexcept {

        bool isEq = false;
        const T* pVal = pBegin;

        while (!isEq && pVal != pEnd) {

            isEq = *pVal == val;
            pVal += 1;
        }

        return isEq ? pVal - 1 : pVal;
    }
};

} // namespace _AlgorithmsDetails

} // namespace Algorithms
} // namespace Alt

#endif // __ALT_STD_ALGORITHMS__