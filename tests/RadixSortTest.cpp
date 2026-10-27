#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/IfErr.h"
#include "../alt-std/src/Utils/ScopeDefer.h"
#include "../alt-std/src/Random/Xoshiro256.h"

#include "../alt-std/src/Collections/Vector.h"
#include "../alt-std/src/Algorithms/RadixSort.h"

namespace {

	constexpr static usize TEST_CNT = 10;
	constexpr static usize TEST_MAX_ELEMENT_CNT = 1'000'000;

	template <typename T>
	constexpr static bool is_byte = std::is_same_v<T, u8> || std::is_same_v<T, s8>;

	struct Key {
		usize key;
	};

	template <typename T>
	static Alt::Optional<Alt::Vector<T>> AllocateVector(const usize min, const usize max) noexcept {

		Alt::Vector<T> result;

		auto iterator = Alt::IteratorForwardFromFunc([]() { return GetNum<T>(); })
			.Take(GetNum<usize>(min, max));

		for (T val : iterator) {

			result.Push(val);
		}

		return result;

		//return Alt::IteratorForwardFromFunc([]() { return GetNum<T>(); })
		//	.Take(GetNum<usize>(min, max))
		//	.Collect<Alt::Vector<T>>();
	}

	static Alt::Optional<Alt::Vector<Key>> FillKeysVector(Alt::Vector<Key>& res, const usize size) noexcept {
		iferr_scope;

		res.Resize(size) iferr_return Alt::Nullopt;

		res
			.GetIterator()
			.Enumerate()
			.ForEach([](Alt::Pair<usize, Key&> key) {

				key.s() = Key{ key.f() };
			});

		return std::move(res);
	}

	template <typename T>
	static bool CheckIsSorted(const Alt::Vector<T>& v) noexcept {

		return v.GetIterator()
			.Zip(v.GetIterator().Skip(1))
			.Filter([](Alt::Pair<const T&, const T&> val) {
				return val.f() > val.s();
			})
			.Count() == 0;
	}

	template <typename T, typename FuncT>
	static bool CheckIsSorted(const Alt::Vector<T>& v, FuncT&& func) noexcept {

		return v.GetIterator()
			.Zip(v.GetIterator().Skip(1))
			.Filter([&](Alt::Pair<const T&, const T&> val) {
				return (func)(val.f(), val.s());
			})
			.Count() == v.GetSize();
	}
}

