#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Random/Xoshiro256.h"

#include "../alt-std/src/Concurrency/Thread.h"

#include "../alt-std/src/Concurrency/Mutex.h"
#include "../alt-std/src/Concurrency/RWMutex.h"
#include "../alt-std/src/Concurrency/SpinMutex.h"
#include "../alt-std/src/Concurrency/Semaphore.h"
#include "../alt-std/src/Concurrency/SemaphoreTimeline.h"
#include "../alt-std/src/Concurrency/Latch.h"

namespace {

template <bool IS_HARD_LOAD, typename MutexT>
void MutexTest(MutexT& mutex) noexcept {

	iferr_scope_on_err {

		FAIL();
	};

	struct alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) ThreadData {

		usize counter = 0;
	};

	Alt::Xoshiro256StarStar rng = Alt::Xoshiro256StarStar::CreateSeededByTime();
	Alt::Vector<ThreadData> data = Alt::Vector<ThreadData>::Allocate() iferr_return;
	Alt::Vector<Alt::Thread> threads = Alt::Vector<Alt::Thread>::Allocate() iferr_return;

	alignas(ALT_PLATFORM_DESTRUCTIVE_CACHE_SIZE_GUARD) usize counter = 0;

	Alt::IteratorForwardFromFunc iter{
		[&]() {

			static usize s_Id = 0;

			const usize id = s_Id;
			s_Id += 1;

			Alt::Thread thread = Alt::Thread::Allocate([&, id]() {
				iferr_scope;

				const usize iters = IS_HARD_LOAD ? 10'000 : 1'000'000;
				for (usize i = 0; i < iters; ++i) {

					mutex.Lock() iferr_panic_only_dev("poisoned mutex");
					defer{ mutex.Unlock(); };

					counter += 1;

					if (IS_HARD_LOAD) {

						const usize size = (usize)rng.GenerateU64(0, 1024);
						usize acc = 0;

						for (usize i = 0; i < size; ++i) {

							acc += (usize)rng.GenerateU64();
						}

						data[id].counter += acc;
					}
				}
			}) iferr_panic("Threads!!");

			Alt::Thread::PriorityDesc desc{};
			desc.priority = Alt::Thread::Priority::Highest;
			desc.isBoostEnabled = true;

			thread.SetPriority(desc, static_cast<u64>(1) << (id % 64));

			return thread;
		}
	};

	const usize numThreads = Alt::Thread::GetNumOfLogicalProcessors();
	data.Resize(numThreads) iferr_return;

	const f64 start = Alt::Platform::GetTicksNs();
	const Alt::Platform::SysTime sysTimeStart = Alt::Platform::PerformanceGetCurrentProcessSysTime() iferr_return;
	const u64 cyclesStart = Alt::Platform::PerformanceGetCurrentProcessCycles() iferr_return;

	iter
		.Take(numThreads)
		.CollectInto(threads) iferr_return;

	for (Alt::Pair<usize, Alt::Thread&> val : threads.GetIterator().Enumerate()) {

		val.s().WaitCompletion();
		ALT_PRINTLN("Thread {} completed : {}", val.f(), data[val.f()].counter);
	}

	ALT_PRINTLN("{}", counter);

	const u64 cyclesEnd = Alt::Platform::PerformanceGetCurrentProcessCycles() iferr_return;
	const Alt::Platform::SysTime sysTimeEnd = Alt::Platform::PerformanceGetCurrentProcessSysTime() iferr_return;
	const f64 end = Alt::Platform::GetTicksNs();

