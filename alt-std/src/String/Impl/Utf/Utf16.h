#pragma once

#ifndef __ALT_STD_UTF16__
#define __ALT_STD_UTF16__

#include "UtfCore.h"

#include "Utf32.h"

namespace Alt {
namespace Utf {

namespace _16 {

ALT_NODISCARD ALT_FORCE_INLINE bool IsSurrogateUtf16(const u16 utf16) noexcept {

    // 3.9.2 UTF-16
    //return utf16 >= 0xD800 && utf16 <= 0xDFFF;

    // 0xD800 -> 11011000 00000000
    // 0xDFFF -> 11011111 11111111
    // 0xF800 -> 11111000 00000000

    return (utf16 & 0xF800) == 0xD800;
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsLowSurrogateUtf16(const u16 utf16Second) noexcept {

    // 23.6.1 Surrogates Area: U+D800-U+DFFF
    //return utf16Second >= 0xDC00 && utf16Second <= 0xDFFF;

    // 0xDC00 -> 11011100 00000000
    // 0xDFFF -> 11011111 11111111
    // 0xFC00 -> 11111100 00000000

    return (utf16Second & 0xFC00) == 0xDC00;
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsHighSurrogateUtf16(const u16 utf16First) noexcept {

    // 23.6.1 Surrogates Area: U+D800-U+DFFF
    //return utf16First >= 0xD800 && utf16First <= 0xDBFF;

    // 0xD800 -> 11011000 00000000
    // 0xDBFF -> 11011011 11111111
    // 0xFC00 -> 11111100 00000000

    return (utf16First & 0xFC00) == 0xD800;
}

// return numbers of bytes to forward adjust or 0 if invalid utf8
ALT_NODISCARD ALT_FORCE_INLINE usize ForwardUtf16(const u16 (&utf16)[2]) noexcept {

    // 3.9.2 UTF-16
    // 23.6.1 Surrogates Area: U+D800-U+DFFF

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsSurrogateUtf16(utf16[0])) {

        return IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1])
            ? 2
            : 0;
    }

    return 1;

    // This check has already been handled by 'IsValidUtf16(...)'
    //return !(utf16 > 0xD7FF && utf16 < 0xE000);
    //return (utf16 <= 0xD7FF) || (utf16 >= 0xE000 && utf16 <= 0xFFFF);
#else

    constexpr static u32 MASK = 4227922944; // { 0xFC00, 0xFC00 };

#if defined(ALT_PLATFORM_LITTLE_ENDIAN)

    constexpr static u32 VALIDATION_MASK = 3691042816; // { 0xD800, 0xDC00 };
#elif defined(ALT_PLATFORM_BIG_ENDIAN)

    //constexpr static u32 VALIDATION_MASK = 3623934976; // { 0xDC00, 0xD800 };
#else

#error specify endianness
#endif

    const u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);

    usize result = (asU32 & MASK) == VALIDATION_MASK
        ? 2
        : 0;

    result += IsSurrogateUtf16(utf16[0])
        ? 0
        : 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

// return numbers of bytes to backward adjust or 0 if invalid utf8
ALT_NODISCARD ALT_FORCE_INLINE usize BackwardUtf16(const u16 (&utf16)[2]) noexcept {

    // 3.9.2 UTF-16
    // 23.6.1 Surrogates Area: U+D800-U+DFFF

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsSurrogateUtf16(utf16[1])) {

        return IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1])
            ? 2
            : 0;
    }

    return 1;

    // This check has already been handled by 'IsValidUtf16(...)'
    //return !(utf16 > 0xD7FF && utf16 < 0xE000);
    //return (utf16 <= 0xD7FF) || (utf16 >= 0xE000 && utf16 <= 0xFFFF);
#else

    constexpr static u32 MASK = 4227922944; // { 0xFC00, 0xFC00 };

#if defined(ALT_PLATFORM_LITTLE_ENDIAN)

    constexpr static u32 VALIDATION_MASK = 3691042816; // { 0xD800, 0xDC00 };
#elif defined(ALT_PLATFORM_BIG_ENDIAN)

    //constexpr static u32 VALIDATION_MASK = 3623934976; // { 0xDC00, 0xD800 };
#else

#error specify endianness
#endif

    const u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);

    usize result = (asU32 & MASK) == VALIDATION_MASK
        ? 2
        : 0;

    result += IsSurrogateUtf16(utf16[1])
        ? 0
        : 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsValidUtf16(const u16 (&utf16)[2]) noexcept {

    return ForwardUtf16(utf16) != 0;
}

