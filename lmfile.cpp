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


#define _DEBUG
#ifdef _DEBUG
#define SHOW(a) std::cout << #a << ": 0x" << std::hex << (a) << std::endl
#else
#define SHOW(a)
#endif

#ifdef _DEBUG
#define SHOWdec(a) std::cout << #a << ": " << std::dec << (a) << std::endl
#else
#define SHOWdec(a)
#endif


lmfile::lmfile(char const * mypath) : ifs ( mypath, std::ifstream::ate | std::ifstream::binary ), filesize ( 0 )
{
   std::cout << "CREATE! \n";
   // "ate" placed cursor to EOF, we can read out the filesize now and go to start.
   filesize = ifs.tellg();
   ifs.seekg (0, ifs.beg);
}

lmfile::~lmfile()
{
  std::cout << "DESTROY! \n";
}


u_filesize_t lmfile::showfilesize()
{
  return lmfile::filesize;
}

int lmfile::showheaderlength()
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
  int header_number_of_ascii_lines = std::stoi (sustri,nullptr,10);
  assert(header_number_of_ascii_lines==2); // we do not know about files <> 2 header lines yet.
  
  pos_dataheader = ifs.tellg() + 8;
  return header_number_of_ascii_lines;
}




