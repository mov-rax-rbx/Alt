#pragma once

#ifndef __ALT_STD_FORMAT__
#define __ALT_STD_FORMAT__

#include "../Utils/Result.h"

#include "Impl/StringDetails.h"
#include "Impl/FormatPreInclude.h"

#if defined(ALT_COMPILER_MSVC)
#pragma warning(push, 0)
#elif defined(ALT_COMPILER_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-function"
#endif

#include "../Algorithms/zmij-1.2/zmij.h"
#include "../Algorithms/zmij-1.2/zmij_cc.h"

#if defined(ALT_COMPILER_MSVC)
#pragma warning(pop)
#elif defined(ALT_COMPILER_CLANG)
#pragma clang diagnostic pop
#endif

#include "../Algorithms/fast_float-8.2.10/fast_float.h"

/*
 * @note To add custom format support:
 * 1. Implement `WriteAppend(...)` inside a specialization of `Alt::Format<YourType>`.
 * 2. Optionally implement `EstimateExactSize(...)` to allow the formatter to pre-allocate buffer memory.
 * 
 * @example
 *
 * namespace {
 *
 *    struct CustomStruct {
 *        f64 val = 0.0;
 *    };
 * }
 *
 * namespace Alt {
 *
 *    template <>
 *    class Format<CustomStruct> {
 *    public:
 *
 *        // Optional method - if this method provided that means we can preallocaate (used as optimization) memory
 *        // before main format routine, by default from user perspective you probaaly do not want to implement this
 *        // method, but as for library or some core data structures probably to implement.
 *        ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
 *            FormatEstimator& estimator, const Slice<const char> parameters, const CustomStruct& val
 *        ) noexcept {
 *            iferr_scope;
 *
 *            if (parameters.IsPopulated()) {
 *                return Nullopt;
 *            }
 *
 *            EstimateExactSizeResult result{};
 *            result.size = 14;
 *
 *            const EstimateExactSizeResult estimated = Alt::Format<f64>::EstimateExactSize(estimator, val.val) iferr_return Nullopt;
 *
 *            result.size += estimated.size;
 *            result.size += 2;
 *
 *            return result;
 *        }
 *
 *        // The main method which inserts in string buffer. To be compatible with alt format this method must be implemented.
 *        // 'parameters' - optional string that does not contain '}'.
 *        //     {some_parameter_name} - ok
 *        //     { some parameter name } - ok
 *        //     { some {parameter name } - ok
 *        //     {some parameter} name } - incorrect (only '{some parameter}' will be in 'parameters')
 *        template <typename FormatWriterT>
 *        ALT_FORCE_INLINE static bool WriteAppend(
 *            FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const CustomStruct& val
 *        ) noexcept {
 *            iferr_scope;
 *
 *            if (parameters.IsPopulated()) {
 *                return false;
 *            }
 *
 *            writer.Append("CustomStruct{ ") iferr_return false;
 *            Alt::Format<f64>::WriteAppend(writer, ""_s, val.val) iferr_return false;
 *            writer.Append(" }") iferr_return false;
 *
 *            return true;
 *        }
 *    };
 *
 * } // namespace Alt
 *
 * Some Format implementation uses 'inline' and 'noinline' parameter words - which indicated whetewer inline "in one line"
 * all values (without internal indentation) or to use "pretty" like formatting with indentation. Indentation must only
 * be applied only inside of 'EstimateExactSize(...)' or 'WriteAppend(...)' on each new line. The parent call of
 * 'WriteAppend(...)' must always to use identation on new line.
 * 
 * For some types allowed nested formating, nested formating must be separated by ';' - for example Arrays, Optional, ...
 * Example of nested parameters:
 * 
 * const Alt::Array<Alt::Array<Alt::String, 2>, 2> array{
 *     Alt::Array<Alt::String, 2>{ "String0"_salloc, "String1"_salloc },
 *     Alt::Array<Alt::String, 2>{ "String0"_salloc, "String1"_salloc },
 * };
 * 
 * ALT_PRINTLN("{inline;noinline}", array);
 * [[
 *     String0,
 *     String1
 * ], [
 *     String0,
 *     String1
 * ]]
 * 
 * ALT_PRINTLN("{noinline;inline}", array);
 * [
 *     [String0, String1],
 *     [String0, String1]
 * ]
 * 
 * ALT_PRINTLN("{noinline;noinline}", array);
 * [
 *     [
 *         String0,
 *         String1
 *     ],
 *     [
 *         String0,
 *         String1
 *     ]
 * ]
 * 
 * You need to use 'GetIndentEstimatedSize(...)' and 'AppendIndent(...)' only when uses newline e.g. '\n' inside
 * 'EstimateExactSize(...)' or 'WriteAppend(...)'.
 * 
 * Escaping: Each "{}" matches an argument. If there are too many arguments, an error will be returned ('FormatError::ManyArguments').
 * If there are too many '{}' characters, the format parser will match them with the actual arguments, and all other format characters
 * will be printed as text. To escape a sequence of '{}' characters, just add a pair of '{' characters (e.g. '{{}}' -> '{}' or '{{{{}}' -> '{{}}').
 * Think like the parser: if the parser finds a '{', then it should be a '}' pair. So, if the parser sees a pair of '{{', it means that
 * it has not should find a '}' match, so just escape it.
 */
namespace Alt {

/**
 * @brief Enumeration of possible errors occurring during formatting operations.
 */
enum class FormatError {

    Overflow, ///< Buffer or numeric overflow occurred.
    ManyArguments, ///< Too many arguments provided for the format string.
    InvalidFormat, ///< The provided format string is malformed.

