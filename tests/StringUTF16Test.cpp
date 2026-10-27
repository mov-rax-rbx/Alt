#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/String/StringUTF16.h"
#include "../alt-std/src/Random/Xoshiro256.h"

namespace {

	struct CustomStruct {
		f64 val = 0.0;
	};
}

namespace Alt {

	template <>
	class Format<CustomStruct> {
	public:

		ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(FormatEstimator& estimator, const Slice<const char> parameters, const CustomStruct& val) noexcept {
			iferr_scope;

			if (parameters.IsPopulated()) {
				return Nullopt;
			}

			EstimateExactSizeResult result{};
			result.size = 14;

			const EstimateExactSizeResult estimated = Alt::Format<f64>::EstimateExactSize(estimator, ""_s, val.val) iferr_return Nullopt;

			result.size += estimated.size;
			result.size += 2;

			return result;
		}

		template <typename FormatWriterT>
		ALT_FORCE_INLINE static bool WriteAppend(FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const CustomStruct& val) noexcept {
			iferr_scope;

			if (parameters.IsPopulated() && parameters != "just_some_parameter_example") {
				return false;
			}

			writer.Append("CustomStruct{ ") iferr_return false;
			Alt::Format<f64>::WriteAppend(writer, ""_s, val.val) iferr_return false;
			writer.Append(" }") iferr_return false;

			return true;
		}
	};

} // namespace Alt

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

	inline Alt::Optional<Alt::Vector<Alt::Slice<const char16_t>>> SplitByWordStable(Alt::Slice<const char16_t> str, const Alt::Slice<const char16_t> del) {
		iferr_scope;

		Alt::Vector<Alt::Slice<const char16_t>> result;

		if (str.IsEmpty()) {
			return result;
		}

		while (Alt::Optional<usize> pos = str.FindFirst(del)) {

			const Alt::Slice<const char16_t> split = str.SubSlice(0, *pos);
			str = str.SubSlice(*pos + del.GetSize());

			result.Push(split) iferr_return Alt::Nullopt;
		}

		if (str.IsPopulated()) {
			result.Push(str) iferr_return Alt::Nullopt;
		}

		return result;
	}
}

TEST(StringUTF16, TypeTraits) {

	bool v = true;

	v = Alt::TypeTraits::is_clonable_v<Alt::StringUTF16<>>;
	EXPECT_TRUE(v);

	v = Alt::TypeTraits::is_trivially_reallocatable_v<Alt::StringUTF16<>>;
	EXPECT_TRUE(v);
}

TEST(StringUTF16, Construct) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("String") iferr_return;
		EXPECT_EQ(string, L"String");
		EXPECT_EQ(std::wcscmp((wchar_t*)string.CStr(), L"String"), 0);
	}

	{
		Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate(L"String") iferr_return;
		EXPECT_EQ(string, L"String");
		EXPECT_EQ(std::wcscmp((wchar_t*)string.CStr(), L"String"), 0);
	}

	{
		Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate(L"String") iferr_return;
		EXPECT_EQ(string, L"String");
		EXPECT_EQ(std::wcscmp((wchar_t*)string.CStr(), L"String"), 0);

		string.Clear();
		EXPECT_TRUE(string.IsEmpty());
		EXPECT_EQ(string.GetSizeU16(), 0);

		string.Clear();
		string.Clear();
		string.Clear();
		EXPECT_TRUE(string.IsEmpty());
		EXPECT_EQ(string.GetSizeU16(), 0);
	}
}

TEST(StringUTF16, PushPop) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StringUTF16<> string;
	string.Push(
		'a',
		Utf8ToUtf32("П").GetValueOr(0),
		Utf8ToUtf32("р").GetValueOr(0),
		Utf8ToUtf32("и").GetValueOr(0),
		Utf8ToUtf32("в").GetValueOr(0),
		Utf8ToUtf32("е").GetValueOr(0),
		Utf8ToUtf32("т").GetValueOr(0)
	) iferr_return;
	EXPECT_EQ(string, L"aПривет");

	chara v;

	v = string.Pop();
	EXPECT_EQ(v, Utf8ToUtf32("т").GetValueOr(0));

	v = string.Pop();
	EXPECT_EQ(v, Utf8ToUtf32("е").GetValueOr(0));

	v = string.Pop();
	EXPECT_EQ(v, Utf8ToUtf32("в").GetValueOr(0));

	v = string.Pop();
	EXPECT_EQ(v, Utf8ToUtf32("и").GetValueOr(0));

	v = string.Pop();
	EXPECT_EQ(v, Utf8ToUtf32("р").GetValueOr(0));

	v = string.Pop();
	EXPECT_EQ(v, Utf8ToUtf32("П").GetValueOr(0));

	EXPECT_TRUE(string.IsPopulated());
	EXPECT_EQ(string.GetSizeU16(), 1);

	string.Append("AAAA") iferr_return;
	string.Pop();
	string.Pop();
	string.Pop();
	string.Pop();

	v = string.Pop();
	EXPECT_EQ(v, U'a');

	EXPECT_TRUE(string.IsEmpty());
	EXPECT_EQ(string.GetSizeU16(), 0);

	string.Push('b') iferr_return;

	v = string.Pop();
	EXPECT_EQ(v, U'b');
	EXPECT_TRUE(string.IsEmpty());

	string.Append("c"_salloc) iferr_return;
	EXPECT_EQ(string.Pop(), U'c');
	EXPECT_TRUE(string.IsEmpty());
}

TEST(StringUTF16, Compare) {
	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("ng") iferr_return;
		EXPECT_EQ(string.Compare(L"ng"), 0);

		EXPECT_EQ(string, L"ng");

		EXPECT_EQ(string.Compare(L"nga"), -1);

		EXPECT_EQ(string.Compare(L"n"), 1);
	}

	{
		Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("String") iferr_return;
		EXPECT_EQ(string.Compare(L"String"), 0);

		EXPECT_EQ(string, L"String");

		EXPECT_EQ(string.Compare(L"Stringa"), -1);

		EXPECT_EQ(string.Compare(L"Strin"), 1);
	}

	{
		Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("Привет") iferr_return;
		EXPECT_EQ(string.Compare(L"Привет"), 0);

		EXPECT_EQ(string, L"Привет");

		EXPECT_EQ(string.Compare(L"Приветa"), -1);

		EXPECT_EQ(string.Compare(L"Приве"), 1);

		auto arr = Alt::MakeArray(L'S', L't');
		EXPECT_EQ(string.Compare(arr), 1);
	}
}

