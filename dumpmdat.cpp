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
    std::stringstream msg;                                  // make cout thread save
    msg << "Usage: dump <filename>" << std::endl;  
    std::cout << msg.str();
  }

int main(int argc, char *argv[]){
    std::stringstream msgerr;                               // make cout thread save

    if (argc != 2)
    {
        msgerr << "Error wrong number of arguments. Expected 1, got " << argc-1 << ". Stopped." << std::endl;
        std::cerr  << msgerr.str();

        printhelp();
        exit(3);
    }
    else  {
        std::string ArgFilename(argv[1]);

        int verbosity = 1;

        if(!fileExists(ArgFilename))
        {
            msgerr << "File not found: " << ArgFilename << std::endl;
            std::cerr  << msgerr.str();   
            return(EXIT_FAILURE);
        }
        else {
            lmfile* limo;
            limo = new lmfile(ArgFilename);

            limo->setverbositylevel(verbosity);

            msgerr << "# size (Bytes): " << limo->getfilesize() << std::endl ; 
            limo->convertlistmodefile();
            //limo->sortEventlist();
            //limo->el_printstatus();
            std::cerr << "# Number of Events: " << limo ->getNumberOfEvents() << std::endl; 

            //      if (vm.count("histo")) {
            //      limo->el_printhistogram();
            //      }
            //limo->el_printallevents();

            delete(limo);
            return(EXIT_SUCCESS);
          }
      }

  }

