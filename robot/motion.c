
#include "standard_includes.h"


u08 move_manneuver(u08 cmd, s16 speed, float distance)
{
	static u08 state=0;
	static s16 sign=1;

	if(cmd==1) //start
	{
		odometry_set_checkpoint(); 
		if(distance < 0) sign=-1; else sign=1;
		motor_command(7,2,2,(10)*sign,(10)*sign);
		motor_command(6,1,1,(speed)*sign,(speed)*sign);
		state = 1;
	}
	else
	{
		if     (( fabs(odometry_get_distance_since_checkpoint()) >= fabs(distance)    )) { motor_command(2,0,0, 0,  0); state = 0; } //done
		else if(( fabs(odometry_get_distance_since_checkpoint()) >  fabs(distance)-40 ))   motor_command(7,1,1, sign*10, sign*10 );
		else if(( fabs(odometry_get_distance_since_checkpoint()) >  fabs(distance)-90 ))   motor_command(6,1,1, sign*20,sign*20);
	}
	return state;
}


u08 move_manneuver2(u08 cmd, s16 speed, float distance, s16 safe_left, s16 safe_right)
{
	static u08 state=0;
	static s16 sign=1;
	static s16 bias=0;
	s16 ne, nw;

	ne = s.ir[IR_NE];
	nw = s.ir[IR_NW];

	if(cmd==1) //start
	{
		bias=0;
		if( (ne < safe_right) && (nw < safe_left) )
		{
			if (ne < nw)
			{
				bias = 5;
			}
			else
			{
				bias = -5;
			}
		}
		else if (ne < safe_right)
		{
			bias = 5;
		}
		else if (nw < safe_left)
		{
			bias = -5;
		}
		else
		{
			bias = 0;
		}
		motor_command(7,2,2,(10-bias)*sign,(10+bias)*sign); 
		odometry_set_checkpoint(); 
		if(distance < 0) sign=-1; else sign=1;
		state = 1;
		dbg_printf("move_manneuver2(c=%d, s=%d, d=%d, sl=%d, sr=%d)\n",cmd, speed, (int)distance, safe_left, safe_right);
	}
	else
	{
		if(state==1) 
		{	
			motor_command(7,1,1,((speed-bias)/2)*sign,((speed+bias)/2)*sign); 
			state++;
		}
		else if(state==2) 
		{ 
			motor_command(7,1,1,(speed-bias)*sign,(speed+bias)*sign); 
			state++;
		}
		else
		{
			if( (ne < safe_right) && (nw < safe_left) )
			{
				if (ne < nw)
				{
					bias = 5;
				}
				else
				{
					bias = -5;
				}
			}
			else if (ne < safe_right)
			{
				bias = 5;
			}
			else if (nw < safe_left)
			{
				bias = -5;
			}
			else if ( (ne<180) && (ne > safe_right + 5) )
			{
				bias = -3;
			}
			else if ( (nw<180) && (nw > safe_left + 5) )
			{
				bias = 3;
			}
			else
			{
				bias = 0;
			}
			if     (( fabs(odometry_get_distance_since_checkpoint()) >= fabs(distance)    )) { motor_command(2,0,0, 0,  0); state = 0; } //done
			else if(( fabs(odometry_get_distance_since_checkpoint()) >  fabs(distance)-40 ))   motor_command(7,1,1, sign*(10-bias), sign*(10+bias) );
			else if(( fabs(odometry_get_distance_since_checkpoint()) >  fabs(distance)-90 ))   motor_command(7,1,1, sign*(20-bias), sign*(20+bias));
			else motor_command(7,1,1,(speed-bias)*sign,(speed+bias)*sign);

			if(s.ir[IR_N] < 60) { motor_command(2,0,0, 0, 0); state = 0; }
		}
	}
	return state;
}

u08 turn_in_place_manneuver(u08 cmd, s16 speed, float angle)
{
	static u08 state=0;
	static s16 sign=1;
	float rotation;

	if(cmd==1) //initialize the state
	{
		odometry_set_checkpoint(); 
		if(angle < 0) sign=-1; else sign=1;
		motor_command(7,2,2,(10)*-sign,(10)*sign);
		motor_command(6,2,2,(speed)*-sign,(speed)*sign);
		state = 1;
	}
	else //update
	{
		rotation = fabs(odometry_get_rotation_since_checkpoint());
		if     ((  rotation >= fabs(angle)-1    )) 
		{ 
			motor_command(2,0,0, 0,  0); state = 0; 
		} //done
		//else if(( fabs(odometry_get_rotation_since_checkpoint()) >  fabs(angle)- 5))   motor_command(7,1,1, -sign*5, sign*5 );
		else if(( rotation >  fabs(angle)-10))   
		{
			motor_command(7,3,3, -sign*5, sign*5 );
		}
		//else if(( fabs(odometry_get_rotation_since_checkpoint()) >  fabs(angle)/4 ))   motor_command(6,1,1, -sign*15, sign*15 );
		else if(( rotation >  fabs(angle)-20 ))   
		{
			motor_command(7,3,3, -sign*(speed/2),sign*(speed/2));
		}
	}
	return state;
}




t_scan_result find_path_in_scan(t_scan *data, u16 number_of_points, u16 threashold, u16 hysteresis, u08 use_far_north)
{
	u08 state=1;
	s16 angle1, angle2, opening;
	u16 i;
	u16 distance;
	t_scan_result result = {0,0,0,0,0,0,0,0}; //initialize w/ values that indicate that we did not find a peak
	//output:  starting and ending angle of the largest opening in the radar scan
	//iterate through the data points

	//when we find a data point that is greater than the threashold, start tracking this opening

	for(i=0;i<number_of_points;i++)
	{
		distance = (use_far_north ? data[i].ir_far_north : data[i].ir_north);
		if(state == 1)
		{
			if(distance >= threashold+hysteresis) { state=2;  angle1=data[i].angle; }
		}
		else if(state == 2)
		{
			if( (distance < threashold-hysteresis) || (i==number_of_points) ) 
			{ 
				state = 1; 
				angle2 = data[i-1].angle; 
				opening = abs(abs(angle2)-abs(angle1));
				if(opening > result.opening) 
				{
					result.opening=opening;
					result.rising_edge_angle = angle1;
					result.falling_edge_angle = angle2;
					result.center_angle	= (result.rising_edge_angle + result.falling_edge_angle)/2;
				}
			}
		}
	}
	dbg_printf("find_path_in_scan(t=%d,h=%d,fn=%d): %d,%d,%d,%d\n", 
		threashold,hysteresis,use_far_north,
		result.opening, result.center_angle, result.rising_edge_angle, result.falling_edge_angle);
	return result;
}