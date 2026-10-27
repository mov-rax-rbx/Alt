#pragma once

#ifndef __ALT_STD_UTF8__
#define __ALT_STD_UTF8__

#include "UtfCore.h"

#include "Utf32.h"

namespace Alt {
namespace Utf {
namespace _8 {

#ifndef ALT_USE_UTF_NON_BRANCHLESS
namespace _TableDetails {

#if defined(ALT_PLATFORM_LITTLE_ENDIAN)

constexpr static u32 UTF8_VALIDATION_MASK[4]{
    2147483648, // { 0b00000000, 0b00000000, 0b00000000, 0b10000000 },
    3235905536, // { 0b00000000, 0b00000000, 0b11100000, 0b11000000 },
    3233869824, // { 0b00000000, 0b11110000, 0b11000000, 0b11000000 },
    3233857784, // { 0b11111000, 0b11000000, 0b11000000, 0b11000000 },
};

#elif defined(ALT_PLATFORM_BIG_ENDIAN)

constexpr static u32 UTF8_VALIDATION_MASK[4]{
    128,        // { 0b00000000, 0b00000000, 0b00000000, 0b10000000 },
    57536,      // { 0b00000000, 0b00000000, 0b11100000, 0b11000000 },
    15777984,   // { 0b00000000, 0b11110000, 0b11000000, 0b11000000 },
    4173381824, // { 0b11111000, 0b11000000, 0b11000000, 0b11000000 },
};

#else

#error specify endianness
#endif

} // namespace _TableDetails
#endif // ALT_USE_UTF_NON_BRANCHLESS

ALT_NODISCARD ALT_FORCE_INLINE bool IsAscii(const u8 utf8First) noexcept {

    return utf8First < 0b10000000;
}

// return numbers of bytes to forward adjust or 0 if invalid utf8
ALT_NODISCARD ALT_FORCE_INLINE usize ForwardUtf8(const u8 (&utf8)[4]) noexcept {

    // Table 3-6
    // Table 3-7

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[0])) {

        return 1;
    }

    if ((utf8[0] & 0b11100000) == 0b11000000) {

        const bool isOk = (utf8[1] & 0b11000000) == 0b10000000;

        const u32 codePoint =
            (utf8[0] & 0b00011111) << 6 |
            (utf8[1] & 0b00111111)
        ;

        // 0x0080 -> 00000000 10000000
        // 0x07FF -> 00000111 11111111

        return isOk && codePoint >= 0x0080 && codePoint <= 0x07FF
            ? 2
            : 0;
    }

    if ((utf8[0] & 0b11110000) == 0b11100000) {

        const bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000;

        const u32 codePoint =
            (utf8[0] & 0b00001111) << 12 |
            (utf8[1] & 0b00111111) << 6 |
            (utf8[2] & 0b00111111)
        ;

        // 0x0800 -> 00001000 00000000
        // 0xD7FF -> 11010111 11111111
        // 0xE000 -> 11100000 00000000

        return isOk && ((codePoint >= 0x0800 && codePoint <= 0xD7FF) || (codePoint >= 0xE000))
            ? 3
            : 0;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        const bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        const u32 codePoint =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        // 0x010000 -> 00000001 00000000 00000000
        // 0x10FFFF -> 00010000 11111111 11111111

        return isOk && codePoint >= 0x10000 && codePoint <= 0x10FFFF
            ? 4
            : 0;
    }

    return 0;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    // STEP 0:
    //maskIdx = (utf8[0] & 0b11100000) == 0b11000000
    //	? 1
    //	: maskIdx;

    //maskIdx = (utf8[0] & 0b11110000) == 0b11100000
    //	? 2
    //	: maskIdx;

    //maskIdx = (utf8[0] & 0b11111000) == 0b11110000
    //	? 3
    //	: maskIdx;

    // STEP 1:
    //constexpr static u8 UTF8_MASK_IDX[32]{
    //	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // < 0b11000
    //	1, 1, 1, 1, 2, 2, 3, 0
    //};

