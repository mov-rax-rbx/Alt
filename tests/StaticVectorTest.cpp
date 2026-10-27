#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Collections/Vector.h"
#include "../alt-std/src/Collections/StaticVector.h"
#include "../alt-std/src/Memory/UniquePtr.h"

#pragma warning(push)
#pragma warning(disable: 4244 /* conversion from ... to ..., possible loss of data */)

TEST(StaticVector, TypeTraits) {

	bool v = true;

	v = Alt::TypeTraits::is_clonable_v<Alt::StaticVector<usize, 4>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_clonable_v<Alt::Array<usize, 4>>;
	EXPECT_TRUE(v);
}

TEST(StaticVector, Construct) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticVector<usize, 4> vec0 = Alt::StaticVector<usize, 4>::Create<4>();
		EXPECT_EQ(vec0.GetSize(), 4);
		EXPECT_EQ(vec0.GetCapacity(), 4);
		EXPECT_EQ(vec0[0], 0);

		Alt::StaticVector<usize, 4> vec1 = Alt::StaticVector<usize, 4>::Create<3>();
		EXPECT_EQ(vec1.GetSize(), 3);
		EXPECT_EQ(vec1.GetCapacity(), 4);
		EXPECT_EQ(vec1[0], 0);

		//Alt::StaticVector<usize, 4> vec2 = Alt::StaticVector<usize, 4>::Create<5>(); // compilation error

		Alt::StaticVector<usize, 4> vec3 = Alt::StaticVector<usize, 4>::Create(4) iferr_return;
		EXPECT_EQ(vec3.GetSize(), 4);
		EXPECT_EQ(vec3.GetCapacity(), 4);
		EXPECT_EQ(vec3[0], 0);

		Alt::StaticVector<usize, 4> vec4 = Alt::StaticVector<usize, 4>::Create(3) iferr_return;
		EXPECT_EQ(vec4.GetSize(), 3);
		EXPECT_EQ(vec4.GetCapacity(), 4);
		EXPECT_EQ(vec4[0], 0);

		Alt::Optional<Alt::StaticVector<usize, 4>> vec5 = Alt::StaticVector<usize, 4>::Create(5);
		EXPECT_FALSE(vec5);

		Alt::StaticVector<usize, 4> vec6 = Alt::StaticVector<usize, 4>::Create() iferr_return;
		EXPECT_EQ(vec6.GetSize(), 0);
		EXPECT_EQ(vec6.GetCapacity(), 4);

		Alt::StaticVector<usize, 4> vec7 = Alt::StaticVector<usize, 4>::Create(0) iferr_return;
		EXPECT_EQ(vec7.GetSize(), 0);
		EXPECT_EQ(vec7.GetCapacity(), 4);
		EXPECT_TRUE(vec7.IsEmpty());

		Alt::StaticVector<usize, 4> vec8{};
		EXPECT_EQ(vec8.GetSize(), 0);
		EXPECT_EQ(vec8.GetCapacity(), 4);
		EXPECT_TRUE(vec8.IsEmpty());

		Alt::StaticVector<usize, 4> vec9 = Alt::StaticVector<usize, 4>::Create(3, 1) iferr_return;
		EXPECT_EQ(vec9.GetSize(), 3);
		EXPECT_EQ(vec9.GetCapacity(), 4);
		EXPECT_EQ(vec9[0], 1);

		Alt::StaticVector<usize, 0> vec10 = Alt::StaticVector<usize, 0>::Create(0, 1) iferr_return;
		EXPECT_EQ(vec10.GetSize(), 0);
		EXPECT_EQ(vec10.GetCapacity(), 0);
		EXPECT_TRUE(vec10.IsEmpty());

		Alt::StaticVector<Rule5AllTrivial_DeletedDefaultConstructor, 4> vec11 =
			Alt::StaticVector<Rule5AllTrivial_DeletedDefaultConstructor, 4>::Create<2>(
				Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct }
			);

		EXPECT_EQ(vec11.GetSize(), 2);
		EXPECT_EQ(vec11.GetCapacity(), 4);

		//Alt::StaticVector<Rule5AllTrivial_DeletedDefaultConstructor, 4> vec12 =
		//	Alt::StaticVector<Rule5AllTrivial_DeletedDefaultConstructor, 4>::Create<2>(); // compilation error (type must be default constructible)

		Alt::StaticVector<Rule5AllTrivial_DeletedDefaultConstructor, 4> vec13 =
			Alt::StaticVector<Rule5AllTrivial_DeletedDefaultConstructor, 4>::Create(
				2, Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct }
			) iferr_return;

		EXPECT_EQ(vec13.GetSize(), 2);
		EXPECT_EQ(vec13.GetCapacity(), 4);
	}

	{
		Alt::StaticVector<usize, 4> vec0 = Alt::StaticVector<usize, 4>::CreatePush(0, 1, 2, 3) iferr_return;
		EXPECT_EQ(vec0.GetSize(), 4);
		EXPECT_EQ(vec0.GetCapacity(), 4);
		EXPECT_EQ(vec0[0], 0);
		EXPECT_EQ(vec0[1], 1);

		Alt::StaticVector<usize, 4> vec1 = Alt::StaticVector<usize, 4>::CreatePush(0) iferr_return;
		EXPECT_EQ(vec1.GetSize(), 1);
		EXPECT_EQ(vec1.GetCapacity(), 4);
		EXPECT_EQ(vec1[0], 0);

		//Alt::StaticVector<usize, 4> vec2 = Alt::StaticVector<usize, 4>::CreatePush(0, 1, 2, 3, 4); // compilation error

		Alt::StaticVector<usize, 4> vec3{ 0, 1, 2, 3 };
		EXPECT_EQ(vec3.GetSize(), 4);
		EXPECT_EQ(vec3.GetCapacity(), 4);
		EXPECT_EQ(vec3[0], 0);
		EXPECT_EQ(vec3[1], 1);

		Alt::StaticVector<usize, 4> vec4{ 0 };
		EXPECT_EQ(vec4.GetSize(), 1);
		EXPECT_EQ(vec4.GetCapacity(), 4);
		EXPECT_EQ(vec4[0], 0);

		Alt::StaticVector<usize, 4> vec5{};
		EXPECT_EQ(vec5.GetSize(), 0);
		EXPECT_EQ(vec5.GetCapacity(), 4);
		EXPECT_TRUE(vec5.IsEmpty());

		Alt::StaticVector<usize, 0> vec6{};
		EXPECT_EQ(vec6.GetSize(), 0);
		EXPECT_EQ(vec6.GetCapacity(), 0);
		EXPECT_TRUE(vec6.IsEmpty());
	}

	{
		Alt::Array<usize, 4> vec0 = Alt::Array<usize, 4>::Create();
		EXPECT_EQ(vec0.GetSize(), 4);
		EXPECT_EQ(vec0.GetCapacity(), 4);
		EXPECT_EQ(vec0[0], 0);

		//Alt::Array<Rule5AllTrivial_DeletedDefaultConstructor, 4> vec1 =
		//	Alt::Array<Rule5AllTrivial_DeletedDefaultConstructor, 4>::Create(); // compilation error (type must be default constructible)

		//Alt::Array<Rule5AllTrivial_DeletedDefaultConstructor, 4> vec2{}; // compilation error (type must be default constructible)

		Alt::Optional<Alt::Array<Rule5AllTrivial_DeletedDefaultConstructor, 4>> vec3 =
			Alt::Array<Rule5AllTrivial_DeletedDefaultConstructor, 4>::Create(
				Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct }
			);

		ASSERT_TRUE(vec3);
		EXPECT_EQ(vec3->GetSize(), 4);
		EXPECT_EQ(vec3->GetCapacity(), 4);

		Alt::Optional<Alt::Array<Rule5AllTrivial_DeletedDefaultConstructor, 4>> vec4 =
			Alt::Array<Rule5AllTrivial_DeletedDefaultConstructor, 4>::CreatePush(
				Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct },
				Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct },
				Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct },
				Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct }
			);
		ASSERT_TRUE(vec4);
		EXPECT_EQ(vec4->GetSize(), 4);
		EXPECT_EQ(vec4->GetCapacity(), 4);

		Alt::Array<Rule5AllTrivial_DeletedDefaultConstructor, 4> vec5{
			Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct },
			Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct },
			Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct },
			Rule5AllTrivial_DeletedDefaultConstructor{ Rule5_DefaultConstruct },
		};
		EXPECT_EQ(vec5.GetSize(), 4);
		EXPECT_EQ(vec5.GetCapacity(), 4);

		Alt::Array<usize, 4> vec6 = Alt::Array<usize, 4>::CreatePush(1, 2) iferr_return;
		EXPECT_EQ(vec6.GetSize(), 4);
		EXPECT_EQ(vec6.GetCapacity(), 4);
		EXPECT_EQ(vec6[0], 1);
		EXPECT_EQ(vec6[1], 2);
		EXPECT_EQ(vec6[2], 0);

		Alt::Array<usize, 4> vec7{ 1, 2 };
		EXPECT_EQ(vec7.GetSize(), 4);
		EXPECT_EQ(vec7.GetCapacity(), 4);
		EXPECT_EQ(vec7[0], 1);
		EXPECT_EQ(vec7[1], 2);
		EXPECT_EQ(vec7[2], 0);

		Alt::Array<usize, 2> vec8 = Alt::Array<usize, 2>::CreatePush(1, 2) iferr_return;
		EXPECT_EQ(vec8.GetSize(), 2);
		EXPECT_EQ(vec8.GetCapacity(), 2);
		EXPECT_EQ(vec8[0], 1);
		EXPECT_EQ(vec8[1], 2);

		Alt::Array<usize, 2> vec9{ 1, 2 };
		EXPECT_EQ(vec9.GetSize(), 2);
		EXPECT_EQ(vec9.GetCapacity(), 2);
		EXPECT_EQ(vec9[0], 1);
		EXPECT_EQ(vec9[1], 2);

		auto vec10 = Alt::MakeArray(1, 2);
		EXPECT_EQ(vec9.GetSize(), 2);
		EXPECT_EQ(vec9.GetCapacity(), 2);
		EXPECT_EQ(vec9[0], 1);
		EXPECT_EQ(vec9[1], 2);
	}
}