TEST(StringUTF16, IndexingAsBytes) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StringUTF16<> string = Alt::StringUTF16<>::AllocatePush('a', 'b', 'c') iferr_return;
	EXPECT_EQ(string[0], 'a');
	EXPECT_EQ(string[2], 'c');
}

TEST(StringUTF16, CloneCloneFrom) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("String") iferr_return;

	Alt::StringUTF16<> string1 = string.Clone() iferr_return;
	EXPECT_EQ(string1, string);

	Alt::StringUTF16<> string2;
	string2.CloneFrom(string) iferr_return;
	EXPECT_EQ(string2, string);

	string2.CloneFrom(Alt::StringUTF16<>{}) iferr_return;
	EXPECT_TRUE(string2.IsEmpty());
}

TEST(StringUTF16, Append) {

	iferr_scope_on_err {

		FAIL();
	};

	const char* str = "Строка і String";

	Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate(str) iferr_return;
	string.Append(str) iferr_return;
	string.Append(L"Строка і String") iferr_return;

	EXPECT_EQ(string, L"Строка і StringСтрока і StringСтрока і String");

	string = Alt::StringUTF16<>::Allocate(Alt::MakeArray<u8>(0xEF, 0xBB, 0xBF, 'H', 'i')) iferr_return;

	EXPECT_EQ(string.GetSizeU16(), 2);
	EXPECT_EQ(string[0], 'H');
	EXPECT_EQ(string[1], 'i');
	EXPECT_EQ(Alt::StringUTF16<>::CalculateCStrLen(string.CStr()), 2);
}

TEST(StringUTF16, AppendStressTest) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate() iferr_return;

	for (usize i = 0; i < 100; ++i) {

		string.Append("abc") iferr_return;
	}
	EXPECT_EQ(string.GetSizeU16(), 300);

	for (usize i = 0; i < 300; ++i) {

		if (i % 3 == 0) EXPECT_EQ(string[i], 'a');
		if (i % 3 == 1) EXPECT_EQ(string[i], 'b');
		if (i % 3 == 2) EXPECT_EQ(string[i], 'c');
	}

	string.Clear();
	EXPECT_TRUE(string.IsEmpty());
}

TEST(StringUTF16, Iterator) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const char* str = "Строка і String";

		Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate(str) iferr_return;
		string.Append(str) iferr_return;
		string.Append(L"Строка і String") iferr_return;

		Alt::StringUTF16Iterator iterator = string.GetIterator();

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
		const char* str = "\x41\xC2\xA2\xE2\x82\xAC\xF0\x90\x8D\x88"; 

		Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate(str) iferr_return;
		ASSERT_TRUE(string.IsPopulated());

		Alt::StringUTF16Iterator iterator = string.GetIterator();
		EXPECT_EQ(*iterator.Next(), U'A');
		EXPECT_EQ(*iterator.Next(), U'\u00A2');
		EXPECT_EQ(*iterator.Next(), U'\u20AC');
		EXPECT_EQ(*iterator.Next(), U'\U00010348');
		EXPECT_EQ(iterator.Next(), Alt::Nullopt);
	}
}

TEST(StringUTF16, FindFirst) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("Строка і String Строка і String") iferr_return;

	Alt::Optional<usize> res = str.FindFirst((chara)'t');
	ASSERT_TRUE(res);
	EXPECT_EQ(str[*res], 't');

	res = str.FindFirst((chara)'g');
	ASSERT_TRUE(res);
	EXPECT_EQ(str[*res], 'g');

	res = str.FindFirst(U'к');
	ASSERT_TRUE(res);
	EXPECT_EQ(str[*res], u"к"[0]);

	res = str.FindFirst((chara)'A');
	EXPECT_FALSE(res);

	res = str.FindFirst('g');
	ASSERT_TRUE(res);
	EXPECT_EQ(str[*res], 'g');

	res = str.FindFirst('n');
	ASSERT_TRUE(res);
	EXPECT_EQ(str[*res], 'n');

	res = str.FindFirst(L'к');
	ASSERT_TRUE(res);
	EXPECT_EQ(str[*res], u"к"[0]);
}

TEST(StringUTF16, FindLast) {

	iferr_scope_on_err {

		FAIL();
	};

	const usize sizeOfFirstHalf = L"Строка і String"_s.GetSize();
	const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("Строка і String Строка і String") iferr_return;

	Alt::Optional<usize> res = str.FindLast((chara)'t');
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], 't');

	res = str.FindLast((chara)'g');
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], 'g');

	res = str.FindLast(U'к');
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], u"к"[0]);

	res = str.FindLast((chara)'A');
	EXPECT_FALSE(res);

	res = str.FindLast('g');
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], 'g');

	res = str.FindLast('n');
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], 'n');

	res = str.FindLast(L'к');
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], u"к"[0]);
}

TEST(StringUTF16, FindFirstSubStr) {

	iferr_scope_on_err {

		FAIL();
	};

	const usize sizeOfFirstHalf = L"Строка і String"_s.GetSize();
	const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("Строка і String Строка і String") iferr_return;

	Alt::Optional<usize> res = str.FindFirst(L"tring");
	ASSERT_TRUE(res);
	EXPECT_LT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], 't');

	res = str.FindFirst(L"ng");
	ASSERT_TRUE(res);
	EXPECT_LT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], 'n');
	
	res = str.FindFirst(L"ка і ");
	ASSERT_TRUE(res);
	EXPECT_LT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], u"к"[0]);

	res = str.FindFirst(L"AAAAA");
	EXPECT_FALSE(res);

	res = str.FindFirst(L" String");
	ASSERT_TRUE(res);
	EXPECT_LT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], ' ');

	res = str.FindFirst(L"");
	ASSERT_TRUE(res);

	res = str.FindFirst(L"Строка і String Строка і String Строка і String Строка і String");
	ASSERT_FALSE(res);

	res = str.FindFirst(L"Строка і String");
	ASSERT_TRUE(res);
	EXPECT_LT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], u"С"[0]);

	EXPECT_TRUE(str.FindFirst(L""));

	Alt::StringUTF16<> strEmpty;
	EXPECT_TRUE(strEmpty.FindFirst(L""));
}