    //const usize maskIdx = static_cast<usize>(UTF8_MASK_IDX[utf8[0] >> 3]);

    // STEP 2:
    //maskIdx += (((utf8[0] >> 3) + 1) & (32 - 1)) > 24 ? 1 : 0;
    //maskIdx += (((utf8[0] >> 3) + 1) & (32 - 1)) > 28 ? 1 : 0;
    //maskIdx += (((utf8[0] >> 3) + 1) & (32 - 1)) > 30 ? 1 : 0;

    //maskIdx += ((utf8[0] + 1) & (32 * 8 - 1)) > 24 * 8 ? 1 : 0;
    //maskIdx += ((utf8[0] + 1) & (32 * 8 - 1)) > 28 * 8 ? 1 : 0;
    //maskIdx += ((utf8[0] + 1) & (32 * 8 - 1)) > 30 * 8 ? 1 : 0;

    usize maskIdx = 0;
    maskIdx += utf8[0] >= 24 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 28 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 30 * 8 ? 1 : 0;

    // handles case when >= 0b11111000 (do not need this when using UTF8_VALIDATION_MASK)

    //const u8 utf80Checked = utf8[0] + 0b1000;
    //maskIdx += utf80Checked >= 24 * 8 + 0b1000 ? 1 : 0;
    //maskIdx += utf80Checked >= 28 * 8 + 0b1000 ? 1 : 0;
    //maskIdx += utf80Checked >= 30 * 8 + 0b1000 ? 1 : 0;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);
    asU32 = asU32 << (24 - maskIdx * 8);

    bool isOk = (asU32 & UTF8_VALIDATION_MASK[maskIdx]) == UTF8_MASK1[maskIdx];
    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    const u32 utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    isOk = maskIdx == 1 ? isOk && (utf32 >= 0x0080) : isOk;

    //isOk = maskIdx == 2 ? isOk && ((utf32 >= 0x0800 && utf32 <= 0xD7FF) || (utf32 >= 0xE000)) : isOk;

    //const u32 utf32_ = utf32 + 0xFFFF2000;
    //isOk = (utf32_ <= 0xFFFF2000) || (utf32_ >= 0xFFFF2800 && utf32_ <= 0xFFFFF7FF);
    //isOk = (utf32_ <= 0xFFFF2000 || utf32_ >= 0xFFFF2800) && (utf32_ <= 0xFFFFF7FF);
    //isOk = (utf32_ < 0xFFFF2001 || utf32_ > 0xFFFF27FF) && (utf32_ <= 0xFFFFF7FF);
    isOk = maskIdx == 2 ? isOk && (((utf32 + 0xFFFF2000) ^ 0xFFFF2001) - 0x7FF <= 0xFFFFF7FF - 0x7FF) : isOk;

    isOk = maskIdx == 3 ? isOk && (utf32 >= 0x10000 && utf32 <= 0x10FFFF) : isOk;

    return isOk ? maskIdx + 1 : 0;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE usize ForwardUtf8IgnoreErrors(const u8 (&utf8)[4]) noexcept {

    usize maskIdx = 0;

    const u8 utf80Checked = utf8[0] + 0b1000;
    maskIdx += utf80Checked >= 24 * 8 + 0b1000 ? 1 : 0;
    maskIdx += utf80Checked >= 28 * 8 + 0b1000 ? 1 : 0;
    maskIdx += utf80Checked >= 30 * 8 + 0b1000 ? 1 : 0;

    return maskIdx + 1;
}

ALT_NODISCARD ALT_FORCE_INLINE usize BackwardUtf8(const u8 (&utf8)[4]) noexcept {

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[3])) {

        return 1;
    }

