#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Iterators/Iterator.h"
#include "../alt-std/src/Iterators/STD/STDCollectors.h"

#include "../alt-std/src/Platform/Platform.h"

#include "../alt-std/src/Memory/Ref.h"

#include "../alt-std/src/Utils/Range.h"
#include "../alt-std/src/Utils/Optional.h"

namespace {

	template <typename T>
	class VecIterator final
		: public Alt::Iterator<VecIterator<T>, T, Alt::IIteratorForwardTrait, Alt::IIteratorBackwardTrait, Alt::IIteratorExactSizeTrait>
	{
	public:

	   VecIterator(const std::vector<T>& vec) noexcept
			: m_pBegin(vec.data())
			, m_pEnd(vec.data() + vec.size())
		{}

	   usize GetSizeHint() const noexcept {
		   return GetSize();
	   }

	   usize GetSize() const noexcept {
			return m_pEnd - m_pBegin;
	   }

	   bool IsEmpty() const noexcept {
			return m_pEnd == m_pBegin;
	   }

		Alt::Optional<T> Next() noexcept {

			return m_pBegin != m_pEnd
				? Alt::MakeOptional(*(m_pBegin++))
				: Alt::Nullopt;
		}

		usize Advance(const usize advance_) noexcept {

			const usize advance = Alt::Num::Min(GetSize(), advance_);
			m_pBegin += advance;

			return advance;
		}

		Alt::Optional<T> NextBack() noexcept {

			return m_pBegin != m_pEnd
				? Alt::MakeOptional(*(--m_pEnd))
				: Alt::Nullopt;
		}

		usize AdvanceBack(const usize advance_) noexcept {

			const usize advance = Alt::Num::Min(GetSize(), advance_);
			m_pEnd -= advance;

			return advance;
		}

	private:

		const T* m_pBegin;
		const T* m_pEnd;
	};
}

TEST(Iterator, RangeIterator) {

	usize val = 4;

	Alt::Ref<usize> x0 = val;
	usize& x1 = x0;

	Alt::Optional<usize> xx0 = val;
	x1 = xx0.GetValue();
	x0 = xx0.GetValue();

	Alt::Optional<Alt::Ref<usize>> xx1 = val;

	xx0 = xx1.GetValue();
	xx1 = xx0.GetValue();

	Alt::RangeUsize range{ 0, 10 };

	EXPECT_EQ(range.GetIterator().Count(), range.Size());
	EXPECT_EQ(range.GetIterator().Step(2).Count(), range.Size() / 2);
}

TEST(Iterator, IteratorCustom) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter.CloneIter();
	EXPECT_EQ(*it.Next(), 0);
	EXPECT_EQ(*it.Next(), 1);
	EXPECT_FALSE(it.Next());
}

TEST(Iterator, IteratorCount) {

	const std::vector<usize> vec{ 0, 1 };

	VecIterator<usize> iter{ vec };
	EXPECT_EQ(iter.CloneIter().Count(), 2);
}

TEST(Iterator, IteratorEnumerate) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	iter
		.CloneIter()
		.Enumerate()
		.ForEach([](const Alt::Pair<usize, usize>& val) {

			const usize i = val.f();
			const usize v = val.s();

			EXPECT_EQ(i, v);
		});
}

TEST(Iterator, IteratorPeekable) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter
		.CloneIter()
		.Peekable();

	EXPECT_EQ(*it.Peek(), 0);
	EXPECT_EQ(*it.Peek(), 0);
	EXPECT_EQ(*it.Next(), 0);

	EXPECT_EQ(*it.Peek(), 1);
	EXPECT_EQ(*it.Peek(), 1);
	EXPECT_EQ(*it.Next(), 1);

	EXPECT_FALSE(it.Next());
	EXPECT_FALSE(it.Peek());
}

