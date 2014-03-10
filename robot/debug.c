/*
 * debug.c
 *
 * Created: 11/25/2012 9:48:34 PM
 *  Author: Chris
 */ 


#include "typedefs.h"
#include "debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <pololu/orangutan.h>

char _b[200];

int	usb_printf(const char *__fmt, ...)
{
	#ifdef USB_COMM
	#if 1
	int size;
	va_list ap;
	if(usb_power_present())
	{
		va_start(ap, __fmt);
		size = vsprintf(_b, __fmt, ap);
		serial_send_blocking(USB_COMM,_b,size);
		va_end(ap);
	}
	#endif
	#endif
	return 0;
}


uint8 SIM_serial_get_received_bytes(void)
{
	static uint8 first_call=1;
	static uint8 i=0;

	if(first_call)
	{
		first_call=0;
		
	}	
	else
	{
		i += 2;
		if(i>128) i=128;
	}
	return i;
}


int	SIM_printf(const char *__fmt, ...)
{
	return 0;	
}