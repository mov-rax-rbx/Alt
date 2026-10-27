#pragma once

#ifndef __ALT_STD_RADIX_SORT__
#define __ALT_STD_RADIX_SORT__

#include "../IfErr.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Utils/Num.h"
#include "../Utils/ScopeDefer.h"

namespace Alt {
namespace RadixSort {

template <typename Usize>
ALT_FORCE_INLINE constexpr usize SortUnsignedImpl_GetBacketBufferSize() noexcept {

    constexpr usize BITS = Num::Desc<Usize>::Bits();
    constexpr usize BITS_PER_STEP = 8;

    constexpr usize NUM_STEPS = (BITS + BITS_PER_STEP - 1) / BITS_PER_STEP;
    constexpr usize BUCKET_LEN_PER_STEP = static_cast<usize>(1) << BITS_PER_STEP;

    return BUCKET_LEN_PER_STEP * NUM_STEPS;
}

template <typename SelfT, typename InnerT>
class Ptr {
public:

    ALT_FORCE_INLINE const InnerT& Get(const usize i) const noexcept {
        return static_cast<const SelfT*>(this)->Get(i);
    }

    ALT_FORCE_INLINE InnerT&& Take(const usize i) noexcept {
        return std::move(static_cast<SelfT*>(this)->Take(i));
    }

    ALT_FORCE_INLINE void Set(const usize i, InnerT&& val) noexcept {
        static_cast<SelfT*>(this)->Set(i, std::move(val));
    }

