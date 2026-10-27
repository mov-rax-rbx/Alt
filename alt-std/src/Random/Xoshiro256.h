#pragma once

#ifndef __ALT_STD_XOSHIRO256__
#define __ALT_STD_XOSHIRO256__

#include "../TypeTraits.h"

#include "../Utils/Num.h"
#include "../Collections/StaticVector.h"

#include "../Platform/Impl/PlatformPreInclude.h"

/**
 * https://prng.di.unimi.it/
 * 
 * @brief A 256-bit pseudo-random number generator.
 * @details Implements the xoshiro256** and xoshiro256+ algorithms.
 * @example
 * 
 * //Alt::Xoshiro256StarStar rnd{ Alt::Platform::GetTicks() };
 * Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();
 * 
 * const u64 x0 = rnd.GenerateU64();
 * const s64 x1 = rnd.GenerateS64();
 * 
 * const f32 x2 = rnd.GenerateF32_01();
 * const f64 x3 = rnd.GenerateF64_01();
 * 
 * Alt::Xoshiro256Plus rnd{ Alt::Platform::GetTicks() };
 * Alt::Xoshiro256Plus rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();
 * 
 * const f32 x0 = rnd.GenerateF32_01();
 * const f64 x1 = rnd.GenerateF64_01();
 */

namespace Alt {

class Xoshiro256StarStar;
class Xoshiro256Plus;

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<Xoshiro256StarStar>
    : std::true_type
{};

template <>
struct __is_trivially_reallocatable__<Xoshiro256Plus>
    : std::true_type
{};

} // namespace TypeTraits

