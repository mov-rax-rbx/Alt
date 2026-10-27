#pragma once

#ifndef __ALT_STD_STRING_UTF16__
#define __ALT_STD_STRING_UTF16__

#include "../IfErr.h"
#include "../TypeTraits.h"

#include "../Iterators/Iterator.h"
#include "../Algorithms/Algorithms.h"

#include "../Memory/Memory.h"

#include "../Utils/Num.h"
#include "../Utils/Slice.h"
#include "../Utils/Optional.h"
#include "../Utils/ScopeDefer.h"
#include "../Utils/EnumClassBitFlagsImpl.h"

#include "Utf.h"
#include "Format.h"

#include "Impl/StringDetails.h"

#include "../Algorithms/fast_float-8.2.10/fast_float.h"

namespace Alt {

template <usize MIN_U16_CAPACITY_ON_ALLOCATE = 32>
class StringUTF16;

class StringUTF16Iterator;

namespace TypeTraits {

template <usize MIN_U16_CAPACITY_ON_ALLOCATE>
struct __is_string__<StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE>>
    : std::true_type
{};

template <usize MIN_U16_CAPACITY_ON_ALLOCATE>
struct __is_trivially_reallocatable__<StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE>>
    : is_trivially_reallocatable_t<u16*>
{};

} // namespace TypeTraits

/** \cond */
namespace _StringDetails {

template <usize MIN_U16_CAPACITY_ON_ALLOCATE>
class StringUTF16FormatWriter : public FormatWriter<StringUTF16FormatWriter<MIN_U16_CAPACITY_ON_ALLOCATE>> {
public:

    using String = StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE>;

    ALT_FORCE_INLINE StringUTF16FormatWriter(String& str) noexcept
        : m_pStr(&str)
    {}

