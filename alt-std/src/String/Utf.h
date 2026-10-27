#pragma once

#ifndef __ALT_STD_UTF__
#define __ALT_STD_UTF__

#include "Impl/Utf/UtfCore.h"

#include "Impl/Utf/Utf8.h"
#include "Impl/Utf/Utf16.h"
#include "Impl/Utf/Utf32.h"

#include "../Utils/Slice.h"
#include "../Utils/EnumClassBitFlagsImpl.h"

/*
* see. ALT_USE_UTF_NON_BRANCHLESS
*/
namespace Alt {
namespace Utf {

ALT_NODISCARD inline usize ValidateUtf8(const Slice<const u8> bytes) {

    usize advance = 4;

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    while (advance != 0 && pBytes < pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        advance = _8::ForwardUtf8(utf8);
        pBytes += advance;
    }

    return static_cast<usize>(pBytes - bytes.Data());
}

ALT_NODISCARD inline usize ValidateUtf8NullTermAware(const Slice<const u8> bytes) {

    usize advance = 4;

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    while (advance != 0 && pBytes < pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        advance = _8::ForwardUtf8(utf8);
        advance = utf8[0] == 0 ? 0 : advance;

        pBytes += advance;
    }

    return static_cast<usize>(pBytes - bytes.Data());
}

ALT_NODISCARD inline usize ValidateUtf16(const Slice<const u16> bytes) {

    usize advance = 2;

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    while (advance != 0 && pBytes < pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        advance = _16::ForwardUtf16(utf16);
        pBytes += advance;
    }

    return static_cast<usize>(pBytes - bytes.Data());
}

ALT_NODISCARD inline usize ValidateUtf16NullTermAware(const Slice<const u16> bytes) {

    usize advance = 2;

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    while (advance != 0 && pBytes < pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        advance = _16::ForwardUtf16(utf16);
        advance = utf16[0] == 0 ? 0 : advance;

        pBytes += advance;
    }

    return static_cast<usize>(pBytes - bytes.Data());
}

ALT_NODISCARD inline usize ValidateUtf32(const Slice<const u32> utf32s) {

    bool isValidUtf32 = true;

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    while (isValidUtf32 && pUtf32 < pUtf32End) {

        isValidUtf32 = _32::IsValidUtf32(*pUtf32);
        pUtf32 += isValidUtf32 ? 1 : 0;
    }

    return static_cast<usize>(pUtf32 - utf32s.Data());
}

ALT_NODISCARD inline usize ValidateUtf32NullTermAware(const Slice<const u32> utf32s) {

    bool isValidUtf32 = true;

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    while (isValidUtf32 && pUtf32 < pUtf32End) {

        isValidUtf32 = _32::IsValidUtf32(*pUtf32);
        isValidUtf32 &= *pUtf32 != 0;

        pUtf32 += isValidUtf32 ? 1 : 0;
    }

    return static_cast<usize>(pUtf32 - utf32s.Data());
}

namespace _UtfDetails {

struct Bom { u8 bom[4]; };

constexpr static Bom bomTable[5]{
    Bom{ 0xEF, 0xBB, 0xBF, 0x00 }, // Utf8
    Bom{ 0xFF, 0xFE, 0x00, 0x00 }, // Utf16 le
    Bom{ 0xFF, 0xFE, 0x00, 0x00 }, // Utf32 le
    Bom{ 0xFE, 0xFF, 0x00, 0x00 }, // Utf16 be
    Bom{ 0x00, 0x00, 0xFE, 0xFF }, // Utf32 be
};

constexpr static usize bytesConsumedByBomTable[6]{
    0,
    3,
    2,
    4,
    2,
    4,
};

} // namespace _UtfDetails

ALT_NODISCARD ALT_FORCE_INLINE Slice<const u8> EncodingToBom(const Encoding encoding, const bool isLittleEndian = Platform::IsLittleEndian()) noexcept {

    usize type = static_cast<usize>(encoding);
    const usize offset = type < 2 || isLittleEndian ? 0 : 2;
    type += offset;

    return Slice<const u8>{ _UtfDetails::bomTable[type - (type == 0 ? 0 : 1)].bom, _UtfDetails::bytesConsumedByBomTable[type] };
}

struct DetectEncodingByBomResult {

