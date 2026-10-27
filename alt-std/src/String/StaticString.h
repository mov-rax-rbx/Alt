#pragma once

#ifndef __ALT_STD_STATIC_STRING__
#define __ALT_STD_STATIC_STRING__

#include "../IfErr.h"
#include "../TypeTraits.h"

#include "../Algorithms/Algorithms.h"

#include "../Memory/Impl/MemoryPreInclude.h"

#include "../Utils/Num.h"
#include "../Utils/Slice.h"
#include "../Utils/Optional.h"
#include "../Utils/ScopeDefer.h"
#include "../Utils/EnumClassBitFlagsImpl.h"

#include "Utf.h"
#include "Format.h"

#include "Impl/StringDetails.h"

#include "StringIterator.h"

#include "../Algorithms/fast_float-8.2.10/fast_float.h"

namespace Alt {

template <usize SIZE_IN_BYTES_MINUS_ONE, typename Usize = TypeTraits::count_to_type_t<SIZE_IN_BYTES_MINUS_ONE + 1>>
class StaticStringUTF8;

template <usize SIZE_IN_BYTES_MINUS_ONE>
using StaticString = StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE>;

using StringIterator = StringUTF8Iterator;

namespace TypeTraits {

template <usize SIZE_IN_BYTES_MINUS_ONE, typename Usize>
struct __is_string__<StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE, Usize>>
    : std::true_type
{};

template <usize SIZE_IN_BYTES_MINUS_ONE, typename Usize>
struct __is_trivially_reallocatable__<StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE, Usize>>
    : std::true_type
{};

} // namespace TypeTraits

/** \cond */
namespace _StringDetails {

template <usize SIZE_IN_BYTES_MINUS_ONE, typename Usize>
class StaticStringFormatWriter : public FormatWriter<StaticStringFormatWriter<SIZE_IN_BYTES_MINUS_ONE, Usize>> {
public:

    using String = StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE, Usize>;

