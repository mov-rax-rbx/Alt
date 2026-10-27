#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE
#define ALT_USE_MATH_TYPES_IN_GLOBAL_NAMESPACE
#define ALT_USE_CUSTOM_FAST_MATH

#include "Utils.h"

#include "../alt-std/src/Math/Math.h"
#include "../alt-std/src/Random/Xoshiro256.h"

TEST(Math, IsEqFuzzy) {

	EXPECT_TRUE(Alt::Math::IsEqFuzzy(1.0f, 1.0f));
	EXPECT_TRUE(Alt::Math::IsEqFuzzy(1.0, 1.0));

	EXPECT_FALSE(Alt::Math::IsEqFuzzy(1.0f, 1.1f));
	EXPECT_FALSE(Alt::Math::IsEqFuzzy(1.0, 1.1));

	EXPECT_FALSE(Alt::Math::IsEqFuzzy(1.0f, 1.0f + Alt::Num::Desc<f32>::Eps() * 3.0f));
	EXPECT_FALSE(Alt::Math::IsEqFuzzy(1.0, 1.0 + Alt::Num::Desc<f64>::Eps() * 3.0));

	EXPECT_FALSE(Alt::Math::IsEqFuzzy(1.0f, 1.0f - Alt::Num::Desc<f32>::Eps() * 3.0f));
	EXPECT_FALSE(Alt::Math::IsEqFuzzy(1.0, 1.0 - Alt::Num::Desc<f64>::Eps() * 3.0));

	EXPECT_FALSE(Alt::Math::IsEqFuzzy(Alt::Num::Desc<f32>::Inf(), Alt::Num::Desc<f32>::Inf()));
	EXPECT_FALSE(Alt::Math::IsEqFuzzy(Alt::Num::Desc<f64>::Inf(), Alt::Num::Desc<f64>::Inf()));

	EXPECT_FALSE(Alt::Math::IsEqFuzzy(Alt::Num::Desc<f32>::SignalingNaN(), Alt::Num::Desc<f32>::SignalingNaN()));
	EXPECT_FALSE(Alt::Math::IsEqFuzzy(Alt::Num::Desc<f64>::SignalingNaN(), Alt::Num::Desc<f64>::SignalingNaN()));

	EXPECT_FALSE(Alt::Math::IsEqFuzzy(Alt::Num::Desc<f32>::QuietNaN(), Alt::Num::Desc<f32>::QuietNaN()));
	EXPECT_FALSE(Alt::Math::IsEqFuzzy(Alt::Num::Desc<f64>::QuietNaN(), Alt::Num::Desc<f64>::QuietNaN()));
}

TEST(Math, CompareFuzzy) {

	EXPECT_EQ(Alt::Math::CompareFuzzy(1.0f, 1.0f), 0);
	EXPECT_EQ(Alt::Math::CompareFuzzy(1.0, 1.0), 0);

	EXPECT_EQ(Alt::Math::CompareFuzzy(1.0f, 1.1f), -1);
	EXPECT_EQ(Alt::Math::CompareFuzzy(1.0, 1.1), -1);

	EXPECT_EQ(Alt::Math::CompareFuzzy(1.0f, 1.0f + Alt::Num::Desc<f32>::Eps() * 3.0f), -1);
	EXPECT_EQ(Alt::Math::CompareFuzzy(1.0, 1.0 + Alt::Num::Desc<f64>::Eps() * 3.0), -1);

	EXPECT_EQ(Alt::Math::CompareFuzzy(1.0f, 1.0f - Alt::Num::Desc<f32>::Eps() * 3.0f), 1);
	EXPECT_EQ(Alt::Math::CompareFuzzy(1.0, 1.0 - Alt::Num::Desc<f64>::Eps() * 3.0), 1);

	EXPECT_EQ(Alt::Math::CompareFuzzy(1.99999976f, 1.99999988f, 0.0f), -1);
	EXPECT_EQ(Alt::Math::CompareFuzzy(1.99999976, 1.99999988), -1);

	EXPECT_EQ(Alt::Math::CompareFuzzy(1.234566f * 10.0f, 1.234567f * 10.0f), -1);
	EXPECT_EQ(Alt::Math::CompareFuzzy(1.234566 * 10.0, 1.234567 * 10.0), -1);

	EXPECT_EQ(Alt::Math::CompareFuzzy(0.0f, 1e-7f), -1);
	EXPECT_EQ(Alt::Math::CompareFuzzy(0.0, 1e-7), -1);
}

