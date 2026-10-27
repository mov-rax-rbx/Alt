#include "pch.h"

#include "Utils.h"

#include "../alt-std/src/Memory/Ref.h"
#include "../alt-std/src/Memory/Ptr.h"

#pragma warning(push)
#pragma warning(disable: 4189 /* variable initialized but not referenced */)

TEST(RefPtr, RefTypeTraits) {

	bool v = false;

	{
		v = Alt::TypeTraits::is_alt_ref_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::Ref<const volatile void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Ref<const volatile void>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Ref<const volatile void>>;
		EXPECT_TRUE(v);
	}

	{
		v = Alt::TypeTraits::is_alt_ref_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copyable_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_default_constructible_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::Ref<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);
	}
}

TEST(RefPtr, PtrTypeTraits) {

	bool v = false;

	{
		v = Alt::TypeTraits::is_alt_ref_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_default_constructible_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::_RefDetails::Ptr<const volatile void>>;
		EXPECT_TRUE(v);
	}

	{
		v = Alt::TypeTraits::is_alt_ref_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_trivially_copyable_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_default_constructible_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_default_constructible_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_FALSE(v);

		v = std::is_copy_constructible_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_constructible_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_copy_assignable_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_copy_assignable_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_constructible_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_constructible_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_move_assignable_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_move_assignable_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_destructible_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);

		v = std::is_trivially_destructible_v<Alt::_RefDetails::Ptr<Rule5AllNonTrivial>>;
		EXPECT_TRUE(v);
	}
}

TEST(RefPtr, Ref) {

	{
		Rule5AllNonTrivial val;
		const Rule5AllNonTrivial valConst;

		Alt::Ref<Rule5AllNonTrivial> ref0 = val;
		const Alt::Ref<Rule5AllNonTrivial> ref1 = ref0;
		Alt::Ref<const Rule5AllNonTrivial> ref2 = ref0;

		Alt::Ref<Rule5AllNonTrivial> ref3 = ref0;
		Alt::Ref<const Rule5AllNonTrivial> ref4 = std::move(ref3);

		Rule5AllNonTrivial& strRef0 = ref0;
		const Rule5AllNonTrivial& strRef1 = ref1;
		const Rule5AllNonTrivial& strRef2 = ref2;

		// overwrite whole value
		ref0 = Rule5AllNonTrivial{};

		// overwrite just pointer inside
		ref2 = Alt::Ref<const Rule5AllNonTrivial>{ valConst };
	}

	{
		Alt::String str = "String"_salloc;

		Alt::Ref<Alt::String> ref0 = str;

		// copy!
		//Alt::String str0 = std::move(ref0);
		Alt::String str0 = std::move(ref0)->Clone().GetValueOrDefault();

		EXPECT_TRUE(str.IsPopulated());
		EXPECT_TRUE(str0.IsPopulated());
		EXPECT_EQ(str, str0);

		Alt::Ref<Alt::String> ref1 = str0;

		// move!
		Alt::String str1 = std::move(ref0.GetRef());
		Alt::String str2 = std::move(*ref1);

		EXPECT_TRUE(str.IsEmpty());
		EXPECT_TRUE(str0.IsEmpty());

		EXPECT_TRUE(str1.IsPopulated());
		EXPECT_TRUE(str2.IsPopulated());
	}
}

TEST(RefPtr, Ptr) {

	Rule5AllNonTrivial val;
	const Rule5AllNonTrivial valConst;

	Rule5AllNonTrivial* valPtr = &val;
	const Rule5AllNonTrivial* valConstPtr = &valConst;

	{
		Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr_;

		Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr0 = &val;
		const Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr1 = ptr0;
		Alt::_RefDetails::Ptr<const Rule5AllNonTrivial> ptr2 = ptr0;
		const Rule5AllNonTrivial* _ptr = ptr0;

		Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr3 = ptr0;
		Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr4 = std::move(ptr3);

		EXPECT_TRUE(ptr0 != nullptr);

		// compilation errors
		//Rule5AllNonTrivial& strRef0 = ptr0;
		//const Rule5AllNonTrivial& strRef1 = ptr1;
		//const Rule5AllNonTrivial& strRef2 = ptr2;

		// compilation error
		//ptr0 = Rule5AllNonTrivial{};

		Rule5AllNonTrivial* strPtr0 = ptr0;
		const Rule5AllNonTrivial* strPtr1 = ptr1;
		const Rule5AllNonTrivial* strPtr2 = ptr2;
	}

	{
		Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr0 = valPtr;
		const Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr1 = ptr0;
		Alt::_RefDetails::Ptr<const Rule5AllNonTrivial> ptr2 = ptr0;

		Rule5AllNonTrivial* strPtr0 = ptr0;
		const Rule5AllNonTrivial* strPtr1 = ptr1;
		const Rule5AllNonTrivial* strPtr2 = ptr2;

		ptr2 = valConstPtr;
	}
}

TEST(RefPtr, RefPtr) {

	{
		Rule5AllNonTrivial val;

		Alt::Ref<Rule5AllNonTrivial> ref0 = val;

		Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr0 = ref0;
		const Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr1 = ref0;
		Alt::_RefDetails::Ptr<const Rule5AllNonTrivial> ptr2 = ref0;

		Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr3 = &val;
		Alt::_RefDetails::Ptr<Rule5AllNonTrivial> ptr4 = val; // allowed, because we want to shadow `Ptr` with `Ref` in storages like `Optional`

		ptr2 = ref0;
		ptr0 = ptr1;
		ptr0 = ref0;

		//ref0 = ptr0; // compilation error

		Alt::Ref<Rule5AllNonTrivial> ref1 = *ptr0;

		ptr0 = val; // allowed - just change pointer
		//ptr0 = Rule5AllNonTrivial{}; // compilation error
	}

	{
		Rule5AllNonTrivial* ptr = nullptr;

		Alt::Ref<Rule5AllNonTrivial*> ref = ptr;
		EXPECT_FALSE(ref);

		Rule5AllNonTrivial x{};
		ptr = &x;
		ref = ptr;

		EXPECT_EQ(ref, &x);
	}
}

#pragma warning(pop)