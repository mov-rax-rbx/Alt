#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE
#define ALT_USE_CUSTOM_128

#include "Utils.h"

#include "../alt-std/src/String/StaticString.h"

#include "../alt-std/src/Utils/Num.h"
#include "../alt-std/src/Random/Xoshiro256.h"

namespace {

template <typename T>
inline usize PopcountStable(T x) {

    usize count = 0;
    while (x != 0) {
        count += x & 1;
        x >>= 1;
    }

    return count;
}

template <typename T>
inline usize DigitCountStable(T x) {

    usize result = 0;

    do {

        result += 1;
        x /= 10;
    } while (x != 0);

    return result;
}

inline usize DigitCountStable(u128 num) noexcept {

    constexpr static u64 DIVISOR = 10000000000000000000UL;
    constexpr static usize MAX_DIGIT_COUNT_PER_DIVISOR = 19;

    usize result = 0;

    {
        const u64 val = static_cast<u64>(num % DIVISOR);
        num /= DIVISOR;

        if (num == 0) {
            return result + Alt::DigitCount(val);
        }

        result += MAX_DIGIT_COUNT_PER_DIVISOR;
    }

    {
        const u64 val = static_cast<u64>(num % DIVISOR);
        num /= DIVISOR;

        if (num == 0) {
            return result + Alt::DigitCount(val);
        }

        result += MAX_DIGIT_COUNT_PER_DIVISOR;
    }

    {
        const u64 val = static_cast<u64>(num);

        const usize cnt = Alt::DigitCount(val);
        result += cnt;

        return result;
    }
}

inline usize DigitCountStable(const s128 num) noexcept {

    const u128 x = Alt::IsSignPositive(num)
        ? Alt::Memory::Transmute<s128, u128>(num)
        : ~Alt::Memory::Transmute<s128, u128>(num) + 1;

    return DigitCountStable(x);
}

}

TEST(Num, PopcntConstexprFuzz) {

    constexpr usize TEST_CNT = 1'000'000;

    for (usize i = 0; i < TEST_CNT; ++i) {

        const u32 num = GetNum<u32>();
        EXPECT_EQ(Alt::_NumDetails::PopcountConstexpr(num), PopcountStable(num));
    }

    for (usize i = 0; i < TEST_CNT; ++i) {

        const u64 num = GetNum<u64>();
        EXPECT_EQ(Alt::_NumDetails::PopcountConstexpr(num), PopcountStable(num));
    }
}

TEST(Num, PopcntFuzz) {

    constexpr usize TEST_CNT = 1'000'000;

    for (usize i = 0; i < TEST_CNT; ++i) {

        const u32 num = GetNum<u32>();
        EXPECT_EQ(Alt::Popcount(num), PopcountStable(num));
    }

    for (usize i = 0; i < TEST_CNT; ++i) {

        const u64 num = GetNum<u64>();
        EXPECT_EQ(Alt::Popcount(num), PopcountStable(num));
    }
}

TEST(Num, Count0BitsRightConstexpr) {

    EXPECT_EQ(Alt::_NumDetails::Count0BitsRightConstexpr((u32)0b11111111111111111111111111111111), (u32)0);
    EXPECT_EQ(Alt::_NumDetails::Count0BitsRightConstexpr((u32)0b11111111111111111111111111111110), (u32)1);
    EXPECT_EQ(Alt::_NumDetails::Count0BitsRightConstexpr((u64)0b11111111111111111111111111111111), (u64)0);
    EXPECT_EQ(Alt::_NumDetails::Count0BitsRightConstexpr((u64)0b11111111111111111111111111111110), (u64)1);

    EXPECT_EQ(Alt::_NumDetails::Count0BitsRightConstexpr(Alt::Desc<u32>::Max()), (u32)0);
    EXPECT_EQ(Alt::_NumDetails::Count0BitsRightConstexpr(Alt::Desc<u64>::Max()), (u64)0);

    EXPECT_EQ(Alt::_NumDetails::Count0BitsRightConstexpr((u32)0), (u32)32);
    EXPECT_EQ(Alt::_NumDetails::Count0BitsRightConstexpr((u64)0), (u64)64);
}

TEST(Num, Count0BitsRight) {

    EXPECT_EQ(Alt::Count0BitsRight((u32)0b11111111111111111111111111111111), (u32)0);
    EXPECT_EQ(Alt::Count0BitsRight((u32)0b11111111111111111111111111111110), (u32)1);
    EXPECT_EQ(Alt::Count0BitsRight((u64)0b11111111111111111111111111111111), (u64)0);
    EXPECT_EQ(Alt::Count0BitsRight((u64)0b11111111111111111111111111111110), (u64)1);

    EXPECT_EQ(Alt::Count0BitsRight(Alt::Desc<u32>::Max()), (u32)0);
    EXPECT_EQ(Alt::Count0BitsRight(Alt::Desc<u64>::Max()), (u64)0);

    EXPECT_EQ(Alt::Count0BitsRight((u32)0), (u32)32);
    EXPECT_EQ(Alt::Count0BitsRight((u64)0), (u64)64);
}

TEST(Num, Count0BitsLeftConstexpr) {

    EXPECT_EQ(Alt::_NumDetails::Count0BitsLeftConstexpr((u32)0b11111111111111111111111111111111), (u32)0);
    EXPECT_EQ(Alt::_NumDetails::Count0BitsLeftConstexpr((u32)0b00111111111111111111111111111111), (u32)2);
    EXPECT_EQ(Alt::_NumDetails::Count0BitsLeftConstexpr((u64)0b11111111111111111111111111111111), (u64)(63 - 31));
    EXPECT_EQ(Alt::_NumDetails::Count0BitsLeftConstexpr((u64)0b00111111111111111111111111111111), (u64)(63 - (31 - 2)));
    EXPECT_EQ(Alt::_NumDetails::Count0BitsLeftConstexpr((u64)0b00111111111111111111111111111101), (u64)(63 - (31 - 2)));

    EXPECT_EQ(Alt::_NumDetails::Count0BitsLeftConstexpr(Alt::Desc<u32>::Max()), (u32)0);
    EXPECT_EQ(Alt::_NumDetails::Count0BitsLeftConstexpr(Alt::Desc<u64>::Max()), (u64)0);

    EXPECT_EQ(Alt::_NumDetails::Count0BitsLeftConstexpr((u32)0), (u32)32);
    EXPECT_EQ(Alt::_NumDetails::Count0BitsLeftConstexpr((u64)0), (u64)64);
}

