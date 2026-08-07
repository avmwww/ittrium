/* volatile so the loop is not optimized to an empty branch. */
volatile int counter;
volatile int *pcounter = &counter;

void _start(void *exinf)
{
  (void)exinf;
  for (;;)
    (*pcounter)++;
}