    ALT_NODISCARD ALT_FORCE_INLINE bool ReserveBytesAddition(const usize size) noexcept {

        // most of default formatters operates on ascii
        return m_pStr->ReserveU16Addition(size);
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

namespace _PlatformDetails {

class PlatformInstance;

} // namespace _PlatformDetails 
/** \endcond */

/**
 * @brief A dynamically allocated, UTF-16 encoded string.
 * 
 * @details Manages a contiguous buffer of bytes representing a UTF-16 string. 
 * This class supports appending from Unicode encodings (UTF-16, UTF-16, UTF-32) 
 * and provides iterators for character-based traversal.
 * 
 * @example
 * 
 * iferr_scope;
 * 
 * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("String"_s) iferr_return;
 * string.Append(". Hello from Alt") iferr_return;
 */
template <usize MIN_U16_CAPACITY_ON_ALLOCATE>
class StringUTF16 {
private:

    template <typename ...ArgsT>
    constexpr static bool is_populated_v = sizeof...(ArgsT) != 0;

    struct Rule5Move_t {};
    constexpr static Rule5Move_t Rule5Move = Rule5Move_t{};

    /*
     * This need to provide correct API for 'const char16_t*' (assuming that char16_t is UTF-16)
     */
    static_assert(sizeof(char16_t) == sizeof(u16), "");

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

        ALT_NODISCARD ALT_FORCE_INLINE usize GetOffsetInU16() const noexcept {
            return m_Offset;
        }

    private:

        ALT_FORCE_INLINE Checkpoint() noexcept = default;

    private:

        friend class StringUTF16;

        usize m_Offset = 0;
    };

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize GetMinU16CapacityOnAllocate() noexcept {

        return MIN_U16_CAPACITY_ON_ALLOCATE;
    }

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

    ALT_NODISCARD static usize Hash(const Slice<const u16> utf16, const usize seed = 0) noexcept {

        const Slice<const u8> bytes = utf16.CastToBytes();
        return Algorithms::Hash(bytes.Data(), bytes.DataEnd(), seed);
    }

    ALT_NODISCARD static usize Hash(const u16* pUtf16Begin, const usize size, const usize seed = 0) noexcept {

        const u8* pU16Begin = reinterpret_cast<const u8*>(pUtf16Begin);
        const u8* pU16End = reinterpret_cast<const u8*>(pUtf16Begin + size);

        return Algorithms::Hash(pU16Begin, pU16End, seed);
    }

    ALT_NODISCARD static u64 Hash64(const Slice<const u8> utf16, const u64 seed = 0) noexcept {

        const Slice<const u8> bytes = utf16.CastToBytes();
        return Algorithms::Hash64(bytes.Data(), bytes.DataEnd(), seed);
    }

    ALT_NODISCARD static u64 Hash64(const u16* pUtf16Begin, const usize size, const u64 seed = 0) noexcept {

        const u8* pU16Begin = reinterpret_cast<const u8*>(pUtf16Begin);
        const u8* pU16End = reinterpret_cast<const u8*>(pUtf16Begin + size);

        return Algorithms::Hash64(pU16Begin, pU16End, seed);
    }

    ALT_NODISCARD static u32 Hash32(const Slice<const u8> utf16, const u32 seed = 0) noexcept {

        const Slice<const u8> bytes = utf16.CastToBytes();
        return Algorithms::Hash32(bytes.Data(), bytes.DataEnd(), seed);
    }

    ALT_NODISCARD static u32 Hash32(const u16* pUtf16Begin, const usize size, const u32 seed = 0) noexcept {

        const u8* pU16Begin = reinterpret_cast<const u8*>(pUtf16Begin);
        const u8* pU16End = reinterpret_cast<const u8*>(pUtf16Begin + size);

        return Algorithms::Hash64(pU16Begin, pU16End, seed);
    }

    /**
     * @brief Calculates the size in codepoints of a null-terminated string.
     * 
     * @param str The null-terminated source string (supports char, char16_t, char32_t, or wchar_t).
     * @return The total number of characters before the null terminator.
     * @example
     * 
     * assert(Alt::StringUTF16::CalculateSizeInBytes(u"String") == 6);
     */
    template <typename T,
        std::enable_if<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD static usize CalculateCStrLen(const T* str) noexcept {

        return _StringDetails::CalculateCStrLen(str);
    }

    /**
     * @brief Calculates the size in bytes of a null-terminated string.
     * 
     * @param str The null-terminated source string (supports char, char16_t, char32_t, or wchar_t).
     * @return The total number of bytes occupied by the characters before the null terminator.
     * @example
     * 
     * assert(Alt::StringUTF16::CalculateSizeInBytes("String") == 6);
     */
    template <typename T,
        std::enable_if<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD static usize CalculateSizeInBytes(const T* str) noexcept {

        return _StringDetails::CalculateSizeInBytes(str);
    }

    /**
     * @brief Allocates an empty String with the minimum default capacity.
     * @return An Alt::Optional containing the new String, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate() iferr_return;
     * Alt::ASSERT(s.IsEmpty());
     */
    ALT_NODISCARD static Optional<StringUTF16> Allocate() noexcept {
        iferr_scope;

        StringUTF16 result{};
        result.ReserveU16(MIN_U16_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a new String containing the provided string content (char - UTF-16,
     * char16_t, wchar_t (Win32) - UTF-16, char32_t, wchar_t (non Win32) - UTF-32).
     * 
     * @param str The source string to copy.
     * @param size Optional explicit size characters. If Nullopt, it is determined by the null terminator.
     * @return An Alt::Optional containing the new String, or Alt::Nullopt if allocation fails.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StringUTF16 string0 = Alt::StringUTF16::Allocate(u"Hello") iferr_return;
     * Alt::StringUTF16 string1 = Alt::StringUTF16::Allocate(L"Hello") iferr_return;
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_character_v<InnerT>>* = nullptr>
    ALT_NODISCARD static Optional<StringUTF16> Allocate(T&& str_) noexcept {
        iferr_scope;

        const Slice<const InnerT> str = str_;

        StringUTF16 result{};

        if (!TypeTraits::is_compatible_with_character_utf16_v<T> || str.GetSize() < MIN_U16_CAPACITY_ON_ALLOCATE) {

            result.ReserveU16(MIN_U16_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
        }

        result.Append(str) iferr_return Nullopt;

        return result;
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD static Optional<StringUTF16> Allocate(const T* str, Optional<usize> size = Nullopt) noexcept {
        iferr_scope;

        StringUTF16 result{};

        if (!TypeTraits::is_compatible_with_character_utf16_v<T> || size.GetValueOr(0) < MIN_U16_CAPACITY_ON_ALLOCATE) {

            result.ReserveU16(MIN_U16_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
        }

        result.Append(str, size) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a new String from a byte slice with a specific encoding.
     * @param str The raw byte slice to copy.
     * @param encoding The detected or explicitly provided UTF encoding.
     * @return An Alt::Optional containing the new String, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * const auto bytes = Alt::MakeArray<u8>('S', 't', 'r', 'i', 'n', 'g');
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate(bytes, Alt::Encoding::UTF8) iferr_return;
     * 
     * assert(string == "String")
     */
    ALT_NODISCARD static Optional<StringUTF16> Allocate(const Slice<const u8> str, const Utf::Encoding encoding = Utf::Encoding::Unknown) noexcept {
        iferr_scope;

        StringUTF16 result{};

        if (encoding != Utf::Encoding::Utf16 || str.GetSize() < MIN_U16_CAPACITY_ON_ALLOCATE) {

            result.ReserveU16(MIN_U16_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;
        }

        result.Append(str, encoding) iferr_return Nullopt;

        return result;
    }

    ALT_NODISCARD static Optional<StringUTF16> Allocate(const u8* pBegin, const usize size, const Utf::Encoding encoding = Utf::Encoding::Unknown) noexcept {

        const Slice<const u8> bytes{ pBegin, size };
        return Allocate(bytes, encoding);
    }

    /**
     * @brief Allocates a String with a pre-defined capacity.
     * @param sizeInU16 The desired capacity in bytes.
     * @return An Alt::Optional containing the new String, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::AllocateCapacity(4096) iferr_return;
     * assert(string.GetCapacity() >= 4096);
     */
    ALT_NODISCARD static Optional<StringUTF16> AllocateCapacity(const usize sizeInU16) noexcept {
        iferr_scope;

        StringUTF16 result{};
        result.ReserveU16(Num::Max(sizeInU16, MIN_U16_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a String with a exact pre-defined capacity.
     * @param sizeInU16 The desired capacity in bytes.
     * @return An Alt::Optional containing the new String, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::AllocateCapacityExact(44) iferr_return;
     * assert(string.GetCapacity() == 44);
     */
    ALT_NODISCARD static Optional<StringUTF16> AllocateCapacityExact(const usize sizeInU16) noexcept {
        iferr_scope;

        StringUTF16 result{};
        result.ReserveU16Exact(Num::Max(sizeInU16, MIN_U16_CAPACITY_ON_ALLOCATE)) iferr_return Nullopt;

        return result;
    }

    /**
     * @brief Allocates a string by pushing multiple values in one operation.
     * @tparam ArgsT The types of the arguments to push. Must be convertible to `chara`.
     * @param args A variadic list of characters/values to append.
     * @return An Alt::Optional containing the resulting String, or Alt::Nullopt if allocation fails.
     * @example
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::AllocatePush('S', 't', 'r', 'i', 'n', 'g');
     * assert(string == "String");
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...>>* = nullptr>
    ALT_NODISCARD static Optional<StringUTF16> AllocatePush(ArgsT&&... args) noexcept {
        iferr_scope;

        StringUTF16 result{};
        result.ReserveU16(MIN_U16_CAPACITY_ON_ALLOCATE) iferr_return Nullopt;

        PushVariadicImpl(result, std::forward<ArgsT>(args)...);

        return result;
    }

    ALT_FORCE_INLINE StringUTF16() noexcept = default;

    StringUTF16(const StringUTF16&) noexcept = delete;
    StringUTF16& operator=(const StringUTF16&) noexcept = delete;

    /**
     * @brief Move constructor.
     * @param rhs The string to move from.
     */
    template <usize MIN_U16_CAPACITY_ON_ALLOCATE_ = MIN_U16_CAPACITY_ON_ALLOCATE>
    StringUTF16(StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE_>&& rhs) noexcept
        : StringUTF16(Rule5Move, rhs)
    {}

    /**
     * @brief Move assignment.
     * @param rhs The string to move from.
     */
    template <usize MIN_U16_CAPACITY_ON_ALLOCATE_ = MIN_U16_CAPACITY_ON_ALLOCATE>
    StringUTF16& operator=(StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE_>&& rhs) noexcept {

        Assign(Rule5Move, rhs);
        return *this;
    }

    ~StringUTF16() noexcept {

        Destroy();
    }

    /**
     * @brief Returns the number of bytes in UTF-16.
     * @return The nuber of bytes of the String.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetSizeU16() const noexcept {

        return m_pMemoryNullTerm - m_pMemoryBegin;
    }

    /**
     * @brief Returns the total number of bytes the String can hold before a reallocation is required.
     * @return The capacity in bytes of the String.
     */
    ALT_NODISCARD ALT_FORCE_INLINE usize GetCapacity() const noexcept {

        return m_pCapacityEnd - m_pMemoryBegin;
    }

    /**
     * @brief Checks if the String contains no bytes.
     * @return True if empty, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {

        return m_pMemoryNullTerm == m_pMemoryBegin;
    }

    /**
     * @brief Checks if the String contains at least one byte.
     * @return True if populated, false otherwise.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool IsPopulated() const noexcept {

        return m_pMemoryNullTerm != m_pMemoryBegin;
    }

    /**
     * @brief Removes all bytes from the String.
     * @details Does not deallocate the underlying memory, but reset size.
     */
    void Clear() noexcept {

        m_pMemoryNullTerm = m_pMemoryBegin;
        if (m_pMemoryNullTerm != m_pCapacityEnd) *m_pMemoryNullTerm = u'\0';
    }

    /**
     * @brief Accesses the byte at the specified index.
     * @param i The index of the byte.
     * @return A byte.
     * @example
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("String"_s) iferr_return;
     * assert(string[0] == 'S');
     * assert(string[5] == 'g');
     */
    ALT_NODISCARD ALT_FORCE_INLINE u16 operator[](const usize i) const noexcept {
        ALT_ASSERT(i < GetSizeU16());

        return m_pMemoryBegin[i];
    }

    /**
     * @brief Returns a first byte.
     * @return First byte.
     */
    ALT_NODISCARD ALT_FORCE_INLINE u16 GetFirstByte() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *m_pMemoryBegin;
    }

    /**
     * @brief Returns a last byte.
     * @return Last byte.
     */
    ALT_NODISCARD ALT_FORCE_INLINE u16 GetLastByte() const noexcept {
        ALT_ASSERT(!IsEmpty());

        return *(m_pMemoryNullTerm - 1);
    }

    /**
     * @brief Returns a pointer to the beginning.
     * @return Pointer to the first byte.
     */
    ALT_NODISCARD ALT_FORCE_INLINE const u16* Data() const noexcept {
        return m_pMemoryBegin;
    }

    /**
     * @brief Returns a raw pointer to the end of the String.
     * @return Pointer to the next after last byte.
     */
    ALT_NODISCARD ALT_FORCE_INLINE const u16* DataEnd() const noexcept {
        return m_pMemoryNullTerm;
    }

    /**
     * @brief Returns a C-style null-terminated string pointer.
     * @return A `const char16_t*` representation of the buffer.
     */
    ALT_NODISCARD ALT_FORCE_INLINE const char16_t* CStr() const noexcept {

        return m_pMemoryBegin
            ? reinterpret_cast<const char16_t*>(m_pMemoryBegin)
            : u"";
    }

    /**
     * @brief Returns a C-style null-terminated string pointer to the end (effectively on '\0').
     * @return A `const char16_t*` representation of the buffer.
     */
    ALT_NODISCARD ALT_FORCE_INLINE const char16_t* CStrEnd() const noexcept {

        return m_pMemoryNullTerm
            ? reinterpret_cast<const char16_t*>(m_pMemoryNullTerm)
            : u"";
    }

    /**
     * @brief Returns a constant iterator for the String (chara in UTF-32).
     * @return An `StringIterator`.
     */
    ALT_NODISCARD ALT_FORCE_INLINE StringUTF16Iterator GetIterator() const noexcept;

    /**
     * @brief Return a split iterator for the string.
     * @return An `SplitIterator` instance.
     * @example
     * 
     * const Alt::StringUTF16 str = Alt::StringUTF16::Allocate("image/base64,"_s) iferr_return;
     * const auto splits = str
     *     .GetSplitIterator(u"/")
     *     .CollectOrDefault<Alt::Vector<Alt::Slice<const char16_t>>>();
     * 
     * assert(splits[0] == u"image");
     * assert(splits[1] == u"base64,");
     */
    ALT_NODISCARD ALT_FORCE_INLINE SplitIterator<const char16_t> GetSplitIterator(const Slice<const char16_t> pattern) const noexcept {
        return SplitIterator<const char16_t>{ *this, pattern };
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
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("String"_s) iferr_return;
     * Alt::StringUTF16 string1;
     * string1.CloneFrom(string) iferr_return;
     * 
     * assert(string == string1);
     */
    template <usize MIN_U16_CAPACITY_ON_ALLOCATE_ = MIN_U16_CAPACITY_ON_ALLOCATE>
    ALT_NODISCARD bool CloneFrom(const StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE_>& string) noexcept {
        iferr_scope;

        if (static_cast<const void*>(this) == &string) {
            return true;
        }

        const usize stringSize = string.GetSizeU16();
        if (stringSize == 0 && IsEmpty()) {
            return true;
        }

        ALT_ASSERT(!Num::IsOverflowCheckAdd(stringSize, 1));
        ReserveU16(stringSize + 1) iferr_return false;

        std::memcpy(m_pMemoryBegin, string.Data(), stringSize * sizeof(u16));

        m_pMemoryNullTerm = m_pMemoryBegin + stringSize;
        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    template <usize MIN_U16_CAPACITY_ON_ALLOCATE_ = MIN_U16_CAPACITY_ON_ALLOCATE>
    ALT_NODISCARD bool CloneConstructFrom(const StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE_>& rhs) noexcept {
        ALT_ASSERT(static_cast<const void*>(this) != &rhs);

        iferr_scope_on_err {

            Memory::DestroyObject(*this);
        };

        Memory::ConstructDefaultObject<StringUTF16>(this);
        CloneFrom(rhs) iferr_return false;

        return true;
    }

    ALT_NODISCARD Optional<StringUTF16> Clone() const noexcept {
        iferr_scope;

        MaybeUninit<StringUTF16> valueToConstruct;
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
     * const usize val = Alt::StringUTF16::ParseFrom<usize>(L"44") iferr_return;
     * assert(val == 44);
     * 
     * const Alt::Optional<usize> result = Alt::StringUTF16::ParseFrom<usize>(L"44 abc");
     * assert(result.IsNone());
     */
    template <typename T,
        std::enable_if_t<std::is_same_v<T, u8>>* = nullptr>
    ALT_NODISCARD static Optional<u8> ParseFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u8>>* = nullptr>
    ALT_NODISCARD static Optional<u8> ParseFrom(Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

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
        const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u16>>* = nullptr>
    ALT_NODISCARD static Optional<u16> ParseFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u16>>* = nullptr>
    ALT_NODISCARD static Optional<u16> ParseFrom(Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

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
        const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_NODISCARD static Optional<u32> ParseFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_NODISCARD static Optional<u32> ParseFrom(Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

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
        const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD static Optional<u64> ParseFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD static Optional<u64> ParseFrom(Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

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
        const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s8>>* = nullptr>
    ALT_NODISCARD static Optional<s8> ParseFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s8>>* = nullptr>
    ALT_NODISCARD static Optional<s8> ParseFrom(Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u8 val_;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s8 val = Memory::Transmute<u8, s8>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u8 val_;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s8 val = Memory::Transmute<u8, s8>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s8 val;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s16>>* = nullptr>
    ALT_NODISCARD static Optional<s16> ParseFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s16>>* = nullptr>
    ALT_NODISCARD static Optional<s16> ParseFrom(Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u16 val_;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s16 val = Memory::Transmute<u16, s16>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u16 val_;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s16 val = Memory::Transmute<u16, s16>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s16 val;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s32>>* = nullptr>
    ALT_NODISCARD static Optional<s32> ParseFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s32>>* = nullptr>
    ALT_NODISCARD static Optional<s32> ParseFrom(Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u32 val_;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s32 val = Memory::Transmute<u32, s32>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u32 val_;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s32 val = Memory::Transmute<u32, s32>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s32 val;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s64>>* = nullptr>
    ALT_NODISCARD static Optional<s64> ParseFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s64>>* = nullptr>
    ALT_NODISCARD static Optional<s64> ParseFrom(Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u64 val_;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s64 val = Memory::Transmute<u64, s64>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Hex: {

            desc.base = 16;

            u64 val_;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
            const s64 val = Memory::Transmute<u64, s64>(val_);

            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        case ParseBase::Dec: {

            desc.base = 10;

            s64 val;
            const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
            return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
        }
        default: {
            ALT_UNREACHABLE();
        }
        }
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f32>>* = nullptr>
    ALT_NODISCARD static Optional<f32> ParseFrom(const char16_t* str_, const char16_t decPoint = u'.') noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, decPoint);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f32>>* = nullptr>
    ALT_NODISCARD static Optional<f32> ParseFrom(const Slice<const char16_t> str, const char16_t decPoint = u'.', const ParseFloatBits bits = ParseFloatBits::General) noexcept {

        fast_float::parse_options_t<char16_t> desc{};
        desc.decimal_point = decPoint;
        desc.format = static_cast<fast_float::chars_format>(bits);

        f32 val;
        const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
        return result.ec == std::errc{} && result.ptr == str.DataEnd() ? MakeOptional(val) : Nullopt;
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f64>>* = nullptr>
    ALT_NODISCARD static Optional<f64> ParseFrom(const char16_t* str_, const char16_t decPoint = u'.') noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParseFrom<T>(str, decPoint);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f64>>* = nullptr>
    ALT_NODISCARD static Optional<f64> ParseFrom(const Slice<const char16_t> str, const char16_t decPoint = u'.', const ParseFloatBits bits = ParseFloatBits::General) noexcept {

        fast_float::parse_options_t<char16_t> desc{};
        desc.decimal_point = decPoint;
        desc.format = static_cast<fast_float::chars_format>(bits);

        f64 val;
        const fast_float::from_chars_result_t<char16_t> result = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);
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
     * const Alt::StringUTF16::ParsePrefixOk<usize> val0 = Alt::StringUTF16::ParsePrefixFrom<usize>("44 abc") iferr_return;
     * assert(val0.val == 44);
     * assert(val0.size == 2);
     * 
     * const Alt::StringUTF16::ParsePrefixOk<usize> val1 = Alt::StringUTF16::ParsePrefixFrom<usize>("44") iferr_return;
     * assert(val1.val == 44);
     * assert(val1.size == 2);
     */
    template <typename T,
        std::enable_if_t<std::is_same_v<T, u8>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u8>> ParsePrefixFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u8>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u8>> ParsePrefixFrom(const Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

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
        const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
    ALT_NODISCARD static Optional<ParsePrefixOk<u16>> ParsePrefixFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u16>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u16>> ParsePrefixFrom(const Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

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
        const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
    ALT_NODISCARD static Optional<ParsePrefixOk<u32>> ParsePrefixFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u32>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u32>> ParsePrefixFrom(const Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

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
        const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
    ALT_NODISCARD static Optional<ParsePrefixOk<u64>> ParsePrefixFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, u64>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<u64>> ParsePrefixFrom(const Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

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
        const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
    ALT_NODISCARD static Optional<ParsePrefixOk<s8>> ParsePrefixFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s8>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s8>> ParsePrefixFrom(const Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u8 val_;
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
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
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
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
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
    ALT_NODISCARD static Optional<ParsePrefixOk<s16>> ParsePrefixFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s16>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s16>> ParsePrefixFrom(const Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u16 val_;
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
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
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
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
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
    ALT_NODISCARD static Optional<ParsePrefixOk<s32>> ParsePrefixFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s32>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s32>> ParsePrefixFrom(const Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u32 val_;
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
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
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
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
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
    ALT_NODISCARD static Optional<ParsePrefixOk<s64>> ParsePrefixFrom(const char16_t* str_, const ParseBase base = ParseBase::Dec) noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, base);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, s64>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<s64>> ParsePrefixFrom(const Slice<const char16_t> str, const ParseBase base = ParseBase::Dec) noexcept {

        fast_float::parse_options_t<char16_t> desc{};

        switch (base) {
        case ParseBase::Bin: {

            desc.base = 2;

            u64 val_;
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
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
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val_, desc);
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
            const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
    ALT_NODISCARD static Optional<ParsePrefixOk<f32>> ParsePrefixFrom(const char16_t* str_, const char16_t decPoint = u'.') noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, decPoint);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f32>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<f32>> ParsePrefixFrom(const Slice<const char16_t> str, const char16_t decPoint = u'.', const ParseFloatBits bits = ParseFloatBits::General) noexcept {

        fast_float::parse_options_t<char16_t> desc{};
        desc.decimal_point = decPoint;
        desc.format = static_cast<fast_float::chars_format>(bits);

        f32 val;
        const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
    ALT_NODISCARD static Optional<ParsePrefixOk<f64>> ParsePrefixFrom(const char16_t* str_, const char16_t decPoint = u'.') noexcept {

        const Slice<const char16_t> str{ str_, CalculateCStrLen(str_) };
        return ParsePrefixFrom<T>(str, decPoint);
    }

    template <typename T,
        std::enable_if_t<std::is_same_v<T, f64>>* = nullptr>
    ALT_NODISCARD static Optional<ParsePrefixOk<f64>> ParsePrefixFrom(const Slice<const char16_t> str, const char16_t decPoint = u'.', const ParseFloatBits bits = ParseFloatBits::General) noexcept {

        fast_float::parse_options_t<char16_t> desc{};
        desc.decimal_point = decPoint;
        desc.format = static_cast<fast_float::chars_format>(bits);

        f64 val;
        const fast_float::from_chars_result_t<char16_t> parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), val, desc);

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
     * Alt::StringUTF16 str;
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
     * Alt::StringUTF16 str;
     * str.FormatAppend("{{}} {}: {} + {} == {} {{{{}}}}", "Easy Format", 4, 8, 12) iferr_return;
     * 
     * assert(str == "{} Easy Format: 4 + 8 == 12 {{}}");
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

        const usize sizeOld = GetSizeU16();
        iferr_scope_on_err {

            m_pMemoryNullTerm = m_pMemoryBegin + sizeOld;
            if (m_pMemoryNullTerm) *m_pMemoryNullTerm = u'\0';
        };

        const Slice<const char> fmt = fmt_;
        _StringDetails::StringUTF16FormatWriter<MIN_U16_CAPACITY_ON_ALLOCATE> writer{ *this };

        FormatError err;
        _StringDetails::StringUTF16FormatWriter<MIN_U16_CAPACITY_ON_ALLOCATE>::FormatWrite(writer, fmt, args...) iferr_return_with(err) err;

        ReserveU16Addition(1) iferr_return FormatError::WriterFailed;
        *m_pMemoryNullTerm = u'\0';

        return FormatResult::Ok();
    }

    /**
     * @brief Append copy bytes to the current String end.
     * @return An true if copy success, or false if fails.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StringUTF16 string;
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

    template <usize MIN_U16_CAPACITY_ON_ALLOCATE_ = MIN_U16_CAPACITY_ON_ALLOCATE>
    ALT_NODISCARD bool Append(const StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE_>& str) noexcept {

        const Slice<const u8> bytes{ str.m_pMemoryBegin, str.m_pMemoryNullTerm };
        return AppendUtf16NoValidation(bytes);
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
     * Alt::StringUTF16 string;
     * string.Append(bytes, Alt::Utf::Encoding::UTF8) iferr_return;
     * 
     * assert(string == "String");
     */
    ALT_NODISCARD bool Append(Slice<const u8> str, Utf::Encoding encoding = Utf::Encoding::Unknown) noexcept {
        ALT_ASSERT(!Memory::IsOverlap(m_pMemoryBegin, m_pMemoryNullTerm, str.Data(), str.DataEnd()));

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
     * Alt::StringUTF16 string;
     * string.Push('S') iferr_return;
     * 
     * assert(string[0] == 'S');
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const chara utf32) noexcept {
        iferr_scope;

        if (!Utf::_32::IsValidUtf32(utf32)) {

            return false;
        }

        const usize lenInU16 = Utf::_32::LengthUtf32ToUtf16(utf32);
        ReserveU16Addition(lenInU16 + 1) iferr_return false;

        u16 utf16[2];
        Utf::Result result = Utf::_32::Utf32ToUtf16IgnoreErrors(utf32, utf16);

        if (0 < result.consumed) *(m_pMemoryNullTerm + 0) = utf16[0];
        if (1 < result.consumed) *(m_pMemoryNullTerm + 1) = utf16[1];

        m_pMemoryNullTerm += lenInU16;
        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const char utf8) noexcept {
        iferr_scope;

        if (!IsAscii(utf8)) {

            return false;
        }

        ReserveU16Addition(2) iferr_return false;

        *m_pMemoryNullTerm = static_cast<u16>(utf8);

        m_pMemoryNullTerm += 1;
        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const char (&utf8_)[4]) noexcept {
        iferr_scope;

        const u8 utf8[4] = Memory::Transmute<char[4], u8[4]>(utf8_);
        if (!Utf::_8::IsValidUtf8(utf8)) {

            return false;
        }

        const usize lenInU16 = Utf::_8::LengthUtf8ToUtf16(utf8);
        ReserveU16Addition(lenInU16 + 1) iferr_return false;

        u16 utf16[2];
        Utf::Result result = Utf::_8::Utf8ToUtf16IgnoreErrors(utf8, utf16);

        if (0 < lenInU16) *(m_pMemoryNullTerm + 0) = utf16[0];
        if (1 < lenInU16) *(m_pMemoryNullTerm + 1) = utf16[1];

        m_pMemoryNullTerm += lenInU16;
        *m_pMemoryNullTerm = u'\0';

        return true;
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const char8_t utf8) noexcept {

        return Push(static_cast<char>(utf8));
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const char8_t (&utf8_)[4]) noexcept {

        const char utf8[4] = Memory::Transmute<char8_t[4], char[4]>(utf8_);
        return Push(utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const char16_t utf16_) noexcept {
        iferr_scope;

        const u16 utf16[2]{ static_cast<u16>(utf16_), 0 };

        if (!Utf::_16::IsValidUtf16(utf16)) {
            return false;
        }

        ReserveU16Addition(2) iferr_return false;

        *m_pMemoryNullTerm = utf16[0];

        m_pMemoryNullTerm += 1;
        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool Push(const char16_t (&utf16_)[2]) noexcept {
        iferr_scope;

        const u16 utf16[2] = Memory::Transmute<char16_t[2], u16[2]>(utf16_);
        const usize lenInU16 = Utf::_16::ForwardUtf16(utf16);

        if (lenInU16 == 0) {
            return false;
        }

        ReserveU16Addition(lenInU16 + 1) iferr_return false;

        if (0 < lenInU16) *(m_pMemoryNullTerm + 0) = utf16[0];
        if (1 < lenInU16) *(m_pMemoryNullTerm + 1) = utf16[1];

        m_pMemoryNullTerm += lenInU16;
        *m_pMemoryNullTerm = u'\0';

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
     * Alt::StringUTF16 string;
     * string.Push('S', 't', 'r', 'i', 'n', 'g') iferr_return;
     * 
     * assert(string[0] == 'S');
     * assert(string[1] == 't');
     * assert(string.GetSizeU16() == 6);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT, chara>...>>* = nullptr>
    ALT_NODISCARD bool Push(ArgsT&&... charas) noexcept {
        iferr_scope;

        const usize size = Num::AddOverflowCheck(GetSizeU16(), Utf::_32::LengthUtf32ToUtf16(charas)...) iferr_return false;
        ReserveU16(Num::AddWrap(size, 1)) iferr_return false;

        PushVariadicImpl(*this, charas...);

        return true;
    }

    /**
     * @brief Removes and returns the last chara of the String.
     * @return The chara (UTF-32) that was removed.
     * @note This method requires the String to not be empty. Calling on an empty String triggers an assertion.
     * @example
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("String") iferr_return;
     * chara ch = string.Pop();
     * 
     * assert(ch == 'g');
     */
    ALT_FORCE_INLINE chara Pop() noexcept {
        ALT_ASSERT(!IsEmpty());

        const u16 utf16[2]{
            m_pMemoryBegin <= (m_pMemoryNullTerm - 2) ? *(m_pMemoryNullTerm - 2) : static_cast<u16>(0),
            *(m_pMemoryNullTerm - 1),
        };

        u32 utf32;
        Utf::Result result = Utf::_16::Utf16ToUtf32IgnoreErrorsBackward(utf16, utf32);

        m_pMemoryNullTerm -= result.advance;
        ALT_ASSERT(m_pMemoryBegin <= m_pMemoryNullTerm);

        *m_pMemoryNullTerm = u'\0';

        return static_cast<chara>(utf32);
    }

    /**
     * @brief Reserves at least the specified capacity in the String.
     * @param size The capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * Alt::StringUTF16 string;
     * 
     * const usize capacityOld = string.GetCapacity();
     * string.Reserve(1'024);
     * const usize capacity = string.GetCapacity();
     * 
     * assert(capacityOld < capacity);
     */
    bool ReserveU16(const usize sizeInU16) noexcept {

        if (sizeInU16 <= GetCapacity()) {
            return true;
        }

        return ReallocateToNextGrow(sizeInU16);
    }
    
    /**
     * @brief Additionally reserves at least the 'GetSizeU16() + additional size' capacity in the String.
     * @param size The additional capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed or overflow.
     * @example
     * 
     * Alt::StringUTF16 string;
     * 
     * const usize capacityOld = string.GetCapacity();
     * string.ReserveU16Addition(1);
     * const usize capacity = string.GetCapacity();
     * 
     * assert(capacityOld < capacity);
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>>* = nullptr>
    bool ReserveU16Addition(ArgsT&&... sizeInU16Additions) noexcept {
        iferr_scope;

        const usize sizeInU16 = Num::AddOverflowCheck(GetSizeU16(), sizeInU16Additions...) iferr_return false;

        return ReserveU16(sizeInU16);
    }

    /**
     * @brief Reserves the exact capacity in the String.
     * @param size The capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StringUTF16 string;
     * 
     * string.ReserveExact(9);
     * assert(vec.GetCapacity() == 9);
     */
    bool ReserveU16Exact(const usize sizeInU16) noexcept {

        if (sizeInU16 <= GetCapacity()) {
            return true;
        }

        return Reallocate(sizeInU16);
    }

    /**
     * @brief Additionaly reserves the exact 'GetSizeU16() + additional size' capacity in the String.
     * @param size The capacity to reserve.
     * @return True if the capacity was successfully reserved, false if allocation failed or overflow.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StringUTF16 string;
     * 
     * const usize capacityOld = string.GetCapacity();
     * string.ReserveU16ExactAddition(2, 2);
     * assert(capacityOld + 4 == string.GetCapacity());
     */
    template <typename ...ArgsT,
        std::enable_if_t<is_populated_v<ArgsT...> && std::conjunction_v<std::is_convertible<ArgsT&&, usize>...>>* = nullptr>
    bool ReserveU16ExactAddition(ArgsT&&... sizeInU16Additions) noexcept {
        iferr_scope;

        const usize sizeInU16 = Num::AddOverflowCheck(GetSizeU16(), sizeInU16Additions...) iferr_return false;

        return ReserveU16Exact(sizeInU16);
    }

    /**
     * @brief Reduces the capacity of the String to fit the current size.
     * @param capacityNew The new capacity.
     * @return True if successful, false if allocation failed.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StringUTF16 string;
     * 
     * for (usize i = 0; i < 3; ++i) {
     *     string.Append("String") iferr_return;
     * }
     * 
     * const usize capacityOld = string.GetCapacity();
     * string.ShrinkToFit();
     * const usize capacity = string.GetCapacity();
     * 
     * assert(capacityOld > capacity);
     */
    bool ShrinkToFit(const usize capacityNew = MIN_U16_CAPACITY_ON_ALLOCATE) noexcept {
        iferr_scope;

        if (GetCapacity() <= capacityNew) {

            return true;
        }

        const usize size = GetSizeU16();

        ALT_ASSERT(!Num::IsOverflowCheckAdd(size, 1));
        const usize capacity = Num::Max(size + 1, capacityNew);

        u16* pMemory = nullptr;
        if (capacityNew == 0 && capacity == 1) {

            Memory::ChooseAlignedFree<u16>(m_pMemoryBegin);
        }
        else {

            pMemory = Memory::ChooseAlignedRealloc<u16>(m_pMemoryBegin, size + 1, capacity, Memory::ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace) iferr_return false;
        }

        m_pMemoryBegin = pMemory;
        m_pMemoryNullTerm = pMemory + size;
        m_pCapacityEnd = pMemory + capacity;

        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    /**
     * @brief Get actual checkpoint of valid UTF-16 string, see. 'TruncateToCheckpoint(...)'.
     * @example
     * 
     * iferr_scope;
     * 
     * Alt::StringUTF16 path = "CAlt::StringUTF16::Allocate(:\\Temp") iferr_return;
     * Alt::StringUTF16::Checkpoint checkpoint = path.GetCheckpoint();
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
        checkpoint.m_Offset = static_cast<usize>(m_pMemoryNullTerm - m_pMemoryBegin);

        return checkpoint;
    }

    /**
     * @brief Truncate actual buffer to valid checkpoint state.
     */
    ALT_FORCE_INLINE void TruncateToCheckpoint(const Checkpoint checkpoint) noexcept {
        ALT_ASSERT(checkpoint.GetOffsetInU16() <= GetSizeU16());

        m_pMemoryNullTerm = m_pMemoryBegin + checkpoint.m_Offset;
        if (m_pMemoryNullTerm) *m_pMemoryNullTerm = u'\0';
    }

    /**
     * @brief Trims whitespace from the left.
     */
    void TrimLeft() noexcept {

        if (TrimLeftNoNullTermWrite()) {

            *m_pMemoryNullTerm = u'\0';
        }
    }

    /**
     * @brief Trims whitespace from the right.
     */
    void TrimRight() noexcept {

        if (TrimRightNoNullTermWrite()) {

            *m_pMemoryNullTerm = u'\0';
        }
    }

    /**
     * @brief Trims whitespace.
     */
    void Trim() noexcept {

        if (m_pMemoryBegin == m_pMemoryNullTerm) {
            return;
        }

        bool isOk = false;
        isOk |= TrimRightNoNullTermWrite();
        isOk |= TrimLeftNoNullTermWrite();

        if (isOk) {

            *m_pMemoryNullTerm = u'\0';
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
     * const Alt::StringUTF16 str = "String"_s;
     * 
     * const usize idx = str.FindFirst('i') iferr_return;
     * assert(idx == 3); // index in bytes
     */
    ALT_NODISCARD Optional<usize> FindFirst(const chara utf32) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, utf32);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const u8 utf8) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char utf8) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char (&utf8)[4]) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindFirst(const char8_t utf8) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindFirst(const char16_t utf16) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, utf16);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, utf16);
    }

    ALT_NODISCARD Optional<usize> FindFirst(const wchar_t wchar) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, wchar);
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
     * const Alt::StringUTF16 str = "String.alt"_s;
     * 
     * const usize idx = str.FindLast('.') iferr_return;
     * assert(idx == 6); // index in bytes
     */
    ALT_NODISCARD Optional<usize> FindLast(const chara utf32) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, utf32);
    }

    ALT_NODISCARD Optional<usize> FindLast(const u8 utf8) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char utf8) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char (&utf8)[4]) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindLast(const char8_t utf8) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD Optional<usize> FindLast(const char16_t utf16) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, utf16);
    }

    ALT_NODISCARD Optional<usize> FindLast(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, utf16);
    }

    ALT_NODISCARD Optional<usize> FindLast(const wchar_t wchar) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, wchar);
    }

    /**
     * @brief Check if we start with the occurrence in the string.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to comare for.
     * @return An true if start with or false if not.
     * @example
     * 
     * const Alt::StringUTF16 str = "String"_s;
     * 
     * const bool isStartWith = str.StartWith('S');
     * assert(isStartWith);
     */
    ALT_NODISCARD bool StartWith(const chara utf32) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, utf32);
    }

    ALT_NODISCARD bool StartWith(const u8 utf8) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD bool StartWith(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD bool StartWith(const char utf8) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD bool StartWith(const char (&utf8)[4]) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD bool StartWith(const char8_t utf8) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD bool StartWith(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD bool StartWith(const char16_t utf16) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, utf16);
    }

    ALT_NODISCARD bool StartWith(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, utf16);
    }

    ALT_NODISCARD bool StartWith(const wchar_t wchar) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, wchar);
    }

    /**
     * @brief Check if we end with the occurrence in the string.
     * @tparam T A type that is compatible with character data (e.g., char, u8).
     * @param val The value to comare for.
     * @return An true if end with or false if not.
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::StringUTF16 str = "String.alt"_s;
     * 
     * const bool isEndWith = str.EndWith('t');
     * assert(isEndWith);
     */
    ALT_NODISCARD bool EndWith(const chara utf32) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, utf32);
    }

    ALT_NODISCARD bool EndWith(const u8 utf8) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD bool EndWith(const u8 (&utf8)[4]) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD bool EndWith(const char utf8) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD bool EndWith(const char (&utf8)[4]) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

#ifdef __cpp_char8_t

    ALT_NODISCARD bool EndWith(const char8_t utf8) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }

    ALT_NODISCARD bool EndWith(const char8_t (&utf8)[4]) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, utf8);
    }
#endif // __cpp_char8_t

    ALT_NODISCARD bool EndWith(const char16_t utf16) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, utf16);
    }

    ALT_NODISCARD bool EndWith(const char16_t (&utf16)[2]) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, utf16);
    }

    ALT_NODISCARD bool EndWith(const wchar_t wchar) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, wchar);
    }

