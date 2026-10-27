#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Utils/Slice.h"
#include "../alt-std/src/Collections/Vector.h"
#include "../alt-std/src/Collections/StaticVector.h"
#include "../alt-std/src/Platform/Platform.h"

#include "../alt-std/src/String/Utf.h"

namespace {

	inline Alt::Optional<chara> Utf8ToUtf32(const char* str) {
		iferr_scope;

		static_assert(sizeof(char) == sizeof(u8), "");

		Alt::Slice<const u8> bytes{
			reinterpret_cast<const u8*>(str),
			std::strlen(str)
		};

		usize size;
		ALT_MAYBE_UNUSED const bool isOk = Alt::Utf::Utf8ToUtf32(bytes, nullptr, size);

		if (size > 1) {
			return Alt::Nullopt;
		}

		u32 utf32;
		Alt::Utf::Utf8ToUtf32(bytes, &utf32, size) iferr_return Alt::Nullopt;

		return static_cast<chara>(utf32);
	}
}

TEST(Slice, Slice) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const auto array = Alt::MakeArray<usize>(1, 1, 1, 1);

		Alt::Slice<const usize> slice = array;
		EXPECT_FALSE(slice.IsEmpty());
		EXPECT_EQ(slice.GetSize(), array.GetSize());

		usize sum = 0;
		for (usize v : slice) {
			sum += v;
		}

		EXPECT_EQ(sum, array.GetSize());
	}

	{
		const Alt::Vector<usize> array = Alt::Vector<usize>::AllocatePush(1, 1, 1, 1 ) iferr_return;

		Alt::Slice<const usize> slice = array;
		EXPECT_FALSE(slice.IsEmpty());
		EXPECT_EQ(slice.GetSize(), array.GetSize());

		usize sum = 0;
		for (usize v : slice) {
			sum += v;
		}

		EXPECT_EQ(sum, array.GetSize());
	}
}

TEST(Slice, Basic) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(10, 20, 30, 40, 50) iferr_return;
	Alt::Slice<usize> slice = vec;

	EXPECT_EQ(slice.GetSize(), 5);
	EXPECT_EQ(slice[0], 10);
	EXPECT_EQ(slice[4], 50);
	EXPECT_EQ(slice.GetSizeBytes(), 5 * sizeof(usize));

	EXPECT_FALSE(slice.IsEmpty());
}

TEST(Slice, EmptySlice) {

	Alt::Slice<usize> slice{};

	EXPECT_TRUE(slice.IsEmpty());
	EXPECT_EQ(slice.GetSize(), 0);

	usize sum = 0;
	for (usize v : slice) {
		sum += v;
	}

	EXPECT_EQ(sum, 0);
}

TEST(Slice, ConstCorrectness) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(1, 2, 3) iferr_return;
	Alt::Slice<const usize> slice = vec;

	EXPECT_EQ(slice[0], 1);

	//slice[0] = 10; // compilation error

	auto it = slice.begin();
	EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(*it)>>);
}

TEST(Slice, STDIterator) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(0, 1, 2, 3, 4, 5, 6, 7, 8, 9) iferr_return;
	Alt::Slice<usize> slice{ vec };

	auto it = slice.begin();

	EXPECT_EQ(*it, 0);
	EXPECT_EQ(*(++it), 1);
	EXPECT_EQ(*(it + 3), 4);
	EXPECT_EQ(*(it - 1), 0);

	EXPECT_TRUE(it < slice.end());
	EXPECT_TRUE(it != slice.end());

	auto dist = std::distance(slice.begin(), slice.end());
	EXPECT_EQ(dist, 10);
}

