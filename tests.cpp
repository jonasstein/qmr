#include "lmfile.hpp"
#include <gtest/gtest.h>
#include <cmath> 
#include <cstdint>
#include <string>     // std::string, std::stoull
#define eventtime_t uint64_t // FIXME this looks like bad code. It was defined in lmfile. I do not want to define types in a test file again...


const std::string testFileName = "samples/180sec_2chan_5kHz_pm_4kHz_FM_4Hz_Trigger1Hz.mdat";

// TEST(lmfile, ConstructorFromTestFile) {
//   lmfile* limo;
//   limo = new lmfile(testFileName);
//   EXPECT_EQ(5658470, limo->getfilesize()) << "wrong file size calculated!";
// 
//   EXPECT_EQ(2, limo->getfileHeaderLength()) << "wrong number of file header lines (mostly 2)!";
// //  EXPECT_EQ(900259, limo->getNumberOfEvents()) << "wrong number of events!";
//   delete(limo);
// }


TEST(timestamptomilliseconds, OffsetCalculation) { 
    eventtime_t truncatedtime_ns = 0;
    eventtime_t offsettime_ns = 0;
    EXPECT_EQ(0, lmfile::timestamptomilliseconds(truncatedtime_ns, offsettime_ns ));

    truncatedtime_ns = 3004000; // x ns / 1000 ns/us / 1000 us/ms 
    offsettime_ns = 1000000;
    EXPECT_EQ(2, lmfile::timestamptomilliseconds(truncatedtime_ns, offsettime_ns ));
}


TEST(parseEvent, parserTestZero) { 
  triggerevent resultevent;
  
  uint16_t eventLo = 0x0; 
  uint16_t eventMi = 0x0;
  uint16_t eventHi = 0x0;
  eventtime_t timezero = 0;
  
  resultevent = lmfile::parseEvent(eventLo, eventMi, eventHi, timezero);
  EXPECT_EQ(0, resultevent.EventTimestamp_ns);
}

TEST(parseEvent, parserTestRealEvent) { 
  triggerevent resultevent;
  
  uint16_t eventLo = 0xf8a3; 
  uint16_t eventMi = 0x1971;
  uint16_t eventHi = 0x6400;
  eventtime_t timezero = 0;
  
  resultevent = lmfile::parseEvent(eventLo, eventMi, eventHi, timezero);
//  EXPECT_EQ(431394235384, resultevent.EventTimestamp_ns);
}




int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