    /**
     * @brief Finds the first occurrence of a string within the string.
     * @param str A String compatible with UTF-16.
     * @return An Optional index where the substring starts.
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::StringUTF16 str = "String"_s;
     * 
     * const usize idx = str.FindFirst("rin");
     * assert(idx == 2); // index in bytes
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindFirst(T&& str) const noexcept {

        const Slice<const InnerT> bytes = str;
        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, bytes.Data(), bytes.DataEnd());
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindFirst(const T* str) const noexcept {

        return _StringDetails::FindFirst(m_pMemoryBegin, m_pMemoryNullTerm, str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Finds the last occurrence of a string within the string.
     * @param str A String compatible with UTF-16.
     * @return An Optional index where the substring starts.
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::StringUTF16 str = "String.alt"_s;
     * 
     * const usize idx = str.FindLast(".alt");
     * assert(idx == 6); // index in bytes
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindLast(T&& str) const noexcept {

        const Slice<const InnerT> bytes = str;
        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, bytes.Data(), bytes.DataEnd());
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD Optional<usize> FindLast(const T* str) const noexcept {

        return _StringDetails::FindLast(m_pMemoryBegin, m_pMemoryNullTerm, str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Checks if the string starts with the given substring.
     * @tparam str A String compatible with UTF-16.
     * @param str The prefix to check for.
     * @return True if the prefix is found at the beginning, false otherwise.
     * 
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::StringUTF16 str = "String"_s;
     * 
     * const bool isStart = str.StartWith("St");
     * assert(isStart);
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool StartWith(T&& str) const noexcept {

        const Slice<const InnerT> bytes = str;
        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, bytes.Data(), bytes.DataEnd());
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD bool StartWith(const T* str) const noexcept {

        return _StringDetails::StartWith(m_pMemoryBegin, m_pMemoryNullTerm, str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Checks if the string ends with the given substring.
     * @tparam str A String compatible with UTF-16.
     * @param str The suffix to check for.
     * @return True if the suffix is found at the end, false otherwise.
     * @example
     * 
     * iferr_scope;
     * 
     * const Alt::StringUTF16 str = "String"_s;
     * 
     * const bool isEnd = str.EndWith("ng");
     * assert(isEnd);
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool EndWith(T&& str) const noexcept {

        const Slice<const InnerT> bytes = str;
        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, bytes.Data(), bytes.DataEnd());
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD bool EndWith(const T* str) const noexcept {

        return _StringDetails::EndWith(m_pMemoryBegin, m_pMemoryNullTerm, str, str + CalculateCStrLen(str));
    }

    /**
     * @brief Calculates a hash value for the string.
     * @param seed The initial seed for hashing.
     * @return A hash result.
     */
    ALT_NODISCARD usize CalculateHash(const usize seed = 0) const noexcept {

        const Slice<const u16> uf16 = *this;
        return Hash(uf16, seed);
    }