TEST(Iterator, IteratorFuse) {

	Alt::IteratorForwardFromFunc iter{
		[]() {
			static usize cnt = 0;

			const usize result = cnt;
			cnt += 1;

			return result % 2 == 0
				? Alt::Nullopt
				: Alt::MakeOptional<usize>(result);
		}
	};

	auto it = iter
		.CloneIter();

	auto itFuse = iter
		.CloneIter()
		.Fuse();

	EXPECT_EQ(itFuse.Next(), Alt::Nullopt);
	EXPECT_EQ(itFuse.Next(), Alt::Nullopt);
	EXPECT_EQ(itFuse.Next(), Alt::Nullopt);
	EXPECT_EQ(itFuse.Next(), Alt::Nullopt);

	EXPECT_EQ(*it.Next(), 1);
	EXPECT_EQ(it.Next(), Alt::Nullopt);
	EXPECT_EQ(*it.Next(), 3);
	EXPECT_EQ(it.Next(), Alt::Nullopt);
}

TEST(Iterator, IteratorChain) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter
		.CloneIter()
		.Map([](const usize val) {
			return val + 1;
		})
		.Chain(iter);

	EXPECT_EQ(*it.Next(), 1);
	EXPECT_EQ(*it.Next(), 2);
	EXPECT_EQ(*it.Next(), 0);
	EXPECT_EQ(*it.Next(), 1);
	EXPECT_EQ(it.Next(), Alt::Nullopt);
}

TEST(Iterator, IteratorSkip) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter
		.CloneIter()
		.Chain(iter)
		.Skip(2);

	EXPECT_EQ(*it.Next(), 0);
	EXPECT_EQ(*it.Next(), 1);
	EXPECT_EQ(it.Next(), Alt::Nullopt);
}

TEST(Iterator, IteratorSkipWhile) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter
		.CloneIter()
		.Chain(iter)
		.Enumerate()
		.SkipWhile([](const Alt::Pair<usize, usize>& kv) {
			return kv.f() < 2;
		});

	EXPECT_EQ(it.Next()->s(), 0);
	EXPECT_EQ(it.Next()->s(), 1);
	EXPECT_EQ(it.Next(), Alt::Nullopt);
}

TEST(Iterator, IteratorTake) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter
		.CloneIter()
		.Chain(iter)
		.Take(2);

	EXPECT_EQ(*it.Next(), 0);
	EXPECT_EQ(*it.Next(), 1);
	EXPECT_EQ(it.Next(), Alt::Nullopt);
}

TEST(Iterator, IteratorTakeWhile) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter
		.CloneIter()
		.Chain(iter)
		.Enumerate()
		.TakeWhile([](const Alt::Pair<usize, usize>& kv) {
			return kv.f() < 2;
		});

	EXPECT_EQ(it.Next()->s(), 0);
	EXPECT_EQ(it.Next()->s(), 1);
	EXPECT_EQ(it.Next(), Alt::Nullopt);
}

TEST(Iterator, IteratorStep) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	{
		auto it = iter
			.CloneIter()
			.Chain(iter)
			.Step(1)
			.Filter([](const usize) {
				return true;
			});

		EXPECT_EQ(*it.Next(), 0);
		EXPECT_EQ(*it.Next(), 1);
		EXPECT_EQ(*it.Next(), 0);
		EXPECT_EQ(*it.Next(), 1);
		EXPECT_EQ(it.Next(), Alt::Nullopt);
	}

	{
		auto it = iter
			.CloneIter()
			.Chain(iter)
			.Step(2);

		EXPECT_EQ(*it.Next(), 0);
		EXPECT_EQ(*it.Next(), 0);
		EXPECT_EQ(it.Next(), Alt::Nullopt);
	}
}

TEST(Iterator, IteratorZip) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter
		.CloneIter()
		.Zip(iter);

	const Alt::Pair<usize, usize> p0{ 0, 0 };
	EXPECT_EQ(*it.Next(), p0);

	const Alt::Pair<usize, usize> p1{ 1, 1 };
	EXPECT_EQ(*it.Next(), p1);

	EXPECT_EQ(it.Next(), Alt::Nullopt);
}

