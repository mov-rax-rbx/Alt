#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Collections/StaticBitSet.h"

TEST(StaticBitSet, TypeTraits) {

	bool v = true;

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::StaticBitSet<0>>;
	EXPECT_TRUE(v);
}

TEST(StaticBitSet, Allocation) {

	{
		Alt::StaticBitSet<4> bs{};
		EXPECT_EQ(bs.GetSize(), 4);
		EXPECT_FALSE(bs[0]);
		EXPECT_FALSE(bs[1]);
		EXPECT_FALSE(bs[2]);
		EXPECT_FALSE(bs[3]);
	}

	{
		Alt::StaticBitSet<4> bs{ true };
		EXPECT_EQ(bs.GetSize(), 4);
		EXPECT_TRUE(bs[0]);
		EXPECT_TRUE(bs[1]);
		EXPECT_TRUE(bs[2]);
		EXPECT_TRUE(bs[3]);
	}
}

TEST(StaticBitSet, Compare) {

	Alt::StaticBitSet<66> bs0{ true };
	Alt::StaticBitSet<66> bs1{ true };

	EXPECT_EQ(bs0, bs1);

	bs0[32] = false;
	bs1[32] = false;

	EXPECT_EQ(bs0, bs1);

	bs0[0] = false;

	EXPECT_NE(bs0, bs1);
}

TEST(StaticBitSet, Iterator) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticBitSet<9> bs;

	bs[0] = true;
	bs[1] = true;
	bs[2] = true;

	auto it = bs.GetIterator();

	{
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
	}

	bs[3] = false;

	it = bs.GetIterator();

	{
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_FALSE(*it.Next());
	}

	bs[4] = false;
	bs[5] = true;
	bs[6] = false;
	bs[7] = true;
	bs[8] = false;

	it = bs.GetIterator();

	{
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_FALSE(*it.Next());
		EXPECT_FALSE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_FALSE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_FALSE(*it.Next());
	}

	it = bs.GetIterator();

	{
		it.Advance(1);
		EXPECT_TRUE(*it.Next());
		it.Advance(2);
		EXPECT_FALSE(*it.Next());
		it.Advance(4);

		EXPECT_FALSE(it.Next());
	}
}

TEST(StaticBitSet, IteratorRangeActive) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticBitSet<11> bs;

	bs[0] = true;
	bs[1] = true;
	bs[2] = true;

	auto it = bs.GetIteratorActiveRange();
	Alt::RangeUsize range = it.Next() iferr_return;

	{
		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);
	}

	bs[3] = false;

	it = bs.GetIteratorActiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);
	}

	bs[4] = true;
	bs[5] = true;

	it = bs.GetIteratorActiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 6);
	}

	bs[6] = false;
	bs[7] = true;
	bs[8] = false;
	bs[9] = true;
	bs[10] = false;

	it = bs.GetIteratorActiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 6);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 7);
		EXPECT_EQ(range.end, 8);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 9);
		EXPECT_EQ(range.end, 10);
	}

	bs.SetAll(false);

	it = bs.GetIteratorActiveRange();
	EXPECT_FALSE(it.Next());

	bs.SetAll(true);

	it = bs.GetIteratorActiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, bs.GetSize());
	}
}

TEST(StaticBitSet, IteratorRangeActiveOffset) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticBitSet<11> bs;

	bs[0] = true;
	bs[1] = true;
	bs[2] = true;

	auto it = bs.GetIteratorActiveRange(1);
	Alt::RangeUsize range = it.Next() iferr_return;

	{
		EXPECT_EQ(range.begin, 1);
		EXPECT_EQ(range.end, 3);
	}

	bs[3] = false;

	it = bs.GetIteratorActiveRange(2, 1);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 2);
		EXPECT_EQ(range.end, 3);
	}

	bs[4] = true;
	bs[5] = true;

	it = bs.GetIteratorActiveRange(5);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 5);
		EXPECT_EQ(range.end, 6);
	}

	bs[6] = false;
	bs[7] = true;
	bs[8] = false;
	bs[9] = true;
	bs[10] = false;

	it = bs.GetIteratorActiveRange(4, 10 - 4);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 6);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 7);
		EXPECT_EQ(range.end, 8);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 9);
		EXPECT_EQ(range.end, 10);
	}

	bs.SetAll(false);

	it = bs.GetIteratorActiveRange();
	EXPECT_FALSE(it.Next());

	bs.SetAll(true);

	it = bs.GetIteratorActiveRange(bs.GetSize());
	{
		EXPECT_FALSE(it.Next());
		EXPECT_FALSE(it.Next());
	}
}