TEST(Slice, Iterator) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::Vector<usize> vec = Alt::Vector<usize>::AllocatePush(1, 2, 3) iferr_return;
	Alt::Slice<usize> slice{ vec };

	Alt::SliceIterator<usize> iterator = slice.GetIterator();

	usize* val0 = iterator.Next() iferr_return;
	EXPECT_EQ(*val0, 1);

	EXPECT_EQ(iterator.GetSize(), 2);
	EXPECT_EQ(slice.GetSize(), 3);

	ALT_MAYBE_UNUSED auto _0 = iterator.Next() iferr_return;
	ALT_MAYBE_UNUSED auto _1 = iterator.Next() iferr_return;

	EXPECT_TRUE(iterator.IsEmpty());

	EXPECT_EQ(slice.GetSize(), 3);
}

TEST(Slice, SubSlice) {

	{
		Alt::Array<usize, 5> data = Alt::MakeArray<usize>(1, 2, 3, 4, 5);
		Alt::Slice<usize> slice{ data };

		Alt::Slice<usize> emptySlice0 = slice.SubSlice(2, 0);
		EXPECT_TRUE(emptySlice0.IsEmpty());
		EXPECT_EQ(emptySlice0.GetSize(), 0);

		Alt::Slice<usize> emptySlice1 = slice.SubSlice(data.GetSize());
		EXPECT_EQ(emptySlice1.GetSize(), 0);
		EXPECT_EQ(emptySlice1.GetSize(), 0);

		Alt::Slice<usize> remainSlice0 = slice.SubSlice(3); 
		EXPECT_EQ(remainSlice0.GetSize(), 2);
		EXPECT_EQ(remainSlice0[0], 4);
		EXPECT_EQ(remainSlice0[1], 5);

		Alt::Slice<usize> remainSlice1 = slice.SubSlice(1, 3).SubSlice(1, 1);
		EXPECT_EQ(remainSlice1.GetSize(), 1);
		EXPECT_EQ(remainSlice1[0], 3);
	}

	{
		constexpr usize SIZE = 26;
		constexpr usize COLUMNS = 20;

		Alt::Array<char, SIZE> abc;

		{
			char ch = 'A';
			abc
				.GetIterator()
				.ForEach([&ch](char& x) {
					x = ch;
					ch += 1;
				});
		}

		const auto expect = Alt::MakeArray<Alt::Slice<const char>>(
			"ABCDEFGHIJKLMNOPQRST",
			"BCDEFGHIJKLMNOPQRSTU",
			"CDEFGHIJKLMNOPQRSTUV",
			"DEFGHIJKLMNOPQRSTUVW",
			"EFGHIJKLMNOPQRSTUVWX",
			"FGHIJKLMNOPQRSTUVWXY",
			"GHIJKLMNOPQRSTUVWXYZ"
		);

		for (usize offset = 0; offset < expect.GetSize(); ++offset) {

			const Alt::Slice<const char> str_{ abc };
			const Alt::Slice<const char> str = str_.SubSlice(offset, COLUMNS);

			EXPECT_EQ(str, expect[offset]);
			EXPECT_EQ(str_.SubSlice(offset).GetSize(), SIZE - offset);
		}
	}
}