    WriterFailed, ///< Failure somewhere in 'FormatWriter'.
    FormatFailed, ///< Failure in `Format<SomeType>::WriteAppend(...)`.
};

using FormatResult = Result<void, FormatError>;

/**\cond */
namespace _FormatDetails {

template <typename FormatWriterT>
struct FormatWriteImpl;

constexpr static char _2DecDigitToStr[201]{
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899"
};

template <typename T, typename FormatWriterT,
    std::enable_if_t<std::is_scalar_v<T> && std::is_unsigned_v<T>>* = nullptr>
ALT_FORCE_INLINE bool ToStringAppendDecImpl(FormatWriter<FormatWriterT>& writer, T val) noexcept {

    constexpr static usize MAX_DIGIT_COUNT = Num::Desc<T>::DigitCount() + (Num::Desc<T>::DigitCount() % 2 != 0 ? 1 : 0);
    static_assert(MAX_DIGIT_COUNT % 2 == 0, "");

    char buf[MAX_DIGIT_COUNT];
    char* pBuf = buf + MAX_DIGIT_COUNT - 2;

    while (val >= 100) {

        const T _2Digits = val % 100;
        val /= 100;

        std::memcpy(pBuf, &_2DecDigitToStr[_2Digits * 2], 2);
        pBuf -= 2;
    }

    std::memcpy(pBuf, &_2DecDigitToStr[val * 2], 2);
    pBuf += val > 9 ? 0 : 1;

    const Slice<const char> str{ pBuf, buf + MAX_DIGIT_COUNT };
    return writer.Append(str);
}

template <typename T, typename FormatWriterT,
    std::enable_if_t<std::is_scalar_v<T> && std::is_signed_v<T> && !std::is_floating_point_v<T>>* = nullptr>
ALT_FORCE_INLINE bool ToStringAppendDecImpl(FormatWriter<FormatWriterT>& writer, const T val_) noexcept {

    using U = typename Num::Desc<T>::Unsigned;

    const bool isPlus = Num::IsSignPositive(val_);
    U val = static_cast<U>(Num::Abs(val_));

    constexpr static usize MAX_DIGIT_COUNT = Num::Desc<T>::DigitCount() + 1 + ((Num::Desc<T>::DigitCount() + 1) % 2 != 0 ? 1 : 0);
    static_assert(MAX_DIGIT_COUNT % 2 == 0, "");

    char buf[MAX_DIGIT_COUNT];
    char* pBuf = buf + MAX_DIGIT_COUNT - 2;

    while (val >= 100) {

        const T _2Digits = val % 100;
        val /= 100;

        std::memcpy(pBuf, &_2DecDigitToStr[_2Digits * 2], 2);
        pBuf -= 2;
    }

    std::memcpy(pBuf, &_2DecDigitToStr[val * 2], 2);
    pBuf += val > 9 ? 0 : 1;

    pBuf -= isPlus ? 0 : 1;
    if (!isPlus) *pBuf = '-';

    const Slice<const char> str{ pBuf, buf + MAX_DIGIT_COUNT };
    return writer.Append(str);
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T>>* = nullptr>
ALT_FORCE_INLINE usize ToStringAppendBinImpl_CalculateBitsSize(const T val_, const bool isWithSeparator) noexcept {

    using U = typename Num::Desc<T>::Unsigned;
    U val = Memory::Transmute<T, U>(val_);

    const usize numBits = Num::CeilDiv(Num::Max(Num::Desc<U>::Bits() - Num::Count0BitsLeft(val), 1), 8) * 8;
    const usize numBitSeparators = (numBits / 8) - 1;

    return numBits + (isWithSeparator ? numBitSeparators : 0);
}

template <typename T, typename FormatWriterT,
    std::enable_if_t<std::is_scalar_v<T>>* = nullptr>
ALT_FORCE_INLINE bool ToStringAppendBinImpl(FormatWriter<FormatWriterT>& writer, const T val_, const bool isWithSeparator) noexcept {

    using U = typename Num::Desc<T>::Unsigned;
    U val = Memory::Transmute<T, U>(val_);

    // ...00000001'10101010'00001111
    constexpr static usize MAX_BIN_COUNT = Num::Desc<U>::Bits();
    constexpr static usize MAX_BIN_SEPARATOR_COUNT = MAX_BIN_COUNT / 8;

    constexpr usize MAX_SIZE = MAX_BIN_COUNT + MAX_BIN_SEPARATOR_COUNT;

    char buf[MAX_SIZE];
    char* pBuf = buf + MAX_SIZE - 1;

    do {

        for (usize i = 0; i < 8; ++i) {

            *pBuf = '0' + ((val & 1) == 1 ? 1 : 0);
            pBuf -= 1;

            val >>= 1;
        }

        if (isWithSeparator) {

            *pBuf = '\'';
            pBuf -= val == 0 ? 0 : 1;
        }

    } while (val != 0);

    pBuf += 1;

    const Slice<const char> str{ pBuf, buf + MAX_SIZE };
    return writer.Append(str);
}

constexpr static char _2HexDigitToStr[256 * 2 + 1]{
    "00"
    "0102030405060708090a0b0c0d0e0f"
    "101112131415161718191a1b1c1d1e"
    "1f202122232425262728292a2b2c2d"
    "2e2f303132333435363738393a3b3c"
    "3d3e3f404142434445464748494a4b"
    "4c4d4e4f505152535455565758595a"
    "5b5c5d5e5f60616263646566676869"
    "6a6b6c6d6e6f707172737475767778"
    "797a7b7c7d7e7f8081828384858687"
    "88898a8b8c8d8e8f90919293949596"
    "9798999a9b9c9d9e9fa0a1a2a3a4a5"
    "a6a7a8a9aaabacadaeafb0b1b2b3b4"
    "b5b6b7b8b9babbbcbdbebfc0c1c2c3"
    "c4c5c6c7c8c9cacbcccdcecfd0d1d2"
    "d3d4d5d6d7d8d9dadbdcdddedfe0e1"
    "e2e3e4e5e6e7e8e9eaebecedeeeff0"
    "f1f2f3f4f5f6f7f8f9fafbfcfdfeff"
};

template <typename T,
    std::enable_if_t<std::is_scalar_v<T>>* = nullptr>
ALT_FORCE_INLINE usize ToStringAppendHexImpl_CalculateBitsSize(const T val_) noexcept {

    using U = typename Num::Desc<T>::Unsigned;
    U val = Memory::Transmute<T, U>(val_);

    return static_cast<usize>((Num::Log2((val | 1) - 1) >> 2) + 1);
}

ALT_FORCE_INLINE constexpr usize ToStringAppendPtrHexImpl_CalculateBitsSize() noexcept {

    return static_cast<usize>((Num::Log2Constexpr(Num::Desc<uptr>::Max()) >> 2) + 1);
}

template <typename T, typename FormatWriterT,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) != 1)>* = nullptr>
ALT_FORCE_INLINE bool ToStringAppendHexImpl(FormatWriter<FormatWriterT>& writer, const T val_) noexcept {

    using U = typename Num::Desc<T>::Unsigned;
    U val = Memory::Transmute<T, U>(val_);

    // ff
    constexpr static usize MAX_SIZE = static_cast<usize>((Num::Log2Constexpr((Num::Desc<umax>::Max() | 1) - 1) >> 2) + 1);
    static_assert(MAX_SIZE % 2 == 0, "");

    char buf[MAX_SIZE];
    char* pBuf = buf + MAX_SIZE - 2;

    while (val >= 256) {

        const usize hex = static_cast<usize>(val & 255);
        val /= static_cast<U>(256);

        std::memcpy(pBuf, &_2HexDigitToStr[hex * 2], 2);
        pBuf -= 2;
    }

    std::memcpy(pBuf, &_2HexDigitToStr[val * 2], 2);
    pBuf += val > 15 ? 0: 1;

    const Slice<const char> str{ pBuf, buf + MAX_SIZE };
    return writer.Append(str);
}

template <typename T, typename FormatWriterT,
    std::enable_if_t<std::is_scalar_v<T> && (sizeof(T) == 1)>* = nullptr>
ALT_FORCE_INLINE bool ToStringAppendHexImpl(FormatWriter<FormatWriterT>& writer, const T val_) noexcept {

    using U = typename Num::Desc<T>::Unsigned;
    u16 val = static_cast<u16>(Memory::Transmute<T, U>(val_));

    return ToStringAppendHexImpl(writer, val);
}

template <typename FormatWriterT>
ALT_FORCE_INLINE bool ToStringAppendPtrHexImpl(FormatWriter<FormatWriterT>& writer, uptr val) noexcept {

    // 00000000000000ff
    constexpr static usize MAX_SIZE = ToStringAppendPtrHexImpl_CalculateBitsSize();
    static_assert(MAX_SIZE % 2 == 0, "");

    char buf[MAX_SIZE];
    char* pBuf = buf + MAX_SIZE - 2;

    while (val >= 256) {

        const usize hex = static_cast<usize>(val & 255);
        val /= static_cast<uptr>(256);

        std::memcpy(pBuf, &_2HexDigitToStr[hex * 2], 2);
        pBuf -= 2;
    }

    std::memcpy(pBuf, &_2HexDigitToStr[val * 2], 2);

    while (pBuf != buf) {

        pBuf -= 2;
        std::memcpy(pBuf, _2HexDigitToStr, 2);
    }

    const Slice<const char> str{ buf, buf + MAX_SIZE };
    return writer.Append(str);
}

} // namespace _FormatDetails
/**\endcond */

/**
 * @brief A interaface like for Format Writers, providing unified methods for different types.
 * 
 * @tparam SelfT The internal type of the underlying buffer management.
 */
template <typename SelfT>
class FormatWriter : public FormatEstimator {
public:

    /**
     * @brief Processes a format string and its subsequent arguments into the buffer.
     * @tparam ArgsT Variadic types to be formatted.
     * @param writer The current writer.
     * @param fmt A slice of format (e.g., "Some test: {} - replace value with args, {{}} - prints curly brakets").
     * @param args The values which whill write to format string.
     * @return Success result of the operation, see. 'FormatResult'.
     * 
     * @example
     * 
     * Alt::String str;
     * Alt::_StringDetails::StringFormatWriter<Alt::String::GetMinBytesCapacityOnAllocate()> writer{ str };
     * 
     * Alt::FormatWriter<Buffer>::FormatWrite(writer, "Count: {}", 10);
     * // str.FormatAppend("Count: {}", 10);
     */
    template <typename ...ArgsT>
    static FormatResult FormatWrite(FormatWriter& writer, Slice<const char> fmt, const ArgsT&... args) noexcept {

        return _FormatDetails::FormatWriteImpl<SelfT>::Write(writer, fmt, args...);
    }

