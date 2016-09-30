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
#define NDEBUG 

#define _DEBUG
#ifdef _DEBUG
#define SHOW(a) std::cout << #a << ": 0x" << std::hex << (a) << std::endl
#else
#define SHOW(a) 
#endif

// http://stackoverflow.com/questions/15634114/cant-link-program-using-boost-filesystem
//#include <boost/filesystem.hpp>
#include <sys/stat.h> // because boost::filesystem is fragile between different systems, gcc and boost versions

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

// true if file exists
bool fileExists(const std::string& file) {
    struct stat buf;
    return (stat(file.c_str(), &buf) == 0);
}


int main(int argc, char *argv[]){

  namespace po = boost::program_options;
  po::options_description desc("Options");
  desc.add_options()
      ("help,h", "generate this help message")
      ("histo,H", "generate histogram")
      ("verbose,v", po::value< int >()->implicit_value( 1 ), "enable verbosity (optionally specify level)\n"
        "0 - no extra details\n 1 - show events\n   - 2 show also data buffer"  )
      ("filename,f", po::value<std::string>()->default_value("/tmp/test.mdat"));

  po::variables_map vm;
  po::store(po::parse_command_line(argc,argv,desc),vm);
  po::notify(vm);

  int verbosity = 0;

if ( vm.count("verbose") ) {
    verbosity = vm[ "verbose" ].as< int >() ;
    std::cout << "Verbosity enabled.  Level is " << verbosity << std::endl;
   }

  std::string filename;
  filename = boost::any_cast<std::string>(vm["filename"].value());
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