TEST(Slice, Trim) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const Alt::Slice<const char> str = "          "_s;

		const Alt::Slice<const char> str0 = str.TrimLeft();
		EXPECT_TRUE(str0.IsEmpty());

		const Alt::Slice<const char> str1 = str.TrimRight();
		EXPECT_TRUE(str1.IsEmpty());

		const Alt::Slice<const char> str2 = str.Trim();
		EXPECT_TRUE(str2.IsEmpty());
	}

	{
		const Alt::Slice<const char> str = "   Строка і String       "_s;

		const Alt::Slice<const char> str0 = str.TrimLeft();
		EXPECT_EQ(str0, "Строка і String       ");

		const Alt::Slice<const char> str1 = str.TrimRight();
		EXPECT_EQ(str1, "   Строка і String");

		const Alt::Slice<const char> str2 = str.Trim();
		EXPECT_EQ(str2, "Строка і String");
	}

	{
		const Alt::Slice<const char> str = "Строка і String       "_s;

		const Alt::Slice<const char> str0 = str.TrimLeft();
		EXPECT_EQ(str0, "Строка і String       ");

		const Alt::Slice<const char> str1 = str.TrimRight();
		EXPECT_EQ(str1, "Строка і String");

		const Alt::Slice<const char> str2 = str.Trim();
		EXPECT_EQ(str2, "Строка і String");
	}

	{
		const Alt::Slice<const char> str = "   Строка і String"_s;

		const Alt::Slice<const char> str0 = str.TrimLeft();
		EXPECT_EQ(str0, "Строка і String");

		const Alt::Slice<const char> str1 = str.TrimRight();
		EXPECT_EQ(str1, "   Строка і String");

		const Alt::Slice<const char> str2 = str.Trim();
		EXPECT_EQ(str2, "Строка і String");
	}

	{
		const Alt::Slice<const char> str = "a  "_s;

		const Alt::Slice<const char> str0 = str.TrimLeft();
		EXPECT_EQ(str0, "a  ");

		const Alt::Slice<const char> str1 = str.TrimRight();
		EXPECT_EQ(str1, "a");

		const Alt::Slice<const char> str2 = str.Trim();
		EXPECT_EQ(str2, "a");
	}

	{
		Alt::Slice<const char> str;

		str = str.TrimLeft();
		EXPECT_TRUE(str.IsEmpty());

		str = str.TrimRight();
		EXPECT_TRUE(str.IsEmpty());

		str = str.Trim();
		EXPECT_TRUE(str.IsEmpty());
	}
}

TEST(Slice, IteratorUTF8) {

	{
		Alt::Slice<const char> string = "Строка і StringСтрока і StringСтрока і String"_s;
		Alt::SliceIteratorUTF8 iterator = string.GetIteratorUTF();

		Alt::Optional<Alt::Optional<chara>> result = iterator.Find([](const Alt::Optional<chara> x) {

			if (!x) return false;
			return *x == Utf8ToUtf32("і").GetValueOr(0);
		});
		EXPECT_TRUE(result);
		EXPECT_TRUE(*result);
		EXPECT_EQ(**result, Utf8ToUtf32("і").GetValueOr(0));

		usize advanced = iterator.Advance(6);
		EXPECT_EQ(advanced, 6);

		result = iterator.Next();
		EXPECT_TRUE(result);
		EXPECT_TRUE(*result);
		EXPECT_EQ(**result, U'g');

		result = iterator.NextBack();
		EXPECT_TRUE(result);
		EXPECT_TRUE(*result);
		EXPECT_EQ(**result, U'g');

		const usize restSize = iterator.CloneIter().Count();

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, restSize);

		result = iterator.NextBack();
		EXPECT_FALSE(result);

		advanced = iterator.Advance(restSize + 1);
		EXPECT_EQ(advanced, 0);

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, 0);
	}

	{
		Alt::Slice<const char> string = "\x41\xC2\xA2\xE2\x82\xAC\xF0\x90\x8D\x88"_s;
		ASSERT_TRUE(string.IsPopulated());

		Alt::SliceIteratorUTF8 iterator = string.GetIteratorUTF();
		EXPECT_EQ(**iterator.Next(), U'A');
		EXPECT_EQ(**iterator.Next(), U'\u00A2');
		EXPECT_EQ(**iterator.Next(), U'\u20AC');
		EXPECT_EQ(**iterator.Next(), U'\U00010348');
		EXPECT_EQ(iterator.Next(), Alt::Nullopt);
	}
}

