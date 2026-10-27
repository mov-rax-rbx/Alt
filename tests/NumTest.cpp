#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Utils/Num.h"
#include "../alt-std/src/Random/Xoshiro256.h"

namespace {

	template <typename T>
	static usize PopcountStable(T x) {

		usize count = 0;
		while (x != 0) {
			count += x & 1;
			x >>= 1;
		}

		return count;
	}

	template <typename T>
	static usize DigitCountStable(T x) {

		usize result = 0;

		do {

			result += 1;
			x /= 10;
		} while (x != 0);

		return result;
	}
}

TEST(Num, PopcntConstexprFuzz) {

	constexpr usize TEST_CNT = 1'000'000;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const u32 num = GetNum<u32>();
		EXPECT_EQ(Alt::Num::PopcountConstexpr(num), PopcountStable(num));
	}

	for (usize i = 0; i < TEST_CNT; ++i) {

		const u64 num = GetNum<u64>();
		EXPECT_EQ(Alt::Num::PopcountConstexpr(num), PopcountStable(num));
	}
}

TEST(Num, PopcntFuzz) {

	constexpr usize TEST_CNT = 1'000'000;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const u32 num = GetNum<u32>();
		EXPECT_EQ(Alt::Num::Popcount(num), PopcountStable(num));
	}

	for (usize i = 0; i < TEST_CNT; ++i) {

		const u64 num = GetNum<u64>();
		EXPECT_EQ(Alt::Num::Popcount(num), PopcountStable(num));
	}
}

TEST(Num, Count0BitsRightConstexpr) {

	EXPECT_EQ(Alt::Num::Count0BitsRightConstexpr((u32)0b11111111111111111111111111111111), (u32)0);
	EXPECT_EQ(Alt::Num::Count0BitsRightConstexpr((u32)0b11111111111111111111111111111110), (u32)1);
	EXPECT_EQ(Alt::Num::Count0BitsRightConstexpr((u64)0b11111111111111111111111111111111), (u64)0);
	EXPECT_EQ(Alt::Num::Count0BitsRightConstexpr((u64)0b11111111111111111111111111111110), (u64)1);

	EXPECT_EQ(Alt::Num::Count0BitsRightConstexpr(Alt::Num::Desc<u32>::Max()), (u32)0);
	EXPECT_EQ(Alt::Num::Count0BitsRightConstexpr(Alt::Num::Desc<u64>::Max()), (u64)0);

	EXPECT_EQ(Alt::Num::Count0BitsRightConstexpr((u32)0), (u32)32);
	EXPECT_EQ(Alt::Num::Count0BitsRightConstexpr((u64)0), (u64)64);
}

TEST(Num, Count0BitsRight) {

	EXPECT_EQ(Alt::Num::Count0BitsRight((u32)0b11111111111111111111111111111111), (u32)0);
	EXPECT_EQ(Alt::Num::Count0BitsRight((u32)0b11111111111111111111111111111110), (u32)1);
	EXPECT_EQ(Alt::Num::Count0BitsRight((u64)0b11111111111111111111111111111111), (u64)0);
	EXPECT_EQ(Alt::Num::Count0BitsRight((u64)0b11111111111111111111111111111110), (u64)1);

	EXPECT_EQ(Alt::Num::Count0BitsRight(Alt::Num::Desc<u32>::Max()), (u32)0);
	EXPECT_EQ(Alt::Num::Count0BitsRight(Alt::Num::Desc<u64>::Max()), (u64)0);

	EXPECT_EQ(Alt::Num::Count0BitsRight((u32)0), (u32)32);
	EXPECT_EQ(Alt::Num::Count0BitsRight((u64)0), (u64)64);
}

