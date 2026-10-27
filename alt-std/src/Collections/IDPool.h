#pragma once

#ifndef __ALT_STD_ID_POOL__
#define __ALT_STD_ID_POOL__

#include "../TypeTraits.h"

#include "../Collections/Vector.h"

namespace Alt {

template <typename T, T START_ID = 0>
class IDPool;

namespace TypeTraits {

template <typename T>
struct __is_trivially_reallocatable__<IDPool<T>>
    : std::true_type
{};

} // namespace TypeTraits

template <typename T, T START_ID>
class IDPool {
public:

    IDPool() noexcept = default;

    T CreateID() noexcept {

        if (m_FreeIDs.IsEmpty()) {

            return m_CurrentID++;
        }

        return m_FreeIDs.Pop();
    }

    void RemoveID(const T removeID) noexcept {
        ALT_ASSERT(m_CurrentID != 0);

        if (m_CurrentID - 1 == removeID) {
            m_CurrentID -= 1;
        }
        else {
            m_FreeIDs.Push(removeID);
        }
    }

    void Clear() noexcept {

        m_FreeIDs.Clear();
        m_CurrentID = START_ID;
    }

protected:

    Vector<T> m_FreeIDs;
    T m_CurrentID = START_ID;
};

using IDPoolU32 = IDPool<u32>;
using IDPoolU64 = IDPool<u64>;
using IDPoolUsize = IDPool<usize>;

} // namespace Alt

#endif // __ALT_STD_ID_POOL__