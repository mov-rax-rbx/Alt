#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Memory/RcPtr.h"
#include "../alt-std/src/Collections/StaticVector.h"
#include "../alt-std/src/Platform/Platform.h"

namespace {

	static usize s_Counter = 0;
	static u8 s_OverflowCounter = 0;
	static Alt::Optional<u8> s_WeakOverflowCounter = Alt::Nullopt;

	template <typename T>
	struct RcPtrController {

		struct StrongRefData;

		using StrongRef = StrongRefData;
		using StrongCounter = usize;

		struct StrongRefData {

			T* pObject = nullptr;

			ALT_FORCE_INLINE StrongRefData() noexcept = default;

			ALT_FORCE_INLINE StrongRefData(T* pObject) noexcept
				: pObject(pObject)
			{}
		};

		template <typename ...ArgsT>
		ALT_FORCE_INLINE static Alt::Optional<StrongRef> AllocateStrongWeakRefAndSet(ArgsT&&... args) noexcept {
			iferr_scope;

			StrongRef strongRef{};
			strongRef.pObject = Alt::Memory::AllocateObject<T>(std::forward<ArgsT>(args)...) iferr_return Alt::Nullopt;

			s_Counter += 1;

			return strongRef;
		}

		ALT_FORCE_INLINE static void DeallocateStrongRef(StrongRef& strongRef) noexcept {

			Alt::Memory::DeallocateObject(strongRef.pObject);
		}