TEST(Num, Count0BitsLeftConstexpr) {

	EXPECT_EQ(Alt::Num::Count0BitsLeftConstexpr((u32)0b11111111111111111111111111111111), (u32)0);
	EXPECT_EQ(Alt::Num::Count0BitsLeftConstexpr((u32)0b00111111111111111111111111111111), (u32)2);
	EXPECT_EQ(Alt::Num::Count0BitsLeftConstexpr((u64)0b11111111111111111111111111111111), (u64)(63 - 31));
	EXPECT_EQ(Alt::Num::Count0BitsLeftConstexpr((u64)0b00111111111111111111111111111111), (u64)(63 - (31 - 2)));
	EXPECT_EQ(Alt::Num::Count0BitsLeftConstexpr((u64)0b00111111111111111111111111111101), (u64)(63 - (31 - 2)));

	EXPECT_EQ(Alt::Num::Count0BitsLeftConstexpr(Alt::Num::Desc<u32>::Max()), (u32)0);
	EXPECT_EQ(Alt::Num::Count0BitsLeftConstexpr(Alt::Num::Desc<u64>::Max()), (u64)0);

	EXPECT_EQ(Alt::Num::Count0BitsLeftConstexpr((u32)0), (u32)32);
	EXPECT_EQ(Alt::Num::Count0BitsLeftConstexpr((u64)0), (u64)64);
}

TEST(Num, Count0BitsLeft) {

	EXPECT_EQ(Alt::Num::Count0BitsLeft((u32)0b11111111111111111111111111111111), (u32)0);
	EXPECT_EQ(Alt::Num::Count0BitsLeft((u32)0b00111111111111111111111111111111), (u32)2);
	EXPECT_EQ(Alt::Num::Count0BitsLeft((u64)0b11111111111111111111111111111111), (u64)(63 - 31));
	EXPECT_EQ(Alt::Num::Count0BitsLeft((u64)0b00111111111111111111111111111111), (u64)(63 - (31 - 2)));
	EXPECT_EQ(Alt::Num::Count0BitsLeft((u64)0b00111111111111111111111111111101), (u64)(63 - (31 - 2)));

	EXPECT_EQ(Alt::Num::Count0BitsLeft(Alt::Num::Desc<u32>::Max()), (u32)0);
	EXPECT_EQ(Alt::Num::Count0BitsLeft(Alt::Num::Desc<u64>::Max()), (u64)0);

	EXPECT_EQ(Alt::Num::Count0BitsLeft((u32)0), (u32)32);
	EXPECT_EQ(Alt::Num::Count0BitsLeft((u64)0), (u64)64);
}

TEST(Num, RoundUpToPowerOf2) {

	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2((u32)0), (u32)1);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2((u32)33), (u32)64);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2((u32)64), (u32)64);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2(Alt::Num::Desc<u32>::Max() - 1), Alt::Num::Desc<u32>::Max());
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2(Alt::Num::Desc<u32>::Max()), Alt::Num::Desc<u32>::Max());

	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2((u64)0), (u64)1);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2((u64)33), (u64)64);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2((u64)64), (u64)64);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2(Alt::Num::Desc<u64>::Max() - 1), Alt::Num::Desc<u64>::Max());
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2(Alt::Num::Desc<u64>::Max()), Alt::Num::Desc<u64>::Max());
}

TEST(Num, RoundUpToPowerOf2PreserveZero) {

	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero((u32)0), (u32)0);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero((u32)33), (u32)64);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero((u32)64), (u32)64);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero(Alt::Num::Desc<u32>::Max() - 1), Alt::Num::Desc<u32>::Max());
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero(Alt::Num::Desc<u32>::Max()), Alt::Num::Desc<u32>::Max());

	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero((u64)0), (u64)0);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero((u64)33), (u64)64);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero((u64)64), (u64)64);
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero(Alt::Num::Desc<u64>::Max() - 1), Alt::Num::Desc<u64>::Max());
	EXPECT_EQ(Alt::Num::RoundUpToPowerOf2PreserveZero(Alt::Num::Desc<u64>::Max()), Alt::Num::Desc<u64>::Max());
}