TEST(Math, SafeInv) {

	EXPECT_EQ(Alt::Math::SafeInv(0.0f), 0.0f);
	EXPECT_EQ(Alt::Math::SafeInv(0.0), 0.0);

	EXPECT_EQ(Alt::Math::SafeInv(2.0f), 1.0f / 2.0f);
	EXPECT_EQ(Alt::Math::SafeInv(2.0), 1.0 / 2.0);

	EXPECT_EQ(Alt::Math::SafeInv(0.0f, 4.0f), 4.0f);
	EXPECT_EQ(Alt::Math::SafeInv(0.0, 4.0), 4.0);
}

TEST(Math, Lerp) {

	EXPECT_EQ(Alt::Math::Lerp(-1.0, 1.0, 0.0), -1.0);
	EXPECT_EQ(Alt::Math::Lerp(-1.0, 1.0, 1.0), 1.0);
	EXPECT_EQ(Alt::Math::Lerp(-1.0, 1.0, 0.5), 0.0);
}

TEST(Math, Asinf32) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	for (usize i = 0; i < NUM_ITERS; ++i) {

		const f32 num = rnd.GenerateF32_01();

		const f32 x0 = Alt::Math::Asin(num);
		const f32 x1 = std::asin(num);

		EXPECT_TRUE(Alt::Math::Abs(x0 - x1) <= 0.0001f)
			<< x0 << " != " << x1;
	}
}

TEST(Math, Asinf64) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	for (usize i = 0; i < NUM_ITERS; ++i) {

		const f64 num = rnd.GenerateF64_01();

		const f64 x0 = Alt::Math::Asin(num);
		const f64 x1 = std::asin(num);

		EXPECT_TRUE(Alt::Math::Abs(x0 - x1) <= 0.0001)
			<< x0 << " != " << x1;
	}
}

TEST(Math, Asinf32Bench) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	f32 sum = 0.0f;
	ALT_TIME_BENCH("Alt::Math::Asin", NUM_ITERS, {

		sum += Alt::Math::Asin(rnd.GenerateF32_01());
	});

	ALT_TIME_BENCH("std::asin", NUM_ITERS, {

		sum += std::asin(rnd.GenerateF32_01());
	});
}

TEST(Math, Asinf64Bench) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	f64 sum = 0.0;
	ALT_TIME_BENCH("Alt::Math::Asin", NUM_ITERS, {

		sum += Alt::Math::Asin(rnd.GenerateF64_01());
	});

	ALT_TIME_BENCH("std::asin", NUM_ITERS, {

		sum += std::asin(rnd.GenerateF64_01());
	});
}

TEST(Math, Atanf32) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	for (usize i = 0; i < NUM_ITERS; ++i) {

		const f32 num = rnd.GenerateF32_01();

		const f32 x0 = Alt::Math::Atan(num);
		const f32 x1 = std::atan(num);

		EXPECT_TRUE(Alt::Math::Abs(x0 - x1) <= 0.0001f)
			<< x0 << " != " << x1;
	}
}

TEST(Math, Atanf64) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	for (usize i = 0; i < NUM_ITERS; ++i) {

		const f64 num = rnd.GenerateF64_01();

		const f64 x0 = Alt::Math::Atan(num);
		const f64 x1 = std::atan(num);

		EXPECT_TRUE(Alt::Math::Abs(x0 - x1) <= 0.0001)
			<< x0 << " != " << x1;
	}
}

TEST(Math, Atanf32Bench) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	f32 sum = 0.0f;
	ALT_TIME_BENCH("Alt::Math::Atan", NUM_ITERS, {

		sum += Alt::Math::Atan(rnd.GenerateF32_01());
	});

	ALT_TIME_BENCH("std::atan", NUM_ITERS, {

		sum += std::atan(rnd.GenerateF32_01());
	});
}

TEST(Math, Atanf64Bench) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	f64 sum = 0.0;
	ALT_TIME_BENCH("Alt::Math::Atan", NUM_ITERS, {

		sum += Alt::Math::Atan(rnd.GenerateF64_01());
	});

	ALT_TIME_BENCH("std::atan", NUM_ITERS, {

		sum += std::atan(rnd.GenerateF64_01());
	});
}

