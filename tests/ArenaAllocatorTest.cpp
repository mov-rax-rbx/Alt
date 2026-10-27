#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/String/String.h"
#include "../alt-std/src/Memory/Allocators/ArenaAllocator.h"
#include "../alt-std/src/Platform/Platform.h"

namespace {

#pragma pack(push, 1)
	struct PackedVec3 {
		f32 x, y, z;
	};

	struct Packed {
		PackedVec3 normal;
		PackedVec3 v[2];
		char dummy[2];
	};
#pragma pack(pop)

	struct NonPackedVec3 {
		f32 x, y, z;
	};

	struct NonPacked {
		NonPackedVec3 normal;
		NonPackedVec3 v[2];
		char dummy[2];
	};

	struct alignas(64) AlignmentType {
		f32 data[16];
	};

	template <typename StatsT>
	void PrintStats(const StatsT& stats) {

		ALT_PRINTLN("availableBytes: {} bytes", stats.availableBytes);
		ALT_PRINTLN("consumedBytes: {} bytes", stats.consumedBytes);
		ALT_PRINTLN("numBlocks: {}", stats.numBlocks);
		ALT_PRINTLN("wastedBytes: {} bytes", stats.wastedBytes);
		ALT_PRINTLN("------------------------------------------------\n");
	}
}

TEST(ArenaAllocator, TypeTraits) {

	bool v = false;

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::ArenaAllocator<>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::ArenaAllocatorDynamic<>>;
	EXPECT_TRUE(v);
}

TEST(ArenaAllocator, AllocateDeallocateRepeat) {

	constexpr static usize INITIAL_SIZE = Alt::Memory::KB(4);
	Alt::ArenaAllocator<INITIAL_SIZE, Alt::Num::Desc<usize>::Max(), Alt::ArenaAllocatorBits::None> arena{};

	for (usize i = 0; i < 2; ++i) {

		Alt::ArenaAllocator<>::Checkpoint checkpoint = arena.GetCheckpoint();
		Alt::ArenaAllocator<>::Stats stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.availableBytes, 0);
		EXPECT_EQ(stats.consumedBytes, 0);
		EXPECT_EQ(stats.numBlocks, 0);
		EXPECT_EQ(stats.wastedBytes, 0);

		Packed* pVal0 = arena.Allocate<Packed>();
		EXPECT_TRUE(pVal0);

		stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(Alt::ArenaAllocator<>::GetExtraBlockAlignOf(), Alt::Platform::MemoryDefaultAllocationAlignment());
		usize consumedBytes = Alt::ArenaAllocator<>::GetExtraBlockSizeOf() + sizeof(Packed);

		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);
		EXPECT_EQ(stats.numBlocks, 1);
		EXPECT_EQ(stats.wastedBytes, 0);

		NonPacked* pVal1 = arena.Allocate<NonPacked>();
		EXPECT_TRUE(pVal1);

		stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(alignof(NonPacked), Alt::Platform::MemoryDefaultAllocationAlignment());
		consumedBytes += Alt::Memory::Align((void*)sizeof(Packed), alignof(NonPacked)) + sizeof(NonPacked);

		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);
		EXPECT_EQ(stats.numBlocks, 1);
		EXPECT_EQ(stats.wastedBytes, 0);

		arena.ResetToCheckpoint(checkpoint);

		stats = arena.GetStats();
		//PrintStats(stats);

		consumedBytes = Alt::ArenaAllocator<>::GetExtraBlockSizeOf();
		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);
		EXPECT_EQ(stats.numBlocks, 1);
		EXPECT_EQ(stats.wastedBytes, 0);

		arena.DeallocateBytesExceptFirstBlockAndReset();

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);
		EXPECT_EQ(stats.numBlocks, 1);
		EXPECT_EQ(stats.wastedBytes, 0);

		struct HugeStruct { u8 data[Alt::Memory::MB(2)]; };
		HugeStruct* pVal2 = arena.Allocate<HugeStruct>();
		ASSERT_TRUE(pVal2);

		stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(alignof(HugeStruct), Alt::Platform::MemoryDefaultAllocationAlignment());
		const usize extraBlockSize = Alt::ArenaAllocator<>::GetExtraBlockSizeOf();

		EXPECT_EQ(stats.availableBytes, 0);
		EXPECT_EQ(stats.consumedBytes, Alt::Memory::MB(2) + extraBlockSize * stats.numBlocks);
		EXPECT_EQ(stats.numBlocks, 2);
		EXPECT_EQ(stats.wastedBytes, Alt::Memory::KB(4) - extraBlockSize);

		arena.DeallocateBytesAll();

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.availableBytes, 0);
		EXPECT_EQ(stats.consumedBytes, 0);
		EXPECT_EQ(stats.numBlocks, 0);
		EXPECT_EQ(stats.wastedBytes, 0);
	}
}

