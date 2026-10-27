#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/String/String.h"
#include "../alt-std/src/Memory/Allocators/ArenaAllocatorVirtual.h"
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
		ALT_PRINTLN("------------------------------------------------\n");
	}
}

TEST(ArenaAllocatorVirtual, TypeTraits) {

	bool v = false;

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::ArenaAllocatorVirtual<>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::ArenaAllocatorVirtualDynamic>;
	EXPECT_TRUE(v);
}

TEST(ArenaAllocatorVirtual, AllocateDeallocateRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize INITIAL_SIZE = Alt::Num::RoundUpToPowerOf2Constexpr(Alt::Memory::KB(4));
	using Arena = Alt::ArenaAllocatorVirtual<INITIAL_SIZE>;

	Arena arena = Arena::CreateCapacity() iferr_return;

	for (usize i = 0; i < 2; ++i) {

		Arena::Checkpoint checkpoint = arena.GetCheckpoint();
		Arena::Stats stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.availableBytes, 0);
		EXPECT_EQ(stats.consumedBytes, 0);

		Packed* pVal0 = arena.Allocate<Packed>();
		EXPECT_TRUE(pVal0);

		stats = arena.GetStats();
		//PrintStats(stats);

		usize consumedBytes = sizeof(Packed);

		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		NonPacked* pVal1 = arena.Allocate<NonPacked>();
		EXPECT_TRUE(pVal1);

		stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(alignof(NonPacked), Alt::Platform::MemoryDefaultAllocationAlignment());
		consumedBytes += Alt::Memory::Align((void*)sizeof(Packed), alignof(NonPacked)) + sizeof(NonPacked);

		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		arena.ResetToCheckpoint(checkpoint);

		stats = arena.GetStats();
		//PrintStats(stats);

		consumedBytes = 0;
		EXPECT_EQ(stats.availableBytes, INITIAL_SIZE - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		arena.ReleaseBytesAll();

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.availableBytes, 0);
		EXPECT_EQ(stats.consumedBytes, 0);

		struct HugeStruct { u8 data[Alt::Memory::MB(2)]; };
		HugeStruct* pVal2 = arena.Allocate<HugeStruct>();
		ASSERT_TRUE(pVal2);

		stats = arena.GetStats();
		//PrintStats(stats);

		ASSERT_LE(alignof(HugeStruct), Alt::Platform::MemoryDefaultAllocationAlignment());
		consumedBytes = Alt::Memory::MB(2);

		EXPECT_EQ(stats.availableBytes, Alt::Num::RoundUpToPowerOf2(Alt::Memory::MB(2)) - consumedBytes);
		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		arena.ReleaseBytesAfter(0);

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.availableBytes, 0);
		EXPECT_EQ(stats.consumedBytes, 0);
	}
}

TEST(ArenaAllocatorVirtual, Move) {

	iferr_scope_on_err {

		FAIL();
	};

	using Arena = Alt::ArenaAllocatorVirtualDynamic;

	Arena::MemoryDesc desc{};
	desc.minSizeInBytes = Alt::Num::RoundUpWrap((usize)1234, Alt::Platform::GetPageSizeInBytes());
	desc.maxSizeInBytesReserved = Alt::Num::RoundUpWrap((usize)5678, Alt::Platform::GetPageSizeInBytes());

	Arena arena0 = Arena::CreateCapacity(desc) iferr_return;
	Alt::String* pVal = arena0.Allocate<Alt::String>();
	EXPECT_TRUE(pVal);

	Arena::Stats stats0 = arena0.GetStats();
	//PrintStats(stats0);

	EXPECT_EQ(stats0.availableBytes, desc.minSizeInBytes - sizeof(Alt::String));
	EXPECT_EQ(stats0.consumedBytes, sizeof(Alt::String));

	Arena arena1 = Arena::CreateCapacity(Arena::MemoryDesc{}) iferr_return;
	arena1 = std::move(arena0);

	EXPECT_EQ(arena1.GetMinSizeInBytes(), desc.minSizeInBytes);
	EXPECT_EQ(arena1.GetMaxSizeInBytesReserved(), desc.maxSizeInBytesReserved);

	Arena::Stats stats1 = arena1.GetStats();

	EXPECT_EQ(stats0.availableBytes, stats1.availableBytes);
	EXPECT_EQ(stats0.consumedBytes, stats1.consumedBytes);

	Arena arena2 = Arena::CreateCapacity(Arena::MemoryDesc{}) iferr_return;
	pVal = arena2.Allocate<Alt::String>();
	EXPECT_TRUE(pVal);

	arena2 = std::move(arena1);

	Arena::Stats stats2 = arena2.GetStats();

	EXPECT_EQ(stats0.availableBytes, stats2.availableBytes);
	EXPECT_EQ(stats0.consumedBytes, stats2.consumedBytes);

	arena2.ReleaseBytesAll();

	stats2 = arena2.GetStats();
	EXPECT_EQ(stats2.availableBytes, 0);
	EXPECT_EQ(stats2.consumedBytes, 0);
}

TEST(ArenaAllocatorVirtual, StressTest) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize SIZE = 1'000'000;
	constexpr static usize ITERATIONS = 100;

	using Arena = Alt::ArenaAllocatorVirtualDynamic;

	Arena arena = Arena::CreateCapacity(Arena::MemoryDesc{}) iferr_return;

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

		EXPECT_EQ(stats.consumedBytes, consumedBytes);

		arena.ResetToCheckpoint(checkpoint);

		arena.ReleaseBytesAfter(arena.GetMinSizeInBytes());

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.consumedBytes, arena.GetMinSizeInBytes());
		EXPECT_LE(stats.availableBytes, 0);

		arena.ReleaseBytesAll();

		stats = arena.GetStats();
		//PrintStats(stats);

		EXPECT_EQ(stats.consumedBytes, 0);
		EXPECT_LE(stats.availableBytes, 0);
	}
}

TEST(ArenaAllocatorVirtual, Alignment) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize NUM_ITERATIONS = 10;

	using Arena = Alt::ArenaAllocatorVirtual<>;

	Arena arena = Arena::CreateCapacity() iferr_return;

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
