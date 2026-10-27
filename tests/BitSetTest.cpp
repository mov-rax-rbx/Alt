#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Collections/BitSet.h"

TEST(BitSet, TypeTraits) {

	bool v = true;

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::BitSet>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_clonable_v<Alt::BitSet>;
	EXPECT_TRUE(v);
}

TEST(BitSet, Allocation) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::BitSet bs = Alt::BitSet::Allocate(4) iferr_return;
		EXPECT_EQ(bs.GetSize(), 4);
		EXPECT_FALSE(bs[0]);
		EXPECT_FALSE(bs[1]);
		EXPECT_FALSE(bs[2]);
		EXPECT_FALSE(bs[3]);
	}

	{
		Alt::BitSet bs = Alt::BitSet::Allocate(4, true) iferr_return;
		EXPECT_EQ(bs.GetSize(), 4);
		EXPECT_TRUE(bs[0]);
		EXPECT_TRUE(bs[1]);
		EXPECT_TRUE(bs[2]);
		EXPECT_TRUE(bs[3]);
	}

	const usize DIGITS = Alt::Num::Desc<usize>::Bits();
	using BitSet = Alt::BitSetT<usize, 0>;

	{
		const usize CAPACITY = 1000;

		BitSet bs = BitSet::AllocateCapacity(CAPACITY) iferr_return;
		EXPECT_EQ(bs.GetCapacity(), Alt::Num::RoundUpToPowerOf2((CAPACITY + DIGITS - 1) / DIGITS) * DIGITS);
	}

	{
		const usize CAPACITY = 1000;

		BitSet bs = BitSet::AllocateCapacityExact(CAPACITY) iferr_return;
		EXPECT_EQ(bs.GetCapacity(), ((CAPACITY + DIGITS - 1) / DIGITS) * DIGITS);
	}
}

TEST(BitSet, ShrinkToFit) {

	iferr_scope_on_err {

		FAIL();
	};

	using BitSet = Alt::BitSetT<usize, 0>;

	{
		const usize CAPACITY = 1000;

		BitSet bs = BitSet::AllocateCapacity(CAPACITY) iferr_return;

		bs.ShrinkToFit();
		EXPECT_EQ(bs.GetCapacity(), 0);
	}

	{
		const usize CAPACITY = 1000;

		BitSet bs = BitSet::AllocateCapacityExact(CAPACITY) iferr_return;

		bs.ShrinkToFit();
		EXPECT_EQ(bs.GetCapacity(), 0);
	}

	{
		const usize CAPACITY = 1000;

		BitSet bs = BitSet::AllocateCapacityExact(CAPACITY) iferr_return;

		bs.Resize(Alt::Num::Desc<usize>::Bits() + 1) iferr_return;

		bs.ShrinkToFit();
		EXPECT_EQ(bs.GetCapacity(), Alt::Num::Desc<usize>::Bits() * 2);
	}
}

TEST(BitSet, Reserve) {

	iferr_scope_on_err {

		FAIL();
	};

	const usize DIGITS = Alt::Num::Desc<usize>::Bits();
	using BitSet = Alt::BitSetT<usize, 0>;

	BitSet bs;

	bs.ReserveExact(1) iferr_return;
	EXPECT_EQ(bs.GetCapacity(), DIGITS);

	bs.ReserveExact(DIGITS) iferr_return;
	EXPECT_EQ(bs.GetCapacity(), DIGITS);

	bs.ReserveExact(DIGITS + 1) iferr_return;
	EXPECT_EQ(bs.GetCapacity(), 2 * DIGITS);
}

TEST(BitSet, Resize) {

	iferr_scope_on_err {

		FAIL();
	};

	const usize DIGITS = Alt::Num::Desc<usize>::Bits();
	using BitSet = Alt::BitSetT<usize, 0>;

	BitSet bs;

	bs.Resize(1) iferr_return;
	EXPECT_EQ(bs.GetSize(), 1);
	EXPECT_EQ(bs[0], false);

	bs.Resize(2, true) iferr_return;
	EXPECT_EQ(bs.GetSize(), 2);
	EXPECT_EQ(bs[0], false);
	EXPECT_EQ(bs[1], true);

	bs.Resize(1) iferr_return;
	EXPECT_EQ(bs.GetSize(), 1);
	EXPECT_EQ(bs[0], false);

	bs.Resize(2, true) iferr_return;
	EXPECT_EQ(bs.GetSize(), 2);
	EXPECT_EQ(bs[0], false);
	EXPECT_EQ(bs[1], true);

	bs.Resize(100, true) iferr_return;
	EXPECT_EQ(bs.GetSize(), 100);
	EXPECT_EQ(bs[0], false);
	EXPECT_EQ(bs[1], true);

	for (usize i = 2; i < bs.GetSize(); ++i) {

		EXPECT_EQ(bs[i], true) << i;
	}

	bs.Resize(DIGITS - 1) iferr_return;
	bs[DIGITS - 2] = false;
	bs.Resize(100, true) iferr_return;

	for (usize i = 2; i < bs.GetSize(); ++i) {

		if (i == DIGITS - 2) {

			EXPECT_EQ(bs[i], false) << i;
		}
		else {

			EXPECT_EQ(bs[i], true) << i;
		}
	}
}