TEST(StringUTF16, FindLastSubStr) {

	iferr_scope_on_err {

		FAIL();
	};

	const usize sizeOfFirstHalf = L"Строка і String"_s.GetSize();
	const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("Строка і String Строка і String") iferr_return;

	Alt::Optional<usize> res = str.FindLast(L"tring");
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], 't');

	res = str.FindLast(L"ng");
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], 'n');

	res = str.FindLast(L"ка і ");
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], u"к"[0]);

	res = str.FindLast(L"AAAAA");
	EXPECT_FALSE(res);

	res = str.FindLast(L" String");
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], ' ');

	res = str.FindLast(L"");
	ASSERT_TRUE(res);

	res = str.FindLast(L"Строка і String Строка і String Строка і String Строка і String");
	ASSERT_FALSE(res);

	res = str.FindLast(L"Строка і String");
	ASSERT_TRUE(res);
	EXPECT_GT(*res, sizeOfFirstHalf);
	EXPECT_EQ(str[*res], u"С"[0]);

	EXPECT_TRUE(str.FindLast(L""));

	Alt::StringUTF16<> strEmpty;
	EXPECT_TRUE(strEmpty.FindLast(L""));
}

TEST(StringUTF16, StartWith) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("Строка і String") iferr_return;

	EXPECT_TRUE(str.StartWith(str));
	EXPECT_TRUE(str.StartWith(L"Строка і String"));
	EXPECT_FALSE(str.StartWith(L"і String"));

	EXPECT_TRUE(str.StartWith(L"С"));
	EXPECT_TRUE(str.StartWith(L"Строка "));
	EXPECT_FALSE(str.StartWith(L" "));

	EXPECT_FALSE(str.StartWith(L"Строка і String Строка і String"));
	EXPECT_TRUE(str.StartWith(L""));
}

TEST(StringUTF16, EndWith) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("Строка і String") iferr_return;

	EXPECT_TRUE(str.EndWith(str));
	EXPECT_TRUE(str.EndWith(L"Строка і String"));
	EXPECT_TRUE(str.EndWith(L"і String"));
	EXPECT_FALSE(str.EndWith(L"Строка "));

	EXPECT_TRUE(str.EndWith(L"g"));
	EXPECT_FALSE(str.EndWith(L"Строка "));
	EXPECT_TRUE(str.EndWith(L"і String"));

	EXPECT_FALSE(str.EndWith(L"Строка і String Строка і String"));
	EXPECT_TRUE(str.EndWith(L""));
}

TEST(StringUTF16, Trim) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate(L"          ") iferr_return;

		Alt::StringUTF16<> str0 = str.Clone() iferr_return;
		str0.TrimLeft();

		EXPECT_TRUE(str0.IsEmpty());

		Alt::StringUTF16<> str1 = str.Clone() iferr_return;
		str1.TrimRight();

		EXPECT_TRUE(str1.IsEmpty());

		Alt::StringUTF16<> str2 = str.Clone() iferr_return;
		str2.Trim();

		EXPECT_TRUE(str2.IsEmpty());
	}

	{
		const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("   Строка і String       ") iferr_return;

		Alt::StringUTF16<> str0 = str.Clone() iferr_return;
		str0.TrimLeft();

		EXPECT_EQ(str0, L"Строка і String       ");

		Alt::StringUTF16<> str1 = str.Clone() iferr_return;
		str1.TrimRight();

		EXPECT_EQ(str1, L"   Строка і String");

		Alt::StringUTF16<> str2 = str.Clone() iferr_return;
		str2.Trim();

		EXPECT_EQ(str2, L"Строка і String");
	}

	{
		const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("Строка і String       ") iferr_return;

		Alt::StringUTF16<> str0 = str.Clone() iferr_return;
		str0.TrimLeft();

		EXPECT_EQ(str0, L"Строка і String       ");

		Alt::StringUTF16<> str1 = str.Clone() iferr_return;
		str1.TrimRight();

		EXPECT_EQ(str1, L"Строка і String");

		Alt::StringUTF16<> str2 = str.Clone() iferr_return;
		str2.Trim();

		EXPECT_EQ(str2, L"Строка і String");
	}

	{
		const Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("   Строка і String") iferr_return;

		Alt::StringUTF16<> str0 = str.Clone() iferr_return;
		str0.TrimLeft();

		EXPECT_EQ(str0, L"Строка і String");

		Alt::StringUTF16<> str1 = str.Clone() iferr_return;
		str1.TrimRight();

		EXPECT_EQ(str1, L"   Строка і String");

		Alt::StringUTF16<> str2 = str.Clone() iferr_return;
		str2.Trim();

		EXPECT_EQ(str2, L"Строка і String");
	}

	{
		Alt::StringUTF16<> str;

		str.TrimLeft();
		EXPECT_TRUE(str.IsEmpty());

		str.TrimRight();
		EXPECT_TRUE(str.IsEmpty());

		str.Trim();
		EXPECT_TRUE(str.IsEmpty());
	}
}

