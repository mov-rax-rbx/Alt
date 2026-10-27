#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Utils/Result.h"
#include "../alt-std/src/Memory/UniquePtr.h"

#include "../alt-std/src/String/String.h"
#include "../alt-std/src/Collections/Vector.h"

#pragma warning(push)
#pragma warning(disable: 4624 /* destructor was implicitly defined as deleted */)
#pragma warning(disable: 4244 /* conversion from ... to ..., possible loss of data */)

TEST(Result, TypeTraits) {

	bool v = false;

	{
		v = Alt::TypeTraits::is_result_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_result_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_result_v<Alt::Result<void, Rule5AllTrivial&>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_result_v<Alt::Result<Rule5AllTrivial&, void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_result_v<Alt::Result<Rule5AllTrivial&, Rule5AllTrivial&>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_result_v<Alt::Result<Alt::Result<Rule5AllTrivial, void>, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_result_v<Alt::Result<Rule5AllTrivial*, void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_result_v<Alt::Result<Alt::Result<Rule5AllTrivial*, Rule5AllTrivial>, void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_result_v<Rule5AllTrivial>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copyable_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Rule5AllTrivial&, Rule5AllTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<void, Rule5AllTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Rule5AllTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Alt::String, Alt::String>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Result<void, Alt::String>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Alt::String, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Alt::String&, Alt::String&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<void, Alt::String&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Alt::String&, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Alt::Result<void, Rule5AllTrivial>, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<void, Alt::Result<void, Rule5AllTrivial>>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Result<Alt::Result<void, Rule5AllTrivial>, void>>;
		EXPECT_TRUE(v);
	}

	{
		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<Alt::String, Alt::String>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<void, Alt::String>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<Alt::String, void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<Alt::String&, Alt::String&>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<void, Alt::String&>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<Alt::String&, void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<Alt::Result<void, Rule5AllTrivial>, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<void, Alt::Result<void, Rule5AllTrivial>>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Result<Alt::Result<void, Rule5AllTrivial>, void>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_default_constructible_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_FALSE(v); // we need to assign nullptr

		v = std::is_trivially_default_constructible_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_FALSE(v); // we need to assign nullptr

		v = std::is_trivially_default_constructible_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_FALSE(v); // we need to assign nullptr
	}

	{
		v = std::is_constructible_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_FALSE(v);

		v = std::is_constructible_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_constructible_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_constructible_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_copy_constructible_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_constructible_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_copy_assignable_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_assignable_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_constructible_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_constructible_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_assignable_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_assignable_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_destructible_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		 v = std::is_destructible_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_destructible_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_destructible_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_destructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_destructible_v<Alt::Result<void, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Result<Rule5AllTrivial, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Result<void, Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		 v = std::is_trivially_destructible_v<Alt::Result<Rule5AllTrivial, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Result<Rule5AllNonTrivial, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Result<void, Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Result<Rule5AllNonTrivial, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Result<Rule5AllNonTrivial&, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Result<void, Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Result<Rule5AllNonTrivial&, void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Result<Rule5AllDeleted, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Result<void, Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Result<Rule5AllDeleted, void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Result<void, Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Result<Rule5AllDeletedExceptDestructor, void>>;
		EXPECT_TRUE(v);
	}
}

TEST(Result, Construction) {

	{
		Alt::Result<Rule5AllTrivial, void> result0{};
		Alt::Result<Rule5AllNonTrivial, void> result1{};
		Alt::Result<Rule5AllDeletedExceptDestructor, void> result2{};
	}

	{
		Alt::Result<usize, Alt::String> result(2);
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result, 2);
		EXPECT_EQ(result.GetOk(), 2);

		EXPECT_EQ(result, Alt::Ok);
		EXPECT_NE(result, Alt::Err);
	}

	{
		Alt::Result<usize, Alt::String> result = Alt::Result<usize, Alt::String>::Ok(2);
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result, 2);
		EXPECT_EQ(result.GetOk(), 2);

		EXPECT_EQ(result, Alt::Ok);
		EXPECT_NE(result, Alt::Err);
	}

	{
		Alt::Result<usize, Alt::String> result("Error"_salloc);
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());
		EXPECT_EQ(result.GetErr(), "Error");

		EXPECT_NE(result, Alt::Ok);
		EXPECT_EQ(result, Alt::Err);
	}

	{
		Alt::Result<usize, Alt::String> result = Alt::Result<usize, Alt::String>::Err("Error"_salloc);
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());
		EXPECT_EQ(result.GetErr(), "Error");

		EXPECT_NE(result, Alt::Ok);
		EXPECT_EQ(result, Alt::Err);
	}

	{
		Alt::Result<usize, Alt::String> result = Alt::ResultErr<Alt::String>{ "Error"_salloc };
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());
		EXPECT_EQ(result.GetErr(), "Error");

		EXPECT_NE(result, Alt::Ok);
		EXPECT_EQ(result, Alt::Err);
	}

	{
		Alt::Result<void, Alt::String> result("Error"_salloc);
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());
		EXPECT_EQ(result.GetErr(), "Error");

		EXPECT_NE(result, Alt::Ok);
		EXPECT_EQ(result, Alt::Err);
	}

	{
		Alt::Result<void, Alt::String> result = Alt::Result<void, Alt::String>::Err("Error"_salloc);
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());
		EXPECT_EQ(result.GetErr(), "Error");

		EXPECT_NE(result, Alt::Ok);
		EXPECT_EQ(result, Alt::Err);
	}

	{
		Alt::Result<void, Alt::String> result = Alt::ResultErr<Alt::String>{ "Error"_salloc };
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());
		EXPECT_EQ(result.GetErr(), "Error");

		EXPECT_NE(result, Alt::Ok);
		EXPECT_EQ(result, Alt::Err);
	}

	{
		Alt::Result<Alt::String, Alt::String> result("Ok"_salloc);
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result, "Ok");
		EXPECT_EQ(result.GetOk(), "Ok");

		EXPECT_EQ(result, Alt::Ok);
		EXPECT_NE(result, Alt::Err);
	}

	{
		Alt::Result<Alt::String, Alt::String> result = Alt::Result<Alt::String, Alt::String>::Ok("Ok"_salloc);
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result, "Ok");
		EXPECT_EQ(result.GetOk(), "Ok");

		EXPECT_EQ(result, Alt::Ok);
		EXPECT_NE(result, Alt::Err);
	}

	{
		Alt::Result<Alt::String, Alt::String> result = Alt::Result<Alt::String, Alt::String>::Err("Error"_salloc);
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());
		EXPECT_EQ(result.GetErr(), "Error");

		EXPECT_NE(result, Alt::Ok);
		EXPECT_EQ(result, Alt::Err);
	}

	{
		Alt::Result<Alt::String, Alt::String> result = Alt::ResultErr<Alt::String>{ "Error"_salloc };
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());
		EXPECT_EQ(result.GetErr(), "Error");

		EXPECT_NE(result, Alt::Ok);
		EXPECT_EQ(result, Alt::Err);
	}

	{
		Alt::Result<void, void> empty = Alt::Ok;
		EXPECT_TRUE(empty);
		EXPECT_TRUE(empty.IsOk());

		EXPECT_EQ(empty, Alt::Ok);
		EXPECT_NE(empty, Alt::Err);
	}

	{
		Alt::Result<void, void> empty = Alt::Err;
		EXPECT_FALSE(empty);
		EXPECT_FALSE(empty.IsOk());

		EXPECT_NE(empty, Alt::Ok);
		EXPECT_EQ(empty, Alt::Err);
	}

	{
		Alt::Result<Alt::String, void> result = Alt::MakeResultOk(Alt::String("Ok"_salloc));
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result, "Ok");
		EXPECT_EQ(result.GetOk(), "Ok");

		EXPECT_EQ(result, Alt::Ok);
		EXPECT_NE(result, Alt::Err);
	}

	{
		Alt::Result<void, Alt::String> result = Alt::MakeResultErr(Alt::String("Ok"_salloc));
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());

		EXPECT_EQ(result.GetErr(), "Ok");

		EXPECT_NE(result, Alt::Ok);
		EXPECT_EQ(result, Alt::Err);
	}
}