TEST(Num, RoundDownToPowerOf2) {

	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2((u32)0), (u32)0);
	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2((u32)33), (u32)32);
	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2((u32)64), (u32)64);
	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2(Alt::Num::Desc<u32>::Max() - 1), ((u32)1 << 31));
	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2(Alt::Num::Desc<u32>::Max()), ((u32)1 << 31));

	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2((u64)0), (u64)0);
	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2((u64)33), (u64)32);
	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2((u64)64), (u64)64);
	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2(Alt::Num::Desc<u64>::Max() - 1), ((u64)1 << 63));
	EXPECT_EQ(Alt::Num::RoundDownToPowerOf2(Alt::Num::Desc<u64>::Max()), ((u64)1 << 63));
}

TEST(Num, Log2) {

	EXPECT_EQ(Alt::Num::Log2((u32)0), (u32)0);
	EXPECT_EQ(Alt::Num::Log2((u32)1), (u32)0);
	EXPECT_EQ(Alt::Num::Log2((u32)33), (u32)5);
	EXPECT_EQ(Alt::Num::Log2(Alt::Num::RoundUpToPowerOf2((u32)33)), (u32)6);
	EXPECT_EQ(Alt::Num::Log2(Alt::Num::Desc<u32>::Max()), (u32)31);
	EXPECT_EQ(Alt::Num::Log2(Alt::Num::RoundDownToPowerOf2(Alt::Num::Desc<u32>::Max())), (u32)31);

	EXPECT_EQ(Alt::Num::Log2((u64)0), (u64)0);
	EXPECT_EQ(Alt::Num::Log2((u64)1), (u64)0);
	EXPECT_EQ(Alt::Num::Log2((u64)33), (u64)5);
	EXPECT_EQ(Alt::Num::Log2(Alt::Num::RoundUpToPowerOf2((u64)33)), (u64)6);
	EXPECT_EQ(Alt::Num::Log2(Alt::Num::Desc<u64>::Max()), (u64)63);
	EXPECT_EQ(Alt::Num::Log2(Alt::Num::RoundDownToPowerOf2(Alt::Num::Desc<u64>::Max())), (u64)63);
}

TEST(Num, DigitCount) {

	static const usize ITERS = 10'000'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const u32 num = static_cast<u32>(rnd.GenerateU64(0, Alt::Num::Desc<u32>::Max()));

		const usize res0 = DigitCountStable(num);
		const usize res1 = Alt::Num::DigitCount(num);

		EXPECT_EQ(res0, res1);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u64 num = rnd.GenerateU64();

		const usize res0 = DigitCountStable(num);
		const usize res1 = Alt::Num::DigitCount(num);

		EXPECT_EQ(res0, res1);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s32 num = static_cast<s32>(rnd.GenerateS64(Alt::Num::Desc<s32>::Min(), Alt::Num::Desc<s32>::Max()));

		const usize res0 = DigitCountStable(num);
		const usize res1 = Alt::Num::DigitCount(num);

		EXPECT_EQ(res0, res1);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s64 num = rnd.GenerateS64();

		const usize res0 = DigitCountStable(num);
		const usize res1 = Alt::Num::DigitCount(num);

		EXPECT_EQ(res0, res1);
	}
}

TEST(Num, IsOverflowCheckAdd) {

	EXPECT_TRUE(Alt::Num::IsOverflowCheckAdd(Alt::Num::Desc<u64>::Max(), 2));
	EXPECT_TRUE(Alt::Num::IsOverflowCheckAdd(Alt::Num::Desc<u32>::Max(), 2));
	EXPECT_TRUE(Alt::Num::IsOverflowCheckAdd(Alt::Num::Desc<u16>::Max(), 2));
	EXPECT_TRUE(Alt::Num::IsOverflowCheckAdd(Alt::Num::Desc<u8>::Max(), 2));

	EXPECT_TRUE(Alt::Num::IsOverflowCheckAddConstexpr(Alt::Num::Desc<u64>::Max(), 2));
	EXPECT_TRUE(Alt::Num::IsOverflowCheckAddConstexpr(Alt::Num::Desc<u32>::Max(), 2));
	EXPECT_TRUE(Alt::Num::IsOverflowCheckAddConstexpr(Alt::Num::Desc<u16>::Max(), 2));
	EXPECT_TRUE(Alt::Num::IsOverflowCheckAddConstexpr(Alt::Num::Desc<u8>::Max(), 2));
}

