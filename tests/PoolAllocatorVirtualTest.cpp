#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Memory/Allocators/PoolAllocatorVirtual.h"
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

	template <typename StatsT>
	void PrintStats(const StatsT& stats) {

		ALT_PRINTLN("consumedElements: {}", stats.consumedElements);
		ALT_PRINTLN("availableElements: {}", stats.availableElements);
		ALT_PRINTLN("------------------------------------------------\n");
	}
}

TEST(PoolAllocatorVirtual, Move) {

	iferr_scope_on_err {

		FAIL();
	};

	using Pool = Alt::PoolAllocatorVirtualDynamic;

	Pool::MemoryDesc desc = Pool::MemoryDesc::Create<Triangle>();
	desc.minNumElements = 2;

	Pool pool0 = Pool::CreateCapacity(desc) iferr_return;

	Pool::Stats stats0 = pool0.GetStats();
	//PrintStats(stats0);
	EXPECT_EQ(stats0.consumedElements, 0);
	EXPECT_EQ(stats0.availableElements, 0);

	Triangle* pVal = pool0.Allocate<Triangle>();
	EXPECT_TRUE(pVal);

	const usize totalAvailableElements0 = pool0.GetMinElementsSizeInBytes() / sizeof(Triangle);

	stats0 = pool0.GetStats();
	//PrintStats(stats0);
	ASSERT_LE(alignof(Triangle), Alt::Platform::MemoryDefaultAllocationAlignment());
	EXPECT_EQ(stats0.consumedElements, 1);
	EXPECT_EQ(stats0.availableElements, totalAvailableElements0 - stats0.consumedElements);

	Pool pool1 = Pool::CreateCapacity(Pool::MemoryDesc{}) iferr_return;
	pool1 = std::move(pool0);

	Pool::Stats stats1 = pool1.GetStats();
	//PrintStats(stats1);
	EXPECT_EQ(stats1.consumedElements, stats0.consumedElements);
	EXPECT_EQ(stats1.availableElements, stats0.availableElements);

	pVal = pool1.Allocate<Triangle>();
	EXPECT_TRUE(pVal);

	stats1 = pool1.GetStats();
	//PrintStats(stats1);
	EXPECT_EQ(stats1.consumedElements, stats0.consumedElements + 1);
	EXPECT_EQ(stats1.availableElements, stats0.availableElements - 1);

	Pool pool2 = Pool::CreateCapacity(desc) iferr_return;
	pVal = pool2.Allocate<Triangle>();
	EXPECT_TRUE(pVal);

	pool2 = std::move(pool1);

	Pool::Stats stats2 = pool2.GetStats();
	//PrintStats(stats2);
	EXPECT_EQ(stats2.consumedElements, stats1.consumedElements);
	EXPECT_EQ(stats2.availableElements, stats1.availableElements);
}

TEST(PoolAllocatorVirtual, StressTest) {

	iferr_scope_on_err {

		FAIL();
	};

	using Pool = Alt::PoolAllocatorVirtual<Triangle, 2>;

	Pool pool = Pool::CreateCapacity() iferr_return;

	Pool::Stats stats = pool.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.consumedElements, 0);
	EXPECT_EQ(stats.availableElements, 0);

	Triangle* pVal = pool.Allocate<Triangle>();
	EXPECT_TRUE(pVal);

	const usize totalAvailableElements = pool.GetMinElementsSizeInBytes() / sizeof(Triangle);

	stats = pool.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.consumedElements, 1);
	EXPECT_EQ(stats.availableElements, totalAvailableElements - stats.consumedElements);

	pool.Deallocate(pVal);
	stats = pool.GetStats();
	EXPECT_EQ(stats.consumedElements, 0);
	EXPECT_EQ(stats.availableElements, totalAvailableElements);

	constexpr usize ITERATIONS = 100;
	constexpr usize SIZE = 1'000'000;

	for (usize i = 0; i < ITERATIONS; ++i) {

		usize consumedElements = 0;

		for (usize i = 0; i < SIZE; ++i) {

			Triangle* pVal = pool.Allocate<Triangle>();
			EXPECT_TRUE(pVal);

			consumedElements += 1;

			if (i % 2 == 0) {

				pool.Deallocate(pVal);
				consumedElements -= 1;
			}
		}

		stats = pool.GetStats();
		//PrintStats(stats);
		EXPECT_EQ(stats.consumedElements, consumedElements);

		pool.ReleaseBytesAll();
		pool.ReleaseBytesAll();

		stats = pool.GetStats();
		//PrintStats(stats);
		EXPECT_EQ(stats.consumedElements, 0);
		EXPECT_EQ(stats.availableElements, 0);
	}

	pool.ReleaseBytesAll();

	stats = pool.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.consumedElements, 0);
	EXPECT_EQ(stats.availableElements, 0);
}

TEST(PoolAllocatorVirtual, FragmentationAndReuse) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize NUM_ELEMENTS = 100;

	using Pool = Alt::PoolAllocatorVirtual<Triangle, 10, NUM_ELEMENTS>;

	Pool pool = Pool::CreateCapacity() iferr_return;
	const usize totalAvailableElements = pool.GetMaxElementsSizeInBytesReserved() / sizeof(Triangle);

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
	EXPECT_EQ(stats.availableElements, totalAvailableElements - NUM_ELEMENTS  / 2);

	for (usize i = 0; i < NUM_ELEMENTS / 2; ++i) {

		Triangle* pVal = pool.Allocate<Triangle>();
		EXPECT_TRUE(pVal);
	}

	stats = pool.GetStats();
	EXPECT_EQ(stats.consumedElements, 100);
}

TEST(PoolAllocatorVirtual, Alignment) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize NUM_ELEMENTS = 10;

	using Pool = Alt::PoolAllocatorVirtual<AlignmentType>;

	Pool pool = Pool::CreateCapacity() iferr_return;

	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		AlignmentType* pVal = pool.Allocate<AlignmentType>();
		ASSERT_TRUE(pVal);

		EXPECT_EQ(Alt::Memory::Align(pVal, alignof(AlignmentType)), 0);

		pVal->data[0] = static_cast<f32>(i);
	}
}
