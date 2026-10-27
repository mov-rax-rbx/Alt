#include "pch.h"

#include "../alt-std/src/Utils/NonCopyableMovable.h"

namespace {

	class Class0 {
	public:

		ALT_MAKE_CLASS_NON_COPYABLE(Class0);
		ALT_MAKE_CLASS_NON_MOVABLE(Class0);
	};
}

TEST(NonCopyableMovable, NonCopyableMovable) {

	EXPECT_FALSE(std::is_copy_assignable_v<Class0>);
	EXPECT_FALSE(std::is_copy_constructible_v<Class0>);

	EXPECT_FALSE(std::is_move_assignable_v<Class0>);
	EXPECT_FALSE(std::is_move_constructible_v<Class0>);
}
