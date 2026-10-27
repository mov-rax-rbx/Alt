#pragma once

#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/TypeTraits.h"
#include "../alt-std/src/Random/Xoshiro256.h"
#include "../alt-std/src/String/String.h"
#include "../alt-std/src/String/StringUTF16.h"

#include "../alt-std/src/Platform/Platform.h"

namespace TestPrivate {

static thread_local Alt::Xoshiro256Plus s_Xoshiro256Plus = Alt::Xoshiro256Plus::CreateSeededByTime();
static thread_local Alt::Xoshiro256StarStar s_Xoshiro256StarStar = Alt::Xoshiro256StarStar::CreateSeededByTime();

} // namespace TestPrivate

template <typename T,
	std::enable_if_t<std::is_unsigned_v<T>>* = nullptr>
inline T GetNum(const T min = T(0), const T max = Alt::Num::Desc<T>::Max()) {

	using namespace TestPrivate;

	const u64 num = s_Xoshiro256StarStar.GenerateU64(static_cast<u64>(min), static_cast<u64>(max));

	return static_cast<T>(num);
}

template <typename T,
	std::enable_if_t<std::is_signed_v<T>>* = nullptr>
inline T GetNum(const T min = T(0), const T max = Alt::Num::Desc<T>::Max()) {

	using namespace TestPrivate;

	const s64 num = s_Xoshiro256StarStar.GenerateS64(static_cast<s64>(min), static_cast<s64>(max));

	return static_cast<T>(num);
}

template <>
inline f32 GetNum<f32>(const f32, const f32) {

	using namespace TestPrivate;

	const f32 min = -1.0f;
	const f32 max = 1.0f;

	return min + s_Xoshiro256Plus.GenerateF32_01() * (max - min);
}

template <>
inline f64 GetNum<f64>(const f64, const f64) {

	using namespace TestPrivate;

	const f64 min = -1.0f;
	const f64 max = 1.0f;

	return min + s_Xoshiro256Plus.GenerateF64_01() * (max - min);
}

inline Alt::String AllocateStr(const usize min, const usize max, const bool isAlphanum = true) {

	const usize size = GetNum<usize>(min, max);

	Alt::String res;
	res.ReserveBytes(size);

	for (usize i = 0; i < size; ++i) {

		u8 byte;
		do {

			byte = GetNum<u8>();
		} while (isAlphanum && !std::isalnum(byte));

		ALT_MAYBE_UNUSED const bool isOk = res.Push((chara)byte);
	}

	return res;
}

inline Alt::StringUTF16<> AllocateStrUTF16(const usize min, const usize max, const bool isAlphanum = true) {

	const usize size = GetNum<usize>(min, max);

	Alt::StringUTF16<> res;
	res.ReserveU16(size);

	for (usize i = 0; i < size; ++i) {

		u8 byte;
		do {

			byte = GetNum<u8>();
		} while (isAlphanum && !std::isalnum(byte));

		ALT_MAYBE_UNUSED const bool isOk = res.Push((chara)byte);
	}

	return res;
}

struct Rule5AllTrivial {

	Rule5AllTrivial() noexcept = default;

	Rule5AllTrivial(const Rule5AllTrivial&) noexcept = default;
	Rule5AllTrivial& operator=(const Rule5AllTrivial&) noexcept = default;

	Rule5AllTrivial(Rule5AllTrivial&&) noexcept = default;
	Rule5AllTrivial& operator=(Rule5AllTrivial&&) noexcept = default;

	~Rule5AllTrivial() = default;
};

struct Rule5AllNonTrivial {

	Rule5AllNonTrivial() noexcept {}

	Rule5AllNonTrivial(const Rule5AllNonTrivial&) noexcept {}
	Rule5AllNonTrivial& operator=(const Rule5AllNonTrivial&) noexcept { return *this; }

	Rule5AllNonTrivial(Rule5AllNonTrivial&&) noexcept {}
	Rule5AllNonTrivial& operator=(Rule5AllNonTrivial&&) noexcept { return *this; }

	~Rule5AllNonTrivial() {}
};

struct Rule5AllDeleted {

	Rule5AllDeleted() noexcept = delete;

	Rule5AllDeleted(const Rule5AllDeleted&) noexcept = delete;
	Rule5AllDeleted& operator=(const Rule5AllDeleted&) noexcept = delete;

	Rule5AllDeleted(Rule5AllDeleted&&) noexcept = delete;
	Rule5AllDeleted& operator=(Rule5AllDeleted&&) noexcept = delete;

	~Rule5AllDeleted() = delete;
};

struct Rule5_DefaultConstruct_t {};
constexpr static Rule5_DefaultConstruct_t Rule5_DefaultConstruct = Rule5_DefaultConstruct_t{};

struct Rule5AllDeletedExceptDestructor {

	Rule5AllDeletedExceptDestructor() noexcept = delete;
	Rule5AllDeletedExceptDestructor(Rule5_DefaultConstruct_t) noexcept
	{}

	Rule5AllDeletedExceptDestructor(const Rule5AllDeletedExceptDestructor&) noexcept = delete;
	Rule5AllDeletedExceptDestructor& operator=(const Rule5AllDeletedExceptDestructor&) noexcept = delete;

