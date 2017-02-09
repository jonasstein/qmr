#include <cstdint>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <assert.h>     /* assert */

void printhelp(){
std::cout << "Usage: slicemdat <filename> <ID> <FROM m> <TO n>" << std::endl;
std::cout << "Returns the slice between m-th to n-th occurence of ID" 
}

int main(int argc, char *argv[]){
 
if (argc != 6)
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
  long long ArgMode=atol(argv[5]); 

  long long TrigID; 
  long long DataID;
  long long Data;
  long long CURRENTts;
  long long STARTTIMEts = 0;
  long long OUTSIDEMode = 1;
  long long INSIDEMode = 2;
  
  bool ItsInside = false;
  
  std::cerr << "Read file " << ArgFilename << std::endl;  
  std::cerr << "Filter events after ID=" << ArgID << " events" << std::endl;  
  std::cerr << "from:" << ArgFrom << " ns till " << ArgTill << " ns" << std::endl;
  
  std::ifstream ifs;
  ifs.open(ArgFilename, std::ifstream::in);
  
  if(!ifs)         // file couldn't be opened
  {
    std::cerr << "Error: Could not open file." << std::endl;
    exit(1);
  }

  while (!ifs.eof()){
    ifs >> CURRENTts >> TrigID >> DataID >> Data;
    if (ArgID==DataID)
    {
      STARTTIMEts = CURRENTts;
    }
    else
    {
      CURRENTts = CURRENTts - STARTTIMEts;
    }

    ItsInside = (CURRENTts >= ArgFrom) && (CURRENTts <= ArgTill);
    
    if ((ArgMode==OUTSIDEMode)&& (~ItsInside))
    { 
      std::printf("%lld %lld %lld %lld\n", CURRENTts  , TrigID, DataID, Data);
    } 
    else if ((ArgMode==INSIDEMode)&& ItsInside){ 
      std::printf("%lld %lld %lld %lld\n", CURRENTts  , TrigID, DataID, Data);
    }
  }
  ifs.close();
  return(EXIT_SUCCESS);  
}
