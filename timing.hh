#ifndef __TIMING_HH__
#define __TIMING_HH__

#include <stdint.h>
#include <time.h>

#define BILLION  1000000000L
#define GHZ_TO_HZ BILLION

class Clock {
	public:
		virtual void stop_time() = 0;
		virtual double report_time() = 0;
};

class TimeStampClock : public Clock {
	private:
  	typedef struct {
    	uint8_t pid;
    	uint64_t tsc;
  	} rdtscp_t;

		uint64_t clock_freq;
  	rdtscp_t timing_info;
  	void rdtscp(rdtscp_t *tscp);

	public:
		TimeStampClock(uint64_t);
		void stop_time();
		double report_time();
};

class SysTimeClock : public Clock {
	private:
		struct timespec ts;

	public:
		void stop_time();
		double report_time();
};

class Timer {
private:
	Clock *start_clock, *end_clock;
  bool have_tocked;
  
  Timer() {};

public:
  Timer(double in_freq);
	~Timer();
  void tick(); // start timer
  void tock(); // stop timer
  double get_time(); // get time b/t tick and tock
};

#endif