    if ((utf8[2] & 0b11100000) == 0b11000000) {

        bool isOk = (utf8[3] & 0b11000000) == 0b10000000;

        const u32 utf32 =
            (utf8[2] & 0b00011111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        return isOk && utf32 >= 0x0080
            ? 2
            : 0;
    }

    if ((utf8[1] & 0b11110000) == 0b11100000) {

        bool isOk =
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        const u32 utf32 =
            (utf8[1] & 0b00001111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        return isOk && ((utf32 >= 0x0800 && utf32 <= 0xD7FF) || (utf32 >= 0xE000))
            ? 3
            : 0;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        const u32 utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        return isOk && utf32 >= 0x10000 && utf32 <= 0x10FFFF
            ? 4
            : 0;
    }

    return 0;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;

    maskIdx = (utf8[2] & 0b11100000) == 0b11000000
        ? 1
        : maskIdx;

    maskIdx = (utf8[1] & 0b11110000) == 0b11100000
        ? 2
        : maskIdx;

    maskIdx = (utf8[0] & 0b11111000) == 0b11110000
        ? 3
        : maskIdx;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);

    bool isOk = (asU32 & UTF8_VALIDATION_MASK[maskIdx]) == UTF8_MASK1[maskIdx];
    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    const u32 utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    isOk = maskIdx == 1 ? isOk && (utf32 >= 0x0080) : isOk;
    isOk = maskIdx == 2 ? isOk && (((utf32 + 0xFFFF2000) ^ 0xFFFF2001) - 0x7FF <= 0xFFFFF7FF - 0x7FF) : isOk;
    isOk = maskIdx == 3 ? isOk && (utf32 >= 0x10000 && utf32 <= 0x10FFFF) : isOk;

    return isOk ? maskIdx + 1 : 0;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE usize BackwardUtf8IgnoreErrors(const u8 (&utf8)[4]) noexcept {

    usize maskIdx = 0;

    maskIdx = (utf8[2] & 0b11100000) == 0b11000000
        ? 1
        : maskIdx;

    maskIdx = (utf8[1] & 0b11110000) == 0b11100000
        ? 2
        : maskIdx;

    maskIdx = (utf8[0] & 0b11111000) == 0b11110000
        ? 3
        : maskIdx;

    return maskIdx + 1;
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsValidUtf8(const u8 (&utf8)[4]) noexcept {

    return ForwardUtf8(utf8) != 0;
}

ALT_NODISCARD ALT_FORCE_INLINE usize LengthUtf8ToUtf16(const u8 (&utf8)[4]) noexcept {

    // Table 3-6

    return 1 + (utf8[0] >= 0b11110000 ? 1 : 0);
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf8ToUtf32IgnoreErrorsBackward(const u8 (&utf8)[4], u32& utf32) noexcept {

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[3])) {

        utf32 = static_cast<u32>(utf8[3]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[2] & 0b11100000) == 0b11000000) {

        utf32 =
            (utf8[2] & 0b00011111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        Result result;
        result.advance = 2;
        result.consumed = 1;

        return result;
    }

    if ((utf8[1] & 0b11110000) == 0b11100000) {

        utf32 =
            (utf8[1] & 0b00001111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        Result result;
        result.advance = 3;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        Result result;
        result.advance = 4;
        result.consumed = 1;

        return result;
    }

    // just ignore
    Result result;
    result.advance = 1;
    result.consumed = 1;

    return result;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;

    maskIdx = (utf8[2] & 0b11100000) == 0b11000000
        ? 1
        : maskIdx;

    maskIdx = (utf8[1] & 0b11110000) == 0b11100000
        ? 2
        : maskIdx;

    maskIdx = (utf8[0] & 0b11111000) == 0b11110000
        ? 3
        : maskIdx;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);

    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    Result result;
    result.advance = maskIdx + 1;
    result.consumed = 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf8ToUtf32Backward(const u8 (&utf8)[4], u32& utf32) noexcept {

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[3])) {

        utf32 = static_cast<u32>(utf8[3]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[2] & 0b11100000) == 0b11000000) {

        bool isOk = (utf8[3] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[2] & 0b00011111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x0080;

        Result result;
        result.advance = isOk ? 2 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    if ((utf8[1] & 0b11110000) == 0b11100000) {

        bool isOk =
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[1] & 0b00001111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && ((utf32 >= 0x0800 && utf32 <= 0xD7FF) || (utf32 >= 0xE000));

        Result result;
        result.advance = isOk ? 3 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x10000 && utf32 <= 0x10FFFF;

        Result result;
        result.advance = isOk ? 4 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    Result result;
    result.advance = 1;
    result.consumed = 0;

    return result;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;

    maskIdx = (utf8[2] & 0b11100000) == 0b11000000
        ? 1
        : maskIdx;

    maskIdx = (utf8[1] & 0b11110000) == 0b11100000
        ? 2
        : maskIdx;

    maskIdx = (utf8[0] & 0b11111000) == 0b11110000
        ? 3
        : maskIdx;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);

    bool isOk = (asU32 & UTF8_VALIDATION_MASK[maskIdx]) == UTF8_MASK1[maskIdx];
    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    isOk = maskIdx == 1 ? isOk && (utf32 >= 0x0080) : isOk;
    isOk = maskIdx == 2 ? isOk && (((utf32 + 0xFFFF2000) ^ 0xFFFF2001) - 0x7FF <= 0xFFFFF7FF - 0x7FF) : isOk;
    isOk = maskIdx == 3 ? isOk && (utf32 >= 0x10000 && utf32 <= 0x10FFFF) : isOk;

    Result result;
    result.advance = isOk ? maskIdx + 1 : 1;
    result.consumed = isOk ? 1 : 0;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf8ToUtf32ReplaceBackward(
    const u8 (&utf8)[4], u32& utf32, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[3])) {

        utf32 = static_cast<u32>(utf8[3]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[2] & 0b11100000) == 0b11000000) {

        bool isOk = (utf8[3] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[2] & 0b00011111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x0080;
        utf32 = isOk ? utf32 : replaceUtf32;

        Result result;
        result.advance = isOk ? 2 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    if ((utf8[1] & 0b11110000) == 0b11100000) {

        bool isOk =
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[1] & 0b00001111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && ((utf32 >= 0x0800 && utf32 <= 0xD7FF) || (utf32 >= 0xE000));
        utf32 = isOk ? utf32 : replaceUtf32;

        Result result;
        result.advance = isOk ? 3 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x10000 && utf32 <= 0x10FFFF;
        utf32 = isOk ? utf32 : replaceUtf32;

        Result result;
        result.advance = isOk ? 4 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    utf32 = replaceUtf32;

    Result result;
    result.advance = 1;
    result.consumed = 1;

    return result;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;

    maskIdx = (utf8[2] & 0b11100000) == 0b11000000
        ? 1
        : maskIdx;

    maskIdx = (utf8[1] & 0b11110000) == 0b11100000
        ? 2
        : maskIdx;

    maskIdx = (utf8[0] & 0b11111000) == 0b11110000
        ? 3
        : maskIdx;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);

    bool isOk = (asU32 & UTF8_VALIDATION_MASK[maskIdx]) == UTF8_MASK1[maskIdx];
    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    isOk = maskIdx == 1 ? isOk && (utf32 >= 0x0080) : isOk;
    isOk = maskIdx == 2 ? isOk && (((utf32 + 0xFFFF2000) ^ 0xFFFF2001) - 0x7FF <= 0xFFFFF7FF - 0x7FF) : isOk;
    isOk = maskIdx == 3 ? isOk && (utf32 >= 0x10000 && utf32 <= 0x10FFFF) : isOk;

    utf32 = isOk ? utf32 : replaceUtf32;

    Result result;
    result.advance = isOk ? maskIdx + 1 : 1;
    result.consumed = 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf8ToUtf32IgnoreErrors(const u8 (&utf8)[4], u32& utf32) noexcept {

    // Table 3-6

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[0])) {

        utf32 = static_cast<u32>(utf8[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11100000) == 0b11000000) {

        utf32 =
            (utf8[0] & 0b00011111) << 6 |
            (utf8[1] & 0b00111111)
        ;

        Result result;
        result.advance = 2;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11110000) == 0b11100000) {

        utf32 =
            (utf8[0] & 0b00001111) << 12 |
            (utf8[1] & 0b00111111) << 6 |
            (utf8[2] & 0b00111111)
        ;

        Result result;
        result.advance = 3;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        Result result;
        result.advance = 4;
        result.consumed = 1;

        return result;
    }

    // just ignore
    Result result;
    result.advance = 1;
    result.consumed = 1;

    return result;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;

    const u8 utf80Checked = utf8[0] + 0b1000;
    maskIdx += utf80Checked >= 24 * 8 + 0b1000 ? 1 : 0;
    maskIdx += utf80Checked >= 28 * 8 + 0b1000 ? 1 : 0;
    maskIdx += utf80Checked >= 30 * 8 + 0b1000 ? 1 : 0;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);
    asU32 = asU32 << (24 - maskIdx * 8);

    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    Result result;
    result.advance = maskIdx + 1;
    result.consumed = 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf8ToUtf32(const u8 (&utf8)[4], u32& utf32) noexcept {

    // Table 3-6
    // Table 3-7

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[0])) {

        utf32 = static_cast<u32>(utf8[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11100000) == 0b11000000) {

        bool isOk = (utf8[1] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[0] & 0b00011111) << 6 |
            (utf8[1] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x0080;

        Result result;
        result.advance = isOk ? 2 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    if ((utf8[0] & 0b11110000) == 0b11100000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[0] & 0b00001111) << 12 |
            (utf8[1] & 0b00111111) << 6 |
            (utf8[2] & 0b00111111)
        ;

        isOk = isOk && ((utf32 >= 0x0800 && utf32 <= 0xD7FF) || (utf32 >= 0xE000));

        Result result;
        result.advance = isOk ? 3 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x10000 && utf32 <= 0x10FFFF;

        Result result;
        result.advance = isOk ? 4 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    Result result;
    result.advance = 1;
    result.consumed = 0;

    return result;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;
    maskIdx += utf8[0] >= 24 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 28 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 30 * 8 ? 1 : 0;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);
    asU32 = asU32 << (24 - maskIdx * 8);

    bool isOk = (asU32 & UTF8_VALIDATION_MASK[maskIdx]) == UTF8_MASK1[maskIdx];
    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    isOk = maskIdx == 1 ? isOk && (utf32 >= 0x0080) : isOk;
    isOk = maskIdx == 2 ? isOk && (((utf32 + 0xFFFF2000) ^ 0xFFFF2001) - 0x7FF <= 0xFFFFF7FF - 0x7FF) : isOk;
    isOk = maskIdx == 3 ? isOk && (utf32 >= 0x10000 && utf32 <= 0x10FFFF) : isOk;

    Result result;
    result.advance = isOk ? maskIdx + 1 : 1;
    result.consumed = isOk ? 1 : 0;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf8ToUtf32Replace(
    const u8 (&utf8)[4], u32& utf32, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    // Table 3-6
    // Table 3-7

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[0])) {

        utf32 = static_cast<u32>(utf8[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11100000) == 0b11000000) {

        bool isOk = (utf8[1] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[0] & 0b00011111) << 6 |
            (utf8[1] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x0080;
        utf32 = isOk ? utf32 : replaceUtf32;

        Result result;
        result.advance = isOk ? 2 : 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11110000) == 0b11100000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[0] & 0b00001111) << 12 |
            (utf8[1] & 0b00111111) << 6 |
            (utf8[2] & 0b00111111)
        ;

        isOk = isOk && ((utf32 >= 0x0800 && utf32 <= 0xD7FF) || (utf32 >= 0xE000));
        utf32 = isOk ? utf32 : replaceUtf32;

        Result result;
        result.advance = isOk ? 3 : 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x10000 && utf32 <= 0x10FFFF;
        utf32 = isOk ? utf32 : replaceUtf32;

        Result result;
        result.advance = isOk ? 4 : 1;
        result.consumed = 1;

        return result;
    }

    utf32 = replaceUtf32;

    Result result;
    result.advance = 1;
    result.consumed = 1;

    return result;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;
    maskIdx += utf8[0] >= 24 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 28 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 30 * 8 ? 1 : 0;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);
    asU32 = asU32 << (24 - maskIdx * 8);

    bool isOk = (asU32 & UTF8_VALIDATION_MASK[maskIdx]) == UTF8_MASK1[maskIdx];
    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    isOk = maskIdx == 1 ? isOk && (utf32 >= 0x0080) : isOk;
    isOk = maskIdx == 2 ? isOk && (((utf32 + 0xFFFF2000) ^ 0xFFFF2001) - 0x7FF <= 0xFFFFF7FF - 0x7FF) : isOk;
    isOk = maskIdx == 3 ? isOk && (utf32 >= 0x10000 && utf32 <= 0x10FFFF) : isOk;

    utf32 = isOk ? utf32 : replaceUtf32;

    Result result;
    result.advance = isOk ? maskIdx + 1 : 1;
    result.consumed = 1;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf8ToUtf16IgnoreErrors(const u8 (&utf8)[4], u16 (&utf16)[2]) noexcept {

    // Table 3-6
    // Table 3-7

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[0])) {

        utf16[0] = static_cast<u16>(utf8[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11100000) == 0b11000000) {

        utf16[0] =
            (utf8[0] & 0b00011111) << 6 |
            (utf8[1] & 0b00111111)
        ;

        Result result;
        result.advance = 2;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11110000) == 0b11100000) {

        utf16[0] =
            (utf8[0] & 0b00001111) << 12 |
            (utf8[1] & 0b00111111) << 6 |
            (utf8[2] & 0b00111111)
        ;

        Result result;
        result.advance = 3;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        u32 utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        utf32 -= 0x10000;

        utf16[0] = (static_cast<u16>(utf32 >> 10) & 0b0000001111111111) | 0xD800;
        utf16[1] = (static_cast<u16>(utf32)       & 0b0000001111111111) | 0xDC00;

        Result result;
        result.advance = 4;
        result.consumed = 2;

        return result;
    }

    // just ignore
    Result result;
    result.advance = 1;
    result.consumed = 1;

    return result;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;

    const u8 utf80Checked = utf8[0] + 0b1000;
    maskIdx += utf80Checked >= 24 * 8 + 0b1000 ? 1 : 0;
    maskIdx += utf80Checked >= 28 * 8 + 0b1000 ? 1 : 0;
    maskIdx += utf80Checked >= 30 * 8 + 0b1000 ? 1 : 0;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);
    asU32 = asU32 << (24 - maskIdx * 8);

    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    u32 utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    utf16[0] = static_cast<u16>(utf32);

