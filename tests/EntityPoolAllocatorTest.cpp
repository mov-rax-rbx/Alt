#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Memory/Allocators/EntityPoolAllocator.h"
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

	template <typename PoolStatsT>
	void PrintStats(const PoolStatsT& stats) {

		ALT_PRINTLN("blocks: {}", stats.numBlocks);
		ALT_PRINTLN("consumedElements: {}", stats.consumedElements);
		ALT_PRINTLN("availableElements: {}", stats.availableElements);
		ALT_PRINTLN("------------------------------------------------\n");
	}
}

TEST(EntityPoolAllocator, Move) {

	using Pool = Alt::EntityPoolAllocatorDynamic;

	Pool::MemoryDesc desc = Pool::MemoryDesc::Create<Triangle>();

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

	pVal = pool2.Allocate<Triangle>();
	EXPECT_TRUE(pVal);

	stats = pool2.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 2);
	EXPECT_EQ(stats.consumedElements, 3);
}

TEST(EntityPoolAllocator, StressTest) {

	iferr_scope_on_err {

		FAIL();
	};

	using Pool = Alt::EntityPoolAllocator<Triangle>;

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

	pool.Deallocate(pool.FindBlockIDAndLocalIDByPtr(pVal).blockID, pVal);
	stats = pool.GetStats();
	EXPECT_EQ(stats.numBlocks, 1);
	EXPECT_EQ(stats.consumedElements, 0);

	constexpr usize ITERATIONS = 100;
	constexpr usize SIZE = 1'000'000;

	for (usize i = 0; i < ITERATIONS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			Pool::AllocateResult val = pool.AllocateWithResult<Triangle>() iferr_return;
			//EXPECT_EQ(val.blockID, pool.FindBlockIDAndLocalIDByPtr(val.pMemory).blockID);

			if (i % 2 == 0) {
				pool.Deallocate(val.blockID, val.pMemory);
			}
		}

		stats = pool.GetStats();
		//PrintStats(stats);
		EXPECT_EQ(stats.numBlocks, Alt::Num::Log2(Alt::Num::RoundUpToPowerOf2(SIZE / 2)));
		EXPECT_EQ(stats.consumedElements, SIZE / 2);

		if (i % 2 == 0) {

			pool.DeallocateBytesAllBlocksUnusedExceptFirstUnused();
			pool.DeallocateBytesAllBlocksUnusedExceptFirstUnused();

			stats = pool.GetStats();
			//PrintStats(stats);
			EXPECT_EQ(stats.numBlocks, Alt::Num::Log2(Alt::Num::RoundUpToPowerOf2(SIZE / 2)));
			EXPECT_EQ(stats.consumedElements, SIZE / 2);
		}
		else {

			const usize blocksSizeNew = Alt::Num::SubWrap(Alt::Num::Log2(Alt::Num::RoundUpToPowerOf2(SIZE / 2)), 1);
			pool.ReleaseAllBlocksExcept(blocksSizeNew);

			EXPECT_EQ(pool.DeallocateBytesAllBlocksUnused().GetValueOrDefault(), blocksSizeNew);
			EXPECT_FALSE(pool.DeallocateBytesAllBlocksUnused());

			stats = pool.GetStats();
			//PrintStats(stats);
			EXPECT_EQ(stats.numBlocks, blocksSizeNew);
			EXPECT_EQ(stats.consumedElements, pool.GetBlockSize(blocksSizeNew));
		}

		pool.DeallocateBytesExceptBlocksAndRelease(1);
	}

	pool.DeallocateBytesAllBlocksExcept(0);

	stats = pool.GetStats();
	//PrintStats(stats);
	EXPECT_EQ(stats.numBlocks, 0);
	EXPECT_EQ(stats.consumedElements, 0);
}

TEST(EntityPoolAllocator, FragmentationAndReuse) {

	using Pool = Alt::EntityPoolAllocator<Triangle>;

	Pool pool{};

	constexpr static usize NUM_ELEMENTS = 100;
	Triangle* ptrs[NUM_ELEMENTS];

	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		ptrs[i] = pool.Allocate<Triangle>();
		ASSERT_TRUE(ptrs[i]);
	}

	for (usize i = 0; i < NUM_ELEMENTS; i += 2) {

		pool.Deallocate(pool.FindBlockIDAndLocalIDByPtr(ptrs[i]).blockID, ptrs[i]);
	}

	Pool::Stats stats = pool.GetStats();
	EXPECT_EQ(stats.consumedElements, NUM_ELEMENTS / 2);
	EXPECT_EQ(stats.availableElements, Alt::Num::RoundUpToPowerOf2(NUM_ELEMENTS) - NUM_ELEMENTS / 2);

	for (usize i = 0; i < NUM_ELEMENTS / 2; ++i) {

		Triangle* pVal = pool.Allocate<Triangle>();
		EXPECT_TRUE(pVal);
	}

	stats = pool.GetStats();
	EXPECT_EQ(stats.consumedElements, 100);
}

