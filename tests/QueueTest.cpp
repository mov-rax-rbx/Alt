#include "pch.h"

#include <deque>

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/IfErr.h"

#include "../alt-std/src/Utils/Num.h"
#include "../alt-std/src/Collections/Queue.h"
#include "../alt-std/src/Random/Xoshiro256.h"

namespace {

	template <typename T>
	inline T RemoveSwapLast(std::deque<T>& q, const usize i) {

		if (i + 1 != q.size()) {

			std::swap(q[i], q.back());
		}

		T result = std::move(q.back());
		q.pop_back();

		return result;
	}

	template <typename T>
	inline T RemoveSwapFirst(std::deque<T>& q, const usize i) {

		if (i != 0) {

			std::swap(q[i], q.front());
		}

		T result = std::move(q.front());
		q.pop_front();

		return result;
	}
}

TEST(Queue, TypeTraits) {

	bool v = true;

	v = Alt::TypeTraits::is_clonable_v<Alt::Queue<Rule5AllTrivial>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Queue<Rule5AllNonTrivial>>;
	EXPECT_TRUE(v);
}

TEST(Queue, Construct) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Queue<usize> q0 = Alt::Queue<usize>::Allocate(4) iferr_return;
	EXPECT_EQ(q0.GetSize(), 4);
	EXPECT_EQ(q0[0], 0);

	Alt::Queue<usize, 4> q1 = Alt::Queue<usize, 4>::Allocate() iferr_return;
	EXPECT_EQ(q1.GetSize(), 0);
	EXPECT_EQ(q1.GetCapacity(), 4);

	Alt::Queue<usize> q2{};
	EXPECT_EQ(q2.GetSize(), 0);
	EXPECT_TRUE(q2.IsEmpty());

	Alt::Queue<usize> q3 = Alt::Queue<usize>::Allocate(3, 1) iferr_return;
	EXPECT_EQ(q3.GetSize(), 3);
	EXPECT_EQ(q3[0], 1);

	Alt::Queue<usize> q4 = Alt::Queue<usize>::Allocate(0, 1) iferr_return;
	EXPECT_EQ(q4.GetSize(), 0);
	EXPECT_TRUE(q4.IsEmpty());

	Alt::Queue<Rule5AllTrivial_DeletedDefaultConstructor> q5 =
		Alt::Queue<Rule5AllTrivial_DeletedDefaultConstructor>::Allocate(
			2, Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct }
		) iferr_return;

	EXPECT_EQ(q5.GetSize(), 2);

	Alt::Queue<Rule5AllTrivial_DeletedDefaultConstructor> q6 =
		Alt::Queue<Rule5AllTrivial_DeletedDefaultConstructor>::Allocate() iferr_return;

	EXPECT_TRUE(q6.IsEmpty());

	Alt::Queue<Rule5AllTrivial_DeletedDefaultConstructor> q7{};
	EXPECT_TRUE(q7.IsEmpty());

	Alt::Queue<Rule5AllTrivial_DeletedDefaultConstructor, 0> q8 =
		Alt::Queue<Rule5AllTrivial_DeletedDefaultConstructor, 0>::AllocateCapacity(44) iferr_return;

	EXPECT_TRUE(q8.IsEmpty());
	EXPECT_EQ(q8.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)44));

	Alt::Queue<usize> q9 = Alt::Queue<usize>::AllocatePush(0, 1, 2, 3) iferr_return;
	EXPECT_EQ(q9.GetSize(), 4);
	EXPECT_EQ(q9[0], 0);
	EXPECT_EQ(q9[1], 1);
	EXPECT_EQ(q9[2], 2);
}

