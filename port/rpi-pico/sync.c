// For now, just include the pico's sync.h
#include "hal/sync.h"
#include "hardware/sync.h"

void sync_mem_fence_acquire(void) {
    __mem_fence_acquire();
}

void sync_mem_fence_release(void) {
    __mem_fence_release();
}
