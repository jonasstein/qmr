#include "js_debug.cpp"
#include <cstdint>
#include "lmfile.hpp"
#include <inttypes.h>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
//#include <stdint.h>
#include <byteswap.h> // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
#include <iomanip>
#include <math.h>       /* pow */
//#define NDEBUG  // at the beginning of the code, before the inclusion of <assert.h> will disable all asserts
#include <assert.h>

#include <bitset>



lmfile::lmfile(char const * mypath) : ifs ( mypath, std::ifstream::ate | std::ifstream::binary ), filesize ( 0 ), el_lastevent ( 0 )
{
   // "ate" placed cursor to EOF, we can read out the filesize now and go to start.
   filesize = ifs.tellg();
   ifs.seekg (0, ifs.beg);
}

lmfile::~lmfile()
{
  std::cout << "DESTROY! \n";
}

u_int16_t lmfile::readWord ( )
{
  u_int16_t sequenceRAW;
  ifs.read ( reinterpret_cast<char *> ( &sequenceRAW ),2 );
  // nb: little endian!
  
  return __builtin_bswap16 ( sequenceRAW ); 
}


u_int64_t lmfile::read64bit ( )
{
  u_int64_t sequenceRAW = 0;
  ifs.read ( reinterpret_cast<char *> ( &sequenceRAW ),8 );
// nb: little endian!
  return __builtin_bswap64 ( sequenceRAW );
}


u_filesize_t lmfile::showfilesize()
{
  return lmfile::filesize;
}

void lmfile::parsedatablock()
{
  u_filesize_t startposition = ifs.tellg();
  SHOWdec(startposition);
  
  dblock.bufferlength = lmfile::readWord();
  dblock.buffertype = lmfile::readWord();
  dblock.headerlength = lmfile::readWord();
  
  u_int16_t oldbuffernumber = dblock.buffernumber;
  dblock.buffernumber = lmfile::readWord();
  assert(oldbuffernumber < dblock.buffernumber); // buffernumber should increase!
  
  dblock.runid = lmfile::readWord();
  
  u_int16_t wordRAW = lmfile::readWord(); // mcpdid + status(DAQ running, sync OK) in one word
  dblock.mcpdid = wordRAW >> 8;
  dblock.daqrunning = wordRAW & 0b00000001;  //should be 1 usually
  dblock.syncok = wordRAW & 0b00000010; //should be 0 usually, we do not use a sync line
  
  // read header time stamp 
  u_int64_t wordRAWLo = lmfile::readWord(); 
  u_int64_t wordRAWMi = lmfile::readWord();
  u_int64_t wordRAWHi = lmfile::readWord();
  // FIXME: (cosmetic and to learn style) this is ugly code in my eyes. I want to read in 16bit and merge it to 64 bit in one line.
  dblock.header_timestamp = (wordRAWHi << 32) + (wordRAWMi << 16) + (wordRAWLo << 0);

  
  //std::cout << dblock.syncok << std::endl;  
  
   
  float milliseconds =  0.0001 * static_cast<float>(dblock.header_timestamp - 431394235384);
  
  // std::cout << milliseconds << std::endl;

  // ignore parameter0 .. parameter3 forward 4*3*16 bits = 24 bytes
  ifs.seekg(+24, std::ios_base::cur);  
  u_int16_t eventsinthisbuffer = (dblock.bufferlength - 20)/3;
  
  //SHOWdec(eventsinthisbuffer);
  
  u_int64_t eventLo;
  u_int64_t eventMi;
  u_int64_t eventHi;
  u_int64_t eventRAW;
  u_int8_t eventtype;
  u_int32_t eventdata;
  u_int64_t eventtimestamp;
  
  for (int i = 0; i < eventsinthisbuffer; i++)
  {
  eventLo = lmfile::readWord(); 
  eventMi = lmfile::readWord();
  eventHi = lmfile::readWord();
  eventRAW = (eventHi << 32) + (eventMi << 16) + (eventLo << 0);
  
  
  eventtype = (eventRAW  >> 40);//& 0x0F ;
  eventtimestamp = eventRAW & ((0b1 << 20) - 1);
  
  std::bitset<48> b1(eventRAW);
  std::bitset<64> b2(eventtimestamp);
    
  float milliseconds =  0.0001 * static_cast<float>((dblock.header_timestamp + eventtimestamp) - 431394235384);
  
  std::cout << b1 << " : " << b2 << "==> " << milliseconds << std::endl;
  }
  
  //go to end of datablock
  
  ifs.seekg(startposition + (dblock.bufferlength * 2), std::ios_base::beg);
  SHOWdec(ifs.tellg());
  
  u_int64_t sequenceRAW = lmfile::read64bit ( );
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
  
  u_int64_t sequenceRAW = lmfile::read64bit ( );
  assert(sequenceRAW == headersignature);

  pos_dataheader = ifs.tellg();
  SHOWdec(pos_dataheader);
  
  dblock.buffernumber = 0;
  
}


bool lmfile::EOFahead()
{
  u_int64_t sequenceRAW = lmfile::read64bit ( );
  ifs.seekg(-8, std::ios_base::cur);

  return (sequenceRAW==filesignature);
}

float lmfile::timestamptomilliseconds(u_int64_t& ts)
{
  float milliseconds =  0.0001 * static_cast<float>(ts);
  return milliseconds;
}


void eventlist::el_addevent ( uint8_t mysource, eventtime_t mytime )
{
  assert((el_lastevent + 1) < MAX_EVENTS);
  el_sources[el_lastevent + 1] = mysource;
  el_times[el_lastevent + 1] = mytime;
  lastevent += 1;
}