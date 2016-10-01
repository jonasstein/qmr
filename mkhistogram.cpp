//#include "js_debug.cpp"
#include <cstdint>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
#include <byteswap.h> // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
#include "lmfile.hpp"
//#include "histogram.hpp"
#include <assert.h>     /* assert */

#include <sys/stat.h> // because boost::filesystem is fragile between different systems, gcc and boost versions

// true if file exists
bool fileExists(const std::string& file) {
  struct stat buf;
  return (stat(file.c_str(), &buf) == 0);
}

void printhelp(){
  std::cout << "Usage: mkhistogram <ChDet> <ChSync> <ChSuper> <ChMonitor> <filename>" << std::endl;
}

int main(int argc, char *argv[]){
  
  if (argc != 6)
  {
    std::cout << "Error wrong number of arguments. Stopped." << std::endl;
    printhelp();
    exit(3);
  }
  
  // read parameter  
  std::string ArgThisProgram(argv[0]);
  uint8_t ArgChDet=1;
  uint8_t ArgChSync=3;
  uint8_t ArgChSuper=3;
  uint8_t ArgChMonitor=4;
  
  std::cerr << " ChDet:" << (int)ArgChDet << " ChSync:" << (int)ArgChSync << " ChSuper:" << (int)ArgChSuper << " ChMonitor:" << (int)ArgChMonitor << std::endl;

  std::string ArgFilename(argv[5]);
  std::cout <<ArgFilename << std::endl;
  
  std::ifstream ifs;
  ifs.open(ArgFilename, std::ifstream::in);
  
  if(!ifs)         // file couldn't be opened
  {
    std::cerr << "Error: file could not be opened" << std::endl;
    exit(1);
  }
  
  
  // read .csv, read line, if not comment, 
  // if SYNC, then save new SYNC, inc SYNCcounter
  // calculate mean(SYNC(k))
  // go to start of file
  // read line, if not comment, 
  // if not SUPER read CURRENTts and printf 
  // if SUPER then histo.print(); histo.reset()
  
  uint64_t STARTts;
  uint64_t SYNCts;
  uint64_t SUPERts;
  uint64_t CURRENTts;
  
  
  uint64_t SYNCtsSUM=0;
  uint64_t SYNCtsQty=0;
  uint64_t SYNCtsMEAN=0;
  
  uint8_t TrigID; 
  uint8_t DataID;
  uint32_t Data;
  
  
  // calculate mean time between SYNC 
  while (!ifs.eof()){
    ifs >> CURRENTts >> TrigID >> DataID >> Data;
    
    std::cout << (int)DataID << std::endl;
    if ((TrigID==7)&&(DataID==3)){
        SYNCtsSUM+=CURRENTts;
        SYNCtsQty++;
        std::cout << ifs.tellg() << std::endl;
    }
  }
  
  if (SYNCtsQty==0){
  std::cout << "Zero SYNC signals found!" << std::endl;  
  }
  else{
  std::cout << SYNCtsSUM/SYNCtsQty << std::endl;
  }
  
  
  ifs.close();
  
  return(EXIT_SUCCESS);
}
