// This is start of the header guard and can be any unique name.  By convention, we use the name of the header file.
#ifndef LMFILE_H
#define LMFILE_H

#include <cstdint>
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull FIXME: Do I need this line *here*?


struct datablock{
     u_int16_t bufferlength;
     u_int16_t buffertype;
     u_int16_t headerlength; // number of 16 bit words in the list mode file header
     u_int16_t buffernumber = 0;
     u_int16_t runid;
     u_int8_t mcpdid;
     bool daqrunning;
     bool syncok;
     u_int64_t header_timestamp=0;
   };

  const u_int64_t headersignature     = 0x00005555AAAAFFFF;
  const u_int64_t datablocksignature  = 0x0000FFFF5555AAAA;
  const u_int64_t filesignature       = 0xFFFFAAAA55550000;
   
typedef u_int64_t u_filesize_t;
typedef u_int64_t eventtime_t;


class lmfile
{
  private:
    std::ifstream ifs;
    //char myfilename[80];
    u_filesize_t filesize;
    u_filesize_t pos_dataheader;
    eventtime_t firsttimestamp;
    datablock dblock; 
   
    eventtime_t times[1000000];
    u_int8_t  sources[1000000];
    
   /* u_int64_t file_last_position_after_signature; // points to first char behind the last signature
    u_int16_t file_last_signature_type; // 1=header, 2=databuffer, 3= eofsig, -1=else
  
    u_int64_t events;     // number of events
    u_int64_t datablocks; // number of datablocks that have been imported
    u_int64_t timestampstart; // timestamp offset; when did the file start?
   */ 
   
  public:
    lmfile( const char* mypath );
    ~lmfile();
    u_int16_t readWord();
    u_int64_t read64bit();
    void parsedatablock();
    u_filesize_t showfilesize();
    void parseheader();
    bool EOFahead();
    float timestamptomilliseconds(u_int64_t& ts);
};

// This is the end of the header guard
#endif