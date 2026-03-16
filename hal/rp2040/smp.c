#include "../smp.h"

#include <pico/multicore.h>
#include <hardware/sync.h>

#include "util/xprint.h"

typedef void (*core1_fn_t)(void);

void smp_start_core(int core_id, void * deets) {
    core1_fn_t fn = (core1_fn_t)deets;
    int my_core = get_core_num();

    xprintln(INFO, "Starting core %d from core %d...", core_id, my_core);

    if(core_id == my_core) {
        (*fn)();
    }
    else if(1 == core_id) {
        multicore_launch_core1(fn);
    }
    else {
        xprintln(ERR, "No such core %d on this board", core_id);
    }
}

int smp_get_core_id(void) {
    return get_core_num();
}
