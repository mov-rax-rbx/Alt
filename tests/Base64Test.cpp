#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Random/Xoshiro256.h"
#include "../alt-std/src/Algorithms/Base64.h"

TEST(Base64, Fuzzy) {
	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize TEST_CNT = 10'000;
	constexpr usize TEST_STR_MAX_SIZE = 1'000;
	constexpr bool IS_ALPHANUM = false;

	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < TEST_CNT; ++i) {

		const Alt::String str = AllocateStr(0, TEST_STR_MAX_SIZE, IS_ALPHANUM);
		const Alt::Slice<const u8> strBytes(str.Data(), str.GetSizeBytes());

		const bool isForUrl = rnd.GenerateU64(0, 1) == 0 ? false : true;

		Alt::Vector<u8> base64 = Alt::Base64::ConvertToBase64(strBytes, isForUrl) iferr_return;
		Alt::Vector<u8> bytes = Alt::Base64::FromBase64(base64) iferr_return;

		EXPECT_EQ(strBytes, bytes);
	}
}
