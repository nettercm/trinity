

#include "standard_includes.h"

const unsigned char pulseInPins[] = { IO_US_ECHO_AND_PING_1 , IO_US_ECHO_AND_PING_2 };

extern int svp_demo(void);
extern void commands_process_fsm(void);


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

//execution times for various functions
volatile uint32 t_inputs_fsm = 0,  t_lcd_fsm = 0,  t_loop = 0;



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



void analog_update_fsm(void)
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



void lcd_update_fsm(void) //(uint32 event)
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
		// display the selected screen
		***************************************************************************/
		clear();  
		if(s.lcd_screen==0)
		{
			lcd_goto_xy(0,0);
			lcd_printf("L=%3d,%3d", s.inputs.analog[AI_LINE_RIGHT],s.inputs.analog[AI_LINE_LEFT]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 	
			lcd_printf("Fl=%03d %03d", s.inputs.analog[AI_FLAME_NE],s.inputs.analog[AI_FLAME_N]); 
		}
		else if(s.lcd_screen==1)
		{
			lcd_goto_xy(0,0); 
			lcd_printf("US:  %4d %4d",  s.inputs.sonar[0],s.inputs.sonar[1]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 
			lcd_printf("AVG: %4d %4d",  s.us_avg[0], s.us_avg[1]);
		}
		else if(s.lcd_screen==2)
		{
			lcd_goto_xy(0,0); 
			lcd_printf("i%03d %03d %03d %03d",s.ir[AI_IR_NE], s.ir[AI_IR_N_long], s.ir[AI_IR_N], s.ir[AI_IR_NW]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 
			lcd_printf("u%03d %03d %03d",  us_nw, us_n, us_ne); 
		}
		else if(s.lcd_screen==3)
		{
			lcd_goto_xy(0,0);   task_wait(5);
			lcd_printf("V=%5d",	read_battery_millivolts_svp());     task_wait(5);
			lcd_goto_xy(0,1);   task_wait(5);
			lcd_printf("V=%5d,  %5d",	s.inputs.vbatt, count);     task_wait(5);
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



uint8 align_to_line(uint8 cmd)
{
	static uint8 state=0;
	static uint8 first=0; //indicates which line sensor saw the line first

	//if behavior gets turned off, i.e. we are in a running state and now the enabled flag goes to zero, go to the stopped state
	if(state!=0 && cmd==0)
	{
		state = 0;
	}	
	
	switch(state)
	{
		case 0:
		
		if(cmd==1) 
		{
			state = 1;
			first = 0;
		}			
		break;
		
		case 1:

		//if( (s.inputs.analog[AI_LINE_RIGHT] < 80) && (s.inputs.analog[AI_LINE_LEFT] < 40) )
		if( (s.line[0] < 50) && (s.line[1] < 50) )
		{
			s.rm_target=s.rm_actual=0;
			s.lm_target=s.lm_actual=0;
			if(!first) first=3;
		}
		if( (s.line[1] < 50) )
		{
			s.rm_target=s.rm_actual=0;
			if(!first) first=1;
		}			
		if( (s.line[0] < 50) )
		{
			s.lm_target=s.lm_actual=0;
			if(!first) first=2;
		}
		
		if( first && (s.rm_actual==0) && (s.lm_actual==0) ) //stopped..
		{
			//s.behavior_state[5] = 0; //turn this behavior off
			state = 2;
		}
		break;
		
		case 2:
		
		if(first==1) motor_command(5,3,0,80,0);
		if(first==2) motor_command(5,0,3,0,80);
		first = 0;
		state = 3;
		break;
		
		case 3:
		break;
	}
	return state;
}

#define ATWOR_DBG //usb_printf("a.t.w.o.r: cmd: %d, s: %d=>%d  us_c: %d,%d\r\n",cmd, last_state,state,us_cycles_last,s.us_cycles); last_state=state

#if 0
uint8 align_to_wall_on_right(uint8 cmd)
{
	uint8 result=0;
	sint16 diff;
	static uint8 last_state=0, state=0;
	static uint8 cycles=0, back_and_forth=0;
	static uint8 last_turn=0; //0 indicates no turns made yet
	static uint16 us_cycles_last=0, turn_time=100;
	
	//if behavior gets turned off, i.e. we are in a running state and now the enabled flag goes to zero, go to the stopped state
	if(state!=0 && cmd==0) 
	{
		state = last_state = cycles = back_and_forth = last_turn = us_cycles_last = 0;
	}		
	
	switch(state)
	{
		case 0:  //stopped
			if(cmd==1)
			{
				us_cycles_last = s.us_cycles;
				cycles = 0; back_and_forth = 0; last_turn = 0; turn_time = 100;
				//optionally, focus sonars on NW, W and SW
				ultrasonic_set_sequence(us_sequence_E_SE_NE);  
				state = 1;
				ATWOR_DBG;
			}
			break;
			
		case 1:
			//wait for sonar update cycle to complete, then issue a motor command and goto state 2
			if(s.us_cycles != us_cycles_last) 
			{   
				//s.inputs.sonar[US_SE]+=1;
				cycles++;
				state=2;  	
				//if US_E > US_SE, turn right;  if we don't see a wall close by on US_SE, also turn right until we start seeing a wall....
				if( (s.inputs.sonar[US_SE]>150) || (s.inputs.sonar[US_E] > s.inputs.sonar[US_SE]) ) 
				{	
					motor_command(1,turn_time,0,80,-80); //motor_command_fsm(5,1,1,80,-80);
					if(last_turn==2) { back_and_forth++; turn_time=turn_time/2; }
					last_turn = 1; 
				}						
				else if((s.inputs.sonar[US_E]) < s.inputs.sonar[US_SE]) 
				{
					motor_command(1,turn_time,0,-80,80); //motor_command_fsm(5,1,1,-80,80);
					if(last_turn==1) { back_and_forth++; turn_time=turn_time/2; }
					last_turn = 2;
				}					
			}
			break;
			
		case 2:
			//wait for motor command to complete, then goto state 3
			if(s.motor_command_state==0) 
			{   
				state=3; 	
				us_cycles_last=s.us_cycles;   //do a full sonar cycle AFTER the motors have stopped
				ATWOR_DBG;
			}
			break;
		
		case 3:
			//wait for sonar update cycle to complete, then check if we are done;  if not, go back to state 1
			//also, if we it looks like we are stuck in a back-and-forth or some loop, then we are done.
			if( (cycles>200) || (back_and_forth>8) )
			{
				state=4;
				ATWOR_DBG;
				break;				
			}
			if(s.us_cycles != us_cycles_last)
			{
				//s.inputs.sonar[US_SE]+=1;
				diff = ((sint16)s.inputs.sonar[US_E]) - ((sint16)s.inputs.sonar[US_SE]);
				if( (abs(diff) < 1) && (s.inputs.sonar[US_E]<3000)) state=4; else state=1;
				ATWOR_DBG;
			}
			break;
			
		case 4:
			ultrasonic_set_sequence(us_sequence_uniform);
			state = 5;
			break;

		case 5:
			break;
					
		default:
			break;
	}	
	s.inputs.fsm_states[1] = state;
	return state;
}

uint8 monitor_speed(uint8 cmd)
{
	static uint8 state=0;
	static uint16 l_enc=0,r_enc=0;
	static uint32 t_last;
	uint32 t_now, delta_t;
	
	//if behavior gets turned off, i.e. we are in a running state and now the enabled flag goes to zero, go to the stopped state
	if(state!=0 && cmd==0) 
	{
		state = 0;
		s.lm_actual = s.lm_target;
		s.rm_actual = s.rm_target;
	}		
	
	t_now = get_ms();
	delta_t = t_now - t_last;
	
	switch(state)
	{
		case 0:  //stopped
			if(cmd==1)
			{
				l_enc = svp_get_counts_ab();
				r_enc = svp_get_counts_cd();
				t_last = t_now;
				state = 1;
			}
			break;

		case 1:
			if(delta_t >= 100) //integrate encoder clicks over 100ms, otherwise we can't really measure speed 
			{
				//is the left wheel supposed to move, but appears stuck?
				if( (abs(s.lm_actual) >= 40) && (abs(s.inputs.encoders[0] - l_enc) < 1) ) //getting false alarm on low spees if using "< 2"
				{
					s.lm_actual = 200 * (s.lm_target > 0 ? 1 : -1);
				}
				else s.lm_actual = s.lm_target;
				
				//is the right wheel supposed to move, but appears stuck?
				if( (abs(s.rm_actual) >= 40) && (abs(s.inputs.encoders[1] - r_enc) < 1) )
				{
					s.rm_actual = 200 * (s.rm_target > 0 ? 1 : -1);
				}
				else s.rm_actual = s.rm_target;
				
				l_enc = s.inputs.encoders[0];
				r_enc = s.inputs.encoders[1];
				t_last = t_now;
			}
			break;
	}		
	s.inputs.fsm_states[2] = state;
	return state;
}




void determine_orientation(void)
{
	uint16 south=0,east=0;
	static uint16 cycles = 0;
			
	if(cycles == 0) cycles = s.us_cycles; //initialize
			
	if(s.us_cycles > cycles+4)
	{
		cycles = s.us_cycles;
		south=east=0;
				
		if(s.inputs.analog[AI_IR_NW] > s.inputs.analog[AI_IR_NE]) east++;
		if(s.inputs.analog[AI_IR_NW] > (s.inputs.analog[AI_IR_NE]+15)) east++;

		if(s.inputs.analog[AI_IR_NE] > s.inputs.analog[AI_IR_NW]) south++;
		if(s.inputs.analog[AI_IR_NE] > (s.inputs.analog[AI_IR_NW]+15)) south++;
				
		if(s.us_avg[US_SW] < 150) east++;
		if(s.us_avg[US_W] < 150) east++;
		if(s.us_avg[US_NW] < 130) east++;

		if(s.us_avg[US_SE] < 150) south++;
		if(s.us_avg[US_E] < 150) south++;
		if(s.us_avg[US_NE] < 130) south++;
				
		if( (s.us_avg[US_SW]+s.us_avg[US_W]+s.us_avg[US_NW]) > (s.us_avg[US_SE]+s.us_avg[US_E]+s.us_avg[US_NE]) )
		{
			south++;
		}
		else
		{
			east++;
		}
				
		//usb_printf("us_avg=%d,%d,%d,%d,%d,%d,%d   s,e=%d,%d",s.us_avg[0],s.us_avg[1],s.us_avg[2],s.us_avg[3],s.us_avg[4],s.us_avg[5],s.us_avg[6],south,east);
				
		//s.inputs.p2 = 256*south + east;
	}
}

#define STATE_WAIT_FOR_START 0
#define STATE_HOME 1
#define STATE_FIND_ROOM 2
#define STATE_FOUND_DOOR 3
#define STATE_FIND_FLAME 4

//#define STATE_HAS_CHANGED ( (last_state != state) ? (last_state=state) : 0)

uint8 old_master_logic_fsm(uint8 cmd)
{
	static uint8 state=0, last_state=255;
	uint8 result1,result2;
	static uint32 t_last;
	uint32 t_now, delta_t;
		
	//if behavior gets turned off, i.e. we are in a running state and now the enabled flag goes to zero, go to the stopped state
	if(state!=0 && cmd==0)
	{
		state = STATE_WAIT_FOR_START;
		stop_all_behaviours();
	}
	
	t_now = get_ms();
	delta_t = t_now - t_last;	
	
	switch(state)
	{
		/****************************************************************************/
		case STATE_WAIT_FOR_START:
		
		if(STATE_HAS_CHANGED) {}
		if(cmd==1) state = STATE_HOME;
		
		break;
		/****************************************************************************/
		
		
		/****************************************************************************/
		case STATE_HOME:
		
		if(STATE_HAS_CHANGED)
		{
			motors_stop();
			align_to_wall_on_right(0);
		}		
		
		//run the "align to wall on right"  behavior until it has finished, then turn it off
		result1 = align_to_wall_on_right(1);
		if(result1==5) 
		{
			align_to_wall_on_right(0);
			state=STATE_FIND_ROOM;
		}			
		break;
		/****************************************************************************/

		
		/****************************************************************************/
		case STATE_FIND_ROOM:
		
		if(STATE_HAS_CHANGED) 
		{
			motors_set(140,(sint16)(140*1.07f));
			follow_right_wall_and_turn(0);
			align_to_line(0);
		}			
		
		result1 = follow_right_wall_and_turn(1);
		result2 = align_to_line(1);
		if(result2==2)
		{
			state = STATE_FOUND_DOOR;
			follow_right_wall_and_turn(0);
		}
		break;
		/****************************************************************************/
		
		
		/****************************************************************************/
		case STATE_FOUND_DOOR:
		
		if(STATE_HAS_CHANGED) {}

		result2 = align_to_line(1); //finish the alignment.
		if(result2==3)
		{
			state = STATE_FIND_FLAME;
			align_to_line(0);
		}
		
		break;
		/****************************************************************************/
		
		
		/****************************************************************************/
		case STATE_FIND_FLAME:
				
		if(STATE_HAS_CHANGED) 
		{
			find_flame_fsm(0);
		}
		result1 = find_flame_fsm(1);
		

		break;
		/****************************************************************************/		
		
	}
	s.inputs.fsm_states[0] = state;
	return state;
}
#endif

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


u08 lines_crossed=0;

void line_detection_fsm(void)
{
	enum states { s_none=0, s_disabled=1, s_not_sure, s_on_black, s_on_line, s_crossed_line };
	static enum states state=s_disabled;
	static enum states last_state=s_none;
	
	task_open();

	while(1)
	{
		first_(s_disabled)
		{
			enter_(s_disabled) 	{}
			state = s_not_sure;	leave_(s_disabled);
			exit_(s_disabled)  {}
		}


		next_(s_not_sure)
		{
			enter_(s_not_sure) 	{}
			if(s.line[0]>100) state = s_on_black;
			exit_(s_not_sure)  {}
		}


		next_(s_on_black)
		{
			enter_(s_on_black) 	{}
			if(s.line[0]<20) state = s_on_line;
			exit_(s_on_black)  {}
		}


		next_(s_on_line)
		{
			enter_(s_on_line) 	{}
			if(s.line[0]>100) state = s_crossed_line;
			exit_(s_on_line)  {}
		}


		next_(s_crossed_line)
		{
			enter_(s_crossed_line) 	{ lines_crossed++; }
			state = s_on_black;
			exit_(s_crossed_line)  {}
		}
		OS_SCHEDULE;
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

#define move(speed,distance) \
	odometry_set_checkpoint(); \
	motor_command(6,0,0,(speed),(speed)); \
	while ( abs(odometry_get_distance_since_checkpoint()) < (distance) ) { task_wait(10); } \
	motor_command(7,0,0,0,0)


#define turn(speed,angle) \
	odometry_set_checkpoint(); \
	motor_command(6,0,0,(speed),-(speed)); \
	while ( abs(odometry_get_rotation_since_checkpoint()) < angle ) { task_wait(10); } \
	motor_command(7,0,0,0,0)


void master_logic_fsm(void)
{
	enum states 
	{ 
		s_none=0, 
		s_disabled=1, 
		s_waiting_for_start, 
		s_aligning_south, 
		s_finding_room_3, 
		s_searching_room_3,
		s_finding_room_2,
		s_searching_room_2,
		s_finding_room_1,
		s_searching_room_1,
		s_finding_room_4,
		s_searching_room_4
	};
	static enum states state=s_disabled;
	static enum states last_state=s_none;
	
	task_open();

	while(1)
	{
		//the following state transition applies to all states
		if(s.behavior_state[3]==0) state = s_disabled;

		first_(s_disabled)
		{
			enter_(s_disabled) 
			{  
				motor_command(6,0,0,0,0);
				s.behavior_state[1] = 0;
				s.behavior_state[2] = 0;
				s.behavior_state[3] = 0;
			}

			if(s.behavior_state[3]==1) state = s_waiting_for_start;

			exit_(s_disabled)  
			{ 
			}
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_waiting_for_start)
		{
			enter_(s_waiting_for_start) { }

			//just fall through for now
			state = s_aligning_south;

			exit_(s_waiting_for_start) { }
		}
		
		//-------------------------------------------------------------------------------------------------------

		next_(s_aligning_south)
		{
			enter_(s_aligning_south) { }
			
			turn(20,90); //turn 90 degrees right @ speed 20
			task_wait(200);

			if(s.ir[AI_IR_N] < 120)  //something right in front of us?
			{
				//if so, then we were facing south initially, now we are facing west; turn back...
				turn(-20,90); //turn 90 degrees left @ speed 20
				task_wait(200);
			}
			state = s_finding_room_3;

			exit_(s_aligning_south) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_finding_room_3)
		{
			enter_(s_finding_room_3)
			{
				//activate "follow right wall"
				s.behavior_state[2]=1; //right wall
				s.behavior_state[1]=1; //start wall following
				lines_crossed = 0;
			}

			if(lines_crossed>0)
			{
				motor_command(2,0,0,0,0);
				s.behavior_state[1] = 0;  s.behavior_state[2] = 0;  s.behavior_state[3] = 0;
				state = s_searching_room_3;
			}

			exit_(s_finding_room_3) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_searching_room_3)
		{
			enter_(s_searching_room_3) { }
			//move 10cm into the room
			move(20,100);
			task_wait(200);

			//we are facing more or less N right now.  turn right about 120degrees so that we are facing SE
			turn(20,120);
			task_wait(200);

			//now start to scan for the flame by turning left about 250 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			turn(-20, 250 );
			task_wait(200);

			//if there was no candle, go on to the next room.
			//we should be facing the wall more or less SW, so we just need to start following the right wall
			state = s_finding_room_2;

			exit_(s_searching_room_3) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_finding_room_2)
		{
			enter_(s_finding_room_2)
			{
				//activate "follow right wall"
				s.behavior_state[2]=1; //right wall
				s.behavior_state[1]=1; //start wall following
				lines_crossed = 0;
			}

			if(lines_crossed>0)
			{
				motor_command(2,0,0,0,0);
				s.behavior_state[1] = 0;  s.behavior_state[2] = 0;  s.behavior_state[3] = 0;
				state = s_searching_room_2;
			}

			exit_(s_finding_room_2) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_searching_room_2)
		{
			enter_(s_searching_room_2) { }
			//move 10cm into the room
			move(20,100);
			task_wait(200);

			//we are facing more or less W right now.  turn right about 120degrees so that we are facing NE
			turn(20,120);
			task_wait(200);

			//now start to scan for the flame by turning left about 250 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			turn(-20, 250 );
			task_wait(200);

			//if there was no candle, go on to the next room.
			//we should be facing the wall more or less SE, we just need to start following the right wall
			state = s_finding_room_1;

			exit_(s_searching_room_2) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_finding_room_1)
		{
			enter_(s_finding_room_1)
			{
				//activate "follow right wall"
				s.behavior_state[2]=1; //right wall
				s.behavior_state[1]=1; //start wall following
				lines_crossed = 0;
			}

			if(lines_crossed>0)
			{
				motor_command(2,0,0,0,0);
				s.behavior_state[1] = 0;  s.behavior_state[2] = 0;  s.behavior_state[3] = 0;
				state = s_searching_room_1;
			}

			exit_(s_finding_room_1) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_searching_room_1)
		{
			enter_(s_searching_room_1) { }
			//move 10cm into the room
			move(20,100);
			task_wait(200);

			//we are facing more or less E right now.  turn left about 120degrees so that we are facing NW
			turn(-20,120);
			task_wait(200);

			//now start to scan for the flame by turning right about 250 degrees
			//while turning, keep a history of the flame data so we can detect the peak and hence can hone in on the candle.
			turn(20, 250 );
			task_wait(200);

			//if there was no candle, go on to the next room.
			//we should be facing more or less SW, but too far away from the wall depending on door location
			
			//we first need to find the wall before we can follow it; let's turn left to face SE, then go straight towards the wall
			turn(-20,45);
			task_wait(200);

			state = s_finding_room_4;

			exit_(s_searching_room_1) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_finding_room_4)
		{
			enter_(s_finding_room_4) { }
			exit_(s_finding_room_4) {}
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_searching_room_4)
		{
			enter_(s_searching_room_4) { }
			exit_(s_searching_room_4) {}
		}


		task_wait(25);
	}

	task_close();
}


#define move_(speed,distance) \
	odometry_set_checkpoint(); \
	motor_command(6,0,0,(speed),(speed)); \
	while ( abs(odometry_get_distance_since_checkpoint()) < (distance) ) { task_wait(2); } \
	motor_command(2,0,0,0,0)


#define turn_(speed,angle) \
	odometry_set_checkpoint(); \
	motor_command(6,0,0,(speed),-(speed)); \
	while ( abs(odometry_get_rotation_since_checkpoint()) < angle ) { task_wait(2); } \
	motor_command(2,0,0,0,0)


void test(void)
{
	task_open();

	while(1)
	{
		task_wait(100);
		motor_command(7,0,0,(80),(60)); \
		//move_(20,150);
		//turn_(-20,90);
		//move_(20,150);
		while(1) 
		{
			task_wait(10);
			NOP();
		}
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
	play_note(A(4), 50, 10);


	//initialize the configuration parameter module
	cfg_init();


	//initialize misc support libraries
	LOOKUP_init();
	SHARPIR_init();
	//PID_init();  //not actually using the PID module right now
	//servos_start(demuxPins, sizeof(demuxPins));
	//set_servo_target(0, 1375);


	//initialize our state
	memset(&s,0,sizeof(t_state));
	s.inputs.vbatt=10000;
	//LOOKUP_initialize_table(line_sensor_table);  //only needed if we need to normalize left & right line sensor for some reason


	//initialize the cooperative multitasking subsystem and start all tasks
	os_init();
	#if 1
	serial_cmd_evt = event_create();

#ifdef SVP_ON_WIN32
	task_create( sim,						 1,  NULL, 0, 0);

	task_create( test,						 2,  NULL, 0, 0);
#endif

	task_create( lcd_update_fsm,			10,  NULL, 0, 0 );		
	task_create( analog_update_fsm,			11,  NULL, 0, 0 );	
	task_create( serial_send_fsm,			12 , NULL, 0, 0 );		
	task_create( serial_receive_fsm,		13,  NULL, 0, 0);
	task_create( commands_process_fsm,		14,  NULL, 0, 0);
	task_create( motor_command_fsm,			15,  NULL, 0, 0);
	task_create( ultrasonic_update_fsm,		16,  NULL, 0, 0);
	//task_create( debug_fsm, 17, NULL, 0, 0); //not used right now
	task_create( wall_follow_fsm,			18,  NULL, 0, 0);
	task_create( master_logic_fsm,			19,  NULL, 0, 0);
	task_create( line_detection_fsm,		20,  NULL, 0, 0);


	#else
	task_create( fsm_test_task, 1, NULL, 0, 0 );
	#endif
	os_start();
	
	//won't ever get here...
		
	return 0;
}