TEST(Queue, Move) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE =
		Alt::Queue<Alt::String>::GetMinElementCapacityOnAllocate();

	Alt::Queue<Alt::String, MIN_ELEMENT_CAPACITY_ON_ALLOCATE * 2> q0;
	q0.PushBack("Queue"_salloc) iferr_return;

	Alt::Queue<Alt::String> q1 = std::move(q0);
	EXPECT_TRUE(q0.IsEmpty());
	EXPECT_EQ(q1.GetSize(), 1);
	EXPECT_EQ(q1[0], "Queue");

	Alt::Queue<Alt::String, MIN_ELEMENT_CAPACITY_ON_ALLOCATE * 4> q2;
	q2 = std::move(q1);
	EXPECT_TRUE(q1.IsEmpty());
	EXPECT_EQ(q2.GetSize(), 1);
	EXPECT_EQ(q2[0], "Queue");
}

TEST(Queue, Clone) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Queue<Alt::String> q0;
		q0.PushBack("Queue0"_salloc) iferr_return;
		q0.PushBack("Queue1"_salloc) iferr_return;

		Alt::Queue<Alt::String> q1 = q0.Clone() iferr_return;
		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("Queue2"_salloc);
		q1.PopFront();

		EXPECT_EQ(q1[0], "Queue1");
		EXPECT_EQ(q1[1], "Queue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}
	}

	{
		Alt::Queue<Alt::String> q0;
		q0.PushFront("Queue1"_salloc) iferr_return;
		q0.PushFront("Queue0"_salloc) iferr_return;

		Alt::Queue<Alt::String> q1 = q0.Clone() iferr_return;
		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("Queue2"_salloc);
		q1.PopFront();

		EXPECT_EQ(q1[0], "Queue1");
		EXPECT_EQ(q1[1], "Queue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}
	}
}