/* This is xoshiro256** 1.0, one of our all-purpose, rock-solid
   generators. It has excellent (sub-ns) speed, a state (256 bits) that is
   large enough for any parallel application, and it passes all tests we
   are aware of.

   For generating just floating-point numbers, xoshiro256+ is even faster.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

// 'xoshiro256**', for 'u64'. Can be used for 'f32' and 'f64', but better to use 'xoshiro256+'
class Xoshiro256StarStar {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Xoshiro256StarStar CreateSeededByTime() noexcept {

        u64 seed = static_cast<u64>(Platform::GetReturnAddress()) ^ static_cast<u64>(Platform::GetSeedAddress()) ^ Platform::GetTicks();
        return Xoshiro256StarStar(seed);
    }

    ALT_FORCE_INLINE Xoshiro256StarStar() noexcept = default;

    Xoshiro256StarStar(u64 seed) noexcept {

        _s[0] = SplitMix64(seed);
        _s[1] = SplitMix64(seed);
        _s[2] = SplitMix64(seed);
        _s[3] = SplitMix64(seed);
    }

    ALT_FORCE_INLINE u64 GenerateU64() noexcept {

        const u64 result = Rotl(_s[1] * 5, 7) * 9;
        const u64 t = _s[1] << 17;

        _s[2] ^= _s[0];
        _s[3] ^= _s[1];
        _s[1] ^= _s[2];
        _s[0] ^= _s[3];

        _s[2] ^= t;

        _s[3] = Rotl(_s[3], 45);

        return result;
    }

    ALT_FORCE_INLINE u64 GenerateU64(const u64 min, const u64 max) noexcept {
        ALT_ASSERT(max >= min);

        const u64 num = GenerateU64();

        u64 len = max - min;
        if (Num::IsOverflowCheckAdd(len, 1, &len)) {

            return num;
        }

        return min + (num % len);
    }

    ALT_FORCE_INLINE s64 GenerateS64() noexcept {

        const u64 num = GenerateU64();
        return Memory::Transmute<u64, s64>(num);
    }

    ALT_FORCE_INLINE s64 GenerateS64(const s64 min, const s64 max) noexcept {
        ALT_ASSERT(max >= min);

        const u64 umin = static_cast<u64>(Num::ConvertSignedToUnsignedOrdered(min));
        const u64 umax = static_cast<u64>(Num::ConvertSignedToUnsignedOrdered(max));

        const u64 unum = GenerateU64(umin, umax);

        return Num::ConvertUnsignedToSignedOrdered(unum);
    }

    // [-1.0, 1.0]
    ALT_FORCE_INLINE f32 GenerateF32_01() noexcept {

        const u64 gen = GenerateU64();

        // https://www.h-schmidt.net/FloatConverter/IEEE754.html
        // (-1)^(sign) * 2^(exponent) * (1 + mantissa)
        // 1 bit + 8 bits + 23 bits

        //const u32 mantissa = 0x007FFFFF;
        const u32 exponent_2_pow_0_and_sign_pow_0 = 0x3F800000;

        // extract upper bits
        const u32 bits = (static_cast<u32>(gen) >> 9) | exponent_2_pow_0_and_sign_pow_0;

        // extract lower bits
        //const u32 bits = (static_cast<u32>(gen) & mantissa) | exponent_2_pow_0_and_sign_pow_0;

        return Memory::Transmute<u32, f32>(bits) - 1.0f;
    }

    // [-1.0, 1.0]
    ALT_FORCE_INLINE f64 GenerateF64_01() noexcept {

        const u64 gen = GenerateU64();

        // (-1)^(sign) * 2^(exponent) * (1 + mantissa)
        // 1 bit + 11 bits + 52 bits

        //const u64 mantissa = 0x000FFFFFFFFFFFFFull;
        const u64 exponent_2_pow_0_and_sign_pow_0 = 0x3FF0000000000000ull;

        // extract upper bits
        const u64 bits = (static_cast<u64>(gen) >> 12) | exponent_2_pow_0_and_sign_pow_0;

        // extract lower bits
        //const u64 bits = (static_cast<u64>(gen) & mantissa) | exponent_2_pow_0_and_sign_pow_0;

        return Memory::Transmute<u64, f64>(bits) - 1.0;
    }

    /* This is the jump function for the generator. It is equivalent
       to 2^128 calls to GenerateU64(); it can be used to generate 2^128
       non-overlapping subsequences for parallel computations. */
    void Jump() noexcept {

        constexpr static u64 JUMP[4]{
            0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c
        };

        u64 s0 = 0;
        u64 s1 = 0;
        u64 s2 = 0;
        u64 s3 = 0;

        for (usize i = 0; i < sizeof(JUMP) / sizeof(*JUMP); ++i) {

            for (u64 b = 0; b < 64; ++b) {

                if (JUMP[i] & (1ull << b)) {

                    s0 ^= _s[0];
                    s1 ^= _s[1];
                    s2 ^= _s[2];
                    s3 ^= _s[3];
                }

                GenerateU64();	
            }
        }

        _s[0] = s0;
        _s[1] = s1;
        _s[2] = s2;
        _s[3] = s3;
    }

    /* This is the long-jump function for the generator. It is equivalent to
       2^192 calls to GenerateU64(); it can be used to generate 2^64 starting points,
       from each of which Jump() will generate 2^64 non-overlapping
       subsequences for parallel distributed computations. */
    void LongJump() noexcept {

        constexpr static u64 LONG_JUMP[4]{
            0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635
        };

        u64 s0 = 0;
        u64 s1 = 0;
        u64 s2 = 0;
        u64 s3 = 0;

        for (usize i = 0; i < sizeof(LONG_JUMP) / sizeof(*LONG_JUMP); ++i) {

            for (u64 b = 0; b < 64; ++b) {

                if (LONG_JUMP[i] & (1ull << b)) {

                    s0 ^= _s[0];
                    s1 ^= _s[1];
                    s2 ^= _s[2];
                    s3 ^= _s[3];
                }

                GenerateU64();	
            }
        }

        _s[0] = s0;
        _s[1] = s1;
        _s[2] = s2;
        _s[3] = s3;
    }

private:

    ALT_FORCE_INLINE static u64 Rotl(const u64 x, const u64 k) noexcept {

        return (x << k) | (x >> (64 - k));
    }

    ALT_FORCE_INLINE static u64 SplitMix64(u64& seed) noexcept {

        u64 z = (seed += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }

private:

    Array<u64, 4> _s;
};

