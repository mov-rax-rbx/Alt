#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Utils/Optional.h"
#include "../alt-std/src/Memory/UniquePtr.h"

#include "../alt-std/src/String/String.h"
#include "../alt-std/src/Collections/Vector.h"

#pragma warning(push)
#pragma warning(disable: 4624 /* destructor was implicitly defined as deleted */)
#pragma warning(disable: 4244 /* conversion from ... to ..., possible loss of data */)

TEST(Optional, TypeTraits) {

	bool v = false;

	{
		v = Alt::TypeTraits::is_optional_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_optional_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_optional_v<Alt::Optional<Alt::Optional<Rule5AllTrivial>>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_optional_v<Alt::Optional<Rule5AllTrivial*>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_optional_v<Alt::Optional<Alt::Optional<Rule5AllTrivial*>>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_optional_v<Alt::Optional<Rule5AllTrivial&>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_optional_v<Rule5AllTrivial>;
		EXPECT_FALSE(v);
	}

	{
		v = Alt::TypeTraits::is_clonable_v<Alt::Optional<const volatile void>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Optional<Alt::Optional<Rule5AllTrivial>>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Optional<Rule5AllTrivial*>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Optional<Alt::Optional<Rule5AllTrivial*>>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_clonable_v<Alt::Optional<Rule5AllTrivial&>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_copyable_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::Optional<Alt::Optional<Rule5AllTrivial>>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Optional<Rule5AllTrivial&>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Optional<Rule5AllTrivial*>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Optional<Rule5AllNonTrivial*>>;
		EXPECT_TRUE(v);
	}

	{
		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Optional<Alt::Optional<Rule5AllTrivial>>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Optional<Rule5AllTrivial&>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Optional<Rule5AllNonTrivial&>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Optional<Rule5AllNonTrivial*>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_default_constructible_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Optional<Rule5AllNonTrivial*>>;
		EXPECT_FALSE(v); // we need to assign nullptr
	}

	{
		v = std::is_default_constructible_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_default_constructible_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_constructible_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_constructible_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_constructible_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_copy_constructible_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_constructible_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_constructible_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_copy_assignable_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_copy_assignable_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_copy_assignable_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_constructible_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_constructible_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_constructible_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_constructible_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_move_assignable_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_move_assignable_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_trivially_move_assignable_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_move_assignable_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_FALSE(v);
	}

	{
		v = std::is_destructible_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_destructible_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);
	}

	{
		v = std::is_trivially_destructible_v<Alt::Optional<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Optional<Rule5AllTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Optional<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Optional<Rule5AllDeleted>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_destructible_v<Alt::Optional<Rule5AllDeletedExceptDestructor>>;
		EXPECT_TRUE(v);
	}
}

TEST(Optional, Construction) {

	{
		Alt::Optional<void> result0{};
		Alt::Optional<volatile void> result1{};
		Alt::Optional<const volatile void> result2{};

		Alt::Optional<Rule5AllTrivial> result3{};
		Alt::Optional<Rule5AllNonTrivial> result4{};
		//Alt::Optional<Rule5AllDeleted> result5{}; // compilation error
		Alt::Optional<Rule5AllDeletedExceptDestructor> result6{};
	}

	{
		Alt::Optional<usize> result(2);
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result, 2);
		EXPECT_EQ(result.GetValue(), 2);

		EXPECT_NE(result, Alt::Nullopt);

		result = Alt::Optional<usize>{ 4 };
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result, 4);
		EXPECT_EQ(result.GetValue(), 4);

		EXPECT_NE(result, Alt::Nullopt);

		result = Alt::Nullopt;
		EXPECT_EQ(result, Alt::Nullopt);
	}

	{
		Alt::Optional<usize> result = Alt::MakeOptional(2);
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result, 2);
		EXPECT_EQ(result.GetValue(), 2);

		EXPECT_NE(result, Alt::Nullopt);

		result = Alt::MakeOptional(4);
		ASSERT_TRUE(result);
		ASSERT_TRUE(result.IsOk());

		EXPECT_EQ(*result, 4);
		EXPECT_EQ(result.GetValue(), 4);

		EXPECT_NE(result, Alt::Nullopt);

		result = Alt::Nullopt;
		EXPECT_EQ(result, Alt::Nullopt);
	}
}

