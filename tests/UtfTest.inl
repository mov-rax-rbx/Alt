#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/String/Utf.h"
#include "../alt-std/src/Collections/Vector.h"
#include "../alt-std/src/Platform/Platform.h"

#include "../alt-std/src/Utils/Range.h"

namespace {

#pragma warning(push)
#pragma warning(disable: 4244 /* conversion from '...' to '...', possible loss of data */)

struct UtfTest {

	u32 utf32;
	Alt::StaticVector<u16, 2> utf16;
	Alt::StaticVector<u8, 4> utf8;
};

const auto s_WellFormedUtf8 = Alt::MakeArray(
	  Alt::Array<Alt::RangeU8, 4>{ Alt::RangeU8{ 0x00, 0x7F } }
	, Alt::Array<Alt::RangeU8, 4>{ Alt::RangeU8{ 0xC2, 0xDF }, Alt::RangeU8{ 0x80, 0xBF } }
	, Alt::Array<Alt::RangeU8, 4>{ Alt::RangeU8{ 0xE0, 0xE0 }, Alt::RangeU8{ 0xA0, 0xBF }, Alt::RangeU8{ 0x80, 0xBF } }
	, Alt::Array<Alt::RangeU8, 4>{ Alt::RangeU8{ 0xE1, 0xEC }, Alt::RangeU8{ 0x80, 0xBF }, Alt::RangeU8{ 0x80, 0xBF } }
	, Alt::Array<Alt::RangeU8, 4>{ Alt::RangeU8{ 0xED, 0xED }, Alt::RangeU8{ 0x80, 0x9F }, Alt::RangeU8{ 0x80, 0xBF } }
	, Alt::Array<Alt::RangeU8, 4>{ Alt::RangeU8{ 0xEE, 0xEF }, Alt::RangeU8{ 0x80, 0xBF }, Alt::RangeU8{ 0x80, 0xBF } }
	, Alt::Array<Alt::RangeU8, 4>{ Alt::RangeU8{ 0xF0, 0xF0 }, Alt::RangeU8{ 0x90, 0xBF }, Alt::RangeU8{ 0x80, 0xBF }, Alt::RangeU8{ 0x80, 0xBF } }
	, Alt::Array<Alt::RangeU8, 4>{ Alt::RangeU8{ 0xF1, 0xF3 }, Alt::RangeU8{ 0x80, 0xBF }, Alt::RangeU8{ 0x80, 0xBF }, Alt::RangeU8{ 0x80, 0xBF } }
	, Alt::Array<Alt::RangeU8, 4>{ Alt::RangeU8{ 0xF4, 0xF4 }, Alt::RangeU8{ 0x80, 0x8F }, Alt::RangeU8{ 0x80, 0xBF }, Alt::RangeU8{ 0x80, 0xBF } }
);

struct BomTest {

	Alt::StaticVector<u8, 16> bytes;
	Alt::Utf::Encoding encoding = Alt::Utf::Encoding::Unknown;
	usize bomSize = 0;
	bool isLittleEndian = true;
};

const auto s_BytesWithBom = Alt::MakeArray(

	  BomTest{ { 0xEF, 0xBB, 0xBF }, Alt::Utf::Encoding::Utf8, 3, true }
	, BomTest{ { 0xEF, 0xBB, 0xBF, 0x00, 0x00 }, Alt::Utf::Encoding::Utf8, 3, true }
	, BomTest{ { 0xEF, 0xBB, 0xBF, 0x01, 0x01 }, Alt::Utf::Encoding::Utf8, 3, true }
	, BomTest{ { 0xEF, 0xBB, 0xBF, 0x01, 0x02 }, Alt::Utf::Encoding::Utf8, 3, true }

	, BomTest{ { 0xFE, 0xFF }, Alt::Utf::Encoding::Utf16, 2, false }
	, BomTest{ { 0xFE, 0xFF, 0x00, 0x00 }, Alt::Utf::Encoding::Utf16, 2, false }
	, BomTest{ { 0xFE, 0xFF, 0x01, 0x01 }, Alt::Utf::Encoding::Utf16, 2, false }
	, BomTest{ { 0xFE, 0xFF, 0x01, 0x02 }, Alt::Utf::Encoding::Utf16, 2, false }

	, BomTest{ { 0xFF, 0xFE }, Alt::Utf::Encoding::Utf16, 2, true }
	, BomTest{ { 0xFF, 0xFE, 0x00, 0x02 }, Alt::Utf::Encoding::Utf16, 2, true }
	, BomTest{ { 0xFF, 0xFE, 0x01, 0x01 }, Alt::Utf::Encoding::Utf16, 2, true }
	, BomTest{ { 0xFF, 0xFE, 0x01, 0x02 }, Alt::Utf::Encoding::Utf16, 2, true }

	, BomTest{ { 0x00, 0x00, 0xFE, 0xFF }, Alt::Utf::Encoding::Utf32, 4, false }
	, BomTest{ { 0x01, 0x01, 0xFE, 0xFF }, Alt::Utf::Encoding::Unknown, 0, true }
	, BomTest{ { 0x01, 0x02, 0xFE, 0xFF }, Alt::Utf::Encoding::Unknown, 0, true }

	, BomTest{ { 0xFF, 0xFE, 0x00, 0x00 }, Alt::Utf::Encoding::Utf32, 4, true }
);

static const auto s_Utf32Test = Alt::MakeArray(
	 UtfTest{
		0x0000004D,
		{ 0x004D },
		{ 0x4D },
	}
	, UtfTest{
		0x00000430,
		{ 0x0430 },
		{ 0xD0, 0xB0 },
	}
	, UtfTest{
		0x00004E8C,
		{ 0x4E8C },
		{ 0xE4, 0xBA, 0x8C },
	}
	, UtfTest{
		0x00010302,
		{ 0xD800, 0xDF02 },
		{ 0xF0, 0x90, 0x8C, 0x82 },
	}
	, UtfTest{
		0x0041,
		{ 0x0041 },
		{ 0x41 },
	}
	, UtfTest{
		0x03A9,
		{ 0x03A9 },
		{ 0xCE, 0xA9 },
	}
	, UtfTest{
		0x10000,
		{ 0xD800, 0xDC00 },
		{ 0xF0, 0x90, 0x80, 0x80 },
	}
	, UtfTest{
		0x1F600,
		{ 0xD83D, 0xDE00 },
		{ 0xF0, 0x9F, 0x98, 0x80 },
	}
	, UtfTest{
		0x0000,
		{ 0x0000 },
		{ 0x00 },
	}
	, UtfTest{
		0x007F,
		{ 0x007F },
		{ 0x7F },
	}
	, UtfTest{
		0x0080,
		{ 0x0080 },
		{ 0xC2, 0x80 },
	}
	, UtfTest{
		0x10FFFF,
		{ 0xDBFF, 0xDFFF },
		{ 0xF4, 0x8F, 0xBF, 0xBF },
	}
	, UtfTest{
		0x07FF,
		{ 0x07FF },
		{ 0xDF, 0xBF },
	}
	, UtfTest{
		0x0800,
		{ 0x0800 },
		{ 0xE0, 0xA0, 0x80 },
	}
	, UtfTest{
		0x0FFF,
		{ 0x0FFF },
		{ 0xE0, 0xBF, 0xBF },
	}
	, UtfTest{
		0x1000,
		{ 0x1000 },
		{ 0xE1, 0x80, 0x80 },
	}
	, UtfTest{
		0xCFFF,
		{ 0xCFFF },
		{ 0xEC, 0xBF, 0xBF },
	}
	, UtfTest{
		0xD000,
		{ 0xD000 },
		{ 0xED, 0x80, 0x80 },
	}
	, UtfTest{
		0xD7FF,
		{ 0xD7FF },
		{ 0xED, 0x9F, 0xBF },
	}
	, UtfTest{
		0xE000,
		{ 0xE000 },
		{ 0xEE, 0x80, 0x80 },
	}
	, UtfTest{
		0xFFFF,
		{ 0xFFFF },
		{ 0xEF, 0xBF, 0xBF },
	}
	, UtfTest{
		0x3FFFF,
		{ 0xD8BF, 0xDFFF },
		{ 0xF0, 0xBF, 0xBF, 0xBF },
	}
	, UtfTest{
		0x40000,
		{ 0xD8C0, 0xDC00 },
		{ 0xF1, 0x80, 0x80, 0x80 },
	}
	, UtfTest{
		0xFFFFF,
		{ 0xDBBF, 0xDFFF },
		{ 0xF3, 0xBF, 0xBF, 0xBF },
	}
	, UtfTest{
		0x100000,
		{ 0xDBC0, 0xDC00 },
		{ 0xF4, 0x80, 0x80, 0x80 },
	}
	, UtfTest{
		0x10FFFF,
		{ 0xDBFF, 0xDFFF },
		{ 0xF4, 0x8F, 0xBF, 0xBF },
	}

	// idx == 26
	, UtfTest{
		0x110000,
		{ 0xD800, 0xD800 },
		{ 0xFF },
	}
	, UtfTest{
		0xFFFFFFFF,
		{ 0xD800, 0xD800 },
		{ 0xFF },
	}
);

struct UtfText {

