
#include <Windows.h>
#include <timeapi.h>
#include <stdio.h>
#include "cocoos/cocoos.h"



int us_data[100];

void win32_main(void)
{
	timeBeginPeriod(1);
}
