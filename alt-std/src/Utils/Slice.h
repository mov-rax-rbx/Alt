#pragma once

#ifndef __ALT_STD_SLICE__
#define __ALT_STD_SLICE__

#include "../IfErr.h"
#include "../TypeTraits.h"

#include "../Iterators/Iterator.h"
#include "../Algorithms/Algorithms.h"

#include "../String/Impl/StringDetails.h"
#include "../String/Impl/Utf/Utf8.h"

namespace Alt {

/** \cond */
namespace _SliceDetails {

enum class InnerTypeCheckTrait {

    None,
    CppData,
    AltData,
    AltCStr,
};

template <InnerTypeCheckTrait TRAIT>
using InnerTypeCheck = std::integral_constant<InnerTypeCheckTrait, TRAIT>;

template <typename T, typename = void>
struct __has_cppdata_method_for_inner_type_check__
    : std::false_type
{};

template <typename T>
struct __has_cppdata_method_for_inner_type_check__<T, std::void_t<std::remove_pointer_t<decltype(std::declval<T&>().data())>>>
    : std::true_type
{};

template <typename T, typename = void>
struct __has_altdata_method_for_inner_type_check__
    : std::false_type
{};

template <typename T>
struct __has_altdata_method_for_inner_type_check__<T, std::void_t<std::remove_pointer_t<decltype(std::declval<T&>().Data())>>>
    : std::true_type
{};

template <typename T, typename = void>
struct __has_altcstr_method_for_inner_type_check__
    : std::false_type
{};

template <typename T>
struct __has_altcstr_method_for_inner_type_check__<T, std::void_t<std::remove_pointer_t<decltype(std::declval<T&>().CStr())>>>
    : std::true_type
{};

template <
    typename T,
    InnerTypeCheckTrait TRAIT =
        std::conditional_t<
            __has_cppdata_method_for_inner_type_check__<T>::value,
            InnerTypeCheck<InnerTypeCheckTrait::CppData>,

            std::conditional_t<
                // firstly check for 'string slice'
                __has_altcstr_method_for_inner_type_check__<T>::value,
                InnerTypeCheck<InnerTypeCheckTrait::AltCStr>,

                std::conditional_t<
                    __has_altdata_method_for_inner_type_check__<T>::value,
                    InnerTypeCheck<InnerTypeCheckTrait::AltData>,

                    InnerTypeCheck<InnerTypeCheckTrait::None>
                >
            >
        >::value
>
struct __unwrap_inner_type_from_like_array__ {

    using type = Unit_t;
};

template <typename T>
struct __unwrap_inner_type_from_like_array__<T, InnerTypeCheckTrait::CppData> {

    using type = TypeTraits::clean_t<std::remove_pointer_t<decltype(std::declval<T&>().data())>>;
};

template <typename T>
struct __unwrap_inner_type_from_like_array__<T, InnerTypeCheckTrait::AltData> {

    using type = TypeTraits::clean_t<std::remove_pointer_t<decltype(std::declval<T&>().Data())>>;
};

template <typename T>
struct __unwrap_inner_type_from_like_array__<T, InnerTypeCheckTrait::AltCStr> {

    using type = TypeTraits::clean_t<std::remove_pointer_t<decltype(std::declval<T&>().CStr())>>;
};

template <typename T>
constexpr static bool is_string_slice_v = TypeTraits::is_compatible_with_character_v<T>;

} // namespace _SliceDetails
/** \endcond */

template <typename T, bool IS_STRING_SLICE = _SliceDetails::is_string_slice_v<T>>
class Slice;

template <typename T>
class SliceSTDIterator;

template <typename T>
class SliceIterator;

template <typename T>
class SliceIteratorConst;

template <Utf::Encoding ENCODING>
class SliceIteratorUTF;

template <Utf::Encoding ENCODING>
class SliceIteratorUTFTrusted;

using SliceIteratorUTF8 = SliceIteratorUTF<Utf::Encoding::Utf8>;
using SliceIteratorUTF8Trusted = SliceIteratorUTFTrusted<Utf::Encoding::Utf8>;

using SliceIteratorUTF16 = SliceIteratorUTF<Utf::Encoding::Utf16>;
using SliceIteratorUTF16Trusted = SliceIteratorUTFTrusted<Utf::Encoding::Utf16>;

using SliceIteratorUTF32 = SliceIteratorUTF<Utf::Encoding::Utf32>;
using SliceIteratorUTF32Trusted = SliceIteratorUTFTrusted<Utf::Encoding::Utf32>;

template <typename T, bool IS_STRING_SLICE = _SliceDetails::is_string_slice_v<T>>
class SplitIterator;

namespace TypeTraits {

template <typename T, bool IS_STRING_SLICE>
struct __is_trivially_reallocatable__<Slice<T, IS_STRING_SLICE>>
    : is_trivially_reallocatable_t<T*>
{};

/*
 * template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
 *    std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_v<InnerU>>* = nullptr>
 * void func(U&& val) noexcept {
 * 
 *     const Slice<const InnerU> slice = val;
 * }
*/ 
template <typename T>
using unwrap_inner_type_from_like_array_t = typename _SliceDetails::__unwrap_inner_type_from_like_array__<T>::type;

} // namespace TypeTraits

template <typename T>
class Slice<T, true>;

/**
 * @brief Represents a non-owning, contiguous view of a range of elements of type T.
 * @details A Slice acts as a window into an existing buffer (like an array or a vector),
 * providing a safe and efficient way to manipulate sub-sections of memory without 
 * performing allocations or copies.
 * 
 * @tparam T The type of the elements contained in the slice.
 * 
 * @example
 * 
 * usize arr[]{ 10, 20, 30, 40, 50 };
 * Alt::Slice<usize> slice{ arr };
 * 
 * assert(slice.GetSize() == 5);
 * assert(slice[1] == 20);
 * 
 * Alt::Slice<usize> sub = slice.SubSlice(1, 3); // Represents {20, 30, 40}
 * 
 * assert(sub.GetSize() == 3);
 * assert(sub[2] == 40);
 */
template <typename T>
class Slice<T, false> {
public:

    using iterator = SliceSTDIterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    /**
     * @brief Initializes an empty slice.
     */
    constexpr ALT_FORCE_INLINE Slice() noexcept = default;

    /**
     * @brief Initializes a slice from a pointer and a size.
     * @param begin Pointer to the start of the range.
     * @param size The number of elements in the range.
     * @example
     * 
     * usize arr[]{ 1, 2, 3 };
     * 
     * assert(slice.GetSize() == 3);
     */
    constexpr ALT_FORCE_INLINE Slice(T* begin, const usize size) noexcept
        : m_pBegin(begin)
        , m_pEnd(begin + size)
    {}

    /**
     * @brief Initializes a slice from a start and end pointer.
     * @param begin Pointer to the start of the range.
     * @param end Pointer to the end of the range (exclusive).
     * @example
     * 
     * usize arr[]{ 1, 2, 3 };
     * Alt::Slice<usize> slice{ arr, arr + 3 };
     * 
     * assert(slice.GetSize() == 3);
     */
    constexpr ALT_FORCE_INLINE Slice(T* begin, T* end) noexcept
        : m_pBegin(begin)
        , m_pEnd(end)
    {}

    /**
     * @brief Initializes a slice from a fixed-size array.
     * @tparam N The size of the array.
     * @param a Reference to the array.
     * @example
     * 
     * usize arr[]{ 1, 2, 3 };
     * Alt::Slice<usize> slice(arr);
     * 
     * assert(slice.GetSize() == 3);
     */
    template <usize N>
    constexpr ALT_FORCE_INLINE Slice(T (&arr)[N]) noexcept
        : m_pBegin(arr)
        , m_pEnd(arr + N)
    {}

    /**
     * @brief Initializes a slice from a container using .data() and .size().
     * @tparam Container A type providing .data() and .size() methods. 
     * The return value of .data() must be convertible to T*.
     * @param container The container to wrap.
     * @example
     * 
     * std::vector<usize> vec{ 1, 2, 3 };
     * Alt::Slice<usize> slice(vec);
     * 
     * assert(slice.GetSize() == 3);
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().data())>*, T*>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.data()))
        , m_pEnd(static_cast<T*>(container.data()) + container.size())
    {}

    /**
     * @brief Initializes a slice from a const container using .data() and .size().
     * @tparam Container A type providing .data() and .size() methods. 
     * The return value of .data() must be convertible to T*.
     * @param container The constant container to wrap.
     * @example
     * 
     * const std::vector<usize> vec{ 1, 2, 3 };
     * Alt::Slice<usize> slice(vec);
     * 
     * assert(slice.GetSize() == 3);
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().data())>*, T*>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(const Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.data()))
        , m_pEnd(static_cast<T*>(container.data()) + container.size())
    {}

    /**
     * @brief Initializes a slice from a container using .Data() and .DataEnd().
     * @tparam Container A type providing .Data() and .DataEnd() methods. 
     * The return value of .Data() must be convertible to T*.
     * @param container The container to wrap.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(1, 2, 3) iferr_panic("Allocation failed");
     * Alt::Slice<usize> slice(vec);
     * 
     * assert(slice.GetSize() == 3);
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().Data())>*, T*> &&
            !TypeTraits::is_string_v<Container>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.Data()))
        , m_pEnd(static_cast<T*>(container.DataEnd()))
    {}

    /**
     * @brief Initializes a slice from a const container using .Data() and .DataEnd().
     * @tparam Container A type providing .Data() and .DataEnd() methods. 
     * The return value of .Data() must be convertible to T*.
     * @param container The constant container to wrap.
     * @example
     * 
     * const Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(1, 2, 3) iferr_panic("Allocation failed");
     * Alt::Slice<usize> slice(vec);
     * 
     * assert(slice.GetSize() == 3);
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().Data())>*, T*> &&
            !TypeTraits::is_string_v<Container>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(const Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.Data()))
        , m_pEnd(static_cast<T*>(container.DataEnd()))
    {}

    /**
     * @brief Initializes a slice from another Slice of a compatible type.
     * @tparam U The type of the source Slice. Must be convertible to T*.
     * @param rhs The source slice.
     * @example
     * 
     * usize arr[]{ 1, 2, 3 };
     * Alt::Slice<usize> slice(arr);
     * 
     * Alt::Slice<usize> sliceCopy(slice);
     * 
     * assert(sliceCopy.GetSize() == 3);
     */
    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U*, T*>>* = nullptr>
    ALT_FORCE_INLINE Slice(const Slice<U> rhs) noexcept
        : m_pBegin(static_cast<T*>(rhs.m_pBegin))
        , m_pEnd(static_cast<T*>(rhs.m_pEnd))
    {}

