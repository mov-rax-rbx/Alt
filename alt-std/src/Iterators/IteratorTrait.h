#pragma once

#ifndef __ALT_STD_ITERATOR_TRAIT__
#define __ALT_STD_ITERATOR_TRAIT__

#include "../Utils/Optional.h"

#include "Impl/IteratorCore.h"

namespace Alt {

class IIteratorBase : public _IterDetails::TraitMarker {
public:

    /*
     * Interface
     */
    //virtual usize GetSizeHint() const noexcept = 0;

    //virtual ~IIteratorBase() noexcept = default;
};

template <typename T>
class IIteratorForwardTrait : public _IterDetails::TraitMarker {
public:

    /*
     * Interface
     */
    //virtual Optional<T> Next() noexcept = 0;
    //virtual usize Advance(const usize advance) noexcept = 0;

    //virtual ~IIteratorForwardTrait() noexcept = default;
};

template <typename T>
class IIteratorBackwardTrait : public _IterDetails::TraitMarker {
public:

    /*
     * Interface
     */
    //virtual Optional<T> NextBack() noexcept = 0;
    //virtual usize AdvanceBack(const usize advance) noexcept = 0;

    //virtual ~IIteratorBackwardTrait() noexcept = default;
};

template <typename T>
class IIteratorExactSizeTrait : public _IterDetails::TraitMarker {
public:

    /*
     * Interface
     */
    //virtual usize GetSize() const noexcept = 0;
    //virtual bool IsEmpty() const noexcept = 0;

    //virtual ~IIteratorExactSizeTrait() noexcept = default;
};

} // namespace Alt

#endif // __ALT_STD_ITERATOR_TRAIT__