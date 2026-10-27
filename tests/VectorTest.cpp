#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/IfErr.h"

#include "../alt-std/src/Utils/Num.h"
#include "../alt-std/src/Collections/Vector.h"

namespace {

	template <typename T>
	inline T RemoveSwapLast(std::vector<T>& vec, const usize i) {

		if (i + 1 != vec.size()) {

			std::swap(vec[i], vec.back());
		}

		T result = std::move(vec.back());
		vec.pop_back();

		return result;
	}
}

TEST(Vector, TypeTraits) {

	bool v = true;

	v = Alt::TypeTraits::is_clonable_v<Alt::Vector<Rule5AllTrivial>>;
	EXPECT_TRUE(v);
}

TEST(Vector, Construct) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Vector<usize> vec0 = Alt::Vector<usize>::Allocate(4) iferr_return;
	EXPECT_EQ(vec0.GetSize(), 4);
	EXPECT_EQ(vec0[0], 0);

	Alt::Vector<usize, 4> vec1 = Alt::Vector<usize, 4>::Allocate() iferr_return;
	EXPECT_EQ(vec1.GetSize(), 0);
	EXPECT_EQ(vec1.GetCapacity(), 4);

	Alt::Vector<usize> vec2{};
	EXPECT_EQ(vec2.GetSize(), 0);
	EXPECT_TRUE(vec2.IsEmpty());

	Alt::Vector<usize> vec3 = Alt::Vector<usize>::Allocate(3, 1) iferr_return;
	EXPECT_EQ(vec3.GetSize(), 3);
	EXPECT_EQ(vec3[0], 1);

	Alt::Vector<usize> vec4 = Alt::Vector<usize>::Allocate(0, 1) iferr_return;
	EXPECT_EQ(vec4.GetSize(), 0);
	EXPECT_TRUE(vec4.IsEmpty());

	Alt::Vector<Rule5AllTrivial_DeletedDefaultConstructor> vec5 =
		Alt::Vector<Rule5AllTrivial_DeletedDefaultConstructor>::Allocate(
			2, Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct }
		) iferr_return;

	EXPECT_EQ(vec5.GetSize(), 2);

	Alt::Vector<Rule5AllTrivial_DeletedDefaultConstructor> vec6 =
		Alt::Vector<Rule5AllTrivial_DeletedDefaultConstructor>::Allocate() iferr_return;

	EXPECT_TRUE(vec6.IsEmpty());

	Alt::Vector<Rule5AllTrivial_DeletedDefaultConstructor> vec7{};
	EXPECT_TRUE(vec7.IsEmpty());

	Alt::Vector<Rule5AllTrivial_DeletedDefaultConstructor, 0> vec8 =
		Alt::Vector<Rule5AllTrivial_DeletedDefaultConstructor, 0>::AllocateCapacity(44) iferr_return;

	EXPECT_TRUE(vec8.IsEmpty());
	EXPECT_EQ(vec8.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)44));

	Alt::Vector<usize> vec9 = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3) iferr_return;
	EXPECT_EQ(vec9.GetSize(), 4);
	EXPECT_EQ(vec9[0], 0);
	EXPECT_EQ(vec9[1], 1);
	EXPECT_EQ(vec9[2], 2);
}

TEST(Vector, Move) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE =
		Alt::Vector<Alt::String>::GetMinElementCapacityOnAllocate();

	Alt::Vector<Alt::String, MIN_ELEMENT_CAPACITY_ON_ALLOCATE * 2> vec0;
	vec0.Push("Vector"_salloc) iferr_return;

	Alt::Vector<Alt::String> vec1 = std::move(vec0);
	EXPECT_TRUE(vec0.IsEmpty());
	EXPECT_EQ(vec1.GetSize(), 1);
	EXPECT_EQ(vec1[0], "Vector");

	Alt::Vector<Alt::String, MIN_ELEMENT_CAPACITY_ON_ALLOCATE * 4> vec2;
	vec2 = std::move(vec1);
	EXPECT_TRUE(vec1.IsEmpty());
	EXPECT_EQ(vec2.GetSize(), 1);
	EXPECT_EQ(vec2[0], "Vector");
}