    /**
     * @brief Assigns a compatible slice to this slice.
     * @tparam U The type of the source Slice. Must be convertible to T*.
     * @param rhs The source slice.
     * @return Reference to this slice.
     * @example
     * 
     * usize arr0[]{ 1, 2, 3 };
     * Alt::Slice<usize> slice0(arr0);
     * 
     * usize arr1[]{ 4, 5, 6 };
     * Alt::Slice<usize> slice1(arr1);
     * 
     * Alt::Slice<usize> s0 = slice0;
     * Alt::Slice<usize> s1 = slice1;
     * 
     * s1 = s0;
     * assert(s1.GetSize() == 3);
     * assert(s1[0] == 1);
     * assert(s1[1] == 2);
     * assert(s1[3] == 3);
     */
    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U*, T*>>* = nullptr>
    ALT_FORCE_INLINE Slice& operator=(const Slice<U> rhs) noexcept {

        m_pBegin = static_cast<T*>(rhs.m_pBegin);
        m_pEnd = static_cast<T*>(rhs.m_pEnd);

        return *this;
    }

    ALT_FORCE_INLINE Slice(const Slice&) noexcept = default;
    ALT_FORCE_INLINE Slice& operator=(const Slice&) noexcept = default;

    /**
     * @brief Returns the number of elements in the slice.
     * @return The element count.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetSize() const noexcept { return static_cast<usize>(m_pEnd - m_pBegin); }

    /**
     * @brief Returns the total size of the slice in bytes.
     * @return Total bytes.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetSizeBytes() const noexcept { return GetSize() * sizeof(T); }

    /**
     * @brief Checks if the slice is empty.
     * @return True if the slice contains no elements, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsEmpty() const noexcept { return m_pBegin == m_pEnd; }

    /**
     * @brief Checks if the slice is contains at least one element.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPopulated() const noexcept { return m_pBegin != m_pEnd; }

    /**
     * @brief Creates a new Slice representing of the current slice.
     * @param offset The starting index within the current slice.
     * @param size The number of elements to include. If max, includes until the end.
     * @return A new Sub Slice.
     * 
     * @note The offset and size must be within the bounds of the current slice.
     * 
     * @example
     * 
     * usize arr[]{ 0, 1, 2, 3, 4 };
     * Alt::Slice<usize> slice(arr, 5);
     * 
     * Alt::Slice<usize> sub = slice.SubSlice(1, 2); // {1, 2}
     * 
     * assert(sub.GetSize() == 2);
     * assert(sub[0] == 1);
     * assert(sub[1] == 2);
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr Slice<T> SubSlice(const usize offset, const usize size = Num::Desc<usize>::Max()) const noexcept {
        ALT_ASSERT(GetSize() >= offset && (size == Num::Desc<usize>::Max() || size <= GetSize() - offset));

        T* pBegin = m_pBegin + offset;
        T* pEnd = size == Num::Desc<usize>::Max()
            ? m_pEnd
            : pBegin + size;

        return Slice<T>{ pBegin, pEnd };
    }

    /**
     * @brief Creates a new Slice<u8> representing of the current slice.
     * @return A new byte Slice.
     * @example
     * 
     * usize arr[]{ 0, 1, 2, 3, 4 };
     * Alt::Slice<usize> slice(arr, 5);
     * 
     * Alt::Slice<const u8> bytes = slice.CastToBytes();
     */
    ALT_NODISCARD ALT_FORCE_INLINE Slice<TypeTraits::preserve_qualifiers_t<T, u8>> CastToBytes() noexcept {

        using ByteT = TypeTraits::preserve_qualifiers_t<T, u8>;

        return Slice<ByteT>(reinterpret_cast<ByteT*>(m_pBegin), reinterpret_cast<ByteT*>(m_pEnd));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Slice<const TypeTraits::preserve_qualifiers_t<T, u8>> CastToBytes() const noexcept {

        using ByteT = const TypeTraits::preserve_qualifiers_t<T, u8>;

        return Slice<ByteT>(reinterpret_cast<ByteT*>(m_pBegin), reinterpret_cast<ByteT*>(m_pEnd));
    }

    /**
     * @brief Accesses the element at the specified index.
     * @param idx The index of the element.
     * @return Reference to the element.
     * 
     * @note The index must be less than GetSize().
     * 
     * @example
     * 
     * usize arr[]{ 0, 1, 2, 3, 4 };
     * Alt::Slice<usize> slice{ arr };
     * 
     * assert(s[4] == 4);
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize idx) noexcept {
        ALT_ASSERT(idx < GetSize());

        return m_pBegin[idx];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize idx) const noexcept {
        ALT_ASSERT(idx < GetSize());

        return m_pBegin[idx];
    }
    
    /**
     * @brief Returns a reference to the first element.
     * @return Reference to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetFirst() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pBegin;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetFirst() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pBegin;
    }

    /**
     * @brief Returns a reference to the last element.
     * @return Reference to the last element.
     * @note Asserts that the Slice is not empty.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetLast() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(m_pEnd - 1);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetLast() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(m_pEnd - 1);
    }

    /**
     * @brief Returns a raw pointer to the beginning of the slice.
     * @return Pointer to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr T* Data() noexcept { return m_pBegin; }
    ALT_NODISCARD ALT_FORCE_INLINE constexpr const T* Data() const noexcept { return m_pBegin; }

    /**
     * @brief Returns a raw pointer to the end of the slice.
     * @return Pointer to the next after last element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr T* DataEnd() noexcept { return m_pEnd; }
    ALT_NODISCARD ALT_FORCE_INLINE constexpr const T* DataEnd() const noexcept { return m_pEnd; }

    /**
     * @brief Operators for comparison with Slice.
     * @example
     *
     * auto array = Alt::MakeArray(0, 1);
     * 
     * Alt::Slice<usize> slice0 = array;
     * Alt::Slice<usize> slice1 = array;
     * 
     * assert(slice0 == slice1);
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_same_v<InnerU, T> && std::is_constructible_v<Slice<const T>, U&&>>* = nullptr>
    ALT_NODISCARD bool operator==(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareEQ(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_same_v<InnerU, T> && std::is_constructible_v<Slice<const T>, U&&>>* = nullptr>
    ALT_NODISCARD bool operator!=(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareNE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_same_v<InnerU, T> && std::is_constructible_v<Slice<const T>, U&&>>* = nullptr>
    ALT_NODISCARD bool operator<(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareLT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_same_v<InnerU, T> && std::is_constructible_v<Slice<const T>, U&&>>* = nullptr>
    ALT_NODISCARD bool operator<=(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareLE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_same_v<InnerU, T> && std::is_constructible_v<Slice<const T>, U&&>>* = nullptr>
    ALT_NODISCARD bool operator>(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareGT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_same_v<InnerU, T> && std::is_constructible_v<Slice<const T>, U&&>>* = nullptr>
    ALT_NODISCARD bool operator>=(U&& rhs_) const noexcept {

        const Slice<const T> rhs = rhs_;
        return Algorithms::CompareGE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    /**
     * @brief Returns std iterators.
     * @return Iterator pointing to the first element.
     */
    ALT_FORCE_INLINE iterator begin() const noexcept { return iterator{ m_pBegin }; }
    ALT_FORCE_INLINE iterator end() const noexcept { return iterator{ m_pEnd}; }

    ALT_FORCE_INLINE reverse_iterator rbegin() const noexcept { return reverse_iterator{ end() }; }
    ALT_FORCE_INLINE reverse_iterator rend() const noexcept { return reverse_iterator{ begin() }; }

    /**
     * @brief Return a custom Alt::Iterator for the slice.
     * @return An Iterator instance.
     */
    ALT_NODISCARD ALT_FORCE_INLINE SliceIterator<T> GetIterator() noexcept {
        return SliceIterator<T>{ *this };
    }

    ALT_NODISCARD ALT_FORCE_INLINE SliceIteratorConst<T> GetIterator() const noexcept {
        return SliceIteratorConst<T>{ *this };
    }

private:

    template <typename, bool>
    friend class Slice;

    T* m_pBegin = nullptr;
    T* m_pEnd = nullptr;
};

/**
 * @brief Specialized slice for character types.
 * @details This specialization handles C-strings and other char-based representations, 
 * including specific logic to calculate lengths of null-terminated strings.
 */
template <typename T>
class Slice<T, true> {
public:

    using iterator = SliceSTDIterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;

    /**
     * @brief Initializes an empty slice.
     */
    constexpr ALT_FORCE_INLINE Slice() noexcept = default;

    /**
     * @brief Initializes a slice from a pointer and a size.
     * @param begin Pointer to the start of the range.
     * @param size The number of elements in the range.
     * @example
     * 
     * const char* name = "Slice";
     * Alt::Slice<const char> slice{ name, Alt::String::CalculateCStrLen(name) };
     * 
     * assert(slice == name);
     */
    constexpr ALT_FORCE_INLINE Slice(T* begin, const usize size) noexcept
        : m_pBegin(begin)
        , m_pEnd(begin + size)
    {}

    /**
     * @brief Initializes a slice from a start and end pointer.
     * @param begin Pointer to the start of the range.
     * @param end Pointer to the end of the range (exclusive).
     * @example
     * 
     * const char* name = "Slice";
     * Alt::Slice<const char> slice{ name, name + Alt::String::CalculateCStrLen(name) };
     * 
     * assert(slice == name);
     */
    constexpr ALT_FORCE_INLINE Slice(T* begin, T* end) noexcept
        : m_pBegin(begin)
        , m_pEnd(end)
    {}

    /**
     * @brief Initializes a slice from a fixed-size array.
     * @tparam N The size of the array.
     * @param a Reference to the array.
     * @example
     * 
     * const char name[]{ "Slice" };
     * Alt::Slice<const char> slice = "name"
     * 
     * assert(slice == name);
     */
    template <usize N>
    constexpr ALT_FORCE_INLINE Slice(T (&arr)[N]) noexcept
        : m_pBegin(arr)
        , m_pEnd(arr + N - 1)
    {}