TEST(Optional, CopyMoveConstructAssignment) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Optional<Alt::String> result0("String"_salloc);
	ASSERT_TRUE(result0);
	ASSERT_TRUE(result0.IsOk());

	result0 = std::move(result0);
	ASSERT_TRUE(result0);
	ASSERT_TRUE(result0.IsOk());

	result0 = result0.Clone() iferr_return;
	ASSERT_TRUE(result0);
	ASSERT_TRUE(result0.IsOk());

	Alt::Optional<Alt::String> result1 = result0.Clone() iferr_return;
	ASSERT_TRUE(result1);
	ASSERT_TRUE(result1.IsOk());

	EXPECT_EQ(*result0, "String");
	EXPECT_EQ(result0.GetValue(), "String");
	EXPECT_EQ(*result1, "String");
	EXPECT_EQ(result1.GetValue(), "String");

	Alt::Optional<Alt::String> copy = result1.Clone() iferr_return;
	ASSERT_TRUE(copy);
	ASSERT_TRUE(copy.IsOk());

	EXPECT_EQ(*copy, "String");

	Alt::Optional<Alt::String> move = std::move(copy);
	ASSERT_TRUE(move);
	ASSERT_TRUE(move.IsOk());

	EXPECT_EQ(*move, "String");

	EXPECT_TRUE(copy);
	EXPECT_TRUE(copy.IsOk());
	EXPECT_TRUE(copy->IsEmpty());

	Alt::String strCopy = move->Clone() iferr_return;
	EXPECT_EQ(strCopy, "String");

	Alt::String strMove = std::move(*move);
	EXPECT_EQ(strMove, "String");

	EXPECT_TRUE(move);
	EXPECT_TRUE(move.IsOk());
	EXPECT_TRUE(move->IsEmpty());

	result0 = Alt::Nullopt;
	EXPECT_FALSE(result0);

	result1 = result0.Clone() iferr_return;
	EXPECT_FALSE(result1);

	result1 = std::move(result0);
	EXPECT_FALSE(result1);
}

TEST(Optional, GetOr) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Optional<Alt::String> result;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetValueOr("Value"_salloc), "Value");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetValueOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Value");

		EXPECT_EQ(std::move(result).GetValueOrDefault(), "");
	}

	{
		Alt::Optional<Alt::String> result = "Optional"_salloc;

		EXPECT_EQ(result.Clone().GetValueOrDefault().GetValueOr("Value"_salloc), "Optional");

		Alt::String valueOrExec = result.Clone().GetValueOrDefault().GetValueOrExec([]() { return "Value"_salloc; });
		EXPECT_EQ(valueOrExec, "Optional");

		EXPECT_EQ(std::move(result).GetValueOrDefault(), "Optional");
	}

	{
		Alt::String str = "Optional"_salloc;
		Alt::Optional<Alt::String*> result = &str;

		{
			Alt::String* pValOr = result.GetValueOr(nullptr);
			EXPECT_EQ(pValOr, &str);

			Alt::String* pValOrFunc = result.GetValueOrExec([]() { return nullptr; });
			EXPECT_EQ(pValOrFunc, &str);

			Alt::String* pValOrDefault = result.GetValueOrDefault();
			EXPECT_EQ(pValOrDefault, &str);

			if (Alt::String* pCastStr = result) {
			}
			else {
				FAIL();
			}
		}

		result = Alt::Nullopt;

		{
			Alt::String* pValOr = result.GetValueOr(nullptr);
			EXPECT_FALSE(pValOr);

			Alt::String* pValOrFunc = result.GetValueOrExec([]() { return nullptr; });
			EXPECT_FALSE(pValOrFunc);

			Alt::String* pValOrDefault = result.GetValueOrDefault();
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
			Alt::Optional<Alt::UniquePtr<Alt::String>> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0));

			Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetValueOr(nullptr);
			EXPECT_TRUE(pValOr);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Optional<Alt::UniquePtr<Alt::String>> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0));

			Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetValueOrExec([]() { return nullptr; });
			EXPECT_TRUE(pValOrFunc);
		}

		{
			Alt::String str0 = str.Clone() iferr_return;
			Alt::Optional<Alt::UniquePtr<Alt::String>> result = Alt::UniquePtr<Alt::String>::AllocateObject(std::move(str0));

			Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetValueOrDefault();
			EXPECT_TRUE(pValOrDefault);
		}

		Alt::Optional<Alt::UniquePtr<Alt::String>> result = Alt::Nullopt;

		Alt::UniquePtr<Alt::String> pValOr = std::move(result).GetValueOr(nullptr);
		EXPECT_FALSE(pValOr);

		Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result).GetValueOrExec([]() { return nullptr; });
		EXPECT_FALSE(pValOrFunc);

		Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result).GetValueOrDefault();
		EXPECT_FALSE(pValOrDefault);
	}
}