    ALT_FORCE_INLINE StaticStringFormatWriter(String& str) noexcept
        : m_pStr(&str)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE bool ReserveBytesAddition(const usize) noexcept {
        return true;
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(const T val) noexcept {

        return m_pStr->Append(val);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool Append(const String& self) noexcept {

        return m_pStr->Append(self);
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(const T* str) noexcept {

        return m_pStr->Append(str);
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_character_v<InnerT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(T&& str) noexcept {

        return m_pStr->Append(std::forward<T>(str));
    }

private:

    String* m_pStr = nullptr;
};

} // namespace _StringDetails
/** \endcond */

/**
 * @brief A fixed-capacity string UTF-8 (no allocations) that stores up to a specified number of elements.
 * 
 * @details Manages a contiguous buffer of bytes representing a UTF-8 string. 
 * This class supports appending from Unicode encodings (UTF-8, UTF-16, UTF-32) 
 * and provides iterators for character-based traversal.
 * 
 * @tparam SIZE_IN_BYTES_MINUS_ONE The maximum capacity of the string and +1 for the '\0'.
 * @tparam Usize The type used for size and indexing. Defaults to TypeTraits::count_to_type_t<SIZE_IN_BYTES_MINUS_ONE + 1>.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::StaticString<255> string = "String";
 * string.Append(". Hello from Alt") iferr_return;
 */
template <usize SIZE_IN_BYTES_MINUS_ONE, typename Usize>
class StaticStringUTF8 {
private:

    constexpr static usize SIZE_IN_BYTES = SIZE_IN_BYTES_MINUS_ONE + 1;

    template <typename ...ArgsT>
    constexpr static bool is_populated_v = sizeof...(ArgsT) != 0;

    template <typename U>
    using is_pushable_t = std::is_constructible<chara, U>;

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

    struct Rule5Copy_t {};
    constexpr static Rule5Copy_t Rule5Copy = Rule5Copy_t{};

    /*
     * This need to provide correct API for 'const char*' (assuming that char
     * is ascii or UTF-8 - or other (invalid result) but in byte)
     */
    static_assert(sizeof(char) == sizeof(u8), "");

public:

    using Type = chara;

    template <typename T>
    struct ParsePrefixOk {
        T val;
        usize size;
    };

    enum class ParseBase : usize {

        Bin,
        Hex,
        Dec,
    };

    enum class ParseFloatBits : usize {

        Scientific = 1 << 0,
        Fixed = 1 << 2,

        // 'fast_float' can't parse float hex
        // Hex = 1 << 3,

        General = Fixed | Scientific,

        NoInfNan = (1 << 4), // no_infnan
    };
    ALT_ENUM_CLASS_BIT_FLAGS_IN_CLASS_DECL(ParseFloatBits);

    class Checkpoint {
    public:

        ALT_FORCE_INLINE Checkpoint(const Checkpoint&) noexcept = default;
        ALT_FORCE_INLINE Checkpoint& operator=(const Checkpoint&) noexcept = default;

        ALT_FORCE_INLINE Checkpoint(Checkpoint&&) noexcept = default;
        ALT_FORCE_INLINE Checkpoint& operator=(Checkpoint&&) noexcept = default;

        ALT_NODISCARD ALT_FORCE_INLINE usize GetOffsetInBytes() const noexcept {
            return static_cast<usize>(m_Offset);
        }

    private:

        ALT_FORCE_INLINE Checkpoint() noexcept = default;

    private:

        friend class StaticStringUTF8;

        Usize m_Offset = 0;
    };

    /**
     * @brief Helpers to handle ascii
     * 
     * https://cppreference.com/cpp/language/ascii
     * 
     * Printable ASCII [32..126]:
     *   ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ?
     * @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \ ] ^ _
     * ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~
     * 
     */
    ALT_NODISCARD ALT_FORCE_INLINE static bool IsAscii(const chara utf32) noexcept {

        return _StringDetails::IsAscii(utf32);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static bool IsAsciiWhitespace(const chara utf32) noexcept {

        return _StringDetails::IsAsciiWhitespace(utf32); // \t, \n, \v, \f, \r
    }

    ALT_NODISCARD ALT_FORCE_INLINE static bool IsAsciiPrintable(const chara utf32) noexcept {

        return _StringDetails::IsAsciiPrintable(utf32);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static bool IsAsciiAlphabetLower(const chara utf32) noexcept {

        return _StringDetails::IsAsciiAlphabetLower(utf32); // [a..z]
    }

    ALT_NODISCARD ALT_FORCE_INLINE static bool IsAsciiAlphabetUpper(const chara utf32) noexcept {

        return _StringDetails::IsAsciiAlphabetUpper(utf32); // [A..Z]
    }

    ALT_NODISCARD ALT_FORCE_INLINE static bool IsAsciiAlphabet(const chara utf32) noexcept {

        return _StringDetails::IsAsciiAlphabet(utf32);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static bool IsAsciiDigit(const chara utf32) noexcept {

        return _StringDetails::IsAsciiDigit(utf32); // [0..9]
    }

    ALT_NODISCARD ALT_FORCE_INLINE static bool IsAsciiHex(const chara utf32) noexcept {

        return _StringDetails::IsAsciiHex(utf32); // [0..9] [ABCDEF] [abcdef]
    }

    ALT_NODISCARD ALT_FORCE_INLINE static chara AsciiToLower(const chara utf32) noexcept {

        return _StringDetails::AsciiToLower(utf32);
    }

    ALT_NODISCARD ALT_FORCE_INLINE static chara AsciiToUpper(const chara utf32) noexcept {

        return _StringDetails::AsciiToUpper(utf32);
    }

    ALT_NODISCARD static usize Hash(const Slice<const u8> bytes, const usize seed = 0) noexcept {

        return Algorithms::Hash(bytes.Data(), bytes.DataEnd(), seed);
    }

    ALT_NODISCARD static usize Hash(const u8* pBytesBegin, const usize size, const usize seed = 0) noexcept {

        return Algorithms::Hash(pBytesBegin, pBytesBegin + size, seed);
    }

    ALT_NODISCARD static u64 Hash64(const Slice<const u8> bytes, const u64 seed = 0) noexcept {

        return Algorithms::Hash64(bytes.Data(), bytes.DataEnd(), seed);
    }

    ALT_NODISCARD static u64 Hash64(const u8* pBytesBegin, const usize size, const u64 seed = 0) noexcept {

        return Algorithms::Hash64(pBytesBegin, pBytesBegin + size, seed);
    }

    ALT_NODISCARD static u32 Hash32(const Slice<const u8> bytes, const u32 seed = 0) noexcept {

        return Algorithms::Hash32(bytes.Data(), bytes.DataEnd(), seed);
    }

    ALT_NODISCARD static u32 Hash32(const u8* pBytesBegin, const usize size, const u32 seed = 0) noexcept {

        return Algorithms::Hash64(pBytesBegin, pBytesBegin + size, seed);
    }

    /**
     * @brief Calculates the size in codepoints of a null-terminated string.
     * @param str The null-terminated source string (supports char, char16_t, char32_t, or wchar_t).
     * @return The total number of characters before the null terminator.
     * @example
     * 
     * assert(Alt::StaticString<255>::CalculateSizeInBytes("String") == 6);
     */
    template <typename T,
        std::enable_if<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD static usize CalculateCStrLen(const T* str) noexcept {

        return _StringDetails::CalculateCStrLen(str);
    }

    /**
     * @brief Calculates the size in bytes of a null-terminated string.
     * @param str The null-terminated source string (supports char, char16_t, char32_t, or wchar_t).
     * @return The total number of bytes occupied by the characters before the null terminator.
     * @example
     * 
     * assert(Alt::StaticString<255>::CalculateSizeInBytes("String") == 6);
     */
    template <typename T,
        std::enable_if<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD static usize CalculateSizeInBytes(const T* str) noexcept {

        return _StringDetails::CalculateSizeInBytes(str);
    }

    /**
     * @brief Creates an empty String.
     * @return Instance of StaticString.
     * @example
     * 
     * Alt::StaticString<255> string = Alt::StaticString::Create();
     * Alt::ASSERT(s.IsEmpty());
     */
    ALT_NODISCARD ALT_FORCE_INLINE static StaticStringUTF8 Create() noexcept {

        return StaticStringUTF8{};
    }

    /**
     * @brief Creates a new String containing the provided string content (char - UTF-8,
     * char16_t, wchar_t (Win32) - UTF-16, char32_t, wchar_t (non Win32) - UTF-32).
     * 
     * @param str The source string to copy.
     * @param size Optional explicit size characters. If Nullopt, it is determined by the null terminator.
     * @return An Alt::Optional containing the new String, or Alt::Nullopt if capacity excited.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticString<255> string0 = Alt::StaticString<255>::Create("Hello") iferr_return;
     * Alt::StaticString<255> string1 = Alt::StaticString<255>::Create(L"Hello") iferr_return;
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_character_v<InnerT>>* = nullptr>
    ALT_NODISCARD static Optional<StaticStringUTF8> Create(T&& str_) noexcept {
        iferr_scope;

        const Slice<const InnerT> str = str_;

        StaticStringUTF8 result{};
        result.Append(str) iferr_return Nullopt;

        return result;
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD static Optional<StaticStringUTF8> Create(const T* str, Optional<usize> size = Nullopt) noexcept {
        iferr_scope;

        StaticStringUTF8 result{};
        result.Append(str, size) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Creates a new String from a byte slice with a specific encoding.
     * @param str The raw byte slice to copy.
     * @param encoding The detected or explicitly provided UTF encoding.
     * @return An Alt::Optional containing the new String, or Alt::Nullopt if capacity excited.
     * @example
     * 
     * iferr_scope;
     * 
     * const auto bytes = Alt::MakeArray<u8>('S', 't', 'r', 'i', 'n', 'g');
     * Alt::StaticString<255> string = Alt::StaticString<255>::Create(bytes, Alt::Encoding::UTF8) iferr_return;
     * 
     * assert(string == "String")
     */
    ALT_NODISCARD static Optional<StaticStringUTF8> Create(const Slice<const u8> str, const Utf::Encoding encoding = Utf::Encoding::Unknown) noexcept {
        iferr_scope;

        StaticStringUTF8 result{};
        result.Append(str, encoding) iferr_return Nullopt;

        return result;
    }

    ALT_NODISCARD static Optional<StaticStringUTF8> Create(const u8* pBegin, const usize size, const Utf::Encoding encoding = Utf::Encoding::Unknown) noexcept {

        const Slice<const u8> bytes{ pBegin, size };
        return Create(bytes, encoding);
    }

    /**
     * @brief Creates a string by pushing multiple values in one operation.
     * @tparam ArgsT The types of the arguments to push. Must be convertible to `chara`.
     * @param args A variadic list of characters/values to append.
     * @return The resulting String
     * @example
     * 
     * Alt::StaticString<255> string = Alt::StaticString<255>::AllocatePush('S', 't', 'r', 'i', 'n', 'g');
     * assert(string == "String");
     */
    template <typename ...ArgsT,
        std::enable_if_t<(is_populated_v<ArgsT...> && sizeof...(ArgsT) <= SIZE_IN_BYTES_MINUS_ONE) && std::conjunction_v<is_pushable_t<ArgsT&&>...>>* = nullptr>
    ALT_NODISCARD static StaticStringUTF8 CreatePush(ArgsT&&... args) noexcept {

        StaticStringUTF8 result{};
        PushVariadicImpl(result, std::forward<ArgsT>(args)...);

        return result;
    }

    ALT_FORCE_INLINE StaticStringUTF8() noexcept {

        *GetMemoryNullTerm() = '\0';
    }

    /**
     * @brief Constructor for string array.
     * @tparam N The size of array of chars.
     * @param str The array of chars.
     * @example
     * 
     * Alt::StaticString<255> string = "String";
     * assert(string == "String");
     */
    template <usize N,
        std::enable_if_t<(N <= SIZE_IN_BYTES)>* = nullptr>
    StaticStringUTF8(const char (&str)[N]) noexcept {
        ALT_ASSERT(str[N - 1] == '\0');

        ALT_MAYBE_UNUSED const bool isOk = Append(str, N - 1);
        ALT_ASSERT(isOk);
    }

    template <usize N,
        std::enable_if_t<(N <= SIZE_IN_BYTES)>* = nullptr>
    StaticStringUTF8& operator=(const char (&str)[N]) noexcept {
        ALT_ASSERT(str[N - 1] == '\0');

        Clear();

        ALT_MAYBE_UNUSED const bool isOk = Append(str, N - 1);
        ALT_ASSERT(isOk);

        return *this;
    }

    /**
     * @brief Constructor for variadic push.
     * @tparam ArgsT The types of the arguments to push. Must be convertible to `chara`.
     * @param args A variadic list of characters/values to append.
     * @example
     * 
     * Alt::StaticString<255> string{ 'S', 't', 'r', 'i', 'n', 'g' };
     * assert(string == "String");
     */
    template <typename ...ArgsT,
        std::enable_if_t<(sizeof...(ArgsT) <= SIZE_IN_BYTES_MINUS_ONE) && std::conjunction_v<std::is_constructible<chara, ArgsT&&>...>>* = nullptr>
    StaticStringUTF8(ArgsT&&... args) noexcept {

        PushVariadicImpl(*this, std::forward<ArgsT>(args)...);
    }

    /**
     * @brief Move constructor.
     * @param rhs The string to move from.
     */
    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_,
        std::enable_if_t<(SIZE_IN_BYTES_MINUS_ONE_ <= SIZE_IN_BYTES_MINUS_ONE)>* = nullptr>
    StaticStringUTF8(StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>&& rhs) noexcept
        : StaticStringUTF8(Rule5Move, rhs)
    {}

    /**
     * @brief Move assignment.
     * @param rhs The string to move from.
     */
    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_,
        std::enable_if_t<(SIZE_IN_BYTES_MINUS_ONE_ <= SIZE_IN_BYTES_MINUS_ONE)>* = nullptr>
    StaticStringUTF8& operator=(StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    /**
     * @brief Copy constructor.
     * @param rhs The string to copy from.
     */
    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_,
        std::enable_if_t<(SIZE_IN_BYTES_MINUS_ONE_ <= SIZE_IN_BYTES_MINUS_ONE)>* = nullptr>
    StaticStringUTF8(const StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>& rhs) noexcept
        : StaticStringUTF8(Rule5Copy, rhs)
    {}

    /**
     * @brief Copy assignment.
     * @param rhs The string to copy from.
     */
    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_,
        std::enable_if_t<(SIZE_IN_BYTES_MINUS_ONE_ <= SIZE_IN_BYTES_MINUS_ONE)>* = nullptr>
    StaticStringUTF8& operator=(const StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>& rhs) noexcept {

        Assign(Rule5Copy, rhs);
        return *this;
    }

    /**
     * @brief Returns the number of bytes in UTF-8.
     * @return The nuber of bytes of the String.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeBytes() const noexcept {

        return static_cast<usize>(m_Size);
    }

    /**
     * @brief Returns the total number of bytes the String can hold before a reallocation is required.
     * @return The capacity in bytes of the String.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetCapacity() const noexcept {

        return SIZE_IN_BYTES_MINUS_ONE;
    }

    /**
     * @brief Checks if the String contains no bytes.
     * @return True if empty, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        return m_Size == 0;
    }

    /**
     * @brief Checks if the String contains at least one byte.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept {

        return m_Size != 0;
    }

    /**
     * @brief Checks if the vector full - any push will return false.
     * @return True if full, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsFull() const noexcept {

        return m_Size == SIZE_IN_BYTES_MINUS_ONE;
    }

    /**
     * @brief Removes all bytes from the String.
     * @details Does not deallocate the underlying memory, but reset size.
     */
    ALT_FORCE_INLINE void Clear() noexcept {

        m_Size = 0;
        *GetMemoryNullTerm() = '\0';
    }

    /**
     * @brief Accesses the byte at the specified index.
     * @param i The index of the byte.
     * @return A byte.
     * @example
     * 
     * Alt::StaticString<255> string = "String";
     * 
     * assert(string[0] == 'S');
     * assert(string[5] == 'g');
     */
    ALT_NODISCARD ALT_FORCE_INLINE u8 operator[](const usize i) const noexcept {
        ALT_ASSERT(i < GetSizeBytes());

        return GetMemoryBegin()[i];
    }

    /**
     * @brief Returns a first byte.
     * @return First byte.
     */
    ALT_NODISCARD ALT_FORCE_INLINE u8 GetFirstByte() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *GetMemoryBegin();
    }

    /**
     * @brief Returns a last byte.
     * @return Last byte.
     */
    ALT_NODISCARD ALT_FORCE_INLINE u8 GetLastByte() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(GetMemoryNullTerm() - 1);
    }

    /**
     * @brief Returns a pointer to the beginning.
     * @return Pointer to the first byte.
     */
    ALT_NODISCARD ALT_FORCE_INLINE const u8* Data() const noexcept {

        return GetMemoryBegin();
    }

    /**
     * @brief Returns a raw pointer to the end of the String.
     * @return Pointer to the next after last byte.
     */
    ALT_NODISCARD ALT_FORCE_INLINE const u8* DataEnd() const noexcept {

        return GetMemoryNullTerm();
    }

    /**
     * @brief Returns a C-style null-terminated string pointer.
     * @return A `const char*` representation of the buffer.
     */
    ALT_NODISCARD ALT_FORCE_INLINE const char* CStr() const noexcept {

        return reinterpret_cast<const char*>(GetMemoryBegin());
    }

    /**
     * @brief Returns a C-style null-terminated string pointer to the end (effectively on '\0').
     * @return A `const char*` representation of the buffer.
     */
    ALT_NODISCARD ALT_FORCE_INLINE const char* CStrEnd() const noexcept {

        return reinterpret_cast<const char*>(GetMemoryNullTerm());
    }

    /**
     * @brief Returns a constant iterator for the String (chara in UTF-32).
     * @return An `StringIterator`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE StringUTF8Iterator GetIterator() const noexcept;

    /**
     * @brief Return a split iterator for the string.
     * @return An `SplitIterator` instance.
     * @example
     * 
     * const Alt::StaticString<255> str = "image/base64,";
     * const auto splits = str
     *     .GetSplitIterator("/")
     *     .CollectOrDefault<Alt::Vector<Alt::Slice<const char>>>();
     * 
     * assert(splits[0] == "image");
     * assert(splits[1] == "base64,");
     */
    ALT_NODISCARD ALT_FORCE_INLINE SplitIterator<const char> GetSplitIterator(const Slice<const char> pattern) const noexcept {

        return SplitIterator<const char>{ *this, pattern };
    }

    /**
     * Clonable Trait
     * 
     * @brief A deep copy current String to another.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticString<255> string = "String";
     * Alt::StaticString<255> string1;
     * string1.CloneFrom(string) iferr_return;
     * 
     * assert(string == string1);
     */
    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_>
    ALT_NODISCARD bool CloneFrom(const StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>& string) noexcept {

        if (static_cast<const void*>(this) == &string) {
            return true;
        }

        const usize stringSize = string.GetSizeBytes();
        if (stringSize == 0 && IsEmpty()) {
            return true;
        }

        if (stringSize > GetCapacity()) {
            return false;
        }

        std::memcpy(GetMemoryBegin(), string.Data(), stringSize);

        m_Size = static_cast<Usize>(stringSize);
        *GetMemoryNullTerm() = '\0';

        return true;
    }

    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_>
    ALT_NODISCARD bool CloneConstructFrom(const StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>& rhs) noexcept {
        ALT_ASSERT(static_cast<const void*>(this) != &rhs);

        iferr_scope_on_err {

            Memory::DestroyObject(*this);
        };

        Memory::ConstructDefaultObject<StaticStringUTF8>(this);
        CloneFrom(rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD Optional<StaticStringUTF8> Clone() const noexcept {
        iferr_scope;

        MaybeUninit<StaticStringUTF8> valueToConstruct;
        valueToConstruct.GetPtr()->CloneConstructFrom(*this) iferr_return Nullopt;

        defer {
            valueToConstruct.Destroy();
        };

        return std::move(valueToConstruct).GetVal();
    }

    /**
     * @brief Parses a full value from a string Slice or C-string.
     * @tparam T The target type to parse into.
     * @param str_ A C-string.
     * @return An Optional containing the value or Nullopt on failure.
     * @example
     * 
     * iferr_scope;
     * 
     * using String = Alt::StaticString<255>;
     * 
     * const usize val = String::ParseFrom<usize>("44") iferr_return;
     * assert(val == 44);
     * 
     * const Alt::Optional<usize> result = String::ParseFrom<usize>("44 abc");
     * assert(result.IsNone());
     */
    template <typename T,
        std::enable_if_t<std::is_same_v<T, u8>>* = nullptr>
    ALT_NODISCARD static Optional<u8> ParseFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u8>>* = nullptr>
    ALT_NODISCARD static Optional<u8> ParseFrom(Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {
            desc.base = 2;
            break;
        }
        case ParseBase::Hex: {
            desc.base = 16;
            break;
        }
        case ParseBase::Dec: {
            desc.base = 10;
            break;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        u8 val;
        const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u16>>* = nullptr>
    ALT_NODISCARD static Optional<u16> ParseFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u16>>* = nullptr>
    ALT_NODISCARD static Optional<u16> ParseFrom(Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {
            desc.base = 2;
            break;
        }
        case ParseBase::Hex: {
            desc.base = 16;
            break;
        }
        case ParseBase::Dec: {
            desc.base = 10;
            break;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        u16 val;
        const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_NODISCARD static Optional<u32> ParseFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_NODISCARD static Optional<u32> ParseFrom(Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {
            desc.base = 2;
            break;
        }
        case ParseBase::Hex: {
            desc.base = 16;
            break;
        }
        case ParseBase::Dec: {
            desc.base = 10;
            break;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        u32 val;
        const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD static Optional<u64> ParseFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD static Optional<u64> ParseFrom(Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {
            desc.base = 2;
            break;
        }
        case ParseBase::Hex: {
            desc.base = 16;
            break;
        }
        case ParseBase::Dec: {
            desc.base = 10;
            break;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        u64 val;
        const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s8>>* = nullptr>
    ALT_NODISCARD static Optional<s8> ParseFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s8>>* = nullptr>
    ALT_NODISCARD static Optional<s8> ParseFrom(Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u8 val_;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s8 val = Memory::Transmute<u8, s8>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u8 val_;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s8 val = Memory::Transmute<u8, s8>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s8 val;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s16>>* = nullptr>
    ALT_NODISCARD static Optional<s16> ParseFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s16>>* = nullptr>
    ALT_NODISCARD static Optional<s16> ParseFrom(Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u16 val_;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s16 val = Memory::Transmute<u16, s16>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u16 val_;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s16 val = Memory::Transmute<u16, s16>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s16 val;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s32>>* = nullptr>
    ALT_NODISCARD static Optional<s32> ParseFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s32>>* = nullptr>
    ALT_NODISCARD static Optional<s32> ParseFrom(Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u32 val_;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s32 val = Memory::Transmute<u32, s32>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u32 val_;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s32 val = Memory::Transmute<u32, s32>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s32 val;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s64>>* = nullptr>
    ALT_NODISCARD static Optional<s64> ParseFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s64>>* = nullptr>
    ALT_NODISCARD static Optional<s64> ParseFrom(Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u64 val_;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s64 val = Memory::Transmute<u64, s64>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u64 val_;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s64 val = Memory::Transmute<u64, s64>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s64 val;
            const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f32>>* = nullptr>
    ALT_NODISCARD static Optional<f32> ParseFrom(const char* str_, const char decPoint = '.') noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, decPoint);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f32>>* = nullptr>
    ALT_NODISCARD static Optional<f32> ParseFrom(const Slice<const char> str, const char decPoint = '.', const ParseFloatBits bits = ParseFloatBits::General) noexcept {

        fast_float::parse_options desc{};
        desc.decimal_point = decPoint;
        desc.format = static_cast<fast_float::chars_format>(bits);

        f32 val;
        const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f64>>* = nullptr>
    ALT_NODISCARD static Optional<f64> ParseFrom(const char* str_, const char decPoint = '.') noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, decPoint);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f64>>* = nullptr>
    ALT_NODISCARD static Optional<f64> ParseFrom(const Slice<const char> str, const char decPoint = '.', const ParseFloatBits bits = ParseFloatBits::General) noexcept {

        fast_float::parse_options desc{};
        desc.decimal_point = decPoint;
        desc.format = static_cast<fast_float::chars_format>(bits);

        f64 val;
        const fast_float::from_chars_result result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    /**
     * @brief Parses a prefix value from a string Slice or C-string.
     * @tparam T The target type to parse into.
     * @param str_ A C-string.
     * @return An Optional containing the value or Nullopt on failure.
     * @example
     * 
     * iferr_scope;
     * 
     * using String = Alt::StaticString<255>;
     * 
     * const String::ParsePrefixOk<usize> val0 = String::ParsePrefixFrom<usize>("44 abc") iferr_return;
     * assert(val0.val == 44);
     * assert(val0.size == 2);
     * 
     * const String::ParsePrefixOk<usize> val1 = String::ParsePrefixFrom<usize>("44") iferr_return;
     * assert(val1.val == 44);
     * assert(val1.size == 2);
     */
    template <typename T,
        std::enable_if_t<std::is_same_v<T, u8>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u8>> ParsePrefixFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u8>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u8>> ParsePrefixFrom(const Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {
            desc.base = 2;
            break;
        }
        case ParseBase::Hex: {
            desc.base = 16;
            break;
        }
        case ParseBase::Dec: {
            desc.base = 10;
            break;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        u8 val;
        const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

        if (parseResult.ec != std::errc{}) {
            return Nullopt;
        }

        ParsePrefixOk<T> result{};
        result.val = val;
        result.size = static_cast<usize>(parseResult.ptr - str.Data());

        return result;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u16>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u16>> ParsePrefixFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u16>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u16>> ParsePrefixFrom(const Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {
            desc.base = 2;
            break;
        }
        case ParseBase::Hex: {
            desc.base = 16;
            break;
        }
        case ParseBase::Dec: {
            desc.base = 10;
            break;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        u16 val;
        const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

        if (parseResult.ec != std::errc{}) {
            return Nullopt;
        }

        ParsePrefixOk<T> result{};
        result.val = val;
        result.size = static_cast<usize>(parseResult.ptr - str.Data());

        return result;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u32>> ParsePrefixFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u32>> ParsePrefixFrom(const Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {
            desc.base = 2;
            break;
        }
        case ParseBase::Hex: {
            desc.base = 16;
            break;
        }
        case ParseBase::Dec: {
            desc.base = 10;
            break;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        u32 val;
        const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

        if (parseResult.ec != std::errc{}) {
            return Nullopt;
        }

        ParsePrefixOk<T> result{};
        result.val = val;
        result.size = static_cast<usize>(parseResult.ptr - str.Data());

        return result;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u64>> ParsePrefixFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u64>> ParsePrefixFrom(const Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {
            desc.base = 2;
            break;
        }
        case ParseBase::Hex: {
            desc.base = 16;
            break;
        }
        case ParseBase::Dec: {
            desc.base = 10;
            break;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }

        u64 val;
        const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

        if (parseResult.ec != std::errc{}) {
            return Nullopt;
        }

        ParsePrefixOk<T> result{};
        result.val = val;
        result.size = static_cast<usize>(parseResult.ptr - str.Data());

        return result;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s8>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s8>> ParsePrefixFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s8>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s8>> ParsePrefixFrom(const Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u8 val_;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s8 val = Memory::Transmute<u8, s8>(val_);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u8 val_;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s8 val = Memory::Transmute<u8, s8>(val_);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s8 val;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s16>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s16>> ParsePrefixFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s16>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s16>> ParsePrefixFrom(const Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u16 val_;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s16 val = Memory::Transmute<u16, s16>(val_);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u16 val_;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s16 val = Memory::Transmute<u16, s16>(val_);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s16 val;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s32>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s32>> ParsePrefixFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s32>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s32>> ParsePrefixFrom(const Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u32 val_;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s32 val = Memory::Transmute<u32, s32>(val_);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u32 val_;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s32 val = Memory::Transmute<u32, s32>(val_);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s32 val;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s64>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s64>> ParsePrefixFrom(const char* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s64>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s64>> ParsePrefixFrom(const Slice<const char> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u64 val_;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s64 val = Memory::Transmute<u64, s64>(val_);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u64 val_;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s64 val = Memory::Transmute<u64, s64>(val_);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s64 val;
            const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

            if (parseResult.ec != std::errc{}) {
                return Nullopt;
            }

            ParsePrefixOk<T> result{};
            result.val = val;
            result.size = static_cast<usize>(parseResult.ptr - str.Data());

            return result;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f32>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<f32>> ParsePrefixFrom(const char* str_, const char decPoint = '.') noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, decPoint);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f32>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<f32>> ParsePrefixFrom(const Slice<const char> str, const char decPoint = '.', const ParseFloatBits bits = ParseFloatBits::General) noexcept {

        fast_float::parse_options desc{};
        desc.decimal_point = decPoint;
        desc.format = static_cast<fast_float::chars_format>(bits);

        f32 val;
        const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

        if (parseResult.ec != std::errc{}) {
            return Nullopt;
        }

        ParsePrefixOk<T> result{};
        result.val = val;
        result.size = static_cast<usize>(parseResult.ptr - str.Data());

        return result;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f64>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<f64>> ParsePrefixFrom(const char* str_, const char decPoint = '.') noexcept {

        const Slice<const char> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, decPoint);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f64>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<f64>> ParsePrefixFrom(const Slice<const char> str, const char decPoint = '.', const ParseFloatBits bits = ParseFloatBits::General) noexcept {

        fast_float::parse_options desc{};
        desc.decimal_point = decPoint;
        desc.format = static_cast<fast_float::chars_format>(bits);

        f64 val;
        const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

        if (parseResult.ec != std::errc{}) {
            return Nullopt;
        }

        ParsePrefixOk<T> result{};
        result.val = val;
        result.size = static_cast<usize>(parseResult.ptr - str.Data());

        return result;
    }

    /**
     * @brief Convert value to string.
     * @tparam T The target type to convert.
     * @param val A value.
     * @return An true if success, or false on failure.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticString<255> str;
     * str.ToStringAppend<usize>(44) iferr_return;
     * 
     * assert(str == "44");
     */
    template <typename T,
        std::enable_if_t<std::is_same_v<T, u8>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u16>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s8>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s16>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s32>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s64>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, void*>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const void* ptr) noexcept {

        return ToStringAppendImpl<T>(ptr);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f32>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f64>>* = nullptr>
    ALT_NODISCARD bool ToStringAppend(const T val) noexcept {

        return ToStringAppendImpl<T>(val);
    }

    /**
     * @brief Convert arguments to string using 'format'.
     * @tparam ArgsT The target types to convert.
     * @param args A arguments.
     * @return FormatResult (Result<void, FormatError>), see. `Format`.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticString<255> str;
     * str.FormatAppend("{{} {}: {} + {} == {} {{{{}}", "Easy Format", 4, 8, 12) iferr_return;
     * 
     * assert(str == "{} Easy Format: 4 + 8 == 12 {{}");
     */
    template <typename ...ArgsT>
    FormatResult Format(const char* fmt_, const ArgsT&... args) noexcept {

        Clear();
        return FormatAppend(fmt_, args...);
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>, typename ...ArgsT,
        std::enable_if_t<std::is_constructible_v<const Slice<const char>, T&&>>* = nullptr>
    FormatResult Format(T&& fmt_, const ArgsT&... args) noexcept {

        Clear();
        return FormatAppend(fmt_, args...);
    }

    template <typename ...ArgsT>
    FormatResult FormatAppend(const char* fmt_, const ArgsT&... args) noexcept {

        const Slice<const char> fmt{ fmt_, CalculateCStrLen(fmt_) };
        return FormatAppend(fmt, args...);
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>, typename ...ArgsT,
        std::enable_if_t<std::is_constructible_v<const Slice<const char>, T&&>>* = nullptr>
    FormatResult FormatAppend(T&& fmt_, const ArgsT&... args) noexcept {

        const usize sizeOld = GetSizeBytes();
        iferr_scope_on_err {

            m_Size = static_cast<Usize>(sizeOld);
            *GetMemoryNullTerm() = '\0';
        };

        const Slice<const char> fmt = fmt_;
        _StringDetails::StaticStringFormatWriter<SIZE_IN_BYTES_MINUS_ONE, Usize> writer{ *this };

        FormatError err;
        _StringDetails::StaticStringFormatWriter<SIZE_IN_BYTES_MINUS_ONE, Usize>::FormatWrite(writer, fmt, args...) iferr_return_with(err) err;

        *GetMemoryNullTerm() = '\0';

        return FormatResult::Ok();
    }

    /**
     * @brief Append copy bytes to the current String end.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticString<255> string;
     * string.Append("String") iferr_return;
     * string.Append(" from") iferr_return;
     * string.Append(" Alt") iferr_return;
     * 
     * assert(string == "String from Alt");
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD bool Append(const T* str, Optional<usize> size = Nullopt) noexcept {

        if (!size) {

            size = CalculateCStrLen(str);
        }

        const Slice<const u8> bytes{ reinterpret_cast<const u8*>(str), *size * sizeof(T) };
        return Append(bytes, _StringDetails::character_to_utf_type_v<T>);
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_character_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool Append(T&& str_) noexcept {

        const Slice<const InnerT> str = str_;
        const Slice<const u8> bytes{ reinterpret_cast<const u8*>(str.Data()), reinterpret_cast<const u8*>(str.DataEnd()) };

        return Append(bytes, _StringDetails::character_to_utf_type_v<InnerT>);
    }

    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_>
    ALT_NODISCARD bool Append(const StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>& str) noexcept {

        const Slice<const u8> bytes{ str.GetMemoryBegin(), str.GetMemoryNullTerm() };
        return AppendUtf8NoValidation(bytes);
    }

    /**
     * @brief Append copy bytes to the current String end.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * auto bytes = Alt::MakeStatic('S', 't', 'r', 'i', 'n', 'g');
     * 
     * Alt::String string;
     * string.Append(bytes, Alt::Utf::Encoding::UTF8) iferr_return;
     * 
     * assert(string == "String");
     */
    ALT_NODISCARD bool Append(Slice<const u8> str, Utf::Encoding encoding = Utf::Encoding::Unknown) noexcept {

        // Not necessary but for compatibility with 'String'
        ALT_ASSERT(!Memory::IsOverlap(GetMemoryBegin(), GetMemoryNullTerm(), str.Data(), str.DataEnd()));

        if (encoding == Utf::Encoding::Unknown) {

            Utf::GuessEncodingResult guessed = Utf::GuessEncodingNullTermAware(str);
            ALT_ASSERT(guessed.isLittleEndian == Platform::IsLittleEndian()); // "need to swap bytes"

            str = str.SubSlice(guessed.bytesConsumedByBom);

            encoding = Utf::GuessedEncodingToFirstEncoding(guessed.guessedEncoding);
        }

        switch (encoding) {
        case Utf::Encoding::Utf8: {

            return AppendUtf8(str);
        }
        case Utf::Encoding::Utf16: {

            ALT_ASSERT(str.GetSize() % sizeof(u16) == 0);
            ALT_ASSERT(Memory::Align(str.Data(), alignof(u16)) == 0);

            const Slice<const u16> strUtf16{
                reinterpret_cast<const u16*>(str.Data()),
                reinterpret_cast<const u16*>(str.DataEnd()),
            };

            return AppendUtf16(strUtf16);
        }
        case Utf::Encoding::Utf32: {

            ALT_ASSERT(str.GetSize() % sizeof(u32) == 0);
            ALT_ASSERT(Memory::Align(str.Data(), alignof(u32)) == 0);

            const Slice<const u32> strUtf32{
                reinterpret_cast<const u32*>(str.Data()),
                reinterpret_cast<const u32*>(str.DataEnd()),
            };

            return AppendUtf32(strUtf32);
        }

        default: {

            return false;
        }
        }
    }

    ALT_NODISCARD bool Append(const u8* pBegin, const usize size, Utf::Encoding encoding = Utf::Encoding::Unknown) noexcept {

        const Slice<const u8> bytes{ pBegin, size };
        return Append(bytes, encoding);
    }

    /**
     * @brief Aliases for 'Push(...)'.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(const chara utf32) noexcept {

        return Push(utf32);
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(const T val) noexcept {

        return Push(val);
    }

    /**
     * @brief Appends an chara (UTF-32) to the end of the String.
     * @param val The value to push.
     * @return True if the chara was successfully added, false if memory allocation failed or invalid UTF-32.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticString<255> string;
     * string.Push('S') iferr_return;
     * 
     * assert(string[0] == 'S');
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const chara utf32) noexcept {

        if (!Utf::_32::IsValidUtf32(utf32)) {

            return false;
        }

        const usize lenInBytes = Utf::_32::LengthUtf32ToUtf8(utf32);

        if (lenInBytes > GetCapacity() - GetSizeBytes()) {
            return false;
        }

        u8 utf8[4];
        Utf::Result result = Utf::_32::Utf32ToUtf8IgnoreErrors(utf32, utf8);

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        if (0 < result.consumed) *(pMemoryNullTerm + 0) = utf8[0];
        if (1 < result.consumed) *(pMemoryNullTerm + 1) = utf8[1];
        if (2 < result.consumed) *(pMemoryNullTerm + 2) = utf8[2];
        if (3 < result.consumed) *(pMemoryNullTerm + 3) = utf8[3];

        m_Size += static_cast<Usize>(lenInBytes);

        pMemoryNullTerm += lenInBytes;
        *pMemoryNullTerm = '\0';

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const char utf8) noexcept {

        if (!IsAscii(utf8)) {

            return false;
        }

        if (GetCapacity() - GetSizeBytes() < 1) {
            return false;
        }

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        *pMemoryNullTerm = utf8;

        m_Size += 1;

        pMemoryNullTerm += 1;
        *pMemoryNullTerm = '\0';

        return true;
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const char8_t utf8) noexcept {

        return Push(static_cast<char>(utf8));
    }
#endif // __cpp_char8_t

    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const char16_t (&utf16_)[2]) noexcept {
        iferr_scope;

        const u16 utf16[2] = Memory::Transmute<char16_t[2], u16[2]>(utf16_);

        if (!Utf::_16::IsValidUtf16(utf16)) {

            return false;
        }

        const usize lenInBytes = Utf::_16::LengthUtf16ToUtf8(utf16);

        if (lenInBytes > GetCapacity() - GetSizeBytes()) {
            return false;
        }

        u8 utf8[4];
        Utf::Result result = Utf::_16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        if (0 < result.consumed) *(pMemoryNullTerm + 0) = utf8[0];
        if (1 < result.consumed) *(pMemoryNullTerm + 1) = utf8[1];
        if (2 < result.consumed) *(pMemoryNullTerm + 2) = utf8[2];
        if (3 < result.consumed) *(pMemoryNullTerm + 3) = utf8[3];

        m_Size += static_cast<Usize>(lenInBytes);

        pMemoryNullTerm += lenInBytes;
        *pMemoryNullTerm = '\0';

        return true;
    }

    /**
     * @brief Variadic append an charas to the end of the String.
     * @param ...vals The values to push.
     * @return True if the element was successfully added, false if memory allocation failed or invalid UTF-32.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StaticString<255> string;
     * string.Push('S', 't', 'r', 'i', 'n', 'g') iferr_return;
     * 
     * assert(string[0] == 'S');
     * assert(string[1] == 't');
     * assert(string.GetSizeBytes() == 6);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT, chara>...>>* = nullptr>
    ALT_NODISCARD bool Push(ArgsT&&... charas) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSizeBytes(), Utf::_32::LengthUtf32ToUtf8(charas)...) iferr_return false;

        if (size > GetCapacity() - GetSizeBytes()) {
            return false;
        }

        PushVariadicImpl(*this, charas...);

        return true;
    }

    /**
     * @brief Removes and returns the last chara of the String.
     * @return The chara (UTF-32) that was removed.
     * @note This method requires the String to not be empty. Calling on an empty String triggers an assertion.
     * @example
     * 
     * Alt::StaticString<255> string = "String";
     * chara ch = string.Pop();
     * 
     * assert(ch == 'g');
     */
    ALT_FORCE_INLINE chara Pop() noexcept {
        ALT_ASSERT(!IsEmpty());

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        const u8 utf8[4]{
            m_Size >= 4 ? *(pMemoryNullTerm - 4) : static_cast<u8>(0),
            m_Size >= 3 ? *(pMemoryNullTerm - 3) : static_cast<u8>(0),
            m_Size >= 2 ? *(pMemoryNullTerm - 2) : static_cast<u8>(0),
            *(pMemoryNullTerm - 1),
        };

        u32 utf32;
        Utf::Result result = Utf::_8::Utf8ToUtf32IgnoreErrorsBackward(utf8, utf32);

        ALT_ASSERT(m_Size >= result.advance);
        m_Size -= static_cast<Usize>(result.advance);

        pMemoryNullTerm -= result.advance;
        *pMemoryNullTerm = '\0';

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Get actual checkpoint of valid UTF-8 string, see. 'TruncateToCheckpoint(...)'.
     * @example
     * 
     * iferr_scope;
     * 
     * using String = Alt::StaticString<255>;
     * 
     * String path = "C:\\Temp";
     * String::Checkpoint checkpoint = path.GetCheckpoint();
     * 
     * {
     *     path.FormatAppend("\\Downloads\\Data") iferr_return;
     *     assert(path == "C:\\Temp\\Downloads\\Data"_s);
     * }
     * 
     * path.TruncateToCheckpoint(checkpoint);
     * assert(path == "C:\\Temp"_s);
     */
    ALT_NODISCARD ALT_FORCE_INLINE Checkpoint GetCheckpoint() const noexcept {

        Checkpoint checkpoint{};
        checkpoint.m_Offset = m_Size;

        return checkpoint;
    }

    /**
     * @brief Truncate actual buffer to valid checkpoint state.
     */
    ALT_FORCE_INLINE void TruncateToCheckpoint(const Checkpoint checkpoint) noexcept {
        ALT_ASSERT(checkpoint.GetOffsetInBytes() <= GetSizeBytes());

        m_Size = checkpoint.m_Offset;
        *GetMemoryNullTerm() = '\0';
    }

    /**
     * @brief Trims whitespace from the left.
     */
    void TrimLeft() noexcept {

        if (TrimLeftNoNullTermWrite()) {

            *GetMemoryNullTerm() = '\0';
        }
    }

    /**
     * @brief Trims whitespace from the right.
     */
    void TrimRight() noexcept {

        if (TrimRightNoNullTermWrite()) {

            *GetMemoryNullTerm() = '\0';
        }
    }

    /**
     * @brief Trims whitespace.
     */
    void Trim() noexcept {

        if (m_Size == 0) {
            return;
        }

        bool isOk = false;
        isOk |= TrimRightNoNullTermWrite();
        isOk |= TrimLeftNoNullTermWrite();

        if (isOk) {

            *GetMemoryNullTerm() = '\0';
        }
    }

    /**
     * @brief Finds the first occurrence of a value in the string.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to search for.
     * @return An Optional containing the index or Nullopt if not found.
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::StaticString<255> str = "String";
     * 
     * const usize idx = str.FindFirst('i') iferr_return;
     * assert(idx == 3); // index in bytes
     */
    ALT_NODISCARD Optional<usize> FindFirst(const chara utf32) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), utf32);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const u8 utf8) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char utf8) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char (&utf8)[4]) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindFirst(const char8_t utf8) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindFirst(const char16_t utf16) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), utf16);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), utf16);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const wchar_t wchar) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), wchar);
    }

    /**
     * @brief Finds the last occurrence of a value in the string.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to search for.
     * @return An Optional containing the index or Nullopt if not found.
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::StaticString<255> str = "String.alt";
     * 
     * const usize idx = str.FindLast('.') iferr_return;
     * assert(idx == 6); // index in bytes
     */
    ALT_NODISCARD Optional<usize> FindLast(const chara utf32) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), utf32);
    }

    ALT_NODISCARD Optional<usize> FindLast(const u8 utf8) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char utf8) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char (&utf8)[4]) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindLast(const char8_t utf8) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindLast(const char16_t utf16) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), utf16);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), utf16);
    }

    ALT_NODISCARD Optional<usize> FindLast(const wchar_t wchar) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), wchar);
    }

    /**
     * @brief Check if we start with the occurrence in the string.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to comare for.
     * @return An true if start with or false if not.
     * @example
     * 
     * const Alt::StaticString<255> str = "String";
     * 
     * const bool isStartWith = str.StartWith('S');
     * assert(isStartWith);
     */
    ALT_NODISCARD bool StartWith(const chara utf32) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), utf32);
    }

    ALT_NODISCARD bool StartWith(const u8 utf8) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD bool StartWith(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD bool StartWith(const char utf8) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD bool StartWith(const char (&utf8)[4]) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD bool StartWith(const char8_t utf8) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD bool StartWith(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD bool StartWith(const char16_t utf16) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), utf16);
    }

    ALT_NODISCARD bool StartWith(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), utf16);
    }

    ALT_NODISCARD bool StartWith(const wchar_t wchar) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), wchar);
    }