    Encoding encoding = Encoding::Unknown;
    usize bytesConsumedByBom = 0;
    bool isLittleEndian = true; // if `isLittleEndian != Alt::Platform::IsLittleEndian()` - manually swap bytes
};
ALT_NODISCARD ALT_FORCE_INLINE DetectEncodingByBomResult DetectEncodingByBom(const Slice<const u8> bytes_) noexcept {

    constexpr static _UtfDetails::Bom bomMaskTable[5]{
        _UtfDetails::Bom{ 0xFF, 0xFF, 0xFF, 0x00 },
        _UtfDetails::Bom{ 0xFF, 0xFF, 0x00, 0x00 },
        _UtfDetails::Bom{ 0xFF, 0xFF, 0xFF, 0xFF },
        _UtfDetails::Bom{ 0xFF, 0xFF, 0x00, 0x00 },
        _UtfDetails::Bom{ 0xFF, 0xFF, 0xFF, 0xFF },
    };

    constexpr static Encoding encodingTable[6]{
        Encoding::Unknown,
        Encoding::Utf8,
        Encoding::Utf16, // le
        Encoding::Utf32, // le
        Encoding::Utf16, // be
        Encoding::Utf32, // be
    };

    const u8 bytes[4]{
        0 < bytes_.GetSize() ? bytes_[0] : static_cast<u8>(0x01),
        1 < bytes_.GetSize() ? bytes_[1] : static_cast<u8>(0x01),
        2 < bytes_.GetSize() ? bytes_[2] : static_cast<u8>(0x01),
        3 < bytes_.GetSize() ? bytes_[3] : static_cast<u8>(0x01),
    };

    usize resultIdx = 0;

    resultIdx =
            (bytes[0] & bomMaskTable[0].bom[0]) == _UtfDetails::bomTable[0].bom[0] &&
            (bytes[1] & bomMaskTable[0].bom[1]) == _UtfDetails::bomTable[0].bom[1] &&
            (bytes[2] & bomMaskTable[0].bom[2]) == _UtfDetails::bomTable[0].bom[2] &&
            (bytes[3] & bomMaskTable[0].bom[3]) == _UtfDetails::bomTable[0].bom[3]
        ? 1
        : resultIdx;

    resultIdx =
            (bytes[0] & bomMaskTable[1].bom[0]) == _UtfDetails::bomTable[1].bom[0] &&
            (bytes[1] & bomMaskTable[1].bom[1]) == _UtfDetails::bomTable[1].bom[1] &&
            (bytes[2] & bomMaskTable[1].bom[2]) == _UtfDetails::bomTable[1].bom[2] &&
            (bytes[3] & bomMaskTable[1].bom[3]) == _UtfDetails::bomTable[1].bom[3]
        ? 2
        : resultIdx;

    resultIdx =
            (bytes[0] & bomMaskTable[2].bom[0]) == _UtfDetails::bomTable[2].bom[0] &&
            (bytes[1] & bomMaskTable[2].bom[1]) == _UtfDetails::bomTable[2].bom[1] &&
            (bytes[2] & bomMaskTable[2].bom[2]) == _UtfDetails::bomTable[2].bom[2] &&
            (bytes[3] & bomMaskTable[2].bom[3]) == _UtfDetails::bomTable[2].bom[3]
        ? 3
        : resultIdx;

    resultIdx =
            (bytes[0] & bomMaskTable[3].bom[0]) == _UtfDetails::bomTable[3].bom[0] &&
            (bytes[1] & bomMaskTable[3].bom[1]) == _UtfDetails::bomTable[3].bom[1] &&
            (bytes[2] & bomMaskTable[3].bom[2]) == _UtfDetails::bomTable[3].bom[2] &&
            (bytes[3] & bomMaskTable[3].bom[3]) == _UtfDetails::bomTable[3].bom[3]
        ? 4
        : resultIdx;

    resultIdx =
            (bytes[0] & bomMaskTable[4].bom[0]) == _UtfDetails::bomTable[4].bom[0] &&
            (bytes[1] & bomMaskTable[4].bom[1]) == _UtfDetails::bomTable[4].bom[1] &&
            (bytes[2] & bomMaskTable[4].bom[2]) == _UtfDetails::bomTable[4].bom[2] &&
            (bytes[3] & bomMaskTable[4].bom[3]) == _UtfDetails::bomTable[4].bom[3]
        ? 5
        : resultIdx;

    DetectEncodingByBomResult result;
    result.encoding = encodingTable[resultIdx];
    result.bytesConsumedByBom = _UtfDetails::bytesConsumedByBomTable[resultIdx];
    result.isLittleEndian = resultIdx <= 3 ? true : false;

    return result;
}

// see. Encoding
enum class GuessedEncoding {

    Unknown = 0,

    Utf8  = 1 << (static_cast<usize>(Encoding::Utf8) - 1),
    Utf16 = 1 << (static_cast<usize>(Encoding::Utf16) - 1),
    Utf32 = 1 << (static_cast<usize>(Encoding::Utf32) - 1),
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(GuessedEncoding);

namespace _UtfDetails {

ALT_NODISCARD ALT_FORCE_INLINE GuessedEncoding EncodingToGuessedEncoding(const Encoding encoding) noexcept {

    return encoding == Encoding::Unknown
        ? GuessedEncoding::Unknown
        : static_cast<GuessedEncoding>(1 << (static_cast<usize>(encoding) - 1));
}

} // namespace _UtfDetails

ALT_NODISCARD ALT_FORCE_INLINE Encoding GuessedEncodingToFirstEncoding(const GuessedEncoding guessed) noexcept {

    if (BitFlag::IsSetEnumFlags(guessed, GuessedEncoding::Utf8)) {
        return Encoding::Utf8;
    }

    if (BitFlag::IsSetEnumFlags(guessed, GuessedEncoding::Utf16)) {
        return Encoding::Utf16;
    }

    if (BitFlag::IsSetEnumFlags(guessed, GuessedEncoding::Utf32)) {
        return Encoding::Utf32;
    }

    return Encoding::Unknown;
}

struct GuessEncodingResult {