	Alt::Vector<u32> utf32s;
	Alt::Vector<u16> utf16;
	Alt::Vector<u8> utf8;
};

static const auto s_UtfText = Alt::MakeArray(
	  UtfText{
		Alt::Vector<u32>::AllocatePush(0x0000041F, 0x00000440, 0x00000438, 0x00000432, 0x00000435, 0x00000442, 0x00000020, 0x0000043C, 0x00000438, 0x00000440, 0x00000021).GetValueOrDefault(),
		Alt::Vector<u16>::AllocatePush(0x041F, 0x0440, 0x0438, 0x0432, 0x0435, 0x0442, 0x0020, 0x043C, 0x0438, 0x0440, 0x0021).GetValueOrDefault(),
		Alt::Vector<u8>::AllocatePush(0xD0, 0x9F, 0xD1, 0x80, 0xD0, 0xB8, 0xD0 ,0xB2 ,0xD0 ,0xB5 ,0xD1 ,0x82 ,0x20 ,0xD0 ,0xBC ,0xD0 ,0xB8 ,0xD1 ,0x80 ,0x21).GetValueOrDefault(),
	}
	, UtfText{
		Alt::Vector<u32>::AllocatePush(0x00000041, 0x0000006C, 0x0000006C, 0x00000020, 0x00000074, 0x00000068, 0x00000061, 0x00000074, 0x00000020, 0x00000067, 0x0000006C, 0x00000069, 0x00000074, 0x00000074, 0x00000065, 0x00000072, 0x00000073, 0x00000020, 0x00000069, 0x00000073, 0x00000020, 0x0000006E, 0x0000006F, 0x00000074, 0x00000020, 0x00000067, 0x0000006F, 0x0000006C, 0x00000064, 0x0000002E).GetValueOrDefault(),
		Alt::Vector<u16>::AllocatePush(0x0041, 0x006C, 0x006C, 0x0020, 0x0074, 0x0068, 0x0061, 0x0074, 0x0020, 0x0067, 0x006C, 0x0069, 0x0074, 0x0074, 0x0065, 0x0072, 0x0073, 0x0020, 0x0069, 0x0073, 0x0020, 0x006E, 0x006F, 0x0074, 0x0020, 0x0067, 0x006F, 0x006C, 0x0064, 0x002E).GetValueOrDefault(),
		Alt::Vector<u8>::AllocatePush(0x41, 0x6C, 0x6C, 0x20, 0x74, 0x68, 0x61, 0x74, 0x20, 0x67, 0x6C, 0x69, 0x74, 0x74, 0x65, 0x72, 0x73, 0x20, 0x69, 0x73, 0x20, 0x6E, 0x6F, 0x74, 0x20, 0x67, 0x6F, 0x6C, 0x64, 0x2E).GetValueOrDefault(),
	}
	, UtfText{
		Alt::Vector<u32>::AllocatePush(0x00000412, 0x00000441, 0x00000435, 0x00000020, 0x00000441, 0x0000043C, 0x00000435, 0x00000448, 0x00000430, 0x0000043B, 0x0000043E, 0x00000020, 0x00000442, 0x0000044C, 0x00000020, 0x00000432, 0x00000020, 0x00000434, 0x0000043E, 0x0000043C, 0x00000435, 0x00000020, 0x0000041E, 0x00000431, 0x0000043B, 0x0000043E, 0x0000043D, 0x00000441, 0x0000043A, 0x00000438, 0x00000445, 0x0000002E).GetValueOrDefault(),
		Alt::Vector<u16>::AllocatePush(0x0412, 0x0441, 0x0435, 0x0020, 0x0441, 0x043C, 0x0435, 0x0448, 0x0430, 0x043B, 0x043E, 0x0020, 0x0442, 0x044C, 0x0020,0x0432, 0x0020, 0x0434, 0x043E, 0x043C, 0x0435, 0x0020, 0x041E, 0x0431, 0x043B, 0x043E, 0x043D, 0x0441, 0x043A, 0x0438, 0x0445, 0x002E).GetValueOrDefault(),
		Alt::Vector<u8>::AllocatePush(0xD0, 0x92, 0xD1, 0x81, 0xD0, 0xB5, 0x20, 0xD1, 0x81, 0xD0, 0xBC, 0xD0, 0xB5, 0xD1, 0x88, 0xD0, 0xB0, 0xD0, 0xBB, 0xD0, 0xBE, 0x20,0xD1, 0x82, 0xD1, 0x8C, 0x20, 0xD0, 0xB2, 0x20, 0xD0, 0xB4, 0xD0, 0xBE, 0xD0, 0xBC, 0xD0, 0xB5, 0x20, 0xD0, 0x9E, 0xD0, 0xB1, 0xD0, 0xBB, 0xD0, 0xBE, 0xD0, 0xBD, 0xD1, 0x81, 0xD0, 0xBA, 0xD0, 0xB8, 0xD1, 0x85, 0x2E).GetValueOrDefault(),
	}
	, UtfText{
		Alt::Vector<u32>::AllocatePush(0x0000004C, 0x00000061, 0x00000020, 0x0000006C, 0x00000075, 0x0000006D, 0x00000069, 0x000000E8, 0x00000072, 0x00000065, 0x00000020, 0x00000065, 0x00000073, 0x00000074, 0x00000020, 0x0000006C, 0x00002019, 0x0000006F, 0x0000006D, 0x00000062, 0x00000072, 0x00000065, 0x00000020, 0x00000064, 0x00000065, 0x00000020, 0x00000044, 0x00000069, 0x00000065, 0x00000075, 0x0000002E).GetValueOrDefault(),
		Alt::Vector<u16>::AllocatePush(0x004C, 0x0061, 0x0020, 0x006C, 0x0075, 0x006D, 0x0069, 0x00E8, 0x0072, 0x0065, 0x0020, 0x0065, 0x0073, 0x0074, 0x0020, 0x006C, 0x2019, 0x006F, 0x006D, 0x0062, 0x0072, 0x0065, 0x0020, 0x0064, 0x0065, 0x0020, 0x0044, 0x0069, 0x0065, 0x0075, 0x002E).GetValueOrDefault(),
		Alt::Vector<u8>::AllocatePush(0x4C, 0x61, 0x20, 0x6C, 0x75, 0x6D, 0x69, 0xC3, 0xA8, 0x72, 0x65, 0x20, 0x65, 0x73, 0x74, 0x20, 0x6C, 0xE2, 0x80, 0x99, 0x6F, 0x6D, 0x62, 0x72, 0x65, 0x20, 0x64, 0x65, 0x20, 0x44, 0x69, 0x65, 0x75, 0x2E).GetValueOrDefault(),
	}
	, UtfText{
		Alt::Vector<u32>::AllocatePush(0x00004EBA, 0x00009593, 0x0000306F, 0x00008003, 0x00003048, 0x0000308B, 0x000082A6, 0x00003067, 0x00003042, 0x0000308B, 0x00003002).GetValueOrDefault(),
		Alt::Vector<u16>::AllocatePush(0x4EBA, 0x9593, 0x306F, 0x8003, 0x3048, 0x308B, 0x82A6, 0x3067, 0x3042, 0x308B, 0x3002).GetValueOrDefault(),
		Alt::Vector<u8>::AllocatePush(0xE4, 0xBA, 0xBA, 0xE9, 0x96, 0x93, 0xE3, 0x81, 0xAF, 0xE8, 0x80, 0x83, 0xE3, 0x81, 0x88, 0xE3, 0x82, 0x8B, 0xE8, 0x8A, 0xA6, 0xE3, 0x81, 0xA7, 0xE3, 0x81, 0x82, 0xE3, 0x82, 0x8B, 0xE3, 0x80, 0x82).GetValueOrDefault(),
	}
	, UtfText{
		Alt::Vector<u32>::AllocatePush(0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01).GetValueOrDefault(),
		Alt::Vector<u16>::AllocatePush(0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01).GetValueOrDefault(),
		Alt::Vector<u8>::AllocatePush(0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01).GetValueOrDefault(),
	}
);

#pragma warning(pop)
}

TEST(UTF_TEST_NAME, Utf32IsValid) {

	EXPECT_TRUE(Alt::Utf::_32::IsValidUtf32(0x0000004D));
	EXPECT_TRUE(Alt::Utf::_32::IsValidUtf32(0x00000430));
	EXPECT_TRUE(Alt::Utf::_32::IsValidUtf32(0x00004E8C));
	EXPECT_TRUE(Alt::Utf::_32::IsValidUtf32(0x00010302));
	EXPECT_TRUE(Alt::Utf::_32::IsValidUtf32(0x10302));

	for (u32 illFormed = 0x0000D800; illFormed <= 0x0000DFFF; ++illFormed) {

		EXPECT_FALSE(Alt::Utf::_32::IsValidUtf32(illFormed));
	}

	EXPECT_TRUE(Alt::Utf::_32::IsValidUtf32(0x0010FFFF));

	EXPECT_FALSE(Alt::Utf::_32::IsValidUtf32(0x0010FFFF + 1));
	EXPECT_FALSE(Alt::Utf::_32::IsValidUtf32(0x0010FFFF + 10));
	EXPECT_FALSE(Alt::Utf::_32::IsValidUtf32(0x0010FFFF + 20));

	//for (u32 illFormed = 0x0010FFFF + 1; illFormed < 0xFFFFFFFF; ++illFormed) {

	//	EXPECT_FALSE(Alt::Utf::_32::IsValidUtf32(illFormed));
	//}
	//EXPECT_FALSE(Alt::Utf::_32::IsValidUtf32(0xFFFFFFFF));
}