TEST(Num, IsUnderflowCheckSub) {

	EXPECT_TRUE(Alt::Num::IsUnderflowCheckSub<u64>(0, 2));
	EXPECT_TRUE(Alt::Num::IsUnderflowCheckSub<u32>(0, 2));
	EXPECT_TRUE(Alt::Num::IsUnderflowCheckSub<u16>(0, 2));
	EXPECT_TRUE(Alt::Num::IsUnderflowCheckSub<u8>(0, 2));

	EXPECT_TRUE(Alt::Num::IsUnderflowCheckSubConstexpr<u64>(0, 2));
	EXPECT_TRUE(Alt::Num::IsUnderflowCheckSubConstexpr<u32>(0, 2));
	EXPECT_TRUE(Alt::Num::IsUnderflowCheckSubConstexpr<u16>(0, 2));
	EXPECT_TRUE(Alt::Num::IsUnderflowCheckSubConstexpr<u8>(0, 2));
}

TEST(Num, AddWrap) {

	EXPECT_EQ(Alt::Num::AddWrap<usize>(1, 2), 3);
	EXPECT_EQ(Alt::Num::AddWrap<usize>(Alt::Num::Desc<usize>::Max(), 1), Alt::Num::Desc<usize>::Max());
	EXPECT_EQ(Alt::Num::AddWrap<usize>(Alt::Num::Desc<usize>::Max(), Alt::Num::Desc<usize>::Max()), Alt::Num::Desc<usize>::Max());
}

TEST(Num, SubWrap) {

	EXPECT_EQ(Alt::Num::SubWrap<usize>(1, 2), 0);
	EXPECT_EQ(Alt::Num::SubWrap<usize>(Alt::Num::Desc<usize>::Max(), 1), Alt::Num::Desc<usize>::Max() - 1);
	EXPECT_EQ(Alt::Num::SubWrap<usize>(Alt::Num::Desc<usize>::Max(), Alt::Num::Desc<usize>::Max()), 0);
}

TEST(Num, IsSignPositive) {

	EXPECT_TRUE(Alt::Num::IsSignPositive(0.0f));
	EXPECT_TRUE(Alt::Num::IsSignPositive(0.0));

	EXPECT_FALSE(Alt::Num::IsSignPositive(-0.0f));
	EXPECT_FALSE(Alt::Num::IsSignPositive(-0.0));
}

TEST(Num, RoundUpWrap) {

	EXPECT_EQ(Alt::Num::RoundUpWrap(0u, 8), 8u);
	EXPECT_EQ(Alt::Num::RoundUpWrapPreserveZero(0u, 8), 0u);

	EXPECT_EQ(Alt::Num::RoundUpWrap(1u, 8), 8u);
	EXPECT_EQ(Alt::Num::RoundUpWrap(16u, 8), 16u);
	EXPECT_EQ(Alt::Num::RoundUpWrap(16u, 3), 18u);

	EXPECT_EQ(Alt::Num::RoundUpWrap(13u, 0), 13u);
	EXPECT_EQ(Alt::Num::RoundUpWrap(13u, 1), 13u);
}

TEST(Num, RoundDown) {

	EXPECT_EQ(Alt::Num::RoundDown(0u, 8), 0u);
	EXPECT_EQ(Alt::Num::RoundDown(1u, 8), 0u);
	EXPECT_EQ(Alt::Num::RoundDown(16u, 8), 16u);
	EXPECT_EQ(Alt::Num::RoundDown(16u, 3), 15u);

	EXPECT_EQ(Alt::Num::RoundDown(13u, 0), 13u);
	EXPECT_EQ(Alt::Num::RoundDown(13u, 1), 13u);
}

