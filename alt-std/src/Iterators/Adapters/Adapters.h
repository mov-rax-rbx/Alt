#pragma once

#ifndef __ALT_STD_ITERATOR_ADAPTERS__
#define __ALT_STD_ITERATOR_ADAPTERS__

#include "../IteratorCollector.h"

#include "../STD/STDIteratorForAdapter.h"

namespace Alt {

template <typename T, template <typename> typename ...TraitsT>
class IteratorInterface;

namespace _IterDetails {
namespace Adapters {

template <typename PrevIterT>
class Enumerate_;

template <typename PrevIterT>
class Peekable_;

template <typename PrevIterT>
class Fuse_;

template <typename PrevIterT>
class Skip_;

template <typename PrevIterT, typename FuncT>
class SkipWhile_;

template <typename PrevIterT>
class Take_;

template <typename PrevIterT, typename FuncT>
class TakeWhile_;

template <typename PrevIterT>
class Step_;

template <typename PrevIterT>
class Flatten_;

template <typename PrevIterT0, typename PrevIterT1>
class Zip_;

template <typename PrevIterT0, typename PrevIterT1>
class Chain_;

template <typename PrevIterT, typename FuncT>
class Filter_;

template <typename PrevIterT, typename FuncT>
class Map_;

template <typename PrevIterT, typename FuncT>
class Inspect_;

template <typename PrevIterT, typename FuncT>
class Intersperse_;

template <typename PrevIterT>
class Copied_;

template <typename PrevIterT>
class Rev_;

template <bool IS_NEED_IMPL, typename SelfT>
class IteratorForwardAdaptersImpl
{};

template <typename SelfT>
class IteratorForwardAdaptersImpl<true, SelfT> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE SelfT CloneIter() const noexcept {
        return Self();
    }

    ALT_NODISCARD ALT_FORCE_INLINE STDIterator<SelfT> begin() const noexcept {
        return STDIterator{ Self() };
    }

    ALT_NODISCARD ALT_FORCE_INLINE STDIteratorEnd end() const noexcept {
        return STDIteratorEnd{};
    }

    template <typename ContainerT>
    ALT_NODISCARD ALT_FORCE_INLINE Optional<ContainerT> Collect() noexcept {

        ContainerT container{};

        IteratorInterface<typename SelfT::Type, IIteratorForwardTrait> iteratorInterface = Self();
        const bool isOk = IteratorCollector<ContainerT>::ExtendMove(container, iteratorInterface);

        return isOk
            ? MakeOptional(std::move(container))
            : Nullopt;
    }

    template <typename ContainerT>
    ALT_NODISCARD ALT_FORCE_INLINE ContainerT CollectOrDefault() noexcept {

        return Collect<ContainerT>()
            .GetValueOrDefault();
    }

