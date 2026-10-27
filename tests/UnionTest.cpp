#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Utils/Pair.h"
#include "../alt-std/src/Utils/Union.h"
#include "../alt-std/src/String/String.h"
#include "../alt-std/src/Collections/Vector.h"

#pragma warning(push)
#pragma warning(disable: 4624 /* destructor was implicitly defined as deleted */)

TEST(Union, TypeTraits) {

	bool v = false;

	{
		v = Alt::TypeTraits::is_union_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_union_v<Alt::Union<Rule5AllTrivial, f32>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_union_v<Rule5AllTrivial>;
		EXPECT_FALSE(v);
	}

	{
		v = Alt::TypeTraits::is_clonable_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Union<Alt::Vector<Rule5AllTrivial>>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Union<Alt::Vector<Rule5AllTrivial>, Rule5AllTrivial, Alt::Union<Alt::Vector<Rule5AllNonTrivial>>>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_copyable_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Union<usize>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Union<usize, f32>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Union<usize, f32, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Union<usize, f32, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Union<usize, f32, Alt::Ref<Rule5AllNonTrivial>>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Union<usize, Alt::Union<f32, f64>>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Union<usize, Alt::Union<f32, f64, Rule5AllNonTrivial>>>;
		EXPECT_FALSE(v);
	}

	{
		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Union<usize>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Union<usize, f32>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Union<usize, f32, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Union<usize, f32, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Union<usize, Alt::Union<f32, f64>>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Union<usize, Alt::Union<f32, f64, Rule5AllNonTrivial>>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_default_constructible_v<Alt::Union<>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_constructible_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_constructible_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_copy_constructible_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_constructible_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_copy_assignable_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_assignable_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_constructible_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_constructible_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_assignable_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_assignable_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_destructible_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_destructible_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_destructible_v<Alt::Union<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Union<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Union<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Union<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Union<Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);
	}
}

TEST(Union, MemoryLayout) {

	struct Storage0 {
		usize val;
		u8 tag;
	};

	struct alignas(alignof(usize)) Storage1 {
		Alt::Pair<f32, f32> val;
		u8 tag;
	};

	struct Storage2 {
		Alt::Pair<usize, usize> val;
		u8 tag;
	};

	EXPECT_EQ(sizeof(Alt::Union<>), 1);
	EXPECT_EQ(sizeof(Alt::Union<usize>), sizeof(Storage0));
	EXPECT_EQ(sizeof(Alt::Union<usize, usize>), sizeof(Storage0));
	EXPECT_EQ(sizeof(Alt::Union<usize, usize, usize>), sizeof(Storage0));
	EXPECT_EQ(sizeof(Alt::Union<usize, usize, Alt::Pair<f32, f32>>), sizeof(Storage1));
	EXPECT_EQ(sizeof(Alt::Union<usize, usize, Alt::Pair<usize, usize>>), sizeof(Storage2));
}

TEST(Union, InnerValueSizeOf) {

	EXPECT_EQ(Alt::Union<>::MaxInnerValueSizeOf(), sizeof(Alt::Union<>));
	EXPECT_EQ(Alt::Union<>::MaxInnerValueAlignOf(), alignof(Alt::Union<>));

	EXPECT_EQ(Alt::Union<usize>::MaxInnerValueSizeOf(), sizeof(usize));
	EXPECT_EQ(Alt::Union<usize>::MaxInnerValueAlignOf(), alignof(usize));

	usize sizeof_ = Alt::Union<usize, usize, usize>::MaxInnerValueSizeOf();
	usize alignof_ = Alt::Union<usize, usize, usize>::MaxInnerValueAlignOf();
	EXPECT_EQ(sizeof_, sizeof(usize));
	EXPECT_EQ(alignof_, alignof(usize));

	sizeof_ = Alt::Union<usize, usize, usize, Alt::Pair<usize, usize>>::MaxInnerValueSizeOf();
	alignof_ = Alt::Union<usize, usize, usize, Alt::Pair<usize, usize>>::MaxInnerValueAlignOf();
	EXPECT_EQ(sizeof_, sizeof(Alt::Pair<usize, usize>));
	EXPECT_EQ(alignof_, alignof(Alt::Pair<usize, usize>));
}

