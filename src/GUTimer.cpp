#include "../include/GUTimer.h"

GUTimer::GUTimer(GUTimer::Type type)
{

        p_type=type;
}

GUTimer::~GUTimer()
{
p_tlo.reset();
    //dtor
}

GUTimer& GUTimer::operator=(const GUTimer& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}


uint64_t GUTimer::getElapsed()
{
  switch (p_type){
    case GUTimer::ctime:{
        return (uint64_t)((p_endtime-p_starttime)*1000000000)/ (uint64_t) CLOCKS_PER_SEC;
    break;}
    case GUTimer::LOFAR_timer:{
       uint64_t t= (uint64_t)p_tlo.getElapsed();
        return t;
    break;}
    default:
        return p_endtime-p_starttime;
    }
}



void GUTimer::start(){

  switch (p_type){
    case GUTimer::time_h:{
      p_starttime= GUTimer::p_ClockGetTime();
      break;}
    case GUTimer::rdtsc:{
      p_starttime= GUTimer::p_rdtscGetTime();
      break;}
    case GUTimer::chrono_hr:{
      auto now = GUTimer::p_chrono_hr_Clock::now();
      p_starttime = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
      break;}
    case GUTimer::ctime:{
      clock_t now = clock();
      p_starttime= now;
      break;}
    case GUTimer::LOFAR_timer:{
    p_tlo.start();
    break;}
  }
return;}

void GUTimer::stop(){
  switch (p_type){
    case GUTimer::time_h:{
      p_endtime= GUTimer::p_ClockGetTime();
      break;}
    case GUTimer::rdtsc:{
      p_endtime= GUTimer::p_rdtscGetTime();
      break;}
    case GUTimer::chrono_hr:{
      auto now = GUTimer::p_chrono_hr_Clock::now();
      p_endtime= std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
      break;}
    case GUTimer::ctime:{
      clock_t now = clock();
      p_endtime= now;
      break;}
    case GUTimer::LOFAR_timer:{
    p_tlo.stop();
    break;}
  }
return;}



/*
The Get time functions use different methods to get the system time.

*/
inline uint64_t GUTimer::p_rdtscGetTime()
{
    uint32_t lo, hi;
    __asm__ __volatile__ (
      "xorl %%eax, %%eax\n"
      "cpuid\n"
      "rdtsc\n"
      : "=a" (lo), "=d" (hi)
      :
      : "%ebx", "%ecx" );
    return (uint64_t)hi << 32 | lo;
}

uint64_t GUTimer::p_ClockGetTime() //gets the clock time using the time_t timer type
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000LL + (uint64_t)ts.tv_nsec ;
}

