#ifndef ITT_TELEMETRY_H
#define ITT_TELEMETRY_H

#include "ittrium.h"
#include "task.h"

#define STACK_CANARY  ((UB)0xa5)

extern volatile UW telemetry_wall_ticks;
extern volatile UW telemetry_idle_ticks;

SIZE task_stack_used(const TCB *tcb);
unsigned task_stack_pct(const TCB *tcb);
unsigned task_cpu_pct(const TCB *tcb);
unsigned idle_cpu_pct(void);

#define TELEMETRY_IRQ_MAX  32
/* defined in cortex-a53 cpu_support.c */
extern volatile UW irq_count[TELEMETRY_IRQ_MAX];
const char *irq_vec_name(unsigned vec);

#endif