    /**
     * @brief Appends a slice to the buffer.
     * @param writer The current writer.
     * @param fmt A slice of characters.
     * @return True if successful.
     * 
     * @example
     * 
     * Alt::String str;
     * Alt::_StringDetails::StringFormatWriter<Alt::String::GetMinBytesCapacityOnAllocate()> writer{ str };
     * 
     * Alt::FormatWriter<Buffer>::FormatWrite(writer, "Count: {}"); // just "Count: {}"
     * // str.FormatAppend("Count: {}");
     */
    static FormatResult FormatWrite(FormatWriter& writer, Slice<const char> fmt) noexcept {

        return _FormatDetails::FormatWriteImpl<SelfT>::Write(writer, fmt);
    }

    /**
     * @brief Additional reserve bytes before write.
     * @param sizeInBytesAddition The amount of memory to reserve.
     * @return True if allocation was successful.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool ReserveBytesAddition(const usize sizeInBytesAddition) noexcept {

        return Self().ReserveBytesAddition(sizeInBytesAddition);
    }

    /**
     * @brief Appends a character-type value (chara, char, char8_t, char16_t, wchar_t, ...).
     * @tparam T A character.
     * @param val The value to append.
     * @return True if successful.
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(const T val) noexcept {

        return Self().Append(val);
    }

    /**
     * @brief Appends the internal writer.
     * @param self The underlying buffer.
     * @return True if successful.
     */
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(const SelfT& self) noexcept {

        return Self().Append(self);
    }

    /**
     * @brief Appends a null-terminated string.
     * @tparam T A character type.
     * @param str The pointer to the start of the buffer.
     * @return True if successful.
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(const T* str) noexcept {

        return Self().Append(str);
    }

    /**
     * @brief Appends a string.
     * @tparam T A character type.
     * @param str The pointer to the start of the buffer.
     * @param size The size of string.
     * @return True if successful.
     */
    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(const T* str_, const usize size) noexcept {

        const Slice<const T> str{ str_, size };
        return Self().Append(str);
    }

    /**
     * @brief Appends a string Slice (e.g., Slice<const char>, Slice<const wchar_t>, ...).
     * @tparam T A type convertible to a Slice of character types.
     * @param str The value which convertible to Slice<const InnerT>.
     * @return True if successful.
     */
    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_character_v<InnerT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE bool Append(T&& str) noexcept {

        return Self().Append(std::forward<T>(str));
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool AppendIndent(const ssize additionalIndent = 0) noexcept {
        iferr_scope;

        const usize indentScope = static_cast<usize>(Num::Max(this->m_IndentScope + additionalIndent, 0));
        for (usize i = 0; i < indentScope; ++i) {

            Append("    ") iferr_return false;
        }

        return true;
    }

private:

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

/**
 * @brief The formatting class to format specific type to format.
 * 
 * @tparam T The type of the value to be formatted.
 * @details This class uses template specialization to provide custom logic for primitive types, 
 * collection types. It allows either direct write append and/or pre-allocation via `EstimateExactSize` (optional).
 */
template <typename T>
class Format {
public:

    /**
     * @brief Estimates the length of a slice for formatting.
     * @tparam U The type within the collection (U must be estimatable).
     * @param estimator A estimator to help estimate indent size.
     * @param parameters A parameters of format.
     * @param vals A slice of values.
     * @return An Optional containing the estimated byte count.
     */
    template <typename U = T,
        std::enable_if_t<!TypeTraits::is_character_v<U> && TypeTraits::is_format_estimatable_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, Slice<const char> parameters, const Slice<const U> vals) noexcept {
        iferr_scope;

        using CleanU = _FormatDetails::clean_t<U>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "noinline"_s) {

            EstimateExactSizeResult result{};
            result.size = 2;

            if (vals.IsEmpty()) {
                return result;
            }

            estimator.PushIndent();

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const U& val : iter) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val) iferr_return Nullopt;

                result.size += estimator.GetIndentEstimatedSize() + estimated.size;
                result.size += 2;
            }

            if (vals.IsPopulated()) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, vals.GetLast()) iferr_return Nullopt;

                result.size += estimator.GetIndentEstimatedSize() + estimated.size;
                result.size += 1;
            }

            estimator.PopIndent();

            result.size += estimator.GetIndentEstimatedSize() + 1;

            return result;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            EstimateExactSizeResult result{};
            result.size = 1;

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const U& val : iter) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, val) iferr_return Nullopt;

                result.size += estimated.size;
                result.size += 2;
            }

            if (vals.IsPopulated()) {

                const EstimateExactSizeResult estimated = Format<CleanU>::EstimateExactSize(estimator, parametersNext, vals.GetLast()) iferr_return Nullopt;

                result.size += estimated.size;
            }

            result.size += 1;

            return result;
        }

        return Nullopt;
    }

    /**
     * @brief Writes a collection of items formatted as a list (e.g., [0, 1, 2, 3]).
     * @tparam FormatWriterT The format writter.
     * @tparam U A inner estimatable type.
     * @param vals A slice of values to be printed as a comma-separated list.
     * @param parameters A parameters inside '{parameter_example}'/'{}'.
     * @return True if all items and brackets were appended successfully.
     */
    template <typename FormatWriterT, typename U = T,
        std::enable_if_t<!TypeTraits::is_character_v<U> && TypeTraits::is_formattable_v<U, FormatWriterT>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Slice<const U> vals) noexcept {
        iferr_scope;

        using CleanU = _FormatDetails::clean_t<U>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "noinline"_s) {

            writer.Append('[') iferr_return false;

            if (vals.IsEmpty()) {

                writer.Append(']') iferr_return false;
                return true;
            }

            writer.Append('\n') iferr_return false;
            writer.PushIndent();

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const U& val : iter) {

                writer.AppendIndent() iferr_return false;
                Format<CleanU>::WriteAppend(writer, parametersNext, val) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append('\n') iferr_return false;
            }

            if (vals.IsPopulated()) {

                writer.AppendIndent() iferr_return false;
                Format<CleanU>::WriteAppend(writer, parametersNext, vals.GetLast()) iferr_return false;
                writer.Append('\n') iferr_return false;
            }

            writer.PopIndent();

            writer.AppendIndent() iferr_return false;
            writer.Append(']') iferr_return false;

            return true;
        }

        if (parameters.IsEmpty() || parameters == "inline"_s) {

            writer.Append('[') iferr_return false;

            auto iter = vals
                .GetIterator()
                .Take(Num::SubWrap(vals.GetSize(), 1));

            for (const U& val : iter) {

                Format<CleanU>::WriteAppend(writer, parametersNext, val) iferr_return false;
                writer.Append(',') iferr_return false;
                writer.Append(' ') iferr_return false;
            }

            if (vals.IsPopulated()) {

                Format<CleanU>::WriteAppend(writer, parametersNext, vals.GetLast()) iferr_return false;
            }

            writer.Append(']') iferr_return false;

            return true;
        }

        return false;
    }

    /**
     * @brief Estimates size for like Slice constructible types with internal estimatable type.
     * @tparam U The type constructible to Slice<const InnerU>.
     */
    template <typename U = T, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            (!TypeTraits::is_character_v<InnerU> && TypeTraits::is_format_estimatable_v<InnerU>)
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, const Slice<const char> parameters, U&& val) noexcept {

        const Slice<const InnerU> vals = val;
        return EstimateExactSize(estimator, parameters, vals);
    }

    /**
     * @brief Writes a like Slice constructible types with internal formattable type.
     * @tparam FormatWriterT The Format Writer.
     * @tparam U The type constructible to Slice<const InnerU>.
     */
    template <typename FormatWriterT, typename U = T, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            !TypeTraits::is_character_v<InnerU> && TypeTraits::is_formattable_v<InnerU, FormatWriterT>
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, U&& val) noexcept {

        const Slice<const InnerU> vals = val;
        return WriteAppend(writer, parameters, vals);
    }

    /**
     * @brief Estimates size for Slice of characters.
     * @tparam U The type constructible to Slice<const char>, Slice<const wchar_t>, ...
     */
    template <typename U = T, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            TypeTraits::is_character_v<InnerU>
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, U&& val) noexcept {

        if (parameters.IsPopulated()) {
            return Nullopt;
        }

        const Slice<const InnerU> vals = val;

        EstimateExactSizeResult estimated{};
        estimated.size = vals.GetSizeBytes();

        return estimated;
    }

    /**
     * @brief Writes a Slice of characters.
     * @tparam FormatWriterT The Format Writer.
     * @tparam U The type constructible to Slice<const char>, Slice<const wchar_t>, ...
     */
    template <typename FormatWriterT, typename U = T, typename InnerU = TypeTraits::unwrap_inner_type_from_like_array_t<U>,
        std::enable_if_t<
            std::is_constructible_v<Slice<const InnerU>, U&&> &&
            TypeTraits::is_character_v<InnerU> 
        >* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, U&& val) noexcept {

        if (parameters.IsPopulated()) {
            return false;
        }

        const Slice<const InnerU> vals = val;
        return writer.Append(vals);
    }

    /**
     * @brief Estimates size for a C-string.
     * @tparam U A character type.
     * @param str A pointer to the C-string.
     * @return Optional length of the string.
     */
    template <typename U = T,
        std::enable_if_t<TypeTraits::is_character_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const U* str) noexcept {

        if (parameters.IsPopulated()) {
            return Nullopt;
        }

        EstimateExactSizeResult estimated{};
        estimated.size = _StringDetails::CalculateCStrLen(str) * sizeof(U);

        return estimated;
    }

    /**
     * @brief Writes a C-string.
     * @tparam FormatWriterT The Format Writer.
     * @tparam U The type characters (e.g., const char*, const wchar_t*, ...)
     */
    template <typename FormatWriterT, typename U = T,
        std::enable_if_t<TypeTraits::is_character_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const U* str) noexcept {

        if (parameters.IsPopulated()) {
            return false;
        }

        return writer.Append(str);
    }

    /**
     * @brief Estimates size for a character.
     * @tparam U A character type.
     * @param str A character.
     * @return Optional length of the character.
     */
    template <typename U = T,
        std::enable_if_t<TypeTraits::is_character_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const U) noexcept {

        if (parameters.IsPopulated()) {
            return Nullopt;
        }

        EstimateExactSizeResult estimated{};
        estimated.size = sizeof(U);

        return estimated;
    }

    /**
     * @brief Writes a character.
     * @tparam FormatWriterT The Format Writer.
     * @tparam U The type character (e.g., char, wchar_t, ...)
     */
    template <typename FormatWriterT, typename U = T,
        std::enable_if_t<TypeTraits::is_character_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const U ch) noexcept {

        if (parameters.IsPopulated()) {
            return false;
        }

        return writer.Append(ch);
    }
};