TEST(Union, Constructor) {

	{
		Alt::Union<> unEmpty{};
		Alt::Union<Rule5AllTrivial> result1{};
		Alt::Union<Rule5AllNonTrivial> result2{};
		Alt::Union<Rule5AllDeletedExceptDestructor> result3{};
	}

	{
		Alt::Union<f32> un = 5.0f;
		EXPECT_TRUE(un.Is<f32>());
	}

	{
		f32 num = 1.0f;
		Alt::Union<f32*> un = &num;
		EXPECT_TRUE(un.Is<f32*>());
		EXPECT_TRUE(un.Is<f32* const>());

		// pointer to pointer
		f32** pVal = un.Is<f32*>();
		EXPECT_TRUE(pVal);
	}

	Alt::Union<f32, f64> un = 5.0f;
	constexpr usize f32TypeIdx = Alt::Union<f32, f64>::GetTypeIdx<f32>();
	constexpr usize f64TypeIdx = Alt::Union<f32, f64>::GetTypeIdx<f64>();

	EXPECT_EQ(un.GetTypeIdx().GetValueOr(Alt::Num::Desc<usize>::Max()), f32TypeIdx);
	EXPECT_EQ(un.GetUnderlyingTypeIdx(), f32TypeIdx);
	EXPECT_TRUE(un.Is<const f32>());
	EXPECT_FALSE(un.Is<f64>());

	un = 6.0;
	EXPECT_EQ(un.GetTypeIdx().GetValueOr(Alt::Num::Desc<usize>::Max()), f64TypeIdx);
	EXPECT_EQ(un.GetUnderlyingTypeIdx(), f64TypeIdx);
	EXPECT_FALSE(un.Is<f32>());
	EXPECT_TRUE(un.Is<const f64>());
}

TEST(Union, CopyMoveConstructAssignment) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Union<f32, f64, Alt::String> un0 = 5.0f;
	EXPECT_TRUE(un0.Is<f32>());

	Alt::Union<f32, f64, Alt::String> un1 = "Union"_salloc;
	EXPECT_TRUE(un1.Is<Alt::String>());

	un0 = un1.Clone() iferr_return;
	EXPECT_TRUE(un0.Is<Alt::String>());
	EXPECT_EQ(*un0.Is<Alt::String>(), *un1.Is<Alt::String>());

	Alt::Union<f32, f64, Alt::String> un2 = un1.Clone() iferr_return;
	EXPECT_TRUE(un2.Is<Alt::String>());
	EXPECT_EQ(*un2.Is<Alt::String>(), *un1.Is<Alt::String>());

	Alt::Union<f32, f64, Alt::String> un3 = 2.0;
	EXPECT_TRUE(un3.Is<f64>());

	un3 = std::move(un1);
	EXPECT_TRUE(un1.Is<Alt::String>());
	EXPECT_TRUE(un3.Is<Alt::String>());
	EXPECT_EQ(*un3.Is<Alt::String>(), "Union");

	Alt::Union<f32, f64, Alt::String> un4 = std::move(un2);
	EXPECT_TRUE(un2.Is<Alt::String>());
	EXPECT_TRUE(un4.Is<Alt::String>());
	EXPECT_EQ(*un4.Is<Alt::String>(), "Union");

	un4 = Alt::Noval;
	EXPECT_TRUE(un4.GetTypeIdx().IsNone());
	EXPECT_EQ(un4.GetUnderlyingTypeIdx(), un4.GetTypeIdx<Alt::Noval_t>());

	un4 = Alt::Noval;
	EXPECT_TRUE(un4.GetTypeIdx().IsNone());
	EXPECT_EQ(un4.GetUnderlyingTypeIdx(), un4.GetTypeIdx<Alt::Noval_t>());

	Alt::Union<f32, f64, Alt::String> un5{ Alt::Noval };
	EXPECT_TRUE(un5.GetTypeIdx().IsNone());
	EXPECT_EQ(un5.GetUnderlyingTypeIdx(), un5.GetTypeIdx<Alt::Noval_t>());
}