TEST(StringUTF16, ToString) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<u8>(4) iferr_return;
		EXPECT_EQ(str, L"4");
	}

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<u16>(4) iferr_return;
		EXPECT_EQ(str, L"4");
	}

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<u32>(4) iferr_return;
		EXPECT_EQ(str, L"4");
	}

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<u64>(4) iferr_return;
		EXPECT_EQ(str, L"4");
	}

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<s8>(-4) iferr_return;
		EXPECT_EQ(str, L"-4");
	}

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<s16>(-4) iferr_return;
		EXPECT_EQ(str, L"-4");
	}

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<s32>(-4) iferr_return;
		EXPECT_EQ(str, L"-4");
	}

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<s64>(-4) iferr_return;
		EXPECT_EQ(str, L"-4");
	}

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<f32>(4.8f) iferr_return;
		ASSERT_GE(str.GetSizeU16(), 3);
		EXPECT_EQ(Alt::Slice<const char16_t>{ str }.SubSlice(0, 3), L"4.8") << str.CStr();
	}

	{
		Alt::StringUTF16<> str;
		str.ToStringAppend<f64>(4.8) iferr_return;
		ASSERT_GE(str.GetSizeU16(), 3);
		EXPECT_EQ(Alt::Slice<const char16_t>{ str }.SubSlice(0, 3), L"4.8") << str.CStr();
	}
}

TEST(StringUTF16, FormatAppendBracketEscape) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StringUTF16<> str;
		str.FormatAppend("{{{}}", false) iferr_return;
		EXPECT_EQ(str, L"{false}") << str.CStr();

		str.Clear();
		str.FormatAppend("{{{}}", true) iferr_return;
		EXPECT_EQ(str, L"{true}") << str.CStr();

		str.Clear();
		str.FormatAppend("{{{digit}}", true) iferr_return;
		EXPECT_EQ(str, L"{1}") << str.CStr();

		str.Clear();
		str.FormatAppend("{{{digit}}", false) iferr_return;
		EXPECT_EQ(str, L"{0}") << str.CStr();
	}

	{
		Alt::StringUTF16<> str;
		str.FormatAppend("{{{}}", 4.8f) iferr_return;
		EXPECT_EQ(str, L"{4.8}") << str.CStr();
	}

	{
		Alt::StringUTF16<> str;
		str.FormatAppend("{{{{}} {}", 4.8f) iferr_return;
		EXPECT_EQ(str, L"{{}} 4.8") << str.CStr();
	}

	{
		Alt::StringUTF16<> str;
		str.FormatAppend("{{{{{{{{}}}}} {}", 4.8f) iferr_return;
		EXPECT_EQ(str, L"{{{{}}}}} 4.8") << str.CStr();
	}

	{
		Alt::StringUTF16<> str;
		str.FormatAppend("{{{{{{{{{{}}}} {}", 4.8f) iferr_return;
		EXPECT_EQ(str, L"{{{{{}}}} 4.8") << str.CStr();
	}

	{
		Alt::StringUTF16<> str;
		str.FormatAppend("{} {{{{}}} {}", 4.8f, 2.4f) iferr_return;
		EXPECT_EQ(str, L"4.8 {{}}} 2.4") << str.CStr();
	}

	{
		Alt::StringUTF16<> str;
		Alt::FormatResult result = str.FormatAppend("{{{{{{}}}", 4.8f);
		ASSERT_FALSE(result);
		EXPECT_EQ(result.GetErr(), Alt::FormatError::ManyArguments);
	}

	{
		Alt::StringUTF16<> str;
		Alt::FormatResult result = str.FormatAppend("{} {{}", 4.8f, 2.4f);
		ASSERT_FALSE(result);
		EXPECT_EQ(result.GetErr(), Alt::FormatError::ManyArguments);
	}
}

