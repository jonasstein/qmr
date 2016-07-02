// This is start of the header guard and can be any unique name.  By convention, we use the name of the header file.
#ifndef histogram_H
#define histogram_H

#include "js_debug.cpp"


class histogram{
private:
  uint64_t buckets[10000] ;//= {0}; // Make the size flexible later
  uint16_t maxbuckets; 
  uint64_t binwidth;
  
public:
  histogram(const uint16_t setmaxbuckets, const uint64_t setbinwidth);
  ~histogram();
  
  void put(uint64_t& value);
  void print();
};


// This is the end of the header guard
#endif