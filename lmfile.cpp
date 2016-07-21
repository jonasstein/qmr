#include "js_debug.cpp"
#include <cstdint>
#include "lmfile.hpp"
//#include <inttypes.h>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
#include <byteswap.h> // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
#include <iomanip>
#include <math.h>       /* pow */
//#define NDEBUG  // at the beginning of the code, before the inclusion of <assert.h> will disable all asserts
#include <assert.h>

#include <bitset>

#include "histogram.hpp"

#include <gtest/gtest.h> // add google test 
#define eventtime_t uint64_t 

lmfile::lmfile(char const * mypath) : ifs ( mypath, std::ifstream::ate | std::ifstream::binary ), filesize ( 0 ),  firsttimestamp_ns ( 0 ), NumberOfEvents ( 0 )
{
   // "ate" placed cursor to EOF, we can read out the filesize now and go to start.
   filesize = ifs.tellg();
   assert(filesize > 0);
   ifs.seekg (0, ifs.beg);
   
   lmfile::parsefileheader();
   
   bool fileEOF=false;
  
   while (fileEOF == false)
   {
   lmfile::parsedatablock();
   fileEOF = lmfile::EOFahead();
   };
}

lmfile::~lmfile()
{
  //std::cout << "closed file! \n";
}

uint64_t lmfile::timestamptomilliseconds(eventtime_t& ts_ns, eventtime_t& offset_ns)
{
  // double milliseconds= 1e-6 * static_cast<double>(ts_ns - offset_ns); // old code
  uint64_t milliseconds= (ts_ns - offset_ns)/1e6; // make integer division to convert ns -> ms
  return milliseconds;
}

uint16_t lmfile::readWord ( )
{
  uint16_t sequenceRAW;
  ifs.read ( reinterpret_cast<char *> ( &sequenceRAW ),2 );
  return __builtin_bswap16 ( sequenceRAW );   // nb: little endian!
}

uint16_t lmfile::readWordNoSwap ( )
{
  uint16_t sequenceRAW;
  ifs.read ( reinterpret_cast<char *> ( &sequenceRAW ),2 );
  return( sequenceRAW ); 
}

uint64_t lmfile::read64bit ( )
{
  uint64_t sequenceRAW = 0;
  ifs.read ( reinterpret_cast<char *> ( &sequenceRAW ),8 );
// nb: little endian!
  return __builtin_bswap64 ( sequenceRAW );
}


bool lmfile::geteventID(uint64_t rawevent)
{
  // 6 bytes = 48 bit in, first bit = ID
  bool theID;
  theID = (rawevent >> 47) & 0b1;
  return(theID);
};


ufilesize_t lmfile::getfilesize()
{
  return lmfile::filesize;
}

ufilesize_t lmfile::getfileHeaderLength()
{
  return lmfile::fileHeaderLength;
}

ufilesize_t lmfile::getNumberOfEvents()
{
  return lmfile::NumberOfEvents;
}

triggerevent lmfile::parseEvent(uint16_t LoWord, uint16_t MiWord, uint16_t HiWord, eventtime_t header_timestamp_ns)
{
  const uint64_t filterEventID     = 0b0000000000000000100000000000000000000000000000000000000000000000;
  const uint64_t filterEventTrigID = 0b0000000000000000011100000000000000000000000000000000000000000000;
  const uint64_t filterEventDataID = 0b0000000000000000000011110000000000000000000000000000000000000000;
  const uint64_t filterEventData   = 0b0000000000000000000000001111111111111111111110000000000000000000;
  const uint64_t filterEventTime   = 0b0000000000000000000000000000000000000000000001111111111111111111;
  
  triggerevent myEvent;
  uint64_t eventRAW = ((uint64_t)HiWord << 32) + ((uint64_t)MiWord << 16) + ((uint64_t)LoWord << 0);

//   std::bitset<64> LoAsBitset(LoWord); 
//   std::cout << "Lo    : " << LoAsBitset << std::endl;
// 
//   std::bitset<64> MiAsBitset(MiWord); 
//   std::cout << "Mi    : " << MiAsBitset << std::endl;
//   
//   std::bitset<64> HiAsBitset(HiWord); 
//   std::cout << "Hi    : " << HiAsBitset << std::endl;
//   
//   std::bitset<64> eventAsBitset(eventRAW); 
//   std::cout << "Bitset: " << eventAsBitset << std::endl;
//   
  
  myEvent.TrigID = (eventRAW & filterEventTrigID) >> 44;
  myEvent.DataID = (eventRAW & filterEventDataID) >> 40;
  myEvent.Data = (eventRAW & filterEventData) >> 19;
  
  eventtime_t RAWtimestamp_ns  = (eventRAW & filterEventData) * 100;
     
  myEvent.EventTimestamp_ns = header_timestamp_ns + RAWtimestamp_ns;
  return(myEvent);
}

void lmfile::DebugPrintFullEvent(triggerevent OneFullEvent)
{
std::cout << ">>> TrigID: " << std::dec << (uint16_t) OneFullEvent.TrigID;
std::cout << " DataID: " << std::dec << (uint16_t) OneFullEvent.DataID;
std::cout << " Data: " << OneFullEvent.Data;
std::cout << " Time: " << OneFullEvent.EventTimestamp_ns - FirstOffsetTimestamp_ns << std::endl;
}