TEST(StringUTF16, FormatAppend) {

	iferr_scope_on_err {

		FAIL();
	};

	{
		Alt::StaticVector<usize, 4> vec{ 0, 1, 2, 3, };

		Alt::StringUTF16<> str;
		str.FormatAppend("{{{{}} {{} float: {{{}}, {{} {}, {}", 4.8f, "string", vec) iferr_return;
		EXPECT_EQ(str, L"{{}} {} float: {4.8}, {} string, [0, 1, 2, 3]") << str.CStr();
	}

	{
		Alt::StaticVector<usize, 4> vec{ 0, 1, 2, 3, };

		const bool isEstimatable = Alt::TypeTraits::is_format_estimatable_v<Alt::StaticVector<usize, 4>>;
		EXPECT_TRUE(isEstimatable);

		Alt::StringUTF16<> str;
		str.FormatAppend("{{{{}} {{} float: {{{}}, {{}} {}, {}", 4.8f) iferr_return;
		EXPECT_EQ(str, L"{{}} {} float: {4.8}, {{}} {}, {}") << str.CStr();
	}

	{
		Alt::StringUTF16<> str;
		str.FormatAppend("{} + {} == {}", 2, 30, 32) iferr_return;
		EXPECT_EQ(str, L"2 + 30 == 32") << str.CStr();
	}

	{
		EXPECT_TRUE(Alt::TypeTraits::is_format_estimatable_v<CustomStruct>);

		CustomStruct data{};
		data.val = 0.1;

		Alt::StringUTF16<> str;
		str.FormatAppend("{just_some_parameter_example}", data) iferr_return;

		EXPECT_EQ(str, L"CustomStruct{ 0.1 }") << str.CStr();

		str.Clear();
		str.FormatAppend("{{ {just_some_parameter_example} }", data) iferr_return;

		EXPECT_EQ(str, L"{ CustomStruct{ 0.1 } }") << str.CStr();

		str.Clear();
		str.FormatAppend("{{just_some_parameter_example}}") iferr_return;

		EXPECT_EQ(str, L"{{just_some_parameter_example}}") << str.CStr();

		str.Clear();
		str.FormatAppend("{{{{just_some_parameter_example}} {}", data) iferr_return;

		EXPECT_EQ(str, L"{{just_some_parameter_example}} CustomStruct{ 0.1 }") << str.CStr();

		str.Clear();
		str.FormatAppend("{{{just_some_parameter_example}}", data) iferr_return;

		EXPECT_EQ(str, L"{CustomStruct{ 0.1 }}") << str.CStr();
	}

	{
		auto arr = Alt::MakeArray(CustomStruct{}, CustomStruct{}, CustomStruct{}, CustomStruct{});

		Alt::StringUTF16<> str;
		str.FormatAppend("{}", arr) iferr_return;

		EXPECT_EQ(str, L"[CustomStruct{ 0 }, CustomStruct{ 0 }, CustomStruct{ 0 }, CustomStruct{ 0 }]") << str.CStr();

		str.Clear();
		str.FormatAppend("{inline}", arr) iferr_return;

		EXPECT_EQ(str, L"[CustomStruct{ 0 }, CustomStruct{ 0 }, CustomStruct{ 0 }, CustomStruct{ 0 }]") << str.CStr();

		str.Clear();
		str.FormatAppend("{noinline}", arr) iferr_return;

		EXPECT_EQ(str, L"[\n    CustomStruct{ 0 },\n    CustomStruct{ 0 },\n    CustomStruct{ 0 },\n    CustomStruct{ 0 }\n]") << str.CStr();
	}

	{
		Alt::StringUTF16<> str = Alt::StringUTF16<>::Allocate("Something") iferr_return;

		Alt::FormatResult result = str.FormatAppend("{} {}", 0, 0, 0);
		EXPECT_EQ(str, L"Something");

		ASSERT_FALSE(result);
		EXPECT_EQ(result.GetErr(), Alt::FormatError::ManyArguments);

		result = str.FormatAppend("{} {", 0, 0);
		EXPECT_EQ(str, L"Something");

		ASSERT_FALSE(result);
		EXPECT_EQ(result.GetErr(), Alt::FormatError::InvalidFormat);

		result = str.FormatAppend("{} {{   ", 0, 0);
		EXPECT_EQ(str, L"Something");

		ASSERT_FALSE(result);
		EXPECT_EQ(result.GetErr(), Alt::FormatError::ManyArguments);
	}

	{
		const Alt::Array<Alt::Array<Alt::StringUTF16<>, 2>, 2> arr{
			Alt::Array<Alt::StringUTF16<>, 2>{
				Alt::StringUTF16<>::Allocate(L"String0").GetValueOrDefault(),
				Alt::StringUTF16<>::Allocate(L"String1").GetValueOrDefault()
			},
			Alt::Array<Alt::StringUTF16<>, 2>{
				Alt::StringUTF16<>::Allocate(L"String0").GetValueOrDefault(),
				Alt::StringUTF16<>::Allocate(L"String1").GetValueOrDefault()
			},
		};

		Alt::StringUTF16<> str;

		str.FormatAppend("{inline;noinline}", arr) iferr_return;
		const Alt::Slice<const char16_t> resultStr0 =
u"[[\n"
"    String0,\n"
"    String1\n"
"], [\n"
"    String0,\n"
"    String1\n"
"]]"_s;

		EXPECT_EQ(str, resultStr0);

		str.Clear();
		str.FormatAppend("{noinline;inline}", arr) iferr_return;
		const Alt::Slice<const char16_t> resultStr1 =
u"[\n"
"    [String0, String1],\n"
"    [String0, String1]\n"
"]";

		EXPECT_EQ(str, resultStr1);

		str.Clear();
		str.FormatAppend("{noinline;noinline}", arr) iferr_return;
		const Alt::Slice<const char16_t> resultStr2 =
u"[\n"
"    [\n"
"        String0,\n"
"        String1\n"
"    ],\n"
"    [\n"
"        String0,\n"
"        String1\n"
"    ]\n"
"]";

		EXPECT_EQ(str, resultStr2);
	}
}

TEST(StringUTF16, ParseFromUnsigned) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const u8 num = (u8)rnd.GenerateU64(0, Alt::Num::Desc<u8>::Max());

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const u8 val = Alt::StringUTF16<>::ParseFrom<u8>(str) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u16 num = (u16)rnd.GenerateU64(0, Alt::Num::Desc<u16>::Max());

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const u16 val = Alt::StringUTF16<>::ParseFrom<u16>(str) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u32 num = (u32)rnd.GenerateU64(0, Alt::Num::Desc<u32>::Max());

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const u32 val = Alt::StringUTF16<>::ParseFrom<u32>(str) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u64 num = (u64)rnd.GenerateU64(0, Alt::Num::Desc<u64>::Max());

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const u64 val = Alt::StringUTF16<>::ParseFrom<u64>(str) iferr_return;
		EXPECT_EQ(val, num);
	}
}

TEST(StringUTF16, ParseFromSigned) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const s8 num = (s8)rnd.GenerateS64(Alt::Num::Desc<s8>::Min(), Alt::Num::Desc<s8>::Max());

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const s8 val = Alt::StringUTF16<>::ParseFrom<s8>(str) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s16 num = (s16)rnd.GenerateS64(Alt::Num::Desc<s16>::Min(), Alt::Num::Desc<s16>::Max());

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const s16 val = Alt::StringUTF16<>::ParseFrom<s16>(str) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s32 num = (s32)rnd.GenerateS64(Alt::Num::Desc<s32>::Min(), Alt::Num::Desc<s32>::Max());

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const s32 val = Alt::StringUTF16<>::ParseFrom<s32>(str) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s64 num = (s64)rnd.GenerateS64(Alt::Num::Desc<s64>::Min(), Alt::Num::Desc<s64>::Max());

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const s64 val = Alt::StringUTF16<>::ParseFrom<s64>(str) iferr_return;
		EXPECT_EQ(val, num);
	}
}

TEST(StringUTF16, ParseFromUnsignedHex) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const u8 num = (u8)rnd.GenerateU64(0, Alt::Num::Desc<u8>::Max());

		str.Format("{hex}", num) iferr_return;

		const u8 val = Alt::StringUTF16<>::ParseFrom<u8>(str, Alt::StringUTF16<>::ParseBase::Hex) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u16 num = (u16)rnd.GenerateU64(0, Alt::Num::Desc<u16>::Max());

		str.Format("{hex}", num) iferr_return;

		const u16 val = Alt::StringUTF16<>::ParseFrom<u16>(str, Alt::StringUTF16<>::ParseBase::Hex) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u32 num = (u32)rnd.GenerateU64(0, Alt::Num::Desc<u32>::Max());

		str.Format("{hex}", num) iferr_return;

		const u32 val = Alt::StringUTF16<>::ParseFrom<u32>(str, Alt::StringUTF16<>::ParseBase::Hex) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u64 num = (u64)rnd.GenerateU64(0, Alt::Num::Desc<u64>::Max());

		str.Format("{hex}", num) iferr_return;

		const u64 val = Alt::StringUTF16<>::ParseFrom<u64>(str, Alt::StringUTF16<>::ParseBase::Hex) iferr_return;
		EXPECT_EQ(val, num);
	}
}

