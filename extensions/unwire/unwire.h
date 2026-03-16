#ifndef unwire_H
#define unwire_H

#include <time.h>
#include "unrtos_types.h"
#include "unrtos_config.h"

// unwire_BUFFER macro deprecated - now uses dynamic allocation in unwire_init()
// Old approach: #define unwire_BUFFER(t,n) static t n[unrtosNUM_CORES][unrtosUNWIRE_MAX_ITEMS]

/**
 * Call from a task to initialise its unWire
 * Memory is now dynamically allocated - no need for static buffers
 * 
 * @param ctx:       A pointer to the task's own context
 * @param data:      A pointer to the data array to hold the unWire data
 * @param item_size: The size of the items in the data array
 */
void unwire_init(volatile unrtos_task_ctx_t* ctx, size_t item_size);

/**
 * Post to a task's unWire
 * @param task:       The task function to post data to
 * @param data:       A pointer to memory to holding the items to post
 * @param len:        The maximum number of items to post
 * @return The actual number of items posted
 */
size_t unwire_post_item(volatile unrtos_task_ctx_t* task, void* data);


/**
 * Call from a task to consume data
 * @param data:       A pointer to memory to hold the consumed items
 * @param len:        The maximum number of items available at `data`
 * @return The actual number of items consumed
 */
size_t unwire_consume_item(volatile unrtos_task_ctx_t* ctx, void* data);

/**
 * Remove all items from the queue and return it to a clean state
 * @param ctx: Task context for the owner of the queue
 */
void unwire_flush(volatile unrtos_task_ctx_t* ctx);

#endif /* unwire_H */
