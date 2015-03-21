
#ifdef WIN32
#include <Windows.h>
#include <timeapi.h>
#endif



/*
Issues:

•	When the flame is close to a wall, the flame’s reflection from the wall can max out the flame sensors => may end up to looking straight at the candle when stopped
	o	Do a sweep and use IR and/or flame sensor data to find the correct peak position
	o	If the wall is known to be on the left, sweep from the right until the right sensor is maxed out

•	When doing a 360 spin to look for the candle, need to make sure we don’t get confused by any IR potentially coming from outside the room
	o	Need to look at the angle associated with the reading, i.e. ignore if it is coming from behind the robot
	o	Don’t do a 360 degree spin – turn right >90deg and then left >180deg

•	If the candle is in the middle of a large room, the robot will crash into it if we are approaching it off-center (sonar won’t see it)
	o	Solution: add a NE and NW facing sonar

•	When exiting room number 1 after extinguishing,  the door chosen depends on where the candle was

•	line detection: signal-to-noise ration on analog[10]/right line is bad;   also,  missing the entrance to room 1 (maybe exit from room 2 confuses the line detector)

•	acceleration from 0 to min speed / turn speed needs to be more smooth if target speed is > 30

•	robot does not go into room 3 if door opening is < 16". 
	o	may have to do w/ recently added front sensor logic, 
	o	it improved after tweaking wall_following.c line 120)

•	when turning left after exiting from room 1, robot is way too far to the right

•	too slow when make 90deg inside turns


*/



#include "standard_includes.h"

const unsigned char pulseInPins[] = PULSE_PINS; 
const unsigned char demuxPins[] = SERVO_DEMUX_PINS; // eight servos

extern void commands_process_fsm(u08 cmd, u08 *param);
extern void lcd_update_fsm(u08 cmd, u08 *param);
extern void analog_update_fsm(u08 cmd, u08 *param);
extern u08 line_alignment_fsm_v2(u08 cmd, u08 *param);
extern void find_flame_fsm(u08 cmd, u08 *param);

//char buffer[128];

uint32 t_last_output_update = 0;

const char welcome[] PROGMEM = ">g32>>c32";

u08 ml_tid,us_tid;


	
void stop_all_behaviours(void)
{
	uint8 i;
	
	for(i=0;i<16;i++) s.behavior_state[i] = 0;
}


