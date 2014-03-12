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


void dbg_test(void)
{
	/*
	t1=get_ticks(); 	usb_printf("0"); 			t2=get_ticks();  	usb_printf("\ntime for usb_printf(1) : %ld us\n", ticks_to_microseconds(t2-t1));
	t1=get_ticks(); 	usb_printf("01"); 			t2=get_ticks();  	usb_printf("\ntime for usb_printf(2) : %ld us\n", ticks_to_microseconds(t2-t1));
	t1=get_ticks(); 	usb_printf("0123456789"); 	t2=get_ticks();  	usb_printf("\ntime for usb_printf(10): %ld us\n", ticks_to_microseconds(t2-t1));
	*/
	//DBG_printf(1,("DBG_printf()\n"));
}


//the following is not used right now, so don't bother including it in the build
#if 0
uint8 console_buffer[128];

void console(void)
{
	static uint8 state=0;
	uint8 b=0;
	
	switch(state)
	{
		case 0:
		serial_receive(USB_COMM,console_buffer,127);
		state++;
		break;
		
		case 1:
		if( (b=serial_get_received_bytes(USB_COMM)) != 0 ) 
		{
			console_buffer[b]=0;
			usb_printf("%s",console_buffer);
		}
		state = 0;
		break;
	}
}


void debug_fsm(void)
{
	uint8 b=0;
	task_open();
	
	usb_printf("debug_fsm()\n");
	serial_receive(USB_COMM,(char*)console_buffer,127);

	while(1)
	{
		serial_check();
		if( (b=serial_get_received_bytes(USB_COMM)) != 0 )
		{
			console_buffer[b]=0;
			usb_printf("%s",console_buffer);
			serial_receive(USB_COMM,(char*)console_buffer,127);
		}
		OS_SCHEDULE;
	}
	
	task_close();
}
#endif