TEST(Num, Count0BitsLeft) {

    EXPECT_EQ(Alt::Count0BitsLeft((u32)0b11111111111111111111111111111111), (u32)0);
    EXPECT_EQ(Alt::Count0BitsLeft((u32)0b00111111111111111111111111111111), (u32)2);
    EXPECT_EQ(Alt::Count0BitsLeft((u64)0b11111111111111111111111111111111), (u64)(63 - 31));
    EXPECT_EQ(Alt::Count0BitsLeft((u64)0b00111111111111111111111111111111), (u64)(63 - (31 - 2)));
    EXPECT_EQ(Alt::Count0BitsLeft((u64)0b00111111111111111111111111111101), (u64)(63 - (31 - 2)));

    EXPECT_EQ(Alt::Count0BitsLeft(Alt::Desc<u32>::Max()), (u32)0);
    EXPECT_EQ(Alt::Count0BitsLeft(Alt::Desc<u64>::Max()), (u64)0);

    EXPECT_EQ(Alt::Count0BitsLeft((u32)0), (u32)32);
    EXPECT_EQ(Alt::Count0BitsLeft((u64)0), (u64)64);
}

TEST(Num, RoundUpToPowerOf2) {

    EXPECT_EQ(Alt::RoundUpToPowerOf2((u32)0), (u32)1);
    EXPECT_EQ(Alt::RoundUpToPowerOf2((u32)33), (u32)64);
    EXPECT_EQ(Alt::RoundUpToPowerOf2((u32)64), (u32)64);
    EXPECT_EQ(Alt::RoundUpToPowerOf2(Alt::Desc<u32>::Max() - 1), Alt::Desc<u32>::Max());
    EXPECT_EQ(Alt::RoundUpToPowerOf2(Alt::Desc<u32>::Max()), Alt::Desc<u32>::Max());

    EXPECT_EQ(Alt::RoundUpToPowerOf2((u64)0), (u64)1);
    EXPECT_EQ(Alt::RoundUpToPowerOf2((u64)33), (u64)64);
    EXPECT_EQ(Alt::RoundUpToPowerOf2((u64)64), (u64)64);
    EXPECT_EQ(Alt::RoundUpToPowerOf2(Alt::Desc<u64>::Max() - 1), Alt::Desc<u64>::Max());
    EXPECT_EQ(Alt::RoundUpToPowerOf2(Alt::Desc<u64>::Max()), Alt::Desc<u64>::Max());
}

TEST(Num, RoundUpToPowerOf2PreserveZero) {

    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero((u32)0), (u32)0);
    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero((u32)33), (u32)64);
    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero((u32)64), (u32)64);
    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero(Alt::Desc<u32>::Max() - 1), Alt::Desc<u32>::Max());
    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero(Alt::Desc<u32>::Max()), Alt::Desc<u32>::Max());

    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero((u64)0), (u64)0);
    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero((u64)33), (u64)64);
    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero((u64)64), (u64)64);
    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero(Alt::Desc<u64>::Max() - 1), Alt::Desc<u64>::Max());
    EXPECT_EQ(Alt::RoundUpToPowerOf2PreserveZero(Alt::Desc<u64>::Max()), Alt::Desc<u64>::Max());
}

TEST(Num, RoundDownToPowerOf2) {

    EXPECT_EQ(Alt::RoundDownToPowerOf2((u32)0), (u32)0);
    EXPECT_EQ(Alt::RoundDownToPowerOf2((u32)33), (u32)32);
    EXPECT_EQ(Alt::RoundDownToPowerOf2((u32)64), (u32)64);
    EXPECT_EQ(Alt::RoundDownToPowerOf2(Alt::Desc<u32>::Max() - 1), ((u32)1 << 31));
    EXPECT_EQ(Alt::RoundDownToPowerOf2(Alt::Desc<u32>::Max()), ((u32)1 << 31));

    EXPECT_EQ(Alt::RoundDownToPowerOf2((u64)0), (u64)0);
    EXPECT_EQ(Alt::RoundDownToPowerOf2((u64)33), (u64)32);
    EXPECT_EQ(Alt::RoundDownToPowerOf2((u64)64), (u64)64);
    EXPECT_EQ(Alt::RoundDownToPowerOf2(Alt::Desc<u64>::Max() - 1), ((u64)1 << 63));
    EXPECT_EQ(Alt::RoundDownToPowerOf2(Alt::Desc<u64>::Max()), ((u64)1 << 63));
}

TEST(Num, Log2) {

    EXPECT_EQ(Alt::Log2((u32)0), (u32)0);
    EXPECT_EQ(Alt::Log2((u32)1), (u32)0);
    EXPECT_EQ(Alt::Log2((u32)33), (u32)5);
    EXPECT_EQ(Alt::Log2(Alt::RoundUpToPowerOf2((u32)33)), (u32)6);
    EXPECT_EQ(Alt::Log2(Alt::Desc<u32>::Max()), (u32)31);
    EXPECT_EQ(Alt::Log2(Alt::RoundDownToPowerOf2(Alt::Desc<u32>::Max())), (u32)31);

    EXPECT_EQ(Alt::Log2((u64)0), (u64)0);
    EXPECT_EQ(Alt::Log2((u64)1), (u64)0);
    EXPECT_EQ(Alt::Log2((u64)33), (u64)5);
    EXPECT_EQ(Alt::Log2(Alt::RoundUpToPowerOf2((u64)33)), (u64)6);
    EXPECT_EQ(Alt::Log2(Alt::Desc<u64>::Max()), (u64)63);
    EXPECT_EQ(Alt::Log2(Alt::RoundDownToPowerOf2(Alt::Desc<u64>::Max())), (u64)63);
}