TEST(Result, CopyMoveConstructAssignment) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Result<Alt::String, usize> result0("String"_salloc);
		Alt::Result<Alt::String, usize> result = result0.Clone() iferr_return;

		ASSERT_TRUE(result0);
		ASSERT_TRUE(result0.IsOk());
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result0, "String");
		EXPECT_EQ(*result, "String");

		Alt::Result<Alt::String, usize> copy = result.Clone() iferr_return;

		ASSERT_TRUE(copy);
		ASSERT_TRUE(copy.IsOk());

		EXPECT_EQ(*copy, "String");

		Alt::Result<Alt::String, usize> move = std::move(copy);

		ASSERT_TRUE(move);
		ASSERT_TRUE(move.IsOk());

		EXPECT_EQ(*move, "String");

		Alt::String strCopy = move->Clone() iferr_return;
		EXPECT_EQ(strCopy, "String");

		Alt::String strCopy1 = move.GetOk().Clone() iferr_return;
		EXPECT_EQ(strCopy1, "String");

		Alt::String strMove = std::move(*move);
		EXPECT_EQ(strMove, "String");

		Alt::String strMove1 = std::move(result.GetOk());
		EXPECT_EQ(strMove1, "String");

		result = Alt::ResultErr<usize>{ 2 };
		EXPECT_EQ(result.Clone().GetValueOrDefault().GetOkOr("Value"_salloc), "Value");

		Alt::String valueOrExec = std::move(result).GetOkOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Value");
	}

	{
		Alt::Result<usize, Alt::String> result0("String"_salloc);
		Alt::Result<usize, Alt::String> result = result0.Clone() iferr_return;

		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());

		EXPECT_EQ(result0.GetErr(), "String");
		EXPECT_EQ(result.GetErr(), "String");

		Alt::Result<usize, Alt::String> copy = result.Clone() iferr_return;

		EXPECT_FALSE(copy);
		EXPECT_FALSE(copy.IsOk());

		EXPECT_EQ(copy.GetErr(), "String");

		Alt::Result<usize, Alt::String> move = std::move(copy);

		EXPECT_FALSE(move);
		EXPECT_FALSE(move.IsOk());

		EXPECT_EQ(move.GetErr(), "String");

		Alt::String strCopy = move.GetErr().Clone() iferr_return;
		EXPECT_EQ(strCopy, "String");

		Alt::String strMove = std::move(move.GetErr());
		EXPECT_EQ(strMove, "String");
	}

	{
		Alt::Result<void, Alt::String> result0("String"_salloc);
		Alt::Result<void, Alt::String> result = result0.Clone() iferr_return;

		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());
		EXPECT_FALSE(result);
		EXPECT_FALSE(result.IsOk());

		EXPECT_EQ(result0.GetErr(), "String");
		EXPECT_EQ(result.GetErr(), "String");

		Alt::Result<void, Alt::String> copy = result.Clone() iferr_return;

		EXPECT_FALSE(copy);
		EXPECT_FALSE(copy.IsOk());

		EXPECT_EQ(copy.GetErr(), "String");

		Alt::Result<void, Alt::String> move = std::move(copy);

		EXPECT_FALSE(move);
		EXPECT_FALSE(move.IsOk());

		EXPECT_EQ(move.GetErr(), "String");

		Alt::String strCopy = move.GetErr().Clone() iferr_return;
		EXPECT_EQ(strCopy, "String");

		Alt::String strMove = std::move(move.GetErr());
		EXPECT_EQ(strMove, "String");

		result0 = Alt::Ok;
		EXPECT_TRUE(result0);

		result = result0.Clone() iferr_return;
		EXPECT_TRUE(result);

		result = std::move(result0);
		EXPECT_TRUE(result);
	}

	{
		Alt::Result<Alt::String, void> result0("String"_salloc);
		Alt::Result<Alt::String, void> result = result0.Clone() iferr_return;

		ASSERT_TRUE(result0);
		ASSERT_TRUE(result0.IsOk());
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result0, "String");
		EXPECT_EQ(result.GetOk(), "String");

		Alt::Result<Alt::String, void> copy = result.Clone() iferr_return;

		ASSERT_TRUE(copy);
		ASSERT_TRUE(copy.IsOk());

		EXPECT_EQ(*copy, "String");

		Alt::Result<Alt::String, void> move = std::move(copy);

		ASSERT_TRUE(move);
		ASSERT_TRUE(move.IsOk());

		EXPECT_EQ(*move, "String");

		Alt::String strCopy = move->Clone() iferr_return;
		EXPECT_EQ(strCopy, "String");

		Alt::String strMove = std::move(*move);
		EXPECT_EQ(strMove, "String");

		result0 = Alt::Err;
		EXPECT_FALSE(result0);

		result = result0.Clone() iferr_return;
		EXPECT_FALSE(result);

		result = std::move(result0);
		EXPECT_FALSE(result);

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetOkOr("Value"_salloc), "Value");

		Alt::String valueOrExec = std::move(result).GetOkOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Value");
	}
}

