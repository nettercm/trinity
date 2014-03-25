

#include "standard_includes.h"

const unsigned char pulseInPins[] = { IO_US_ECHO_AND_PING_1 , IO_US_ECHO_AND_PING_2 };

extern int svp_demo(void);
extern void commands_process_fsm(u08 cmd, u08 *param);


/*
//#pragma pack(1)
typedef struct
{
	uint8 current_value;
	uint8 filtered_value;
	sint8 slope;
} __attribute__((__packed__)) t_analog;
*/

char buffer[128];

uint32 t_last_output_update = 0;
//const int t_cycle = 100;

const char welcome[] PROGMEM = ">g32>>c32";

//line crossing / candle circle / home circle detection
u08 lines_crossed=0;
u08 last_lines_crossed=0;
DEFINE_CFG2(u08,black,6,1);					
DEFINE_CFG2(u08,white,6,2);					


#if 0
t_LOOKUP_table line_sensor_table[] = // R,L  I.E. Left senser reads a smaller value
{
	{16},
	{0		,0},
	{27		,10},
	{29		,13},
	{42		,19},
	{51		,26},	
	{55		,33},	
	{60		,38},	
	{78		,51},	
	{98		,78},
	{100	,84},	
	{129	,102},
	{177	,156},
	{183	,165},
	{188	,170},
	{199	,182},
	{255	,255},	
};
#endif

/*
void wait_for_start_button(void)
{
	set_digital_input(IO_US_ECHO, PULL_UP_ENABLED);

	set_ultrasonic_mux(7);
	
	while(0)
	{
		delay_ms(200);
		usb_printf("Start=%d.  V=%d\r\n",is_digital_input_high(IO_US_ECHO),  read_battery_millivolts_svp());
	}
	
	while( is_digital_input_high(IO_US_ECHO) ) ;

	set_digital_input(IO_US_ECHO, HIGH_IMPEDANCE);
}
*/	
	
	
void stop_all_behaviours(void)
{
	uint8 i;
	
	for(i=0;i<16;i++) s.behavior_state[i] = 0;
}


