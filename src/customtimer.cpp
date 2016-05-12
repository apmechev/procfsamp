#include "../include/customtimer.h"

customtimer::customtimer(std::string type)
{
    //ctor
}

customtimer::~customtimer()
{
    //dtor
}

customtimer& customtimer::operator=(const customtimer& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}


uint64_t customtimer::ClockGetTime()
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000LL + (uint64_t)ts.tv_nsec ;
}
