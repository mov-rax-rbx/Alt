#pragma once

#ifndef __ALT_STD_UTF32__
#define __ALT_STD_UTF32__

#include "UtfCore.h"

namespace Alt {
namespace Utf {

namespace _32 {

#ifndef ALT_USE_UTF_NON_BRANCHLESS
namespace _TableDetails {

#if defined(ALT_PLATFORM_LITTLE_ENDIAN)

constexpr static u32 UTF8_MASK0[4]{
    2130706432, // { 0b00000000, 0b00000000, 0b00000000, 0b01111111 },
    1058996224, // { 0b00000000, 0b00000000, 0b00011111, 0b00111111 },
    1061097216, // { 0b00000000, 0b00001111, 0b00111111, 0b00111111 },
    1061109511, // { 0b00000111, 0b00111111, 0b00111111, 0b00111111 },
};

constexpr static u32 UTF8_MASK1[4]{
    0,          // { 0b00000000, 0b00000000, 0b00000000, 0b00000000 },
    2160066560, // { 0b00000000, 0b00000000, 0b11000000, 0b10000000 },
    2155929600, // { 0b00000000, 0b11100000, 0b10000000, 0b10000000 },
    2155905264, // { 0b11110000, 0b10000000, 0b10000000, 0b10000000 },
};

constexpr static usize UTF16_MASK0 = 67044351;   // { 0b0000001111111111, 0b0000001111111111 }
constexpr static usize UTF16_MASK1 = 3691042816; // { 0xD800, 0xDC00 }

#elif defined(ALT_PLATFORM_BIG_ENDIAN)

constexpr static u32 UTF8_MASK0[4]{
    127,       // { 0b00000000, 0b00000000, 0b00000000, 0b01111111 },
    7999,      // { 0b00000000, 0b00000000, 0b00011111, 0b00111111 },
    999231,    // { 0b00000000, 0b00001111, 0b00111111, 0b00111111 },
    121585471, // { 0b00000111, 0b00111111, 0b00111111, 0b00111111 },
};

constexpr static u32 UTF8_MASK1[4]{
    0,          // { 0b00000000, 0b00000000, 0b00000000, 0b00000000 },
    49280,      // { 0b00000000, 0b00000000, 0b11000000, 0b10000000 },
    14712960,   // { 0b00000000, 0b11100000, 0b10000000, 0b10000000 },
    4034953344, // { 0b11110000, 0b10000000, 0b10000000, 0b10000000 },
};

constexpr static usize UTF16_MASK0 = 67044351;   // { 0b0000001111111111, 0b0000001111111111 }
constexpr static usize UTF16_MASK1 = 3623934976; // { 0xD800, 0xDC00 }

#else

#error specify endianness
#endif

} // namespace _TableDetails
#endif // ALT_USE_UTF_NON_BRANCHLESS

ALT_NODISCARD ALT_FORCE_INLINE bool IsValidUtf32(const u32 utf32) noexcept {

    // 3.9.1 UTF-32
    //return !(utf32 > 0x0010FFFF || (utf32 >= 0x0000D800 && utf32 <= 0x0000DFFF));

    // 0x0010FFFF -> 00010000 11111111 11111111
    // 0x0000D800 -> 00000000 11011000 00000000
    // 0x0000DFFF -> 00000000 11011111 11111111
    // 0xFFFFF800 -> 11111111 11111000 00000000

    //return !(utf32 > 0x0010FFFF || (utf32 & 0xFFFFF800) == 0x0000D800);

    // 0x0010FFFF -> 00010000 11111111 11111111
    // 0x0000D800 -> 00000000 11011000 00000000
    // 0x00000800 -> 00000000 00001000 00000000

    //return utf32 <= 0x0010FFFF && (utf32 < 0x0000D800 || utf32 > 0x0000DFFF);
    //return (utf32 ^ 0x0000D800) - (0x0000DFFF - 0x0000D800 + 1) <= 0x0010FFFF - (0x0000DFFF - 0x0000D800 + 1);

    return (utf32 ^ 0x0000D800) - 0x00000800 <= 0x0010FFFF - 0x00000800;
}

ALT_NODISCARD ALT_FORCE_INLINE usize LengthUtf32ToUtf8(const u32 utf32) {

    // Table 3-7

    usize result = 1;
    result += utf32 >= 0x0080  ? 1 : 0;
    result += utf32 >= 0x0800  ? 1 : 0;
    result += utf32 >= 0x10000 ? 1 : 0;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE usize LengthUtf32ToUtf16(const u32 utf32) {

    // Table 3-5

    usize result = 1;
    result += utf32 > 0xFFFF;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf32ToUtf8IgnoreErrors(const u32 utf32, u8 (&utf8)[4]) noexcept {

    // Table 3-6

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    const usize len = LengthUtf32ToUtf8(utf32);

    if (len == 1) {

        utf8[0] = (static_cast<u8>(utf32 >> 0) & 0b01111111);
    }
    else if (len == 2) {

        utf8[0] = (static_cast<u8>(utf32 >> 6) & 0b00011111) | 0b11000000;
        utf8[1] = (static_cast<u8>(utf32 >> 0) & 0b00111111) | 0b10000000;
    }
    else if (len == 3) {

        // if ((utf32 & 0xFFFFF800) == 0x0000D800) return error;

        utf8[0] = (static_cast<u8>(utf32 >> 12) & 0b00001111) | 0b11100000;
        utf8[1] = (static_cast<u8>(utf32 >> 6 ) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf32 >> 0 ) & 0b00111111) | 0b10000000;
    }
    else if (len == 4) {

        // if (utf32 > 0x0010FFFF) return error;

        utf8[0] = (static_cast<u8>(utf32 >> 18) & 0b00000111) | 0b11110000;
        utf8[1] = (static_cast<u8>(utf32 >> 12) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf32 >> 6 ) & 0b00111111) | 0b10000000;
        utf8[3] = (static_cast<u8>(utf32 >> 0 ) & 0b00111111) | 0b10000000;
    }

    Result result;
    result.advance = 1;
    result.consumed = len;

    return result;
#else

    using namespace _TableDetails;

    const usize len = LengthUtf32ToUtf8(utf32);

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
    result.advance = 1;
    result.consumed = len;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf32ToUtf8(const u32 utf32, u8 (&utf8)[4]) noexcept {

    // Table 3-6

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    bool isOk = true;
    const usize len = LengthUtf32ToUtf8(utf32);

    if (len == 1) {

        utf8[0] = (static_cast<u8>(utf32 >> 0) & 0b01111111);
    }
    else if (len == 2) {

        utf8[0] = (static_cast<u8>(utf32 >> 6) & 0b00011111) | 0b11000000;
        utf8[1] = (static_cast<u8>(utf32 >> 0) & 0b00111111) | 0b10000000;
    }
    else if (len == 3) {

        isOk = (utf32 & 0xFFFFF800) != 0x0000D800;

        utf8[0] = (static_cast<u8>(utf32 >> 12) & 0b00001111) | 0b11100000;
        utf8[1] = (static_cast<u8>(utf32 >> 6 ) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf32 >> 0 ) & 0b00111111) | 0b10000000;
    }
    else if (len == 4) {

        isOk = utf32 <= 0x0010FFFF;

        utf8[0] = (static_cast<u8>(utf32 >> 18) & 0b00000111) | 0b11110000;
        utf8[1] = (static_cast<u8>(utf32 >> 12) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf32 >> 6 ) & 0b00111111) | 0b10000000;
        utf8[3] = (static_cast<u8>(utf32 >> 0 ) & 0b00111111) | 0b10000000;
    }

