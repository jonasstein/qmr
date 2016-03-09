// This is start of the header guard and can be any unique name.  By convention, we use the name of the header file.
#ifndef LMFILE_H
#define LMFILE_H

#include <cstdint>
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull FIXME: Do I need this line *here*?
#include "eventlist.hpp" // definition of events, eventlists 

struct datablock{
     uint16_t bufferlength;
     uint16_t buffertype;
     uint16_t headerlength; // number of 16 bit words in the list mode file header
     uint16_t buffernumber = 0;
     uint16_t runid;
     uint8_t mcpdid;
     bool daqrunning;
     bool syncok;
     uint64_t header_timestamp=0;
   };

  const uint64_t headersignature     = 0x00005555AAAAFFFF;
  const uint64_t datablocksignature  = 0x0000FFFF5555AAAA;
  const uint64_t filesignature       = 0xFFFFAAAA55550000;
  
const uint64_t MAX_EVENTS = 1000000;

typedef uint64_t eventtime_t;  
typedef uint64_t u_filesize_t;
// typedef uint64_t eventtime_t; // now in eventlist


class lmfile
{
  private:
    std::ifstream ifs;
    //char myfilename[80];
    u_filesize_t filesize;
    u_filesize_t pos_dataheader;
    eventtime_t firsttimestamp;
    datablock dblock; 
       
    //event list items
    eventtime_t el_times[MAX_EVENTS];
    uint8_t  el_sources[MAX_EVENTS];
    uint64_t el_lastevent;

    
   /* uint64_t file_last_position_after_signature; // points to first char behind the last signature
    uint16_t file_last_signature_type; // 1=header, 2=databuffer, 3= eofsig, -1=else
   */ 
   
  public:
    lmfile( const char* mypath );
    ~lmfile();
    uint16_t readWord();
    uint64_t read64bit();
    void parsedatablock();
    u_filesize_t showfilesize();
    void parseheader();
    bool EOFahead();
    float timestamptomilliseconds(uint64_t& ts);
    
    void el_addevent(uint8_t mysource, eventtime_t mytime);

};

// This is the end of the header guard
#endif