TEST(ArenaAllocator, AllocateDeallocateRepeatWithPool) {

	constexpr static usize INITIAL_SIZE = Alt::Memory::KB(4);
	Alt::ArenaAllocator<INITIAL_SIZE, Alt::Num::Desc<usize>::Max(), Alt::ArenaAllocatorBits::PoolForInternalBlocks> arena{};

	for (usize i = 0; i < 2; ++i) {

		Alt::ArenaAllocator<>::Checkpoint checkpoint = arena.GetCheckpoint();
		Alt::ArenaAllocator<>::Stats stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.availableBytes, 0);
		EXPECT_EQ(stats.consumedBytes, 0);
		EXPECT_EQ(stats.numBlocks, 0);
		EXPECT_EQ(stats.wastedBytes, 0);

		Packed* pVal0 = arena.Allocate<Packed>();
		EXPECT_TRUE(pVal0);

		stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(Alt::ArenaAllocator<>::GetExtraBlockAlignOf(), Alt::Platform::MemoryDefaultAllocationAlignment());
		usize consumedBytes = Alt::ArenaAllocator<>::GetExtraBlockSizeOf() + sizeof(Packed);

		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);
		EXPECT_EQ(stats.numBlocks, 1);
		EXPECT_EQ(stats.wastedBytes, 0);

		NonPacked* pVal1 = arena.Allocate<NonPacked>();
		EXPECT_TRUE(pVal1);

		stats = arena.GetStats();
		//PrintStats(stats);

		consumedBytes += Alt::Memory::Align((void*)sizeof(Packed), alignof(NonPacked)) + sizeof(NonPacked);
		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);
		EXPECT_EQ(stats.numBlocks, 1);
		EXPECT_EQ(stats.wastedBytes, 0);

		arena.ResetToCheckpoint(checkpoint);

		stats = arena.GetStats();
		//PrintStats(stats);

		consumedBytes = Alt::ArenaAllocator<>::GetExtraBlockSizeOf();
		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);
		EXPECT_EQ(stats.numBlocks, 1);
		EXPECT_EQ(stats.wastedBytes, 0);

		arena.ReleaseBytesAllToPool();
		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);
		EXPECT_EQ(stats.numBlocks, 1);
		EXPECT_EQ(stats.wastedBytes, 0);

		struct HugeStruct { u8 data[Alt::Memory::MB(2)]; };
		HugeStruct* pVal2 = arena.Allocate<HugeStruct>();
		ASSERT_TRUE(pVal2);

		stats = arena.GetStats();
		//PrintStats(stats);

		consumedBytes = Alt::ArenaAllocator<>::GetExtraBlockSizeOf() * stats.numBlocks;
		EXPECT_EQ(stats.availableBytes, 0);
		EXPECT_EQ(stats.consumedBytes, Alt::Memory::MB(2) + consumedBytes);
		EXPECT_EQ(stats.numBlocks, 1); // 1 because we are deallocating small blocks
		EXPECT_EQ(stats.wastedBytes, 0);

		arena.DeallocateBytesAll();

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.availableBytes, 0);
		EXPECT_EQ(stats.consumedBytes, 0);
		EXPECT_EQ(stats.numBlocks, 0);
		EXPECT_EQ(stats.wastedBytes, 0);
	}
}

