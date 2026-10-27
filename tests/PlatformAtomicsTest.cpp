#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Platform/Platform.h"
#include "../alt-std/src/Random/Xoshiro256.h"

namespace {

template <typename NumT,
	std::enable_if_t<Alt::Num::Desc<NumT>::IsUnsigned()>* = nullptr>
void BaseAtomicChecks() noexcept {

	using UT = NumT;
	using ST = typename Alt::Num::Desc<UT>::Signed;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	{
		UT truth = 4;

		UT seqcst = 0;
		UT acquire = 0;
		UT release = 0;
		UT acqrel = 0;
		UT relaxed = 0;

		UT old = 0;

		Alt::Platform::AtomicStore(&seqcst, truth, Alt::Platform::MemoryOrder::SeqCst);
		Alt::Platform::AtomicStore(&acquire, truth, Alt::Platform::MemoryOrder::Relaxed);
		Alt::Platform::AtomicStore(&release, truth, Alt::Platform::MemoryOrder::Release);
		Alt::Platform::AtomicStore(&relaxed, truth, Alt::Platform::MemoryOrder::Relaxed);

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);

		old = Alt::Platform::AtomicInc(&seqcst, Alt::Platform::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicInc(&acquire, Alt::Platform::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicInc(&release, Alt::Platform::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicInc(&relaxed, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		truth += 1;

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);

		old = Alt::Platform::AtomicDec(&seqcst, Alt::Platform::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicDec(&acquire, Alt::Platform::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicDec(&release, Alt::Platform::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicDec(&relaxed, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		truth -= 1;

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);

		constexpr static usize ITERS = 10;

		for (usize i = 0; i < ITERS; ++i) {

			const UT num = (UT)rnd.GenerateU64(0, (u64)Alt::Num::Desc<UT>::Max());

			if (rnd.GenerateU64(0, 1) == 0) {

				UT old = 0;

				old = Alt::Platform::AtomicAdd(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicAdd(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicAdd(&release, num, Alt::Platform::MemoryOrder::Release);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicAdd(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
				EXPECT_EQ(old, truth);

				truth += num;

				EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
			}
			else {

				UT old = 0;

				old = Alt::Platform::AtomicSub(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicSub(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicSub(&release, num, Alt::Platform::MemoryOrder::Release);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicSub(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
				EXPECT_EQ(old, truth);

				truth -= num;

				EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
			}
		}

		for (usize i = 0; i < ITERS; ++i) {

			const UT num = (UT)rnd.GenerateU64(0, (u64)Alt::Num::Desc<UT>::Max());

			UT old = 0;

			old = Alt::Platform::AtomicAnd(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicAnd(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicAnd(&release, num, Alt::Platform::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicAnd(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth &= num;

			EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
		}

		for (usize i = 0; i < ITERS; ++i) {

			const UT num = (UT)rnd.GenerateU64(0, (u64)Alt::Num::Desc<UT>::Max());

			UT old = 0;

			old = Alt::Platform::AtomicOr(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicOr(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicOr(&release, num, Alt::Platform::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicOr(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth |= num;

			EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
		}

		for (usize i = 0; i < ITERS; ++i) {

			const UT num = (UT)rnd.GenerateU64(0, (u64)Alt::Num::Desc<UT>::Max());

			UT old = 0;

			old = Alt::Platform::AtomicXor(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicXor(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicXor(&release, num, Alt::Platform::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicXor(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth ^= num;

			EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
		}

		truth = 4;

		Alt::Platform::AtomicStore(&seqcst, truth, Alt::Platform::MemoryOrder::SeqCst);
		Alt::Platform::AtomicStore(&acquire, truth, Alt::Platform::MemoryOrder::Relaxed);
		Alt::Platform::AtomicStore(&release, truth, Alt::Platform::MemoryOrder::Release);
		Alt::Platform::AtomicStore(&relaxed, truth, Alt::Platform::MemoryOrder::Relaxed);

		UT exchange = 64;

		old = Alt::Platform::AtomicExchange(&seqcst, exchange, Alt::Platform::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicExchange(&acquire, exchange, Alt::Platform::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicExchange(&release, exchange, Alt::Platform::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicExchange(&relaxed, exchange, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), exchange);

		acqrel = exchange;

		old = Alt::Platform::AtomicCompareExchange(&seqcst, truth, 0, Alt::Platform::MemoryOrder::SeqCst, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&acquire, truth, 0, Alt::Platform::MemoryOrder::Acquire, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&release, truth, 0, Alt::Platform::MemoryOrder::Release, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&acqrel, truth, 0, Alt::Platform::MemoryOrder::AcqRel, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&relaxed, truth, 0, Alt::Platform::MemoryOrder::Relaxed, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), exchange);

		old = Alt::Platform::AtomicCompareExchange(&seqcst, exchange, truth, Alt::Platform::MemoryOrder::SeqCst, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&acquire, exchange, truth, Alt::Platform::MemoryOrder::Acquire, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&release, exchange, truth, Alt::Platform::MemoryOrder::Release, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&acqrel, exchange, truth, Alt::Platform::MemoryOrder::AcqRel, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&relaxed, exchange, truth, Alt::Platform::MemoryOrder::Relaxed, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acqrel, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
	}

	{
		ST truth = 4;

		ST seqcst = 0;
		ST acquire = 0;
		ST release = 0;
		ST acqrel = 0;
		ST relaxed = 0;

		ST old = 0;

		Alt::Platform::AtomicStore(&seqcst, truth, Alt::Platform::MemoryOrder::SeqCst);
		Alt::Platform::AtomicStore(&acquire, truth, Alt::Platform::MemoryOrder::Relaxed);
		Alt::Platform::AtomicStore(&release, truth, Alt::Platform::MemoryOrder::Release);
		Alt::Platform::AtomicStore(&relaxed, truth, Alt::Platform::MemoryOrder::Relaxed);

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);

		old = Alt::Platform::AtomicInc(&seqcst, Alt::Platform::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicInc(&acquire, Alt::Platform::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicInc(&release, Alt::Platform::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicInc(&relaxed, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		truth += 1;

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);

		old = Alt::Platform::AtomicDec(&seqcst, Alt::Platform::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicDec(&acquire, Alt::Platform::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicDec(&release, Alt::Platform::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicDec(&relaxed, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		truth -= 1;

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);

		constexpr static usize ITERS = 10;

		for (usize i = 0; i < ITERS; ++i) {

			const ST num = (ST)rnd.GenerateS64(0, (u64)Alt::Num::Desc<ST>::Max());

			if (rnd.GenerateU64(0, 1) == 0) {

				ST old = 0;

				old = Alt::Platform::AtomicAdd(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicAdd(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicAdd(&release, num, Alt::Platform::MemoryOrder::Release);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicAdd(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
				EXPECT_EQ(old, truth);

				truth += num;

				EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
			}
			else {

				ST old = 0;

				old = Alt::Platform::AtomicSub(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicSub(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicSub(&release, num, Alt::Platform::MemoryOrder::Release);
				EXPECT_EQ(old, truth);

				old = Alt::Platform::AtomicSub(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
				EXPECT_EQ(old, truth);

				truth -= num;

				EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
				EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
			}
		}

		for (usize i = 0; i < ITERS; ++i) {

			const ST num = (ST)rnd.GenerateS64(0, (u64)Alt::Num::Desc<ST>::Max());

			ST old = 0;

			old = Alt::Platform::AtomicAnd(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicAnd(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicAnd(&release, num, Alt::Platform::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicAnd(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth &= num;

			EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
		}

		for (usize i = 0; i < ITERS; ++i) {

			const ST num = (ST)rnd.GenerateS64(0, (u64)Alt::Num::Desc<ST>::Max());

			ST old = 0;

			old = Alt::Platform::AtomicOr(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicOr(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicOr(&release, num, Alt::Platform::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicOr(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth |= num;

			EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
		}

		for (usize i = 0; i < ITERS; ++i) {

			const ST num = (ST)rnd.GenerateS64(0, (u64)Alt::Num::Desc<ST>::Max());

			ST old = 0;

			old = Alt::Platform::AtomicXor(&seqcst, num, Alt::Platform::MemoryOrder::SeqCst);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicXor(&acquire, num, Alt::Platform::MemoryOrder::Acquire);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicXor(&release, num, Alt::Platform::MemoryOrder::Release);
			EXPECT_EQ(old, truth);

			old = Alt::Platform::AtomicXor(&relaxed, num, Alt::Platform::MemoryOrder::Relaxed);
			EXPECT_EQ(old, truth);

			truth ^= num;

			EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
			EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
		}

		truth = 4;

		Alt::Platform::AtomicStore(&seqcst, truth, Alt::Platform::MemoryOrder::SeqCst);
		Alt::Platform::AtomicStore(&acquire, truth, Alt::Platform::MemoryOrder::Relaxed);
		Alt::Platform::AtomicStore(&release, truth, Alt::Platform::MemoryOrder::Release);
		Alt::Platform::AtomicStore(&relaxed, truth, Alt::Platform::MemoryOrder::Relaxed);

		ST exchange = 64;

		old = Alt::Platform::AtomicExchange(&seqcst, exchange, Alt::Platform::MemoryOrder::SeqCst);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicExchange(&acquire, exchange, Alt::Platform::MemoryOrder::Acquire);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicExchange(&release, exchange, Alt::Platform::MemoryOrder::Release);
		EXPECT_EQ(old, truth);

		old = Alt::Platform::AtomicExchange(&relaxed, exchange, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, truth);

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), exchange);

		acqrel = exchange;

		old = Alt::Platform::AtomicCompareExchange(&seqcst, truth, 0, Alt::Platform::MemoryOrder::SeqCst, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&acquire, truth, 0, Alt::Platform::MemoryOrder::Acquire, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&release, truth, 0, Alt::Platform::MemoryOrder::Release, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&acqrel, truth, 0, Alt::Platform::MemoryOrder::AcqRel, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&relaxed, truth, 0, Alt::Platform::MemoryOrder::Relaxed, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acqrel, Alt::Platform::MemoryOrder::Relaxed), exchange);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), exchange);

		old = Alt::Platform::AtomicCompareExchange(&seqcst, exchange, truth, Alt::Platform::MemoryOrder::SeqCst, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&acquire, exchange, truth, Alt::Platform::MemoryOrder::Acquire, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&release, exchange, truth, Alt::Platform::MemoryOrder::Release, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&acqrel, exchange, truth, Alt::Platform::MemoryOrder::AcqRel, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		old = Alt::Platform::AtomicCompareExchange(&relaxed, exchange, truth, Alt::Platform::MemoryOrder::Relaxed, Alt::Platform::MemoryOrder::Relaxed);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&acqrel, Alt::Platform::MemoryOrder::Relaxed), truth);
		EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
	}
}

template <typename NumT,
	std::enable_if_t<Alt::Num::Desc<NumT>::IsFloat()>* = nullptr>
void BaseAtomicChecks() noexcept {

	using real = NumT;

	real truth = 4.0;

	real seqcst = 0.0;
	real acquire = 0.0;
	real release = 0.0;
	real acqrel = 0.0;
	real relaxed = 0.0;

	real old = 0.0;

	Alt::Platform::AtomicStore(&seqcst, truth, Alt::Platform::MemoryOrder::SeqCst);
	Alt::Platform::AtomicStore(&acquire, truth, Alt::Platform::MemoryOrder::Relaxed);
	Alt::Platform::AtomicStore(&release, truth, Alt::Platform::MemoryOrder::Release);
	Alt::Platform::AtomicStore(&relaxed, truth, Alt::Platform::MemoryOrder::Relaxed);

	EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);

	real exchange = 64.0;

	old = Alt::Platform::AtomicExchange(&seqcst, exchange, Alt::Platform::MemoryOrder::SeqCst);
	EXPECT_EQ(old, truth);

	old = Alt::Platform::AtomicExchange(&acquire, exchange, Alt::Platform::MemoryOrder::Acquire);
	EXPECT_EQ(old, truth);

	old = Alt::Platform::AtomicExchange(&release, exchange, Alt::Platform::MemoryOrder::Release);
	EXPECT_EQ(old, truth);

	old = Alt::Platform::AtomicExchange(&relaxed, exchange, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, truth);

	EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), exchange);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), exchange);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), exchange);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), exchange);

	acqrel = exchange;

	old = Alt::Platform::AtomicCompareExchange(&seqcst, truth, 0.0, Alt::Platform::MemoryOrder::SeqCst, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = Alt::Platform::AtomicCompareExchange(&acquire, truth, 0.0, Alt::Platform::MemoryOrder::Acquire, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = Alt::Platform::AtomicCompareExchange(&release, truth, 0.0, Alt::Platform::MemoryOrder::Release, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = Alt::Platform::AtomicCompareExchange(&acqrel, truth, 0.0, Alt::Platform::MemoryOrder::AcqRel, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = Alt::Platform::AtomicCompareExchange(&relaxed, truth, 0.0, Alt::Platform::MemoryOrder::Relaxed, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), exchange);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), exchange);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), exchange);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&acqrel, Alt::Platform::MemoryOrder::Relaxed), exchange);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), exchange);

	old = Alt::Platform::AtomicCompareExchange(&seqcst, exchange, truth, Alt::Platform::MemoryOrder::SeqCst, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = Alt::Platform::AtomicCompareExchange(&acquire, exchange, truth, Alt::Platform::MemoryOrder::Acquire, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = Alt::Platform::AtomicCompareExchange(&release, exchange, truth, Alt::Platform::MemoryOrder::Release, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = Alt::Platform::AtomicCompareExchange(&acqrel, exchange, truth, Alt::Platform::MemoryOrder::AcqRel, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	old = Alt::Platform::AtomicCompareExchange(&relaxed, exchange, truth, Alt::Platform::MemoryOrder::Relaxed, Alt::Platform::MemoryOrder::Relaxed);
	EXPECT_EQ(old, exchange);

	EXPECT_EQ(Alt::Platform::AtomicLoad(&seqcst, Alt::Platform::MemoryOrder::SeqCst), truth);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&acquire, Alt::Platform::MemoryOrder::Acquire), truth);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&release, Alt::Platform::MemoryOrder::Relaxed), truth);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&acqrel, Alt::Platform::MemoryOrder::Relaxed), truth);
	EXPECT_EQ(Alt::Platform::AtomicLoad(&relaxed, Alt::Platform::MemoryOrder::Relaxed), truth);
}

}

TEST(PlatformAtomics, Atomic8) {

	BaseAtomicChecks<u8>();
}

TEST(PlatformAtomics, Atomic16) {

	BaseAtomicChecks<u16>();
}

TEST(PlatformAtomics, Atomic32) {

	BaseAtomicChecks<u32>();
	BaseAtomicChecks<f32>();
}

TEST(PlatformAtomics, Atomic64) {

	BaseAtomicChecks<u64>();
	BaseAtomicChecks<f64>();
}

#ifdef ALT_PLATFORM_64BIT

TEST(PlatformAtomics, Atomic128) {

	const bool isAtomicSupported128 = Alt::_PlatformAtomicDetails::IsAtomic128Supported();
	ASSERT_TRUE(isAtomicSupported128);

	{
		Alt::_PlatformAtomicDetails::u128 truth{ 4, 0 };

		Alt::_PlatformAtomicDetails::u128 exchange{ 64, 0 };

		Alt::_PlatformAtomicDetails::u128 seqcst = exchange;
		Alt::_PlatformAtomicDetails::u128 acquire = exchange;
		Alt::_PlatformAtomicDetails::u128 release = exchange;
		Alt::_PlatformAtomicDetails::u128 acqrel = exchange;
		Alt::_PlatformAtomicDetails::u128 relaxed = exchange;

		Alt::_PlatformAtomicDetails::u128 old{};

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeSeqCstRelaxed(&seqcst, truth, Alt::_PlatformAtomicDetails::u128{});
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeAcquireRelaxed(&acquire, truth, Alt::_PlatformAtomicDetails::u128{});
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeReleaseRelaxed(&release, truth, Alt::_PlatformAtomicDetails::u128{});
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeAcqRelRelaxed(&release, acqrel, Alt::_PlatformAtomicDetails::u128{});
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeRelaxedRelaxed(&relaxed, truth, Alt::_PlatformAtomicDetails::u128{});
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadSeqCst(&seqcst), exchange);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadAcquire(&acquire), exchange);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&release), exchange);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&acqrel), exchange);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&relaxed), exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeSeqCstRelaxed(&seqcst, truth, exchange);
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeAcquireRelaxed(&acquire, truth, exchange);
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeReleaseRelaxed(&release, truth, exchange);
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeAcqRelRelaxed(&acqrel, truth, exchange);
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeRelaxedRelaxed(&relaxed, truth, exchange);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadSeqCst(&seqcst), truth);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadAcquire(&acquire), truth);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&release), truth);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&acqrel), truth);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&relaxed), truth);
	}

	{
		Alt::_PlatformAtomicDetails::s128 truth{ 4, 0 };

		Alt::_PlatformAtomicDetails::s128 exchange{ 64, 0 };

		Alt::_PlatformAtomicDetails::s128 seqcst = exchange;
		Alt::_PlatformAtomicDetails::s128 acquire = exchange;
		Alt::_PlatformAtomicDetails::s128 release = exchange;
		Alt::_PlatformAtomicDetails::s128 acqrel = exchange;
		Alt::_PlatformAtomicDetails::s128 relaxed = exchange;

		Alt::_PlatformAtomicDetails::s128 old{};

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeSeqCstRelaxed(&seqcst, truth, Alt::_PlatformAtomicDetails::s128{});
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeAcquireRelaxed(&acquire, truth, Alt::_PlatformAtomicDetails::s128{});
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeReleaseRelaxed(&release, truth, Alt::_PlatformAtomicDetails::s128{});
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeAcqRelRelaxed(&acqrel, truth, Alt::_PlatformAtomicDetails::s128{});
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeRelaxedRelaxed(&relaxed, truth, Alt::_PlatformAtomicDetails::s128{});
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadSeqCst(&seqcst), exchange);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadAcquire(&acquire), exchange);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&release), exchange);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&acqrel), exchange);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&relaxed), exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeSeqCstRelaxed(&seqcst, truth, exchange);
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeAcquireRelaxed(&acquire, truth, exchange);
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeReleaseRelaxed(&release, truth, exchange);
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeAcqRelRelaxed(&acqrel, truth, exchange);
		EXPECT_EQ(old, exchange);

		old = Alt::_PlatformAtomicDetails::AtomicCompareExchangeRelaxedRelaxed(&relaxed, truth, exchange);
		EXPECT_EQ(old, exchange);

		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadSeqCst(&seqcst), truth);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadAcquire(&acquire), truth);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&release), truth);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&acqrel), truth);
		EXPECT_EQ(Alt::_PlatformAtomicDetails::AtomicLoadRelaxed(&relaxed), truth);
	}
}

#endif // ALT_PLATFORM_64BIT

TEST(PlatformAtomics, CheckMemoryOrderBarrierSeqCst) {

	Alt::Platform::ThreadMemoryOrderBarrier(Alt::Platform::MemoryOrder::SeqCst);
}