TEST(StringUTF16, ParseFromSignedHex) {

	iferr_scope_on_err {

		ALT_BREAKPOINT;
		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const s8 num = (s8)rnd.GenerateS64(Alt::Num::Desc<s8>::Min(), Alt::Num::Desc<s8>::Max());

		str.Format("{hex}", num) iferr_return;

		const s8 val = Alt::StringUTF16<>::ParseFrom<s8>(str, Alt::StringUTF16<>::ParseBase::Hex) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s16 num = (s16)rnd.GenerateS64(Alt::Num::Desc<s16>::Min(), Alt::Num::Desc<s16>::Max());

		str.Format("{hex}", num) iferr_return;

		const s16 val = Alt::StringUTF16<>::ParseFrom<s16>(str, Alt::StringUTF16<>::ParseBase::Hex) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s32 num = (s32)rnd.GenerateS64(Alt::Num::Desc<s32>::Min(), Alt::Num::Desc<s32>::Max());

		str.Format("{hex}", num) iferr_return;

		const s32 val = Alt::StringUTF16<>::ParseFrom<s32>(str, Alt::StringUTF16<>::ParseBase::Hex) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s64 num = (s64)rnd.GenerateS64(Alt::Num::Desc<s64>::Min(), Alt::Num::Desc<s64>::Max());

		str.Format("{hex}", num) iferr_return;

		const s64 val = Alt::StringUTF16<>::ParseFrom<s64>(str, Alt::StringUTF16<>::ParseBase::Hex) iferr_return;
		EXPECT_EQ(val, num);
	}
}

TEST(StringUTF16, ParseFromUnsignedBin) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const u8 num = (u8)rnd.GenerateU64(0, Alt::Num::Desc<u8>::Max());

		str.Format("{bin}", num) iferr_return;

		const u8 val = Alt::StringUTF16<>::ParseFrom<u8>(str, Alt::StringUTF16<>::ParseBase::Bin) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u16 num = (u16)rnd.GenerateU64(0, Alt::Num::Desc<u16>::Max());

		str.Format("{bin}", num) iferr_return;

		const u16 val = Alt::StringUTF16<>::ParseFrom<u16>(str, Alt::StringUTF16<>::ParseBase::Bin) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u32 num = (u32)rnd.GenerateU64(0, Alt::Num::Desc<u32>::Max());

		str.Format("{bin}", num) iferr_return;

		const u32 val = Alt::StringUTF16<>::ParseFrom<u32>(str, Alt::StringUTF16<>::ParseBase::Bin) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const u64 num = (u64)rnd.GenerateU64(0, Alt::Num::Desc<u64>::Max());

		str.Format("{bin}", num) iferr_return;

		const u64 val = Alt::StringUTF16<>::ParseFrom<u64>(str, Alt::StringUTF16<>::ParseBase::Bin) iferr_return;
		EXPECT_EQ(val, num);
	}
}

TEST(StringUTF16, ParseFromSignedBin) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const s8 num = (s8)rnd.GenerateS64(Alt::Num::Desc<s8>::Min(), Alt::Num::Desc<s8>::Max());

		str.Format("{bin}", num) iferr_return;

		const s8 val = Alt::StringUTF16<>::ParseFrom<s8>(str, Alt::StringUTF16<>::ParseBase::Bin) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s16 num = (s16)rnd.GenerateS64(Alt::Num::Desc<s16>::Min(), Alt::Num::Desc<s16>::Max());

		str.Format("{bin}", num) iferr_return;

		const s16 val = Alt::StringUTF16<>::ParseFrom<s16>(str, Alt::StringUTF16<>::ParseBase::Bin) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s32 num = (s32)rnd.GenerateS64(Alt::Num::Desc<s32>::Min(), Alt::Num::Desc<s32>::Max());

		str.Format("{bin}", num) iferr_return;

		const s32 val = Alt::StringUTF16<>::ParseFrom<s32>(str, Alt::StringUTF16<>::ParseBase::Bin) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const s64 num = (s64)rnd.GenerateS64(Alt::Num::Desc<s64>::Min(), Alt::Num::Desc<s64>::Max());

		str.Format("{bin}", num) iferr_return;

		const s64 val = Alt::StringUTF16<>::ParseFrom<s64>(str, Alt::StringUTF16<>::ParseBase::Bin) iferr_return;
		EXPECT_EQ(val, num);
	}
}

TEST(StringUTF16, ParseFromFloat) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const f32 num = rnd.GenerateF32_01();

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const f32 val = Alt::StringUTF16<>::ParseFrom<f32>(str) iferr_return;
		EXPECT_EQ(val, num);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const f64 num = rnd.GenerateF32_01();

		str.Clear();
		str.ToStringAppend(num) iferr_return;

		const f64 val = Alt::StringUTF16<>::ParseFrom<f64>(str) iferr_return;
		EXPECT_EQ(val, num);
	}

	{
		f32 val = Alt::StringUTF16<>::ParseFrom<f32>(u"inf") iferr_return;
		EXPECT_EQ(val, Alt::Num::Desc<f32>::Inf());
	}

	{
		f64 val = Alt::StringUTF16<>::ParseFrom<f64>(u"inf") iferr_return;
		EXPECT_EQ(val, Alt::Num::Desc<f64>::Inf());
	}
}