    template <typename ContainerT>
    ALT_NODISCARD ALT_FORCE_INLINE bool CollectInto(ContainerT& container) noexcept {

        IteratorInterface<typename SelfT::Type, IIteratorForwardTrait> iteratorInterface = Self();
        return IteratorCollector<ContainerT>::ExtendMove(container, iteratorInterface);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Enumerate_<SelfT> Enumerate() noexcept {
        return Enumerate_{ Self() };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Peekable_<SelfT> Peekable() noexcept {
        return Peekable_{ Self() };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Fuse_<SelfT> Fuse() noexcept {
        return Fuse_{ Self() };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Skip_<SelfT> Skip(const usize skipN) noexcept {
        return Skip_{ Self(), skipN };
    }

    template <typename SelfU = SelfT, typename FuncT,
        std::enable_if_t<std::is_same_v<std::invoke_result_t<FuncT, const typename SelfU::Type&>, bool>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE SkipWhile_<SelfU, FuncT> SkipWhile(FuncT&& pred) noexcept {
        return SkipWhile_{ Self(), std::forward<FuncT>(pred) };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Take_<SelfT> Take(const usize takeCnt) noexcept {
        return Take_{ Self(), takeCnt };
    }

    template <typename SelfU = SelfT, typename FuncT,
        std::enable_if_t<std::is_same_v<std::invoke_result_t<FuncT, const typename SelfU::Type&>, bool>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE TakeWhile_<SelfU, FuncT> TakeWhile(FuncT&& pred) noexcept {
        return TakeWhile_{ Self(), std::forward<FuncT>(pred)};
    }

    ALT_NODISCARD ALT_FORCE_INLINE Step_<SelfT> Step(const usize step) noexcept {
        return Step_{ Self(), step };
    }

    template <typename SelfU = SelfT,
        std::enable_if_t<_IterDetails::is_trait_alt_iterator_v<SelfU> && _IterDetails::is_trait_alt_iterator_v<typename SelfU::Type>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Flatten_<SelfU> Flatten() noexcept {
        return Flatten_{ Self() };
    }

    template <typename T,
        std::enable_if_t<_IterDetails::is_trait_alt_iterator_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Zip_<SelfT, T> Zip(const T& iter) noexcept {
        return Zip_{ Self(), iter };
    }

    template <typename SelfU = SelfT, typename T,
        std::enable_if_t<_IterDetails::is_trait_alt_iterator_v<T> && std::is_convertible_v<typename T::Type, typename SelfU::Type>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Chain_<SelfU, T> Chain(const T& iter) noexcept {
        return Chain_{ Self(), iter };
    }

    template <typename SelfU = SelfT, typename FuncT,
        std::enable_if_t<std::is_same_v<std::invoke_result_t<FuncT, const typename SelfU::Type&>, bool>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Filter_<SelfT, FuncT> Filter(FuncT&& pred) noexcept {
        return Filter_{ Self(), std::forward<FuncT>(pred) };
    }

    template <typename SelfU = SelfT, typename FuncT,
        std::enable_if_t<std::is_same_v<std::invoke_result_t<FuncT, const typename SelfU::Type&>, bool>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Optional<typename SelfU::Type> Find(FuncT&& pred) noexcept {

        /*
         * Same as:
         *     return Filter(std::forward<FuncT>(pred)).Next();
         * But we are modifying iterator, because 'Find(...)' returns value like 'Next()'
         */
        while (Optional<typename SelfU::Type> val = Self().Next()) {

            if ((pred)(*val)) {

                return val;
            }
        }

        return Nullopt;
    }

    template <typename SelfU = SelfT, typename FuncT,
        std::invoke_result_t<FuncT, const typename SelfU::Type&>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Map_<SelfU, FuncT> Map(FuncT&& func) noexcept {
        return Map_{ Self(), std::forward<FuncT>(func) };
    }

    template <typename SelfU = SelfT, typename FuncT,
        std::enable_if_t<std::is_same_v<std::invoke_result_t<FuncT, const typename SelfU::Type&>, void>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Inspect_<SelfU, FuncT> Inspect(FuncT&& func) noexcept {
        return Inspect_{ Self(), std::forward<FuncT>(func) };
    }

    template <typename SelfU = SelfT, typename FuncT,
        std::enable_if_t<std::is_convertible_v<std::invoke_result_t<FuncT>, typename SelfU::Type>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE Intersperse_<SelfU, FuncT> Intersperse(FuncT&& func) noexcept {
        return Intersperse_{ Self(), std::forward<FuncT>(func) };
    }

    ALT_NODISCARD ALT_FORCE_INLINE Copied_<SelfT> Copied() noexcept {
        return Copied_{ Self() };
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Count() noexcept {

        return Fold((usize)0, [](const usize acc, const typename SelfT::Type&) {
            return acc + 1;
        });
    }

    template <typename SelfU = SelfT, typename FuncT,
        std::enable_if_t<std::is_same_v<std::invoke_result_t<FuncT, typename SelfU::Type>, void>>* = nullptr>
    ALT_FORCE_INLINE void ForEach(FuncT&& func) noexcept {

        using T = typename SelfU::Type;

        while (Optional<T> val = Self().Next()) {

            (func)(val.GetValue());
        }
    }

    template <typename U, typename SelfU = SelfT, typename FuncT,
        std::enable_if_t<std::is_same_v<std::invoke_result_t<FuncT, const U&, const typename SelfU::Type&>, U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE U Fold(U&& init, FuncT&& func) noexcept {

        using T = typename SelfT::Type;

        while (Optional<T> val = Self().Next()) {

            const U& init_ = init;
            init = (func)(init_, val.GetValue());
        }

        return std::forward<U>(init);
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

template <bool IS_NEED_IMPL, typename SelfT>
class IteratorBackwardAdaptersImpl
{};

template <typename SelfT>
class IteratorBackwardAdaptersImpl<true, SelfT> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE Rev_<SelfT> Rev() noexcept {
        return Rev_{ Self() };
    }

    template <typename U, typename Func>
    ALT_NODISCARD ALT_FORCE_INLINE U FoldRight(U&& init, Func&& func) noexcept {

        using T = typename SelfT::Type;

        while (Optional<T> val = Self().NextBack()) {

            const U& init_ = init;
            init = (func)(init_, std::move(val.GetValue()));
        }

        return std::forward<U>(init);
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

} // namespace Alt
} // namespace _IterDetails
} // namespace Adapters

#include "Enumerate.inl"
#include "Peekable.inl"
#include "Fuse.inl"
#include "Skip.inl"
#include "SkipWhile.inl"
#include "Take.inl"
#include "TakeWhile.inl"
#include "Step.inl"
#include "Flatten.inl"
#include "Zip.inl"
#include "Chain.inl"
#include "Map.inl"
#include "Inspect.inl"
#include "Intersperse.inl"
#include "Copied.inl"
#include "Filter.inl"
#include "Rev.inl"

#endif // __ALT_STD_ITERATOR_ADAPTERS__