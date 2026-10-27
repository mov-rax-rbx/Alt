#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Utils/Pair.h"

#include "../alt-std/src/String/String.h"
#include "../alt-std/src/Collections/Vector.h"

#pragma warning(push)
#pragma warning(disable: 4624 /* destructor was implicitly defined as deleted */)

TEST(Pair, TypeTraits) {

	bool v = false;

	{
		v = Alt::TypeTraits::is_pair_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_pair_v<Alt::Pair<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_pair_v<Rule5AllTrivial>;
		EXPECT_FALSE(v);
	}

	{
		v = Alt::TypeTraits::is_clonable_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Pair<Alt::Pair<Rule5AllTrivial, Alt::Vector<Rule5AllTrivial>>, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Pair<Alt::Vector<Rule5AllTrivial>, Rule5AllTrivial>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_copyable_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Pair<Rule5AllNonTrivial&, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllNonTrivial&>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Pair<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Pair<Rule5AllTrivial, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Pair<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Pair<Rule5AllTrivial, Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>>;
		EXPECT_TRUE(v);
	}

	{
		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Pair<Rule5AllNonTrivial&, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllNonTrivial&>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Pair<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Pair<Rule5AllTrivial, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Pair<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Pair<Rule5AllTrivial, Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_default_constructible_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_default_constructible_v<Alt::Pair<Rule5AllTrivial&, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Pair<Rule5AllTrivial, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_constructible_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_constructible_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_copy_constructible_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_constructible_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_copy_assignable_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_assignable_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_constructible_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_constructible_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_assignable_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_assignable_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_destructible_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_destructible_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_destructible_v<Alt::Pair<bool, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Pair<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Pair<Rule5AllNonTrivial, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Pair<Rule5AllDeleted, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial>>;
		EXPECT_TRUE(v);
	}
}

TEST(Pair, Pair) {

	const auto pair0 = Alt::MakePair(Rule5AllTrivial{}, Rule5AllNonTrivial{});
	const Alt::Pair<Rule5AllTrivial, Rule5AllNonTrivial> pair1 = Alt::MakePair(Rule5AllTrivial{}, Rule5AllNonTrivial{});

	const bool v = std::is_same_v<decltype(pair0), decltype(pair1)>;
	EXPECT_TRUE(v);

	Alt::Pair<usize, usize> pair2 = Alt::MakePair(4, 8);

	EXPECT_EQ(pair2.f(), 4);
	EXPECT_EQ(pair2.first(), 4);

	EXPECT_EQ(pair2.s(), 8);
	EXPECT_EQ(pair2.second(), 8);

	Alt::Pair<Rule5AllDeletedExceptDestructor, Rule5AllTrivial> pair3{ Rule5_DefaultConstruct, {} };
	//pair3.f() = Rule5AllDeletedExceptDestructor{ Rule5_DefaultConstruct }; // compilation error
	pair3.s() = Rule5AllTrivial{};
}