TEST(Num, Abs) {

	EXPECT_EQ(Alt::Num::Abs(0.0), 0.0);
	EXPECT_EQ(Alt::Num::Abs(0.0f), 0.0f);

	EXPECT_EQ(Alt::Num::Abs(-0.0), 0.0);
	EXPECT_EQ(Alt::Num::Abs(-0.0f), 0.0f);

	EXPECT_EQ(Alt::Num::Abs(-4.0), 4.0);
	EXPECT_EQ(Alt::Num::Abs(-4.0f), 4.0f);

	EXPECT_EQ(Alt::Num::Abs(-8.0), 8.0);
	EXPECT_EQ(Alt::Num::Abs(-8.0f), 8.0f);
}

TEST(Num, MaxMin) {

	EXPECT_EQ(Alt::Num::Max(0, -1), 0);
	EXPECT_EQ(Alt::Num::Min(0, -1), -1);

	EXPECT_EQ(Alt::Num::Max(0.0, 1.0), 1.0);
	EXPECT_EQ(Alt::Num::Max(0.0f, 1.0f), 1.0f);

	EXPECT_EQ(Alt::Num::Min(0.0, 1.0), 0.0);
	EXPECT_EQ(Alt::Num::Min(0.0f, 1.0f), 0.0f);

	EXPECT_EQ(Alt::Num::Max(0.0001, 0.0002), 0.0002);
	EXPECT_EQ(Alt::Num::Max(0.0001f, 0.0002f), 0.0002f);

	EXPECT_EQ(Alt::Num::Max(0.0001, -0.0002), 0.0001);
	EXPECT_EQ(Alt::Num::Max(0.0001f, -0.0002f), 0.0001f);
}

TEST(Num, Clamp) {

	EXPECT_EQ(Alt::Num::Clamp(2, -1, 1), 1);
	EXPECT_EQ(Alt::Num::Clamp(0, -1, 1), 0);

	EXPECT_EQ(Alt::Num::Clamp(-2.0, 0.0, 1.0), 0.0);
	EXPECT_EQ(Alt::Num::Clamp(2.0, 0.0, 1.0), 1.0);
	EXPECT_EQ(Alt::Num::Clamp(0.4, 0.0, 1.0), 0.4);
}

TEST(Num, ConvertOrdered_SignedToUnsigned_UnsignedToSigned) {

	static const usize NUM_ITER = 20;

	{
		{
			const usize num0 = 0;
			const usize num1 = 3;

			const ssize snum0 = Alt::Num::ConvertUnsignedToSignedOrdered(num0);
			const ssize snum1 = Alt::Num::ConvertUnsignedToSignedOrdered(num1);

			EXPECT_TRUE(snum0 < snum1);
		}

		{
			const usize num0 = Alt::Num::Desc<usize>::Max();
			const usize num1 = Alt::Num::Desc<usize>::Max();

			const ssize snum0 = Alt::Num::ConvertUnsignedToSignedOrdered(num0);
			const ssize snum1 = Alt::Num::ConvertUnsignedToSignedOrdered(num1);

			EXPECT_TRUE(snum0 == snum1);
		}
	}

	{
		{
			const ssize num0 = -1;
			const ssize num1 = 1;

			const usize unum0 = Alt::Num::ConvertSignedToUnsignedOrdered(num0);
			const usize unum1 = Alt::Num::ConvertSignedToUnsignedOrdered(num1);

			EXPECT_TRUE(unum0 < unum1);
		}

		{
			const ssize num0 = Alt::Num::Desc<ssize>::Max();
			const ssize num1 = Alt::Num::Desc<ssize>::Max();

			const ssize snum0 = Alt::Num::ConvertSignedToUnsignedOrdered(num0);
			const ssize snum1 = Alt::Num::ConvertSignedToUnsignedOrdered(num1);

			EXPECT_TRUE(snum0 == snum1);
		}
	}

	for (usize i = 0; i < NUM_ITER; ++i) {

		const usize num = GetNum<usize>();

		const ssize snum = Alt::Num::ConvertUnsignedToSignedOrdered(num);
		const usize unum = Alt::Num::ConvertSignedToUnsignedOrdered(snum);

		EXPECT_EQ(num, unum);
	}

	for (usize i = 0; i < NUM_ITER; ++i) {

		const ssize num = GetNum<ssize>();

		const usize unum = Alt::Num::ConvertSignedToUnsignedOrdered(num);
		const ssize snum = Alt::Num::ConvertUnsignedToSignedOrdered(unum);

		EXPECT_EQ(num, snum);
	}
}

