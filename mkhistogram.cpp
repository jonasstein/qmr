//#include "js_debug.cpp"
#include <cstdint>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
#include <assert.h>     /* assert */
#include <sys/stat.h> // because boost::filesystem is fragile between different systems, gcc and boost versions
#include "histogram.hpp"

// true if file exists
bool fileExists(const std::string& file) {
  struct stat buf;
  return (stat(file.c_str(), &buf) == 0);
}

void printhelp(){
  std::cout << "Usage: mkhistogram <ChDet> <ChSync> <ChSuper> <ChMonitor> <filename> <bins> <mode>" << std::endl;
}

int main(int argc, char *argv[]){
  
  if (argc != 8)
  {
    std::cout << "Error wrong number of arguments. Stopped." << std::endl;
    printhelp();
    exit(3);
  }
  
  const long INFOMODE=1;
  const long HISTOGRAMMODE=2;
  const long MAX64INT = 9223372036854775807;
  
  // read parameter  
  std::string ArgThisProgram(argv[0]);
  long ArgChDet= atol(argv[1]);
  long ArgChSync=atol(argv[2]);
  long ArgChSuper=atol(argv[3]);
  long ArgChMonitor=atol(argv[4]);
  std::string ArgFilename(argv[5]);
  long ArgBins=atol(argv[6]);
  long ArgMode=atol(argv[7]); // 1=get info about periods, 2=generate histogram
  
  std::cerr << "Read file " << ArgFilename << std::endl;  
  std::cerr << "Generate histograms with " << ArgBins << " bins" << std::endl;  
  std::cerr << "ChDet:" << ArgChDet << " ChSync:" << ArgChSync << " ChSuper:" << ArgChSuper << " ChMonitor:" << ArgChMonitor << std::endl;
  
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
  
  long StartOffsetts=0;
  long CURRENTts;
  uint64_t uCURRENTts;
  long SYNCtsSUM=0;
  long SYNCtsQty=0;
  long SYNCtsMEAN=0;
  long LastSYNCts=0;
  long MindSYNCts=MAX64INT;
  long MaxdSYNCts=0;
  
  long TrigID; 
  long DataID;
  long Data;
  
  
  // calculate mean time between SYNC 
  while (!ifs.eof()){
    ifs >> CURRENTts >> TrigID >> DataID >> Data;
      if (StartOffsetts==0){StartOffsetts=CURRENTts;}
      CURRENTts-=StartOffsetts;
      
      if ((TrigID==7)&&(DataID==ArgChSync)){ //found a SYNC event
        if (LastSYNCts>0){
          if (ArgMode==INFOMODE){
            std::cout<< CURRENTts - LastSYNCts << " ns period between " << LastSYNCts << " ns and " << CURRENTts << " ns"<< std::endl;
            MindSYNCts = std::min(MindSYNCts, CURRENTts-LastSYNCts);
            MaxdSYNCts = std::max(MaxdSYNCts, CURRENTts-LastSYNCts);
          }
        }
        SYNCtsSUM+=(CURRENTts-LastSYNCts);
        
        SYNCtsQty++;
        LastSYNCts=CURRENTts;
      }
    }
    
        
    if (SYNCtsQty==0){
      std::cerr << "Zero SYNC signals found on channel " << ArgChSync << " !" << std::endl;  
    }
    else{
      SYNCtsMEAN = SYNCtsSUM/SYNCtsQty;
      if (ArgMode==INFOMODE){
        std::cout << "# Start offset ts: " << StartOffsetts  << std::endl;
        std::cout << "# SYNC event found: " << SYNCtsQty  << std::endl;
        std::cout << "# avg SYNC period: " << SYNCtsMEAN << " ns = " << (float)(SYNCtsMEAN)/1000000 << " ms " << std::endl;
        std::cout << "# min SYNC period: " << MindSYNCts << " ns = " << (float)(MindSYNCts)/1000000 << " ms " << std::endl;
        std::cout << "# max SYNC period: " << MaxdSYNCts << " ns = " << (float)(MaxdSYNCts)/1000000 << " ms " << std::endl;
      }
    }
    
    if (ArgMode==HISTOGRAMMODE){
      ifs.seekg (0, ifs.beg); // go to file start again
      
      LastSYNCts = 0; //set time t0
      
      histogram* histo;
      histo = new histogram(ArgBins, SYNCtsMEAN);
      
      while (!ifs.eof()){
        ifs >> CURRENTts >> TrigID >> DataID >> Data;
        CURRENTts-=StartOffsetts;
        
          if ((TrigID==7)&&(DataID==ArgChDet)){ //found a detector event
            uCURRENTts = CURRENTts-LastSYNCts;
            std::cout << CURRENTts << std::endl;
            histo-> put(uCURRENTts); 
          }
          
          if ((TrigID==7)&&(DataID==ArgChSync)){ //found a sync event
            LastSYNCts=CURRENTts;        
          }
          
          if ((TrigID==7)&&(DataID==ArgChSuper)){ //found a super event (new histogram/new scan)
            histo-> print();
            histo-> reset();
          }
      }
      
      histo-> print();
      delete(histo); 
    }
    
    ifs.close();
    return(EXIT_SUCCESS);
}
