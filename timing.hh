#ifndef __TIMING_HH__
#define __TIMING_HH__

#include <cstdint>

class Timer {
private:
  typedef struct {
    uint8_t pid;
    uint64_t tsc;
  } rdtscp_t;

  uint64_t clock_freq, start_time;
  uint8_t start_pid;
  rdtscp_t timing_info;
  bool have_tocked;
  
  void rdtscp(rdtscp_t *tscp);
  Timer() {};

public:
  Timer(uint64_t in_freq);
  void tick(); // start timer
  void tock(); // stop timer
  double get_time(); // get time b/t tick and tock
};

void Timer::rdtscp(rdtscp_t *tscp) {
  unsigned cycles_low, cycles_high;
  unsigned pid; // processor id

  asm volatile ("RDTSCP\n\t" // rdtscp into eax and edx
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                "mov %%ecx, %2\n\t"
                :"=r" (cycles_high), "=r" (cycles_low), "=r" (pid) //store in vars
                :// no input
                :"%eax", "%edx", "%ecx" // clobbered by rdtscp
               );
  
  tscp->tsc = ((uint64_t)cycles_high << 32) | cycles_low;
  tscp->pid = pid;
}

Timer::Timer(uint64_t in_freq) : clock_freq(in_freq), have_tocked(false) {}

void Timer::tick() {
  rdtscp(&timing_info);
  start_time = timing_info.tsc;
  start_pid = timing_info.pid;

  have_tocked = false;
}

void Timer::tock() {
  rdtscp(&timing_info);

  have_tocked = true;
}

// return -1 if failure due to thread switching CPUs or incorrect tick/tock use
double Timer::get_time() {
  // check that we're 
  if (!have_tocked || (start_pid != timing_info.pid))
    return -1;

  return (static_cast<double>(timing_info.tsc) - static_cast<double>(start_time)) / clock_freq;
}

#endif