TEST(UTF_TEST_NAME, Utf32ToUtf16) {

	for (Alt::Pair<usize, const UtfTest&> val : s_Utf32Test.GetIterator().Enumerate()) {

		const UtfTest& test = val.s();

		if (Alt::Utf::_32::IsValidUtf32(test.utf32)) {

			u16 utf16[2]{};
			Alt::Utf::Result result = Alt::Utf::_32::Utf32ToUtf16(test.utf32, utf16);

			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.consumed, test.utf16.GetSize())
				<< "i: " << val.f();

			if (result.consumed == test.utf16.GetSize()) {

				for (usize i = 0; i < result.consumed; ++i) {

					EXPECT_EQ(utf16[i], test.utf16[i])
						<< "i: " << val.f() << " | " << "idx: " << i;
				}
			}
		}
		else {

			u16 utf16[2]{};
			Alt::Utf::Result result = Alt::Utf::_32::Utf32ToUtf16(test.utf32, utf16);
			EXPECT_FALSE(result.IsOk());
			EXPECT_EQ(result.advance, 1);

			result = Alt::Utf::_32::Utf32ToUtf16Replace(test.utf32, utf16, 0xFFFD);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
			EXPECT_EQ(utf16[0], 0xFFFD);
		}
	}
}

TEST(UTF_TEST_NAME, Utf32ToUtf8) {

	for (Alt::Pair<usize, const UtfTest&> val : s_Utf32Test.GetIterator().Enumerate()) {

		const UtfTest& test = val.s();

		if (Alt::Utf::_32::IsValidUtf32(test.utf32)) {

			u8 utf8[4]{};
			Alt::Utf::Result result = Alt::Utf::_32::Utf32ToUtf8(test.utf32, utf8);

			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.consumed, test.utf8.GetSize())
				<< "i: " << val.f();

			if (result.consumed == test.utf8.GetSize()) {

				for (usize i = 0; i < result.consumed; ++i) {

					EXPECT_EQ(utf8[i], test.utf8[i])
						<< "i: " << val.f() << " | " << "idx: " << i;
				}
			}
		}
		else {

			u8 utf8[4]{};
			Alt::Utf::Result result = Alt::Utf::_32::Utf32ToUtf8(test.utf32, utf8);
			EXPECT_FALSE(result.IsOk());
			EXPECT_EQ(result.advance, 1);

			result = Alt::Utf::_32::Utf32ToUtf8Replace(test.utf32, utf8, 0xFFFD);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 3);
			EXPECT_EQ(utf8[0], 0xEF);
			EXPECT_EQ(utf8[1], 0xBF);
			EXPECT_EQ(utf8[2], 0xBD);
		}
	}
}

TEST(UTF_TEST_NAME, Utf32Validate) {

	iferr_scope_on_err {

		FAIL();
	};

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		const bool isOk = Alt::Utf::ValidateUtf32(test.utf32s) == test.utf32s.GetSize();
		EXPECT_TRUE(isOk);
	}

	const bool isOk = Alt::Utf::ValidateUtf32(Alt::MakeArray<u32>(0xFFFFFFFF)) != 0;
	EXPECT_FALSE(isOk);
}

TEST(UTF_TEST_NAME, LengthUtf32ToUtf16) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF32 = Alt::MakeArray<u32>(
			0x1,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0x1
		);

		{
			const usize size = Alt::Utf::LengthUtf32ToUtf16IgnoreErrors(INVALID_UTF32);
			EXPECT_EQ(size, 1 + 4 * 2 + 1);
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf32ToUtf16(INVALID_UTF32);
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
		}

		{
			constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

			const usize size = Alt::Utf::LengthUtf32ToUtf16Replace(INVALID_UTF32, replaceUtf32);
			EXPECT_EQ(size, 1 + 4 * Alt::Utf::_32::LengthUtf32ToUtf16(replaceUtf32) + 1);
		}
	}

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			const usize size = Alt::Utf::LengthUtf32ToUtf16IgnoreErrors(test.utf32s);
			EXPECT_EQ(size, test.utf16.GetSize());
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf32ToUtf16(test.utf32s);
			EXPECT_EQ(result.advance, test.utf32s.GetSize());
			EXPECT_EQ(result.consumed, test.utf16.GetSize());
		}

		{
			const usize size = Alt::Utf::LengthUtf32ToUtf16Replace(test.utf32s);
			EXPECT_EQ(size, test.utf16.GetSize());
		}
	}
}

TEST(UTF_TEST_NAME, InvalidUtf32ToUtf16) {

	iferr_scope_on_err {

		FAIL();
	};

	const auto INVALID_UTF32 = Alt::MakeArray<u32>(
		0x1,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0x1
	);

	{
		Alt::StaticVector<u16, 2> utf16Invalid;
		{
			usize size;
			Alt::Utf::Utf32ToUtf16IgnoreErrors(Alt::MakeArray(0xFFFFFFFF), nullptr, size);

			utf16Invalid.Resize(size) iferr_return;
			Alt::Utf::Utf32ToUtf16IgnoreErrors(Alt::MakeArray(0xFFFFFFFF), utf16Invalid.Data(), size);
		}

		usize size;
		Alt::Utf::Utf32ToUtf16IgnoreErrors(INVALID_UTF32, nullptr, size);

		Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
		Alt::Utf::Utf32ToUtf16IgnoreErrors(INVALID_UTF32, utf16.Data(), size);

		EXPECT_EQ(utf16[0], 0x01);

		EXPECT_EQ(utf16[1], utf16Invalid[0]);
		EXPECT_EQ(utf16[2], utf16Invalid[1]);

		EXPECT_EQ(utf16[3], utf16Invalid[0]);
		EXPECT_EQ(utf16[4], utf16Invalid[1]);

		EXPECT_EQ(utf16[5], utf16Invalid[0]);
		EXPECT_EQ(utf16[6], utf16Invalid[1]);

		EXPECT_EQ(utf16[7], utf16Invalid[0]);
		EXPECT_EQ(utf16[8], utf16Invalid[1]);

		EXPECT_EQ(utf16[9], 0x01);
	}

	{
		usize size;
		bool isOk = Alt::Utf::Utf32ToUtf16(INVALID_UTF32, nullptr, size);
		EXPECT_TRUE(isOk);
		EXPECT_EQ(size, Alt::Utf::LengthUtf32ToUtf16IgnoreErrors(INVALID_UTF32));

		Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
		isOk = Alt::Utf::Utf32ToUtf16(INVALID_UTF32, utf16.Data(), size);
		EXPECT_FALSE(isOk);
	}

	{
		constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

		usize size;
		Alt::Utf::Utf32ToUtf16Replace(INVALID_UTF32, nullptr, size, replaceUtf32);

		Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
		Alt::Utf::Utf32ToUtf16Replace(INVALID_UTF32, utf16.Data(), size, replaceUtf32);

		EXPECT_EQ(utf16[0], 0x01);

		EXPECT_EQ(utf16[1], replaceUtf32);
		EXPECT_EQ(utf16[2], replaceUtf32);
		EXPECT_EQ(utf16[3], replaceUtf32);
		EXPECT_EQ(utf16[4], replaceUtf32);

		EXPECT_EQ(utf16[5], 0x01);
	}
}

TEST(UTF_TEST_NAME, LengthUtf32ToUtf8) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF32 = Alt::MakeArray<u32>(
			0x1,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0x1
		);

		{
			const usize size = Alt::Utf::LengthUtf32ToUtf8IgnoreErrors(INVALID_UTF32);
			EXPECT_EQ(size, 1 + 4 * 4 + 1);
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf32ToUtf8(INVALID_UTF32);
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
		}

		{
			constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

			const usize size = Alt::Utf::LengthUtf32ToUtf8Replace(INVALID_UTF32, replaceUtf32);
			EXPECT_EQ(size, 1 + 4 * Alt::Utf::_32::LengthUtf32ToUtf8(replaceUtf32) + 1);
		}
	}

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			const usize size = Alt::Utf::LengthUtf32ToUtf8IgnoreErrors(test.utf32s);
			EXPECT_EQ(size, test.utf8.GetSize());
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf32ToUtf8(test.utf32s);
			EXPECT_EQ(result.advance, test.utf32s.GetSize());
			EXPECT_EQ(result.consumed, test.utf8.GetSize());
		}

		{
			const usize size = Alt::Utf::LengthUtf32ToUtf8Replace(test.utf32s);
			EXPECT_EQ(size, test.utf8.GetSize());
		}
	}
}