TEST(Num, DigitCount) {

    static const usize ITERS = 10'000'000;

    Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

    for (usize i = 0; i < ITERS; ++i) {

        const u32 num = static_cast<u32>(rnd.GenerateU64(0, Alt::Desc<u32>::Max()));

        const usize res0 = DigitCountStable(num);
        const usize res1 = Alt::DigitCount(num);

        EXPECT_EQ(res0, res1);
    }

    for (usize i = 0; i < ITERS; ++i) {

        const u64 num = rnd.GenerateU64();

        const usize res0 = DigitCountStable(num);
        const usize res1 = Alt::DigitCount(num);

        EXPECT_EQ(res0, res1);
    }

    for (usize i = 0; i < ITERS; ++i) {

        const s32 num = static_cast<s32>(rnd.GenerateS64(Alt::Desc<s32>::Min(), Alt::Desc<s32>::Max()));

        const usize res0 = DigitCountStable(num);
        const usize res1 = Alt::DigitCount(num);

        EXPECT_EQ(res0, res1);
    }

    for (usize i = 0; i < ITERS; ++i) {

        const s64 num = rnd.GenerateS64();

        const usize res0 = DigitCountStable(num);
        const usize res1 = Alt::DigitCount(num);

        EXPECT_EQ(res0, res1);
    }
}

TEST(Num, IsOverflowCheckAdd) {

    EXPECT_TRUE(Alt::IsOverflowCheckAdd(Alt::Desc<u64>::Max(), 2));
    EXPECT_TRUE(Alt::IsOverflowCheckAdd(Alt::Desc<u32>::Max(), 2));
    EXPECT_TRUE(Alt::IsOverflowCheckAdd(Alt::Desc<u16>::Max(), 2));
    EXPECT_TRUE(Alt::IsOverflowCheckAdd(Alt::Desc<u8>::Max(), 2));
}

TEST(Num, IsUnderflowCheckSub) {

    EXPECT_TRUE(Alt::IsUnderflowCheckSub<u64>(0, 2));
    EXPECT_TRUE(Alt::IsUnderflowCheckSub<u32>(0, 2));
    EXPECT_TRUE(Alt::IsUnderflowCheckSub<u16>(0, 2));
    EXPECT_TRUE(Alt::IsUnderflowCheckSub<u8>(0, 2));
}

TEST(Num, AddWrap) {

    EXPECT_EQ(Alt::AddWrap<usize>(1, 2), 3);
    EXPECT_EQ(Alt::AddWrap<usize>(Alt::Desc<usize>::Max(), 1), Alt::Desc<usize>::Max());
    EXPECT_EQ(Alt::AddWrap<usize>(Alt::Desc<usize>::Max(), Alt::Desc<usize>::Max()), Alt::Desc<usize>::Max());
}

TEST(Num, SubWrap) {

    EXPECT_EQ(Alt::SubWrap<usize>(1, 2), 0);
    EXPECT_EQ(Alt::SubWrap<usize>(Alt::Desc<usize>::Max(), 1), Alt::Desc<usize>::Max() - 1);
    EXPECT_EQ(Alt::SubWrap<usize>(Alt::Desc<usize>::Max(), Alt::Desc<usize>::Max()), 0);
}

TEST(Num, IsSignPositive) {

    EXPECT_TRUE(Alt::IsSignPositive(0.0f));
    EXPECT_TRUE(Alt::IsSignPositive(0.0));

    EXPECT_FALSE(Alt::IsSignPositive(-0.0f));
    EXPECT_FALSE(Alt::IsSignPositive(-0.0));
}

TEST(Num, RoundUpWrap) {

    EXPECT_EQ(Alt::RoundUpWrap(0u, 8), 8u);
    EXPECT_EQ(Alt::RoundUpWrapPreserveZero(0u, 8), 0u);

    EXPECT_EQ(Alt::RoundUpWrap(1u, 8), 8u);
    EXPECT_EQ(Alt::RoundUpWrap(16u, 8), 16u);
    EXPECT_EQ(Alt::RoundUpWrap(16u, 3), 18u);

    EXPECT_EQ(Alt::RoundUpWrap(13u, 0), 13u);
    EXPECT_EQ(Alt::RoundUpWrap(13u, 1), 13u);
}

TEST(Num, RoundDown) {

    EXPECT_EQ(Alt::RoundDown(0u, 8), 0u);
    EXPECT_EQ(Alt::RoundDown(1u, 8), 0u);
    EXPECT_EQ(Alt::RoundDown(16u, 8), 16u);
    EXPECT_EQ(Alt::RoundDown(16u, 3), 15u);

    EXPECT_EQ(Alt::RoundDown(13u, 0), 13u);
    EXPECT_EQ(Alt::RoundDown(13u, 1), 13u);
}

TEST(Num, Abs) {

    EXPECT_EQ(Alt::Abs(0.0), 0.0);
    EXPECT_EQ(Alt::Abs(0.0f), 0.0f);

    EXPECT_EQ(Alt::Abs(-0.0), 0.0);
    EXPECT_EQ(Alt::Abs(-0.0f), 0.0f);

    EXPECT_EQ(Alt::Abs(-4.0), 4.0);
    EXPECT_EQ(Alt::Abs(-4.0f), 4.0f);

    EXPECT_EQ(Alt::Abs(-8.0), 8.0);
    EXPECT_EQ(Alt::Abs(-8.0f), 8.0f);
}

