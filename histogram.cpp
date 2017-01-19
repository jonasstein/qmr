#include "js_debug.cpp"
#include <iostream>     // std::cout
#include "histogram.hpp"
#include <cstdint>
#include <stdio.h>
//#define NDEBUG  // at the beginning of the code, before the inclusion of <assert.h> will disable all asserts
#include <assert.h>


histogram::histogram(const uint64_t setmaxbuckets, const uint64_t setbinwidth)
{
  maxbuckets = setmaxbuckets;
  binwidth = setbinwidth;
  period_ns = binwidth * maxbuckets;
}

histogram::~histogram()
{}

void histogram::put(uint64_t& value)
{
  uint64_t timeOnHistoScale;
  uint64_t index = 0;
  timeOnHistoScale = value % (maxbuckets*binwidth);
  // std::cout << timeOnHistoScale << " " << value << " " << maxbuckets << " " << binwidth << std::endl; 
  index = (uint64_t) ((timeOnHistoScale * maxbuckets) / period_ns);
  buckets[index]++;
}

void histogram::reset()
{
  for (uint64_t n=0; n< maxbuckets; n++){
    buckets[n]=0;
   }
}

void histogram::print()
{
  //std::cout << "# maxbuckets: " << maxbuckets << std::endl;
  //std::cout << "# period_ns: " << period_ns << std::endl;
  //for( uint16_t a = 0; a < maxbuckets; a++ )
  // {std::cout << a << ", ";}
  //std::cout << std::endl;

  for( uint64_t a = 0; a < maxbuckets; a++ )
  {std::cout << a * binwidth << ", " << buckets[a] << ", " << std::endl;}

  std::cout << std::endl;
}