    /**
     * @brief Check if we end with the occurrence in the string.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to comare for.
     * @return An true if end with or false if not.
     * @example
     * 
     * const Alt::StringString<255> str = "String.alt";
     * 
     * const bool isEndWith = str.EndWith('t');
     * assert(isEndWith);
     */
    ALT_NODISCARD bool EndWith(const chara utf32) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), utf32);
    }

    ALT_NODISCARD bool EndWith(const u8 utf8) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD bool EndWith(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD bool EndWith(const char utf8) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD bool EndWith(const char (&utf8)[4]) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD bool EndWith(const char8_t utf8) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }

    ALT_NODISCARD bool EndWith(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD bool EndWith(const char16_t utf16) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), utf16);
    }

    ALT_NODISCARD bool EndWith(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), utf16);
    }

    ALT_NODISCARD bool EndWith(const wchar_t wchar) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), wchar);
    }

    /**
     * @brief Finds the first occurrence of a string within the string.
     * @param str A String compatible with UTF-8.
     * @return An Optional index where the substring starts.
     * @example
     * 
     * const Alt::StringString<255> str = "String";
     * 
     * const usize idx = str.FindFirst("rin");
     * assert(idx == 2); // index in bytes
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindFirst(T&& str) const noexcept {

        const Slice<const InnerT> bytes = str;
        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), bytes.Data(), bytes.DataEnd());
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindFirst(const T* str) const noexcept {

        return _StringDetails::FindFirst(GetMemoryBegin(), GetMemoryNullTerm(), str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Finds the last occurrence of a string within the string.
     * @param str A String compatible with UTF-8.
     * @return An Optional index where the substring starts.
     * @example
     * 
     * const Alt::StringString<255> str = "String.alt";
     * 
     * const usize idx = str.FindLast(".alt");
     * assert(idx == 6); // index in bytes
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindLast(T&& str) const noexcept {

        const Slice<const InnerT> bytes = str;
        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), bytes.Data(), bytes.DataEnd());
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindLast(const T* str) const noexcept {

        return _StringDetails::FindLast(GetMemoryBegin(), GetMemoryNullTerm(), str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Checks if the string starts with the given substring.
     * @tparam str A String compatible with UTF-8.
     * @param str The prefix to check for.
     * @return True if the prefix is found at the beginning, false otherwise.
     * @example
     * 
     * const Alt::StringString<255> str = "String";
     * 
     * const bool isStart = str.StartWith("St");
     * assert(isStart);
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool StartWith(T&& str) const noexcept {

        const Slice<const InnerT> bytes = str;
        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), bytes.Data(), bytes.DataEnd());
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD bool StartWith(const T* str) const noexcept {

        return _StringDetails::StartWith(GetMemoryBegin(), GetMemoryNullTerm(), str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Checks if the string ends with the given substring.
     * @tparam str A String compatible with UTF-8.
     * @param str The suffix to check for.
     * @return True if the suffix is found at the end, false otherwise.
     * @example
     * 
     * const Alt::StringString<255> str = "String";
     * 
     * const bool isEnd = str.EndWith("ng");
     * assert(isEnd);
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool EndWith(T&& str) const noexcept {

        const Slice<const InnerT> bytes = str;
        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), bytes.Data(), bytes.DataEnd());
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD bool EndWith(const T* str) const noexcept {

        return _StringDetails::EndWith(GetMemoryBegin(), GetMemoryNullTerm(), str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Calculates a hash value for the string.
     * @param seed The initial seed for hashing.
     * @return A hash result.
     */
    ALT_NODISCARD usize CalculateHash(const usize seed = 0) const noexcept {

        const Slice<const u8> bytes = *this;
        return Hash(bytes, seed);
    }

    /**
     * @brief Compares strings.
     * @param str The null-terminated string to compare against.
     * @return -1 if this < str, 0 if this == str, and +1 if this > str.
     * @example
     * 
     * Alt::StringString<255> string = "String";
     * 
     * assert(string.Compare("String") == 0);
     * assert(string.Compare("Stringa") < 0);
     * assert(string.Compare("Strin") > 0);
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD ssize Compare(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;

        return Algorithms::Compare(
            GetMemoryBegin(),
            GetMemoryNullTerm(),
            reinterpret_cast<const u8*>(rhs.Data()),
            reinterpret_cast<const u8*>(rhs.DataEnd())
        );
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD ssize Compare(const T* rhs) const noexcept {

        const s32 result = std::strcmp(CStr(), reinterpret_cast<const char*>(rhs));

        if (result == 0) {
            return 0;
        }

        return result < 0 ? -1 : 1;
    }

    /**
     * @brief Operators for comparison with String.
     * @example
     * 
     * Alt::StringString<255> string = "String";
     * assert(string == "String");
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator==(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return Compare(rhs) == 0;
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator!=(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return !(*this == rhs);
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator<(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return Compare(rhs) < 0;
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator<=(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return !(*this > rhs);
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator>(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return Compare(rhs) > 0;
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf8_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator>=(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return !(*this < rhs);
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD bool operator==(const T* rhs) const noexcept {
        return Compare(rhs) == 0;
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD bool operator!=(const T* rhs) const noexcept {
        return !(*this == rhs);
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD bool operator<(const T* rhs) const noexcept {
        return Compare(rhs) < 0;
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD bool operator<=(const T* rhs) const noexcept {
        return !(*this > rhs);
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD bool operator>(const T* rhs) const noexcept {
        return Compare(rhs) > 0;
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf8_v<T>>* = nullptr>
    ALT_NODISCARD bool operator>=(const T* rhs) const noexcept {
        return !(*this < rhs);
    }

private:

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE static void PushVariadicImpl(StaticStringUTF8& self, HeadT&& head, TailT&&... tail) noexcept {

        ALT_MAYBE_UNUSED const bool isOk = self.Push(std::forward<HeadT>(head));

        // must be checked by previouse call - never fail
        ALT_ASSERT(isOk);

        PushVariadicImpl(self, std::forward<TailT>(tail)...);
    }

    ALT_FORCE_INLINE static void PushVariadicImpl(StaticStringUTF8&) noexcept {}

    template <typename T>
    ALT_FORCE_INLINE bool ToStringAppendImpl(const T val) noexcept {

        const usize sizeOld = GetSizeBytes();
        iferr_scope_on_err {

            m_Size = static_cast<Usize>(sizeOld);
            *GetMemoryNullTerm() = '\0';
        };

        _StringDetails::StaticStringFormatWriter<SIZE_IN_BYTES_MINUS_ONE, Usize> writer{ *this };
        Alt::Format<T>::WriteAppend(writer, ""_s, val) iferr_return false;

        *GetMemoryNullTerm() = '\0';

        return true;
    }

    ALT_FORCE_INLINE bool TrimLeftNoNullTermWrite() noexcept {

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        const u8* pBegin = _StringDetails::TrimLeftImpl(GetMemoryBegin(), pMemoryNullTerm);

        if (pBegin == GetMemoryBegin()) {
            return false;
        }

        const usize sizeNew = static_cast<usize>(pMemoryNullTerm - pBegin);
        std::memmove(GetMemoryBegin(), pBegin, sizeNew * sizeof(u8));

        m_Size = static_cast<Usize>(sizeNew);

        return true;
    }

    ALT_FORCE_INLINE bool TrimRightNoNullTermWrite() noexcept {

        if (m_Size == 0) {
            return false;
        }

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        const u8* pEnd = _StringDetails::TrimRightImpl(GetMemoryBegin(), pMemoryNullTerm);

        const bool isShouldWriteNullTerm = pEnd != pMemoryNullTerm;
        m_Size = static_cast<Usize>(pEnd - GetMemoryBegin());

        return isShouldWriteNullTerm;
    }

    ALT_FORCE_INLINE bool AppendUtf8(const Slice<const u8> utf8) noexcept {

        if (Utf::ValidateUtf8(utf8) != utf8.GetSize()) {

            return false;
        }

        return AppendUtf8NoValidation(utf8);
    }

    ALT_FORCE_INLINE bool AppendUtf8NoValidation(const Slice<const u8> utf8) noexcept {

        const usize sizeToAppend = utf8.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        if (sizeToAppend > GetCapacity() - GetSizeBytes()) {
            return false;
        }

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        std::memcpy(pMemoryNullTerm, utf8.Data(), sizeToAppend);

        m_Size += static_cast<Usize>(sizeToAppend);

        pMemoryNullTerm += sizeToAppend;
        *pMemoryNullTerm = '\0';

        return true;
    }

    ALT_FORCE_INLINE bool AppendUtf16(const Slice<const u16> utf16) noexcept {

        usize sizeToAppend;
        bool isOk = Utf::Utf16ToUtf8(utf16, nullptr, sizeToAppend);

        if (sizeToAppend == 0) {
            return true;
        }

        if (sizeToAppend > GetCapacity() - GetSizeBytes()) {
            return false;
        }

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        const Usize sizeToAppendReserved = static_cast<Usize>(sizeToAppend);
        isOk = Utf::Utf16ToUtf8(utf16, pMemoryNullTerm, sizeToAppend);

        m_Size += isOk ? sizeToAppendReserved : 0;

        pMemoryNullTerm += isOk ? sizeToAppendReserved : 0;
        *pMemoryNullTerm = '\0';

        return isOk;
    }

    ALT_FORCE_INLINE bool AppendUtf16NoValidation(const Slice<const u16> utf16) noexcept {
        iferr_scope;

        usize sizeToAppend;
        Utf::Utf16ToUtf8IgnoreErrors(utf16, nullptr, sizeToAppend);

        if (sizeToAppend == 0) {
            return true;
        }

        if (sizeToAppend > GetCapacity() - GetSizeBytes()) {
            return false;
        }

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        const Usize sizeToAppendReserved = static_cast<Usize>(sizeToAppend);
        Utf::Utf16ToUtf8IgnoreErrors(utf16, pMemoryNullTerm, sizeToAppend);

        m_Size += sizeToAppendReserved;
        *pMemoryNullTerm = '\0';

        return true;
    }

    ALT_FORCE_INLINE bool AppendUtf32(const Slice<const u32> utf32) noexcept {

        usize sizeToAppend;
        bool isOk = Utf::Utf32ToUtf8(utf32, nullptr, sizeToAppend);

        if (sizeToAppend == 0) {
            return true;
        }

        if (sizeToAppend > GetCapacity() - GetSizeBytes()) {
            return false;
        }

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        const Usize sizeToAppendReserved = static_cast<Usize>(sizeToAppend);
        isOk = Utf::Utf32ToUtf8(utf32, pMemoryNullTerm, sizeToAppend);

        m_Size += isOk ? sizeToAppendReserved : 0;

        pMemoryNullTerm += isOk ? sizeToAppendReserved : 0;
        *pMemoryNullTerm = '\0';

        return isOk;
    }

    ALT_FORCE_INLINE bool AppendUtf32NoValidation(const Slice<const u32> utf32) noexcept {

        usize sizeToAppend;
        Utf::Utf32ToUtf8IgnoreErrors(utf32, nullptr, sizeToAppend);

        if (sizeToAppend == 0) {
            return true;
        }

        if (sizeToAppend > GetCapacity() - GetSizeBytes()) {
            return false;
        }

        u8* pMemoryNullTerm = GetMemoryNullTerm();

        const Usize sizeToAppendReserved = static_cast<Usize>(sizeToAppend);
        Utf::Utf32ToUtf8IgnoreErrors(utf32, pMemoryNullTerm, sizeToAppend);

        m_Size += sizeToAppendReserved;

        pMemoryNullTerm += sizeToAppendReserved;
        *pMemoryNullTerm = '\0';

        return true;
    }

    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_,
        std::enable_if_t<(SIZE_IN_BYTES_MINUS_ONE_ <= SIZE_IN_BYTES_MINUS_ONE)>* = nullptr>
    ALT_FORCE_INLINE StaticStringUTF8(Rule5Move_t, StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>& rhs) noexcept
        : m_Size(Exchange(rhs.m_Size, 0))
    {
        std::memcpy(m_Memory, rhs.m_Memory, static_cast<usize>(m_Size + 1) * sizeof(u8));
    }

    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_,
        std::enable_if_t<(SIZE_IN_BYTES_MINUS_ONE_ <= SIZE_IN_BYTES_MINUS_ONE)>* = nullptr>
    ALT_FORCE_INLINE void Assign(Rule5Move_t, StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        m_Size = Exchange(rhs.m_Size, 0);
        std::memcpy(m_Memory, rhs.m_Memory, static_cast<usize>(m_Size + 1) * sizeof(u8));
    }

    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_,
        std::enable_if_t<(SIZE_IN_BYTES_MINUS_ONE_ <= SIZE_IN_BYTES_MINUS_ONE)>* = nullptr>
    ALT_FORCE_INLINE StaticStringUTF8(Rule5Copy_t, StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>& rhs) noexcept
        : m_Size(rhs.m_Size)
    {
        std::memcpy(m_Memory, rhs.m_Memory, static_cast<usize>(m_Size + 1) * sizeof(u8));
    }

    template <usize SIZE_IN_BYTES_MINUS_ONE_ = SIZE_IN_BYTES_MINUS_ONE, typename Usize_,
        std::enable_if_t<(SIZE_IN_BYTES_MINUS_ONE_ <= SIZE_IN_BYTES_MINUS_ONE)>* = nullptr>
    ALT_FORCE_INLINE void Assign(Rule5Copy_t, StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE_, Usize_>& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        m_Size = rhs.m_Size;
        std::memcpy(m_Memory, rhs.m_Memory, static_cast<usize>(m_Size + 1) * sizeof(u8));
    }

    ALT_FORCE_INLINE u8* GetMemoryBegin() noexcept {
        return m_Memory;
    }

    ALT_FORCE_INLINE const u8* GetMemoryBegin() const noexcept {
        return m_Memory;
    }

    ALT_FORCE_INLINE u8* GetMemoryNullTerm() noexcept {
        return m_Memory + m_Size;
    }

    ALT_FORCE_INLINE const u8* GetMemoryNullTerm() const noexcept {
        return m_Memory + m_Size;
    }

private:

    template <usize, typename>
    friend class _StringDetails::StaticStringFormatWriter;

    u8 m_Memory[SIZE_IN_BYTES]{};
    Usize m_Size = 0;
};

/**
 * @brief Returns a constant iterator for the String (chara in UTF-32).
 * @return An `StringIterator`.
 */
