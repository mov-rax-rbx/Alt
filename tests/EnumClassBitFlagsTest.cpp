#include "pch.h"

#include "../alt-std/src/Utils/EnumClassBitFlagsImpl.h"

namespace {

	struct FlagsStruct {

		enum class Flags {
			F1 = 1 << 0,
			F2 = 1 << 1,
			F3 = 1 << 2,
			F4 = 1 << 3,
			F5 = 1 << 4,
		};
		ALT_ENUM_CLASS_BIT_FLAGS_IN_CLASS_DECL(Flags);
	};
	ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FlagsStruct::Flags);

	enum class Flags {
		F1 = 1 << 0,
		F2 = 1 << 1,
		F3 = 1 << 2,
		F4 = 1 << 3,
		F5 = 1 << 4,
	};
	ALT_ENUM_CLASS_BIT_FLAGS_IMPL(Flags);
}

TEST(EnumClassBitFlags, EnumClassBitFlags) {


	Flags flags = Flags::F2 | Flags::F4;

	EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(flags, Flags::F1));
	EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(flags, Flags::F2));
	EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(flags, Flags::F4));

	flags &= Flags::F2;

	EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(flags, Flags::F2));
	EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(flags, Flags::F4));

	flags = ~flags;

	EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(flags, Flags::F2));
	EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(flags, Flags::F4));
	EXPECT_TRUE(Alt::BitFlag::IsSetAnyEnumFlags(flags, Flags::F2 | Flags::F4));
}

TEST(EnumClassBitFlags, EnumClassBitFlagsStruct) {

	FlagsStruct::Flags flags = FlagsStruct::Flags::F2 | FlagsStruct::Flags::F4;

	EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(flags, FlagsStruct::Flags::F1));
	EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(flags, FlagsStruct::Flags::F2));
	EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(flags, FlagsStruct::Flags::F4));

	flags &= FlagsStruct::Flags::F2;

	EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(flags, FlagsStruct::Flags::F2));
	EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(flags, FlagsStruct::Flags::F4));

	flags = ~flags;

	EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(flags, FlagsStruct::Flags::F2));
	EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(flags, FlagsStruct::Flags::F4));
	EXPECT_TRUE(Alt::BitFlag::IsSetAnyEnumFlags(flags, FlagsStruct::Flags::F2 | FlagsStruct::Flags::F4));
}