TEST(ArenaAllocator, Move) {

	Alt::ArenaAllocatorDynamic<>::MemoryDesc desc{};
	desc.minSizeInBytes = Alt::Num::RoundUpWrap((usize)1234, Alt::Platform::GetPageSizeInBytes());
	desc.maxSizeInBytes = Alt::Num::RoundUpWrap((usize)5678, Alt::Platform::GetPageSizeInBytes());

	Alt::ArenaAllocatorDynamic<> arena0(desc);
	Alt::String* pVal = arena0.Allocate<Alt::String>();
	EXPECT_TRUE(pVal);

	Alt::ArenaAllocatorDynamic<>::Stats stats0 = arena0.GetStats();
	//PrintStats(stats0);

	ASSERT_LE(Alt::ArenaAllocator<>::GetExtraBlockAlignOf(), Alt::Platform::MemoryDefaultAllocationAlignment());
	usize consumedBytes = Alt::ArenaAllocatorDynamic<>::GetExtraBlockSizeOf() + sizeof(Alt::String);

	EXPECT_EQ(stats0.availableBytes, desc.minSizeInBytes - consumedBytes);
	EXPECT_EQ(stats0.consumedBytes, consumedBytes);
	EXPECT_EQ(stats0.numBlocks, 1);
	EXPECT_EQ(stats0.wastedBytes, 0);

	Alt::ArenaAllocatorDynamic<> arena1{ Alt::ArenaAllocatorDynamic<>::MemoryDesc{} };
	arena1 = std::move(arena0);

	EXPECT_EQ(arena1.GetMinSizeInBytes(), desc.minSizeInBytes);
	EXPECT_EQ(arena1.GetMaxSizeInBytes(), desc.maxSizeInBytes);

	Alt::ArenaAllocatorDynamic<>::Stats stats1 = arena1.GetStats();

	EXPECT_EQ(stats0.availableBytes, stats1.availableBytes);
	EXPECT_EQ(stats0.consumedBytes, stats1.consumedBytes);
	EXPECT_EQ(stats0.numBlocks, stats1.numBlocks);
	EXPECT_EQ(stats0.wastedBytes, stats1.wastedBytes);

	Alt::ArenaAllocatorDynamic<> arena2{ Alt::ArenaAllocatorDynamic<>::MemoryDesc{} };
	pVal = arena2.Allocate<Alt::String>();
	EXPECT_TRUE(pVal);

	arena2 = std::move(arena1);

	Alt::ArenaAllocatorDynamic<>::Stats stats2 = arena2.GetStats();

	EXPECT_EQ(stats0.availableBytes, stats2.availableBytes);
	EXPECT_EQ(stats0.consumedBytes, stats2.consumedBytes);
	EXPECT_EQ(stats0.numBlocks, stats2.numBlocks);
	EXPECT_EQ(stats0.wastedBytes, stats2.wastedBytes);

	arena2.DeallocateBytesAll();

	stats2 = arena2.GetStats();
	EXPECT_EQ(stats2.availableBytes, 0);
	EXPECT_EQ(stats2.consumedBytes, 0);
	EXPECT_EQ(stats2.numBlocks, 0);
	EXPECT_EQ(stats2.wastedBytes, 0);
}

TEST(ArenaAllocator, StressTest) {

	constexpr static usize SIZE = 1'000'000;
	constexpr static usize ITERATIONS = 100;
	constexpr static usize INITIAL_SIZE = 64;

	using Arena = Alt::ArenaAllocator<INITIAL_SIZE, Alt::Memory::MB(256), Alt::ArenaAllocatorBits::None>;
	//using Arena = Alt::ArenaAllocator<INITIAL_SIZE * 100, INITIAL_SIZE * 100>;

	Arena arena{};

	for (usize i = 0; i < ITERATIONS; ++i) {

		usize consumedBytes = 0;

		Arena::Checkpoint checkpoint{};
		for (usize i = 0; i < SIZE; ++i) {

			auto pVal = arena.Allocate<NonPacked>();
			EXPECT_TRUE(pVal);

			static_assert(alignof(NonPacked) <= Alt::Memory::MallocDefaultAlignment());
			consumedBytes += sizeof(NonPacked);

			if (i == SIZE / 4) {
				checkpoint = arena.GetCheckpoint();
			}
		}

		Arena::Stats stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(Alt::ArenaAllocator<>::GetExtraBlockAlignOf(), Alt::Platform::MemoryDefaultAllocationAlignment());
		consumedBytes += Alt::ArenaAllocator<>::GetExtraBlockSizeOf() * stats.numBlocks;

		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		arena.ResetToCheckpoint(checkpoint);

		if (i % 2 == 0) {

			arena.DeallocateBytesExceptFirstBlockAndReset();
			arena.DeallocateBytesExceptFirstBlockAndReset();
		}
		else {

			arena.DeallocateBytesExceptLargestBlockAndReset();
			arena.DeallocateBytesExceptLargestBlockAndReset();
		}

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.numBlocks, 1);
	}
}

