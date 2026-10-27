#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/IfErr.h"
#include "../alt-std/src/Memory/MaybeUninit.h"
#include "../alt-std/src/Collections/Vector.h"

#pragma warning(push)
#pragma warning(disable: 4624 /* destructor was implicitly defined as deleted */)

TEST(MaybeUninit, TypeTraits) {

	bool v = false;

	{
		v = std::is_default_constructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_default_constructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_default_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, void>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_default_constructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_default_constructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_default_constructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_default_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_copyable_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);
	}

	{
		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_constructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_constructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_constructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_constructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_constructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_constructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_constructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_copy_constructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_constructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_copy_assignable_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_assignable_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_constructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_constructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_assignable_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_assignable_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_destructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_destructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_destructible_v<Alt::MaybeUninit<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::MaybeUninit<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::MaybeUninit<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::MaybeUninitOneOf<>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::MaybeUninitOneOf<void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::MaybeUninitOneOf<Rule5AllTrivial, Rule5AllNonTrivial, Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);
	}
}

TEST(MaybeUninit, DefaultUsage) {

	iferr_scope_on_err {
		FAIL();
	};

	{
		Alt::MaybeUninit<usize> opt;
		opt.Construct(42);

		EXPECT_EQ(opt.GetVal(), 42);

		opt.Destroy();
	}

	{
	    Alt::MaybeUninitOneOf<Alt::Vector<usize>, Alt::String> opt;

		Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3) iferr_return;
	    opt.Construct<Alt::Vector<usize>>(vec.Clone().GetValueOrDefault());

		Alt::Vector<usize>& uVec = opt.GetVal<Alt::Vector<usize>>();
	    EXPECT_EQ(uVec[0], vec[0]);
	    EXPECT_EQ(uVec[1], vec[1]);
	    EXPECT_EQ(uVec[2], vec[2]);

		uVec[0] = 4;
		Alt::Vector<usize>& uVec1 = opt.GetVal<Alt::Vector<usize>>();
	    EXPECT_EQ(uVec1[0], uVec[0]);
	    EXPECT_NE(uVec1[0], vec[0]);

	    opt.Destroy<Alt::Vector<usize>>();

		Alt::String str = "MaybeUninit"_salloc;
		opt.Construct<Alt::String>(str.Clone().GetValueOrDefault());

		Alt::String& uStr = opt.GetVal<Alt::String>();
		EXPECT_EQ(uStr, str);

		opt.Destroy<Alt::String>();
	}
}

TEST(MaybeUninit, Rule5Tracking) {

	iferr_scope_on_err {
		FAIL();
	};

	{
		Rule5Tracker::Core tracker{};
		Rule5Tracker track{ tracker };

		Alt::MaybeUninit<Rule5Tracker> opt;
		opt.Construct(track);

		EXPECT_EQ(tracker.cntMoveConstruct, 0);
		EXPECT_EQ(tracker.cntMoveAssignment, 0);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);
		EXPECT_EQ(tracker.cntDestructor, 0);

		//Alt::MaybeUninit<Rule5Tracker> opt1 = opt; // compilation error
		//Alt::MaybeUninit<Rule5Tracker> opt2 = std::move(opt); // compilation error

		Rule5Tracker val = std::move(opt.GetVal());
		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 0);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);
		EXPECT_EQ(tracker.cntDestructor, 0);

		opt.Destroy();

		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 0);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);
		EXPECT_EQ(tracker.cntDestructor, 1);
	}

	{
		Rule5Tracker::Core tracker{};
		Rule5Tracker track{ tracker };

	    Alt::MaybeUninitOneOf<Rule5Tracker, Alt::String> opt;

	    opt.Construct<Rule5Tracker>(track);

		EXPECT_EQ(tracker.cntMoveConstruct, 0);
		EXPECT_EQ(tracker.cntMoveAssignment, 0);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);
		EXPECT_EQ(tracker.cntDestructor, 0);

		//Alt::MaybeUninitOneOf<Rule5Tracker> opt1 = opt; // compilation error
		//Alt::MaybeUninitOneOf<Rule5Tracker> opt2 = std::move(opt); // compilation error

		Rule5Tracker val = std::move(opt.GetVal<Rule5Tracker>());
		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 0);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);
		EXPECT_EQ(tracker.cntDestructor, 0);

	    opt.Destroy<Rule5Tracker>();

		Alt::String str = "MaybeUninit"_salloc;
		opt.Construct<Alt::String>(str.Clone().GetValueOrDefault());

		Alt::String& uStr = opt.GetVal<Alt::String>();
		EXPECT_EQ(uStr, str);

		opt.Destroy<Alt::String>();

		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 0);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);
		EXPECT_EQ(tracker.cntDestructor, 1);

	}
}

#pragma warning(pop)