/**
 * @brief Specialization for bool.
 */
template <>
class Format<bool> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const bool val) noexcept {

        if (parameters == "digit"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = 1; // '1' or '0'

            return estimated;
        }

        if (parameters.IsPopulated()) {
            return Nullopt;
        }

        EstimateExactSizeResult estimated{};
        estimated.size = val ? 4 : 5; // 'true' or 'false'

        return estimated;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const bool val) noexcept {

        if (parameters == "digit"_s) {

            return writer.Append(val ? '1' : '0');
        }

        if (parameters.IsPopulated()) {
            return false;
        }

        return writer.Append(val ? "true" : "false");
    }
};

/**
 * @brief Specialization for u8.
 */
template <>
class Format<u8> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const u8 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "hex"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendHexImpl_CalculateBitsSize(val);

            return estimated;
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = Num::DigitCount(val);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const u8 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "hex"_s) {
            return _FormatDetails::ToStringAppendHexImpl(writer, val);
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {
            return _FormatDetails::ToStringAppendDecImpl(writer, val);
        }

        return false;
    }
};

/**
 * @brief Specialization for u16.
 */
template <>
class Format<u16> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const u16 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "hex"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendHexImpl_CalculateBitsSize(val);

            return estimated;
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = Num::DigitCount(val);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const u16 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "hex"_s) {
            return _FormatDetails::ToStringAppendHexImpl(writer, val);
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {
            return _FormatDetails::ToStringAppendDecImpl(writer, val);
        }

        return false;
    }
};

/**
 * @brief Specialization for u32.
 */
template <>
class Format<u32> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const u32 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "hex"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendHexImpl_CalculateBitsSize(val);

            return estimated;
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = Num::DigitCount(val);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const u32 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "hex"_s) {
            return _FormatDetails::ToStringAppendHexImpl(writer, val);
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {
            return _FormatDetails::ToStringAppendDecImpl(writer, val);
        }

        return false;
    }
};

/**
 * @brief Specialization for u64.
 */
template <>
class Format<u64> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const u64 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "hex"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendHexImpl_CalculateBitsSize(val);

            return estimated;
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = Num::DigitCount(val);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const u64 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "hex"_s) {
            return _FormatDetails::ToStringAppendHexImpl(writer, val);
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {
            return _FormatDetails::ToStringAppendDecImpl(writer, val);
        }

        return false;
    }
};

/**
 * @brief Specialization for s8.
 */
template <>
class Format<s8> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const s8 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "hex"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendHexImpl_CalculateBitsSize(val);

            return estimated;
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {

            EstimateExactSizeResult estimated{};

            estimated.size = Num::IsSignPositive(val) ? 0 : 1;
            estimated.size += Num::DigitCount(val);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const s8 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "hex"_s) {
            return _FormatDetails::ToStringAppendHexImpl(writer, val);
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {
            return _FormatDetails::ToStringAppendDecImpl(writer, val);
        }

        return false;
    }
};

/**
 * @brief Specialization for s16.
 */
template <>
class Format<s16> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const s16 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "hex"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendHexImpl_CalculateBitsSize(val);

            return estimated;
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {

            EstimateExactSizeResult estimated{};

            estimated.size = Num::IsSignPositive(val) ? 0 : 1;
            estimated.size += Num::DigitCount(val);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const s16 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "hex"_s) {
            return _FormatDetails::ToStringAppendHexImpl(writer, val);
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {
            return _FormatDetails::ToStringAppendDecImpl(writer, val);
        }

        return false;
    }
};

/**
 * @brief Specialization for s32.
 */
template <>
class Format<s32> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const s32 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "hex"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendHexImpl_CalculateBitsSize(val);

            return estimated;
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {

            EstimateExactSizeResult estimated{};

            estimated.size = Num::IsSignPositive(val) ? 0 : 1;
            estimated.size += Num::DigitCount(val);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const s32 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "hex"_s) {
            return _FormatDetails::ToStringAppendHexImpl(writer, val);
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {
            return _FormatDetails::ToStringAppendDecImpl(writer, val);
        }

        return false;
    }
};

/**
 * @brief Specialization for s64.
 */
template <>
class Format<s64> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const s64 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "hex"_s) {

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendHexImpl_CalculateBitsSize(val);

            return estimated;
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {

            EstimateExactSizeResult estimated{};

            estimated.size = Num::IsSignPositive(val) ? 0 : 1;
            estimated.size += Num::DigitCount(val);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const s64 val) noexcept {

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "hex"_s) {
            return _FormatDetails::ToStringAppendHexImpl(writer, val);
        }

        if (parameters.IsEmpty() || parameters == "dec"_s) {
            return _FormatDetails::ToStringAppendDecImpl(writer, val);
        }

        return false;
    }
};

/**
 * @brief Specialization for void*.
 */
template <typename T>
class Format<T*> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const void*) noexcept {

        if (parameters.IsPopulated()) {
            return Nullopt;
        }

        EstimateExactSizeResult estimated{};
        estimated.size = _FormatDetails::ToStringAppendPtrHexImpl_CalculateBitsSize();

        return estimated;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const void* ptr) noexcept {

        if (parameters.IsPopulated()) {
            return false;
        }

        return _FormatDetails::ToStringAppendPtrHexImpl(writer, reinterpret_cast<uptr>(ptr));
    }

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_character_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, const Slice<const char> parameters, const U* str) noexcept {

        if (parameters.IsPopulated()) {
            return Nullopt;
        }

        EstimateExactSizeResult estimated{};
        estimated.size = _StringDetails::CalculateCStrLen(str) * sizeof(U);

        return estimated;
    }

    template <typename FormatWriterT, typename U = T,
        std::enable_if_t<TypeTraits::is_character_v<U>>* = nullptr>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const U* str) noexcept {

        if (parameters.IsPopulated()) {
            return false;
        }

        return writer.Append(str);
    }
};

/**
 * @brief Specialization for f32.
 */
