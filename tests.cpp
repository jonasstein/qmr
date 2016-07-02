#include "lmfile.hpp"
#include <gtest/gtest.h>

#define eventtime_t uint64_t // FIXME this looks like bad code. It was defined in lmfile. I do not want to define types in a test file again...
 
TEST(timestamptomilliseconds, PositiveNos) { 
  eventtime_t truncatedtime = 0;
  eventtime_t offsettime = 0;
  ASSERT_DOUBLE_EQ(0, lmfile::timestamptomilliseconds(truncatedtime, offsettime ));
}
 
 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
