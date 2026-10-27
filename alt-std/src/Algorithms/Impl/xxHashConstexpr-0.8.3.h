#pragma once

#ifndef __ALT_STD_ALGORITHMS_HASH__
#define __ALT_STD_ALGORITHMS_HASH__

#include "../../TypeTraits.h"

#include "../../Memory/Impl/MemoryPreInclude.h"

#define XXH_STATIC_LINKING_ONLY
#define XXH_INLINE_ALL
#define XXH_NO_EXTERNC_GUARD
#define XXH_NO_STREAM // #define XXH_NO_STDLIB
#include "../xxHash-0.8.3/xxh3.h"

namespace Alt {
namespace Algorithms {
namespace _HashDetails {

alignas(64) constexpr static const xxh_u8 XXH3_kSecret[XXH_SECRET_DEFAULT_SIZE] = {
    0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c, 0xf7, 0x21, 0xad, 0x1c,
    0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb, 0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f,
    0xcb, 0x79, 0xe6, 0x4e, 0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
    0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6, 0x81, 0x3a, 0x26, 0x4c,
    0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb, 0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3,
    0x71, 0x64, 0x48, 0x97, 0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
    0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7, 0xc7, 0x0b, 0x4f, 0x1d,
    0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31, 0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64,
    0xea, 0xc5, 0xac, 0x83, 0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
    0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26, 0x29, 0xd4, 0x68, 0x9e,
    0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc, 0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce,
    0x45, 0xcb, 0x3a, 0x8f, 0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
};

constexpr static const xxh_u64 PRIME_MX1 = 0x165667919E3779F9ULL;  /*!< 0b0001011001010110011001111001000110011110001101110111100111111001 */
constexpr static const xxh_u64 PRIME_MX2 = 0x9FB21C651E98DF25ULL;  /*!< 0b1001111110110010000111000110010100011110100110001101111100100101 */

template <typename T>
constexpr XXH_FORCE_INLINE xxh_u32
XXH_readLE32Constexpr(const T* ptr)
{
    return (static_cast<xxh_u32>(ptr[0]) << 0 ) |
           (static_cast<xxh_u32>(ptr[1]) << 8 ) |
           (static_cast<xxh_u32>(ptr[2]) << 16) |
           (static_cast<xxh_u32>(ptr[3]) << 24);
}

template <typename T>
constexpr XXH_FORCE_INLINE xxh_u64
XXH_readLE64Constexpr(const T* ptr)
{
    return (static_cast<xxh_u64>(ptr[0]) << 0 ) |
           (static_cast<xxh_u64>(ptr[1]) << 8 ) |
           (static_cast<xxh_u64>(ptr[2]) << 16) |
           (static_cast<xxh_u64>(ptr[3]) << 24) |
           (static_cast<xxh_u64>(ptr[4]) << 32) |
           (static_cast<xxh_u64>(ptr[5]) << 40) |
           (static_cast<xxh_u64>(ptr[6]) << 48) |
           (static_cast<xxh_u64>(ptr[7]) << 56);
}

constexpr XXH_FORCE_INLINE xxh_u32
XXH_swap32Constexpr(xxh_u32 x)
{
    return  ((x << 24) & 0xff000000 ) |
            ((x <<  8) & 0x00ff0000 ) |
            ((x >>  8) & 0x0000ff00 ) |
            ((x >> 24) & 0x000000ff );
}

constexpr XXH_FORCE_INLINE xxh_u64
XXH_swap64Constexpr(xxh_u64 x)
{
    return  ((x << 56) & 0xff00000000000000ULL) |
            ((x << 40) & 0x00ff000000000000ULL) |
            ((x << 24) & 0x0000ff0000000000ULL) |
            ((x << 8)  & 0x000000ff00000000ULL) |
            ((x >> 8)  & 0x00000000ff000000ULL) |
            ((x >> 24) & 0x0000000000ff0000ULL) |
            ((x >> 40) & 0x000000000000ff00ULL) |
            ((x >> 56) & 0x00000000000000ffULL);
}

template <typename T>
constexpr XXH_FORCE_INLINE
void XXH_writeLE64Constexpr(T* dst, xxh_u64 v64)
{
    if (!XXH_CPU_LITTLE_ENDIAN) v64 = XXH_swap64Constexpr(v64);
    dst[0] = static_cast<xxh_u8>(v64 >> 0);
    dst[1] = static_cast<xxh_u8>(v64 >> 8);
    dst[2] = static_cast<xxh_u8>(v64 >> 16);
    dst[3] = static_cast<xxh_u8>(v64 >> 24);
    dst[4] = static_cast<xxh_u8>(v64 >> 32);
    dst[5] = static_cast<xxh_u8>(v64 >> 40);
    dst[6] = static_cast<xxh_u8>(v64 >> 48);
    dst[7] = static_cast<xxh_u8>(v64 >> 56);
}

constexpr XXH_FORCE_INLINE xxh_u32
XXH32_avalancheConstexpr(xxh_u32 hash)
{
    hash ^= hash >> 15;
    hash *= XXH_PRIME32_2;
    hash ^= hash >> 13;
    hash *= XXH_PRIME32_3;
    hash ^= hash >> 16;
    return hash;
}

constexpr XXH_FORCE_INLINE xxh_u32
XXH_rotl32Constexpr(xxh_u32 x, xxh_u32 r) {
    return (x << r) | (x >> (32 - r));
}

constexpr XXH_FORCE_INLINE xxh_u64
XXH_rotl64Constexpr(xxh_u64 x, xxh_u64 r) {
    return (x << r) | (x >> (64 - r));
}

template <typename T>
constexpr XXH_FORCE_INLINE xxh_u32
XXH_get32bitsConstexpr(const T* p) {
    return XXH_readLE32Constexpr(p);
}

constexpr XXH_FORCE_INLINE xxh_u64
XXH_mult32to64Constexpr(xxh_u64 x, xxh_u64 y) {
    return static_cast<xxh_u64>(static_cast<xxh_u32>(x)) * static_cast<xxh_u64>(static_cast<xxh_u32>(y));
}

constexpr XXH_FORCE_INLINE xxh_u32
XXH32_roundConstexpr(xxh_u32 acc, xxh_u32 input)
{
    acc += input * XXH_PRIME32_2;
    acc  = XXH_rotl32Constexpr(acc, 13);
    acc *= XXH_PRIME32_1;
    return acc;
}

constexpr XXH_FORCE_INLINE xxh_u64
XXH_xorshift64Constexpr(xxh_u64 v64, int shift)
{
    //XXH_ASSERT(0 <= shift && shift < 64);
    return v64 ^ (v64 >> shift);
}

constexpr XXH_FORCE_INLINE XXH64_hash_t
XXH3_rrmxmxConstexpr(xxh_u64 h64, xxh_u64 len)
{
    /* this mix is inspired by Pelle Evensen's rrmxmx */
    h64 ^= XXH_rotl64Constexpr(h64, 49) ^ XXH_rotl64Constexpr(h64, 24);
    h64 *= PRIME_MX2;
    h64 ^= (h64 >> 35) + len ;
    h64 *= PRIME_MX2;
    return XXH_xorshift64Constexpr(h64, 28);
}

constexpr XXH_FORCE_INLINE XXH64_hash_t
XXH3_avalancheConstexpr(xxh_u64 h64)
{
    h64 = XXH_xorshift64Constexpr(h64, 37);
    h64 *= PRIME_MX1;
    h64 = XXH_xorshift64Constexpr(h64, 32);
    return h64;
}

constexpr XXH_FORCE_INLINE xxh_u64
XXH64_avalancheConstexpr(xxh_u64 hash)
{
    hash ^= hash >> 33;
    hash *= XXH_PRIME64_2;
    hash ^= hash >> 29;
    hash *= XXH_PRIME64_3;
    hash ^= hash >> 32;
    return hash;
}

constexpr XXH_FORCE_INLINE XXH128_hash_t
XXH_mult64to128Constexpr(xxh_u64 lhs, xxh_u64 rhs)
{
    /* First calculate all of the cross products. */
    xxh_u64 const lo_lo = XXH_mult32to64Constexpr(lhs & 0xFFFFFFFF, rhs & 0xFFFFFFFF);
    xxh_u64 const hi_lo = XXH_mult32to64Constexpr(lhs >> 32,        rhs & 0xFFFFFFFF);
    xxh_u64 const lo_hi = XXH_mult32to64Constexpr(lhs & 0xFFFFFFFF, rhs >> 32);
    xxh_u64 const hi_hi = XXH_mult32to64Constexpr(lhs >> 32,        rhs >> 32);

    /* Now add the products together. These will never overflow. */
    xxh_u64 const cross = (lo_lo >> 32) + (hi_lo & 0xFFFFFFFF) + lo_hi;
    xxh_u64 const upper = (hi_lo >> 32) + (cross >> 32)        + hi_hi;
    xxh_u64 const lower = (cross << 32) | (lo_lo & 0xFFFFFFFF);

    XXH128_hash_t r128{};
    r128.low64  = lower;
    r128.high64 = upper;
    return r128;
}

constexpr XXH_FORCE_INLINE xxh_u64
XXH3_mul128_fold64Constexpr(xxh_u64 lhs, xxh_u64 rhs)
{
    XXH128_hash_t product = XXH_mult64to128Constexpr(lhs, rhs);
    return product.low64 ^ product.high64;
}

constexpr XXH_FORCE_INLINE void
XXH32_initAccsConstexpr(xxh_u32 *acc, xxh_u32 seed)
{
    //XXH_ASSERT(acc != NULL);
    acc[0] = seed + XXH_PRIME32_1 + XXH_PRIME32_2;
    acc[1] = seed + XXH_PRIME32_2;
    acc[2] = seed + 0;
    acc[3] = seed - XXH_PRIME32_1;
}

template <typename T>
constexpr XXH_FORCE_INLINE const T*
XXH32_consumeLongConstexpr(
    xxh_u32 *XXH_RESTRICT acc,
    T const *XXH_RESTRICT input,
    size_t len
)
{
    const T* const bEnd = input + len;
    const T* const limit = bEnd - 15;
    //XXH_ASSERT(acc != NULL);
    //XXH_ASSERT(input != NULL);
    //XXH_ASSERT(len >= 16);
    do {
        acc[0] = XXH32_roundConstexpr(acc[0], XXH_get32bitsConstexpr(input)); input += 4;
        acc[1] = XXH32_roundConstexpr(acc[1], XXH_get32bitsConstexpr(input)); input += 4;
        acc[2] = XXH32_roundConstexpr(acc[2], XXH_get32bitsConstexpr(input)); input += 4;
        acc[3] = XXH32_roundConstexpr(acc[3], XXH_get32bitsConstexpr(input)); input += 4;
    } while (input < limit);

    return input;
}

constexpr XXH_FORCE_INLINE XXH_PUREF xxh_u32
XXH32_mergeAccsConstexpr(const xxh_u32 *acc)
{
    //XXH_ASSERT(acc != NULL);
    return XXH_rotl32Constexpr(acc[0], 1)  + XXH_rotl32Constexpr(acc[1], 7)
         + XXH_rotl32Constexpr(acc[2], 12) + XXH_rotl32Constexpr(acc[3], 18);
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH_PUREF xxh_u32
XXH32_finalizeConstexpr(xxh_u32 hash, const T* ptr, size_t len)
{
#define XXH_PROCESS1 do {                                      \
    hash += (*ptr++) * XXH_PRIME32_5;                          \
    hash = XXH_rotl32Constexpr(hash, 11) * XXH_PRIME32_1;      \
} while (0)

#define XXH_PROCESS4 do {                                      \
    hash += XXH_get32bitsConstexpr(ptr) * XXH_PRIME32_3;       \
    ptr += 4;                                                  \
    hash  = XXH_rotl32Constexpr(hash, 17) * XXH_PRIME32_4;     \
} while (0)