TEST(StringUTF16, ParseFromFloatDec) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const f32 num = rnd.GenerateF32_01();

		str.Format("{dec.4}", num) iferr_return;

		const f32 val = Alt::StringUTF16<>::ParseFrom<f32>(str, u'.', Alt::StringUTF16<>::ParseFloatBits::General) iferr_return;
		EXPECT_TRUE(Alt::Num::Abs(val - num) < 1e-4f);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const f64 num = rnd.GenerateF64_01();

		str.Format("{dec.4}", num) iferr_return;

		const f64 val = Alt::StringUTF16<>::ParseFrom<f64>(str, u'.', Alt::StringUTF16<>::ParseFloatBits::General) iferr_return;
		EXPECT_TRUE(Alt::Num::Abs(val - num) < 1e-4);
	}
}

TEST(StringUTF16, ParseFromFloatFixed) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const f32 num = rnd.GenerateF32_01();

		str.Format("{fixed.4}", num) iferr_return;

		const f32 val = Alt::StringUTF16<>::ParseFrom<f32>(str, u'.', Alt::StringUTF16<>::ParseFloatBits::Fixed) iferr_return;
		EXPECT_TRUE(Alt::Num::Abs(val - num) < 1e-4f);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const f64 num = rnd.GenerateF64_01();

		str.Format("{fixed.4}", num) iferr_return;

		const f64 val = Alt::StringUTF16<>::ParseFrom<f64>(str, u'.', Alt::StringUTF16<>::ParseFloatBits::Fixed) iferr_return;
		EXPECT_TRUE(Alt::Num::Abs(val - num) < 1e-4);
	}
}

TEST(StringUTF16, ParseFromFloatScientific) {

	iferr_scope_on_err {

		FAIL();
	};

	static const usize ITERS = 100'000;

	Alt::StringUTF16<> str;
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const f32 num = rnd.GenerateF32_01();

		str.Format("{scientific.4}", num) iferr_return;

		const f32 val = Alt::StringUTF16<>::ParseFrom<f32>(str, u'.', Alt::StringUTF16<>::ParseFloatBits::Scientific) iferr_return;
		EXPECT_TRUE(Alt::Num::Abs(val - num) < 1e-4f);
	}

	for (usize i = 0; i < ITERS; ++i) {

		const f64 num = rnd.GenerateF64_01();

		str.Format("{scientific.4}", num) iferr_return;

		const f64 val = Alt::StringUTF16<>::ParseFrom<f64>(str, u'.', Alt::StringUTF16<>::ParseFloatBits::Scientific) iferr_return;
		EXPECT_TRUE(Alt::Num::Abs(val - num) < 1e-4);
	}
}

TEST(StringUTF16, FormatPtr) {

	iferr_scope_on_err {

		FAIL();
	};

	Alt::StringUTF16<> str;

#if defined(ALT_PLATFORM_64BIT)

	str.Format("{}", (void*)nullptr);
	EXPECT_EQ(str, u"0000000000000000"_s) << str.CStr();

	str.Format("{}", (void*)4);
	EXPECT_EQ(str, u"0000000000000004"_s) << str.CStr();

	str.Format("{}", (void*)11111111444444444);
	EXPECT_EQ(str, u"0027797f3ab4b51c"_s) << str.CStr();

	str.Format("{}", (void*)11199999999);
	EXPECT_EQ(str, u"000000029b926fff"_s) << str.CStr();

	str.Format("{}", (void*)1444441);
	EXPECT_EQ(str, u"0000000000160a59"_s) << str.CStr();

#elif defined(ALT_PLATFORM_32BIT)

	str.Format("{}", (void*)nullptr);
	EXPECT_EQ(str, u"00000000"_s) << str.CStr();

	str.Format("{}", (void*)4);
	EXPECT_EQ(str, u"00000004"_s) << str.CStr();

	str.Format("{}", (void*)11111111444444444);
	EXPECT_EQ(str, u"3ab4b51c"_s) << str.CStr();

	str.Format("{}", (void*)11199999999);
	EXPECT_EQ(str, u"9b926fff"_s) << str.CStr();

	str.Format("{}", (void*)1444441);
	EXPECT_EQ(str, u"00160a59"_s) << str.CStr();

#endif

	usize x;
	ALT_PRINTLN("{}", &x);
}

TEST(StringUTF16, SplitIteratorByChar0) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234") iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u",");

	Alt::Vector<Alt::Slice<const char16_t>> result = iterator
		.Collect<Alt::Vector<Alt::Slice<const char16_t>>>() iferr_return;

	const auto expected = Alt::MakeArray<Alt::Slice<const char16_t>>(
		u"asdasd",
		u" asdasd ",
		u"adasd",
		u"asdqweqw",
		u"werwe",
		u"sdfer",
		u"345345345",
		u"",
		u"",
		u"",
		u"",
		u"",
		u"",
		u"234234234"
	);

	EXPECT_EQ(result, expected);
}

TEST(StringUTF16, SplitIteratorByChar1) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234") iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u"w");

	Alt::Vector<Alt::Slice<const char16_t>> result = iterator
		.Collect<Alt::Vector<Alt::Slice<const char16_t>>>() iferr_return;

	const auto expected = Alt::MakeArray<Alt::Slice<const char16_t>>(
		u"asdasd, asdasd ,adasd,asdq",
		u"eq",
		u",",
		u"er",
		u"e,sdfer,345345345,,,,,,,234234234"
	);

	EXPECT_EQ(result, expected);
}

TEST(StringUTF16, SplitIteratorByChar2) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234") iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u"w");

	Alt::Vector<Alt::Slice<const char16_t>> result;
	while (Alt::Optional<Alt::Slice<const char16_t>> str = iterator.NextBack()) {

		result.Push(*str) iferr_return;
	}

	const auto expected = Alt::MakeArray<Alt::Slice<const char16_t>>(
		u"e,sdfer,345345345,,,,,,,234234234",
		u"er",
		u",",
		u"eq",
		u"asdasd, asdasd ,adasd,asdq"
	);

	EXPECT_EQ(result, expected);
}

