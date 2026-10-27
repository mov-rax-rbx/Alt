#include "pch.h"

int main(int argc, char** argv) {

	::testing::InitGoogleTest(&argc, argv);

	::testing::GTEST_FLAG(filter) = "BoundedMPMC.*";

	return RUN_ALL_TESTS();
}