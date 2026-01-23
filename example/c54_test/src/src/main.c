#include "ittrium.h"

void timer_handler(void);

void start_hw_timer(void)
{
  install_handler(timer_handler, TINT0_VEC);
  // ??? ENABLE int
}

void terminate_hw_timer(void)
{
  // ??? DISABLE int
}

void init_tsk(void *exinf)
{
  // Initialize periphery ???????
  for (;;) {
    // Wait 1 sec
    dly_tsk(1000);
  }
}