    Result result;
    result.advance = 1;
    result.consumed = isOk ? len : 0;

    return result;
#else

    using namespace _TableDetails;

    const bool isOk = IsValidUtf32(utf32);
    const usize len = LengthUtf32ToUtf8(utf32);

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
    result.advance = 1;
    result.consumed = isOk ? len : 0;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf32ToUtf8Replace(
    u32 utf32, u8 (&utf8)[4], const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    // Table 3-6

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    const bool isOk = IsValidUtf32(utf32);
    utf32 = isOk ? utf32 : replaceUtf32;

    const usize len = LengthUtf32ToUtf8(utf32);

    if (len == 1) {

        utf8[0] = (static_cast<u8>(utf32 >> 0) & 0b01111111);
    }
    else if (len == 2) {

        utf8[0] = (static_cast<u8>(utf32 >> 6) & 0b00011111) | 0b11000000;
        utf8[1] = (static_cast<u8>(utf32 >> 0) & 0b00111111) | 0b10000000;
    }
    else if (len == 3) {

        utf8[0] = (static_cast<u8>(utf32 >> 12) & 0b00001111) | 0b11100000;
        utf8[1] = (static_cast<u8>(utf32 >> 6 ) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf32 >> 0 ) & 0b00111111) | 0b10000000;
    }
    else if (len == 4) {

        utf8[0] = (static_cast<u8>(utf32 >> 18) & 0b00000111) | 0b11110000;
        utf8[1] = (static_cast<u8>(utf32 >> 12) & 0b00111111) | 0b10000000;
        utf8[2] = (static_cast<u8>(utf32 >> 6 ) & 0b00111111) | 0b10000000;
        utf8[3] = (static_cast<u8>(utf32 >> 0 ) & 0b00111111) | 0b10000000;
    }

