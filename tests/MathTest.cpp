#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE
#define ALT_USE_MATH_TYPES_IN_GLOBAL_NAMESPACE
#define ALT_USE_CUSTOM_FAST_MATH

#include "Utils.h"

#include "../alt-std/src/Math/Math.h"
#include "../alt-std/src/Random/Xoshiro256.h"

#include "../alt-std/src/Concurrency/Thread.h"
#include "../alt-std/src/Utils/Range.h"

TEST(Math, IsEqFuzzy) {

    EXPECT_TRUE(Alt::IsEqFuzzy(1.0f, 1.0f));
    EXPECT_TRUE(Alt::IsEqFuzzy(1.0, 1.0));

    EXPECT_FALSE(Alt::IsEqFuzzy(1.0f, 1.1f));
    EXPECT_FALSE(Alt::IsEqFuzzy(1.0, 1.1));

    EXPECT_FALSE(Alt::IsEqFuzzy(1.0f, 1.0f + Alt::Desc<f32>::Eps() * 3.0f));
    EXPECT_FALSE(Alt::IsEqFuzzy(1.0, 1.0 + Alt::Desc<f64>::Eps() * 3.0));

    EXPECT_FALSE(Alt::IsEqFuzzy(1.0f, 1.0f - Alt::Desc<f32>::Eps() * 3.0f));
    EXPECT_FALSE(Alt::IsEqFuzzy(1.0, 1.0 - Alt::Desc<f64>::Eps() * 3.0));

    EXPECT_FALSE(Alt::IsEqFuzzy(Alt::Desc<f32>::Inf(), Alt::Desc<f32>::Inf()));
    EXPECT_FALSE(Alt::IsEqFuzzy(Alt::Desc<f64>::Inf(), Alt::Desc<f64>::Inf()));

    EXPECT_FALSE(Alt::IsEqFuzzy(Alt::Desc<f32>::SignalingNaN(), Alt::Desc<f32>::SignalingNaN()));
    EXPECT_FALSE(Alt::IsEqFuzzy(Alt::Desc<f64>::SignalingNaN(), Alt::Desc<f64>::SignalingNaN()));

    EXPECT_FALSE(Alt::IsEqFuzzy(Alt::Desc<f32>::QuietNaN(), Alt::Desc<f32>::QuietNaN()));
    EXPECT_FALSE(Alt::IsEqFuzzy(Alt::Desc<f64>::QuietNaN(), Alt::Desc<f64>::QuietNaN()));
}

TEST(Math, CompareFuzzy) {

    EXPECT_EQ(Alt::CompareFuzzy(1.0f, 1.0f), 0);
    EXPECT_EQ(Alt::CompareFuzzy(1.0, 1.0), 0);

    EXPECT_EQ(Alt::CompareFuzzy(1.0f, 1.1f), -1);
    EXPECT_EQ(Alt::CompareFuzzy(1.0, 1.1), -1);

    EXPECT_EQ(Alt::CompareFuzzy(1.0f, 1.0f + Alt::Desc<f32>::Eps() * 3.0f), -1);
    EXPECT_EQ(Alt::CompareFuzzy(1.0, 1.0 + Alt::Desc<f64>::Eps() * 3.0), -1);

    EXPECT_EQ(Alt::CompareFuzzy(1.0f, 1.0f - Alt::Desc<f32>::Eps() * 3.0f), 1);
    EXPECT_EQ(Alt::CompareFuzzy(1.0, 1.0 - Alt::Desc<f64>::Eps() * 3.0), 1);

    EXPECT_EQ(Alt::CompareFuzzy(1.99999976f, 1.99999988f, 0.0f), -1);
    EXPECT_EQ(Alt::CompareFuzzy(1.99999976, 1.99999988), -1);

    EXPECT_EQ(Alt::CompareFuzzy(1.234566f * 10.0f, 1.234567f * 10.0f), -1);
    EXPECT_EQ(Alt::CompareFuzzy(1.234566 * 10.0, 1.234567 * 10.0), -1);

    EXPECT_EQ(Alt::CompareFuzzy(0.0f, 1e-7f), -1);
    EXPECT_EQ(Alt::CompareFuzzy(0.0, 1e-7), -1);
}

