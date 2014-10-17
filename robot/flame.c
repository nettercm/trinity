
#include "standard_includes.h"

extern uint8 align_to_line(uint8 cmd);



void test_flame(void)
{
	u08 data[256];
	int x,y;
	t_peak result;
	t_scan_result scan_result;

	memset(data,0,256);

	for(x=0;x<256;x++)
	{
		y=x*2;
		if(y>255) y=255;
		data[x] = y;
	}
	result = find_peak(data,256,3);

	{
		u08 d[] = {1,2,3,4,5,6,4,6,4,6,4,6,4,3,2,1,0};
		result = find_peak(d,sizeof(d),3);
		NOP();
	}
	{
		u08 d[] = {1,2,3,4,5,6,4,6,4,6,4,6,4,9,9,9,9};
		result = find_peak(d,sizeof(d),1);
		NOP();
	}
	{
		t_scan d[] = { {-90,1,2},{-80,1,4},{-75,1,6},{-60,1,8},{-40,1,8},{-25,1,8},{-22,1,8},{-20,1,6},{-10,1,2},{-00,1,2},{10,1,2},{20,1,0} };
		scan_result = find_flame_in_scan(d,12,3);
		NOP();
	}
	{
		t_scan d[100];
		int i;
		for(i=0;i<100;i++) { d[i].angle = i; d[i].ir_north=997; }
		scan_result = find_path_in_scan(d,100,100,0,0);
		NOP();
	}
	{
		t_scan d[] = { {-90,0,0},{-80,100,0},{-75,110,0},{-60,100,0},{-40,90,0},{-25,0,0},{-22,0,0},{-20,0,0},{-10,0,0},{-00,0,0},{10,0,0},{20,0,0} };
		scan_result = find_path_in_scan(d,12,100,0,0);
		NOP();
	}
}



t_scan_result find_flame_in_scan(t_scan *data, u16 number_of_points, uint8 threashold)
{
	uint8 min=255,max=0;
	uint16 i=0;
	uint8 value=0;
	u16 position1=0,position2=0;
	s16 rising_edge_abs_angle=0,falling_edge_abs_angle=0;
	t_scan_result result = {0,0,0,0,0,0,0,0,0}; //initialize w/ values that indicate that we did not find a peak
	
	min=255;
	max=0;
	
	for(i=0;i<number_of_points;i++)
	{
		if(data[i].flame<min) { min=data[i].flame; position1=i; }
		if(data[i].flame>max) { max=data[i].flame; position2=i; }
	}
	
	//if( (min<=0) && (max<=5) ) return result;
	
	result.rising_edge_position  = 0;
	result.falling_edge_position = number_of_points-1;

	//start from the peak and find the falling edge
	for(i=position2;i<number_of_points;i++)
	{
		value = data[i].flame;
		if(value < max-threashold)
		{
			result.falling_edge_position = i-1; ////...and mark this as the falling edge location
			result.falling_edge_angle = data[i-1].angle;
			falling_edge_abs_angle = data[i-1].abs_angle;
			break;
		}
	}

	//start from the peak and find the rising edge
	for(i=position2+1;i>0;i--)
	{
		value = data[i-1].flame;
		if(value < max-threashold)
		{
			result.rising_edge_position = i; ////...and mark this as the falling edge location
			result.rising_edge_angle = data[i].angle;
			rising_edge_abs_angle = data[i].abs_angle;
			break;
		}
	}

	position1					= result.rising_edge_position + ( result.falling_edge_position   - result.rising_edge_position)/2;
	position2					= result.rising_edge_position + ((result.falling_edge_position+1)- result.rising_edge_position)/2;
	result.center_position		= (position1+position2)/2;
	result.center_angle			= (result.rising_edge_angle + result.falling_edge_angle)/2;
	result.center_abs_angle		= rising_edge_abs_angle + (abs(rising_edge_abs_angle-falling_edge_abs_angle))/2;
	if(result.center_abs_angle >= 360) result.center_abs_angle-=360;
	result.flame_center_value	= max;

	dbg_printf("find_flame_in_scan(np=%d, t=%d):  cv=%d,ca=%d,rea=%d,fea=%d, abs_angle=%d\n", 
		number_of_points, threashold,
		result.flame_center_value,  result.center_angle,  result.rising_edge_angle,  result.falling_edge_angle, result.center_abs_angle);

	return result;
}





