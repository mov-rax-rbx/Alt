#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/String/String.h"
#include "../alt-std/src/Memory/UniquePtr.h"
#include "../alt-std/src/Platform/Platform.h"

namespace {

	static usize s_Counter = 0;

	struct UniquePtrDeleter {

		template <typename T>
		void operator()(T* ptr) const noexcept {

			Alt::Memory::DeallocateObject(ptr);
			s_Counter += 1;
		}
	};
}

TEST(UniquePtr, TypeTraits) {

	bool v = Alt::TypeTraits::is_smart_pointer_v<Alt::UniquePtr<usize>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<UniquePtrDeleter>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::UniquePtr<usize, UniquePtrDeleter>>;
	EXPECT_TRUE(v);
}

TEST(UniquePtr, Ownership) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::UniquePtr<usize> ptr0 = Alt::UniquePtr<usize>::AllocateObject((usize)5) iferr_return;
	EXPECT_TRUE(ptr0);
	EXPECT_EQ(*ptr0, 5);

	Alt::UniquePtr<usize> ptr1 = ptr0.Release();
	EXPECT_FALSE(ptr0);

	EXPECT_TRUE(ptr1);
	EXPECT_EQ(*ptr1, 5);

	ptr1.Reset(Alt::Memory::AllocateObject<usize>((usize)10));
	EXPECT_TRUE(ptr1);
	EXPECT_EQ(*ptr1, 10);

	Alt::String str = "UniquePtr"_salloc;
	Alt::UniquePtr<Alt::String> ptr2 = Alt::UniquePtr<Alt::String>::AllocateObject(str.Clone().GetValueOrDefault()) iferr_return;
	EXPECT_EQ(ptr2->GetSizeBytes(), str.GetSizeBytes());

	ptr0 = std::move(ptr1);
	EXPECT_TRUE(ptr0);
	EXPECT_EQ(*ptr0, 10);

	//Alt::UniquePtr<usize[]> ptr3{ nullptr }; // compilation error
}

TEST(UniquePtr, Deleter) {

	iferr_scope_on_err {

		FAIL();
	};
	
	{
		Alt::UniquePtr<usize, UniquePtrDeleter> ptr0 = Alt::UniquePtr<usize, UniquePtrDeleter>::AllocateObject((usize)5) iferr_return;
		Alt::UniquePtr<Alt::String, UniquePtrDeleter> ptr1 = Alt::UniquePtr<Alt::String, UniquePtrDeleter>::AllocateObject("UniquePtr"_salloc) iferr_return;
	}

	EXPECT_EQ(s_Counter, 2);
}