    GuessedEncoding guessedEncoding = GuessedEncoding::Unknown;
    usize bytesConsumedByBom = 0; // 0 means no bom
    bool isLittleEndian = Platform::IsLittleEndian(); // if `isLittleEndian != Alt::Platform::IsLittleEndian()` - manually swap bytes
};

ALT_NODISCARD inline GuessEncodingResult GuessEncoding(const Slice<const u8> bytes) {

    const DetectEncodingByBomResult resultByBom = DetectEncodingByBom(bytes);
    if (resultByBom.encoding != Encoding::Unknown) {

        GuessEncodingResult result;
        result.guessedEncoding = _UtfDetails::EncodingToGuessedEncoding(resultByBom.encoding);
        result.bytesConsumedByBom = resultByBom.bytesConsumedByBom;
        result.isLittleEndian = resultByBom.isLittleEndian;

        return result;
    }

    GuessedEncoding encoding = GuessedEncoding::Unknown;

    if (ValidateUtf8(bytes) == bytes.GetSize()) {

        encoding |= GuessedEncoding::Utf8;
    }

    if (bytes.GetSize() % sizeof(u16) == 0 && Memory::Align(bytes.Data(), alignof(u16)) == 0) {

        const Slice<const u16> bytesU16{
            reinterpret_cast<const u16*>(bytes.Data()),
            reinterpret_cast<const u16*>(bytes.DataEnd()),
        };

        if (ValidateUtf16(bytesU16) == bytesU16.GetSize()) {

            encoding |= GuessedEncoding::Utf16;
        }
    }

    if (bytes.GetSize() % sizeof(u32) == 0 && Memory::Align(bytes.Data(), alignof(u32)) == 0) {

        const Slice<const u32> utf32s{
            reinterpret_cast<const u32*>(bytes.Data()),
            reinterpret_cast<const u32*>(bytes.DataEnd()),
        };

        if (ValidateUtf32(utf32s) == utf32s.GetSize()) {

            encoding |= GuessedEncoding::Utf32;
        }
    }

    GuessEncodingResult result{};
    result.guessedEncoding = encoding;

    return result;
}

ALT_NODISCARD inline GuessEncodingResult GuessEncodingNullTermAware(const Slice<const u8> bytes) {

    const DetectEncodingByBomResult resultByBom = DetectEncodingByBom(bytes);
    if (resultByBom.encoding != Encoding::Unknown) {

        GuessEncodingResult result;
        result.guessedEncoding = _UtfDetails::EncodingToGuessedEncoding(resultByBom.encoding);
        result.bytesConsumedByBom = resultByBom.bytesConsumedByBom;
        result.isLittleEndian = resultByBom.isLittleEndian;

        return result;
    }

    GuessedEncoding encoding = GuessedEncoding::Unknown;

    if (ValidateUtf8NullTermAware(bytes) == bytes.GetSize()) {

        encoding |= GuessedEncoding::Utf8;
    }

    if (bytes.GetSize() % sizeof(u16) == 0 && Memory::Align(bytes.Data(), alignof(u16)) == 0) {

        const Slice<const u16> bytesU16{
            reinterpret_cast<const u16*>(bytes.Data()),
            reinterpret_cast<const u16*>(bytes.DataEnd()),
        };

        if (ValidateUtf16NullTermAware(bytesU16) == bytesU16.GetSize()) {

            encoding |= GuessedEncoding::Utf16;
        }
    }

    if (bytes.GetSize() % sizeof(u32) == 0 && Memory::Align(bytes.Data(), alignof(u32)) == 0) {

        const Slice<const u32> utf32s{
            reinterpret_cast<const u32*>(bytes.Data()),
            reinterpret_cast<const u32*>(bytes.DataEnd()),
        };

        if (ValidateUtf32NullTermAware(utf32s) == utf32s.GetSize()) {

            encoding |= GuessedEncoding::Utf32;
        }
    }

    GuessEncodingResult result{};
    result.guessedEncoding = encoding;

    return result;
}

ALT_NODISCARD inline usize LengthUtf32ToUtf8IgnoreErrors(const Slice<const u32> utf32s) noexcept {

    usize size = 0;

    const u32* pUtf32End = utf32s.DataEnd();
    for (const u32* pUtf32 = utf32s.Data(); pUtf32 < pUtf32End; ++pUtf32) {

        size += _32::LengthUtf32ToUtf8(*pUtf32);
    }

    return size;
}

ALT_NODISCARD inline Result LengthUtf32ToUtf8(const Slice<const u32> utf32s) noexcept {

    Result result{};

    usize size = 0;
    bool isValidUtf32 = true;

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    while (isValidUtf32 && pUtf32 != pUtf32End) {

        isValidUtf32 = _32::IsValidUtf32(*pUtf32);
        size = _32::LengthUtf32ToUtf8(*pUtf32);

        result.consumed += size;
        pUtf32 += isValidUtf32 ? 1 : 0;
    }

    result.consumed -= isValidUtf32 ? 0 : size;

    result.advance = static_cast<usize>(pUtf32 - utf32s.Data());

    return result;
}

ALT_NODISCARD inline usize LengthUtf32ToUtf8Replace(
    const Slice<const u32> utf32s, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    usize resultSize = 0;

    const u32* pUtf32End = utf32s.DataEnd();
    const usize replaceSize = _32::LengthUtf32ToUtf8(replaceUtf32);

    for (const u32* pUtf32 = utf32s.Data(); pUtf32 != pUtf32End; ++pUtf32) {

        const usize size = _32::LengthUtf32ToUtf8(*pUtf32);

        resultSize += _32::IsValidUtf32(*pUtf32)
            ? size
            : replaceSize;
    }

    return resultSize;
}

inline void Utf32ToUtf8IgnoreErrors(const Slice<const u32> utf32s, u8* pBytes, usize& capacity) noexcept {

    if (!pBytes) {

        capacity = LengthUtf32ToUtf8IgnoreErrors(utf32s);
        return;
    }

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    bool isOk = true;
    usize consumed = 4;

    while (capacity > 0 && consumed != 0 && pUtf32 != pUtf32End) {

        u8 utf8[4];
        Result result = _32::Utf32ToUtf8IgnoreErrors(*pUtf32, utf8);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytes + 0) = utf8[0];
        if (1 < consumed) *(pBytes + 1) = utf8[1];
        if (2 < consumed) *(pBytes + 2) = utf8[2];
        if (3 < consumed) *(pBytes + 3) = utf8[3];

        pUtf32 += 1;
        pBytes += consumed;
        capacity -= consumed;
    }
}

ALT_NODISCARD inline bool Utf32ToUtf8(const Slice<const u32> utf32s, u8* pBytes, usize& capacity) noexcept {

    if (!pBytes) {

        capacity = LengthUtf32ToUtf8IgnoreErrors(utf32s);
        return true;
    }

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    bool isOk = true;
    usize consumed = 4;

    while (capacity > 0 && consumed != 0 && pUtf32 != pUtf32End) {

        u8 utf8[4];
        Result result = _32::Utf32ToUtf8(*pUtf32, utf8);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytes + 0) = utf8[0];
        if (1 < consumed) *(pBytes + 1) = utf8[1];
        if (2 < consumed) *(pBytes + 2) = utf8[2];
        if (3 < consumed) *(pBytes + 3) = utf8[3];

        pUtf32 += 1;
        pBytes += consumed;
        capacity -= consumed;
    }

