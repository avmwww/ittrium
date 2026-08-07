#include "telemetry.h"

SIZE task_stack_used(const TCB *tcb)
{
  const UB *p;
  SIZE freeb = 0;

  if (!tcb || !tcb->stk_base || tcb->stksz == 0)
    return 0;
  p = (const UB *)tcb->stk_base;
  while (freeb < tcb->stksz && p[freeb] == STACK_CANARY)
    freeb++;
  return (SIZE)(tcb->stksz - freeb);
}

unsigned task_stack_pct(const TCB *tcb)
{
  SIZE used;

  if (!tcb || tcb->stksz == 0)
    return 0;
  used = task_stack_used(tcb);
  return (unsigned)((used * 100u) / tcb->stksz);
}

unsigned task_cpu_pct(const TCB *tcb)
{
  UW wall = telemetry_wall_ticks;

  if (!tcb || wall == 0)
    return 0;
  return (unsigned)((tcb->run_ticks * 100ull + wall / 2ull) / wall);
}

unsigned idle_cpu_pct(void)
{
  UW wall = telemetry_wall_ticks;

  if (wall == 0)
    return 0;
  return (unsigned)((telemetry_idle_ticks * 100ull + wall / 2ull) / wall);
}