TEST(ArenaAllocator, StressTestExponentialBlocks) {

	constexpr static usize SIZE = 1'000'000;
	constexpr static usize ITERATIONS = 100;
	constexpr static usize INITIAL_SIZE = 64;

	using Arena = Alt::ArenaAllocatorDynamic<Alt::ArenaAllocatorTrait::ExponentialBlockSize, Alt::ArenaAllocatorBits::None>;
	Arena arena{ Arena::MemoryDesc{ INITIAL_SIZE } };

	for (usize i = 0; i < ITERATIONS; ++i) {

		usize consumedBytes = 0;

		Arena::Checkpoint checkpoint{};
		for (usize i = 0; i < SIZE; ++i) {

			auto pVal = arena.Allocate<NonPacked>();
			EXPECT_TRUE(pVal);

			static_assert(alignof(NonPacked) <= Alt::Memory::MallocDefaultAlignment());
			consumedBytes += sizeof(NonPacked);

			if (i == SIZE / 4) {
				checkpoint = arena.GetCheckpoint();
			}
		}

		Arena::Stats stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(Arena::GetExtraBlockAlignOf(), Alt::Platform::MemoryDefaultAllocationAlignment());
		consumedBytes += Arena::GetExtraBlockSizeOf() * stats.numBlocks;

		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		arena.ResetToCheckpoint(checkpoint);

		if (i % 2 == 0) {

			arena.DeallocateBytesExceptFirstBlockAndReset();
			arena.DeallocateBytesExceptFirstBlockAndReset();
		}
		else {

			arena.DeallocateBytesExceptLargestBlockAndReset();
			arena.DeallocateBytesExceptLargestBlockAndReset();
		}

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.numBlocks, 1);
	}
}

TEST(ArenaAllocator, StressTestExponentialBlocksWithPool) {

	constexpr static usize SIZE = 1'000'000;
	constexpr static usize ITERATIONS = 100;
	constexpr static usize INITIAL_SIZE = 64;

	using Arena = Alt::ArenaAllocatorDynamic<Alt::ArenaAllocatorTrait::ExponentialBlockSize, Alt::ArenaAllocatorBits::PoolForInternalBlocks>;
	Arena arena{ Arena::MemoryDesc{ INITIAL_SIZE } };

	for (usize i = 0; i < ITERATIONS; ++i) {

		usize consumedBytes = 0;

		Arena::Checkpoint checkpoint{};
		for (usize i = 0; i < SIZE; ++i) {

			auto pVal = arena.Allocate<NonPacked>();
			EXPECT_TRUE(pVal);

			static_assert(alignof(NonPacked) <= Alt::Memory::MallocDefaultAlignment());
			consumedBytes += sizeof(NonPacked);

			if (i == SIZE / 4) {
				checkpoint = arena.GetCheckpoint();
			}
		}

		Arena::Stats stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(Arena::GetExtraBlockAlignOf(), Alt::Platform::MemoryDefaultAllocationAlignment());
		consumedBytes += Arena::GetExtraBlockSizeOf() * stats.numBlocks;

		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		arena.ResetToCheckpoint(checkpoint);

		if (i % 2 == 0) {

			arena.DeallocateBytesExceptFirstBlockAndReset();
			arena.DeallocateBytesExceptFirstBlockAndReset();

			stats = arena.GetStats();
			//PrintStats(stats);

			EXPECT_EQ(stats.numBlocks, 1);
		}
		else {

			arena.ReleaseBytesAllToPool();
			arena.ReleaseBytesAllToPool();

			const usize numBlockOld = stats.numBlocks;

			stats = arena.GetStats();
			//PrintStats(stats);

			EXPECT_EQ(stats.numBlocks, numBlockOld);
		}
	}
}