TEST(Slice, IteratorUTF16) {

	{
		Alt::Slice<const char16_t> string = u"Строка і StringСтрока і StringСтрока і String"_s;
		Alt::SliceIteratorUTF16 iterator = string.GetIteratorUTF();

		Alt::Optional<Alt::Optional<chara>> result = iterator.Find([](const Alt::Optional<chara> x) {

			if (!x) return false;
			return *x == Utf8ToUtf32("і").GetValueOr(0);
		});
		EXPECT_TRUE(result);
		EXPECT_TRUE(*result);
		EXPECT_EQ(**result, Utf8ToUtf32("і").GetValueOr(0));

		usize advanced = iterator.Advance(6);
		EXPECT_EQ(advanced, 6);

		result = iterator.Next();
		EXPECT_TRUE(result);
		EXPECT_TRUE(*result);
		EXPECT_EQ(**result, U'g');

		result = iterator.NextBack();
		EXPECT_TRUE(result);
		EXPECT_TRUE(*result);
		EXPECT_EQ(**result, U'g');

		const usize restSize = iterator.CloneIter().Count();

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, restSize);

		result = iterator.NextBack();
		EXPECT_FALSE(result);

		advanced = iterator.Advance(restSize + 1);
		EXPECT_EQ(advanced, 0);

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, 0);
	}

	{
		Alt::Slice<const char16_t> string = u"A¢€𐍈"_s;
		ASSERT_TRUE(string.IsPopulated());

		Alt::SliceIteratorUTF16 iterator = string.GetIteratorUTF();
		EXPECT_EQ(**iterator.Next(), U'A');
		EXPECT_EQ(**iterator.Next(), U'¢');
		EXPECT_EQ(**iterator.Next(), U'€');
		EXPECT_EQ(**iterator.Next(), U'𐍈');
		EXPECT_EQ(iterator.Next(), Alt::Nullopt);
	}
}

TEST(Slice, IteratorUTF32) {

	{
		Alt::Slice<const chara> string = U"Строка і StringСтрока і StringСтрока і String"_s;
		Alt::SliceIteratorUTF32 iterator = string.GetIteratorUTF();

		Alt::Optional<Alt::Optional<chara>> result = iterator.Find([](const Alt::Optional<chara> x) {

			if (!x) return false;
			return *x == Utf8ToUtf32("і").GetValueOr(0);
		});
		EXPECT_TRUE(result);
		EXPECT_TRUE(*result);
		EXPECT_EQ(**result, Utf8ToUtf32("і").GetValueOr(0));

		usize advanced = iterator.Advance(6);
		EXPECT_EQ(advanced, 6);

		result = iterator.Next();
		EXPECT_TRUE(result);
		EXPECT_TRUE(*result);
		EXPECT_EQ(**result, U'g');

		result = iterator.NextBack();
		EXPECT_TRUE(result);
		EXPECT_TRUE(*result);
		EXPECT_EQ(**result, U'g');

		const usize restSize = iterator.CloneIter().Count();

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, restSize);

		result = iterator.NextBack();
		EXPECT_FALSE(result);

		advanced = iterator.Advance(restSize + 1);
		EXPECT_EQ(advanced, 0);

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, 0);
	}

	{
		Alt::Slice<const chara> string = U"A¢€𐍈"_s;
		ASSERT_TRUE(string.IsPopulated());

		Alt::SliceIteratorUTF32 iterator = string.GetIteratorUTF();
		EXPECT_EQ(**iterator.Next(), U'A');
		EXPECT_EQ(**iterator.Next(), U'¢');
		EXPECT_EQ(**iterator.Next(), U'€');
		EXPECT_EQ(**iterator.Next(), U'𐍈');
		EXPECT_EQ(iterator.Next(), Alt::Nullopt);
	}
}

TEST(Slice, IteratorUTF8Trusted) {

	{
		Alt::Slice<const char> string = "Строка і StringСтрока і StringСтрока і String"_s;
		Alt::SliceIteratorUTF8Trusted iterator = string.GetIteratorUTFTrusted();

		Alt::Optional<chara> result = iterator.Find([](const chara x) {
			return x == Utf8ToUtf32("і").GetValueOr(0);
		});
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, Utf8ToUtf32("і").GetValueOr(0));

		usize advanced = iterator.Advance(6);
		EXPECT_EQ(advanced, 6);

		result = iterator.Next();
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, U'g');

		result = iterator.NextBack();
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, U'g');

		const usize restSize = iterator.CloneIter().Count();

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, restSize);

		result = iterator.NextBack();
		EXPECT_FALSE(result);

		advanced = iterator.Advance(restSize + 1);
		EXPECT_EQ(advanced, 0);

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, 0);
	}

	{
		Alt::Slice<const char> string = "\x41\xC2\xA2\xE2\x82\xAC\xF0\x90\x8D\x88"_s;
		ASSERT_TRUE(string.IsPopulated());

		Alt::SliceIteratorUTF8Trusted iterator = string.GetIteratorUTFTrusted();
		EXPECT_EQ(*iterator.Next(), U'A');
		EXPECT_EQ(*iterator.Next(), U'\u00A2');
		EXPECT_EQ(*iterator.Next(), U'\u20AC');
		EXPECT_EQ(*iterator.Next(), U'\U00010348');
		EXPECT_EQ(iterator.Next(), Alt::Nullopt);
	}
}