    if (maskIdx == 3) {

        utf32 -= 0x10000;

        utf16[0] = static_cast<u16>(utf32 >> 10);
        utf16[1] = static_cast<u16>(utf32);

        u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
        asU32 = (asU32 & UTF16_MASK0) | UTF16_MASK1;

        Memory::TransmuteInto<u32, u16[2]>(asU32, utf16);
    }

    const usize consumed = maskIdx == 3
        ? 2
        : 1;

    Result result;
    result.advance = maskIdx + 1;
    result.consumed = consumed;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf8ToUtf16(const u8 (&utf8)[4], u16 (&utf16)[2]) noexcept {

    // Table 3-6
    // Table 3-7

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[0])) {

        utf16[0] = static_cast<u16>(utf8[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11100000) == 0b11000000) {

        bool isOk = (utf8[1] & 0b11000000) == 0b10000000;

        utf16[0] =
            (utf8[0] & 0b00011111) << 6 |
            (utf8[1] & 0b00111111)
        ;

        isOk = isOk && utf16[0] >= 0x0080;

        Result result;
        result.advance = isOk ? 2 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    if ((utf8[0] & 0b11110000) == 0b11100000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000;

        utf16[0] =
            (utf8[0] & 0b00001111) << 12 |
            (utf8[1] & 0b00111111) << 6 |
            (utf8[2] & 0b00111111)
        ;

        isOk = isOk && ((utf16[0] >= 0x0800 && utf16[0] <= 0xD7FF) || (utf16[0] >= 0xE000));

        Result result;
        result.advance = isOk ? 3 : 1;
        result.consumed = isOk ? 1 : 0;

        return result;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        u32 utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x10000 && utf32 <= 0x10FFFF;

        utf32 -= 0x10000;

        utf16[0] = (static_cast<u16>(utf32 >> 10) & 0b0000001111111111) | 0xD800;
        utf16[1] = (static_cast<u16>(utf32)       & 0b0000001111111111) | 0xDC00;

        Result result;
        result.advance = isOk ? 4 : 1;
        result.consumed = isOk ? 2 : 0;

        return result;
    }

    Result result;
    result.advance = 1;
    result.consumed = 0;

    return result;
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;
    maskIdx += utf8[0] >= 24 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 28 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 30 * 8 ? 1 : 0;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);
    asU32 = asU32 << (24 - maskIdx * 8);

    bool isOk = (asU32 & UTF8_VALIDATION_MASK[maskIdx]) == UTF8_MASK1[maskIdx];
    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    u32 utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    utf16[0] = static_cast<u16>(utf32);

    isOk = maskIdx == 1 ? isOk && (utf32 >= 0x0080) : isOk;
    isOk = maskIdx == 2 ? isOk && (((utf32 + 0xFFFF2000) ^ 0xFFFF2001) - 0x7FF <= 0xFFFFF7FF - 0x7FF) : isOk;

    utf32 -= 0x10000;
    isOk = maskIdx == 3 ? isOk && (utf32 <= 0xFFFFF) : isOk;

    if (maskIdx == 3 && isOk) {

        utf16[0] = static_cast<u16>(utf32 >> 10);
        utf16[1] = static_cast<u16>(utf32);

        u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
        asU32 = (asU32 & UTF16_MASK0) | UTF16_MASK1;

        Memory::TransmuteInto<u32, u16[2]>(asU32, utf16);
    }

    const usize consumed = maskIdx == 3
        ? 2
        : 1;

    Result result;
    result.advance = isOk ? maskIdx + 1 : 1;
    result.consumed = isOk ? consumed : 0;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

ALT_NODISCARD ALT_FORCE_INLINE Result Utf8ToUtf16Replace(
    const u8 (&utf8)[4], u16 (&utf16)[2], const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    // Table 3-6
    // Table 3-7

#ifdef ALT_USE_UTF_NON_BRANCHLESS

    if (IsAscii(utf8[0])) {

        utf16[0] = static_cast<u16>(utf8[0]);

        Result result;
        result.advance = 1;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11100000) == 0b11000000) {

        bool isOk = (utf8[1] & 0b11000000) == 0b10000000;

        utf16[0] =
            (utf8[0] & 0b00011111) << 6 |
            (utf8[1] & 0b00111111)
        ;

        isOk = isOk && utf16[0] >= 0x0080;

        if (!isOk) {

            return _32::Utf32ToUtf16(replaceUtf32, utf16);
        }

        Result result;
        result.advance = 2;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11110000) == 0b11100000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000;

        utf16[0] =
            (utf8[0] & 0b00001111) << 12 |
            (utf8[1] & 0b00111111) << 6 |
            (utf8[2] & 0b00111111)
        ;

        isOk = isOk && ((utf16[0] >= 0x0800 && utf16[0] <= 0xD7FF) || (utf16[0] >= 0xE000));

        if (!isOk) {

            return _32::Utf32ToUtf16(replaceUtf32, utf16);
        }

        Result result;
        result.advance = 3;
        result.consumed = 1;

        return result;
    }

    if ((utf8[0] & 0b11111000) == 0b11110000) {

        bool isOk =
            (utf8[1] & 0b11000000) == 0b10000000 &&
            (utf8[2] & 0b11000000) == 0b10000000 &&
            (utf8[3] & 0b11000000) == 0b10000000;

        u32 utf32 =
            (utf8[0] & 0b00000111) << 18 |
            (utf8[1] & 0b00111111) << 12 |
            (utf8[2] & 0b00111111) << 6 |
            (utf8[3] & 0b00111111)
        ;

        isOk = isOk && utf32 >= 0x10000 && utf32 <= 0x10FFFF;

        if (!isOk) {

            return _32::Utf32ToUtf16(replaceUtf32, utf16);
        }

        utf32 -= 0x10000;

        utf16[0] = (static_cast<u16>(utf32 >> 10) & 0b0000001111111111) | 0xD800;
        utf16[1] = (static_cast<u16>(utf32)       & 0b0000001111111111) | 0xDC00;

        Result result;
        result.advance = isOk ? 4 : 1;
        result.consumed = isOk ? 2 : 0;

        return result;
    }

    return _32::Utf32ToUtf16IgnoreErrors(replaceUtf32, utf16);