    template <typename BufferT>
    ALT_FORCE_INLINE void MoveObjectFromPtr(Ptr<BufferT, InnerT>& ptr, const usize size) noexcept {
        static_cast<SelfT*>(this)->MoveObjectFromPtr(ptr, size);
    }
};

// len(backets) == BUCKET_LEN_PER_STEP * NUM_STEPS == SortUnsignedImpl_GetBacketBufferSize<Usize>()
template <typename Usize, typename KeyT, typename LookupUsizeFunc>
ALT_FORCE_INLINE void SortUnsignedImpl(
    KeyT* const __restrict keys, KeyT* const __restrict buffer, usize* backets, const usize size, LookupUsizeFunc&& funcLookupVal
) noexcept {

    class PtrDefault : public Ptr<PtrDefault, KeyT> {
    public:

        PtrDefault(KeyT* const __restrict data) noexcept
            : m_pData(data)
        {}

        const KeyT& Get(const usize i) const noexcept {
            return m_pData[i];
        }

        KeyT&& Take(const usize i) noexcept {
            return std::move(m_pData[i]);
        }

        void Set(const usize i, KeyT&& val) noexcept {
            m_pData[i] = std::move(val);
        }

        void MoveObjectFromPtr(Ptr<PtrDefault, KeyT>& ptr, const usize size) const noexcept {

            KeyT* const __restrict buffer = static_cast<PtrDefault&>(ptr).m_pData;
            Memory::MoveObjectsFromFirstElementNonOverlap(m_pData, buffer, buffer + size);
        }

    private:

        KeyT* const __restrict m_pData;
    };

    PtrDefault keysPtr(keys);
    PtrDefault bufferPtr(buffer);

    SortUnsignedImpl<Usize>(keysPtr, bufferPtr, backets, size, std::forward<LookupUsizeFunc>(funcLookupVal));
}

template <typename Usize, typename KeysPtrT, typename BufferPtrT, typename KeyT, typename LookupUsizeFunc>
ALT_FORCE_INLINE void SortUnsignedImpl(
    Ptr<KeysPtrT, KeyT>& keys, Ptr<BufferPtrT, KeyT>& buffer, usize* backets, const usize size, LookupUsizeFunc&& funcLookupVal
) noexcept {
    ALT_ASSERT(backets);

    using U = typename Num::Desc<Usize>::Unsigned;

    constexpr usize BITS = Num::Desc<Usize>::Bits();
    constexpr usize BITS_PER_STEP = 8;
    static_assert(BITS_PER_STEP <= BITS, "");

    constexpr usize NUM_STEPS = (BITS + BITS_PER_STEP - 1) / BITS_PER_STEP;
    constexpr usize BUCKET_LEN_PER_STEP = static_cast<usize>(1) << BITS_PER_STEP;
    constexpr usize MASK = static_cast<usize>((1 << BITS_PER_STEP)) - 1;

    std::memset(backets, 0, BUCKET_LEN_PER_STEP * NUM_STEPS * sizeof(usize));

    for (usize iD = 0; iD < size; ++iD) {

        const KeyT& key = keys.Get(iD);
        const U val = (funcLookupVal)(key);

        for (usize st = 0; st < NUM_STEPS; ++st) {

            const usize idx = static_cast<usize>(val >> (BITS_PER_STEP * st)) & MASK;

            backets[BUCKET_LEN_PER_STEP * st + idx] += 1;
        }
    }

    usize sum[NUM_STEPS]{ 0 };
    for (usize iB = 0; iB < BUCKET_LEN_PER_STEP; ++iB) {

        for (usize st = 0; st < NUM_STEPS; ++st) {

            const usize localBacket = backets[BUCKET_LEN_PER_STEP * st + iB];
            backets[BUCKET_LEN_PER_STEP * st + iB] = sum[st];
            sum[st] += localBacket;
        }
    }

    bool isFlipped = false;
    for (usize st = 0; st < NUM_STEPS; ++st) {

        if (isFlipped) {

            for (usize iD = 0; iD < size; ++iD) {

                KeyT key = buffer.Take(iD);
                const U val = (funcLookupVal)(key);
                const usize idx = static_cast<usize>(val >> (BITS_PER_STEP * st)) & MASK;

                keys.Set(backets[BUCKET_LEN_PER_STEP * st + idx], std::move(key));
                backets[BUCKET_LEN_PER_STEP * st + idx] += 1;
            }
        }
        else {

            for (usize iD = 0; iD < size; ++iD) {

                KeyT key = keys.Take(iD);
                const U val = (funcLookupVal)(key);
                const usize idx = static_cast<usize>(val >> (BITS_PER_STEP * st)) & MASK;

                buffer.Set(backets[BUCKET_LEN_PER_STEP * st + idx], std::move(key));
                backets[BUCKET_LEN_PER_STEP * st + idx] += 1;
            }
        }

        isFlipped = !isFlipped;
    }

    if (isFlipped) {

        keys.MoveObjectFromPtr(buffer, size);
    }
}

template <typename ValT>
inline bool Sort(ValT* const __restrict data, ValT* const __restrict buffer, const usize size) noexcept;

template <>
inline bool Sort<u8>(u8* const __restrict data, u8* const __restrict buffer, const usize size) noexcept {

    usize backets[SortUnsignedImpl_GetBacketBufferSize<u8>()]{ 0 };

    SortUnsignedImpl<u8>(data, buffer, backets, size, [&](const u8 val) -> u8 {
        return val;
    });

    return true;
}

template <>
inline bool Sort<u16>(u16* const __restrict data, u16* const __restrict buffer, const usize size) noexcept {

    usize backets[SortUnsignedImpl_GetBacketBufferSize<u16>()]{ 0 };

    SortUnsignedImpl<u16>(data, buffer, backets, size, [&](const u16 val) -> u16 {
        return val;
    });

    return true;
}

template <>
inline bool Sort<u32>(u32* const __restrict data, u32* const __restrict buffer, const usize size) noexcept {

    usize backets[SortUnsignedImpl_GetBacketBufferSize<u32>()]{ 0 };

    SortUnsignedImpl<u32>(data, buffer, backets, size, [&](const u32 val) -> u32 {
        return val;
    });

    return true;
}

template <>
ALT_NODISCARD inline bool Sort<u64>(u64* const __restrict data, u64* const __restrict buffer, const usize size) noexcept {
    iferr_scope;

    usize* pBackets = Memory::ChooseAlignedMalloc<usize>(SortUnsignedImpl_GetBacketBufferSize<u64>()) iferr_return false;
    defer{ Memory::ChooseAlignedFree<usize>(pBackets); };

    SortUnsignedImpl<u64>(data, buffer, pBackets, size, [&](const u64 val) -> u64 {
        return val;
    });

    return true;
}

template <>
inline bool Sort<s8>(s8* const __restrict data, s8* const __restrict buffer, const usize size) noexcept {

    usize backets[SortUnsignedImpl_GetBacketBufferSize<u8>()]{ 0 };

    SortUnsignedImpl<u8>(data, buffer, backets, size, Num::ConvertSignedToUnsignedOrdered<s8>);

    return true;
}

template <>
inline bool Sort<s16>(s16* const __restrict data, s16* const __restrict buffer, const usize size) noexcept {

    usize backets[SortUnsignedImpl_GetBacketBufferSize<u16>()]{ 0 };

    SortUnsignedImpl<u16>(data, buffer, backets, size, Num::ConvertSignedToUnsignedOrdered<s16>);

    return true;
}

template <>
inline bool Sort<s32>(s32* const __restrict data, s32* const __restrict buffer, const usize size) noexcept {

    usize backets[SortUnsignedImpl_GetBacketBufferSize<u32>()]{ 0 };

    SortUnsignedImpl<u32>(data, buffer, backets, size, Num::ConvertSignedToUnsignedOrdered<s32>);

    return true;
}

template <>
ALT_NODISCARD inline bool Sort<s64>(s64* const __restrict data, s64* const __restrict buffer, const usize size) noexcept {
    iferr_scope;

    usize* pBackets = Memory::ChooseAlignedMalloc<usize>(SortUnsignedImpl_GetBacketBufferSize<u64>()) iferr_return false;
    defer{ Memory::ChooseAlignedFree<usize>(pBackets); };

    SortUnsignedImpl<u64>(data, buffer, pBackets, size, Num::ConvertSignedToUnsignedOrdered<s64>);

    return true;
}

template <>
inline bool Sort<f32>(f32* const __restrict data, f32* const __restrict buffer, const usize size) noexcept {

    usize backets[SortUnsignedImpl_GetBacketBufferSize<u32>()]{ 0 };

    SortUnsignedImpl<u32>(data, buffer, backets, size, Num::ConvertFloatToUnsignedOrdered<f32>);

    return true;
}

template <>
ALT_NODISCARD inline bool Sort<f64>(f64* const __restrict data, f64* const __restrict buffer, const usize size) noexcept {
    iferr_scope;

    usize* pBackets = Memory::ChooseAlignedMalloc<usize>(SortUnsignedImpl_GetBacketBufferSize<u64>()) iferr_return false;
    defer{ Memory::ChooseAlignedFree<usize>(pBackets); };

    SortUnsignedImpl<u64>(data, buffer, pBackets, size, Num::ConvertFloatToUnsignedOrdered<f64>);

    return true;
}

template <typename KeyT, typename ValT>
struct SortKeyByValue {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupVal) noexcept;
};

template <typename KeyT>
struct SortKeyByValue<KeyT, u8> {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupVal) noexcept {