// if 'length == 4' then utf16 is surrogate
ALT_NODISCARD ALT_FORCE_INLINE usize LengthUtf16ToUtf8(const u16 (&utf16)[2]) {

    // Table 3-7

    usize result = 1;
    result += utf16[0] >= 0x0080 ? 1 : 0;
    result += utf16[0] >= 0x0800 ? 1 : 0;

    result += IsSurrogateUtf16(utf16[0]) ? 1 : 0;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf16ToUtf8IgnoreErrors(const u16 (&utf16)[2], u8 (&utf8)[4]) noexcept {

    // Table 3-6

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    const usize len = LengthUtf16ToUtf8(utf16);

    if (len == 1) {

        utf8[0] = (static_cast<u8>(utf16[0] >> 0) & 0b01111111);
    }
    else if (len == 2) {

        utf8[0] = (static_cast<u8>(utf16[0] >> 6) & 0b00011111) | 0b11000000;
        utf8[1] = (static_cast<u8>(utf16[0] >> 0) & 0b00111111) | 0b10000000;
    }
    else if (len == 3) {

        utf8[0] = (static_cast<u8>(utf16[0] >> 12) & 0b00001111) | 0b11100000;
        utf8[1] = (static_cast<u8>(utf16[0] >> 6 ) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf16[0] >> 0 ) & 0b00111111) | 0b10000000;
    }
    else if (len == 4) {

        // Table 3-5, convert to utf32 and then to utf8
        const u32 extractedHighSurrogate = static_cast<u32>(utf16[0] & 0b0000001111111111);
        const u32 extractedLowSurrogate  = static_cast<u32>(utf16[1] & 0b0000001111111111);

        const u32 utf32 = ((extractedHighSurrogate << 10) | extractedLowSurrogate) + 0x10000;

        utf8[0] = (static_cast<u8>(utf32 >> 18) & 0b00000111) | 0b11110000;
        utf8[1] = (static_cast<u8>(utf32 >> 12) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf32 >> 6 ) & 0b00111111) | 0b10000000;
        utf8[3] = (static_cast<u8>(utf32 >> 0 ) & 0b00111111) | 0b10000000;
    }

    Result result;
    result.advance = len == 4 ? 2 : 1;
    result.consumed = len;

    return result;
#else

    using namespace _32::_TableDetails;

    const usize len = LengthUtf16ToUtf8(utf16);

    u32 utf32 = static_cast<u32>(utf16[0]);
    if (IsSurrogateUtf16(utf16[0])) {

        // Table 3-5, convert to utf32 and then to utf8
        u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
        asU32 = asU32 & UTF16_MASK0;

        u16 utf16_[2];
        Memory::TransmuteInto<u32, u16[2]>(asU32, utf16_);
        utf32 = ((utf16_[0] << 10) | utf16_[1]) + 0x10000;
    }

    utf8[0] = static_cast<u8>(utf32 >> 18);
    utf8[1] = static_cast<u8>(utf32 >> 12);
    utf8[2] = static_cast<u8>(utf32 >> 6);
    utf8[3] = static_cast<u8>(utf32 >> 0);

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);

    const usize maskIdx = len - 1;

    asU32 = (asU32 & UTF8_MASK0[maskIdx]) | UTF8_MASK1[maskIdx];
    asU32 = asU32 >> (32 - len * 8);

    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8);

    Result result;
    result.advance = len == 4 ? 2 : 1;
    result.consumed = len;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf16ToUtf8(const u16 (&utf16)[2], u8 (&utf8)[4]) noexcept {

    // Table 3-6

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    bool isOk = true;
    const usize len = LengthUtf16ToUtf8(utf16);

    if (len == 1) {

        utf8[0] = (static_cast<u8>(utf16[0] >> 0) & 0b01111111);
    }
    else if (len == 2) {

        utf8[0] = (static_cast<u8>(utf16[0] >> 6) & 0b00011111) | 0b11000000;
        utf8[1] = (static_cast<u8>(utf16[0] >> 0) & 0b00111111) | 0b10000000;
    }
    else if (len == 3) {

        utf8[0] = (static_cast<u8>(utf16[0] >> 12) & 0b00001111) | 0b11100000;
        utf8[1] = (static_cast<u8>(utf16[0] >> 6 ) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf16[0] >> 0 ) & 0b00111111) | 0b10000000;
    }
    else if (len == 4) {

        // Table 3-5, convert to utf32 and then to utf8
        const u32 extractedHighSurrogate = static_cast<u32>(utf16[0] & 0b0000001111111111);
        const u32 extractedLowSurrogate  = static_cast<u32>(utf16[1] & 0b0000001111111111);

        const u32 utf32 = ((extractedHighSurrogate << 10) | extractedLowSurrogate) + 0x10000;

        isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

        utf8[0] = (static_cast<u8>(utf32 >> 18) & 0b00000111) | 0b11110000;
        utf8[1] = (static_cast<u8>(utf32 >> 12) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf32 >> 6 ) & 0b00111111) | 0b10000000;
        utf8[3] = (static_cast<u8>(utf32 >> 0 ) & 0b00111111) | 0b10000000;
    }

    Result result;
    result.advance = isOk && len == 4 ? 2 : 1;
    result.consumed = isOk ? len : 0;

    return result;
