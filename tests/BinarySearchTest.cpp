#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/IfErr.h"
#include "../alt-std/src/Algorithms/RadixSort.h"
#include "../alt-std/src/Collections/Vector.h"
#include "../alt-std/src/Collections/StaticVector.h"
#include "../alt-std/src/Platform/Platform.h"

namespace {

	template <typename T>
	static Alt::Optional<Alt::Vector<T>> AllocateVector(const usize min, const usize max) noexcept {
		iferr_scope;

		const usize size = GetNum<usize>(min, max);
		Alt::Vector<T> result = Alt::Vector<T>::AllocateCapacity(size) iferr_return Alt::Nullopt;

		auto iterator = Alt::IteratorForwardFromFunc([]() { return GetNum<T>(); })
			.Take(size);

		for (T val : iterator) {

			result.Push(val);
		}

		Alt::Vector<T> buffer = Alt::Vector<T>::Allocate(size) iferr_return Alt::Nullopt;
		Alt::RadixSort::Sort<T>(result.Data(), buffer.Data(), result.GetSize()) iferr_return Alt::Nullopt;

		return result;
	}
}

TEST(BinarySearch, Search) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize ITERATIONS = 1'000;

	const usize min = 1'000;
	const usize max = 1'000'000;
	Alt::Vector<usize> vec;

	vec = AllocateVector<usize>(min, max) iferr_return;

	for (usize i = 0; i < ITERATIONS; ++i) {

		const usize idx = GetNum<usize>(0, Alt::Num::SubWrap(vec.GetSize(), 1));
		const usize* pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return vec[idx] > y;
		});

		ASSERT_TRUE(pResult != vec.DataEnd());
		EXPECT_EQ(*pResult, vec[idx])
			<< "idx: " << idx << " | result idx: " << pResult - vec.Data();

		if (idx + 1 != vec.GetSize()) {

			EXPECT_LE(*pResult, vec[idx + 1]);
		}

		if (idx != 0) {

			EXPECT_GE(*pResult, vec[idx - 1]);
		}
	}

	{
		const usize* pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return Alt::Num::Desc<usize>::Max() > y;
		});

		ASSERT_TRUE(pResult == vec.DataEnd())
			<< "result idx: " << pResult - vec.Data() << " | total size: " << vec.GetSize();
	}
}

TEST(BinarySearch, SearchEdgeCases) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const Alt::StaticVector<usize, 0> vec;

		const usize* pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize) {
			return true;
		});

		EXPECT_EQ(pResult, vec.DataEnd());
	}

	{
		const Alt::StaticVector<usize, 1> vec{ 4 };

		const usize* pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return 5 > y;
		});

		EXPECT_EQ(pResult, vec.DataEnd());

		pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return 2 > y;
		});

		EXPECT_EQ(pResult, vec.Data());
	}

	{
		const Alt::StaticVector<usize, 3> vec{ 3, 4, 4 };

		const usize* pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return 5 > y;
		});

		EXPECT_EQ(pResult, vec.DataEnd());

		pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return 2 > y;
		});

		EXPECT_EQ(pResult, vec.Data());

		pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return 4 > y;
		});

		EXPECT_EQ(pResult, vec.Data() + 1);

		const usize* pResultLowerBound = Alt::Algorithms::BinarySearchLowerBound(vec.Data(), vec.DataEnd(), (usize)4);
		EXPECT_EQ(pResultLowerBound, vec.Data() + 1);

		pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return 4 >= y;
		});

		EXPECT_EQ(pResult, vec.DataEnd());

		const usize* pResultUpperBound = Alt::Algorithms::BinarySearchUpperBound(vec.Data(), vec.DataEnd(), (usize)4);
		EXPECT_EQ(pResultUpperBound, vec.DataEnd());
	}

	{
		const Alt::StaticVector<usize, 2> vec{ 4, 4 };

		const usize* pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return 4 >= y;
		});

		EXPECT_EQ(pResult, vec.DataEnd());

		const usize* pResultUpperBound = Alt::Algorithms::BinarySearchUpperBound(vec.Data(), vec.DataEnd(), (usize)4);
		EXPECT_EQ(pResultUpperBound, vec.DataEnd());
	}

	{
		const Alt::StaticVector<usize, 3> vec{ 4, 4, 5 };

		const usize* pResult = Alt::Algorithms::BinarySearch(vec.Data(), vec.DataEnd(), [&](const usize y) {
			return 4 >= y;
		});

		EXPECT_EQ(pResult, vec.DataEnd() - 1);

		const usize* pResultUpperBound = Alt::Algorithms::BinarySearchUpperBound(vec.Data(), vec.DataEnd(), (usize)4);
		EXPECT_EQ(pResultUpperBound, vec.DataEnd() - 1);
	}
}