template <>
class Format<f32> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, Slice<const char> parameters, const f32 val) noexcept {
        iferr_scope;

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        // hex.4
        if (parameters.StartWith("hex"_s)) {

            parameters = parameters.SubSlice(3);

            const bool isPrefix_0x = false;

            if (parameters.StartWith('.')) {

                const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return Nullopt;

                char buf[zmij::buffer_sizes<f32>::hex];
                const usize size = zmij::detail::write_hex(buf, sizeof(buf), val, precision, isPrefix_0x);

                EstimateExactSizeResult estimated{};
                estimated.size = size;

                return estimated;
            }
            else {

                char buf[zmij::buffer_sizes<f32>::hex];
                char* end = zmij::detail::write_hex(buf, val, isPrefix_0x);

                EstimateExactSizeResult estimated{};
                estimated.size = static_cast<usize>(end - buf);

                return estimated;
            }
        }

        // dec.4
        if (parameters.StartWith("dec"_s)) {

            parameters = parameters.SubSlice(3);

            if (parameters.StartWith('.')) {

                goto f32_estimate_dec_with_precission;
            }
            else {

                char buf[zmij::buffer_sizes<f32>::shortest];
                char* end = zmij::write(buf, sizeof(buf), val);

                EstimateExactSizeResult estimated{};
                estimated.size = static_cast<usize>(end - buf);

                return estimated;
            }
        }

        // .4
        if (parameters.StartWith('.')) {
f32_estimate_dec_with_precission:

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return Nullopt;

            // general == fixed | scientific
            char buf[zmij::buffer_sizes<f32>::scientific];
            char* end = zmij::write_general(buf, sizeof(buf), val, precision);

            EstimateExactSizeResult estimated{};
            estimated.size = static_cast<usize>(end - buf);

            return estimated;
        }

        // fixed.4
        if (parameters.StartWith("fixed."_s)) {

            parameters = parameters.SubSlice(5);

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return Nullopt;

            char buf[zmij::buffer_sizes<f32>::fixed];
            char* end = zmij::write_fixed(buf, sizeof(buf), val, precision);

            EstimateExactSizeResult estimated{};
            estimated.size = static_cast<usize>(end - buf);

            return estimated;
        }

        // scientific.4
        if (parameters.StartWith("scientific."_s)) {

            parameters = parameters.SubSlice(10);

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return Nullopt;

            char buf[zmij::buffer_sizes<f32>::scientific];
            char* end = zmij::write_scientific(buf, sizeof(buf), val, precision);

            EstimateExactSizeResult estimated{};
            estimated.size = static_cast<usize>(end - buf);

            return estimated;
        }

        if (parameters.IsEmpty()) {

            char buf[zmij::buffer_sizes<f32>::shortest];
            char* end = zmij::write(buf, sizeof(buf), val);

            EstimateExactSizeResult estimated{};
            estimated.size = static_cast<usize>(end - buf);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const f32 val) noexcept {
        iferr_scope;

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        // hex.4
        if (parameters.StartWith("hex"_s)) {

            parameters = parameters.SubSlice(3);

            const bool isPrefix_0x = false;

            if (parameters.StartWith('.')) {

                const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return false;

                char buf[zmij::buffer_sizes<f32>::hex];
                const usize size = zmij::detail::write_hex(buf, sizeof(buf), val, precision, isPrefix_0x);

                return writer.Append(Slice<const char>{ buf, size });
            }
            else {

                char buf[zmij::buffer_sizes<f32>::hex];
                char* end = zmij::detail::write_hex(buf, val, isPrefix_0x);

                return writer.Append(Slice<const char>{ buf, end });
            }
        }

        // dec.4
        if (parameters.StartWith("dec"_s)) {

            parameters = parameters.SubSlice(3);

            if (parameters.StartWith('.')) {

                goto f32_write_dec_with_precission;
            }
            else {

                char buf[zmij::buffer_sizes<f32>::shortest];
                char* end = zmij::write(buf, sizeof(buf), val);

                return writer.Append(Slice<const char>{ buf, end });
            }
        }

        // .4
        if (parameters.StartWith('.')) {
f32_write_dec_with_precission:

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return false;

            // general == fixed | scientific
            char buf[zmij::buffer_sizes<f32>::scientific];
            char* end = zmij::write_general(buf, sizeof(buf), val, precision);

            return writer.Append(Slice<const char>{ buf, end });
        }

        // fixed.4
        if (parameters.StartWith("fixed."_s)) {

            parameters = parameters.SubSlice(5);

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return false;

            char buf[zmij::buffer_sizes<f32>::fixed];
            char* end = zmij::write_fixed(buf, sizeof(buf), val, precision);

            return writer.Append(Slice<const char>{ buf, end });
        }

        // scientific.4
        if (parameters.StartWith("scientific."_s)) {

            parameters = parameters.SubSlice(10);

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return false;

            char buf[zmij::buffer_sizes<f32>::scientific];
            char* end = zmij::write_scientific(buf, sizeof(buf), val, precision);

            return writer.Append(Slice<const char>{ buf, end });
        }

        if (parameters.IsEmpty()) {

            char buf[zmij::buffer_sizes<f32>::shortest];
            char* end = zmij::write(buf, sizeof(buf), val);

            return writer.Append(Slice<const char>{ buf, end });
        }

        return false;
    }

private:

    static Optional<int> ParsePrecision(const Slice<const char> str) noexcept {

        fast_float::parse_options desc{};
        desc.base = 10;

        usize precison;
        const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), precison, desc);

        if (parseResult.ptr != str.DataEnd()) {
            return Nullopt;
        }

        return static_cast<int>(precison);
    }
};

/**
 * @brief Specialization for f64.
 */
template <>
class Format<f64> {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator&, Slice<const char> parameters, const f64 val) noexcept {
        iferr_scope;

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            EstimateExactSizeResult estimated{};
            estimated.size = _FormatDetails::ToStringAppendBinImpl_CalculateBitsSize(val, isWithSeparator);

