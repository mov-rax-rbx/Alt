#include "pch.h"

#include <deque>

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/IfErr.h"

#include "../alt-std/src/Utils/Num.h"
#include "../alt-std/src/Collections/StaticQueue.h"
#include "../alt-std/src/Memory/UniquePtr.h"
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

TEST(StaticQueue, TypeTraits) {

	bool v = true;

	v = Alt::TypeTraits::is_clonable_v<Alt::StaticQueue<Rule5AllTrivial, 0>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_clonable_v<Alt::StaticQueue<Rule5AllTrivial, 4>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::StaticQueue<Rule5AllNonTrivial, 0>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::StaticQueue<Rule5AllNonTrivial, 4>>;
	EXPECT_TRUE(v);
}

TEST(StaticQueue, Construct) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticQueue<usize, 4> q0 = Alt::StaticQueue<usize, 4>::Create<4>();
	EXPECT_EQ(q0.GetSize(), 4);
	EXPECT_EQ(q0.GetCapacity(), 4);
	EXPECT_EQ(q0[0], 0);

	Alt::StaticQueue<usize, 4> q1 = Alt::StaticQueue<usize, 4>::Create<3>();
	EXPECT_EQ(q1.GetSize(), 3);
	EXPECT_EQ(q1.GetCapacity(), 4);
	EXPECT_EQ(q1[0], 0);

	//Alt::StaticQueue<usize, 4> q2 = Alt::StaticQueue<usize, 4>::Create<5>(); // compilation error

	Alt::StaticQueue<usize, 4> q3 = Alt::StaticQueue<usize, 4>::Create(4) iferr_return;
	EXPECT_EQ(q3.GetSize(), 4);
	EXPECT_EQ(q3.GetCapacity(), 4);
	EXPECT_EQ(q3[0], 0);

	Alt::StaticQueue<usize, 4> q4 = Alt::StaticQueue<usize, 4>::Create(3) iferr_return;
	EXPECT_EQ(q4.GetSize(), 3);
	EXPECT_EQ(q4.GetCapacity(), 4);
	EXPECT_EQ(q4[0], 0);

	Alt::Optional<Alt::StaticQueue<usize, 4>> q5 = Alt::StaticQueue<usize, 4>::Create(5);
	EXPECT_FALSE(q5);

	Alt::StaticQueue<usize, 4> q6 = Alt::StaticQueue<usize, 4>::Create() iferr_return;
	EXPECT_EQ(q6.GetSize(), 0);
	EXPECT_EQ(q6.GetCapacity(), 4);

	Alt::StaticQueue<usize, 4> q7 = Alt::StaticQueue<usize, 4>::Create(0) iferr_return;
	EXPECT_EQ(q7.GetSize(), 0);
	EXPECT_EQ(q7.GetCapacity(), 4);
	EXPECT_TRUE(q7.IsEmpty());

	Alt::StaticQueue<usize, 4> q8{};
	EXPECT_EQ(q8.GetSize(), 0);
	EXPECT_EQ(q8.GetCapacity(), 4);
	EXPECT_TRUE(q8.IsEmpty());

	Alt::StaticQueue<usize, 4> q9 = Alt::StaticQueue<usize, 4>::Create(3, 1) iferr_return;
	EXPECT_EQ(q9.GetSize(), 3);
	EXPECT_EQ(q9.GetCapacity(), 4);
	EXPECT_EQ(q9[0], 1);

	Alt::StaticQueue<usize, 0> q10 = Alt::StaticQueue<usize, 0>::Create(0, 1) iferr_return;
	EXPECT_EQ(q10.GetSize(), 0);
	EXPECT_EQ(q10.GetCapacity(), 0);
	EXPECT_TRUE(q10.IsEmpty());

	Alt::StaticQueue<Rule5AllTrivial_DeletedDefaultConstructor, 4> q11 =
		Alt::StaticQueue<Rule5AllTrivial_DeletedDefaultConstructor, 4>::Create<2>(
			Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct }
		);

	EXPECT_EQ(q11.GetSize(), 2);
	EXPECT_EQ(q11.GetCapacity(), 4);

	//Alt::StaticQueue<Rule5AllTrivial_DeletedDefaultConstructor, 4> q12 =
	//	Alt::StaticQueue<Rule5AllTrivial_DeletedDefaultConstructor, 4>::Create<2>(); // compilation error (type must be default constructible)

	Alt::StaticQueue<Rule5AllTrivial_DeletedDefaultConstructor, 4> q13 =
		Alt::StaticQueue<Rule5AllTrivial_DeletedDefaultConstructor, 4>::Create(
			2, Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct }
		) iferr_return;

	EXPECT_EQ(q13.GetSize(), 2);
	EXPECT_EQ(q13.GetCapacity(), 4);
}