TEST(RadixSort, SortByValueU8Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = u8;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;
		Alt::Vector<DataT> buffer = Alt::Vector<DataT>::Allocate(data.GetSize()) iferr_return;

		Alt::RadixSort::Sort<DataT>(data.Data(), buffer.Data(), data.GetSize());

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(RadixSort, SortByValueU16Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = u16;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		Alt::Vector<DataT> buffer = Alt::Vector<DataT>::Allocate(data.GetSize()) iferr_return;
		Alt::RadixSort::Sort<DataT>(data.Data(), buffer.Data(), data.GetSize());

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(RadixSort, SortByValueU32Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = u32;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		Alt::Vector<DataT> buffer = Alt::Vector<DataT>::Allocate(data.GetSize()) iferr_return;
		Alt::RadixSort::Sort<DataT>(data.Data(), buffer.Data(), data.GetSize());

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(RadixSort, SortByValueU64Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = u64;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		Alt::Vector<DataT> buffer = Alt::Vector<DataT>::Allocate(data.GetSize()) iferr_return;
		Alt::RadixSort::Sort<DataT>(data.Data(), buffer.Data(), data.GetSize()) iferr_return;

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(RadixSort, SortByValueI8Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = s8;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		Alt::Vector<DataT> buffer = Alt::Vector<DataT>::Allocate(data.GetSize()) iferr_return;
		Alt::RadixSort::Sort<DataT>(data.Data(), buffer.Data(), data.GetSize());

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(RadixSort, SortByValueI16Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = s16;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		Alt::Vector<DataT> buffer = Alt::Vector<DataT>::Allocate(data.GetSize()) iferr_return;
		Alt::RadixSort::Sort<DataT>(data.Data(), buffer.Data(), data.GetSize());

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(RadixSort, SortByValueI32Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = s32;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		Alt::Vector<DataT> buffer = Alt::Vector<DataT>::Allocate(data.GetSize()) iferr_return;
		Alt::RadixSort::Sort<DataT>(data.Data(), buffer.Data(), data.GetSize());

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(RadixSort, SortByValueI64Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = s64;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		Alt::Vector<DataT> buffer = Alt::Vector<DataT>::Allocate(data.GetSize()) iferr_return;
		Alt::RadixSort::Sort<DataT>(data.Data(), buffer.Data(), data.GetSize()) iferr_return;

		EXPECT_TRUE(CheckIsSorted(data));
	}
}

TEST(RadixSort, SortByKeyU8Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = u8;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		});

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, SortByKeyU16Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = u16;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		});

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, SortByKeyU32Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = u32;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		});

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, SortByKeyU64Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = u64;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		}) iferr_return;

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, SortByKeyI8Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = s8;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		});

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, SortByKeyI16Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = s16;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		});

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, SortByKeyI32Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = s32;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		});

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, SortByKeyI64Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = s64;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		});

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, SortByKeyF32Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = f32;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		});

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, SortByKeyF64Fuzzy) {
	iferr_scope_on_err {
		FAIL();
	};

	Alt::Vector<Key> keys;
	Alt::Vector<Key> buffer;

	for (usize i = 0; i < TEST_CNT; ++i) {

		const usize size = GetNum<usize>(0, TEST_MAX_ELEMENT_CNT);

		using DataT = f64;
		Alt::Vector<DataT> data = AllocateVector<DataT>(0, size) iferr_return;

		FillKeysVector(keys, data.GetSize());
		buffer.Resize(keys.GetSize()) iferr_return;

		Alt::RadixSort::SortKeyByValue<Key, DataT>::Sort(keys.Data(), buffer.Data(), keys.GetSize(), [&](const Key& key) {
			return data[key.key];
		});

		EXPECT_TRUE(CheckIsSorted(keys, [&](const Key& x, const Key& y) {
			return data[x.key] < data[y.key];
		}));
	}
}

TEST(RadixSort, GenericMemory) {
	iferr_scope_on_err {
		FAIL();
	};

	using DataT = usize;

	class VectorPtr : public Alt::RadixSort::Ptr<VectorPtr, DataT> {
	public:

		VectorPtr(Alt::Vector<DataT>& vec) noexcept
			: m_Vec(vec)
		{}

		const DataT& Get(const usize i) const {
			return m_Vec[i];
		}

		DataT&& Take(const usize i) {
			return std::move(m_Vec[i]);
		}

		void Set(const usize i, DataT&& val) {
			m_Vec[i] = std::move(val);
		}

		void MoveObjectFromPtr(Alt::RadixSort::Ptr<VectorPtr, DataT>& bufferPtr, const usize dataLen) {

			for (usize i = 0; i < dataLen; ++i) {
				m_Vec[i] = std::move(bufferPtr.Take(i));
			}
		}

	private:

		Alt::Vector<DataT>& m_Vec;
	};

	Alt::Vector<DataT> data = AllocateVector<DataT>(0, TEST_MAX_ELEMENT_CNT) iferr_return;
	Alt::Vector<DataT> buffer = Alt::Vector<DataT>::Allocate(data.GetSize()) iferr_return;
	Alt::Vector<usize> backets = Alt::Vector<usize>::Allocate(Alt::RadixSort::SortUnsignedImpl_GetBacketBufferSize<usize>()) iferr_return;

	VectorPtr d(data);
	VectorPtr b(buffer);

	Alt::RadixSort::SortUnsignedImpl<DataT>(d, b, backets.Data(), data.GetSize(), [](const usize key) -> usize {
		return key;
	});

	EXPECT_TRUE(CheckIsSorted(data));
}
