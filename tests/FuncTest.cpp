#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/IfErr.h"

#include "../alt-std/src/Utils/Func.h"
#include "../alt-std/src/String/String.h"
#include "../alt-std/src/Platform/Platform.h"

namespace {

static void FuncPtrVoid() noexcept {}

static usize FuncPtr0(const f32) noexcept {
	return 0;
}

struct EmulateLambda {

	mutable usize acc = 0;

	usize operator()(const f32) const noexcept {
		return acc++;
	}
};

};

TEST(Func, FuncPtr) {

	const auto lambdaVoid = []() noexcept {};

	Alt::FuncPtr<void> funcVoid0 = lambdaVoid;
	(funcVoid0)();

	Alt::FuncPtr<void> funcVoid1 = FuncPtrVoid;
	(funcVoid1)();

	Alt::FuncPtr<usize, f32> func = FuncPtr0;
	EXPECT_EQ((*func)(0.0f), 0);
}

TEST(Func, FuncRef) {

	Alt::FuncRef<usize, f32> func = [](const f32) -> usize {
		return 0;
	};
	EXPECT_EQ((func)(0.0f), 0);

	func = FuncPtr0;
	EXPECT_EQ((func)(0.0f), 0);

	usize acc = 0;
	auto lambda0 = [&acc](const f32) -> usize {
		return acc++;
	};

	func = lambda0;

	EXPECT_EQ((func)(0.0f), 0);
	EXPECT_EQ((func)(0.0f), 1);
	EXPECT_EQ((func)(0.0f), 2);
	EXPECT_EQ((func)(0.0f), 3);

	EXPECT_EQ(acc, 4);

	const EmulateLambda emuLambda{};
	func = emuLambda;

	EXPECT_EQ((func)(0.0f), 0);
	EXPECT_EQ((func)(0.0f), 1);
	EXPECT_EQ((func)(0.0f), 2);
	EXPECT_EQ((func)(0.0f), 3);

	EXPECT_EQ(emuLambda.acc, 4);
}

TEST(Func, StaticFunc) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticFunc<4096, 16, void> func0 = []() {};
		(func0)();

		Alt::StaticFunc<4096, 16, void> func1 = &FuncPtrVoid;
		(func1)();
	}

	{
		Alt::String str = "String"_salloc;

		Alt::StaticFunc<4096, 16, Alt::String> func0 = [str = str.Clone()]() mutable -> Alt::String {

			ALT_PRINTLN("{}", str);

			return std::move(str).GetValueOrDefault();
		};

		Alt::StaticFunc<4096, 16, Alt::String> func = std::move(func0);

		Alt::String str1 = (func)();
		EXPECT_EQ(str, str1);
	}

	{
		usize x = 44;

		Alt::StaticFunc<4096, 16, usize> func0 = [x]() -> usize {

			return x;
		};

		Alt::StaticFunc<4096, 16, usize> func = std::move(func0);

		usize x1 = (func)();
		EXPECT_EQ(x, x1);

		Alt::FuncRef<usize> funcRef = func;

		usize x2 = (func)();
		EXPECT_EQ(x, x2);
	}

	{
		const EmulateLambda emuLambda{};
		Alt::StaticFunc<4096, 16, usize, f32> func0 = emuLambda;

		Alt::StaticFunc<4096, 16, usize, f32> func = std::move(func0);

		EXPECT_EQ((func)(0.0f), 0);
		EXPECT_EQ((func)(0.0f), 1);
		EXPECT_EQ((func)(0.0f), 2);
		EXPECT_EQ((func)(0.0f), 3);
	}
}