	ALT_PRINTLN("Is Harad Load - {}", IS_HARD_LOAD);
	ALT_PRINTLN("elapsed - {}ms", (end - start) / 1'000'000.0);
	ALT_PRINTLN("elapsed - {}ns", end - start);
	ALT_PRINTLN(" cycles - {}", cyclesEnd - cyclesStart);
	ALT_PRINTLN(" kernel - {}ms", static_cast<f64>(sysTimeEnd.kernelTimeNS - sysTimeStart.kernelTimeNS) / 1'000'000.0);
	ALT_PRINTLN(" kernel - {}ns", static_cast<f64>(sysTimeEnd.kernelTimeNS - sysTimeStart.kernelTimeNS));
	ALT_PRINTLN("   user - {}ms", static_cast<f64>(sysTimeEnd.userTimeNS - sysTimeStart.userTimeNS) / 1'000'000.0);
	ALT_PRINTLN("   user - {}ns", static_cast<f64>(sysTimeEnd.userTimeNS - sysTimeStart.userTimeNS));
}
}

TEST(Concurrency, Thread) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Thread::SetCurrentPriority(Alt::Thread::GetCurrentPriority(), 1);

	{
		Alt::Thread thread = Alt::Thread::Create([]() noexcept {

			ALT_PRINTLN("Println from another thread");
		}) iferr_return;

		bool isCompleted = thread.WaitCompletion() iferr_return;
		EXPECT_TRUE(isCompleted);

		isCompleted = thread.WaitCompletion() iferr_return;
		EXPECT_TRUE(isCompleted);
	}

	{
		u32 id0 = 0;
		Alt::Thread thread = Alt::Thread::Allocate([&]() noexcept {

			id0 = thread.GetThreadID();
			EXPECT_EQ(id0, Alt::Thread::GetCurrentID());

			ALT_PRINTLN("Println from another thread");
		}) iferr_return;

		const u32 id1 = thread.GetThreadID();
		EXPECT_NE(id1, Alt::Thread::GetCurrentID());

		thread.SetName("Alt Thread");
		thread.Forget();

		const u32 id2 = thread.GetThreadID();
		EXPECT_NE(id1, Alt::Thread::GetCurrentID());

		EXPECT_EQ(id1, id2);

		EXPECT_TRUE(thread.IsForgotten());

		Alt::Thread::PriorityDesc priority{};
		priority.priority = Alt::Thread::Priority::Highest;
		priority.isBoostEnabled = true;

		thread.SetPriority(priority, 1 << 1);

		const Alt::Thread::PriorityDesc priorityNew = thread.GetPriority();
		EXPECT_EQ(priorityNew.priority, priority.priority);
		EXPECT_EQ(priorityNew.isBoostEnabled, priority.isBoostEnabled);

		bool isCompleted = thread.WaitCompletion() iferr_return;
		EXPECT_TRUE(isCompleted);

		isCompleted = thread.WaitCompletion() iferr_return;
		EXPECT_TRUE(isCompleted);

		EXPECT_EQ(id0, id2);

		Alt::String threadName = thread.GetName() iferr_return;
		EXPECT_EQ(threadName, "Alt Thread");
	}
}

constexpr static bool IS_HARD_LOAD = true;

TEST(Concurrency, NoMutex) {

	class Mutex {
	public:

		ALT_FORCE_INLINE bool Lock() const noexcept {
			return true;
		}

		ALT_FORCE_INLINE void Unlock() const noexcept {
			return;
		}
	};

	Mutex mutex{};
	MutexTest<IS_HARD_LOAD>(mutex);
}

TEST(Concurrency, SpinMutex) {

	class SpinMutex {
	public:

		ALT_FORCE_INLINE bool Lock() noexcept {

			m_Mutex.Lock();
			return true;
		}

		ALT_FORCE_INLINE void Unlock() noexcept {

			m_Mutex.Unlock();
		}

	private:

		Alt::SpinMutex m_Mutex;
	};

	SpinMutex mutex{};
	MutexTest<IS_HARD_LOAD>(mutex);
}

TEST(Concurrency, Mutex) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Mutex mutex = Alt::Mutex::Create() iferr_return;
	MutexTest<IS_HARD_LOAD>(mutex);
}

