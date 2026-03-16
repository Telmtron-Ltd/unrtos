/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "unwire.h"
#include "unrtos_config.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include "tasks.h"
#include "hal/timer.h"
#include "hal/smp.h"
#include "hal/sync.h"

#define XLOG_LEVEL XLOG_INFO
#include "util/xprint.h"


static bool unwire_has_space(unwire_t* bus) {
    size_t next = bus->head+1;
    if(next >= unrtosUNWIRE_MAX_ITEMS) {
        next = 0;
    }

    return (next != bus->tail);
}

static bool unwire_put(unwire_t* bus, void* item) {
    bool putted = false;

    size_t current_head = bus->head;
    size_t next = current_head + 1;
    if(next >= unrtosUNWIRE_MAX_ITEMS) {
        next = 0;
    }
    
    // Check if queue has space
    if(next != bus->tail) {
        void* p_next_item = bus->data + (current_head * bus->item_size);
        
        // Write data first
        memcpy(p_next_item, item, bus->item_size);
        
        // Update head index last - this makes the item visible to consumers
        bus->head = next;
        putted = true;
    }

    return putted;
}

static bool unwire_get(unwire_t* bus, void* item) {
    bool getted = false;

    size_t current_tail = bus->tail;
    
    // Check if queue has data
    if(current_tail != bus->head) {
        void* p_next_item = bus->data + (current_tail * bus->item_size);

        // Read data first
        memcpy(item, p_next_item, bus->item_size);
        
        // Calculate next tail with wraparound
        size_t next_tail = current_tail + 1;
        if(next_tail >= unrtosUNWIRE_MAX_ITEMS) {
            next_tail = 0;
        }
        
        // Update tail index last - this frees the slot for producers
        bus->tail = next_tail;
        getted = true;
    }

    return getted;
}


/**
 * Call from a task to initialise its unWire
 * It's the responsibility of a task to create a static buffer of
 * `unrtosUNWIRE_MAX_ITEMS` items of a chosen type to hold the unWire data
 * 
 * @param ctx:       A pointer to the task's own context
 * @param data:      A pointer to the data array to hold the unWire data
 * @param item_size: The size of the items in the data array
 */
void unwire_init(volatile unrtos_task_ctx_t* ctx, size_t item_size) {
    xprintln(DEBUG, "=== unwire_init DEBUG ===");
    xprintln(DEBUG, "Core: %u, Task: %u", ctx->core, ctx->idx);
    
    int core;
    for(core = 0; core < unrtosNUM_CORES; core++) {
        void* core_buf = malloc(unrtosUNWIRE_MAX_ITEMS*item_size);
        if(!core_buf) {
            xprintln(ERR, "Failed to allocate %zu byte unwire buffer for core %u", item_size, core);
            break;
        }

        unrtos_tasks[ctx->core][ctx->idx].unwire[core].data = core_buf;
        unrtos_tasks[ctx->core][ctx->idx].unwire[core].item_size = item_size;
        unrtos_tasks[ctx->core][ctx->idx].unwire[core].head = 0;
        unrtos_tasks[ctx->core][ctx->idx].unwire[core].tail = 0;

        xprintln(DEBUG, "=== Core %u buffer ===", core);
        xprintln(DEBUG, "item_size: %zu", item_size);
        xprintln(DEBUG, "data: %p", unrtos_tasks[ctx->core][ctx->idx].unwire[core].data);
    }

    if(core < unrtosNUM_CORES) {
        xprintln(DEBUG, "=== END unwire_init (FAILED) ===");
        while(--core >= 0) {
            free(unrtos_tasks[ctx->core][ctx->idx].unwire[core].data);
            unrtos_tasks[ctx->core][ctx->idx].unwire[core].data = NULL;
        }
    }
    else {
        xprintln(DEBUG, "=== END unwire_init (SUCCESS) ===");
    }
}

/**
 * Post to a task's unWire
 * @param task:       The task function to post data to
 * @param data:       A pointer to memory to holding the items to post
 * @return The actual number of items posted
 */
size_t unwire_post_item(volatile unrtos_task_ctx_t* task, void* data) {
    size_t posted = 0;

    unwire_t* bus = &unrtos_tasks[task->core][task->idx].unwire[smp_get_core_id()];
    void* p_data = data;

    if(!bus->data) {
        xprintln(ERR, "Unable to post item, unwire not initialised");
    }
    else if(unwire_put(bus, data)) {
        posted++;
    }

    return posted;
}

/**
 * Call from a task to consume data
 * @param data:       A pointer to memory to hold the consumed items
 * @return The actual number of items consumed
 */
size_t unwire_consume_item(volatile unrtos_task_ctx_t* ctx, void* data) {
    size_t consumed = 0;

    unwire_t* bus = unrtos_tasks[ctx->core][ctx->idx].unwire;

    // Clear the output buffer first to prevent garbage data
    // Use the item_size from the first bus (they should all be the same)
    
    for(int core = 0; core < unrtosNUM_CORES; core++) {
        memset(data, 0, bus[core].item_size);

        if(unwire_get(&bus[core], data)) {
            consumed++;
            break;
        }
    }

    return consumed;
}

/**
 * Remove all items from the queue and return it to a clean state
 * @param ctx: Task context for the owner of the queue
 */
void unwire_flush(volatile unrtos_task_ctx_t* ctx) {

    unwire_t* bus = unrtos_tasks[ctx->core][ctx->idx].unwire;
    
    for(int core = 0; core < unrtosNUM_CORES; core++) {
        bus[core].head = bus[core].tail = 0;
    }
}