        usize backets[SortUnsignedImpl_GetBacketBufferSize<u8>()]{ 0 };

        SortUnsignedImpl<u8>(keys, buffer, backets, size, [&](const KeyT key) -> u8 {
            return (funcLookupVal)(key);
        });

        return true;
    }
};

template <typename KeyT>
struct SortKeyByValue<KeyT, u16> {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupVal) noexcept {

        usize backets[SortUnsignedImpl_GetBacketBufferSize<u16>()]{ 0 };

        SortUnsignedImpl<u16>(keys, buffer, backets, size, [&](const KeyT key) -> u16 {
            return (funcLookupVal)(key);
        });

        return true;
    }
};

template <typename KeyT>
struct SortKeyByValue<KeyT, u32> {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupVal) noexcept {

        usize backets[SortUnsignedImpl_GetBacketBufferSize<u32>()]{ 0 };

        SortUnsignedImpl<u32>(keys, buffer, backets, size, [&](const KeyT key) -> u32 {
            return (funcLookupVal)(key);
        });

        return true;
    }
};

template <typename KeyT>
struct SortKeyByValue<KeyT, u64> {

    template <typename LookupValFunc>
    ALT_NODISCARD static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupVal) noexcept {
        iferr_scope;

        usize* pBackets = Memory::ChooseAlignedMalloc<usize>(SortUnsignedImpl_GetBacketBufferSize<u64>()) iferr_return false;
        defer{ Memory::ChooseAlignedFree<usize>(pBackets); };

        SortUnsignedImpl<u64>(keys, buffer, pBackets, size, [&](const KeyT key) -> u64 {
            return (funcLookupVal)(key);
        });

        return true;
    }
};

