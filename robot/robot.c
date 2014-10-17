

#include "standard_includes.h"

const unsigned char pulseInPins[] = { IO_US_ECHO_AND_PING_1 , IO_US_ECHO_AND_PING_2 , SOUND_START_PIN};

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



	
void stop_all_behaviours(void)
{
	uint8 i;
	
	for(i=0;i<16;i++) s.behavior_state[i] = 0;
}


int hardware_init(void)
{
	UCSR1A = 0;
	DDRD = 0;

	//Make SSbar be an output so it does not interfere with SPI communication.
	//SSbar == FAN_PIN
	set_digital_output(FAN_PIN, HIGH);

	set_digital_input(SOUND_START_PIN, PULL_UP_ENABLED);
	set_digital_input(BUTTON_START_PIN, PULL_UP_ENABLED);
	
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
	
	pulse_in_start(pulseInPins, 3);		// start measuring pulses (1 per each sonar;  uvtorn not used right now)
	
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

	usb_printf("analog_update_fsm()\n");

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

		s.line[RIGHT_LINE] = s.inputs.analog[AI_LINE_RIGHT];
		s.line[LEFT_LINE] = s.inputs.analog[AI_LINE_LEFT];

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

	usb_printf("lcd_update_fsm()\n");

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
			if(s.lcd_screen > 4) s.lcd_screen = 0;
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
		else if(s.lcd_screen==4)
		{
			clear();  
			lcd_goto_xy(0,0);   task_wait(5);
			lcd_printf("Hz=%ld  ssc=%d",frequency_in_hz,sound_start_count);  task_wait(5);
			lcd_goto_xy(0,1);   task_wait(5);
			lcd_printf("cssc=%d",consecutive_sound_start_count);     task_wait(5);
		}
		else if(s.lcd_screen==255) 
		{
			//allow another task to update the screen, i.e. don't clear the screen
		}
	}
	task_close();
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
	if(t_last > 20)
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



#if 1

void busy_task(u08 cmd, u08 *param)
{
	static u16 i;
	static u32 ic1=0,ic2,ic3;
	static u32 t_last,t_now;
	static u32 min=999999,max=0,avg=0;

	task_open();

	t_now=t_last=get_ms();
	
	//110526 counts per second w/ no other tasks running
	i=0;
	while(1)
	{
		i++;
		if(i>9) i=1;
		usb_printf("%d percent busy...\n",i*10);
		t_last=get_ms();
		while(get_ms()-t_last < 10000)
		{
			delay_ms(i);
			task_wait(10-i);
		}
	}
	task_close();
}