TEST(Math, SafeInv) {

    EXPECT_EQ(Alt::SafeInv(0.0f), 0.0f);
    EXPECT_EQ(Alt::SafeInv(0.0), 0.0);

    EXPECT_EQ(Alt::SafeInv(2.0f), 1.0f / 2.0f);
    EXPECT_EQ(Alt::SafeInv(2.0), 1.0 / 2.0);

    EXPECT_EQ(Alt::SafeInv(0.0f, 4.0f), 4.0f);
    EXPECT_EQ(Alt::SafeInv(0.0, 4.0), 4.0);
}

TEST(Math, Lerp) {

    EXPECT_EQ(Alt::Lerp(-1.0, 1.0, 0.0), -1.0);
    EXPECT_EQ(Alt::Lerp(-1.0, 1.0, 1.0), 1.0);
    EXPECT_EQ(Alt::Lerp(-1.0, 1.0, 0.5), 0.0);
}

TEST(Math, Asinf32) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    for (usize i = 0; i < NUM_ITERS; ++i) {

        const f32 num = rnd.GenerateF32_01();

        const f32 x0 = Alt::Asin(num);
        const f32 x1 = std::asin(num);

        EXPECT_TRUE(Alt::Abs(x0 - x1) <= 0.0001f)
            << x0 << " != " << x1;
    }
}

TEST(Math, Asinf64) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    for (usize i = 0; i < NUM_ITERS; ++i) {

        const f64 num = rnd.GenerateF64_01();

        const f64 x0 = Alt::Asin(num);
        const f64 x1 = std::asin(num);

        EXPECT_TRUE(Alt::Abs(x0 - x1) <= 0.0001)
            << x0 << " != " << x1;
    }
}

TEST(Math, Asinf32Bench) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    f32 sum = 0.0f;
    ALT_TIME_BENCH("Alt::Asin", NUM_ITERS, {

        sum += Alt::Asin(rnd.GenerateF32_01());
    });

    ALT_TIME_BENCH("std::asin", NUM_ITERS, {

        sum += std::asin(rnd.GenerateF32_01());
    });
}

TEST(Math, Asinf64Bench) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    f64 sum = 0.0;
    ALT_TIME_BENCH("Alt::Asin", NUM_ITERS, {

        sum += Alt::Asin(rnd.GenerateF64_01());
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

        const f32 x0 = Alt::Atan(num);
        const f32 x1 = std::atan(num);

        EXPECT_TRUE(Alt::Abs(x0 - x1) <= 0.0001f)
            << x0 << " != " << x1;
    }
}

TEST(Math, Atanf64) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    for (usize i = 0; i < NUM_ITERS; ++i) {

        const f64 num = rnd.GenerateF64_01();

        const f64 x0 = Alt::Atan(num);
        const f64 x1 = std::atan(num);

        EXPECT_TRUE(Alt::Abs(x0 - x1) <= 0.0001)
            << x0 << " != " << x1;
    }
}

TEST(Math, Atanf32Bench) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    f32 sum = 0.0f;
    ALT_TIME_BENCH("Alt::Atan", NUM_ITERS, {

        sum += Alt::Atan(rnd.GenerateF32_01());
    });

    ALT_TIME_BENCH("std::atan", NUM_ITERS, {

        sum += std::atan(rnd.GenerateF32_01());
    });
}

