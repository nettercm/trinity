
#include "utils.h"

volatile unsigned long g_flags;



void flag_set(unsigned long flag)
{
	g_flags |= flag;
}

void flag_clear(unsigned long flag)
{
	g_flags &= ~flag;
}

int flag_get(unsigned long flag, int clear)
{
	if(g_flags && flag)
	{
		if(clear) flag_clear(flag);
		return 1;
	}
	return 0;
}


