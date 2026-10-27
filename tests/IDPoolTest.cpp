#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/IfErr.h"
#include "../alt-std/src/Random/Xoshiro256.h"

#include "../alt-std/src/Algorithms/RadixSort.h"
#include "../alt-std/src/Collections/IDPool.h"
#include "../alt-std/src/Collections/Vector.h"
#include "../alt-std/src/Platform/Platform.h"

namespace {

	template <typename T>
	static void Shuffle(Alt::Vector<T>& v) noexcept {

		Alt::Xoshiro256StarStar rnd{ (usize)time(nullptr) };

		for (T& val : v.GetIterator()) {

			const usize i = (usize)rnd.GenerateU64(0, Alt::Num::SubWrap(v.GetSize(), 1));
			Alt::Swap(val, v[i]);
		}
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

	template <typename T>
	static bool CheckUnique(const Alt::Vector<T>& v) noexcept {

		ALT_ASSERT(CheckIsSorted(v));

		return v.GetIterator()
			.Zip(v.GetIterator().Skip(1))
			.Filter([](Alt::Pair<const T&, const T&> val) {
				return val.f() == val.s();
			})
			.Count() == 0;
	}
}

TEST(IDPool, CreateRemoveClearRepeat) {
	iferr_scope_on_err{
		FAIL();
	};

	using Type = usize;
	constexpr static usize SIZE = 10'000;

	Alt::Vector<Type> vector{};
	Alt::Vector<Type> buffer = Alt::Vector<Type>::Allocate(SIZE) iferr_return;

	Alt::IDPool<Type> idPool{};

	for (usize i = 0; i < 2; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const usize id = idPool.CreateID();
			vector.Push(id);
		}

		EXPECT_TRUE(CheckIsSorted(vector));
		EXPECT_TRUE(CheckUnique(vector));

		Shuffle(vector);

		for (usize i = 0; i < SIZE; ++i) {

			idPool.RemoveID(vector[i]);
		}

		vector.Clear();

		for (usize i = 0; i < SIZE / 2; ++i) {

			const usize id = idPool.CreateID();
			vector.Push(id);
		}

		Alt::RadixSort::Sort(vector.Data(), buffer.Data(), vector.GetSize()) iferr_return;
		EXPECT_TRUE(CheckUnique(vector));

		vector.Clear();
		idPool.Clear();
	}
}
