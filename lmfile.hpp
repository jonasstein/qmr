// This is start of the header guard and can be any unique name.  By convention, we use the name of the header file.
#ifndef LMFILE_H
#define LMFILE_H

#include <cstdint>
#include <fstream>      // std::ifstream

/*
struct datablock{
     u_int16_t bufferlength;
     u_int16_t buffertype;
     u_int16_t headerlength; // number of 16 bit words in the list mode file header
     u_int16_t buffernumber;
     u_int16_t runid;
     u_int8_t mcpdid;
     bool daqrunning;
     bool syncok;
     u_int64_t header_timestamp=0;
   };
*/

typedef u_int64_t u_filesize_t;

class lmfile
{
  private:
    std::ifstream ifs;
    //char myfilename[80];
    u_filesize_t filesize;

   /* u_int64_t file_last_position_after_signature; // points to first char behind the last signature
    u_int16_t file_last_signature_type; // 1=header, 2=databuffer, 3= eofsig, -1=else
    datablock db; 
    u_int64_t eventtime[1000000]; // TODO save eventtime and eventtype as two arrays time[] type[], or one array per type ch0[] ch1[] ch2[] ch3[]? 
    u_int64_t events;     // number of events
    u_int64_t datablocks; // number of datablocks that have been imported
    u_int64_t timestampstart; // timestamp offset; when did the file start?
   */ 
  public:
    lmfile( const char* mypath );
    ~lmfile();
    
  u_filesize_t showfilesize();
};

// This is the end of the header guard
#endif