/* This is xoshiro256+ 1.0, our best and fastest generator for floating-point
   numbers. We suggest to use its upper bits for floating-point
   generation, as it is slightly faster than xoshiro256++/xoshiro256**. It
   passes all tests we are aware of except for the lowest three bits,
   which might fail linearity tests (and just those), so if low linear
   complexity is not considered an issue (as it is usually the case) it
   can be used to generate 64-bit outputs, too.

   We suggest to use a sign test to extract a random Boolean value, and
   right shifts to extract subsets of bits.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

// 'xoshiro256+', for 'f32' and 'f64'
class Xoshiro256Plus {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Xoshiro256Plus CreateSeededByTime() noexcept {

        u64 seed = static_cast<u64>(Platform::GetReturnAddress()) ^ static_cast<u64>(Platform::GetSeedAddress()) ^ Platform::GetTicks();
        return Xoshiro256Plus(seed);
    }

    ALT_FORCE_INLINE Xoshiro256Plus() noexcept = default;

    Xoshiro256Plus(u64 seed) noexcept {

        _s[0] = SplitMix64(seed);
        _s[1] = SplitMix64(seed);
        _s[2] = SplitMix64(seed);
        _s[3] = SplitMix64(seed);
    }

    // [-1.0, 1.0]
    ALT_FORCE_INLINE f32 GenerateF32_01() noexcept {

        const u64 gen = GenerateU64();

        // https://www.h-schmidt.net/FloatConverter/IEEE754.html
        // (-1)^(sign) * 2^(exponent) * (1 + mantissa)
        // 1 bit + 8 bits + 23 bits

        const u32 exponent_2_pow_0_and_sign_pow_0 = 0x3F800000;

        // extract upper bits
        const u32 bits = (static_cast<u32>(gen) >> 9) | exponent_2_pow_0_and_sign_pow_0;

        return Memory::Transmute<u32, f32>(bits) - 1.0f;
    }

    // [-1.0, 1.0]
    ALT_FORCE_INLINE f64 GenerateF64_01() noexcept {

        const u64 gen = GenerateU64();

        // (-1)^(sign) * 2^(exponent) * (1 + mantissa)
        // 1 bit + 11 bits + 52 bits

        const u64 exponent_2_pow_0_and_sign_pow_0 = 0x3FF0000000000000ull;

        // extract upper bits
        const u64 bits = (static_cast<u64>(gen) >> 12) | exponent_2_pow_0_and_sign_pow_0;

        return Memory::Transmute<u64, f64>(bits) - 1.0;
    }

    /* This is the jump function for the generator. It is equivalent
       to 2^128 calls to GenerateU64(); it can be used to generate 2^128
       non-overlapping subsequences for parallel computations. */
    void Jump() noexcept {

        constexpr static u64 JUMP[4]{
            0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c
        };

        u64 s0 = 0;
        u64 s1 = 0;
        u64 s2 = 0;
        u64 s3 = 0;

        for (usize i = 0; i < sizeof(JUMP) / sizeof(*JUMP); ++i) {

            for (u64 b = 0; b < 64; ++b) {

                if (JUMP[i] & (1ull << b)) {

                    s0 ^= _s[0];
                    s1 ^= _s[1];
                    s2 ^= _s[2];
                    s3 ^= _s[3];
                }

                GenerateU64();	
            }
        }
        
        _s[0] = s0;
        _s[1] = s1;
        _s[2] = s2;
        _s[3] = s3;
    }

    /* This is the long-jump function for the generator. It is equivalent to
       2^192 calls to GenerateU64(); it can be used to generate 2^64 starting points,
       from each of which Jump() will generate 2^64 non-overlapping
       subsequences for parallel distributed computations. */
    void LongJump() noexcept {

        constexpr static u64 LONG_JUMP[4]{
            0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635
        };

        u64 s0 = 0;
        u64 s1 = 0;
        u64 s2 = 0;
        u64 s3 = 0;

        for (usize i = 0; i < sizeof(LONG_JUMP) / sizeof(*LONG_JUMP); ++i) {

            for (u64 b = 0; b < 64; ++b) {

                if (LONG_JUMP[i] & UINT64_C(1) << b) {

                    s0 ^= _s[0];
                    s1 ^= _s[1];
                    s2 ^= _s[2];
                    s3 ^= _s[3];
                }

                GenerateU64();	
            }
        }

        _s[0] = s0;
        _s[1] = s1;
        _s[2] = s2;
        _s[3] = s3;
    }

private:

    ALT_FORCE_INLINE static u64 Rotl(const u64 x, const u64 k) noexcept {

        return (x << k) | (x >> (64 - k));
    }

    ALT_FORCE_INLINE static u64 SplitMix64(u64& seed) noexcept {

        u64 z = (seed += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }

    ALT_FORCE_INLINE u64 GenerateU64() noexcept {

        const u64 result = _s[0] + _s[3];
        const u64 t = _s[1] << 17;

        _s[2] ^= _s[0];
        _s[3] ^= _s[1];
        _s[1] ^= _s[2];
        _s[0] ^= _s[3];

        _s[2] ^= t;

        _s[3] = Rotl(_s[3], 45);

        return result;
    }

private:

    Array<u64, 4> _s;
};

} // namespace Alt

#endif // __ALT_STD_XOSHIRO256__