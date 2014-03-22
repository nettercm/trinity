
#include "standard_includes.h"

extern uint8 align_to_line(uint8 cmd);

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


u08 data[256];
void test_flame(void)
{
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
		scan_result = find_peak_in_scan(d,12,3);
		NOP();
	}
}




t_scan_result find_peak_in_scan(t_scan *data, u16 number_of_points, uint8 threashold)
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
		if(data[i].flame<min) min=data[i].flame;
		if(data[i].flame>max) max=data[i].flame;
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


t_peak find_peak(uint8 *data, uint16 size, uint8 threashold)
{
	uint8 min,max;
	uint16 i;
	uint16 rising_edge_position, falling_edge_position;
	uint8 value;
	uint16 position;
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
//TODO: the following needs to be rewritting to match the 2014 platform
uint8 hone_in_on_candle(uint8 cmd, uint8 range)
{
	//use an array of 150 to maintain history 	
	static uint8 history1[150];
	static uint8 history2[150];
	static uint8 min1,max1,min2,max2;
	uint8 measurement;
	//uint8 center1,center2;
	static uint8 state=0,last_state=0;
	uint16 position;
	uint8 peak;
	
	if(range > 150) range = 150;
	
	//if behavior gets turned off, i.e. we are in a running state and now the enabled flag goes to zero, go to the stopped state
	if(state!=0 && cmd==0) 
	{
		state = 0;
	}		
	
	switch(state)
	{
		case 0:  //stopped
			if(cmd==1)
			{
				//stop motors and reset encoders
				motors_stop();
				//encoders_reset();
				min1 = min2 = 255;
				max1 = max2 = 0;
				//motors_set(100,-100);
				motor_command(4 , range/2,range/2, -120, 120);
				state = 1;
			}
			break;

		case 1:
			if(s.motor_command_state==0) 
			{
				state = 2;
				encoders_reset();
				motors_set(120,-120);
			}			
			break;
			
		case 2:
			position = (abs(s.inputs.encoders[0]) + abs(s.inputs.encoders[1]))/2;
			if(position>=range)  {state = 3;  break; }
			measurement = s.inputs.analog[6];
			history1[position] = measurement;
			if(measurement > max1) max1 = measurement;
			if(measurement < min1) min1 = measurement;
			measurement = s.inputs.analog[7];
			history2[position] = measurement;
			if(measurement > max2) max2 = measurement;
			if(measurement < min2) min2 = measurement;
			break;

		case 3:
			motors_stop();
			//usb_printf("min/max= %d,%d ; %d,%d\r\n\n",min1,max1,min2,max2);
			if(max1 > max2) peak = find_peak(history1,range); else peak = find_peak(history2,range);
			motor_command(4 , (range+1)-peak,(range+1)-peak, -120, 120);
			state = 4;
			break;		
			
		case 4:
			if(s.motor_command_state == 0)
			{
				if( (max1>3) || (max2>3)) state = 5; 
				else state=6;
			}
			break;	
			
		case 5: //found candle
			break;
			
		case 6: //candle not found
			break; 
	}		
	//s.inputs.fsm_states[7] = state;
	
	if(state != last_state)
	{
		usb_printf("%08ld: hone_in_on_candle(): ls,s=%d,%d\r\n", get_ms(), last_state,state);
		last_state=state;
	}
	return state;
}


//#define STATE_HAS_CHANGED ( last_state != state ? 1 : 0)
#define TIMEOUT(t) (delta_t > t ? t_last=t_now : 0)

uint8 find_flame_fsm(uint8 cmd)
{
	static uint8 state=0, last_state=0;
	uint8 result1,result2;
	static uint32 t_last;
	uint32 t_now, delta_t;
	uint8 STATE_HAS_CHANGED;
	
	//if behavior gets turned off, i.e. we are in a running state and now the enabled flag goes to zero, go to the stopped state
	if(state!=0 && cmd==0)
	{
		state = 0;
		t_last = get_ms();
	}
	
	t_now = get_ms();
	delta_t = t_now - t_last;
	
	STATE_HAS_CHANGED = ( last_state != state ? (last_state=state),1 : 0);
	
	switch(state)
	{
		/****************************************************************************/
		case 0: //waiting for start signal	
		
		if(STATE_HAS_CHANGED) {}
		
		if(cmd==1) 
		{
			if( TIMEOUT(500) ) state = 1;
		}		
		break;
		/****************************************************************************/
		
		
		/****************************************************************************/
		case 1: //move just a little over the door line
		
		if(STATE_HAS_CHANGED)
		{
			//move a small amount to clear the line - otherwise we can't be 100% sure
			//if we are seeing the line or the candle circle
			motors_stop();
			motor_command(4,10,10,140,150);
			ultrasonic_set_sequence(us_sequence_N_priority);
			t_last = t_now;
		}
		if( (s.motor_command_state == 0)  && (TIMEOUT(1500)) ) state = 2;
		break;
		/****************************************************************************/
		
		
		/****************************************************************************/
		case 2: //move a little into the room
		
		if(STATE_HAS_CHANGED)
		{
			//move 10" into the 
			t_last = t_now;
			motor_command(4,50,50,140,150);
			align_to_line(0);
		}
		//if( align_to_line(1) == 3) state = 
		//if( (s.inputs.sonar[US_N] < 30) ) motors_set(0,0);
		if( (s.motor_command_state == 0)  && (TIMEOUT(1500)) ) state = 3;

		break;
		/****************************************************************************/
		
		
		/****************************************************************************/
		case 3: //home in on candle
		
		if(STATE_HAS_CHANGED)
		{
			result1 = hone_in_on_candle(0,0);
		}
		result1 = hone_in_on_candle(1,150);
		if(result1 > 4 )
		{
			hone_in_on_candle(0,0); //stop this behavior
			if(result1==6) //not found
			{
				
			}
			state = 4;
		}
		break;
		/****************************************************************************/						
		
		
		/****************************************************************************/
		case 4:
		
		if(STATE_HAS_CHANGED)
		{
			
		}

		break;
		/****************************************************************************/		
	}
	//s.inputs.fsm_states[6] = state;
	
	if(STATE_HAS_CHANGED)
	{
		usb_printf("%08ld: find_flame_fsm()L ls,s=%d,%d\r\n", get_ms(), last_state,state);
	}
	return state;			
}
#endif