TEST(Queue, CloneFromAndMoveFrom) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Queue<Alt::String> q0;

		//Alt::Queue<Alt::String> q1 = q0; // compilation error, copy can fail
		//q1 = q0; // compilation error, copy can fail
	}

	{
		Alt::Queue<usize> q0;

		//Alt::Queue<usize> q1 = q0; // compilation error, copy can fail
		//q1 = q0; // compilation error, copy can fail
	}

	Alt::Queue<Alt::String> q0;
	q0.PushBack("Queue0"_salloc) iferr_return;
	q0.PushBack("Queue1"_salloc) iferr_return;

	{
		Alt::Queue<Alt::String> q1;
		q1.CloneFrom(q0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("Queue2"_salloc);
		q1.PopFront();

		EXPECT_EQ(q1[0], "Queue1");
		EXPECT_EQ(q1[1], "Queue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}

		Alt::Queue<Alt::String> q2 = q1.Clone() iferr_return;

		q1.PushFront("Queue3"_salloc);
		q1.PushFront("Queue4"_salloc);
		q1.PushFront("Queue5"_salloc);
		EXPECT_EQ(q1.GetSize(), 5);

		q1.CloneFrom(q2) iferr_return;
		EXPECT_EQ(q1.GetSize(), 2);

		q1.CloneFrom(Alt::Queue<Alt::String>{}) iferr_return;
		EXPECT_TRUE(q1.IsEmpty());
	}

	{
		Alt::Queue<Alt::String> q1;
		q1.MoveFrom(q0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("Queue2"_salloc);
		q1.PopFront();

		EXPECT_EQ(q1[0], "Queue1");
		EXPECT_EQ(q1[1], "Queue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}

		Alt::Queue<Alt::String> q2 = q1.Clone() iferr_return;

		q1.PushFront("Queue3"_salloc);
		q1.PushFront("Queue4"_salloc);
		q1.PushFront("Queue5"_salloc);
		EXPECT_EQ(q1.GetSize(), 5);

		q1.MoveFrom(q2) iferr_return;
		EXPECT_EQ(q1.GetSize(), 2);

		q1.MoveFrom(Alt::Queue<Alt::String>{}) iferr_return;
		EXPECT_TRUE(q1.IsEmpty());
	}
}

TEST(Queue, AppendBackCloneAndAppendMove) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Queue<Alt::String> q0;
	q0.PushBack("Queue0"_salloc) iferr_return;
	q0.PushBack("Queue1"_salloc) iferr_return;

	{
		Alt::Queue<Alt::String> q1;
		q1.AppendBackClone(q0) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("Queue2"_salloc);
		q1.PushFront("Queue3"_salloc);
		q1.PopFront();
		q1.PopFront();

		EXPECT_EQ(q1[0], "Queue1");
		EXPECT_EQ(q1[1], "Queue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}

		const usize sizeOld = q1.GetSize();
		q1.AppendBackClone(q1.Clone().GetValueOrDefault()) iferr_return;

		for (usize offs = 0; offs < sizeOld; ++offs) {

			EXPECT_EQ(q1[offs], q1[sizeOld + offs]);
		}
	}

	{
		Alt::Queue<Alt::String> q1;
		q1.AppendBackMove(q0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("Queue2"_salloc);
		q1.PushFront("Queue3"_salloc);
		q1.PopFront();
		q1.PopFront();

		EXPECT_EQ(q1[0], "Queue1");
		EXPECT_EQ(q1[1], "Queue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}

		const usize sizeOld = q1.GetSize();
		q1.AppendBackMove(q1.Clone().GetValueOrDefault()) iferr_return;

		for (usize offs = 0; offs < sizeOld; ++offs) {

			EXPECT_EQ(q1[offs], q1[sizeOld + offs]);
		}
	}
}

TEST(Queue, AppendFrontCloneAndAppendMove) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Queue<Alt::String> q0;
	q0.PushBack("Queue0"_salloc) iferr_return;
	q0.PushBack("Queue1"_salloc) iferr_return;

	{
		Alt::Queue<Alt::String> q1;
		q1.AppendFrontClone(q0) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("Queue2"_salloc);
		q1.PopFront();

		EXPECT_EQ(q1[0], "Queue1");
		EXPECT_EQ(q1[1], "Queue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}

		const usize sizeOld = q1.GetSize();
		q1.AppendFrontClone(q1.Clone().GetValueOrDefault()) iferr_return;

		for (usize offs = 0; offs < sizeOld; ++offs) {

			EXPECT_EQ(q1[offs], q1[sizeOld + offs]);
		}
	}

	{
		Alt::Queue<Alt::String> q1;
		q1.AppendFrontMove(q0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("Queue2"_salloc);
		q1.PopFront();

		EXPECT_EQ(q1[0], "Queue1");
		EXPECT_EQ(q1[1], "Queue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}

		const usize sizeOld = q1.GetSize();
		q1.AppendFrontMove(q1.Clone().GetValueOrDefault()) iferr_return;

		for (usize offs = 0; offs < sizeOld; ++offs) {

			EXPECT_EQ(q1[offs], q1[sizeOld + offs]);
		}
	}
}

TEST(Queue, Rule5Tracking) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Queue<Rule5TrackerInlined> q = Alt::Queue<Rule5TrackerInlined>::Allocate(5) iferr_return;

		Rule5TrackerInlined v0 = q.PopBack();
		EXPECT_EQ(v0.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v0.core.cntMoveConstruct, 1);
		EXPECT_EQ(v0.core.cntMoveAssignment, 0);

		Rule5TrackerInlined v1 = q.PopFront();
		EXPECT_EQ(v1.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v1.core.cntMoveConstruct, 1);
		EXPECT_EQ(v1.core.cntMoveAssignment, 0);

		Rule5TrackerInlined v2 = q.PopFront();
		EXPECT_EQ(v2.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v2.core.cntMoveConstruct, 1);
		EXPECT_EQ(v2.core.cntMoveAssignment, 0);

		Rule5TrackerInlined v3 = q.PopFront();
		EXPECT_EQ(v3.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v3.core.cntMoveConstruct, 1);
		EXPECT_EQ(v3.core.cntMoveAssignment, 0);

		Rule5TrackerInlined v4 = q.PopFront();
		EXPECT_EQ(v4.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v4.core.cntMoveConstruct, 1);
		EXPECT_EQ(v4.core.cntMoveAssignment, 0);

		EXPECT_TRUE(q.IsEmpty());
	}

	{
		Alt::Queue<Rule5TrackerInlinedTriviallyReallocatable> q =
			Alt::Queue<Rule5TrackerInlinedTriviallyReallocatable>::Allocate(5) iferr_return;

		Rule5TrackerInlinedTriviallyReallocatable v0 = q.PopBack();
		EXPECT_EQ(v0.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v0.core.cntMoveConstruct, 1);
		EXPECT_EQ(v0.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v1 = q.PopFront();
		EXPECT_EQ(v1.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v1.core.cntMoveConstruct, 1);
		EXPECT_EQ(v1.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v2 = q.PopFront();
		EXPECT_EQ(v2.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v2.core.cntMoveConstruct, 1);
		EXPECT_EQ(v2.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v3 = q.PopFront();
		EXPECT_EQ(v3.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v3.core.cntMoveConstruct, 1);
		EXPECT_EQ(v3.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v4 = q.PopFront();
		EXPECT_EQ(v4.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v4.core.cntMoveConstruct, 1);
		EXPECT_EQ(v4.core.cntMoveAssignment, 0);

		EXPECT_TRUE(q.IsEmpty());
	}

	{
		Rule5Tracker::Core tracker{};
		Rule5Tracker track{ tracker };

		Alt::Queue<Rule5Tracker> q0 = Alt::Queue<Rule5Tracker>::Allocate(5, track) iferr_return;
		Alt::Queue<Rule5Tracker> q1 = Alt::Queue<Rule5Tracker>::Allocate(5, track) iferr_return;

		q0 = std::move(q1);
		EXPECT_EQ(tracker.cntDestructor, 5);

		Alt::Queue<Rule5Tracker> q2 = std::move(q0);
		EXPECT_EQ(tracker.cntDestructor, 5);

		q2 = {};
		EXPECT_EQ(tracker.cntDestructor, 10);
	}
}

TEST(Queue, ResizeReserve) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize MIN_ELEMENT_CAPACITY_ON_ALLOCATE = 16;

	Alt::Queue<Alt::String, MIN_ELEMENT_CAPACITY_ON_ALLOCATE> q;
	EXPECT_TRUE(q.IsEmpty());
	EXPECT_EQ(q.GetCapacity(), 0);

	q.ResizeBack(10) iferr_return;
	EXPECT_EQ(q.GetSize(), 10);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10));

	q.ResizeBack(0) iferr_return;
	EXPECT_TRUE(q.IsEmpty());
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10));

	q.ResizeFront(100) iferr_return;
	EXPECT_EQ(q.GetSize(), 100);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)100));

	q.ResizeBack(10'000) iferr_return;
	EXPECT_EQ(q.GetSize(), 10'000);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10'000));

	q.Reserve(2);
	EXPECT_EQ(q.GetSize(), 10'000);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10'000));

	q.ResizeFront(1) iferr_return;
	EXPECT_EQ(q.GetSize(), 1);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)10'000));

	q.Reserve(1'000'000);
	EXPECT_EQ(q.GetSize(), 1);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	q.ResizeBack(1'000'000) iferr_return;
	EXPECT_EQ(q.GetSize(), 1'000'000);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	q.ResizeBack(0) iferr_return;
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	q.ShrinkToFit();
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	q.Reserve(1'000'000);
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	q.ShrinkToFit();
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	q.ResizeBack(1'000) iferr_return;
	EXPECT_EQ(q.GetSize(), 1'000);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000));

	q.Reserve(1'000'000);
	EXPECT_EQ(q.GetSize(), 1'000);
	EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2((usize)1'000'000));

	q.ShrinkToFit();
	EXPECT_EQ(q.GetSize(), 1'000);
	EXPECT_EQ(q.GetCapacity(), (usize)1'000);

	q.Clear();
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), (usize)1'000);

	q.ShrinkToFit();
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	q.ReserveExact(3);
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	q.ReserveExact(4);
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);

	q.ReserveExact(MIN_ELEMENT_CAPACITY_ON_ALLOCATE + 1);
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE + 1);

	q.ReserveExact(MIN_ELEMENT_CAPACITY_ON_ALLOCATE + 2);
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE + 2);

	q.ShrinkToFit();
	EXPECT_EQ(q.GetSize(), 0);
	EXPECT_EQ(q.GetCapacity(), MIN_ELEMENT_CAPACITY_ON_ALLOCATE);
}

