
#include "standard_includes.h"


DEFINE_CFG2(flt,room_entry_distance,	7,1);			
DEFINE_CFG2(u08,omni_flame_threashold,	7,2);			
DEFINE_CFG2(u08,pan_left,				7,3);			
DEFINE_CFG2(u08,pan_right,				7,4);			
DEFINE_CFG2(u08,tilt_up,				7,5);			
DEFINE_CFG2(u08,tilt_down,				7,6);			
DEFINE_CFG2(u16,pan_tilt_speed,			7,7);			
DEFINE_CFG2(u08,flame_threshold_1,		7,8);			
DEFINE_CFG2(u08,flame_threshold_2,		7,9);			
DEFINE_CFG2(u16,stopping_distance,		7,10);
DEFINE_CFG2(u16, ir_wall_threashold,	7, 11);
DEFINE_CFG2(u16, us_wall_threashold,	7, 12);
DEFINE_CFG2(u16, flame_scan_moving_avg, 7, 13);


extern uint8 align_to_line(uint8 cmd);
extern void scan(u08 cmd, u16 moving_avg);


typedef struct
{
	u16 x;
	u16 y;
} t_pan_tilt_pos;

//pan-center = 125
//tilt-center = 145
//Ly: -10 ... 40  (-10 = top) = 140...155    Lx: -10....30 (-10 = right) = 115...140   top-left  x,y = 30,-10 bottom-right = -10,40
t_pan_tilt_pos pan_tilt_pos[]=
{
	{150, 155}, //{140, 140},  //bl
	{110, 155}, //{125, 140},  //br
	//{130, 145}, //{110, 140},  //center

	{110, 135}, //{125, 148},  //tr
	{150, 135} //{110, 148},  //tl
	//{130, 145}, //{110, 140},  //center
};


u08 is_flame_present(void)
{
#ifdef WIN32
	if ((s.current_room == m.candle_location) && (m.candle_location > 0))
	{
		return 1;
	}
	else return 0;
#endif
	if (s.inputs.analog[AI_FLAME_N] > omni_flame_threashold) //TODO: use parameter to define the threshold for the omni flame sensor
	{
		return 1;
	}
	else return 0;
}



void track_candle(void)
{
	s16 bias;
	static s16 ne=0, nw=0;

	ne = (ne + (s16) s.inputs.analog[AI_FLAME_NE])/2;
	nw = (nw + (s16) s.inputs.analog[AI_FLAME_NW])/2;
	bias = 0;
	if( (ne>245) && (nw>245) ) bias = 0;
	else if( abs(ne-nw) < 10 ) bias = 0;
	else if( ne>nw ) bias = -1;
	else if( nw>ne ) bias =  1;
	pan_relative(bias);
}



