
#include "standard_includes.h"


#define is_nth_iteration(counter, n) (counter++ >= (n) ?  counter=0, 1 : 0)
#define invalid_error_value 9999


void wall_follow_fsm(void)
{
	enum states { s_none=0, s_disabled=1, s_tracking_wall=2, s_lost_wall=3, s_turning_corner=4, s_turning_sharp_corner=5 };
	static enum states state=s_disabled;
	static enum states last_state=s_none;
	//static u08 state=1, last_state=0;
	
	s16 error=0, e1=0, e2=0, correction=0;
	u08 at_limit_flag=0;
	static s16 target_speed=0;
	static u08 which_wall = 0; //left
	static s16 side=0, front=0;
	
	DEFINE_CFG2(u08,interval,			10,1);			DEFINE_CFG2(s16,nominal_speed,		10,2);
	DEFINE_CFG2(s16,target_distance,	10,3);			DEFINE_CFG2(s16,max_error,			10,4);
	DEFINE_CFG2(s16,max_correction,		10,5);			DEFINE_CFG2(s16,Kp,					10,6);
	DEFINE_CFG2(s16,Ki,					10,7);			DEFINE_CFG2(s16,Kd,					10,8);
	DEFINE_CFG2(s16,min_speed,			10,9);			DEFINE_CFG2(s16,up_ramp,			10,10);
	DEFINE_CFG2(s16,down_ramp,			10,11);			DEFINE_CFG2(u08,use_corner_logic,	10,12);
	DEFINE_CFG2(flt,corner_distance,	10,13);			DEFINE_CFG2(s16,corner_speed,		10,14);
	DEFINE_CFG2(s16,integral_limit,		10,15);			DEFINE_CFG2(s16,lost_wall_distance,	10,16);
	DEFINE_CFG2(s16,found_wall_distance,10,17);			DEFINE_CFG2(s16,corner_radius,		10,18);
	DEFINE_CFG2(s16,sharp_corner_radius,10,19);
	
	static s16 integral=0;
	static s16 last_error=invalid_error_value;
	

	
	//coder here gets executing every time this "task" runs


	//code that comes after this resumes where it left off
	task_open();
	
	PREPARE_CFG2(interval);					PREPARE_CFG2(nominal_speed);
	PREPARE_CFG2(target_distance);			PREPARE_CFG2(max_error);
	PREPARE_CFG2(max_correction);			PREPARE_CFG2(Kp);
	PREPARE_CFG2(Ki);						PREPARE_CFG2(Kd);
	PREPARE_CFG2(min_speed);				PREPARE_CFG2(up_ramp);
	PREPARE_CFG2(down_ramp);				PREPARE_CFG2(use_corner_logic);
	PREPARE_CFG2(corner_distance);			PREPARE_CFG2(corner_speed);
	PREPARE_CFG2(integral_limit);			PREPARE_CFG2(lost_wall_distance);
	PREPARE_CFG2(found_wall_distance);		PREPARE_CFG2(corner_radius);
	PREPARE_CFG2(sharp_corner_radius);
	
	while(1)
	{
		UPDATE_CFG2(interval);					UPDATE_CFG2(nominal_speed);
		UPDATE_CFG2(target_distance);			UPDATE_CFG2(max_error);
		UPDATE_CFG2(max_correction);			UPDATE_CFG2(Kp);
		UPDATE_CFG2(Ki);						UPDATE_CFG2(Kd);
		UPDATE_CFG2(min_speed);					UPDATE_CFG2(up_ramp);
		UPDATE_CFG2(down_ramp);					UPDATE_CFG2(use_corner_logic);
		UPDATE_CFG2(corner_distance);			UPDATE_CFG2(corner_speed);
		UPDATE_CFG2(integral_limit);			UPDATE_CFG2(lost_wall_distance);
		UPDATE_CFG2(found_wall_distance);		UPDATE_CFG2(corner_radius);
		UPDATE_CFG2(sharp_corner_radius);

		
		//the following state transition applies to all states
		if(s.behavior_state[1]==0) state = s_disabled;
		
		if(button_is_pressed(TOP_BUTTON))
		{
			play_note(A(3), 50, 10);
			s.behavior_state[1] = 1; 
			state = s_disabled;
		}
		
		side =  (which_wall == 0 ? s.ir[AI_IR_NW] : s.ir[AI_IR_NE]);
		front = s.ir[AI_IR_N];
		
		/*
		state 1: behavior disabled - waiting for enable flag to be set
			entry: stop motor
			exit:  start moving at minimal speed (1)
			during:  n/a
		*/
		first_(s_disabled)
		{
			enter_(s_disabled) { motor_command(2,0,0,0,0); }
			which_wall = s.behavior_state[2];
			if(s.behavior_state[1]==1) state = s_tracking_wall;
			exit_(s_disabled)  { motor_command(8,1000,0,10,10); target_speed = 10; }
		}
		
		
		/*	
		state 2: tracking wall - looking for door/intersection 
			entry: n/a
			exit: n/a
			during:  
				regulate speed & heading to maintain distance (includes turning on inside corners)
				if distance from wall is > X, then goto state 3
		*/
		next_(s_tracking_wall)
		{
			enter_(s_tracking_wall) 
			{ 
				play_note(C(3), 50, 10);
				integral = 0;
				last_error = invalid_error_value;
			}
							
			if( (use_corner_logic) && (side > lost_wall_distance) ) { state = s_lost_wall;  leave_(s_tracking_wall); }

			e1 = side-target_distance;
			e2 = front -(target_distance+30);
			
			//if there is something right in front, slow down; otherwise speed up
			if( (front < 140)  ||  (abs(e1)>30) ) 
			{
				if(target_speed > min_speed) target_speed -= down_ramp;
			}				
			else 
			{
				//ramp
				if(target_speed < nominal_speed) target_speed += up_ramp;
				//if( (abs(error) > 20) && (target_speed > 30) ) target_speed -= 1;
			}				

			if( (e2<0) && (e2<e1) ) error = e2;
			else error = e1;
						
			LIMIT(error,-max_error,+max_error);
			
			if(last_error == invalid_error_value) last_error = error; //initialize the derivative if necessary
			
			correction = (Kp*error)/100 + (Ki*integral)/100 + (Kd*(error-last_error))/100;
			LIMIT2( correction , -((target_speed*max_correction)/100) , +((target_speed*max_correction)/100) , at_limit_flag );

			last_error = error;
			
			if(at_limit_flag == 0) //don't wind up the integral if we are already saturating the correction
			{
				integral += error;
				LIMIT(integral,-integral_limit,integral_limit);
			}

			if(which_wall == 1) correction *= -1; //reverse the motor command if we are tracking the right wall
			
			if(correction > 0)
			{
				motor_command(8,1000,0,target_speed-correction,target_speed+(correction*0));
			}
			else
			{
				motor_command(8,1000,0,target_speed-(correction*0),target_speed+correction);
			}

			exit_(s_tracking_wall) { }
		}
		
		
		/*
		state 3: potentially reached door/intersection - need to confirm
			entry: reset odometry
			exit: n/a
			during:
				slow down
				go straight
				if we have traveled more than Y inches, then goto state 4
				if something is right in front then go to sate 2 ?????
				if distance from wall is < X, then assume "false alarm" and go back to state 2
		*/		
		next_(s_lost_wall)
		{
			enter_(s_lost_wall) 
			{ 
				//s.inputs.x = s.inputs.y = s.inputs.theta = 0;  
				odometry_set_checkpoint();
				play_note(E(3), 50, 10); 
			}
			
			if(target_speed > corner_speed) target_speed -= down_ramp;
			if(target_speed < corner_speed) target_speed += up_ramp;
			motor_command(8,0,0,target_speed,target_speed);
			//if( s.inputs.x >=  corner_distance) state = s_turning_corner;
			if( odometry_get_distance_since_checkpoint() >= corner_distance ) state = s_turning_corner;
			if( side <= found_wall_distance ) state = s_tracking_wall;
			//if( s.ir[AI_IR_N] <= 50 ) state = 2;

			exit_(s_lost_wall) { }	
		}
		
		
		/*
		state 4: turn the corner
			entry: reset odometry
			during:
				if we have turned 90 degrees, then go to state 2
				if something is right in front then go to sate 2 ?????
				if distance from wall is < X, then assume it wasn't a corner or not a 90deg corner and goto state 2
		*/		
		next_(s_turning_corner)
		{
			enter_(s_turning_corner) 
			{ 
				play_note(G(3), 50, 10);
				//s.inputs.x = s.inputs.y = s.inputs.theta = 0; 
				odometry_set_checkpoint();
			}
			
			if(target_speed > corner_speed) target_speed -= down_ramp;
			if(target_speed < corner_speed) target_speed += up_ramp;
			if(which_wall==0) motor_command(8,0,0,(target_speed*10)/corner_radius,target_speed);
			else motor_command(8,0,0,target_speed,(target_speed*10)/corner_radius);
	
			if( side <= target_distance ) state = s_tracking_wall;
			if ( abs(odometry_get_rotation_since_checkpoint()) >= 90 ) 
			{
				state = s_tracking_wall; //by default, go back to tracking the wall, unless....
				if( side > lost_wall_distance) state = s_turning_sharp_corner;
			}			
			//if( s.ir[AI_IR_N] <= 50 ) state = 2;
			
			exit_(s_turning_corner) { }
		}


		next_(s_turning_sharp_corner)
		{
			enter_(s_turning_sharp_corner)
			{
				play_note(C(4), 50, 10);
				//s.inputs.x = s.inputs.y = s.inputs.theta = 0; 
				odometry_set_checkpoint();
			}
			if(target_speed > corner_speed) target_speed -= down_ramp;
			if(target_speed < corner_speed) target_speed += up_ramp;
			if(which_wall==0) motor_command(8,0,0,(target_speed*10)/sharp_corner_radius,target_speed);
			else motor_command(8,0,0,target_speed,(target_speed*10)/sharp_corner_radius);
			
			if( side <= target_distance ) state = s_tracking_wall;
			if ( abs(odometry_get_rotation_since_checkpoint()) >= 90 ) 
			{
				state = s_tracking_wall; //by default, go back to racking the wall, unless....
				if( side > found_wall_distance) state = s_turning_corner;
			}
			//if( s.ir[AI_IR_N] <= 50 ) state = 2;
			
		exit_(s_turning_sharp_corner) { }
	}
		
		s.inputs.watch[0]=error;
		s.inputs.watch[1]=integral;
		s.inputs.watch[2]=correction;
		s.inputs.watch[3]=state;
			
		task_wait(interval);
	}
	
	task_close();
}








//the following is some code from the 2013 verson of the robot.
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