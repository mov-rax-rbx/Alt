#include "pch.h"

#include "../alt-std/src/Utils/ScopeDefer.h"

TEST(ScopeDefer, ScopeDefer) {

	bool isDeferred = false;

	{
		defer {
			isDeferred = true;
		};
	}

	EXPECT_TRUE(isDeferred);
}

TEST(ScopeDefer, ScopeDeferDisable) {

	bool isDeferred = false;

	{
		auto cleanup = defer_disable {
			isDeferred = true;
		};

		cleanup.Disable();
	}

	EXPECT_FALSE(isDeferred);
}
