
#include "standard_includes.h"


extern uint8 align_to_line(uint8 cmd);

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