TEST(StaticBitSet, IteratorRangeInactive) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticBitSet<11> bs{ true };

	bs[0] = false;
	bs[1] = false;
	bs[2] = false;

	auto it = bs.GetIteratorInactiveRange();
	Alt::RangeUsize range = it.Next() iferr_return;

	{
		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);
	}

	bs[3] = true;

	it = bs.GetIteratorInactiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);
	}

	bs[4] = false;
	bs[5] = false;
	bs[6] = true;

	it = bs.GetIteratorInactiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 6);
	}

	bs[6] = true;
	bs[7] = false;
	bs[8] = true;
	bs[9] = false;
	bs[10] = true;

	it = bs.GetIteratorInactiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 6);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 7);
		EXPECT_EQ(range.end, 8);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 9);
		EXPECT_EQ(range.end, 10);
	}

	bs.SetAll(true);

	it = bs.GetIteratorInactiveRange();
	EXPECT_FALSE(it.Next());

	bs.SetAll(false);

	it = bs.GetIteratorInactiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, bs.GetSize());
	}
}

TEST(StaticBitSet, IteratorRangeInactiveOffset) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticBitSet<11> bs{ true };

	bs[0] = false;
	bs[1] = false;
	bs[2] = false;

	auto it = bs.GetIteratorInactiveRange(1);
	Alt::RangeUsize range = it.Next() iferr_return;

	{
		EXPECT_EQ(range.begin, 1);
		EXPECT_EQ(range.end, 3);
	}

	bs[3] = true;

	it = bs.GetIteratorInactiveRange(2, 1);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 2);
		EXPECT_EQ(range.end, 3);
	}

	bs[4] = false;
	bs[5] = false;
	bs[6] = true;

	it = bs.GetIteratorInactiveRange(5);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 5);
		EXPECT_EQ(range.end, 6);
	}

	bs[6] = true;
	bs[7] = false;
	bs[8] = true;
	bs[9] = false;
	bs[10] = true;

	it = bs.GetIteratorInactiveRange(4, 10 - 4);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 6);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 7);
		EXPECT_EQ(range.end, 8);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 9);
		EXPECT_EQ(range.end, 10);
	}

	bs.SetAll(true);

	it = bs.GetIteratorInactiveRange();
	EXPECT_FALSE(it.Next());

	bs.SetAll(false);

	it = bs.GetIteratorInactiveRange(bs.GetSize());
	{
		EXPECT_FALSE(it.Next());
		EXPECT_FALSE(it.Next());
	}
}

TEST(StaticBitSet, FindFirst) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticBitSet<100> bs{ false };

	Alt::Optional<usize> res = bs.FindFirstSet(24);
	EXPECT_FALSE(res);

	res = bs.FindFirstNonSet(0);
	ASSERT_TRUE(res);
	EXPECT_EQ(*res, 0);

	bs[bs.GetSize() - 1] = true;

	res = bs.FindFirstSet(0);
	EXPECT_TRUE(res);
	EXPECT_EQ(*res, bs.GetSize() - 1);

	bs.SetAll(true);

	res = bs.FindFirstSet(0);
	EXPECT_TRUE(res);
	EXPECT_EQ(*res, 0);

	res = bs.FindFirstNonSet(0);
	EXPECT_FALSE(res);
}