TEST(StaticVector, ZeroSize) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticVector<usize, 0> vec = Alt::StaticVector<usize, 0>::Create<0>();
		EXPECT_TRUE(vec.IsEmpty());
		EXPECT_FALSE(vec.Data());

		const bool isOk = vec.Push(0);
		EXPECT_FALSE(isOk);

		ALT_MAYBE_UNUSED Alt::StaticVector<usize, 0> vec1 = vec.Clone() iferr_return;
	}

	{
		Alt::Array<usize, 0> vec = Alt::Array<usize, 0>::Create();
		EXPECT_TRUE(vec.IsEmpty());
		EXPECT_FALSE(vec.Data());

		ALT_MAYBE_UNUSED Alt::Array<usize, 0> vec1 = vec.Clone() iferr_return;
	}
}

TEST(StaticVector, CapacityOverflow) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticVector<usize, 2> vec;

	vec.Push(0) iferr_return;
	vec.Push(1) iferr_return;

	const bool isOk = vec.Push(2);
	EXPECT_FALSE(isOk);

	//Alt::StaticVector<usize, 2> vec1 = Alt::StaticVector<usize, 2>::CreatePush(0, 1, 2); // compilation error

	Alt::Optional<Alt::StaticVector<usize, 2>> vec2 = Alt::StaticVector<usize, 2>::Create(4);
	EXPECT_FALSE(vec2);
}

