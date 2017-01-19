//#include "js_debug.cpp"
#include <cstdint>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
//#include <stdio.h>
#include <assert.h>     /* assert */


void printhelp(){
std::cout << "Usage: filter <filename> <ID> <from time (ns)> <till time (ns)>" << std::endl;
}

int main(int argc, char *argv[]){
 
  if (argc != 4)
  {
    std::cerr << "Error wrong number of arguments. Stopped." << std::endl;
    printhelp();
    exit(3);
  }
  
  // read parameter  
  std::string ArgThisProgram(argv[0]);
  std::string ArgFilename(argv[1]);
  long long ArgID=atol(argv[2]);
  long long ArgFrom=atol(argv[3]);
  long long ArgTill=atol(argv[4]); 
  
  std::cerr << "Read file " << ArgFilename << std::endl;  
  std::cerr << "Filter events after ID=" << ArgID << " events" << std::endl;  
  std::cerr << "from:" << ArgChDet << " ChSync:" << ArgChSync << " ChSuper:" << ArgChSuper << " ChMonitor:" << ArgChMonitor << std::endl;
  
  std::ifstream ifs;
  ifs.open(ArgFilename, std::ifstream::in);
  
  if(!ifs)         // file couldn't be opened
  {
    std::cerr << "Error: file could not be opened" << std::endl;
    exit(1);
  }