TEST(Vector, Clone) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Vector<Alt::String> vec0;
	vec0.Push("Vector0"_salloc) iferr_return;
	vec0.Push("Vector1"_salloc) iferr_return;

	Alt::Vector<Alt::String> vec1 = vec0.Clone() iferr_return;
	EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

	for (usize i = 0; i < vec1.GetSize(); ++i) {

		EXPECT_EQ(vec1[i], vec0[i]);
	}

	vec1.Push("Vector2"_salloc);
	vec1.Remove(0);

	EXPECT_EQ(vec1[0], "Vector1");
	EXPECT_EQ(vec1[1], "Vector2");

	EXPECT_EQ(vec1.GetSize(), 2);

	for (usize i = 0; i < vec1.GetSize(); ++i) {

		EXPECT_NE(vec1[i], vec0[i]);
	}
}

TEST(Vector, CloneFromAndMoveFrom) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Vector<Alt::String> vec0;

		//Alt::Vector<Alt::String> vec1 = vec0; // compilation error, copy can fail
		//vec1 = vec0; // compilation error, copy can fail
	}

	{
		Alt::Vector<usize> vec0;

		//Alt::Vector<usize> vec1 = vec0; // compilation error, copy can fail
		//vec1 = vec0; // compilation error, copy can fail
	}

	Alt::Vector<Alt::String> vec0;
	vec0.Push("Vector0"_salloc) iferr_return;
	vec0.Push("Vector1"_salloc) iferr_return;

	{
		Alt::Vector<Alt::String> vec1;
		vec1.CloneFrom(vec0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_EQ(vec1[i], vec0[i]);
		}

		vec1.Push("Vector2"_salloc);
		vec1.Remove(0);

		EXPECT_EQ(vec1[0], "Vector1");
		EXPECT_EQ(vec1[1], "Vector2");

		EXPECT_EQ(vec1.GetSize(), 2);

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_NE(vec1[i], vec0[i]);
		}

		Alt::Vector<Alt::String> vec2 = vec1.Clone() iferr_return;

		vec1.Push("Vector3"_salloc);
		EXPECT_EQ(vec1.GetSize(), 3);

		vec1.CloneFrom(vec2) iferr_return;
		EXPECT_EQ(vec1.GetSize(), 2);

		vec1.CloneFrom(Alt::Vector<Alt::String>{}) iferr_return;
		EXPECT_TRUE(vec1.IsEmpty());
	}

	{
		Alt::Vector<Alt::String> vec1;
		vec1.MoveFrom(vec0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_EQ(vec1[i], vec0[i]);
		}

		vec1.Push("Vector2"_salloc);
		vec1.Remove(0);

		EXPECT_EQ(vec1[0], "Vector1");
		EXPECT_EQ(vec1[1], "Vector2");

		EXPECT_EQ(vec1.GetSize(), 2);

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_NE(vec1[i], vec0[i]);
		}

		Alt::Vector<Alt::String> vec2 = vec1.Clone() iferr_return;

		vec1.Push("Vector3"_salloc);
		EXPECT_EQ(vec1.GetSize(), 3);

		vec1.MoveFrom(vec2) iferr_return;
		EXPECT_EQ(vec1.GetSize(), 2);

		vec1.MoveFrom(Alt::Vector<Alt::String>{}) iferr_return;
		EXPECT_TRUE(vec1.IsEmpty());
	}
}

TEST(Vector, AppendCloneAndAppendMove) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Vector<Alt::String> vec0;
	vec0.Push("Vector0"_salloc) iferr_return;
	vec0.Push("Vector1"_salloc) iferr_return;

	{
		Alt::Vector<Alt::String> vec1;
		vec1.AppendClone(vec0) iferr_return;

		EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_EQ(vec1[i], vec0[i]);
		}

		vec1.Push("Vector2"_salloc);
		vec1.Remove(0);

		EXPECT_EQ(vec1[0], "Vector1");
		EXPECT_EQ(vec1[1], "Vector2");

		EXPECT_EQ(vec1.GetSize(), 2);

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_NE(vec1[i], vec0[i]);
		}

		const usize sizeOld = vec1.GetSize();
		vec1.AppendClone(vec1.Clone().GetValueOrDefault()) iferr_return;

		for (usize offs = 0; offs < sizeOld; ++offs) {

			EXPECT_EQ(vec1[offs], vec1[sizeOld + offs]);
		}
	}

	{
		Alt::Vector<Alt::String> vec1;
		vec1.AppendMove(vec0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_EQ(vec1[i], vec0[i]);
		}

		vec1.Push("Vector2"_salloc);
		vec1.Remove(0);

		EXPECT_EQ(vec1[0], "Vector1");
		EXPECT_EQ(vec1[1], "Vector2");

		EXPECT_EQ(vec1.GetSize(), 2);

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_NE(vec1[i], vec0[i]);
		}

		const usize sizeOld = vec1.GetSize();
		vec1.AppendMove(vec1.Clone().GetValueOrDefault()) iferr_return;

		for (usize offs = 0; offs < sizeOld; ++offs) {

			EXPECT_EQ(vec1[offs], vec1[sizeOld + offs]);
		}
	}
}

