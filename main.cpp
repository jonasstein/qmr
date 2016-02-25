#include <cstdint>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
#include <byteswap.h> // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
#include <inttypes.h>
#include "lmfile.hpp"

#include <assert.h>     /* assert */
#define NDEBUG 

#define _DEBUG
#ifdef _DEBUG
#define SHOW(a) std::cout << #a << ": 0x" << std::hex << (a) << std::endl
#else
#define SHOW(a) 
#endif

  

int main () {
 
  lmfile* limo;
  limo = new lmfile("180sec_2chan_5kHz_pm_4kHz_FM_4Hz_Trigger1Hz.mdat");
  
  std::cout << limo->showfilesize() << " Bytes" << std::endl ; 
  
  delete(limo);
  
  return 0;
}

