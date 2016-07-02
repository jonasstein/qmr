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

struct datablock{
     uint16_t bufferlength;
     uint16_t buffertype;
     uint16_t headerlength; // number of 16 bit words in the list mode file header
     uint16_t buffernumber = 65535;
     uint16_t runid;
     uint8_t mcpdid;
     bool daqrunning;
     bool syncok;
     uint64_t header_timestamp=0;
   };

   
  const uint64_t headersignature     = 0x00005555AAAAFFFF;
  const uint64_t datablocksignature  = 0x0000FFFF5555AAAA;
  const uint64_t filesignature       = 0xFFFFAAAA55550000;
  
const uint64_t MAX_EVENTS = 100000000; // 9 Byte per event

typedef uint64_t ufilesize_t;
 

class lmfile
{
  private:
    std::ifstream ifs;
    ufilesize_t filesize;
    ufilesize_t pos_dataheader;
    eventtime_t firsttimestamp;
    datablock dblock; 
       
    //event list items
    eventtime_t el_times[MAX_EVENTS];
    char el_IDbyte[MAX_EVENTS]; //ID (1 bit) = 1 TrigID (3 bit) DataID (4)
    uint64_t el_lastevent;

    const char IDmon1 = 0b11110000;
    const char IDmon2 = 0b11110001;
    const char IDmon3 = 0b11110010;
    const char IDmon4 = 0b11110011;
    
   /* uint64_t file_last_position_after_signature; // points to first char behind the last signature
    uint16_t file_last_signature_type; // 1=header, 2=databuffer, 3= eofsig, -1=else
   */ 
   
  public:
    lmfile( const char* mypath );
    ~lmfile();
    float timestamptomilliseconds(eventtime_t& ts, eventtime_t& offset);
    uint16_t readWord();
    uint64_t read64bit();
    void parsedatablock();
    ufilesize_t showfilesize();
    void parseheader();
    bool EOFahead();
    
    
    void el_addevent(eventtime_t& mytime, uint8_t& mysource);
    void el_printallevents();
    void printhistogram();
};

// This is the end of the header guard
#endif