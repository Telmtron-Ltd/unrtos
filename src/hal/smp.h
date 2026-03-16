#ifndef SMP_H
#define SMP_H

void smp_start_core(int core_id, void * deets);
int smp_get_core_id(void);

#endif /* SMP_H */