TEST(Result, GetOkOr) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Result<Alt::String, void> result;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetOkOr("Value"_salloc), "Value");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetOkOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Value");

		EXPECT_EQ(std::move(result).GetOkOrDefault(), "");
	}

	{
		Alt::Result<Alt::String, void> result = "Result"_salloc;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetOkOr("Value"_salloc), "Result");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetOkOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Result");

		EXPECT_EQ(std::move(result).GetOkOrDefault(), "Result");
	}

	{
		Alt::Result<Alt::String, usize> result;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetOkOr("Value"_salloc), "Value");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetOkOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Value");

		EXPECT_EQ(std::move(result).GetOkOrDefault(), "");
	}

	{
		Alt::Result<Alt::String, usize> result = "Result"_salloc;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetOkOr("Value"_salloc), "Result");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetOkOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Result");

		EXPECT_EQ(std::move(result).GetOkOrDefault(), "Result");
	}

	{
		Alt::String str = "Result"_salloc;
		Alt::Result<Alt::String*, void> result = &str;

		{
			Alt::String* pValOr = result.GetOkOr(nullptr);
			EXPECT_EQ(pValOr, &str);

			Alt::String* pValOrFunc = result.GetOkOrExec([]() { return nullptr; });
			EXPECT_EQ(pValOrFunc, &str);

			Alt::String* pValOrDefault = result.GetOkOrDefault();
			EXPECT_EQ(pValOrDefault, &str);

			if (Alt::String* pCastStr = result) {
			}
			else {
				FAIL();
			}
		}

		result = Alt::Err;

		{
			Alt::String* pValOr = result.GetOkOr(nullptr);
			EXPECT_FALSE(pValOr);

			Alt::String* pValOrFunc = result.GetOkOrExec([]() { return nullptr; });
			EXPECT_FALSE(pValOrFunc);

			Alt::String* pValOrDefault = result.GetOkOrDefault();
			EXPECT_FALSE(pValOrDefault);

			if (Alt::String* pCastStr = result) {
				FAIL();
			}
		}
	}

	{
		Alt::String str = "Result"_salloc;
		Alt::Result<Alt::String*, usize> result = &str;

		{
			Alt::String* pValOr = result.GetOkOr(nullptr);
			EXPECT_EQ(pValOr, &str);

			Alt::String* pValOrFunc = result.GetOkOrExec([]() { return nullptr; });
			EXPECT_EQ(pValOrFunc, &str);

			Alt::String* pValOrDefault = result.GetOkOrDefault();
			EXPECT_EQ(pValOrDefault, &str);

			if (Alt::String* pCastStr = result) {
			}
			else {
				FAIL();
			}
		}

		result = 4;

		{
			Alt::String* pValOr = result.GetOkOr(nullptr);
			EXPECT_FALSE(pValOr);

			Alt::String* pValOrFunc = result.GetOkOrExec([]() { return nullptr; });
			EXPECT_FALSE(pValOrFunc);

			Alt::String* pValOrDefault = result.GetOkOrDefault();
			EXPECT_FALSE(pValOrDefault);

			if (Alt::String* pCastStr = result) {
				FAIL();
			}
		}
	}

	{
		Alt::String str = "Optional"_salloc;

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<Alt::UniquePtr<Alt::String>, void> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetOkOr(nullptr);
			EXPECT_TRUE(pValOr);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<Alt::UniquePtr<Alt::String>, void> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetOkOrExec([]() { return nullptr; });
			EXPECT_TRUE(pValOrFunc);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<Alt::UniquePtr<Alt::String>, void> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetOkOrDefault();
			EXPECT_TRUE(pValOrDefault);
		}

		Alt::Result<Alt::UniquePtr<Alt::String>, void> result = Alt::Err;

		Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetOkOr(nullptr);
		EXPECT_FALSE(pValOr);

		Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetOkOrExec([]() { return nullptr; });
		EXPECT_FALSE(pValOrFunc);

		Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetOkOrDefault();
		EXPECT_FALSE(pValOrDefault);
	}

	{
		Alt::String str = "Optional"_salloc;

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<Alt::UniquePtr<Alt::String>, usize> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetOkOr(nullptr);
			EXPECT_TRUE(pValOr);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<Alt::UniquePtr<Alt::String>, usize> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetOkOrExec([]() { return nullptr; });
			EXPECT_TRUE(pValOrFunc);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<Alt::UniquePtr<Alt::String>, usize> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetOkOrDefault();
			EXPECT_TRUE(pValOrDefault);
		}

		Alt::Result<Alt::UniquePtr<Alt::String>, usize> result = 4;

		Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetOkOr(nullptr);
		EXPECT_FALSE(pValOr);

		Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetOkOrExec([]() { return nullptr; });
		EXPECT_FALSE(pValOrFunc);

		Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetOkOrDefault();
		EXPECT_FALSE(pValOrDefault);
	}
}

TEST(Result, GetErrOr) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Result<void, Alt::String> result;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetErrOr("Value"_salloc), "Value");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetErrOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Value");

		EXPECT_EQ(std::move(result).GetErrOrDefault(), "");
	}

	{
		Alt::Result<void, Alt::String> result = "Result"_salloc;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetErrOr("Value"_salloc), "Result");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetErrOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Result");

		EXPECT_EQ(std::move(result).GetErrOrDefault(), "Result");
	}

	{
		Alt::Result<usize, Alt::String> result = 4;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetErrOr("Value"_salloc), "Value");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetErrOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Value");

		EXPECT_EQ(std::move(result).GetErrOrDefault(), "");
	}

	{
		Alt::Result<usize, Alt::String> result = "Result"_salloc;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetErrOr("Value"_salloc), "Result");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetErrOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Result");

		EXPECT_EQ(std::move(result).GetErrOrDefault(), "Result");
	}

	{
		Alt::String str = "Result"_salloc;
		Alt::Result<void, Alt::String*> result = &str;

		{
			Alt::String* pValOr = result.GetErrOr(nullptr);
			EXPECT_EQ(pValOr, &str);

			Alt::String* pValOrFunc = result.GetErrOrExec([]() { return nullptr; });
			EXPECT_EQ(pValOrFunc, &str);

			Alt::String* pValOrDefault = result.GetErrOrDefault();
			EXPECT_EQ(pValOrDefault, &str);
		}

		result = Alt::Ok;

		{
			Alt::String* pValOr = result.GetErrOr(nullptr);
			EXPECT_FALSE(pValOr);

			Alt::String* pValOrFunc = result.GetErrOrExec([]() { return nullptr; });
			EXPECT_FALSE(pValOrFunc);

			Alt::String* pValOrDefault = result.GetErrOrDefault();
			EXPECT_FALSE(pValOrDefault);
		}
	}

	{
		Alt::String str = "Result"_salloc;
		Alt::Result<usize, Alt::String*> result = &str;

		{
			Alt::String* pValOr = result.GetErrOr(nullptr);
			EXPECT_EQ(pValOr, &str);

			Alt::String* pValOrFunc = result.GetErrOrExec([]() { return nullptr; });
			EXPECT_EQ(pValOrFunc, &str);

			Alt::String* pValOrDefault = result.GetErrOrDefault();
			EXPECT_EQ(pValOrDefault, &str);
		}

		result = 4;

		{
			Alt::String* pValOr = result.GetErrOr(nullptr);
			EXPECT_FALSE(pValOr);

			Alt::String* pValOrFunc = result.GetErrOrExec([]() { return nullptr; });
			EXPECT_FALSE(pValOrFunc);

			Alt::String* pValOrDefault = result.GetErrOrDefault();
			EXPECT_FALSE(pValOrDefault);
		}
	}

	{
		Alt::String str = "Optional"_salloc;

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<void, Alt::UniquePtr<Alt::String>> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetErrOr(nullptr);
			EXPECT_TRUE(pValOr);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<void, Alt::UniquePtr<Alt::String>> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetErrOrExec([]() { return nullptr; });
			EXPECT_TRUE(pValOrFunc);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<void, Alt::UniquePtr<Alt::String>> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetErrOrDefault();
			EXPECT_TRUE(pValOrDefault);
		}

		Alt::Result<void, Alt::UniquePtr<Alt::String>> result = Alt::Ok;

		Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetErrOr(nullptr);
		EXPECT_FALSE(pValOr);

		Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetErrOrExec([]() { return nullptr; });
		EXPECT_FALSE(pValOrFunc);

		Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetErrOrDefault();
		EXPECT_FALSE(pValOrDefault);
	}

	{
		Alt::String str = "Optional"_salloc;

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<usize, Alt::UniquePtr<Alt::String>> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetErrOr(nullptr);
			EXPECT_TRUE(pValOr);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<usize, Alt::UniquePtr<Alt::String>> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetErrOrExec([]() { return nullptr; });
			EXPECT_TRUE(pValOrFunc);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Result<usize, Alt::UniquePtr<Alt::String>> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0)) iferr_return;

			Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetErrOrDefault();
			EXPECT_TRUE(pValOrDefault);
		}

		Alt::Result<usize, Alt::UniquePtr<Alt::String>> result = 4;

		Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetErrOr(nullptr);
		EXPECT_FALSE(pValOr);

		Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetErrOrExec([]() { return nullptr; });
		EXPECT_FALSE(pValOrFunc);

		Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetErrOrDefault();
		EXPECT_FALSE(pValOrDefault);
	}
}