TEST(StaticQueue, ZeroSize) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticQueue<usize, 0> q = Alt::StaticQueue<usize, 0>::Create<0>();
		EXPECT_TRUE(q.IsEmpty());

		const bool isOk = q.PushBack(0);
		EXPECT_FALSE(isOk);

		ALT_MAYBE_UNUSED Alt::StaticQueue<usize, 0> q1 = q.Clone() iferr_return;
	}

	{
		Alt::Array<usize, 0> q = Alt::Array<usize, 0>::Create();
		EXPECT_TRUE(q.IsEmpty());
		EXPECT_FALSE(q.Data());

		ALT_MAYBE_UNUSED Alt::Array<usize, 0> q1 = q.Clone() iferr_return;
	}
}

TEST(StaticQueue, CapacityOverflow) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticQueue<usize, 2> q;

	q.PushBack(0) iferr_return;
	q.PushBack(1) iferr_return;

	const bool isOk = q.PushBack(2);
	EXPECT_FALSE(isOk);

	//Alt::StaticQueue<usize, 2> q1 = Alt::StaticQueue<usize, 2>::CreatePush(0, 1, 2); // compilation error

	Alt::Optional<Alt::StaticQueue<usize, 2>> q2 = Alt::StaticQueue<usize, 2>::Create(4);
	EXPECT_FALSE(q2);
}

/*
 * Below the tests which are identical to the 'QueueTest' (except reserve stuff)
 */
TEST(StaticQueue, Move) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticQueue<Alt::String, 4> q0{};
	q0.PushBack("Queue"_salloc) iferr_return;

	//Alt::StaticQueue<Alt::String, 3> q1 = std::move(q0); // compilation error

	Alt::StaticQueue<Alt::String, 5> q1 = std::move(q0);
	EXPECT_TRUE(q0.IsEmpty());
	EXPECT_EQ(q1.GetSize(), 1);
	EXPECT_EQ(q1[0], "Queue");

	Alt::StaticQueue<Alt::String, 5> q2;
	q2 = std::move(q1);
	EXPECT_TRUE(q1.IsEmpty());
	EXPECT_EQ(q2.GetSize(), 1);
	EXPECT_EQ(q2[0], "Queue");
}

TEST(StaticQueue, Clone) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticQueue<Alt::String, 8> q0;
		q0.PushBack("StaticQueue0"_salloc) iferr_return;
		q0.PushBack("StaticQueue1"_salloc) iferr_return;

		Alt::StaticQueue<Alt::String, 8> q1 = q0.Clone() iferr_return;
		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("StaticQueue2"_salloc) iferr_return;
		q1.PopFront();

		EXPECT_EQ(q1[0], "StaticQueue1");
		EXPECT_EQ(q1[1], "StaticQueue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}
	}

	{
		Alt::StaticQueue<Alt::String, 8> q0;
		q0.PushFront("StaticQueue1"_salloc) iferr_return;
		q0.PushFront("StaticQueue0"_salloc) iferr_return;

		Alt::StaticQueue<Alt::String, 8> q1 = q0.Clone() iferr_return;
		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("StaticQueue2"_salloc) iferr_return;
		q1.PopFront();

		EXPECT_EQ(q1[0], "StaticQueue1");
		EXPECT_EQ(q1[1], "StaticQueue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}
	}
}