TEST(Pair, PairRef) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Rule5AllTrivial x0{};
		Rule5AllTrivial x1{};

		Alt::Pair<Rule5AllTrivial&, Rule5AllTrivial&> xx0 = Alt::Pair<Alt::Ref<Rule5AllTrivial>, Alt::Ref<Rule5AllTrivial>>{ x0, x1 };
		Alt::Pair<Alt::Ref<Rule5AllTrivial>, Alt::Ref<Rule5AllTrivial>> xx1 = Alt::Pair<Rule5AllTrivial&, Rule5AllTrivial&>{ x0, x1 };

		xx0 = Alt::Pair<Alt::Ref<Rule5AllTrivial>, Alt::Ref<Rule5AllTrivial>>{ x0, x1 };
		xx1 = Alt::Pair<Rule5AllTrivial&, Rule5AllTrivial&>{ x0, x1 };

		xx0 = Alt::Pair<Rule5AllTrivial&, Rule5AllTrivial&>{ x0, x1 };
		xx1 = Alt::Pair<Alt::Ref<Rule5AllTrivial>, Alt::Ref<Rule5AllTrivial>>{ x0, x1 };

		Alt::Pair<Alt::Ref<Rule5AllTrivial>, Alt::Ref<Rule5AllTrivial>> xx2 = Alt::Pair<Alt::Ref<Rule5AllTrivial>, Alt::Ref<Rule5AllTrivial>>{ x0, x1 };
		Alt::Pair<Rule5AllTrivial&, Rule5AllTrivial&> xx3 = Alt::Pair<Rule5AllTrivial&, Rule5AllTrivial&>{ x0, x1 };

		xx2 = xx0;
		xx3 = xx1;

		xx2 = std::move(xx0);
		xx3 = std::move(xx1);
	}

	{
		Alt::String first = "First"_salloc;
		Alt::String second = "Second"_salloc;

		Alt::Pair<Alt::String&, Alt::String&> xx0{ first, second };

		// just copy pointers inside pair
		Alt::Pair<Alt::String&, Alt::String&> xx1 = std::move(xx0);

		EXPECT_TRUE(first.IsPopulated());
		EXPECT_TRUE(second.IsPopulated());

		EXPECT_EQ(xx1.f(), first);
		EXPECT_EQ(xx1.s(), second);

		// move!
		Alt::String first1 = std::move(xx1.f());
		Alt::String second1 = std::move(xx1.s());

		EXPECT_TRUE(first.IsEmpty());
		EXPECT_TRUE(second.IsEmpty());

		EXPECT_TRUE(xx1.f().IsEmpty());
		EXPECT_TRUE(xx1.s().IsEmpty());
	}

	{
		Alt::String first = "First"_salloc;

		Alt::Pair<Alt::String&, Rule5AllNonTrivial> xx0{ first, Rule5AllNonTrivial{} };

		// just copy pointers inside pair
		Alt::Pair<Alt::String&, Rule5AllNonTrivial> xx1 = std::move(xx0);

		EXPECT_TRUE(first.IsPopulated());

		EXPECT_EQ(xx1.f(), first);

		// move!
		Alt::String first1 = std::move(xx1.f());

		EXPECT_TRUE(first.IsEmpty());

		EXPECT_TRUE(xx1.f().IsEmpty());
	}

	{
		Alt::String second = "Second"_salloc;

		Alt::Pair<Rule5AllNonTrivial, Alt::String&> xx0{ Rule5AllNonTrivial{}, second };

		// just copy pointers inside pair
		Alt::Pair<Rule5AllNonTrivial, Alt::String&> xx1 = std::move(xx0);

		EXPECT_TRUE(second.IsPopulated());

		EXPECT_EQ(xx1.s(), second);

		// move!
		Alt::String second1 = std::move(xx1.s());

		EXPECT_TRUE(second.IsEmpty());

		EXPECT_TRUE(xx1.s().IsEmpty());
	}

	{
		Alt::String first = "First"_salloc;
		Alt::String second = "Second"_salloc;

		Alt::Pair<Alt::Ref<Alt::String>, Alt::Ref<Alt::String>> xx0{ first, second };

		// just copy pointers inside pair
		Alt::Pair<Alt::Ref<Alt::String>, Alt::Ref<Alt::String>> xx1 = std::move(xx0);

		EXPECT_TRUE(first.IsPopulated());
		EXPECT_TRUE(second.IsPopulated());

		EXPECT_EQ(*xx1.f(), first);
		EXPECT_EQ(*xx1.s(), second);

		// copy! (we are moving 'Alt::Ref')
		//Alt::String first1 = std::move(xx1.f());
		//Alt::String second1 = std::move(xx1.s());
		Alt::String first1 = xx1.f()->Clone() iferr_return;
		Alt::String second1 = xx1.s()->Clone() iferr_return;

		EXPECT_TRUE(first.IsPopulated());
		EXPECT_TRUE(second.IsPopulated());

		EXPECT_TRUE(xx1.f()->IsPopulated());
		EXPECT_TRUE(xx1.s()->IsPopulated());
	}

	{
		Alt::String first = "First"_salloc;

		Alt::Pair<Alt::Ref<Alt::String>, Rule5AllNonTrivial> xx0{ first, Rule5AllNonTrivial{} };

		// just copy pointers inside pair
		Alt::Pair<Alt::Ref<Alt::String>, Rule5AllNonTrivial> xx1 = std::move(xx0);

		EXPECT_TRUE(first.IsPopulated());

		EXPECT_EQ(*xx1.f(), first);

		// copy! (we are moving 'Alt::Ref')
		//Alt::String first1 = std::move(xx1.f());
		Alt::String first1 = xx1.f()->Clone() iferr_return;

		EXPECT_TRUE(first.IsPopulated());

		EXPECT_TRUE(xx1.f()->IsPopulated());
	}

	{
		Alt::String second = "Second"_salloc;

		Alt::Pair<Rule5AllNonTrivial, Alt::Ref<Alt::String>> xx0{ Rule5AllNonTrivial{}, second };

		// just copy pointers inside pair
		Alt::Pair<Rule5AllNonTrivial, Alt::Ref<Alt::String>> xx1 = std::move(xx0);

		EXPECT_TRUE(second.IsPopulated());

		EXPECT_EQ(*xx1.s(), second);

		// copy! (we are moving 'Alt::Ref')
		//Alt::String second1 = std::move(xx1.s());
		Alt::String second1 = xx1.s()->Clone() iferr_return;

		EXPECT_TRUE(second.IsPopulated());

		EXPECT_TRUE(xx1.s()->IsPopulated());
	}

	{
		Alt::String first = "First"_salloc;
		Alt::String second = "Second"_salloc;

		// for reference we return reference (move stuff)
		Alt::String& f = Alt::Pair<Alt::String&, Alt::String&>(first, second).f();
		Alt::String& s = Alt::Pair<Alt::String&, Alt::String&>(first, second).s();
		EXPECT_EQ(&f, &first);
		EXPECT_EQ(&s, &second);

		// now move for reference - move value
		Alt::String newFirst = std::move(Alt::Pair<Alt::String&, Alt::String&>(first, second).f());
		Alt::String newSecond = std::move(Alt::Pair<Alt::String&, Alt::String&>(first, second).s());

		EXPECT_TRUE(first.IsEmpty());
		EXPECT_TRUE(second.IsEmpty());
	}
}

