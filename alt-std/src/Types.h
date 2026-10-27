#pragma once

#ifndef __ALT_STD_TYPES__
#define __ALT_STD_TYPES__

#include <cstdint>

#include "Defines.h"

namespace Alt {
namespace Types {

using usize = size_t;
using umax = uintmax_t;
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

using ssize = ptrdiff_t;
using smax = intmax_t;
using s64 = int64_t;
using s32 = int32_t;
using s16 = int16_t;
using s8 = int8_t;

using uptr = uintptr_t;
using sptr = intptr_t;

using f32 = float;
using f64 = double;

// UTF-32 character
using chara = char32_t;

} // namespace Types
} // namespace Alt

#endif // __ALT_STD_TYPES__