TEST(StaticQueue, CloneFromAndMoveFrom) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticQueue<Alt::String, 8> q0;

		//Alt::StaticQueue<Alt::String, 8> q1 = q0; // compilation error, copy can fail
		//q1 = q0; // compilation error, copy can fail
	}

	{
		Alt::StaticQueue<usize, 8> q0;

		Alt::StaticQueue<usize, 8> q1 = q0;
		q1 = q0;
	}

	Alt::StaticQueue<Alt::String, 8> q0;
	q0.PushBack("StaticQueue0"_salloc) iferr_return;
	q0.PushBack("StaticQueue1"_salloc) iferr_return;

	{
		Alt::StaticQueue<Alt::String, 8> q1;
		q1.CloneFrom(q0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("StaticQueue2"_salloc) iferr_return;
		q1.PopFront();

		EXPECT_EQ(q1[0], "StaticQueue1");
		EXPECT_EQ(q1[1], "StaticQueue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}

		Alt::StaticQueue<Alt::String, 8> q2 = q1.Clone() iferr_return;

		q1.PushFront("StaticQueue3"_salloc) iferr_return;
		q1.PushFront("StaticQueue4"_salloc) iferr_return;
		q1.PushFront("StaticQueue5"_salloc) iferr_return;
		EXPECT_EQ(q1.GetSize(), 5);

		q1.CloneFrom(q2) iferr_return;
		EXPECT_EQ(q1.GetSize(), 2);

		q1.CloneFrom(Alt::StaticQueue<Alt::String, 8>{}) iferr_return;
		EXPECT_TRUE(q1.IsEmpty());
	}

	{
		Alt::StaticQueue<Alt::String, 8> q1;
		q1.MoveFrom(q0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("StaticQueue2"_salloc) iferr_return;
		q1.PopFront();

		EXPECT_EQ(q1[0], "StaticQueue1");
		EXPECT_EQ(q1[1], "StaticQueue2");

		EXPECT_EQ(q1.GetSize(), 2);

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_NE(q1[i], q0[i]);
		}

		Alt::StaticQueue<Alt::String, 8> q2 = q1.Clone() iferr_return;

		q1.PushFront("StaticQueue3"_salloc) iferr_return;
		q1.PushFront("StaticQueue4"_salloc) iferr_return;
		q1.PushFront("StaticQueue5"_salloc) iferr_return;
		EXPECT_EQ(q1.GetSize(), 5);

		q1.MoveFrom(q2) iferr_return;
		EXPECT_EQ(q1.GetSize(), 2);

		q1.MoveFrom(Alt::StaticQueue<Alt::String, 8>{}) iferr_return;
		EXPECT_TRUE(q1.IsEmpty());
	}
}

TEST(StaticQueue, AppendBackCloneAndAppendMove) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticQueue<Alt::String, 8> q0;
	q0.PushBack("StaticQueue0"_salloc) iferr_return;
	q0.PushBack("StaticQueue1"_salloc) iferr_return;

	{
		Alt::StaticQueue<Alt::String, 8> q1;
		q1.AppendBackClone(q0) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("StaticQueue2"_salloc) iferr_return;
		q1.PushFront("StaticQueue3"_salloc) iferr_return;
		q1.PopFront();
		q1.PopFront();

		EXPECT_EQ(q1[0], "StaticQueue1");
		EXPECT_EQ(q1[1], "StaticQueue2");

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
		Alt::StaticQueue<Alt::String, 8> q1;
		q1.AppendBackMove(q0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("StaticQueue2"_salloc) iferr_return;
		q1.PushFront("StaticQueue3"_salloc) iferr_return;
		q1.PopFront();
		q1.PopFront();

		EXPECT_EQ(q1[0], "StaticQueue1");
		EXPECT_EQ(q1[1], "StaticQueue2");

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

TEST(StaticQueue, AppendFrontCloneAndAppendMove) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticQueue<Alt::String, 8> q0;
	q0.PushBack("StaticQueue0"_salloc) iferr_return;
	q0.PushBack("StaticQueue1"_salloc) iferr_return;

	{
		Alt::StaticQueue<Alt::String, 8> q1;
		q1.AppendFrontClone(q0) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("StaticQueue2"_salloc) iferr_return;
		q1.PopFront();

		EXPECT_EQ(q1[0], "StaticQueue1");
		EXPECT_EQ(q1[1], "StaticQueue2");

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
		Alt::StaticQueue<Alt::String, 8> q1;
		q1.AppendFrontMove(q0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(q1.GetSize(), q0.GetSize());

		for (usize i = 0; i < q1.GetSize(); ++i) {

			EXPECT_EQ(q1[i], q0[i]);
		}

		q1.PushBack("StaticQueue2"_salloc) iferr_return;
		q1.PopFront();

		EXPECT_EQ(q1[0], "StaticQueue1");
		EXPECT_EQ(q1[1], "StaticQueue2");

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

TEST(StaticQueue, Rule5Tracking) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticQueue<Rule5TrackerInlined, 16> vec = Alt::StaticQueue<Rule5TrackerInlined, 16>::Create<5>();

		Rule5TrackerInlined v0 = vec.PopBack();
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
		EXPECT_EQ(v2.core.cntMoveAssignment, 0);

		Rule5TrackerInlined v3 = vec.Remove(0);
		EXPECT_EQ(v3.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v3.core.cntMoveConstruct, 2);
		EXPECT_EQ(v3.core.cntMoveAssignment, 0);

		Rule5TrackerInlined v4 = vec.Remove(0);
		EXPECT_EQ(v4.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v4.core.cntMoveConstruct, 2);
		EXPECT_EQ(v4.core.cntMoveAssignment, 0);

		EXPECT_TRUE(vec.IsEmpty());
	}

	{
		Alt::StaticQueue<Rule5TrackerInlinedTriviallyReallocatable, 16> vec =
			Alt::StaticQueue<Rule5TrackerInlinedTriviallyReallocatable, 16>::Create<5>();

		Rule5TrackerInlinedTriviallyReallocatable v0 = vec.PopBack();
		EXPECT_EQ(v0.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v0.core.cntMoveConstruct, 1);
		EXPECT_EQ(v0.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v1 = vec.Remove(0);
		EXPECT_EQ(v1.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v1.core.cntMoveConstruct, 2);
		EXPECT_EQ(v1.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v2 = vec.Remove(0);
		EXPECT_EQ(v2.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v2.core.cntMoveConstruct, 2);
		EXPECT_EQ(v2.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v3 = vec.Remove(0);
		EXPECT_EQ(v3.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v3.core.cntMoveConstruct, 2);
		EXPECT_EQ(v3.core.cntMoveAssignment, 0);

		Rule5TrackerInlinedTriviallyReallocatable v4 = vec.Remove(0);
		EXPECT_EQ(v4.core.cntDefaultConstruct, 0);
		EXPECT_EQ(v4.core.cntMoveConstruct, 2);
		EXPECT_EQ(v4.core.cntMoveAssignment, 0);

		EXPECT_TRUE(vec.IsEmpty());
	}

	{
		Rule5Tracker::Core tracker{};
		Rule5Tracker track{ tracker };

		Alt::StaticQueue<Rule5Tracker, 16> vec0 = Alt::StaticQueue<Rule5Tracker, 16>::Create<5>(track);
		Alt::StaticQueue<Rule5Tracker, 16> vec1 = Alt::StaticQueue<Rule5Tracker, 16>::Create<5>(track);

		vec0 = std::move(vec1);
		EXPECT_EQ(tracker.cntDestructor, 10); // destruct in '::Create' then in 'operator=(...)'

		Alt::StaticQueue<Rule5Tracker, 16> vec2 = std::move(vec0);
		EXPECT_EQ(tracker.cntDestructor, 20);

		vec2 = {};
		EXPECT_EQ(tracker.cntDestructor, 25);
	}
}

TEST(StaticQueue, PushPopClearRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	Alt::UniquePtr<Alt::StaticQueue<Alt::String, SIZE>> qPtr =
		Alt::UniquePtr<Alt::StaticQueue<Alt::String, SIZE>>::AllocateObject() iferr_return;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		if (i % 2 == 0) {

			for (usize i = 0; i < SIZE; ++i) {

				Alt::String val = AllocateStr(1, 100);

				if (rnd.GenerateU64(0, 1) == 0) {

					qPtr->PushBack(std::move(val)) iferr_return;
				}
				else {

					qPtr->PushFront(std::move(val)) iferr_return;
				}
			}
		}
		else {

			static_assert(SIZE % 2 == 0, "");
			for (usize i = 0; i < SIZE / 2; ++i) {

				if (rnd.GenerateU64(0, 1) == 0) {

					qPtr->PushBack(AllocateStr(1, 100), AllocateStr(1, 100)) iferr_return;
				}
				else {

					qPtr->PushFront(AllocateStr(1, 100), AllocateStr(1, 100)) iferr_return;
				}
			}
		}

		EXPECT_EQ(qPtr->GetSize(), SIZE);
		EXPECT_EQ(qPtr->GetCapacity(), SIZE);

		for (usize i = 0; i < SIZE / 2; ++i) {

			if (rnd.GenerateU64(0, 1) == 0) {

				qPtr->PopBack();
			}
			else {

				qPtr->PopFront();
			}
		}

		EXPECT_TRUE(qPtr->IsPopulated());
		EXPECT_EQ(qPtr->GetSize(), SIZE - SIZE / 2);

		qPtr->Clear();
		EXPECT_TRUE(qPtr->IsEmpty());
		EXPECT_EQ(qPtr->GetSize(), 0);
		EXPECT_EQ(qPtr->GetCapacity(), SIZE);
	}
}

TEST(StaticQueue, PushRemoveSwapRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	std::deque<Alt::String> stdq;

	Alt::UniquePtr<Alt::StaticQueue<Alt::String, SIZE>> qPtr =
		Alt::UniquePtr<Alt::StaticQueue<Alt::String, SIZE>>::AllocateObject() iferr_return;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			if (rnd.GenerateU64(0, 1) == 0) {

				qPtr->PushBack(val.Clone().GetValueOrDefault()) iferr_return;
				stdq.push_back(val.Clone().GetValueOrDefault());
			}
			else {

				qPtr->PushFront(val.Clone().GetValueOrDefault()) iferr_return;
				stdq.push_front(val.Clone().GetValueOrDefault());
			}
		}

		for (usize i = 0; i < SIZE; ++i) {

			const usize idToRemove = GetNum<usize>(0, qPtr->GetSize() - 1);

			EXPECT_EQ(stdq[idToRemove], (*qPtr)[idToRemove]);

			if (rnd.GenerateU64(0, 1) == 0) {

				Alt::String res0 = RemoveSwapLast(stdq, idToRemove);
				Alt::String res1 = qPtr->RemoveSwapLast(idToRemove);

				EXPECT_EQ(res0, res1);
			}
			else {

				Alt::String res0 = RemoveSwapFirst(stdq, idToRemove);
				Alt::String res1 = qPtr->RemoveSwapFirst(idToRemove);

				EXPECT_EQ(res0, res1);
			}
		}

		EXPECT_EQ(qPtr->GetSize(), stdq.size());
		EXPECT_EQ(qPtr->GetCapacity(), SIZE);
	}
}

TEST(StaticQueue, PushRemoveRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	std::deque<Alt::String> stdq;

	Alt::UniquePtr<Alt::StaticQueue<Alt::String, SIZE>> qPtr =
		Alt::UniquePtr<Alt::StaticQueue<Alt::String, SIZE>>::AllocateObject() iferr_return;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			if (rnd.GenerateU64(0, 1) == 0) {

				qPtr->PushBack(val.Clone().GetValueOrDefault()) iferr_return;
				stdq.push_back(val.Clone().GetValueOrDefault());
			}
			else {

				qPtr->PushFront(val.Clone().GetValueOrDefault()) iferr_return;
				stdq.push_front(val.Clone().GetValueOrDefault());
			}
		}

		for (usize i = 0; i < SIZE; ++i) {

			const usize idToRemove = GetNum<usize>(0, qPtr->GetSize() - 1);

			EXPECT_EQ(stdq[idToRemove], (*qPtr)[idToRemove]);

			Alt::String res1 = qPtr->Remove(idToRemove);
			EXPECT_EQ(stdq[idToRemove], res1);

			stdq.erase(stdq.begin() + idToRemove);
		}

		EXPECT_EQ(qPtr->GetSize(), stdq.size());
		EXPECT_EQ(qPtr->GetCapacity(), SIZE);
	}
}

TEST(StaticQueue, PushIteratorClearRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	std::deque<Alt::String> stdq;

	Alt::UniquePtr<Alt::StaticQueue<Alt::String, SIZE>> qPtr =
		Alt::UniquePtr<Alt::StaticQueue<Alt::String, SIZE>>::AllocateObject() iferr_return;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			if (rnd.GenerateU64(0, 1) == 0) {

				qPtr->PushBack(val.Clone().GetValueOrDefault()) iferr_return;
				stdq.push_back(val.Clone().GetValueOrDefault());

				EXPECT_EQ(stdq.back(), qPtr->GetLast());
			}
			else {

				qPtr->PushFront(val.Clone().GetValueOrDefault()) iferr_return;
				stdq.push_front(val.Clone().GetValueOrDefault());

				EXPECT_EQ(stdq.front(), qPtr->GetFirst());
			}
		}

		{
			usize i = 0;
			Alt::QueueIterator<Alt::String> it = qPtr->GetIterator();
			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		{
			usize i = 0;
			Alt::QueueIterator<Alt::String> it = qPtr->GetIterator();
			while (Alt::String* pVal = it.NextBack()) {

				EXPECT_EQ(stdq[stdq.size() - i - 1], *pVal);
				i += 1;
			}
		}

		{
			Alt::QueueIterator<Alt::String> it = qPtr->GetIterator();

			usize i = SIZE / 2;
			it.Advance(SIZE / 2);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::QueueIterator<Alt::String> it = qPtr->GetIterator();

			usize i = 0;
			it.AdvanceBack(SIZE / 2);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::QueueIterator<Alt::String> it = qPtr->GetIterator();

			usize i = SIZE;
			it.Advance(SIZE);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::QueueIterator<Alt::String> it = qPtr->GetIterator();

			usize i = 0;
			it.AdvanceBack(SIZE);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdq[i], *pVal);
				i += 1;
			}
		}

		EXPECT_EQ(qPtr->GetSize(), SIZE);
		EXPECT_EQ(qPtr->GetCapacity(), SIZE);

		stdq.clear();
		qPtr->Clear();
		EXPECT_TRUE(qPtr->IsEmpty());
	}
}