    return isOk && consumed != 0;
}

inline void Utf32ToUtf8Replace(
    const Slice<const u32> utf32s, u8* pBytes, usize& capacity, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    if (!pBytes) {

        capacity = LengthUtf32ToUtf8Replace(utf32s, replaceUtf32);
        return;
    }

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    bool isOk = true;
    usize consumed = 4;

    while (capacity > 0 && consumed != 0 && pUtf32 != pUtf32End) {

        u8 utf8[4];
        Result result = _32::Utf32ToUtf8Replace(*pUtf32, utf8, replaceUtf32);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytes + 0) = utf8[0];
        if (1 < consumed) *(pBytes + 1) = utf8[1];
        if (2 < consumed) *(pBytes + 2) = utf8[2];
        if (3 < consumed) *(pBytes + 3) = utf8[3];

        pUtf32 += 1;
        pBytes += consumed;
        capacity -= consumed;
    }
}

ALT_NODISCARD inline usize LengthUtf32ToUtf16IgnoreErrors(const Slice<const u32> utf32s) noexcept {

    usize size = 0;

    const u32* pUtf32End = utf32s.DataEnd();
    for (const u32* pUtf32 = utf32s.Data(); pUtf32 < pUtf32End; ++pUtf32) {

        size += _32::LengthUtf32ToUtf16(*pUtf32);
    }

    return size;
}

