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
  
  //limo = new lmfile("/home/stein/my/prj/qmr/data/M215/M215_4000ms_5000mVIN_HiZ_monitor.mdat");
  //limo = new lmfile("samples/180sec_2chan_5kHz_pm_4kHz_FM_4Hz_Trigger1Hz.mdat");
  limo = new lmfile("samples/closed_shutter_8kv_16ms_on_sample_after_shutdown.mdat");
  
  std::cout << "File size: " << limo->getfilesize() << " Bytes" << std::endl ; 
  
  //limo->el_printallevents();
  std::cout << "\n\n Total events in this file: " << limo->getNumberOfEvents() << " " << std::endl ; 
  
  std::string userkey;
//  std::cout << "Press <Enter> to close and quit";
//  getline (std::cin, userkey);
  
  delete(limo);
  
  return 0;
}

