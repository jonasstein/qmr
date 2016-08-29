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

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

//http://stackoverflow.com/questions/15634114/cant-link-program-using-boost-filesystem

#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


int main(int argc, char *argv[]){

  namespace po = boost::program_options;
  po::options_description desc("Options");
  desc.add_options()
      ("help,h", "Generate this help message")
      ("dry-run,n", "do nothing")
      ("filename,f", po::value<std::string>()->default_value("/tmp/test.mdat"));
//             ("filename,f", po::value<std::string>()->default_value("/tmp/YOURFILENAME.mdat"));

  po::variables_map vm;
  po::store(po::parse_command_line(argc,argv,desc),vm);
  po::notify(vm);

  std::string filename;
  filename = boost::any_cast<std::string>(vm["filename"].value());

  std::cout << "# filename: '" << filename << std::endl;
  
  if (vm.count("help")) {
    std::cout << "Usage: " << argv[0] << " [options] " << std::endl;
    std::cout << desc << std::endl;
    return 0;
}
  
  if (vm.count("n") > 0){
   //just simulate 
  }
  
  
  if (vm.count("filename") > 0)
  {
    if ( !boost::filesystem::exists( "/home/stein/my/prj/qmr" ) ) //filename
      {
       std::cout << "File not found: " << filename << std::endl;
        //return(EXIT_FAILURE);
      }
    else {
  lmfile* limo;
  limo = new lmfile(filename);
  std::cout << "# size (Bytes): " << limo->getfilesize() << std::endl ; 
  limo->convertlistmodefile();
  limo->sortEventlist();
  //limo->el_printstatus();
  std::cout << limo ->getNumberOfEvents() << std::endl; 
  limo->el_printhistogram();
  //limo->el_printallevents();

  delete(limo);
  return(EXIT_SUCCESS);
  }
  }
  
  
}