	Rule5AllDeletedExceptDestructor(Rule5AllDeletedExceptDestructor&&) noexcept = delete;
	Rule5AllDeletedExceptDestructor& operator=(Rule5AllDeletedExceptDestructor&&) noexcept = delete;

	~Rule5AllDeletedExceptDestructor() = default;
};

struct Rule5AllTrivial_DeletedDefaultConstructor {

	Rule5AllTrivial_DeletedDefaultConstructor() noexcept = delete;

	Rule5AllTrivial_DeletedDefaultConstructor(Rule5_DefaultConstruct_t) noexcept
	{}

	Rule5AllTrivial_DeletedDefaultConstructor(const Rule5AllTrivial_DeletedDefaultConstructor&) noexcept = default;
	Rule5AllTrivial_DeletedDefaultConstructor& operator=(const Rule5AllTrivial_DeletedDefaultConstructor&) noexcept = default;

	Rule5AllTrivial_DeletedDefaultConstructor(Rule5AllTrivial_DeletedDefaultConstructor&&) noexcept = default;
	Rule5AllTrivial_DeletedDefaultConstructor& operator=(Rule5AllTrivial_DeletedDefaultConstructor&&) noexcept = default;

	~Rule5AllTrivial_DeletedDefaultConstructor() = default;
};

struct Rule5Tracker {

	struct Core {

		usize cntDefaultConstruct = 0;

		usize cntCopyConstruct = 0;
		usize cntMoveConstruct = 0;

		usize cntCopyAssignment = 0;
		usize cntMoveAssignment = 0;

		usize cntDestructor = 0;
	};

	Core* pCore = nullptr;

	Rule5Tracker(Core& core) noexcept { pCore = &core; }

	Rule5Tracker(const Rule5Tracker& rhs) noexcept { assert(rhs.pCore); pCore = rhs.pCore; pCore->cntCopyConstruct += 1; }
	Rule5Tracker& operator=(const Rule5Tracker& rhs) noexcept { assert(rhs.pCore); pCore = rhs.pCore; pCore->cntCopyAssignment += 1; return *this; }

	Rule5Tracker(Rule5Tracker&& rhs) noexcept { assert(rhs.pCore); pCore = rhs.pCore; pCore->cntMoveConstruct += 1; }
	Rule5Tracker& operator=(Rule5Tracker&& rhs) noexcept { assert(rhs.pCore); pCore = rhs.pCore; pCore->cntMoveAssignment += 1; return *this; }

	~Rule5Tracker() noexcept { assert(pCore); pCore->cntDestructor += 1; }
};

struct Rule5TrackerInlined {

	struct Core {

		usize cntDefaultConstruct = 0;

		usize cntCopyConstruct = 0;
		usize cntMoveConstruct = 0;

		usize cntCopyAssignment = 0;
		usize cntMoveAssignment = 0;

		usize cntDestructor = 0;
	};

	Core core;

	Rule5TrackerInlined() noexcept = default;

	Rule5TrackerInlined(const Rule5TrackerInlined& rhs) noexcept { core = rhs.core; core.cntCopyConstruct += 1; }
	Rule5TrackerInlined& operator=(const Rule5TrackerInlined& rhs) noexcept { core = rhs.core; core.cntCopyAssignment += 1; return *this; }

	Rule5TrackerInlined(Rule5TrackerInlined&& rhs) noexcept { core = rhs.core; core.cntMoveConstruct += 1; }
	Rule5TrackerInlined& operator=(Rule5TrackerInlined&& rhs) noexcept { core = rhs.core; core.cntMoveAssignment += 1; return *this; }

	~Rule5TrackerInlined() noexcept { core.cntDestructor += 1; }
};

struct Rule5TrackerInlinedTriviallyReallocatable;

namespace Alt::TypeTraits {

template <>
struct __is_trivially_reallocatable__<Rule5TrackerInlinedTriviallyReallocatable>
	: std::true_type
{};

}

struct Rule5TrackerInlinedTriviallyReallocatable {

	struct Core {

		usize cntDefaultConstruct = 0;

		usize cntCopyConstruct = 0;
		usize cntMoveConstruct = 0;

		usize cntCopyAssignment = 0;
		usize cntMoveAssignment = 0;

		usize cntDestructor = 0;
	};

	Core core;

	Rule5TrackerInlinedTriviallyReallocatable() noexcept = default;

	Rule5TrackerInlinedTriviallyReallocatable(const Rule5TrackerInlinedTriviallyReallocatable& rhs) noexcept { core = rhs.core; core.cntCopyConstruct += 1; }
	Rule5TrackerInlinedTriviallyReallocatable& operator=(const Rule5TrackerInlinedTriviallyReallocatable& rhs) noexcept { core = rhs.core; core.cntCopyAssignment += 1; return *this; }

	Rule5TrackerInlinedTriviallyReallocatable(Rule5TrackerInlinedTriviallyReallocatable&& rhs) noexcept { core = rhs.core; core.cntMoveConstruct += 1; }
	Rule5TrackerInlinedTriviallyReallocatable& operator=(Rule5TrackerInlinedTriviallyReallocatable&& rhs) noexcept { core = rhs.core; core.cntMoveAssignment += 1; return *this; }

	~Rule5TrackerInlinedTriviallyReallocatable() noexcept { core.cntDestructor += 1; }
};
