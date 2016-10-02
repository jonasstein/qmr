//#include "js_debug.cpp"
#include <iostream>     // std::cout
#include "histolong.hpp"
#include <cstdint>
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

void histogram::put(long long &Eventts)
{
  long long timeOnHistoScale;
  long long index = 0;
  timeOnHistoScale = Eventts % (maxbuckets*binwidth);
  index = (long) ((timeOnHistoScale * maxbuckets) / fullperiod_ns);
  buckets[index]++;
}

void histogram::reset()
{
  for (long long n=0; n< maxbuckets; n++){
    buckets[n]=0;
   }
}

void histogram::printheader()
{
  for( long long a = 0; a < maxbuckets; a++ ){
   if (a < maxbuckets-1)
   {
     std::cout << a * binwidth << ",";
   }else std::cout << a * binwidth << std::endl;
  }
}

void histogram::print()
{
  for( long long a = 0; a < maxbuckets; a++ ){
   if (a < maxbuckets-1)
   {
     std::cout << buckets[a] << ",";
   }else std::cout << buckets[a] << std::endl;
  }
}