#pragma once

#ifndef __ALT_STD_MEMORY__
#define __ALT_STD_MEMORY__

#include "Impl/MemoryPreInclude.h"

#include "../Utils/Optional.h"

namespace Alt {
namespace Memory {

template <typename T>
struct MallocWrapResult {

    T* pMemory = nullptr;
    usize size = 0;
};

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Optional<MallocWrapResult<T>> MallocWrap(
    const usize cnt,
    const usize wrapCnt = Num::Desc<usize>::Max() / sizeof(T),
    const MallocBits bits = MallocBits::None
) noexcept {

    _MemoryDetails::MallocWrapResult<T> result_ = _MemoryDetails::MallocImpl<T>::MallocWrap(cnt, wrapCnt, bits);
    if (!result_.pMemory) {
        return Nullopt;
    }

    MallocWrapResult<T> result{};
    result.pMemory = result_.pMemory;
    result.size = result_.size;

    return result;
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Optional<MallocWrapResult<T>> ReallocWrap(
    T* pMemoryOld,
    const usize cntOld,
    const usize cntNew,
    const usize wrapCnt = Num::Desc<usize>::Max() / sizeof(T),
    const ReallocBits bits = ReallocBits::None
) noexcept {

    _MemoryDetails::MallocWrapResult<T> result_ = _MemoryDetails::MallocImpl<T>::ReallocWrap(pMemoryOld, cntOld, cntNew, wrapCnt, bits);
    if (!result_.pMemory) {
        return Nullopt;
    }

    MallocWrapResult<T> result{};
    result.pMemory = result_.pMemory;
    result.size = result_.size;

    return result;
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Optional<MallocWrapResult<T>> AlignedMallocWrap(
    const usize cnt,
    const usize wrapCnt = Num::Desc<usize>::Max() / sizeof(T),
    const MallocBits bits = MallocBits::None
) noexcept {

    _MemoryDetails::MallocWrapResult<T> result_ = _MemoryDetails::AlignedMallocImpl<T>::AlignedMallocWrap(cnt, wrapCnt, bits);
    if (!result_.pMemory) {
        return Nullopt;
    }

    MallocWrapResult<T> result{};
    result.pMemory = result_.pMemory;
    result.size = result_.size;

    return result;
}

template <typename T>
ALT_NODISCARD ALT_FORCE_INLINE Optional<MallocWrapResult<T>> AlignedReallocWrap(
    T* pMemoryOld,
    const usize cntOld,
    const usize cntNew,
    const usize wrapCnt = Num::Desc<usize>::Max() / sizeof(T),
    const ReallocBits bits = ReallocBits::None
) noexcept {

    _MemoryDetails::MallocWrapResult<T> result_ = _MemoryDetails::AlignedMallocImpl<T>::AlignedReallocWrap(pMemoryOld, cntOld, cntNew, wrapCnt, bits);
    if (!result_.pMemory) {
        return Nullopt;
    }

    MallocWrapResult<T> result{};
    result.pMemory = result_.pMemory;
    result.size = result_.size;

    return result;
}

template <typename T,
    std::enable_if_t<(alignof(T) <= MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<MallocWrapResult<T>> ChooseAlignedMallocWrap(
    const usize cnt,
    const usize wrapCnt = Num::Desc<usize>::Max() / sizeof(T),
    const MallocBits bits = MallocBits::None
) noexcept {

    _MemoryDetails::MallocWrapResult<T> result_ = _MemoryDetails::MallocImpl<T>::MallocWrap(cnt, wrapCnt, bits);
    if (!result_.pMemory) {
        return Nullopt;
    }

    MallocWrapResult<T> result{};
    result.pMemory = result_.pMemory;
    result.size = result_.size;

    return result;
}

template <typename T,
    std::enable_if_t<(alignof(T) > MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<MallocWrapResult<T>> ChooseAlignedMallocWrap(
    const usize cnt,
    const usize wrapCnt = Num::Desc<usize>::Max() / sizeof(T),
    const MallocBits bits = MallocBits::None
) noexcept {

    _MemoryDetails::MallocWrapResult<T> result_ = _MemoryDetails::AlignedMallocImpl<T>::AlignedMallocWrap(cnt, wrapCnt, bits);
    if (!result_.pMemory) {
        return Nullopt;
    }

    MallocWrapResult<T> result{};
    result.pMemory = result_.pMemory;
    result.size = result_.size;

    return result;
}

template <typename T,
    std::enable_if_t<(alignof(T) <= MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<MallocWrapResult<T>> ChooseAlignedReallocWrap(
    T* pMemoryOld,
    const usize cntOld,
    const usize cntNew,
    const usize wrapCnt = Num::Desc<usize>::Max() / sizeof(T),
    const ReallocBits bits = ReallocBits::None
) noexcept {

    _MemoryDetails::MallocWrapResult<T> result_ = _MemoryDetails::MallocImpl<T>::ReallocWrap(pMemoryOld, cntOld, cntNew, wrapCnt, bits);
    if (!result_.pMemory) {
        return Nullopt;
    }

    MallocWrapResult<T> result{};
    result.pMemory = result_.pMemory;
    result.size = result_.size;

    return result;
}

template <typename T,
    std::enable_if_t<(alignof(T) > MallocDefaultAlignment())>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Optional<MallocWrapResult<T>> ChooseAlignedReallocWrap(
    T* pMemoryOld,
    const usize cntOld,
    const usize cntNew,
    const usize wrapCnt = Num::Desc<usize>::Max() / sizeof(T),
    const ReallocBits bits = ReallocBits::None
) noexcept {

    _MemoryDetails::MallocWrapResult<T> result_ = _MemoryDetails::AlignedMallocImpl<T>::AlignedReallocWrap(pMemoryOld, cntOld, cntNew, wrapCnt, bits);
    if (!result_.pMemory) {
        return Nullopt;
    }

    MallocWrapResult<T> result{};
    result.pMemory = result_.pMemory;
    result.size = result_.size;

    return result;
}

} // namespace Memory
} // namespace Alt

#endif // __ALT_STD_MEMORY__