TEST(Num, MaxMin) {

    EXPECT_EQ(Alt::Max(0, -1), 0);
    EXPECT_EQ(Alt::Min(0, -1), -1);

    EXPECT_EQ(Alt::Max(0.0, 1.0), 1.0);
    EXPECT_EQ(Alt::Max(0.0f, 1.0f), 1.0f);

    EXPECT_EQ(Alt::Min(0.0, 1.0), 0.0);
    EXPECT_EQ(Alt::Min(0.0f, 1.0f), 0.0f);

    EXPECT_EQ(Alt::Max(0.0001, 0.0002), 0.0002);
    EXPECT_EQ(Alt::Max(0.0001f, 0.0002f), 0.0002f);

    EXPECT_EQ(Alt::Max(0.0001, -0.0002), 0.0001);
    EXPECT_EQ(Alt::Max(0.0001f, -0.0002f), 0.0001f);
}

TEST(Num, Clamp) {

    EXPECT_EQ(Alt::Clamp(2, -1, 1), 1);
    EXPECT_EQ(Alt::Clamp(0, -1, 1), 0);

    EXPECT_EQ(Alt::Clamp(-2.0, 0.0, 1.0), 0.0);
    EXPECT_EQ(Alt::Clamp(2.0, 0.0, 1.0), 1.0);
    EXPECT_EQ(Alt::Clamp(0.4, 0.0, 1.0), 0.4);
}

TEST(Num, ConvertOrdered_SignedToUnsigned_UnsignedToSigned) {

    static const usize NUM_ITER = 20;

    {
        {
            const usize num0 = 0;
            const usize num1 = 3;

            const ssize snum0 = Alt::ConvertUnsignedToSignedOrdered(num0);
            const ssize snum1 = Alt::ConvertUnsignedToSignedOrdered(num1);

            EXPECT_TRUE(snum0 < snum1);
        }

        {
            const usize num0 = Alt::Desc<usize>::Max();
            const usize num1 = Alt::Desc<usize>::Max();

            const ssize snum0 = Alt::ConvertUnsignedToSignedOrdered(num0);
            const ssize snum1 = Alt::ConvertUnsignedToSignedOrdered(num1);

            EXPECT_TRUE(snum0 == snum1);
        }
    }

    {
        {
            const ssize num0 = -1;
            const ssize num1 = 1;

            const usize unum0 = Alt::ConvertSignedToUnsignedOrdered(num0);
            const usize unum1 = Alt::ConvertSignedToUnsignedOrdered(num1);

            EXPECT_TRUE(unum0 < unum1);
        }

        {
            const ssize num0 = Alt::Desc<ssize>::Max();
            const ssize num1 = Alt::Desc<ssize>::Max();

            const ssize snum0 = Alt::ConvertSignedToUnsignedOrdered(num0);
            const ssize snum1 = Alt::ConvertSignedToUnsignedOrdered(num1);

            EXPECT_TRUE(snum0 == snum1);
        }
    }

    for (usize i = 0; i < NUM_ITER; ++i) {

        const usize num = GetNum<usize>();

        const ssize snum = Alt::ConvertUnsignedToSignedOrdered(num);
        const usize unum = Alt::ConvertSignedToUnsignedOrdered(snum);

        EXPECT_EQ(num, unum);
    }

    for (usize i = 0; i < NUM_ITER; ++i) {

        const ssize num = GetNum<ssize>();

        const usize unum = Alt::ConvertSignedToUnsignedOrdered(num);
        const ssize snum = Alt::ConvertUnsignedToSignedOrdered(unum);

        EXPECT_EQ(num, snum);
    }
}

TEST(Num, ConvertOrdered_FloatToUnsigned_FloatToUnsigned) {

    {
        {
            const f32 num0 = -1.0f;
            const f32 num1 = 1.0f;

            const s32 snum0 = Alt::ConvertFloatToSignedOrdered(num0);
            const s32 snum1 = Alt::ConvertFloatToSignedOrdered(num1);

            EXPECT_TRUE(snum0 < snum1);
        }

        {
            const f32 num0 = 0.9f;
            const f32 num1 = 1.0f;

            const s32 snum0 = Alt::ConvertFloatToSignedOrdered(num0);
            const s32 snum1 = Alt::ConvertFloatToSignedOrdered(num1);

            EXPECT_TRUE(snum0 < snum1);
        }

        {
            const f32 num0 = Alt::Desc<f32>::Max();
            const f32 num1 = Alt::Desc<f32>::Max();

            const s32 snum0 = Alt::ConvertFloatToSignedOrdered(num0);
            const s32 snum1 = Alt::ConvertFloatToSignedOrdered(num1);

            EXPECT_TRUE(snum0 == snum1);
        }
    }

    {
        {
            const f32 num0 = 0.0f;
            const f32 num1 = 3.0f;

            const u32 unum0 = Alt::ConvertFloatToUnsignedOrdered(num0);
            const u32 unum1 = Alt::ConvertFloatToUnsignedOrdered(num1);

            EXPECT_TRUE(unum0 < unum1);
        }

        {
            const f32 num0 = 2.9f;
            const f32 num1 = 3.0f;

            const u32 unum0 = Alt::ConvertFloatToUnsignedOrdered(num0);
            const u32 unum1 = Alt::ConvertFloatToUnsignedOrdered(num1);

            EXPECT_TRUE(unum0 < unum1);
        }

        {
            const f32 num0 = Alt::Desc<f32>::Max();
            const f32 num1 = Alt::Desc<f32>::Max();

            const u32 unum0 = Alt::ConvertFloatToUnsignedOrdered(num0);
            const u32 unum1 = Alt::ConvertFloatToUnsignedOrdered(num1);

            EXPECT_TRUE(unum0 == unum1);
        }
    }

    {
        {
            const f64 num0 = -1.0;
            const f64 num1 = 1.0;

            const s64 snum0 = Alt::ConvertFloatToSignedOrdered(num0);
            const s64 snum1 = Alt::ConvertFloatToSignedOrdered(num1);

            EXPECT_TRUE(snum0 < snum1);
        }

        {
            const f64 num0 = 0.9;
            const f64 num1 = 1.0;

            const s64 snum0 = Alt::ConvertFloatToSignedOrdered(num0);
            const s64 snum1 = Alt::ConvertFloatToSignedOrdered(num1);

            EXPECT_TRUE(snum0 < snum1);
        }

        {
            const f64 num0 = Alt::Desc<f64>::Max();
            const f64 num1 = Alt::Desc<f64>::Max();

            const s64 snum0 = Alt::ConvertFloatToSignedOrdered(num0);
            const s64 snum1 = Alt::ConvertFloatToSignedOrdered(num1);

            EXPECT_TRUE(snum0 == snum1);
        }
    }

    {
        {
            const f64 num0 = 0.0;
            const f64 num1 = 3.0;

            const u64 unum0 = Alt::ConvertFloatToUnsignedOrdered(num0);
            const u64 unum1 = Alt::ConvertFloatToUnsignedOrdered(num1);

            EXPECT_TRUE(unum0 < unum1);
        }

        {
            const f64 num0 = 2.9;
            const f64 num1 = 3.0;

            const u64 unum0 = Alt::ConvertFloatToUnsignedOrdered(num0);
            const u64 unum1 = Alt::ConvertFloatToUnsignedOrdered(num1);

            EXPECT_TRUE(unum0 < unum1);
        }

        {
            const f64 num0 = Alt::Desc<f64>::Max();
            const f64 num1 = Alt::Desc<f64>::Max();

            const u64 unum0 = Alt::ConvertFloatToUnsignedOrdered(num0);
            const u64 unum1 = Alt::ConvertFloatToUnsignedOrdered(num1);

            EXPECT_TRUE(unum0 == unum1);
        }
    }
}