TEST(Result, MapOk) {

	{
		Alt::Result<usize, void> result = 2;

		Alt::Result<Alt::String, void> resultStr = result.Map([](usize) {
			return "String"_salloc;
		});

		EXPECT_EQ(resultStr.GetOk(), "String");
	}

	{
		Alt::Result<usize, Alt::String> result = 2;

		Alt::Result<Alt::String, usize> resultStr = std::move(result).Map([](usize) {
			return Alt::Result<Alt::String, usize>::Ok("String"_salloc);
		});

		EXPECT_EQ(resultStr.GetOk(), "String");
	}

	{
		Alt::Result<usize, void> result = 10;

		Alt::Result<usize, void> result0 = result.Map([](usize x) { return Alt::Result<usize, void>::Ok(x * 2); });
		EXPECT_EQ(result0.GetOk(), 20);

		Alt::Result<usize, void> result1 = result.Map([](usize x) { return Alt::Result<usize, void>::Ok(x + 4); });
		EXPECT_EQ(result1.GetOk(), 14);

		Alt::Result<usize, void> empty = Alt::Err;
		Alt::Result<usize, void> result2 = empty.Map([](usize x) { return Alt::Result<usize, void>::Ok(x * 2); });
		EXPECT_FALSE(result2);

		Alt::Result<usize, void> result3 = result
			.Map([](const usize x) { return Alt::Result<usize, void>::Ok(x * 2); })
			.Map([](const usize x) { return Alt::Result<usize, void>::Ok(x + 4); })
			.Map([](const usize x) { return Alt::Result<usize, void>::Ok(x * 2); })
		;

		EXPECT_EQ(result3.GetOk(), 48);
	}

	{
		Alt::Result<usize, void> result = 10;

		Alt::Result<usize, void> result0 = result.Map([](usize x) { return x * 2; });
		EXPECT_EQ(result0.GetOk(), 20);

		Alt::Result<usize, void> result1 = result.Map([](usize x) { return x + 4; });
		EXPECT_EQ(result1.GetOk(), 14);

		Alt::Result<usize, void> empty = Alt::Err;
		Alt::Result<usize, void> result2 = empty.Map([](usize x) { return x * 2; });
		EXPECT_FALSE(result2);

		Alt::Result<usize, void> result3 = result
			.Map([](const usize x) { return x * 2; })
			.Map([](const usize x) { return x + 4; })
			.Map([](const usize x) { return x * 2; })
		;

		EXPECT_EQ(result3.GetOk(), 48);
	}

	{
		Alt::Result<usize, Alt::String> result = 10;

		Alt::Result<usize, Alt::String> result0 = result.Clone().GetValueOrDefault().Map([](usize x) { return Alt::Result<usize, Alt::String>::Ok(x * 2); });
		EXPECT_EQ(result0.GetOk(), 20);

		Alt::Result<usize, Alt::String> result1 = result.Clone().GetValueOrDefault().Map([](usize x) { return Alt::Result<usize, Alt::String>::Ok(x + 4); });
		EXPECT_EQ(result1.GetOk(), 14);

		Alt::Result<usize, Alt::String> empty = ""_salloc;
		Alt::Result<usize, Alt::String> result2 = empty.Clone().GetValueOrDefault().Map([](usize x) { return Alt::Result<usize, Alt::String>::Ok(x * 2); });
		EXPECT_FALSE(result2);

		Alt::Result<usize, Alt::String> result3 = result
			.Clone().GetValueOrDefault()
			.Map([](const usize x) { return Alt::Result<usize, Alt::String>::Ok(x * 2); })
			.Map([](const usize x) { return Alt::Result<usize, Alt::String>::Ok(x + 4); })
			.Map([](const usize x) { return Alt::Result<usize, Alt::String>::Ok(x * 2); })
		;

		EXPECT_EQ(result3.GetOk(), 48);
	}

	{
		Alt::Result<usize, Alt::String> result = 10;

		Alt::Result<usize, Alt::String> result0 = result.Clone().GetValueOrDefault().Map([](usize x) { return x * 2; });
		EXPECT_EQ(result0.GetOk(), 20);

		Alt::Result<usize, Alt::String> result1 = result.Clone().GetValueOrDefault().Map([](usize x) { return x + 4; });
		EXPECT_EQ(result1.GetOk(), 14);

		Alt::Result<usize, Alt::String> empty = ""_salloc;
		Alt::Result<usize, Alt::String> result2 = empty.Clone().GetValueOrDefault().Map([](usize x) { return x * 2; });
		EXPECT_FALSE(result2);

		Alt::Result<usize, Alt::String> result3 = result
			.Clone().GetValueOrDefault()
			.Map([](const usize x) { return x * 2; })
			.Map([](const usize x) { return x + 4; })
			.Map([](const usize x) { return x * 2; })
		;

		EXPECT_EQ(result3.GetOk(), 48);
	}

	{
		Alt::String str = "Result"_salloc;

		Alt::Result<Alt::String&, void> result0 = str;

		Alt::Result<Alt::String, void> result1 = result0.Map([](const Alt::String& str) -> Alt::Result<Alt::String, void> {
			iferr_scope;

			Alt::String result = str.Clone() iferr_return Alt::Err;
			return result;
		});

		EXPECT_NE(&result1.GetOk(), &str);

		Alt::Result<Alt::String&, void> result2 = result0.Map([&](const Alt::String&) -> Alt::String& {
			return str;
		});

		EXPECT_EQ(&result2.GetOk(), &str);
	}

	{
		Alt::String str = "Result"_salloc;

		Alt::Result<Alt::String&, usize> result0 = str;

		Alt::Result<Alt::String, usize> result1 = result0.Map([](const Alt::String& str) {
			iferr_scope;

			Alt::String result = str.Clone() iferr_panic_silent;
			return result;
		});

		EXPECT_NE(&result1.GetOk(), &str);

		Alt::Result<Alt::String&, usize> result2 = result0.Map([&](const Alt::String&) -> Alt::String& {
			return str;
		});

		EXPECT_EQ(&result2.GetOk(), &str);
	}
}