//does not initialize servos
int hardware_init(void)
{
	UCSR1A = 0;
	DDRD = 0;

	//analog
	set_analog_mode(MODE_10_BIT); // 10-bit analog-to-digital conversions

	//Make SSbar be an output so it does not interfere with SPI communication.
	//SSbar == PUMP_PIN
	set_digital_output(PUMP_PIN, LOW);

	set_digital_input(SOUND_START_PIN, PULL_UP_ENABLED);
	//set_digital_input(BUTTON_START_PIN, PULL_UP_ENABLED);

	//analog mux pins
	set_digital_output(ANALOG_MUX_ADDR_0_PIN, LOW);
	set_digital_output(ANALOG_MUX_ADDR_1_PIN, LOW);
	set_digital_output(ANALOG_MUX_ADDR_2_PIN, LOW);
	set_digital_output(ANALOG_MUX_ADDR_3_PIN, LOW);

	
	//sonar pins	
	set_digital_output(IO_US_ECHO_AND_PING_1, LOW);
	set_digital_output(IO_US_ECHO_AND_PING_2, LOW);
	set_digital_output(IO_US_ECHO_AND_PING_3, LOW);
	set_digital_output(IO_US_ECHO_AND_PING_4, LOW);
	set_digital_output(IO_US_ECHO_AND_PING_5, LOW);
	set_digital_input (IO_US_ECHO_AND_PING_1 ,HIGH_IMPEDANCE);
	set_digital_input (IO_US_ECHO_AND_PING_2, HIGH_IMPEDANCE);
	set_digital_input (IO_US_ECHO_AND_PING_3, HIGH_IMPEDANCE);
	set_digital_input (IO_US_ECHO_AND_PING_4, HIGH_IMPEDANCE);
	set_digital_input (IO_US_ECHO_AND_PING_5, HIGH_IMPEDANCE);
	
	//uvtron pulse	
	//set_digital_input(IO_UV_PULSE, HIGH_IMPEDANCE);
	
	//set baud rate etc.
	serial_hardware_init();

	lcd_init_printf();

	motors_hardware_init();
	
	pulse_in_start(pulseInPins, sizeof(pulseInPins));		// start measuring pulses (1 per each sonar, +1 for sound start)
	
	return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





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
	ir_n = s.ir[IR_N];	   if(ir_n >  600) ir_n =  600;
	ir_fn= ir_n; //s.ir[IR_FAR_N]; if(ir_fn > 600) ir_fn = 600;


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
			scan_data[i].ir_north		= ir_n_avg; //s.ir[IR_N];
			scan_data[i].ir_far_north	= ir_fn_avg; //s.ir[IR_FAR_N];
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
		s_move_to_candle,		//11
		s_exit_from_room,		//12

		s_none=255
	};
	static enum states state=s_disabled;
	static enum states last_state=s_none;
	static u32 t_entry=0;
	static u08 still_inside_room=0;
	static u32 context_switch_counter=0;
	static u32 t_last=0;
	static u08 dog_position=0; //1=N side of Rm#4,  2=E side,    3=S side
	static u08 stop;
	static u08 last_room=0;

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
		if(s.behavior_state[MASTER_LOGIC_FSM]==0) state = s_disabled;


		first_(s_disabled)
		{
			enter_(s_disabled) 
			{  
				HARD_STOP(); //motor_command(cmd,accel,decel,0,0);
				PUMP_OFF();
				STOP_BEHAVIOR(FOLLOW_WALL_FSM);
				//STOP_BEHAVIOR(MASTER_LOGIC_FSM);
				RESET_LINE_DETECTION();
				dog_position=0;
				s.inputs.watch[0] = s.inputs.watch[1] = s.inputs.watch[2] = s.inputs.watch[3] = 0;
			}

			//if(s.behavior_state[MASTER_LOGIC_FSM]!=0) state = s.behavior_state[MASTER_LOGIC_FSM]; //s_waiting_for_start;
			state = s_waiting_for_start;
			s.behavior_state[MASTER_LOGIC_FSM] = s_waiting_for_start;

			exit_(s_disabled)  { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_waiting_for_start)
		{
			enter_(s_waiting_for_start) 
			{
				//TODO: should not be using low-level APIs here
				//TODO: number of US sensors is hardcoded here
				pulse_in_stop();
				pulse_in_start(pulseInPins,sizeof(pulseInPins)); //capture US sensor + sound start
			}

			if(check_for_start_signal()) 
			{
				state = s_aligning_south;
				//START_BEHAVIOR(TEST_LOGIC_FSM,5); //TODO:  fix this
			}
			else if(s.behavior_state[MASTER_LOGIC_FSM]!=0) state = s.behavior_state[MASTER_LOGIC_FSM]; //s_waiting_for_start;

			exit_(s_waiting_for_start)
			{
				pulse_in_stop();
				pulse_in_start(pulseInPins,sizeof(pulseInPins)-1); //only capture US sensor, i.e. skip the last one, which is for sound start
			}
		}
		

		//-------------------------------------------------------------------------------------------------------
		//		Align the robot so that it faces south
		//-------------------------------------------------------------------------------------------------------
		next_(s_aligning_south)
		{
			enter_(s_aligning_south) 
			{
				NOP();
			}
			
			TURN_IN_PLACE(turn_speed, -90);

			//is something right in front of us?
			//if so, then we were facing south initially, and now we are facing west; turn back...
			if( (s.ir[IR_N] < 120) || (s.inputs.sonar[0] < 120) )  TURN_IN_PLACE(turn_speed, 90);

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
				START_BEHAVIOR(FOLLOW_WALL_FSM,RIGHT_WALL); //start following the RIGHT wall
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
				STOP_BEHAVIOR(FOLLOW_WALL_FSM);
				HARD_STOP();
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
				task_wait(500);
			}

			//line_alignment_fsm_v2(1,0);  while(line_alignment_fsm_v2(0,0)!=0) { OS_SCHEDULE; }

			odometry_update_postion( ((float)(s.ir[IR_NW]))/16.0f , 65.0f, 90.0f);

			//TODO: read the omni-directional flame sensor to determine if this room contains the flame or not
			if(/* omni flame sensor sees the flame */1)
			{
				START_BEHAVIOR(FIND_FLAME_FSM,1);
				while(1)
				{
					OS_SCHEDULE;
				}
			}
			else
			{
				TURN_IN_PLACE(turn_speed, room3_turn_3);
				still_inside_room = 1;
				//at this point we should be able to just follow the wall a gain
				state = s_finding_room_2;
			}

			exit_(s_searching_room_3) 
			{ 
				NOP();
			}
		}



		//-------------------------------------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------------------------------------
		next_(s_finding_room_2)
		{
			enter_(s_finding_room_2)
			{
				START_BEHAVIOR(FOLLOW_WALL_FSM,RIGHT_WALL); //start following the RIGHT wall
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
					STOP_BEHAVIOR(FOLLOW_WALL_FSM);
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
			enter_(s_searching_room_2) 
			{ 
				s.current_room = 2;
			}

			//line_alignment_fsm_v2(1,0);  while(line_alignment_fsm_v2(0,0)!=0) { OS_SCHEDULE; }

			odometry_update_postion(27.0f, ((float)(s.ir[IR_NW]))/16.0f , 180.0f);

			//if there was no candle, go on to the next room.
			//we should be facing the wall more or less SE, we just need to start following the right wall
			still_inside_room = 1;
			RESET_LINE_DETECTION();

			TURN_IN_PLACE(turn_speed, room2_turn_3);

			state = s_finding_room_1;

			exit_(s_searching_room_2) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_finding_room_1)
		{
			enter_(s_finding_room_1)
			{
				START_BEHAVIOR(FOLLOW_WALL_FSM,RIGHT_WALL); //start following the RIGHT wall
			}

			if( LINE_WAS_DETECTED() )
			{
				//we'll be crossing the line while exiting from the prev. room!!!!
				if(still_inside_room) 
				{
					play_note(C(4), 50, 10);
					//s.inputs.watch[2]++;
					RESET_LINE_DETECTION();
					still_inside_room=0;
				}
				else
				{
					play_note(C(3), 50, 10);
					HARD_STOP();
					STOP_BEHAVIOR(FOLLOW_WALL_FSM);
					state = s_searching_room_1;
				}
			}

			exit_(s_finding_room_1) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_searching_room_1)
		{
			enter_(s_searching_room_1) 
			{ 
				s.current_room = 1; 
			}

			//line_alignment_fsm_v2(1,0);  while(line_alignment_fsm_v2(0,0)!=0) { OS_SCHEDULE; }

			//which door are we enterhing through?

			still_inside_room = 1;
			RESET_LINE_DETECTION();

			//if there was no candle, go on to the next room.
			//we should be facing more or less SW, but too far away from the wall depending on door location
			//we first need to find the wall before we can follow it; let's turn left to face SE, then go straight towards the wall
			TURN_IN_PLACE(turn_speed, room1_turn_3);

			motor_command(6,accel,decel,turn_speed,turn_speed);
			while( (s.ir[IR_NE] > 120) && (s.ir[IR_N] > 100) )
			{
				OS_SCHEDULE; //TODO: use parameters here!
			}
			motor_command(cmd,accel,decel,0,0);

			state = s_finding_room_4;

			exit_(s_searching_room_1) { }
		}

		//-------------------------------------------------------------------------------------------------------

		next_(s_finding_room_4)
		{
			enter_(s_finding_room_4) 
			{ 
				START_BEHAVIOR(FOLLOW_WALL_FSM,RIGHT_WALL); //start following the RIGHT wall
			}

			//keep following the right wall until we have passed through the door
			WAIT_FOR_LINE_DETECTION();

			//now we are facing N
			play_note(C(3), 50, 10);
			STOP_BEHAVIOR(FOLLOW_WALL_FSM);  //s.behavior_state[2] = 0;
			HARD_STOP();
			
			//now we are on the line that marks the north-side door of Room #1 - let's align ourselves to that line so that we are facing North
			line_alignment_fsm_v2(1,0);  while(line_alignment_fsm_v2(0,0)!=0) { OS_SCHEDULE; }
			//TODO: make a note how far away we are from the wall on the right, because it affects a maneuver further down. (but watch out for mirrors!)

			odometry_update_postion(89.0f, ((float)(s.ir[IR_NE]))/16.0f, 90.0f);


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
				START_BEHAVIOR(FOLLOW_WALL_FSM,LEFT_WALL); //start  following the LEFT wall

				//keep following the left wall until we have passed through the door
				WAIT_FOR_LINE_DETECTION();
				play_note(C(3), 50, 10);
				STOP_BEHAVIOR(FOLLOW_WALL_FSM);
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

			if(s.ir[IR_N] > 160)	//is there an opening right in front of us?
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
				START_BEHAVIOR(FOLLOW_WALL_FSM,LEFT_WALL);
				WAIT_FOR_LINE_DETECTION();

				//we reached room 4 - so proceed to the appropriate state
				play_note(C(3), 50, 10);
				STOP_BEHAVIOR(FOLLOW_WALL_FSM);
				HARD_STOP();
				RESET_LINE_DETECTION();
				switch_(s_finding_room_4, s_searching_room_4);
			}

			//at this point, we can conclude that the only way to get into Room #4 is to follow the right wall clock-wise around Rom #4
			
			TURN_IN_PLACE(turn_speed, 90);  //turn left to face West
			//keep following the left wall until we have passed through the door;  i.e. going counter-clockwise around Rm #4
			RESET_LINE_DETECTION();
			START_BEHAVIOR(FOLLOW_WALL_FSM,RIGHT_WALL);
			WAIT_FOR_LINE_DETECTION();

			//we reached room 4 - so proceed to the appropriate state
			play_note(C(3), 50, 10);
			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
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
			enter_(s_searching_room_4) 
			{ 
				s.current_room = 4; 
			}

			//line_alignment_fsm_v2(1,0);  while(line_alignment_fsm_v2(0,0)!=0) { OS_SCHEDULE; }
			//if(s.inputs.theta < (180.0f*K_deg_to_rad)) 	odometry_update_postion(57.0f, 56.0f, 90.0f);
			//else odometry_update_postion(66.0f, 76.0f, 270.0f);

			//we are facing either North or South, but it doesn't really matter...
			switch_(s_searching_room_4, s_move_to_candle);

			//if there was no candle, then we messed up, because Room #4 is alwasy the last room we search!
			//state = s_disabled;

			exit_(s_searching_room_4) {}
		}



		//-------------------------------------------------------------------------------------------------------
		//    Move closer to the candle and blow it out
		//-------------------------------------------------------------------------------------------------------
		next_(s_move_to_candle)
		{
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

			exit_(s_move_to_candle) {}
		}

		next_(s_exit_from_room)
		{
			enter_(s_exit_from_room) 
			{
				NOP();
			}

			//at this point we are looking straight at the candle
			TURN_IN_PLACE(50, 90);
			RESET_LINE_DETECTION();
			GO(50);
			while( (s.ir[IR_NE] > 120) && (s.ir[IR_N] > 100) && (s.inputs.sonar[0] > 100) ) 
			{
				OS_SCHEDULE; //TODO: use parameters here!
			}
			if( (s.ir[IR_N] < 100) || (s.inputs.sonar[0] < 100) ) 
			{
				TURN_IN_PLACE(50, 90);
			}
			START_BEHAVIOR(FOLLOW_WALL_FSM,LEFT_WALL);
			WAIT_FOR_LINE_DETECTION();
			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
			HARD_STOP();
			RESET_LINE_DETECTION();
			/*
			HARD_STOP();
			if(s.current_room==3) state = s_finding_room_2;
			if(s.current_room==2) state = s_finding_room_1;
			if(s.current_room==1) state = s_finding_room_4; //TODO: this one has different pre-conditions, so it won't work as is
			if(s.current_room==4) state = s_disabled;
			*/
			state = s_disabled;

			exit_(s_exit_from_room) 
			{
				NOP();
			}
		}

		s.inputs.watch[2]=state;
		if(s.behavior_state[3]!=0) s.behavior_state[3]=state;
		if(state!=last_state) 
		{
			dbg_printf("ML:state: %d->%d\n", last_state,state);
		}

		//task_wait(25);
 		OS_SCHEDULE;
	}

	task_close();
}
#pragma endregion


typedef struct
{
	u32 time;
	s16 l_speed;
	s16 r_speed;
} t_speed_profile;

t_speed_profile speed_profile[]=
{
	{   0,		0,		0},
	{ 500,		40,		-40},
	{1000,		100,	-100},
	{1500,		40,		-40},
	{2000,		100,	-100},
	{2500,		0,		0}
};

