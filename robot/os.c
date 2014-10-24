
#include "standard_includes.h"


void _os_tick(void)
{
	static unsigned long t_last=0;
	unsigned long t_now;
	unsigned long t_delta;

#ifndef WIN32
	t_now = get_ms();
	if(t_now != t_last)
	{
		t_delta=t_now-t_last;
		t_last=t_now;
		//os_tick();
		m.elapsed_milliseconds+=t_delta;
		os_task_tick(0,(unsigned short)t_delta);
	}
#else
	t_last++;
	if(t_last > 20)
	{
		//Sleep(1);
		t_last = 0;
		t_delta = 1;
		delay_ms(1); //this will update elapsed_milliseconds and issue a Sleep(1) to make sure PC does not get bogged down
		os_task_tick(0,(unsigned short)t_delta);
	}
#endif
}

volatile unsigned long idle_counter=0;
void _os_idle(void)
{
	idle_counter++;
}



#if 1

void busy_task(u08 cmd, u08 *param)
{
	static u16 i;
	static u32 ic1=0,ic2,ic3;
	static u32 t_last,t_now;
	static u32 min=999999,max=0,avg=0;

	task_open();

	t_now=t_last=get_ms();
	
	//110526 counts per second w/ no other tasks running
	i=0;
	while(1)
	{
		i++;
		if(i>9) i=1;
		usb_printf("%d percent busy...\n",i*10);
		t_last=get_ms();
		while(get_ms()-t_last < 10000)
		{
			delay_ms(i);
			task_wait(10-i);
		}
	}
	task_close();
}


void idle_task(u08 cmd, u08 *param)
{
	u08 i;
	static u32 ic1=0,ic2,ic3;
	static u32 t_last,t_now;
	static u32 min=999999,max=0,avg=0;

	task_open();

	usb_printf("idle_task()\n");

	t_now=t_last=get_ms();
	
	//110526 counts per second w/ no other tasks running
	while(1)
	{
		t_now=get_ms();
		ic1 = idle_counter;	
		task_wait(100); 
		ic2 = idle_counter - ic1;
		if(ic2<min) min=ic2;
		if(ic2>max) max=ic2;
		avg = ((avg*9)+ic2)/10;
		if(t_now-t_last>=1000)
		{
			usb_printf("idle: min=%ld, avg=%ld, max=%ld\n",min,avg,max);
			t_last=t_now;
			min=999999;
			max=0;
		}
	}
	task_close();
}
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
