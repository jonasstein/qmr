#include "lmfile.cpp"
#include <gtest/gtest.h>
 
TEST(timestamptomilliseconds, PositiveNos) { 
    ASSERT_EQ(0, lmfile::timestamptomilliseconds(0, 0));
}
 
 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}