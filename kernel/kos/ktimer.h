////////////////////////////////////////////////////////////////////////////////
///@file    ktimer.h
///@short   kernel timer interface
///@author  J.Nider
///@date    30/10/2009
////////////////////////////////////////////////////////////////////////////////
/// These functions can only be called from kernel mode
////////////////////////////////////////////////////////////////////////////////

#ifndef _KTIMER__H
#define _KTIMER__H

#include "timer.h"   // timer_callback
#include "isr.h"     // regs_t

typedef void(*pSchedulerFN)(regs_t*);

// kernel mode only!
void k_initTimer(int hz);
unsigned short k_getTickDivisor();
void k_printTimers(void);
unsigned int readCR4(void);
void writeCR4(unsigned int val);
void k_setScheduler(pSchedulerFN pScheduler, unsigned int time);
int k_startTimer(unsigned int ms, timer_callback cb, void* param, unsigned int taskID);
int k_stopTimer(int timerId);
unsigned int k_getTickCount();

#endif // _KTIMER__H

