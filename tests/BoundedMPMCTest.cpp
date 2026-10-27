#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Concurrency/Thread.h"
#include "../alt-std/src/Concurrency/Atomic.h"
#include "../alt-std/src/Concurrency/Semaphore.h"
#include "../alt-std/src/Concurrency/BoundedMPMC.h"

#include "../alt-std/src/Memory/UniquePtr.h"
#include "../alt-std/src/Utils/Func.h"

namespace {

//class ITask {
//public:
//
//	virtual void Run() noexcept = 0;
//
//	virtual ~ITask() {}
//};

class Sheduler {
private:

	//using TaskPtr = Alt::UniquePtr<ITask>;
	using Task = Alt::StaticFunc<128, 16, void>;

public:

	ALT_FORCE_INLINE Sheduler() noexcept = default;

	ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize GetMaxNumThreads() noexcept {
		return MAX_THREADS;
	}

	ALT_NODISCARD bool Initialize() noexcept {
		iferr_scope;

		const usize numThreadsIdeal = Alt::Platform::ThreadGetNumOfLogicalProcessors();
		const usize numThreads = Alt::Num::Min(Alt::Num::SubWrap(numThreadsIdeal, 1), MAX_THREADS);

		for (usize i = 0; i < numThreads; ++i) {

			Alt::Thread thread = Alt::Thread::CreateFromNativeFuncPtr([](void* pArgs) noexcept -> Alt::Thread::NativeReturnType {
				iferr_scope;

				ALT_ASSERT(pArgs);

				Sheduler* pSheduler = reinterpret_cast<Sheduler*>(pArgs);

				Alt::Backoff backoff{};

				while (pSheduler->m_SharedState.isRunning.Load(Alt::MemoryOrder::Acquire)) {

					Alt::Optional<Task> task = pSheduler->m_Queue.TryPopFront() ifok {

						//(*taskPtr)->Run();
						(*task)();

						backoff = Alt::Backoff{};
					}
					else {

						backoff.SpinHeavy();
					}
				}

				ALT_THREAD_RETURN_SUCCESS;

			}, this) iferr_return false;

			Alt::Thread::PriorityDesc desc{};
			desc.priority = Alt::Thread::Priority::Highest;
			desc.isBoostEnabled = true;

			thread.SetPriority(desc, (u64)1 << (i % numThreads));

			m_Units.Push(Unit{ std::move(thread) }) iferr_panic_only_dev("increase 'MAX_THREADS'");
		}

		return true;
	}

	void Shutdown() noexcept {

		m_SharedState.isRunning.Store(false, Alt::MemoryOrder::Release);

		for (const Unit& unit : m_Units.GetIterator()) {

			unit.thread.WaitCompletion();
		}
	}

	template <typename FuncT>
	bool Post(FuncT&& func) noexcept {
		//iferr_scope;

		//class TaskFunc : public ITask {
		//public:

		//	TaskFunc(FuncT&& func) noexcept
		//		: m_Func(std::forward<FuncT>(func))
		//	{}

		//	void Run() noexcept override {

		//		(m_Func)();
		//	}

		//public:

		//	std::remove_reference_t<FuncT> m_Func;
		//};

		//TaskFunc task(std::forward<FuncT>(func));
		//TaskPtr taskPtr = Alt::UniquePtr<TaskFunc>::AllocateObject(std::move(task)) iferr_return false;

		Task task = std::forward<FuncT>(func);

		Alt::Backoff backoff{};
		while (!m_Queue.TryPushBackMove(task)) {

			backoff.SpinHeavy();
		}

		return true;
	}

	ALT_NODISCARD ALT_FORCE_INLINE usize GetNumThreads() const noexcept {

		return m_Units.GetSize();
	}

private:

	constexpr static usize MAX_THREADS = 64;

	struct SharedState {

		Alt::AtomicBool isRunning = true;
	};

	struct alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) Unit {

		Alt::Thread thread;
	};

	alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) SharedState m_SharedState;

	//alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) Alt::StaticBoundedMPMC<TaskPtr, 4096> m_Queue;
	alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) Alt::StaticBoundedMPMC<Task, 4096> m_Queue;
	alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) Alt::StaticVector<Unit, MAX_THREADS> m_Units;
};

}

