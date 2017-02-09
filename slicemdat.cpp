#include <cstdint>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <assert.h>     /* assert */

void printhelp(){
std::cout << "Usage: slicemdat <filename> <ID> <FROM m> <TO n>" << std::endl;
std::cout << "Returns the slice between m-th to n-th occurence of ID" << std::endl; 
}

int main(int argc, char *argv[]){
 
if (argc != 5)
  {
    std::cerr << "Error wrong number of arguments. Expected 4, got " << argc-1 << ". Stopped." << std::endl;
    printhelp();
    exit(3);
  }

  // read parameter  
  std::string ArgThisProgram(argv[0]);
  std::string ArgFilename(argv[1]);
  long long ArgID=atol(argv[2]);
  long long ArgFrom=atol(argv[3]);
  long long ArgTo=atol(argv[4]); 

  long long TrigID; 
  long long DataID;
  long long Data;
  long long CURRENTts;

  long long EventCounter = 0;
  
  bool ItsInside = false;
  
  std::cerr << "Read file " << ArgFilename << std::endl;  
  std::cerr << "Filter events after ID=" << ArgID << " events" << std::endl;  
  std::cerr << "from:" << ArgFrom << " .. " << ArgTo << std::endl;
  
  std::ifstream ifs;
  ifs.open(ArgFilename, std::ifstream::in);
  
  if(!ifs)         // file couldn't be opened
  {
    std::cerr << "Error: Could not open file." << std::endl;
    exit(1);
  }

  while (!ifs.eof()){
    ItsInside = false;
    ifs >> CURRENTts >> TrigID >> DataID >> Data;

    if (ArgID==DataID)
    {
	EventCounter++;
	if (ArgTo==EventCounter) //include end-marker
	{
	ItsInside = true;
	}
    }
    
    // ItsInside is already true, or make true if event is inside the markers
    ItsInside = ItsInside || ((ArgFrom <= EventCounter) && (EventCounter < ArgTo));

    if (ItsInside)
	{
	std::printf("%lld %lld %lld %lld\n", CURRENTts  , TrigID, DataID, Data);
	}
    }

  
  ifs.close();
  return(EXIT_SUCCESS);  
}