TEST(Num, ConvertOrdered_FloatToUnsigned_FloatToUnsigned) {

	{
		{
			const f32 num0 = -1.0f;
			const f32 num1 = 1.0f;

			const s32 snum0 = Alt::Num::ConvertFloatToSignedOrdered(num0);
			const s32 snum1 = Alt::Num::ConvertFloatToSignedOrdered(num1);

			EXPECT_TRUE(snum0 < snum1);
		}

		{
			const f32 num0 = 0.9f;
			const f32 num1 = 1.0f;

			const s32 snum0 = Alt::Num::ConvertFloatToSignedOrdered(num0);
			const s32 snum1 = Alt::Num::ConvertFloatToSignedOrdered(num1);

			EXPECT_TRUE(snum0 < snum1);
		}

		{
			const f32 num0 = Alt::Num::Desc<f32>::Max();
			const f32 num1 = Alt::Num::Desc<f32>::Max();

			const s32 snum0 = Alt::Num::ConvertFloatToSignedOrdered(num0);
			const s32 snum1 = Alt::Num::ConvertFloatToSignedOrdered(num1);

			EXPECT_TRUE(snum0 == snum1);
		}
	}

	{
		{
			const f32 num0 = 0.0f;
			const f32 num1 = 3.0f;

			const u32 unum0 = Alt::Num::ConvertFloatToUnsignedOrdered(num0);
			const u32 unum1 = Alt::Num::ConvertFloatToUnsignedOrdered(num1);

			EXPECT_TRUE(unum0 < unum1);
		}

		{
			const f32 num0 = 2.9f;
			const f32 num1 = 3.0f;

			const u32 unum0 = Alt::Num::ConvertFloatToUnsignedOrdered(num0);
			const u32 unum1 = Alt::Num::ConvertFloatToUnsignedOrdered(num1);

			EXPECT_TRUE(unum0 < unum1);
		}

		{
			const f32 num0 = Alt::Num::Desc<f32>::Max();
			const f32 num1 = Alt::Num::Desc<f32>::Max();

			const u32 unum0 = Alt::Num::ConvertFloatToUnsignedOrdered(num0);
			const u32 unum1 = Alt::Num::ConvertFloatToUnsignedOrdered(num1);

			EXPECT_TRUE(unum0 == unum1);
		}
	}

	{
		{
			const f64 num0 = -1.0;
			const f64 num1 = 1.0;

			const s64 snum0 = Alt::Num::ConvertFloatToSignedOrdered(num0);
			const s64 snum1 = Alt::Num::ConvertFloatToSignedOrdered(num1);

			EXPECT_TRUE(snum0 < snum1);
		}

		{
			const f64 num0 = 0.9;
			const f64 num1 = 1.0;

			const s64 snum0 = Alt::Num::ConvertFloatToSignedOrdered(num0);
			const s64 snum1 = Alt::Num::ConvertFloatToSignedOrdered(num1);

			EXPECT_TRUE(snum0 < snum1);
		}

		{
			const f64 num0 = Alt::Num::Desc<f64>::Max();
			const f64 num1 = Alt::Num::Desc<f64>::Max();

			const s64 snum0 = Alt::Num::ConvertFloatToSignedOrdered(num0);
			const s64 snum1 = Alt::Num::ConvertFloatToSignedOrdered(num1);

			EXPECT_TRUE(snum0 == snum1);
		}
	}

	{
		{
			const f64 num0 = 0.0;
			const f64 num1 = 3.0;

			const u64 unum0 = Alt::Num::ConvertFloatToUnsignedOrdered(num0);
			const u64 unum1 = Alt::Num::ConvertFloatToUnsignedOrdered(num1);

			EXPECT_TRUE(unum0 < unum1);
		}

		{
			const f64 num0 = 2.9;
			const f64 num1 = 3.0;

			const u64 unum0 = Alt::Num::ConvertFloatToUnsignedOrdered(num0);
			const u64 unum1 = Alt::Num::ConvertFloatToUnsignedOrdered(num1);

			EXPECT_TRUE(unum0 < unum1);
		}

		{
			const f64 num0 = Alt::Num::Desc<f64>::Max();
			const f64 num1 = Alt::Num::Desc<f64>::Max();

			const u64 unum0 = Alt::Num::ConvertFloatToUnsignedOrdered(num0);
			const u64 unum1 = Alt::Num::ConvertFloatToUnsignedOrdered(num1);

			EXPECT_TRUE(unum0 == unum1);
		}
	}
}

