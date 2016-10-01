#include <cstdint>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
#include <byteswap.h> // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
#include "lmfile.hpp"
#include "histogram.hpp"
#include <assert.h>     /* assert */
#define NDEBUG 

#define _DEBUG
#ifdef _DEBUG
#define SHOW(a) std::cout << #a << ": 0x" << std::hex << (a) << std::endl
#else
#define SHOW(a) 
#endif

#include <sys/stat.h> // because boost::filesystem is fragile between different systems, gcc and boost versions

// remove boost until I know how to make the code more portable

// true if file exists
bool fileExists(const std::string& file) {
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}

void printhelp(){
  std::cout << "Usage:" << std::endl;
  std::cout << "Usage: qmr <det> <sync> <super> <mon> <verbosity> <mode>" << std::endl;
}

int main(int argc, char *argv[]){

  uint8_t ArgChDet;
  uint8_t ArgChSync;
  uint8_t ArgChSuper;
  uint8_t ArgChMon;
  uint8_t ArgVerbosity;
  uint8_t ArgMode;
  std::string  ArgFilename;

  
  if (argc == 7)
  {
    std::istringstream iss(arg[i]);
    iss >> ArgChDet;  
  }
    
  

  int verbosity = 0;


  
  std::cout << "# filename: '" << filename << std::endl;
  
  if (vm.count("help")) {
    std::cout << "Usage: " << argv[0] << " [options] " << std::endl;
    std::cout << desc << std::endl;
    return 0;
}
    
  if (vm.count("filename") > 0)
  {
  if(!fileExists(filename))
      {
       std::cout << "File not found: " << filename << std::endl;
       return(EXIT_FAILURE);
      }
    else {
      lmfile* limo;
      limo = new lmfile(filename);
 
      limo->setverbositylevel(verbosity);


      std::cout << "# size (Bytes): " << limo->getfilesize() << std::endl ; 
      limo->convertlistmodefile();
      limo->sortEventlist();
      //limo->el_printstatus();
      std::cout << "# Number of Events: " << limo ->getNumberOfEvents() << std::endl; 
      
      if (vm.count("histo")) {
      limo->el_printhistogram();
      }
      //limo->el_printallevents();

      delete(limo);
      return(EXIT_SUCCESS);
      }
  }
}

