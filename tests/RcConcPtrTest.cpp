#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Memory/RcConcPtr.h"

#include "../alt-std/src/Collections/StaticVector.h"

#include "../alt-std/src/Concurrency/Atomic.h"
#include "../alt-std/src/Concurrency/Thread.h"
#include "../alt-std/src/Concurrency/Semaphore.h"

#include "../alt-std/src/Platform/Platform.h"

/*
 * Below the tests which are identical to the 'RcPtr'; Additionaly added section with threads.
 */

namespace {

	static usize s_Counter = 0;
	static usize s_WeakCounter = 0;
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
	struct RcPtrControllerWeak {

		struct StrongRefData;

		using StrongRef = StrongRefData;
		using StrongCounter = usize;
		using WeakCounter = usize;

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
			s_WeakCounter += 1;

			return strongRef;
		}

		ALT_FORCE_INLINE static void DeallocateObject(StrongRef& strongRef) noexcept {

			Alt::Memory::DeallocateObject(strongRef.pObject);
		}

		ALT_FORCE_INLINE static void DeallocateWeakCounter(StrongRef&) noexcept {
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
			return strongRef.pObject ? Alt::MakeOptional(s_WeakCounter) : Alt::Nullopt;
		}

		ALT_FORCE_INLINE static Alt::Optional<const WeakCounter&> GetWeakCounterRef(const StrongRef& strongRef) noexcept {
			return strongRef.pObject ? Alt::MakeOptional(s_WeakCounter) : Alt::Nullopt;
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

TEST(RcConcPtr, TypeTraits) {

	bool v = Alt::TypeTraits::is_smart_pointer_v<Alt::RcConcPtr<usize>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<RcPtrController<usize>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::None, RcPtrController<usize>>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_smart_pointer_v<Alt::RcConcWeakPtr<usize>>;
	EXPECT_FALSE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::RcConcWeakPtr<usize>>;
	EXPECT_TRUE(v);
}

TEST(RcConcPtr, OwnershipIntrusive) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::All, Alt::RcConcIntrusiveMemoryLayoutController<usize, true>>;
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

TEST(RcConcPtr, OwnershipControlBlock) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::All, Alt::RcControlBlockMemoryLayoutController<usize, true>>;
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

TEST(RcConcPtr, OwnershipIntrusiveWeak) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::All, Alt::RcConcIntrusiveMemoryLayoutController<usize, true>>;
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

TEST(RcConcPtr, OwnershipControlBlockWeak) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::All, Alt::RcControlBlockMemoryLayoutController<usize, true>>;
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

TEST(RcConcPtr, CustomController) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::None, RcPtrController<usize>>;

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

TEST(RcConcPtr, StrongOverflowCheck) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::OverflowCheck, RcPtrControllerOverflowCheck<usize>>;

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

TEST(RcConcPtr, WeakOverflowCheck) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::OverflowCheck | Alt::RcConcPtrTrait::WeakPtr, RcPtrControllerWeakOverflowCheck<usize>>;

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

TEST(RcConcPtr, Thread0_0) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::OverflowCheck | Alt::RcConcPtrTrait::WeakPtr, RcPtrControllerWeakOverflowCheck<usize>>;

	RcPtr rcPtr = RcPtr::AllocateObject(4) iferr_return;
	Alt::Semaphore sem = Alt::Semaphore::Create() iferr_return;

	Alt::Thread thread0 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.Clone()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	Alt::Thread thread1 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.Clone()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 3);

	sem.Signal(2) iferr_panic_only_dev("ignore");

	thread0.WaitCompletion() iferr_return;
	thread1.WaitCompletion() iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
}

TEST(RcConcPtr, Thread0_1) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::WeakPtr, RcPtrControllerWeakOverflowCheck<usize>>;

	RcPtr rcPtr = RcPtr::AllocateObject(4) iferr_return;
	Alt::Semaphore sem = Alt::Semaphore::Create() iferr_return;

	Alt::Thread thread0 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.Clone()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	Alt::Thread thread1 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.Clone()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 3);

	sem.Signal(2) iferr_panic_only_dev("ignore");

	thread0.WaitCompletion() iferr_return;
	thread1.WaitCompletion() iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
}

