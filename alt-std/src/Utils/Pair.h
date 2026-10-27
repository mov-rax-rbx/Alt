#pragma once

#ifndef __ALT_STD_PAIR__
#define __ALT_STD_PAIR__

#include <utility>

#include "../TypeTraits.h"

#include "../Memory/Ref.h"

#include "Optional.h"

namespace Alt {

template <typename T0, typename T1>
class Pair;

/**
 * @brief Creates a Pair.
 * 
 * @tparam T0 The type of the first element.
 * @tparam T1 The type of the second element.
 * @param first The first element to be stored in the pair.
 * @param second The second element to be stored in the pair.
 * @return A constructed Pair<T0, T1> object.
 * @example
 * 
 * Alt::Pair<s32, f32> pair = Alt::MakePair(10, 20.5f);
 */
template <typename T0, typename T1>
ALT_NODISCARD ALT_FORCE_INLINE constexpr Pair<T0, T1> MakePair(T0&& val0, T1&& val1) noexcept {
    return Pair<T0, T1>{ std::forward<T0>(val0), std::forward<T1>(val1) };
}

namespace TypeTraits {

template <typename T0, typename T1>
struct __is_pair__<Pair<T0, T1>>
    : std::true_type
{};

template <typename T0, typename T1>
struct __is_trivially_reallocatable__<Pair<T0, T1>>
    : std::conditional_t<

        is_trivially_reallocatable_v<TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<T0>> &&
        is_trivially_reallocatable_v<TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<T1>>,

        std::true_type,
        std::false_type
    >
{};

} // namespace TypeTraits

/**
 * @brief A lightweight pair container.
 * 
 * @tparam T0 The type of the first element.
 * @tparam T1 The type of the second element.
 * @example
 * 
 * Alt::Pair<usize, f64> pair(4, 4.5);
 * assert(pair.f() == 4);
 * assert(pair.s() == 4.5);
 * 
 * Alt::Pair<usize, usize> pair1 = Alt::Pair<usize, usize>(40, 450);
 * assert(pair1.f() == 40);
 */
template <typename T0_, typename T1_>
class Pair {
private:

    using CleanT0 = TypeTraits::clean_t<T0_>;
    using CleanT1 = TypeTraits::clean_t<T1_>;

    using T0 = TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<CleanT0>;
    using T1 = TypeTraits::unwrap_reference_to_alt_ref_or_just_type_t<CleanT1>;

public:

    ALT_FORCE_INLINE Pair() noexcept = default;

    Pair(const Pair&) noexcept = default;
    Pair& operator=(const Pair&) noexcept = default;

    Pair(Pair&&) noexcept = default;
    Pair& operator=(Pair&&) noexcept = default;

    ~Pair() noexcept = default;

    /**
     * @brief Accesses the first element of the pair.
     * @return The first element.
     * @example
     * 
     * Alt::Pair<usize, usize> pair(4, 8);
     * assert(pair.f() == 4);
     * assert(pair.first() == 4);
     * 
     * Alt::Pair<usize&, usize&> pairRef(pair.f(), pair.s());
     * assert(&pairRef.f() == &pair.f());
     * assert(&pairRef.s() == &pair.s());
     * 
     * assert(&Alt::Pair<usize&, usize&>(pair.f(), pair.s()).f() == &pair.f());
     * assert(&Alt::Pair<usize&, usize&>(pair.f(), pair.s()).s() == &pair.s());
     */
    template <typename U = CleanT0>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT0>&, CleanT0&>
        f() & noexcept { return _f; }

    template <typename U = CleanT0>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT0>&, const CleanT0&>
        f() const & noexcept { return _f; }

    template <typename U = CleanT0>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT0>&, CleanT0&&>
        f() && noexcept { return std::move(_f); }

    template <typename U = CleanT0>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT0>&, const CleanT0&&>
        f() const && noexcept { return std::move(_f); }

    template <typename U = CleanT0>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT0>&, CleanT0&>
        first() & noexcept { return _f; }

    template <typename U = CleanT0>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT0>&, const CleanT0&>
        first() const & noexcept { return _f; }

    template <typename U = CleanT0>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT0>&, CleanT0&&>
        first() && noexcept { return std::move(_f); }

    template <typename U = CleanT0>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT0>&, const CleanT0&&>
        first() const && noexcept { return std::move(_f); }

