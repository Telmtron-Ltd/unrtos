#ifndef unrtos_H
#define unrtos_H

#include <time.h> 
#include "unwire/unwire.h"
#include "unrtos_types.h"
#include "unrtos_config.h"

#define UNRTOS_ADD_INIT_TASK(c,f,o,p) { unrtos_add_task(c,f ## _init,f,o,p); }
#define UNRTOS_MStoTICKS(t)           (t / unrtosTICK_PERIOD_MS)

#define TASK_HEARTBEAT() do { \
    static uint32_t hb_count = 0; \
    static uint32_t last_tick = 0; \
    uint32_t current_tick = unrtos_get_current_ticks(); \
    if (current_tick - last_tick > 2000) { /* ~2 seconds at 20ms ticks */ \
        hb_count++; \
        last_tick = current_tick; \
        xprintln(DEBUG, "heartbeat: %lu (%lu sec)", hb_count, hb_count * 2); \
        fflush(stdout); \
    } \
} while(0)

#define UNRTOS_TASK_DECL(n) \
    void n ## _init(unrtos_task_ctx_t* ctx); \
    volatile void n(void);

typedef int (*unrtos_setup_fn_t)(int core_id);

void unrtos_run(unrtos_setup_fn_t setup_fn);
unrtos_task_ctx_t* unrtos_add_task(int core_id, task_init_fn_t init, task_fn_t fn, time_t offset, time_t period);
unrtos_task_ctx_t* unrtos_get_task_ctx(task_fn_t fn);
time_t unrtos_get_current_ticks(void);
void unrtos_stop(void);

#endif /* unrtos_H */