TEST(UTF_TEST_NAME, InvalidUtf32ToUtf8) {

	iferr_scope_on_err {

		FAIL();
	};

	const auto INVALID_UTF32 = Alt::MakeArray<u32>(
		0x1,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0xFFFFFFFF,
		0x1
	);

	{
		Alt::StaticVector<u8, 4> utf8Invalid;
		{
			usize size;
			Alt::Utf::Utf32ToUtf8IgnoreErrors(Alt::MakeArray(0xFFFFFFFF), nullptr, size);

			utf8Invalid.Resize(size) iferr_return;
			Alt::Utf::Utf32ToUtf8IgnoreErrors(Alt::MakeArray(0xFFFFFFFF), utf8Invalid.Data(), size);
		}

		usize size;
		Alt::Utf::Utf32ToUtf8IgnoreErrors(INVALID_UTF32, nullptr, size);

		Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
		Alt::Utf::Utf32ToUtf8IgnoreErrors(INVALID_UTF32, utf8.Data(), size);

		EXPECT_EQ(utf8[0], 0x01);

		EXPECT_EQ(utf8[1], utf8Invalid[0]);
		EXPECT_EQ(utf8[2], utf8Invalid[1]);
		EXPECT_EQ(utf8[3], utf8Invalid[2]);
		EXPECT_EQ(utf8[4], utf8Invalid[3]);

		EXPECT_EQ(utf8[5], utf8Invalid[0]);
		EXPECT_EQ(utf8[6], utf8Invalid[1]);
		EXPECT_EQ(utf8[7], utf8Invalid[2]);
		EXPECT_EQ(utf8[8], utf8Invalid[3]);

		EXPECT_EQ(utf8[9], utf8Invalid[0]);
		EXPECT_EQ(utf8[10], utf8Invalid[1]);
		EXPECT_EQ(utf8[11], utf8Invalid[2]);
		EXPECT_EQ(utf8[12], utf8Invalid[3]);

		EXPECT_EQ(utf8[13], utf8Invalid[0]);
		EXPECT_EQ(utf8[14], utf8Invalid[1]);
		EXPECT_EQ(utf8[15], utf8Invalid[2]);
		EXPECT_EQ(utf8[16], utf8Invalid[3]);

		EXPECT_EQ(utf8[17], 0x01);
	}

	{
		usize size;
		bool isOk = Alt::Utf::Utf32ToUtf8(INVALID_UTF32, nullptr, size);
		EXPECT_TRUE(isOk);
		EXPECT_EQ(size, Alt::Utf::LengthUtf32ToUtf8IgnoreErrors(INVALID_UTF32));

		Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
		isOk = Alt::Utf::Utf32ToUtf8(INVALID_UTF32, utf8.Data(), size);
		EXPECT_FALSE(isOk);
	}

	{
		constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

		Alt::StaticVector<u8, 3> utf8Invalid;
		{
			usize size;
			Alt::Utf::Utf32ToUtf8IgnoreErrors(Alt::MakeArray((u32)replaceUtf32), nullptr, size);

			utf8Invalid.Resize(size) iferr_return;
			Alt::Utf::Utf32ToUtf8IgnoreErrors(Alt::MakeArray((u32)replaceUtf32), utf8Invalid.Data(), size);
		}

		usize size;
		Alt::Utf::Utf32ToUtf8Replace(INVALID_UTF32, nullptr, size, replaceUtf32);

		Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
		Alt::Utf::Utf32ToUtf8Replace(INVALID_UTF32, utf8.Data(), size, replaceUtf32);

		EXPECT_EQ(utf8[0], 0x01);

		EXPECT_EQ(utf8[1], utf8Invalid[0]);
		EXPECT_EQ(utf8[2], utf8Invalid[1]);
		EXPECT_EQ(utf8[3], utf8Invalid[2]);

		EXPECT_EQ(utf8[4], utf8Invalid[0]);
		EXPECT_EQ(utf8[5], utf8Invalid[1]);
		EXPECT_EQ(utf8[6], utf8Invalid[2]);

		EXPECT_EQ(utf8[7], utf8Invalid[0]);
		EXPECT_EQ(utf8[8], utf8Invalid[1]);
		EXPECT_EQ(utf8[9], utf8Invalid[2]);

		EXPECT_EQ(utf8[10], utf8Invalid[0]);
		EXPECT_EQ(utf8[11], utf8Invalid[1]);
		EXPECT_EQ(utf8[12], utf8Invalid[2]);

		EXPECT_EQ(utf8[13], 0x01);
	}
}

TEST(UTF_TEST_NAME, ConvertUtf32ToUtf8) {

	iferr_scope_on_err {

		FAIL();
	};

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			usize size;
			Alt::Utf::Utf32ToUtf8IgnoreErrors(test.utf32s, nullptr, size);
			EXPECT_EQ(size, test.utf8.GetSize());

			Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
			Alt::Utf::Utf32ToUtf8IgnoreErrors(test.utf32s, utf8.Data(), size);

			const usize cnt = utf8
				.GetIterator()
				.Zip(test.utf8.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u8, u8>> val_) {

					const Alt::Pair<u8, u8>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			bool isOk = true;

			usize size;
			isOk = Alt::Utf::Utf32ToUtf8(test.utf32s, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, test.utf8.GetSize());

			Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf32ToUtf8(test.utf32s, utf8.Data(), size);
			EXPECT_TRUE(isOk);

			const usize cnt = utf8
				.GetIterator()
				.Zip(test.utf8.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u8, u8>> val_) {

					const Alt::Pair<u8, u8>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			usize size;
			Alt::Utf::Utf32ToUtf8Replace(test.utf32s, nullptr, size);
			EXPECT_EQ(size, test.utf8.GetSize());

			Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
			Alt::Utf::Utf32ToUtf8Replace(test.utf32s, utf8.Data(), size);

			const usize cnt = utf8
				.GetIterator()
				.Zip(test.utf8.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u8, u8>> val_) {

					const Alt::Pair<u8, u8>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}
	}
}

TEST(UTF_TEST_NAME, ConvertUtf32ToUtf16) {

	iferr_scope_on_err {

		FAIL();
	};

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			usize size;
			Alt::Utf::Utf32ToUtf16IgnoreErrors(test.utf32s, nullptr, size);
			EXPECT_EQ(size, test.utf16.GetSize());

			Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
			Alt::Utf::Utf32ToUtf16IgnoreErrors(test.utf32s, utf16.Data(), size);

			const usize cnt = utf16
				.GetIterator()
				.Zip(test.utf16.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u16, u16>> val_) {

					const Alt::Pair<u16, u16>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			bool isOk = true;

			usize size;
			isOk = Alt::Utf::Utf32ToUtf16(test.utf32s, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, test.utf16.GetSize());

			Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf32ToUtf16(test.utf32s, utf16.Data(), size);
			EXPECT_TRUE(isOk);

			const usize cnt = utf16
				.GetIterator()
				.Zip(test.utf16.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u16, u16>> val_) {

					const Alt::Pair<u16, u16>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			usize size;
			Alt::Utf::Utf32ToUtf16Replace(test.utf32s, nullptr, size);
			EXPECT_EQ(size, test.utf16.GetSize());

			Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
			Alt::Utf::Utf32ToUtf16Replace(test.utf32s, utf16.Data(), size);

			const usize cnt = utf16
				.GetIterator()
				.Zip(test.utf16.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u16, u16>> val_) {

					const Alt::Pair<u16, u16>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}
	}
}

TEST(UTF_TEST_NAME, Utf16IsValid) {

	EXPECT_TRUE(Alt::Utf::_16::IsValidUtf16({ 0xD800, 0xDF02 }));

	for (u16 utf16 = 0x0; utf16 <= 0xD7FF; ++utf16) {

		EXPECT_TRUE(Alt::Utf::_16::IsValidUtf16({ utf16, 0 }));

		EXPECT_FALSE(Alt::Utf::_16::IsHighSurrogateUtf16(utf16));
		EXPECT_FALSE(Alt::Utf::_16::IsLowSurrogateUtf16(utf16));
	}

	for (u16 utf16 = 0xE000; utf16 < 0xFFFF; ++utf16) {

		EXPECT_TRUE(Alt::Utf::_16::IsValidUtf16({ utf16, 0 }));

		EXPECT_FALSE(Alt::Utf::_16::IsHighSurrogateUtf16(utf16));
		EXPECT_FALSE(Alt::Utf::_16::IsLowSurrogateUtf16(utf16));
	}
	EXPECT_TRUE(Alt::Utf::_16::IsValidUtf16({ 0xFFFF, 0 }));

	// High-Surrogate
	for (u16 utf16First = 0xD800; utf16First <= 0xDBFF; ++utf16First) {

		EXPECT_TRUE(Alt::Utf::_16::IsHighSurrogateUtf16(utf16First));
		EXPECT_FALSE(Alt::Utf::_16::IsLowSurrogateUtf16(utf16First));

		// Low-Surrogate
		for (u16 utf16Second = 0xDC00; utf16Second <= 0xDFFF; ++utf16Second) {

			EXPECT_TRUE(Alt::Utf::_16::IsValidUtf16({ utf16First, utf16Second }));

			EXPECT_FALSE(Alt::Utf::_16::IsHighSurrogateUtf16(utf16Second));
			EXPECT_TRUE(Alt::Utf::_16::IsLowSurrogateUtf16(utf16Second));
		}

		for (u16 utf16Second = 0; utf16Second < 0xDC00; ++utf16Second) {

			EXPECT_FALSE(Alt::Utf::_16::IsValidUtf16({ utf16First, utf16Second }));

			EXPECT_FALSE(Alt::Utf::_16::IsLowSurrogateUtf16(utf16Second));
		}

		for (u16 utf16Second = 0xDFFF + 1; utf16Second < 0xFFFF; ++utf16Second) {

			EXPECT_FALSE(Alt::Utf::_16::IsValidUtf16({ utf16First, utf16Second }));

			EXPECT_FALSE(Alt::Utf::_16::IsLowSurrogateUtf16(utf16Second));
		}
		EXPECT_FALSE(Alt::Utf::_16::IsValidUtf16({ utf16First, 0xFFFF }));
		EXPECT_FALSE(Alt::Utf::_16::IsLowSurrogateUtf16(0xFFFF));
	}
}

TEST(UTF_TEST_NAME, Utf16ToUtf8) {

	for (Alt::Pair<usize, const UtfTest&> val : s_Utf32Test.GetIterator().Enumerate()) {

		const UtfTest& test = val.s();

		const u16 utf16[2]{
			0 < test.utf16.GetSize() ? test.utf16[0] : (u16)0,
			1 < test.utf16.GetSize() ? test.utf16[1] : (u16)0,
		};

		if (Alt::Utf::_32::IsValidUtf32(test.utf32)) {

			u8 utf8[4]{};
			Alt::Utf::Result result = Alt::Utf::_16::Utf16ToUtf8(utf16, utf8);

			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.consumed, test.utf8.GetSize())
				<< "i: " << val.f();

			if (result.consumed == test.utf8.GetSize()) {

				for (usize i = 0; i < result.consumed; ++i) {

					EXPECT_EQ(utf8[i], test.utf8[i])
						<< "i: " << val.f() << " | " << "idx: " << i;
				}
			}
		}
		else {

			u8 utf8[4]{};
			Alt::Utf::Result result = Alt::Utf::_16::Utf16ToUtf8(utf16, utf8);
			EXPECT_FALSE(result.IsOk());
			EXPECT_EQ(result.advance, 1);

			result = Alt::Utf::_16::Utf16ToUtf8Replace(utf16, utf8, 0xFFFD);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 3);
			EXPECT_EQ(utf8[0], 0xEF);
			EXPECT_EQ(utf8[1], 0xBF);
			EXPECT_EQ(utf8[2], 0xBD);
		}
	}
}