TEST(Optional, Map) {

	{
		Alt::Optional<usize> result = 2;

		Alt::Optional<Alt::String> resultStr = result.Map([](usize) {
			return "String"_salloc;
		});

		EXPECT_EQ(*resultStr, "String");
	}

	{
		Alt::Optional<usize> result = 10;

		Alt::Optional<usize> result0 = result.Map([](usize x) { return x * 2; });
		EXPECT_EQ(*result0, 20);

		Alt::Optional<usize> result1 = result.Map([](usize x) { return Alt::MakeOptional(x + 4); });
		EXPECT_EQ(*result1, 14);

		Alt::Optional<usize> empty = Alt::Nullopt;
		Alt::Optional<usize> result2 = empty.Map([](usize x) { return x * 2; });
		EXPECT_FALSE(result2);

		Alt::Optional<usize> result3 = result
			.Map([](const usize x) { return x * 2; })
			.Map([](const usize x) { return x + 4; })
			.Map([](const usize x) { return x * 2; })
		;

		EXPECT_EQ(*result3, 48);
	}

	{
		bool v = false;

		Alt::Optional<Rule5AllNonTrivial> opt = Rule5AllNonTrivial{};
		const Alt::Optional<Rule5AllNonTrivial>& constOpt = opt;

		auto constMap = constOpt.Map([](const Rule5AllNonTrivial&) { return (usize)4; });

		v = std::is_same_v<decltype(constMap), Alt::Optional<usize>>;
		EXPECT_TRUE(v);

		v = std::is_const_v<std::remove_reference_t<decltype(constOpt.GetValue())>>;
		EXPECT_TRUE(v);
	}

	{
		Alt::String str = "Optional"_salloc;

		Alt::Optional<Alt::String&> result0 = str;

		Alt::Optional<Alt::String> result1 = result0.Map([](const Alt::String& str) {
			return str.Clone();
		});

		EXPECT_NE(&*result1, &str);

		Alt::Optional<Alt::String&> result2 = result0.Map([&](const Alt::String&) -> Alt::String& {
			return str;
		});

		EXPECT_EQ(&*result2, &str);
	}
}