TEST(RcConcPtr, Thread0_2) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::None, RcPtrController<usize>>;

	RcPtr rcPtr = RcPtr::AllocateObject(4) iferr_return;
	Alt::Semaphore sem = Alt::Semaphore::Create() iferr_return;

	Alt::Thread thread0 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.Clone()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	Alt::Thread thread1 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.Clone()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 3);

	sem.Signal(2) iferr_panic_only_dev("ignore");

	thread0.WaitCompletion() iferr_return;
	thread1.WaitCompletion() iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
}

TEST(RcConcPtr, Thread0_3) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::None, RcPtrController<usize>>;

	RcPtr rcPtr = RcPtr::AllocateObject(4) iferr_return;
	Alt::Semaphore sem = Alt::Semaphore::Create() iferr_return;

	Alt::Thread thread0 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.Clone()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	Alt::Thread thread1 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.Clone()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 3);

	sem.Signal(2) iferr_panic_only_dev("ignore");

	thread0.WaitCompletion() iferr_return;
	thread1.WaitCompletion() iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
}

TEST(RcConcPtr, Thread1_0) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::OverflowCheck | Alt::RcConcPtrTrait::WeakPtr, RcPtrControllerWeakOverflowCheck<usize>>;

	RcPtr rcPtr = RcPtr::AllocateObject(4) iferr_return;
	Alt::Semaphore sem = Alt::Semaphore::Create() iferr_return;

	Alt::Thread thread0 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.AsWeak()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	Alt::Thread thread1 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.AsWeak()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
	EXPECT_EQ(rcPtr.GetWeakCount(), 3);

	sem.Signal(2) iferr_panic_only_dev("ignore");

	thread0.WaitCompletion() iferr_return;
	thread1.WaitCompletion() iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
	EXPECT_EQ(rcPtr.GetWeakCount(), 1);
}

TEST(RcConcPtr, Thread1_1) {

	iferr_scope_on_err {

		FAIL();
	};

	using RcPtr = Alt::RcConcPtr<usize, Alt::RcConcPtrTrait::WeakPtr, RcPtrControllerWeakOverflowCheck<usize>>;

	RcPtr rcPtr = RcPtr::AllocateObject(4) iferr_return;
	Alt::Semaphore sem = Alt::Semaphore::Create() iferr_return;

	Alt::Thread thread0 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.AsWeak()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	Alt::Thread thread1 = Alt::Thread::Allocate([&sem, rcPtr = rcPtr.AsWeak()]() {
		iferr_scope;

		sem.Wait() iferr_panic_only_dev("ignore");

	}) iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
	EXPECT_EQ(rcPtr.GetWeakCount(), 3);

	sem.Signal(2) iferr_panic_only_dev("ignore");

	thread0.WaitCompletion() iferr_return;
	thread1.WaitCompletion() iferr_return;

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
	EXPECT_EQ(rcPtr.GetWeakCount(), 1);
}

namespace {

template <usize THREAD_SIZE, usize PONG_SIZE, typename RcPtr>
void Pong0(RcPtr& rcPtr) noexcept {
	iferr_scope;

	Alt::StaticVector<Alt::Thread, THREAD_SIZE> threads;

	for (usize i = 0; i < THREAD_SIZE; ++i) {

		Alt::Thread thread = Alt::Thread::Allocate([rcPtr_ = rcPtr.Clone()]() mutable {
			iferr_scope;

			RcPtr rcPtr = std::move(rcPtr_) iferr_panic_silent;

			const usize val = rcPtr->Load();
			if (val == PONG_SIZE) {
				return;
			}

			rcPtr->FetchInc();
			Pong0<THREAD_SIZE, PONG_SIZE>(rcPtr);

		}) iferr_panic_silent;

		threads.Push(std::move(thread)) iferr_panic_only_dev("increase vector size");
	}

	for (const Alt::Thread& thread : threads.GetIterator()) {

		thread.WaitCompletion();
	}
}

template <usize THREAD_SIZE, usize PONG_SIZE, typename RcWeakPtr>
void Pong1(RcWeakPtr& weakPtr) noexcept {
	iferr_scope;

	Alt::StaticVector<Alt::Thread, THREAD_SIZE> threads;

	for (usize i = 0; i < THREAD_SIZE; ++i) {

		Alt::Thread thread = Alt::Thread::Allocate([weakPtr_ = weakPtr.Clone()]() mutable {
			iferr_scope;

			using RcPtr = typename RcWeakPtr::RcPtr;

			RcWeakPtr weakPtr = std::move(weakPtr_) iferr_panic_silent;
			RcPtr rcPtr = weakPtr.AsRc() iferr_panic_only_dev("bug !!");

			const usize val = rcPtr->Load();
			if (val == PONG_SIZE) {
				return;
			}

			rcPtr->FetchInc();
			Pong1<THREAD_SIZE, PONG_SIZE>(weakPtr);

		}) iferr_panic_silent;

		threads.Push(std::move(thread)) iferr_panic_only_dev("increase vector size");
	}

	for (const Alt::Thread& thread : threads.GetIterator()) {

		thread.WaitCompletion();
	}
}
}

