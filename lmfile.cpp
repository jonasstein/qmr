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


lmfile::lmfile(char const * mypath) : filesize ( 0 )
{
  ifs ( mypath, std::ifstream::ate | std::ifstream::binary );
}

lmfile::~lmfile()
{
}


u_int64_t lmfile::getfilesize()
{
  return lmfile::filesize;
}


//explicit Parser(char const * path) : m_stream(path) {} 
//explicit Parser(std::string const & path) : Parser(path.c_str()) {} 