TEST(Optional, AsPtr) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Optional<void*> result0 = nullptr;
		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());

		Alt::Optional<const void*> result1 = Alt::Nullopt;
		EXPECT_FALSE(result1);
		EXPECT_FALSE(result1.IsOk());
	}

	{
		usize val = 4;
		Alt::Optional<usize*> result0 = &val;
		EXPECT_EQ(sizeof(result0), sizeof(usize*));
		EXPECT_EQ(*result0, val);

		Alt::Optional<usize*> result1 = std::move(result0);
		EXPECT_EQ(*result1, val);

		Alt::Optional<usize*> result2 = result1;
		EXPECT_EQ(*result2, *result1);

		Alt::String str = "Optional"_salloc;
		Alt::Optional<Alt::String*> result3 = &str;
		EXPECT_EQ(result3->GetSizeBytes(), str.GetSizeBytes());

		result3 = Alt::Nullopt;
		EXPECT_FALSE(result3);
		EXPECT_EQ(result3, Alt::Nullopt);

		Alt::String* pValOr = result3.GetValueOr(nullptr);
		EXPECT_FALSE(pValOr);

		Alt::String* pValOrFunc = result3.GetValueOrExec([]() { return nullptr; });
		EXPECT_FALSE(pValOrFunc);

		Alt::String* pValOrDefault = result3.GetValueOrDefault();
		EXPECT_FALSE(pValOrDefault);

		if (Alt::String* pCastStr = result3) {
			FAIL();
		}
	}

	{
		usize val = 4;
		Alt::Optional<Alt::UniquePtr<usize>> result = Alt::Memory::AllocateObject<usize>(val);
		EXPECT_EQ(sizeof(result), sizeof(Alt::UniquePtr<usize>));

		EXPECT_EQ(**result, val);

		Alt::Optional<Alt::UniquePtr<usize>> result1 = std::move(result);
		EXPECT_EQ(**result1, val);

		Alt::String str = "Optional"_salloc;
		Alt::String str0 = str.Clone() iferr_return;

		Alt::Optional<Alt::UniquePtr<Alt::String>> result3 = Alt::Memory::AllocateObject<Alt::String>(std::move(str0));
		EXPECT_EQ(result3->Get()->GetSizeBytes(), str.GetSizeBytes());

		result3 = Alt::Nullopt;
		EXPECT_FALSE(result3);
		EXPECT_EQ(result3, Alt::Nullopt);

		Alt::UniquePtr<Alt::String> pValOr = std::move(result3).GetValueOr(nullptr);
		EXPECT_FALSE(pValOr);

		Alt::UniquePtr<Alt::String> pValOrFunc = std::move(result3).GetValueOrExec([]() { return nullptr; });
		EXPECT_FALSE(pValOrFunc);

		Alt::UniquePtr<Alt::String> pValOrDefault = std::move(result3).GetValueOrDefault();
		EXPECT_FALSE(pValOrDefault);
	}

	{
		Alt::String str = "Optional"_salloc;

		Alt::Optional<Alt::String*> opt0 = &str;
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(opt0.GetValue(), &str);

		Alt::Optional<const Alt::String*> opt1 = opt0;
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(opt1.GetValue(), &str);

		Alt::Optional<const Alt::String*> opt2 = std::move(opt1);
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(opt2.GetValue(), &str);

		const Alt::Optional<const Alt::String*> opt3 = std::move(opt2);
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(opt3.GetValue(), &str);

		Alt::Optional<const Alt::String*> opt4 = opt3;
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(opt4.GetValue(), &str);

		//Alt::Optional<Alt::String*> opt5 = opt3; // compilation error
		//const Alt::Optional<Alt::String*> opt6 = opt3; // compilation error
		//Alt::Optional<Alt::String*> opt7 = opt2; // compilation error
	}
}

TEST(Optional, AsRef) {

	{
		Alt::Optional<usize&> result0 = Alt::Nullopt;
		EXPECT_FALSE(result0);
		EXPECT_FALSE(result0.IsOk());

		Alt::Optional<const usize&> result1 = Alt::Nullopt;
		EXPECT_FALSE(result1);
		EXPECT_FALSE(result1.IsOk());
	}

	{
		f32 f3232 = 4.0f;

		Alt::Optional<const f32&> result0 = f3232;
		EXPECT_EQ(*result0, f3232);

		Alt::Optional<const f32*> result1 = &f3232;
		EXPECT_EQ(*result1, f3232);

		result1 = nullptr;
		EXPECT_FALSE(result1);

		//Alt::Optional<const usize&> result2 = f3232; // compilation error

		Alt::Optional<Alt::Ref<f32>> result3 = f3232;
		EXPECT_EQ(*result3, f3232);

		f32& f32FromOpt = result3.GetValue();
		EXPECT_EQ(f32FromOpt, f3232);
	}

	{
		usize val0 = 4;
		Alt::Optional<usize&> result = val0;
		EXPECT_EQ(*result, val0);

		usize val1 = 2;
		result = val1;
		EXPECT_EQ(*result, val1);

		EXPECT_EQ(sizeof(result), sizeof(usize*));
	}

	{
		Alt::String val0 = "Optional0"_salloc;
		Alt::Optional<Alt::String&> result = val0;
		EXPECT_EQ(*result, val0);
		EXPECT_EQ(&*result, &val0);

		Alt::String val1 = "Optional1"_salloc;
		result = val1;
		EXPECT_EQ(*result, val1);
		EXPECT_EQ(&*result, &val1);
		EXPECT_EQ(result->GetSizeBytes(), val1.GetSizeBytes());

		if (Alt::String* pCastStr = result) {
		}
		else {
			FAIL();
		}

		result = Alt::Nullopt;
		EXPECT_FALSE(result);
		EXPECT_EQ(result, Alt::Nullopt);

		Alt::String& valOr = result.GetValueOr(val0);
		EXPECT_FALSE(valOr.IsEmpty());
		EXPECT_FALSE(val0.IsEmpty());

		Alt::String& valOrFunc = result.GetValueOrExec([&]() -> Alt::String& { return val0; });
		EXPECT_FALSE(valOrFunc.IsEmpty());
		EXPECT_FALSE(valOrFunc.IsEmpty());

		//Alt::String& valOrDefault = result.GetValueOrDefault(); // compilation error
		//EXPECT_FALSE(valOrDefault.IsEmpty());
		//EXPECT_FALSE(valOrDefault.IsEmpty());

		if (Alt::String* pCastStr = result) {
			FAIL();
		}

		EXPECT_EQ(sizeof(result), sizeof(Alt::String*));
	}

	{
		Alt::String str = "Optional"_salloc;

		Alt::Optional<Alt::String&> opt0 = str;
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(&opt0.GetValue(), &str);

		Alt::Optional<const Alt::String&> opt1 = opt0;
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(&opt1.GetValue(), &str);

		Alt::Optional<const Alt::String&> opt2 = std::move(opt1);
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(&opt2.GetValue(), &str);

		const Alt::Optional<const Alt::String&> opt3 = std::move(opt2);
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(&opt3.GetValue(), &str);

		Alt::Optional<const Alt::String&> opt4 = opt3;
		EXPECT_FALSE(str.IsEmpty());
		EXPECT_EQ(&opt4.GetValue(), &str);

		//Alt::Optional<Alt::String&> opt5 = opt3; // compilation error
		//const Alt::Optional<Alt::String&> opt6 = opt3; // compilation error
		//Alt::Optional<Alt::String&> opt7 = opt2; // compilation error
	}

	{
		Alt::String str = "Optional"_salloc;

		// for reference we return reference (move stuff)
		Alt::String& s = Alt::Optional<Alt::String&>(str).GetValue();
		EXPECT_EQ(&s, &str);

		// now move for reference - move value
		Alt::String newStr = std::move(Alt::Optional<Alt::String&>(str).GetValue());

		EXPECT_TRUE(str.IsEmpty());
	}
}