/*
 * Below the tests which are identical to the 'VectorTest' (except reserve stuff)
 */
TEST(StaticVector, Move) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticVector<Alt::String, 4> vec0{};
		vec0.Push("Vector"_salloc) iferr_return;

		//Alt::StaticVector<Alt::String, 3> vec1 = std::move(vec0); // compilation error

		Alt::StaticVector<Alt::String, 5> vec1 = std::move(vec0);
		EXPECT_TRUE(vec0.IsEmpty());
		EXPECT_EQ(vec1.GetSize(), 1);
		EXPECT_EQ(vec1[0], "Vector");

		Alt::StaticVector<Alt::String, 5> vec2;
		vec2 = std::move(vec1);
		EXPECT_TRUE(vec1.IsEmpty());
		EXPECT_EQ(vec2.GetSize(), 1);
		EXPECT_EQ(vec2[0], "Vector");
	}

	{
		Alt::Array<Alt::String, 4> vec0{};
		vec0[0] = "Vector"_salloc;

		//Alt::Array<Alt::String, 3> vec1 = std::move(vec0); // compilation error
		//Alt::Array<Alt::String, 5> vec1 = std::move(vec0); // compilation error

		Alt::Array<Alt::String, 4> vec1 = std::move(vec0);
		EXPECT_FALSE(vec0.IsEmpty());
		EXPECT_EQ(vec1.GetSize(), 4);
		EXPECT_EQ(vec1[0], "Vector");

		Alt::Array<Alt::String, 4> vec2;
		vec2 = std::move(vec1);
		EXPECT_FALSE(vec1.IsEmpty());
		EXPECT_EQ(vec2.GetSize(), 4);
		EXPECT_EQ(vec2[0], "Vector");
	}
}