TEST(Result, MapErr) {

	{
		Alt::Result<void, usize> result = 2;

		Alt::Result<void, Alt::String> resultStr = result.MapErr([](usize) {
			return "String"_salloc;
		});

		EXPECT_EQ(resultStr.GetErr(), "String");
	}

	{
		Alt::Result<Alt::String, usize> result = 2;

		Alt::Result<usize, Alt::String> resultStr = result.Clone().GetValueOrDefault().MapErr([](usize) {
			return Alt::Result<usize, Alt::String>::Err("String"_salloc);
		});

		EXPECT_EQ(resultStr.GetErr(), "String");
	}

	{
		Alt::Result<void, usize> result = 10;

		Alt::Result<void, usize> result0 = result.MapErr([](usize x) { return Alt::Result<void, usize>::Err(x * 2); });
		EXPECT_EQ(result0.GetErr(), 20);

		Alt::Result<void, usize> result1 = result.MapErr([](usize x) { return Alt::Result<void, usize>::Err(x + 4); });
		EXPECT_EQ(result1.GetErr(), 14);

		Alt::Result<void, usize> empty = Alt::Ok;
		Alt::Result<void, usize> result2 = empty.MapErr([](usize x) { return Alt::Result<void, usize>::Err(x * 2); });
		EXPECT_TRUE(result2);

		Alt::Result<void, usize> result3 = result
			.MapErr([](const usize x) { return Alt::Result<void, usize>::Err(x * 2); })
			.MapErr([](const usize x) { return Alt::Result<void, usize>::Err(x + 4); })
			.MapErr([](const usize x) { return Alt::Result<void, usize>::Err(x * 2); })
		;

		EXPECT_EQ(result3.GetErr(), 48);
	}

	{
		Alt::Result<void, usize> result = 10;

		Alt::Result<void, usize> result0 = result.MapErr([](usize x) { return x * 2; });
		EXPECT_EQ(result0.GetErr(), 20);

		Alt::Result<void, usize> result1 = result.MapErr([](usize x) { return x + 4; });
		EXPECT_EQ(result1.GetErr(), 14);

		Alt::Result<void, usize> empty = Alt::Ok;
		Alt::Result<void, usize> result2 = empty.MapErr([](usize x) { return x * 2; });
		EXPECT_TRUE(result2);

		Alt::Result<void, usize> result3 = result
			.MapErr([](const usize x) { return x * 2; })
			.MapErr([](const usize x) { return x + 4; })
			.MapErr([](const usize x) { return x * 2; })
		;

		EXPECT_EQ(result3.GetErr(), 48);
	}

	{
		Alt::Result<Alt::String, usize> result = 10;

		Alt::Result<Alt::String, usize> result0 = result.Clone().GetValueOrDefault().MapErr([](usize x) { return Alt::Result<Alt::String, usize>::Err(x * 2); });
		EXPECT_EQ(result0.GetErr(), 20);

		Alt::Result<Alt::String, usize> result1 = result.Clone().GetValueOrDefault().MapErr([](usize x) { return Alt::Result<Alt::String, usize>::Err(x + 4); });
		EXPECT_EQ(result1.GetErr(), 14);

		Alt::Result<Alt::String, usize> empty = ""_salloc;
		Alt::Result<Alt::String, usize> result2 = empty.Clone().GetValueOrDefault().MapErr([](usize x) { return Alt::Result<Alt::String, usize>::Err(x * 2); });
		EXPECT_TRUE(result2);

		Alt::Result<Alt::String, usize> result3 = result
			.Clone().GetValueOrDefault()
			.MapErr([](const usize x) { return Alt::Result<Alt::String, usize>::Err(x * 2); })
			.MapErr([](const usize x) { return Alt::Result<Alt::String, usize>::Err(x + 4); })
			.MapErr([](const usize x) { return Alt::Result<Alt::String, usize>::Err(x * 2); })
		;

		EXPECT_EQ(result3.GetErr(), 48);
	}

	{
		Alt::Result<Alt::String, usize> result = 10;

		Alt::Result<Alt::String, usize> result0 = result.Clone().GetValueOrDefault().MapErr([](usize x) { return x * 2; });
		EXPECT_EQ(result0.GetErr(), 20);

		Alt::Result<Alt::String, usize> result1 = result.Clone().GetValueOrDefault().MapErr([](usize x) { return x + 4; });
		EXPECT_EQ(result1.GetErr(), 14);

		Alt::Result<Alt::String, usize> empty = ""_salloc;
		Alt::Result<Alt::String, usize> result2 = empty.Clone().GetValueOrDefault().MapErr([](usize x) { return x * 2; });
		EXPECT_TRUE(result2);

		Alt::Result<Alt::String, usize> result3 = result
			.Clone().GetValueOrDefault()
			.MapErr([](const usize x) { return x * 2; })
			.MapErr([](const usize x) { return x + 4; })
			.MapErr([](const usize x) { return x * 2; })
		;

		EXPECT_EQ(result3.GetErr(), 48);
	}

	{
		Alt::String str = "Result"_salloc;

		Alt::Result<void, Alt::String&> result0 = str;

		Alt::Result<void, Alt::String> result1 = result0.MapErr([](const Alt::String& str) {
			return str.Clone().GetValueOrDefault();
		});

		EXPECT_NE(&result1.GetErr(), &str);

		Alt::Result<void, Alt::String&> result2 = result0.MapErr([&](const Alt::String&) -> Alt::String& {
			return str;
		});

		EXPECT_EQ(&result2.GetErr(), &str);
	}

	{
		Alt::String str = "Result"_salloc;

		Alt::Result<usize, Alt::String&> result0 = str;

		Alt::Result<usize, Alt::String> result1 = result0.MapErr([](const Alt::String& str) {
			return str.Clone().GetValueOrDefault();
		});

		EXPECT_NE(&result1.GetErr(), &str);

		Alt::Result<usize, Alt::String&> result2 = result0.MapErr([&](const Alt::String&) -> Alt::String& {
			return str;
		});

		EXPECT_EQ(&result2.GetErr(), &str);
	}
}

TEST(Result, ImplicitConversion) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Result<usize, void> sizeresult = 4;
		Alt::Result<const ssize, void> intresult = 5;

		Alt::Result<usize, void> sizeresult1 = intresult;
		sizeresult = intresult;

		EXPECT_EQ(sizeresult.GetOk(), (usize)intresult.GetOk());
		EXPECT_EQ(sizeresult1.GetOk(), (usize)intresult.GetOk());

		Alt::Result<const bool, void> boolresult = false;
		sizeresult = boolresult;

		EXPECT_EQ(sizeresult.GetOk(), (usize)boolresult.GetOk());

		Alt::Result<f32, void> f32result;

		f32result = intresult;
		EXPECT_EQ(f32result.GetOk(), (f32)5);

		ssize val = intresult.GetOk();
		EXPECT_EQ(val, 5);

		intresult = 20;
		EXPECT_EQ(intresult.GetOk(), 20);
	}

	{
		Alt::Result<void, usize> sizeresult = 4;
		Alt::Result<void, const ssize> intresult = 5;

		Alt::Result<void, usize> sizeresult1 = intresult;
		sizeresult = intresult;

		EXPECT_EQ(sizeresult.GetErr(), (usize)intresult.GetErr());
		EXPECT_EQ(sizeresult1.GetErr(), (usize)intresult.GetErr());

		Alt::Result<void, const bool> boolresult = false;
		sizeresult = boolresult;

		EXPECT_EQ(sizeresult.GetErr(), (usize)boolresult.GetErr());

		Alt::Result<void, f32> f32result;

		f32result = intresult;
		EXPECT_EQ(f32result.GetErr(), (f32)5);

		ssize val = intresult.GetErr();
		EXPECT_EQ(val, 5);

		intresult = 20;
		EXPECT_EQ(intresult.GetErr(), 20);
	}

	{
		Alt::Result<usize, Alt::String> sizeresult = 4;
		Alt::Result<const ssize, Alt::String> intresult = 5;

		Alt::Result<usize, Alt::String> sizeresult1 = intresult.Clone() iferr_return;
		sizeresult = intresult.Clone() iferr_return;

		EXPECT_EQ(sizeresult.GetOk(), (usize)intresult.GetOk());
		EXPECT_EQ(sizeresult1.GetOk(), (usize)intresult.GetOk());

		Alt::Result<const bool, Alt::String> boolresult = false;
		sizeresult = boolresult.Clone() iferr_return;

		EXPECT_EQ(sizeresult.GetOk(), (usize)boolresult.GetOk());

		Alt::Result<f32, Alt::String> f32result;

		f32result = intresult.Clone() iferr_return;
		EXPECT_EQ(f32result.GetOk(), (f32)5);

		ssize val = intresult.GetOk();
		EXPECT_EQ(val, 5);

		intresult = 20;
		EXPECT_EQ(intresult.GetOk(), 20);
	}

	{
		Alt::Result<Alt::String, usize> sizeresult = 4;
		Alt::Result<Alt::String, const ssize> intresult = 5;

		Alt::Result<Alt::String, usize> sizeresult1 = intresult.Clone() iferr_return;
		sizeresult = intresult.Clone() iferr_return;

		EXPECT_EQ(sizeresult.GetErr(), (usize)intresult.GetErr());
		EXPECT_EQ(sizeresult1.GetErr(), (usize)intresult.GetErr());

		Alt::Result<Alt::String, const bool> boolresult = false;
		sizeresult = boolresult.Clone() iferr_return;

		EXPECT_EQ(sizeresult.GetErr(), (usize)boolresult.GetErr());

		Alt::Result<Alt::String, f32> f32result;

		f32result = intresult.Clone() iferr_return;
		EXPECT_EQ(f32result.GetErr(), (f32)5);

		ssize val = intresult.GetErr();
		EXPECT_EQ(val, 5);

		intresult = 20;
		EXPECT_EQ(intresult.GetErr(), 20);
	}
}