/*
D_ ( 221,   1,   9573,  T,  7,20.0,   V,10.1,  0x0000,  A, 27, 14,  0, 24,  0,  1,  2,  2,     E,    0,   -3,   O,-0.2, 0.0,-0.2,    S, 421, 392, 425,4000,  I, 997, 416, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ ( 222,   1,   9593,  T, 16,19.8,   V,10.1,  0x0000,  A, 26, 14,  0, 26,  0,  0,  0,  0,     E,    0,   -3,   O,-0.2, 0.0,-0.2,    S, 421, 358, 425,4000,  I, 997, 433, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ ( 223,   1,   9616,  T, 29,20.2,   V,10.1,  0x0000,  A, 26, 15,  2, 22,  0,  0,  0,  0,     E,    0,   -3,   O,-0.2, 0.0,-0.2,    S, 421, 358, 435,4000,  I, 997, 433, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ ( 224,   1,   9634,  T, 22,20.3,   V,10.1,  0x0000,  A, 27, 13,  2, 22,  0,  0,  1,  1,     E,    0,   -3,   O,-0.2, 0.0,-0.2,    S, 421, 358, 435,4000,  I, 997, 416, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ ( 225,   1,   9653,  T, 20,20.3,   V,10.1,  0x0000,  A, 27, 13,  1, 22,  0,  0,  0,  0,     E,    0,   -3,   O,-0.2, 0.0,-0.2,    S, 421, 358, 435,4000,  I, 997, 416, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ ( 226,   1,   9673,  T, 33,20.9,   V,10.1,  0x0000,  A, 27, 13,  1, 23,  0,  1,  1,  0,     E,    0,   -3,   O,-0.2, 0.0,-0.2,    S, 407, 358, 435,4000,  I, 997, 416, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ ( 227,   1,   9694,  T,  5,20.1,   V,10.1,  0x0000,  A, 27, 14,  0, 25,  2,  0,  3,  2,     E,    0,   -3,   O,-0.2, 0.0,-0.2,    S, 407, 390, 435,4000,  I, 997, 416, 997,  M, 40,-40,   0,  0,  69,-69,   W,  0,  0,  1,  0 )
D_ ( 228,   1,   9714,  T, 19,20.1,   V,10.0,  0x0000,  A, 28, 14,  1, 24,  0,  0,  1,  1,     E,    9,  -13,   O,-0.2, 0.0,-0.2,    S, 407, 390, 431,4000,  I, 997, 400, 997,  M, 40,-40,   9,-10,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 229,   1,   9735,  T, 22,20.2,   V,10.0,  0x0000,  A, 27, 12,  0, 25,  1,  2,  0,  0,     E,   28,  -32,   O,-0.3, 0.0,-1.3,    S, 407, 390, 431,4000,  I, 997, 416, 997,  M, 40,-40,  19,-19,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 230,   1,   9754,  T, 23,20.3,   V,10.0,  0x0000,  A, 27, 13,  0, 24,  0,  0,  0,  0,     E,   51,  -54,   O,-0.3, 0.0,-3.4,    S, 407, 390, 431,4000,  I, 997, 416, 997,  M, 40,-40,  23,-22,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 231,   1,   9773,  T, 13,20.0,   V,10.0,  0x0000,  A, 27, 14,  0, 30,  0,  1,  1,  1,     E,   80,  -81,   O,-0.2,-0.0,-6.0,    S, 386, 390, 431,4000,  I, 997, 416, 997,  M, 40,-40,  29,-27,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 232,   1,   9794,  T, 21,20.0,   V,10.0,  0x0000,  A, 12, 15, 25, 32,  0,  0,  0,  0,     E,  111, -111,   O,-0.1,-0.0,-9.2,    S, 386, 354, 431,4000,  I, 997, 520, 220,  M, 40,-40,  31,-30,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 233,   1,   9814,  T, 22,20.1,   V,10.0,  0x0000,  A, 13, 16, 26, 32,  1,  0,  1,  0,     E,  145, -144,   O,-0.0,-0.0,-12.7,    S, 386, 354, 430,4000,  I, 997, 581, 213,  M, 40,-40,  34,-33,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 234,   1,   9833,  T, 24,20.3,   V,10.0,  0x0000,  A,  1, 18, 38, 30,  0,  0,  0,  0,     E,  179, -177,   O, 0.1,-0.0,-16.6,    S, 386, 354, 430,4000,  I, 771, 720, 145,  M, 40,-40,  34,-33,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 235,   1,   9856,  T, 20,20.3,   V,10.0,  0x0000,  A,  0, 12, 39, 26,  0,  1,  1,  1,     E,  214, -211,   O, 0.1,-0.1,-20.4,    S, 386, 354, 430,4000,  I, 847, 813, 140,  M, 40,-40,  35,-34,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 236,   1,   9874,  T, 33,20.9,   V,10.0,  0x0000,  A,  0, 11, 29, 25,  1,  2,  2,  2,     E,  250, -246,   O, 0.2,-0.1,-24.4,    S, 464, 354, 430,4000,  I, 897, 875, 193,  M, 40,-40,  36,-35,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 237,   1,   9893,  T,  3,20.0,   V,10.0,  0x0000,  A,  0, 14, 29, 15, 11, 10,  9,  8,     E,  287, -281,   O, 0.3,-0.1,-28.4,    S, 464, 399, 430,4000,  I, 931, 916, 193,  M, 40,-40,  37,-35,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 238,   1,   9914,  T, 22,20.1,   V,9.9,  0x0000,  A, 25, 14,  1, 18,  9,  8,  8,  7,     E,  324, -317,   O, 0.4,-0.2,-32.5,    S, 464, 399, 525,4000,  I, 953, 450, 461,  M, 40,-40,  37,-36,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 239,   1,   9934,  T, 19,20.1,   V,9.9,  0x0000,  A, 27, 24,  2, 16, 11, 10, 11,  9,     E,  362, -351,   O, 0.5,-0.2,-36.7,    S, 464, 399, 525,4000,  I, 230, 416, 640,  M, 40,-40,  38,-34,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 240,   1,   9953,  T, 16,19.9,   V,9.9,  0x0000,  A, 24, 23,  2, 16, 11, 11, 11,  9,     E,  399, -386,   O, 0.7,-0.4,-40.9,    S, 464, 399, 525,4000,  I, 240, 466, 759,  M, 40,-40,  37,-35,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 241,   1,   9976,  T, 23,20.0,   V,9.9,  0x0000,  A, 24, 21,  0, 18, 10,  9,  9,  8,     E,  437, -422,   O, 0.9,-0.5,-45.0,    S, 443, 399, 525,4000,  I, 246, 466, 839,  M, 40,-40,  38,-36,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 242,   1,   9994,  T, 21,20.1,   V,9.9,  0x0000,  A, 15, 20,  0, 18, 10,  9,  8,  7,     E,  474, -457,   O, 1.0,-0.6,-49.2,    S, 443, 430, 525,4000,  I, 257, 528, 892,  M, 40,-40,  37,-35,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 243,   1,  10014,  T, 22,20.2,   V,9.9,  0x0000,  A, 15, 19,  0, 15,  6,  5,  5,  5,     E,  512, -493,   O, 1.1,-0.8,-53.3,    S, 443, 430, 605,4000,  I, 271, 569, 927,  M, 40,-40,  38,-36,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 244,   1,  10033,  T, 18,20.1,   V,9.9,  0x0000,  A, 30, 18,  1, 15,  5,  4,  6,  5,     E,  550, -528,   O, 1.2,-0.9,-57.6,    S, 443, 430, 605,4000,  I, 287, 380, 951,  M, 40,-40,  38,-35,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 245,   1,  10054,  T, 16,19.9,   V,9.9,  0x0000,  A, 30,  0,  0,  9,  2,  2,  2,  2,     E,  588, -563,   O, 1.3,-1.1,-61.8,    S, 443, 430, 605,4000,  I, 524, 380, 967,  M, 40,-40,  38,-35,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 246,   1,  10074,  T, 40,20.9,   V,9.9,  0x0000,  A, 69,  0,  8,  9,  2,  2,  1,  1,     E,  626, -598,   O, 1.4,-1.3,-65.9,    S, 185, 430, 605,4000,  I, 682, 168, 977,  M, 40,-40,  38,-35,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 247,   1,  10095,  T,  6,20.1,   V,9.9,  0x0000,  A, 70,  0,  8, 15,  2,  2,  2,  1,     E,  664, -633,   O, 1.5,-1.5,-70.1,    S, 185, 456, 605,4000,  I, 787, 167, 984,  M, 40,-40,  38,-35,  70,-70,   W,  0,  0,  1,  0 )
D_ ( 248,   1,  10114,  T, 18,20.0,   V,9.9,  0x0000,  A, 76,  0,  7, 16, 12, 10, 12, 10,     E,  702, -669,   O, 1.6,-1.7,-74.3,    S, 185, 456, 500,4000,  I, 857, 156, 989,  M, 40,-40,  38,-36,  71,-71,   W,  0,  0,  1,  0 )
D_ ( 249,   1,  10134,  T, 30,20.5,   V,9.9,  0x0000,  A, 76,  0,  8, 27, 11, 10, 10,  9,     E,  740, -704,   O, 1.6,-1.9,-78.6,    S, 185, 456, 500,4000,  I, 904, 156, 992,  M, 40,-40,  38,-35,  71,-71,   W,  0,  0,  1,  0 )
D_ ( 250,   1,  10153,  T,  4,19.7,   V,9.9,  0x0000,  A,  0, 15,  8, 27,  9, 10, 10,  9,     E,  778, -739,   O, 1.6,-2.1,-82.7,    S, 185, 456, 500,4000,  I, 935, 437, 994,  M, 40,-40,  38,-35,  71,-71,   W,  0,  0,  1,  0 )
D_ ( 251,   1,  10174,  T, 24,19.9,   V,9.9,  0x0000,  A,  1, 13,  7, 22, 10,  8,  8,  7,     E,  816, -775,   O, 1.7,-2.3,-86.9,    S, 600, 456, 500,4000,  I, 956, 624, 995,  M, 40,-40,  38,-36,  71,-71,   W,  0,  0,  1,  0 )
D_ ( 252,   1,  10194,  T, 17,19.8,   V,9.9,  0x0000,  A, 21, 14,  7, 22, 13, 12, 11,  9,     E,  854, -810,   O, 1.7,-2.5,-91.2,    S, 600, 194, 500,4000,  I, 970, 589, 996,  M,100,-100,  38,-35, 167,-167,   W,  0,  0,  1,  0 )
D_ ( 253,   1,  10216,  T, 22,19.9,   V,9.6,  0x0000,  A, 21, 15,  6, 23, 12, 11, 10,  9,     E,  902, -855,   O, 1.7,-2.7,-95.3,    S, 600, 194, 469,4000,  I, 979, 566, 997,  M,100,-100,  48,-45, 170,-170,   W,  0,  0,  1,  0 )
D_ ( 254,   1,  10234,  T, 18,19.8,   V,9.6,  0x0000,  A, 17, 19,  1, 14, 13, 12, 12, 10,     E,  958, -910,   O, 1.7,-2.9,-100.7,    S, 600, 194, 469,4000,  I, 752, 577, 997,  M,100,-100,  56,-55, 170,-170,   W,  0,  0,  1,  0 )
D_ ( 255,   1,  10254,  T, 26,20.1,   V,9.6,  0x0000,  A, 19, 18,  2, 15, 14, 12, 13, 11,     E, 1022, -971,   O, 1.6,-3.0,-107.0,    S, 600, 194, 469,4000,  I, 608, 571, 997,  M,100,-100,  64,-61, 170,-170,   W,  0,  0,  1,  0 )
D_ (   0,   1,  10274,  T, 34,20.8,   V,9.6,  0x0000,  A,  0, 20,  1,  0, 14, 14, 14, 13,     E, 1091,-1036,   O, 1.6,-3.2,-114.2,    S, 535, 194, 469,4000,  I, 498, 713, 997,  M,100,-100,  69,-65, 170,-170,   W,  0,  0,  1,  0 )
D_ (   1,   1,  10294,  T,  6,20.0,   V,9.6,  0x0000,  A,  0, 20,  6,  0, 14, 15, 13, 11,     E, 1166,-1106,   O, 1.5,-3.5,-121.9,    S, 535, 588, 469,4000,  I, 425, 808, 997,  M,100,-100,  75,-70, 170,-170,   W,  0,  0,  1,  0 )
D_ (   2,   1,  10314,  T, 24,20.2,   V,9.5,  0x0000,  A, 12, 11,  6,  6, 13, 11, 11, 10,     E, 1243,-1177,   O, 1.2,-3.8,-130.2,    S, 535, 588, 503,4000,  I, 616, 782, 997,  M,100,-100,  77,-71, 174,-174,   W,  0,  0,  1,  0 )
D_ (   3,   1,  10335,  T, 20,20.2,   V,9.5,  0x0000,  A, 12, 12, 11,  5, 12, 13, 12, 10,     E, 1322,-1250,   O, 0.9,-4.2,-138.7,    S, 535, 588, 503,4000,  I, 743, 764, 997,  M,100,-100,  79,-73, 174,-174,   W,  0,  0,  1,  0 )
D_ (   4,   1,  10354,  T, 16,20.0,   V,9.5,  0x0000,  A, 14,  3, 11, 16, 18, 15, 17, 14,     E, 1405,-1326,   O, 0.6,-4.5,-147.4,    S, 535, 588, 503,4000,  I, 828, 735, 997,  M,100,-100,  83,-76, 174,-174,   W,  0,  0,  1,  0 )
D_ (   5,   1,  10374,  T, 19,20.0,   V,9.5,  0x0000,  A, 13,  0, 12, 18, 18, 16, 16, 14,     E, 1490,-1403,   O, 0.1,-4.8,-156.5,    S, 454, 588, 503,4000,  I, 885, 724, 997,  M,100,-100,  85,-77, 174,-174,   W,  0,  0,  1,  0 )
D_ (   6,   1,  10394,  T, 22,20.1,   V,9.5,  0x0000,  A,  7,  1, 12, 39, 26, 23, 24, 21,     E, 1578,-1481,   O,-0.4,-5.0,-165.8,    S, 454, 385, 503,4000,  I, 923, 815, 997,  M,100,-100,  88,-78, 174,-174,   W,  0,  0,  1,  0 )
D_ (   7,   1,  10414,  T, 18,20.0,   V,9.4,  0x0000,  A,  9,  0, 11, 38, 25, 24, 24, 21,     E, 1665,-1559,   O,-1.2,-5.2,-175.3,    S, 454, 385, 506,4000,  I, 948, 876, 997,  M,100,-100,  87,-78, 176,-176,   W,  0,  0,  1,  0 )
D_ (   8,   1,  10434,  T, 22,20.1,   V,9.4,  0x0000,  A, 16,  8,  9,  0, 17, 14, 14, 13,     E, 1753,-1638,   O,-1.9,-5.3,175.3,    S, 454, 385, 506,4000,  I, 965, 792, 997,  M,100,-100,  88,-79, 176,-176,   W,  0,  0,  1,  0 )
D_ (   9,   1,  10456,  T, 21,20.1,   V,9.4,  0x0000,  A, 17,  9, 31,  1, 16, 15, 15, 13,     E, 1843,-1717,   O,-2.5,-5.2,165.7,    S, 454, 385, 506,4000,  I, 976, 728, 180,  M,100,-100,  90,-79, 176,-176,   W,  0,  0,  1,  0 )
D_ (  10,   1,  10474,  T, 30,20.6,   V,9.4,  0x0000,  A, 16,  8, 30,  0, 29, 28, 28, 25,     E, 1934,-1797,   O,-3.4,-5.0,156.0,    S, 516, 385, 506,4000,  I, 983, 694, 186,  M,100,-100,  91,-80, 176,-176,   W,  0,  0,  1,  0 )
D_ (  11,   1,  10494,  T,  7,19.9,   V,9.4,  0x0000,  A, 20,  7, 26,  1, 30, 28, 28, 25,     E, 2026,-1877,   O,-4.1,-4.7,146.2,    S, 516, 508, 506,4000,  I, 988, 642, 213,  M,100,-100,  92,-80, 176,-176,   W,  0,  0,  1,  0 )
D_ (  12,   1,  10514,  T, 19,19.9,   V,9.3,  0x0000,  A, 20, 12, 25, 11, 17, 15, 16, 14,     E, 2119,-1958,   O,-4.9,-4.1,136.4,    S, 516, 508, 217,4000,  I, 991, 608, 220,  M,100,-100,  93,-81, 176,-176,   W,  0,  0,  1,  0 )
D_ (  13,   1,  10533,  T, 10,19.4,   V,9.3,  0x0000,  A, 15, 12, 16, 11, 16, 15, 15, 13,     E, 2211,-2038,   O,-5.6,-3.5,126.4,    S, 516, 508, 217,4000,  I, 993, 622, 479,  M,100,-100,  92,-80, 176,-176,   W,  0,  0,  1,  0 )
D_ (  14,   1,  10554,  T, 30,19.9,   V,9.3,  0x0000,  A, 15, 13, 16, 10, 16, 15, 15, 13,     E, 2305,-2119,   O,-6.1,-2.8,116.6,    S, 516, 508, 217,4000,  I, 995, 632, 652,  M,100,-100,  94,-81, 176,-176,   W,  0,  0,  1,  0 )
D_ (  15,   1,  10576,  T, 23,20.1,   V,9.3,  0x0000,  A, 62, 11, 13,  9, 15, 14, 15, 13,     E, 2399,-2201,   O,-6.6,-1.9,106.5,    S, 249, 508, 217,4000,  I, 996, 183, 767,  M,100,-100,  94,-82, 176,-176,   W,  0,  0,  1,  0 )
D_ (  16,   1,  10594,  T, 19,20.0,   V,9.3,  0x0000,  A, 61, 13, 13,  8, 15, 14, 14, 13,     E, 2493,-2282,   O,-6.8,-1.0,96.4,    S, 249, 208, 217,4000,  I, 997, 187, 844,  M,100,-100,  94,-81, 176,-176,   W,  0,  0,  1,  0 )
D_ (  17,   1,  10614,  T, 22,20.1,   V,9.3,  0x0000,  A, 58, 12, 13,  8, 16, 16, 15, 13,     E, 2588,-2365,   O,-6.9, 0.0,86.4,    S, 249, 208, 389,4000,  I, 997, 197, 895,  M,100,-100,  95,-83, 178,-178,   W,  0,  0,  1,  0 )
D_ (  18,   1,  10634,  T, 13,19.8,   V,9.3,  0x0000,  A, 59, 13, 12, 15, 16, 14, 15, 13,     E, 2681,-2446,   O,-6.9, 0.9,76.2,    S, 249, 208, 389,4000,  I, 997, 194, 929,  M,100,-100,  93,-81, 178,-178,   W,  0,  0,  1,  0 )
D_ (  19,   1,  10654,  T, 25,20.0,   V,9.3,  0x0000,  A, 45, 28,  5, 14, 15, 15, 15, 13,     E, 2775,-2528,   O,-6.7, 1.8,66.2,    S, 249, 208, 389,4000,  I, 200, 247, 952,  M,100,-100,  94,-82, 178,-178,   W,  0,  0,  1,  0 )
D_ (  20,   1,  10673,  T, 33,20.7,   V,9.3,  0x0000,  A, 45, 27,  4, 15,  0,  1,  2,  2,     E, 2869,-2611,   O,-6.3, 2.7,56.2,    S, 204, 208, 389,4000,  I, 206, 247, 967,  M,100,-100,  94,-83, 178,-178,   W,  0,  0,  1,  0 )
D_ (  21,   1,  10696,  T, 38,21.5,   V,9.3,  0x0000,  A, 51, 29,  4, 17,  0,  0,  0,  0,     E, 2964,-2694,   O,-5.8, 3.4,46.0,    S, 204, 272, 389,4000,  I, 193, 222, 977,  M, 40,-40,  95,-83,  76,-76,   W,  0,  0,  1,  0 )
D_ (  22,   1,  10714,  T,  3,20.6,   V,9.4,  0x0000,  A, 51, 29, 17, 15, 19, 16, 17, 15,     E, 3047,-2768,   O,-5.2, 4.0,35.8,    S, 204, 272, 385,4000,  I, 193, 222, 764,  M, 40,-40,  83,-74,  75,-75,   W,  0,  0,  1,  0 )
D_ (  23,   1,  10733,  T,  3,19.7,   V,9.4,  0x0000,  A, 26, 18, 19, 16, 18, 17, 18, 15,     E, 3116,-2826,   O,-4.6, 4.5,26.8,    S, 204, 272, 385,4000,  I, 235, 433, 609,  M, 40,-40,  69,-58,  75,-75,   W,  0,  0,  1,  0 )
D_ (  24,   1,  10754,  T, 25,20.0,   V,9.4,  0x0000,  A, 25, 18,  0, 27, 21, 19, 19, 17,     E, 3179,-2879,   O,-3.9, 4.8,19.5,    S, 204, 272, 385,4000,  I, 263, 450, 739,  M, 40,-40,  63,-53,  75,-75,   W,  0,  0,  1,  0 )
D_ (  25,   1,  10774,  T, 15,19.7,   V,9.4,  0x0000,  A, 28, 30,  1, 27, 21, 19, 19, 17,     E, 3237,-2928,   O,-3.1, 5.1,12.9,    S, 365, 272, 385,4000,  I, 186, 400, 825,  M, 40,-40,  58,-49,  75,-75,   W,  0,  0,  1,  0 )
D_ (  26,   1,  10793,  T, 23,19.9,   V,9.4,  0x0000,  A, 27, 28, 13, 34, 15, 13, 14, 12,     E, 3290,-2974,   O,-2.5, 5.2, 6.8,    S, 365, 364, 385,4000,  I, 200, 416, 883,  M, 40,-40,  53,-46,  75,-75,   W,  0,  0,  1,  0 )
D_ (  27,   1,  10816,  T, 24,20.1,   V,9.4,  0x0000,  A, 34, 12, 13, 33, 14, 12, 14, 12,     E, 3340,-3018,   O,-1.9, 5.3, 1.1,    S, 365, 364, 441,4000,  I, 466, 340, 921,  M, 40,-40,  50,-44,  74,-74,   W,  0,  0,  1,  0 )
D_ (  28,   1,  10834,  T, 16,19.9,   V,9.4,  0x0000,  A, 34, 12, 13, 23,  0,  1,  2,  2,     E, 3385,-3058,   O,-1.5, 5.3,-4.3,    S, 365, 364, 441,4000,  I, 643, 340, 947,  M, 40,-40,  45,-40,  74,-74,   W,  0,  0,  1,  0 )
D_ (  29,   1,  10853,  T, 16,19.7,   V,9.4,  0x0000,  A, 28, 10, 12, 22,  1,  1,  1,  1,     E, 3429,-3096,   O,-1.1, 5.3,-9.2,    S, 365, 364, 441,4000,  I, 761, 400, 964,  M, 40,-40,  44,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  30,   1,  10874,  T, 40,20.7,   V,9.4,  0x0000,  A, 28,  9,  6,  6,  0,  0,  0,  0,     E, 3472,-3135,   O,-0.6, 5.2,-13.9,    S, 427, 364, 441,4000,  I, 840, 400, 975,  M, 40,-40,  43,-39,  74,-74,   W,  0,  0,  1,  0 )
D_ (  31,   1,  10894,  T,  4,19.9,   V,9.4,  0x0000,  A, 27, 18,  4,  6,  0,  0,  2,  1,     E, 3514,-3174,   O,-0.3, 5.1,-18.6,    S, 427, 360, 441,4000,  I, 666, 416, 983,  M, 40,-40,  42,-39,  74,-74,   W,  0,  0,  1,  0 )
D_ (  32,   1,  10914,  T, 13,19.5,   V,9.5,  0x0000,  A, 26, 18, 32, 31,  0,  0,  1,  1,     E, 3555,-3213,   O,-0.1, 5.1,-23.2,    S, 427, 360, 442,4000,  I, 550, 433, 175,  M, 40,-40,  41,-39,  74,-74,   W,  0,  0,  1,  0 )
D_ (  33,   1,  10936,  T, 28,20.0,   V,9.5,  0x0000,  A,  8, 15, 32, 31,  0,  1,  0,  0,     E, 3596,-3250,   O, 0.0, 5.0,-27.8,    S, 427, 360, 442,4000,  I, 699, 621, 175,  M, 40,-40,  41,-37,  74,-74,   W,  0,  0,  1,  0 )
D_ (  34,   1,  10954,  T, 21,20.0,   V,9.5,  0x0000,  A,  7, 14, 37, 26,  1,  0,  0,  0,     E, 3637,-3288,   O, 0.3, 4.9,-32.2,    S, 427, 360, 442,4000,  I, 799, 747, 150,  M, 40,-40,  41,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  35,   1,  10974,  T, 16,19.8,   V,9.5,  0x0000,  A,  0, 11, 37, 24,  2,  1,  2,  2,     E, 3678,-3326,   O, 0.5, 4.7,-36.8,    S, 434, 360, 442,4000,  I, 865, 831, 150,  M, 40,-40,  41,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  36,   1,  10994,  T, 24,20.0,   V,9.5,  0x0000,  A,  1, 10,  0, 14,  0,  0,  1,  1,     E, 3719,-3364,   O, 0.7, 4.6,-41.3,    S, 434, 407, 442,4000,  I, 909, 887, 433,  M, 40,-40,  41,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  37,   1,  11014,  T, 22,20.1,   V,9.5,  0x0000,  A, 24, 20,  1, 15, 11,  8,  9,  8,     E, 3760,-3402,   O, 0.9, 4.4,-45.8,    S, 434, 407, 202,4000,  I, 699, 466, 621,  M, 40,-40,  41,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  38,   1,  11034,  T, 16,19.9,   V,9.5,  0x0000,  A, 26, 19,  0, 14, 10,  9, 10,  9,     E, 3800,-3439,   O, 1.0, 4.3,-50.4,    S, 434, 407, 202,4000,  I, 566, 433, 747,  M, 40,-40,  40,-37,  74,-74,   W,  0,  0,  1,  0 )
D_ (  39,   1,  11055,  T, 22,20.0,   V,9.5,  0x0000,  A, 24, 20,  2, 14, 11, 10, 10,  9,     E, 3841,-3477,   O, 1.2, 4.1,-54.8,    S, 434, 407, 202,4000,  I, 470, 466, 831,  M, 40,-40,  41,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  40,   1,  11074,  T, 35,20.8,   V,9.5,  0x0000,  A, 23, 22,  0, 22, 12, 12, 10,  9,     E, 3882,-3515,   O, 1.3, 3.9,-59.3,    S, 449, 407, 202,4000,  I, 396, 471, 887,  M, 40,-40,  41,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  41,   1,  11094,  T,  5,20.0,   V,9.5,  0x0000,  A, 17, 19,  0, 22, 10,  9,  9,  8,     E, 3923,-3553,   O, 1.4, 3.7,-63.8,    S, 449, 436, 202,4000,  I, 364, 514, 924,  M, 40,-40,  41,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  42,   1,  11114,  T, 20,20.0,   V,9.5,  0x0000,  A, 15, 16,  0, 16, 13,  9,  9,  8,     E, 3964,-3591,   O, 1.5, 3.5,-68.3,    S, 449, 436, 604,4000,  I, 575, 560, 949,  M, 40,-40,  41,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  43,   1,  11134,  T, 17,19.8,   V,9.5,  0x0000,  A, 35, 16,  0, 16,  3,  2,  3,  3,     E, 4004,-3629,   O, 1.6, 3.3,-72.9,    S, 449, 436, 604,4000,  I, 716, 330, 965,  M, 40,-40,  40,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  44,   1,  11154,  T, 22,19.9,   V,9.5,  0x0000,  A, 35,  0,  0, 10,  3,  2,  2,  2,     E, 4045,-3667,   O, 1.7, 3.1,-77.3,    S, 449, 436, 604,4000,  I, 810, 330, 976,  M, 40,-40,  41,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  45,   1,  11176,  T, 10,19.4,   V,9.5,  0x0000,  A, 70,  1,  7, 10,  5,  4,  4,  3,     E, 4085,-3705,   O, 1.7, 2.9,-81.9,    S, 200, 436, 604,4000,  I, 873, 167, 983,  M, 40,-40,  40,-38,  74,-74,   W,  0,  0,  1,  0 )
D_ (  46,   1,  11194,  T, 29,19.9,   V,9.5,  0x0000,  A, 71,  1,  7, 14,  4,  4,  4,  3,     E, 4125,-3743,   O, 1.7, 2.8,-86.3,    S, 200, 205, 604,4000,  I, 915, 165, 988,  M,100,-100,  40,-38, 172,-172,   W,  0,  0,  1,  0 )
D_ (  47,   1,  11214,  T, 19,19.9,   V,9.3,  0x0000,  A, 64,  1, 10, 13, 10,  9, 10,  8,     E, 4175,-3790,   O, 1.7, 2.6,-90.8,    S, 200, 205, 482,4000,  I, 943, 178, 991,  M,100,-100,  50,-47, 176,-176,   W,  0,  0,  1,  0 )
D_ (  48,   1,  11233,  T, 16,19.7,   V,9.3,  0x0000,  A, 63, 17, 10, 22,  9,  8,  9,  8,     E, 4234,-3846,   O, 1.7, 2.4,-96.4,    S, 200, 205, 482,4000,  I, 742, 180, 993,  M,100,-100,  59,-56, 176,-176,   W,  0,  0,  1,  0 )
D_ (  49,   1,  11254,  T, 25,20.0,   V,9.3,  0x0000,  A,  0, 16,  6, 22, 13, 14, 12, 10,     E, 4300,-3909,   O, 1.7, 2.2,-102.9,    S, 200, 205, 482,4000,  I, 827, 453, 995,  M,100,-100,  66,-63, 176,-176,   W,  0,  0,  1,  0 )
D_ (  50,   1,  11274,  T, 34,20.7,   V,9.3,  0x0000,  A,  0, 17,  7, 15, 13, 11, 11, 10,     E, 4373,-3977,   O, 1.7, 1.9,-110.3,    S, 503, 205, 482,4000,  I, 664, 635, 996,  M,100,-100,  73,-68, 176,-176,   W,  0,  0,  1,  0 )
D_ (  51,   1,  11296,  T,  8,20.0,   V,9.3,  0x0000,  A, 10, 18,  3, 14, 15, 12, 13, 11,     E, 4449,-4049,   O, 1.5, 1.6,-118.4,    S, 503, 601, 482,4000,  I, 549, 756, 997,  M,100,-100,  76,-72, 176,-176,   W,  0,  0,  1,  0 )
D_ (  52,   1,  11314,  T, 13,19.7,   V,9.2,  0x0000,  A, 10, 24,  2,  0, 14, 14, 13, 12,     E, 4530,-4124,   O, 1.4, 1.3,-126.9,    S, 503, 601, 349,4000,  I, 230, 837, 997,  M,100,-100,  81,-75, 180,-180,   W,  0,  0,  1,  0 )
D_ (  53,   1,  11334,  T, 27,20.0,   V,9.2,  0x0000,  A,  0, 24,  6,  1, 14, 14, 13, 12,     E, 4612,-4200,   O, 1.1, 0.9,-135.8,    S, 503, 601, 349,4000,  I, 230, 891, 997,  M,100,-100,  82,-76, 180,-180,   W,  0,  0,  1,  0 )
D_ (  54,   1,  11354,  T, 24,20.2,   V,9.2,  0x0000,  A,  2,  0,  6,  1, 14, 13, 14, 12,     E, 4698,-4278,   O, 0.8, 0.6,-144.9,    S, 503, 601, 349,4000,  I, 486, 927, 997,  M,100,-100,  86,-78, 180,-180,   W,  0,  0,  1,  0 )
D_ (  55,   1,  11374,  T, 19,20.2,   V,9.2,  0x0000,  A, 13,  1, 12, 18, 15, 13, 13, 12,     E, 4787,-4358,   O, 0.3, 0.3,-154.3,    S, 360, 601, 349,4000,  I, 657, 852, 997,  M,100,-100,  89,-80, 180,-180,   W,  0,  0,  1,  0 )
D_ (  56,   1,  11394,  T, 14,19.9,   V,9.2,  0x0000,  A, 14,  1, 10, 18, 16, 14, 13, 12,     E, 4878,-4439,   O,-0.4,-0.0,-164.0,    S, 360, 361, 349,4000,  I, 771, 794, 997,  M,100,-100,  91,-81, 180,-180,   W,  0,  0,  1,  0 )
D_ (  57,   1,  11416,  T, 23,20.0,   V,9.1,  0x0000,  A, 14,  0, 12, 23, 23, 21, 21, 19,     E, 4969,-4521,   O,-1.1,-0.3,-173.8,    S, 360, 361, 503,4000,  I, 847, 755, 997,  M,100,-100,  91,-82, 180,-180,   W,  0,  0,  1,  0 )
D_ (  58,   1,  11434,  T, 17,19.9,   V,9.1,  0x0000,  A, 13,  0, 11, 22, 25, 24, 21, 19,     E, 5061,-4602,   O,-1.8,-0.3,176.3,    S, 360, 361, 503,4000,  I, 897, 738, 997,  M,100,-100,  92,-81, 180,-180,   W,  0,  0,  1,  0 )
D_ (  59,   1,  11454,  T, 17,19.7,   V,9.1,  0x0000,  A, 10,  0, 23,  0, 18, 17, 20, 18,     E, 5155,-4684,   O,-2.6,-0.3,166.4,    S, 360, 361, 503,4000,  I, 931, 825, 240,  M,100,-100,  94,-82, 180,-180,   W,  0,  0,  1,  0 )
D_ (  60,   1,  11474,  T, 40,20.7,   V,9.1,  0x0000,  A,  9, 11, 24,  0, 20, 18, 17, 16,     E, 5250,-4767,   O,-3.5,-0.1,156.3,    S, 509, 361, 503,4000,  I, 953, 883, 230,  M,100,-100,  95,-83, 180,-180,   W,  0,  0,  1,  0 )
D_ (  61,   1,  11494,  T,  6,20.0,   V,9.1,  0x0000,  A, 21, 10, 28,  0, 29, 26, 27, 24,     E, 5344,-4850,   O,-4.4, 0.3,146.1,    S, 509, 509, 503,4000,  I, 968, 762, 200,  M,100,-100,  94,-83, 180,-180,   W,  0,  0,  1,  0 )
D_ (  62,   1,  11514,  T, 20,20.0,   V,9.1,  0x0000,  A, 21,  8, 28,  0, 28, 26, 26, 23,     E, 5441,-4934,   O,-5.1, 0.8,135.9,    S, 509, 509, 208,4000,  I, 978, 681, 200,  M,100,-100,  97,-84, 182,-182,   W,  0,  0,  1,  0 )
D_ (  63,   1,  11536,  T, 21,20.1,   V,9.1,  0x0000,  A, 18,  8, 17,  5, 16, 14, 16, 14,     E, 5537,-5017,   O,-5.8, 1.5,125.6,    S, 509, 509, 208,4000,  I, 985, 647, 246,  M,100,-100,  96,-83, 182,-182,   W,  0,  0,  1,  0 )
D_ (  64,   1,  11554,  T, 23,20.2,   V,9.1,  0x0000,  A, 18,  9, 17,  4, 16, 14, 15, 13,     E, 5634,-5100,   O,-6.4, 2.3,115.3,    S, 509, 509, 208,4000,  I, 989, 624, 277,  M,100,-100,  97,-83, 182,-182,   W,  0,  0,  1,  0 )
D_ (  65,   1,  11574,  T, 13,19.8,   V,9.1,  0x0000,  A, 56, 12, 26, 12, 18, 15, 16, 14,     E, 5732,-5184,   O,-6.8, 3.3,105.0,    S, 211, 509, 208,4000,  I, 992, 204, 213,  M,100,-100,  98,-84, 182,-182,   W,  0,  0,  1,  0 )
D_ (  66,   1,  11594,  T, 27,20.2,   V,9.1,  0x0000,  A, 56, 13, 26, 11, 17, 15, 15, 14,     E, 5830,-5268,   O,-7.1, 4.3,94.6,    S, 211, 239, 208,4000,  I, 994, 204, 213,  M,100,-100,  98,-84, 182,-182,   W,  0,  0,  1,  0 )
D_ (  67,   1,  11614,  T, 19,20.1,   V,9.1,  0x0000,  A, 60, 11, 26, 11, 15, 14, 16, 14,     E, 5928,-5353,   O,-7.2, 5.4,84.1,    S, 211, 239, 401,4000,  I, 995, 190, 213,  M,100,-100,  98,-85, 182,-182,   W,  0,  0,  1,  0 )
D_ (  68,   1,  11633,  T, 13,19.8,   V,9.1,  0x0000,  A, 59, 12, 10, 11, 15, 14, 15, 13,     E, 6025,-5437,   O,-7.1, 6.4,73.6,    S, 211, 239, 401,4000,  I, 996, 194, 475,  M,100,-100,  97,-84, 182,-182,   W,  0,  0,  1,  0 )
D_ (  69,   1,  11655,  T, 22,19.9,   V,9.1,  0x0000,  A, 42, 16, 10, 16, 11,  9,  9,  9,     E, 6123,-5522,   O,-6.8, 7.3,63.3,    S, 211, 239, 401,4000,  I, 997, 270, 649,  M,100,-100,  98,-85, 182,-182,   W,  0,  0,  1,  0 )
D_ (  70,   1,  11674,  T, 34,20.6,   V,9.1,  0x0000,  A, 44, 16, 11, 12, 10,  8, 10,  9,     E, 6220,-5608,   O,-6.4, 8.2,52.8,    S, 211, 239, 401,4000,  I, 997, 255, 765,  M,100,-100,  97,-86, 182,-182,   W,  0,  0,  1,  0 )
D_ (  71,   1,  11693,  T,  5,19.8,   V,9.1,  0x0000,  A, 62, 39,  8, 15,  0,  0,  1,  1,     E, 6317,-5693,   O,-5.9, 8.9,42.3,    S, 211, 240, 401,4000,  I, 140, 183, 843,  M,  0,  0,  97,-85,   0,  0,   W,  0,  0,  1,  0 )
D_ (  72,   1,  11714,  T, 23,20.0,   V,9.1,  0x0000,  A, 63, 37, 13, 15,  2,  0,  0,  0,     E, 6395,-5758,   O,-5.2, 9.5,31.9,    S, 211, 240, 358,4000,  I, 150, 180, 895,  M,  0,  0,  78,-65,   0,  0,   W,  0,  0,  1,  0 )
D_ (  73,   1,  11734,  T, 18,19.9,   V,9.1,  0x0000,  A, 23, 25, 12, 17, 20, 16, 17, 14,     E, 6440,-5791,   O,-4.3,10.1,23.7,    S, 211, 240, 358,4000,  I, 220, 281, 929,  M,  0,  0,  45,-33,   0,  0,   W,  0,  0,  1,  0 )
D_ (  74,   1,  11753,  T, 22,20.0,   V,9.1,  0x0000,  A, 23, 26,  0, 17, 21, 17, 17, 14,     E, 6467,-5813,   O,-3.5,10.4,19.2,    S, 211, 240, 358,4000,  I, 213, 348, 952,  M,  0,  0,  27,-22,   0,  0,   W,  0,  0,  1,  0 )
D_ (  75,   1,  11776,  T, 21,20.0,   V,9.1,  0x0000,  A, 32, 26,  0, 24, 13, 12, 11, 10,     E, 6483,-5824,   O,-3.1,10.6,16.4,    S, 387, 240, 358,4000,  I, 213, 360, 967,  M,  0,  0,  16,-11,   0,  0,   W,  0,  0,  1,  0 )
D_ (  76,   1,  11794,  T, 23,20.2,   V,9.1,  0x0000,  A, 32, 25,  1, 24, 11, 11, 10,  9,     E, 6491,-5829,   O,-2.8,10.7,14.9,    S, 387, 192, 358,4000,  I, 220, 360, 977,  M,  0,  0,   8, -5,   0,  0,   W,  0,  0,  1,  0 )
D_ (  77,   1,  11814,  T,  6,19.5,   V,9.2,  0x0000,  A, 30, 34,  1, 36, 22, 20, 20, 17,     E, 6495,-5829,   O,-2.5,10.7,14.1,    S, 387, 192, 354,4000,  I, 165, 380, 984,  M,  0,  0,   4,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  78,   1,  11833,  T, 33,20.2,   V,9.2,  0x0000,  A, 32, 31,  2, 35, 23, 20, 20, 18,     E, 6496,-5829,   O,-2.2,10.8,13.9,    S, 387, 192, 354,4000,  I, 180, 360, 989,  M,  0,  0,   1,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  79,   1,  11853,  T, 47,21.5,   V,9.2,  0x0000,  A, 30, 31,  1, 35, 18, 17, 18, 16,     E, 6496,-5828,   O,-2.2,10.8,13.8,    S, 387, 192, 354,4000,  I, 180, 380, 992,  M,  0,  0,   0,  1,   0,  0,   W,  0,  0,  1,  0 )
D_ (  80,   1,  11873,  T,  4,20.6,   V,9.2,  0x0000,  A, 31, 31,  1, 35, 18, 17, 18, 16,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 386, 192, 354,4000,  I, 180, 370, 994,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  81,   1,  11896,  T,  7,19.9,   V,9.2,  0x0000,  A, 30, 31,  0, 33, 15, 13, 13, 11,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 386, 195, 354,4000,  I, 180, 380, 995,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  82,   1,  11914,  T, 26,20.2,   V,9.3,  0x0000,  A, 30, 32,  0, 32, 15, 13, 13, 12,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 386, 195, 361,4000,  I, 175, 380, 996,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  83,   1,  11933,  T,  8,19.6,   V,9.3,  0x0000,  A, 30, 31,  1, 32, 14, 13, 15, 13,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 386, 195, 361,4000,  I, 180, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  84,   1,  11953,  T, 23,19.8,   V,9.3,  0x0000,  A, 30, 31,  0, 32, 17, 15, 17, 15,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 386, 195, 361,4000,  I, 180, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  85,   1,  11973,  T, 50,21.3,   V,9.3,  0x0000,  A, 30, 31,  0, 35, 17, 15, 16, 14,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 195, 361,4000,  I, 180, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  86,   1,  11993,  T,  3,20.4,   V,9.3,  0x0000,  A, 32, 30,  1, 34, 16, 15, 12, 11,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 226, 361,4000,  I, 186, 360, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  87,   1,  12016,  T, 15,20.1,   V,9.4,  0x0000,  A, 31, 32,  1, 33, 15, 13, 15, 13,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 226, 356,4000,  I, 175, 370, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  88,   1,  12034,  T, 14,19.8,   V,9.4,  0x0000,  A, 31, 30,  1, 34, 17, 16, 14, 13,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 226, 356,4000,  I, 186, 370, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  89,   1,  12053,  T, 20,19.8,   V,9.4,  0x0000,  A, 30, 30,  0, 34, 17, 15, 16, 14,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 226, 356,4000,  I, 186, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  90,   1,  12073,  T, 33,20.5,   V,9.4,  0x0000,  A, 30, 30,  0, 34, 16, 15, 16, 14,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 388, 226, 356,4000,  I, 186, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  91,   1,  12093,  T,  6,19.8,   V,9.4,  0x0000,  A, 30, 30,  0, 34, 16, 15, 16, 14,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 388, 200, 356,4000,  I, 186, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  92,   1,  12114,  T, 20,19.8,   V,9.5,  0x0000,  A, 30, 31,  2, 33, 17, 15, 15, 14,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 388, 200, 354,4000,  I, 180, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  93,   1,  12135,  T, 18,19.7,   V,9.5,  0x0000,  A, 31, 32,  0, 33, 17, 15, 16, 15,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 388, 200, 354,4000,  I, 175, 370, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  94,   1,  12154,  T, 30,20.2,   V,9.5,  0x0000,  A, 32, 31,  2, 33, 14, 12, 13, 12,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 388, 200, 354,4000,  I, 180, 360, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  95,   1,  12173,  T, 11,19.7,   V,9.5,  0x0000,  A, 31, 32,  0, 33, 15, 12, 13, 12,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 200, 354,4000,  I, 175, 370, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  96,   1,  12193,  T, 25,20.0,   V,9.5,  0x0000,  A, 30, 31,  1, 34, 16, 15, 16, 14,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 196, 354,4000,  I, 180, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  97,   1,  12214,  T, 19,20.0,   V,9.6,  0x0000,  A, 30, 30,  2, 36, 16, 14, 15, 13,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 196, 357,4000,  I, 186, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  98,   1,  12233,  T, 18,19.9,   V,9.6,  0x0000,  A, 30, 32,  0, 35, 17, 15, 15, 13,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 196, 357,4000,  I, 175, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ (  99,   1,  12255,  T, 23,20.0,   V,9.6,  0x0000,  A, 31, 31,  1, 33, 16, 14, 16, 14,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 399, 196, 357,4000,  I, 180, 370, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
D_ ( 100,   1,  12274,  T, 41,21.1,   V,9.6,  0x0000,  A, 30, 31,  0, 32, 18, 16, 16, 14,     E, 6496,-5828,   O,-2.1,10.8,13.9,    S, 390, 196, 357,4000,  I, 180, 380, 997,  M,  0,  0,   0,  0,   0,  0,   W,  0,  0,  1,  0 )
*/