TEST(Union, TypeCleaning) {

	s32 v0 = 4;
	const s32 v1 = 8;

	using Union = Alt::Union<s32*, const s32*>;

	constexpr usize idxA = Union::GetTypeIdx<s32*>();
	constexpr usize idxB = Union::GetTypeIdx<const s32*>();
	EXPECT_NE(idxA, idxB);

	Union unSame;

	unSame = &v0;
	EXPECT_TRUE(unSame.Is<s32*>());
	EXPECT_EQ(*unSame.Is<s32*>(), &v0);

	unSame = &v1;
	EXPECT_TRUE(unSame.Is<const s32*>());
	EXPECT_EQ(*unSame.Is<const s32*>(), &v1);

	using MyType = s32;
	Alt::Union<MyType, f32> un;

	un = 4.5f;

	EXPECT_FALSE(un.Is<const MyType>());
	EXPECT_FALSE(un.Is<volatile MyType>());
	EXPECT_FALSE(un.Is<const volatile MyType>());

	un = 4;

	EXPECT_TRUE(un.Is<const MyType>());
	EXPECT_TRUE(un.Is<volatile MyType>());
	EXPECT_TRUE(un.Is<const volatile MyType>());
}

TEST(Union, Reference) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		usize usizevalref_ = 8;
		usize sizevalaltref_ = 16;

		usize& usizevalref = usizevalref_;
		Alt::Ref<usize> usizevalaltref = sizevalaltref_;
		Alt::String string = "Union"_salloc;

		Alt::Union<usize, usize&, Alt::String&, Alt::Ref<usize>, Alt::String> un;
		EXPECT_EQ(un.GetNumTypes(), 5);

		// explicit move, because we also storing 'usize&'
		un = (usize)4;
		EXPECT_TRUE(un.Is<usize>());
		EXPECT_EQ(*un.Is<usize>(), 4);

		un = usizevalref;
		EXPECT_TRUE(un.Is<usize&>());
		EXPECT_EQ(*un.Is<usize&>(), usizevalref);

		un = usizevalaltref;
		EXPECT_TRUE(un.Is<Alt::Ref<usize>>());
		EXPECT_EQ(*un.Is<Alt::Ref<usize>>(), usizevalaltref);

		un = string;
		EXPECT_TRUE(un.Is<Alt::String&>());
		EXPECT_EQ(*un.Is<Alt::String&>(), string);

		// explicit copy, because we also storing 'Alt::String&'
		un = string.Clone() iferr_return;
		EXPECT_TRUE(un.Is<Alt::String>());
		EXPECT_EQ(*un.Is<Alt::String>(), string);
	}

	{
		usize usizeval = 4;
		Alt::String string = "Union"_salloc;

		Alt::Union<usize, Alt::String> un;

		un = usizeval;
		EXPECT_TRUE(un.Is<usize>());
		EXPECT_EQ(*un.Is<usize>(), usizeval);

		un = string.Clone() iferr_return;
		EXPECT_TRUE(un.Is<Alt::String>());
		EXPECT_EQ(*un.Is<Alt::String>(), string);
	}
}

TEST(Union, UnionInUnion) {

	using Inner = Alt::Union<s32, f32>;

	Alt::Union<Inner, f64> un;
	un = Inner(4.0f);

	ASSERT_TRUE(un.Is<Inner>());
	ASSERT_TRUE(un.Is<Inner>()->Is<f32>());
	EXPECT_EQ(*un.Is<Inner>()->Is<f32>(), 4.0f);

	un = 4.0;
	EXPECT_TRUE(un.Is<f64>());
	EXPECT_FALSE(un.Is<Inner>());
}

TEST(Union, NovalReset) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Union<Alt::String, s32> un;

		un = "Union"_salloc;
		EXPECT_TRUE(un.Is<Alt::String>());

		un = Alt::Noval;
		EXPECT_TRUE(un.GetTypeIdx().IsNone());
		EXPECT_EQ(un.GetUnderlyingTypeIdx(), un.GetTypeIdx<Alt::Noval_t>());
		EXPECT_FALSE(un.Is<Alt::String>());

		un = 4;
		EXPECT_TRUE(un.Is<s32>());
		EXPECT_EQ(*un.Is<s32>(), 4);

		un = Alt::Noval;
		EXPECT_TRUE(un.GetTypeIdx().IsNone());
		EXPECT_EQ(un.GetUnderlyingTypeIdx(), un.GetTypeIdx<Alt::Noval_t>());
		EXPECT_FALSE(un.Is<s32>());
	}

	{
		Alt::Union<Alt::Noval_t, Alt::String, s32> un;

		un = "Union"_salloc;
		EXPECT_TRUE(un.Is<Alt::String>());

		un = Alt::Noval;
		EXPECT_TRUE(un.GetTypeIdx().IsNone());
		EXPECT_EQ(un.GetUnderlyingTypeIdx(), un.GetTypeIdx<Alt::Noval_t>());
		EXPECT_FALSE(un.Is<Alt::String>());

		un = 4;
		EXPECT_TRUE(un.Is<s32>());
		EXPECT_EQ(*un.Is<s32>(), 4);

		un = Alt::Noval;
		EXPECT_TRUE(un.GetTypeIdx().IsNone());
		EXPECT_EQ(un.GetUnderlyingTypeIdx(), un.GetTypeIdx<Alt::Noval_t>());
		EXPECT_FALSE(un.Is<s32>());
	}
}