TEST(Vector, Copy) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticVector<Alt::String, 4> vec0;
	vec0.Push("Vector0"_salloc) iferr_return;
	vec0.Push("Vector1"_salloc) iferr_return;

	//Alt::StaticVector<Alt::String, 3> vec1 = vec0; // compilation error

	{
		Alt::StaticVector<Alt::String, 4> vec1 = vec0.Clone() iferr_return;
		EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_EQ(vec1[i], vec0[i]);
		}

		vec1.Push("Vector2"_salloc) iferr_return;
		vec1.Remove(0);

		EXPECT_EQ(vec1[0], "Vector1");
		EXPECT_EQ(vec1[1], "Vector2");

		EXPECT_EQ(vec1.GetSize(), 2);

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_NE(vec1[i], vec0[i]);
		}
	}

	{
		Alt::StaticVector<Alt::String, 5> vec1 = vec0.Clone() iferr_return;
		EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_EQ(vec1[i], vec0[i]);
		}

		vec1.Push("Vector2"_salloc) iferr_return;
		vec1.Remove(0);

		EXPECT_EQ(vec1[0], "Vector1");
		EXPECT_EQ(vec1[1], "Vector2");

		EXPECT_EQ(vec1.GetSize(), 2);

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_NE(vec1[i], vec0[i]);
		}
	}
}

