//#include "js_debug.cpp"
#include <cstdint>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
#include <byteswap.h> // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
#include <inttypes.h>
#include "lmfile.hpp"
#include "histogram.hpp"
#include <assert.h>     /* assert */

#include <sys/stat.h> // because boost::filesystem is fragile between different systems, gcc and boost versions

// remove boost until I know how to make the code more portable

// true if file exists
bool fileExists(const std::string& file) {
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}

void printhelp(){
  std::cout << "Usage: dump <filename>" << std::endl;
}

int main(int argc, char *argv[]){

  if (argc == 2)
  {
  std::string ArgFilename(argv[1]);

  int verbosity = 3;

  if(!fileExists(ArgFilename))
      {
       std::cout << "File not found: " << ArgFilename << std::endl;
       return(EXIT_FAILURE);
      }
    else {
      lmfile* limo;
      limo = new lmfile(ArgFilename);

      limo->setverbositylevel(verbosity);

      std::cout << "# size (Bytes): " << limo->getfilesize() << std::endl ; 
      limo->convertlistmodefile();
      limo->sortEventlist();
      //limo->el_printstatus();
      std::cout << "# Number of Events: " << limo ->getNumberOfEvents() << std::endl; 

//      if (vm.count("histo")) {
//      limo->el_printhistogram();
//      }
      //limo->el_printallevents();

      delete(limo);
      return(EXIT_SUCCESS);
      }
  }

  else{
    std::cout << "Error wrong number of arguments. Stopped." << std::endl;
    printhelp();
  }
    
 

}