TEST(RcConcPtr, StressStrongRef0) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize THREAD_SIZE = 4;
	constexpr usize PONG_SIZE = 64 * THREAD_SIZE;

	using RcPtr = Alt::RcConcPtr<
		Alt::AtomicUsize,
		Alt::RcConcPtrTrait::OverflowCheck | Alt::RcConcPtrTrait::WeakPtr,
		RcPtrControllerWeak<Alt::AtomicUsize>
	>;

	RcPtr rcPtr = RcPtr::AllocateObject(0) iferr_return;

	Pong0<THREAD_SIZE, PONG_SIZE>(rcPtr);

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
	EXPECT_EQ(rcPtr.GetWeakCount(), 1);
	EXPECT_EQ(rcPtr->Load(), PONG_SIZE);
}

TEST(RcConcPtr, StressStrongRef1) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize THREAD_SIZE = 4;
	constexpr usize PONG_SIZE = 64 * THREAD_SIZE;

	using RcPtr = Alt::RcConcPtr<
		Alt::AtomicUsize,
		Alt::RcConcPtrTrait::None,
		RcPtrController<Alt::AtomicUsize>
	>;

	RcPtr rcPtr = RcPtr::AllocateObject(0) iferr_return;

	Pong0<THREAD_SIZE, PONG_SIZE>(rcPtr);

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
	EXPECT_EQ(rcPtr->Load(), PONG_SIZE);
}

TEST(RcConcPtr, StressWeakRef0) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize THREAD_SIZE = 4;
	constexpr usize PONG_SIZE = 64 * THREAD_SIZE;

	using RcPtr = Alt::RcConcPtr<
		Alt::AtomicUsize,
		Alt::RcConcPtrTrait::OverflowCheck | Alt::RcConcPtrTrait::WeakPtr,
		RcPtrControllerWeak<Alt::AtomicUsize>
	>;

	RcPtr rcPtr = RcPtr::AllocateObject(0) iferr_return;

	{
		RcPtr::WeakPtr weakPtr = rcPtr.AsWeak() iferr_return;
		Pong1<THREAD_SIZE, PONG_SIZE>(weakPtr);
	}

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
	EXPECT_EQ(rcPtr.GetWeakCount(), 1);
	EXPECT_EQ(rcPtr->Load(), PONG_SIZE);
}

TEST(RcConcPtr, StressWeakRef1) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize THREAD_SIZE = 4;
	constexpr usize PONG_SIZE = 64 * THREAD_SIZE;

	using RcPtr = Alt::RcConcPtr<
		Alt::AtomicUsize,
		Alt::RcConcPtrTrait::WeakPtr,
		RcPtrControllerWeak<Alt::AtomicUsize>
	>;

	RcPtr rcPtr = RcPtr::AllocateObject(0) iferr_return;

	{
		RcPtr::WeakPtr weakPtr = rcPtr.AsWeak();
		Pong1<THREAD_SIZE, PONG_SIZE>(weakPtr);
	}

	EXPECT_EQ(rcPtr.GetStrongCount(), 1);
	EXPECT_EQ(rcPtr.GetWeakCount(), 1);
	EXPECT_EQ(rcPtr->Load(), PONG_SIZE);
}