ALT_NODISCARD inline Result LengthUtf32ToUtf16(const Slice<const u32> utf32s) noexcept {

    Result result{};

    usize size = 0;
    bool isValidUtf32 = true;

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    while (isValidUtf32 && pUtf32 != pUtf32End) {

        isValidUtf32 = _32::IsValidUtf32(*pUtf32);
        size = _32::LengthUtf32ToUtf16(*pUtf32);

        result.consumed += size;
        pUtf32 += isValidUtf32 ? 1 : 0;
    }

    result.consumed -= isValidUtf32 ? 0 : size;

    result.advance = static_cast<usize>(pUtf32 - utf32s.Data());

    return result;
}

ALT_NODISCARD inline usize LengthUtf32ToUtf16Replace(
    const Slice<const u32> utf32s, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    usize resultSize = 0;

    const u32* pUtf32End = utf32s.DataEnd();
    const usize replaceSize = _32::LengthUtf32ToUtf16(replaceUtf32);

    for (const u32* pUtf32 = utf32s.Data(); pUtf32 != pUtf32End; ++pUtf32) {

        const usize size = _32::LengthUtf32ToUtf16(*pUtf32);

        resultSize += _32::IsValidUtf32(*pUtf32)
            ? size
            : replaceSize;
    }

    return resultSize;
}

inline void Utf32ToUtf16IgnoreErrors(const Slice<const u32> utf32s, u16* pBytes, usize& capacity) noexcept {

    if (!pBytes) {

        capacity = LengthUtf32ToUtf16IgnoreErrors(utf32s);
        return;
    }

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    bool isOk = true;
    usize consumed = 2;

    while (capacity > 0 && consumed != 0 && pUtf32 != pUtf32End) {

        u16 utf16[2];
        Result result = _32::Utf32ToUtf16IgnoreErrors(*pUtf32, utf16);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytes + 0) = utf16[0];
        if (1 < consumed) *(pBytes + 1) = utf16[1];

        pUtf32 += 1;
        pBytes += consumed;
        capacity -= consumed;
    }
}

ALT_NODISCARD inline bool Utf32ToUtf16(const Slice<const u32> utf32s, u16* pBytes, usize& capacity) noexcept {

    if (!pBytes) {

        capacity = LengthUtf32ToUtf16IgnoreErrors(utf32s);
        return true;
    }

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    bool isOk = true;
    usize consumed = 2;

    while (capacity > 0 && consumed != 0 && pUtf32 != pUtf32End) {

        u16 utf16[2];
        Result result = _32::Utf32ToUtf16(*pUtf32, utf16);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytes + 0) = utf16[0];
        if (1 < consumed) *(pBytes + 1) = utf16[1];

        pUtf32 += 1;
        pBytes += consumed;
        capacity -= consumed;
    }

    return isOk && consumed != 0;
}

inline void Utf32ToUtf16Replace(
    const Slice<const u32> utf32s, u16* pBytes, usize& capacity, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    if (!pBytes) {

        capacity = LengthUtf32ToUtf16Replace(utf32s, replaceUtf32);
        return;
    }

    const u32* pUtf32 = utf32s.Data();
    const u32* pUtf32End = utf32s.DataEnd();

    bool isOk = true;
    usize consumed = 2;

    while (capacity > 0 && consumed != 0 && pUtf32 != pUtf32End) {

        u16 utf16[2];
        Result result = _32::Utf32ToUtf16Replace(*pUtf32, utf16, replaceUtf32);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytes + 0) = utf16[0];
        if (1 < consumed) *(pBytes + 1) = utf16[1];

        pUtf32 += 1;
        pBytes += consumed;
        capacity -= consumed;
    }
}

ALT_NODISCARD inline usize LengthUtf16ToUtf32IgnoreErrors(const Slice<const u16> bytes) noexcept {

    usize size = 0;

    const u16* pBytesEnd = bytes.DataEnd();

    for (const u16* pBytes = bytes.Data(); pBytes < pBytesEnd;) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        const usize advance = _16::IsValidUtf16(utf16) && _16::IsSurrogateUtf16(utf16[0])
            ? 2
            : 1;

        size += 1;
        pBytes += advance;
    }

    return size;
}

ALT_NODISCARD inline Result LengthUtf16ToUtf32(const Slice<const u16> bytes) noexcept {

    Result result{};

    usize advance = 0;
    bool isValidUtf16 = true;

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    while (isValidUtf16 && pBytes < pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        isValidUtf16 = _16::IsValidUtf16(utf16);
        advance = isValidUtf16 && _16::IsSurrogateUtf16(utf16[0])
            ? 2
            : 1;

        result.consumed += isValidUtf16 ? 1 : 0;
        pBytes += advance;
    }

    result.advance = static_cast<usize>(pBytes - bytes.Data());
    result.advance -= isValidUtf16 ? 0 : advance;

    return result;
}

ALT_NODISCARD inline usize LengthUtf16ToUtf32Replace(const Slice<const u16> bytes) noexcept {

    usize size = 0;

    const u16* pBytesEnd = bytes.DataEnd();

    for (const u16* pBytes = bytes.Data(); pBytes < pBytesEnd;) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        const usize advance = _16::IsValidUtf16(utf16) && _16::IsSurrogateUtf16(utf16[0])
            ? 2
            : 1;

        size += 1;
        pBytes += advance;
    }

    return size;
}

