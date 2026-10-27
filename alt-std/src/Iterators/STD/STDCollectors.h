#pragma once

#ifndef __ALT_STD_ITERATOR_STD_COLLECTORS__
#define __ALT_STD_ITERATOR_STD_COLLECTORS__

#include <vector>
#include <deque>

#include <list>
#include <forward_list>

#include <unordered_set>
#include <unordered_map>

#include <set>
#include <map>

#include "../IteratorCollector.h"
#include "../IteratorInterface.h"

#ifndef __cpp_exceptions
#define __ALT_STD_INTERNAL_TRY__
#define __ALT_STD_INTERNAL_CATCH_ANY__
#else
#define __ALT_STD_INTERNAL_TRY__ try
#define __ALT_STD_INTERNAL_CATCH_ANY__ catch (...)
#endif // __cpp_exceptions

#if defined(ALT_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable: 4702 /* unreachable code */)
#elif defined(ALT_COMPILER_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#endif

namespace Alt {

template <typename T, typename Alloc>
class IteratorCollector<std::vector<T, Alloc>> {
public:

    using SelfT = std::vector<T, Alloc>;

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait> iter) noexcept {

        __ALT_STD_INTERNAL_TRY__ {

            const usize reserve = Num::AddWrap(self.size(), iter.GetSizeHint());
            self.reserve(reserve);

            while (Optional<U> val = iter.Next()) {

                self.push_back(std::move(val.GetValue()));
            }

            return true;
        }
        __ALT_STD_INTERNAL_CATCH_ANY__ {

            return false;
        }
    }
};

template <typename T, typename Alloc>
class IteratorCollector<std::deque<T, Alloc>> {
public:

    using SelfT = std::deque<T, Alloc>;

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait> iter) noexcept {

        __ALT_STD_INTERNAL_TRY__ {

            while (Optional<T> val = iter.Next()) {

                self.push_back(std::move(val.GetValue()));
            }

            return true;
        }
        __ALT_STD_INTERNAL_CATCH_ANY__ {

            return false;
        }
    }
};

template <typename T, typename Alloc>
class IteratorCollector<std::list<T, Alloc>> {
public:

    using SelfT = std::list<T, Alloc>;

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait> iter) noexcept {

        __ALT_STD_INTERNAL_TRY__ {

            while (Optional<T> val = iter.Next()) {

                self.push_back(std::move(val.GetValue()));
            }

            return true;
        }
        __ALT_STD_INTERNAL_CATCH_ANY__ {

            return false;
        }
    }
};

template <typename T, typename Alloc>
class IteratorCollector<std::forward_list<T, Alloc>> {
public:

    using SelfT = std::forward_list<T, Alloc>;

    template <typename U = T,
        std::enable_if_t<std::is_constructible_v<T, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait> iter) noexcept {

        __ALT_STD_INTERNAL_TRY__ {

            while (Optional<T> val = iter.NextBack()) {

                self.push_front(std::move(val.GetValue()));
            }

            return true;
        }
        __ALT_STD_INTERNAL_CATCH_ANY__ {

            return false;
        }
    }
};

template <typename K, typename Hasher, typename Keyeq, typename Alloc>
class IteratorCollector<std::unordered_set<K, Hasher, Keyeq, Alloc>> {
public:

    using SelfT = std::unordered_set<K, Hasher, Keyeq, Alloc>;

    template <typename U = K,
        std::enable_if_t<std::is_constructible_v<K, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait> iter) noexcept {

        __ALT_STD_INTERNAL_TRY__ {

            const usize reserve = Num::AddWrap(self.size(), iter.GetSizeHint());
            self.reserve(reserve);

            while (Optional<K> val = iter.Next()) {

                self.insert(std::move(val.GetValue()));
            }

            return true;
        }
        __ALT_STD_INTERNAL_CATCH_ANY__ {

            return false;
        }
    }
};

template <typename K, typename V, typename Hasher, typename Keyeq, typename Alloc>
class IteratorCollector<std::unordered_map<K, V, Hasher, Keyeq, Alloc>> {
public:

    using SelfT = std::unordered_map<K, V, Hasher, Keyeq, Alloc>;

    template <typename UK = K, typename UV = V,
        std::enable_if_t<std::is_constructible_v<K, UK> && (std::is_constructible_v<V, UV> && std::is_assignable_v<V&, UV>)>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<Pair<UK, UV>, IIteratorForwardTrait> iter) noexcept {

        __ALT_STD_INTERNAL_TRY__ {

            const usize reserve = Num::AddWrap(self.size(), iter.GetSizeHint());
            self.reserve(reserve);

            while (Optional<Pair<UK, UV>> val = iter.Next()) {

                self.insert_or_assign(val->f(), val->s());
            }

            return true;
        }
        __ALT_STD_INTERNAL_CATCH_ANY__ {

            return false;
        }
    }
};

template <typename K, typename Pr, typename Alloc>
class IteratorCollector<std::set<K, Pr, Alloc>> {
public:

    using SelfT = std::set<K, Pr, Alloc>;

    template <typename U = K,
        std::enable_if_t<std::is_constructible_v<K, U>>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait> iter) noexcept {

        __ALT_STD_INTERNAL_TRY__ {

            while (Optional<K> val = iter.Next()) {

                self.insert(std::move(val.GetValue()));
            }

            return true;
        }
        __ALT_STD_INTERNAL_CATCH_ANY__ {

            return false;
        }
    }
};

template <typename K, typename V, typename Pr, typename Alloc>
class IteratorCollector<std::map<K, V, Pr, Alloc>> {
public:

    using SelfT = std::map<K, V, Pr, Alloc>;

    template <typename UK = K, typename UV = V,
        std::enable_if_t<std::is_constructible_v<K, UK> && (std::is_constructible_v<V, UV> && std::is_assignable_v<V&, UV>)>* = nullptr>
    static bool ExtendMove(SelfT& self, IteratorInterface<Pair<UK, UV>, IIteratorForwardTrait> iter) noexcept {

        __ALT_STD_INTERNAL_TRY__ {

            while (Optional<Pair<UK, UV>> val = iter.Next()) {

                self.insert_or_assign(val->f(), val->s());
            }

            return true;
        }
        __ALT_STD_INTERNAL_CATCH_ANY__ {

            return false;
        }
    }
};

} // namespace Alt

#if defined(ALT_COMPILER_MSVC)
#pragma warning(pop)
#elif defined(ALT_COMPILER_CLANG)
#pragma clang diagnostic pop
#endif

#undef __ALT_STD_INTERNAL_TRY__
#undef __ALT_STD_INTERNAL_CATCH_ANY__

#endif // __ALT_STD_ITERATOR_STD_COLLECTORS__