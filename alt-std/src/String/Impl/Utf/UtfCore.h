#pragma once

#ifndef __ALT_STD_UTF_CORE__
#define __ALT_STD_UTF_CORE__

#include "../../../TypeTraits.h"

#include "../../../Memory/Impl/MemoryPreInclude.h"
#include "../../../Platform/Impl/PlatformPreInclude.h"

namespace Alt {

/*
 * https://www.unicode.org/versions/Unicode17.0.0/#Technical_Overview
 */
namespace Utf {

enum class Encoding {

    Unknown,

    // The endian depends from machine (ALT_PLATFORM_X86 - little endian)
    Utf8,
    Utf16,
    Utf32,
};

struct Result {

    usize advance = 0; // advance source (read)
    usize consumed = 0; // consumed in destination (write)

    bool IsOk() const noexcept {

        return consumed != 0;
    }
};

ALT_FORCE_INLINE constexpr u32 GetSubstitutionForInvalidUTF32() noexcept {

    // 3.9.6 U+FFFD Substitution of Maximal Subparts

    return 0xFFFD;
}

} // namespace Utf
} // namespace Alt

#endif // __ALT_STD_UTF_CORE__