TEST(Result, ValueAsPtr) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Result<void*, void> result0 = nullptr;
		Alt::Result<void*, void> result1 = Alt::Err;

		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());
		EXPECT_FALSE(result1);
		EXPECT_FALSE(result1.IsOk());
	}

	{
		usize val = 4;
		Alt::Result<usize*, void> result = &val;
		EXPECT_EQ(sizeof(result), sizeof(usize*));

		EXPECT_EQ(*result, val);

		Alt::Result<usize*, void> result1 = std::move(result);
		EXPECT_EQ(*result1, val);

		Alt::Result<usize*, void> result2 = result1;
		EXPECT_EQ(*result2, *result1);

		Alt::String str = "Result"_salloc;
		Alt::Result<Alt::String*, void> result3 = &str;
		EXPECT_EQ(result3->GetSizeBytes(), str.GetSizeBytes());

		result3 = Alt::Err;
		EXPECT_FALSE(result3);
		EXPECT_EQ(result3, Alt::Err);

		Alt::String* pValOr = result3.GetOkOr(nullptr);
		EXPECT_FALSE(pValOr);

		Alt::String* pValOrExec = result3.GetOkOrExec([]() { return nullptr; });
		EXPECT_FALSE(pValOrExec);

		if (Alt::String* pCastStr = result3) {
			FAIL();
		}
	}

	{
		usize val = 4;
		Alt::Result<Alt::UniquePtr<usize>, void> result = Alt::Memory::AllocateObject<usize>(val);
		EXPECT_EQ(sizeof(result), sizeof(Alt::UniquePtr<usize>));

		EXPECT_EQ(**result, val);

		Alt::Result<Alt::UniquePtr<usize>, void> result1 = std::move(result);
		EXPECT_EQ(**result1, val);

		Alt::String str = "Result"_salloc;
		Alt::String str0 = str.Clone() iferr_return;

		Alt::Result<Alt::UniquePtr<Alt::String>, void> result3 = Alt::Memory::AllocateObject<Alt::String>(std::move(str0));
		EXPECT_EQ(result3->Get()->GetSizeBytes(), str.GetSizeBytes());

		result3 = Alt::Err;
		EXPECT_FALSE(result3);
		EXPECT_EQ(result3, Alt::Err);
	}
}

TEST(Result, ValueAsRef) {

	{
		Alt::Result<usize&, void> result0 = Alt::Err;
		Alt::Result<usize&, void> result1 = Alt::Err;

		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());
		EXPECT_FALSE(result1);
		EXPECT_FALSE(result1.IsOk());
	}

	{
		usize val0 = 4;
		Alt::Result<usize&, void> result = val0;
		EXPECT_EQ(*result, val0);

		usize val1 = 2;
		result = val1;
		EXPECT_EQ(*result, val1);

		EXPECT_EQ(sizeof(result), sizeof(usize*));
	}

	{
		Alt::String val0 = "Result0"_salloc;
		Alt::Result<Alt::String&, void> result = val0;
		EXPECT_EQ(*result, val0);

		Alt::String val1 = "Result1"_salloc;
		result = val1;
		EXPECT_EQ(*result, val1);
		EXPECT_EQ(result->GetSizeBytes(), val1.GetSizeBytes());

		if (Alt::String* pCastStr = result) {
		}
		else {
			FAIL();
		}

		EXPECT_EQ(sizeof(result), sizeof(Alt::String*));
	}
}

TEST(Result, ErrorAsPtr) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Result<void, void*> result0 = nullptr;
		Alt::Result<void, void*> result1 = Alt::Ok;

		EXPECT_TRUE(result0);
		EXPECT_TRUE(result0.IsOk());
		EXPECT_TRUE(result1);
		EXPECT_TRUE(result1.IsOk());
	}

	{
		usize val = 4;
		Alt::Result<void, usize*> result = &val;
		EXPECT_EQ(sizeof(result), sizeof(usize*));

		EXPECT_EQ(*result.GetErr(), val);

		Alt::Result<void, usize*> result1 = std::move(result);
		EXPECT_EQ(*result1.GetErr(), val);

		Alt::Result<void, usize*> result2 = result1;
		EXPECT_EQ(*result2.GetErr(), *result1.GetErr());

		Alt::String str = "Result"_salloc;
		Alt::Result<void, Alt::String*> result3 = &str;
		EXPECT_EQ(result3.GetErr()->GetSizeBytes(), str.GetSizeBytes());

		result3 = Alt::Ok;
		EXPECT_TRUE(result3);
		EXPECT_EQ(result3, Alt::Ok);
	}

	{
		usize val = 4;
		Alt::Result<void, Alt::UniquePtr<usize>> result = Alt::Memory::AllocateObject<usize>(val);
		EXPECT_EQ(sizeof(result), sizeof(Alt::UniquePtr<usize>));

		EXPECT_EQ(*result.GetErr(), val);

		Alt::Result<void, Alt::UniquePtr<usize>> result1 = std::move(result);

		// result is now 'Alt::Ok', because err pointer is nullptr
		EXPECT_TRUE(result);

		EXPECT_EQ(*result1.GetErr(), val);

		Alt::String str = "Result"_salloc;
		Alt::String str0 = str.Clone() iferr_return;

		Alt::Result<void, Alt::UniquePtr<Alt::String>> result3 = Alt::Memory::AllocateObject<Alt::String>(std::move(str0));
		EXPECT_FALSE(result3);
		EXPECT_EQ(result3.GetErr()->GetSizeBytes(), str.GetSizeBytes());

		result3 = Alt::Ok;
		EXPECT_TRUE(result3);
		EXPECT_EQ(result3, Alt::Ok);
	}
}

TEST(Result, ErrorAsRef) {

	{
		Alt::Result<void, usize&> result0 = Alt::Ok;
		Alt::Result<void, usize&> result1 = Alt::Ok;

		EXPECT_TRUE(result0);
		EXPECT_TRUE(result0.IsOk());
		EXPECT_TRUE(result1);
		EXPECT_TRUE(result1.IsOk());
	}

	{
		usize val0 = 4;
		Alt::Result<void, usize&> result = val0;
		EXPECT_EQ(result.GetErr(), val0);

		usize val1 = 2;
		result = val1;
		EXPECT_EQ(result.GetErr(), val1);

		EXPECT_EQ(sizeof(result), sizeof(usize*));
	}

	{
		Alt::String val0 = "Result0"_salloc;
		Alt::Result<void, Alt::String&> result = val0;
		EXPECT_EQ(result.GetErr(), val0);

		Alt::String val1 = "Result1"_salloc;
		result = val1;
		EXPECT_EQ(result.GetErr(), val1);
		EXPECT_EQ(result.GetErr().GetSizeBytes(), val1.GetSizeBytes());

		EXPECT_EQ(sizeof(result), sizeof(Alt::String*));
	}
}

TEST(Result, OkAsRef) {

	{
		Alt::Result<usize&, void> result0 = Alt::Err;
		Alt::Result<usize&, void> result1 = Alt::Err;

		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());
		EXPECT_FALSE(result1);
		EXPECT_FALSE(result1.IsOk());
	}

	{
		usize val0 = 4;
		Alt::Result<usize&, void> result = val0;
		EXPECT_EQ(result.GetOk(), val0);

		usize val1 = 2;
		result = val1;
		EXPECT_EQ(result.GetOk(), val1);

		EXPECT_EQ(sizeof(result), sizeof(usize*));
	}

	{
		Alt::String val0 = "Result0"_salloc;
		Alt::Result<Alt::String&, void> result = val0;
		EXPECT_EQ(result.GetOk(), val0);

		Alt::String val1 = "Result1"_salloc;
		result = val1;
		EXPECT_EQ(result.GetOk(), val1);
		EXPECT_EQ(result.GetOk().GetSizeBytes(), val1.GetSizeBytes());

		EXPECT_EQ(sizeof(result), sizeof(Alt::String*));
	}
}

