#ifndef GUTIMER_H
#define GUTIMER_H

//#include <stdint.h>       /*Not required? uint64_t*/
#include <time.h>           /*timespec, CLOCK_REALTIME, ts, clock_gettime*/
#include <iostream>         /* std::cout */
#include <chrono>           /*high_resolution_clock, duration_cast, nanoseconds */
#include <ctime>            /*clock_t,  clock, CLOCKS_PER_SEC*/
#include "LOFARTimer.h"          /*LOFAR::NSTimer */


class GUTimer
{
    public:
        enum Type { time_h,rdtsc,chrono_hr,ctime,LOFAR_timer };

        GUTimer(Type type);
        virtual ~GUTimer();
        GUTimer& operator=(const GUTimer& other);

        Type p_type;
        void start();
        void stop();
        uint64_t getElapsed();

    protected:

    private:
    uint64_t p_ClockGetTime();
    uint64_t p_rdtscGetTime();
    uint64_t p_starttime;
    uint64_t p_endtime;
    LOFAR::NSTimer p_tlo;
    typedef std::chrono::high_resolution_clock p_chrono_hr_Clock;
};

#endif // GUTIMER_H
