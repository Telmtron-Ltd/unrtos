#ifndef TIMER_H
#define TIMER_H

#include <time.h>

extern void timer_start(time_t tick_period_ms);
extern void timer_onTick(void);
void timer_wait(int core);
void timer_stop(void);

#endif /* TIMER_H */