TEST(Result, AsRef) {

	{
		usize valusize = 4;
		f32 valf32 = 4.0f;

		Alt::Result<f32, const usize&> result0 = Alt::ResultErr<usize&>{ valusize };
		Alt::Result<f32, const usize&> result1 = valf32;

		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());
		EXPECT_TRUE(result1);
		EXPECT_TRUE(result1.IsOk());

		Alt::Swap(result0, result1);

		EXPECT_FALSE(result1);
		EXPECT_FALSE(result1.IsOk());
		EXPECT_TRUE(result0);
		EXPECT_TRUE(result0.IsOk());
	}

	{
		usize valusize = 4;
		f32 valf32 = 4.0f;

		Alt::Result<f32, const usize> result0 = Alt::ResultErr<usize>{ valusize };
		Alt::Result<f32, const usize> result1 = valf32;

		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());
		EXPECT_TRUE(result1);
		EXPECT_TRUE(result1.IsOk());

		Alt::Swap(result0, result1);

		EXPECT_FALSE(result1);
		EXPECT_FALSE(result1.IsOk());
		EXPECT_TRUE(result0);
		EXPECT_TRUE(result0.IsOk());
	}

	{
		usize valusize = 4;
		f32 valf32 = 4.0f;

		Alt::Result<f32&, const usize&> result0 = valusize;
		Alt::Result<f32&, const usize&> result1 = valf32;

		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());
		EXPECT_TRUE(result1);
		EXPECT_TRUE(result1.IsOk());

		Alt::Swap(result0, result1);

		EXPECT_FALSE(result1);
		EXPECT_FALSE(result1.IsOk());
		EXPECT_TRUE(result0);
		EXPECT_TRUE(result0.IsOk());
	}

	{
		usize val0 = 4;
		Alt::Result<f32&, usize&> result = val0;
		EXPECT_EQ(result.GetErr(), val0);

		usize val1 = 2;
		result = val1;
		EXPECT_EQ(result.GetErr(), val1);
	}

	{
		Alt::String val0 = "Result0"_salloc;
		Alt::Result<usize&, Alt::String&> result = val0;
		EXPECT_EQ(result.GetErr(), val0);

		Alt::String val1 = "Result1"_salloc;
		result = val1;
		EXPECT_EQ(result.GetErr(), val1);
		EXPECT_EQ(result.GetErr().GetSizeBytes(), val1.GetSizeBytes());
	}
}

TEST(Result, Rule5Tracking) {

	{
		Rule5Tracker::Core tracker{};
		Rule5Tracker track{ tracker };

		{
			Alt::Result<Rule5Tracker, void> result0 = track;

			EXPECT_TRUE(result0);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);

			Alt::Result<Rule5Tracker, void> result1 = std::move(result0);

			// result0 has value - like with 'std::optional'
			EXPECT_TRUE(result0);

			EXPECT_TRUE(result1);

			EXPECT_EQ(tracker.cntMoveConstruct, 1);
			EXPECT_EQ(tracker.cntMoveAssignment, 0);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);
			EXPECT_EQ(tracker.cntCopyAssignment, 0);

			result0 = std::move(result1);

			EXPECT_TRUE(result0);

			// result1 has value - like with 'std::optional'
			EXPECT_TRUE(result1);

			EXPECT_EQ(tracker.cntMoveConstruct, 1);
			EXPECT_EQ(tracker.cntMoveAssignment, 1);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);
			EXPECT_EQ(tracker.cntCopyAssignment, 0);

			result1 = result0;
			EXPECT_TRUE(result0);
			EXPECT_TRUE(result1);

			EXPECT_EQ(tracker.cntMoveConstruct, 1);
			EXPECT_EQ(tracker.cntMoveAssignment, 1);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);
			EXPECT_EQ(tracker.cntCopyAssignment, 1);

			result1 = Alt::Err;
		}

		EXPECT_EQ(tracker.cntDestructor, 2);
	}

	{
		Rule5Tracker::Core tracker{};
		Rule5Tracker track{ tracker };

		{
			Alt::Result<void, Rule5Tracker> result0 = track;

			EXPECT_FALSE(result0);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);

			Alt::Result<void, Rule5Tracker> result1 = std::move(result0);

			// result0 has value - like with 'std::optional'
			EXPECT_FALSE(result0);

			EXPECT_FALSE(result1);

			EXPECT_EQ(tracker.cntMoveConstruct, 1);
			EXPECT_EQ(tracker.cntMoveAssignment, 0);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);
			EXPECT_EQ(tracker.cntCopyAssignment, 0);

			result0 = std::move(result1);

			EXPECT_FALSE(result0);

			// result1 has value - like with 'std::optional'
			EXPECT_FALSE(result1);

			EXPECT_EQ(tracker.cntMoveConstruct, 1);
			EXPECT_EQ(tracker.cntMoveAssignment, 1);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);
			EXPECT_EQ(tracker.cntCopyAssignment, 0);

			result1 = result0;
			EXPECT_FALSE(result0);
			EXPECT_FALSE(result1);

			EXPECT_EQ(tracker.cntMoveConstruct, 1);
			EXPECT_EQ(tracker.cntMoveAssignment, 1);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);
			EXPECT_EQ(tracker.cntCopyAssignment, 1);

			result1 = Alt::Ok;
		}

		EXPECT_EQ(tracker.cntDestructor, 2);
	}

	{
		Rule5Tracker::Core tracker{};
		Rule5Tracker track{ tracker };

		{
			Alt::Result<Rule5Tracker, usize> result0 = track;

			EXPECT_TRUE(result0);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);

			Alt::Result<Rule5Tracker, usize> result1 = std::move(result0);

			// result0 has value - like with 'std::optional'
			EXPECT_TRUE(result0);

			EXPECT_TRUE(result1);

			EXPECT_EQ(tracker.cntMoveConstruct, 1);
			EXPECT_EQ(tracker.cntMoveAssignment, 0);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);
			EXPECT_EQ(tracker.cntCopyAssignment, 0);

			result0 = std::move(result1);

			EXPECT_TRUE(result0);

			// result1 has value - like with 'std::optional'
			EXPECT_TRUE(result1);

			EXPECT_EQ(tracker.cntMoveConstruct, 1);
			EXPECT_EQ(tracker.cntMoveAssignment, 1);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);
			EXPECT_EQ(tracker.cntCopyAssignment, 0);

			result1 = result0;
			EXPECT_TRUE(result0);
			EXPECT_TRUE(result1);

			EXPECT_EQ(tracker.cntMoveConstruct, 1);
			EXPECT_EQ(tracker.cntMoveAssignment, 1);
			EXPECT_EQ(tracker.cntCopyConstruct, 1);
			EXPECT_EQ(tracker.cntCopyAssignment, 1);

			result1 = 4;
		}

		EXPECT_EQ(tracker.cntDestructor, 2);
	}
}