TEST(Iterator, IteratorFind) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter0{ vec };

	{
		auto iter = iter0
			.CloneIter()
			.Chain(iter0);

		Alt::Optional<usize> val = iter
			.Find([](const usize val) -> bool {

				return val % 2 == 1;
			});

		EXPECT_TRUE(val);
		EXPECT_EQ(*val, 1);

		val = iter
			.Find([](const usize val) -> bool {

				return val % 2 == 1;
			});

		EXPECT_TRUE(val);
		EXPECT_EQ(*val, 1);

		val = iter
			.Find([](const usize val) -> bool {

				return val % 2 == 1;
			});

		EXPECT_FALSE(val);
	}

	{
		Alt::Optional<Alt::Pair<usize, usize>> val = iter0
			.CloneIter()
			.Enumerate()
			.Find([](const Alt::Pair<usize, usize>& val) -> bool {

				return val.s() % 2 == 1;
			});

		EXPECT_TRUE(val);
		EXPECT_EQ(val->f(), 1);
		EXPECT_EQ(val->s(), 1);
	}
}

TEST(Iterator, IteratorMap) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	Alt::Optional<Alt::Pair<usize, usize>> val = iter
		.CloneIter()
		.Map([](const usize val) {
			return val + 1;
		})
		.Enumerate()
		.Find([](const Alt::Pair<usize, usize>& val) -> bool {

			return val.s() % 2 == 1;
		});

	EXPECT_TRUE(val);
	EXPECT_EQ(val->f(), 0);
	EXPECT_EQ(val->s(), 1);
}

TEST(Iterator, IteratorIntersperse) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	Alt::Optional<Alt::Pair<usize, usize>> val = iter
		.CloneIter()
		.Inspect([](const usize val) {

			ALT_PRINTLN("{}", val);
		})
		.Enumerate()
		.Find([](const Alt::Pair<usize, usize>& val) -> bool {

			return val.s() % 2 == 1;
		});

	EXPECT_TRUE(val);
	EXPECT_EQ(val->f(), 1);
	EXPECT_EQ(val->s(), 1);
}

TEST(Iterator, IteratorInspect) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	Alt::Optional<Alt::Pair<usize, usize>> val = iter
		.CloneIter()
		.Intersperse([]() {
			return 0;
		})
		.Enumerate()
		.Find([](const Alt::Pair<usize, usize>& val) -> bool {

			return val.s() % 2 == 1;
		});

	EXPECT_TRUE(val);
	EXPECT_EQ(val->f(), 2);
	EXPECT_EQ(val->s(), 1);
}

TEST(Iterator, IteratorCopied) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	Alt::Optional<Alt::Pair<usize, usize>> val = iter
		.CloneIter()
		.Copied()
		.Map([](const usize val) {
			return val + 1;
		})
		.Enumerate()
		.Find([](const Alt::Pair<usize, usize>& val) -> bool {

			return val.s() % 2 == 1;
		});

	EXPECT_TRUE(val);
	EXPECT_EQ(val->f(), 0);
	EXPECT_EQ(val->s(), 1);
}

TEST(Iterator, IteratorFlatten) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter
		.CloneIter()
		.Map([](const usize) {

			static const std::vector<usize> vec{ 0, 1 };
			VecIterator<usize> iter{ vec };

			return iter;
		})
		.Flatten();

	EXPECT_EQ(*it.Next(), 0);
	EXPECT_EQ(*it.Next(), 1);
	EXPECT_EQ(*it.Next(), 0);
	EXPECT_EQ(*it.Next(), 1);
	EXPECT_EQ(it.Next(), Alt::Nullopt);
}

