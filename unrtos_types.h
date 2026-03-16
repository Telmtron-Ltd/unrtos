#ifndef unrtos_TYPES_H
#define unrtos_TYPES_H


typedef struct unrtos_task_info_s {
    int core;
    int idx;
    int period;
} unrtos_task_ctx_t;

typedef void (*task_fn_t)(void);
typedef void (*task_init_fn_t)(unrtos_task_ctx_t* ctx);

typedef void** unwire_data_buf_t;

#endif /* unrtos_TYPES_H */
