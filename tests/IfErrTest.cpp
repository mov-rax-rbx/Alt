#include "pch.h"

#include <vector>
#include <optional>

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/IfErr.h"

#include "../alt-std/src/Utils/Result.h"
#include "../alt-std/src/Utils/Optional.h"
#include "../alt-std/src/Memory/UniquePtr.h"
#include "../alt-std/src/Platform/Platform.h"

namespace {

	class SomeClassWhichCantBeDefaultConstructed {
	public:

	    static SomeClassWhichCantBeDefaultConstructed Create() noexcept { return SomeClassWhichCantBeDefaultConstructed{}; }

	private:

	    SomeClassWhichCantBeDefaultConstructed() noexcept = default;
	    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(SomeClassWhichCantBeDefaultConstructed);
	};

	class SomeClassWhichCantBeDefaultConstructed2 {
	public:

	    static SomeClassWhichCantBeDefaultConstructed2 Create() noexcept { return SomeClassWhichCantBeDefaultConstructed2{}; }

	private:

	    SomeClassWhichCantBeDefaultConstructed2() noexcept = default;
	};

	Alt::Optional<SomeClassWhichCantBeDefaultConstructed> ComputeAndReturnSomeClassWhichCantBeDefaultConstructed() noexcept{

		return SomeClassWhichCantBeDefaultConstructed::Create();
	}

	Alt::Optional<SomeClassWhichCantBeDefaultConstructed2> ComputeAndReturnSomeClassWhichCantBeDefaultConstructed2() noexcept{

		return SomeClassWhichCantBeDefaultConstructed2::Create();
	}

	bool CheckDefaultConstructorInjection0True() noexcept{
		iferr_scope;

		SomeClassWhichCantBeDefaultConstructed result = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed() iferr_return false;
		Alt::Optional<SomeClassWhichCantBeDefaultConstructed> result2 = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed() iferr {

			// We just fail and contionue execution;
		}

		// compilation error
		//SomeClassWhichCantBeDefaultConstructed result3 = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed() iferr {

		//	// We just fail and contionue execution;
		//}

		return true;
	}

	bool CheckDefaultConstructorInjection1True() noexcept{
		iferr_scope;

		// compilation error 
		//SomeClassWhichCantBeDefaultConstructed2 result = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed2() iferr_return false;

		Alt::Optional<SomeClassWhichCantBeDefaultConstructed2> result = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed2() iferr_return false;
		Alt::Optional<SomeClassWhichCantBeDefaultConstructed2> result2 = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed2() iferr {

			// We just fail and contionue execution;
		}

		// compilation error
		//SomeClassWhichCantBeDefaultConstructed result3 = ComputeAndReturnSomeClassWhichCantBeDefaultConstructed() iferr {

		//	// We just fail and contionue execution;
		//}

		return true;
	}

	bool IfErrScopeOptional0False() noexcept {
		iferr_scope;

		std::optional<usize> v = std::nullopt;
		v iferr_return false;

		return true;
	}

	bool IfErrScopeOptional1False() noexcept {
		iferr_scope;

		Alt::Optional<usize> v = Alt::Nullopt;
		ALT_MAYBE_UNUSED auto _ = v iferr_return false;

		return true;
	}

	bool IfErrScopeOptional2True() noexcept {
		iferr_scope;

		{
			std::optional<usize> v = 4;
			v iferr_return false;
		}

		{
			Alt::Optional<usize> v = 4;
			ALT_MAYBE_UNUSED auto _ = v iferr_return false;

			usize val = v iferr_return false;
			EXPECT_EQ(val, 4);
		}

		return true;
	}