TEST(StaticVector, CloneFromAndMoveFrom) {

	iferr_scope_on_err {

		ALT_BREAKPOINT;

		FAIL();
	};

	{
		Alt::StaticVector<Alt::String, 4> vec0;

		//Alt::StaticVector<Alt::String, 4> vec1 = vec0;
		//vec1 = vec0;
	}

	{
		Alt::StaticVector<usize, 4> vec0;

		Alt::StaticVector<usize, 4> vec1 = vec0;
		vec1 = vec0;
	}

	{
		Alt::Array<Alt::String, 4> vec0;

		//Alt::Array<Alt::String, 4> vec1 = vec0;
		//vec1 = vec0;
	}

	{
		Alt::Array<usize, 4> vec0;

		Alt::Array<usize, 4> vec1 = vec0;
		vec1 = vec0;
	}

	{
		Alt::StaticVector<Alt::String, 4> vec0;
		vec0.Push("Vector0"_salloc) iferr_return;
		vec0.Push("Vector1"_salloc) iferr_return;

		{
			Alt::StaticVector<Alt::String, 3> vec1;
			vec1.CloneFrom(vec0) iferr_return; // CloneFrom work: 3 <- 4

			EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

			for (usize i = 0; i < vec1.GetSize(); ++i) {

				EXPECT_EQ(vec1[i], vec0[i]);
			}

			vec1.Push("Vector2"_salloc) iferr_return;
			vec1.Remove(0);

			EXPECT_EQ(vec1[0], "Vector1");
			EXPECT_EQ(vec1[1], "Vector2");

			EXPECT_EQ(vec1.GetSize(), 2);

			for (usize i = 0; i < vec1.GetSize(); ++i) {

				EXPECT_NE(vec1[i], vec0[i]);
			}

			Alt::StaticVector<Alt::String, 3> vec2 = vec1.Clone() iferr_return;

			vec1.Push("Vector3"_salloc) iferr_return;
			EXPECT_EQ(vec1.GetSize(), 3);

			vec1.CloneFrom(vec2) iferr_return;
			EXPECT_EQ(vec1.GetSize(), 2);
		}

		{
			Alt::StaticVector<Alt::String, 3> vec1;
			vec1.MoveFrom(vec0.Clone().GetValueOrDefault()) iferr_return; // MoveFrom work: 3 <- 4

			EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

			for (usize i = 0; i < vec1.GetSize(); ++i) {

				EXPECT_EQ(vec1[i], vec0[i]);
			}

			vec1.Push("Vector2"_salloc) iferr_return;
			vec1.Remove(0);

			EXPECT_EQ(vec1[0], "Vector1");
			EXPECT_EQ(vec1[1], "Vector2");

			EXPECT_EQ(vec1.GetSize(), 2);

			for (usize i = 0; i < vec1.GetSize(); ++i) {

				EXPECT_NE(vec1[i], vec0[i]);
			}

			Alt::StaticVector<Alt::String, 3> vec2 = vec1.Clone() iferr_return;

			vec1.Push("Vector3"_salloc) iferr_return;
			EXPECT_EQ(vec1.GetSize(), 3);

			vec1.MoveFrom(vec2) iferr_return;
			EXPECT_EQ(vec1.GetSize(), 2);
		}
	}

	{
		Alt::Array<Alt::String, 2> vec0{ "Vector0"_salloc, "Vector1"_salloc };

		{
			Alt::Array<Alt::String, 3> vec1;
			vec1.CloneFrom(vec0) iferr_return; // CloneFrom work: 3 <- 4

			const usize size = Alt::Num::Min(vec1.GetSize(), vec0.GetSize());
			for (usize i = 0; i < size; ++i) {

				EXPECT_EQ(vec1[i], vec0[i]);
			}

			Alt::Array<Alt::String, 3> vec2 = vec1.Clone() iferr_return;
			EXPECT_TRUE(vec2[2].IsEmpty());

			vec2[2] = "Vector2"_salloc;

			vec1.CloneFrom(vec2) iferr_return;
			EXPECT_TRUE(vec2[2].IsPopulated());
		}

		{
			Alt::Array<Alt::String, 3> vec1;
			vec1.MoveFrom(vec0.Clone().GetValueOrDefault()) iferr_return; // MoveFrom work: 3 <- 4

			const usize size = Alt::Num::Min(vec1.GetSize(), vec0.GetSize());
			for (usize i = 0; i < size; ++i) {

				EXPECT_EQ(vec1[i], vec0[i]);
			}

			Alt::Array<Alt::String, 3> vec2 = vec1.Clone() iferr_return;
			EXPECT_TRUE(vec2[2].IsEmpty());

			vec2[2] = "Vector2"_salloc;

			vec1.MoveFrom(vec2) iferr_return;
			EXPECT_TRUE(vec2[2].IsEmpty());
			EXPECT_TRUE(vec1[2].IsPopulated());
		}
	}

	{
		Alt::StaticVector<Alt::String, 3> vec1;
		Alt::StaticVector<Alt::String, 4> vec2;
		Alt::StaticVector<Alt::String, 2> vec3;

		vec1.CloneFrom(vec1) iferr_return;
		vec1.CloneFrom(vec3) iferr_return;
		vec1.CloneFrom(vec2) iferr_return;

		vec1.MoveFrom(vec1) iferr_return;
		vec1.MoveFrom(vec3) iferr_return;
		vec1.MoveFrom(vec2) iferr_return;
	}

	{
		Alt::Array<Alt::String, 3> vec1;
		Alt::Array<Alt::String, 4> vec2;
		Alt::Array<Alt::String, 2> vec3;

		vec1.CloneFrom(vec1) iferr_return;
		vec1.CloneFrom(vec3) iferr_return;
		//vec1.CloneFrom(vec2) iferr_return; // compilation error

		vec1.MoveFrom(vec1) iferr_return;
		vec1.MoveFrom(vec3) iferr_return;
		//vec1.MoveFrom(vec2) iferr_return; // compilation error
	}
}