t_peak find_peak(uint8 *data, uint16 size, uint8 threashold)
{
	uint8 min,max;
	uint16 i;
	uint16 rising_edge_position, falling_edge_position;
	uint8 value;
	uint8 state=0; //0 means we don't know where we are. 1=valley;  2=hill
	t_peak result = {255, 255}; //initialize w/ values that indicate that we did not find a peak
	
	min=255;
	max=0;
	
	for(i=0;i<size;i++)
	{
		if(data[i]<min) min=data[i];
		if(data[i]>max) max=data[i];
	}
	if( (min<=0) && (max<=5) ) return result;
	
	rising_edge_position  = 0;
	falling_edge_position = size-1;
	
	//find the rising/falling edges
	//we'll keep going until all the data points have been processed
	//as a result of that, only the location of the last peak will be remembered
	for(i=0;i<size;i++)
	{
		value = data[i];
		if(state==0) //if we don't know where we are, let's first initialize the state
		{
			if(value >= max-threashold) state=2; //we could be starting out on a hill....
			else state=1;				//...or not
		}

		if(state==1) //if we are currently in a valley...
		{
			if(value >= max-threashold) //...and if we suddently reach a point near the peak....
			{ 
				state = 2;  //...then let's say we have reached the peak...
				rising_edge_position = i; //...and mark this as the rising edge location
			}			
		}

		if(state==2)  //if we are currently on are near the peak....
		{
			if(value < max-threashold) //...and if we then suddenly drop below the peak....
			{ 
				state = 1;  //..then let's assume we are back in a valley
				falling_edge_position = i; ////...and mark this as the falling edge location
			}
		}
	}

	//if( (rising_edge_position==size) /*|| (falling_edge_position==size)*/ ) return result;

	result.position = rising_edge_position + (falling_edge_position-rising_edge_position)/2;
	result.value    = max;

	return result;
}

#if 0
t_scan_result find_peak_in_scan_v1(t_scan *data, u16 number_of_points, uint8 threashold)
{
	uint8 min,max;
	uint16 i;
	uint8 value;
	u16 position1,position2;
	uint8 state=0; //0 means we don't know where we are. 1=valley;  2=hill
	t_scan_result result = {0,0,0,0,0,0,0}; //initialize w/ values that indicate that we did not find a peak
	
	min=255;
	max=0;
	
	for(i=0;i<number_of_points;i++)
	{
		if(data[i].flame<min) { min=data[i].flame; position1=i; }
		if(data[i].flame>max) { max=data[i].flame; position2=i; }
	}
	
	//if( (min<=0) && (max<=5) ) return result;
	
	result.rising_edge_position  = 0;
	result.falling_edge_position = number_of_points-1;
	
	//find the rising/falling edges
	//we'll keep going until all the data points have been processed
	//as a result of that, only the location of the last peak will be remembered
	for(i=0;i<number_of_points;i++)
	{
		value = data[i].flame;
		if(state==0) //if we don't know where we are, let's first initialize the state
		{
			if(value >= max-threashold) state=2; //we could be starting out on a hill....
			else state=1;				//...or not
		}

		if(state==1) //if we are currently in a valley...
		{
			if(value >= max-threashold) //...and if we suddently reach a point near the peak....
			{ 
				state = 2;  //...then let's say we have reached the peak...
				result.rising_edge_position = i; //...and mark this as the rising edge location
				result.rising_edge_angle = data[i].angle;
			}			
		}

		if(state==2)  //if we are currently on are near the peak....
		{
			if(value < max-threashold) //...and if we then suddenly drop below the peak....
			{ 
				state = 1;  //..then let's assume we are back in a valley
				result.falling_edge_position = i-1; ////...and mark this as the falling edge location
				result.falling_edge_angle = data[i-1].angle;
			}
		}
	}

	//if( (rising_edge_position==size) /*|| (falling_edge_position==size)*/ ) return result;

	position1					= result.rising_edge_position + ( result.falling_edge_position   - result.rising_edge_position)/2;
	position2					= result.rising_edge_position + ((result.falling_edge_position+1)- result.rising_edge_position)/2;
	result.center_position		= (position1+position2)/2;
	result.center_angle			= (result.rising_edge_angle + result.falling_edge_angle)/2;
	result.flame_center_value	= max;

	return result;
}
#endif


#if 0

void uvtron_update(void)
{
	uint8 p;
	static uint32 t=0;
	static uint8 count=0, last_count=0;
	static uint32 pulse;
	p=new_pulse(UV_PULSE_CHANNEL);
	if(p & HIGH_PULSE)
	{
		pulse = get_last_high_pulse(UV_PULSE_CHANNEL);
		//pulse=pulse_to_microseconds(pulse);
		count++;
		//DBG_printf(1,(_b,"%d\r\n\n",count));
		//pulse=pulse/100;
		//s.inputs.p1 = pulse;
	}
	if(get_ms() - t >= 1000)
	{
		t=get_ms();
		//usb_printf("%d counts per second\r\n\n",count-last_count);
		//s.inputs.p1 = count-last_count;
		last_count = count;
	}
}


#endif

