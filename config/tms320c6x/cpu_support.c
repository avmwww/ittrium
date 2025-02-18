#include "ittrium.h"

#define INT_VEC_NUM     16

FP _int_vector_table[INT_VEC_NUM];
SYSTIM _system_ticks;
static int timer_nesting;

void low_level_init(void)
{
  int i;

  // Initialize vector table
  for (i = 0; i < INT_VEC_NUM; i++) {
    _int_vector_table[i] = (FP)0;
  }
}

void install_handler(FP handler, INHNO vec_no)
{
  BEGIN_CRITICAL_SECTION;
  _int_vector_table[vec_no] = handler;
  END_CRITICAL_SECTION;
}

// return no 0 if running
int timer_hw_enter(void)
{
  int ret;

  BEGIN_CRITICAL_SECTION;
  _system_ticks.st_low++;
  if (TIC_DENO == _system_ticks.st_low) {
    _system_ticks.st_low = 0;
    _system_ticks.st_high++;
  }
  if (timer_nesting) {
    ret = 1;
  } else {
    timer_nesting++;
    ret = 0;
  }
  END_CRITICAL_SECTION;
  return ret;
}

void timer_hw_exit(void)
{
  timer_nesting--;
}

void timer_handler(void);
void start_hw_timer(void)
{
  _int_vector_table[TICKER_VEC_NO] = (FP)timer_handler;
  timer_nesting = 0;
  _system_ticks.st_low = 0;
  _system_ticks.st_high = 0;
  ENABLE_TICKER_INT();
}
void terminate_hw_timer(void)
{
  DISABLE_TICKER_INT();
  _int_vector_table[TICKER_VEC_NO] = (FP)0;
}