    /**
     * @brief Compares strings.
     * @param str The null-terminated string to compare against.
     * @return -1 if this < str, 0 if this == str, and +1 if this > str.
     * @example
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("String") iferr_return;
     * assert(string.Compare("String") == 0);
     * assert(string.Compare("Stringa") < 0);
     * assert(string.Compare("Strin") > 0);
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD ssize Compare(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;

        return Algorithms::Compare(
            m_pMemoryBegin,
            m_pMemoryNullTerm,
            reinterpret_cast<const u16*>(rhs.Data()),
            reinterpret_cast<const u16*>(rhs.DataEnd())
        );
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD ssize Compare(const T* rhs) const noexcept {

#ifdef ALT_OS_WINDOWS

        static_assert(sizeof(wchar_t) == sizeof(char16_t), "");

        const s32 result = std::wcscmp(reinterpret_cast<const wchar_t*>(CStr()), reinterpret_cast<const wchar_t*>(rhs));

        if (result == 0) {
            return 0;
        }

        return result < 0 ? -1 : 1;
#else

        return Algorithms::Compare(
            m_pMemoryBegin,
            m_pMemoryNullTerm,
            reinterpret_cast<const u16*>(rhs),
            reinterpret_cast<const u16*>(rhs + CalculateCStrLen(rhs))
        );
#endif // ALT_OS_WINDOWS
    }

    /**
     * @brief Operators for comparison with String.
     * @example
     *
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("String") iferr_return;
     * assert(string == "String"_s);
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator==(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return Compare(rhs) == 0;
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator!=(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return !(*this == rhs);
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator<(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return Compare(rhs) < 0;
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator<=(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return !(*this > rhs);
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator>(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return Compare(rhs) > 0;
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_compatible_with_character_utf16_v<InnerT>>* = nullptr>
    ALT_NODISCARD bool operator>=(T&& rhs_) const noexcept {

        const Slice<const InnerT> rhs = rhs_;
        return !(*this < rhs);
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD bool operator==(const T* rhs) const noexcept {
        return Compare(rhs) == 0;
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD bool operator!=(const T* rhs) const noexcept {
        return !(*this == rhs);
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD bool operator<(const T* rhs) const noexcept {
        return Compare(rhs) < 0;
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD bool operator<=(const T* rhs) const noexcept {
        return !(*this > rhs);
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD bool operator>(const T* rhs) const noexcept {
        return Compare(rhs) > 0;
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_utf16_v<T>>* = nullptr>
    ALT_NODISCARD bool operator>=(const T* rhs) const noexcept {
        return !(*this < rhs);
    }

private:

    ALT_FORCE_INLINE static usize GetNextSize(const usize size) noexcept {

        return Num::RoundUpToPowerOf2(size);
    }

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE static void PushVariadicImpl(StringUTF16& self, HeadT&& head, TailT&&... tail) noexcept {

        ALT_MAYBE_UNUSED const bool isOk = self.Push(std::forward<HeadT>(head));

        // must be checked by previouse call - never fail
        ALT_ASSERT(isOk);

        PushVariadicImpl(self, std::forward<TailT>(tail)...);
    }

    ALT_FORCE_INLINE static void PushVariadicImpl(StringUTF16&) noexcept {}

    ALT_FORCE_INLINE bool ReallocateToNextGrow(const usize size) noexcept {

        const usize sizeNext = GetNextSize(size);
        return Reallocate(sizeNext, Num::Max(size, Num::Desc<usize>::Max() / sizeof(u16)));
    }

    ALT_FORCE_INLINE bool Reallocate(const usize size, const usize wrapSize = Num::Desc<usize>::Max() / sizeof(u16)) noexcept {
        iferr_scope;

        ALT_ASSERT(size > GetCapacity());

        const usize sizeOld = GetSizeU16();

        ALT_ASSERT(!Num::IsOverflowCheckAdd(sizeOld, 1));
        const Memory::ReallocBits reallocBits = sizeOld + 1 == GetCapacity()
            ? Memory::ReallocBits::AllowWholeMemcpyOnReallocateAndMightIgnoreInPlace
            : Memory::ReallocBits::None;

        const Memory::MallocWrapResult<u16> result = Memory::ChooseAlignedReallocWrap<u16>(m_pMemoryBegin, sizeOld, size, wrapSize, reallocBits)
            iferr_return false;

        m_pMemoryBegin = result.pMemory;
        m_pMemoryNullTerm = result.pMemory + sizeOld;
        m_pCapacityEnd = result.pMemory + result.size;

        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    template <typename T>
    ALT_FORCE_INLINE bool ToStringAppendImpl(const T val) noexcept {

        const usize sizeOld = GetSizeU16();
        iferr_scope_on_err {

            m_pMemoryNullTerm = m_pMemoryBegin + sizeOld;
            if (m_pMemoryNullTerm) *m_pMemoryNullTerm = u'\0';
        };

        _StringDetails::StringUTF16FormatWriter<MIN_U16_CAPACITY_ON_ALLOCATE> writer{ *this };
        Alt::Format<T>::WriteAppend(writer, ""_s, val) iferr_return false;

        ReserveU16Addition(1) iferr_return false;
        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    ALT_FORCE_INLINE bool TrimLeftNoNullTermWrite() noexcept {

        const u16* pBegin = _StringDetails::TrimLeftImpl(m_pMemoryBegin, m_pMemoryNullTerm);

        if (pBegin == m_pMemoryBegin) {
            return false;
        }

        const usize sizeNew = static_cast<usize>(m_pMemoryNullTerm - pBegin);
        std::memmove(m_pMemoryBegin, pBegin, sizeNew * sizeof(u16));

        m_pMemoryNullTerm = m_pMemoryBegin + sizeNew;

        return true;
    }

    ALT_FORCE_INLINE bool TrimRightNoNullTermWrite() noexcept {

        if (m_pMemoryBegin == m_pMemoryNullTerm) {
            return false;
        }

        const u16* pEnd = _StringDetails::TrimRightImpl(m_pMemoryBegin, m_pMemoryNullTerm);

        const bool isShouldWriteNullTerm = pEnd != m_pMemoryNullTerm;
        m_pMemoryNullTerm = const_cast<u16*>(pEnd);

        return isShouldWriteNullTerm;
    }

    ALT_FORCE_INLINE bool AppendUtf8(const Slice<const u8> utf8) noexcept {
        iferr_scope;

        usize sizeToAppend;
        bool isOk = Utf::Utf8ToUtf16(utf8, nullptr, sizeToAppend);

        if (sizeToAppend == 0) {
            return true;
        }

        ReserveU16Addition(sizeToAppend, 1) iferr_return false;

        const usize sizeToAppendReserved = sizeToAppend;
        isOk = Utf::Utf8ToUtf16(utf8, m_pMemoryNullTerm, sizeToAppend);

        m_pMemoryNullTerm += isOk ? sizeToAppendReserved : 0;
        *m_pMemoryNullTerm = u'\0';

        return isOk;
    }

    ALT_FORCE_INLINE bool AppendUtf8NoValidation(const Slice<const u8> utf8) noexcept {
        iferr_scope;

        usize sizeToAppend;
        Utf::Utf8ToUtf16IgnoreErrors(utf8, nullptr, sizeToAppend);

        if (sizeToAppend == 0) {
            return true;
        }

        ReserveU16Addition(sizeToAppend, 1) iferr_return false;

        const usize sizeToAppendReserved = sizeToAppend;
        Utf::Utf8ToUtf16IgnoreErrors(utf8, m_pMemoryNullTerm, sizeToAppend);

        m_pMemoryNullTerm += sizeToAppendReserved;
        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    ALT_FORCE_INLINE bool AppendUtf16(const Slice<const u16> utf16) noexcept {

        if (Utf::ValidateUtf16(utf16) != utf16.GetSize()) {

            return false;
        }

        return AppendUtf16NoValidation(utf16);
    }

    ALT_FORCE_INLINE bool AppendUtf16NoValidation(const Slice<const u16> utf16) noexcept {
        iferr_scope;

        const usize sizeToAppend = utf16.GetSize();
        if (sizeToAppend == 0) {
            return true;
        }

        ReserveU16Addition(sizeToAppend, 1) iferr_return false;

        std::memcpy(m_pMemoryNullTerm, utf16.Data(), sizeToAppend * sizeof(u16));

        m_pMemoryNullTerm += sizeToAppend;
        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    ALT_FORCE_INLINE bool AppendUtf32(const Slice<const u32> utf32) noexcept {
        iferr_scope;

        usize sizeToAppend;
        bool isOk = Utf::Utf32ToUtf16(utf32, nullptr, sizeToAppend);

        if (sizeToAppend == 0) {
            return true;
        }

        ReserveU16Addition(sizeToAppend, 1) iferr_return false;

        const usize sizeToAppendReserved = sizeToAppend;
        isOk = Utf::Utf32ToUtf16(utf32, m_pMemoryNullTerm, sizeToAppend);

        m_pMemoryNullTerm += isOk ? sizeToAppendReserved : 0;
        *m_pMemoryNullTerm = u'\0';

        return isOk;
    }

    ALT_FORCE_INLINE bool AppendUtf32NoValidation(const Slice<const u32> utf32) noexcept {
        iferr_scope;

        usize sizeToAppend;
        Utf::Utf32ToUtf16IgnoreErrors(utf32, nullptr, sizeToAppend);

        if (sizeToAppend == 0) {
            return true;
        }

        ReserveU16Addition(sizeToAppend, 1) iferr_return false;

        const usize sizeToAppendReserved = sizeToAppend;
        Utf::Utf32ToUtf16IgnoreErrors(utf32, m_pMemoryNullTerm, sizeToAppend);

        m_pMemoryNullTerm += sizeToAppendReserved;
        *m_pMemoryNullTerm = u'\0';

        return true;
    }

    template <usize MIN_U16_CAPACITY_ON_ALLOCATE_ = MIN_U16_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE StringUTF16(Rule5Move_t, StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE_>& rhs) noexcept
        : m_pMemoryBegin(Exchange(rhs.m_pMemoryBegin, nullptr))
        , m_pMemoryNullTerm(Exchange(rhs.m_pMemoryNullTerm, nullptr))
        , m_pCapacityEnd(Exchange(rhs.m_pCapacityEnd, nullptr))
    {}

    template <usize MIN_U16_CAPACITY_ON_ALLOCATE_ = MIN_U16_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE void Assign(Rule5Move_t, StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE_>& rhs) noexcept {

        if (this == &rhs) {
            return;
        }

        if (m_pMemoryBegin) {

            Memory::ChooseAlignedFree<u16>(m_pMemoryBegin);
        }

        m_pMemoryBegin = Exchange(rhs.m_pMemoryBegin, nullptr);
        m_pMemoryNullTerm = Exchange(rhs.m_pMemoryNullTerm, nullptr);
        m_pCapacityEnd = Exchange(rhs.m_pCapacityEnd, nullptr);
    }

    ALT_FORCE_INLINE void Destroy() noexcept {

        if (!m_pMemoryBegin) {
            return;
        }

        Memory::ChooseAlignedFree<u16>(m_pMemoryBegin);

        m_pMemoryBegin = nullptr;
        m_pMemoryNullTerm = nullptr;
        m_pCapacityEnd = nullptr;
    }

private:

    template <usize>
    friend class _StringDetails::StringUTF16FormatWriter;

    u16* m_pMemoryBegin = nullptr;

    u16* m_pMemoryNullTerm = nullptr;
    u16* m_pCapacityEnd = nullptr;
};

/**
 * @brief An iterator that allows traversing the String.
 * @details This iterator maintains its own copy of the String's boundaries. 
 * Advancing the iterator modifies its internal view, not the original String object.
 * @tparam T The numeric type of the String chara (UTF-32).
 */
class StringUTF16Iterator final : public Iterator<StringUTF16Iterator, chara, IIteratorForwardTrait, IIteratorBackwardTrait> {
public:

    /**
     * @brief Constructs an iterator from a specific String.
     * @param self The String object to copy boundaries from.
     */
    template <usize MIN_U16_CAPACITY_ON_ALLOCATE>
    ALT_FORCE_INLINE StringUTF16Iterator(const StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE>& self) noexcept
        : m_pBegin(self.Data())
        , m_pEnd(self.DataEnd())
    {}

    /**
     * @brief Retrieves a hint regarding the number of elements remaining.
     * @return 0 - mean no hint.
     */
    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return m_pEnd - m_pBegin;
    }

    /**
     * @brief Retrieves the next chara (UTF-32) and advances the start boundary.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is exhausted.
     * @example
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("St") iferr_return;
     * Alt::StringUTF16Iterator it = string.GetIterator();
     * 
     * assert(*it.Next() == U'S');
     * assert(*it.Next() == U't');
     * assert(it.Next() == Alt::Nullopt);
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
     * @brief Skips a specified number of charas (UTF-32) forward in the current view.
     * @param advance_ The number of charas (UTF-32) to skip.
     * @return The actual number of charas (UTF-32) successfully skipped.
     * @example
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("String") iferr_return;
     * Alt::StringUTF16Iterator it = string.GetIterator();
     * 
     * const usize skipped = it.Advance(3);
     * 
     * assert(skipped == 3);
     * assert(it.Count() == 3);
     */
    ALT_FORCE_INLINE usize Advance(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u16 utf16[2]{
                *m_pBegin,
                m_pBegin + 1 < m_pEnd ? *(m_pBegin + 1) : static_cast<u16>(0),
            };

            const usize advanceU16 = Utf::_16::ForwardUtf16(utf16);

            m_pBegin += advanceU16;
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Retrieves the lasts charas (UTF-32) and advances the end boundary backward.
     * @return An Alt::Optional<T> containing the value, or Alt::Nullopt if the range is empty.
     * @example
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("ng") iferr_return;
     * Alt::StringUTF16Iterator it = string.GetIterator();
     * 
     * assert(*it.NextBack() == 'g');
     * assert(*it.NextBack() == 'n');
     * assert(it.NextBack() == Alt::Nullopt);
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
     * @brief Skips a specified number of charasa (UTF-32) backward in the current view.
     * @param advance_ The number of charasa (UTF-32) to skip.
     * @return The actual number of charasa (UTF-32) successfully skipped.
     * @example
     * 
     * Alt::StringUTF16 string = Alt::StringUTF16::Allocate("String") iferr_return iferr_return;
     * Alt::StringUTF16Iterator it = string.GetIterator();
     * 
     * const usize skipped = it.AdvanceBack(1);
     * 
     * assert(skipped == 1);
     * assert(it.Count() == 5);
     */
    ALT_FORCE_INLINE usize AdvanceBack(const usize advance_) noexcept {

        usize advance = advance_;
        while (advance != 0 && m_pBegin != m_pEnd) {

            const u16 utf16[2]{
                m_pBegin <= (m_pEnd - 2) ? *(m_pEnd - 2) : static_cast<u16>(0),
                *(m_pEnd - 1),
            };

            const usize advanceU16 = Utf::_16::BackwardUtf16(utf16);

            m_pEnd -= advanceU16;
            ALT_ASSERT(m_pBegin <= m_pEnd);

            advance -= 1;
        }

        return advance_ - advance;
    }

    /**
     * @brief Checks if the iterator's is empty.
     * @return True if the iterator has no charas (UTF-32) left, false otherwise.
     */
    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_pEnd == m_pBegin;
    }

private:

    const u16* m_pBegin;
    const u16* m_pEnd;
};

/**
 * @brief Returns a constant iterator for the String (chara in UTF-32).
 * @return An `StringIterator`.
 */
template <usize MIN_U16_CAPACITY_ON_ALLOCATE>
ALT_NODISCARD ALT_FORCE_INLINE StringUTF16Iterator StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE>::GetIterator() const noexcept {
    return StringUTF16Iterator{ *this };
}

template <usize MIN_U16_CAPACITY_ON_ALLOCATE>
class IteratorCollector<StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE>> {
public:

    using SelfT = StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE>;

    static bool ExtendMove(SelfT& self, IteratorInterface<chara, IIteratorForwardTrait>& iter) noexcept {
        iferr_scope;

        while (Optional<chara> val = iter.Next()) {

            self.Push(val.GetValue()) iferr_return false;
        }

        return true;
    }
};

ALT_ENUM_CLASS_BIT_FLAGS_WITH_TEMPLATE_IMPL(
    template <usize MIN_U16_CAPACITY_ON_ALLOCATE>,
    typename StringUTF16<MIN_U16_CAPACITY_ON_ALLOCATE>::ParseFloatBits
);

} // namespace Alt

#endif // __ALT_STD_STRING_UTF16__