#else

    using namespace _32::_TableDetails;

    bool isOk = true;
    const usize len = LengthUtf16ToUtf8(utf16);

    u32 utf32 = static_cast<u32>(utf16[0]);
    if (IsSurrogateUtf16(utf16[0])) {

        isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

        // Table 3-5, convert to utf32 and then to utf8
        u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
        asU32 = asU32 & UTF16_MASK0;

        u16 utf16_[2];
        Memory::TransmuteInto<u32, u16[2]>(asU32, utf16_);
        utf32 = ((utf16_[0] << 10) | utf16_[1]) + 0x10000;
    }

    utf8[0] = static_cast<u8>(utf32 >> 18);
    utf8[1] = static_cast<u8>(utf32 >> 12);
    utf8[2] = static_cast<u8>(utf32 >> 6);
    utf8[3] = static_cast<u8>(utf32 >> 0);

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);

    const usize maskIdx = len - 1;

    asU32 = (asU32 & UTF8_MASK0[maskIdx]) | UTF8_MASK1[maskIdx];
    asU32 = asU32 >> (32 - len * 8);

    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8);

    Result result;
    result.advance = isOk && len == 4 ? 2 : 1;
    result.consumed = isOk ? len : 0;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf16ToUtf8Replace(
    const u16 (&utf16)[2], u8 (&utf8)[4], const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    // Table 3-6

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    const usize len = LengthUtf16ToUtf8(utf16);

    if (len == 1) {

        utf8[0] = (static_cast<u8>(utf16[0] >> 0) & 0b01111111);
    }
    else if (len == 2) {

        utf8[0] = (static_cast<u8>(utf16[0] >> 6) & 0b00011111) | 0b11000000;
        utf8[1] = (static_cast<u8>(utf16[0] >> 0) & 0b00111111) | 0b10000000;
    }
    else if (len == 3) {

        utf8[0] = (static_cast<u8>(utf16[0] >> 12) & 0b00001111) | 0b11100000;
        utf8[1] = (static_cast<u8>(utf16[0] >> 6 ) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf16[0] >> 0 ) & 0b00111111) | 0b10000000;
    }
    else if (len == 4) {

        if ((utf16[0] & 0xFC00) != 0xD800 || (utf16[1] & 0xFC00) != 0xDC00){

            return Utf::_32::Utf32ToUtf8IgnoreErrors(replaceUtf32, utf8);
        }

        // Table 3-5, convert to utf32 and then to utf8
        const u32 extractedHighSurrogate = static_cast<u32>(utf16[0] & 0b0000001111111111);
        const u32 extractedLowSurrogate  = static_cast<u32>(utf16[1] & 0b0000001111111111);

        const u32 utf32 = ((extractedHighSurrogate << 10) | extractedLowSurrogate) + 0x10000;

        utf8[0] = (static_cast<u8>(utf32 >> 18) & 0b00000111) | 0b11110000;
        utf8[1] = (static_cast<u8>(utf32 >> 12) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf32 >> 6 ) & 0b00111111) | 0b10000000;
        utf8[3] = (static_cast<u8>(utf32 >> 0 ) & 0b00111111) | 0b10000000;
    }

    Result result;
    result.advance = len == 4 ? 2 : 1;
    result.consumed = len;

    return result;