template <typename KeyT>
struct SortKeyByValue<KeyT, s8> {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupVal) noexcept {

        usize backets[SortUnsignedImpl_GetBacketBufferSize<u8>()]{ 0 };

        SortUnsignedImpl<u8>(keys, buffer, backets, size, [&](const KeyT key) -> u8 {
            return Num::ConvertSignedToUnsignedOrdered((funcLookupVal)(key));
        });

        return true;
    }
};

template <typename KeyT>
struct SortKeyByValue<KeyT, s16> {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupVal) noexcept {

        usize backets[SortUnsignedImpl_GetBacketBufferSize<u16>()]{ 0 };

        SortUnsignedImpl<u16>(keys, buffer, backets, size, [&](const KeyT key) -> u16 {
            return Num::ConvertSignedToUnsignedOrdered((funcLookupVal)(key));
        });

        return true;
    }
};

template <typename KeyT>
struct SortKeyByValue<KeyT, s32> {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupVal) noexcept {

        usize backets[SortUnsignedImpl_GetBacketBufferSize<u32>()]{ 0 };

        SortUnsignedImpl<u32>(keys, buffer, backets, size, [&](const KeyT key) -> u32 {
            return Num::ConvertSignedToUnsignedOrdered((funcLookupVal)(key));
        });

        return true;
    }
};

template <typename KeyT>
struct SortKeyByValue<KeyT, s64> {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupVal) noexcept {
        iferr_scope;

        usize* pBackets = Memory::ChooseAlignedMalloc<usize>(SortUnsignedImpl_GetBacketBufferSize<u64>()) iferr_return false;
        defer{ Memory::ChooseAlignedFree<usize>(pBackets); };

        SortUnsignedImpl<u64>(keys, buffer, pBackets, size, [&](const KeyT key) -> u64 {
            return Num::ConvertSignedToUnsignedOrdered((funcLookupVal)(key));
        });

        return true;
    }
};

template <typename KeyT>
struct SortKeyByValue<KeyT, f32> {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupF32) noexcept {

        usize backets[SortUnsignedImpl_GetBacketBufferSize<u32>()]{ 0 };

        SortUnsignedImpl<u32>(keys, buffer, backets, size, [&](const KeyT key) -> u32 {
            return Num::ConvertFloatToUnsignedOrdered<f32>((funcLookupF32)(key));
        });

        return true;
    }
};

template <typename KeyT>
struct SortKeyByValue<KeyT, f64> {

    template <typename LookupValFunc>
    static bool Sort(KeyT* const __restrict keys, KeyT* const __restrict buffer, const usize size, LookupValFunc&& funcLookupF64) noexcept {
        iferr_scope;

        usize* pBackets = Memory::ChooseAlignedMalloc<usize>(SortUnsignedImpl_GetBacketBufferSize<u64>()) iferr_return false;
        defer{ Memory::ChooseAlignedFree<usize>(pBackets); };

        SortUnsignedImpl<u64>(keys, buffer, pBackets, size, [&](const KeyT key) -> u64 {
            return Num::ConvertFloatToUnsignedOrdered<f64>((funcLookupF64)(key));
        });

        return true;
    }
};

} // namespace RadixSort
} // namespace Alt

#endif // __ALT_STD_RADIX_SORT__