    /**
     * @brief Initializes a slice from a container using .data() and .size().
     * @tparam Container A type providing .data() and .size() methods. 
     * The return value of .data() must be convertible to T*.
     * @param container The container to wrap.
     * @example
     * 
     * std::string name = "Slice";
     * Alt::Slice<const char> slice = name;
     * 
     * assert(slice == name.c_str());
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().data())>*, T*>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.data()))
        , m_pEnd(static_cast<T*>(container.data()) + container.size())
    {}

    /**
     * @brief Initializes a slice from a const container using .data() and .size().
     * @tparam Container A type providing .data() and .size() methods. 
     * The return value of .data() must be convertible to T*.
     * @param container The constant container to wrap.
     * @example
     * 
     * std::string name = "Slice";
     * Alt::Slice<const char> slice = name;
     * 
     * assert(slice == name.c_str());
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().data())>*, T*>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(const Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.data()))
        , m_pEnd(static_cast<T*>(container.data()) + container.size())
    {}

    /**
     * @brief Initializes a slice from a container using .CStr() and .GetSize().
     * @tparam Container A type providing .CStr() and .CStrEnd() methods. 
     * The return value of .CStr() must be convertible to T*.
     * @param container The container to wrap.
     * @example
     * 
     * Alt::String name = "Slice"_salloc;
     * Alt::Slice<const char> slice = name;
     * 
     * assert(slice == name);
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().CStr())>*, T*> &&
            TypeTraits::is_string_v<Container>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.CStr()))
        , m_pEnd(static_cast<T*>(container.CStrEnd()))
    {}

    /**
     * @brief Initializes a slice from a container using .Data() and .DataEnd().
     * @tparam Container A type providing .Data() and .DataEnd() methods. 
     * The return value of .Data() must be convertible to T*.
     * @param container The container to wrap.
     * @example
     * 
     * auto name = Alt::MakeArray('S', 'l', 'i', 'c', 'e');
     * Alt::Slice<const char> slice = name;
     * 
     * assert(slice == name.Data());
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().Data())>*, T*> &&
            !TypeTraits::is_string_v<Container>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.Data()))
        , m_pEnd(static_cast<T*>(container.DataEnd()))
    {}

    /**
     * @brief Initializes a slice from a const container using .Data() and .GetSize().
     * @tparam Container A type providing .Data() and .CStrEnd() methods. 
     * The return value of .Data() must be convertible to T*.
     * @param container The constant container to wrap.
     * @example
     * 
     * std::string name = "Slice";
     * Alt::Slice<const char> slice = name;
     * 
     * assert(slice == name.c_str());
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().CStr())>*, T*> &&
            TypeTraits::is_string_v<Container>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(const Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.CStr()))
        , m_pEnd(static_cast<T*>(container.CStrEnd()))
    {}

    /**
     * @brief Initializes a slice from a container using .Data() and .DataEnd().
     * @tparam Container A type providing .Data() and .DataEnd() methods. 
     * The return value of .Data() must be convertible to T*.
     * @param container The container to wrap.
     * @example
     * 
     * auto name = Alt::MakeArray('S', 'l', 'i', 'c', 'e');
     * Alt::Slice<const char> slice = name;
     * 
     * assert(slice == name.Data());
     */
    template <typename Container,
        std::enable_if_t<
            std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<Container&>().Data())>*, T*> &&
            !TypeTraits::is_string_v<Container>
        >* = nullptr>
    ALT_FORCE_INLINE Slice(const Container& container) noexcept
        : m_pBegin(static_cast<T*>(container.Data()))
        , m_pEnd(static_cast<T*>(container.DataEnd()))
    {}

    /**
     * @brief Initializes a slice from another Slice of a compatible type.
     * @tparam U The type of the source Slice. Must be convertible to T*.
     * @param rhs The source slice.
     * @example
     * 
     * auto arr = Alt::MakeArray('S', 'l', 'i', 'c', 'e');
     * Alt::Slice<const char> slice(arr);
     * 
     * Alt::Slice<const char> sliceCopy(slice);
     * assert(sliceCopy.GetSize() == arr.GetSize());
     */
    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U*, T*>>* = nullptr>
    ALT_FORCE_INLINE Slice(const Slice<U> rhs) noexcept
        : m_pBegin(static_cast<T*>(rhs.m_pBegin))
        , m_pEnd(static_cast<T*>(rhs.m_pEnd))
    {}

    /**
     * @brief Assigns a compatible slice to this slice.
     * @tparam U The type of the source Slice. Must be convertible to T*.
     * @param rhs The source slice.
     * @return Reference to this slice.
     * @example
     * 
     * Alt::Array<char, 32> arr0{ 'S', 'l', 'i' };
     * Alt::Slice<const char> slice0(arr0);
     * 
     * Alt::Array<char, 32> arr1{ 'c', 'e' };
     * Alt::Slice<const char> slice1(arr1);
     * 
     * Alt::Slice<const char> s0 = slice0;
     * Alt::Slice<const char> s1 = slice1;
     * 
     * s1 = s0;
     * assert(s1.GetSize() == 2);
     * assert(s1[0] == 'c');
     * assert(s1[1] == 'e');
     */
    template <typename U = T,
        std::enable_if_t<std::is_convertible_v<U*, T*>>* = nullptr>
    ALT_FORCE_INLINE Slice& operator=(const Slice<U> rhs) noexcept {

        m_pBegin = static_cast<T*>(rhs.m_pBegin);
        m_pEnd = static_cast<T*>(rhs.m_pEnd);

        return *this;
    }

    ALT_FORCE_INLINE Slice(const Slice&) noexcept = default;
    ALT_FORCE_INLINE Slice& operator=(const Slice&) noexcept = default;