TEST(StaticVector, AppendCloneAndAppendMove) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StaticVector<Alt::String, 6> vec0;
	vec0.Push("Vector0"_salloc) iferr_return;
	vec0.Push("Vector1"_salloc) iferr_return;

	{
		Alt::StaticVector<Alt::String, 4> vec1;
		vec1.AppendClone(vec0) iferr_return;

		EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_EQ(vec1[i], vec0[i]);
		}

		vec1.Push("Vector2"_salloc) iferr_return;
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
		Alt::StaticVector<Alt::String, 4> vec1;
		vec1.AppendMove(vec0.Clone().GetValueOrDefault()) iferr_return;

		EXPECT_EQ(vec1.GetSize(), vec0.GetSize());

		for (usize i = 0; i < vec1.GetSize(); ++i) {

			EXPECT_EQ(vec1[i], vec0[i]);
		}

		vec1.Push("Vector2"_salloc) iferr_return;
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

TEST(StaticVector, Insert) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize NUM_INSERTS = 64;

	Alt::StaticVector<usize, NUM_INSERTS> vec;
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

TEST(StaticVector, Rule5Tracking) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticVector<Rule5TrackerInlined, 16> vec = Alt::StaticVector<Rule5TrackerInlined, 16>::Create<5>();

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
		Alt::StaticVector<Rule5TrackerInlinedTriviallyReallocatable, 16> vec =
			Alt::StaticVector<Rule5TrackerInlinedTriviallyReallocatable, 16>::Create<5>();

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

		Alt::StaticVector<Rule5Tracker, 16> vec0 = Alt::StaticVector<Rule5Tracker, 16>::Create<5>(track);
		Alt::StaticVector<Rule5Tracker, 16> vec1 = Alt::StaticVector<Rule5Tracker, 16>::Create<5>(track);

		vec0 = std::move(vec1);
		EXPECT_EQ(tracker.cntDestructor, 10); // destruct in '::Create' then in 'operator=(...)'

		Alt::StaticVector<Rule5Tracker, 16> vec2 = std::move(vec0);
		EXPECT_EQ(tracker.cntDestructor, 20);

		vec2 = {};
		EXPECT_EQ(tracker.cntDestructor, 25);
	}
}

TEST(StaticVector, PushPopClearRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::UniquePtr<Alt::StaticVector<Alt::String, SIZE>> vecPtr =
		Alt::UniquePtr<Alt::StaticVector<Alt::String, SIZE>>::AllocateObject() iferr_return;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		if (i % 2 == 0) {

			for (usize i = 0; i < SIZE; ++i) {

				Alt::String val = AllocateStr(1, 100);
				vecPtr->Push(std::move(val)) iferr_return;
			}
		}
		else {

			static_assert(SIZE % 2 == 0, "");
			for (usize i = 0; i < SIZE / 2; ++i) {

				vecPtr->Push(AllocateStr(1, 100), AllocateStr(1, 100)) iferr_return;
			}
		}

		EXPECT_EQ(vecPtr->GetSize(), SIZE);

		for (usize i = 0; i < SIZE / 2; ++i) {

			vecPtr->Pop();
		}

		EXPECT_TRUE(vecPtr->IsPopulated());
		EXPECT_EQ(vecPtr->GetSize(), SIZE - SIZE / 2);

		vecPtr->Clear();
		EXPECT_TRUE(vecPtr->IsEmpty());
		EXPECT_EQ(vecPtr->GetSize(), 0);
	}
}

