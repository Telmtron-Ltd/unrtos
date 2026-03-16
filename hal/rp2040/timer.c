
#include "../timer.h"

#include <pico/stdlib.h>
#include "hardware/timer.h"
#include "hardware/sync.h"

#include "util/xprint.h"

static volatile int64_t tick_period_us = 1000;  // Default 1ms
static volatile alarm_id_t _timer_alarm;

// Hardware alarm callback - minimal overhead
static int64_t timer_alarm_callback(alarm_id_t id, void *user_data) {
    timer_onTick();  // Call the scheduler
    __sev();

    return -tick_period_us;  // Return period for automatic reschedule
}

void timer_start(time_t tick_period_ms) {
    xprintln(INFO, "Starting scheduler tick every %llu ms (hardware alarm)", tick_period_ms);
    tick_period_us = tick_period_ms * 1000;  // Convert to microseconds
    
    // Use hardware alarm for minimal overhead
    _timer_alarm = add_alarm_in_us(tick_period_us, timer_alarm_callback, NULL, true);
}

void timer_stop(void) {
    cancel_alarm(_timer_alarm);
}

void timer_wait(int core) {
    __wfe();
}