    /**
     * @brief Returns the number of elements in the slice.
     * @return The element count.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetSize() const noexcept { return static_cast<usize>(m_pEnd - m_pBegin); }

    /**
     * @brief Returns the total size of the slice in bytes.
     * @return Total bytes.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetSizeBytes() const noexcept { return GetSize() * sizeof(T); }

    /**
     * @brief Checks if the slice is empty.
     * @return True if the slice contains no elements, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsEmpty() const noexcept { return m_pBegin == m_pEnd; }

    /**
     * @brief Checks if the slice is contains at least one element.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsPopulated() const noexcept { return m_pBegin != m_pEnd; }

    /**
     * @brief Creates a new Slice representing of the current slice.
     * @param offset The starting index within the current slice.
     * @param size The number of elements to include. If max, includes until the end.
     * @return A new Sub Slice.
     * 
     * @note The offset and size must be within the bounds of the current slice.
     * 
     * @example
     * 
     * Alt::Slice<const char> slice = "Slice"_s;
     * 
     * Alt::Slice<const char> sub = slice.SubSlice(1, 2); // "li"
     * 
     * assert(sub.GetSize() == 2);
     * assert(sub[0] == 'l');
     * assert(sub[1] == 'i');
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr Slice<T> SubSlice(const usize offset, const usize size = Num::Desc<usize>::Max()) const noexcept {
        ALT_ASSERT(GetSize() >= offset && (size == Num::Desc<usize>::Max() || size <= GetSize() - offset));

        T* pBegin = m_pBegin + offset;
        T* pEnd = size == Num::Desc<usize>::Max()
            ? m_pEnd
            : pBegin + size;

        return Slice<T>{ pBegin, pEnd };
    }

    /**
     * @brief Creates a new Slice<u8> representing of the current slice.
     * @return A new byte Slice.
     * @example
     * 
     * usize arr[]{ 0, 1, 2, 3, 4 };
     * Alt::Slice<usize> slice(arr, 5);
     * 
     * Alt::Slice<const u8> bytes = slice.CastToBytes();
     */
    ALT_NODISCARD ALT_FORCE_INLINE Slice<TypeTraits::preserve_qualifiers_t<T, u8>> CastToBytes() noexcept {

        using ByteT = TypeTraits::preserve_qualifiers_t<T, u8>;

        return Slice<ByteT>(reinterpret_cast<ByteT*>(m_pBegin), reinterpret_cast<ByteT*>(m_pEnd));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Slice<const TypeTraits::preserve_qualifiers_t<T, u8>> CastToBytes() const noexcept {

        using ByteT = const TypeTraits::preserve_qualifiers_t<T, u8>;

        return Slice<ByteT>(reinterpret_cast<ByteT*>(m_pBegin), reinterpret_cast<ByteT*>(m_pEnd));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Slice<TypeTraits::character_to_character_compatible_type_t<T>> CastToCompatibleChar() noexcept {

        using CompatibleCharacterT = TypeTraits::character_to_character_compatible_type_t<T>;
        return Slice<CompatibleCharacterT>(reinterpret_cast<CompatibleCharacterT*>(m_pBegin), reinterpret_cast<CompatibleCharacterT*>(m_pEnd));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Slice<const TypeTraits::character_to_character_compatible_type_t<T>> CastToCompatibleChar() const noexcept {

        using CompatibleCharacterT = const TypeTraits::character_to_character_compatible_type_t<T>;
        return Slice<CompatibleCharacterT>(reinterpret_cast<CompatibleCharacterT*>(m_pBegin), reinterpret_cast<CompatibleCharacterT*>(m_pEnd));
    }

    /**
     * @brief Accesses the element at the specified index.
     * @param idx The index of the element.
     * @return Reference to the element.
     * 
     * @note The index must be less than GetSize().
     * 
     * @example
     * 
     * Alt::Slice<const char> slice = "Slice"_s;
     * 
     * assert(s[4] == 'e');
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& operator[](const usize idx) noexcept {
        ALT_ASSERT(idx < GetSize());

        return m_pBegin[idx];
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& operator[](const usize idx) const noexcept {
        ALT_ASSERT(idx < GetSize());

        return m_pBegin[idx];
    }

    /**
     * @brief Returns a reference to the first element.
     * @return Reference to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetFirst() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pBegin;
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetFirst() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pBegin;
    }

    /**
     * @brief Returns a reference to the last element.
     * @return Reference to the last element.
     * @note Asserts that the Slice is not empty.
     */
    ALT_NODISCARD ALT_FORCE_INLINE T& GetLast() noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(m_pEnd - 1);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const T& GetLast() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(m_pEnd - 1);
    }

    /**
     * @brief Returns a raw pointer to the beginning of the slice.
     * @return Pointer to the first element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr T* Data() noexcept { return m_pBegin; }
    ALT_NODISCARD ALT_FORCE_INLINE constexpr const T* Data() const noexcept { return m_pBegin; }

    /**
     * @brief Returns a raw pointer to the end of the slice.
     * @return Pointer to the next after last element.
     */
    ALT_NODISCARD ALT_FORCE_INLINE constexpr T* DataEnd() noexcept { return m_pEnd; }
    ALT_NODISCARD ALT_FORCE_INLINE constexpr const T* DataEnd() const noexcept { return m_pEnd; }

    /**
     * @brief Trims whitespace from the left.
     * @example
     * 
     * const Alt::String str = "    Slice    "_salloc;
     * Alt::Slice<const char> slice = str;
     * 
     * slice = slice.TrimLeft();
     * 
     * assert(slice == "Slice    ");
     * assert(str == "    Slice    ");
     */
    ALT_NODISCARD Slice TrimLeft() const noexcept {

        const T* pBegin = _StringDetails::TrimLeft(m_pBegin, m_pEnd);
        return Slice{ pBegin, m_pEnd };
    }

    /**
     * @brief Trims whitespace from the right.
     * @example
     * 
     * const Alt::String str = "    Slice    "_salloc;
     * Alt::Slice<const char> slice = str;
     * 
     * slice = slice.TrimRight();
     * 
     * assert(slice == "    Slice");
     * assert(str == "    Slice    ");
     */
    ALT_NODISCARD Slice TrimRight() const noexcept {

        const T* pEnd = _StringDetails::TrimRight(m_pBegin, m_pEnd);
        return Slice{ m_pBegin, pEnd };
    }

    /**
     * @brief Trims whitespace.
     * @example
     * 
     * const Alt::String str = "    Slice    "_salloc;
     * Alt::Slice<const char> slice = str;
     * 
     * slice = slice.Trim();
     * 
     * assert(slice == "Slice");
     * assert(str == "    Slice    ");
     */
    ALT_NODISCARD Slice Trim() const noexcept {

        Slice result = TrimLeft();
        return result.TrimRight();
    }

    /**
     * @brief Finds the first occurrence of a value in the slice.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to search for.
     * @return An Optional containing the index or Nullopt if not found.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::Slice<const char> slice = "Slice"_s;
     * 
     * const usize idx = slice.FindFirst('i') iferr_return;
     * assert(idx == 2); // index in bytes
     */
    ALT_NODISCARD Optional<usize> FindFirst(const chara utf32) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, utf32);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const u8 utf8) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char utf8) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char (&utf8)[4]) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindFirst(const char8_t utf8) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindFirst(const char16_t utf16) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, utf16);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, utf16);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const wchar_t wchar) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, wchar);
    }

    /**
     * @brief Finds the last occurrence of a value in the slice.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to search for.
     * @return An Optional containing the index or Nullopt if not found.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::Slice<const char> slice = "Slice.alt"_s;
     * 
     * const usize idx = slice.FindLast('.') iferr_return;
     * assert(idx == 5); // index in bytes
     */
    ALT_NODISCARD Optional<usize> FindLast(const chara utf32) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, utf32);
    }

    ALT_NODISCARD Optional<usize> FindLast(const u8 utf8) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char utf8) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char (&utf8)[4]) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindLast(const char8_t utf8) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindLast(const char16_t utf16) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, utf16);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, utf16);
    }

    ALT_NODISCARD Optional<usize> FindLast(const wchar_t wchar) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, wchar);
    }

    /**
     * @brief Check if we start with the occurrence in the slice.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to comare for.
     * @return An true if start with or false if not.
     * 
     * @example
     * 
     * const Alt::Slice<const char> slice = "Slice"_s;
     * 
     * const bool isStartWith = slice.StartWith('S');
     * assert(isStartWith);
     */
    ALT_NODISCARD bool StartWith(const chara utf32) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, utf32);
    }

    ALT_NODISCARD bool StartWith(const u8 utf8) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD bool StartWith(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD bool StartWith(const char utf8) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD bool StartWith(const char (&utf8)[4]) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD bool StartWith(const char8_t utf8) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD bool StartWith(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD bool StartWith(const char16_t utf16) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, utf16);
    }

    ALT_NODISCARD bool StartWith(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, utf16);
    }

    ALT_NODISCARD bool StartWith(const wchar_t wchar) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, wchar);
    }

    /**
     * @brief Check if we end with the occurrence in the slice.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to comare for.
     * @return An true if end with or false if not.
     * 
     * @example
     * 
     * const Alt::Slice<const char> slice = "Slice.alt"_s;
     * 
     * cpnst bool isEndWith = slice.EndWith('t');
     * assert(isEndWith);
     */
    ALT_NODISCARD bool EndWith(const chara utf32) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, utf32);
    }

    ALT_NODISCARD bool EndWith(const u8 utf8) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD bool EndWith(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD bool EndWith(const char utf8) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD bool EndWith(const char (&utf8)[4]) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD bool EndWith(const char8_t utf8) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, utf8);
    }

    ALT_NODISCARD bool EndWith(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD bool EndWith(const char16_t utf16) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, utf16);
    }

    ALT_NODISCARD bool EndWith(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, utf16);
    }

    ALT_NODISCARD bool EndWith(const wchar_t wchar) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, wchar);
    }

    /**
     * @brief Finds the first occurrence of a string within the slice.
     * @param str A Slice compatible with UTF-8.
     * @return An Optional index where the substring starts.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::Slice<const char> slice = "Slice"_s;
     * 
     * const usize idx = slice.FindFirst("ice");
     * assert(idx == 2); // index in bytes
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindFirst(U&& str) const noexcept {

        const Slice<const InnerU> bytes = str;
        return _StringDetails::FindFirst(m_pBegin, m_pEnd, bytes.Data(), bytes.DataEnd());
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindFirst(const U* str) const noexcept {

        return _StringDetails::FindFirst(m_pBegin, m_pEnd, str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Finds the last occurrence of a string within the slice.
     * @param str A Slice compatible with UTF-8.
     * @return An Optional index where the substring starts.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::Slice<const char> slice = "Slice.alt"_s;
     * 
     * const usize idx = slice.FindLast(".alt");
     * assert(idx == 5); // index in bytes
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindLast(U&& str) const noexcept {

        const Slice<const InnerU> bytes = str;
        return _StringDetails::FindLast(m_pBegin, m_pEnd, bytes.Data(), bytes.DataEnd());
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindLast(const U* str) const noexcept {

        return _StringDetails::FindLast(m_pBegin, m_pEnd, str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Checks if the slice starts with the given substring.
     * @tparam str A Slice compatible with UTF-8.
     * @param str The prefix to check for.
     * @return True if the prefix is found at the beginning, false otherwise.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::Slice<const char> slice = "Slice"_s;
     * 
     * const bool isStart = slice.StartWith("Sl");
     * assert(isStart);
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD bool StartWith(U&& str) const noexcept {

        const Slice<const InnerU> bytes = str;
        return _StringDetails::StartWith(m_pBegin, m_pEnd, bytes.Data(), bytes.DataEnd());
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD bool StartWith(const U* str) const noexcept {

        return _StringDetails::StartWith(m_pBegin, m_pEnd, str, str + _StringDetails::CalculateCStrLen(str));
    }

    /**
     * @brief Checks if the slice ends with the given substring.
     * @tparam str A Slice compatible with UTF-8.
     * @param str The suffix to check for.
     * @return True if the suffix is found at the end, false otherwise.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::Slice<const char> slice = "Slice"_s;
     * 
     * const bool isEnd = slice.StartWith("ce");
     * assert(isEnd);
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD bool EndWith(U&& str) const noexcept {

        const Slice<const InnerU> bytes = str;
        return _StringDetails::EndWith(m_pBegin, m_pEnd, bytes.Data(), bytes.DataEnd());
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD bool EndWith(const U* str) const noexcept {

        return _StringDetails::EndWith(m_pBegin, m_pEnd, str, str + _StringDetails::CalculateCStrLen(str));
    }

    /**
     * @brief Calculates a hash value for the underlying string.
     * @param seed The initial seed for hashing.
     * @return A hash result.
     */
    ALT_NODISCARD usize CalculateHash(const usize seed = 0) const noexcept {

        return Algorithms::Hash(m_pBegin, m_pEnd, seed);
    }

    /**
     * @brief Compares the slices.
     * @param rhs_ A Slice compatible with UTF-8.
     * @return -1 if this < rhs_, 0 if this == rhs_, and +1 if this > rhs_.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::Slice<const char> slice = "Slice"_s;
     * 
     * ssize result = slice.Compare("Sli");
     * assert(result == 1);
     * 
     * result = slice.Compare("Slice");
     * assert(result == 0);
     * 
     * result = slice.Compare("Slice Slice");
     * assert(result == -1);
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD ssize Compare(U&& rhs_) const noexcept {

        using CompatibleCharacterT = TypeTraits::character_to_character_compatible_type_t<T>;

        const Slice<const InnerU> rhs = rhs_;

        return Algorithms::Compare(
            reinterpret_cast<const CompatibleCharacterT*>(m_pBegin),
            reinterpret_cast<const CompatibleCharacterT*>(m_pEnd),
            reinterpret_cast<const CompatibleCharacterT*>(rhs.Data()),
            reinterpret_cast<const CompatibleCharacterT*>(rhs.DataEnd())
        );
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD ssize Compare(const U* rhs) const noexcept {

        using CompatibleCharacterT = TypeTraits::character_to_character_compatible_type_t<T>;

        return Algorithms::Compare(
            reinterpret_cast<const CompatibleCharacterT*>(m_pBegin),
            reinterpret_cast<const CompatibleCharacterT*>(m_pEnd),
            reinterpret_cast<const CompatibleCharacterT*>(rhs),
            reinterpret_cast<const CompatibleCharacterT*>(rhs + _StringDetails::CalculateCStrLen(rhs))
        );
    }

    /**
     * @brief Operators for comparison with Slice.
     * @example
     * 
     * Alt::Slice<const char> slice0 = "Slice"_s;
     * Alt::Slice<const char> slice1 = "Slice"_s;
     * 
     * assert(slice0 == slice1);
     */
    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD bool operator==(U&& rhs_) const noexcept {

        const Slice<const InnerU> rhs = rhs_;
        return Algorithms::CompareEQ(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD bool operator!=(U&& rhs_) const noexcept {

        const Slice<const InnerU> rhs = rhs_;
        return Algorithms::CompareNE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD bool operator<(U&& rhs_) const noexcept {

        const Slice<const InnerU> rhs = rhs_;
        return Algorithms::CompareLT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD bool operator<=(U&& rhs_) const noexcept {

        const Slice<const InnerU> rhs = rhs_;
        return Algorithms::CompareLE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD bool operator>(U&& rhs_) const noexcept {

        const Slice<const InnerU> rhs = rhs_;
        return Algorithms::CompareGT(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerU>, U&&> && TypeTraits::is_character_compatible_have_same_encoding_v<T, InnerU>>* = nullptr>
    ALT_NODISCARD bool operator>=(U&& rhs_) const noexcept {

        const Slice<const InnerU> rhs = rhs_;
        return Algorithms::CompareGE(Data(), DataEnd(), rhs.Data(), rhs.DataEnd());
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD bool operator==(const U* rhs) const noexcept {
        return Compare(rhs) == 0;
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD bool operator!=(const U* rhs) const noexcept {
        return !(*this == rhs);
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD bool operator<(const U* rhs) const noexcept {
        return Compare(rhs) < 0;
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD bool operator<=(const U* rhs) const noexcept {
        return !(*this > rhs);
    }

    template <typename U,
        std::enable_if_t<TypeTraits::is_character_have_same_encoding_v<T, U>>* = nullptr>
    ALT_NODISCARD bool operator>(const U* rhs) const noexcept {
        return Compare(rhs) > 0;
    }

    /**
     * @brief Returns std iterators.
     * @return Iterator pointing to the first element.
     */
    ALT_FORCE_INLINE iterator begin() const noexcept { return iterator{ m_pBegin }; }
    ALT_FORCE_INLINE iterator end() const noexcept { return iterator{ m_pEnd}; }

    ALT_FORCE_INLINE reverse_iterator rbegin() const noexcept { return reverse_iterator{ end() }; }
    ALT_FORCE_INLINE reverse_iterator rend() const noexcept { return reverse_iterator{ begin() }; }

    /**
     * @brief Return a iterator for the slice.
     * @return An `SliceIterator` instance.
     */
    ALT_NODISCARD ALT_FORCE_INLINE SliceIterator<T> GetIterator() noexcept {
        return SliceIterator<T>{ *this };
    }

    /**
     * @brief Return a constant iterator for the slice.
     * @return An `SliceIteratorConst` instance.
     */
    ALT_NODISCARD ALT_FORCE_INLINE SliceIteratorConst<T> GetIterator() const noexcept {
        return SliceIteratorConst<T>{ *this };
    }

    /**
     * @brief Return a iterator for the slice.
     * @return An `SliceIteratorUTF` instance.
     */
    ALT_NODISCARD ALT_FORCE_INLINE SliceIteratorUTF<_StringDetails::character_to_utf_type_v<T>> GetIteratorUTF() const noexcept {
        return SliceIteratorUTF<_StringDetails::character_to_utf_type_v<T>>{ *this };
    }

    /**
     * @brief Return a iterator for the slice.
     * @details Eliminates validation checks for UTF.
     * @return An `SliceIteratorUTFTrusted` instance.
     */
    ALT_NODISCARD ALT_FORCE_INLINE SliceIteratorUTFTrusted<_StringDetails::character_to_utf_type_v<T>> GetIteratorUTFTrusted() const noexcept {
        return SliceIteratorUTFTrusted<_StringDetails::character_to_utf_type_v<T>>{ *this };
    }

    /**
     * @brief Return a split iterator for the slice.
     * @return An `SplitIterator` instance.
     * @example
     * 
     * const Alt::Slice<const char> str = "image/base64,"_s;
     * const Alt::SplitIterator<const char> iter = str.GetSplitIterator("/");
     * 
     * assert(*iter.Next() == "image");
     * assert(*iter.Next() == "base64,");
     */
    ALT_NODISCARD ALT_FORCE_INLINE SplitIterator<T> GetSplitIterator(const Slice<const T> pattern) const noexcept {
        return SplitIterator<T>{ *this, pattern };
    }

private:

    template <typename, bool>
    friend class Slice;

    T* m_pBegin = nullptr;
    T* m_pEnd = nullptr;
};

/**
 * @brief An iterator that allows traversing the slice.
 * @details This iterator maintains its own copy of the slice's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Slice object.
 * @tparam T The numeric type of the slice elements.
 */
template <typename T>
class SliceIterator final : public Iterator<SliceIterator<T>, T&, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait> {
public:

    /**
     * @brief Constructs an iterator from a specific slice.
     * @param self The Slice object to copy boundaries from.
     */
    ALT_FORCE_INLINE SliceIterator(Slice<T>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return The remaining size of the iterator's current view.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return GetSize();
    }

    /**
     * @brief Retrieves the next element and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * usize arr[]{ 0, 1 };
     * Alt::Slice<usize> slice{ arr };
     * Alt::SliceIterator<usize> it = slice.GetIterator();
     * 
     * assert(*it.Next() == 0);
     * assert(*it.Next() == 1);
     * assert(it.Next() == Alt::Nullopt);
     */
    ALT_FORCE_INLINE Optional<T&> Next() noexcept {

        if (m_pBegin == m_pEnd) {

            return Nullopt;
        }

        T* pResult = m_pBegin;
        m_pBegin += 1;

        return *pResult;
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * usize arr[]{ 0, 1, 2, 3, 4 };
     * Alt::Slice<usize> slice{ arr };
     * Alt::SliceIterator<usize> it = slice.GetIterator();
     * 
     * const usize skipped = it.Advance(3);
     * 
     * assert(skipped == 3);
     * assert(it.GetSize() == 2);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pBegin += advance;

        return advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * usize arr[]{ 0, 1 };
     * Alt::Slice<usize> slice{ arr };
     * Alt::SliceIterator<usize> it = slice.GetIterator();
     * 
     * assert(*it.NextBack() == 1);
     * assert(*it.NextBack() == 0);
     * assert(it.NextBack() == Alt::Nullopt);
     */
    ALT_FORCE_INLINE Optional<T&> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {

            return Nullopt;
        }

        m_pEnd -= 1;
        T* pResult = m_pEnd;

        return *pResult;
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * usize arr[]{ 0, 1, 2, 3, 4 };
     * Alt::Slice<usize> slice{ arr };
     * Alt::SliceIterator<usize> it = slice.GetIterator();
     * 
     * const usize skipped = it.AdvanceBack(1);
     * 
     * assert(skipped == 1);
     * assert(it.GetSize() == 4);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pEnd -= advance;

        return advance;
    }

    /**
     * @brief Returns the remaining size.
     * @return The number of elements left in the iterator's slice.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    T* m_pBegin;
    T* m_pEnd;
};

/**
 * @brief An iterator that allows traversing the slice.
 * @details This iterator maintains its own copy of the slice's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Slice object.
 * @tparam T The numeric type of the slice elements.
 */
template <typename T>
class SliceIteratorConst final : public Iterator<SliceIteratorConst<T>, const T&, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait> {
public:

    /**
     * @brief Constructs an iterator from a specific slice.
     * @param self The Slice object to copy boundaries from.
     */
    ALT_FORCE_INLINE SliceIteratorConst(const Slice<T>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return The remaining size of the iterator's current view.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return GetSize();
    }

    /**
     * @brief Retrieves the next element and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * usize arr[]{ 0, 1 };
     * Alt::Slice<usize> slice{ arr };
     * Alt::SliceIterator<usize> it = slice.GetIterator();
     * 
     * assert(*it.Next() == 0);
     * assert(*it.Next() == 1);
     * assert(it.Next() == Alt::Nullopt);
     */
    ALT_FORCE_INLINE Optional<const T&> Next() noexcept {

        if (m_pBegin == m_pEnd) {

            return Nullopt;
        }

        const T* pResult = m_pBegin;
        m_pBegin += 1;

        return *pResult;
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * usize arr[]{ 0, 1, 2, 3, 4 };
     * Alt::Slice<usize> slice{ arr };
     * Alt::SliceIterator<usize> it = slice.GetIterator();
     * 
     * const usize skipped = it.Advance(3);
     * 
     * assert(skipped == 3);
     * assert(it.GetSize() == 2);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pBegin += advance;

        return advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * usize arr[]{ 0, 1 };
     * Alt::Slice<usize> slice{ arr };
     * Alt::SliceIterator<usize> it = slice.GetIterator();
     * 
     * assert(*it.NextBack() == 1);
     * assert(*it.NextBack() == 0);
     * assert(it.NextBack() == Alt::Nullopt);
     */
    ALT_FORCE_INLINE Optional<const T&> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {

            return Nullopt;
        }

        m_pEnd -= 1;
        const T* pResult = m_pEnd;

        return *pResult;
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * usize arr[]{ 0, 1, 2, 3, 4 };
     * Alt::Slice<usize> slice{ arr };
     * Alt::SliceIterator<usize> it = slice.GetIterator();
     * 
     * const usize skipped = it.AdvanceBack(1);
     * 
     * assert(skipped == 1);
     * assert(it.GetSize() == 4);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pEnd -= advance;

        return advance;
    }

    /**
     * @brief Returns the remaining size.
     * @return The number of elements left in the iterator's slice.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const T* m_pBegin;
    const T* m_pEnd;
};

/**
 * @brief An iterator that allows traversing the slice UTF-8. Validates UTF-8 sequences.
 * @details This iterator maintains its own copy of the slice's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Slice object.
 */
template <>
class SliceIteratorUTF<Utf::Encoding::Utf8> final
    : public Iterator<SliceIteratorUTF<Utf::Encoding::Utf8>, Optional<chara>, IIteratorForwardTrait, IIteratorBackwardTrait>
{
public:

    /**
     * @brief Constructs an iterator from a slice which support UTF-8.
     * @param self The Slice object to copy boundaries from.
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_FORCE_INLINE SliceIteratorUTF(const Slice<T>& self) noexcept
        : m_pBegin(reinterpret_cast<const u8*>(self.Data()))
        , m_pEnd(reinterpret_cast<const u8*>(self.DataEnd()))
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return The remaining size of the iterator's current view.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Retrieves the next element and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * Alt::Slice<const char> slice = "Слайс"_s;
     * Alt::SliceIteratorUTF8 it = slice.GetIteratorUTF8();
     * 
     * assert(**it.Next() == U"С"[0]);
     * assert(**it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE Optional<Optional<chara>> Next() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u8 utf8[4]{
            *m_pBegin,
            m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u8>(0),
            m_pBegin + 2 < m_pEnd ? *(m_pBegin + 2) : static_cast<u8>(0),
            m_pBegin + 3 < m_pEnd ? *(m_pBegin + 3) : static_cast<u8>(0),
        };

        u32 utf32;
        const Utf::Result result = Utf::_8::Utf8ToUtf32(utf8, utf32);

        m_pBegin += result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return result.IsOk()
            ? MakeOptional(static_cast<chara>(utf32))
            : Nullopt;
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const char> slice = "Слайс"_s;
     * Alt::SliceIteratorUTF8 it = slice.GetIteratorUTF8();
     * 
     * it.Advance(2);
     * assert(**it.Next() == U"а"[0]);
     * assert(**it.Next() == U"й"[0]);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u8 utf8[4]{
                *m_pBegin,
                m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u8>(0),
                m_pBegin + 2 < m_pEnd ? *(m_pBegin + 2) : static_cast<u8>(0),
                m_pBegin + 3 < m_pEnd ? *(m_pBegin + 3) : static_cast<u8>(0),
            };

            const usize advanceBytes = Utf::_8::ForwardUtf8(utf8);

            m_pBegin += Num::Max(advanceBytes, 1);
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * Alt::Slice<const char> slice = "Слайс"_s;
     * Alt::SliceIteratorUTF8 it = slice.GetIteratorUTF8();
     * 
     * assert(**it.NextBack() == U"с"[0]);
     * assert(**it.NextBack() == U"й"[0]);
     */
    ALT_FORCE_INLINE Optional<Optional<chara>> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u8 utf8[4]{
            m_pBegin <= (m_pEnd - 4) ? *(m_pEnd - 4) : static_cast<u8>(0),
            m_pBegin <= (m_pEnd - 3) ? *(m_pEnd - 3) : static_cast<u8>(0),
            m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u8>(0),
            *(m_pEnd - 1),
        };

        u32 utf32;
        const Utf::Result result = Utf::_8::Utf8ToUtf32Backward(utf8, utf32);

        m_pEnd -= result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return result.IsOk()
            ? MakeOptional(static_cast<chara>(utf32))
            : Nullopt;
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const char> slice = "Слайс"_s;
     * Alt::SliceIteratorUTF8 it = slice.GetIteratorUTF8();
     * 
     * it.AdvanceBack(2);
     * assert(**it.Next() == U"а"[0]);
     * assert(**it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u8 utf8[4]{
                m_pBegin <= (m_pEnd - 4) ? *(m_pEnd - 4) : static_cast<u8>(0),
                m_pBegin <= (m_pEnd - 3) ? *(m_pEnd - 3) : static_cast<u8>(0),
                m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u8>(0),
                *(m_pEnd - 1),
            };

            const usize advanceBytes = Utf::_8::BackwardUtf8(utf8);

            m_pEnd -= Num::Max(advanceBytes, 1);
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const u8* m_pBegin;
    const u8* m_pEnd;
};

/**
 * @brief An iterator that allows traversing the slice UTF-16. Validates UTF-16 sequences.
 * @details This iterator maintains its own copy of the slice's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Slice object.
 */
template <>
class SliceIteratorUTF<Utf::Encoding::Utf16> final
    : public Iterator<SliceIteratorUTF<Utf::Encoding::Utf16>, Optional<chara>, IIteratorForwardTrait, IIteratorBackwardTrait>
{
public:

    /**
     * @brief Constructs an iterator from a slice which support UTF-16.
     * @param self The Slice object to copy boundaries from.
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_FORCE_INLINE SliceIteratorUTF(const Slice<T>& self) noexcept
        : m_pBegin(reinterpret_cast<const u16*>(self.Data()))
        , m_pEnd(reinterpret_cast<const u16*>(self.DataEnd()))
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return The remaining size of the iterator's current view.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Retrieves the next element and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * Alt::Slice<const char16_t> slice = u"Слайс"_s;
     * Alt::SliceIteratorUTF16 it = slice.GetIteratorUTF8();
     * 
     * assert(**it.Next() == U"С"[0]);
     * assert(**it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE Optional<Optional<chara>> Next() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u16 utf16[2]{
            *m_pBegin,
            m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u16>(0),
        };

        u32 utf32;
        const Utf::Result result = Utf::_16::Utf16ToUtf32(utf16, utf32);

        m_pBegin += result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return result.IsOk()
            ? MakeOptional(static_cast<chara>(utf32))
            : Nullopt;
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const char16_t> slice = u"Слайс"_s;
     * Alt::SliceIteratorUTF16 it = slice.GetIteratorUTF8();
     * 
     * it.Advance(2);
     * assert(**it.Next() == U"а"[0]);
     * assert(**it.Next() == U"й"[0]);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u16 utf16[2]{
                *m_pBegin,
                m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u16>(0),
            };

            const usize advanceBytes = Utf::_16::ForwardUtf16(utf16);

            m_pBegin += Num::Max(advanceBytes, 1);
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * Alt::Slice<const char16_t> slice = u"Слайс"_s;
     * Alt::SliceIteratorUTF16 it = slice.GetIteratorUTF8();
     * 
     * assert(**it.NextBack() == U"с"[0]);
     * assert(**it.NextBack() == U"й"[0]);
     */
    ALT_FORCE_INLINE Optional<Optional<chara>> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u16 utf16[2]{
            m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u16>(0),
            *(m_pEnd - 1),
        };

        u32 utf32;
        const Utf::Result result = Utf::_16::Utf16ToUtf32Backward(utf16, utf32);

        m_pEnd -= result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return result.IsOk()
            ? MakeOptional(static_cast<chara>(utf32))
            : Nullopt;
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const char16_t> slice = u"Слайс"_s;
     * Alt::SliceIteratorUTF16 it = slice.GetIteratorUTF8();
     * 
     * it.AdvanceBack(2);
     * assert(**it.Next() == U"а"[0]);
     * assert(**it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u16 utf16[2]{
                m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u16>(0),
                *(m_pEnd - 1),
            };

            const usize advanceBytes = Utf::_16::BackwardUtf16(utf16);

            m_pEnd -= Num::Max(advanceBytes, 1);
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const u16* m_pBegin;
    const u16* m_pEnd;
};

/**
 * @brief An iterator that allows traversing the slice UTF-32. Validates UTF-32 sequences.
 * @details This iterator maintains its own copy of the slice's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Slice object.
 */
template <>
class SliceIteratorUTF<Utf::Encoding::Utf32> final
    : public Iterator<SliceIteratorUTF<Utf::Encoding::Utf32>, Optional<chara>, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait>
{
public:

    /**
     * @brief Constructs an iterator from a slice which support UTF-32.
     * @param self The Slice object to copy boundaries from.
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf32_v<T>>* = nullptr>
    ALT_FORCE_INLINE SliceIteratorUTF(const Slice<T>& self) noexcept
        : m_pBegin(reinterpret_cast<const u32*>(self.Data()))
        , m_pEnd(reinterpret_cast<const u32*>(self.DataEnd()))
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return The remaining size of the iterator's current view.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return GetSize();
    }

    /**
     * @brief Retrieves the next element and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * Alt::Slice<const chara> slice = U"Слайс"_s;
     * Alt::SliceIteratorUTF32 it = slice.GetIteratorUTF8();
     * 
     * assert(**it.Next() == U"С"[0]);
     * assert(**it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE Optional<Optional<chara>> Next() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u32 utf32 = *m_pBegin;
        m_pBegin += 1;

        return utf32;
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const chara> slice = U"Слайс"_s;
     * Alt::SliceIteratorUTF32 it = slice.GetIteratorUTF8();
     * 
     * it.Advance(2);
     * assert(**it.Next() == U"а"[0]);
     * assert(**it.Next() == U"й"[0]);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pBegin += advance;

        return advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * Alt::Slice<const chara> slice = U"Слайс"_s;
     * Alt::SliceIteratorUTF32 it = slice.GetIteratorUTF8();
     * 
     * assert(**it.NextBack() == U"с"[0]);
     * assert(**it.NextBack() == U"й"[0]);
     */
    ALT_FORCE_INLINE Optional<Optional<chara>> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        m_pEnd -= 1;
        const u32 utf32 = *m_pEnd;

        return utf32;
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const chara> slice = U"Слайс"_s;
     * Alt::SliceIteratorUTF32 it = slice.GetIteratorUTF8();
     * 
     * it.AdvanceBack(2);
     * assert(**it.Next() == U"а"[0]);
     * assert(**it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pEnd -= advance;

        return advance;
    }

    /**
     * @brief Returns the remaining size.
     * @return The number of elements left in the iterator's slice.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const u32* m_pBegin;
    const u32* m_pEnd;
};

/**
 * @brief An iterator that allows traversing the slice UTF-8. Do not validates UTF-8 sequences - trust Slice.
 * @details This iterator maintains its own copy of the slice's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Slice object.
 */
template <>
class SliceIteratorUTFTrusted<Utf::Encoding::Utf8> final
    : public Iterator<SliceIteratorUTFTrusted<Utf::Encoding::Utf8>, chara, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait>
{
public:

    /**
     * @brief Constructs an iterator from a slice which support UTF-8.
     * @param self The Slice object to copy boundaries from.
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_FORCE_INLINE SliceIteratorUTFTrusted(const Slice<T>& self) noexcept
        : m_pBegin(reinterpret_cast<const u8*>(self.Data()))
        , m_pEnd(reinterpret_cast<const u8*>(self.DataEnd()))
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return The remaining size of the iterator's current view.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_pEnd - m_pBegin;
    }
    
    /**
     * @brief Retrieves the next element and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * Alt::Slice<const char> slice = "Слайс"_s;
     * Alt::SliceIteratorUTF8Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * assert(*it.Next() == U"С"[0]);
     * assert(*it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE Optional<chara> Next() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u8 utf8[4]{
            *m_pBegin,
            m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u8>(0),
            m_pBegin + 2 < m_pEnd ? *(m_pBegin + 2) : static_cast<u8>(0),
            m_pBegin + 3 < m_pEnd ? *(m_pBegin + 3) : static_cast<u8>(0),
        };

        u32 utf32;
        const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

        m_pBegin += result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const char> slice = "Слайс"_s;
     * Alt::SliceIteratorUTF8Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * it.Advance(2);
     * assert(*it.Next() == U"а"[0]);
     * assert(*it.Next() == U"й"[0]);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u8 utf8[4]{
                *m_pBegin,
                m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u8>(0),
                m_pBegin + 2 < m_pEnd ? *(m_pBegin + 2) : static_cast<u8>(0),
                m_pBegin + 3 < m_pEnd ? *(m_pBegin + 3) : static_cast<u8>(0),
            };

            const usize advanceBytes = Utf::_8::ForwardUtf8IgnoreErrors(utf8);

            m_pBegin += advanceBytes;
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * Alt::Slice<const char> slice = "Слайс"_s;
     * Alt::SliceIteratorUTF8Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * assert(*it.NextBack() == U"с"[0]);
     * assert(*it.NextBack() == U"й"[0]);
     */
    ALT_FORCE_INLINE Optional<chara> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u8 utf8[4]{
            m_pBegin <= (m_pEnd - 4) ? *(m_pEnd - 4) : static_cast<u8>(0),
            m_pBegin <= (m_pEnd - 3) ? *(m_pEnd - 3) : static_cast<u8>(0),
            m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u8>(0),
            *(m_pEnd - 1),
        };

        u32 utf32;
        const Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrorsBackward(utf8, utf32);

        m_pEnd -= result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const char> slice = "Слайс"_s;
     * Alt::SliceIteratorUTF8Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * it.AdvanceBack(2);
     * assert(*it.Next() == U"а"[0]);
     * assert(*it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u8 utf8[4]{
                m_pBegin <= (m_pEnd - 4) ? *(m_pEnd - 4) : static_cast<u8>(0),
                m_pBegin <= (m_pEnd - 3) ? *(m_pEnd - 3) : static_cast<u8>(0),
                m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u8>(0),
                *(m_pEnd - 1),
            };

            const usize advanceBytes = Utf::_8::BackwardUtf8IgnoreErrors(utf8);

            m_pEnd -= advanceBytes;
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const u8* m_pBegin;
    const u8* m_pEnd;
};

/**
 * @brief An iterator that allows traversing the slice UTF-16. Do not validates UTF-16 sequences - trust Slice.
 * @details This iterator maintains its own copy of the slice's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Slice object.
 */
template <>
class SliceIteratorUTFTrusted<Utf::Encoding::Utf16> final
    : public Iterator<SliceIteratorUTFTrusted<Utf::Encoding::Utf16>, chara, IIteratorForwardTrait, IIteratorBackwardTrait>
{
public:

    /**
     * @brief Constructs an iterator from a slice which support UTF-16.
     * @param self The Slice object to copy boundaries from.
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_FORCE_INLINE SliceIteratorUTFTrusted(const Slice<T>& self) noexcept
        : m_pBegin(reinterpret_cast<const u16*>(self.Data()))
        , m_pEnd(reinterpret_cast<const u16*>(self.DataEnd()))
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return The remaining size of the iterator's current view.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_pEnd - m_pBegin;
    }
    
    /**
     * @brief Retrieves the next element and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * Alt::Slice<const char16_t> slice = u"Слайс"_s;
     * Alt::SliceIteratorUTF16Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * assert(*it.Next() == U"С"[0]);
     * assert(*it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE Optional<chara> Next() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u16 utf16[2]{
            *m_pBegin,
            m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u16>(0),
        };

        u32 utf32;
        const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

        m_pBegin += result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const char16_t> slice = u"Слайс"_s;
     * Alt::SliceIteratorUTF16Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * it.Advance(2);
     * assert(*it.Next() == U"а"[0]);
     * assert(*it.Next() == U"й"[0]);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u16 utf16[2]{
                *m_pBegin,
                m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u16>(0),
            };

            const usize advanceBytes = Utf::_16::ForwardUtf16(utf16);

            m_pBegin += Num::Max(advanceBytes, 1);
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * Alt::Slice<const char16_t> slice = u"Слайс"_s;
     * Alt::SliceIteratorUTF16Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * assert(*it.NextBack() == U"с"[0]);
     * assert(*it.NextBack() == U"й"[0]);
     */
    ALT_FORCE_INLINE Optional<chara> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u16 utf16[2]{
            m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u16>(0),
            *(m_pEnd - 1),
        };

        u32 utf32;
        const Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrorsBackward(utf16, utf32);

        m_pEnd -= result.advance;
        ALT_ASSERT(m_pBegin <= m_pEnd);

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const char16_t> slice = u"Слайс"_s;
     * Alt::SliceIteratorUTF16Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * it.AdvanceBack(2);
     * assert(*it.Next() == U"а"[0]);
     * assert(*it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u16 utf16[2]{
                m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u16>(0),
                *(m_pEnd - 1),
            };

            const usize advanceBytes = Utf::_16::BackwardUtf16(utf16);

            m_pEnd -= Num::Max(advanceBytes, 1);
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const u16* m_pBegin;
    const u16* m_pEnd;
};

/**
 * @brief An iterator that allows traversing the slice UTF-32. Do not validates UTF-32 sequences - trust Slice.
 * @details This iterator maintains its own copy of the slice's boundaries. 
 * Advancing the iterator modifies its internal view, not the original Slice object.
 */
template <>
class SliceIteratorUTFTrusted<Utf::Encoding::Utf32> final
    : public Iterator<SliceIteratorUTFTrusted<Utf::Encoding::Utf32>, chara, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait>
{
public:

    /**
     * @brief Constructs an iterator from a slice which support UTF-32.
     * @param self The Slice object to copy boundaries from.
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf32_v<T>>* = nullptr>
    ALT_FORCE_INLINE SliceIteratorUTFTrusted(const Slice<T>& self) noexcept
        : m_pBegin(reinterpret_cast<const u32*>(self.Data()))
        , m_pEnd(reinterpret_cast<const u32*>(self.DataEnd()))
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return The remaining size of the iterator's current view.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return GetSize();
    }
    
    /**
     * @brief Retrieves the next element and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * Alt::Slice<const chara> slice = U"Слайс"_s;
     * Alt::SliceIteratorUTF32Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * assert(*it.Next() == U"С"[0]);
     * assert(*it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE Optional<chara> Next() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        const u32 utf32 = *m_pBegin;
        m_pBegin += 1;

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Skips a specified number of elements forward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const chara> slice = U"Слайс"_s;
     * Alt::SliceIteratorUTF32Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * it.Advance(2);
     * assert(*it.Next() == U"а"[0]);
     * assert(*it.Next() == U"й"[0]);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pBegin += advance;

        return advance;
    }

    /**
     * @brief Retrieves the lasts element and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * Alt::Slice<const chara> slice = U"Слайс"_s;
     * Alt::SliceIteratorUTF32Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * assert(*it.NextBack() == U"с"[0]);
     * assert(*it.NextBack() == U"й"[0]);
     */
    ALT_FORCE_INLINE Optional<chara> NextBack() noexcept {

        if (m_pBegin == m_pEnd) {
            return Nullopt;
        }

        m_pEnd -= 1;
        const u32 utf32 = *m_pEnd;

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Skips a specified number of elements backward in the current view.
     * @param advance_ The number of elements to skip.
     * @return The actual number of elements successfully skipped.
     * @example
     * 
     * Alt::Slice<const chara> slice = U"Слайс"_s;
     * Alt::SliceIteratorUTF32Trusted it = slice.GetIteratorUTFTrusted();
     * 
     * it.AdvanceBack(2);
     * assert(*it.Next() == U"а"[0]);
     * assert(*it.Next() == U"л"[0]);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        const usize advance = Num::Min(advance_, static_cast<usize>(m_pEnd - m_pBegin));
        m_pEnd -= advance;

        return advance;
    }

    /**
     * @brief Returns the remaining size.
     * @return The number of elements left in the iterator's slice.
     */
    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no elements left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const u32* m_pBegin;
    const u32* m_pEnd;
};

/**
 * @brief Provides a random-access iterator compatible with the C++ Standard Library for `Alt::Slice`.
 * @details Implements the `std::random_access_iterator_tag` interface, enabling the use of 
 * `Alt::Slice` with standard algorithms.
 * 
 * @tparam T The type of the underlying elements.
 * 
 * @example
 * 
 * usize arr[]{ 3, 1, 4, 2 };
 * Alt::Slice<usize> slice{ arr };
 * Alt::SliceSTDIterator<usize> it = slice.begin();
 * 
 * assert(*it == 3);
 * it += 2; 
 * assert(*it == 4);
 * assert(it[1] == 2);
 * 
 * for (usize v : slice) {
 *     ALT_PRINT("{}, ", v);
 * }
 */
template <typename T>
class SliceSTDIterator {
public:

    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::remove_cv_t<T>;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    ALT_FORCE_INLINE SliceSTDIterator() noexcept
        : m_Current(nullptr)
    {}

    ALT_FORCE_INLINE SliceSTDIterator(pointer current) noexcept
        : m_Current(current)
    {}

    ALT_FORCE_INLINE reference operator*() const noexcept {
        return *m_Current;
    }

    ALT_FORCE_INLINE pointer operator->() const noexcept {
        return m_Current;
    }

    ALT_FORCE_INLINE SliceSTDIterator& operator++() noexcept {
        ++m_Current;
        return *this;
    }

    ALT_FORCE_INLINE SliceSTDIterator operator++(int) noexcept {
        SliceSTDIterator ret = *this;
        ++*this;
        return ret;
    }

    ALT_FORCE_INLINE SliceSTDIterator& operator--() noexcept {
        --m_Current;
        return *this;
    }

    ALT_FORCE_INLINE SliceSTDIterator operator--(int) noexcept {
        SliceSTDIterator ret = *this;
        --*this;
        return ret;
    }

    ALT_FORCE_INLINE SliceSTDIterator& operator+=(const difference_type n) noexcept {
        m_Current += n;
        return *this;
    }

    ALT_FORCE_INLINE SliceSTDIterator operator+(const difference_type n) const noexcept {
        SliceSTDIterator ret = *this;
        ret += n;
        return ret;
    }

    ALT_FORCE_INLINE friend SliceSTDIterator operator+(const difference_type n, const SliceSTDIterator& rhs) noexcept {
        return rhs + n;
    }

    ALT_FORCE_INLINE SliceSTDIterator& operator-=(const difference_type n) noexcept {
        m_Current -= n;
        return *this;
    }

    ALT_FORCE_INLINE SliceSTDIterator operator-(const difference_type n) const noexcept {
        SliceSTDIterator ret = *this;
        ret -= n;
        return ret;
    }

    template <typename U,
        std::enable_if_t<std::is_same_v<std::remove_cv_t<U>, value_type>, int> = 0>
    ALT_FORCE_INLINE difference_type operator-(const SliceSTDIterator<U>& rhs) const noexcept {
        return m_Current - rhs.m_Current;
    }

    ALT_FORCE_INLINE reference operator[](const difference_type n) const noexcept {
        return *(*this + n);
    }

    template <typename U,
        std::enable_if_t<std::is_same_v<std::remove_cv_t<U>, value_type>, int> = 0>
    ALT_FORCE_INLINE bool operator==(const SliceSTDIterator<U>& rhs) const noexcept {
        return m_Current == rhs.m_Current;
    }

    template <typename U,
        std::enable_if_t<std::is_same_v<std::remove_cv_t<U>, value_type>, int> = 0>
    ALT_FORCE_INLINE bool operator!=(const SliceSTDIterator<U>& rhs) const noexcept {
        return !(*this == rhs);
    }

    template <typename U,
        std::enable_if_t<std::is_same_v<std::remove_cv_t<U>, value_type>, int> = 0>
    ALT_FORCE_INLINE bool operator<(const SliceSTDIterator<U>& rhs) const noexcept {
        return m_Current < rhs.m_Current;
    }

    template <typename U,
        std::enable_if_t<std::is_same_v<std::remove_cv_t<U>, value_type>, int> = 0>
    ALT_FORCE_INLINE bool operator>(const SliceSTDIterator<U>& rhs) const noexcept {
        return rhs < *this;
    }

    template <typename U,
        std::enable_if_t<std::is_same_v<std::remove_cv_t<U>, value_type>, int> = 0>
    ALT_FORCE_INLINE bool operator<=(const SliceSTDIterator<U>& rhs) const noexcept {
        return !(rhs < *this);
    }

    template <typename U,
        std::enable_if_t<std::is_same_v<std::remove_cv_t<U>, value_type>, int> = 0>
    ALT_FORCE_INLINE bool operator>=(const SliceSTDIterator<U>& rhs) const noexcept {
        return !(*this < rhs);
    }

private:

    pointer m_Current;
};

/**
 * @brief Provides an iterator for iterating over splits.
 * @tparam T The underlying type of the elements in the slice.
 * @example
 * 
 * Alt::Slice<const char> str = "c,cpp,rust,cs,odin"_s;
 * Alt::SplitIterator<const char> iter = str.GetSplitIterator(",");
 * 
 * assert(*iter.Next() == "c");
 * assert(*iter.Next() == "cpp");
 * assert(*iter.Next() == "rust");
 * assert(*iter.NextBack() == "odin");
 * assert(*iter.NextBack() == "cs");
 */
template <typename T>
class SplitIterator<T, true> final : public Iterator<SplitIterator<T>, Slice<T>, IIteratorForwardTrait, IIteratorBackwardTrait> {
public:

    /**
     * @brief Constructs a new split iterator with the given source and pattern.
     * @param str The source slice to be partitioned.
     * @param pattern The pattern used to split.
     */
    ALT_FORCE_INLINE SplitIterator(const Slice<T> str, const Slice<const T> pattern) noexcept
        : m_Str(str)
        , m_Pattern(pattern)
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return 0;
    }

    /**
     * @brief Advances next split.
     * @return An Optional containing the Slice if a split was found, otherwise Nullopt.
     * @example
     * 
     * Alt::Slice<const char> str = "one||two||three"_s;
     * Alt::SplitIterator<const char> iter = str.GetSplitIterator("||");
     * 
     * assert(*iter.Next() == "one");
     */
    ALT_FORCE_INLINE Optional<Slice<T>> Next() noexcept {
        iferr_scope;

        if (m_Str.IsEmpty()) {
            return Nullopt;
        }

        const usize offset = m_Str.FindFirst(m_Pattern) ifok {

            Slice<T> result = m_Str.SubSlice(0, offset);
            m_Str = m_Str.SubSlice(offset + m_Pattern.GetSize());

            return result;
        }

        Slice<T> result = m_Str;
        m_Str = m_Str.SubSlice(m_Str.GetSize());

        return result;
    }

    /**
     * @brief Advances the iterator forward by a specified number of splits.
     * @param advance The number of positions to move forward.
     * @return The actual number of successful steps taken.
     * @example
     * 
     * Alt::Slice<const char> str = "one||two||three"_s;
     * Alt::SplitIterator<const char> iter = str.GetSplitIterator("||");
     * 
     * assert(iter.Advance(2), 2);
     * assert(*iter.Next() == "three");
     */
    ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {
        iferr_scope;

        usize size = 0;
        for (usize i = 0; i < advance; ++i) {

            ALT_MAYBE_UNUSED const Slice<T> val = Next() iferr_return size;
            size += 1;
        }

        return size;
    }

    /**
     * @brief Advances last split.
     * @return An Optional containing the Slice if a split was found, otherwise Nullopt.
     * @example
     * 
     * Alt::Slice<const char> str = "one||two||three"_s;
     * Alt::SplitIterator<const char> iter = str.GetSplitIterator("||");
     * 
     * assert(*iter.NextBack() == "three");
     */
    ALT_FORCE_INLINE Optional<Slice<T>> NextBack() noexcept {
        iferr_scope;

        if (m_Str.IsEmpty()) {
            return Nullopt;
        }

        const usize offset = m_Str.FindLast(m_Pattern) ifok {

            Slice<T> result = m_Str.SubSlice(offset + m_Pattern.GetSize());
            m_Str = m_Str.SubSlice(0, offset);

            return result;
        }

        Slice<T> result = m_Str;
        m_Str = m_Str.SubSlice(m_Str.GetSize());

        return result;
    }

    /**
     * @brief Advances the iterator backward by a specified number of splits.
     * @param advance The number of positions to move backward.
     * @return The actual number of successful steps taken.
     * @example
     * 
     * Alt::Slice<const char> str = "one||two||three"_s;
     * Alt::SplitIterator<const char> iter = str.GetSplitIterator("||");
     * 
     * assert(iter.AdvanceBack(2), 2);
     * assert(*iter.NextBack() == "one");
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {
        iferr_scope;

        usize size = 0;
        for (usize i = 0; i < advance; ++i) {

            ALT_MAYBE_UNUSED const Slice<T> val = NextBack() iferr_return size;
            size += 1;
        }

        return size;
    }

private:

    Slice<T> m_Str;
    Slice<const T> m_Pattern;

/*
    TODO: benchmarks

    std::vector<usize> SplitIterator::CalculatePrefixFunctionTable(const std::string_view str, const bool isCaseSensitive) {

        if (str.empty()) {
            return {};
        }

        std::vector<usize> prefixFuncTable(str.size());
        prefixFuncTable[0] = 0;

        if (isCaseSensitive) {

            for (usize i = 1; i < str.size(); ++i) {

                usize prevPrefixFuncVal = prefixFuncTable[i - 1];
                while (prevPrefixFuncVal != 0 && str[prevPrefixFuncVal] != str[i]) {

                    prevPrefixFuncVal = prefixFuncTable[prevPrefixFuncVal - 1];
                }

                prefixFuncTable[i] = str[prevPrefixFuncVal] == str[i]
                    ? prevPrefixFuncVal + 1
                    : 0;
            }
        }
        else {

            for (usize i = 1; i < str.size(); ++i) {

                usize prevPrefixFuncVal = prefixFuncTable[i - 1];
                while (prevPrefixFuncVal != 0 && std::tolower(str[prevPrefixFuncVal]) != std::tolower(str[i])) {

                    prevPrefixFuncVal = prefixFuncTable[prevPrefixFuncVal - 1];
                }

                prefixFuncTable[i] = std::tolower(str[prevPrefixFuncVal]) == std::tolower(str[i])
                    ? prevPrefixFuncVal + 1
                    : 0;
            }
        }

        return prefixFuncTable;
    }

    Optional<std::string_view> SplitIterator::NextUsingPrefixFuncTableImpl() noexcept {

        ALT_ASSERT(!m_PatternPrefixFuncTable.empty() && m_Pattern.size() > 1);

        const char* end = m_Str.data() + m_Str.size();
        if (m_Ptr == end) {
            return Nullopt;
        }

        const char* ptr = m_Ptr;
        usize available = static_cast<usize>(end - ptr);

        if (m_IsCaseSensitive) {

            for (;;) {

                if (m_Pattern.size() - m_PrevPrefixFuncVal > available) {

                    m_Ptr = end;
                    return std::string_view{ ptr, static_cast<usize>(m_Ptr - ptr) };
                }

                while (m_PrevPrefixFuncVal != 0 && *m_Ptr != m_Pattern[m_PrevPrefixFuncVal]) {
                    m_PrevPrefixFuncVal = m_PatternPrefixFuncTable[m_PrevPrefixFuncVal - 1];
                }

                if (*m_Ptr == m_Pattern[m_PrevPrefixFuncVal]) {
                    m_PrevPrefixFuncVal += 1;
                }

                m_Ptr += 1;
                available -= 1;

                if (m_PrevPrefixFuncVal == m_Pattern.size()) {

                    m_PrevPrefixFuncVal = 0;
                    return std::string_view{ ptr, static_cast<usize>(m_Ptr - ptr - m_Pattern.size()) };
                }
            }
        }
        else {

            for (;;) {

                if (m_Pattern.size() - m_PrevPrefixFuncVal > available) {

                    m_Ptr = end;
                    return std::string_view{ ptr, static_cast<usize>(m_Ptr - ptr) };
                }

                while (m_PrevPrefixFuncVal != 0 && std::tolower(*m_Ptr) != std::tolower(m_Pattern[m_PrevPrefixFuncVal])) {
                    m_PrevPrefixFuncVal = m_PatternPrefixFuncTable[m_PrevPrefixFuncVal - 1];
                }

                if (std::tolower(*m_Ptr) == std::tolower(m_Pattern[m_PrevPrefixFuncVal])) {
                    m_PrevPrefixFuncVal += 1;
                }

                m_Ptr += 1;
                available -= 1;

                if (m_PrevPrefixFuncVal == m_Pattern.size()) {

                    m_PrevPrefixFuncVal = 0;
                    return std::string_view{ ptr, static_cast<usize>(m_Ptr - ptr - m_Pattern.size()) };
                }
            }
        }
    }
*/
};

/**
 * @brief Literal for creating Alt::Slice from a `const char*` UTF-8.
 * @example
 * 
 * Alt::Slice<const char> string = "Hello World"_s;
 */
constexpr inline Slice<const char> operator""_s(const char* str, const usize size) noexcept {

    return Slice<const char>{ str, size };
}

#ifdef __cpp_char8_t

/**
 * @brief Literal for creating Alt::Slice from a `const char8_t*` UTF-8.
 * @example
 * 
 * Alt::Slice<const char8_t> string = "Hello World"_s;
 */
constexpr inline Slice<const char8_t> operator""_s(const char8_t* str, const usize size) noexcept {

    return Slice<const char8_t>{ str, size };
}
#endif // __cpp_char8_t

/**
 * @brief Literal for creating Alt::Slice from a `const char16_t*` UTF-16.
 * @example
 * 
 * Alt::Slice<const char16_t> string = u"Hello World"_s;
 */
constexpr inline Slice<const char16_t> operator""_s(const char16_t* str, const usize size) noexcept {

    return Slice<const char16_t>{ str, size };
}

/**
 * @brief Literal for creating Alt::Slice from a `const chara*` UTF-32.
 * @example
 * 
 * Alt::Slice<const chara> string = U"Hello World"_s;
 */
constexpr inline Slice<const chara> operator""_s(const chara* str, const usize size) noexcept {

    return Slice<const chara>{ str, size };
}

/**
 * @brief Literal for creating Alt::Slice from a `const wchar_t*` UTF-16 (WIN32) /UTF-32.
 * @example
 * 
 * Alt::Slice<const wchar_t> string = L"Hello World"_s;
 */
constexpr inline Slice<const wchar_t> operator""_s(const wchar_t* str, const usize size) noexcept {

    return Slice<const wchar_t>{ str, size };
}

/**
 * @brief Literal for compiletime hashing of `const char*`
 * @example
 * 
 * constexpr static hash = "Hello World"_hash;
 */
constexpr inline usize operator""_hash(const char* str_, const usize size) noexcept {

    Slice<const char> str{ str_, size };
    return ALT_HASH_CONST(str);
}

#ifdef __cpp_char8_t

/**
 * @brief Literal for compiletime hashing of `const char*`
 * @example
 * 
 * constexpr static hash = "Hello World"_hash;
 */
constexpr inline usize operator""_hash(const char8_t* str_, const usize size) noexcept {

    Slice<const char8_t> str{ str_, size };
    return ALT_HASH_CONST(str);
}
#endif // __cpp_char8_t

} // namespace Alt

using Alt::operator""_s;
using Alt::operator""_hash;

#endif // __ALT_STD_SLICE__