// This is start of the header guard and can be any unique name.  By convention, we use the name of the header file.
#ifndef LMFILE_H
#define LMFILE_H

#define eventtime_t uint64_t 

//this was 
//typedef uint64_t eventtime_t;  
// but had no time now to find out, how to fix problems with this. Perhaps try the new "using" technique?


#include <cstdint>
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull FIXME: Do I need this line *here*?
#include <vector>


struct datablock{
     uint16_t metaBufferlength;
     uint16_t metaBuffertype;
     uint16_t metaHeaderlength; // number of 16 bit words in the list mode file header
     uint16_t metaBuffernumber=0;
     uint16_t metaPreviousBuffernumber=0;
     uint16_t runid;
     uint8_t mcpdid;
     bool daqrunning;
     bool syncok;
     uint64_t header_timestamp_ns=0; 
     bool ThisIsTheFirstDatabufferEverRead = true;
   };

struct triggerevent{
  uint8_t TrigID = 0;
  uint8_t DataID = 0;
  uint32_t Data = 0; //Counter, Timer or ADC value of the identified data source 21 bit
  eventtime_t EventTimestamp_ns = 0;
};   

bool operator<(const triggerevent& a, const triggerevent& b)
{
    return a.EventTimestamp_ns < b.EventTimestamp_ns;
}


eventtime_t FirstOffsetTimestamp_ns = 0; 

const uint64_t headersignature     = 0x00005555AAAAFFFF;
  const uint64_t datablocksignature  = 0x0000FFFF5555AAAA;
  const uint64_t filesignature       = 0xFFFFAAAA55550000;
  
 
  
typedef uint64_t ufilesize_t;
typedef struct rawevent_t { char x[6]; } rawevent_t;

class lmfile
{
  private:
    std::ifstream ifs;  // FIXME: should I hand over a pointer in each function? Will this collide, if I create 2 instances?
    ufilesize_t filesize;
    ufilesize_t fileHeaderLength; // header length: nnnnn lines 
    ufilesize_t pos_dataheader;
    eventtime_t firsttimestamp_ns;
    datablock dblock; 
    
    std::vector<eventtime_t> Ch0;
    std::vector<eventtime_t> Ch1;
    std::vector<eventtime_t> Ch2;
    std::vector<eventtime_t> Ch3;

    std::vector<triggerevent> Eventlist;
    
    uint64_t ChSum[4] = {};
    
    //event list items
    //eventtime_t el_times_ns[MAX_EVENTS]; 
    
    const char IDmon1 = 0b11110000;
    const char IDmon2 = 0b11110001;
    const char IDmon3 = 0b11110010;
    const char IDmon4 = 0b11110011;
  
    const uint8_t ChannelOfDetector = 0;
    const uint8_t ChannelOfPeriod   = 1;
    const uint8_t ChannelOfFlipper  = 2;
    const uint8_t ChannelOfMonitor  = 3;
    
    bool beVerbose = false;
    
  eventtime_t PeriodTime_ns[2] = {}; 
    
   /* uint64_t file_last_position_after_signature; // points to first char behind the last signature
    uint16_t file_last_signature_type; // 1=header, 2=databuffer, 3= eofsig, -1=else
   */ 
   
  public:
    lmfile( const std::string mypath );
    ~lmfile();
    void convertlistmodefile();
    static uint64_t timestamptomilliseconds(eventtime_t& ts_ns, eventtime_t& offset_ns);
    static uint64_t getIndexOfMinimum(uint64_t arrayOfint[], uint64_t size);
    uint16_t readWord();
    uint16_t readWordNoSwap();
    uint64_t read64bit();
    eventtime_t readevent();
    static bool geteventID(uint64_t rawevent); // 0 = neutron, 1 = trigger
    void parsedatablock();
    ufilesize_t getfilesize();
    ufilesize_t getfileHeaderLength();
    ufilesize_t getNumberOfEvents();
    void parsefileheader();
    void pushEventToVector(triggerevent thisevent);
    void sortEventlist();
    bool EOFahead();
    
    static triggerevent parseEvent(uint16_t LoWord, uint16_t MiWord, uint16_t HiWord, eventtime_t header_timestamp_ns);
    void DebugPrintFullEvent(triggerevent OneFullEvent, bool PrintOnlyHeader);
    void DebugPrintDatablock(bool PrintOnlyHeader);
    void el_addevent(eventtime_t& mytime_ns, uint8_t& mysource);
    triggerevent el_getnexttriggerevent(eventtime_t currenttime);
    void el_printstatus();
    void el_printallevents();
    void el_printhistogram();
};

// This is the end of the header guard
#endif