TEST(Num, Mod) {

    EXPECT_EQ(Alt::Mod<u64>(0, 1), 0);
    EXPECT_EQ(Alt::Mod<u64>(1, 1), 0);
    EXPECT_EQ(Alt::Mod<u64>(5, 2), 1);
    EXPECT_EQ(Alt::Mod<u64>(6, 2), 0);
    EXPECT_EQ(Alt::Mod<u64>(7, 3), 1);
    EXPECT_EQ(Alt::Mod<u64>(8, 3), 2);
    EXPECT_EQ(Alt::Mod<u64>(9, 3), 0);
    EXPECT_EQ(Alt::Mod<u64>(100, 7), 2);
    EXPECT_EQ(Alt::Mod<u64>(Alt::Desc<u64>::Max(), 2), 1);
    EXPECT_EQ(Alt::Mod<u64>(Alt::Desc<u64>::Max(), Alt::Desc<u64>::Max()), 0);
    EXPECT_EQ(Alt::Mod<u64>(Alt::Desc<u64>::Max(), Alt::Desc<u64>::Max() - 1), 1);

    EXPECT_EQ(Alt::Mod<s64>(7, 3), 1);
    EXPECT_EQ(Alt::Mod<s64>(-7, 3), 2);
    EXPECT_EQ(Alt::Mod<s64>(7, -3), -2);
    EXPECT_EQ(Alt::Mod<s64>(-7, -3), -1);
    EXPECT_EQ(Alt::Mod<s64>(8, 3), 2);
    EXPECT_EQ(Alt::Mod<s64>(-8, 3), 1);
    EXPECT_EQ(Alt::Mod<s64>(8, -3), -1);
    EXPECT_EQ(Alt::Mod<s64>(-8, -3), -2);
    EXPECT_EQ(Alt::Mod<s64>(6, 3), 0);
    EXPECT_EQ(Alt::Mod<s64>(-6, 3), 0);
    EXPECT_EQ(Alt::Mod<s64>(1, 2), 1);
    EXPECT_EQ(Alt::Mod<s64>(-1, 2), 1);
    EXPECT_EQ(Alt::Mod<s64>(Alt::Desc<s64>::Max(), 2), 1);
    EXPECT_EQ(Alt::Mod<s64>(Alt::Desc<s64>::Min(), 2), 0);
    EXPECT_EQ(Alt::Mod<s64>(Alt::Desc<s64>::Min(), 3), 1);
}

TEST(Num, Transmute) {

    u8 bytes[4]{ 0xAA, 0xBB, 0xCC, 0xDD };

    const u32 num = Alt::Memory::Transmute<u8[4], u32>(bytes);
    EXPECT_EQ(num, ALT_PLATFORM_LITTLE_ENDIAN_BOOL ? 0xDDCCBBAA : 0xAABBCCDD);

    u8 result[4]{0};
    Alt::Memory::TransmuteInto<u32, u8[4]>(num, result);

    EXPECT_EQ(result[0], bytes[0]);
    EXPECT_EQ(result[1], bytes[1]);
    EXPECT_EQ(result[2], bytes[2]);
    EXPECT_EQ(result[3], bytes[3]);
}

//TEST(Num, _u128_ReciprocalWord_2by1) {
//
//	constexpr static usize ITERS = 1'000'000;
//
//	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();
//
//	for (usize i = 0; i < ITERS; ++i) {
//
//		u64 d = (u64)i | 1;
//		d <<= Alt::Count0BitsLeftUnsafe(d);
//
//		const u64 rec0 = u128::ReciprocalWord_2by1(d);
//		const u64 rec1 = u128::ReciprocalWord_2by1_Hardware(d);
//
//		EXPECT_EQ(rec0, rec1);
//	}
//
//	for (usize i = 0; i < ITERS; ++i) {
//
//		u64 d = (Alt::Desc<u64>::Max() - (u64)i) | 1;
//		d <<= Alt::Count0BitsLeftUnsafe(d);
//
//		const u64 rec0 = u128::ReciprocalWord_2by1(Alt::Desc<u64>::Max() - (u64)i);
//		const u64 rec1 = u128::ReciprocalWord_2by1_Hardware(Alt::Desc<u64>::Max() - (u64)i);
//
//		EXPECT_EQ(rec0, rec1);
//	}
//
//	for (usize i = 0; i < ITERS; ++i) {
//
//		u64 d = rnd.GenerateU64() | 1;
//		d <<= Alt::Count0BitsLeftUnsafe(d);
//
//		const u64 rec0 = u128::ReciprocalWord_2by1(d);
//		const u64 rec1 = u128::ReciprocalWord_2by1_Hardware(d);
//
//		EXPECT_EQ(rec0, rec1);
//	}
//}

