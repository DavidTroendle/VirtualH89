/// \file h89-timer.cpp
///
/// \date Mar 26, 2009
/// \author Mark Garlanger
///

#include "h89-timer.h"
#include "CPU.h"
#include "InterruptController.h"
#include "SignalHandler.h"
#include "WallClock.h"
#include "logger.h"

#include <sys/time.h>
#include <signal.h>

#define USE_ONE_MSEC 0

#if TWOMSEC
#if USE_ONE_MSEC
static const int TimerInterval_c = 1000;
#else
static const int TimerInterval_c = 2000;
#endif
#else
// in order for this to work, the CPU will need to create the intermediary interrupts.
// - add a second counter to the CPU object, one counter for the total cycles to use, the
//  second counter to determine when to initiate the interrupt. May still have some issue
//  for interrupt driven code???
static const int TimerInterval_c = 10000;
#endif

H89Timer::H89Timer(CPU *cpu, InterruptController *ic, unsigned char intlvl): cpu_m(cpu),
    ic_m(ic),
    intEnabled_m(false),
    count_m(0),
    intLevel(intlvl)
{
    // We need to start up the timer since it performs two tasks, it always provide the cpu
    // with extra clock ticks to accurately emulate the speed of the processor.
    // Plus, if interrupts are enabled, it will interrupt the cpu with the timer tick.

    static struct itimerval tim;

    debugss(ssTimer, INFO, "%s\n", __FUNCTION__);

    SignalHandler::instance()->registerHandler(SIGALRM, this);

    tim.it_value.tv_sec = 0;
    tim.it_value.tv_usec = TimerInterval_c;

    tim.it_interval.tv_sec = 0;
    tim.it_interval.tv_usec = TimerInterval_c;

    setitimer(ITIMER_REAL, &tim, NULL);
}


H89Timer::H89Timer(unsigned char intlvl): cpu_m(0),
    ic_m(0),
    intEnabled_m(false),
    count_m(0),
    intLevel(intlvl)
{
    static struct itimerval tim;

    debugss(ssTimer, INFO, "%s: ic_m(NULL)\n", __FUNCTION__);

    SignalHandler::instance()->registerHandler(SIGALRM, this);

    tim.it_value.tv_sec = 0;
    tim.it_value.tv_usec = TimerInterval_c;

    tim.it_interval.tv_sec = 0;
    tim.it_interval.tv_usec = TimerInterval_c;

#if TEN_X_SLOWER
    tim.it_value.tv_usec *= 20;
    tim.it_interval.tv_usec *= 20;
#endif

    setitimer(ITIMER_REAL, &tim, NULL);
}

void H89Timer::setInterruptController(InterruptController *ic)
{
    debugss(ssTimer, INFO, "%s\n", __FUNCTION__);

    ic_m = ic;
}

void H89Timer::setCPU(CPU *cpu)
{
    debugss(ssTimer, INFO, "%s\n", __FUNCTION__);

    cpu_m = cpu;
}

H89Timer::~H89Timer()
{
    static struct itimerval tim;

    debugss(ssTimer, INFO, "%s\n", __FUNCTION__);

    SignalHandler::instance()->removeHandler(SIGALRM);

    tim.it_value.tv_sec = 0;
    tim.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &tim, NULL);
}



int H89Timer::handleSignal(int signum)
{
    debugss(ssTimer, ALL, "Timer Handle Signal\n");

    if (signum != SIGALRM)
    {
        debugss(ssTimer, ERROR, "%s: signum != SIGALRM: %d\n", __FUNCTION__, signum);

        return (0);
    }

    count_m++;

    WallClock::instance()->addTimerEvent();

    if (cpu_m)
    {
        debugss(ssTimer, VERBOSE, "%s: adding clock ticks\n", __FUNCTION__);

        // must always give the CPU more cycles.
        cpu_m->addClockTicks();

        // Only if interrrupt is enabled.
#if USE_ONE_MSEC

        if ((intEnabled_m) && ((count_m & 0x01) == 0x00))
#else
        if (intEnabled_m && ic_m)
#endif
        {
            debugss(ssTimer, VERBOSE, "%s: raising Interrupt\n", __FUNCTION__);

            ic_m->raiseInterrupt(intLevel);
        }
    }

    else
    {
        debugss(ssTimer, ERROR, "%s: cpu_m is NULL\n", __FUNCTION__);
    }

    return (0);
}

void H89Timer::enableINT()
{
    debugss(ssTimer, INFO, "Enable INT\n");

    intEnabled_m = true;
}

void H89Timer::disableINT()
{
    debugss(ssTimer, INFO, "Disable INT\n");

    intEnabled_m = false;

    if (ic_m)
    {
        ic_m->lowerInterrupt(intLevel);
    }
}
