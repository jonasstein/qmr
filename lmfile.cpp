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
#include <vector>
#include <bitset>
#include <algorithm>
#include "histogram.hpp"

#include <gtest/gtest.h> // add google test 
#define eventtime_t uint64_t 

lmfile::lmfile(std::string const mypath) : ifs ( mypath, std::ifstream::ate | std::ifstream::binary ), filesize ( 0 ),  firsttimestamp_ns ( 0 )
{
  // "ate" placed cursor to EOF, we can read out the filesize now and go to start.
  filesize = ifs.tellg();
  assert(filesize > 0);
  ifs.seekg (0, ifs.beg);
}

lmfile::~lmfile()
{
  //std::cout << "closed file! \n";
}

void lmfile::convertlistmodefile()
{
  lmfile::parsefileheader();
  
  bool fileEOF=false;
  
  while (fileEOF == false)
  {
    lmfile::parsedatablock();
    fileEOF = lmfile::EOFahead();
  };
}

uint64_t lmfile::timestamptomilliseconds(eventtime_t& ts_ns, eventtime_t& offset_ns)
{
  // double milliseconds= 1e-6 * static_cast<double>(ts_ns - offset_ns); // old code
  uint64_t milliseconds= (ts_ns - offset_ns)/1e6; // make integer division to convert ns -> ms
  return milliseconds;
}

uint64_t lmfile::getIndexOfMinimum(uint64_t arrayOfint[], uint64_t size)
{
  uint64_t index = 0;
  for(uint64_t n = 1; n < size; n++)
  {
    if(arrayOfint[n] < arrayOfint[index])
      index = n;              
  }
  return index;
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
  return Eventlist.size(); 
}

triggerevent lmfile::parseEvent(uint16_t LoWord, uint16_t MiWord, uint16_t HiWord, eventtime_t header_timestamp_ns)
{
  //const uint64_t filterEventID     = 0b0000000000000000100000000000000000000000000000000000000000000000;
  const uint64_t filterEventTrigID = 0b0000000000000000011100000000000000000000000000000000000000000000;
  const uint64_t filterEventDataID = 0b0000000000000000000011110000000000000000000000000000000000000000;
  const uint64_t filterEventData   = 0b0000000000000000000000001111111111111111111110000000000000000000;
  const uint64_t filterEventTime   = 0b0000000000000000000000000000000000000000000001111111111111111111;
  
  triggerevent myEvent;
  uint64_t eventRAW = ((uint64_t)HiWord << 32) + ((uint64_t)MiWord << 16) + ((uint64_t)LoWord << 0);
  
  myEvent.TrigID = (eventRAW & filterEventTrigID) >> 44;
  myEvent.DataID = (eventRAW & filterEventDataID) >> 40;
  myEvent.Data = (eventRAW & filterEventData) >> 19;
  
  eventtime_t RAWtimestamp_ns  = (eventRAW & filterEventTime) * 100;
  
  myEvent.EventTimestamp_ns = header_timestamp_ns + RAWtimestamp_ns;
  return(myEvent);
}