TEST(Slice, IteratorUTF16Trusted) {

	{
		Alt::Slice<const char16_t> string = u"Строка і StringСтрока і StringСтрока і String"_s;
		Alt::SliceIteratorUTF16Trusted iterator = string.GetIteratorUTFTrusted();

		Alt::Optional<chara> result = iterator.Find([](const chara x) {
			return x == Utf8ToUtf32("і").GetValueOr(0);
		});
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, Utf8ToUtf32("і").GetValueOr(0));

		usize advanced = iterator.Advance(6);
		EXPECT_EQ(advanced, 6);

		result = iterator.Next();
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, U'g');

		result = iterator.NextBack();
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, U'g');

		const usize restSize = iterator.CloneIter().Count();

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, restSize);

		result = iterator.NextBack();
		EXPECT_FALSE(result);

		advanced = iterator.Advance(restSize + 1);
		EXPECT_EQ(advanced, 0);

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, 0);
	}

	{
		Alt::Slice<const char16_t> string = u"A¢€𐍈"_s;
		ASSERT_TRUE(string.IsPopulated());

		Alt::SliceIteratorUTF16Trusted iterator = string.GetIteratorUTFTrusted();
		EXPECT_EQ(*iterator.Next(), U'A');
		EXPECT_EQ(*iterator.Next(), U'¢');
		EXPECT_EQ(*iterator.Next(), U'€');
		EXPECT_EQ(*iterator.Next(), U'𐍈');
		EXPECT_EQ(iterator.Next(), Alt::Nullopt);
	}
}

TEST(Slice, IteratorUTF32Trusted) {

	{
		Alt::Slice<const chara> string = U"Строка і StringСтрока і StringСтрока і String"_s;
		Alt::SliceIteratorUTF32Trusted iterator = string.GetIteratorUTFTrusted();

		Alt::Optional<chara> result = iterator.Find([](const chara x) {
			return x == Utf8ToUtf32("і").GetValueOr(0);
		});
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, Utf8ToUtf32("і").GetValueOr(0));

		usize advanced = iterator.Advance(6);
		EXPECT_EQ(advanced, 6);

		result = iterator.Next();
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, U'g');

		result = iterator.NextBack();
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, U'g');

		const usize restSize = iterator.CloneIter().Count();

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, restSize);

		result = iterator.NextBack();
		EXPECT_FALSE(result);

		advanced = iterator.Advance(restSize + 1);
		EXPECT_EQ(advanced, 0);

		advanced = iterator.AdvanceBack(restSize + 1);
		EXPECT_EQ(advanced, 0);
	}

	{
		Alt::Slice<const chara> string = U"A¢€𐍈"_s;
		ASSERT_TRUE(string.IsPopulated());

		Alt::SliceIteratorUTF32Trusted iterator = string.GetIteratorUTFTrusted();
		EXPECT_EQ(*iterator.Next(), U'A');
		EXPECT_EQ(*iterator.Next(), U'¢');
		EXPECT_EQ(*iterator.Next(), U'€');
		EXPECT_EQ(*iterator.Next(), U'𐍈');
		EXPECT_EQ(iterator.Next(), Alt::Nullopt);
	}
}