void idle_task(u08 cmd, u08 *param)
{
	u08 i;
	static u32 ic1=0,ic2,ic3;
	static u32 t_last,t_now;
	static u32 min=999999,max=0,avg=0;

	task_open();

	usb_printf("idle_task()\n");

	t_now=t_last=get_ms();
	
	//110526 counts per second w/ no other tasks running
	while(1)
	{
		t_now=get_ms();
		ic1 = idle_counter;	
		task_wait(100); 
		ic2 = idle_counter - ic1;
		if(ic2<min) min=ic2;
		if(ic2>max) max=ic2;
		avg = ((avg*9)+ic2)/10;
		if(t_now-t_last>=1000)
		{
			usb_printf("idle: min=%ld, avg=%ld, max=%ld\n",min,avg,max);
			t_last=t_now;
			min=999999;
			max=0;
		}
	}
	task_close();
}
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void scan(u08 cmd, u16 moving_avg)
{
	static s16 last_angle;
	s16 angle;
	static u16 i=0;
	static u16 flame_avg;
	static u16 ir_n_avg;
	static u16 ir_fn_avg;
	u16 ir_n;
	u16 ir_fn;

	if(moving_avg == 0 ) moving_avg = 1;
	if(moving_avg >= 16) moving_avg = 16;

	angle = (s16) (odometry_get_rotation_since_checkpoint()+0.5);

	//currently our sharp IR lookup table doesn't clamp the readings at the sensors max range, so there could be some large values; need to clamp here
	ir_n = s.ir[AI_IR_N];	  if(ir_n >  300) ir_n =  300;
	ir_fn= s.ir[AI_IR_FAR_N]; if(ir_fn > 600) ir_fn = 600;


	if(cmd==1) //initialize the state and our moving average
	{
		last_angle = angle;
		memset(scan_data,0,sizeof(scan_data));
		flame_avg = 255 - s.inputs.analog[AI_FLAME_N];
		ir_n_avg  = ir_n;
		ir_fn_avg = ir_fn;
		i=0;
		dbg_printf("scan(%d,%d): f=%d, ir_n=%d, ir_fn=%d\n", cmd, moving_avg, flame_avg,ir_n_avg,ir_fn_avg);
	}
	else //update
	{
		//use a moving average to filter out any noise spikes; prevent noise spikes from showing up as "peak" values later
		flame_avg = ((flame_avg * (moving_avg-1)) + ((u16)(255 - s.inputs.analog[AI_FLAME_N])) ) / moving_avg;
		ir_n_avg  = ((ir_n_avg  * (moving_avg-1)) + ((u16)(ir_n)) ) / moving_avg;
		ir_fn_avg = ((ir_fn_avg * (moving_avg-1)) + ((u16)(ir_fn)) ) / moving_avg;
		if(angle != last_angle) 
		{
			scan_data[i].angle = angle;
			scan_data[i].abs_angle = (s16)(s.inputs.theta * K_rad_to_deg);
			scan_data[i].flame			= (u08) flame_avg; //255 - s.inputs.analog[AI_FLAME_N];
			scan_data[i].ir_north		= ir_n_avg; //s.ir[AI_IR_N];
			scan_data[i].ir_far_north	= ir_fn_avg; //s.ir[AI_IR_FAR_N];
			last_angle = angle;
			i++;
			if(i>=360)
			{
				NOP(); //for debugging, i.e. to set a break point
				i=0;
				dbg_printf("scan(): wrapped around, i.e. turned > 360 degrees!\n");
			}
		}
	}
}



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
		s_disabled=0,			//0 
		s_waiting_for_start,	//1
		s_aligning_south,		//2
		s_finding_room_3,		//3
		s_searching_room_3,		//4
		s_finding_room_2,		//5
		s_searching_room_2,		//6
		s_finding_room_1,		//7
		s_searching_room_1,		//8
		s_finding_room_4,		//9
		s_searching_room_4,		//10
		s_move_to_candle		//11
	};
	static enum states state=s_disabled;
	static enum states last_state=s_disabled;
	static u32 t_entry=0;
	static u08 still_inside_room=0;
	static u32 context_switch_counter=0;
	static u32 t_last=0;
	static u08 dog_position=0; //1=N side of Rm#4,  2=E side,    3=S side
	static u08 stop;

	DEFINE_CFG2(s16,turn_speed,						9,10);
	DEFINE_CFG2(s16,cmd,							9,11);
	DEFINE_CFG2(s16,accel,							9,12);
	DEFINE_CFG2(s16,decel,							9,13);
	DEFINE_CFG2(s16,flame_scan_edge_threashold,		9,14);
	DEFINE_CFG2(s16,flame_found_threashold ,		9,15);
	DEFINE_CFG2(u16,flame_scan_filter ,				9,16);

	DEFINE_CFG2(s16,room3_enter,					9,20);
	DEFINE_CFG2(s16,room3_turn_1,					9,21);
	DEFINE_CFG2(s16,room3_turn_2,					9,22);
	DEFINE_CFG2(s16,room3_turn_3,					9,23);

	DEFINE_CFG2(s16,room2_enter,					9,30);
	DEFINE_CFG2(s16,room2_turn_1,					9,31);
	DEFINE_CFG2(s16,room2_turn_2,					9,32);
	DEFINE_CFG2(s16,room2_turn_3,					9,33);

	DEFINE_CFG2(s16,room1_enter,					9,40);
	DEFINE_CFG2(s16,room1_turn_1,					9,41);
	DEFINE_CFG2(s16,room1_turn_2,					9,42);
	DEFINE_CFG2(s16,room1_turn_3,					9,43);

	DEFINE_CFG2(s16,dog_scan_distance,				9,51);
	DEFINE_CFG2(s16,dog_scan_sensor,				9,52);
	DEFINE_CFG2(s16,dog_scan_angle,					9,53);
	DEFINE_CFG2(u16,dog_scan_filter ,				9,54);

	DEFINE_CFG2(s16,find4_distance_1,				9,60);
	DEFINE_CFG2(s16,find4_turn_1,					9,61);
	DEFINE_CFG2(s16,find4_distance_2,				9,62);
	DEFINE_CFG2(s16,find4_left_margin_1,			9,63);
	DEFINE_CFG2(s16,find4_right_margin_1,			9,64);
	DEFINE_CFG2(s16,find4_distance_3,				9,65);
	DEFINE_CFG2(s16,find4_left_margin_2,			9,66);
	DEFINE_CFG2(s16,find4_right_margin_2,			9,67);

	DEFINE_CFG2(s16,search4_distance_1,				9,71);
	DEFINE_CFG2(s16,search4_turn_1,					9,72);
	DEFINE_CFG2(s16,search4_turn_2,					9,73);

	task_open_1();

	//initialize the parameter logic only the first time through
	if( context_switch_counter==0 )
	{
		PREPARE_CFG2(turn_speed);
		PREPARE_CFG2(cmd);
		PREPARE_CFG2(accel);
		PREPARE_CFG2(decel);
		PREPARE_CFG2(flame_scan_edge_threashold);
		PREPARE_CFG2(flame_found_threashold);

		PREPARE_CFG2(room3_enter			);
		PREPARE_CFG2(room3_turn_1			);
		PREPARE_CFG2(room3_turn_2			);
		PREPARE_CFG2(room3_turn_3			);

		PREPARE_CFG2(room2_enter			);
		PREPARE_CFG2(room2_turn_1			);
		PREPARE_CFG2(room2_turn_2			);
		PREPARE_CFG2(room2_turn_3			);

		PREPARE_CFG2(room1_enter			);
		PREPARE_CFG2(room1_turn_1			);
		PREPARE_CFG2(room1_turn_2			);
		PREPARE_CFG2(room1_turn_3			);

		PREPARE_CFG2(dog_scan_distance		);
		PREPARE_CFG2(dog_scan_sensor		);
		PREPARE_CFG2(dog_scan_angle			);

		PREPARE_CFG2(find4_distance_1		);
		PREPARE_CFG2(find4_turn_1			);
		PREPARE_CFG2(find4_distance_2		);
		PREPARE_CFG2(find4_left_margin_1	);
		PREPARE_CFG2(find4_right_margin_1	);
		PREPARE_CFG2(find4_distance_3		);
		PREPARE_CFG2(find4_left_margin_2	);
		PREPARE_CFG2(find4_right_margin_2	);

		PREPARE_CFG2(search4_distance_1);
		PREPARE_CFG2(search4_turn_1	);
		PREPARE_CFG2(search4_turn_2	);	
	}

	//update the parameter values the first time through and then everyh 500ms
	if( (context_switch_counter==0) || (get_ms()-t_last) > 500 )  
	{
		UPDATE_CFG2(turn_speed);
		UPDATE_CFG2(cmd);
		UPDATE_CFG2(accel);
		UPDATE_CFG2(decel);
		UPDATE_CFG2(flame_scan_edge_threashold);
		UPDATE_CFG2(flame_found_threashold);

		UPDATE_CFG2(room3_enter				);
		UPDATE_CFG2(room3_turn_1			);
		UPDATE_CFG2(room3_turn_2			);
		UPDATE_CFG2(room3_turn_3			);

		UPDATE_CFG2(room2_enter				);
		UPDATE_CFG2(room2_turn_1			);
		UPDATE_CFG2(room2_turn_2			);
		UPDATE_CFG2(room2_turn_3			);

		UPDATE_CFG2(room1_enter				);
		UPDATE_CFG2(room1_turn_1			);
		UPDATE_CFG2(room1_turn_2			);
		UPDATE_CFG2(room1_turn_3			);

		UPDATE_CFG2(dog_scan_distance		);
		UPDATE_CFG2(dog_scan_sensor		);
		UPDATE_CFG2(dog_scan_angle			);

		UPDATE_CFG2(find4_distance_1		);
		UPDATE_CFG2(find4_turn_1			);
		UPDATE_CFG2(find4_distance_2		);
		UPDATE_CFG2(find4_left_margin_1	);
		UPDATE_CFG2(find4_right_margin_1	);
		UPDATE_CFG2(find4_distance_3		);
		UPDATE_CFG2(find4_left_margin_2	);
		UPDATE_CFG2(find4_right_margin_2	);

		UPDATE_CFG2(search4_distance_1);
		UPDATE_CFG2(search4_turn_1	);
		UPDATE_CFG2(search4_turn_2	);	
		t_last = get_ms();
	}

	context_switch_counter++;

	task_open_2();
	//code after this point resumes execution wherever it left off when a context switch happened

	usb_printf("master_logic_fsm()\n");

	while(1)
	{
		//the following state transition applies to all states
		if(s.behavior_state[MASTER_LOGIC]==0) state = s_disabled;


		first_(s_disabled)
		{
			enter_(s_disabled) 
			{  
				HARD_STOP(); //motor_command(cmd,accel,decel,0,0);
				FAN_OFF();
				STOP_BEHAVIOR(FOLLOW_WALL);
				//STOP_BEHAVIOR(MASTER_LOGIC);
				RESET_LINE_DETECTION();
				dog_position=0;
				s.inputs.watch[0] = s.inputs.watch[1] = s.inputs.watch[2] = s.inputs.watch[3] = 0;
			}

			//if(s.behavior_state[MASTER_LOGIC]!=0) state = s.behavior_state[MASTER_LOGIC]; //s_waiting_for_start;
			state = s_waiting_for_start;
			s.behavior_state[MASTER_LOGIC] = s_waiting_for_start;

			exit_(s_disabled)  { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_waiting_for_start)
		{
			enter_(s_waiting_for_start) 
			{
				//TODO: should not be using low-level APIs here
				pulse_in_stop();
				pulse_in_start(pulseInPins,3);
			}

			if(check_for_start_signal()) 
			{
				state = s_aligning_south;
			}
			else if(s.behavior_state[MASTER_LOGIC]!=0) state = s.behavior_state[MASTER_LOGIC]; //s_waiting_for_start;

			exit_(s_waiting_for_start)
			{
				pulse_in_stop();
				pulse_in_start(pulseInPins,2);
			}
		}
		

		//-------------------------------------------------------------------------------------------------------
		//		Align the robot so that it faces south
		//-------------------------------------------------------------------------------------------------------
		next_(s_aligning_south)
		{
			enter_(s_aligning_south) { }
			
			TURN_IN_PLACE(turn_speed, -90);

			//is something right in front of us?
			//if so, then we were facing south initially, and now we are facing west; turn back...
			if( (s.ir[AI_IR_N] < 120) || (s.inputs.sonar[0] < 120) )  TURN_IN_PLACE(turn_speed, 90);

			odometry_update_postion(35.0f, 92.0f, 270.0f);

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
				//MOVE2(turn_speed,200,120,120); //need to take into account the fact that initially we'll be on the home circle
				START_BEHAVIOR(FOLLOW_WALL,RIGHT_WALL); //start following the RIGHT wall
				RESET_LINE_DETECTION();
			}

			//take into account the fact that initially we'll be on the home circle
			if( LINE_WAS_DETECTED() && (time_since_entry_()<2000) )
			{
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
		//  We are standing at the entrance to room 3 - let's see if there is a candle in this room
		//-------------------------------------------------------------------------------------------------------
		next_(s_searching_room_3)
		{
			enter_(s_searching_room_3)
			{
				s.current_room = 3;
			}

			event_signal(line_alignment_start_evt); 
			event_wait(line_alignment_done_evt);

			odometry_update_postion( ((float)(s.ir[AI_IR_NW]))/16.0f , 65.0f, 90.0f);

			//move a little into the room
			MOVE(turn_speed, room3_enter);

			//we should facing more or less N right now.  turn right about 120degrees so that we are facing SE
			TURN_IN_PLACE(turn_speed, room3_turn_1);

			//now start to scan for the flame by turning left about 250 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			TURN_IN_PLACE_AND_SCAN(turn_speed, room3_turn_2, flame_scan_filter);

			//now analyze the data...
			scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
			if(scan_result.flame_center_value > flame_found_threashold) //TODO: make the minimum flame value a parameter
			{
				/* move this logic to another state 
				if(scan_result.center_abs_angle > 270)
				{
					TURN_IN_PLACE( turn_speed, -(room3_turn_2+room3_turn_1) );
					MOVE(turn_speed, room3_enter);
					TURN_IN_PLACE( turn_speed, scan_result.center_abs_angle - s.inputs.theta );
				}
				else
				*/
				{
					//turn into the direction where we saw the peak
					//TODO: (low priority) move the "turn into the direction of the flame" logic into the "move to candle" state
					TURN_IN_PLACE( turn_speed, -(room3_turn_2-scan_result.center_angle) );
					//TODO: now confirm that we are still seeing the flame
				}
				switch_(s_searching_room_3, s_move_to_candle);
			}

			//if there was no candle, go on to the next room.
			//we should be facing the wall more or less SW, so we just need to start following the right wall
			still_inside_room = 1;
			RESET_LINE_DETECTION();

			TURN_IN_PLACE(turn_speed, room3_turn_3);

			//TODO:  implement a more general "find wall" logic and make it part of the "follow wall" state machine
			/*
			motor_command(cmd,accel,decel,turn_speed+5,turn_speed);
			while( (s.ir[AI_IR_NE] > 120) && (s.ir[AI_IR_N] > 100) ) task_wait(10); //TODO: use parameters here!
			motor_command(cmd,accel,decel,0,0);
			*/

			//at this point we should be able to just follow the wall a gain
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

			odometry_update_postion(27.0f, ((float)(s.ir[AI_IR_NW]))/16.0f , 180.0f);

			//move a little bit into the room
			MOVE(turn_speed, room2_enter);

			//we are facing more or less W right now.  turn right about 120degrees so that we are facing NE
			TURN_IN_PLACE(turn_speed, room2_turn_1);

			//now start to scan for the flame by turning left about 250 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			TURN_IN_PLACE_AND_SCAN(turn_speed, room2_turn_2, flame_scan_filter);
			scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
			if(scan_result.flame_center_value > flame_found_threashold) 
			{
				/*
				if(scan_result.center_abs_angle < 90)
				{
					TURN_IN_PLACE( turn_speed, -(room2_turn_2+room2_turn_1) );
					MOVE(turn_speed, room2_enter);
					TURN_IN_PLACE( turn_speed, scan_result.center_abs_angle - s.inputs.theta );
				}
				else
				*/
				{
					//turn into the direction where we saw the peak
					TURN_IN_PLACE( turn_speed, -(room2_turn_2-scan_result.center_angle) );
				}
				//we are looging straight at the candle - proceed with extinguishing
				switch_(s_searching_room_2, s_move_to_candle);
			}

			//if there was no candle, go on to the next room.
			//we should be facing the wall more or less SE, we just need to start following the right wall
			still_inside_room = 1;
			RESET_LINE_DETECTION();

			TURN_IN_PLACE(turn_speed, room2_turn_3);

			/*
			motor_command(6,accel,decel,turn_speed+5,turn_speed);
			while( (s.ir[AI_IR_NE] > 120) && (s.ir[AI_IR_N] > 100) ) task_wait(10); //TODO: use parameters here!
			motor_command(cmd,accel,decel,0,0);
			*/
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

			//which door are we enterhing through?
			if(s.inputs.y > (18.0f*25.4f))  odometry_update_postion(47.0f, 36.0f - ((float)(s.ir[AI_IR_NW]))/16.0f, 0.0f);
			else odometry_update_postion(47.0f, ((float)(s.ir[AI_IR_NE]))/16.0f, 0.0f);

			//move a little into the room
			MOVE(turn_speed, room1_enter);

			//we are facing more or less E right now.  turn left about 120degrees so that we are facing NW
			TURN_IN_PLACE(turn_speed, room1_turn_1);

			//now start to scan for the flame by turning right about 250 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			TURN_IN_PLACE_AND_SCAN(turn_speed, room1_turn_2, flame_scan_filter);
			scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
			if(scan_result.flame_center_value > flame_found_threashold) 
			{
				/*
				if(scan_result.center_abs_angle > 90)
				{
					TURN_IN_PLACE( turn_speed, -(room1_turn_2+room1_turn_1) );
					MOVE(turn_speed, room1_enter);
					TURN_IN_PLACE( turn_speed, scan_result.center_abs_angle - s.inputs.theta );
				}
				else
				*/
				{
					//turn into the direction where we saw the peak
					TURN_IN_PLACE( turn_speed, -(room1_turn_2-scan_result.center_angle) );
				}
				switch_(s_searching_room_1, s_move_to_candle);
			}


			still_inside_room = 1;
			RESET_LINE_DETECTION();

			//if there was no candle, go on to the next room.
			//we should be facing more or less SW, but too far away from the wall depending on door location
			//we first need to find the wall before we can follow it; let's turn left to face SE, then go straight towards the wall
			TURN_IN_PLACE(turn_speed, room1_turn_3);

			motor_command(6,accel,decel,turn_speed,turn_speed);
			while( (s.ir[AI_IR_NE] > 120) && (s.ir[AI_IR_N] > 100) ) task_wait(10); //TODO: use parameters here!
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
			
			//now we are on the line that marks the north-side door of Room #1 - let's align ourselves to that line so that we are facing North
			event_signal(line_alignment_start_evt);
			event_wait(line_alignment_done_evt);
			//TODO: make a note how far away we are from the wall on the right, because it affects a maneuver further down. (but watch out for mirrors!)

			odometry_update_postion(89.0f, ((float)(s.ir[AI_IR_NE]))/16.0f, 90.0f);


			//completely exit from room 1 until we have reached the center of the intersection
			MOVE(turn_speed, find4_distance_1); //260); //TODO: make this a parameter

			//TODO:  add a scan for a possible dog to the east side of room #4, before we look for the dog & door on the south side
			TURN_IN_PLACE(turn_speed, -45); //let's start from a north-east facing position
			TURN_IN_PLACE_AND_SCAN( 40, 90 , dog_scan_filter); //scan until we reach a south west facing position
			scan_result = find_path_in_scan(scan_data, 100, dog_scan_distance, 0, dog_scan_sensor); //TODO: adjust this range - is 30" to far?
			if(scan_result.opening < dog_scan_angle) //TODO: fix this angle
			{
				//OK so there is a dog blocking this hallway. let's take note of that. 
				//if this is 100% reliable, we basically know that we'll have to take the other route
				dog_position=2;
			}

			if(dog_position==0) //if we didn't already see the dog, scan the other hallway for it ...
			{
				//now scan for another 90 degrees, i.e. from NW to SW - this covers the west facing hallway
				TURN_IN_PLACE_AND_SCAN( 40, 90 , dog_scan_filter);
				scan_result = find_path_in_scan(scan_data, 100, dog_scan_distance, 0, dog_scan_sensor); //TODO: adjust this range - is 30" to far?
			}
			else
			{
				TURN_IN_PLACE(turn_speed, 45);	//now we are facing West again
			}

			if( (dog_position == 0) && (scan_result.opening < dog_scan_angle) ) //TODO: fix this angle
			{
				dog_position=3;
				//if there is a dog / obstacle right in front, then there won't be a 2nd dog to worry about...
				//just fallow the left wall...//eventually we'll wind up inside room 4, either via door on North side or on South side
				//so this takes care of 2 of 6 possible cases
				TURN_IN_PLACE(turn_speed, -(90+45)); //now we are facing north again
				TURN_IN_PLACE(turn_speed, find4_turn_1); //15);
				MOVE2(turn_speed, find4_distance_2,find4_left_margin_1,find4_right_margin_1);
				RESET_LINE_DETECTION();
				START_BEHAVIOR(FOLLOW_WALL,LEFT_WALL); //start  following the LEFT wall

				//keep following the left wall until we have passed through the door
				WAIT_FOR_LINE_DETECTION();
				play_note(C(3), 50, 10);
				STOP_BEHAVIOR(FOLLOW_WALL);
				HARD_STOP();
				RESET_LINE_DETECTION();
				switch_(s_finding_room_4, s_searching_room_4);
			}


			//there is no dog at south side of room #4
			//...check for a door on the South side of room #4
			if(dog_position==0)//if we still have not yet seen the dog yet, then it means we did a bunch of looking around so we need to re-align ourselves...
			{
				TURN_IN_PLACE(turn_speed, -45);	//now we are facing West again
			}

			//MOVE(turn_speed, 32*25);		//move forward about 30inches so that we are in the position where the door could be
			//MOVE2(turn_speed, 32*25,135,135);		//move forward about 30inches so that we are in the position where the door could be
			HARD_STOP();
			MOVE2(turn_speed, find4_distance_3, find4_left_margin_2, find4_right_margin_2);
			HARD_STOP();
			TURN_IN_PLACE(turn_speed, -90);	//now we would be facing the door

			if(s.ir[AI_IR_N] > 160)	//is there an opening right in front of us?
			{
				//yes...
				//at this point we are facing north and a door into room #4 is right in front of us
				//this takes care of another 2 of the 6 possbile cases
				RESET_LINE_DETECTION();
				GO(turn_speed);
				WAIT_FOR_LINE_DETECTION();
				HARD_STOP();
				switch_(s_finding_room_4, s_searching_room_4);
			}


			//nope...there's no door here....we are looking at the south side wall of room #4

			if(dog_position==0) /*..if there was no dog on the east side of room 4 when we checked earlier, then just go around that side....*/
			{
				TURN_IN_PLACE(turn_speed, -90);  //turn rigth to face East

				//keep following the left wall until we have passed through the door;  i.e. going counter-clockwise around Rm #4
				RESET_LINE_DETECTION();
				START_BEHAVIOR(FOLLOW_WALL,LEFT_WALL);
				WAIT_FOR_LINE_DETECTION();

				//we reached room 4 - so proceed to the appropriate state
				play_note(C(3), 50, 10);
				STOP_BEHAVIOR(FOLLOW_WALL);
				HARD_STOP();
				RESET_LINE_DETECTION();
				switch_(s_finding_room_4, s_searching_room_4);
			}

			//at this point, we can conclude that the only way to get into Room #4 is to follow the right wall clock-wise around Rom #4
			
			TURN_IN_PLACE(turn_speed, 90);  //turn left to face West
			//keep following the left wall until we have passed through the door;  i.e. going counter-clockwise around Rm #4
			RESET_LINE_DETECTION();
			START_BEHAVIOR(FOLLOW_WALL,RIGHT_WALL);
			WAIT_FOR_LINE_DETECTION();

			//we reached room 4 - so proceed to the appropriate state
			play_note(C(3), 50, 10);
			STOP_BEHAVIOR(FOLLOW_WALL);
			HARD_STOP();
			RESET_LINE_DETECTION();
			switch_(s_finding_room_4, s_searching_room_4);

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

			if(s.inputs.theta < (180.0f*K_deg_to_rad)) 	odometry_update_postion(57.0f, 56.0f, 90.0f);
			else odometry_update_postion(66.0f, 76.0f, 270.0f);

			//move a little bit into the room
			MOVE(turn_speed, search4_distance_1); //100);

			//we are facing either North or South, but it doesn't really matter...

			//turn right
			TURN_IN_PLACE(turn_speed, search4_turn_1); //-100);

			//now start to scan for the flame by turning left about 200 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			TURN_IN_PLACE_AND_SCAN(turn_speed, search4_turn_2, flame_scan_filter); //200);
			scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
			if(scan_result.flame_center_value > flame_found_threashold) 
			{
				/*
				if(scan_result.center_abs_angle > 270)
				{
					TURN_IN_PLACE( turn_speed, -(search4_turn_2+search4_turn_1) );
					MOVE(turn_speed, room2_enter);
					TURN_IN_PLACE( turn_speed, -135 );
				}
				else
				*/
				{
					//turn into the direction where we saw the peak
					TURN_IN_PLACE( turn_speed, -(search4_turn_2-scan_result.center_angle) );
				}
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
			static s16 bias=0;

			enter_(s_move_to_candle) {}

			//make sure we are not in front of some obstacle (in case we saw a reflection from the wall)

			//now move forward until we reach the candle circle; 
			RESET_LINE_DETECTION();

			play_frequency(440,25000,15);
			task_wait(500);
			FAN_ON(); 


			//start moving straight
			motor_command(7,2,2,10,10); motor_command(6,1,1,30,30);
			s.inputs.watch[0] = 1;
			stop=0;
			while(stop==0) 
			{
				s.inputs.watch[0] = 2;
				motor_command(6,1,1,30-bias,30+bias);		
				task_wait(50);
				if(s.ir[AI_IR_NE] < 70) bias=10;
				else if(s.ir[AI_IR_NW] < 70) bias=-10;
				else bias=0;

				if( (s.ir[AI_IR_N] <= 100) ) stop |= 0x01;
				//if( (s.ir[AI_IR_NE] <= 50)) stop |= 0x02;
				//if( (s.ir[AI_IR_NW] <= 50)) stop |= 0x04;
				if( (s.inputs.sonar[0] <= 100) ) stop |= 0x08;

				//if(LINE_WAS_DETECTED()) stop |= 0x10;


				//just comment out the following 2 if() statements if we are not doing "arbitrary candle locaction"
				if ((!stop) && (bias < 0) )
				{
					s.inputs.watch[0] = 3; task_wait(25);
					//wall on the left!
					dbg_printf("wall on the left!\n");
					HARD_STOP();
					s.inputs.watch[0] = 4; task_wait(25);
					TURN_IN_PLACE(30,-90);
					s.inputs.watch[0] = 5; task_wait(25);
					MOVE(30,100);
					s.inputs.watch[0] = 6; task_wait(25);
					TURN_IN_PLACE_AND_SCAN(turn_speed, 180, flame_scan_filter);
					s.inputs.watch[0] = 7; task_wait(25);
					scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
					if(scan_result.flame_center_value > flame_found_threashold) 
					{
							s.inputs.watch[0] = 8; task_wait(25);
							TURN_IN_PLACE( turn_speed, -(180-scan_result.center_angle) );
							s.inputs.watch[0] = 9; task_wait(25);
					}
					//motor_command(7,2,2,10,10); motor_command(6,1,1,30,30);
					bias=0;
					//FAN_ON(); 
				}
				if( (!stop) && (bias > 0) )
				{
					s.inputs.watch[0] = 10; task_wait(25);
					//wall on the right!
					dbg_printf("wall on the right!\n");
					HARD_STOP();
					s.inputs.watch[0] = 11; task_wait(25);
					TURN_IN_PLACE(30, 90);
					s.inputs.watch[0] = 12; task_wait(25);
					MOVE(30,100);
					s.inputs.watch[0] = 13; task_wait(25);
					TURN_IN_PLACE_AND_SCAN(turn_speed, -180, flame_scan_filter);
					s.inputs.watch[0] = 14; task_wait(25);
					scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
					if(scan_result.flame_center_value > flame_found_threashold) 
					{
						s.inputs.watch[0] = 15; task_wait(25);
							TURN_IN_PLACE( turn_speed, (180+scan_result.center_angle) );
						s.inputs.watch[0] = 16; task_wait(25);
					}
					//motor_command(7,2,2,10,10); motor_command(6,1,1,30,30);
					bias=0;
					//FAN_ON(); 
				}

			}
			dbg_printf("stopped! reason: 0x%02x\n",stop);
			HARD_STOP();
			s.inputs.watch[0] = 17; task_wait(25);
			FAN_ON(); 


			TURN_IN_PLACE(turn_speed, -45);
			s.inputs.watch[0] = 18; task_wait(25);
			TURN_IN_PLACE_AND_SCAN(turn_speed, 90, flame_scan_filter);
			s.inputs.watch[0] = 19; task_wait(25);
			scan_result = find_flame_in_scan(scan_data,360,flame_scan_edge_threashold);
			if(scan_result.flame_center_value > flame_found_threashold) 
			{
				s.inputs.watch[0] = 20; task_wait(25);
				TURN_IN_PLACE( turn_speed, -(90-scan_result.center_angle) );
				s.inputs.watch[0] = 21; task_wait(25);
			}

			s.inputs.watch[0] = 22; task_wait(25);
			while( (s.inputs.sonar[0] > 70) && (s.ir[AI_IR_N] > 70) )
			{
				motor_command(7,1,1,10,10);
				OS_SCHEDULE;
			}
			s.inputs.watch[0] = 23; task_wait(25);
			HARD_STOP();


			//now turn on the fan and sweep left and right for some time
			FAN_ON(); 
			task_wait(1000);
			TURN_IN_PLACE(5,-10); 
			task_wait(1000);
			TURN_IN_PLACE(5, 10);
			task_wait(1000);
			TURN_IN_PLACE(5, 10);
			task_wait(1000);
			TURN_IN_PLACE(5,-10);
			task_wait(1000);
			FAN_OFF(); 

			//TODO: go back to the home circle (optional)
			state = s_disabled;

			exit_(s_move_to_candle) {}
		}


		s.inputs.watch[2]=state;
		if(state!=last_state) dbg_printf("ML:state: %d->%d\n", last_state,state);

		task_wait(25);
	}

	task_close();
}
#pragma endregion





int main(void)
{
	#ifdef SVP_ON_WIN32
	{ extern void win32_main(void); 	win32_main(); }
	#endif
	hardware_init();  //initialize hardware & pololu libraries
	//i2c_init();

	//clear(); lcd_goto_xy(0,0); printf("V=%d",	read_battery_millivolts_svp()); delay_ms(100);

	play_from_program_space(welcome);


	//--------  initialize misc support libraries  -----------
	cfg_init();  //initialize the configuration parameter module
	LOOKUP_init();
	SHARPIR_init();
	//PID_init();  //not actually using the PID module right now


	//--------------  misc unit test stuff ----------------
	//test_flame();
	//sound_start_test();
	//{ extern void fsm_test_2(void);   while(1) {fsm_test_2();} }
#if 0
	T0(1,2);
	dbg_printf("0123456789 %d",0);
	T1(2,3);
	T2(4);
	T3(5,6);
#endif


	//initialize our state
	memset(&s,0,sizeof(t_state));
	s.inputs.vbatt=read_battery_millivolts_svp();
	//LOOKUP_initialize_table(line_sensor_table);  //only needed if we need to normalize left & right line sensor for some reason


	os_init();  //initialize the cooperative multitasking subsystem and start all tasks
#if 1
	serial_cmd_evt = event_create();
	line_alignment_start_evt = event_create();
	line_alignment_done_evt = event_create();

#ifdef SVP_ON_WIN32
	task_create( sim_task,					 1,  NULL, 0, 0);
#endif

	task_create( test_task,					 2,  NULL, 0, 0);

	//task_create( lcd_update_fsm,			10,  NULL, 0, 0 );	//lcd is incompatible with servos	
#ifndef SVP_ON_WIN32
	task_create( analog_update_fsm,			11,  NULL, 0, 0 );	//dont' run this in simulation mode
#endif
	task_create( serial_send_fsm,			12 , NULL, 0, 0 );		
	task_create( serial_receive_fsm,		13,  NULL, 0, 0);
	task_create( commands_process_fsm,		14,  NULL, 0, 0);
	task_create( motor_command_fsm,			15,  NULL, 0, 0);
	task_create( ultrasonic_update_fsm,		16,  NULL, 0, 0);
	//task_create( debug_fsm,				17, NULL, 0, 0);   //not used right now
	task_create( wall_follow_fsm,			18,  NULL, 0, 0);
	task_create( master_logic_fsm,			19,  NULL, 0, 0);
	//task_create( line_detection_fsm,		20,  NULL, 0, 0);
	task_create( line_alignment_fsm,		21,  NULL, 0, 0);
	task_create( line_detection_fsm_v2,		22,  NULL, 0, 0);
	task_create( servo_task,				23,  NULL, 0, 0);
	//task_create( busy_task,				24,  NULL, 0, 0);
	//task_create( idle_task,				25,  NULL, 0, 0);
	task_create( fsm_test_task,				29,   NULL, 0, 0 );

#else
	task_create( fsm_test_task,				1,   NULL, 0, 0 );
#endif
	os_start();
	
	//won't ever get here...
		
	return 0;
}