		ALT_FORCE_INLINE static Alt::Optional<T*> GetObjectPtr(StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(strongRef.pObject) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static const Alt::Optional<T*> GetObjectPtr(const StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(strongRef.pObject) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<StrongCounter&> GetStrongCounterRef(StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(s_Counter) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<const StrongCounter&> GetStrongCounterRef(const StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(s_Counter) : Alt::Nullopt;
		}
	};

	template <typename T>
	struct RcPtrControllerOverflowCheck {

		struct StrongRefData;

		using StrongRef = StrongRefData;
		using StrongCounter = u8;

		struct StrongRefData {

			T* pObject = nullptr;

			ALT_FORCE_INLINE StrongRefData() noexcept = default;

			ALT_FORCE_INLINE StrongRefData(T* pObject) noexcept
				: pObject(pObject)
			{}
		};

		template <typename ...ArgsT>
		ALT_FORCE_INLINE static Alt::Optional<StrongRef> AllocateStrongWeakRefAndSet(ArgsT&&... args) noexcept {
			iferr_scope;

			StrongRef strongRef{};
			strongRef.pObject = Alt::Memory::AllocateObject<T>(std::forward<ArgsT>(args)...) iferr_return Alt::Nullopt;

			s_OverflowCounter += 1;

			return strongRef;
		}

		ALT_FORCE_INLINE static void DeallocateStrongRef(StrongRef& strongRef) noexcept {

			Alt::Memory::DeallocateObject(strongRef.pObject);
		}

		ALT_FORCE_INLINE static Alt::Optional<T*> GetObjectPtr(StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(strongRef.pObject) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static const Alt::Optional<T*> GetObjectPtr(const StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(strongRef.pObject) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<StrongCounter&> GetStrongCounterRef(StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(s_OverflowCounter) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<const StrongCounter&> GetStrongCounterRef(const StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(s_OverflowCounter) : Alt::Nullopt;
		}
	};

	template <typename T>
	struct RcPtrControllerWeakOverflowCheck {

		struct StrongRefData;

		using StrongRef = StrongRefData;
		using StrongCounter = usize;
		using WeakCounter = u8;

		struct StrongRefData {

			T* pObject = nullptr;

			ALT_FORCE_INLINE StrongRefData() noexcept = default;

			ALT_FORCE_INLINE StrongRefData(T* pObject) noexcept
				: pObject(pObject)
			{}
		};

		template <typename ...ArgsT>
		ALT_FORCE_INLINE static Alt::Optional<StrongRef> AllocateStrongWeakRefAndSet(ArgsT&&... args) noexcept {
			iferr_scope;

			StrongRef strongRef{};
			strongRef.pObject = Alt::Memory::AllocateObject<T>(std::forward<ArgsT>(args)...) iferr_return Alt::Nullopt;

			s_Counter += 1;
			s_WeakOverflowCounter = (u8)1;

			return strongRef;
		}

		ALT_FORCE_INLINE static void DeallocateObject(StrongRef& strongRef) noexcept {

			Alt::Memory::DeallocateObject(strongRef.pObject);
		}

		ALT_FORCE_INLINE static void DeallocateWeakCounter(StrongRef&) noexcept {

			s_WeakOverflowCounter = Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<T*> GetObjectPtr(StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(strongRef.pObject) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static const Alt::Optional<T*> GetObjectPtr(const StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(strongRef.pObject) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<StrongCounter&> GetStrongCounterRef(StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(s_Counter) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<const StrongCounter&> GetStrongCounterRef(const StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(s_Counter) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<WeakCounter&> GetWeakCounterRef(StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(s_WeakOverflowCounter.GetValue()) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<const WeakCounter&> GetWeakCounterRef(const StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(s_WeakOverflowCounter.GetValue()) : Alt::Nullopt;
		}
	};
}

TEST(RcPtr, TypeTraits) {

	bool v = Alt::TypeTraits::is_smart_pointer_v<Alt::RcPtr<usize>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<RcPtrController<usize>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::RcPtr<usize, Alt::RcPtrTrait::None, RcPtrController<usize>>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_smart_pointer_v<Alt::RcWeakPtr<usize>>;
	EXPECT_FALSE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::RcWeakPtr<usize>>;
	EXPECT_TRUE(v);
}

TEST(RcPtr, OwnershipIntrusive) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcPtr<usize, Alt::RcPtrTrait::All, Alt::RcIntrusiveMemoryLayoutController<usize, true>>;
	using RcWeakPtr = RcPtr::WeakPtr;

	RcWeakPtr ptrWeak;
	{
		RcPtr ptr0 = nullptr;
		EXPECT_FALSE(ptr0);
		EXPECT_EQ(ptr0.GetStrongCount(), 0);

		ptr0 = RcPtr::AllocateObject(4) iferr_return;
		ASSERT_TRUE(ptr0);
		EXPECT_EQ(ptr0.GetStrongCount(), 1);
		EXPECT_EQ(*ptr0, 4);

		RcPtr ptr1 = ptr0.Clone() iferr_return;
		ASSERT_TRUE(ptr1);
		EXPECT_EQ(ptr0.GetStrongCount(), 2);
		EXPECT_EQ(ptr1.GetStrongCount(), 2);
		EXPECT_EQ(*ptr1, 4);

		RcPtr ptr2 = std::move(ptr1);
		ASSERT_TRUE(ptr2);
		EXPECT_EQ(ptr0.GetStrongCount(), 2);
		EXPECT_EQ(ptr2.GetStrongCount(), 2);
		EXPECT_EQ(*ptr2, 4);

		ptr0.Reset() iferr_return;
		EXPECT_FALSE(ptr0);
		EXPECT_EQ(ptr0.GetStrongCount(), 0);

		EXPECT_EQ(ptr2.GetStrongCount(), 1);

		ptrWeak = ptr2.AsWeak() iferr_return;
	}

	EXPECT_EQ(ptrWeak.GetStrongCount(), 0);
	EXPECT_EQ(ptrWeak.GetWeakCount(), 1);

	EXPECT_FALSE(ptrWeak.AsRc());
}

TEST(RcPtr, OwnershipControlBlock) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcPtr<usize, Alt::RcPtrTrait::All, Alt::RcControlBlockMemoryLayoutController<usize, true>>;
	using RcWeakPtr = RcPtr::WeakPtr;

	RcWeakPtr ptrWeak;
	{
		RcPtr ptr0 = nullptr;
		EXPECT_FALSE(ptr0);
		EXPECT_EQ(ptr0.GetStrongCount(), 0);

		ptr0 = RcPtr::AllocateObject(4) iferr_return;
		ASSERT_TRUE(ptr0);
		EXPECT_EQ(ptr0.GetStrongCount(), 1);
		EXPECT_EQ(*ptr0, 4);

		RcPtr ptr1 = ptr0.Clone() iferr_return;
		ASSERT_TRUE(ptr1);
		EXPECT_EQ(ptr0.GetStrongCount(), 2);
		EXPECT_EQ(ptr1.GetStrongCount(), 2);
		EXPECT_EQ(*ptr1, 4);

		RcPtr ptr2 = std::move(ptr1);
		ASSERT_TRUE(ptr2);
		EXPECT_EQ(ptr0.GetStrongCount(), 2);
		EXPECT_EQ(ptr2.GetStrongCount(), 2);
		EXPECT_EQ(*ptr2, 4);

		ptr0.Reset() iferr_return;
		EXPECT_FALSE(ptr0);
		EXPECT_EQ(ptr0.GetStrongCount(), 0);

		EXPECT_EQ(ptr2.GetStrongCount(), 1);

		ptrWeak = ptr2.AsWeak() iferr_return;
	}

	EXPECT_EQ(ptrWeak.GetStrongCount(), 0);
	EXPECT_EQ(ptrWeak.GetWeakCount(), 1);

	EXPECT_FALSE(ptrWeak.AsRc());
}

TEST(RcPtr, OwnershipIntrusiveWeak) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcPtr<usize, Alt::RcPtrTrait::All, Alt::RcIntrusiveMemoryLayoutController<usize, true>>;
	using RcWeakPtr = RcPtr::WeakPtr;

	RcWeakPtr ptrWeak;
	{
		RcPtr ptr0 = RcPtr::AllocateObject(4) iferr_return;

		ASSERT_TRUE(ptr0);
		EXPECT_EQ(*ptr0, 4);
		EXPECT_EQ(ptr0.GetStrongCount(), 1);
		EXPECT_EQ(ptr0.GetWeakCount(), 1);

		RcPtr ptr1 = ptr0.Clone() iferr_return;
		EXPECT_EQ(*ptr1, 4);
		EXPECT_EQ(ptr1.GetStrongCount(), 2);
		EXPECT_EQ(ptr1.GetWeakCount(), 1);

		RcWeakPtr ptr0Weak = ptr0.AsWeak() iferr_return;
		EXPECT_EQ(ptr0Weak.GetStrongCount(), 2);
		EXPECT_EQ(ptr0Weak.GetWeakCount(), 2);

		RcWeakPtr ptr1Weak = ptr0Weak.Clone() iferr_return;
		EXPECT_EQ(ptr0Weak.GetStrongCount(), 2);
		EXPECT_EQ(ptr0Weak.GetWeakCount(), 3);

		ptrWeak = ptr1Weak.Clone() iferr_return;

		RcPtr ptr2 = ptrWeak.AsRc() iferr_return;
		EXPECT_EQ(ptr2.GetStrongCount(), 3);
		EXPECT_EQ(ptr2.GetWeakCount(), 4);

		ptr1 = ptr2.Clone() iferr_return;
		EXPECT_EQ(ptr2.GetStrongCount(), 3);
		EXPECT_EQ(ptr2.GetWeakCount(), 4);
	}

	EXPECT_EQ(ptrWeak.GetStrongCount(), 0);
	EXPECT_EQ(ptrWeak.GetWeakCount(), 1);

	EXPECT_FALSE(ptrWeak.AsRc());
}

TEST(RcPtr, OwnershipControlBlockWeak) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcPtr<usize, Alt::RcPtrTrait::All, Alt::RcControlBlockMemoryLayoutController<usize, true>>;
	using RcWeakPtr = RcPtr::WeakPtr;

	RcWeakPtr ptrWeak;
	{
		RcPtr ptr0 = RcPtr::AllocateObject(4) iferr_return;

		ASSERT_TRUE(ptr0);
		EXPECT_EQ(*ptr0, 4);
		EXPECT_EQ(ptr0.GetStrongCount(), 1);
		EXPECT_EQ(ptr0.GetWeakCount(), 1);

		RcPtr ptr1 = ptr0.Clone() iferr_return;
		EXPECT_EQ(*ptr1, 4);
		EXPECT_EQ(ptr1.GetStrongCount(), 2);
		EXPECT_EQ(ptr1.GetWeakCount(), 1);

		RcWeakPtr ptr0Weak = ptr0.AsWeak() iferr_return;
		EXPECT_EQ(ptr0Weak.GetStrongCount(), 2);
		EXPECT_EQ(ptr0Weak.GetWeakCount(), 2);

		RcWeakPtr ptr1Weak = ptr0Weak.Clone() iferr_return;
		EXPECT_EQ(ptr0Weak.GetStrongCount(), 2);
		EXPECT_EQ(ptr0Weak.GetWeakCount(), 3);

		ptrWeak = ptr1Weak.Clone() iferr_return;

		RcPtr ptr2 = ptrWeak.AsRc() iferr_return;
		EXPECT_EQ(ptr2.GetStrongCount(), 3);
		EXPECT_EQ(ptr2.GetWeakCount(), 4);

		ptr1 = ptr2.Clone() iferr_return;
		EXPECT_EQ(ptr2.GetStrongCount(), 3);
		EXPECT_EQ(ptr2.GetWeakCount(), 4);
	}

	EXPECT_EQ(ptrWeak.GetStrongCount(), 0);
	EXPECT_EQ(ptrWeak.GetWeakCount(), 1);

	EXPECT_FALSE(ptrWeak.AsRc());
}

TEST(RcPtr, CustomController) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		using RcPtr = Alt::RcPtr<usize, Alt::RcPtrTrait::None, RcPtrController<usize>>;

		RcPtr ptr0 = RcPtr::AllocateObjectWithController(RcPtrController<usize>{}, 4) iferr_return;

		ASSERT_TRUE(ptr0);
		EXPECT_EQ(*ptr0, 4);
		EXPECT_EQ(ptr0.GetStrongCount(), 1);
		EXPECT_EQ(s_Counter, 1);

		RcPtr ptr1 = ptr0;
		EXPECT_EQ(*ptr1, 4);
		EXPECT_EQ(ptr1.GetStrongCount(), 2);
		EXPECT_EQ(s_Counter, 2);
	}

	EXPECT_EQ(s_Counter, 0);
}

TEST(RcPtr, StrongOverflowCheck) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcPtr<usize, Alt::RcPtrTrait::OverflowCheck, RcPtrControllerOverflowCheck<usize>>;

	{
		RcPtr ptr = RcPtr::AllocateObject(4) iferr_return;
		Alt::StaticVector<RcPtr, Alt::Num::Desc<u8>::Max() - 1> ptrs;

		{
			for (usize i = 0; i < ptrs.GetCapacity(); ++i) {

				RcPtr cloned = ptr.Clone() iferr_return;
				ptrs.Push(std::move(cloned)) iferr_return;
			}

			EXPECT_EQ(ptr.GetStrongCount(), Alt::Num::Desc<u8>::Max());
			EXPECT_FALSE(ptr.Clone());
		}
	}

	EXPECT_EQ(s_OverflowCounter, 0);
}

TEST(RcPtr, WeakOverflowCheck) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcPtr<usize, Alt::RcPtrTrait::OverflowCheck | Alt::RcPtrTrait::WeakPtr, RcPtrControllerWeakOverflowCheck<usize>>;

	{
		RcPtr ptr = RcPtr::AllocateObject(4) iferr_return;

		// -2 because strong counter always increment weak counter
		Alt::StaticVector<RcPtr::WeakPtr, Alt::Num::Desc<u8>::Max() - 2> ptrs;

		RcPtr::WeakPtr weakPtr = ptr.AsWeak() iferr_return;

		{
			for (usize i = 0; i < ptrs.GetCapacity(); ++i) {

				RcPtr::WeakPtr cloned = weakPtr.Clone() iferr_return;
				ptrs.Push(std::move(cloned)) iferr_return;
			}

			EXPECT_EQ(ptr.GetStrongCount(), 1);

			EXPECT_EQ(ptr.GetWeakCount(), Alt::Num::Desc<u8>::Max());
			EXPECT_EQ(weakPtr.GetWeakCount(), Alt::Num::Desc<u8>::Max());
			EXPECT_FALSE(weakPtr.Clone());
		}
	}

	EXPECT_EQ(s_Counter, 0);
	EXPECT_FALSE(s_WeakOverflowCounter);
}