void test_fsm(u08 cmd, u08 *param)
{
	//enum states { s_disabled=0, s_tracking_wall=1, s_lost_wall=2, s_turning_corner=3, s_turning_sharp_corner=4 };
	static u08 state=0;
	static u08 last_state=255;
	static u32 t_start;
	static s16 bias=0;
	static u08 count;
	u32 t_delta;
	u08 i;
	t_config_value v;
	
	static u08 initialized=0;
	

	if(!initialized)
	{
		initialized=1;
		usb_printf("wall_follow_fsm()\n");
	}

	if(s.behavior_state[TEST_LOGIC_FSM]==1) 
	{
		PUMP_ON();
		s.behavior_state[TEST_LOGIC_FSM]=0;
	}

	if(s.behavior_state[TEST_LOGIC_FSM]==2) 
	{
		PUMP_OFF();
		s.behavior_state[TEST_LOGIC_FSM]=0;
	}

	if(s.behavior_state[TEST_LOGIC_FSM]==3) 
	{
		dbg_printf("start = %d\n",is_digital_input_high(IO_B3));
		s.behavior_state[TEST_LOGIC_FSM]=0;
	}

	if(s.behavior_state[TEST_LOGIC_FSM]==4) 
	{
		switch(state)
		{
		case 0:
			t_start = get_ms();
			motor_command(8,0,0,speed_profile[0].l_speed,speed_profile[0].r_speed);
			state++;
			break;
		case 1:
			t_delta = get_ms() - t_start;
			i=0;
			while(speed_profile[i].time < t_delta) i++;
			motor_command(8,0,0,speed_profile[i-1].l_speed,speed_profile[i-1].r_speed);
			break;
		}
	}

	if(s.behavior_state[TEST_LOGIC_FSM]==5) 
	{
		static s16 ne=0, nw=0;

		ne = (ne + (s16) s.inputs.analog[AI_FLAME_NE])/2;
		nw = (nw + (s16) s.inputs.analog[AI_FLAME_NW])/2;
		bias = 0;
		if( (ne>245) && (nw>245) ) bias = 0;
		else if( abs(ne-nw) < 10 ) bias = 0;
		else if( ne>nw ) bias = -1;
		else if( nw>ne ) bias =  1;
		v.u16 = cfg_get_u16_by_grp_id(15,6);
		v.u16 += bias;
		cfg_set_value_by_grp_id(15,6, v);
	}

	if(s.behavior_state[TEST_LOGIC_FSM]==6) 
	{
		if(s.inputs.analog[AI_FLAME_NW]<80)
		{
			motor_command(6,3,3,-40,40);
		}
		else
		{
			motor_command(2,0,0,0,0);
			s.behavior_state[TEST_LOGIC_FSM]=0;
		}
	}
}