            return estimated;
        }

        // hex.4
        if (parameters.StartWith("hex"_s)) {

            parameters = parameters.SubSlice(3);

            const bool isPrefix_0x = false;

            if (parameters.StartWith('.')) {

                const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return Nullopt;

                char buf[zmij::buffer_sizes<f64>::hex];
                const usize size = zmij::detail::write_hex(buf, sizeof(buf), val, precision, isPrefix_0x);

                EstimateExactSizeResult estimated{};
                estimated.size = size;

                return estimated;
            }
            else {

                char buf[zmij::buffer_sizes<f64>::hex];
                char* end = zmij::detail::write_hex(buf, val, isPrefix_0x);

                EstimateExactSizeResult estimated{};
                estimated.size = static_cast<usize>(end - buf);

                return estimated;
            }
        }

        // dec.4
        if (parameters.StartWith("dec"_s)) {

            parameters = parameters.SubSlice(3);

            if (parameters.StartWith('.')) {

                goto f64_estimate_dec_with_precission;
            }
            else {

                char buf[zmij::buffer_sizes<f64>::shortest];
                char* end = zmij::write(buf, sizeof(buf), val);

                EstimateExactSizeResult estimated{};
                estimated.size = static_cast<usize>(end - buf);

                return estimated;
            }
        }

        // .4
        if (parameters.StartWith('.')) {
f64_estimate_dec_with_precission:

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return Nullopt;

            // general == fixed | scientific
            char buf[zmij::buffer_sizes<f64>::scientific];
            char* end = zmij::write_general(buf, sizeof(buf), val, precision);

            EstimateExactSizeResult estimated{};
            estimated.size = static_cast<usize>(end - buf);

            return estimated;
        }

        // fixed.4
        if (parameters.StartWith("fixed."_s)) {

            parameters = parameters.SubSlice(5);

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return Nullopt;

            char buf[zmij::buffer_sizes<f64>::fixed];
            char* end = zmij::write_fixed(buf, sizeof(buf), val, precision);

            EstimateExactSizeResult estimated{};
            estimated.size = static_cast<usize>(end - buf);

            return estimated;
        }

        // scientific.4
        if (parameters.StartWith("scientific."_s)) {

            parameters = parameters.SubSlice(10);

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return Nullopt;

            char buf[zmij::buffer_sizes<f64>::scientific];
            char* end = zmij::write_scientific(buf, sizeof(buf), val, precision);

            EstimateExactSizeResult estimated{};
            estimated.size = static_cast<usize>(end - buf);

            return estimated;
        }

        if (parameters.IsEmpty()) {

            char buf[zmij::buffer_sizes<f64>::shortest];
            char* end = zmij::write(buf, sizeof(buf), val);

            EstimateExactSizeResult estimated{};
            estimated.size = static_cast<usize>(end - buf);

            return estimated;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const f64 val) noexcept {
        iferr_scope;

        if (parameters == "bin"_s) {

            const bool isWithSeparator = false;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        if (parameters == "bin_sep"_s) {

            const bool isWithSeparator = true;

            return _FormatDetails::ToStringAppendBinImpl(writer, val, isWithSeparator);
        }

        // hex.4
        if (parameters.StartWith("hex"_s)) {

            parameters = parameters.SubSlice(3);

            const bool isPrefix_0x = false;

            if (parameters.StartWith('.')) {

                const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return false;

                char buf[zmij::buffer_sizes<f64>::hex];
                const usize size = zmij::detail::write_hex(buf, sizeof(buf), val, precision, isPrefix_0x);

                return writer.Append(Slice<const char>{ buf, size });
            }
            else {

                char buf[zmij::buffer_sizes<f64>::hex];
                char* end = zmij::detail::write_hex(buf, val, isPrefix_0x);

                return writer.Append(Slice<const char>{ buf, end });
            }
        }

        // dec.4
        if (parameters.StartWith("dec"_s)) {

            parameters = parameters.SubSlice(3);

            if (parameters.StartWith('.')) {

                goto f64_write_dec_with_precission;
            }
            else {

                char buf[zmij::buffer_sizes<f64>::shortest];
                char* end = zmij::write(buf, sizeof(buf), val);

                return writer.Append(Slice<const char>{ buf, end });
            }
        }

        // .4
        if (parameters.StartWith('.')) {
f64_write_dec_with_precission:

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return false;

            // general == fixed | scientific
            char buf[zmij::buffer_sizes<f64>::scientific];
            char* end = zmij::write_general(buf, sizeof(buf), val, precision);

            return writer.Append(Slice<const char>{ buf, end });
        }

        // fixed.4
        if (parameters.StartWith("fixed."_s)) {

            parameters = parameters.SubSlice(5);

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return false;

            char buf[zmij::buffer_sizes<f64>::fixed];
            char* end = zmij::write_fixed(buf, sizeof(buf), val, precision);

            return writer.Append(Slice<const char>{ buf, end });
        }

        // scientific.4
        if (parameters.StartWith("scientific."_s)) {

            parameters = parameters.SubSlice(10);

            const int precision = ParsePrecision(parameters.SubSlice(1)) iferr_return false;

            char buf[zmij::buffer_sizes<f64>::scientific];
            char* end = zmij::write_scientific(buf, sizeof(buf), val, precision);

            return writer.Append(Slice<const char>{ buf, end });
        }

        if (parameters.IsEmpty()) {

            char buf[zmij::buffer_sizes<f64>::shortest];
            char* end = zmij::write(buf, sizeof(buf), val);

            return writer.Append(Slice<const char>{ buf, end });
        }

        return false;
    }

private:

    static Optional<int> ParsePrecision(const Slice<const char> str) noexcept {

        fast_float::parse_options desc{};
        desc.base = 10;

        usize precison;
        const fast_float::from_chars_result parseResult = fast_float::from_chars_advanced(str.Data(), str.DataEnd(), precison, desc);

        if (parseResult.ptr != str.DataEnd()) {
            return Nullopt;
        }

        return static_cast<int>(precison);
    }
};

/**
 * @brief Specialization for Optional<T>.
 */
template <typename T>
class Format<Optional<T>> {
public:

    template <typename U = T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U>>* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, Slice<const char> parameters, const Optional<U>& val
    ) noexcept {
        iferr_scope;

        using CleanT = _FormatDetails::clean_t<T>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            EstimateExactSizeResult result{};

            if (val) {

                const EstimateExactSizeResult estimated = Format<CleanT>::EstimateExactSize(estimator, parametersNext, val.GetValue()) iferr_return Nullopt;
                result.size = 12 + estimated.size;
            }
            else {

                result.size = 7;
            }

            return result;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {

            EstimateExactSizeResult result{};

            if (val) {

                result.size = 11;

                estimator.PushIndent();
                {
                    const EstimateExactSizeResult estimated = Format<CleanT>::EstimateExactSize(estimator, parametersNext, val.GetValue()) iferr_return Nullopt;
                    result.size += estimator.GetIndentEstimatedSize() + estimated.size + 1;
                }
                estimator.PopIndent();

                result.size += estimator.GetIndentEstimatedSize() + 1;
            }
            else {

                result.size = 7;
            }

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Optional<T>& val) noexcept {
        iferr_scope;

        using CleanT = _FormatDetails::clean_t<T>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            if (val) {

                writer.Append("Optional{ ") iferr_return false;
                Format<CleanT>::WriteAppend(writer, parametersNext, val.GetValue()) iferr_return false;
                writer.Append(" }") iferr_return false;
            }
            else {

                writer.Append("Nullopt") iferr_return false;
            }

            return true;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {
            
            if (val) {

                writer.Append("Optional {\n") iferr_return false;

                writer.PushIndent();
                {
                    writer.AppendIndent() iferr_return false;
                    Format<CleanT>::WriteAppend(writer, parametersNext, val.GetValue()) iferr_return false;
                    writer.Append('\n') iferr_return false;
                }
                writer.PopIndent();

                writer.AppendIndent() iferr_return false;
                writer.Append('}') iferr_return false;
            }
            else {

                writer.Append("Nullopt") iferr_return false;
            }

            return true;
        }

        return false;
    }
};

/**
 * @brief Specialization for Result<OkT, ErrT>.
 */
template <typename OkT, typename ErrT>
class Format<Result<OkT, ErrT>> {
public:

    template <typename OkU = OkT, typename ErrU = ErrT,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<OkU> && TypeTraits::is_format_estimatable_v<ErrU>>* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, Slice<const char> parameters, const Result<OkU, ErrU>& val
    ) noexcept {
        iferr_scope;

        using CleanOkT = _FormatDetails::clean_t<OkT>;
        using CleanErrT = _FormatDetails::clean_t<ErrT>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            EstimateExactSizeResult result{};

            if (val) {

                result.size = 4;
                const EstimateExactSizeResult estimated = Format<CleanOkT>::EstimateExactSize(estimator, parametersNext, val.GetOk()) iferr_return Nullopt;
                result.size += estimated.size + 2;
            }
            else {

                result.size = 5;
                const EstimateExactSizeResult estimated = Format<CleanErrT>::EstimateExactSize(estimator, parametersNext, val.GetErr()) iferr_return Nullopt;
                result.size += estimated.size + 2;
            }

            return result;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {

            EstimateExactSizeResult result{};

            if (val) {

                result.size = 5;

                estimator.PushIndent();
                {
                    const EstimateExactSizeResult estimated = Format<CleanOkT>::EstimateExactSize(estimator, parametersNext, val.GetOk()) iferr_return Nullopt;
                    result.size += estimator.GetIndentEstimatedSize() + estimated.size + 1;
                }
                estimator.PopIndent();

                result.size += estimator.GetIndentEstimatedSize() + 1;
            }
            else {

                result.size = 6;

                estimator.PushIndent();
                {
                    const EstimateExactSizeResult estimated = Format<CleanErrT>::EstimateExactSize(estimator, parametersNext, val.GetErr()) iferr_return Nullopt;
                    result.size += estimator.GetIndentEstimatedSize() + estimated.size + 1;
                }
                estimator.PopIndent();

                result.size += estimator.GetIndentEstimatedSize() + 1;
            }

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Result<OkT, ErrT>& val) noexcept {
        iferr_scope;

        using CleanOkT = _FormatDetails::clean_t<OkT>;
        using CleanErrT = _FormatDetails::clean_t<ErrT>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            if (val) {

                writer.Append("Ok{ ") iferr_return false;
                Format<CleanOkT>::WriteAppend(writer, parametersNext, val.GetOk()) iferr_return false;
                writer.Append(" }") iferr_return false;
            }
            else {

                writer.Append("Err{ ") iferr_return false;
                Format<CleanErrT>::WriteAppend(writer, parametersNext, val.GetErr()) iferr_return false;
                writer.Append(" }") iferr_return false;
            }

            return true;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {
            
            if (val) {

                writer.Append("Ok {\n") iferr_return false;

                writer.PushIndent();
                {
                    writer.AppendIndent() iferr_return false;
                    Format<CleanOkT>::WriteAppend(writer, parametersNext, val.GetOk()) iferr_return false;
                    writer.Append('\n') iferr_return false;
                }
                writer.PopIndent();

                writer.AppendIndent() iferr_return false;
                writer.Append('}') iferr_return false;
            }
            else {

                writer.Append("Err {\n") iferr_return false;

                writer.PushIndent();
                {
                    writer.AppendIndent() iferr_return false;
                    Format<CleanErrT>::WriteAppend(writer, parametersNext, val.GetErr()) iferr_return false;
                    writer.Append('\n') iferr_return false;
                }
                writer.PopIndent();

                writer.AppendIndent() iferr_return false;
                writer.Append('}') iferr_return false;
            }

            return true;
        }

        return false;
    }
};

