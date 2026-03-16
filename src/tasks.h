#ifndef TASKS_H
#define TASKS_H

#include <stdbool.h>

#include "unrtos_config.h"
#include "unrtos_types.h"

typedef struct unwire_s {
    void*           data;
    size_t          item_size;
    volatile size_t head;
    volatile size_t tail;
} unwire_t;

typedef struct unrtosTask_s {
    task_init_fn_t	   init;	    /* init function pointer */
    task_fn_t	       fn;			/* task function pointer */
    time_t			   period;		/* period in ticks */
    volatile time_t    tmr;		    /* initial offset in ticks */
    unwire_t           unwire[unrtosNUM_CORES];
    unrtos_task_ctx_t ctx;
} unrtosTask_t;

static unrtosTask_t unrtos_tasks[unrtosNUM_CORES][unrtosMAX_TASKS];
static size_t unrtos_num_tasks[unrtosNUM_CORES] = {0};
static time_t unrtos_task_wdt[unrtosNUM_CORES] = {unrtosTASK_WDT_TICKS};
static bool ticked = false;

#endif /* TASKS_H */