void find_flame_fsm(u08 cmd, u08 *param)
{
	enum states { s_disabled = 0, s_scanning_v2 = 1, s_scanning, s_moving_towards_candle, s_extinguish, s_turning_sharp_corner };
	static enum states state=s_disabled;
	static enum states last_state=s_disabled;
	static u32 t_entry=0;
	static u08 initialized=0;
	static s16 bias=0,  last_bias=0;
	static u08 count;
	static u32 t_start;
	static s16 pan_offset, tilt_offset;
	static t_scan_result scan_result;
	static u16 wall;
	static u08 flame_is_out = 0;
	u32 t_delta;
	u08 i;
	t_config_value v;

	task_open_1();

	//DEFINE_CFG2(s16,sharp_corner_radius,10,19);

	if(!initialized)
	{
		initialized=1;
		usb_printf("find_flame_fsm()\n");

		PREPARE_CFG2(room_entry_distance);
		PREPARE_CFG2(omni_flame_threashold);
		PREPARE_CFG2(pan_left);
		PREPARE_CFG2(pan_right);
		PREPARE_CFG2(tilt_up);
		PREPARE_CFG2(tilt_down);
		PREPARE_CFG2(pan_tilt_speed);
		PREPARE_CFG2(flame_threshold_1);
		PREPARE_CFG2(flame_threshold_2);
		PREPARE_CFG2(stopping_distance);
		PREPARE_CFG2(ir_wall_threashold);
		PREPARE_CFG2(us_wall_threashold);
		PREPARE_CFG2(flame_scan_moving_avg);
	}

	UPDATE_CFG2(room_entry_distance);
	UPDATE_CFG2(omni_flame_threashold);
	UPDATE_CFG2(pan_left);
	UPDATE_CFG2(pan_right);
	UPDATE_CFG2(tilt_up);
	UPDATE_CFG2(tilt_down);
	UPDATE_CFG2(pan_tilt_speed);
	UPDATE_CFG2(flame_threshold_1);
	UPDATE_CFG2(flame_threshold_2);
	UPDATE_CFG2(stopping_distance);
	UPDATE_CFG2(ir_wall_threashold);
	UPDATE_CFG2(us_wall_threashold);
	UPDATE_CFG2(flame_scan_moving_avg);

	pan_tilt_pos[0].x = pan_left;
	pan_tilt_pos[0].y = tilt_down;
	pan_tilt_pos[1].x = pan_right;
	pan_tilt_pos[1].y = tilt_down;
	pan_tilt_pos[2].x = pan_right;
	pan_tilt_pos[2].y = tilt_up;
	pan_tilt_pos[3].x = pan_left;
	pan_tilt_pos[3].y = tilt_up;


	//the following state transition applies to all states
	if(s.behavior_state[FIND_FLAME_FSM]==0) state = s_disabled;


	task_open_2();

	while(1)
	{
		//------------------------------------------------------------------------------------------------------------------
		//
		//------------------------------------------------------------------------------------------------------------------
		first_(s_disabled)
		{
			enter_(s_disabled) 
			{
				FIRE_ALARM(0);
				flame_is_out = 0;
				PUMP_OFF();
				pan_tilt_center();
				motor_command(2,0,0,0,0);
			}

			if(s.behavior_state[FIND_FLAME_FSM] != 0) state = s.behavior_state[FIND_FLAME_FSM];

			exit_(s_disabled)  
			{
				pan_tilt_center();
				FIRE_ALARM(1);
				NOP();
			}
		}
		//------------------------------------------------------------------------------------------------------------------
		//
		//------------------------------------------------------------------------------------------------------------------
		next_(s_scanning)
		{
			enter_(s_scanning) 
			{ 
				NOP();
				MOVE(20,room_entry_distance); //enter the room a little bit before scanning for the flame
				task_wait(100);
			}

			//TODO:  need to deal w/ reflections from the wall. can result in undershoot. sensor readings are almost maxed out by reflextion.
			if(s.inputs.analog[AI_FLAME_NE]<flame_threshold_1)
			{
				motor_command(6,3,3,40,-40);
			}
			else
			{
				motor_command(2,0,0,0,0);
				task_wait(100); //allow the robot tocome to a complete stop
				//in case of overshoot, need to make a correction
				while (/*(s.inputs.analog[AI_FLAME_NW]<40) && */(s.inputs.analog[AI_FLAME_NE]<flame_threshold_2))
				{
					motor_command(7, 2, 2, -5, 5);
					OS_SCHEDULE;
				}
				motor_command(2, 0, 0, 0, 0);
				switch_(s_scanning, s_moving_towards_candle);
			}

			exit_(s_scanning) 
			{ 
				NOP();
			}
		}
		//------------------------------------------------------------------------------------------------------------------
		//
		//------------------------------------------------------------------------------------------------------------------
		next_(s_scanning_v2)
		{
			enter_(s_scanning_v2) 
			{ 
				NOP();
				MOVE(20,room_entry_distance); //enter the room a little bit before scanning for the flame
				task_wait(100);
			}

			TURN_IN_PLACE( 40, -130 );
			TURN_IN_PLACE_AND_SCAN(10, 300, flame_scan_moving_avg);
			scan_result = find_flame_in_scan(scan_data,360,flame_threshold_1);
			TURN_IN_PLACE( 30, -(300-scan_result.center_angle) );
			switch_(s_scanning_v2, s_moving_towards_candle);

			exit_(s_scanning_v2) 
			{ 
				NOP();
			}
		}
		//------------------------------------------------------------------------------------------------------------------
		//
		//------------------------------------------------------------------------------------------------------------------
		next_(s_moving_towards_candle)
		{
			enter_(s_moving_towards_candle) 
			{ 
				NOP();
				wall = 0;
			}

			bias = 0;

			if (wall > 0) wall -= 1;

			if ((s.inputs.sonar[0] < stopping_distance) || (s.inputs.sonar[1] < stopping_distance) || (s.inputs.sonar[2] < stopping_distance))
			{
				motor_command(2,0,0,0,0);
				pan_tilt_center();
				TURN_IN_PLACE(20, -45);
				TURN_IN_PLACE_AND_SCAN(10, 90, flame_scan_moving_avg);
				scan_result = find_flame_in_scan(scan_data, 360, flame_threshold_1);
				TURN_IN_PLACE(10, -(90 - scan_result.center_angle));
				//if ((s.inputs.sonar[0] < stopping_distance) || (s.inputs.sonar[1] < stopping_distance) || (s.inputs.sonar[2] < stopping_distance))
				{
					switch_(s_moving_towards_candle, s_extinguish);
				}
			}

			if (s.inputs.sonar[US_E] < us_wall_threashold)
			{
				TURN_IN_PLACE(30, 90);
				MOVE(30, 150);
				TURN_IN_PLACE_AND_SCAN(10, -180, flame_scan_moving_avg);
				scan_result = find_flame_in_scan(scan_data, 360, flame_threshold_1);
				TURN_IN_PLACE(10, (180 - abs(scan_result.center_angle)));
			}
			else if (s.inputs.sonar[US_W] < us_wall_threashold)
			{
				TURN_IN_PLACE(30, -90);
				MOVE(30, 150);
				TURN_IN_PLACE_AND_SCAN(10, 180, flame_scan_moving_avg);
				scan_result = find_flame_in_scan(scan_data, 360, flame_threshold_1);
				TURN_IN_PLACE(10, -(180 - abs(scan_result.center_angle)));
			}
			/*
			else if( (s.inputs.ir[IR_NE] < ir_wall_threashold) || ( s.inputs.sonar[US_E] < us_wall_threashold) )
			{
				//TODO: need to remember the fact that we are sliding/tracking along the wall - need a separate state
				bias = -2;
				wall += 2;
				track_candle();
			}
			else if( (s.inputs.ir[IR_NW] < ir_wall_threashold) || ( s.inputs.sonar[US_W] < us_wall_threashold ) )
			{
				bias = 2;
				wall += 2;
				track_candle();
			}
			else
			*/
			{
				if(s.inputs.analog[AI_FLAME_NE]>s.inputs.analog[AI_FLAME_NW]) bias = 2;
				if(s.inputs.analog[AI_FLAME_NW]>s.inputs.analog[AI_FLAME_NE]) bias = -2;
			}

			if ( (wall==0) && ((s.inputs.analog[AI_FLAME_NE] + s.inputs.analog[AI_FLAME_NE]) < flame_threshold_2) )
			{
				pan_tilt_center();
				TURN_IN_PLACE(20, -45);
				TURN_IN_PLACE_AND_SCAN(10, 90, flame_scan_moving_avg);
				scan_result = find_flame_in_scan(scan_data, 360, flame_threshold_1);
				TURN_IN_PLACE(5, -(90 - scan_result.center_angle));
			}


			motor_command(7,2,2,20+bias,20-bias);


			exit_(s_moving_towards_candle) 
			{ 
				NOP();
			}
		}
		//------------------------------------------------------------------------------------------------------------------
		//
		//------------------------------------------------------------------------------------------------------------------
		next_(s_extinguish)
		{
			enter_(s_extinguish) 
			{
				pan_offset = cfg_get_u16_by_grp_id(15, 6) - cfg_get_u16_by_grp_id(15, 10);
				tilt_offset= cfg_get_u16_by_grp_id(15,5) - cfg_get_u16_by_grp_id(15,9);

				PUMP_ON();
				task_wait(2000);  //let's start by sprying straight ahead for 2sec
				t_start = get_ms();
				count = 0;
			}

			t_delta = get_ms() - t_start;
			i=t_delta / pan_tilt_speed;
			if(i>3) 
			{
				i = 0; 
				t_start = get_ms(); 
				count++;
			}
			pan_absolute(pan_tilt_pos[i].x + pan_offset);
			tilt_absolute(pan_tilt_pos[i].y+ tilt_offset);
			//v.u16 = pan_tilt_pos[i].x;  cfg_set_value_by_grp_id(15,6, v);
			//v.u16 = pan_tilt_pos[i].y;  cfg_set_value_by_grp_id(15,5, v);

			if (s.inputs.analog[AI_FLAME_N] < omni_flame_threashold)
			{
				flame_is_out++;
			}
			else
			{
				flame_is_out = 0;
			}


			if ( (count>20) || (flame_is_out>150) )
			{
				state = s_disabled;
				s.behavior_state[FIND_FLAME_FSM] = 0; //to make sure we don't restart the whole FSM
			}


			exit_(s_extinguish)  
			{
				PUMP_OFF();
				pan_tilt_center();
			}
		}

		if(state != last_state) { dbg_printf("FF:state: %3d -> %3d\n", last_state,state); }
		if(bias != last_bias)	{ dbg_printf("FF:bias:  %3d -> %3d\n", last_bias,bias); last_bias=bias; }
		OS_SCHEDULE;
	}
	task_close();
}