void task_run(taskproctype taskproc, u08 cmd, u08 *param)
{
	u08 currently_running_tid;

	currently_running_tid  = running_tid;	
	running_tid = task_id_get(taskproc);
	if(os_task_is_ready(running_tid)) 
	{
		taskproc(cmd,param);
	}
	running_tid = currently_running_tid;
}


void main_loop(void)
{

	#ifdef SVP_ON_WIN32
	sim_task(0,0); //needs to run first, because it initializes some things.
	#endif
	//incoming communication, i.e. commands
	serial_receive_fsm(0,0); //includes processing of commands

	//sample and process low-level inputs - most of this is only needed on the actual robot
	#ifndef SVP_ON_WIN32	
	ultrasonic_update_fsm(0,0);
	analog_update_fsm(0,0);  //we don't want this when running on the PC and/or in simulation mode
	SHARPIR_update_fsm(0,0);
	#else	
	{ extern void sim_inputs(void); sim_inputs(); } //on the PC, IR, Sonar and line sensor readings are provided by V-REP
	#endif
	odometry_update_fsm(0,0);
	line_detection_fsm_v2(0,0);


	//behaviors
	task_run(master_logic_fsm,0,0); //ml fsm makes sleep statements, so it is a task and not just a simply fsm

	task_run(find_flame_fsm,0,0);
	wall_follow_fsm(0,0);
	line_alignment_fsm_v2(0,0);

	//outputs
	motor_command(0,0,0,0,0);
	servo_task(0,0);

	//testing
	test_fsm(0,0);

	//ui
	task_run(lcd_update_fsm,0,0);	//lcd is incompatible with servos; also, not yet converted from task back to pure fsm	

	//outgoing communication
	serial_send_fsm(0,0);
	#ifdef SVP_ON_WIN32
	{ extern void sim_outputs(void); sim_outputs(); }
	{ extern void sim_step(void);	 sim_step();    }
	#endif	
}