void lmfile::DebugPrintFullEvent(triggerevent OneFullEvent, bool PrintOnlyHeader)
{
  if (PrintOnlyHeader==true){
    std::cout << "TrigID, DataID, Data, Time_ns" << std::endl;
  }
  else {
    std::printf("%d, %d, %u, %lu \n", OneFullEvent.TrigID, OneFullEvent.DataID, OneFullEvent.Data, 
                OneFullEvent.EventTimestamp_ns - FirstOffsetTimestamp_ns);
    //std::cout << std::dec << (uint16_t) OneFullEvent.TrigID;
    //std::cout << ", " << std::dec << (uint16_t) OneFullEvent.DataID;
    //std::cout << ", " << OneFullEvent.Data;
    //std::cout << ", " << OneFullEvent.EventTimestamp_ns - FirstOffsetTimestamp_ns << std::endl;
  }}
  
  void lmfile::DebugPrintDatablock(bool PrintOnlyHeader)
  {
    if (PrintOnlyHeader==true){
      std::cout << "Buffer #, pre#, length, type, headerlength_words, ts_ns, t0_ns" << std::endl;
    }
    else {
      std::cout << dblock.metaBuffernumber; 
      std::cout << ", " << dblock.metaPreviousBuffernumber;
      std::cout << ", " << dblock.metaBufferlength;
      std::cout << ", " << dblock.metaBuffertype;
      std::cout << ", " << dblock.metaHeaderlength;  
      std::cout << ", " << dblock.header_timestamp_ns;
      std::cout << ", " << FirstOffsetTimestamp_ns << std::endl;
    }
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
    
    bool PrintOnlyHeaderNow=false;
    if (verbosityLevel >= 2){DebugPrintDatablock(PrintOnlyHeaderNow);}
    
    for (int i = 0; i < eventsinthisbuffer; i++)
    {
      triggerevent thisEvent;
      
      uint16_t eventLo = lmfile::readWord(); 
      uint16_t eventMi = lmfile::readWord();
      uint16_t eventHi = lmfile::readWord();
      
      thisEvent = parseEvent(eventLo, eventMi, eventHi, dblock.header_timestamp_ns);
      
      pushEventToVector(thisEvent);
      
      bool PrintOnlyHeaderNow=false;
      if (verbosityLevel >= 1){DebugPrintFullEvent(thisEvent, PrintOnlyHeaderNow);}
    }
    
    //go to end of datablock -> TODO make function bool lmfile::isEndOfDatablock();
    
    ifs.seekg(startposition + (dblock.metaBufferlength * 2), std::ios_base::beg);
    uint64_t sequenceRAW = lmfile::read64bit ();
    assert(sequenceRAW == datablocksignature);
    dblock.metaPreviousBuffernumber = dblock.metaBuffernumber;
  }
  
  void lmfile::pushEventToVector(triggerevent thisevent)
  {
    ChSum[thisevent.DataID]++;
    
    if (thisevent.DataID == ChannelOfPeriod){
      if (PeriodTime_ns[0] == 0){PeriodTime_ns[0] = thisevent.EventTimestamp_ns;}
      else if (PeriodTime_ns[1] == 0){PeriodTime_ns[1] = thisevent.EventTimestamp_ns;}
    }
    
    Eventlist.push_back(thisevent);
  }
  
  void lmfile::sortEventlist()
  {
    std::sort(Eventlist.begin(), Eventlist.end());
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
    //assert((NumberOfEvents + 1) < MAX_EVENTS);
    //el_IDbyte[NumberOfEvents + 1] = myIDbyte;
    //el_times_ns[NumberOfEvents + 1] = mytime_ns;
    //NumberOfEvents += 1;
  }
  
  triggerevent lmfile::el_getnexttriggerevent(eventtime_t currenttime)
  {
    triggerevent t;
    t.Data=0;
    return (t);
  }
  
  void lmfile::el_printstatus()
  {
    for(int n=0; n<4; n++){
      std::cout << "Ch" << n << ": " << ChSum[n] << std::endl;  
    }
    
    std::cout << "Period (ns): " << PeriodTime_ns[1] - PeriodTime_ns[0] << std::endl;  
  }
  
  void lmfile::el_printallevents()
  {
    
    for (auto it = Eventlist.begin(); it!=Eventlist.end(); ++it) {
      std::cout << (*it).EventTimestamp_ns << std::endl; 
    }
  }
  
  void lmfile::el_printhistogram()
  {
    // should be sortet before
    const uint64_t numOfBins = 100;
    uint64_t   monitorSum=0;
    
    histogram* histo;
    histo = new histogram(numOfBins, (PeriodTime_ns[1] - PeriodTime_ns[0]));
    
    uint64_t aa=0;
    
    for (auto it = Eventlist.begin(); it!=Eventlist.end(); ++it) {
      if ((*it).DataID == ChannelOfDetector){
        aa = (*it).EventTimestamp_ns; 
        histo-> put(aa);
      }
      if ((*it).DataID == ChannelOfFlipper){
        // std::cout << "# Monitor Sum: " << monitorSum << std::endl;
        monitorSum = 0;
        histo-> print();
        histo-> reset();
      }
      if ((*it).DataID == ChannelOfMonitor){
        monitorSum++;  
      }
    }
    
    histo-> print();
    delete(histo); 
  }
  
void lmfile::setverbositylevel(uint8_t vlevel)
{
 verbosityLevel = vlevel; 
}

inline bool lmfile::operator<(const triggerevent& lhs, const triggerevent& rhs)
{
    return lhs.EventTimestamp_ns < rhs.EventTimestamp_ns;
}

