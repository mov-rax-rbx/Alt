#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Memory/Allocators/PoolAllocator.h"
#include "../alt-std/src/Platform/Platform.h"

namespace {

	struct Vec3 {
		f32 x, y, z;
	};

	struct Triangle {
		Vec3 normal;
		Vec3 v[3];
		char dummy[2];
	};

	struct alignas(64) AlignmentType {
		f32 data[16];
	};

	void PrintStats(const Alt::PoolAllocatorDynamic<>::Stats& stats) {

		ALT_PRINTLN("blocks: {}", stats.numBlocks);
		ALT_PRINTLN("consumedElements: {}", stats.consumedElements);
		ALT_PRINTLN("availableElements: {}", stats.availableElements);
		ALT_PRINTLN("------------------------------------------------\n");
	}
}

TEST(PoolAllocator, Move) {

	using Pool = Alt::PoolAllocatorDynamic<>;

	Pool::MemoryDesc desc = Pool::MemoryDesc::Create<Triangle>();
	desc.minNumElementsPerBlock = 2;

	Pool pool0{ desc };

	Pool::Stats stats = pool0.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 0);
	EXPECT_EQ(stats.consumedElements, 0);

	Triangle* pVal = pool0.Allocate<Triangle>();
	EXPECT_TRUE(pVal);

	stats = pool0.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 1);
	EXPECT_EQ(stats.consumedElements, 1);

	Pool pool1{ Pool::MemoryDesc{} };
	pool1 = std::move(pool0);

	stats = pool1.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 1);
	EXPECT_EQ(stats.consumedElements, 1);

	pVal = pool1.Allocate<Triangle>();
	EXPECT_TRUE(pVal);

	stats = pool1.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 1);
	EXPECT_EQ(stats.consumedElements, 2);

	Pool pool2{ desc };
	pVal = pool2.Allocate<Triangle>();
	EXPECT_TRUE(pVal);

	pool2 = std::move(pool1);

	stats = pool2.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 1);
	EXPECT_EQ(stats.consumedElements, 2);
}

TEST(PoolAllocator, StressTest) {

	using Pool = Alt::PoolAllocator<Triangle, 2>;

	Pool pool{};

	Pool::Stats stats = pool.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 0);
	EXPECT_EQ(stats.consumedElements, 0);

	Triangle* pVal = pool.Allocate<Triangle>();
	EXPECT_TRUE(pVal);

	stats = pool.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 1);
	EXPECT_EQ(stats.consumedElements, 1);

	pool.Deallocate(pVal);
	stats = pool.GetStats();
	EXPECT_EQ(stats.numBlocks, 1);
	EXPECT_EQ(stats.consumedElements, 0);

	constexpr usize ITERATIONS = 100;
	constexpr usize SIZE = 1'000'000;

	for (usize i = 0; i < ITERATIONS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			Triangle* pVal = pool.Allocate<Triangle>();
			EXPECT_TRUE(pVal);

			if (i % 2 == 0) {
				pool.Deallocate(pVal);
			}
		}

		stats = pool.GetStats();
		//PrintStats(stats);
		EXPECT_EQ(stats.numBlocks, Alt::Num::Log2(SIZE / 2));
		EXPECT_EQ(stats.consumedElements, SIZE / 2);

		if (i % 2 == 0) {

			pool.DeallocateBytesExceptFirstBlockAndReset();
			pool.DeallocateBytesExceptFirstBlockAndReset();

			stats = pool.GetStats();
			//PrintStats(stats);
			EXPECT_EQ(stats.numBlocks, 1);
			EXPECT_EQ(stats.consumedElements, 0);
		}
		else {

			pool.DeallocateBytesAllBlocks();
			pool.DeallocateBytesAllBlocks();

			stats = pool.GetStats();
			//PrintStats(stats);
			EXPECT_EQ(stats.numBlocks, 0);
			EXPECT_EQ(stats.consumedElements, 0);
		}
	}

	pool.DeallocateBytesAllBlocks();

	stats = pool.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 0);
	EXPECT_EQ(stats.consumedElements, 0);
}

TEST(PoolAllocator, FragmentationAndReuse) {

	using Pool = Alt::PoolAllocator<Triangle, 10, 10>;

	Pool pool{};

	constexpr static usize NUM_ELEMENTS = 100;
	Triangle* ptrs[NUM_ELEMENTS];

	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		ptrs[i] = pool.Allocate<Triangle>();
		ASSERT_TRUE(ptrs[i]);
	}

	for (usize i = 0; i < NUM_ELEMENTS; i += 2) {

		pool.Deallocate(ptrs[i]);
	}

	Pool::Stats stats = pool.GetStats();
	EXPECT_EQ(stats.consumedElements, NUM_ELEMENTS / 2);
	EXPECT_EQ(stats.availableElements, NUM_ELEMENTS / 2);

	for (usize i = 0; i < NUM_ELEMENTS / 2; ++i) {

		Triangle* pVal = pool.Allocate<Triangle>();
		EXPECT_TRUE(pVal);
	}

	stats = pool.GetStats();
	EXPECT_EQ(stats.consumedElements, 100);
}

TEST(PoolAllocator, Alignment) {

	constexpr static usize NUM_ELEMENTS = 10;

	using Pool = Alt::PoolAllocator<AlignmentType>;

	Pool pool{};

	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		AlignmentType* pVal = pool.Allocate<AlignmentType>();
		ASSERT_TRUE(pVal);

		EXPECT_EQ(Alt::Memory::Align(pVal, alignof(AlignmentType)), 0);

		pVal->data[0] = static_cast<f32>(i);
	}
}

TEST(PoolAllocator, DeallocateBytesOneElement) {

	constexpr static usize NUM_ELEMENTS = 20;

	using Pool = Alt::PoolAllocator<Triangle, 1, 1>;

	Pool pool{};

	Triangle* ptrs[NUM_ELEMENTS];
	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		ptrs[i] = pool.Allocate<Triangle>();
	}

	pool.Deallocate(ptrs[0]);
	pool.Deallocate(ptrs[5]);
	pool.Deallocate(ptrs[15]);

	pool.DeallocateBytesExceptFirstBlockAndReset();

	Triangle* pVal = pool.Allocate<Triangle>();
	EXPECT_TRUE(pVal); 

	Pool::Stats stats = pool.GetStats();
	EXPECT_EQ(stats.numBlocks, 1);
}