template <typename OkT>
class Format<Result<OkT, void>> {
public:

    template <typename OkU = OkT,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<OkU>>* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, Slice<const char> parameters, const Result<OkU, void>& val
    ) noexcept {
        iferr_scope;

        using CleanOkT = _FormatDetails::clean_t<OkT>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            EstimateExactSizeResult result{};

            if (val) {

                result.size = 4;
                const EstimateExactSizeResult estimated = Format<CleanOkT>::EstimateExactSize(estimator, parametersNext, val.GetOk()) iferr_return Nullopt;
                result.size += estimated.size + 2;
            }
            else {

                result.size = 5;
            }

            return result;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {

            EstimateExactSizeResult result{};

            if (val) {

                result.size = 5;

                estimator.PushIndent();
                {
                    const EstimateExactSizeResult estimated = Format<CleanOkT>::EstimateExactSize(estimator, parametersNext, val.GetOk()) iferr_return Nullopt;
                    result.size += estimator.GetIndentEstimatedSize() + estimated.size + 1;
                }
                estimator.PopIndent();

                result.size += estimator.GetIndentEstimatedSize() + 1;
            }
            else {

                result.size = 5;
            }

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Result<OkT, void>& val) noexcept {
        iferr_scope;

        using CleanOkT = _FormatDetails::clean_t<OkT>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            if (val) {

                writer.Append("Ok{ ") iferr_return false;
                Format<CleanOkT>::WriteAppend(writer, parametersNext, val.GetOk()) iferr_return false;
                writer.Append(" }") iferr_return false;
            }
            else {

                writer.Append("Err{}") iferr_return false;
            }

            return true;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {
            
            if (val) {

                writer.Append("Ok {\n") iferr_return false;

                writer.PushIndent();
                {
                    writer.AppendIndent() iferr_return false;
                    Format<CleanOkT>::WriteAppend(writer, parametersNext, val.GetOk()) iferr_return false;
                    writer.Append('\n') iferr_return false;
                }
                writer.PopIndent();

                writer.AppendIndent() iferr_return false;
                writer.Append('}') iferr_return false;
            }
            else {

                writer.Append("Err{}") iferr_return false;
            }

            return true;
        }

        return false;
    }
};

template <typename ErrT>
class Format<Result<void, ErrT>> {
public:

    template <typename ErrU = ErrT,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<ErrU>>* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, Slice<const char> parameters, const Result<void, ErrU>& val
    ) noexcept {
        iferr_scope;

        using CleanErrT = _FormatDetails::clean_t<ErrT>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            EstimateExactSizeResult result{};

            if (val) {

                result.size = 4;
            }
            else {

                result.size = 5;
                const EstimateExactSizeResult estimated = Format<CleanErrT>::EstimateExactSize(estimator, parametersNext, val.GetErr()) iferr_return Nullopt;
                result.size += estimated.size + 2;
            }

            return result;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {

            EstimateExactSizeResult result{};

            if (val) {

                result.size = 4;
            }
            else {

                result.size = 6;

                estimator.PushIndent();
                {
                    const EstimateExactSizeResult estimated = Format<CleanErrT>::EstimateExactSize(estimator, parametersNext, val.GetErr()) iferr_return Nullopt;
                    result.size += estimator.GetIndentEstimatedSize() + estimated.size + 1;
                }
                estimator.PopIndent();

                result.size += estimator.GetIndentEstimatedSize() + 1;
            }

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Result<void, ErrT>& val) noexcept {
        iferr_scope;

        using CleanErrT = _FormatDetails::clean_t<ErrT>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            if (val) {

                writer.Append("Ok{}") iferr_return false;
            }
            else {

                writer.Append("Err{ ") iferr_return false;
                Format<CleanErrT>::WriteAppend(writer, parametersNext, val.GetErr()) iferr_return false;
                writer.Append(" }") iferr_return false;
            }

            return true;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {
            
            if (val) {

                writer.Append("Ok{}") iferr_return false;
            }
            else {

                writer.Append("Err {\n") iferr_return false;

                writer.PushIndent();
                {
                    writer.AppendIndent() iferr_return false;
                    Format<CleanErrT>::WriteAppend(writer, parametersNext, val.GetErr()) iferr_return false;
                    writer.Append('\n') iferr_return false;
                }
                writer.PopIndent();

                writer.AppendIndent() iferr_return false;
                writer.Append('}') iferr_return false;
            }

            return true;
        }

        return false;
    }
};

/**
 * @brief Specialization for Pair<T0, T1>.
 */
template <typename T0, typename T1>
class Format<Pair<T0, T1>> {
public:

    template <typename U0 = T0, typename U1 = T1,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<U0> && TypeTraits::is_format_estimatable_v<U1>>* = nullptr>
    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, Slice<const char> parameters, const Pair<U0, U1>& val
    ) noexcept {
        iferr_scope;

        using CleanT0 = _FormatDetails::clean_t<U0>;
        using CleanT1 = _FormatDetails::clean_t<U1>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            EstimateExactSizeResult result{};

            result.size = 10;

            const EstimateExactSizeResult estimated0 = Format<CleanT0>::EstimateExactSize(estimator, parametersNext, val.f()) iferr_return Nullopt;
            const EstimateExactSizeResult estimated1 = Format<CleanT1>::EstimateExactSize(estimator, parametersNext, val.s()) iferr_return Nullopt;

            result.size += estimated0.size;
            result.size += estimated1.size;

            return result;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {

            EstimateExactSizeResult result{};

            result.size = 7;

            estimator.PushIndent();
            {
                result.size += estimator.GetIndentEstimatedSize();
                const EstimateExactSizeResult estimated0 = Format<CleanT0>::EstimateExactSize(estimator, parametersNext, val.f()) iferr_return Nullopt;
                result.size += estimated0.size;
                result.size += 2;

                result.size += estimator.GetIndentEstimatedSize();
                const EstimateExactSizeResult estimated1 = Format<CleanT1>::EstimateExactSize(estimator, parametersNext, val.s()) iferr_return Nullopt;
                result.size += estimated1.size;
                result.size += 1;
            }
            estimator.PopIndent();

            result.size += estimator.GetIndentEstimatedSize() + 1;

            return result;
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, Slice<const char> parameters, const Pair<T0, T1>& val) noexcept {
        iferr_scope;

        using CleanT0 = _FormatDetails::clean_t<T0>;
        using CleanT1 = _FormatDetails::clean_t<T1>;

        Slice<const char> parametersNext;
        const usize offset = parameters.FindFirst(';') ifok {

            parametersNext = parameters.SubSlice(offset + 1);
            parameters = parameters.SubSlice(0, offset);
        }

        if (parameters == "inline"_s) {

            writer.Append("Pair{ ") iferr_return false;
            Format<CleanT0>::WriteAppend(writer, parametersNext, val.f()) iferr_return false;
            writer.Append(", ") iferr_return false;
            Format<CleanT1>::WriteAppend(writer, parametersNext, val.s()) iferr_return false;
            writer.Append(" }") iferr_return false;

            return true;
        }

        if (parameters.IsEmpty() || parameters == "noinline"_s) {

            writer.Append("Pair {\n") iferr_return false;

            writer.PushIndent();
            {
                writer.AppendIndent() iferr_return false;
                Format<CleanT0>::WriteAppend(writer, parametersNext, val.f()) iferr_return false;
                writer.Append(",\n") iferr_return false;

                writer.AppendIndent() iferr_return false;
                Format<CleanT1>::WriteAppend(writer, parametersNext, val.s()) iferr_return false;
                writer.Append('\n') iferr_return false;
            }
            writer.PopIndent();

            writer.AppendIndent() iferr_return false;
            writer.Append('}') iferr_return false;

            return true;
        }

        return false;
    }
};

