/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "unrtos.h"
#include "unrtos_config.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <pico/stdlib.h>
#include "private/tasks.h"
#include "hal/timer.h"
#include "hal/smp.h"
#include "hal/sync.h"
#include "hardware/watchdog.h"

#define XLOG_LEVEL XLOG_INFO
#include "util/xprint.h"

// Forward declaration for app_main in main.c
int app_setup(int core_id);

static volatile bool _running = false;
static volatile time_t _ticks = 0; 


void unrtos_launch_core(void);
void unrtos_scheduler(int core_id);

int main(void) {
    smp_start_core(0, unrtos_launch_core);

    return 0;
}

static void __task_watchdog_tick(int core) {
    if(0 == --unrtos_task_wdt[core]) {
        xprintln(ERR, "Timer expired on core %d", core);
        // fflush(stdout); // Ensure the error message is sent before watchdog reset
        // watchdog_enable(1, 1); // TODO: move to HAL
        // while(1) {
        //     sleep_ms(1);
        // }
    }
}

static void __task_watchdog_feed(int core) {
    unrtos_task_wdt[core] = unrtosTASK_WDT_TICKS;
}


void timer_onTick(void) {
    for(int core = 0; core < unrtosNUM_CORES; core++) {
        __task_watchdog_tick(core);
        for(int tsk = 0; tsk < unrtos_num_tasks[core]; tsk++) {
            if(unrtos_tasks[core][tsk].tmr) {
                unrtos_tasks[core][tsk].tmr--;
            }
        }
    }
    
    // Release fence to ensure all timer decrements are visible before tick increment
    __mem_fence_release();
    _ticks++;
}

unrtos_task_ctx_t* unrtos_add_task(int core_id, task_init_fn_t init, task_fn_t fn, time_t offset, time_t period) {
    unrtos_task_ctx_t ctx = { .core = core_id, .idx = unrtos_num_tasks[core_id], .period = period };
    unrtos_task_ctx_t* task_ctx = NULL;

    if(!_running) {
        unrtos_tasks[ctx.core][ctx.idx].init = init;
        unrtos_tasks[ctx.core][ctx.idx].fn = fn;
        unrtos_tasks[ctx.core][ctx.idx].tmr = offset+1;
        unrtos_tasks[ctx.core][ctx.idx].period = period;
        unrtos_tasks[ctx.core][ctx.idx].ctx = ctx;

        task_ctx = &unrtos_tasks[ctx.core][ctx.idx].ctx;
        unrtos_num_tasks[ctx.core]++;    

        // (*unrtos_tasks[ctx.core][ctx.idx].init)(&unrtos_tasks[ctx.core][ctx.idx].ctx);
    }
    else {
        xprintln(ERR, "Unable to add task while scheduler is running");
    }

    return task_ctx;
}

unrtos_task_ctx_t* unrtos_get_task_ctx(task_fn_t fn) {
    unrtos_task_ctx_t* task_ctx = NULL;

    for(int core = 0; (NULL == task_ctx) && (core < unrtosNUM_CORES); core++) {
        for(int tsk = 0; (NULL == task_ctx) && (tsk < unrtos_num_tasks[core]); tsk++) {
            if(unrtos_tasks[core][tsk].fn == fn) {
                task_ctx = &unrtos_tasks[core][tsk].ctx;
            }
        }
    }

    return task_ctx;
}

time_t unrtos_get_current_ticks(void) {
    return _ticks;
}

static void unrtos_launch_other_cores(int me) {
    for(int core = 0; core < unrtosNUM_CORES; core++) {
        if(core != me) {
            xprintln(INFO, "Launching Core %d from core %d...", core, me);
            smp_start_core(core, unrtos_launch_core);
        }
    }
}

void unrtos_launch_core(void) {
    int my_core = smp_get_core_id();
    int next_core = my_core+1;

    /* app_setup must return >= 0 to start the scheduler */
    if(app_setup(my_core) < 0) {
        xprintln(ERR, "Core %d: Error in app_setup. Cannot continue", my_core);
        while(1) { };
    }

    for(int tsk = 0; tsk < unrtos_num_tasks[my_core]; tsk++) {
        xprintln(INFO, "Initialising Core %d, Task %d...", my_core, tsk);

        unrtos_task_ctx_t* ctx = &unrtos_tasks[my_core][tsk].ctx;
        (*unrtos_tasks[my_core][tsk].init)(ctx);
    }
    
    xprintln(INFO, "Core %d: Starting unrtos scheduler...", my_core);
    unrtos_scheduler(my_core);
    /* No return from unrtos_scheduler */
}

void unrtos_scheduler(int my_core) {
    time_t my_ticks = 0;
    time_t tick_diff;

    if(0 == my_core) {
        _ticks = 0;
        timer_onTick();
        for(int i = 0; i < unrtosNUM_CORES; i++) {
            __task_watchdog_feed(i);
        }
        timer_start(unrtosTICK_PERIOD_MS);
        unrtos_launch_other_cores(my_core);
    }
        
    while(1) {
        timer_wait(my_core);
        // Acquire fence to ensure we see the latest tick count and timer values
        __mem_fence_acquire();
        tick_diff = (my_ticks < _ticks) ? (_ticks - my_ticks) : 0;
        if(tick_diff) {
            if(tick_diff > 1) {
                xprintln(WARN, "Core %d: missed %llu ticks", my_core, tick_diff-1);
            }
            for(int tsk = 0; tsk < unrtos_num_tasks[my_core]; tsk++) {
                if(!unrtos_tasks[my_core][tsk].tmr) {
                    unrtos_tasks[my_core][tsk].tmr = (unrtos_tasks[my_core][tsk].period);
                    (*unrtos_tasks[my_core][tsk].fn)();
                }
            }
            my_ticks = _ticks;
        }
        __task_watchdog_feed(my_core);
    }
}

void unrtos_stop(void) {
    timer_stop();
}