TEST(Optional, ImplicitConversion) {

	Alt::Optional<usize> sizeresult = 4;
	Alt::Optional<const ssize> intresult = 5;

	Alt::Optional<usize> sizeresult1 = intresult;
	sizeresult = intresult;

	EXPECT_EQ(*sizeresult, (usize)*intresult);
	EXPECT_EQ(*sizeresult1, (usize)*intresult);

	Alt::Optional<const bool> boolresult = false;
	sizeresult = boolresult;

	EXPECT_EQ(*sizeresult, (usize)*boolresult);

	Alt::Optional<f32> f32result;

	f32result = intresult; 
	EXPECT_EQ(*f32result, (f32)5);

	ssize val = *intresult; 
	EXPECT_EQ(val, 5);

	intresult = 20;
	EXPECT_EQ(*intresult, 20);
}

TEST(Optional, OptionalInOptional) {

	{
		Alt::Optional<Alt::Optional<usize>> sizeresult = 4;
		Alt::Optional<ssize> intresult = 5;

		Alt::Optional<Alt::Optional<usize>> sizeresult1 = intresult;
		sizeresult = intresult;

		EXPECT_EQ(**sizeresult, (usize)*intresult);
		EXPECT_EQ(**sizeresult1, (usize)*intresult);

		Alt::Optional<bool> boolresult = false;
		sizeresult = boolresult;

		EXPECT_TRUE(sizeresult);
		EXPECT_TRUE(*sizeresult);

		EXPECT_EQ(**sizeresult, (usize)*boolresult);

		sizeresult1 = sizeresult;

		EXPECT_TRUE(sizeresult1);
		EXPECT_TRUE(*sizeresult1);

		EXPECT_TRUE(sizeresult);
		EXPECT_TRUE(*sizeresult);

		EXPECT_EQ(**sizeresult1, **sizeresult);

		*sizeresult = Alt::Nullopt; 
		sizeresult1 = sizeresult;

		EXPECT_FALSE(*sizeresult);
		EXPECT_FALSE(*sizeresult1);

		sizeresult = Alt::Nullopt; 
		sizeresult1 = sizeresult;

		EXPECT_FALSE(sizeresult);
		EXPECT_FALSE(sizeresult1);

		Alt::Optional<Alt::Optional<ssize>> intintresult = 5;
		sizeresult = intintresult;

		EXPECT_TRUE(sizeresult);
		EXPECT_TRUE(intintresult);

		EXPECT_TRUE(*sizeresult);
		EXPECT_TRUE(*intintresult);

		EXPECT_EQ(**sizeresult, (usize)**intintresult);
	}
}

