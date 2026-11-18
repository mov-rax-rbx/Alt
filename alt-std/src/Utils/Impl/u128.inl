#include "u128.h"
#include "s128.h"

namespace Alt {
namespace Types {

ALT_FORCE_INLINE constexpr u128::u128(const s128& rhs) noexcept
    : m_Low(rhs.GetLow())
    , m_High(Memory::Transmute<s64, u64>(rhs.GetHigh()))
{}

ALT_FORCE_INLINE constexpr u128::operator s128() noexcept {

    return s128::FromHighLow(Memory::Transmute<u64, s64>(m_High), m_Low);
}

} // namespace Types
} // namespace Alt