TEST(Vector, Insert) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize NUM_INSERTS = 64;

	Alt::Vector<usize> vec;
	vec.Push(10) iferr_return;
	vec.Push(20) iferr_return;

	vec.Insert(0, 5) iferr_return;
	EXPECT_EQ(vec[0], 5);

	vec.Insert(vec.GetSize(), 25) iferr_return;
	EXPECT_EQ(vec.GetLast(), 25);

	vec.Clear();

	static const usize DEFAULT_VALUE = 40;
	for (usize i = 0; i < NUM_INSERTS; ++i) {

		vec.Insert(vec.GetSize(), DEFAULT_VALUE) iferr_return;
	}

	EXPECT_EQ(vec.GetSize(), NUM_INSERTS);

	for (usize i = 0; i < vec.GetSize(); ++i) {

		EXPECT_EQ(vec[i], DEFAULT_VALUE);
	}
}

TEST(Vector, Rule5Tracking) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Vector<Rule5TrackerInlined> vec = Alt::Vector<Rule5TrackerInlined>::Allocate(5) iferr_return;

		Rule5TrackerInlined v0 = vec.Pop();
		EXPECT_EQ(v0.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v0.core.cntMoveConstruct, 1);
		EXPECT_EQ(v0.core.cntMoveAssignment, 0);

		Rule5TrackerInlined v1 = vec.Remove(0);
		EXPECT_EQ(v1.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v1.core.cntMoveConstruct, 2);
		EXPECT_EQ(v1.core.cntMoveAssignment, 0);

		Rule5TrackerInlined v2 = vec.Remove(0);
		EXPECT_EQ(v2.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v2.core.cntMoveConstruct, 2);
		EXPECT_EQ(v2.core.cntMoveAssignment, 1);

		Rule5TrackerInlined v3 = vec.Remove(0);
		EXPECT_EQ(v3.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v3.core.cntMoveConstruct, 2);
		EXPECT_EQ(v3.core.cntMoveAssignment, 2);

		Rule5TrackerInlined v4 = vec.Remove(0);
		EXPECT_EQ(v4.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v4.core.cntMoveConstruct, 1);
		EXPECT_EQ(v4.core.cntMoveAssignment, 3);

		EXPECT_TRUE(vec.IsEmpty());
	}

	{
		Alt::Vector<Rule5TrackerInlinedTriviallyReallocatable> vec =
			Alt::Vector<Rule5TrackerInlinedTriviallyReallocatable>::Allocate(5) iferr_return;

		Rule5TrackerInlinedTriviallyReallocatable v0 = vec.Pop();
		EXPECT_EQ(v0.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v0.core.cntMoveConstruct, 1);
		EXPECT_EQ(v0.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v1 = vec.Remove(0);
		EXPECT_EQ(v1.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v1.core.cntMoveConstruct, 1);
		EXPECT_EQ(v1.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v2 = vec.Remove(0);
		EXPECT_EQ(v2.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v2.core.cntMoveConstruct, 1);
		EXPECT_EQ(v2.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v3 = vec.Remove(0);
		EXPECT_EQ(v3.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v3.core.cntMoveConstruct, 1);
		EXPECT_EQ(v3.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v4 = vec.Remove(0);
		EXPECT_EQ(v4.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v4.core.cntMoveConstruct, 1);
		EXPECT_EQ(v4.core.cntMoveAssignment, 0);

		EXPECT_TRUE(vec.IsEmpty());
	}

	{
		Rule5Tracker::Core tracker{};
		Rule5Tracker track{ tracker };

		Alt::Vector<Rule5Tracker> vec0 = Alt::Vector<Rule5Tracker>::Allocate(5, track) iferr_return;
		Alt::Vector<Rule5Tracker> vec1 = Alt::Vector<Rule5Tracker>::Allocate(5, track) iferr_return;

		vec0 = std::move(vec1);
		EXPECT_EQ(tracker.cntDestructor, 5);

		Alt::Vector<Rule5Tracker> vec2 = std::move(vec0);
		EXPECT_EQ(tracker.cntDestructor, 5);

		vec2 = {};
		EXPECT_EQ(tracker.cntDestructor, 10);
	}
}

TEST(Vector, ResizeReserve) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE = 16;

	Alt::Vector<Alt::String, MIN_ELEMENT_CAPACITY_ON_ALLOCATE> vec;
	EXPECT_TRUE(vec.IsEmpty());
	EXPECT_EQ(vec.GetCapacity(), 0);

	vec.Resize(10) iferr_return;
	EXPECT_EQ(vec.GetSize(), 10);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10));

	vec.Resize(0) iferr_return;
	EXPECT_TRUE(vec.IsEmpty());
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10));

	vec.Resize(100) iferr_return;
	EXPECT_EQ(vec.GetSize(), 100);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)100));

	vec.Resize(10'000) iferr_return;
	EXPECT_EQ(vec.GetSize(), 10'000);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10'000));

	vec.Reserve(2);
	EXPECT_EQ(vec.GetSize(), 10'000);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10'000));

	vec.Resize(1) iferr_return;
	EXPECT_EQ(vec.GetSize(), 1);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10'000));

	vec.Reserve(1'000'000);
	EXPECT_EQ(vec.GetSize(), 1);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	vec.Resize(1'000'000) iferr_return;
	EXPECT_EQ(vec.GetSize(), 1'000'000);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	vec.Resize(0) iferr_return;
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	vec.ShrinkToFit();
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	vec.Reserve(1'000'000);
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	vec.ShrinkToFit();
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	vec.Resize(1'000) iferr_return;
	EXPECT_EQ(vec.GetSize(), 1'000);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000));

	vec.Reserve(1'000'000);
	EXPECT_EQ(vec.GetSize(), 1'000);
	EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	vec.ShrinkToFit();
	EXPECT_EQ(vec.GetSize(), 1'000);
	EXPECT_EQ(vec.GetCapacity(), (usize)1'000);

	vec.Clear();
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), (usize)1'000);

	vec.ShrinkToFit();
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	vec.ReserveExact(3);
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	vec.ReserveExact(4);
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	vec.ReserveExact(MIN_ELEMENT_CAPACITY_ON_ALLOCATE + 1);
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE + 1);

	vec.ReserveExact(MIN_ELEMENT_CAPACITY_ON_ALLOCATE + 2);
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE + 2);

	vec.ShrinkToFit();
	EXPECT_EQ(vec.GetSize(), 0);
	EXPECT_EQ(vec.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);
}

TEST(Vector, PushPopClearRepeat) {

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Vector<Alt::String> vec;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		if (i % 2 == 0) {

			for (usize i = 0; i < SIZE; ++i) {

				Alt::String val = AllocateStr(1, 100);
				vec.Push(std::move(val));
			}
		}
		else {

			static_assert(SIZE % 2 == 0, "");
			for (usize i = 0; i < SIZE / 2; ++i) {

				vec.Push(AllocateStr(1, 100), AllocateStr(1, 100));
			}
		}

		EXPECT_EQ(vec.GetSize(), SIZE);
		EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));

		for (usize i = 0; i < SIZE / 2; ++i) {

			vec.Pop();
		}

		EXPECT_TRUE(vec.IsPopulated());
		EXPECT_EQ(vec.GetSize(), SIZE - SIZE / 2);

		vec.Clear();
		EXPECT_TRUE(vec.IsEmpty());
		EXPECT_EQ(vec.GetSize(), 0);
		EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));
	}
}