TEST(Union, Rule5Tracking) {

	Rule5Tracker::Core tracker{};
	Rule5Tracker track{ tracker };

	{
		using Union = Alt::Union<Rule5Tracker, Rule5AllNonTrivial, Rule5AllTrivial>;

		Union un0 = track; 

		EXPECT_TRUE(un0.GetTypeIdx().IsPopulated());
		EXPECT_EQ(tracker.cntCopyConstruct, 1);

		Union un1 = std::move(un0);

		// un0 has value - like with std::variant
		EXPECT_TRUE(un0.GetTypeIdx().IsPopulated());

		EXPECT_TRUE(un1.GetTypeIdx().IsPopulated());

		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 0);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);

		un0 = std::move(un1);

		EXPECT_TRUE(un0.GetTypeIdx().IsPopulated());

		// un1 has value - like with std::variant
		EXPECT_TRUE(un1.GetTypeIdx().IsPopulated());

		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 1);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);

		un1 = un0;
		EXPECT_TRUE(un0.GetTypeIdx().IsPopulated());
		EXPECT_TRUE(un1.GetTypeIdx().IsPopulated());

		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 1);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 1);

		un1 = Alt::Noval;
	}

	EXPECT_EQ(tracker.cntDestructor, 2);
}

TEST(Union, Clone) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Union<Alt::Vector<usize, 0>, usize> un0 = Alt::Vector<usize, 0>::AllocatePush(0, 1) iferr_return;
	Alt::Union<Alt::Vector<usize, 1>, usize> un1;
	un1 = std::move(un0);

	Alt::Vector<usize, 0>* pVal0 = un0.Is<Alt::Vector<usize, 0>>();
	Alt::Vector<usize, 1>* pVal1 = un1.Is<Alt::Vector<usize, 1>>();
	//Alt::Vector<usize, 1>* pVal2 = un1.Is<Alt::Vector<usize, 0>>(); // compilation error

	EXPECT_TRUE(pVal0);
	EXPECT_TRUE(pVal0->IsEmpty());
	EXPECT_TRUE(pVal1);
	EXPECT_TRUE(pVal1->IsPopulated());

	un0 = (usize)4;

	un0.CloneFrom(un1) iferr_return;
	const bool isEq = *un0.Is<Alt::Vector<usize, 0>>() == *un1.Is<Alt::Vector<usize, 1>>();

	EXPECT_TRUE(pVal0->IsPopulated());
	EXPECT_TRUE(pVal1->IsPopulated());

	EXPECT_TRUE(isEq);

	//Alt::Union<Alt::Vector<usize, 2>, usize> un2 = un1; // compilation error
	//Alt::Union<usize, Alt::Vector<usize, 2>> un2 = std::move(un1); // compilation error

	un1 = Alt::Vector<usize, 1>::AllocatePush(1, 1) iferr_return;

	EXPECT_TRUE(un1.GetTypeIdx());

	constexpr usize typeIdx = Alt::Union<Alt::Vector<usize, 1>, usize>::GetTypeIdx<Alt::Vector<usize, 1>>();
	EXPECT_EQ(*un1.GetTypeIdx(), typeIdx);

	Alt::Vector<usize, 1>* pVal = un1.Is<Alt::Vector<usize, 1>>();
	EXPECT_TRUE(pVal);

	Alt::Union<Alt::Vector<usize, 4>, usize> un2 = un1.Clone() iferr_return;

	Alt::Vector<usize, 4>* pVal2 = un2.Is<Alt::Vector<usize, 4>>();
	EXPECT_EQ(*pVal1, *pVal2);
}

#pragma warning(pop)