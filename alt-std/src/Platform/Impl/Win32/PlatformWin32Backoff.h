#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_BACKOFF__
#define __ALT_STD_PLATFORM_WIN32_BACKOFF__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32.h"

namespace Alt {
namespace _PlatformDetails {

class Backoff;

} // namespace _PlatformDetails

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<_PlatformDetails::Backoff>
    : std::true_type
{};

} // namespace TypeTraits

namespace _PlatformDetails {

/**
 * @brief Provides an exponential backoff strategy for synchronization primitives.
 * @details https://docs.rs/crossbeam-utils/latest/src/crossbeam_utils/backoff.rs.html#80-82
 * @example
 * 
 * Alt::Backoff backoff{};
 * 
 * while (Platform::AtomicLoadRelaxed(&state) != State::Ready) {
 *     backoff.SpinLight();
 * }
 */
class Backoff {
public:

    /**
     * Retry some oepration because another thread something might change
     */
    ALT_FORCE_INLINE void SpinLight() noexcept {

        const u32 step = m_Step;
        m_Step += m_Step < MAX_STEPS_SPIN ? 1 : 0;

        const usize iters = static_cast<usize>(1) << Num::Min(step, MAX_STEPS_SPIN);
        for (usize i = 0; i < iters; ++i) {

            Platform::SpinHint();
        }
    }

    /**
     * Wait some operation completion from another thread
     */
    ALT_FORCE_INLINE void SpinHeavy() noexcept {

        const u32 step = m_Step;
        m_Step += m_Step < MAX_STEPS_YIELD ? 1 : 0;

        if (step <= MAX_STEPS_SPIN) {

            const usize iters = static_cast<usize>(1) << step;
            for (usize i = 0; i < iters; ++i) {

                Platform::SpinHint();
            }
        }
        else {

            Platform::ThreadSwitchToAnother();
        }
    }

private:

    // 2^6 = 64
    // Typical cache coherence latency (~30-60 cycles).
    inline constexpr static u32 MAX_STEPS_SPIN = 6;

    inline constexpr static u32 MAX_STEPS_YIELD = 10;

    u32 m_Step = 0;
};

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_BACKOFF__