TEST(Queue, PushPopClearRepeat) {

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	Alt::Queue<Alt::String> q;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		if (i % 2 == 0) {

			for (usize i = 0; i < SIZE; ++i) {

				Alt::String val = AllocateStr(1, 100);

				if (rnd.GenerateU64(0, 1) == 0) {

					q.PushBack(std::move(val));
				}
				else {

					q.PushFront(std::move(val));
				}
			}
		}
		else {

			static_assert(SIZE % 2 == 0, "");
			for (usize i = 0; i < SIZE / 2; ++i) {

				if (rnd.GenerateU64(0, 1) == 0) {

					q.PushBack(AllocateStr(1, 100), AllocateStr(1, 100));
				}
				else {

					q.PushFront(AllocateStr(1, 100), AllocateStr(1, 100));
				}
			}
		}

		EXPECT_EQ(q.GetSize(), SIZE);
		EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));

		for (usize i = 0; i < SIZE / 2; ++i) {

			if (rnd.GenerateU64(0, 1) == 0) {

				q.PopBack();
			}
			else {

				q.PopFront();
			}
		}

		EXPECT_TRUE(q.IsPopulated());
		EXPECT_EQ(q.GetSize(), SIZE - SIZE / 2);

		q.Clear();
		EXPECT_TRUE(q.IsEmpty());
		EXPECT_EQ(q.GetSize(), 0);
		EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));
	}
}