	bool IfErrScopeOptionalUnwrapRefToPtrTrue() noexcept {
		iferr_scope;

		{
			usize val = 4;

			Alt::Optional<usize&> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			usize* val0 = v iferr_return false;
			EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		{
			usize val = 4;

			Alt::Optional<const usize&> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			//usize* val0 = v iferr_return false; // compilation error
			//EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		{
			usize val = 4;

			const Alt::Optional<usize&> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			//usize* val0 = v iferr_return false; // compilation error
			//EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		{
			usize val = 4;

			const Alt::Optional<const usize&> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			//usize* val0 = v iferr_return false;
			//EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		return true;
	}

	bool IfErrScopeResult0False() noexcept {
		iferr_scope;

		Alt::Result<usize, std::string> result = "Error";
		ALT_MAYBE_UNUSED auto _ = result iferr_return false;

		return true;
	}

	bool IfErrScopeResult1True() noexcept {
		iferr_scope;

		Alt::Result<usize, std::string> result = 4;
		ALT_MAYBE_UNUSED auto _ = result iferr_return false;

		usize val = result iferr_return false;
		EXPECT_EQ(val, 4);

		return true;
	}

	bool IfErrScopeResultUnwrapRefToPtr0True() noexcept {
		iferr_scope;

		{
			usize val = 4;

			Alt::Result<usize&, void> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			usize* val0 = v iferr_return false;
			EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		{
			usize val = 4;

			Alt::Result<const usize&, void> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			//usize* val0 = v iferr_return false; // compilation error
			//EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		{
			usize val = 4;

			const Alt::Result<usize&, void> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			//usize* val0 = v iferr_return false;
			//EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		{
			usize val = 4;

			const Alt::Result<const usize&, void> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			//usize* val0 = v iferr_return false;
			//EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		return true;
	}

	bool IfErrScopeResultUnwrapRefToPtr1True() noexcept {
		iferr_scope;

		{
			usize val = 4;

			Alt::Result<usize&, f32> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			usize* val0 = v iferr_return false;
			EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		{
			usize val = 4;

			Alt::Result<const usize&, f32> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			//usize* val0 = v iferr_return false; // compilation error
			//EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		{
			usize val = 4;

			const Alt::Result<usize&, f32> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			//usize* val0 = v iferr_return false; // compilation error
			//EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		{
			usize val = 4;

			const Alt::Result<const usize&, f32> v = val;

			//usize& valRef0 = v iferr_return false; // compilation error
			//const usize& valRef1 = v iferr_return false; // compilation error

			//usize* val0 = v iferr_return false;
			//EXPECT_TRUE(val0);

			const usize* val1 = v iferr_return false;
			EXPECT_TRUE(val1);
		}

		return true;
	}

	bool IfErrScopeResultUnwrapRefToPtr2True() noexcept {
		iferr_scope;

		const Alt::Result<const usize&, std::string> v = "";

		std::string err;
		const usize* val = v iferr_return_with(err) err.empty();
		EXPECT_FALSE(val);

		return true;
	}

	bool IfErrScopePtr0False() noexcept {
		iferr_scope;

		usize* ptr = nullptr;

		usize* ptr0 = ptr iferr_return false;
		EXPECT_FALSE(ptr0);

		return true;
	}

	bool IfErrScopePtr1True() noexcept {
		iferr_scope;

		usize v = 4;
		usize* ptr = &v;

		ptr iferr_return false;

		usize* ptr0 = ptr iferr_return false;
		EXPECT_TRUE(ptr0);

		//ssize* ptr1 = ptr iferr_return false; // compilation error
		//EXPECT_TRUE(ptr1);

		return true;
	}

	bool IfErrScopeIterator0False() noexcept {
		iferr_scope;

		std::vector<usize> vv{ 4 };

		const auto it = std::find(vv.begin(), vv.end(), 0) iferr_return_with_iter(vv) false;

		return true;
	}

	bool IfErrScopeIterator1True() noexcept {
		iferr_scope;

		std::vector<usize> vv{ 4 };

		const auto it = std::find(vv.begin(), vv.end(), 4) iferr_return_with_iter(vv) false;

		return true;
	}

	bool IfErrScopeOnErrorTrue() noexcept {

		bool isErr = false;

		[&]() {

			iferr_scope_on_err {
				isErr = true;
			};

			Alt::Result<usize, std::string> result = "Error";
			ALT_MAYBE_UNUSED auto _ = result iferr_return;
		}();

		return isErr;
	}

	bool IfErrScopeOnErrorFalse() noexcept {

		bool isErr = false;

		[&]() {

			iferr_scope_on_err {
				isErr = true;
			};

			Alt::Result<usize, std::string> result = 4;
			ALT_MAYBE_UNUSED auto _ = result iferr_return;
		}();

		return isErr;
	}

	bool IfErrScopeOnErrorIgnoreFalse() noexcept {

		bool isErr = false;

		[&]() {

			iferr_scope_on_err {
				isErr = true;
			};

			return;
		}();

		return isErr;
	}

	bool IfErrScopeOnErrorNoIgnoreTrue() noexcept {

		bool isErr = false;

		[&]() {

			iferr_scope_on_err {
				isErr = true;
			};

			iferr_return_return;
		}();

		return isErr;
	}

	bool IfErrScopeUnwrapConstRefTrue() noexcept {
		iferr_scope;

		Alt::Result<usize, std::string> g_Err = "Error";
		const usize etalonNum = g_Err.GetErr().size();

		const auto funcGetErr = [&]() -> const Alt::Result<usize, std::string>& {
			return g_Err;
		};

		const auto funcTest0 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			std::string err;
			const usize num = (funcGetErr)() iferr_return_with(err) err.size();

			return num;
		};

		const auto funcTest1 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::Optional<std::string> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err->size();

			return num;
		};

		{
			const usize num = (funcTest0)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		{
			const usize num = (funcTest1)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		return true;
	}

	bool IfErrScopeUnwrapRefTrue() noexcept {
		iferr_scope;

		Alt::Result<usize, std::string> g_Err = "Error";
		const usize etalonNum = g_Err.GetErr().size();

		const auto funcGetErr = [&]() -> Alt::Result<usize, std::string>& {
			return g_Err;
		};

		const auto funcTest0 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			std::string err;
			const usize num = (funcGetErr)() iferr_return_with(err) err.size();

			return num;
		};

		const auto funcTest1 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::Optional<std::string> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err->size();

			return num;
		};

		{
			const usize num = (funcTest0)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		{
			const usize num = (funcTest1)() iferr_return false;

			// In 'funcTest0' we are 'std::move' error using 'iferr_return_with'
			EXPECT_EQ(num, 0);
		}

		return true;
	}

	bool IfErrScopeUnwrapValueTrue() noexcept {
		iferr_scope;

		const auto funcGetErr = [&]() -> Alt::Result<usize, std::string> {
			return "Error";
		};

		Alt::Result<usize, std::string> etalonErr = (funcGetErr)();
		const usize etalonNum = etalonErr.GetErr().size();

		const auto funcTest0 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			std::string err;
			const usize num = (funcGetErr)() iferr_return_with(err) err.size();

			return num;
		};

		const auto funcTest1 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::Optional<std::string> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err->size();

			return num;
		};

		{
			const usize num = (funcTest0)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		{
			const usize num = (funcTest1)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		return true;
	}

	bool IfErrScopeUnwrapInsideConstRefTrue() noexcept {
		iferr_scope;

		std::string str = "Error";
		Alt::Result<usize, std::string&> g_Err = str;
		const usize etalonNum = str.size();

		const auto funcGetErr = [&]() -> const Alt::Result<usize, std::string&>& {
			return g_Err;
		};

		const auto funcTest0 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			std::string err;
			const usize num = (funcGetErr)() iferr_return_with(err) err.size();

			return num;
		};

		const auto funcTest1 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::Optional<std::string> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err->size();

			return num;
		};

		{
			const usize num = (funcTest0)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		{
			const usize num = (funcTest1)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		return true;
	}

	bool IfErrScopeUnwrapInsideRefTrue() noexcept {
		iferr_scope;

		std::string str = "Error";
		Alt::Result<usize, std::string&> g_Err = str;
		const usize etalonNum = str.size();

		const auto funcGetErr = [&]() -> Alt::Result<usize, std::string&>& {
			return g_Err;
		};

		const auto funcTest0 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			std::string err;
			const usize num = (funcGetErr)() iferr_return_with(err) err.size();

			return num;
		};

		const auto funcTest1 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::Optional<std::string> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err->size();

			return num;
		};

		{
			const usize num = (funcTest0)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		{
			const usize num = (funcTest1)() iferr_return false;

			// In 'funcTest0' we are 'std::move' error using 'iferr_return_with'
			EXPECT_EQ(num, 0);
		}

		return true;
	}

	bool IfErrScopeUnwrapInsideValueTrue() noexcept {
		iferr_scope;

		std::string str = "Error";
		Alt::Result<usize, std::string&> g_Err = str;
		const usize etalonNum = str.size();

		const auto funcGetErr = [&]() -> Alt::Result<usize, std::string&> {
			return g_Err;
		};

		const auto funcTest0 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			std::string err;
			const usize num = (funcGetErr)() iferr_return_with(err) err.size();

			return num;
		};

		const auto funcTest1 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::Optional<std::string> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err->size();

			return num;
		};

		{
			const usize num = (funcTest0)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		{
			const usize num = (funcTest1)() iferr_return false;

			// In 'funcTest0' we are 'std::move' error using 'iferr_return_with'
			EXPECT_EQ(num, 0);
		}

		return true;
	}

	bool IfErrScopeUnwrapOnlyMoveRefTrue() noexcept {
		iferr_scope;

		std::string str = "Error";
		const usize etalonNum = str.size();

		Alt::Result<usize, Alt::UniquePtr<std::string>> g_Err =
			Alt::UniquePtr<std::string>::AllocateObject(std::move(str)) iferr_return false;

		const auto funcGetErr = [&]() -> Alt::Result<usize, Alt::UniquePtr<std::string>>& {
			return g_Err;
		};

		const auto funcTest0 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::UniquePtr<std::string> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err->size();

			return num;
		};

		const auto funcTest1 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::Optional<Alt::UniquePtr<std::string>> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err
				.Map([](Alt::UniquePtr<std::string>& err) { return err->size(); }).GetValueOr(0);

			return num;
		};

		{
			const usize num = (funcTest0)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		{
			const usize num = (funcTest1)() iferr_return false;

			// In 'funcTest0' we are 'std::move' error using 'iferr_return_with'
			EXPECT_EQ(num, 0);
		}

		return true;
	}

	bool IfErrScopeUnwrapOnlyMoveValueTrue() noexcept {
		iferr_scope;

		std::string str = "Error";
		const usize etalonNum = str.size();

		const auto funcGetErr = [&]() -> Alt::Result<usize, Alt::UniquePtr<std::string>> {

			return Alt::UniquePtr<std::string>::AllocateObject(str).GetValueOr(nullptr);
		};

		const auto funcTest0 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::UniquePtr<std::string> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err->size();

			return num;
		};

		const auto funcTest1 = [&]() -> Alt::Optional<usize> {
			iferr_scope;

			Alt::Optional<Alt::UniquePtr<std::string>> err;
			const usize num = (funcGetErr)() iferr_return_with(err) err->Get()->size();

			return num;
		};

		{
			const usize num = (funcTest0)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		{
			const usize num = (funcTest1)() iferr_return false;
			EXPECT_EQ(num, etalonNum);
		}

		return true;
	}
}

TEST(IfErr, Main) {

	EXPECT_TRUE(CheckDefaultConstructorInjection0True());
	EXPECT_TRUE(CheckDefaultConstructorInjection1True());

	EXPECT_FALSE(IfErrScopeOptional0False());
	EXPECT_FALSE(IfErrScopeOptional1False());
	EXPECT_TRUE(IfErrScopeOptional2True());
	EXPECT_TRUE(IfErrScopeOptionalUnwrapRefToPtrTrue());

	EXPECT_FALSE(IfErrScopeResult0False());
	EXPECT_TRUE(IfErrScopeResult1True());

	EXPECT_TRUE(IfErrScopeResultUnwrapRefToPtr0True());
	EXPECT_TRUE(IfErrScopeResultUnwrapRefToPtr1True());
	EXPECT_TRUE(IfErrScopeResultUnwrapRefToPtr2True());

	EXPECT_FALSE(IfErrScopePtr0False());
	EXPECT_TRUE(IfErrScopePtr1True());

	EXPECT_FALSE(IfErrScopeIterator0False());
	EXPECT_TRUE(IfErrScopeIterator1True());

	EXPECT_FALSE(IfErrScopeOnErrorFalse());
	EXPECT_TRUE(IfErrScopeOnErrorTrue());

	EXPECT_FALSE(IfErrScopeOnErrorIgnoreFalse());
	EXPECT_TRUE(IfErrScopeOnErrorNoIgnoreTrue());

	EXPECT_TRUE(IfErrScopeUnwrapConstRefTrue());
	EXPECT_TRUE(IfErrScopeUnwrapRefTrue());
	EXPECT_TRUE(IfErrScopeUnwrapValueTrue());

	EXPECT_TRUE(IfErrScopeUnwrapInsideConstRefTrue());
	EXPECT_TRUE(IfErrScopeUnwrapInsideRefTrue());
	EXPECT_TRUE(IfErrScopeUnwrapInsideValueTrue());

	EXPECT_TRUE(IfErrScopeUnwrapOnlyMoveRefTrue());
	EXPECT_TRUE(IfErrScopeUnwrapOnlyMoveValueTrue());
}
