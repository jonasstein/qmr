// This is start of the header guard and can be any unique name.  By convention, we use the name of the header file.
#ifndef histogram_H
#define histogram_H

//#include "js_debug.cpp"


class histogram{
private:
  long long buckets[10000]={}; // Make the size flexible later
  long long maxbuckets; 
  long long binwidth;
  long long fullperiod_ns;
  
public:
  histogram(const long long setmaxbuckets, const long long setbinwidth);
  ~histogram();
  
  void put(long long &Eventts);
  void reset();
  void printheader();
  void print();
};


// This is the end of the header guard
#endif