    Result result;
    result.advance = 1;
    result.consumed = len;

    return result;
#else

    using namespace _TableDetails;

    const bool isOk = IsValidUtf32(utf32);
    utf32 = isOk ? utf32 : replaceUtf32;

    const usize len = LengthUtf32ToUtf8(utf32);

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
    result.advance = 1;
    result.consumed = len;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf32ToUtf16IgnoreErrors(u32 utf32, u16 (&utf16)[2]) noexcept {

    // Table 3-5

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if ((utf32 & 0xFFFF0000) == 0) {

        utf16[0] = static_cast<u16>(utf32);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    // see. 'Utf16ToUtf8(...)' for suroggate part

    utf32 -= 0x10000;

    utf16[0] = (static_cast<u16>(utf32 >> 10) & 0b0000001111111111) | 0xD800;
    utf16[1] = (static_cast<u16>(utf32)       & 0b0000001111111111) | 0xDC00;

    Result result;
    result.advance = 1;
    result.consumed = 2;

    return result;
#else

    using namespace _TableDetails;

    if ((utf32 & 0xFFFF0000) == 0) {

        utf16[0] = static_cast<u16>(utf32);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    // see. 'Utf16ToUtf8(...)' for suroggate part

    utf32 -= 0x10000;

    utf16[0] = static_cast<u16>(utf32 >> 10);
    utf16[1] = static_cast<u16>(utf32);

    u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);

    asU32 = (asU32 & UTF16_MASK0) | UTF16_MASK1;

    Memory::TransmuteInto<u32, u16[2]>(asU32, utf16);

    Result result;
    result.advance = 1;
    result.consumed = 2;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf32ToUtf16(u32 utf32, u16 (&utf16)[2]) noexcept {

    // Table 3-5

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if ((utf32 & 0xFFFF0000) == 0) {

        utf16[0] = static_cast<u16>(utf32);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    // see. 'Utf16ToUtf8(...)' for suroggate part

    const bool isOk = IsValidUtf32(utf32);

    utf32 -= 0x10000;

    utf16[0] = (static_cast<u16>(utf32 >> 10) & 0b0000001111111111) | 0xD800;
    utf16[1] = (static_cast<u16>(utf32)       & 0b0000001111111111) | 0xDC00;

    Result result;
    result.advance = 1;
    result.consumed = isOk ? 2 : 0;

    return result;
#else

    using namespace _TableDetails;

    if ((utf32 & 0xFFFF0000) == 0) {

        utf16[0] = static_cast<u16>(utf32);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    // see. 'Utf16ToUtf8(...)' for suroggate part

    const bool isOk = IsValidUtf32(utf32);

    utf32 -= 0x10000;

    utf16[0] = static_cast<u16>(utf32 >> 10);
    utf16[1] = static_cast<u16>(utf32);

    u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);

    asU32 = (asU32 & UTF16_MASK0) | UTF16_MASK1;

    Memory::TransmuteInto<u32, u16[2]>(asU32, utf16);

    Result result;
    result.advance = 1;
    result.consumed = isOk ? 2 : 0;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf32ToUtf16Replace(
    u32 utf32, u16 (&utf16)[2], const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    // Table 3-5

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    const bool isOk = IsValidUtf32(utf32);
    utf32 = isOk ? utf32 : replaceUtf32;

    if ((utf32 & 0xFFFF0000) == 0) {

        utf16[0] = static_cast<u16>(utf32);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    // see. 'Utf16ToUtf8(...)' for suroggate part

    utf32 -= 0x10000;

    utf16[0] = (static_cast<u16>(utf32 >> 10) & 0b0000001111111111) | 0xD800;
    utf16[1] = (static_cast<u16>(utf32)       & 0b0000001111111111) | 0xDC00;

    Result result;
    result.advance = 1;
    result.consumed = 2;

    return result;
#else

    using namespace _TableDetails;

    const bool isOk = IsValidUtf32(utf32);
    utf32 = isOk ? utf32 : replaceUtf32;

    if ((utf32 & 0xFFFF0000) == 0) {

        utf16[0] = static_cast<u16>(utf32);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    // see. 'Utf16ToUtf8(...)' for suroggate part

    utf32 -= 0x10000;

    utf16[0] = static_cast<u16>(utf32 >> 10);
    utf16[1] = static_cast<u16>(utf32);

    u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);

    asU32 = (asU32 & UTF16_MASK0) | UTF16_MASK1;

    Memory::TransmuteInto<u32, u16[2]>(asU32, utf16);

    Result result;
    result.advance = 1;
    result.consumed = 2;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

} // namespace _32
} // namespace Utf
} // namespace Alt

#endif // __ALT_STD_UTF32__