TEST(Vector, PushRemoveSwapRepeat) {

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	std::vector<Alt::String> stdvec;
	Alt::Vector<Alt::String> vec;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			vec.Push(val.Clone().GetValueOrDefault());
			stdvec.push_back(val.Clone().GetValueOrDefault());
		}

		for (usize i = 0; i < SIZE; ++i) {

			const usize idToRemove = GetNum<usize>(0, vec.GetSize() - 1);

			EXPECT_EQ(stdvec[idToRemove], vec[idToRemove]);

			Alt::String res0 = RemoveSwapLast(stdvec, idToRemove);
			Alt::String res1 = vec.RemoveSwapLast(idToRemove);

			EXPECT_EQ(res0, res1);
		}

		EXPECT_EQ(vec.GetSize(), stdvec.size());
		EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));
	}
}

TEST(Vector, PushRemoveRepeat) {

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	std::vector<Alt::String> stdvec;
	Alt::Vector<Alt::String> vec;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			vec.Push(val.Clone().GetValueOrDefault());
			stdvec.push_back(val.Clone().GetValueOrDefault());
		}

		for (usize i = 0; i < SIZE; ++i) {

			const usize idToRemove = GetNum<usize>(0, vec.GetSize() - 1);

			EXPECT_EQ(stdvec[idToRemove], vec[idToRemove]);

			Alt::String res1 = vec.Remove(idToRemove);
			EXPECT_EQ(stdvec[idToRemove], res1);

			stdvec.erase(stdvec.begin() + idToRemove);
		}

		EXPECT_EQ(vec.GetSize(), stdvec.size());
		EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));
	}
}