void lmfile::DebugPrintDatablock()
{
  std::cout << "Buffer #" << dblock.metaBuffernumber; 
  std::cout << " pre#: " << dblock.metaPreviousBuffernumber;
  std::cout << " length: " << dblock.metaBufferlength << " 16 bit words";
  std::cout << " type: " << dblock.metaBuffertype;
  std::cout << " header length: " << dblock.metaHeaderlength;  
  std::cout << " ts_ns: " << dblock.header_timestamp_ns;
  std::cout << " t0_ns: " << FirstOffsetTimestamp_ns << std::endl;
}

void lmfile::parsedatablock()
{
  ufilesize_t startposition = ifs.tellg();
 
  dblock.metaBufferlength = lmfile::readWord();
  dblock.metaBuffertype = lmfile::readWord();
  dblock.metaHeaderlength = lmfile::readWord();
  dblock.metaBuffernumber = lmfile::readWord();
  
  dblock.runid = lmfile::readWord();
  
  uint16_t wordRAW = lmfile::readWord(); // mcpdid + status(DAQ running, sync OK) in one word
  dblock.mcpdid = wordRAW >> 8;
  dblock.daqrunning = wordRAW & 0b00000001;  //should be 1 usually
  dblock.syncok = wordRAW & 0b00000010; //should be 0 usually, we do not use a sync line
  
  // read header time stamp 
  uint64_t wordRAWLo = lmfile::readWord(); uint64_t wordRAWMi = lmfile::readWord(); uint64_t wordRAWHi = lmfile::readWord();
  // FIXME: (cosmetic and to learn style) this is ugly code in my eyes. I want to read in 16bit and merge it to 64 bit in one line.
  dblock.header_timestamp_ns = ((wordRAWHi << 32) + (wordRAWMi << 16) + (wordRAWLo << 0)) * 100;
  
  ifs.seekg(+24, std::ios_base::cur);  // ignore parameter0 .. parameter3 forward 4*3*16 bits = 24 bytes
  uint16_t eventsinthisbuffer = (dblock.metaBufferlength - 20)/3;

  // now test, if buffer numbers increase
  if (dblock.ThisIsTheFirstDatabufferEverRead  == true){
    FirstOffsetTimestamp_ns = dblock.header_timestamp_ns;
    dblock.ThisIsTheFirstDatabufferEverRead  = false;    
  }
  else{
    if (dblock.metaPreviousBuffernumber +1 != dblock.metaBuffernumber){
      std::cout<< "WWW: Missing Datablock between " << dblock.metaPreviousBuffernumber << " and " << dblock.metaBuffernumber << std::endl; 
    }
  }

  DebugPrintDatablock();
  
  for (int i = 0; i < eventsinthisbuffer; i++)
  {
  triggerevent thisEvent;
  
  uint16_t eventLo = lmfile::readWord(); 
  uint16_t eventMi = lmfile::readWord();
  uint16_t eventHi = lmfile::readWord();
  
  thisEvent = parseEvent(eventLo, eventMi, eventHi, dblock.header_timestamp_ns);
  DebugPrintFullEvent(thisEvent);  
  }

  //go to end of datablock -> TODO make function bool lmfile::isEndOfDatablock();
  
  ifs.seekg(startposition + (dblock.metaBufferlength * 2), std::ios_base::beg);
  uint64_t sequenceRAW = lmfile::read64bit ();
  assert(sequenceRAW == datablocksignature);
  dblock.metaPreviousBuffernumber = dblock.metaBuffernumber;
}

void lmfile::parsefileheader()
{
  // read first line and parse secondline with number of lines
  std::string thisline;
  std::getline(ifs, thisline); 
  assert(thisline == ("mesytec psd listmode data"));

  std::getline(ifs, thisline ); // header length: nnnnn lines
  int posi = thisline.find(": ");
  std::string sustri  = thisline.substr(posi+1,posi+4); // TODO  + 4 => EOL
  fileHeaderLength = std::stoi (sustri,nullptr,10);
  assert(fileHeaderLength==2); // we do not know about files <> 2 header lines yet. execute n-2 readlines here some day.
  
  uint64_t sequenceRAW = lmfile::read64bit ( );
  assert(sequenceRAW == headersignature);
}


bool lmfile::EOFahead()
{
  uint64_t sequenceRAW = lmfile::read64bit ( );
  ifs.seekg(-8, std::ios_base::cur);

  return (sequenceRAW==filesignature);
}


void lmfile::el_addevent (eventtime_t& mytime_ns, uint8_t& myIDbyte)
{
  assert((NumberOfEvents + 1) < MAX_EVENTS);
  el_IDbyte[NumberOfEvents + 1] = myIDbyte;
  el_times_ns[NumberOfEvents + 1] = mytime_ns;
  NumberOfEvents += 1;
}

void lmfile::el_printallevents()
{
  for( uint64_t a = 0; a < NumberOfEvents; a = a + 1 )
   {
     uint16_t sourcebuffer = el_IDbyte[a]; 
     // FIXME print more here + make headline
     // std::printf("%hu , %llu ns \n", (0xffff - sourcebuffer), el_times_ns[a]); // printf is much faster than cout here!
  }
}
