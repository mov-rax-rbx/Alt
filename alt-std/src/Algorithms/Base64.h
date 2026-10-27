#pragma once

#ifndef __ALT_STD_BASE64__
#define __ALT_STD_BASE64__

#include "../IfErr.h"
#include "../TypeTraits.h"

#include "../Collections/Vector.h"

#include "../Utils/Slice.h"
#include "../Utils/Optional.h"

namespace Alt {

namespace _Base64Details {

constexpr static const char* _Base64Chars[2]{
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/",

    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "-_"
};

constexpr static u8 _FromBase64Chars[256]{
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 62, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 63,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

ALT_FORCE_INLINE usize PosToChar(const u8 ch) {

    // 64 - invalid
   return _FromBase64Chars[ch];
}

ALT_FORCE_INLINE bool Base64Convert(const u8* bytes, const usize size, const bool isForUrl, Vector<u8>& result) {

    const usize sizeOld = result.GetSize();
    iferr_scope_on_err {

        ALT_MAYBE_UNUSED const bool isOk = result.Resize(sizeOld);

        // resize to the less size - never panic
        ALT_ASSERT(isOk);
    };

    const usize sizeBase = (size / 3) * 3;
    usize sizeToAllocate = (size / 3) * 4;

    if (!isForUrl) {

        sizeToAllocate += size - sizeBase != 0 ? 4 : 0;
    }
    else {

        sizeToAllocate += size - sizeBase != 0 ? (size - sizeBase + 1) : 0;
    }

    result.ResizeAddition(sizeToAllocate) iferr_return false;

    const u8 TRAILING = static_cast<u8>('=');
    const char* base64_ = _Base64Chars[isForUrl ? 1 : 0];

    usize iBytes = 3;
    u8* pResult = result.Data() + sizeOld;

    for (; iBytes <= size; iBytes += 3) {

        usize pos = iBytes - 3;

        pResult[0] = static_cast<u8>(base64_[(bytes[pos + 0] & 0xfc) >> 2]);
        pResult[1] = static_cast<u8>(base64_[((bytes[pos + 0] & 0x03) << 4) + ((bytes[pos + 1] & 0xf0) >> 4)]);
        pResult[2] = static_cast<u8>(base64_[((bytes[pos + 1] & 0x0f) << 2) + ((bytes[pos + 2] & 0xc0) >> 6)]);
        pResult[3] = static_cast<u8>(base64_[bytes[pos + 2] & 0x3f]);

        pResult += 4;
    }

    const usize pos = iBytes - 3;
    if (size - pos == 1) {

        pResult[0] = static_cast<u8>(base64_[(bytes[pos + 0] & 0xfc) >> 2]);
        pResult[1] = static_cast<u8>(base64_[(bytes[pos + 0] & 0x03) << 4]);

        if (!isForUrl) {

            pResult[2] = TRAILING;
            pResult[3] = TRAILING;
        }
    }
    else if (size - pos == 2) {

        pResult[0] = static_cast<u8>(base64_[(bytes[pos + 0] & 0xfc) >> 2]);
        pResult[1] = static_cast<u8>(base64_[((bytes[pos + 0] & 0x03) << 4) + ((bytes[pos + 1] & 0xf0) >> 4)]);
        pResult[2] = static_cast<u8>(base64_[(bytes[pos + 1] & 0x0f) << 2]);

        if (!isForUrl) {

            pResult[3] = TRAILING;
        }
    }

    return true;
}

ALT_FORCE_INLINE bool FromBase64(const u8* bytes, usize size, Vector<u8>& result) {

    const usize sizeOld = result.GetSize();
    iferr_scope_on_err {

        ALT_MAYBE_UNUSED const bool isOk = result.Resize(sizeOld);

        // resize to the less size - never panic
        ALT_ASSERT(isOk);
    };

    const u8 TRAILING = static_cast<u8>('=');
    while (size != 0 && bytes[size - 1] == TRAILING) {
        size -= 1;
    }

    if (size == 0) {
        return true;
    }

    const usize sizeBase = (size / 4) * 4;

    ALT_ASSERT(size - sizeBase != 1);
    if (size - sizeBase == 1) {
        return false;
    }

    usize sizeToAllocate = (size / 4) * 3;
    if (size - sizeBase == 2) {

        sizeToAllocate += 1;
    }
    else if (size - sizeBase == 3) {

        sizeToAllocate += 2;
    }

    result.ResizeAddition(sizeToAllocate) iferr_return false;

    usize iBytes = 4;
    u8* pResult = result.Data() + sizeOld;

    for (; iBytes <= size; iBytes += 4) {

        const usize pos = iBytes - 4;

        const usize chpos0 = PosToChar(bytes[pos + 0]);
        const usize chpos1 = PosToChar(bytes[pos + 1]);
        const usize chpos2 = PosToChar(bytes[pos + 2]);
        const usize chpos3 = PosToChar(bytes[pos + 3]);

        const usize chvalid = chpos0 | chpos1 | chpos2 | chpos3;
        if (chvalid >= 64) {
            return false;
        }

        pResult[0] = static_cast<u8>((chpos0 << 2) + ((chpos1 & 0x30) >> 4));
        pResult[1] = static_cast<u8>(((chpos1 & 0x0f) << 4) + ((chpos2 & 0x3c) >> 2));
        pResult[2] = static_cast<u8>(((chpos2 & 0x03) << 6) + chpos3);

        pResult += 3;
    }

    const usize pos = iBytes - 4;

    if (size - pos == 2) {

        const usize chpos0 = PosToChar(bytes[pos + 0]);
        const usize chpos1 = PosToChar(bytes[pos + 1]);

        const usize chvalid = chpos0 | chpos1;
        if (chvalid >= 64) {
            return false;
        }

        pResult[0] = static_cast<u8>((chpos0 << 2) + ((chpos1 & 0x30) >> 4));
        pResult += 1;
    }
    else if (size - pos == 3) {

        const usize chpos0 = PosToChar(bytes[pos + 0]);
        const usize chpos1 = PosToChar(bytes[pos + 1]);
        const usize chpos2 = PosToChar(bytes[pos + 2]);

        const usize chvalid = chpos0 | chpos1 | chpos2;
        if (chvalid >= 64) {
            return false;
        }

        pResult[0] = static_cast<u8>((chpos0 << 2) + ((chpos1 & 0x30) >> 4));
        pResult[1] = static_cast<u8>(((chpos1 & 0x0f) << 4) + ((chpos2 & 0x3c) >> 2));
        pResult += 2;
    }

    return true;
}

} // namespace _Base64Details

namespace Base64 {

ALT_NODISCARD inline bool ConvertToBase64(const u8* bytes, const usize size, const bool isForUrl, Vector<u8>& result) {

    result.Clear();
    return _Base64Details::Base64Convert(bytes, size, isForUrl, result);
}

ALT_NODISCARD inline bool ConvertToBase64(const Slice<const u8> bytes, const bool isForUrl, Vector<u8>& result) {

    result.Clear();
    return _Base64Details::Base64Convert(bytes.Data(), bytes.GetSize(), isForUrl, result);
}

ALT_NODISCARD inline Optional<Vector<u8>> ConvertToBase64(const Slice<const u8> bytes, const bool isForUrl) {
    iferr_scope;

    Vector<u8> result;
    _Base64Details::Base64Convert(bytes.Data(), bytes.GetSize(), isForUrl, result) iferr_return Nullopt;

    return result;
}

ALT_NODISCARD inline bool ConvertToBase64Append(const u8* bytes, const usize size, const bool isForUrl, Vector<u8>& result) {

    return _Base64Details::Base64Convert(bytes, size, isForUrl, result);
}

ALT_NODISCARD inline bool ConvertToBase64Append(const Slice<const u8> bytes, const bool isForUrl, Vector<u8>& result) {

    return _Base64Details::Base64Convert(bytes.Data(), bytes.GetSize(), isForUrl, result);
}

ALT_NODISCARD inline bool FromBase64(const u8* bytes, const usize size, Vector<u8>& result) {

    return _Base64Details::FromBase64(bytes, size, result);
}

ALT_NODISCARD inline bool FromBase64(const Slice<const u8> bytes, Vector<u8>& result) {

    return _Base64Details::FromBase64(bytes.Data(), bytes.GetSize(), result);
}

ALT_NODISCARD inline Optional<Vector<u8>> FromBase64(const Slice<const u8> bytes) {
    iferr_scope;

    Vector<u8> result;
    _Base64Details::FromBase64(bytes.Data(), bytes.GetSize(), result) iferr_return Nullopt;

    return result;
}

} // namespace Base64
} // namespace Alt

#endif // __ALT_STD_BASE64__