TEST(Vector, PushIteratorClearRepeat) {

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	std::vector<Alt::String> stdvec;
	Alt::Vector<Alt::String> vec;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			vec.Push(val.Clone().GetValueOrDefault());
			stdvec.push_back(val.Clone().GetValueOrDefault());

			EXPECT_EQ(stdvec[i], vec[i]);
		}

		{
			usize i = 0;
			Alt::VectorIterator<Alt::String> it = vec.GetIterator();
			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdvec[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::VectorIterator<Alt::String> it = vec.GetIterator();

			usize i = SIZE / 2;
			it.Advance(SIZE / 2);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdvec[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::VectorIterator<Alt::String> it = vec.GetIterator();

			usize i = SIZE;
			it.Advance(SIZE);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdvec[i], *pVal);
				i += 1;
			}
		}

		EXPECT_EQ(vec.GetSize(), SIZE);
		EXPECT_EQ(vec.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));

		stdvec.clear();
		vec.Clear();
		EXPECT_TRUE(vec.IsEmpty());
	}
}

TEST(Vector, InsertRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Vector<Alt::String> vec0;
	Alt::Vector<Alt::String> vec1;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		vec0.Clear();
		vec1.Clear();

		for (usize i = 0; i < SIZE; ++i) {

			Alt::String val0 = AllocateStr(1, 100);
			vec0.Push(std::move(val0));

			Alt::String val1 = AllocateStr(1, 100);
			vec1.Push(std::move(val1));
		}

		{
			auto iter = vec0.GetIterator()
				.Map([](const Alt::String& str) {
					return str.Clone().GetValueOrDefault();
				});

			vec1.InsertIteratorMove(SIZE - 1, iter) iferr_return;
		}

		EXPECT_EQ(vec1.GetSize(), SIZE * 2);

		if (i == 0) {

			EXPECT_EQ(vec1.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE * 2));
		}

		{
			Alt::VectorIterator<Alt::String> it0 = vec0.GetIterator();

			Alt::VectorIterator<Alt::String> it1 = vec1.GetIterator();
			it1.Advance(SIZE - 1);

			for (Alt::Pair<Alt::String&, Alt::String&> val : it0.Zip(it1)) {

				EXPECT_EQ(val.f(), val.s());
			}
		}

		{
			auto iter = vec0.GetIterator()
				.Map([](const Alt::String& str) {
					return str.Clone().GetValueOrDefault();
				});

			vec1.InsertIteratorMove(SIZE, iter) iferr_return;
		}

		EXPECT_EQ(vec1.GetSize(), SIZE * 3);

		if (i == 0) {

			EXPECT_EQ(vec1.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE * 3));
		}

		{
			Alt::VectorIterator<Alt::String> it0 = vec0.GetIterator();

			Alt::VectorIterator<Alt::String> it1 = vec1.GetIterator();
			it1.Advance(SIZE);

			for (Alt::Pair<Alt::String&, Alt::String&> val : it0.Zip(it1)) {

				EXPECT_EQ(val.f(), val.s());
			}
		}

		{
			auto iter = vec0.GetIterator()
				.Map([](const Alt::String& str) {
					return str.Clone().GetValueOrDefault();
				});

			vec1.InsertIteratorMove(0, iter) iferr_return;
		}

		EXPECT_EQ(vec1.GetSize(), SIZE * 4);
		EXPECT_EQ(vec1.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE * 4));

		{
			Alt::VectorIterator<Alt::String> it0 = vec0.GetIterator();
			Alt::VectorIterator<Alt::String> it1 = vec1.GetIterator();

			for (Alt::Pair<Alt::String&, Alt::String&> val : it0.Zip(it1)) {

				EXPECT_EQ(val.f(), val.s());
			}
		}
	}
}