TEST(StaticVector, PushRemoveSwapRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Vector<Alt::String> stdvec;

	Alt::UniquePtr<Alt::StaticVector<Alt::String, SIZE>> vecPtr =
		Alt::UniquePtr<Alt::StaticVector<Alt::String, SIZE>>::AllocateObject() iferr_return;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			vecPtr->Push(val.Clone().GetValueOrDefault()) iferr_return;
			stdvec.Push(val.Clone().GetValueOrDefault()) iferr_return;
		}

		for (usize i = 0; i < SIZE; ++i) {

			const usize idToRemove = GetNum<usize>(0, vecPtr->GetSize() - 1);

			EXPECT_EQ(stdvec[idToRemove], (*vecPtr)[idToRemove]);

			Alt::String res0 = stdvec.RemoveSwapLast(idToRemove);
			Alt::String res1 = vecPtr->RemoveSwapLast(idToRemove);

			EXPECT_EQ(res0, res1);
		}

		EXPECT_EQ(vecPtr->GetSize(), stdvec.GetSize());
	}
}

TEST(StaticVector, PushRemoveRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Vector<Alt::String> stdvec;

	Alt::UniquePtr<Alt::StaticVector<Alt::String, SIZE>> vecPtr =
		Alt::UniquePtr<Alt::StaticVector<Alt::String, SIZE>>::AllocateObject() iferr_return;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			vecPtr->Push(val.Clone().GetValueOrDefault()) iferr_return;
			stdvec.Push(val.Clone().GetValueOrDefault()) iferr_return;
		}

		for (usize i = 0; i < SIZE; ++i) {

			const usize idToRemove = GetNum<usize>(0, vecPtr->GetSize() - 1);

			EXPECT_EQ(stdvec[idToRemove], (*vecPtr)[idToRemove]);

			Alt::String res1 = vecPtr->Remove(idToRemove);
			EXPECT_EQ(stdvec[idToRemove], res1);

			stdvec.Remove(idToRemove);
		}

		EXPECT_EQ(vecPtr->GetSize(), stdvec.GetSize());
	}
}

TEST(StaticVector, PushIteratorClearRepeat) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize NUM_ITERS = 10;
	constexpr usize SIZE = 1'000;

	Alt::Vector<Alt::String> stdvec;

	Alt::UniquePtr<Alt::StaticVector<Alt::String, SIZE>> vecPtr =
		Alt::UniquePtr<Alt::StaticVector<Alt::String, SIZE>>::AllocateObject() iferr_return;

	for (usize i = 0; i < NUM_ITERS; ++i) {

		for (usize i = 0; i < SIZE; ++i) {

			const Alt::String val = AllocateStr(1, 100);

			vecPtr->Push(val.Clone().GetValueOrDefault()) iferr_return;
			stdvec.Push(val.Clone().GetValueOrDefault()) iferr_return;

			EXPECT_EQ(stdvec[i], (*vecPtr)[i]);
		}

		{
			usize i = 0;
			Alt::VectorIterator<Alt::String> it = vecPtr->GetIterator();
			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdvec[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::VectorIterator<Alt::String> it = vecPtr->GetIterator();

			usize i = SIZE / 2;
			it.Advance(SIZE / 2);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdvec[i], *pVal);
				i += 1;
			}
		}

		{
			Alt::VectorIterator<Alt::String> it = vecPtr->GetIterator();

			usize i = SIZE;
			it.Advance(SIZE);

			while (Alt::String* pVal = it.Next()) {

				EXPECT_EQ(stdvec[i], *pVal);
				i += 1;
			}
		}

		EXPECT_EQ(vecPtr->GetSize(), SIZE);

		vecPtr->Clear();
		stdvec.Clear();
		EXPECT_TRUE(vecPtr->IsEmpty());
	}
}

#pragma warning(pop)