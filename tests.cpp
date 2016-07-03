#include "lmfile.hpp"
#include <gtest/gtest.h>
#include <cmath> 
#define eventtime_t uint64_t // FIXME this looks like bad code. It was defined in lmfile. I do not want to define types in a test file again...


const char testFileName[] = "180sec_2chan_5kHz_pm_4kHz_FM_4Hz_Trigger1Hz.mdat";

TEST(lmfile, ConstructorFromTestFile) {
  lmfile* limo;
  limo = new lmfile(testFileName);
  EXPECT_EQ(5658470, limo->getfilesize()) << "wrong file size calculated!";
  delete(limo);
}

TEST(timestamptomilliseconds, OffsetCalculation) { 
    eventtime_t truncatedtime_ns = 0;
    eventtime_t offsettime_ns = 0;
    ASSERT_DOUBLE_EQ(0, lmfile::timestamptomilliseconds(truncatedtime_ns, offsettime_ns ));

    truncatedtime_ns = 3004000; // x ns / 1000 ns/us / 1000 us/ms 
    offsettime_ns = 1000000;
    ASSERT_DOUBLE_EQ(2.004, lmfile::timestamptomilliseconds(truncatedtime_ns, offsettime_ns ));
}
 
 
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