TEST(EntityPoolAllocator, Alignment) {

	constexpr static usize NUM_ELEMENTS = 10;

	using Pool = Alt::EntityPoolAllocator<AlignmentType>;

	Pool pool{};

	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		AlignmentType* pVal = pool.Allocate<AlignmentType>();
		ASSERT_TRUE(pVal);

		EXPECT_EQ(Alt::Memory::Align(pVal, alignof(AlignmentType)), 0);

		pVal->data[0] = static_cast<f32>(i);
	}
}

TEST(EntityPoolAllocator, DeallocateBytesOneElement) {

	constexpr static usize NUM_ELEMENTS = 20;

	using Pool = Alt::EntityPoolAllocator<Triangle>;

	Pool pool{};

	Triangle* ptrs[NUM_ELEMENTS];
	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		ptrs[i] = pool.Allocate<Triangle>();
	}

	pool.Deallocate(pool.FindBlockIDAndLocalIDByPtr(ptrs[0]).blockID, ptrs[0]);
	pool.Deallocate(pool.FindBlockIDAndLocalIDByPtr(ptrs[5]).blockID, ptrs[5]);
	pool.Deallocate(pool.FindBlockIDAndLocalIDByPtr(ptrs[15]).blockID, ptrs[15]);

	pool.DeallocateBytesExceptBlocksAndRelease(1);

	Triangle* pVal = pool.Allocate<Triangle>();
	EXPECT_TRUE(pVal); 

	Pool::Stats stats = pool.GetStats();
	EXPECT_EQ(stats.numBlocks, 1);
}

TEST(EntityPoolAllocator, Iterator) {

	iferr_scope_on_err {

		FAIL();
	};

	using Pool = Alt::EntityPoolAllocator<Triangle>;

	Pool pool{};
	pool.Allocate<Triangle>() iferr_return;

	Pool::IteratorRange<Triangle> iterator = pool.GetIteratorRange<Triangle>(0, 1);
	EXPECT_TRUE(iterator.Next());
	EXPECT_FALSE(iterator.Next());
	EXPECT_EQ(iterator.GetSize(), 0);

	const usize NUM_ELEMENTS = 10'000;
	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		pool.Allocate<Triangle>() iferr_return;
	}

	iterator = pool.GetIteratorRange<Triangle>(1, NUM_ELEMENTS);
	EXPECT_EQ(iterator.GetSize(), NUM_ELEMENTS);

	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		EXPECT_TRUE(iterator.Next());
	}
	EXPECT_FALSE(iterator.Next());
	EXPECT_EQ(iterator.GetSize(), 0);

	iterator = pool.GetIteratorRange<Triangle>(0, NUM_ELEMENTS + 1);
	EXPECT_EQ(iterator.GetSize(), NUM_ELEMENTS + 1);

	for (usize i = 0; i < NUM_ELEMENTS; ++i) {

		Pool::IteratorRange<Triangle>::Result result = iterator.Next() iferr_return;

		EXPECT_EQ(pool.BlockIDAndLocalIDToID(result.blockID, result.localID), i);
		EXPECT_EQ(result.blockID, pool.FindBlockIDAndLocalIDByPtr(result.pResult).blockID);

		if (i % 2 == 0) {
			pool.Deallocate(result.blockID, result.pResult);
		}
	}

	for (usize i = 0; i < (NUM_ELEMENTS + 1) / 2; ++i) {

		iterator = pool.GetIteratorRange<Triangle>(i * 2 + 1, 1);
		EXPECT_TRUE(iterator.Next());
		EXPECT_FALSE(iterator.Next());
		EXPECT_EQ(iterator.GetSize(), 0);
	}

	iterator = pool.GetIteratorRange<Triangle>(0, NUM_ELEMENTS + 1);
	EXPECT_EQ(iterator.GetSize(), NUM_ELEMENTS + 1);

	for (usize i = 0; i < (NUM_ELEMENTS + 1) / 2; ++i) {

		EXPECT_TRUE(iterator.Next());
		iterator.Advance(1);
	}

	if constexpr ((NUM_ELEMENTS + 1) % 2 != 0) {
		iterator.Advance(1);
	}
	EXPECT_FALSE(iterator.Next());
	EXPECT_EQ(iterator.GetSize(), 0);

	pool.DeallocateBytesAllBlocksExcept(0);
}