TEST(Math, Atan2f32) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	for (usize i = 0; i < NUM_ITERS; ++i) {

		const f32 num0 = rnd.GenerateF32_01();
		const f32 num1 = rnd.GenerateF32_01();

		const f32 x0 = Alt::Math::Atan2(num0, num1);
		const f32 x1 = std::atan2(num0, num1);

		EXPECT_TRUE(Alt::Math::Abs(x0 - x1) <= 0.0001f)
			<< x0 << " != " << x1;
	}
}

TEST(Math, Atan2f64) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	for (usize i = 0; i < NUM_ITERS; ++i) {

		const f64 num0 = rnd.GenerateF64_01();
		const f64 num1 = rnd.GenerateF64_01();

		const f64 x0 = Alt::Math::Atan2(num0, num1);
		const f64 x1 = std::atan2(num0, num1);

		EXPECT_TRUE(Alt::Math::Abs(x0 - x1) <= 0.0001)
			<< x0 << " != " << x1;
	}
}

TEST(Math, Atan2f32Bench) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	f32 sum = 0.0f;
	ALT_TIME_BENCH("Alt::Math::Atan2", NUM_ITERS, {

		sum += Alt::Math::Atan2(rnd.GenerateF32_01(), rnd.GenerateF32_01());
	});

	ALT_TIME_BENCH("std::atan2", NUM_ITERS, {

		sum += std::atan2(rnd.GenerateF32_01(), rnd.GenerateF32_01());
	});
}

TEST(Math, Atan2f64Bench) {

	constexpr static usize NUM_ITERS = 10'000'000;

	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

	f64 sum = 0.0;
	ALT_TIME_BENCH("Alt::Math::Atan2", NUM_ITERS, {

		sum += Alt::Math::Atan2(rnd.GenerateF64_01(), rnd.GenerateF64_01());
	});

	ALT_TIME_BENCH("std::atan2", NUM_ITERS, {

		sum += std::atan2(rnd.GenerateF64_01(), rnd.GenerateF64_01());
	});
}

//TEST(Math, Rsqrtf32) {
//
//	constexpr static usize NUM_ITERS = 10'000'000;
//
//	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();
//
//	for (usize i = 0; i < NUM_ITERS; ++i) {
//
//		const f32 num = rnd.GenerateF32_01();
//
//		const f32 x0 = Alt::Math::Rsqrt(num);
//		const f32 x1 = 1.0f / std::sqrt(num);
//
//		EXPECT_TRUE(Alt::Math::Abs(x0 - x1) <= 0.01f)
//			<< x0 << " != " << x1;
//	}
//}
//
//TEST(Math, Rsqrtf64) {
//
//	constexpr static usize NUM_ITERS = 10'000'000;
//
//	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();
//
//	for (usize i = 0; i < NUM_ITERS; ++i) {
//
//		const f64 num = rnd.GenerateF64_01();
//
//		const f64 x0 = Alt::Math::Rsqrt(num);
//		const f64 x1 = 1.0 / std::sqrt(num);
//
//		EXPECT_TRUE(Alt::Math::Abs(x0 - x1) <= 0.01)
//			<< x0 << " != " << x1;
//	}
//}
//
//TEST(Math, Rsqrtf32Bench) {
//
//	constexpr static usize NUM_ITERS = 10'000'000;
//
//	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();
//
//	f32 sum = 0.0f;
//	ALT_TIME_BENCH("Alt::Math::Rsqrt", NUM_ITERS, {
//
//		sum += Alt::Math::Rsqrt(rnd.GenerateF32_01());
//	});
//
//	ALT_TIME_BENCH("1.0 / std::sqrt", NUM_ITERS, {
//
//		sum += 1.0f / std::sqrt(rnd.GenerateF32_01());
//	});
//}
//
//TEST(Math, Rsqrtf64Bench) {
//
//	constexpr static usize NUM_ITERS = 10'000'000;
//
//	Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();
//
//	f64 sum = 0.0;
//	ALT_TIME_BENCH("Alt::Math::Rsqrt", NUM_ITERS, {
//
//		sum += Alt::Math::Rsqrt(rnd.GenerateF64_01());
//	});
//
//	ALT_TIME_BENCH("1.0 / std::sqrt", NUM_ITERS, {
//
//		sum += 1.0 / std::sqrt(rnd.GenerateF64_01());
//	});
//}