TEST(Concurrency, RWMutex) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::RWMutex mutex = Alt::RWMutex::Create() iferr_return;
	MutexTest<IS_HARD_LOAD>(mutex);
}

TEST(Concurrency, Semaphore) {

	iferr_scope_on_err {

		FAIL();
	};

	usize state = 0;

	Alt::Semaphore sem = Alt::Semaphore::Create() iferr_return;
	Alt::StaticVector<Alt::Thread, 4> threads;

	for (usize i = 0; i < threads.GetCapacity(); ++i) {

		Alt::Thread thread = Alt::Thread::Allocate([&]() {
			iferr_scope;

			Alt::Platform::AtomicInc(&state, Alt::Platform::MemoryOrder::Release);

			sem.Wait() iferr_panic("poisoning or some system error");

			ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());

		}) iferr_return;

		threads.Push(std::move(thread)) iferr_return;
	}

	while (Alt::Platform::AtomicLoad(&state, Alt::Platform::MemoryOrder::Acquire) != threads.GetCapacity()) {
	}

	// Additionaly sleep to increase probability for '.Wait()' call
	Alt::Thread::Sleep(16);

	sem.Signal(threads.GetCapacity()) iferr_panic_only_dev("ignore");

	for (const Alt::Thread& thread : threads.GetIterator()) {

		thread.WaitCompletion() iferr_return;
	}
}

TEST(Concurrency, SemaphoreTimeline) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize NUM_SIGNALS = 16;

	usize state = 0;

	Alt::SemaphoreTimeline sem = Alt::SemaphoreTimeline::Create() iferr_return;
	Alt::StaticVector<Alt::Thread, 4> threads;

	for (usize i = 0; i < threads.GetCapacity(); ++i) {

		Alt::Thread thread = Alt::Thread::Allocate([&]() {
			iferr_scope;

			Alt::Platform::AtomicInc(&state, Alt::Platform::MemoryOrder::Release);

			for (usize i = 0; i < NUM_SIGNALS; ++i) {

				sem.Wait((u64)i) iferr_panic("poisoning or some system error");
				ALT_PRINTLN("[{}] Thread {} completed", i, Alt::Thread::GetCurrentID());
			}

		}) iferr_return;

		threads.Push(std::move(thread)) iferr_return;
	}

	while (Alt::Platform::AtomicLoad(&state, Alt::Platform::MemoryOrder::Acquire) != threads.GetCapacity()) {
	}

	// Additionaly sleep to increase probability for '.Wait()' call
	Alt::Thread::Sleep(16);

	for (usize i = 0; i < NUM_SIGNALS; ++i) {

		const usize id = i;
		Alt::Thread thread = Alt::Thread::Allocate([&sem, id]() {
			iferr_scope;

			ALT_PRINTLN("Signal {}", id);
			sem.Signal((u64)id) iferr_panic_only_dev("ignore");

			Alt::Thread::Sleep(1);

		}) iferr_return;

		thread.Forget();
	}

	for (const Alt::Thread& thread : threads.GetIterator()) {

		thread.WaitCompletion() iferr_return;
	}
}

TEST(Concurrency, Latch) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr static usize NUM_THREADS = 4;

	Alt::Latch latch = Alt::Latch::Create((s32)NUM_THREADS) iferr_return;
	Alt::StaticVector<Alt::Thread, NUM_THREADS> threads;

	for (usize i = 0; i < NUM_THREADS; ++i) {

		Alt::Thread thread = Alt::Thread::Allocate([&]() noexcept {

			ALT_PRINTLN("Thread {} completed", Alt::Thread::GetCurrentID());

			latch.Dec();

		}) iferr_return;

		threads.Push(std::move(thread)) iferr_return;
	}

	latch.Wait() iferr_panic("poisoning or some system error");

	for (const Alt::Thread& thread : threads.GetIterator()) {

		thread.WaitCompletion() iferr_return;
	}
}