TEST(UTF_TEST_NAME, Utf16ToUtf32) {

	for (Alt::Pair<usize, const UtfTest&> val : s_Utf32Test.GetIterator().Enumerate()) {

		const UtfTest& test = val.s();

		const u16 utf16[2]{
			0 < test.utf16.GetSize() ? test.utf16[0] : (u16)0,
			1 < test.utf16.GetSize() ? test.utf16[1] : (u16)0,
		};

		if (Alt::Utf::_32::IsValidUtf32(test.utf32)) {

			u32 utf32;
			Alt::Utf::Result result = Alt::Utf::_16::Utf16ToUtf32(utf16, utf32);

			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.consumed, 1)
				<< "i: " << val.f();

			EXPECT_EQ(utf32, test.utf32);
		}
		else {

			u32 utf32;
			Alt::Utf::Result result = Alt::Utf::_16::Utf16ToUtf32(utf16, utf32);
			EXPECT_FALSE(result.IsOk());
			EXPECT_EQ(result.advance, 1);

			result = Alt::Utf::_16::Utf16ToUtf32Replace(utf16, utf32, 0xFFFD);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
			EXPECT_EQ(utf32, (u32)0xFFFD);
		}
	}
}

TEST(UTF_TEST_NAME, Utf16Validate) {

	iferr_scope_on_err {

		FAIL();
	};

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		const bool isOk = Alt::Utf::ValidateUtf16(test.utf16) == test.utf16.GetSize();
		EXPECT_TRUE(isOk);
	}
}

TEST(UTF_TEST_NAME, LengthUtf16ToUtf32) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF16 = Alt::MakeArray<u16>(
			0x1,
			0xD800, // valid
			0xD7FF, // invalid
			0xDC00, // invalid
			0xDBFF, // valid
			0x1
		);

		{
			const usize size = Alt::Utf::LengthUtf16ToUtf32IgnoreErrors(INVALID_UTF16);
			EXPECT_EQ(size, 1 + 4 * 1 + 1);
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf16ToUtf32(INVALID_UTF16);
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
		}

		{
			const usize size = Alt::Utf::LengthUtf16ToUtf32Replace(INVALID_UTF16);
			EXPECT_EQ(size, 1 + 4 * 1 + 1);
		}
	}

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			const usize size = Alt::Utf::LengthUtf16ToUtf32IgnoreErrors(test.utf16);
			EXPECT_EQ(size, test.utf32s.GetSize());
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf16ToUtf32(test.utf16);
			EXPECT_EQ(result.advance, test.utf16.GetSize());
			EXPECT_EQ(result.consumed, test.utf32s.GetSize());
		}

		{
			const usize size = Alt::Utf::LengthUtf16ToUtf32Replace(test.utf16);
			EXPECT_EQ(size, test.utf32s.GetSize());
		}
	}
}

TEST(UTF_TEST_NAME, InvalidUtf16ToUtf32) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF16 = Alt::MakeArray<u16>(
			0x1,
			0xD800, // valid
			0xD7FF, // invalid surrogate (but valid scalar)
			0xDC00, // invalid
			0xDBFF, // valid surrogate (but invalid scalar)
			0x1
		);

		{
			usize size;
			Alt::Utf::Utf16ToUtf32IgnoreErrors(INVALID_UTF16, nullptr, size);

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			Alt::Utf::Utf16ToUtf32IgnoreErrors(INVALID_UTF16, utf32s.Data(), size);

			EXPECT_EQ(utf32s[0], (u32)0x1);

			EXPECT_EQ(utf32s[5], (u32)0);
		}

		{
			usize size;
			bool isOk = Alt::Utf::Utf16ToUtf32(INVALID_UTF16, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, Alt::Utf::LengthUtf16ToUtf32IgnoreErrors(INVALID_UTF16));

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf16ToUtf32(INVALID_UTF16, utf32s.Data(), size);
			EXPECT_FALSE(isOk);
		}

		{
			constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

			usize size;
			Alt::Utf::Utf16ToUtf32Replace(INVALID_UTF16, nullptr, size, replaceUtf32);

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			Alt::Utf::Utf16ToUtf32Replace(INVALID_UTF16, utf32s.Data(), size, replaceUtf32);

			EXPECT_EQ(utf32s[0], (u32)0x1);

			EXPECT_EQ(utf32s[1], replaceUtf32);
			EXPECT_EQ(utf32s[2], (u32)0xD7FF);
			EXPECT_EQ(utf32s[3], replaceUtf32);
			EXPECT_EQ(utf32s[4], replaceUtf32);

			EXPECT_EQ(utf32s[5], (u32)0x1);
		}
	}
}

TEST(UTF_TEST_NAME, LengthUtf16ToUtf8) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF16 = Alt::MakeArray<u16>(
			0x1,
			0xD800, // valid
			0xD7FF, // invalid
			0xDC00, // invalid
			0xDBFF, // valid
			0x1
		);

		{
			const usize size = Alt::Utf::LengthUtf16ToUtf8IgnoreErrors(INVALID_UTF16);
			EXPECT_EQ(size, 1 + 4 + 1 + 4);
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf16ToUtf8(INVALID_UTF16);
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
		}

		{
			constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

			const usize size = Alt::Utf::LengthUtf16ToUtf8Replace(INVALID_UTF16, replaceUtf32);
			EXPECT_EQ(size, 1 + 4 * Alt::Utf::_32::LengthUtf32ToUtf8(replaceUtf32) + 1);
		}
	}

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			const usize size = Alt::Utf::LengthUtf16ToUtf8IgnoreErrors(test.utf16);
			EXPECT_EQ(size, test.utf8.GetSize());
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf16ToUtf8(test.utf16);
			EXPECT_EQ(result.advance, test.utf16.GetSize());
			EXPECT_EQ(result.consumed, test.utf8.GetSize());
		}

		{
			const usize size = Alt::Utf::LengthUtf16ToUtf8Replace(test.utf16);
			EXPECT_EQ(size, test.utf8.GetSize());
		}
	}
}

TEST(UTF_TEST_NAME, InvalidUtf16ToUtf8) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF16 = Alt::MakeArray<u16>(
			0x1,
			0xD800, // valid
			0xD7FF, // invalid surrogate (but valid scalar)
			0xDC00, // invalid
			0xDBFF, // valid surrogate (but invalid scalar)
			0x1
		);

		{
			usize size;
			Alt::Utf::Utf16ToUtf8IgnoreErrors(INVALID_UTF16, nullptr, size);

			Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
			Alt::Utf::Utf16ToUtf8IgnoreErrors(INVALID_UTF16, utf8.Data(), size);

			EXPECT_EQ(utf8[0], 0x1);

			EXPECT_EQ(utf8[9], 0x1);
		}

		{
			usize size;
			bool isOk = Alt::Utf::Utf16ToUtf8(INVALID_UTF16, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, Alt::Utf::LengthUtf16ToUtf8IgnoreErrors(INVALID_UTF16));

			Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf16ToUtf8(INVALID_UTF16, utf8.Data(), size);
			EXPECT_FALSE(isOk);
		}

		{
			constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

			Alt::StaticVector<u8, 3> utf8Replace;
			{
				usize size;
				Alt::Utf::Utf32ToUtf8IgnoreErrors(Alt::MakeArray((u32)replaceUtf32), nullptr, size);

				utf8Replace.Resize(size) iferr_return;
				Alt::Utf::Utf32ToUtf8IgnoreErrors(Alt::MakeArray((u32)replaceUtf32), utf8Replace.Data(), size);
			}

			Alt::StaticVector<u8, 3> utf8ValidScalar;
			{
				usize size;
				Alt::Utf::Utf16ToUtf8IgnoreErrors(Alt::MakeArray<u16>(0xD7FF), nullptr, size);

				utf8ValidScalar.Resize(size) iferr_return;
				Alt::Utf::Utf16ToUtf8IgnoreErrors(Alt::MakeArray<u16>(0xD7FF), utf8ValidScalar.Data(), size);
			}

			usize size;
			Alt::Utf::Utf16ToUtf8Replace(INVALID_UTF16, nullptr, size, replaceUtf32);

			Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
			Alt::Utf::Utf16ToUtf8Replace(INVALID_UTF16, utf8.Data(), size, replaceUtf32);

			EXPECT_EQ(utf8[0], 0x1);

			EXPECT_EQ(utf8[1], utf8Replace[0]);
			EXPECT_EQ(utf8[2], utf8Replace[1]);
			EXPECT_EQ(utf8[3], utf8Replace[2]);

			EXPECT_EQ(utf8[4], utf8ValidScalar[0]);
			EXPECT_EQ(utf8[5], utf8ValidScalar[1]);
			EXPECT_EQ(utf8[6], utf8ValidScalar[2]);

			EXPECT_EQ(utf8[7], utf8Replace[0]);
			EXPECT_EQ(utf8[8], utf8Replace[1]);
			EXPECT_EQ(utf8[9], utf8Replace[2]);

			EXPECT_EQ(utf8[10], utf8Replace[0]);
			EXPECT_EQ(utf8[11], utf8Replace[1]);
			EXPECT_EQ(utf8[12], utf8Replace[2]);

			EXPECT_EQ(utf8[13], 0x1);
		}
	}
}

