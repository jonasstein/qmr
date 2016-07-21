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
  // nb: little endian!
  
  return __builtin_bswap16 ( sequenceRAW ); 
}


uint64_t lmfile::read64bit ( )
{
  uint64_t sequenceRAW = 0;
  ifs.read ( reinterpret_cast<char *> ( &sequenceRAW ),8 );
// nb: little endian!
  return __builtin_bswap64 ( sequenceRAW );
}

uint64_t lmfile::readevent()
{ // we read 48 bit and store it in one 64 bit variable.
  uint16_t eventLo = lmfile::readWord(); 
  uint16_t eventMi = lmfile::readWord();
  uint16_t eventHi = lmfile::readWord();
  uint16_t eventRAW = ((uint64_t)eventHi << 32) + ((uint64_t)eventMi << 16) + ((uint64_t)eventLo << 0);
  
  return(eventRAW); 
};

bool lmfile::geteventID(uint64_t rawevent)
{
  // 6 bytes = 48 bit in, first bit = ID
  bool theID;
  theID = (rawevent >> 47) & 0b1;
  return(theID);
};

uint64_t lmfile::geteventTIME(uint64_t rawevent)
{
  return(0);
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

void lmfile::DebugPrintDatablock()
{
  std::cout << "Buffer #" << dblock.metaBuffernumber; 
  std::cout << " length: " << dblock.metaBufferlength << " 16 bit words";
  std::cout << " type: " << dblock.metaBuffertype;
  std::cout << " header length: " << dblock.metaHeaderlength;  
  std::cout << " pre#: " << dblock.metaPreviousBuffernumber << std::endl;
}

void lmfile::parsedatablock()
{
  ufilesize_t startposition = ifs.tellg();
 
  dblock.metaBufferlength = lmfile::readWord();
  dblock.metaBuffertype = lmfile::readWord();
  dblock.metaHeaderlength = lmfile::readWord();
   
  // now test, if buffer numbers increase
  if (dblock.NoDatabufferParsedBefore == true){
    dblock.metaBuffernumber = lmfile::readWord();
    dblock.NoDatabufferParsedBefore = false;    
  }
  else{
    dblock.metaBuffernumber = lmfile::readWord();
    if (dblock.metaPreviousBuffernumber +1 != dblock.metaBuffernumber)
    {
      std::cout<< "WWW: Missing Datablock between " << dblock.metaPreviousBuffernumber << " and " << dblock.metaBuffernumber << std::endl; 
    }
  }
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

  eventtime_t eventRAW;
  char eventtype;
  //uint32_t eventdata; //Counter, Timer or ADC value not needed yet
  eventtime_t eventtimestamp_ns;
  
  DebugPrintDatablock();
  
  for (int i = 0; i < eventsinthisbuffer; i++)
  {

  uint64_t eventRAW = readevent();
  eventtype = (eventRAW  >> 40);
  eventtimestamp_ns = (eventRAW & ((0b1 << 20) - 1)) * 100;
  
  std::bitset<48> b1(eventRAW);
  std::bitset<64> b2(eventtimestamp_ns);
  
  if (firsttimestamp_ns == 0) {
    firsttimestamp_ns = dblock.header_timestamp_ns + eventtimestamp_ns; 
  }
  
  eventtime_t timeofthisevent_ns = dblock.header_timestamp_ns + eventtimestamp_ns;
  uint8_t testsource = eventtype;  
  lmfile::el_addevent(eventtimestamp_ns, testsource);   //FIXME should be source
  }

  //go to end of datablock
  
  ifs.seekg(startposition + (dblock.metaBufferlength * 2), std::ios_base::beg);
    
  uint64_t sequenceRAW = lmfile::read64bit ( );
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
     std::printf("%hu , %llu ns \n", (0xffff - sourcebuffer), el_times_ns[a]); // printf is much faster than cout here!
  }
}