    /**
     * @brief Accesses the second element of the pair.
     * @return The second element.
     * @example
     * 
     * Alt::Pair<usize, usize> pair(4, 8);
     * assert(pair.s() == 8);
     * assert(pair.second() == 8);
     * 
     * Alt::Pair<usize&, usize&> pairRef(pair.f(), pair.s());
     * assert(&pairRef.f() == &pair.f());
     * assert(&pairRef.s() == &pair.s());
     * 
     * assert(&Alt::Pair<usize&, usize&>(pair.f(), pair.s()).f() == &pair.f());
     * assert(&Alt::Pair<usize&, usize&>(pair.f(), pair.s()).s() == &pair.s());
     */
    template <typename U = CleanT1>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT1>&, CleanT1&>
        s() & noexcept { return _s; }

    template <typename U = CleanT1>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT1>&, const CleanT1&>
        s() const & noexcept { return _s; }

    template <typename U = CleanT1>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT1>&, CleanT1&&>
        s() && noexcept { return std::move(_s); }

    template <typename U = CleanT1>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT1>&, const CleanT1&&>
        s() const && noexcept { return std::move(_s); }

    template <typename U = CleanT1>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT1>&, CleanT1&>
        second() & noexcept { return _s; }

    template <typename U = CleanT1>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT1>&, const CleanT1&>
        second() const & noexcept { return _s; }

    template <typename U = CleanT1>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, std::remove_reference_t<CleanT1>&, CleanT1&&>
        second() && noexcept { return std::move(_s); }

    template <typename U = CleanT1>
    ALT_FORCE_INLINE std::conditional_t<std::is_reference_v<U>, const std::remove_reference_t<CleanT1>&, const CleanT1&&>
        second() const && noexcept { return std::move(_s); }

    /**
     * @brief Constructs a pair from two values.
     * 
     * @tparam U0 The type of the first value. Must be constructible into T0.
     * @tparam U1 The type of the second value. Must be constructible into T1.
     * @param f The value for the first element.
     * @param s The value for the second element.
     * @example
     * 
     * Alt::Pair<usize, usize> pair(4, 8);
     */
    template <typename U0 = T0, typename U1 = T1,
        std::enable_if_t<std::is_constructible_v<T0, U0&&> && std::is_constructible_v<T1, U1&&>>* = nullptr>
    ALT_FORCE_INLINE Pair(U0&& f, U1&& s) noexcept
        : _f(std::forward<U0>(f))
        , _s(std::forward<U1>(s))
    {}

    /**
     * @brief Constructs a pair from another pair.
     * 
     * @tparam U0 The type of the first element in the source pair.
     * @tparam U1 The type of the second element in the source pair.
     * @param pair The source pair to move from.
     * @example
     * 
     * Alt::Pair<usize, usize> pair0(4, 8);
     * Alt::Pair<usize, usize> pair1(std::move(pair0));
     * assert(pair1.f() == 4);
     */
    template <typename U0 = T0, typename U1 = T1,
        std::enable_if_t<std::is_constructible_v<T0, U0&&> && std::is_constructible_v<T1, U1&&>>* = nullptr>
    ALT_FORCE_INLINE Pair(Pair<U0, U1>&& pair) noexcept
        : _f(std::move(pair._f))
        , _s(std::move(pair._s))
    {}

    template <typename U0 = T0, typename U1 = T1,
        std::enable_if_t<std::is_constructible_v<T0, const U0&> && std::is_constructible_v<T1, const U1&>>* = nullptr>
    ALT_FORCE_INLINE Pair(const Pair<U0, U1>& pair) noexcept
        : _f(pair._f)
        , _s(pair._s)
    {}

    /**
     * @brief Assigns a pair to another pair.
     * 
     * @tparam U0 The type of the first element in the source pair. Must be assignable to T0.
     * @tparam U1 The type of the second element in the source pair. Must be assignable to T1.
     * @param pair The source pair to move from.
     * @return A reference to this instance.
     * @example
     * 
     * Alt::Pair<usize, usize> pair0(4, 8);
     * Alt::Pair<usize, usize> pair1(16, 24);
     * 
     * pair0 = std::move(p2);
     * assert(pair0.f() == 16);
     */
    template <typename U0 = T0, typename U1 = T1,
        std::enable_if_t<std::is_assignable_v<T0&, U0&&> && std::is_assignable_v<T1&, U1&&>>* = nullptr>
    ALT_FORCE_INLINE Pair& operator=(Pair<U0, U1>&& pair) noexcept {

        _f = std::move(pair._f);
        _s = std::move(pair._s);

        return *this;
    }

    template <typename U0 = T0, typename U1 = T1,
        std::enable_if_t<std::is_assignable_v<T0&, const U0&> && std::is_assignable_v<T1&, const U1&>>* = nullptr>
    ALT_FORCE_INLINE Pair& operator=(const Pair<U0, U1>& pair) noexcept {

        _f = pair._f;
        _s = pair._s;

        return *this;
    }