TEST(Optional, Rule5Tracking) {

	Rule5Tracker::Core tracker{};
	Rule5Tracker track{ tracker };

	{
		Alt::Optional<Rule5Tracker> opt0 = track; 

		EXPECT_TRUE(opt0);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);

		Alt::Optional<Rule5Tracker> opt1 = std::move(opt0);

		// opt0 has value - like with 'std::optional'
		EXPECT_TRUE(opt0);

		EXPECT_TRUE(opt1);

		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 0);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);

		opt0 = std::move(opt1);

		EXPECT_TRUE(opt0);

		// opt1 has value - like with 'std::optional'
		EXPECT_TRUE(opt1);

		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 1);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 0);

		opt1 = opt0;
		EXPECT_TRUE(opt0);
		EXPECT_TRUE(opt1);

		EXPECT_EQ(tracker.cntMoveConstruct, 1);
		EXPECT_EQ(tracker.cntMoveAssignment, 1);
		EXPECT_EQ(tracker.cntCopyConstruct, 1);
		EXPECT_EQ(tracker.cntCopyAssignment, 1);

		opt1 = Alt::Nullopt;
	}

	EXPECT_EQ(tracker.cntDestructor, 2);
}

TEST(Optional, Clone) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::Optional<Alt::Vector<usize, 0>> opt0 = Alt::Vector<usize>::AllocatePush(0, 1) iferr_return;
		Alt::Optional<Alt::Vector<usize, 1>> opt1;

		opt1 = std::move(opt0);

		ASSERT_TRUE(opt0);
		ASSERT_TRUE(opt1);

		EXPECT_TRUE(opt0->IsEmpty());
		EXPECT_TRUE(opt1->IsPopulated());

		opt0.CloneFrom(opt1) iferr_return;

		ASSERT_TRUE(opt0);
		ASSERT_TRUE(opt1);

		EXPECT_TRUE(opt0->IsPopulated());
		EXPECT_TRUE(opt1->IsPopulated());
		EXPECT_EQ(*opt0, *opt1);

		opt0 = Alt::Nullopt;
		opt0.CloneFrom(opt1) iferr_return;

		ASSERT_TRUE(opt0);
		ASSERT_TRUE(opt1);

		EXPECT_TRUE(opt0->IsPopulated());
		EXPECT_TRUE(opt1->IsPopulated());
		EXPECT_EQ(*opt0, *opt1);

		//Alt::Optional<Alt::Vector<usize, 2>> opt2 = opt1; // compilation error
		Alt::Optional<Alt::Vector<usize, 4>> opt2 = opt1.Clone() iferr_return;

		ASSERT_TRUE(opt1);
		ASSERT_TRUE(opt2);

		EXPECT_TRUE(opt1->IsPopulated());
		EXPECT_TRUE(opt2->IsPopulated());
		EXPECT_EQ(*opt1, *opt2);
	}

	{
		usize x = 4;
		Alt::Optional<usize&> opt0 = x;
		Alt::Optional<usize&> opt1;

		opt1 = std::move(opt0);

		EXPECT_TRUE(opt0);
		ASSERT_TRUE(opt1);

		EXPECT_EQ(*opt1, 4);

		opt0.CloneFrom(opt1) iferr_return;

		ASSERT_TRUE(opt0);
		ASSERT_TRUE(opt1);

		EXPECT_EQ(*opt0, 4);
		EXPECT_EQ(*opt1, 4);

		opt0 = Alt::Nullopt;
		opt0.CloneFrom(opt1) iferr_return;

		ASSERT_TRUE(opt0);
		ASSERT_TRUE(opt1);

		EXPECT_EQ(*opt0, 4);
		EXPECT_EQ(*opt1, 4);

		Alt::Optional<usize&> opt2 = opt1.Clone() iferr_return;

		ASSERT_TRUE(opt1);
		ASSERT_TRUE(opt2);

		EXPECT_EQ(*opt1, 4);
		EXPECT_EQ(*opt2, 4);
	}
}

#pragma warning(pop)