TEST(StaticQueue, Compare) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;
	constexpr usize CAPACITY = SIZE + 1;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	Alt::UniquePtr<Alt::StaticQueue<Alt::String, CAPACITY>> q0Ptr =
		Alt::UniquePtr<Alt::StaticQueue<Alt::String, CAPACITY>>::AllocateObject() iferr_return;

	Alt::UniquePtr<Alt::StaticQueue<Alt::String, CAPACITY>> q1Ptr =
		Alt::UniquePtr<Alt::StaticQueue<Alt::String, CAPACITY>>::AllocateObject() iferr_return;

	Alt::Vector<Alt::String> vec;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		q0Ptr->ResizeBack((usize)rnd.GenerateU64(0, SIZE)) iferr_return;
		q0Ptr->ResizeFront(0) iferr_return;

		q1Ptr->ResizeBack((usize)rnd.GenerateU64(0, SIZE)) iferr_return;
		q1Ptr->ResizeFront(0) iferr_return;

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			if (rnd.GenerateU64(0, 1) == 0) {

				q0Ptr->PushBack(val.Clone().GetValueOrDefault()) iferr_return;
				q1Ptr->PushBack(val.Clone().GetValueOrDefault()) iferr_return;
			}
			else {

				q0Ptr->PushFront(val.Clone().GetValueOrDefault()) iferr_return;
				q1Ptr->PushFront(val.Clone().GetValueOrDefault()) iferr_return;
			}
		}

		vec.Clear();

		q1Ptr
			->GetIterator()
			.Map([](const Alt::String& str) {
				iferr_scope;

				Alt::String result = str.Clone() iferr_panic("Allocation...");
				return result;
			})
			.CollectInto(vec) iferr_return;

		EXPECT_TRUE(*q0Ptr == *q1Ptr);
		EXPECT_TRUE(*q0Ptr == vec);

		EXPECT_TRUE(*q0Ptr <= *q1Ptr);
		EXPECT_TRUE(*q0Ptr <= vec);

		EXPECT_TRUE(*q0Ptr >= *q1Ptr);
		EXPECT_TRUE(*q0Ptr >= vec);

		q0Ptr->PushBack("Some string0"_salloc) iferr_return;
		q1Ptr->PushBack("Some string1"_salloc) iferr_return;

		vec.Clear();

		q1Ptr
			->GetIterator()
			.Map([](const Alt::String& str) {
				iferr_scope;

				Alt::String result = str.Clone() iferr_panic("Allocation...");
				return result;
			})
			.CollectInto(vec) iferr_return;

		EXPECT_TRUE(*q0Ptr < *q1Ptr);
		EXPECT_TRUE(*q0Ptr < vec);

		q0Ptr->PopBack();
		q1Ptr->PopBack();

		q0Ptr->PushBack("Some string1"_salloc) iferr_return;
		q1Ptr->PushBack("Some string0"_salloc) iferr_return;

		vec.Clear();

		q1Ptr
			->GetIterator()
			.Map([](const Alt::String& str) {
				iferr_scope;

				Alt::String result = str.Clone() iferr_panic("Allocation...");
				return result;
			})
			.CollectInto(vec) iferr_return;

		EXPECT_TRUE(*q0Ptr > *q1Ptr);
		EXPECT_TRUE(*q0Ptr > vec);
	}
}
