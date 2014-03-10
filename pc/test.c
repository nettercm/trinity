#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>

#include "messages.h"
#include "serial.h"
#include "debug.h"
#include "kalman.h"
#include "utils.h"
#include "data.h"
#include "logic.h"

#include <math.h>

#include "../robot/typedefs.h"
#include "../robot/pid.h"
#include "../robot/config.h"

#include "odometry.h"

//////////////////////////////////////////////////////////////////////////////


void time_test(void)
{
	static unsigned long t0=0;
	static unsigned long t1=0;
	static int state = 0;

	if(state==0)
	{
		t0 = timeGetTime ();
		state = 1;
	}
	else
	{
		t1 = timeGetTime () - t0;
		printf("delta-T = %lu\n",t1);
		state = 0;
	}
}



//////////////////////////////////////////////////////////////////////////////


void loopback_simulate_inputs_message(HANDLE p)
{
#ifdef LOOPBACK
	int result;
	//simulate a inputs message
	result = serial_write(p,(char*)&ginputs,sizeof(t_inputs));
	Sleep(5);
	if(result != sizeof(t_inputs))
	{
		//error
	}
#endif

}





//////////////////////////////////////////////////////////////////////////////


uint8 h1[] = {1,2,3,4,3,4,2,1,3,4,5,6,7,8,9,9,9,9,9,9,9,8,7,6,5,4,3,2,1,1};
uint8 h2[] = {9,9,9,9,8,7,6,5,4,3,2,1,1,2,3,4,5,6,7,8,9,9,9,9,9,9,8,7,6,5};
uint8 h3[] = {0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

uint8 find_peak(uint8 *history, uint8 size)
{
	uint8 min,max;
	uint8 i;
	uint8 rising, falling;
	uint8 value;
	uint8 peak;
	uint8 valley=0; //0 means we don't know where we are. 1=valley;  2=hill
	
	min=255;
	max=0;
	
	for(i=0;i<size;i++)
	{
		if(history[i]<min) min=history[i];
		if(history[i]>max) max=history[i];
	}
	if( (min<=0) && (max<=5) ) return 255;
	
	rising=falling=255;
	
	//find the rising/falling edges
	for(i=0;i<size;i++)
	{
		value = history[i];
		if(valley==0)
		{
			if(value >= max-3) valley=2; else valley=1; //we might be starting out on a hill....
		}
		if(valley==1)
		{
			if(value >= max-3) 
			{ 
				valley = 2;  
				rising = i; //found rising edge;
			}
		}
		if(valley==2)
		{
			if(value < max-3) 
			{ 
				valley = 1;  
				falling = i; 
			}
		}
	}

	if( (rising==255) || (falling==255) ) return 255;

	peak = rising + (falling-rising)/2;
	return peak;
}


void test_find_peak(void)
{
	printf("%d\n",find_peak(h1,30));
	printf("%d\n",find_peak(h2,30));
	printf("%d\n",find_peak(h3,30));
	return 0;
}




void test_keyboard(void)
{
	while(1)
	{
		unsigned int c;
		if(_kbhit())
		{
			c = _getch();
			printf("c = '%c' = 0x%02x\n",c,c);
		}
		Sleep(50);
	}
}
