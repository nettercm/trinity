
#include "standard_includes.h"



#if 0

uint8 follow_left_wall_using_ir(s16 min_distance, s16 max_distance, s16 correction)
{
	static uint32 t_last=0;
	sint16 speed, adj;
	
	//12,60,62 works ok
	//16,80,80 also works
	if( (get_ms() - t_last) >= 10 ) //don't bother running this faster than 100Hz
	{
		t_last = get_ms();
		speed = s.lm_target;
		adj   = abs(s.lm_target / correction);
		s.lm_actual = s.lm_target;
		s.rm_actual = s.rm_target;
		
		//if(s.inputs.analog[AI_IR_NW] < min_distance)
		if(s.ir[AI_IR_NW] > max_distance) //too far? => veer left
		{
			s.lm_actual = speed - adj;
		}
		//if(s.inputs.analog[AI_IR_NW] > max_distance)
		if(s.ir[AI_IR_NW] < min_distance) //too close? => veer right
		{
			s.rm_actual = speed - adj;
		}

		if(s.ir[AI_IR_NW] < 45) //way too close? => veer right more
		{
			s.rm_actual = speed - 4*adj;
			s.lm_actual = speed + 4*adj;
		}
		
		usb_printf("follow_left_wall_using_ir(): min,max,actual=%d,%d,%d\r\n\n",min_distance,max_distance,s.ir[AI_IR_NW]);
	}
	return 0;
}





uint8 follow_right_wall_using_ir(s16 min_distance, s16 max_distance, s16 correction)
{
	static uint32 t_last=0;
	sint16 speed, adj;
	
	//12,60,62 works ok
	//16,80,80 also works
	if( (get_ms() - t_last) >= 10 ) //don't bother running this faster than 100Hz
	{
		t_last = get_ms();
		speed = s.lm_target;
		adj   = abs(s.lm_target / correction);
		s.lm_actual = s.lm_target;
		s.rm_actual = s.rm_target;
		
		//if(s.inputs.analog[AI_IR_NW] < min_distance)
		if(s.ir[AI_IR_NE] > max_distance) //too far? => veer left
		{
			s.rm_actual = speed - adj;
		}
		//if(s.inputs.analog[AI_IR_NW] > max_distance)
		if(s.ir[AI_IR_NE] < min_distance) //too close? => veer right
		{
			s.lm_actual = speed - adj;
		}

		if(s.ir[AI_IR_NE] < 45) //way too close? => veer right more
		{
			s.lm_actual = speed - 4*adj;
			s.rm_actual = speed + 4*adj;
		}
		
		usb_printf("follow_left_wall_using_ir(): min,max,actual=%d,%d,%d\r\n\n",min_distance,max_distance,s.ir[AI_IR_NE]);
	}
	return 0;
}





uint8 follow_left_wall_and_turn(uint8 cmd)
{
	static uint8 state=0;
	static uint32 t_last;
	uint32 t_now, delta_t;
	
	//if behavior gets turned off, i.e. we are in a running state and now the enabled flag goes to zero, go to the stopped state
	if(state!=0 && cmd==0)
	{
		state = 0;
		motors_reapply_target_speed();
	}
	
	t_now = get_ms();
	delta_t = t_now - t_last;
	
	switch(state)
	{
		case 0:  //stopped
		if(cmd==1) 	state = 1;
		break;

		case 1:
		follow_left_wall_using_ir(105,105,16);
		if(s.ir[AI_IR_NW] > 160)
		{
			motors_reapply_target_speed();
			encoders_set_checkpoint();
			state = 2;
		}
		break;

		case 2:
		if(s.ir[AI_IR_NW] < 130) //false alarm - didn't actually reach the corner yet
		{
			state = 1;
		}
		else if(encoders_get_distance_since_checkpoint()>60) //better to go a little further, otherwise we'll get stuck in the corner
		{
			s.lm_actual = 90;
			s.rm_actual = 140;
			encoders_set_checkpoint();
			state = 3;
		}
		break;

		case 3:
		if(encoders_get_distance_since_checkpoint()>140) //better to turn too much, otherwise we won't see the wall
		{
			//motors_stop();
			motors_reapply_target_speed();
			state = 4;
		}
		break;

		case 4:
			state = 1;
		break;
	}
	s.inputs.fsm_states[4] = state;
	return state;
}