TEST(Num, _u128_DigitCount) {

    for (usize i = 0; i < 128; ++i) {

        const u128 num = u128(1) << i;
        const u128 numNext = u128(1) << (i + 1);

        const usize cnt = DigitCountStable(num);
        const usize cntNext = DigitCountStable(numNext);

        if (cnt != cntNext) {

            u128 begin = num;
            const u128 end = numNext + 1;

            u128 size = end - begin;

            while (size > 1) {

                const u128 half = size >> 1;
                size -= half;

                const u128 mid = begin + half;

                if (DigitCountStable(mid) != cntNext) {
                    begin += half;
                }
            }

            begin += size != 0 ? DigitCountStable(begin) != cntNext : 0;

            const usize cnt0 = Alt::DigitCount(begin);
            const usize cnt1 = DigitCountStable(begin);

            EXPECT_EQ(cnt0, cnt1);
        }
    }

    static const usize ITERS = 10'000'000;

    Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

    for (usize i = 0; i < ITERS; ++i) {

        const u128 num = u128::FromHighLow(rnd.GenerateU64(), rnd.GenerateU64());

        const usize cnt0 = Alt::DigitCount(num);
        const usize cnt1 = DigitCountStable(num);

        /*
         * On 'ALT_PLATFORM_X86_32' Clang, MSVC Debug this fails
         */
        EXPECT_EQ(cnt0, cnt1);
    }
}

TEST(Num, _u128_OverflowUnderflow) {
    iferr_scope;

    {
        u128 num = 1;
        for (;;) {

            Alt::MulOverflowCheck(num, 10) iferr {
                break;
            }

            num *= 10;
        }

        EXPECT_EQ(num, u128::FromHighLow(5421010862427522170ULL, 687399551400673280ULL));
    }

    Alt::Optional<u128> result = Alt::AddOverflowCheck(u128::Max() - 1, 2);
    EXPECT_FALSE(result);

    result = Alt::AddOverflowCheck(u128::Max() - 1, 1);
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, u128::Max());

    result = Alt::SubUnderflowCheck(u128(1), 2);
    EXPECT_FALSE(result);

    result = Alt::SubUnderflowCheck(u128(1), 1);
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 0);
}

TEST(Num, _u128_BitShift) {

    u128 x = 1;

    for (usize i = 0; i < 128; ++i) {

        EXPECT_TRUE(Alt::IsPowerOf2(x));
        EXPECT_TRUE(Alt::IsPowerOf2PreserveZero(x));

        if (x != 1) {

            EXPECT_FALSE(Alt::IsPowerOf2(x + 1));
            EXPECT_FALSE(Alt::IsPowerOf2PreserveZero(x + 1));
        }

        EXPECT_EQ(Alt::Popcount(x), 1);

        EXPECT_EQ(Alt::Log2(x), i);
        EXPECT_EQ(Alt::Count0BitsRight(x), i);
        EXPECT_EQ(Alt::Count0BitsLeft(x), 127 - i);

        EXPECT_EQ(x << 1, x * 2);

        x <<= 1;
    }

    EXPECT_FALSE(Alt::IsPowerOf2(u128{ 0 }));
    EXPECT_TRUE(Alt::IsPowerOf2PreserveZero(u128{ 0 }));

    EXPECT_EQ(Alt::Popcount(Alt::Desc<u128>::Max()), Alt::Desc<u128>::Bits());
}

TEST(Num, _u128) {
    iferr_scope;

    constexpr static usize ITERS = 1'000;

    Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

    for (usize i = 0; i < ITERS; ++i) {

        const u128 orig = u128::FromHighLow(rnd.GenerateU64(), rnd.GenerateU64());
        const u64 rhs = rnd.GenerateU64(1, 256);

        const u128 numMul = Alt::MulOverflowCheck(orig, rhs) iferr {

            EXPECT_LT(numMul, orig);

            continue;
        }

        const u128 numMulDiv = numMul / rhs;
        const u128 numMulRem = numMul % rhs;

        EXPECT_EQ(numMulDiv, orig);
        EXPECT_EQ(numMulRem, 0);
    }

    for (usize i = 0; i < ITERS; ++i) {

        const u128 orig = u128::FromHighLow(0, rnd.GenerateU64());
        const u128 rhs = u128::FromHighLow(rnd.GenerateU64(0, 2), rnd.GenerateU64(1, 128));

        const u128 numMul = Alt::MulOverflowCheck(orig, rhs) iferr {

            EXPECT_LT(numMul, orig);

            continue;
        }

        const u128 numMulDiv = numMul / rhs;
        const u128 numMulRem = numMul % rhs;

        EXPECT_EQ(numMulDiv, orig);
        EXPECT_EQ(numMulRem, 0);
    }

    EXPECT_EQ((u128)0 - 1, Alt::Desc<u128>::Max());
    EXPECT_EQ((u128)((s128)0 - 1), Alt::Desc<u128>::Max());

    EXPECT_EQ(Alt::Desc<u128>::Max() & Alt::Desc<u128>::Max(), Alt::Desc<u128>::Max());
    EXPECT_EQ(Alt::Desc<u128>::Max() | Alt::Desc<u128>::Max(), Alt::Desc<u128>::Max());
    EXPECT_EQ(Alt::Desc<u128>::Max() ^ Alt::Desc<u128>::Max(), 0);

    EXPECT_EQ((u128)0 / 1, 0);
    EXPECT_EQ((u128)0 % 1, 0);
    EXPECT_EQ((u128)0 / (u128)1, 0);
    EXPECT_EQ((u128)0 % (u128)1, 0);

    EXPECT_EQ((u128)1 / 1, 1);
    EXPECT_EQ((u128)1 % 1, 0);
    EXPECT_EQ((u128)1 / (u128)1, 1);
    EXPECT_EQ((u128)1 % (u128)1, 0);

    EXPECT_EQ(Alt::Desc<u128>::Max() / 1, Alt::Desc<u128>::Max());
    EXPECT_EQ(Alt::Desc<u128>::Max() % 1, 0);
    EXPECT_EQ(Alt::Desc<u128>::Max() / (u128)1, Alt::Desc<u128>::Max());
    EXPECT_EQ(Alt::Desc<u128>::Max() % (u128)1, 0);

    EXPECT_EQ(Alt::Desc<u128>::Max() / Alt::Desc<u128>::Max(), 1);
    EXPECT_EQ(Alt::Desc<u128>::Max() % Alt::Desc<u128>::Max(), 0);
}

