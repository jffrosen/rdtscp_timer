#include "timing.hh"

void TimeStampClock::rdtscp(rdtscp_t *tscp) {
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

TimeStampClock::TimeStampClock(uint64_t in_freq) : clock_freq(in_freq) {}

inline
void TimeStampClock::stop_time() {
  rdtscp(&timing_info);
}

double TimeStampClock::report_time() {
  return static_cast<double>(timing_info.tsc) / clock_freq;
}

inline
void SysTimeClock::stop_time() {
	clock_gettime(CLOCK_MONOTONIC, &ts);
}

double SysTimeClock::report_time() {
	return ts.tv_sec + static_cast<double>(ts.tv_nsec) / BILLION;
}

Timer::Timer(double in_freq_ghz) : have_tocked(false) {
	if (in_freq_ghz <= 0) {
		start_clock = new SysTimeClock();
		end_clock = new SysTimeClock();
	} else {
		start_clock = new TimeStampClock(in_freq_ghz * GHZ_TO_HZ);
		end_clock = new TimeStampClock(in_freq_ghz * GHZ_TO_HZ);
	}
}

Timer::~Timer() {
	delete start_clock;
	delete end_clock;
}

inline
void Timer::tick() {
	start_clock->stop_time();

  have_tocked = false;
}

inline
void Timer::tock() {
	end_clock->stop_time();

  have_tocked = true;
}

// return -1 if failure due to thread switching CPUs or incorrect tick/tock use
double Timer::get_time() {
  if (!have_tocked)
    return -1;

	return end_clock->report_time() - start_clock->report_time();
}