int hardware_init(void)
{
	//Make SSbar be an output so it does not interfere with SPI communication.
	set_digital_output(IO_B4, LOW);

	//sonar pins	
	ultrasonic_hardware_init();
	
	//uvtron pulse	
	//set_digital_input(IO_UV_PULSE, HIGH_IMPEDANCE);
	
	//set baud rate etc.
	serial_hardware_init();

	//analog
	set_analog_mode(MODE_10_BIT); // 10-bit analog-to-digital conversions


	lcd_init_printf();
	//clear();
	//print_from_program_space(PSTR("Analog. dT="));

	motors_hardware_init();
	
	pulse_in_start(pulseInPins, 2);		// start measuring pulses (1 per each sonar;  uvtorn not used right now)
	
	return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void analog_update_fsm(u08 cmd, u08 *param)
{
	static uint8 count=0;
	static uint32 vbatt;
	uint8 i;
	s16 value;
	static u08 cfg_idx_interval;
	static u08 cfg_idx_short_filter_threashold;
	static u08 cfg_idx_short_filter_amount;
	static u08 cfg_idx_long_filter_threashold;
	static u08 cfg_idx_long_filter_amount;
	u08 interval;
	s16 short_filter_threashold;
	s16 short_filter_amount;
	s16 long_filter_threashold;
	s16 long_filter_amount;

	task_open();

	for(i=0;i<=7;i++) s.inputs.analog[i] = (analog_read(i))>>2;
	s.inputs.vbatt = read_battery_millivolts_svp();
	vbatt = read_battery_millivolts_svp();
	
	cfg_idx_interval					= cfg_get_index_by_grp_and_id(1,4);
	cfg_idx_short_filter_threashold		= cfg_get_index_by_grp_and_id(2,1);
	cfg_idx_short_filter_amount			= cfg_get_index_by_grp_and_id(2,2);
	cfg_idx_long_filter_threashold		= cfg_get_index_by_grp_and_id(2,3);
	cfg_idx_long_filter_amount			= cfg_get_index_by_grp_and_id(2,4);
	
	while(1)
	{
		interval					= cfg_get_u08_by_index(cfg_idx_interval);
		short_filter_threashold		= cfg_get_u16_by_index(cfg_idx_short_filter_threashold);
		short_filter_amount			= cfg_get_u16_by_index(cfg_idx_short_filter_amount);
		long_filter_threashold		= cfg_get_u16_by_index(cfg_idx_long_filter_threashold);
		long_filter_amount			= cfg_get_u16_by_index(cfg_idx_long_filter_amount);
		
		count++;

		for(i=0;i<=7;i++) s.inputs.analog[i] = (uint8)((((uint16)(s.inputs.analog[i]))*1 + (uint16)(analog_read(i)>>2) )/2);
		
		value = SHARPIR_get_real_value(AI_IR_NW,s.inputs.analog[AI_IR_NW]);
		if(value > short_filter_threashold) value = ((s.ir[AI_IR_NW]*short_filter_amount) + value)/(short_filter_amount+1);
		s.inputs.ir[0] = s.ir[AI_IR_NW]		= value;
		
		value = SHARPIR_get_real_value(AI_IR_N,s.inputs.analog[AI_IR_N]);
		if(value > short_filter_threashold) value = ((s.ir[AI_IR_N]*short_filter_amount) + value)/(short_filter_amount+1);
		s.inputs.ir[1] = s.ir[AI_IR_N]		= value;

		value = SHARPIR_get_real_value(AI_IR_NE,s.inputs.analog[AI_IR_NE]);
		if(value > short_filter_threashold) value = ((s.ir[AI_IR_NE]*short_filter_amount) + value)/(short_filter_amount+1);
		s.inputs.ir[2] = s.ir[AI_IR_NE]		= value;

		value = SHARPIR_get_real_value(AI_IR_N_long,s.inputs.analog[AI_IR_N_long]);
		if(value > long_filter_threashold) value = ((s.ir[AI_IR_N_long]*long_filter_amount) + value)/(long_filter_amount+1);
		s.inputs.ir[3] = s.ir[AI_IR_N_long]		= value;

		s.line[0] = s.inputs.analog[AI_LINE_RIGHT];
		s.line[1] = s.inputs.analog[AI_LINE_LEFT];

		//sample the following only once every 5 * 20 == 100ms
		if( count >= 20)
		{
			count = 0;
			vbatt = (vbatt*31UL + (uint32)read_battery_millivolts_svp())/32UL;
			s.inputs.vbatt = (uint16)vbatt;
		}

		task_wait(interval);
	}

	task_close();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void lcd_update_fsm(u08 cmd, u08 *param) //(uint32 event)
{
	static uint8 state=0;
	static uint32 t_last=0, t_now=0;
	static uint8 result = 0;
	static u08 button_state=0;
	static int us_n=0,us_w=0,us_e=0,us_nw=0,us_ne=0,us_sw=0,us_se=0;
	static uint8 count=0;
	static uint8 update_rate_cfg_idx;
	static uint16 update_rate;
	
	task_open();
	
	update_rate_cfg_idx = cfg_get_index_by_grp_and_id(1,2);
	
	while(1)
	{
		update_rate = cfg_get_u16_by_index(update_rate_cfg_idx);
		task_wait(update_rate);
		count++;


		/***************************************************************************
		// switch between LCD screen modes
		***************************************************************************/
		if(button_is_pressed(MIDDLE_BUTTON))
		{
			button_state = 1;
		}
		else if(button_state ==1) //button is getting released.....
		{
			play_note(A(4), 50, 10);
			button_state = 0;
			s.lcd_screen ++;
			if(s.lcd_screen > 3) s.lcd_screen = 0;
		}


		/***************************************************************************
		// control behavior state via buttons
		***************************************************************************/
		if(button_is_pressed(TOP_BUTTON))
		{
			play_note(A(3), 50, 10);
			//START_BEHAVIOR(FOLLOW_WALL, LEFT_WALL); 
			START_BEHAVIOR(MASTER_LOGIC,9); 
		}
		if(button_is_pressed(BOTTOM_BUTTON))
		{
			play_note(A(3), 50, 10);
			START_BEHAVIOR(MASTER_LOGIC,1); 
		}


		/***************************************************************************
		// display the selected screen
		***************************************************************************/
		if(s.lcd_screen==0)
		{
			clear();  
			lcd_goto_xy(0,0);
			lcd_printf("L: %3d %3d", s.inputs.analog[AI_LINE_RIGHT],s.inputs.analog[AI_LINE_LEFT]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 	
			lcd_printf("F: %03d  V: %3d", s.inputs.analog[AI_FLAME_N],s.inputs.vbatt/10); 
		}
		else if(s.lcd_screen==1)
		{
			clear();  
			lcd_goto_xy(0,0); 
			lcd_printf("U: %4d %4d",  s.inputs.sonar[0],s.inputs.sonar[1]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 
			lcd_printf("AVG: %4d %4d",  s.us_avg[0], s.us_avg[1]);
		}
		else if(s.lcd_screen==2)
		{
			clear();  
			lcd_goto_xy(0,0); 
			lcd_printf("i%03d %03d %03d %03d",s.ir[AI_IR_NE], s.ir[AI_IR_N_long], s.ir[AI_IR_N], s.ir[AI_IR_NW]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 
			lcd_printf("u%03d %03d %03d",  us_nw, us_n, us_ne); 
		}
		else if(s.lcd_screen==3)
		{
			clear();  
			lcd_goto_xy(0,0);   task_wait(5);
			lcd_printf("V=%5d",	read_battery_millivolts_svp());     task_wait(5);
			lcd_goto_xy(0,1);   task_wait(5);
			lcd_printf("V=%5d,  %5d",	s.inputs.vbatt, count);     task_wait(5);
		}
		else if(s.lcd_screen==255) 
		{
			//allow another task to update the screen, i.e. don't clear the screen
		}
	}
	task_close();
}



 t_PID_state pid =
{
	50,	//setpoint;
	0,	//error;
	20, //max_pos_error;
	-20, //max_neg_error;
	0,	//last_error;
	0,	//integral;
	10,	//integral_max;
	-10,	//integral_min;
	0,	//output;
	1,	//Kp;
	5,	//Kd;
	0,	//Ki;
	1,	//dT;
	0		//dEdT;
};

/*
typedef struct 
{
	sint16 lm;
	sint16 rm;
} t_set_motors_cmd;
*/



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void unit_test(void)
{
#if 0
	//for serial test only
	hardware_init();
	serial_test();
#endif
#if 0
	//for unit test only
	LOOKUP_test();
	SHARPIR_test();
	PID_test();
#endif	
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	if(t_last > 2)
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



#if 0
void idle(void)
{
	u08 i;
	static u32 ic1=0,ic2,ic3;
	task_open();
	
	//110526 counts per second w/ no other tasks running
	while(1)
	{
		ic1 = idle_counter;	
		task_wait(1000); 
		ic2 = idle_counter - ic1;
		ic3 = (ic2*100UL)/(110526UL);
		usb_printf("idle: %ld counts => %ld idle\n",ic2,ic3);
	}
	task_close();
}
#endif



void line_detection_fsm(u08 cmd, u08 *param)
{
	enum states { s_none=0, s_disabled=1, s_not_crossed, s_l_crossed, s_r_crossed, s_crossed_line };
	static enum states state=s_disabled;
	static enum states last_state=s_none;
	static u32 t_crossed=0;
	static u08 loop_counter=0;
	
	task_open();

	PREPARE_CFG2(black);					
	PREPARE_CFG2(white);					

	UPDATE_CFG2(black);					
	UPDATE_CFG2(white);

	while(1)
	{
		if(++loop_counter == 0) //no need to do this every time throught the loop
		{
			UPDATE_CFG2(black);					
			UPDATE_CFG2(white);
		}

		first_(s_disabled)
		{
			enter_(s_disabled) 	{}
			state = s_not_crossed;	leave_(s_disabled);
			exit_(s_disabled)  {}
		}


		next_(s_not_crossed)
		{
			enter_(s_not_crossed) 	{}
			if(s.line[0]<=white) state = s_l_crossed;
			if(s.line[1]<=white) state = s_r_crossed;
			exit_(s_not_crossed)  { t_crossed = get_ms(); }
		}


		next_(s_l_crossed)
		{
			enter_(s_l_crossed) 	{}
			if(get_ms() - t_crossed > 700) state = s_not_crossed;
			if(s.line[1] <= white) state = s_crossed_line;
			exit_(s_l_crossed)  {}
		}


		next_(s_r_crossed)
		{
			enter_(s_l_crossed) 	{}
			if(get_ms() - t_crossed > 700) state = s_not_crossed;
			if(s.line[0] <= white) state = s_crossed_line;
			exit_(s_r_crossed)  {}
		}


		next_(s_crossed_line)
		{
			enter_(s_crossed_line) 	{ lines_crossed++; }
			if( (s.line[0]>=black) && (s.line[1]>=black) )  state = s_not_crossed;
			exit_(s_crossed_line)  {}
		}
		s.inputs.watch[0]=state;
		s.inputs.watch[1]=lines_crossed;
		OS_SCHEDULE;
	}
	task_close();
}


Evt_t line_alignment_start_evt;
Evt_t line_alignment_done_evt;

void line_alignment_fsm(u08 cmd, u08 *param)
{
	task_open();

	for(;;)
	{
		event_wait(line_alignment_start_evt);
		while(s.line[0] <= white) { motor_command(7,0,0,0,20); task_wait(10); } motor_command(7,0,0,0,0);
		while(s.line[0] > white) { motor_command(7,0,0,0,-20); task_wait(10); } motor_command(7,0,0,0,0);
		while(s.line[1] <= white) { motor_command(7,0,0,20,0); task_wait(10); } motor_command(7,0,0,0,0);
		while(s.line[1] > white) { motor_command(7,0,0,-20,0); task_wait(10); } motor_command(7,0,0,0,0);
		task_wait(100);
		event_signal(line_alignment_done_evt);
	}

	task_close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*


state 1 - waiting for start signal

state 2 - aligning to face south

state 3 - navigating to room 3
	enter:	activate ( follow right wall )
	during:	look for white line (non-blocking)
	exit:	stop; deactivate (follow right wall )

state 4 - searching room 3
	enter:	move forward a few inches	
	during:	scan for flame
	exit:	turn around to face room exit

face south
follow wall (left / right)
enter room
search room
exit room


activate ( face south )

activate ( follow right wall )

*/


/* issues:

*/




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


u08 turn_in_place_manneuver(u08 cmd, s16 speed, float angle)
{
	static u08 state=0;
	static s16 sign=1;

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
		if     (( fabs(odometry_get_rotation_since_checkpoint()) >= fabs(angle)    )) { motor_command(2,0,0, 0,  0); state = 0; } //done
		else if(( fabs(odometry_get_rotation_since_checkpoint()) >  fabs(angle)- 5))   motor_command(7,1,1, -sign*5, sign*5 );
		else if(( fabs(odometry_get_rotation_since_checkpoint()) >  fabs(angle)-10))   motor_command(6,3,3, -sign*10, sign*10 );
		else if(( fabs(odometry_get_rotation_since_checkpoint()) >  fabs(angle)/4 ))   motor_command(6,1,1, -sign*15, sign*15 );
		else if(( fabs(odometry_get_rotation_since_checkpoint()) >  fabs(angle)/2 ))   motor_command(6,4,4, -sign*(speed/2),sign*(speed/2));
	}
	return state;
}

t_scan scan_data[360];

void scan(u08 cmd)
{
	static s16 last_angle;
	s16 angle;
	static u16 i=0;

	if(i>=360)
	{
		NOP();
	}

	angle = (s16) (odometry_get_rotation_since_checkpoint()+0.5);

	if(cmd==1) //initialize the state
	{
		last_angle = angle;
		memset(scan_data,0,sizeof(scan_data));
		i=0;
	}
	else //update
	{
		if(angle != last_angle) 
		{
			scan_data[i].angle = angle;
			scan_data[i].flame			= 255- s.inputs.analog[AI_FLAME_N];
			scan_data[i].ir_north		= s.ir[AI_IR_N];
			scan_data[i].ir_far_north	= s.ir[AI_IR_FAR_N];
			last_angle = angle;
			i++;
		}
	}
}


#define TURN_IN_PLACE(speed,angle) turn_in_place_manneuver(1,(speed),(angle)); while(turn_in_place_manneuver(0,(speed),(angle))) {OS_SCHEDULE;}
#define TURN_IN_PLACE_AND_SCAN(speed,angle) turn_in_place_manneuver(1,(speed),(angle)); scan(1); while(turn_in_place_manneuver(0,(speed),(angle))) {scan(0); OS_SCHEDULE;}

#define MOVE(speed,distance)		move_manneuver(1,(speed),(distance)); while(move_manneuver(0,(speed),(distance))) {OS_SCHEDULE;}
#define GO(speed)					motor_command(7,2,2,10,10); motor_command(6,1,1,(speed),(speed))

#define HARD_STOP()					motor_command(2,0,0,0,0);
#define SOFT_STOP()					motor_command(6,5,5,0,0);

#define FAN_ON()					set_digital_output(IO_D0,0)
#define FAN_OFF()					set_digital_output(IO_D0,1)

#define RESET_LINE_DETECTION()		last_lines_crossed = lines_crossed;
#define WAIT_FOR_LINE_DETECTION()	while(lines_crossed == last_lines_crossed) {OS_SCHEDULE;}
#define LINE_WAS_DETECTED()			(lines_crossed != last_lines_crossed)


/*
#define move(speed,distance) \
	odometry_set_checkpoint(); \
	motor_command(cmd,accel,decel,(speed),(speed)); \
	while ( abs(odometry_get_distance_since_checkpoint()) < (distance) ) { task_wait(10); } \
	motor_command(cmd,accel,decel,0,0)


#define turn(speed,angle) \
	odometry_set_checkpoint(); \
	motor_command(cmd,accel,decel,(speed),-(speed)); \
	while ( abs(odometry_get_rotation_since_checkpoint()) < angle ) { task_wait(10); } \
	motor_command(cmd,accel,decel,0,0)
*/
#pragma region Master Logic FSM
void master_logic_fsm(u08 fsm_cmd, u08 *param)
{
	static t_scan_result scan_result;
	enum states 
	{ 
		s_disabled=0, 
		s_waiting_for_start, 
		s_aligning_south, 
		s_finding_room_3, 
		s_searching_room_3,
		s_finding_room_2,
		s_searching_room_2,
		s_finding_room_1,
		s_searching_room_1,
		s_finding_room_4,
		s_searching_room_4,
		s_move_to_candle
	};
	static enum states state=s_disabled;
	static enum states last_state=s_disabled;
	static u32 t_entry=0;
	static u08 still_inside_room=0;
	DEFINE_CFG2(s16,turn_speed,			9,1);
	DEFINE_CFG2(s16,room3_enter,		9,2);
	DEFINE_CFG2(s16,room3_turn_1,		9,3);
	DEFINE_CFG2(s16,room3_turn_2,		9,4);
	DEFINE_CFG2(s16,room2_enter,		9,5);
	DEFINE_CFG2(s16,room2_turn_1,		9,6);
	DEFINE_CFG2(s16,room2_turn_2,		9,7);
	DEFINE_CFG2(s16,room1_enter,		9,8);
	DEFINE_CFG2(s16,room1_turn_1,		9,9);
	DEFINE_CFG2(s16,room1_turn_2,		9,10);
	DEFINE_CFG2(s16,cmd,				9,11);
	DEFINE_CFG2(s16,accel,				9,12);
	DEFINE_CFG2(s16,decel,				9,13);
	DEFINE_CFG2(s16,ninety,				9,14);
	
	task_open();

	PREPARE_CFG2(turn_speed);
	PREPARE_CFG2(room3_enter);
	PREPARE_CFG2(room3_turn_1);
	PREPARE_CFG2(room3_turn_2);
	PREPARE_CFG2(room2_enter);
	PREPARE_CFG2(room2_turn_1);
	PREPARE_CFG2(room2_turn_2);
	PREPARE_CFG2(room1_enter);
	PREPARE_CFG2(room1_turn_1);
	PREPARE_CFG2(room1_turn_2);
	PREPARE_CFG2(cmd);
	PREPARE_CFG2(accel);
	PREPARE_CFG2(decel);
	PREPARE_CFG2(ninety);

	while(1)
	{
		UPDATE_CFG2(turn_speed);
		UPDATE_CFG2(room3_enter);
		UPDATE_CFG2(room3_turn_1);
		UPDATE_CFG2(room3_turn_2);
		UPDATE_CFG2(room2_enter);
		UPDATE_CFG2(room2_turn_1);
		UPDATE_CFG2(room2_turn_2);
		UPDATE_CFG2(room1_enter);
		UPDATE_CFG2(room1_turn_1);
		UPDATE_CFG2(room1_turn_2);
		UPDATE_CFG2(cmd);
		UPDATE_CFG2(accel);
		UPDATE_CFG2(decel);
		UPDATE_CFG2(ninety);

		//the following state transition applies to all states
		if(s.behavior_state[MASTER_LOGIC]==0) state = s_disabled;


		first_(s_disabled)
		{
			enter_(s_disabled) 
			{  
				HARD_STOP(); //motor_command(cmd,accel,decel,0,0);
				FAN_OFF();
				STOP_BEHAVIOR(FOLLOW_WALL);
				STOP_BEHAVIOR(MASTER_LOGIC);
				RESET_LINE_DETECTION();
				s.inputs.watch[0] = s.inputs.watch[1] = s.inputs.watch[2] = s.inputs.watch[3] = 0;
			}

			if(s.behavior_state[MASTER_LOGIC]!=0) state = s.behavior_state[MASTER_LOGIC]; //s_waiting_for_start;

			exit_(s_disabled)  { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_waiting_for_start)
		{
			enter_(s_waiting_for_start) { }

			//TODO: Add actual start button / audio start logic. For now,  just fall through to the next state
			state = s_aligning_south;

			exit_(s_waiting_for_start) { }
		}
		

		//-------------------------------------------------------------------------------------------------------
		//		Align the robot so that it faces south
		//-------------------------------------------------------------------------------------------------------
		next_(s_aligning_south)
		{
			enter_(s_aligning_south) { }
			
			TURN_IN_PLACE(turn_speed, -ninety);

			//is something right in front of us?
			//if so, then we were facing south initially, and now we are facing west; turn back...
			if(s.ir[AI_IR_N] < 120)  TURN_IN_PLACE(turn_speed, ninety);

			//on to the next state...
			state = s_finding_room_3;

			exit_(s_aligning_south) { }
		}


		//-------------------------------------------------------------------------------------------------------
		//	Find Room #3
		//-------------------------------------------------------------------------------------------------------
		next_(s_finding_room_3)
		{
			enter_(s_finding_room_3)
			{
				START_BEHAVIOR(FOLLOW_WALL,RIGHT_WALL); //start following the RIGHT wall
				//TODO: need to take into account the fact that initiall we'll be on the home circle
				RESET_LINE_DETECTION();
			}

			if( LINE_WAS_DETECTED() )
			{

				RESET_LINE_DETECTION();
				play_note(C(3), 50, 10);
				HARD_STOP();
				STOP_BEHAVIOR(FOLLOW_WALL);
				state = s_searching_room_3;
			}

			exit_(s_finding_room_3) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_searching_room_3)
		{
			enter_(s_searching_room_3) { s.current_room = 3;}

			event_signal(line_alignment_start_evt); 
			event_wait(line_alignment_done_evt);

			//move a little into the room
			MOVE(turn_speed, room3_enter);

			//we should facing more or less N right now.  turn right about 120degrees so that we are facing SE
			TURN_IN_PLACE(turn_speed, room3_turn_1);

			//now start to scan for the flame by turning left about 250 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			TURN_IN_PLACE_AND_SCAN(turn_speed, room3_turn_2);

			//now analyze the data...
			scan_result = find_peak_in_scan(scan_data,360,30);
			if(scan_result.flame_center_value > 200) //TODO: make the minimum flame value a parameter
			{
				//turn into the direction where we saw the peak
				//TODO: move the "turn into the direction of the flame" logic into the "move to candle" state
				TURN_IN_PLACE( turn_speed, -(room3_turn_2-scan_result.center_angle) );
				switch_(s_searching_room_3, s_move_to_candle);
			}

			//if there was no candle, go on to the next room.
			//we should be facing the wall more or less SW, so we just need to start following the right wall
			//TODO:  implement a more general "find wall" logic and make it part of the "follow wall" state machine
			still_inside_room = 1;
			RESET_LINE_DETECTION();

			motor_command(cmd,accel,decel,turn_speed+5,turn_speed);
			while( (s.ir[AI_IR_NE] > 100) && (s.ir[AI_IR_N] > 80) ) task_wait(10);
			motor_command(cmd,accel,decel,0,0);

			//at this point we should be able to just follow the line a gain
			state = s_finding_room_2;

			exit_(s_searching_room_3) { }
		}



		//-------------------------------------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------------------------------------
		next_(s_finding_room_2)
		{
			enter_(s_finding_room_2)
			{
				START_BEHAVIOR(FOLLOW_WALL,RIGHT_WALL); //start following the RIGHT wall
			}

			if( LINE_WAS_DETECTED() )
			{
				//we'll be crossing the line while exiting from the prev. room, so let's take that into account!!!!
				if(still_inside_room) 
				{
					play_note(C(4), 50, 10);
					RESET_LINE_DETECTION();
					still_inside_room=0;
				}
				else
				{
					play_note(C(3), 50, 10);
					HARD_STOP();
					STOP_BEHAVIOR(FOLLOW_WALL);
					state = s_searching_room_2;
				}
			}

			exit_(s_finding_room_2) { }
		}



		//-------------------------------------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------------------------------------
		next_(s_searching_room_2)
		{
			enter_(s_searching_room_2) { s.current_room = 2;}

			event_signal(line_alignment_start_evt);
			event_wait(line_alignment_done_evt);

			//move a little bit into the room
			MOVE(turn_speed, room2_enter);

			//we are facing more or less W right now.  turn right about 120degrees so that we are facing NE
			TURN_IN_PLACE(turn_speed, room2_turn_1);

			//now start to scan for the flame by turning left about 250 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			TURN_IN_PLACE_AND_SCAN(turn_speed, room2_turn_2);
			scan_result = find_peak_in_scan(scan_data,360,30);
			if(scan_result.flame_center_value > 200) 
			{
				//turn into the direction where we saw the peak
				TURN_IN_PLACE( turn_speed, -(room2_turn_2-scan_result.center_angle) );
				//we are looging straight at the candle - proceed with extinguishing
				switch_(s_searching_room_2, s_move_to_candle);
			}

			//if there was no candle, go on to the next room.
			//we should be facing the wall more or less SE, we just need to start following the right wall
			still_inside_room = 1;
			last_lines_crossed = lines_crossed;
			motor_command(6,accel,decel,turn_speed+5,turn_speed);
			while( (s.ir[AI_IR_NE] > 100) && (s.ir[AI_IR_N] > 80) ) task_wait(10);
			motor_command(cmd,accel,decel,0,0);
			state = s_finding_room_1;

			exit_(s_searching_room_2) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_finding_room_1)
		{
			enter_(s_finding_room_1)
			{
				START_BEHAVIOR(FOLLOW_WALL,RIGHT_WALL); //start following the RIGHT wall
			}

			if(lines_crossed != last_lines_crossed)
			{
				//we'll be crossing the line while exiting from the prev. room!!!!
				if(still_inside_room) 
				{
					play_note(C(4), 50, 10);
					//s.inputs.watch[2]++;
					last_lines_crossed = lines_crossed;
					still_inside_room=0;
				}
				else
				{
					play_note(C(3), 50, 10);
					motor_command(cmd,accel,decel,0,0);
					s.behavior_state[FOLLOW_WALL] = 0;  //s.behavior_state[2] = 0;
					state = s_searching_room_1;
				}
			}

			exit_(s_finding_room_1) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_searching_room_1)
		{
			enter_(s_searching_room_1) { s.current_room = 1; }

			event_signal(line_alignment_start_evt);
			event_wait(line_alignment_done_evt);

			//move a little into the room
			MOVE(turn_speed, room1_enter);

			//we are facing more or less E right now.  turn left about 120degrees so that we are facing NW
			TURN_IN_PLACE(turn_speed, room1_turn_1);

			//now start to scan for the flame by turning right about 250 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			TURN_IN_PLACE_AND_SCAN(turn_speed, room1_turn_2);
			scan_result = find_peak_in_scan(scan_data,360,30);
			if(scan_result.flame_center_value > 200) 
			{
				//turn into the direction where we saw the peak
				TURN_IN_PLACE( turn_speed, -(room1_turn_2-scan_result.center_angle) );
				switch_(s_searching_room_1, s_move_to_candle);
			}

			//if there was no candle, go on to the next room.
			//we should be facing more or less SW, but too far away from the wall depending on door location
			//we first need to find the wall before we can follow it; let's turn left to face SE, then go straight towards the wall
			TURN_IN_PLACE(turn_speed, 60);

			still_inside_room = 1;
			last_lines_crossed = lines_crossed;
			motor_command(6,accel,decel,turn_speed,turn_speed);
			while( (s.ir[AI_IR_NE] > 100) && (s.ir[AI_IR_N] > 80) ) task_wait(10);
			motor_command(cmd,accel,decel,0,0);

			state = s_finding_room_4;

			exit_(s_searching_room_1) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_finding_room_4)
		{
			enter_(s_finding_room_4) 
			{ 
				START_BEHAVIOR(FOLLOW_WALL,RIGHT_WALL); //start following the RIGHT wall
			}

			//keep following the right wall until we have passed through the door
			WAIT_FOR_LINE_DETECTION();

			//now we are facing N
			play_note(C(3), 50, 10);
			STOP_BEHAVIOR(FOLLOW_WALL);  //s.behavior_state[2] = 0;
			HARD_STOP();

			event_signal(line_alignment_start_evt);
			event_wait(line_alignment_done_evt);

			//completely exit from room 1 until we have reached the center of the intersection
			MOVE(turn_speed, 240);

			//turn left and check for dog
			TURN_IN_PLACE(turn_speed, 45);
			TURN_IN_PLACE_AND_SCAN( 40, 90 );
			scan_result = find_path_in_scan(scan_data, 100, 300, 0, 1);
			
			if(scan_result.opening <= 30) //TODO: fix this angle
			{
				//if there is a dog / obstacle right in front, then 
				//turn right 90deg and start following the left wall
				//there won't be a 2nd dog to worry about...
				//eventuall we'll wind up inside room 4, either via door on North side or on South side
				TURN_IN_PLACE(turn_speed, -(90+45));
				MOVE(turn_speed, 240);
				RESET_LINE_DETECTION();
				START_BEHAVIOR(FOLLOW_WALL,LEFT_WALL); //start  following the LEFT wall

				//keep following the left wall until we have passed through the door
				WAIT_FOR_LINE_DETECTION();
				play_note(C(3), 50, 10);
				STOP_BEHAVIOR(FOLLOW_WALL);
				HARD_STOP();
				RESET_LINE_DETECTION();
				state = s_searching_room_4;
			}


			//there was no dog at the exit of room 1

			//check for door on South side of room 4
			TURN_IN_PLACE(turn_speed, -45);
			MOVE(turn_speed, 30*25);
			TURN_IN_PLACE(turn_speed, -90);
			if(s.inputs.ir[AI_IR_N] < 160)
			{
				//if there is no door on Rm4-south, then 
				//turn around (to face E)
				TURN_IN_PLACE(turn_speed, -90);
				//start following left wall
				RESET_LINE_DETECTION();
				START_BEHAVIOR(FOLLOW_WALL,LEFT_WALL);
				//keep following the left wall until we have passed through the door
				WAIT_FOR_LINE_DETECTION();
				play_note(C(3), 50, 10);
				STOP_BEHAVIOR(FOLLOW_WALL);
				HARD_STOP();
				RESET_LINE_DETECTION();
				state = s_searching_room_4;
			}
				//if we hit a dead end (i.e. dog) before we reach the door
					//turn around 180deg
					//start following right wall


			exit_(s_finding_room_4) {}
		}



		//-------------------------------------------------------------------------------------------------------
		//    Search for the candle in Room #4
		//-------------------------------------------------------------------------------------------------------
		next_(s_searching_room_4)
		{
			enter_(s_searching_room_4) { s.current_room = 4; }

			event_signal(line_alignment_start_evt);
			event_wait(line_alignment_done_evt);

			//move a little bit into the room
			MOVE(turn_speed, 100);

			//we are facing either North or South, but it doesn't really matter...

			//turn right
			TURN_IN_PLACE(turn_speed, -100);

			//now start to scan for the flame by turning left about 200 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			TURN_IN_PLACE_AND_SCAN(turn_speed, 200);
			scan_result = find_peak_in_scan(scan_data,360,30);
			if(scan_result.flame_center_value > 200) 
			{
				//turn into the direction where we saw the peak
				TURN_IN_PLACE( turn_speed, -(200-scan_result.center_angle) );
				//we are looging straight at the candle - proceed with extinguishing
				switch_(s_searching_room_4, s_move_to_candle);
			}

			//if there was no candle, then we messed up, because Room #4 is alwasy the last room we search!
			state = s_disabled;

			exit_(s_searching_room_4) {}
		}



		//-------------------------------------------------------------------------------------------------------
		//    Move closer to the candle and blow it out
		//-------------------------------------------------------------------------------------------------------
		next_(s_move_to_candle)
		{
			enter_(s_move_to_candle) {}

			//make sure we are not in front of some obstacle (in case we saw a reflection from the wall)

			//now move forward until we reach the candle circle; 
			RESET_LINE_DETECTION();

			//start moving straight
			GO(turn_speed*2);

			//if we reach the candle circle, stop - at this point we are withing 30cm / 12" of the candle
			WAIT_FOR_LINE_DETECTION();
			HARD_STOP();
			task_wait(1000);

			//now get a little closer
			//MOVE(turn_speed,100);

			//now turn on the fan and sweep left and right for some time
			FAN_ON(); 
			task_wait(1000);
			TURN_IN_PLACE(5,-20);
			TURN_IN_PLACE(5, 40);
			TURN_IN_PLACE(5,-40);
			TURN_IN_PLACE(5, 20);
			task_wait(1000);
			FAN_OFF(); 

			//TODO: go back to the home circle (optional)
			state = s_disabled;

			exit_(s_move_to_candle) {}
		}


		s.inputs.watch[2]=state;
		task_wait(25);
	}

	task_close();
}
#pragma endregion



void test(u08 cmd, u08 *param)
{
	static u16 i;
	float time_to_stop=0,distance_to_stop;
	static t_scan_result scan_result;
	DEFINE_CFG2(s16,accel,99,1);					
	DEFINE_CFG2(s16,decel,99,2);					
	DEFINE_CFG2(s16,speed,99,3);					
	DEFINE_CFG2(s16,distance,99,4);					

	task_open_1();

	if(cmd==0) 
	{
		NOP();
	}
	else
	{
		NOP();
		return;
	}
	//task_open();
	task_open_2();

	PREPARE_CFG2(accel);
	PREPARE_CFG2(decel);
	PREPARE_CFG2(speed);
	PREPARE_CFG2(distance);
	
	test(1,(uint8 *)0x1234);

	/*
	for(;;)
	{
		dbg_printf("0123456789\n");
		task_wait(100);
	}
	*/

	/*
	task_wait(200);
	motor_command(2,0,0,0,0);
	task_wait(200);
	motor_command(7,0,0,100,100);
	task_wait(500);
	motor_command(6,2,2,0,0);
	task_wait(500);
	*/

	while(1)
	{
		task_wait(100);
		UPDATE_CFG2(accel);
		UPDATE_CFG2(decel);
		UPDATE_CFG2(speed);
		UPDATE_CFG2(distance);

		if(s.behavior_state[TEST_LOGIC]==1) 
		{
			/*
			1) gradually ramp up (at specified rate) to target speed
			2) when we are <= 30degrees from the target, start ramping down to speed 15
			3) when we are <= 10degrees from the target, apply target speed 5 (w/ feed forward) & regulate to maintain 5
			3) when we are at the target, hit the brakes - full stop w/out ramping down
			*/
			/*
			odometry_set_checkpoint(); 
			motor_command(6,1,1,(speed),-(speed)); 
			while ( abs(odometry_get_rotation_since_checkpoint()) < 60 ) { task_wait(10); } 
			motor_command(6,1,1,15,-15);
			while ( abs(odometry_get_rotation_since_checkpoint()) < 80 ) { task_wait(10); } 
			motor_command(7,1,1,5,-5);
			while ( abs(odometry_get_rotation_since_checkpoint()) < 90 ) { task_wait(10); } 
			motor_command(7,1,1,0,0);
			*/

			/*
			MOVE(50,100);
			TURN_IN_PLACE( 50, 90);
			MOVE(50,100);
			TURN_IN_PLACE( 50, 90);
			MOVE(50,100);
			TURN_IN_PLACE( 50, 90);
			MOVE(50,100);
			TURN_IN_PLACE( 50, 90);
			*/

			#if 0
			//set_digital_output(IO_D1,0);
			set_digital_output(IO_D0,0);
			task_wait(2000);
			//set_digital_output(IO_D1,1);
			set_digital_output(IO_D0,1);
			#endif

			#if 0
			//dbg_printf("Starting scan....\n");
			TURN_IN_PLACE_AND_SCAN( 40, 220 );
			//dbg_printf("....done\n");
			scan_result = find_peak_in_scan(scan_data,360,30);
			dbg_printf("scan_result: %d,%d,%d,%d,%d,%d\n",
				scan_result.flame_center_value, scan_result.rising_edge_position, scan_result.falling_edge_position,
				scan_result.center_angle, scan_result.rising_edge_angle, scan_result.falling_edge_angle);
			for(i=0;i<360;i++) {dbg_printf("scan_data[%03d]=%03d,%03d\n",i, scan_data[i].angle, scan_data[i].flame);task_wait(10);}
			TURN_IN_PLACE( 40, -(220-scan_result.center_angle) );
			#endif

			TURN_IN_PLACE_AND_SCAN( 40, 90 );
			scan_result = find_path_in_scan(scan_data, 100, 300, 0, 1);
			dbg_printf("scan_result: %d,%d,%d,%d\n", scan_result.opening, scan_result.center_angle, scan_result.rising_edge_angle, scan_result.falling_edge_angle);
			//for(i=0;i<100;i++) {dbg_printf("scan_data[%03d]=%03d,%03d\n",i, scan_data[i].angle, scan_data[i].ir_north);task_wait(10);}
			TURN_IN_PLACE(40,-90);


			/*

			task_wait(2000);

			TURN_IN_PLACE_AND_SCAN( 100, -90 );
			scan_result = find_peak_in_scan(scan_data,360,3);

			task_wait(2000);

			TURN_IN_PLACE_AND_SCAN( 100, 180 );
			scan_result = find_peak_in_scan(scan_data,360,3);
	
			task_wait(2000);

			TURN_IN_PLACE_AND_SCAN( 100, -180 );
			scan_result = find_peak_in_scan(scan_data,360,3);
			*/
			s.behavior_state[TEST_LOGIC]=0;
		}



		/*
		while(s.behavior_state[11]==1) 
		{
			time_to_stop = (float)s.inputs.actual_speed[0] / (float)50;
			distance_to_stop = ((float)s.inputs.actual_speed[0] * time_to_stop)/2.0;
			distance_to_stop *= 50.0; //adjust for seconds
			distance_to_stop *= 0.13466716824940938560464;  //adjust for mm

			if(s.inputs.x + distance_to_stop < distance)
			{
				motor_command(6,accel,decel,speed,speed);
			}
			else
			{
				motor_command(6,accel,decel,0,0);
				s.behavior_state[TEST_LOGIC]=0;
			}
			task_wait(20);
		}
		*/
	}

	task_close();
}

int main(void)
{
	//initialize hardware & pololu libraries
	hardware_init();
	//i2c_init();
	//clear(); lcd_goto_xy(0,0); printf("V=%d",	read_battery_millivolts_svp());
	//delay_ms(100);


	//wait_for_start_button();
	//play_note(A(4), 50, 10);
	play_from_program_space(welcome);


	//initialize the configuration parameter module
	cfg_init();


	//initialize misc support libraries
	LOOKUP_init();
	SHARPIR_init();
	//PID_init();  //not actually using the PID module right now
	//servos_start(demuxPins, sizeof(demuxPins));
	//set_servo_target(0, 1375);
#ifdef WIN32
	test_flame();
#endif

	//initialize our state
	memset(&s,0,sizeof(t_state));
	s.inputs.vbatt=10000;
	//LOOKUP_initialize_table(line_sensor_table);  //only needed if we need to normalize left & right line sensor for some reason


	//initialize the cooperative multitasking subsystem and start all tasks
	os_init();
	#if 1
	serial_cmd_evt = event_create();
	line_alignment_start_evt = event_create();
	line_alignment_done_evt = event_create();

#ifdef SVP_ON_WIN32
	task_create( sim,						 1,  NULL, 0, 0);

#endif
	task_create( test,						 2,  NULL, 0, 0);

	task_create( lcd_update_fsm,			10,  NULL, 0, 0 );		
	task_create( analog_update_fsm,			11,  NULL, 0, 0 );	
	task_create( serial_send_fsm,			12 , NULL, 0, 0 );		
	task_create( serial_receive_fsm,		13,  NULL, 0, 0);
	task_create( commands_process_fsm,		14,  NULL, 0, 0);
	task_create( motor_command_fsm,			15,  NULL, 0, 0);
	//task_create( ultrasonic_update_fsm,		16,  NULL, 0, 0);
	//task_create( debug_fsm, 17, NULL, 0, 0); //not used right now
	task_create( wall_follow_fsm,			18,  NULL, 0, 0);
	task_create( master_logic_fsm,			19,  NULL, 0, 0);
	task_create( line_detection_fsm,		20,  NULL, 0, 0);
	task_create( line_alignment_fsm,		21,  NULL, 0, 0);


	#else
	task_create( fsm_test_task, 1, NULL, 0, 0 );
	#endif
	os_start();
	
	//won't ever get here...
		
	return 0;
}