TEST(UTF_TEST_NAME, ConvertUtf16ToUtf32) {

	iferr_scope_on_err {

		FAIL();
	};

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			usize size;
			Alt::Utf::Utf16ToUtf32IgnoreErrors(test.utf16, nullptr, size);
			EXPECT_EQ(size, test.utf32s.GetSize());

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			Alt::Utf::Utf16ToUtf32IgnoreErrors(test.utf16, utf32s.Data(), size);
			EXPECT_EQ(size, 0);

			const usize cnt = utf32s
				.GetIterator()
				.Zip(test.utf32s.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u32, u32>> val_) {

					const Alt::Pair<u32, u32>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			bool isOk = true;

			usize size;
			isOk = Alt::Utf::Utf16ToUtf32(test.utf16, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, test.utf32s.GetSize());

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf16ToUtf32(test.utf16, utf32s.Data(), size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, 0);

			const usize cnt = utf32s
				.GetIterator()
				.Zip(test.utf32s.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u32, u32>> val_) {

					const Alt::Pair<u32, u32>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			usize size;
			Alt::Utf::Utf16ToUtf32Replace(test.utf16, nullptr, size);
			EXPECT_EQ(size, test.utf32s.GetSize());

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			Alt::Utf::Utf16ToUtf32Replace(test.utf16, utf32s.Data(), size);
			EXPECT_EQ(size, 0);

			const usize cnt = utf32s
				.GetIterator()
				.Zip(test.utf32s.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u32, u32>> val_) {

					const Alt::Pair<u32, u32>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}
	}
}

TEST(UTF_TEST_NAME, ConvertUtf16ToUtf8) {

	iferr_scope_on_err {

		FAIL();
	};

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			usize size;
			Alt::Utf::Utf16ToUtf8IgnoreErrors(test.utf16, nullptr, size);
			EXPECT_EQ(size, test.utf8.GetSize());

			Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
			Alt::Utf::Utf16ToUtf8IgnoreErrors(test.utf16, utf8.Data(), size);
			EXPECT_EQ(size, 0);

			const usize cnt = utf8
				.GetIterator()
				.Zip(test.utf8.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u8, u8>> val_) {

					const Alt::Pair<u8, u8>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			bool isOk = true;

			usize size;
			isOk = Alt::Utf::Utf16ToUtf8(test.utf16, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, test.utf8.GetSize());

			Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf16ToUtf8(test.utf16, utf8.Data(), size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, 0);

			const usize cnt = utf8
				.GetIterator()
				.Zip(test.utf8.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u8, u8>> val_) {

					const Alt::Pair<u8, u8>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			usize size;
			Alt::Utf::Utf16ToUtf8Replace(test.utf16, nullptr, size);
			EXPECT_EQ(size, test.utf8.GetSize());

			Alt::Vector<u8> utf8 = Alt::Vector<u8>::Allocate(size) iferr_return;
			Alt::Utf::Utf16ToUtf8Replace(test.utf16, utf8.Data(), size);
			EXPECT_EQ(size, 0);

			const usize cnt = utf8
				.GetIterator()
				.Zip(test.utf8.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u8, u8>> val_) {

					const Alt::Pair<u8, u8>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}
	}
}

TEST(UTF_TEST_NAME, Utf8IsValid) {

	EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8({ 0xC0, 0xAF, 0, 0 }));
	EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8({ 0xE0, 0x9F, 0x80, 0 }));
	EXPECT_TRUE(Alt::Utf::_8::IsValidUtf8({ 0xF4, 0x80, 0x83, 0x92 }));

	for (const Alt::Array<Alt::RangeU8, 4>& ranges : s_WellFormedUtf8.GetIterator()) {

		{
			const u8 utf8[4]{ (u8)(ranges[0].end + 1), 0, 0, 0 };
			EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8(utf8));
		}

		for (u8 i0 = ranges[0].begin; i0 <= ranges[0].end; ++i0) {

			if (Alt::Utf::_8::IsAscii(i0)) {

				const u8 utf8[4]{ i0, 0, 0, 0 };
				EXPECT_TRUE(Alt::Utf::_8::IsValidUtf8(utf8));

				continue;
			}

			{
				const u8 utf8[4]{ i0, (u8)(ranges[1].begin - 1), 0, 0 };
				EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8(utf8));
			}

			{
				const u8 utf8[4]{ i0, (u8)(ranges[1].end + 1), 0, 0 };
				EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8(utf8));
			}

			for (u8 i1 = ranges[1].begin; i1 <= ranges[1].end; ++i1) {

				if (ranges[2].begin != 0 && ranges[2].end) {
					{
						const u8 utf8[4]{ i0, i1, (u8)(ranges[2].begin - 1), 0 };
						EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8(utf8));
					}

					{
						const u8 utf8[4]{ i0, i1, (u8)(ranges[2].end + 1), 0 };
						EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8(utf8));
					}
				}

				for (u8 i2 = ranges[2].begin; i2 <= ranges[2].end; ++i2) {

					if (ranges[3].begin != 0 && ranges[3].end) {
						{
							const u8 utf8[4]{ i0, i1, i2, (u8)(ranges[3].begin - 1) };
							EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8(utf8));
						}

						{
							const u8 utf8[4]{ i0, i1, i2, (u8)(ranges[3].end + 1) };
							EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8(utf8));
						}
					}

					for (u8 i3 = ranges[3].begin; i3 <= ranges[3].end; ++i3) {

						const u8 utf8[4]{ i0, i1, i2, i3 };
						EXPECT_TRUE(Alt::Utf::_8::IsValidUtf8(utf8));
					}
				}
			}
		}
	}

	for (u8 utf8InvalidFirst = 0b11111000; utf8InvalidFirst != 0; ++utf8InvalidFirst) {

		const u8 utf8Invalid[4]{ utf8InvalidFirst, 0, 0, 0 };

		EXPECT_FALSE(Alt::Utf::_8::IsValidUtf8(utf8Invalid));

		EXPECT_EQ(Alt::Utf::_8::ForwardUtf8(utf8Invalid), 0);
		EXPECT_EQ(Alt::Utf::_8::ForwardUtf8IgnoreErrors(utf8Invalid), 1);

		const u8 utf8InvalidBackward[4]{ 0, 0, 0, utf8InvalidFirst };

		EXPECT_EQ(Alt::Utf::_8::BackwardUtf8(utf8InvalidBackward), 0);
		EXPECT_EQ(Alt::Utf::_8::BackwardUtf8IgnoreErrors(utf8InvalidBackward), 1);
	}
}

TEST(UTF_TEST_NAME, Utf8ToUtf32) {

	for (Alt::Pair<usize, const UtfTest&> val : s_Utf32Test.GetIterator().Enumerate()) {

		const UtfTest& test = val.s();

		const u8 utf8[4]{
			0 < test.utf8.GetSize() ? test.utf8[0] : (u8)0,
			1 < test.utf8.GetSize() ? test.utf8[1] : (u8)0,
			2 < test.utf8.GetSize() ? test.utf8[2] : (u8)0,
			3 < test.utf8.GetSize() ? test.utf8[3] : (u8)0,
		};

		if (Alt::Utf::_32::IsValidUtf32(test.utf32)) {

			u32 utf32;
			Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf32(utf8, utf32);

			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.consumed, 1)
				<< "i: " << val.f();

			EXPECT_EQ(utf32, test.utf32);
		}
		else {

			u32 utf32;
			Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf32(utf8, utf32);
			EXPECT_FALSE(result.IsOk());
			EXPECT_EQ(result.advance, 1);

			result = Alt::Utf::_8::Utf8ToUtf32Replace(utf8, utf32, 0xFFFD);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
			EXPECT_EQ(utf32, (u32)0xFFFD);
		}
	}
}

TEST(UTF_TEST_NAME, Utf8ToUtf16) {

	for (Alt::Pair<usize, const UtfTest&> val : s_Utf32Test.GetIterator().Enumerate()) {

		const UtfTest& test = val.s();

		const u8 utf8[4]{
			0 < test.utf8.GetSize() ? test.utf8[0] : (u8)0,
			1 < test.utf8.GetSize() ? test.utf8[1] : (u8)0,
			2 < test.utf8.GetSize() ? test.utf8[2] : (u8)0,
			3 < test.utf8.GetSize() ? test.utf8[3] : (u8)0,
		};

		if (Alt::Utf::_32::IsValidUtf32(test.utf32)) {

			u16 utf16[2]{};
			Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf16(utf8, utf16);

			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.consumed, test.utf16.GetSize())
				<< "i: " << val.f();

			if (result.consumed == test.utf16.GetSize()) {

				for (usize i = 0; i < result.consumed; ++i) {

					EXPECT_EQ(utf16[i], test.utf16[i])
						<< "i: " << val.f() << " | " << "idx: " << i;
				}
			}
		}
		else {

			u16 utf16[2]{};
			Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf16(utf8, utf16);
			EXPECT_FALSE(result.IsOk());
			EXPECT_EQ(result.advance, 1);

			result = Alt::Utf::_8::Utf8ToUtf16Replace(utf8, utf16, 0xFFFD);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
			EXPECT_EQ(utf16[0], 0xFFFD);
		}
	}
}