TEST(Queue, PushRemoveSwapRepeat) {

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	std::deque<Alt::String> stdq;
	Alt::Queue<Alt::String> q;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			if (rnd.GenerateU64(0, 1) == 0) {

				q.PushBack(val.Clone().GetValueOrDefault());
				stdq.push_back(val.Clone().GetValueOrDefault());
			}
			else {

				q.PushFront(val.Clone().GetValueOrDefault());
				stdq.push_front(val.Clone().GetValueOrDefault());
			}
		}

		for (usize i = 0; i < SIZE; ++i) {

			const usize idToRemove = GetNum<usize>(0, q.GetSize() - 1);

			EXPECT_EQ(stdq[idToRemove], q[idToRemove]);

			if (rnd.GenerateU64(0, 1) == 0) {

				Alt::String res0 = RemoveSwapLast(stdq, idToRemove);
				Alt::String res1 = q.RemoveSwapLast(idToRemove);

				EXPECT_EQ(res0, res1);
			}
			else {

				Alt::String res0 = RemoveSwapFirst(stdq, idToRemove);
				Alt::String res1 = q.RemoveSwapFirst(idToRemove);

				EXPECT_EQ(res0, res1);
			}
		}

		EXPECT_EQ(q.GetSize(), stdq.size());
		EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));
	}
}

TEST(Queue, PushRemoveRepeat) {

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	std::deque<Alt::String> stdq;
	Alt::Queue<Alt::String> q;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			if (rnd.GenerateU64(0, 1) == 0) {

				q.PushBack(val.Clone().GetValueOrDefault());
				stdq.push_back(val.Clone().GetValueOrDefault());
			}
			else {

				q.PushFront(val.Clone().GetValueOrDefault());
				stdq.push_front(val.Clone().GetValueOrDefault());
			}
		}

		for (usize i = 0; i < SIZE; ++i) {

			const usize idToRemove = GetNum<usize>(0, q.GetSize() - 1);

			EXPECT_EQ(stdq[idToRemove], q[idToRemove]);

			Alt::String res1 = q.Remove(idToRemove);
			EXPECT_EQ(stdq[idToRemove], res1);

			stdq.erase(stdq.begin() + idToRemove);
		}

		EXPECT_EQ(q.GetSize(), stdq.size());
		EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));
	}
}

