/**
 * Generic support CPU
 */

#ifndef INIT_SYSTEM_TICKS
# define INIT_SYSTEM_TICKS()  {\
  _system_ticks.st_low = 0;    \
  _system_ticks.st_high = 0;   \
                              }
#endif // INIT_SYSTEM_TICKS

/**
 *
 */
SYSTIM _system_ticks;

/**
 *
 */
int int_nesting;

/**
 *
 */
static int timer_nesting;

/**
 *
 */
void timer_handler(void);

#ifndef _timer_hw_enter_defined_
/**
 * timer_hw_enter() return nesting count
 */
int timer_hw_enter(void)
{
  int ret;

  BEGIN_CRITICAL_SECTION;
  _system_ticks.st_low++;
  if (TIC_DENO == _system_ticks.st_low) {
    _system_ticks.st_low = 0;
    _system_ticks.st_high++;
  }

  ret = timer_nesting++;
  END_CRITICAL_SECTION;

  return ret;
}
#endif // _timer_hw_enter_defined_

#ifndef _timer_hw_exit_defined_
/**
 *
 */
void timer_hw_exit(void)
{
  if (timer_nesting) timer_nesting--;
  CLEAR_TICKER_INT();
}
#endif // _timer_hw_exit_defined_
