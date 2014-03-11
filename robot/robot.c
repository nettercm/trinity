

#include "standard_includes.h"

const unsigned char pulseInPins[] = { IO_US_ECHO_AND_PING_1 , IO_US_ECHO_AND_PING_2 };

extern int svp_demo(void);


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

volatile unsigned long elapsed_milliseconds=0;


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




#define STATE_HAS_CHANGED ( last_state != state ? (last_state=state) : 0)
#define ENTER(s) if(STATE_HAS_CHANGED)
#define enter_(s) ENTER(s)
#define EXIT(s) _label ## s: if(state!=last_state)
#define exit_(s) EXIT(s)
#define FIRST_STATE(s) if(state==s)
#define first_(s) FIRST_STATE(s)
#define NEXT_STATE(s) else if(state==s)
#define next_(s) NEXT_STATE(s)
#define SWITCH_STATE(cs,ns) state=ns;goto _label ## cs;
#define switch_(cs,ns) SWITCH_STATE(cs,ns)
#define LEAVE_STATE(cs) goto _label ## cs;
#define leave_(cs) LEAVE_STATE(cs)

volatile char _dummy_;
#define NOP() _dummy_++;



static void fsm_test_task(void)
{
	static char state=1, last_state=-1;
	
	task_open();
	while(1)
	{
		first_(1)
		{
			ENTER(1)
			{
				//dbg("Entering state %d\n",state);
				NOP();
			}
			
			state = 2;
			leave_(1);
			//SWITCH_STATE(1,2);
			NOP();
			
			EXIT(1)
			{
				NOP();
			}
		}
		NEXT_STATE(2)
		{
			enter_(2)
			{
				//dbg("Entering state %d\n",state);
				NOP();
			}
			
			state = 1;
			
			EXIT(2)
			{
				NOP();
			}
		}
		NOP();
		OS_SCHEDULE; //task_wait(1);
	}
	task_close();
}



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

		s.line[0] = 0; //s.inputs.analog[AI_LINE_LEFT];
		s.line[1] = 0; //(u08) LOOKUP_do(s.inputs.analog[AI_LINE_RIGHT],line_sensor_table);

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
			printf("L=%3d,%3d", s.inputs.analog[AI_LINE_RIGHT],s.inputs.analog[AI_LINE_LEFT]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 	
			printf("Fl=%03d %03d", s.inputs.analog[AI_FLAME_NE],s.inputs.analog[AI_FLAME_N]); 
		}
		else if(s.lcd_screen==1)
		{
			lcd_goto_xy(0,0); 
			printf("US:  %4d %4d",  s.inputs.sonar[0],s.inputs.sonar[1]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 
			printf("AVG: %4d %4d",  s.us_avg[0], s.us_avg[1]);
		}
		else if(s.lcd_screen==2)
		{
			lcd_goto_xy(0,0); 
			printf("i%03d %03d %03d %03d",s.ir[AI_IR_NE], s.ir[AI_IR_N_long], s.ir[AI_IR_N], s.ir[AI_IR_NW]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 
			printf("u%03d %03d %03d",  us_nw, us_n, us_ne); 
		}
		else if(s.lcd_screen==3)
		{
			lcd_goto_xy(0,0);   task_wait(5);
			printf("V=%5d",	read_battery_millivolts_svp());     task_wait(5);
			lcd_goto_xy(0,1);   task_wait(5);
			printf("V=%5d,  %5d",	s.inputs.vbatt, count);     task_wait(5);
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

void commands_process(void)
{
	uint8 *c; //index into the data/payload 
	uint8 cmd;
	t_config_value *v;
	t_motor_command *motor_cmd;
	t_set_motors_cmd *set_motors_cmd;
		
	c = &(s.commands.d[0]);
	
	while( (cmd = *c) != 0 )  //0 means no more commands
	{
		c++;
		switch(cmd)
		{
			case CMD_SET_MOTORS:
				s.lm_target = s.lm_actual = ((t_set_motors_cmd*)c)->lm;
				s.rm_target = s.rm_actual = ((t_set_motors_cmd*)c)->rm;
				//don't actually update the motors just yet - let the motor command fsm do that
				//set_motors( s.lm_target , s.rm_target );
				c+=sizeof(t_set_motors_cmd);
			break;

			case CMD_MOTOR_COMMAND:
				motor_cmd = (t_motor_command*)c;
				motor_command(motor_cmd->cmd, motor_cmd->p1, motor_cmd->p2, motor_cmd->lm, motor_cmd->rm);
				c+=sizeof(t_motor_command);
			break;

			case CMD_RESET_ENCODERS:
				encoders_reset();
			break;
			
			case CMD_SET_BEHAVIOR_STATE:
				play_note(A(4), 50, 10);			
				s.behavior_state[ c[0] ] = c[1];
				usb_printf("CMD_SET_BEHAVIOR_STATE %02x %02x\r\n",c[0],c[1]);
				if((c[0]==1) && (c[1]==1)) ultrasonic_set_sequence(us_sequence_W_priority); else ultrasonic_set_sequence(us_sequence_uniform);
				c+=2;
			break;
			
			case CMD_SET_SONAR_SEQUENCE:
			break;
			
			case CMD_SET_SONAR_TIMEOUT:
				ultrasonic_set_timeout(c[0]);
			break;

			case CMD_SET_CONFIG_VALUE:
				v = (t_config_value *)&(c[2]);
				usb_printf("config %d,%d = %d\r\n",c[0],c[1],v->s16);
				cfg_set_value_by_grp_id(c[0], c[1], *v);
				c+=6;
			break;
			
			default:
			break;
		}
		
	}
	
}


void commands_process_fsm(void)
{
	task_open();
	
	while(1)
	{
		event_wait(serial_cmd_evt);
		commands_process();
	}
	
	task_close();
}



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

uint8 master_logic_fsm(uint8 cmd)
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

#if 0
void encoder_test(void)
{
	while(1)
	{
		uint32 t1,t2;
		int i;
		volatile int l=0,r=0;
		
		t1 = get_ticks();
		for(i=0;i<1000;i++) //takes about 18ms;  library updates encoder data only once per every ms
		{
			l = l + svp_get_counts_and_reset_ab();
			r = r + svp_get_counts_and_reset_cd();
		}
		t2 = get_ticks();
		clear();
		lcd_goto_xy(0,0); printf("t = %lu",t2-t1);
		lcd_goto_xy(0,1); printf("l = %d,  r = %d",l,r);
	}
}
#endif

uint8 console_buffer[128];
void console(void)
{
	static uint8 state=0;
	uint8 b=0;
	
	switch(state)
	{
		case 0:
		serial_receive(USB_COMM,console_buffer,127);
		state++;
		break;
		
		case 1:
		if( (b=serial_get_received_bytes(USB_COMM)) != 0 ) 
		{
			console_buffer[b]=0;
			usb_printf("%s",console_buffer);
		}
		state = 0;
		break;
	}
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
		elapsed_milliseconds+=t_delta;
		os_task_tick(0,(unsigned short)t_delta);
	}
#else
	t_last++;
	if(t_last > 2)
	{
		//Sleep(1);
		t_last = 0;
		t_delta = 1;
		//elapsed_milliseconds += t_delta;
		delay_ms(1); //this will update elapsed_milliseconds
		os_task_tick(0,(unsigned short)t_delta);
	}
#endif
}

volatile unsigned long idle_counter=0;
void _os_idle(void)
{
	idle_counter++;
}


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

/*
	uint32 timestamp;
	uint8 analog[8];		//8

	uint16 sonar[4];		//8
	uint16 ir[4];			//8

	sint16 encoders[2];		//4
	sint16 actual_speed[2];	//4
	sint16 target_speed[2]; //4
	sint16 motors[2];		//4
	float x,y,theta;		//12
	//uint8 din;
	//uint8 buttons[3];
	uint16 vbatt;			//2
	uint16 flags;			//2
	//uint8  fsm_states[8];	//8
	sint16 watch[4];		//8
*/

t_inputs sim_data[] =
{
//		t		a0	a1	a2	a3	a4	a5	a6	a7		s0	s1	s2	s3		i0	i1	i2	i3		enc			act. spd.	tgt. spd.	motors		x,y,thta	vb	f	watch
	{	0,		0,	0,	0,	0,	0,	0,	0,	0,		0,	0,	0,	0,		0,	0,	0,	0,		0,	0,		0,	0,		0,	0,		0,	0,		0,0,0,		0,	0,	0,0,0,0		},
	{	5,		0,	0,	0,	0,	0,	0,	0,	0,		0,	0,	0,	0,		0,	0,	0,	0,		0,	0,		0,	0,		0,	0,		0,	0,		0,0,0,		0,	0,	0,0,0,0		}
};


void sim(void)
{
	task_open();
	
	while(1)
	{
		task_wait(1);
	}
	task_close();
}


void debug_fsm(void)
{
	uint8 b=0;
	task_open();
	
	usb_printf("debug_fsm()\n");
	serial_receive(USB_COMM,(char*)console_buffer,127);

	while(1)
	{
		serial_check();
		if( (b=serial_get_received_bytes(USB_COMM)) != 0 )
		{
			console_buffer[b]=0;
			usb_printf("%s",console_buffer);
			serial_receive(USB_COMM,(char*)console_buffer,127);
		}
		OS_SCHEDULE;
	}
	
	task_close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define is_nth_iteration(counter, n) (counter++ >= (n) ?  counter=0, 1 : 0)
#define invalid_error_value 9999

void behaviors_fsm(void)
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
	
	DEFINE_CFG2(u08,interval,			10,1);
	DEFINE_CFG2(s16,nominal_speed,		10,2);
	DEFINE_CFG2(s16,target_distance,	10,3);
	DEFINE_CFG2(s16,max_error,			10,4);
	DEFINE_CFG2(s16,max_correction,		10,5);
	DEFINE_CFG2(s16,Kp,					10,6);
	DEFINE_CFG2(s16,Ki,					10,7);
	DEFINE_CFG2(s16,Kd,					10,8);
	DEFINE_CFG2(s16,min_speed,			10,9);
	DEFINE_CFG2(s16,up_ramp,			10,10);
	DEFINE_CFG2(s16,down_ramp,			10,11);
	DEFINE_CFG2(u08,use_corner_logic,	10,12);
	DEFINE_CFG2(flt,corner_distance,	10,13);
	DEFINE_CFG2(s16,corner_speed,		10,14);
	DEFINE_CFG2(s16,integral_limit,		10,15);
	DEFINE_CFG2(s16,lost_wall_distance,	10,16);
	DEFINE_CFG2(s16,found_wall_distance,10,17);
	DEFINE_CFG2(s16,corner_radius,		10,18);
	DEFINE_CFG2(s16,sharp_corner_radius,10,19);
	
	static s16 integral=0;
	static s16 last_error=invalid_error_value;
	

	
	//coder here gets executing every time this "task" runs


	//code that comes after this resumes where it left off
	task_open();
	
	PREPARE_CFG2(interval);
	PREPARE_CFG2(nominal_speed);
	PREPARE_CFG2(target_distance);
	PREPARE_CFG2(max_error);
	PREPARE_CFG2(max_correction);
	PREPARE_CFG2(Kp);
	PREPARE_CFG2(Ki);
	PREPARE_CFG2(Kd);
	PREPARE_CFG2(min_speed);
	PREPARE_CFG2(up_ramp);
	PREPARE_CFG2(down_ramp);
	PREPARE_CFG2(use_corner_logic);
	PREPARE_CFG2(corner_distance);
	PREPARE_CFG2(corner_speed);
	PREPARE_CFG2(integral_limit);
	PREPARE_CFG2(lost_wall_distance);
	PREPARE_CFG2(found_wall_distance);
	PREPARE_CFG2(corner_radius);
	PREPARE_CFG2(sharp_corner_radius);
	
	while(1)
	{
		UPDATE_CFG2(interval);
		UPDATE_CFG2(nominal_speed);
		UPDATE_CFG2(target_distance);
		UPDATE_CFG2(max_error);
		UPDATE_CFG2(max_correction);
		UPDATE_CFG2(Kp);
		UPDATE_CFG2(Ki);
		UPDATE_CFG2(Kd);
		UPDATE_CFG2(min_speed);
		UPDATE_CFG2(up_ramp);
		UPDATE_CFG2(down_ramp);
		UPDATE_CFG2(use_corner_logic);
		UPDATE_CFG2(corner_distance);
		UPDATE_CFG2(corner_speed);
		UPDATE_CFG2(integral_limit);
		UPDATE_CFG2(lost_wall_distance);
		UPDATE_CFG2(found_wall_distance);
		UPDATE_CFG2(corner_radius);
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
			enter_(s_lost_wall) { s.inputs.x = s.inputs.y = s.inputs.theta = 0;  play_note(E(3), 50, 10); }
			
			if(target_speed > corner_speed) target_speed -= down_ramp;
			if(target_speed < corner_speed) target_speed += up_ramp;
			motor_command(8,0,0,target_speed,target_speed);
			if( s.inputs.x >=  corner_distance) state = s_turning_corner;
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
				s.inputs.x = s.inputs.y = s.inputs.theta = 0; 
			}
			
			if(target_speed > corner_speed) target_speed -= down_ramp;
			if(target_speed < corner_speed) target_speed += up_ramp;
			if(which_wall==0) motor_command(8,0,0,(target_speed*10)/corner_radius,target_speed);
			else motor_command(8,0,0,target_speed,(target_speed*10)/corner_radius);
	
			if( side <= target_distance ) state = s_tracking_wall;
			if ( abs((s.inputs.theta/(2*3.1415926535))*360.0) >= 90 ) 
			{
				state = s_tracking_wall; //by default, go back to racking the wall, unless....
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
				s.inputs.x = s.inputs.y = s.inputs.theta = 0;
			}
			if(target_speed > corner_speed) target_speed -= down_ramp;
			if(target_speed < corner_speed) target_speed += up_ramp;
			if(which_wall==0) motor_command(8,0,0,(target_speed*10)/sharp_corner_radius,target_speed);
			else motor_command(8,0,0,target_speed,(target_speed*10)/sharp_corner_radius);
			
			if( side <= target_distance ) state = s_tracking_wall;
			if ( abs((s.inputs.theta/(2*3.1415926535))*360.0) >= 90.0 )
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


int main(void)
{
	uint32 t1,t2,t3;
	int loops=0;
	
	memset(&s,0,sizeof(t_state));
	s.inputs.vbatt=10000;
	
	//odometry_update(5,5);
	//odometry_update(-100,-100);
	//unit_test();

	//if(button_is_pressed(TOP_BUTTON)) svp_demo();
	//if(button_is_pressed(MIDDLE_BUTTON)) servo_test();
	//if(button_is_pressed(BOTTOM_BUTTON)) compass_main();
	

	//initialize hardware & pololu libraries
	//i2c_init();
	hardware_init();
	//clear(); lcd_goto_xy(0,0); printf("V=%d",	read_battery_millivolts_svp());
	
	/*
	t1=get_ticks(); 	usb_printf("0"); 			t2=get_ticks();  	usb_printf("\ntime for usb_printf(1) : %ld us\n", ticks_to_microseconds(t2-t1));
	t1=get_ticks(); 	usb_printf("01"); 			t2=get_ticks();  	usb_printf("\ntime for usb_printf(2) : %ld us\n", ticks_to_microseconds(t2-t1));
	t1=get_ticks(); 	usb_printf("0123456789"); 	t2=get_ticks();  	usb_printf("\ntime for usb_printf(10): %ld us\n", ticks_to_microseconds(t2-t1));
	*/
	//DBG_printf(1,("DBG_printf()\n"));
	
	//delay_ms(100);

	//wait_for_start_button();
	play_note(A(4), 50, 10);
	
	LOOKUP_init();
	SHARPIR_init();
	PID_init();
	SHARPIR_init();
	LOOKUP_initialize_table(line_sensor_table);

	//PID_test();

	//servos_start(demuxPins, sizeof(demuxPins));
	//set_servo_target(0, 1375);

	cfg_init();
	cfg_test();
	
	//s.inputs.vbatt = 10500;
	//motor_command(7,0,0,-10,-10);


	os_init();
	#if 1
	serial_cmd_evt = event_create();
#ifndef SVP_ON_WIN32
	task_create( lcd_update_fsm, 1, NULL, 0, 0 );		
	task_create( analog_update_fsm, 2, NULL, 0, 0 );	
	task_create( serial_send_fsm, 3, NULL, 0, 0 );		
	task_create( commands_receive_fsm, 4, NULL, 0, 0);
	task_create( commands_process_fsm, 5, NULL, 0, 0);
	task_create( motor_command_fsm, 6, NULL, 0, 0);
	task_create( ultrasonic_update_fsm, 7, NULL, 0, 0);
	task_create( debug_fsm, 8, NULL, 0, 0);
#endif
	task_create( behaviors_fsm, 9, NULL, 0, 0);
	//task_create( idle, 10, NULL, 0, 0);

#ifdef SVP_ON_WIN32
	task_create( sim, 255, NULL, 0, 0);
#endif

	#else
	task_create( fsm_test_task, 1, NULL, 0, 0 );
	#endif
	os_start();
	
	//won't get here...
		
	return 0;
}



typedef struct 
{
	//u08 priority;
	s16 speed;
	u08 speed_flag; //1 if the behavior wants to control speed
	s16 heading;
	u08 heading_flag; //1 if the behavior wants to control heading
	u16 persistance;  //>0 means the behavior is executing a ballistic maneuver and needs n cycles or milliseconds
} t_behavior_output;

/*
behavior input:
* enable/disable flag
* state
* sensors
* indication if this behavior won arbitration during the previous update cycle

behavior output:
* target speed
* heading
* duration (for ballistic behaviors / actions)
* future:  sensor focus (e.g. priority for sonar sensors;  which way to pan the servo-mounted sensors to)
* future:  sound

behaviors can leave speed or heading "blank"
the highest priority behavior w/ speed output controls speed

arbitration output:
* speed
* heading
* duration
* speed winner
* heading winner

arbitration:
for priority = 1 (lowest) to #behaviors / highest
	IF behavior_output[priority].speed_flag == 1  THEN  
		arbitration_output.speed = behavior_output[priority].speed;
		arbitration_output.speed_winner = priority;
	ENDIF
	IF behavior_output[priority].heading_flag == 1  THEN
		arbitration_output.heading = behavior_output[priority].heading;
		arbitration_output.heading_winner = priority;
	ENDIF
	

*/