    /**
     * Clonable Trait
     * 
     * @brief Clones the current pair.
     * 
     * @tparam U0 The type of the first element in the source pair. Must be clonable or assignable to T0.
     * @tparam U1 The type of the second element in the source pair. Must be clonable or assignable to T1.
     * @param pair The destination pair to clone.
     * @return A bool, false when clone failed.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
     * Alt::Pair<Alt::Vector<usize>, usize> pair0{ std::move(vec), 0 };
     * Alt::Pair<Alt::Vector<usize>, usize> pair1 = std::move(pair0);
     * pair0.CloneFrom(pair1) iferr_return;
     * 
     * assert(pair0.f(), pair1.f());
     */
    template <typename U0 = T0, typename U1 = T1,
        std::enable_if_t<
            (TypeTraits::is_only_clonable_to_from_v<T0, const U0&> || std::is_assignable_v<T0&, const U0&>) &&
            (TypeTraits::is_only_clonable_to_from_v<T1, const U1&> || std::is_assignable_v<T1&, const U1&>)
        >* = nullptr>
    ALT_NODISCARD bool CloneFrom(const Pair<U0, U1>& rhs) noexcept {

        const T0* pResult0 = Memory::CloneOrMoveObject(_f, rhs._f);
        if (pResult0 == &_f) {

            return false;
        }

        const T1* pResult1 = Memory::CloneOrMoveObject(_s, rhs._s);
        if (pResult1 == &_s) {

            return false;
        }

        return true;
    }

    template <typename U0 = T0, typename U1 = T1,
        std::enable_if_t<
            (TypeTraits::is_only_clonable_to_from_v<T0, const U0&> || std::is_constructible_v<T0, const U0&>) &&
            (TypeTraits::is_only_clonable_to_from_v<T1, const U1&> || std::is_constructible_v<T1, const U1&>)
        >* = nullptr>
    ALT_NODISCARD bool CloneConstructFrom(const Pair<U0, U1>& rhs) noexcept {

        const T0* pResult0 = Memory::CloneConstructOrMoveConstructObject(_f, rhs._f);
        if (pResult0 == &_f) {

            return false;
        }

        const T1* pResult1 = Memory::CloneConstructOrMoveConstructObject(_s, rhs._s);
        if (pResult1 == &_s) {

            Memory::DestroyObject(_f);
            return false;
        }

        return true;
    }

    ALT_NODISCARD Optional<Pair> Clone() const noexcept {

        struct Object {

            ~Object() noexcept {

                if (!isValid) {
                    return;
                }

                Pair* pObject = reinterpret_cast<Pair*>(memory);
                Memory::DestroyObject(pObject);

                isValid = false;
            }

            alignas(Pair) u8 memory[sizeof(Pair)];
            bool isValid = false;
        };

        Object object{};
        Pair* pObject = reinterpret_cast<Pair*>(object.memory);

        const bool isOk = pObject->CloneConstructFrom(*this);
        object.isValid = isOk;

        if (!isOk) {
            return Nullopt;
        }

        return MakeOptional(std::move(*pObject));
    }

    /**
     * @brief Compares two pairs for equality.
     * 
     * @tparam U0 The type of the first element in the right-hand side pair.
     * @tparam U1 The type of the second element in the right-hand side pair.
     * @param rhs The pair to compare against.
     * @return True if both elements are equal, false otherwise.
     * @example
     * 
     * Alt::Pair<usize, usize> pair0(1, 2);
     * Alt::Pair<usize, usize> pair1(1, 2);
     * assert(pair0 == pair2);
     */
    template <typename U0 = T0, typename U1 = T1>
    ALT_FORCE_INLINE bool operator==(const Pair<U0, U1>& rhs) const noexcept {
        return _f == rhs._f && _s == rhs._s;
    }

    /**
     * @brief Compares two pairs for inequality.
     * 
     * @tparam U0 The type of the first element in the right-hand side pair.
     * @tparam U1 The type of the second element in the right-hand side pair.
     * @param rhs The pair to compare against.
     * @return True if the pairs are not equal, false otherwise.
     * @example
     * 
     * Alt::Pair<usize, usize> pair0(1, 2);
     * Alt::Pair<usize, usize> pair1(1, 3);
     * assert(pair0 != pair2);
     */
    template <typename U0 = T0, typename U1 = T1>
    ALT_FORCE_INLINE bool operator!=(const Pair<U0, U1>& rhs) const noexcept {
        return !(*this == rhs);
    }