    //if (ptr==NULL) XXH_ASSERT(len == 0);

    len &= 15;
    while (len >= 4) {
        XXH_PROCESS4;
        len -= 4;
    }
    while (len > 0) {
        XXH_PROCESS1;
        --len;
    }
    return XXH32_avalancheConstexpr(hash);
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH_PUREF XXH64_hash_t
XXH3_len_1to3_64bConstexpr(const T* input, size_t len, const xxh_u8* secret, XXH64_hash_t seed)
{
    //XXH_ASSERT(input != NULL);
    //XXH_ASSERT(1 <= len && len <= 3);
    //XXH_ASSERT(secret != NULL);
    /*
     * len = 1: combined = { input[0], 0x01, input[0], input[0] }
     * len = 2: combined = { input[1], 0x02, input[0], input[1] }
     * len = 3: combined = { input[2], 0x03, input[0], input[1] }
     */
    {   xxh_u8  const c1 = static_cast<xxh_u8>(input[0]);
        xxh_u8  const c2 = static_cast<xxh_u8>(input[len >> 1]);
        xxh_u8  const c3 = static_cast<xxh_u8>(input[len - 1]);
        xxh_u32 const combined = (static_cast<xxh_u32>(c1) << 16) | (static_cast<xxh_u32>(c2)  << 24)
                               | (static_cast<xxh_u32>(c3) <<  0) | (static_cast<xxh_u32>(len) << 8);
        xxh_u64 const bitflip = (XXH_readLE32Constexpr(secret) ^ XXH_readLE32Constexpr(secret+4)) + seed;
        xxh_u64 const keyed = static_cast<xxh_u64>(combined) ^ bitflip;
        return XXH64_avalancheConstexpr(keyed);
    }
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH_PUREF XXH64_hash_t
XXH3_len_4to8_64bConstexpr(const T* input, size_t len, const xxh_u8* secret, XXH64_hash_t seed)
{
    //XXH_ASSERT(input != NULL);
    //XXH_ASSERT(secret != NULL);
    //XXH_ASSERT(4 <= len && len <= 8);
    seed ^= static_cast<xxh_u64>(XXH_swap32Constexpr(static_cast<xxh_u32>(seed))) << 32;
    {   xxh_u32 const input1 = XXH_readLE32Constexpr(input);
        xxh_u32 const input2 = XXH_readLE32Constexpr(input + len - 4);
        xxh_u64 const bitflip = (XXH_readLE64Constexpr(secret+8) ^ XXH_readLE64Constexpr(secret+16)) - seed;
        xxh_u64 const input64 = input2 + (static_cast<xxh_u64>(input1) << 32);
        xxh_u64 const keyed = input64 ^ bitflip;
        return XXH3_rrmxmxConstexpr(keyed, len);
    }
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH_PUREF XXH64_hash_t
XXH3_len_9to16_64bConstexpr(const T* input, size_t len, const xxh_u8* secret, XXH64_hash_t seed)
{
    //XXH_ASSERT(input != NULL);
    //XXH_ASSERT(secret != NULL);
    //XXH_ASSERT(9 <= len && len <= 16);
    {   xxh_u64 const bitflip1 = (XXH_readLE64Constexpr(secret+24) ^ XXH_readLE64Constexpr(secret+32)) + seed;
        xxh_u64 const bitflip2 = (XXH_readLE64Constexpr(secret+40) ^ XXH_readLE64Constexpr(secret+48)) - seed;
        xxh_u64 const input_lo = XXH_readLE64Constexpr(input)           ^ bitflip1;
        xxh_u64 const input_hi = XXH_readLE64Constexpr(input + len - 8) ^ bitflip2;
        xxh_u64 const acc = len
                          + XXH_swap64Constexpr(input_lo) + input_hi
                          + XXH3_mul128_fold64Constexpr(input_lo, input_hi);
        return XXH3_avalancheConstexpr(acc);
    }
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH_PUREF XXH64_hash_t
XXH3_len_0to16_64bConstexpr(const T* input, size_t len, const xxh_u8* secret, XXH64_hash_t seed)
{
    //XXH_ASSERT(len <= 16);
    {   if (XXH_likely(len >  8)) return XXH3_len_9to16_64bConstexpr(input, len, secret, seed);
        if (XXH_likely(len >= 4)) return XXH3_len_4to8_64bConstexpr(input, len, secret, seed);
        if (len) return XXH3_len_1to3_64bConstexpr(input, len, secret, seed);
        return XXH64_avalancheConstexpr(seed ^ (XXH_readLE64Constexpr(secret+56) ^ XXH_readLE64Constexpr(secret+64)));
    }
}

template <typename T>
constexpr XXH_FORCE_INLINE xxh_u64
XXH3_mix16BConstexpr(
    const T* XXH_RESTRICT input,
    const xxh_u8* XXH_RESTRICT secret, xxh_u64 seed64)
{
    {   xxh_u64 const input_lo = XXH_readLE64Constexpr(input);
        xxh_u64 const input_hi = XXH_readLE64Constexpr(input+8);
        return XXH3_mul128_fold64Constexpr(
            input_lo ^ (XXH_readLE64Constexpr(secret)   + seed64),
            input_hi ^ (XXH_readLE64Constexpr(secret+8) - seed64)
        );
    }
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH_PUREF XXH64_hash_t
XXH3_len_17to128_64bConstexpr(
    const T* XXH_RESTRICT input, size_t len,
    const xxh_u8* XXH_RESTRICT secret, size_t,
    XXH64_hash_t seed)
{
    //XXH_ASSERT(secretSize >= XXH3_SECRET_SIZE_MIN); (void)secretSize;
    //XXH_ASSERT(16 < len && len <= 128);

    {   xxh_u64 acc = len * XXH_PRIME64_1;
        if (len > 32) {
            if (len > 64) {
                if (len > 96) {
                    acc += XXH3_mix16BConstexpr(input+48, secret+96, seed);
                    acc += XXH3_mix16BConstexpr(input+len-64, secret+112, seed);
                }
                acc += XXH3_mix16BConstexpr(input+32, secret+64, seed);
                acc += XXH3_mix16BConstexpr(input+len-48, secret+80, seed);
            }
            acc += XXH3_mix16BConstexpr(input+16, secret+32, seed);
            acc += XXH3_mix16BConstexpr(input+len-32, secret+48, seed);
        }
        acc += XXH3_mix16BConstexpr(input+0, secret+0, seed);
        acc += XXH3_mix16BConstexpr(input+len-16, secret+16, seed);
        return XXH3_avalancheConstexpr(acc);
    }
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH_PUREF XXH64_hash_t
XXH3_len_129to240_64bConstexpr(
    const T* XXH_RESTRICT input, size_t len,
    const xxh_u8* XXH_RESTRICT secret, size_t,
    XXH64_hash_t seed)
{
    //XXH_ASSERT(secretSize >= XXH3_SECRET_SIZE_MIN); (void)secretSize;
    //XXH_ASSERT(128 < len && len <= XXH3_MIDSIZE_MAX);

    #define XXH3_MIDSIZE_STARTOFFSET 3
    #define XXH3_MIDSIZE_LASTOFFSET  17

    {   xxh_u64 acc = len * XXH_PRIME64_1;
        xxh_u64 acc_end = 0;
        unsigned int const nbRounds = static_cast<unsigned int>(len) / 16;
        unsigned int i = 0;
        //XXH_ASSERT(128 < len && len <= XXH3_MIDSIZE_MAX);
        for (i=0; i<8; i++) {
            acc += XXH3_mix16BConstexpr(input+(16*i), secret+(16*i), seed);
        }
        /* last bytes */
        acc_end = XXH3_mix16BConstexpr(input + len - 16, secret + XXH3_SECRET_SIZE_MIN - XXH3_MIDSIZE_LASTOFFSET, seed);
        //XXH_ASSERT(nbRounds >= 8);
        acc = XXH3_avalancheConstexpr(acc);
        for (i=8 ; i < nbRounds; i++) {
            /*
             * Prevents clang for unrolling the acc loop and interleaving with this one.
             */
            //XXH_COMPILER_GUARD(acc);
            acc_end += XXH3_mix16BConstexpr(input+(16*i), secret+(16*(i-8)) + XXH3_MIDSIZE_STARTOFFSET, seed);
        }
        return XXH3_avalancheConstexpr(acc + acc_end);
    }
}

constexpr XXH_FORCE_INLINE xxh_u64
XXH_mult32to64_add64Constexpr(xxh_u64 lhs, xxh_u64 rhs, xxh_u64 acc)
{
    return XXH_mult32to64Constexpr(static_cast<xxh_u32>(lhs), static_cast<xxh_u32>(rhs)) + acc;
}

template <typename T>
constexpr XXH_FORCE_INLINE void
XXH3_scalarRoundConstexpr(
    xxh_u64* XXH_RESTRICT xacc,
    const T* XXH_RESTRICT xinput,
    const xxh_u8* XXH_RESTRICT xsecret,
    size_t lane)
{
    //XXH_ASSERT(lane < XXH_ACC_NB);
    //XXH_ASSERT(((size_t)acc & (XXH_ACC_ALIGN-1)) == 0);
    {
        xxh_u64 const data_val = XXH_readLE64Constexpr(xinput + lane * 8);
        xxh_u64 const data_key = data_val ^ XXH_readLE64Constexpr(xsecret + lane * 8);
        xacc[lane ^ 1] += data_val; /* swap adjacent lanes */
        xacc[lane] = XXH_mult32to64_add64Constexpr(data_key /* & 0xFFFFFFFF */, data_key >> 32, xacc[lane]);
    }
}

template <typename T>
constexpr XXH_FORCE_INLINE void
XXH3_accumulate_512_scalarConstexpr(
    xxh_u64* XXH_RESTRICT acc,
    const T* XXH_RESTRICT input,
    const xxh_u8* XXH_RESTRICT secret)
{
    size_t i = 0;
    for (i=0; i < XXH_ACC_NB; i++) {
        XXH3_scalarRoundConstexpr(acc, input, secret, i);
    }
}

template <typename T>
constexpr XXH_FORCE_INLINE void
XXH3_accumulate_scalarConstexpr(
    xxh_u64* XXH_RESTRICT acc,
    const T* XXH_RESTRICT input,
    const xxh_u8* XXH_RESTRICT secret,
    size_t nbStripes)
{
    size_t n = 0;
    for (n = 0; n < nbStripes; n++ ) {
        const T* const in = input + n*XXH_STRIPE_LEN;
        //XXH_PREFETCH(in + XXH_PREFETCH_DIST);
        XXH3_accumulate_512_scalarConstexpr(
                 acc,
                 in,
                 secret + n*XXH_SECRET_CONSUME_RATE);
    }
}

constexpr XXH_FORCE_INLINE void
XXH3_scalarScrambleRoundConstexpr(
    xxh_u64* XXH_RESTRICT xacc,
    const xxh_u8* XXH_RESTRICT xsecret,
    size_t lane)
{
    //XXH_ASSERT((((size_t)acc) & (XXH_ACC_ALIGN-1)) == 0);
    //XXH_ASSERT(lane < XXH_ACC_NB);
    {
        xxh_u64 const key64 = XXH_readLE64Constexpr(xsecret + lane * 8);
        xxh_u64 acc64 = xacc[lane];
        acc64 = XXH_xorshift64Constexpr(acc64, 47);
        acc64 ^= key64;
        acc64 *= XXH_PRIME32_1;
        xacc[lane] = acc64;
    }
}

constexpr XXH_FORCE_INLINE void
XXH3_scrambleAcc_scalarConstexpr(xxh_u64* XXH_RESTRICT acc, const xxh_u8* XXH_RESTRICT secret)
{
    size_t i = 0;
    for (i=0; i < XXH_ACC_NB; i++) {
        XXH3_scalarScrambleRoundConstexpr(acc, secret, i);
    }
}

constexpr XXH_FORCE_INLINE void
XXH3_initCustomSecret_scalarConstexpr(xxh_u8* XXH_RESTRICT customSecret, xxh_u64 seed64)
{
    /*
     * We need a separate pointer for the hack below,
     * which requires a non-const pointer.
     * Any decent compiler will optimize this out otherwise.
     */
    const xxh_u8* kSecretPtr = XXH3_kSecret;
    //XXH_STATIC_ASSERT((XXH_SECRET_DEFAULT_SIZE & 15) == 0);

    {   int const nbRounds = XXH_SECRET_DEFAULT_SIZE / 16;
        int i = 0;
        for (i=0; i < nbRounds; i++) {
            /*
             * The asm hack causes the compiler to assume that kSecretPtr aliases with
             * customSecret, and on aarch64, this prevented LDP from merging two
             * loads together for free. Putting the loads together before the stores
             * properly generates LDP.
             */
            xxh_u64 lo = XXH_readLE64Constexpr(kSecretPtr + 16*i)     + seed64;
            xxh_u64 hi = XXH_readLE64Constexpr(kSecretPtr + 16*i + 8) - seed64;
            XXH_writeLE64Constexpr(customSecret + 16*i,     lo);
            XXH_writeLE64Constexpr(customSecret + 16*i + 8, hi);
    }   }
}

template <typename T>
constexpr XXH_FORCE_INLINE void
XXH3_hashLong_internal_loopConstexpr(
    xxh_u64* XXH_RESTRICT acc,
    const T* XXH_RESTRICT input, size_t len,
    const xxh_u8* XXH_RESTRICT secret, size_t secretSize)
{
    size_t const nbStripesPerBlock = (secretSize - XXH_STRIPE_LEN) / XXH_SECRET_CONSUME_RATE;
    size_t const block_len = XXH_STRIPE_LEN * nbStripesPerBlock;
    size_t const nb_blocks = (len - 1) / block_len;

    size_t n = 0;

    for (n = 0; n < nb_blocks; n++) {
        XXH3_accumulate_scalarConstexpr(acc, input + n*block_len, secret, nbStripesPerBlock);
        XXH3_scrambleAcc_scalarConstexpr(acc, secret + secretSize - XXH_STRIPE_LEN);
    }

    /* last partial block */
    //XXH_ASSERT(len > XXH_STRIPE_LEN);
    {   size_t const nbStripes = ((len - 1) - (block_len * nb_blocks)) / XXH_STRIPE_LEN;
        //XXH_ASSERT(nbStripes <= (secretSize / XXH_SECRET_CONSUME_RATE));
        XXH3_accumulate_scalarConstexpr(acc, input + nb_blocks*block_len, secret, nbStripes);

        /* last stripe */
        {   const T* const p = input + len - XXH_STRIPE_LEN;
#define XXH_SECRET_LASTACC_START 7  /* not aligned on 8, last secret is different from acc & scrambler */
            XXH3_accumulate_512_scalarConstexpr(acc, p, secret + secretSize - XXH_STRIPE_LEN - XXH_SECRET_LASTACC_START);
    }   }
}

constexpr XXH_FORCE_INLINE xxh_u64
XXH3_mix2AccsConstexpr(const xxh_u64* XXH_RESTRICT acc, const xxh_u8* XXH_RESTRICT secret)
{
    return XXH3_mul128_fold64Constexpr(
               acc[0] ^ XXH_readLE64Constexpr(secret),
               acc[1] ^ XXH_readLE64Constexpr(secret+8) );
}

constexpr XXH_FORCE_INLINE XXH_PUREF XXH64_hash_t
XXH3_mergeAccsConstexpr(const xxh_u64* XXH_RESTRICT acc, const xxh_u8* XXH_RESTRICT secret, xxh_u64 start)
{
    xxh_u64 result64 = start;
    size_t i = 0;

    for (i = 0; i < 4; i++) {
        result64 += XXH3_mix2AccsConstexpr(acc+2*i, secret + 16*i);
    }

    return XXH3_avalancheConstexpr(result64);
}

constexpr XXH_FORCE_INLINE XXH_PUREF XXH64_hash_t
XXH3_finalizeLong_64bConstexpr(const xxh_u64* XXH_RESTRICT acc, const xxh_u8* XXH_RESTRICT secret, xxh_u64 len)
{
    return XXH3_mergeAccsConstexpr(acc, secret + XXH_SECRET_MERGEACCS_START, len * XXH_PRIME64_1);
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH64_hash_t
XXH3_hashLong_64b_internalConstexpr(
    const T* XXH_RESTRICT input, size_t len,
    const xxh_u8* XXH_RESTRICT secret, size_t secretSize)
{
    XXH_ALIGN(XXH_ACC_ALIGN) xxh_u64 acc[XXH_ACC_NB] = XXH3_INIT_ACC;

    XXH3_hashLong_internal_loopConstexpr(acc, input, len, secret, secretSize);

    /* converge into final hash */
    //XXH_STATIC_ASSERT(sizeof(acc) == 64);
    //XXH_ASSERT(secretSize >= sizeof(acc) + XXH_SECRET_MERGEACCS_START);
    return XXH3_finalizeLong_64bConstexpr(acc, secret, static_cast<xxh_u64>(len));
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH64_hash_t
XXH3_hashLong_64b_withSeed_internalConstexpr(
    const T* input, size_t len,
    XXH64_hash_t seed)
{
    if (seed == 0)
        return XXH3_hashLong_64b_internalConstexpr(input, len, XXH3_kSecret, sizeof(XXH3_kSecret));

    {   XXH_ALIGN(XXH_SEC_ALIGN) xxh_u8 secret[XXH_SECRET_DEFAULT_SIZE]{};
        XXH3_initCustomSecret_scalarConstexpr(secret, seed);
        return XXH3_hashLong_64b_internalConstexpr(input, len, secret, sizeof(secret));
    }
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH64_hash_t
XXH3_hashLong_64b_withSeedConstexpr(
    const T* XXH_RESTRICT input, size_t len,
    XXH64_hash_t seed, const xxh_u8* XXH_RESTRICT secret, size_t secretLen)
{
    (void)secret; (void)secretLen;
    return XXH3_hashLong_64b_withSeed_internalConstexpr(input, len, seed);
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH64_hash_t
XXH3_64bits_internalConstexpr(
    const T* XXH_RESTRICT input, size_t len,
    XXH64_hash_t seed64, const xxh_u8* XXH_RESTRICT secret, size_t secretLen)
{
    //XXH_ASSERT(secretLen >= XXH3_SECRET_SIZE_MIN);
    /*
     * If an action is to be taken if `secretLen` condition is not respected,
     * it should be done here.
     * For now, it's a contract pre-condition.
     * Adding a check and a branch here would cost performance at every hash.
     * Also, note that function signature doesn't offer room to return an error.
     */
    if (len <= 16)
        return XXH3_len_0to16_64bConstexpr(input, len, secret, seed64);
    if (len <= 128)
        return XXH3_len_17to128_64bConstexpr(input, len, secret, secretLen, seed64);
    if (len <= XXH3_MIDSIZE_MAX)
        return XXH3_len_129to240_64bConstexpr(input, len, secret, secretLen, seed64);

    return XXH3_hashLong_64b_withSeedConstexpr(input, len, seed64, secret, secretLen);
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH_PUREF xxh_u32
XXH32_endian_alignConstexpr(const T* input, size_t len, xxh_u32 seed)
{
    xxh_u32 h32 = 0;

    //if (input==NULL) XXH_ASSERT(len == 0);

    if (len>=16) {
        xxh_u32 acc[4]{};
        XXH32_initAccsConstexpr(acc, seed);

        input = XXH32_consumeLongConstexpr(acc, input, len);

        h32 = XXH32_mergeAccsConstexpr(acc);
    } else {
        h32 = seed + XXH_PRIME32_5;
    }

    h32 += static_cast<xxh_u32>(len);

    return XXH32_finalizeConstexpr(h32, input, len&15);
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH32_hash_t XXH32Constexpr(const T* input, const usize len, const u32 seed) noexcept
{
    return XXH32_endian_alignConstexpr(input, len, seed);
}

template <typename T>
constexpr XXH_FORCE_INLINE XXH64_hash_t XXH3_64bits_withSeedConstexpr(const T* input, const usize length, const u64 seed) noexcept
{
    return XXH3_64bits_internalConstexpr(input, length, seed, XXH3_kSecret, sizeof(XXH3_kSecret));
}

} // namespace _HashDetails
} // namespace Algorithms
} // namespace Alt

#endif // __ALT_STD_ALGORITHMS_HASH__