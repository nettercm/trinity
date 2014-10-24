
#include "standard_includes.h"


#define is_nth_iteration(counter, n) (counter++ >= (n) ?  counter=0, 1 : 0)
#define invalid_error_value 9999


void wall_follow_fsm(u08 cmd, u08 *param)
{
	enum states { s_disabled=0, s_tracking_wall=1, s_lost_wall=2, s_turning_corner=3, s_turning_sharp_corner=4 };
	static enum states state=s_disabled;
	static enum states last_state=s_disabled;
	static u32 t_entry=0;
	s16 error=0, e1=0, e2=0, correction=0;
	u08 at_limit_flag=0;
	static s16 target_speed=0;
	static u08 which_wall = LEFT_WALL; //left
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
	static u08 initialized=0;
	

	//coder here gets executing every time this "task" runs
	
	//code that comes after this resumes where it left off
	//task_open();

	if(!initialized)
	{
		initialized=1;
		usb_printf("wall_follow_fsm()\n");
	
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
	}

	//while(1)
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
		if(s.behavior_state[FOLLOW_WALL]==0) state = s_disabled;
		
		side =  (which_wall == LEFT_WALL ? s.ir[AI_IR_NW] : s.ir[AI_IR_NE]);
		front = s.ir[AI_IR_N];
		

		first_(s_disabled)
		{
			enter_(s_disabled) { motor_command(6,2,2,0,0); }
			if(s.behavior_state[FOLLOW_WALL]!=0)
			{
				which_wall = s.behavior_state[FOLLOW_WALL];
				state = s_tracking_wall;
			}
			exit_(s_disabled)  { motor_command(8,1000,0,min_speed/2,min_speed/2); target_speed = min_speed; }
		}
		
		
		next_(s_tracking_wall)
		{
			enter_(s_tracking_wall) 
			{ 
				integral = 0;
				last_error = invalid_error_value;
			}
							
			if( (use_corner_logic) && (side > lost_wall_distance) ) { state = s_lost_wall;  leave_(s_tracking_wall); }

			e1 = side-target_distance;
			e2 = front -(target_distance+40);
			
			//if there is something right in front, slow down; otherwise speed up
			if( (front < 160)  ||  (abs(e1)>40) ) 
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

			if(which_wall == RIGHT_WALL) correction *= -1; //reverse the motor command if we are tracking the right wall
			
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
		
		
		next_(s_lost_wall)
		{
			enter_(s_lost_wall) 
			{ 
				odometry_set_checkpoint();
			}
			
			if(target_speed > corner_speed) target_speed -= down_ramp;
			if(target_speed < corner_speed) target_speed += up_ramp;
			motor_command(8,0,0,target_speed,target_speed);
			if( odometry_get_distance_since_checkpoint() >= corner_distance ) state = s_turning_corner;
			if( side <= found_wall_distance ) state = s_tracking_wall;

			exit_(s_lost_wall) { }	
		}
		
		
		next_(s_turning_corner)
		{
			enter_(s_turning_corner) { odometry_set_checkpoint(); }
			
			if(target_speed > corner_speed) target_speed -= down_ramp;
			if(target_speed < corner_speed) target_speed += up_ramp;
			if(which_wall==LEFT_WALL) motor_command(8,0,0,(target_speed*10)/corner_radius,target_speed);
			else motor_command(8,0,0,target_speed,(target_speed*10)/corner_radius);
	
			if( side <= target_distance ) state = s_tracking_wall;
			if ( abs(odometry_get_rotation_since_checkpoint()) >= 90 ) 
			{
				state = s_tracking_wall; //by default, go back to tracking the wall, unless....
				if( side > lost_wall_distance) state = s_turning_sharp_corner;
			}			
			
			exit_(s_turning_corner) { }
		}


		next_(s_turning_sharp_corner)
		{
			enter_(s_turning_sharp_corner)	{ odometry_set_checkpoint(); }

			if( target_speed > corner_speed )	target_speed -= down_ramp;
			if( target_speed < corner_speed )	target_speed += up_ramp;
			if( which_wall==LEFT_WALL )			motor_command(8,0,0,(target_speed*10)/sharp_corner_radius,target_speed);
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
		/*	
		s.inputs.watch[0]=error;
		s.inputs.watch[1]=integral;
		s.inputs.watch[2]=correction;
		*/
		s.inputs.watch[3]=state;
		//task_wait(interval);
	}
	
	//task_close();
}