TEST(ArenaAllocator, StressTestFixesBlockSize) {

	constexpr static usize SIZE = 1'000'000;
	constexpr static usize ITERATIONS = 100;
	constexpr static usize INITIAL_SIZE = Alt::Memory::MB(256);

	using Arena = Alt::ArenaAllocatorDynamic<Alt::ArenaAllocatorTrait::FixedBlockSize, Alt::ArenaAllocatorBits::None>;
	Arena arena{ Arena::MemoryDesc{ INITIAL_SIZE } };

	for (usize i = 0; i < ITERATIONS; ++i) {

		usize consumedBytes = 0;

		Arena::Checkpoint checkpoint{};
		for (usize i = 0; i < SIZE; ++i) {

			auto pVal = arena.Allocate<NonPacked>();
			EXPECT_TRUE(pVal);

			static_assert(alignof(NonPacked) <= Alt::Memory::MallocDefaultAlignment());
			consumedBytes += sizeof(NonPacked);

			if (i == SIZE / 4) {
				checkpoint = arena.GetCheckpoint();
			}
		}

		Arena::Stats stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(Arena::GetExtraBlockAlignOf(), Alt::Platform::MemoryDefaultAllocationAlignment());
		consumedBytes += Arena::GetExtraBlockSizeOf() * stats.numBlocks;

		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		arena.ResetToCheckpoint(checkpoint);

		if (i % 2 == 0) {

			arena.DeallocateBytesExceptFirstBlockAndReset();
			arena.DeallocateBytesExceptFirstBlockAndReset();
		}
		else {

			arena.DeallocateBytesExceptLargestBlockAndReset();
			arena.DeallocateBytesExceptLargestBlockAndReset();
		}

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.numBlocks, 1);
	}
}

TEST(ArenaAllocator, StressTestArenaWithPool) {

	constexpr static usize SIZE = 1'000'000;
	constexpr static usize ITERATIONS = 100;
	constexpr static usize INITIAL_SIZE = 64;

	using Arena = Alt::ArenaAllocatorDynamic<Alt::ArenaAllocatorTrait::ExponentialBlockSize, Alt::ArenaAllocatorBits::PoolForInternalBlocks>;
	Arena arena{ Arena::MemoryDesc{ INITIAL_SIZE } };

	for (usize i = 0; i < ITERATIONS; ++i) {

		usize consumedBytes = 0;

		Arena::Checkpoint checkpoint{};
		for (usize i = 0; i < SIZE; ++i) {

			auto pVal = arena.Allocate<NonPacked>();
			EXPECT_TRUE(pVal);

			static_assert(alignof(NonPacked) <= Alt::Memory::MallocDefaultAlignment());
			consumedBytes += sizeof(NonPacked);

			if (i == SIZE / 4) {
				checkpoint = arena.GetCheckpoint();
			}
		}

		Arena::Stats stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(Arena::GetExtraBlockAlignOf(), Alt::Platform::MemoryDefaultAllocationAlignment());
		consumedBytes += Arena::GetExtraBlockSizeOf() * stats.numBlocks;

		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		arena.ResetToCheckpoint(checkpoint);

		if (i % 2 == 0) {

			arena.DeallocateBytesExceptFirstBlockAndReset();
			arena.DeallocateBytesExceptFirstBlockAndReset();

			stats = arena.GetStats();
			//PrintStats(stats);

			EXPECT_EQ(stats.numBlocks, 1);
		}
		else {

			arena.ReleaseBytesAllToPool();
			arena.ReleaseBytesAllToPool();

			const usize numBlockOld = stats.numBlocks;

			stats = arena.GetStats();
			//PrintStats(stats);

			EXPECT_EQ(stats.numBlocks, numBlockOld);
		}
	}
}

TEST(ArenaAllocator, Alignment) {

	constexpr static usize NUM_ITERATIONS = 10;

	using Arena = Alt::ArenaAllocator<64, 1024, Alt::ArenaAllocatorBits::None>;
	Arena arena{};

	for (usize i = 0; i < NUM_ITERATIONS; ++i) {

		for (usize i = 0; i < NUM_ITERATIONS; ++i) {

			u8* pVal0 = arena.Allocate<u8>();
			ASSERT_TRUE(pVal0);
		}

		for (usize i = 0; i < NUM_ITERATIONS; ++i) {

			AlignmentType* pVal1 = arena.Allocate<AlignmentType>();
			ASSERT_TRUE(pVal1);
			ASSERT_EQ(Alt::Memory::Align(pVal1, alignof(AlignmentType)), 0);
		}

		for (usize i = 0; i < NUM_ITERATIONS; ++i) {

			u64* pVal2 = arena.Allocate<u64>(); 
			ASSERT_TRUE(pVal2);
			ASSERT_EQ(Alt::Memory::Align(pVal2, alignof(u64)), 0);
		}

		//Arena::Stats stats = arena.GetStats();
		//PrintStats(stats);
	}
}
