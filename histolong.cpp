#include "js_debug.cpp"
#include <iostream>     // std::cout
#include "histolong.hpp"
#include <cstdint>
#include <stdio.h>
//#define NDEBUG  // at the beginning of the code, before the inclusion of <assert.h> will disable all asserts
#include <assert.h>


histogram::histogram(const long long setmaxbuckets, const long long setbinwidth)
{
  maxbuckets = setmaxbuckets;
  binwidth = setbinwidth;
  fullperiod_ns = binwidth * maxbuckets;
}

histogram::~histogram()
{}

void histogram::put(long long Eventts)
{
  long long timeOnHistoScale;
  long long index = 0;
  timeOnHistoScale = Eventts % (maxbuckets*binwidth);
  std::cout << Eventts << " <<<< " << std::endl;
  // std::cout << timeOnHistoScale << " " << value << " " << maxbuckets << " " << binwidth << std::endl; 
  index = (long) ((timeOnHistoScale * maxbuckets) / fullperiod_ns);
  buckets[index]++;
  std::cout << index << " : " << buckets[index] << std::endl;
}

void histogram::reset()
{
  for (long long n=0; n< maxbuckets; n++){
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

  for( long long a = 0; a < maxbuckets; a++ )
  {std::cout << a * binwidth << ", " << buckets[a] << ", " << std::endl;}

  std::cout << std::endl;
}