inline void Utf16ToUtf32IgnoreErrors(const Slice<const u16> bytes, u32* pUtf32, usize& capacity) noexcept {

    if (!pUtf32) {

        capacity = LengthUtf16ToUtf32IgnoreErrors(bytes);
        return;
    }

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 1;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        u32 utf32;
        Result result = _16::Utf16ToUtf32IgnoreErrors(utf16, utf32);

        consumed = result.consumed;
        isOk = consumed <= capacity;

        if (isOk) *(pUtf32 + 0) = utf32;

        pBytes += result.advance;
        pUtf32 += 1;
        capacity -= isOk ? 1 : 0;
    }
}

ALT_NODISCARD inline bool Utf16ToUtf32(const Slice<const u16> bytes, u32* pUtf32, usize& capacity) noexcept {

    if (!pUtf32) {

        capacity = LengthUtf16ToUtf32IgnoreErrors(bytes);
        return true;
    }

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 1;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        u32 utf32;
        Result result = _16::Utf16ToUtf32(utf16, utf32);

        consumed = result.consumed;
        isOk = consumed <= capacity;

        if (isOk) *(pUtf32 + 0) = utf32;

        pBytes += result.advance;
        pUtf32 += 1;
        capacity -= isOk ? 1 : 0;
    }

    return isOk && consumed != 0;
}

inline void Utf16ToUtf32Replace(
    const Slice<const u16> bytes, u32* pUtf32, usize& capacity, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    if (!pUtf32) {

        capacity = LengthUtf16ToUtf32Replace(bytes);
        return;
    }

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 1;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        u32 utf32;
        Result result = _16::Utf16ToUtf32Replace(utf16, utf32, replaceUtf32);

        consumed = result.consumed;
        isOk = consumed <= capacity;

        if (isOk) *(pUtf32 + 0) = utf32;

        pBytes += result.advance;
        pUtf32 += 1;
        capacity -= isOk ? 1 : 0;
    }
}

ALT_NODISCARD inline usize LengthUtf16ToUtf8IgnoreErrors(const Slice<const u16> bytes) noexcept {

    usize totalSize = 0;

    const u16* pBytesEnd = bytes.DataEnd();

    for (const u16* pBytes = bytes.Data(); pBytes < pBytesEnd;) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        const usize size = _16::LengthUtf16ToUtf8(utf16);

        totalSize += size;
        pBytes += size == 4 ? 2 : 1;
    }

    return totalSize;
}

ALT_NODISCARD inline Result LengthUtf16ToUtf8(const Slice<const u16> bytes) noexcept {

    Result result{};

    usize size = 0;
    usize advance = 0;
    bool isValidUtf16 = true;

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    while (isValidUtf16 && pBytes < pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        isValidUtf16 = _16::IsValidUtf16(utf16);

        size = _16::LengthUtf16ToUtf8(utf16);
        advance = size == 4 ? 2 : 1;

        result.consumed += size;
        pBytes += advance;
    }

    result.consumed -= isValidUtf16 ? 0 : size;

    result.advance = static_cast<usize>(pBytes - bytes.Data());
    result.advance -= isValidUtf16 ? 0 : advance;

    return result;
}

ALT_NODISCARD inline usize LengthUtf16ToUtf8Replace(
    const Slice<const u16> bytes, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    usize resultSize = 0;

    const u16* pBytesEnd = bytes.DataEnd();
    const usize replaceSize = _32::LengthUtf32ToUtf8(replaceUtf32);

    for (const u16* pBytes = bytes.Data(); pBytes < pBytesEnd;) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        const bool isValidUtf16 = _16::IsValidUtf16(utf16);

        const usize size = _16::LengthUtf16ToUtf8(utf16);
        const usize advance = size == 4 ? 2 : 1;

        resultSize += isValidUtf16
            ? size
            : replaceSize;

        pBytes += isValidUtf16
            ? advance
            : 1;
    }

    return resultSize;
}

inline void Utf16ToUtf8IgnoreErrors(const Slice<const u16> bytes, u8* pBytesOut, usize& capacity) noexcept {

    if (!pBytesOut) {

        capacity = LengthUtf16ToUtf8IgnoreErrors(bytes);
        return;
    }

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 1;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        u8 utf8[4];
        Result result = _16::Utf16ToUtf8IgnoreErrors(utf16, utf8);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytesOut + 0) = utf8[0];
        if (1 < consumed) *(pBytesOut + 1) = utf8[1];
        if (2 < consumed) *(pBytesOut + 2) = utf8[2];
        if (3 < consumed) *(pBytesOut + 3) = utf8[3];

        pBytes += result.advance;
        pBytesOut += consumed;
        capacity -= consumed;
    }
}