#else

    using namespace _32::_TableDetails;

    bool isOk = true;
    usize len = LengthUtf16ToUtf8(utf16);
    
    u32 utf32 = static_cast<u32>(utf16[0]);
    if (IsSurrogateUtf16(utf16[0])) {

        isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

        // Table 3-5, convert to utf32 and then to utf8
        u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
        asU32 = asU32 & UTF16_MASK0;

        u16 utf16_[2];
        Memory::TransmuteInto<u32, u16[2]>(asU32, utf16_);
        utf32 = ((utf16_[0] << 10) | utf16_[1]) + 0x10000;

        utf32 = isOk ? utf32 : replaceUtf32;
        len = isOk ? len : _32::LengthUtf32ToUtf8(replaceUtf32);
    }

    utf8[0] = static_cast<u8>(utf32 >> 18);
    utf8[1] = static_cast<u8>(utf32 >> 12);
    utf8[2] = static_cast<u8>(utf32 >> 6);
    utf8[3] = static_cast<u8>(utf32 >> 0);

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);

    const usize maskIdx = len - 1;

    asU32 = (asU32 & UTF8_MASK0[maskIdx]) | UTF8_MASK1[maskIdx];
    asU32 = asU32 >> (32 - len * 8);

    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8);

    Result result;
    result.advance = isOk && len == 4 ? 2 : 1;
    result.consumed = len;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf16ToUtf32IgnoreErrorsBackward(const u16 (&utf16)[2], u32& utf32) noexcept {

    // Table 3-5

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (!IsSurrogateUtf16(utf16[1])) {

        utf32 = static_cast<u32>(utf16[1]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    // if (!IsHighSurrogateUtf16(utf16[0])) return error;
    // if (!IsLowSurrogateUtf16(utf16[1])) return error;

    const u32 extractedHighSurrogate = static_cast<u32>(utf16[0] & 0b0000001111111111);
    const u32 extractedLowSurrogate  = static_cast<u32>(utf16[1] & 0b0000001111111111);

    utf32 = ((extractedHighSurrogate << 10) | extractedLowSurrogate) + 0x10000;

    Result result;
    result.advance = 2;
    result.consumed = 1;

    return result;
#else

    using namespace _32::_TableDetails;

    if (!IsSurrogateUtf16(utf16[1])) {

        utf32 = static_cast<u32>(utf16[1]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
    asU32 = asU32 & UTF16_MASK0;

    u16 utf16_[2];
    Memory::TransmuteInto<u32, u16[2]>(asU32, utf16_);
    utf32 = ((utf16_[0] << 10) | utf16_[1]) + 0x10000;

    Result result;
    result.advance = 2;
    result.consumed = 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf16ToUtf32Backward(const u16 (&utf16)[2], u32& utf32) noexcept {

    // Table 3-5

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (!IsSurrogateUtf16(utf16[1])) {

        utf32 = static_cast<u32>(utf16[1]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    const bool isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

    const u32 extractedHighSurrogate = static_cast<u32>(utf16[0] & 0b0000001111111111);
    const u32 extractedLowSurrogate  = static_cast<u32>(utf16[1] & 0b0000001111111111);

    utf32 = ((extractedHighSurrogate << 10) | extractedLowSurrogate) + 0x10000;

    Result result;
    result.advance = isOk ? 2 : 1;
    result.consumed = isOk ? 1 : 0;

    return result;
#else

    using namespace _32::_TableDetails;

    if (!IsSurrogateUtf16(utf16[1])) {

        utf32 = static_cast<u32>(utf16[1]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    const bool isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

    u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
    asU32 = asU32 & UTF16_MASK0;

    u16 utf16_[2];
    Memory::TransmuteInto<u32, u16[2]>(asU32, utf16_);
    utf32 = ((utf16_[0] << 10) | utf16_[1]) + 0x10000;

    Result result;
    result.advance = isOk ? 2 : 1;
    result.consumed = isOk ? 1 : 0;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf16ToUtf32ReplaceBackward(
    const u16 (&utf16)[2], u32& utf32, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    // Table 3-5

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (!IsSurrogateUtf16(utf16[1])) {

        utf32 = static_cast<u32>(utf16[1]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    const bool isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

    const u32 extractedHighSurrogate = static_cast<u32>(utf16[0] & 0b0000001111111111);
    const u32 extractedLowSurrogate  = static_cast<u32>(utf16[1] & 0b0000001111111111);

    utf32 = ((extractedHighSurrogate << 10) | extractedLowSurrogate) + 0x10000;

    utf32 = isOk ? utf32 : replaceUtf32;

    Result result;
    result.advance = isOk ? 2 : 1;
    result.consumed = 1;

    return result;
#else

    using namespace _32::_TableDetails;

    if (!IsSurrogateUtf16(utf16[1])) {

        utf32 = static_cast<u32>(utf16[1]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    const bool isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

    u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
    asU32 = asU32 & UTF16_MASK0;

    u16 utf16_[2];
    Memory::TransmuteInto<u32, u16[2]>(asU32, utf16_);
    utf32 = ((utf16_[0] << 10) | utf16_[1]) + 0x10000;

    utf32 = isOk ? utf32 : replaceUtf32;

    Result result;
    result.advance = isOk ? 2 : 1;
    result.consumed = 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf16ToUtf32IgnoreErrors(const u16 (&utf16)[2], u32& utf32) noexcept {

    // Table 3-5

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (!IsSurrogateUtf16(utf16[0])) {

        utf32 = static_cast<u32>(utf16[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    // if (!IsHighSurrogateUtf16(utf16[0])) return error;
    // if (!IsLowSurrogateUtf16(utf16[1])) return error;

    const u32 extractedHighSurrogate = static_cast<u32>(utf16[0] & 0b0000001111111111);
    const u32 extractedLowSurrogate  = static_cast<u32>(utf16[1] & 0b0000001111111111);

    utf32 = ((extractedHighSurrogate << 10) | extractedLowSurrogate) + 0x10000;

    Result result;
    result.advance = 2;
    result.consumed = 1;

    return result;
#else

    using namespace _32::_TableDetails;

    if (!IsSurrogateUtf16(utf16[0])) {

        utf32 = static_cast<u32>(utf16[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
    asU32 = asU32 & UTF16_MASK0;

    u16 utf16_[2];
    Memory::TransmuteInto<u32, u16[2]>(asU32, utf16_);
    utf32 = ((utf16_[0] << 10) | utf16_[1]) + 0x10000;

    Result result;
    result.advance = 2;
    result.consumed = 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf16ToUtf32(const u16 (&utf16)[2], u32& utf32) noexcept {

    // Table 3-5

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (!IsSurrogateUtf16(utf16[0])) {

        utf32 = static_cast<u32>(utf16[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    const bool isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

    const u32 extractedHighSurrogate = static_cast<u32>(utf16[0] & 0b0000001111111111);
    const u32 extractedLowSurrogate  = static_cast<u32>(utf16[1] & 0b0000001111111111);

    utf32 = ((extractedHighSurrogate << 10) | extractedLowSurrogate) + 0x10000;

    Result result;
    result.advance = isOk ? 2 : 1;
    result.consumed = isOk ? 1 : 0;

    return result;
#else

    using namespace _32::_TableDetails;

    if (!IsSurrogateUtf16(utf16[0])) {

        utf32 = static_cast<u32>(utf16[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    const bool isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

    u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
    asU32 = asU32 & UTF16_MASK0;

    u16 utf16_[2];
    Memory::TransmuteInto<u32, u16[2]>(asU32, utf16_);
    utf32 = ((utf16_[0] << 10) | utf16_[1]) + 0x10000;

    Result result;
    result.advance = isOk ? 2 : 1;
    result.consumed = isOk ? 1 : 0;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf16ToUtf32Replace(
    const u16 (&utf16)[2], u32& utf32, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    // Table 3-5

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (!IsSurrogateUtf16(utf16[0])) {

        utf32 = static_cast<u32>(utf16[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    const bool isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

    const u32 extractedHighSurrogate = static_cast<u32>(utf16[0] & 0b0000001111111111);
    const u32 extractedLowSurrogate  = static_cast<u32>(utf16[1] & 0b0000001111111111);

    utf32 = ((extractedHighSurrogate << 10) | extractedLowSurrogate) + 0x10000;

    utf32 = isOk ? utf32 : replaceUtf32;

    Result result;
    result.advance = isOk ? 2 : 1;
    result.consumed = 1;

    return result;
#else

    using namespace _32::_TableDetails;

    if (!IsSurrogateUtf16(utf16[0])) {

        utf32 = static_cast<u32>(utf16[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    const bool isOk = IsHighSurrogateUtf16(utf16[0]) && IsLowSurrogateUtf16(utf16[1]);

    u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
    asU32 = asU32 & UTF16_MASK0;

    u16 utf16_[2];
    Memory::TransmuteInto<u32, u16[2]>(asU32, utf16_);
    utf32 = ((utf16_[0] << 10) | utf16_[1]) + 0x10000;

    utf32 = isOk ? utf32 : replaceUtf32;

    Result result;
    result.advance = isOk ? 2 : 1;
    result.consumed = 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

} // namespace _16
} // namespace Utf
} // namespace Alt

#endif // __ALT_STD_UTF16__