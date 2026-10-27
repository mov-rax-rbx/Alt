#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Concurrency/Atomic.h"
#include "../alt-std/src/Random/Xoshiro256.h"

/*
 * Copied from PlatformAtomicsTest.cpp
 */
namespace {

template <typename NumT,
	std::enable_if_t<Alt::Num::Desc<NumT>::IsUnsigned()>* = nullptr>
void BaseAtomicChecks() noexcept {

	using UT = NumT;
	using ST = typename Alt::Num::Desc<UT>::Signed;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	{
		UT truth = 4;

		Alt::Atomic<UT> seqcst = 0;
		Alt::Atomic<UT> acquire = 0;
		Alt::Atomic<UT> release = 0;
		Alt::Atomic<UT> acqrel = 0;
		Alt::Atomic<UT> relaxed = 0;

		UT old = 0;

		seqcst.Store(truth, Alt::MemoryOrder::SeqCst);
		acquire.Store(truth, Alt::MemoryOrder::Relaxed);
		release.Store(truth, Alt::MemoryOrder::Release);
		relaxed.Store(truth, Alt::MemoryOrder::Relaxed);

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);

		old = seqcst.FetchInc(Alt::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = acquire.FetchInc(Alt::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = release.FetchInc(Alt::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = relaxed.FetchInc(Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		truth += 1;

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);

		old = seqcst.FetchDec(Alt::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = acquire.FetchDec(Alt::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = release.FetchDec(Alt::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = relaxed.FetchDec(Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		truth -= 1;

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);

		constexpr static usize ITERS = 10;

		for (usize i = 0; i < ITERS; ++i) {

			const UT num = (UT)rnd.GenerateU64(0, (u64)Alt::Num::Desc<UT>::Max());

			if (rnd.GenerateU64(0, 1) == 0) {

				UT old = 0;

				old = seqcst.FetchAdd(num, Alt::MemoryOrder::SeqCst);
				EXPECT_EQ(old, truth);

				old = acquire.FetchAdd(num, Alt::MemoryOrder::Acquire);
				EXPECT_EQ(old, truth);

				old = release.FetchAdd(num, Alt::MemoryOrder::Release);
				EXPECT_EQ(old, truth);

				old = relaxed.FetchAdd(num, Alt::MemoryOrder::Relaxed);
				EXPECT_EQ(old, truth);

				truth += num;

				EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
				EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
				EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
				EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
			}
			else {

				UT old = 0;

				old = seqcst.FetchSub(num, Alt::MemoryOrder::SeqCst);
				EXPECT_EQ(old, truth);

				old = acquire.FetchSub(num, Alt::MemoryOrder::Acquire);
				EXPECT_EQ(old, truth);

				old = release.FetchSub(num, Alt::MemoryOrder::Release);
				EXPECT_EQ(old, truth);

				old = relaxed.FetchSub(num, Alt::MemoryOrder::Relaxed);
				EXPECT_EQ(old, truth);

				truth -= num;

				EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
				EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
				EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
				EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
			}
		}

		for (usize i = 0; i < ITERS; ++i) {

			const UT num = (UT)rnd.GenerateU64(0, (u64)Alt::Num::Desc<UT>::Max());

			UT old = 0;

			old = seqcst.FetchAnd(num, Alt::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = acquire.FetchAnd(num, Alt::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = release.FetchAnd(num, Alt::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = relaxed.FetchAnd(num, Alt::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth &= num;

			EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
			EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
		}

		for (usize i = 0; i < ITERS; ++i) {

			const UT num = (UT)rnd.GenerateU64(0, (u64)Alt::Num::Desc<UT>::Max());

			UT old = 0;

			old = seqcst.FetchOr(num, Alt::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = acquire.FetchOr(num, Alt::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = release.FetchOr(num, Alt::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = relaxed.FetchOr(num, Alt::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth |= num;

			EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
			EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
		}

		for (usize i = 0; i < ITERS; ++i) {

			const UT num = (UT)rnd.GenerateU64(0, (u64)Alt::Num::Desc<UT>::Max());

			UT old = 0;

			old = seqcst.FetchXor(num, Alt::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = acquire.FetchXor(num, Alt::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = release.FetchXor(num, Alt::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = relaxed.FetchXor(num, Alt::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth ^= num;

			EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
			EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
		}

		truth = 4;

		seqcst.Store(truth, Alt::MemoryOrder::SeqCst);
		acquire.Store(truth, Alt::MemoryOrder::Relaxed);
		release.Store(truth, Alt::MemoryOrder::Release);
		relaxed.Store(truth, Alt::MemoryOrder::Relaxed);

		UT exchange = 64;

		old = seqcst.Exchange(exchange, Alt::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = acquire.Exchange(exchange, Alt::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = release.Exchange(exchange, Alt::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = relaxed.Exchange(exchange, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), exchange);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), exchange);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), exchange);

		acqrel.Store(exchange);

		old = seqcst.CompareExchange(truth, 0, Alt::MemoryOrder::SeqCst, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = acquire.CompareExchange(truth, 0, Alt::MemoryOrder::Acquire, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = release.CompareExchange(truth, 0, Alt::MemoryOrder::Release, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = acqrel.CompareExchange(truth, 0, Alt::MemoryOrder::AcqRel, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = relaxed.CompareExchange(truth, 0, Alt::MemoryOrder::Relaxed, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), exchange);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), exchange);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), exchange);

		old = seqcst.CompareExchange(exchange, truth, Alt::MemoryOrder::SeqCst, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = acquire.CompareExchange(exchange, truth, Alt::MemoryOrder::Acquire, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = release.CompareExchange(exchange, truth, Alt::MemoryOrder::Release, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = acqrel.CompareExchange(exchange, truth, Alt::MemoryOrder::AcqRel, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = relaxed.CompareExchange(exchange, truth, Alt::MemoryOrder::Relaxed, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(acqrel.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
	}

	{
		ST truth = 4;

		Alt::Atomic<ST> seqcst = 0;
		Alt::Atomic<ST> acquire = 0;
		Alt::Atomic<ST> release = 0;
		Alt::Atomic<ST> acqrel = 0;
		Alt::Atomic<ST> relaxed = 0;

		ST old = 0;

		seqcst.Store(truth, Alt::MemoryOrder::SeqCst);
		acquire.Store(truth, Alt::MemoryOrder::Relaxed);
		release.Store(truth, Alt::MemoryOrder::Release);
		relaxed.Store(truth, Alt::MemoryOrder::Relaxed);

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);

		old = seqcst.FetchInc(Alt::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = acquire.FetchInc(Alt::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = release.FetchInc(Alt::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = relaxed.FetchInc(Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		truth += 1;

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);

		old = seqcst.FetchDec(Alt::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = acquire.FetchDec(Alt::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = release.FetchDec(Alt::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = relaxed.FetchDec(Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		truth -= 1;

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);

		constexpr static usize ITERS = 10;

		for (usize i = 0; i < ITERS; ++i) {

			const ST num = (ST)rnd.GenerateS64(0, (u64)Alt::Num::Desc<ST>::Max());

			if (rnd.GenerateU64(0, 1) == 0) {

				ST old = 0;

				old = seqcst.FetchAdd(num, Alt::MemoryOrder::SeqCst);
				EXPECT_EQ(old, truth);

				old = acquire.FetchAdd(num, Alt::MemoryOrder::Acquire);
				EXPECT_EQ(old, truth);

				old = release.FetchAdd(num, Alt::MemoryOrder::Release);
				EXPECT_EQ(old, truth);

				old = relaxed.FetchAdd(num, Alt::MemoryOrder::Relaxed);
				EXPECT_EQ(old, truth);

				truth += num;

				EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
				EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
				EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
				EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
			}
			else {

				ST old = 0;

				old = seqcst.FetchSub(num, Alt::MemoryOrder::SeqCst);
				EXPECT_EQ(old, truth);

				old = acquire.FetchSub(num, Alt::MemoryOrder::Acquire);
				EXPECT_EQ(old, truth);

				old = release.FetchSub(num, Alt::MemoryOrder::Release);
				EXPECT_EQ(old, truth);

				old = relaxed.FetchSub(num, Alt::MemoryOrder::Relaxed);
				EXPECT_EQ(old, truth);

				truth -= num;

				EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
				EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
				EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
				EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
			}
		}

		for (usize i = 0; i < ITERS; ++i) {

			const ST num = (ST)rnd.GenerateS64(0, (u64)Alt::Num::Desc<ST>::Max());

			ST old = 0;

			old = seqcst.FetchAnd(num, Alt::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = acquire.FetchAnd(num, Alt::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = release.FetchAnd(num, Alt::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = relaxed.FetchAnd(num, Alt::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth &= num;

			EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
			EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
		}

		for (usize i = 0; i < ITERS; ++i) {

			const ST num = (ST)rnd.GenerateS64(0, (u64)Alt::Num::Desc<ST>::Max());

			ST old = 0;

			old = seqcst.FetchOr(num, Alt::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = acquire.FetchOr(num, Alt::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = release.FetchOr(num, Alt::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = relaxed.FetchOr(num, Alt::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth |= num;

			EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
			EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
		}

		for (usize i = 0; i < ITERS; ++i) {

			const ST num = (ST)rnd.GenerateS64(0, (u64)Alt::Num::Desc<ST>::Max());

			ST old = 0;

			old = seqcst.FetchXor(num, Alt::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = acquire.FetchXor(num, Alt::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = release.FetchXor(num, Alt::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = relaxed.FetchXor(num, Alt::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth ^= num;

			EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
			EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
		}

		truth = 4;

		seqcst.Store(truth, Alt::MemoryOrder::SeqCst);
		acquire.Store(truth, Alt::MemoryOrder::Relaxed);
		release.Store(truth, Alt::MemoryOrder::Release);
		relaxed.Store(truth, Alt::MemoryOrder::Relaxed);

		ST exchange = 64;

		old = seqcst.Exchange(exchange, Alt::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = acquire.Exchange(exchange, Alt::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = release.Exchange(exchange, Alt::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = relaxed.Exchange(exchange, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), exchange);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), exchange);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), exchange);

		acqrel.Store(exchange);

		old = seqcst.CompareExchange(truth, 0, Alt::MemoryOrder::SeqCst, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = acquire.CompareExchange(truth, 0, Alt::MemoryOrder::Acquire, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = release.CompareExchange(truth, 0, Alt::MemoryOrder::Release, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = acqrel.CompareExchange(truth, 0, Alt::MemoryOrder::AcqRel, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = relaxed.CompareExchange(truth, 0, Alt::MemoryOrder::Relaxed, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), exchange);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), exchange);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(acqrel.Load(Alt::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), exchange);

		old = seqcst.CompareExchange(exchange, truth, Alt::MemoryOrder::SeqCst, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = acquire.CompareExchange(exchange, truth, Alt::MemoryOrder::Acquire, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = release.CompareExchange(exchange, truth, Alt::MemoryOrder::Release, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = acqrel.CompareExchange(exchange, truth, Alt::MemoryOrder::AcqRel, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = relaxed.CompareExchange(exchange, truth, Alt::MemoryOrder::Relaxed, Alt::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
		EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(acqrel.Load(Alt::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
	}
}

template <typename NumT,
	std::enable_if_t<Alt::Num::Desc<NumT>::IsFloat()>* = nullptr>
void BaseAtomicChecks() noexcept {

	using real = NumT;

	real truth = 4.0;

	Alt::Atomic<real> seqcst = 0.0;
	Alt::Atomic<real> acquire = 0.0;
	Alt::Atomic<real> release = 0.0;
	Alt::Atomic<real> acqrel = 0.0;
	Alt::Atomic<real> relaxed = 0.0;

	real old = 0.0;

	seqcst.Store(truth, Alt::MemoryOrder::SeqCst);
	acquire.Store(truth, Alt::MemoryOrder::Relaxed);
	release.Store(truth, Alt::MemoryOrder::Release);
	relaxed.Store(truth, Alt::MemoryOrder::Relaxed);

	EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
	EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
	EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
	EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);

	real exchange = 64.0;

	old = seqcst.Exchange(exchange, Alt::MemoryOrder::SeqCst);
	EXPECT_EQ(old, truth);

	old = acquire.Exchange(exchange, Alt::MemoryOrder::Acquire);
	EXPECT_EQ(old, truth);

	old = release.Exchange(exchange, Alt::MemoryOrder::Release);
	EXPECT_EQ(old, truth);

	old = relaxed.Exchange(exchange, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, truth);

	EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), exchange);
	EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), exchange);
	EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), exchange);
	EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), exchange);

	acqrel.Store(exchange);

	old = seqcst.CompareExchange(truth, 0.0, Alt::MemoryOrder::SeqCst, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = acquire.CompareExchange(truth, 0.0, Alt::MemoryOrder::Acquire, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = release.CompareExchange(truth, 0.0, Alt::MemoryOrder::Release, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = acqrel.CompareExchange(truth, 0.0, Alt::MemoryOrder::AcqRel, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = relaxed.CompareExchange(truth, 0.0, Alt::MemoryOrder::Relaxed, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), exchange);
	EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), exchange);
	EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), exchange);
	EXPECT_EQ(acqrel.Load(Alt::MemoryOrder::Relaxed), exchange);
	EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), exchange);

	old = seqcst.CompareExchange(exchange, truth, Alt::MemoryOrder::SeqCst, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = acquire.CompareExchange(exchange, truth, Alt::MemoryOrder::Acquire, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = release.CompareExchange(exchange, truth, Alt::MemoryOrder::Release, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = acqrel.CompareExchange(exchange, truth, Alt::MemoryOrder::AcqRel, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = relaxed.CompareExchange(exchange, truth, Alt::MemoryOrder::Relaxed, Alt::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	EXPECT_EQ(seqcst.Load(Alt::MemoryOrder::SeqCst), truth);
	EXPECT_EQ(acquire.Load(Alt::MemoryOrder::Acquire), truth);
	EXPECT_EQ(release.Load(Alt::MemoryOrder::Relaxed), truth);
	EXPECT_EQ(acqrel.Load(Alt::MemoryOrder::Relaxed), truth);
	EXPECT_EQ(relaxed.Load(Alt::MemoryOrder::Relaxed), truth);
}

}

TEST(Atomics, Atomic8) {

	BaseAtomicChecks<u8>();
}

TEST(Atomics, Atomic16) {

	BaseAtomicChecks<u16>();
}

TEST(Atomics, Atomic32) {

	BaseAtomicChecks<u32>();
	BaseAtomicChecks<f32>();
}

TEST(Atomics, Atomic64) {

	BaseAtomicChecks<u64>();
	BaseAtomicChecks<f64>();
}

TEST(Atomics, CheckMemoryOrderBarrierSeqCst) {

	Alt::ThreadMemoryOrderBarrier(Alt::MemoryOrder::SeqCst);
}
