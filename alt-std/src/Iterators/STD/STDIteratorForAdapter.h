#pragma once

#ifndef __ALT_STD_ADAPTER_ITERATOR__
#define __ALT_STD_ADAPTER_ITERATOR__

#include "../../TypeTraits.h"

#include "../../Utils/Optional.h"

/*
    Alt::IteratorForwardFromFunc iter{
        []() {
            static usize cnt = 0;
            return cnt++;
        }
    };

    Alt::Vector<usize> vec;
    for (usize val : iter) {

        if (val == 4) {
            break;
        }

        vec.Push(val);
    }

    assert(vec[0] == 0);
    assert(vec[1] == 1);
    assert(vec[2] == 2);
    assert(vec[3] == 3);
*/

namespace Alt {

struct STDIteratorEnd
{};

template <typename SelfT>
class STDIterator final {
public:

    using Type = typename SelfT::Type;

    ALT_FORCE_INLINE STDIterator(const SelfT& iter) noexcept
        : m_Iter(iter)
    {}

    ALT_FORCE_INLINE Type operator*() noexcept {

        if (!m_Deref) {

            m_Deref = MakeOptional(m_Iter.Next());
        }

        ALT_ASSERT(m_Deref.GetValue());

        return m_Deref
            .GetValue()
            .GetValue();
    }

    ALT_FORCE_INLINE STDIterator& operator++() noexcept {

        m_Deref = m_Iter.Next();
        return *this;
    }

    ALT_FORCE_INLINE STDIterator operator++(int) noexcept {

        STDIterator result = *this;
        ++*this;

        return result;
    }

    ALT_FORCE_INLINE bool operator==(STDIteratorEnd) noexcept {

        if (!m_Deref) {

            m_Deref = MakeOptional(m_Iter.Next());
        }

        return !*m_Deref;
    }

    ALT_FORCE_INLINE bool operator!=(STDIteratorEnd) noexcept {

        return !(*this == STDIteratorEnd{});
    }

private:

    SelfT m_Iter;
    Optional<Optional<Type>> m_Deref;
};

} // namespace Alt

#endif // __ALT_STD_ADAPTER_ITERATOR__