TEST(BoundedMPMC, Usage) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize SIZE_POW2 = 4;

	{
		Alt::BoundedMPMC<usize> queue = Alt::BoundedMPMC<usize>::Allocate(SIZE_POW2) iferr_return;

		//Alt::BoundedMPMC<usize> queue1 = std::move(queue); // compilation error

		EXPECT_EQ(queue.GetCapacity(), SIZE_POW2);

		EXPECT_TRUE(queue.IsEmpty());
		EXPECT_FALSE(queue.IsPopulated());
		EXPECT_EQ(queue.GetSize(), 0);

		queue.TryPushBack(0) iferr_return;
		queue.TryPushBack(1) iferr_return;

		queue.TryPopFront() iferr_return;

		queue.TryPushBack(2) iferr_return;
		queue.TryPushBack(3) iferr_return;
		queue.TryPushBack(4) iferr_return;

		EXPECT_FALSE(queue.TryPushBack(5));

		EXPECT_FALSE(queue.IsEmpty());
		EXPECT_TRUE(queue.IsPopulated());
		EXPECT_EQ(queue.GetSize(), 4);

		EXPECT_EQ(queue.TryPopFront().GetValueOrDefault(), 1);
		EXPECT_EQ(queue.TryPopFront().GetValueOrDefault(), 2);
		EXPECT_EQ(queue.TryPopFront().GetValueOrDefault(), 3);
		EXPECT_EQ(queue.TryPopFront().GetValueOrDefault(), 4);
		EXPECT_FALSE(queue.TryPopFront());

		EXPECT_TRUE(queue.IsEmpty());
		EXPECT_FALSE(queue.IsPopulated());
		EXPECT_EQ(queue.GetSize(), 0);

		queue.TryPushBack(0) iferr_return;
		queue.TryPushBack(1) iferr_return;
	}

	{
		Alt::StaticBoundedMPMC<usize, SIZE_POW2> queue;

		//Alt::StaticBoundedMPMC<usize, SIZE_POW2> queue1 = std::move(queue); // compilation error

		EXPECT_EQ(queue.GetCapacity(), SIZE_POW2);

		EXPECT_TRUE(queue.IsEmpty());
		EXPECT_FALSE(queue.IsPopulated());
		EXPECT_EQ(queue.GetSize(), 0);

		queue.TryPushBack(0) iferr_return;
		queue.TryPushBack(1) iferr_return;

		queue.TryPopFront() iferr_return;

		queue.TryPushBack(2) iferr_return;
		queue.TryPushBack(3) iferr_return;
		queue.TryPushBack(4) iferr_return;

		EXPECT_FALSE(queue.TryPushBack(5));

		EXPECT_FALSE(queue.IsEmpty());
		EXPECT_TRUE(queue.IsPopulated());
		EXPECT_EQ(queue.GetSize(), 4);

		EXPECT_EQ(queue.TryPopFront().GetValueOrDefault(), 1);
		EXPECT_EQ(queue.TryPopFront().GetValueOrDefault(), 2);
		EXPECT_EQ(queue.TryPopFront().GetValueOrDefault(), 3);
		EXPECT_EQ(queue.TryPopFront().GetValueOrDefault(), 4);
		EXPECT_FALSE(queue.TryPopFront());

		EXPECT_TRUE(queue.IsEmpty());
		EXPECT_FALSE(queue.IsPopulated());
		EXPECT_EQ(queue.GetSize(), 0);

		queue.TryPushBack(0) iferr_return;
		queue.TryPushBack(1) iferr_return;
	}
}

TEST(BoundedMPMC, Stress) {

	constexpr static usize REPOST_COUNT = 1'000'000;

	struct RepostJob {

		RepostJob(Sheduler* pSheduler, Alt::Semaphore* pSem) noexcept
			: pSheduler(pSheduler)
			, counter(0)
			, pSem(pSem)
		{
			beginCount = Alt::Platform::GetTicks();
		}

		void operator()() noexcept {
			iferr_scope;

			if (++counter < REPOST_COUNT) {

				if (pSheduler) {

					pSheduler->Post(*this) iferr_panic_only_dev("bug or some allocation issue!");
					return;
				}
			}
			else {

				const u64 endCount = Alt::Platform::GetTicks();

				ALT_PRINTLN("reposted {} in {}ms", counter, (f64)(endCount - beginCount) / 1'000'000.0);

				pSem->Signal(1) iferr_panic_only_dev("ignore");
			}
		}

		Sheduler* pSheduler = nullptr;

		volatile usize counter;
		u64 beginCount;

		Alt::Semaphore* pSem;
	};

	iferr_scope_on_err {

		FAIL();
	};

	Alt::UniquePtr<Sheduler> shedulerPtr = Alt::UniquePtr<Sheduler>::AllocateObject() iferr_return;

	shedulerPtr->Initialize() iferr_return;
	defer{ shedulerPtr->Shutdown(); };

	{
		Alt::StaticVector<Alt::Semaphore, Sheduler::GetMaxNumThreads()> semaphores;
		for (usize i = 0; i < shedulerPtr->GetNumThreads(); ++i) {

			auto sem = Alt::Semaphore::Create() iferr_return;
			semaphores.Push(std::move(sem)) iferr_panic_only_dev("shouldn't panic because of 'Sheduler::GetMaxNumThreads()'");
		}

		//for (;;)
		{
			defer {
				for (Alt::Semaphore& sem : semaphores.GetIterator()) {
					sem.Wait() iferr_panic_only_dev("ignore");
				}
			};

			for (usize i = 0; i < shedulerPtr->GetNumThreads(); ++i) {

				shedulerPtr->Post(RepostJob(shedulerPtr.Get(), &semaphores[i]));
			}
		}
	}
}