uint8 follow_right_wall_and_turn(uint8 cmd)
{
	static uint8 state=0;
	static uint32 t_last;
	uint32 t_now, delta_t;
	
	//if behavior gets turned off, i.e. we are in a running state and now the enabled flag goes to zero, go to the stopped state
	if(state!=0 && cmd==0)
	{
		state = 0;
		motors_reapply_target_speed();
	}
	
	t_now = get_ms();
	delta_t = t_now - t_last;
	
	switch(state)
	{
		case 0:  //stopped
		if(cmd==1) 	state = 1;
		break;

		/*********************************************************************************************************************************************/
		case 1:

		follow_right_wall_using_ir(105,105,16);
		if(s.ir[AI_IR_NE] > 160)
		{
			motors_reapply_target_speed();
			encoders_set_checkpoint();
			state = 2;
		}
		break;


		/*********************************************************************************************************************************************/
		case 2:

		if(s.ir[AI_IR_NE] < 130) //false alarm - didn't actually reach the corner yet
		{
			state = 1;
		}
		else if(encoders_get_distance_since_checkpoint()>60) //better to go a little further, otherwise we'll get stuck in the corner
		{
			s.rm_actual = 90;
			s.lm_actual = 140;
			encoders_set_checkpoint();
			state = 3;
		}
		break;

		/*********************************************************************************************************************************************/
		case 3:

		if(  (encoders_get_distance_since_checkpoint()>130) || (s.ir[AI_IR_NE] < 50)) //better to turn too much, otherwise we won't see the wall
		{
			//motors_stop();
			motors_reapply_target_speed();
			state = 4;
		}
		break;

		/*********************************************************************************************************************************************/
		case 4:
		
			state = 1;
		break;
		
	}
	s.inputs.fsm_states[5] = state;
	return state;
}




uint8 follow_left_wall_using_us(uint16 min_distance, uint16 max_distance, sint16 correction)
{
	static uint16 us_cycles=0;
	uint16 distance=0;
	sint16 speed, adj;

	if(s.us_cycles != us_cycles) //did we complete one cycle of sonar updates? if not, don't bother
	{
		us_cycles = s.us_cycles;
		speed = s.lm_target;
		adj   = abs(s.lm_target / correction);
		s.lm_actual = s.lm_target;
		s.rm_actual = s.rm_target;
		
		distance = s.inputs.sonar[US_W]; //(distance*2 + s.inputs.sonar[4])/3;
		
		if(distance < min_distance) //too close?
		{
			if(s.inputs.sonar[US_W]+3 < s.inputs.sonar[US_SW])
			{
				s.rm_actual = speed - 2*adj;
			}
			else if(s.inputs.sonar[US_W] > s.inputs.sonar[US_SW]+3) //if we are already leaning away from wall, no speed change
			{
				
			}
			else s.rm_actual = speed - adj;
		}
		else if(distance > max_distance)
		{
			if(s.inputs.sonar[US_W] > s.inputs.sonar[US_SW]+3)
			{
				s.lm_actual = speed - 2*adj;
			}
			else if(s.inputs.sonar[US_W]+3 < s.inputs.sonar[US_SW]) //if we are already leaning towards from wall, no speed change
			{
				
			}
			else s.lm_actual = speed - adj;
		}
		else if(s.inputs.sonar[US_W] > s.inputs.sonar[US_SW]+1)
		{
			s.lm_actual = speed - adj;
		}
		else if(s.inputs.sonar[US_W] < s.inputs.sonar[US_SW])
		{
			s.rm_actual = speed - adj;
		}
		//usb_printf("d=%d, lm_t=%d, rm_t=%d\r\n\n",distance,s.lm_target,s.rm_target);
	}
}

#endif