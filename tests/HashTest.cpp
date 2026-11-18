#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Algorithms/Algorithms.h"

TEST(Hash, Similarity) {

    iferr_scope_on_err {

        FAIL();
    };

    using HashCosntexpr = Alt::_AlgorithmsDetails::HashConstexprImpl<u8>;
    using Hash32Cosntexpr = Alt::_AlgorithmsDetails::HashConstexprImpl<u8, Alt::_AlgorithmsDetails::PlatformTrait::x32>;
    using Hash64Cosntexpr = Alt::_AlgorithmsDetails::HashConstexprImpl<u8, Alt::_AlgorithmsDetails::PlatformTrait::x64>;

    constexpr usize TEST_CNT = 10'000;
    constexpr usize TEST_STR_MAX_SIZE = 1'000;

    for (usize i = 0; i < TEST_CNT; ++i) {

        const Alt::String str = AllocateStr(0, TEST_STR_MAX_SIZE);

        const usize trueHash = Alt::Algorithms::Hash(str.Data(), str.DataEnd(), 0);
        const u32 trueHash32 = Alt::Algorithms::Hash32(str.Data(), str.DataEnd(), 0);
        const u64 trueHash64 = Alt::Algorithms::Hash64(str.Data(), str.DataEnd(), 0);

        const usize hash = HashCosntexpr::Hash(str.Data(), str.GetSizeBytes(), 0);
        const u32 hash32 = Hash32Cosntexpr::Hash(str.Data(), str.GetSizeBytes(), 0);
        const u64 hash64 = Hash64Cosntexpr::Hash(str.Data(), str.GetSizeBytes(), 0);

        EXPECT_EQ((u64)trueHash, (u64)hash);
        EXPECT_EQ((u64)trueHash32, (u64)hash32);
        EXPECT_EQ((u64)trueHash64, (u64)hash64);
    }
}

TEST(Hash, Constexpr) {

    {
        ALT_MAYBE_UNUSED const usize hash0 = ALT_HASH("hash0"_s);
        ALT_MAYBE_UNUSED const usize hash1 = ALT_HASH("Hello World"_s);

        ALT_MAYBE_UNUSED const usize hash2 = ALT_HASH("It is compiled!"_s);
        ALT_MAYBE_UNUSED const usize hash3 = ALT_HASH("Must be the same as 'Alt::Algorithms::Hash'"_s);
        ALT_MAYBE_UNUSED const usize hash4 = ALT_HASH("Only for bytes!"_s);
    }

    {
        ALT_MAYBE_UNUSED const u64 hash0 = ALT_HASH64("hash0"_s);
        ALT_MAYBE_UNUSED const u64 hash1 = ALT_HASH64("Hello World"_s);

        ALT_MAYBE_UNUSED const u64 hash2 = ALT_HASH64("It is compiled!"_s);
        ALT_MAYBE_UNUSED const u64 hash3 = ALT_HASH64("Must be the same as 'Alt::Algorithms::Hash64'"_s);
        ALT_MAYBE_UNUSED const u64 hash4 = ALT_HASH64("Only for bytes!"_s);
    }

    {
        ALT_MAYBE_UNUSED const u32 hash0 = ALT_HASH32("hash0"_s);
        ALT_MAYBE_UNUSED const u32 hash1 = ALT_HASH32("Hello World"_s);

        ALT_MAYBE_UNUSED const u32 hash2 = ALT_HASH32("It is compiled!"_s);
        ALT_MAYBE_UNUSED const u32 hash3 = ALT_HASH32("Must be the same as 'Alt::Algorithms::Hash32'"_s);
        ALT_MAYBE_UNUSED const u32 hash4 = ALT_HASH32("Only for bytes!"_s);
    }

    {
        ALT_MAYBE_UNUSED constexpr usize hash0 = "hash0"_hash;
        ALT_MAYBE_UNUSED constexpr usize hash1 = "Hello World"_hash;

        ALT_MAYBE_UNUSED constexpr usize hash2 = "It is compiled!"_hash;
        ALT_MAYBE_UNUSED constexpr usize hash3 = "Must be the same as 'Alt::Algorithms::Hash32'"_hash;
        ALT_MAYBE_UNUSED constexpr usize hash4 = "Only for bytes!"_hash;
    }
}