TEST(Result, Clone) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Result<Alt::Vector<usize, 0>, usize> res0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
		Alt::Result<Alt::Vector<usize, 1>, usize> res1;

		res1 = std::move(res0);

		ASSERT_TRUE(res0);
		ASSERT_TRUE(res1);

		EXPECT_TRUE(res0->IsEmpty());
		EXPECT_TRUE(res1->IsPopulated());

		res0.CloneFrom(res1) iferr_return;

		ASSERT_TRUE(res0);
		ASSERT_TRUE(res1);

		EXPECT_TRUE(res0->IsPopulated());
		EXPECT_TRUE(res1->IsPopulated());
		EXPECT_EQ(*res0, *res1);

		res0 = 4;
		res0.CloneFrom(res1) iferr_return;

		ASSERT_TRUE(res0);
		ASSERT_TRUE(res1);

		EXPECT_TRUE(res0->IsPopulated());
		EXPECT_TRUE(res1->IsPopulated());
		EXPECT_EQ(*res0, *res1);

		//Alt::Result<Alt::Vector<usize, 2>, usize> res2 = res1; // compilation error
		Alt::Result<Alt::Vector<usize, 4>, usize> res2 = res1.Clone() iferr_return;

		ASSERT_TRUE(res1);
		ASSERT_TRUE(res2);

		EXPECT_TRUE(res1->IsPopulated());
		EXPECT_TRUE(res2->IsPopulated());
		EXPECT_EQ(*res1, *res2);
	}

	{
		usize x = 4;
		Alt::Result<Alt::Vector<usize>, usize&> res0 = x;
		Alt::Result<Alt::Vector<usize>, usize&> res1 = Alt::Vector<usize>{};

		res1 = std::move(res0);

		EXPECT_FALSE(res0);
		ASSERT_FALSE(res1);

		EXPECT_EQ(res1.GetErr(), 4);

		res0.CloneFrom(res1) iferr_return;

		ASSERT_FALSE(res0);
		ASSERT_FALSE(res1);

		EXPECT_EQ(res0.GetErr(), 4);
		EXPECT_EQ(res1.GetErr(), 4);

		res0 = Alt::Vector<usize>{};
		res0.CloneFrom(res1) iferr_return;

		ASSERT_FALSE(res0);
		ASSERT_FALSE(res1);

		EXPECT_EQ(res0.GetErr(), 4);
		EXPECT_EQ(res1.GetErr(), 4);

		Alt::Optional<Alt::Result<Alt::Vector<usize>, usize&>> res2_ = res1.Clone() iferr_return;
		Alt::Result<Alt::Vector<usize>, usize&> res2 = std::move(*res2_);

		ASSERT_FALSE(res1);
		ASSERT_FALSE(res2);

		EXPECT_EQ(res1.GetErr(), 4);
		EXPECT_EQ(res2.GetErr(), 4);
	}
}

TEST(Result, CloneOk) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Result<Alt::Vector<usize, 0>, void> res0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
		Alt::Result<Alt::Vector<usize, 1>, void> res1;

		res1 = std::move(res0);

		ASSERT_TRUE(res0);
		ASSERT_TRUE(res1);

		EXPECT_TRUE(res0->IsEmpty());
		EXPECT_TRUE(res1->IsPopulated());

		res0.CloneFrom(res1) iferr_return;

		ASSERT_TRUE(res0);
		ASSERT_TRUE(res1);

		EXPECT_TRUE(res0->IsPopulated());
		EXPECT_TRUE(res1->IsPopulated());
		EXPECT_EQ(*res0, *res1);

		res0 = Alt::Err;
		res0.CloneFrom(res1) iferr_return;

		ASSERT_TRUE(res0);
		ASSERT_TRUE(res1);

		EXPECT_TRUE(res0->IsPopulated());
		EXPECT_TRUE(res1->IsPopulated());
		EXPECT_EQ(*res0, *res1);

		//Alt::Result<Alt::Vector<usize, 2>, void> res2 = res1; // compilation error
		Alt::Result<Alt::Vector<usize, 4>, void> res2 = res1.Clone() iferr_return;

		ASSERT_TRUE(res1);
		ASSERT_TRUE(res2);

		EXPECT_TRUE(res1->IsPopulated());
		EXPECT_TRUE(res2->IsPopulated());
		EXPECT_EQ(*res1, *res2);
	}

	{
		usize x = 4;
		Alt::Result<usize&, void> res0 = x;
		Alt::Result<usize&, void> res1;

		res1 = std::move(res0);

		EXPECT_TRUE(res0);
		ASSERT_TRUE(res1);

		EXPECT_EQ(*res1, 4);

		res0.CloneFrom(res1) iferr_return;

		ASSERT_TRUE(res0);
		ASSERT_TRUE(res1);

		EXPECT_EQ(*res0, 4);
		EXPECT_EQ(*res1, 4);

		res0 = Alt::Err;
		res0.CloneFrom(res1) iferr_return;

		ASSERT_TRUE(res0);
		ASSERT_TRUE(res1);

		EXPECT_EQ(*res0, 4);
		EXPECT_EQ(*res1, 4);

		Alt::Optional<Alt::Result<usize&, void>> res2_ = res1.Clone() iferr_return;
		Alt::Result<usize&, void> res2 = std::move(*res2_);

		ASSERT_TRUE(res1);
		ASSERT_TRUE(res2);

		EXPECT_EQ(*res1, 4);
		EXPECT_EQ(*res2, 4);
	}
}

TEST(Result, CloneErr) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Result<void, Alt::Vector<usize, 0>> res0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
		Alt::Result<void, Alt::Vector<usize, 1>> res1;

		res1 = std::move(res0);

		ASSERT_FALSE(res0);
		ASSERT_FALSE(res1);

		EXPECT_TRUE(res0.GetErr().IsEmpty());
		EXPECT_TRUE(res1.GetErr().IsPopulated());

		res0.CloneFrom(res1) iferr_return;

		ASSERT_FALSE(res0);
		ASSERT_FALSE(res1);

		EXPECT_TRUE(res0.GetErr().IsPopulated());
		EXPECT_TRUE(res1.GetErr().IsPopulated());
		EXPECT_EQ(res0.GetErr(), res1.GetErr());

		res0 = Alt::Ok;
		res0.CloneFrom(res1) iferr_return;

		ASSERT_FALSE(res0);
		ASSERT_FALSE(res1);

		EXPECT_TRUE(res0.GetErr().IsPopulated());
		EXPECT_TRUE(res1.GetErr().IsPopulated());
		EXPECT_EQ(res0.GetErr(), res1.GetErr());

		//Alt::Result<void, Alt::Vector<usize, 2>> res2 = res1; // compilation error
		Alt::Result<void, Alt::Vector<usize, 4>> res2 = res1.Clone() iferr_return;

		ASSERT_FALSE(res1);
		ASSERT_FALSE(res2);

		EXPECT_TRUE(res1.GetErr().IsPopulated());
		EXPECT_TRUE(res2.GetErr().IsPopulated());
		EXPECT_EQ(res1.GetErr(), res2.GetErr());
	}

	{
		usize x = 4;
		Alt::Result<void, usize&> res0 = x;
		Alt::Result<void, usize&> res1;

		res1 = std::move(res0);

		EXPECT_FALSE(res0);
		ASSERT_FALSE(res1);

		EXPECT_EQ(res1.GetErr(), 4);

		res0.CloneFrom(res1) iferr_return;

		ASSERT_FALSE(res0);
		ASSERT_FALSE(res1);

		EXPECT_EQ(res0.GetErr(), 4);
		EXPECT_EQ(res1.GetErr(), 4);

		res0 = Alt::Ok;
		res0.CloneFrom(res1) iferr_return;

		ASSERT_FALSE(res0);
		ASSERT_FALSE(res1);

		EXPECT_EQ(res0.GetErr(), 4);
		EXPECT_EQ(res1.GetErr(), 4);

		Alt::Optional<Alt::Result<void, usize&>> res2_ = res1.Clone() iferr_return;
		Alt::Result<void, usize&> res2 = std::move(*res2_);

		ASSERT_FALSE(res1);
		ASSERT_FALSE(res2);

		EXPECT_EQ(res1.GetErr(), 4);
		EXPECT_EQ(res2.GetErr(), 4);
	}
}

#pragma warning(pop)