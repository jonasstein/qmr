#include "js_debug.cpp"
#include <iostream>     // std::cout
#include "histogram.hpp"
#include <cstdint>
#include <stdio.h>
//#define NDEBUG  // at the beginning of the code, before the inclusion of <assert.h> will disable all asserts
#include <assert.h>


histogram::histogram(const uint16_t setmaxbuckets, const uint64_t setbinwidth)
{
  maxbuckets = setmaxbuckets;
  
}

histogram::~histogram()
{}

void histogram::put(uint64_t& value)
{
//  bool Histogram_Range_Exceeded = false;
//  if(value > maxbuckets * binwidth){assert (Histogram_Range_Exceeded = true);}
//  buckets[div(value,binwidth)] += 1;
}

void histogram::print()
{
  for( uint16_t a = 0; a < maxbuckets; a = a + 1 )
   {std::cout << a << ": " << buckets[a] << std::endl;}
}