TEST(BitSet, Compare) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::BitSet bs0;
	Alt::BitSet bs1;

	bs0.Resize(3, true) iferr_return;
	bs1.Resize(3, true) iferr_return;

	EXPECT_EQ(bs0, bs1);

	bs0.Resize(100, false) iferr_return;
	bs1.Resize(100, false) iferr_return;

	EXPECT_EQ(bs0, bs1);

	bs0.Resize(101, true) iferr_return;
	bs1.Resize(101, false) iferr_return;

	EXPECT_NE(bs0, bs1);
}

TEST(BitSet, Clone) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::BitSet bs0;
	Alt::BitSet bs1;

	bs0.Resize(3, true) iferr_return;
	bs1.CloneFrom(bs0) iferr_return;

	EXPECT_EQ(bs0, bs1);

	bs0.Resize(100, false) iferr_return;
	bs1.CloneFrom(bs0) iferr_return;

	EXPECT_EQ(bs0, bs1);

	bs0.Resize(101, true) iferr_return;
	bs1.CloneFrom(bs0) iferr_return;

	EXPECT_EQ(bs0, bs1);
}

TEST(BitSet, Iterator) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::BitSet bs;

	auto it = bs.GetIterator();
	EXPECT_FALSE(it.Next());

	bs = Alt::BitSet::Allocate(3, true) iferr_return;

	it = bs.GetIterator();

	{
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, false) iferr_return;

	it = bs.GetIterator();

	{
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_TRUE(*it.Next());
		EXPECT_FALSE(*it.Next());
		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, false) iferr_return;
	bs.Resize(bs.GetSize() + 1, true) iferr_return;
	bs.Resize(bs.GetSize() + 1, false) iferr_return;
	bs.Resize(bs.GetSize() + 1, true) iferr_return;
	bs.Resize(bs.GetSize() + 1, false) iferr_return;

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

		EXPECT_FALSE(it.Next());
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

TEST(BitSet, IteratorRangeActive) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::BitSet bs;

	auto it = bs.GetIteratorActiveRange();
	EXPECT_FALSE(it.Next());

	bs = Alt::BitSet::Allocate(3, true) iferr_return;

	it = bs.GetIteratorActiveRange();
	Alt::RangeUsize range = it.Next() iferr_return;

	{
		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);
		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, false) iferr_return;

	it = bs.GetIteratorActiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(100, true) iferr_return;

	it = bs.GetIteratorActiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 100);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, false) iferr_return;
	bs.Resize(bs.GetSize() + 1, true) iferr_return;
	bs.Resize(bs.GetSize() + 1, false) iferr_return;
	bs.Resize(bs.GetSize() + 100, true) iferr_return;
	bs.Resize(bs.GetSize() + 100, false) iferr_return;

	it = bs.GetIteratorActiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 100);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 101);
		EXPECT_EQ(range.end, 102);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 103);
		EXPECT_EQ(range.end, 203);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() - 120, false) iferr_return;

	it = bs.GetIteratorActiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 100);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 101);
		EXPECT_EQ(range.end, 102);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 103);
		EXPECT_EQ(range.end, 183);

		EXPECT_FALSE(it.Next());
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

		EXPECT_FALSE(it.Next());
	}

	const usize DIGITS = Alt::Num::Desc<usize>::Bits();
	bs.Resize(DIGITS, true) iferr_return;

	it = bs.GetIteratorActiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, DIGITS);

		EXPECT_FALSE(it.Next());
	}
}

TEST(BitSet, IteratorRangeActiveOffset) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::BitSet bs;

	auto it = bs.GetIteratorActiveRange();
	EXPECT_FALSE(it.Next());

	bs = Alt::BitSet::Allocate(3, true) iferr_return;

	it = bs.GetIteratorActiveRange(1, 2);
	Alt::RangeUsize range = it.Next() iferr_return;

	{
		EXPECT_EQ(range.begin, 1);
		EXPECT_EQ(range.end, 3);
		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, false) iferr_return;

	it = bs.GetIteratorActiveRange(2, 1);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 2);
		EXPECT_EQ(range.end, 3);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(100, true) iferr_return;

	it = bs.GetIteratorActiveRange(64, 100 - 64);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 64);
		EXPECT_EQ(range.end, 100);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, false) iferr_return;
	bs.Resize(bs.GetSize() + 1, true) iferr_return;
	bs.Resize(bs.GetSize() + 1, false) iferr_return;
	bs.Resize(bs.GetSize() + 100, true) iferr_return;
	bs.Resize(bs.GetSize() + 100, false) iferr_return;

	it = bs.GetIteratorActiveRange(63);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 63);
		EXPECT_EQ(range.end, 100);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 101);
		EXPECT_EQ(range.end, 102);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 103);
		EXPECT_EQ(range.end, 203);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() - 120, false) iferr_return;

	it = bs.GetIteratorActiveRange(99);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 99);
		EXPECT_EQ(range.end, 100);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 101);
		EXPECT_EQ(range.end, 102);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 103);
		EXPECT_EQ(range.end, 183);

		EXPECT_FALSE(it.Next());
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

	const usize DIGITS = Alt::Num::Desc<usize>::Bits();
	bs.Resize(DIGITS, true) iferr_return;

	it = bs.GetIteratorActiveRange(DIGITS / 2);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, DIGITS / 2);
		EXPECT_EQ(range.end, DIGITS);

		EXPECT_FALSE(it.Next());
	}

}