/**\cond */
namespace _FormatDetails {

template <typename SelfT>
class FormatParser {
public:

    template <typename HeadT, typename ...TailT>
    ALT_FORCE_INLINE FormatResult Parse(Slice<const char> fmt, const HeadT& head, const TailT&... tail) noexcept {
        iferr_scope;

        usize accPos = 0;
        FormatResult err = FormatResult::Ok();

        for (Slice<const char> fmtSearch = fmt;;) {

            usize tryPos = fmtSearch.FindFirst('{') iferr_return FormatError::ManyArguments;

            const usize size = fmtSearch.GetSize();
            if (tryPos + 1 == size) {
                return FormatError::InvalidFormat;
            }

            const usize firstOpenBracketPos = tryPos;
            tryPos += 1;

            if (fmtSearch[tryPos] == '{') {

                accPos += tryPos;
                tryPos += 1;

                Alt::Slice<const char> fmtEscaping = fmtSearch.SubSlice(tryPos);
                while (fmtEscaping.StartWith("{{")) {

                    fmtEscaping = fmtEscaping.SubSlice(2);

                    accPos += 1;
                }

                OnFmtStrAppend(fmt.SubSlice(0, accPos)) iferr_return_with(err) err;

                fmtSearch = fmtEscaping;
                fmt = fmtEscaping;
                accPos = 0;
            }
            else {

                usize firstCloseBracketPos = 0;
                if (fmtSearch[tryPos] != '}') {

                    firstCloseBracketPos = fmtSearch
                        .SubSlice(tryPos + 1)
                        .FindFirst('}') iferr_return FormatError::InvalidFormat;

                    firstCloseBracketPos += 1;
                }

                accPos += firstOpenBracketPos;
                OnFmtStrAppend(fmt.SubSlice(0, accPos)) iferr_return_with(err) err;

                const Slice<const char> parameters = fmtSearch.SubSlice(tryPos, firstCloseBracketPos);
                OnFormatAppend(parameters, head) iferr_return_with(err) err;

                return Parse(fmt.SubSlice(accPos + 2 + parameters.GetSize()), tail...);
            }
        }
    }

    ALT_FORCE_INLINE FormatResult Parse(Slice<const char> fmt) noexcept {

        return OnFmtStrAppend(fmt);
    }

private:

    template <typename T>
    ALT_FORCE_INLINE FormatResult OnFmtStrAppend(T&& val) noexcept {

        return Self().OnFmtStrAppend(std::forward<T>(val));
    }

    template <typename T>
    ALT_FORCE_INLINE FormatResult OnFormatAppend(const Slice<const char> parameters, T&& val) noexcept {

        return Self().OnFormatAppend(parameters, std::forward<T>(val));
    }

    ALT_FORCE_INLINE SelfT& Self() noexcept {
        return static_cast<SelfT&>(*this);
    }

    ALT_FORCE_INLINE const SelfT& Self() const noexcept {
        return static_cast<const SelfT&>(*this);
    }
};

template <typename FormatWriterT>
class FormatParserWriter : public FormatParser<FormatParserWriter<FormatWriterT>> {
public:

    ALT_FORCE_INLINE FormatParserWriter(FormatWriter<FormatWriterT>& writer) noexcept
        : m_pWriter(&writer)
    {}

    template <typename T>
    ALT_FORCE_INLINE FormatResult OnFmtStrAppend(T&& val) noexcept {
        iferr_scope;

        m_pWriter->Append(std::forward<T>(val)) iferr_return FormatError::WriterFailed;

        return FormatResult::Ok();
    }

    template <typename T>
    ALT_FORCE_INLINE FormatResult OnFormatAppend(const Slice<const char> parameters, T&& val) noexcept {
        iferr_scope;

        using CleanT = _FormatDetails::clean_t<T>;

        Format<CleanT>::WriteAppend(*m_pWriter, parameters, std::forward<T>(val)) iferr_return FormatError::FormatFailed;

        return FormatResult::Ok();
    }

private:

    FormatWriter<FormatWriterT>* m_pWriter;
};

class FormatParserEstimator : public FormatParser<FormatParserEstimator> {
public:

    ALT_FORCE_INLINE FormatParserEstimator() noexcept = default;

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_FORCE_INLINE FormatResult OnFmtStrAppend(const T) noexcept {
        iferr_scope;

        m_EstimatedSize = Num::AddOverflowCheck(m_EstimatedSize, 1) iferr_return FormatError::Overflow;

        return FormatResult::Ok();
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_character_v<T>>* = nullptr>
    ALT_FORCE_INLINE FormatResult OnFmtStrAppend(const T* str) noexcept {
        iferr_scope;

        m_EstimatedSize = Num::AddOverflowCheck(m_EstimatedSize, _StringDetails::CalculateCStrLen(str)) iferr_return FormatError::Overflow;

        return FormatResult::Ok();
    }

    template <typename T, typename InnerT = TypeTraits::unwrap_inner_type_from_like_array_t<T>,
        std::enable_if_t<std::is_constructible_v<Slice<const InnerT>, T&&> && TypeTraits::is_character_v<InnerT>>* = nullptr>
    ALT_FORCE_INLINE FormatResult OnFmtStrAppend(T&& str_) noexcept {
        iferr_scope;

        const Slice<const InnerT> str = str_;
        m_EstimatedSize = Num::AddOverflowCheck(m_EstimatedSize, str.GetSize()) iferr_return FormatError::Overflow;

        return FormatResult::Ok();
    }

    template <typename T,
        std::enable_if_t<TypeTraits::is_format_estimatable_v<T>>* = nullptr>
    ALT_FORCE_INLINE FormatResult OnFormatAppend(const Slice<const char> parameters, const T& val) noexcept {
        iferr_scope;

        using CleanT = _FormatDetails::clean_t<T>;

        const EstimateExactSizeResult estimated = Format<CleanT>::EstimateExactSize(m_FormatEstimator, parameters, val).GetValueOrDefault();
        m_EstimatedSize = Num::AddOverflowCheck(m_EstimatedSize, estimated.size) iferr_return FormatError::Overflow;

        return FormatResult::Ok();
    }

    template <typename T,
        std::enable_if_t<!TypeTraits::is_format_estimatable_v<T>>* = nullptr>
    ALT_FORCE_INLINE FormatResult OnFormatAppend(const Slice<const char>, const T&) noexcept {

        return FormatResult::Ok();
    }

    ALT_FORCE_INLINE usize GetEstimatedSize() const noexcept {

        return m_EstimatedSize;
    }

private:

    FormatEstimator m_FormatEstimator;
    usize m_EstimatedSize = 0;
};

template <typename FormatWriterT>
struct FormatWriteImpl {

    template <typename ...ArgsT>
    ALT_FORCE_INLINE static FormatResult Write(FormatWriter<FormatWriterT>& writer, const Slice<const char> fmt, const ArgsT&... args) noexcept {
        iferr_scope;

#ifndef ALT_USE_FORMAT_WITHOUT_SIZE_ESTIMATION

        {
            FormatResult err = FormatResult::Ok();

            FormatParserEstimator estimator{};
            estimator.Parse(fmt, args...) iferr_return_with(err) err;

            writer.ReserveBytesAddition(estimator.GetEstimatedSize()) iferr_return FormatError::WriterFailed;
        }
#endif // ALT_USE_FORMAT_WITHOUT_SIZE_ESTIMATION

        return WriteImpl(writer, fmt, args...);
    }

    ALT_FORCE_INLINE static FormatResult Write(FormatWriter<FormatWriterT>& writer, const Slice<const char> fmt) noexcept {

        return WriteImpl(writer, fmt);
    }

private:

    template <typename ...ArgsT>
    ALT_FORCE_INLINE static FormatResult WriteImpl(FormatWriter<FormatWriterT>& writer, const Slice<const char> fmt, const ArgsT&... args) noexcept {

        FormatParserWriter<FormatWriterT> parser(writer);
        return parser.Parse(fmt, args...);
    }
};

} // namespace _FormatDetails
/**\endcond */

} // namespace Alt

#endif // __ALT_STD_FORMAT__