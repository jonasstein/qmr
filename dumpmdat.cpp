//#include "js_debug.cpp"
#include <cstdint>
#include <istream>
#include <iostream>                                         // std::cout
#include <fstream>                                          // std::ifstream
#include <string>                                           // std::string, std::stoull
#include <stdio.h>
#include <byteswap.h>                                       // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
#include <inttypes.h>
#include "lmfile.hpp"
#include <assert.h>     /* assert */
#include <sstream>      // std::stringstream
#include <sys/stat.h>                                       // because boost::filesystem is fragile between different systems, gcc and boost versions

// remove boost until I know how to make the code more portable

// true if file exists
bool fileExists(const std::string& file) {
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
  }

void printhelp(){
    fprintf(stdout, "Usage: dumpmdat <filename> \n");
    fprintf(stdout, "\ndumpmdat was compiled at %s on %s \n", __TIME__, __DATE__);
  }

int main(int argc, char *argv[]){
    std::stringstream msgerr;                               // make cout thread save

    if (argc != 2)
    {
        fprintf(stderr, "Error wrong number of arguments. Expected 1, got %d. Stopped\n", argc-1);
        printhelp();
        exit(3);
    }
    else  {
        std::string ArgFilename(argv[1]);
        int verbosity = 1;

        if(!fileExists(ArgFilename))
        {
            fprintf(stderr, "Error: File not found: %s \n", ArgFilename.c_str());
            return(EXIT_FAILURE);
        }
        else {
            lmfile* limo;
            limo = new lmfile(ArgFilename);
            limo->setverbositylevel(verbosity);

            msgerr << "# size (Bytes): " << limo->getfilesize() << std::endl ; 
            limo->convertlistmodefile();
            std::cerr << "# Number of Events: " << limo->getNumberOfEvents() << std::endl; 

            delete(limo);
            return(EXIT_SUCCESS);
          }
      }
  }