/*

static s16 bias=0;

enter_(s_move_to_candle) {}

//move a little bit into the room
MOVE(turn_speed, search4_distance_1); //100);

//make sure we are not in front of some obstacle (in case we saw a reflection from the wall)

//now move forward until we reach the candle circle;
RESET_LINE_DETECTION();

play_frequency(440,25000,15);
//task_wait(500);
//PUMP_ON();


//start moving straight
motor_command(7,2,2,10,10); motor_command(6,1,1,30,30);
s.inputs.watch[0] = 1;
stop=0;
while(stop==0)
{
s.inputs.watch[0] = 2;
motor_command(6,1,1,30-bias,30+bias);
OS_SCHEDULE;
if(s.ir[IR_NE] < 70) bias=10;
else if(s.ir[IR_NW] < 70) bias=-10;
else bias=0;

//TODO: if the candle is in the middle of the room, we might not see it if we are facing it slightly off-center
if( (s.ir[IR_N] <= 100) ) stop |= 0x01;
//if( (s.ir[IR_NE] <= 50)) stop |= 0x02;
//if( (s.ir[IR_NW] <= 50)) stop |= 0x04;
if( (s.inputs.sonar[0] <= 100) ) stop |= 0x08;

//if(LINE_WAS_DETECTED()) stop |= 0x10;


//just comment out the following 2 if() statements if we are not doing "arbitrary candle locaction"
if ((!stop) && (bias < 0) )
{
s.inputs.watch[0] = 3; OS_SCHEDULE;
//wall on the left!
dbg_printf("wall on the left!\n");
HARD_STOP();
s.inputs.watch[0] = 4; OS_SCHEDULE;
TURN_IN_PLACE(30,-90);
s.inputs.watch[0] = 5; OS_SCHEDULE;
MOVE(30,100);
s.inputs.watch[0] = 6; OS_SCHEDULE;
TURN_IN_PLACE_AND_SCAN(turn_speed, 180, flame_scan_filter);
s.inputs.watch[0] = 7; OS_SCHEDULE;
scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
if(scan_result.flame_center_value > flame_found_threashold)
{
s.inputs.watch[0] = 8; OS_SCHEDULE;
TURN_IN_PLACE( turn_speed, -(180-scan_result.center_angle) );
s.inputs.watch[0] = 9; OS_SCHEDULE;
}
//motor_command(7,2,2,10,10); motor_command(6,1,1,30,30);
bias=0;
//PUMP_ON();
}
if( (!stop) && (bias > 0) )
{
s.inputs.watch[0] = 10; OS_SCHEDULE;
//wall on the right!
dbg_printf("wall on the right!\n");
HARD_STOP();
s.inputs.watch[0] = 11; OS_SCHEDULE;
TURN_IN_PLACE(30, 90);
s.inputs.watch[0] = 12; OS_SCHEDULE;
MOVE(30,100);
s.inputs.watch[0] = 13; OS_SCHEDULE;
TURN_IN_PLACE_AND_SCAN(turn_speed, -180, flame_scan_filter);
s.inputs.watch[0] = 14; OS_SCHEDULE;
scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
if(scan_result.flame_center_value > flame_found_threashold)
{
s.inputs.watch[0] = 15; OS_SCHEDULE;
TURN_IN_PLACE( turn_speed, (180+scan_result.center_angle) );
s.inputs.watch[0] = 16; OS_SCHEDULE;
}
//motor_command(7,2,2,10,10); motor_command(6,1,1,30,30);
bias=0;
//PUMP_ON();
}

}
dbg_printf("stopped! reason: 0x%02x\n",stop);
HARD_STOP();
s.inputs.watch[0] = 17; OS_SCHEDULE;
PUMP_ON();


TURN_IN_PLACE(turn_speed, -45);
s.inputs.watch[0] = 18; OS_SCHEDULE;
TURN_IN_PLACE_AND_SCAN(turn_speed, 90, flame_scan_filter);
s.inputs.watch[0] = 19; OS_SCHEDULE;
scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
if(scan_result.flame_center_value > flame_found_threashold)
{
s.inputs.watch[0] = 20; OS_SCHEDULE;
TURN_IN_PLACE( turn_speed, -(90-scan_result.center_angle) );
s.inputs.watch[0] = 21; OS_SCHEDULE;
}

s.inputs.watch[0] = 22; OS_SCHEDULE;
while( (s.inputs.sonar[0] > 70) && (s.ir[IR_N] > 70) )
{
motor_command(7,1,1,10,10);
OS_SCHEDULE;
}
s.inputs.watch[0] = 23; OS_SCHEDULE;
HARD_STOP();


//now turn on the fan and sweep left and right for some time
PUMP_ON();
//task_wait(1000);
{ static int delay_ticks=50; while(delay_ticks>0) {delay_ticks--; OS_SCHEDULE; } }
TURN_IN_PLACE(5,-10);
//task_wait(1000);
{ static int delay_ticks=50; while(delay_ticks>0) {delay_ticks--; OS_SCHEDULE; } }
TURN_IN_PLACE(5, 10);
//task_wait(1000);
{ static int delay_ticks=50; while(delay_ticks>0) {delay_ticks--; OS_SCHEDULE; } }
TURN_IN_PLACE(5, 10);
//task_wait(1000);
{ static int delay_ticks=50; while(delay_ticks>0) {delay_ticks--; OS_SCHEDULE; } }
TURN_IN_PLACE(5,-10);
//task_wait(1000);
{ static int delay_ticks=50; while(delay_ticks>0) {delay_ticks--; OS_SCHEDULE; } }
PUMP_OFF();

//TODO: go back to the home circle (optional)
state = s_exit_from_room;

*/

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