TEST(Num, Mod) {

	EXPECT_EQ(Alt::Num::Mod<u64>(0, 1), 0);
	EXPECT_EQ(Alt::Num::Mod<u64>(1, 1), 0);
	EXPECT_EQ(Alt::Num::Mod<u64>(5, 2), 1);
	EXPECT_EQ(Alt::Num::Mod<u64>(6, 2), 0);
	EXPECT_EQ(Alt::Num::Mod<u64>(7, 3), 1);
	EXPECT_EQ(Alt::Num::Mod<u64>(8, 3), 2);
	EXPECT_EQ(Alt::Num::Mod<u64>(9, 3), 0);
	EXPECT_EQ(Alt::Num::Mod<u64>(100, 7), 2);
	EXPECT_EQ(Alt::Num::Mod<u64>(Alt::Num::Desc<u64>::Max(), 2), 1);
	EXPECT_EQ(Alt::Num::Mod<u64>(Alt::Num::Desc<u64>::Max(), Alt::Num::Desc<u64>::Max()), 0);
	EXPECT_EQ(Alt::Num::Mod<u64>(Alt::Num::Desc<u64>::Max(), Alt::Num::Desc<u64>::Max() - 1), 1);

	EXPECT_EQ(Alt::Num::Mod<s64>(7, 3), 1);
	EXPECT_EQ(Alt::Num::Mod<s64>(-7, 3), 2);
	EXPECT_EQ(Alt::Num::Mod<s64>(7, -3), -2);
	EXPECT_EQ(Alt::Num::Mod<s64>(-7, -3), -1);
	EXPECT_EQ(Alt::Num::Mod<s64>(8, 3), 2);
	EXPECT_EQ(Alt::Num::Mod<s64>(-8, 3), 1);
	EXPECT_EQ(Alt::Num::Mod<s64>(8, -3), -1);
	EXPECT_EQ(Alt::Num::Mod<s64>(-8, -3), -2);
	EXPECT_EQ(Alt::Num::Mod<s64>(6, 3), 0);
	EXPECT_EQ(Alt::Num::Mod<s64>(-6, 3), 0);
	EXPECT_EQ(Alt::Num::Mod<s64>(1, 2), 1);
	EXPECT_EQ(Alt::Num::Mod<s64>(-1, 2), 1);
	EXPECT_EQ(Alt::Num::Mod<s64>(Alt::Num::Desc<s64>::Max(), 2), 1);
	EXPECT_EQ(Alt::Num::Mod<s64>(Alt::Num::Desc<s64>::Min(), 2), 0);
	EXPECT_EQ(Alt::Num::Mod<s64>(Alt::Num::Desc<s64>::Min(), 3), 1);
}