TEST(Math, Atanf64Bench) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    f64 sum = 0.0;
    ALT_TIME_BENCH("Alt::Atan", NUM_ITERS, {

        sum += Alt::Atan(rnd.GenerateF64_01());
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

        const f32 x0 = Alt::Atan2(num0, num1);
        const f32 x1 = std::atan2(num0, num1);

        EXPECT_TRUE(Alt::Abs(x0 - x1) <= 0.0001f)
            << x0 << " != " << x1;
    }
}

TEST(Math, Atan2f64) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    for (usize i = 0; i < NUM_ITERS; ++i) {

        const f64 num0 = rnd.GenerateF64_01();
        const f64 num1 = rnd.GenerateF64_01();

        const f64 x0 = Alt::Atan2(num0, num1);
        const f64 x1 = std::atan2(num0, num1);

        EXPECT_TRUE(Alt::Abs(x0 - x1) <= 0.0001)
            << x0 << " != " << x1;
    }
}

TEST(Math, Atan2f32Bench) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    f32 sum = 0.0f;
    ALT_TIME_BENCH("Alt::Atan2", NUM_ITERS, {

        sum += Alt::Atan2(rnd.GenerateF32_01(), rnd.GenerateF32_01());
    });

    ALT_TIME_BENCH("std::atan2", NUM_ITERS, {

        sum += std::atan2(rnd.GenerateF32_01(), rnd.GenerateF32_01());
    });
}

TEST(Math, Atan2f64Bench) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    f64 sum = 0.0;
    ALT_TIME_BENCH("Alt::Atan2", NUM_ITERS, {

        sum += Alt::Atan2(rnd.GenerateF64_01(), rnd.GenerateF64_01());
    });

    ALT_TIME_BENCH("std::atan2", NUM_ITERS, {

        sum += std::atan2(rnd.GenerateF64_01(), rnd.GenerateF64_01());
    });
}

TEST(Math, Sqrtf32Constexpr) {
    iferr_scope;

    constexpr usize MAX_THREADS = 32;

    struct alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) ThreadData {

        Alt::RangeU32 range;
    };

    const usize numThreads = Alt::Min(Alt::Thread::GetNumOfLogicalProcessors(), MAX_THREADS);

    Alt::StaticVector<Alt::Thread, 32> threads;
    Alt::StaticVector<ThreadData, 32> threadsData;

    for (usize i = 0; i < numThreads; ++i) {

        const u32 size = Alt::Desc<u32>::Max() / numThreads;

        Alt::RangeU32 range;
        range.begin = i * size;

        if (i + 1 == numThreads) {

            range.end = 0;
        }
        else {

            range.end = (i + 1) * size;
        }

        ThreadData data{};
        data.range = range;

        threadsData.Push(data) iferr_panic_only_dev("Increase thread size");

        Alt::Thread thread = Alt::Thread::CreateFromNativeFuncPtr([](void* pArgs) noexcept {

            const ThreadData& data = *reinterpret_cast<ThreadData*>(pArgs);

            for (u32 i = data.range.begin; i != data.range.end; ++i) {

                const f32 num = Alt::Memory::Transmute<u32, f32>((u32)i);

                const f32 x0 = Alt::_MathDetails::SqrtConstexpr(num);
                const f32 x1 = Alt::Sqrt(num);

                const ssize fpclass = std::fpclassify(num);
                if (!Alt::IsSignPositive(num) || fpclass == FP_INFINITE || fpclass == FP_NAN) {

                    EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1))
                        << "Sqrt(" << num << "): " << x0 << " != " << x1;
                }
                else {

                    EXPECT_TRUE(Alt::IsEqFuzzy(x0, x1))
                        << "Sqrt(" << num << "): " << x0 << " != " << x1;
                }

                if ((i - data.range.begin) % 100'000'000 == 0) {
                    ALT_PRINTLN("{}: {}", Alt::Thread::GetCurrentID(), (f32)(i - data.range.begin) / (f32)data.range.GetSize() * 100.0f);
                }
            }

            ALT_PRINTLN("{}: 100", Alt::Thread::GetCurrentID());

            ALT_THREAD_RETURN_SUCCESS;
        }, &threadsData[i]) iferr_panic_only_dev("Failed to create thread");

        Alt::Thread::PriorityDesc desc{};
        desc.isBoostEnabled = true;
        desc.priority = Alt::Thread::Priority::Highest;

        thread.SetPriority(desc, 1 << i);

        threads.Push(std::move(thread)) iferr_panic_only_dev("Increase thread size");
    }

    for (const Alt::Thread& thread : threads.GetIterator()) {

        thread.WaitCompletion();
    }

    {
        const f32 num = -0.0f;

        const f32 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f32 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f32 num = Alt::Memory::Transmute<u32, f32>(Alt::Desc<u32>::Max());

        const f32 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f32 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f32 num = Alt::Desc<f32>::Inf();

        const f32 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f32 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f32 num = -Alt::Desc<f32>::Inf();

        const f32 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f32 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f32 num = Alt::Desc<f32>::QuietNaN();

        const f32 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f32 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f32 num = -Alt::Desc<f32>::QuietNaN();

        const f32 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f32 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f32 num = Alt::Desc<f32>::SignalingNaN();

        const f32 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f32 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f32 num = -Alt::Desc<f32>::SignalingNaN();

        const f32 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f32 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }
}

