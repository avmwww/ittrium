/**
 * Host _sbrk for procfs /proc/mem
 */
#include <unistd.h>

void *_sbrk(int incr)
{
	return sbrk(incr);
}