TEST(BitSet, IteratorRangeInactive) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::BitSet bs;

	auto it = bs.GetIteratorInactiveRange();
	EXPECT_FALSE(it.Next());

	bs = Alt::BitSet::Allocate(3, false) iferr_return;

	it = bs.GetIteratorInactiveRange();
	Alt::RangeUsize range = it.Next() iferr_return;

	{
		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);
		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, true) iferr_return;

	it = bs.GetIteratorInactiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(100, false) iferr_return;

	it = bs.GetIteratorInactiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 100);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, true) iferr_return;
	bs.Resize(bs.GetSize() + 1, false) iferr_return;
	bs.Resize(bs.GetSize() + 1, true) iferr_return;
	bs.Resize(bs.GetSize() + 100, false) iferr_return;
	bs.Resize(bs.GetSize() + 100, true) iferr_return;

	it = bs.GetIteratorInactiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 100);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 101);
		EXPECT_EQ(range.end, 102);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 103);
		EXPECT_EQ(range.end, 203);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() - 120, true) iferr_return;

	it = bs.GetIteratorInactiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, 3);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 4);
		EXPECT_EQ(range.end, 100);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 101);
		EXPECT_EQ(range.end, 102);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 103);
		EXPECT_EQ(range.end, 183);

		EXPECT_FALSE(it.Next());
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

		EXPECT_FALSE(it.Next());
	}

	const usize DIGITS = Alt::Num::Desc<usize>::Bits();
	bs.Resize(DIGITS, false) iferr_return;

	it = bs.GetIteratorInactiveRange();
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 0);
		EXPECT_EQ(range.end, DIGITS);

		EXPECT_FALSE(it.Next());
	}
}

TEST(BitSet, IteratorRangeInactiveOffset) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::BitSet bs;

	auto it = bs.GetIteratorInactiveRange();
	EXPECT_FALSE(it.Next());

	bs = Alt::BitSet::Allocate(3, false) iferr_return;

	it = bs.GetIteratorInactiveRange(1);
	Alt::RangeUsize range = it.Next() iferr_return;

	{
		EXPECT_EQ(range.begin, 1);
		EXPECT_EQ(range.end, 3);
		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, true) iferr_return;

	it = bs.GetIteratorInactiveRange(2, 1);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 2);
		EXPECT_EQ(range.end, 3);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(100, false) iferr_return;

	it = bs.GetIteratorInactiveRange(64, 100 - 64);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 64);
		EXPECT_EQ(range.end, 100);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() + 1, true) iferr_return;
	bs.Resize(bs.GetSize() + 1, false) iferr_return;
	bs.Resize(bs.GetSize() + 1, true) iferr_return;
	bs.Resize(bs.GetSize() + 100, false) iferr_return;
	bs.Resize(bs.GetSize() + 100, true) iferr_return;

	it = bs.GetIteratorInactiveRange(63);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 63);
		EXPECT_EQ(range.end, 100);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 101);
		EXPECT_EQ(range.end, 102);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 103);
		EXPECT_EQ(range.end, 203);

		EXPECT_FALSE(it.Next());
	}

	bs.Resize(bs.GetSize() - 120, true) iferr_return;

	it = bs.GetIteratorInactiveRange(99);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 99);
		EXPECT_EQ(range.end, 100);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 101);
		EXPECT_EQ(range.end, 102);

		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, 103);
		EXPECT_EQ(range.end, 183);

		EXPECT_FALSE(it.Next());
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

	const usize DIGITS = Alt::Num::Desc<usize>::Bits();
	bs.Resize(DIGITS, false) iferr_return;

	it = bs.GetIteratorInactiveRange(DIGITS / 2);
	{
		range = it.Next() iferr_return;

		EXPECT_EQ(range.begin, DIGITS / 2);
		EXPECT_EQ(range.end, DIGITS);

		EXPECT_FALSE(it.Next());
	}
}

TEST(BitSet, FindFirst) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::BitSet bs;
	bs.Resize(100, false) iferr_return;

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
