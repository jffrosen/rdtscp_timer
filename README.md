# rdtscp_timer
Timer using the RDTSCP instruction for nanosecond-granularity

This library provides a means for obtaining nanosecond-granularity timing information for 
experimental analysis or profiling.  It uses the RDTSCP timestamp counter instruction
for x86 if provided with a CPU frequency, or falls back on the Linux clock_gettime function.

Sample program:
#include "timing.hh"
#include <unistd.h>
#include <iostream>

int main() {
  Timer t(2.4);
  
  t.tick();
  sleep(5);
  t.tock();
  
  std::cout << "Time taken: " << t.get_time() << std::endl;
  
  return 0;
}