TEST(Num, _u128_Float) {

    Alt::StaticString<256> str;

    {
        EXPECT_EQ((f32)Alt::Desc<u128>::Max(), Alt::Desc<f32>::Inf());
        EXPECT_EQ((f32)(Alt::Desc<u128>::Max() - (u128(1) << (127 - FLT_MANT_DIG - 1))), Alt::Desc<f32>::Inf());

        EXPECT_EQ((f32)(u128(0)), 0.0f);
        EXPECT_EQ((f32)(Alt::Desc<u128>::Max() - (u128(1) << (127 - FLT_MANT_DIG))), FLT_MAX);

        EXPECT_EQ((u128)(f32)(u128(1) << 64), u128(1) << 64);

        EXPECT_EQ((f32)u128::FromHighLow(9223372036854775807ULL, 18446744073709551615ULL), 170141183460469231731687303715884105727.0f);

        str.Format("{.32}", (f32)(u128(1) << 64));
        EXPECT_EQ(str, "18446744073709551616"_s);

        str.Format("{}", u128(18446744073709551616.0f));
        EXPECT_EQ(str, "18446744073709551616"_s);

        str.Format("{.32}", (f32)((u128(1) << 64) * 5));
        EXPECT_EQ(str, "92233720368547758080"_s);

        EXPECT_EQ((f32)(u128(1) << 64), (f32)(u64(1) << 32) * (f32)(u64(1) << 32));
    }

    {
        EXPECT_EQ((f64)(u128(0)), 0.0);
        EXPECT_NE((f64)Alt::Desc<u128>::Max(), Alt::Desc<f64>::Inf());

        EXPECT_EQ((u128)(f64)(u128(1) << 64), u128(1) << 64);

        EXPECT_EQ((f64)u128::FromHighLow(9223372036854775807ULL, 18446744073709551615ULL), 170141183460469231731687303715884105727.0);

        str.Format("{.32}", (f64)(u128(1) << 64));
        EXPECT_EQ(str, "18446744073709551616"_s);

        str.Format("{}", u128(18446744073709551616.0));
        EXPECT_EQ(str, "18446744073709551616"_s);

        str.Format("{.32}", (f64)((u128(1) << 64) * 5));
        EXPECT_EQ(str, "92233720368547758080"_s);

        EXPECT_EQ((f64)(u128(1) << 64), (f64)(u64(1) << 32) * (f64)(u64(1) << 32));
    }
}

TEST(Num, _s128_DigitCount) {

    for (usize i = 0; i < 127; ++i) {

        const s128 num = s128(1) << i;
        const s128 numNext = s128(1) << (i + 1);

        const usize cnt = DigitCountStable(num);
        const usize cntNext = DigitCountStable(numNext);

        if (cnt != cntNext) {

            s128 begin = num;
            const s128 end = numNext + 1;

            s128 size = end - begin;

            while (size > 1) {

                const s128 half = size >> 1;
                size -= half;

                const s128 mid = begin + half;

                if (DigitCountStable(mid) != cntNext) {
                    begin += half;
                }
            }

            begin += size != 0 ? DigitCountStable(begin) != cntNext : 0;

            const usize cnt0 = Alt::DigitCount(begin);
            const usize cnt1 = DigitCountStable(begin);

            EXPECT_EQ(cnt0, cnt1);
        }
    }

    static const usize ITERS = 10'000'000;

    Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

    for (usize i = 0; i < ITERS; ++i) {

        const s128 num = s128::FromHighLow(rnd.GenerateU64(), rnd.GenerateU64());

        const usize cnt0 = Alt::DigitCount(num);
        const usize cnt1 = DigitCountStable(num);

        EXPECT_EQ(cnt0, cnt1);
    }
}