    /**
     * @brief Compares two pairs using lexicographical order.
     * 
     * @tparam U0 The type of the first element in the right-hand side pair.
     * @tparam U1 The type of the second element in the right-hand side pair.
     * @param rhs The pair to compare against.
     * @return True if the first element of this pair is less than the first element of rhs, 
     *         or if they are equal and the second element of this pair is less than the second of rhs.
     * @example
     * 
     * Alt::Pair<usize, usize> pair0(1, 2);
     * Alt::Pair<usize, usize> pair1(1, 3);
     * assert(pair0 < pair2);
     */
    template <typename U0 = T0, typename U1 = T1>
    ALT_FORCE_INLINE bool operator<(const Pair<U0, U1>& rhs) const noexcept {
        return _f < rhs._f || (_f <= rhs._f && _s < rhs._s);
    }

    /**
     * @brief Compares two pairs for greater-than relationship.
     * 
     * @tparam U0 The type of the first element in the right-hand side pair.
     * @tparam U1 The type of the second element in the right-hand side pair.
     * @param rhs The pair to compare against.
     * @return True if this pair is greater than rhs.
     * @example
     * 
     * Alt::Pair<usize, usize> pair0(2, 1);
     * Alt::Pair<usize, usize> pair1(1, 2);
     * assert(pair0 > pair2);
     */
    template <typename U0 = T0, typename U1 = T1>
    ALT_FORCE_INLINE bool operator>(const Pair<U0, U1>& rhs) const noexcept {
        return rhs < *this;
    }

    /**
     * @brief Compares two pairs for less-than-or-equal relationship.
     * 
     * @tparam U0 The type of the first element in the right-hand side pair.
     * @tparam U1 The type of the second element in the right-hand side pair.
     * @param rhs The pair to compare against.
     * @return True if this pair is less than or equal to rhs.
     * @example
     * 
     * Alt::Pair<usize, usize> pair0(1, 2);
     * Alt::Pair<usize, usize> pair1(1, 2);
     * assert(pair0 <= pair2);
     */
    template <typename U0 = T0, typename U1 = T1>
    ALT_FORCE_INLINE bool operator<=(const Pair<U0, U1>& rhs) const noexcept {
        return !(*this > rhs);
    }

    /**
     * @brief Compares two pairs for greater-than-or-equal relationship.
     * 
     * @tparam U0 The type of the first element in the right-hand side pair.
     * @tparam U1 The type of the second element in the right-hand side pair.
     * @param rhs The pair to compare against.
     * @return True if this pair is greater than or equal to rhs.
     * @example
     * 
     * Alt::Pair<usize, usize> pair0(1, 2);
     * Alt::Pair<usize, usize> pair1(1, 1);
     * assert(pair0 >= pair2);
     */
    template <typename U0 = T0, typename U1 = T1>
    ALT_FORCE_INLINE bool operator>=(const Pair<U0, U1>& rhs) const noexcept {
        return !(*this < rhs);
    }

    /**
     * @brief Allows to use structered bindings.
     * @example
     * 
     * Alt::Pair<usize, Alt::String> pair{ 0, "String"_salloc };
     * auto& [x, str] = pair;
     * 
     * assert(x == pair.f());
     * assert(str == pair.s());
     * assert(str.Data() == pair.s().Data());
     */
    template <std::size_t Index>
    ALT_FORCE_INLINE auto& get() & noexcept { return GetSTDStructuredBindings<Index>(*this); }

    template <std::size_t Index>
    ALT_FORCE_INLINE auto const& get() const & noexcept { return GetSTDStructuredBindings<Index>(*this); }

    template <std::size_t Index>
    ALT_FORCE_INLINE auto&& get() && noexcept { return GetSTDStructuredBindings<Index>(*this); }

    template <std::size_t Index>
    ALT_FORCE_INLINE auto&& get() const && noexcept { return GetSTDStructuredBindings<Index>(*this); }

private:

    template <std::size_t Index, typename SelfT>
    ALT_FORCE_INLINE static auto&& GetSTDStructuredBindings(SelfT&& self) noexcept {
        if constexpr (Index == 0) return std::forward<SelfT>(self)._f;
        if constexpr (Index == 1) return std::forward<SelfT>(self)._s;
    }

private:

    template <typename, typename>
    friend class Pair;

    T0 _f;
    T1 _s;
};

} // namespace Alt

/*
 * Helpers for structured bindings
 */
namespace std {

    template <typename T0, typename T1>
    struct tuple_size<Alt::Pair<T0, T1>>
        : std::integral_constant<size_t, 2>
    {};

    template <typename T0, typename T1>
    struct tuple_element<0, Alt::Pair<T0, T1>> {
        using type = T0;
    };

    template <typename T0, typename T1>
    struct tuple_element<1, Alt::Pair<T0, T1>> {
        using type = T1;
    };

} // namespace std

#endif // __ALT_STD_PAIR__