void main_loop_task(u08 cmd, u08 *param)
{
	static u32 t_now, t_last, t_delta;
	task_open_1();
	//code between _1() and _2() will get executed every time the scheduler resumes this task

	task_open_2();
	//execution below this point will resume wherever it left off when a context switch happens

	usb_printf("main_loop_task()\n");


	while(1)
	{
		t_now = get_ms();
		t_delta = t_now-t_last;
		t_last = t_now;
		#ifdef WIN32
		//usb_printf("t_delta = %ld, m.elapsed_milliseconds = %ld, GetTickCount=%ld\n",t_delta,m.elapsed_milliseconds,timeGetTime());
		#endif
		main_loop();
		#ifndef SVP_ON_WIN32
		//TODO: don't hard-code the execution rate time delay - take loop time into account, i.e make it 20-(t_delta-20)
		task_wait(20);
		#else
		//on the PC, under sim control, the simulation time will drive the progress of time to make sure everything is in lock-step
		//hence no need for a sleep statement
		OS_SCHEDULE;
		#endif
	}

	task_close();
}




void update_grid( int ir_sensor_index,float x1, float y1, float t1)
{
	float d,t2,x2,y2,x3,y3,x4,y4;

	d=((float)s.inputs.ir[ir_sensor_index]/10.0)*25.4;
	t2= s.inputs.theta;
	x2=x1+d*cos(t1);
	y2=y1+d*sin(t1);
	x3=x2*cos(t2)-y2*sin(t2);
	y3=x2*sin(t2)+y2*cos(t2);
	x3+=s.inputs.x;
	y3+=s.inputs.y;
	x4=x1*cos(t2)-y1*sin(t2);
	y4=x1*sin(t2)+y1*cos(t2);
	x4+=s.inputs.x;
	y4+=s.inputs.y;

	if(d<750)
	{
		//plotLine((int)(x4/25.4),(int)(y4/25.4), (int)(x3/25.4),(int)(y3/25.4) , -1);
		//grid[(int)(x3/25.4)][(int)(y3/25.4)]+=2;
		volatile float a,b;
		a=x3;
		b=y3;
		a=x4;
		b=y4;
	}
}
	
	
#ifdef WIN32
int main(int argc, char **argv)
#else
int main(void)
#endif
{
	#ifdef SVP_ON_WIN32
	{ extern void win32_main(int argc, char **argv); 	win32_main(argc, argv); }
	#endif
	hardware_init();  //initialize hardware & pololu libraries
	//i2c_init();

	//clear(); lcd_goto_xy(0,0); printf("V=%d",	read_battery_millivolts_svp()); delay_ms(100);
	
	//odometry_update(120,100,100.0f,100.0f,160.0f);  //440us
	//s.inputs.ir[1]=200;
	//update_grid(1,60.0f,50.0f,30.0f); //480us (math only - no grid update)

	play_from_program_space(welcome);
	//delay_ms(500);
	
	usb_printf("robot.c::main()\n");


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

	//serial_cmd_evt = event_create();
	line_alignment_start_evt = event_create();
	line_alignment_done_evt = event_create();

	task_create( main_loop_task,			1,  NULL, 0, 0);
	task_create( find_flame_fsm	,			2,  NULL, 0, 0);
	task_create( master_logic_fsm,			3,  NULL, 0, 0);
	task_create( lcd_update_fsm,			4,  NULL, 0, 0);

	//don't want the OS to schedule those tasks. we'll do that manually as part of the main loop
	//os_task_suspend(ml_tid);
	//os_task_suspend(us_tid);

	//os_start();
	running = 1;
	os_enable_interrupts();
	for (;;)
	{
		{extern void _os_tick(void); _os_tick(); }
		{extern void _os_idle(void); _os_idle(); }
	    running_tid = os_task_next_ready_task();
		task_run(main_loop_task,0,0);
	}
	//won't ever get here...
		
	return 0;
}