TEST(UTF_TEST_NAME, Utf8ToUtf32Backward) {

	for (Alt::Pair<usize, const UtfTest&> val : s_Utf32Test.GetIterator().Enumerate()) {

		const UtfTest& test = val.s();

		if (test.utf8.IsEmpty()) {
			continue;
		}

		const u8* pUTF8Begin = test.utf8.Data();
		const u8* pUTF8End = test.utf8.DataEnd();

		const u8 utf8[4]{
			pUTF8Begin <= (pUTF8End - 4) ? *(pUTF8End - 4) : (u8)0,
			pUTF8Begin <= (pUTF8End - 3) ? *(pUTF8End - 3) : (u8)0,
			pUTF8Begin <= (pUTF8End - 2) ? *(pUTF8End - 2) : (u8)0,
			*(pUTF8End - 1),
		};

		if (Alt::Utf::_32::IsValidUtf32(test.utf32)) {
			{
				u32 utf32;
				Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf32ReplaceBackward(utf8, utf32);

				EXPECT_TRUE(result.IsOk());
				EXPECT_EQ(result.consumed, 1)
					<< "i: " << val.f();

				EXPECT_EQ(utf32, test.utf32);
			}

			{
				u32 utf32;
				Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf32Backward(utf8, utf32);

				EXPECT_TRUE(result.IsOk());
				EXPECT_EQ(result.consumed, 1)
					<< "i: " << val.f();

				EXPECT_EQ(utf32, test.utf32);
			}
		}
		else {

			u32 utf32;
			Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf32Backward(utf8, utf32);
			EXPECT_FALSE(result.IsOk());
			EXPECT_EQ(result.advance, 1);

			result = Alt::Utf::_8::Utf8ToUtf32ReplaceBackward(utf8, utf32, 0xFFFD);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
			EXPECT_EQ(utf32, (u32)0xFFFD);
		}
	}

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& text = val.s();

		if (text.utf8.IsEmpty()) {
			continue;
		}

		const u8 utf8[4]{
			text.utf8.Data() <= (text.utf8.DataEnd() - 4) ? *(text.utf8.DataEnd() - 4) : (u8)0,
			text.utf8.Data() <= (text.utf8.DataEnd() - 3) ? *(text.utf8.DataEnd() - 3) : (u8)0,
			text.utf8.Data() <= (text.utf8.DataEnd() - 2) ? *(text.utf8.DataEnd() - 2) : (u8)0,
			*(text.utf8.DataEnd() - 1),
		};

		{
			u32 utf32;
			Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf32IgnoreErrorsBackward(utf8, utf32);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.consumed, 1);
			EXPECT_EQ(utf32, text.utf32s.GetLast());
		}

		{
			u32 utf32;
			Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf32Backward(utf8, utf32);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.consumed, 1);
			EXPECT_EQ(utf32, text.utf32s.GetLast());
		}

		{
			u32 utf32;
			Alt::Utf::Result result = Alt::Utf::_8::Utf8ToUtf32ReplaceBackward(utf8, utf32);
			EXPECT_TRUE(result.IsOk());
			EXPECT_EQ(result.consumed, 1);
			EXPECT_EQ(utf32, text.utf32s.GetLast());
		}
	}
}

TEST(UTF_TEST_NAME, Utf8Validate) {

	iferr_scope_on_err {

		FAIL();
	};

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		const bool isOk = Alt::Utf::ValidateUtf8(test.utf8) == test.utf8.GetSize();
		EXPECT_TRUE(isOk);
	}
}

TEST(UTF_TEST_NAME, LengthUtf8ToUtf32) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF8 = Alt::MakeArray<u8>(
			0x1,
			0b11110111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b11100111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b11001111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b10001111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0x1
		);

		{
			const usize size = Alt::Utf::LengthUtf8ToUtf32IgnoreErrors(INVALID_UTF8);
			EXPECT_EQ(size, 1 + 1 + 1 + 1 + 2 + 1 + 3 + 6);
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf8ToUtf32(INVALID_UTF8);
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
		}

		{
			const usize size = Alt::Utf::LengthUtf8ToUtf32Replace(INVALID_UTF8);
			EXPECT_EQ(size, 1 + (1 + 4 * 1) * 4 + 1);
		}
	}

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			const usize size = Alt::Utf::LengthUtf8ToUtf32IgnoreErrors(test.utf8);
			EXPECT_EQ(size, test.utf32s.GetSize());
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf8ToUtf32(test.utf8);
			EXPECT_EQ(result.advance, test.utf8.GetSize());
			EXPECT_EQ(result.consumed, test.utf32s.GetSize());
		}

		{
			const usize size = Alt::Utf::LengthUtf8ToUtf32Replace(test.utf8);
			EXPECT_EQ(size, test.utf32s.GetSize());
		}
	}
}

TEST(UTF_TEST_NAME, LengthUtf8ToUtf16) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF8 = Alt::MakeArray<u8>(
			0x1,
			0b11110111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b11100111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b11001111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b10001111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0x1
		);

		{
			const usize size = Alt::Utf::LengthUtf8ToUtf16IgnoreErrors(INVALID_UTF8);
			EXPECT_EQ(size, 1 + 2 + 1 + 1 + 2 + 1 + 3 + 6);
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf8ToUtf16(INVALID_UTF8);
			EXPECT_EQ(result.advance, 1);
			EXPECT_EQ(result.consumed, 1);
		}

		{
			constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

			const usize size = Alt::Utf::LengthUtf8ToUtf16Replace(INVALID_UTF8, replaceUtf32);
			EXPECT_EQ(size, 1 + (1 + 4 * Alt::Utf::_32::LengthUtf32ToUtf16(replaceUtf32)) * 4 + 1);
		}
	}

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			const usize size = Alt::Utf::LengthUtf8ToUtf16IgnoreErrors(test.utf8);
			EXPECT_EQ(size, test.utf16.GetSize());
		}

		{
			Alt::Utf::Result result = Alt::Utf::LengthUtf8ToUtf16(test.utf8);
			EXPECT_EQ(result.advance, test.utf8.GetSize());
			EXPECT_EQ(result.consumed, test.utf16.GetSize());
		}

		{
			const usize size = Alt::Utf::LengthUtf8ToUtf16Replace(test.utf8);
			EXPECT_EQ(size, test.utf16.GetSize());
		}
	}
}

TEST(UTF_TEST_NAME, InvalidUtf8ToUtf32) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF8 = Alt::MakeArray<u8>(
			0x1,
			0b11110111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b11100111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b11001111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b10001111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0x1
		);

		{
			usize size;
			Alt::Utf::Utf8ToUtf32IgnoreErrors(INVALID_UTF8, nullptr, size);

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			Alt::Utf::Utf8ToUtf32IgnoreErrors(INVALID_UTF8, utf32s.Data(), size);

			EXPECT_EQ(utf32s[0], (u32)0x1);
			EXPECT_EQ(utf32s[15], (u32)0x1);
		}

		{
			usize size;
			bool isOk = Alt::Utf::Utf8ToUtf32(INVALID_UTF8, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, Alt::Utf::LengthUtf8ToUtf32IgnoreErrors(INVALID_UTF8));

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf8ToUtf32(INVALID_UTF8, utf32s.Data(), size);
			EXPECT_FALSE(isOk);
		}

		{
			constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

			usize size;
			Alt::Utf::Utf8ToUtf32Replace(INVALID_UTF8, nullptr, size, replaceUtf32);

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			Alt::Utf::Utf8ToUtf32Replace(INVALID_UTF8, utf32s.Data(), size, replaceUtf32);

			EXPECT_EQ(utf32s[0], (u32)0x1);

			EXPECT_EQ(utf32s[1], replaceUtf32);
			EXPECT_EQ(utf32s[2], replaceUtf32);
			EXPECT_EQ(utf32s[3], replaceUtf32);
			EXPECT_EQ(utf32s[4], replaceUtf32);

			EXPECT_EQ(utf32s[5], replaceUtf32);
			EXPECT_EQ(utf32s[6], replaceUtf32);
			EXPECT_EQ(utf32s[7], replaceUtf32);
			EXPECT_EQ(utf32s[8], replaceUtf32);

			EXPECT_EQ(utf32s[21], (u32)0x1);
		}
	}
}

