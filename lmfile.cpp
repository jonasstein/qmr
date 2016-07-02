#include "js_debug.cpp"
#include <cstdint>
#include "lmfile.hpp"
#include <inttypes.h>
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

lmfile::lmfile(char const * mypath) : ifs ( mypath, std::ifstream::ate | std::ifstream::binary ), filesize ( 0 ),  firsttimestamp ( 0 ), el_lastevent ( 0 )
{
   // "ate" placed cursor to EOF, we can read out the filesize now and go to start.
   filesize = ifs.tellg();
   ifs.seekg (0, ifs.beg);
   
   
}

lmfile::~lmfile()
{
  std::cout << "DESTROY! \n";
}

double lmfile::timestamptomilliseconds(eventtime_t& ts, eventtime_t& offset)
{
  //float milliseconds =  0.0001 * static_cast<float>(ts - offset);  FIXME Do I really need static_cast here???
  double milliseconds= 0.0001 * static_cast<double>(ts - offset);
  
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


ufilesize_t lmfile::showfilesize()
{
  return lmfile::filesize;
}

void lmfile::parsedatablock()
{
  ufilesize_t startposition = ifs.tellg();
  //SHOWdec(startposition);
  
  dblock.bufferlength = lmfile::readWord();
  dblock.buffertype = lmfile::readWord();
  dblock.headerlength = lmfile::readWord();
  
  uint16_t oldbuffernumber = dblock.buffernumber;
  dblock.buffernumber = lmfile::readWord();
  //if (oldbuffernumber >= dblock.buffernumber)
  //{std::cout << "Error: Buffernumber not increasing" << std::endl; }
  if (oldbuffernumber != 65535){
    // buffernumber should increase!
    assert((oldbuffernumber +1 )== dblock.buffernumber);
   } 
  else{
    std::cout << "nocheck" << std::endl;
  }                                                       // Ignore the test for first dataset
  
  dblock.runid = lmfile::readWord();
  
  uint16_t wordRAW = lmfile::readWord(); // mcpdid + status(DAQ running, sync OK) in one word
  dblock.mcpdid = wordRAW >> 8;
  dblock.daqrunning = wordRAW & 0b00000001;  //should be 1 usually
  dblock.syncok = wordRAW & 0b00000010; //should be 0 usually, we do not use a sync line
  
  // read header time stamp 
  uint64_t wordRAWLo = lmfile::readWord(); 
  uint64_t wordRAWMi = lmfile::readWord();
  uint64_t wordRAWHi = lmfile::readWord();
  // FIXME: (cosmetic and to learn style) this is ugly code in my eyes. I want to read in 16bit and merge it to 64 bit in one line.
  dblock.header_timestamp = (wordRAWHi << 32) + (wordRAWMi << 16) + (wordRAWLo << 0);

   
  
  // ignore parameter0 .. parameter3 forward 4*3*16 bits = 24 bytes
  ifs.seekg(+24, std::ios_base::cur);  
  uint16_t eventsinthisbuffer = (dblock.bufferlength - 20)/3;
  
  //SHOWdec(eventsinthisbuffer);
  
  uint64_t eventLo;
  uint64_t eventMi;
  uint64_t eventHi;
  uint64_t eventRAW;
  char eventtype;
  //uint32_t eventdata; //Counter, Timer or ADC value not needed yet
  eventtime_t eventtimestamp;
  
  for (int i = 0; i < eventsinthisbuffer; i++)
  {
  eventLo = lmfile::readWord(); 
  eventMi = lmfile::readWord();
  eventHi = lmfile::readWord();
  eventRAW = (eventHi << 32) + (eventMi << 16) + (eventLo << 0);
  
  
  eventtype = (eventRAW  >> 40);
  eventtimestamp = eventRAW & ((0b1 << 20) - 1);
  
  std::bitset<48> b1(eventRAW);
  std::bitset<64> b2(eventtimestamp);
    
  
  if (firsttimestamp == 0) {
    firsttimestamp = dblock.header_timestamp + eventtimestamp; 
  }
  
  eventtime_t timeofthisevent = dblock.header_timestamp + eventtimestamp;
  
  float milliseconds = lmfile::timestamptomilliseconds(timeofthisevent, firsttimestamp);
  
  std::cout << milliseconds << std::endl;

  
  // std::cout << b1 << " : " << b2 << "==> " << milliseconds << std::endl;
  
  uint8_t testsource = eventtype;  
  lmfile::el_addevent(eventtimestamp, testsource);   //FIXME should be source
  
  }
  
  //go to end of datablock
  
  ifs.seekg(startposition + (dblock.bufferlength * 2), std::ios_base::beg);
  //SHOWdec(ifs.tellg());
  
  uint64_t sequenceRAW = lmfile::read64bit ( );
  assert(sequenceRAW == datablocksignature);
}


void lmfile::parseheader()
{
  // read first line and parse secondline with number of lines
  std::string thisline;
  std::getline(ifs, thisline); 
  assert(thisline == ("mesytec psd listmode data"));

  // read second line  
  std::getline(ifs, thisline ); // header length: nnnnn lines
  std::string ss = ": ";
  int posi = thisline.find(ss);
  std::string sustri  = thisline.substr(posi+1,posi+4); // TODO  + 4 => EOL
  dblock.headerlength = std::stoi (sustri,nullptr,10);
  assert(dblock.headerlength==2); // we do not know about files <> 2 header lines yet.
  
  uint64_t sequenceRAW = lmfile::read64bit ( );
  assert(sequenceRAW == headersignature);

  pos_dataheader = ifs.tellg();
  SHOWdec(pos_dataheader);
}


bool lmfile::EOFahead()
{
  uint64_t sequenceRAW = lmfile::read64bit ( );
  ifs.seekg(-8, std::ios_base::cur);

  return (sequenceRAW==filesignature);
}


void lmfile::el_addevent (eventtime_t& mytime, uint8_t& myIDbyte)
{
  assert((el_lastevent + 1) < MAX_EVENTS);
  el_IDbyte[el_lastevent + 1] = myIDbyte;
  el_times[el_lastevent + 1] = mytime;
  el_lastevent += 1;
  //SHOWdec(el_lastevent);
  
}

void lmfile::el_printallevents()
{
  for( uint64_t a = 0; a < el_lastevent; a = a + 1 )
   {
     uint16_t sourcebuffer = el_IDbyte[a]; 
     std::cout << sourcebuffer << " , " << el_times[a] << std::endl;
  }
}

void lmfile::printhistogram()
{
  histogram* histo;
  
  
  histo = new histogram(100, 100000);
  
  for( uint64_t a = 0; a < el_lastevent; a = a + 1 )
   {
     uint16_t sourcebuffer = el_IDbyte[a]; 
     
     if (sourcebuffer == IDmon1){
     histo->put(el_times[a]);  
     }
  }
  histo->print();
  
}