TEST(Queue, PushIteratorClearRepeat) {

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	std::deque<Alt::String> stdq;
	Alt::Queue<Alt::String> q;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			if (rnd.GenerateU64(0, 1) == 0) {

				q.PushBack(val.Clone().GetValueOrDefault());
				stdq.push_back(val.Clone().GetValueOrDefault());

				EXPECT_EQ(stdq.back(), q.GetLast());
			}
			else {

				q.PushFront(val.Clone().GetValueOrDefault());
				stdq.push_front(val.Clone().GetValueOrDefault());

				EXPECT_EQ(stdq.front(), q.GetFirst());
			}
		}

		{
			usize i = 0;
			Alt::QueueIterator<Alt::String> it = q.GetIterator();
			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		{
			usize i = 0;
			Alt::QueueIterator<Alt::String> it = q.GetIterator();
			while (Alt::String* pVal = it.NextBack()) {

				EXPECT_EQ(stdq[stdq.size() - i - 1], *pVal);
				i += 1;
			}
		}

		{
			Alt::QueueIterator<Alt::String> it = q.GetIterator();

			usize i = SIZE / 2;
			it.Advance(SIZE / 2);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::QueueIterator<Alt::String> it = q.GetIterator();

			usize i = 0;
			it.AdvanceBack(SIZE / 2);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::QueueIterator<Alt::String> it = q.GetIterator();

			usize i = SIZE;
			it.Advance(SIZE);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::QueueIterator<Alt::String> it = q.GetIterator();

			usize i = 0;
			it.AdvanceBack(SIZE);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		EXPECT_EQ(q.GetSize(), SIZE);
		EXPECT_EQ(q.GetCapacity(), Alt::Num::RoundUpToPowerOf2(SIZE));

		stdq.clear();
		q.Clear();
		EXPECT_TRUE(q.IsEmpty());
	}
}

TEST(Queue, Compare) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	Alt::Queue<Alt::String> q0;
	Alt::Queue<Alt::String> q1;
	Alt::Vector<Alt::String> vec;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		q0.ResizeBack((usize)rnd.GenerateU64(0, SIZE)) iferr_return;
		q0.ResizeFront(0) iferr_return;

		q1.ResizeBack((usize)rnd.GenerateU64(0, SIZE)) iferr_return;
		q1.ResizeFront(0) iferr_return;

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			if (rnd.GenerateU64(0, 1) == 0) {

				q0.PushBack(val.Clone().GetValueOrDefault());
				q1.PushBack(val.Clone().GetValueOrDefault());
			}
			else {

				q0.PushFront(val.Clone().GetValueOrDefault());
				q1.PushFront(val.Clone().GetValueOrDefault());
			}
		}

		vec.Clear();

		q1
			.GetIterator()
			.Map([](const Alt::String& str) {
				iferr_scope;

				Alt::String result = str.Clone() iferr_panic("Allocation...");
				return result;
			})
			.CollectInto(vec) iferr_return;

		EXPECT_TRUE(q0 == q1);
		EXPECT_TRUE(q0 == vec);

		EXPECT_TRUE(q0 <= q1);
		EXPECT_TRUE(q0 <= vec);

		EXPECT_TRUE(q0 >= q1);
		EXPECT_TRUE(q0 >= vec);

		q0.PushBack("Some string0"_salloc);
		q1.PushBack("Some string1"_salloc);

		vec.Clear();

		q1
			.GetIterator()
			.Map([](const Alt::String& str) {
				iferr_scope;

				Alt::String result = str.Clone() iferr_panic("Allocation...");
				return result;
			})
			.CollectInto(vec) iferr_return;

		EXPECT_TRUE(q0 < q1);
		EXPECT_TRUE(q0 < vec);

		q0.PopBack();
		q1.PopBack();

		q0.PushBack("Some string1"_salloc);
		q1.PushBack("Some string0"_salloc);

		vec.Clear();

		q1
			.GetIterator()
			.Map([](const Alt::String& str) {
				iferr_scope;

				Alt::String result = str.Clone() iferr_panic("Allocation...");
				return result;
			})
			.CollectInto(vec) iferr_return;

		EXPECT_TRUE(q0 > q1);
		EXPECT_TRUE(q0 > vec);
	}
}