TEST(Num, _s128) {

    constexpr static usize ITERS = 1'000;

    Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

    for (usize i = 0; i < ITERS; ++i) {

        const s128 max = s128::Max() / 128;
        const s128 orig = s128::FromHighLow((s64)rnd.GenerateU64(0, (u64)max.GetHigh()), rnd.GenerateU64(0, max.GetLow()));

        s64 rhs = static_cast<s64>(rnd.GenerateU64(0, 256)) - 128;
        rhs += rhs == 0 ? 1 : 0;

        const s128 numMul = orig * rhs;

        const s128 numMulDiv = numMul / rhs;
        const s128 numMulRem = numMul % rhs;

        EXPECT_EQ(numMulDiv, orig);
        EXPECT_EQ(numMulRem, 0);
    }

    for (usize i = 0; i < ITERS; ++i) {

        const s128 rhsMax = s128::FromHighLow(2, 128);
        const s128 max = s128::Max() / rhsMax;

        const s128 orig = s128::FromHighLow((s64)rnd.GenerateU64(0, (u64)max.GetHigh()), rnd.GenerateU64(0, max.GetLow()));
        const s128 rhs = s128::FromHighLow(static_cast<s64>(rnd.GenerateU64(0, 4)) - 2, rnd.GenerateU64(1, 128));

        const s128 numMul = orig * rhs;

        const s128 numMulDiv = numMul / rhs;
        const s128 numMulRem = numMul % rhs;

        /*
         * On 'ALT_PLATFORM_X86_32' Clang, MSVC Debug/Release this fails
         */

        EXPECT_EQ(numMulDiv, orig);
        EXPECT_EQ(numMulRem, 0);
    }

    EXPECT_EQ((s128)0 - 1, -1);
    EXPECT_EQ((s128)((u128)0 - 1), -1);

    EXPECT_EQ(Alt::Desc<s128>::Max() & Alt::Desc<s128>::Max(), Alt::Desc<s128>::Max());
    EXPECT_EQ(Alt::Desc<s128>::Max() | Alt::Desc<s128>::Max(), Alt::Desc<s128>::Max());
    EXPECT_EQ(Alt::Desc<s128>::Max() ^ Alt::Desc<s128>::Max(), 0);

    EXPECT_EQ((s128)0 / 1, 0);
    EXPECT_EQ((s128)0 % 1, 0);
    EXPECT_EQ((s128)0 / (s128)1, 0);
    EXPECT_EQ((s128)0 % (s128)1, 0);

    EXPECT_EQ((s128)1 / 1, 1);
    EXPECT_EQ((s128)1 % 1, 0);
    EXPECT_EQ((s128)1 / (s128)1, 1);
    EXPECT_EQ((s128)1 % (s128)1, 0);

    EXPECT_EQ((s128)1 / -1, -1);
    EXPECT_EQ((s128)1 / (s128)-1, -1);
    EXPECT_EQ((s128)-1 / 1, -1);
    EXPECT_EQ((s128)-1 / (s128)1, -1);
    EXPECT_EQ((s128)-1 / -1, 1);
    EXPECT_EQ((s128)-1 / (s128)-1, 1);

    EXPECT_EQ(Alt::Desc<s128>::Max() / 1, Alt::Desc<s128>::Max());
    EXPECT_EQ(Alt::Desc<s128>::Max() % 1, 0);
    EXPECT_EQ(Alt::Desc<s128>::Max() / (s128)1, Alt::Desc<s128>::Max());
    EXPECT_EQ(Alt::Desc<s128>::Max() % (s128)1, 0);

    EXPECT_EQ(Alt::Desc<s128>::Max() / -1, -Alt::Desc<s128>::Max());
    EXPECT_EQ(Alt::Desc<s128>::Max() / (s128)-1, -Alt::Desc<s128>::Max());
    EXPECT_EQ(-Alt::Desc<s128>::Max() / 1, -Alt::Desc<s128>::Max());
    EXPECT_EQ(-Alt::Desc<s128>::Max() / (s128)1, -Alt::Desc<s128>::Max());
    EXPECT_EQ(-Alt::Desc<s128>::Max() / -1, Alt::Desc<s128>::Max());
    EXPECT_EQ(-Alt::Desc<s128>::Max() / (s128)-1, Alt::Desc<s128>::Max());

    EXPECT_EQ(Alt::Desc<s128>::Max() / Alt::Desc<s128>::Max(), 1);
    EXPECT_EQ(Alt::Desc<s128>::Max() % Alt::Desc<s128>::Max(), 0);

    EXPECT_EQ(Alt::Desc<s128>::Max() / -Alt::Desc<s128>::Max(), -1);
    EXPECT_EQ(-Alt::Desc<s128>::Max() / Alt::Desc<s128>::Max(), -1);
    EXPECT_EQ(-Alt::Desc<s128>::Max() / -Alt::Desc<s128>::Max(), 1);
}

TEST(Num, _s128_Float) {

    Alt::StaticString<256> str;

    {
        EXPECT_EQ((f32)Alt::Desc<s128>::Max(), 170141183460469231731687303715884105727.0f);

        EXPECT_EQ((f32)(s128(0)), 0.0f);

        EXPECT_EQ((s128)(f32)(s128(1) << 64), s128(1) << 64);

        EXPECT_EQ((f32)s128::FromHighLow(9223372036854775807ULL, 18446744073709551615ULL), 170141183460469231731687303715884105727.0);

        str.Format("{.32}", (f32)(s128(1) << 64));
        EXPECT_EQ(str, "18446744073709551616"_s);

        str.Format("{.32}", (f32)-(s128(1) << 64));
        EXPECT_EQ(str, "-18446744073709551616"_s);

        str.Format("{}", s128(18446744073709551616.0f));
        EXPECT_EQ(str, "18446744073709551616"_s);

        str.Format("{}", s128(-18446744073709551616.0f));
        EXPECT_EQ(str, "-18446744073709551616"_s);

        str.Format("{.32}", (f32)((s128(1) << 64) * 5));
        EXPECT_EQ(str, "92233720368547758080"_s);

        str.Format("{.32}", (f32)(-(s128(1) << 64) * 5));
        EXPECT_EQ(str, "-92233720368547758080"_s);

        EXPECT_EQ((f32)(s128(1) << 64), (f32)(u64(1) << 32) * (f32)(u64(1) << 32));
    }

    {
        EXPECT_EQ((f64)(s128(0)), 0.0);
        EXPECT_NE((f64)Alt::Desc<s128>::Max(), Alt::Desc<f64>::Inf());

        EXPECT_EQ((s128)(f64)(s128(1) << 64), s128(1) << 64);

        EXPECT_EQ((f64)s128::FromHighLow(9223372036854775807ULL, 18446744073709551615ULL), 170141183460469231731687303715884105727.0);

        str.Format("{.32}", (f64)(s128(1) << 64));
        EXPECT_EQ(str, "18446744073709551616"_s);

        str.Format("{.32}", (f64)-(s128(1) << 64));
        EXPECT_EQ(str, "-18446744073709551616"_s);

        str.Format("{}", s128(18446744073709551616.0));
        EXPECT_EQ(str, "18446744073709551616"_s);

        str.Format("{}", s128(-18446744073709551616.0));
        EXPECT_EQ(str, "-18446744073709551616"_s);

        str.Format("{.32}", (f64)((s128(1) << 64) * 5));
        EXPECT_EQ(str, "92233720368547758080"_s);

        str.Format("{.32}", (f64)(-(s128(1) << 64) * 5));
        EXPECT_EQ(str, "-92233720368547758080"_s);

        EXPECT_EQ((f64)(s128(1) << 64), (f64)(u64(1) << 32) * (f64)(u64(1) << 32));
    }
}