TEST(UTF_TEST_NAME, ConvertUtf8ToUtf32) {

	iferr_scope_on_err {

		FAIL();
	};

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			usize size;
			Alt::Utf::Utf8ToUtf32IgnoreErrors(test.utf8, nullptr, size);
			EXPECT_EQ(size, test.utf32s.GetSize());

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			Alt::Utf::Utf8ToUtf32IgnoreErrors(test.utf8, utf32s.Data(), size);
			EXPECT_EQ(size, 0);

			const usize cnt = utf32s
				.GetIterator()
				.Zip(test.utf32s.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u32, u32>> val_) {

					const Alt::Pair<u32, u32>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			bool isOk = true;

			usize size;
			isOk = Alt::Utf::Utf8ToUtf32(test.utf8, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, test.utf32s.GetSize());

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf8ToUtf32(test.utf8, utf32s.Data(), size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, 0);

			const usize cnt = utf32s
				.GetIterator()
				.Zip(test.utf32s.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u32, u32>> val_) {

					const Alt::Pair<u32, u32>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			usize size;
			Alt::Utf::Utf8ToUtf32Replace(test.utf8, nullptr, size);
			EXPECT_EQ(size, test.utf32s.GetSize());

			Alt::Vector<u32> utf32s = Alt::Vector<u32>::Allocate(size) iferr_return;
			Alt::Utf::Utf8ToUtf32Replace(test.utf8, utf32s.Data(), size);
			EXPECT_EQ(size, 0);

			const usize cnt = utf32s
				.GetIterator()
				.Zip(test.utf32s.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u32, u32>> val_) {

					const Alt::Pair<u32, u32>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}
	}
}

TEST(UTF_TEST_NAME, InvalidUtf8ToUtf16) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto INVALID_UTF8 = Alt::MakeArray<u8>(
			0x1,
			0b11110111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b11100111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b11001111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0b10001111, // invalid
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0x1
		);

		{
			usize size;
			Alt::Utf::Utf8ToUtf16IgnoreErrors(INVALID_UTF8, nullptr, size);

			Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
			Alt::Utf::Utf8ToUtf16IgnoreErrors(INVALID_UTF8, utf16.Data(), size);

			EXPECT_EQ(utf16[0], (u16)0x1);
			EXPECT_EQ(utf16[16], (u16)0x1);
		}

		{
			usize size;
			bool isOk = Alt::Utf::Utf8ToUtf16(INVALID_UTF8, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, Alt::Utf::LengthUtf8ToUtf16IgnoreErrors(INVALID_UTF8));

			Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf8ToUtf16(INVALID_UTF8, utf16.Data(), size);
			EXPECT_FALSE(isOk);
		}

		{
			constexpr u32 replaceUtf32 = Alt::Utf::GetSubstitutionForInvalidUTF32();

			Alt::StaticVector<u16, 1> utf16Replace;
			{
				usize size;
				Alt::Utf::Utf32ToUtf16IgnoreErrors(Alt::MakeArray((u32)replaceUtf32), nullptr, size);

				utf16Replace.Resize(size) iferr_return;
				Alt::Utf::Utf32ToUtf16IgnoreErrors(Alt::MakeArray((u32)replaceUtf32), utf16Replace.Data(), size);
			}

			usize size;
			Alt::Utf::Utf8ToUtf16Replace(INVALID_UTF8, nullptr, size, replaceUtf32);

			Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
			Alt::Utf::Utf8ToUtf16Replace(INVALID_UTF8, utf16.Data(), size, replaceUtf32);

			EXPECT_EQ(utf16[0], (u16)0x1);

			EXPECT_EQ(utf16[1], utf16Replace[0]);
			EXPECT_EQ(utf16[2], utf16Replace[0]);
			EXPECT_EQ(utf16[3], utf16Replace[0]);
			EXPECT_EQ(utf16[4], utf16Replace[0]);

			EXPECT_EQ(utf16[5], utf16Replace[0]);
			EXPECT_EQ(utf16[6], utf16Replace[0]);
			EXPECT_EQ(utf16[7], utf16Replace[0]);
			EXPECT_EQ(utf16[8], replaceUtf32);

			EXPECT_EQ(utf16[21], (u32)0x1);
		}
	}
}

TEST(UTF_TEST_NAME, ConvertUtf8ToUtf16) {

	iferr_scope_on_err {

		FAIL();
	};

	for (Alt::Pair<usize, const UtfText&> val : s_UtfText.GetIterator().Enumerate()) {

		const UtfText& test = val.s();

		{
			usize size;
			Alt::Utf::Utf8ToUtf16IgnoreErrors(test.utf8, nullptr, size);
			EXPECT_EQ(size, test.utf16.GetSize());

			Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
			Alt::Utf::Utf8ToUtf16IgnoreErrors(test.utf8, utf16.Data(), size);
			EXPECT_EQ(size, 0);

			const usize cnt = utf16
				.GetIterator()
				.Zip(test.utf16.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u16, u16>> val_) {

					const Alt::Pair<u16, u16>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			bool isOk = true;

			usize size;
			isOk = Alt::Utf::Utf8ToUtf16(test.utf8, nullptr, size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, test.utf16.GetSize());

			Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
			isOk = Alt::Utf::Utf8ToUtf16(test.utf8, utf16.Data(), size);
			EXPECT_TRUE(isOk);
			EXPECT_EQ(size, 0);

			const usize cnt = utf16
				.GetIterator()
				.Zip(test.utf16.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u16, u16>> val_) {

					const Alt::Pair<u16, u16>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}

		{
			usize size;
			Alt::Utf::Utf8ToUtf16Replace(test.utf8, nullptr, size);
			EXPECT_EQ(size, test.utf16.GetSize());

			Alt::Vector<u16> utf16 = Alt::Vector<u16>::Allocate(size) iferr_return;
			Alt::Utf::Utf8ToUtf16Replace(test.utf8, utf16.Data(), size);
			EXPECT_EQ(size, 0);

			const usize cnt = utf16
				.GetIterator()
				.Zip(test.utf16.GetIterator())
				.Enumerate()
				.Filter([](Alt::Pair<usize, Alt::Pair<u16, u16>> val_) {

					const Alt::Pair<u16, u16>& val = val_.s();
					return val.f() != val.s();
				})
				.Count();

			EXPECT_EQ(cnt, 0)
				<< "i: " << val.f();
		}
	}
}

TEST(UTF_TEST_NAME, DetectEncodingByDom) {

	for (Alt::Pair<usize, const BomTest&> val : s_BytesWithBom.GetIterator().Enumerate()) {

		const BomTest& test = val.s();

		Alt::Utf::DetectEncodingByBomResult result = Alt::Utf::DetectEncodingByBom(test.bytes);
		EXPECT_EQ(result.encoding, test.encoding) << "i: " << val.f();
		EXPECT_EQ(result.bytesConsumedByBom, test.bomSize) << "i: " << val.f();
		EXPECT_EQ(result.isLittleEndian, test.isLittleEndian) << "i: " << val.f();
	}

	{
		const auto texts = Alt::MakeArray<const char*>(
			  "ABC"
		);

		Alt::Slice<const u8> slice{
			reinterpret_cast<const u8*>(texts[0]),
			reinterpret_cast<const u8*>(texts[0] + std::strlen(texts[0])),
		};

		Alt::Utf::DetectEncodingByBomResult result = Alt::Utf::DetectEncodingByBom(slice);
		EXPECT_EQ(result.encoding, Alt::Utf::Encoding::Unknown);
	}
}

TEST(UTF_TEST_NAME, GuessEncoding) {

	for (Alt::Pair<usize, const BomTest&> val : s_BytesWithBom.GetIterator().Enumerate()) {

		const BomTest& test = val.s();

		Alt::Utf::GuessEncodingResult result = Alt::Utf::GuessEncoding(test.bytes);

		if (test.encoding == Alt::Utf::Encoding::Unknown) {

			EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::GuessedEncoding::Utf16))
				<< "i: " << val.f();
		}
		else {

			EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::_UtfDetails::EncodingToGuessedEncoding(test.encoding)))
				<< "i: " << val.f();
		}

		EXPECT_EQ(result.bytesConsumedByBom, test.bomSize) << "i: " << val.f();
		EXPECT_EQ(result.isLittleEndian, test.isLittleEndian) << "i: " << val.f();
	}

	for (Alt::Pair<usize, const UtfTest&> val : s_Utf32Test.GetIterator().Enumerate()) {

		const UtfTest& test = val.s();

		const bool isValid = Alt::Utf::_32::IsValidUtf32(test.utf32);

		{
			Alt::Utf::GuessEncodingResult result = Alt::Utf::GuessEncoding(test.utf8);
			//ALT_PRINTLN("Num of guessed encoding: {}", Alt::Num::Popcount((usize)result.guessedEncoding));

			if (isValid) {

				EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::GuessedEncoding::Utf8))
					<< "i: " << val.f();
			}
			else {

				EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::GuessedEncoding::Unknown))
					<< "i: " << val.f();
			}

			EXPECT_EQ(result.isLittleEndian, Alt::Platform::IsLittleEndian()) << "i: " << val.f();
		}

		{
			const Alt::Slice<const u8> bytes{
				reinterpret_cast<const u8*>(test.utf16.Data()),
				reinterpret_cast<const u8*>(test.utf16.DataEnd()),
			};

			Alt::Utf::GuessEncodingResult result = Alt::Utf::GuessEncoding(bytes);
			//ALT_PRINTLN("Num of guessed encoding: {}", Alt::Num::Popcount((usize)result.guessedEncoding));

			if (isValid) {

				EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::GuessedEncoding::Utf16))
					<< "i: " << val.f();
			}
			else {

				const u16 utf16[2]{
					0 < test.utf16.GetSize() ? test.utf16[0] : (u16)0,
					1 < test.utf16.GetSize() ? test.utf16[1] : (u16)0,
				};

				if (Alt::Utf::_16::IsValidUtf16(utf16)) {

					EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::GuessedEncoding::Utf16))
						<< "i: " << val.f();
				}
				else {

					EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::GuessedEncoding::Unknown))
						<< "i: " << val.f();
				}
			}

			EXPECT_EQ(result.isLittleEndian, Alt::Platform::IsLittleEndian()) << "i: " << val.f();
		}

		{
			const Alt::Slice<const u8> bytes{
				reinterpret_cast<const u8*>(&test.utf32),
				reinterpret_cast<const u8*>(&test.utf32) + sizeof(u32),
			};

			Alt::Utf::GuessEncodingResult result = Alt::Utf::GuessEncoding(bytes);
			//ALT_PRINTLN("Num of guessed encoding: {}", Alt::Num::Popcount((usize)result.guessedEncoding));

			if (isValid) {

				EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::GuessedEncoding::Utf32))
					<< "i: " << val.f();
			}
			else {

				const u16 utf16[2]{
					0 < test.utf16.GetSize() ? test.utf16[0] : (u16)0,
					1 < test.utf16.GetSize() ? test.utf16[1] : (u16)0,
				};

				if (Alt::Utf::_16::IsValidUtf16(utf16)) {

					EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::GuessedEncoding::Utf16))
						<< "i: " << val.f();
				}
				else {

					EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(result.guessedEncoding, Alt::Utf::GuessedEncoding::Unknown))
						<< "i: " << val.f();
				}
			}

			EXPECT_EQ(result.isLittleEndian, Alt::Platform::IsLittleEndian()) << "i: " << val.f();
		}
	}
}