TEST(StringUTF16, SplitIteratorByChar3) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234") iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u"W");

	Alt::Vector<Alt::Slice<const char16_t>> result;
	while (Alt::Optional<Alt::Slice<const char16_t>> str = iterator.NextBack()) {

		result.Push(*str) iferr_return;
	}

	const auto expected = Alt::MakeArray<Alt::Slice<const char16_t>>(
		u"asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234"
	);

	EXPECT_EQ(result, expected);
}

TEST(StringUTF16, SplitIteratorByCharFuzzy) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize TEST_CNT = 10'000;
	constexpr usize TEST_STR_MAX_SIZE = 1'000;
	constexpr bool IS_ALPHANUM = false;

	Alt::Vector<Alt::Slice<const char16_t>> result;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const Alt::StringUTF16<> str = AllocateStrUTF16(0, TEST_STR_MAX_SIZE, IS_ALPHANUM);

		const char16_t del = (char16_t)GetNum<u8>();
		const Alt::Slice<const char16_t> delStr{ &del, 1 };

		result.Clear();

		Alt::SplitIterator<const char16_t> splitIter = str.GetSplitIterator(delStr);
		while (Alt::Optional<Alt::Slice<const char16_t>> str = splitIter.Next()) {

			result.Push(*str) iferr_return;
		}

		Alt::Vector<Alt::Slice<const char16_t>> expected = SplitByWordStable(str, delStr) iferr_return;

		result.Push(delStr) iferr_return;
		expected.Push(delStr) iferr_return;

		EXPECT_EQ(result, expected);
	}
}

TEST(StringUTF16, SplitIteratorByCharEmpty) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate() iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u",");

	Alt::Vector<Alt::Slice<const char16_t>> result;
	while (Alt::Optional<Alt::Slice<const char16_t>> str = iterator.NextBack()) {

		result.Push(*str) iferr_return;
	}

	EXPECT_TRUE(result.IsEmpty());
}

TEST(StringUTF16, SplitIteratorByCharNullDelimeter) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate(u"asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234") iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u"\0");

	Alt::Vector<Alt::Slice<const char16_t>> result;
	while (Alt::Optional<Alt::Slice<const char16_t>> str = iterator.NextBack()) {

		result.Push(*str) iferr_return;
	}

	const auto expected = Alt::MakeArray<Alt::Slice<const char16_t>>(
		u"asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234"
	);

	EXPECT_EQ(result, expected);
}

TEST(StringUTF16, SplitIteratorByCharNotExistDelimeter) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234") iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u"1");

	Alt::Vector<Alt::Slice<const char16_t>> result;
	while (Alt::Optional<Alt::Slice<const char16_t>> str = iterator.NextBack()) {

		result.Push(*str) iferr_return;
	}

	const auto expected = Alt::MakeArray<Alt::Slice<const char16_t>>(
		u"asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234"
	);

	EXPECT_EQ(result, expected);
}

TEST(StringUTF16, SplitIterator0) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234") iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u"as");

	Alt::Vector<Alt::Slice<const char16_t>> result;
	while (Alt::Optional<Alt::Slice<const char16_t>> str = iterator.Next()) {

		result.Push(*str) iferr_return;
	}

	const auto expected = Alt::MakeArray<Alt::Slice<const char16_t>>(
		u"",
		u"d",
		u"d, ",
		u"d",
		u"d ,ad",
		u"d,",
		u"dqweqw,werwe,sdfer,345345345,,,,,,,234234234"
	);

	EXPECT_EQ(result, expected);
}

TEST(StringUTF16, SplitIterator1) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234") iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u"As");

	Alt::Vector<Alt::Slice<const char16_t>> result;
	while (Alt::Optional<Alt::Slice<const char16_t>> str = iterator.Next()) {

		result.Push(*str) iferr_return;
	}

	const auto expected = Alt::MakeArray<Alt::Slice<const char16_t>>(
		u"asdasd, asdasd ,adasd,asdqweqw,werwe,sdfer,345345345,,,,,,,234234234"
	);

	EXPECT_EQ(result, expected);
}

TEST(StringUTF16, SplitIterator9) {

	iferr_scope_on_err {

		FAIL();
	};

	const Alt::StringUTF16<> string = Alt::StringUTF16<>::Allocate("KQGnMvmyufVJgv2h6VztPd9wW") iferr_return;
	Alt::SplitIterator<const char16_t> iterator = string.GetSplitIterator(u"GnMvmyufVJgv2h6VztP");

	Alt::Vector<Alt::Slice<const char16_t>> result;
	while (Alt::Optional<Alt::Slice<const char16_t>> str = iterator.Next()) {

		result.Push(*str) iferr_return;
	}

	const auto expected = Alt::MakeArray<Alt::Slice<const char16_t>>(
		u"KQ",
		u"d9wW"
	);

	EXPECT_EQ(result, expected);
}

TEST(StringUTF16, SplitIteratorFuzzy) {

	iferr_scope_on_err {

		FAIL();
	};

	constexpr usize TEST_CNT = 10'000;
	constexpr usize TEST_STR_MAX_SIZE = 1'000;
	constexpr bool IS_ALPHANUM = false;

	Alt::Vector<Alt::Slice<const char16_t>> result;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const Alt::StringUTF16<> str = AllocateStrUTF16(0, TEST_STR_MAX_SIZE, IS_ALPHANUM);

		const usize offs = GetNum<usize>(0, str.GetSizeU16());
		const usize cnt = GetNum<usize>(0, str.GetSizeU16() - offs);
		if (cnt == 0) {
			continue;
		}

		const Alt::Slice<const char16_t> del = Alt::Slice<const char16_t>(str).SubSlice(offs, cnt);

		result.Clear();

		Alt::SplitIterator<const char16_t> splitIter = str.GetSplitIterator(del);
		while (Alt::Optional<Alt::Slice<const char16_t>> str = splitIter.Next()) {

			result.Push(*str) iferr_return;
		}

		Alt::Vector<Alt::Slice<const char16_t>> expected = SplitByWordStable(str, del) iferr_return;

		result.Push(del) iferr_return;
		expected.Push(del) iferr_return;

		EXPECT_EQ(result, expected);
	}
}
