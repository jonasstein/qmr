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
#include <limits>
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
  
  if (argc != 8)
  {
    std::cout << "Error wrong number of arguments. Stopped." << std::endl;
    printhelp();
    exit(3);
  }
  
  // read parameter  
  std::string ArgThisProgram(argv[0]);
   int64_t ArgChDet= atoi(argv[1]);
  int64_t ArgChSync=atoi(argv[2]);
  int64_t ArgChSuper=atoi(argv[3]);
  int64_t ArgChMonitor=atoi(argv[4]);
  std::string ArgFilename(argv[5]);
  int64_t ArgSTARTts=atoi(argv[6]);
  int64_t ArgSTOPts=atoi(argv[7]);
  
  std::cerr << "Read file " << ArgFilename << std::endl;  
  std::cerr << "Evaluate between min and max timestamps: (-1 no limits)" << std::endl; 
  std::cerr << ArgSTARTts << std::endl; 
  std::cerr << ArgSTOPts << std::endl; 
  std::cerr << "ChDet:" << ArgChDet << " ChSync:" << ArgChSync << " ChSuper:" << ArgChSuper << " ChMonitor:" << ArgChMonitor << std::endl;
  
  if (ArgSTOPts==-1){
    ArgSTOPts = std::numeric_limits<int64_t>::max();
  }
  
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
  
  int64_t STARTts;
  int64_t SYNCts;
  int64_t SUPERts;
  int64_t CURRENTts;
  
  
  int64_t SYNCtsSUM=0;
  int64_t SYNCtsQty=0;
  int64_t SYNCtsMEAN=0;
  int64_t LastSYNCts=0;
  int64_t MindSYNCts=std::numeric_limits<int64_t>::max();
  int64_t MaxdSYNCts=0;
  
  int64_t TrigID; 
  int64_t DataID;
  int64_t Data;
  
  
  // calculate mean time between SYNC 
  while (!ifs.eof()){
    ifs >> CURRENTts >> TrigID >> DataID >> Data;

    if ((TrigID==7)&&(DataID==ArgChSync)){ //found a SYNC event
        if (LastSYNCts>0){
          std::cout<< CURRENTts - LastSYNCts << std::endl;
        MaxdSYNCts = std::max(MaxdSYNCts, CURRENTts - LastSYNCts);
        MindSYNCts = std::max(MindSYNCts, CURRENTts - LastSYNCts);
        }
        SYNCtsSUM+=CURRENTts;
        SYNCtsQty++;
        LastSYNCts=CURRENTts;
//        std::cout << ifs.tellg() << std::endl;
    }
  }
  
  if (SYNCtsQty==0){
  std::cout << "Zero SYNC signals found!" << std::endl;  
  }
  else{
    SYNCtsMEAN = SYNCtsSUM/SYNCtsQty;
  std::cout << "# avg SYNC period: " << SYNCtsMEAN << " ns = " << (float)(SYNCtsSUM/SYNCtsQty)/1000000 << " ms " << std::endl;
  std::cout << "# min SYNC period: " << MindSYNCts << " ns = " << (float)(MindSYNCts)/1000000 << " ms " << std::endl;
  std::cout << "# max SYNC period: " << MaxdSYNCts << " ns = " << (float)(MaxdSYNCts)/1000000 << " ms " << std::endl;
  
  }
  
  
  ifs.close();
  
  return(EXIT_SUCCESS);
}