ALT_NODISCARD inline bool Utf16ToUtf8(const Slice<const u16> bytes, u8* pBytesOut, usize& capacity) noexcept {

    if (!pBytesOut) {

        capacity = LengthUtf16ToUtf8IgnoreErrors(bytes);
        return true;
    }

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 1;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        u8 utf8[4];
        Result result = _16::Utf16ToUtf8(utf16, utf8);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytesOut + 0) = utf8[0];
        if (1 < consumed) *(pBytesOut + 1) = utf8[1];
        if (2 < consumed) *(pBytesOut + 2) = utf8[2];
        if (3 < consumed) *(pBytesOut + 3) = utf8[3];

        pBytes += result.advance;
        pBytesOut += consumed;
        capacity -= consumed;
    }

    return isOk && consumed != 0;
}

inline void Utf16ToUtf8Replace(
    const Slice<const u16> bytes, u8* pBytesOut, usize& capacity, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    if (!pBytesOut) {

        capacity = LengthUtf16ToUtf8Replace(bytes, replaceUtf32);
        return;
    }

    const u16* pBytes = bytes.Data();
    const u16* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 1;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u16 utf16[2]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u16>(0),
        };

        u8 utf8[4];
        Result result = _16::Utf16ToUtf8Replace(utf16, utf8, replaceUtf32);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytesOut + 0) = utf8[0];
        if (1 < consumed) *(pBytesOut + 1) = utf8[1];
        if (2 < consumed) *(pBytesOut + 2) = utf8[2];
        if (3 < consumed) *(pBytesOut + 3) = utf8[3];

        pBytes += result.advance;
        pBytesOut += consumed;
        capacity -= consumed;
    }
}

ALT_NODISCARD inline usize LengthUtf8ToUtf32IgnoreErrors(const Slice<const u8> bytes) noexcept {

    usize size = 0;

    const u8* pBytesEnd = bytes.DataEnd();

    for (const u8* pBytes = bytes.Data(); pBytes < pBytesEnd;) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        pBytes += _8::ForwardUtf8IgnoreErrors(utf8);
        size += 1;
    }

    return size;
}

ALT_NODISCARD inline Result LengthUtf8ToUtf32(const Slice<const u8> bytes) noexcept {

    Result result{};

    usize advance = 0;
    bool isValidUtf8 = true;

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    while (isValidUtf8 && pBytes < pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        advance = _8::ForwardUtf8(utf8);

        isValidUtf8 = advance != 0;
        advance = Num::Max(advance, 1);

        result.consumed += isValidUtf8 ? 1 : 0;
        pBytes += advance;
    }

    result.advance = static_cast<usize>(pBytes - bytes.Data());
    result.advance -= isValidUtf8 ? 0 : advance;

    return result;
}

ALT_NODISCARD inline usize LengthUtf8ToUtf32Replace(const Slice<const u8> bytes) noexcept {

    usize size = 0;

    const u8* pBytesEnd = bytes.DataEnd();

    for (const u8* pBytes = bytes.Data(); pBytes < pBytesEnd;) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        usize advance = _8::ForwardUtf8(utf8);
        advance = Num::Max(advance, 1);

        size += 1;
        pBytes += advance;
    }

    return size;
}

inline void Utf8ToUtf32IgnoreErrors(const Slice<const u8> bytes, u32* pUtf32, usize& capacity) noexcept {

    if (!pUtf32) {

        capacity = LengthUtf8ToUtf32IgnoreErrors(bytes);
        return;
    }

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 4;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        u32 utf32;
        Result result = _8::Utf8ToUtf32IgnoreErrors(utf8, utf32);

        consumed = result.consumed;
        isOk = consumed <= capacity;

        if (isOk) *(pUtf32 + 0) = utf32;

        pBytes += result.advance;
        pUtf32 += 1;
        capacity -= isOk ? 1 : 0;
    }
}

ALT_NODISCARD inline bool Utf8ToUtf32(const Slice<const u8> bytes, u32* pUtf32, usize& capacity) noexcept {

    if (!pUtf32) {

        capacity = LengthUtf8ToUtf32IgnoreErrors(bytes);
        return true;
    }

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 4;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        u32 utf32;
        Result result = _8::Utf8ToUtf32(utf8, utf32);

        consumed = result.consumed;
        isOk = consumed <= capacity;

        if (isOk) *(pUtf32 + 0) = utf32;

        pBytes += result.advance;
        pUtf32 += 1;
        capacity -= isOk ? 1 : 0;
    }

    return isOk && consumed != 0;
}

inline void Utf8ToUtf32Replace(
    const Slice<const u8> bytes, u32* pUtf32, usize& capacity, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    if (!pUtf32) {

        capacity = LengthUtf8ToUtf32Replace(bytes);
        return;
    }

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 4;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        u32 utf32;
        Result result = _8::Utf8ToUtf32Replace(utf8, utf32, replaceUtf32);

        consumed = result.consumed;
        isOk = consumed <= capacity;

        if (isOk) *(pUtf32 + 0) = utf32;

        pBytes += result.advance;
        pUtf32 += 1;
        capacity -= isOk ? 1 : 0;
    }
}