TEST(Pair, Clone) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Pair<Alt::Vector<usize, 0>, usize> pair0{ Alt::Vector<usize, 2>::AllocatePush(0, 1).GetValueOrDefault(), 0 };
	Alt::Pair<Alt::Vector<usize, 1>, usize> pair1 = std::move(pair0);

	EXPECT_TRUE(pair0.f().IsEmpty());
	EXPECT_TRUE(pair1.f().IsPopulated());

	pair0.CloneFrom(pair1) iferr_return;

	EXPECT_TRUE(pair0.f().IsPopulated());
	EXPECT_TRUE(pair1.f().IsPopulated());

	EXPECT_EQ(pair0.f(), pair1.f());

	Alt::Pair<Alt::Vector<usize, 1>, usize> pair2;
	pair2 = pair1.Clone() iferr_return;

	EXPECT_EQ(pair1.f(), pair2.f());
}

TEST(Pair, StructuredBinding) {

	Alt::Pair<usize, Alt::String> pair{ 0, "String"_salloc };

	{
		auto& [x, str] = pair;
		EXPECT_EQ(x, pair.f());
		EXPECT_EQ(str, pair.second());
		EXPECT_EQ(str.Data(), pair.s().Data());
	}

	{
		auto [x, str] = std::move(pair);
		EXPECT_EQ(x, 0);
		EXPECT_EQ(str, "String");
		EXPECT_TRUE(pair.s().IsEmpty());
	}
}

#pragma warning(pop)