#else

    using namespace _TableDetails;
    using namespace _32::_TableDetails;

    usize maskIdx = 0;
    maskIdx += utf8[0] >= 24 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 28 * 8 ? 1 : 0;
    maskIdx += utf8[0] >= 30 * 8 ? 1 : 0;

    u32 asU32 = Memory::Transmute<u8[4], u32>(utf8);
    asU32 = asU32 << (24 - maskIdx * 8);

    bool isOk = (asU32 & UTF8_VALIDATION_MASK[maskIdx]) == UTF8_MASK1[maskIdx];
    asU32 = asU32 & UTF8_MASK0[maskIdx];

    u8 utf8_[4];
    Memory::TransmuteInto<u32, u8[4]>(asU32, utf8_);
    u32 utf32 =
        utf8_[0] << 18 |
        utf8_[1] << 12 |
        utf8_[2] << 6 |
        utf8_[3]
    ;

    utf16[0] = static_cast<u16>(utf32);

    isOk = maskIdx == 1 ? isOk && (utf32 >= 0x0080) : isOk;
    isOk = maskIdx == 2 ? isOk && (((utf32 + 0xFFFF2000) ^ 0xFFFF2001) - 0x7FF <= 0xFFFFF7FF - 0x7FF) : isOk;

    utf32 -= 0x10000;
    isOk = maskIdx == 3 ? isOk && (utf32 <= 0xFFFFF) : isOk;

    if (maskIdx == 3 && isOk) {

        utf16[0] = static_cast<u16>(utf32 >> 10);
        utf16[1] = static_cast<u16>(utf32);

        u32 asU32 = Memory::Transmute<u16[2], u32>(utf16);
        asU32 = (asU32 & UTF16_MASK0) | UTF16_MASK1;

        Memory::TransmuteInto<u32, u16[2]>(asU32, utf16);
    }

    if (!isOk) {

        return _32::Utf32ToUtf16IgnoreErrors(replaceUtf32, utf16);
    }

    const usize consumed = maskIdx == 3
        ? 2
        : 1;

    Result result;
    result.advance = maskIdx + 1;
    result.consumed = consumed;

    return result;
#endif // ALT_USE_UTF_NON_BRANCHLESS
}

} // namespace _8
} // namespace Utf
} // namespace Alt

#endif // __ALT_STD_UTF8__