ALT_NODISCARD inline usize LengthUtf8ToUtf16IgnoreErrors(const Slice<const u8> bytes) noexcept {

    usize size = 0;

    const u8* pBytesEnd = bytes.DataEnd();

    for (const u8* pBytes = bytes.Data(); pBytes < pBytesEnd;) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        const usize advance = _8::ForwardUtf8IgnoreErrors(utf8);

        size += advance == 4 ? 2 : 1;
        pBytes += advance;
    }

    return size;
}

ALT_NODISCARD inline Result LengthUtf8ToUtf16(const Slice<const u8> bytes) noexcept {

    Result result{};

    usize size = 0;
    usize advance = 0;
    bool isValidUtf8 = true;

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    while (isValidUtf8 && pBytes < pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        advance = _8::ForwardUtf8(utf8);

        isValidUtf8 = advance != 0;
        advance = Num::Max(advance, 1);

        size = advance == 4 ? 2 : 1;

        result.consumed += size;
        pBytes += advance;
    }

    result.consumed -= isValidUtf8 ? 0 : size;

    result.advance = static_cast<usize>(pBytes - bytes.Data());
    result.advance -= isValidUtf8 ? 0 : advance;

    return result;
}

ALT_NODISCARD inline usize LengthUtf8ToUtf16Replace(
    const Slice<const u8> bytes, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    usize totalSize = 0;
    const usize replaceSize = _32::LengthUtf32ToUtf16(replaceUtf32);

    const u8* pBytesEnd = bytes.DataEnd();

    for (const u8* pBytes = bytes.Data(); pBytes < pBytesEnd;) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        usize advance = _8::ForwardUtf8(utf8);

        const bool isValidUtf8 = advance != 0;
        advance = Num::Max(advance, 1);

        const usize size = advance == 4 ? 2 : 1;

        totalSize += isValidUtf8
            ? size
            : replaceSize;

        pBytes += advance;
    }

    return totalSize;
}

inline void Utf8ToUtf16IgnoreErrors(const Slice<const u8> bytes, u16* pBytesOut, usize& capacity) noexcept {

    if (!pBytesOut) {

        capacity = LengthUtf8ToUtf16IgnoreErrors(bytes);
        return;
    }

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 4;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        u16 utf16[2];
        Result result = _8::Utf8ToUtf16IgnoreErrors(utf8, utf16);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytesOut + 0) = utf16[0];
        if (1 < consumed) *(pBytesOut + 1) = utf16[1];

        pBytes += result.advance;
        pBytesOut += consumed;
        capacity -= isOk ? consumed : 0;
    }
}

ALT_NODISCARD inline bool Utf8ToUtf16(const Slice<const u8> bytes, u16* pBytesOut, usize& capacity) noexcept {

    if (!pBytesOut) {

        capacity = LengthUtf8ToUtf16IgnoreErrors(bytes);
        return true;
    }

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 4;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        u16 utf16[2];
        Result result = _8::Utf8ToUtf16(utf8, utf16);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytesOut + 0) = utf16[0];
        if (1 < consumed) *(pBytesOut + 1) = utf16[1];

        pBytes += result.advance;
        pBytesOut += consumed;
        capacity -= isOk ? consumed : 0;
    }

    return isOk && consumed != 0;
}

inline void Utf8ToUtf16Replace(
    const Slice<const u8> bytes, u16* pBytesOut, usize& capacity, const u32 replaceUtf32 = GetSubstitutionForInvalidUTF32()
) noexcept {

    if (!pBytesOut) {

        capacity = LengthUtf8ToUtf16Replace(bytes, replaceUtf32);
        return;
    }

    const u8* pBytes = bytes.Data();
    const u8* pBytesEnd = bytes.DataEnd();

    bool isOk = true;
    usize consumed = 4;

    while (capacity > 0 && consumed != 0 && pBytes != pBytesEnd) {

        const u8 utf8[4]{
            *pBytes,
            pBytes + 1 < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
            pBytes + 2 < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
            pBytes + 3 < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
        };

        u16 utf16[2];
        Result result = _8::Utf8ToUtf16Replace(utf8, utf16, replaceUtf32);

        isOk = result.consumed <= capacity;
        consumed = isOk ? result.consumed : capacity;

        if (0 < consumed) *(pBytesOut + 0) = utf16[0];
        if (1 < consumed) *(pBytesOut + 1) = utf16[1];

        pBytes += result.advance;
        pBytesOut += consumed;
        capacity -= isOk ? consumed : 0;
    }
}

} // namespace Utf
} // namespace Alt

#endif // __ALT_STD_UTF__