template <usize SIZE_IN_BYTES_MINUS_ONE, typename Usize>
ALT_NODISCARD ALT_FORCE_INLINE StringUTF8Iterator StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE, Usize>::GetIterator() const noexcept {
    return StringUTF8Iterator{ *this };
}

template <usize SIZE_IN_BYTES_MINUS_ONE, typename Usize>
class IteratorCollector<StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE, Usize>> {
public:

    using SelfT = StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE, Usize>;

    static bool ExtendMove(SelfT& self, IteratorInterface<chara, IIteratorForwardTrait>& iter) noexcept {
        iferr_scope;

        while (Optional<chara> val = iter.Next()) {

            self.Push(val.GetValue()) iferr_return false;
        }

        return true;
    }
};

#define __ALT_STD_INTERNAL_STATIC_STRING_TEMPLATE__ template <usize SIZE_IN_BYTES_MINUS_ONE, typename Usize>
#define __ALT_STD_INTERNAL_STATIC_STRING_PARSEFLOATBITS__ typename StaticStringUTF8<SIZE_IN_BYTES_MINUS_ONE, Usize>::ParseFloatBits

ALT_ENUM_CLASS_BIT_FLAGS_WITH_TEMPLATE_IMPL(
    __ALT_STD_INTERNAL_STATIC_STRING_TEMPLATE__,
    __ALT_STD_INTERNAL_STATIC_STRING_PARSEFLOATBITS__
);

#undef __ALT_STD_INTERNAL_STATIC_STRING_TEMPLATE__
#undef __ALT_STD_INTERNAL_STATIC_STRING_PARSEFLOATBITS__

} // namespace Alt

#endif // __ALT_STD_STATIC_STRING__