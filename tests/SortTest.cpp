#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/IfErr.h"
#include "../alt-std/src/Algorithms/Algorithms.h"
#include "../alt-std/src/Collections/Vector.h"
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

		return result;
	}

	static Alt::Optional<Alt::Vector<Alt::String>> AllocateVectorStr(
		const usize min, const usize max, const usize strMin, const usize strMax
	) noexcept {
		iferr_scope;

		const usize size = GetNum<usize>(min, max);
		Alt::Vector<Alt::String> result = Alt::Vector<Alt::String>::AllocateCapacity(size) iferr_return Alt::Nullopt;

		for (usize i = 0; i < size; ++i) {

			const bool isAlphanum = false;
			Alt::String str = AllocateStr(strMin, strMax, isAlphanum);
			result.Push(std::move(str));
		}

		return result;
	}

	template <typename T>
	static bool CheckIsSorted(const Alt::Vector<T>& v) noexcept {

		return v.GetIterator()
			.Zip(v.GetIterator().Skip(1))
			.Filter([](Alt::Pair<const T&, const T&> val) {
				return val.f() > val.s();
			})
			.Count() == 0;
	}
}

TEST(Sort, VerifyScalarFuzzy) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize TEST_CNT = 100;
	constexpr static usize NUM_MAX_ELEMENT = 1'000'000;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, NUM_MAX_ELEMENT);
		Alt::Vector<usize> data = AllocateVector<usize>(0, size) iferr_return;

		Alt::Algorithms::Sort(data.Data(), data.DataEnd());

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(Sort, VerifyStringFuzzy) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize TEST_CNT = 100;
	constexpr static usize NUM_MAX_ELEMENT = 1'000;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, NUM_MAX_ELEMENT);
		const usize sizeStr = GetNum<usize>(0, NUM_MAX_ELEMENT);

		Alt::Vector<Alt::String> data = AllocateVectorStr(0, size, 0, sizeStr) iferr_return;

		Alt::Algorithms::Sort(data.Data(), data.DataEnd());

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(Sort, BenchScalars) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize NUM_ITERS = ALT_DEV_BOOL ? 10 : 1'000;
	constexpr static usize NUM_MAX_ELEMENT = 1'000'000;

	const usize size = GetNum<usize>(0, NUM_MAX_ELEMENT);
	Alt::Vector<usize> data = AllocateVector<usize>(0, size) iferr_return;

	ALT_TIME_BENCH("Alt::Algorithms::Sort", NUM_ITERS, {

		Alt::Vector<usize> sort = AllocateVector<usize>(0, size) iferr_panic_silent;
		Alt::Algorithms::Sort(sort.Data(), sort.DataEnd());
	});

	ALT_TIME_BENCH("std::sort", NUM_ITERS, {

		Alt::Vector<usize> sort = AllocateVector<usize>(0, size) iferr_panic_silent;
		std::sort(data.Data(), data.DataEnd());
	});
}

TEST(Sort, BenchString) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize NUM_ITERS = ALT_DEV_BOOL ? 10 : 1'000;
	constexpr static usize NUM_MAX_ELEMENT = 1'000;

	const usize size = GetNum<usize>(0, NUM_MAX_ELEMENT);
	const usize sizeStr = GetNum<usize>(0, NUM_MAX_ELEMENT);

	ALT_TIME_BENCH("Alt::Algorithms::Sort", NUM_ITERS, {

		Alt::Vector<Alt::String> sort = AllocateVectorStr(0, size, 0, sizeStr) iferr_panic_silent;
		Alt::Algorithms::Sort(sort.Data(), sort.DataEnd());
	});

	ALT_TIME_BENCH("std::sort", NUM_ITERS, {

		Alt::Vector<Alt::String> sort = AllocateVectorStr(0, size, 0, sizeStr) iferr_panic_silent;
		std::sort(sort.Data(), sort.DataEnd());
	});
}