TEST(Iterator, IteratorRev) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto it = iter
		.CloneIter()
		.Rev();

	EXPECT_EQ(*it.Next(), 1);
	EXPECT_EQ(*it.Next(), 0);
	EXPECT_EQ(it.Next(), Alt::Nullopt);
}

TEST(Iterator, Collect) {

	const std::vector<usize> vec{ 0, 1 };
	VecIterator<usize> iter{ vec };

	auto vec_ = iter
		.CloneIter()
		.CollectOrDefault<std::vector<usize>>();

	EXPECT_EQ(vec_.size(), vec.size());
	EXPECT_EQ(vec_, vec);

	auto us_ = iter
		.CloneIter()
		.CollectOrDefault<std::unordered_set<usize>>();

	EXPECT_EQ(us_.size(), vec.size());
	EXPECT_EQ(std::vector<usize>(us_.begin(), us_.end()), vec);

	auto um_ = iter
		.CloneIter()
		.Enumerate()
		.CollectOrDefault<std::unordered_map<usize, usize>>();

	EXPECT_EQ(um_.size(), vec.size());
	EXPECT_EQ(um_[0], vec[0]);
	EXPECT_EQ(um_[1], vec[1]);

	auto s_ = iter
		.CloneIter()
		.CollectOrDefault<std::set<usize>>();

	EXPECT_EQ(s_.size(), vec.size());
	EXPECT_EQ(std::vector<usize>(s_.begin(), s_.end()), vec);

	auto m_ = iter
		.CloneIter()
		.Enumerate()
		.CollectOrDefault<std::map<usize, usize>>();

	EXPECT_EQ(m_.size(), vec.size());
	EXPECT_EQ(m_[0], vec[0]);
	EXPECT_EQ(m_[1], vec[1]);
}

TEST(Iterator, FromFunc) {

	{
		Alt::IteratorForwardFromFunc iter{
			[]() {
				static usize cnt = 0;
				return cnt++;
			}
		};

		auto iter0 = iter
			.Take(2);

		EXPECT_EQ(*iter0.Next(), 0);
		EXPECT_EQ(*iter0.Next(), 1);
		EXPECT_EQ(iter0.Next(), Alt::Nullopt);
	}

	{
		Alt::IteratorBackwardFromFunc iter{
			[]() {
				static usize cnt = 0;
				return cnt--;
			}
		};

		auto iter0 = iter
			.Rev()
			.Take(2);

		EXPECT_EQ(*iter0.Next(), (usize)0);
		EXPECT_EQ(*iter0.Next(), (usize)0 - 1);
		EXPECT_EQ(iter0.Next(), Alt::Nullopt);
	}
}

TEST(Iterator, FromSTDIter) {

	const std::vector<usize> vec{ 0, 1 };
	Alt::IteratorFromSTD stdIter{ vec.begin(), vec.end() };

	const std::vector<usize> vec0 = stdIter
		.CloneIter()
		.Map([](const usize* pVal) {
			return *pVal;
		})
		.CollectOrDefault<std::vector<usize>>();

	EXPECT_EQ(vec0, vec);

	EXPECT_EQ(*stdIter.Next(), 0);
	EXPECT_EQ(*stdIter.Next(), 1);
	EXPECT_EQ(stdIter.Next(), Alt::Nullopt);
}

TEST(Iterator, STDIterator) {

	{
		const std::vector<usize> vec{ 0, 1 };
		const VecIterator<usize> iter{ vec };

		usize i = 0;
		for (const usize val : iter) {

			EXPECT_EQ(val, vec[i]);
			i += 1;
		}
	}

	{
		Alt::IteratorForwardFromFunc iter{
			[]() {
				static usize cnt = 0;
				return cnt++;
			}
		};

		std::vector<usize> vec;
		for (usize val : iter) {

			if (val == 4) {
				break;
			}

			vec.push_back(val);
		}

		assert(vec[0] == 0);
		assert(vec[1] == 1);
		assert(vec[2] == 2);
		assert(vec[3] == 3);
	}
}