TEST(Math, Sqrtf64Constexpr) {

    constexpr static usize NUM_ITERS = 10'000'000;

    Alt::Xoshiro256Plus rnd = Alt::Xoshiro256Plus::CreateSeededByTime();

    for (usize i = 0; i < NUM_ITERS; ++i) {

        const f64 num = rnd.GenerateF64_01();

        const f64 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f64 x1 = Alt::Sqrt(num);

        EXPECT_TRUE(Alt::Abs(x0 - x1) <= 0.00001f)
            << "Sqrt(" << num << ") = " << x0 << " != " << x1;
    }

    {
        const f64 num = -0.0f;

        const f64 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f64 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f64 num = Alt::Memory::Transmute<u64, f64>(Alt::Desc<u64>::Max());

        const f64 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f64 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f64 num = Alt::Desc<f64>::Inf();

        const f64 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f64 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f64 num = -Alt::Desc<f64>::Inf();

        const f64 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f64 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f64 num = Alt::Desc<f64>::QuietNaN();

        const f64 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f64 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f64 num = -Alt::Desc<f64>::QuietNaN();

        const f64 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f64 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f64 num = Alt::Desc<f64>::SignalingNaN();

        const f64 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f64 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }

    {
        const f64 num = -Alt::Desc<f64>::SignalingNaN();

        const f64 x0 = Alt::_MathDetails::SqrtConstexpr(num);
        const f64 x1 = Alt::Sqrt(num);

        EXPECT_EQ(std::fpclassify(x0), std::fpclassify(x1));
    }
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
//		const f32 x0 = Alt::Rsqrt(num);
//		const f32 x1 = 1.0f / std::sqrt(num);
//
//		EXPECT_TRUE(Alt::Abs(x0 - x1) <= 0.01f)
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
//		const f64 x0 = Alt::Rsqrt(num);
//		const f64 x1 = 1.0 / std::sqrt(num);
//
//		EXPECT_TRUE(Alt::Abs(x0 - x1) <= 0.01)
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
//	ALT_TIME_BENCH("Alt::Rsqrt", NUM_ITERS, {
//
//		sum += Alt::Rsqrt(rnd.GenerateF32_01());
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
//	ALT_TIME_BENCH("Alt::Rsqrt", NUM_ITERS, {
//
//		sum += Alt::Rsqrt(rnd.GenerateF64_01());
//	});
//
//	ALT_TIME_BENCH("1.0 / std::sqrt", NUM_ITERS, {
//
//		sum += 1.0 / std::sqrt(rnd.GenerateF64_01());
//	});
//}
