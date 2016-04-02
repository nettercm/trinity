
#ifdef WIN32
#include <Windows.h>
#include <timeapi.h>
#endif



#include "standard_includes.h"

/*



Room 4 permutations:
#	Dog				Door	Lt, rt	dU		Dog, door	Exit strategy
1	K2 / North		North	3,0		3050	1,2			F, r, f, r, f, Rwf around Rm#4 for some distance, then do Lwf until we see the wall
2	K2 / North		South	0,0		1250	1,1			F, r, f, r, f, Lwf 
3	R5 / East		North	0,3		2900	2,2			F, l, Rwf
4	R5 / East		South	0,0		1250	2,1			F, r, f, r, f, Lwf
5	L10 / South		North	1,0		1600	3, 2		F, l, Rwf
6	O10 / South		South	3-4,0	3500	3, 1		F, r, f, r, f, Lwf
  

Todo:

* add large electrolytic capacitors to the 5V power rail to prevent brown-out / reset when the pump turns on
* replace front caster with much smaller version that can traverse carpet edge
* consider attaching a low-friction material to the rim of the base to avoid getting stuck on wall during certain turn maneuvers

* consider changing the type of motor command used during wall following - regulate speed to avoid getting stuck at low speeds
* add a config paramter that turns debug printf via serial comms on or off
* revisit the pan/tilt pattern during extinguishing
* convert the various flame detection threasholds into config parameters


Done:
* move pan/tilt mechanism back by .5" so that the nozzle doesn't stick out beyond the base; avoid nozzle getting stuck on wall or door frame
* add rubber band (or something) to pan/tilt mechanism to dampen vibrations
* confirm that max height of 27cm is not exceeded
* add "front" label and other labels to the handle bar
* attach the water tank and provide a means to refill
* wire-up the kill switch/plug


Issues:

* mirror placement can result in sharp_corner turn
* entering room 4 south: might get too far down the hallway, i.e. too far to the left of the door
* exiting room 1 top door while going home not reliable




Low Priority:

•	too slow when make 90deg inside turns
•	when turning left after exiting from room 1, robot is way too far to the right




Solved Issues:

•	When the flame is close to a wall, the flame’s reflection from the wall can max out the flame sensors => may end up NOT looking straight at the candle when stopped
	o	Do a sweep and use IR and/or flame sensor data to find the correct peak position
	o	If the wall is known to be on the left, sweep from the right until the right sensor is maxed out

*   when the candle is very close right after entering the room, candle finding fails

*	turn sound back on for improved debuggin? (don't initialize servo feature until when its needed)
	=> done, but only for playing sequences and it is only done on powerup / reset. servos get initialized later

•	If the candle is in the middle of a large room, the robot will crash into it if we are approaching it off-center (sonar won’t see it)
	o	Solution: add a NE and NW facing sonar

•	robot does not go into room 3 if door opening is < 16".
	o	may have to do w/ recently added front sensor logic,
	o	it improved after tweaking wall_following.c line 120)

*   what about carpet bump at the entrance to a room?  how will it affect the line sensor readings?
	=> added "suspension" to the back side to dampen the bobble.  Seems to work

•	When the candle is in the bottom right corner of room 3, the "door frame" might be seen by the sonar and so the robot won't go closer to the candle
	(similar situations possible in other rooms) 
	=> move further into the room before locating candle

•	line detection: signal-to-noise ration on analog[10]/right line is bad;   also,  missing the entrance to room 1 (maybe exit from room 2 confuses the line detector)
	=> since we may or may not see the line while exiting from a room, now using odometry to ignore a line that is "too close" to the room we are coming from

•	acceleration from 0 to min speed / turn speed needs to be more smooth if target speed is > 30
	=> added suspension to the back to prevent back-and-forth wobble

•	When doing a 360 spin to look for the candle, need to make sure we don’t get confused by any IR potentially coming from outside the room
	=> added a shield


*   while returning home, there eare cases where the US_N might pick up the corner of the intersection and we consider it as having reached the end...
	=> all 3 front-facing sonars must now read <100 before we stop

Notes:

s.U:
Home:	0
Rm#3:	1500
Rm#2:	3380
Rm#1:	3980  (configuration 1 - door on the bottom)
Rm#1:	4160  (configuration 2 - door on the top)


*/



const unsigned char pulseInPins[] = PULSE_PINS; 
const unsigned char demuxPins[] = SERVO_DEMUX_PINS; // eight servos




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





void scan(u08 cmd, u16 moving_avg)
{
	static s16 last_angle;
	s16 angle;
	static u16 i=0;
	static float flame_avg;
	static u16 ir_n_avg;
	static u16 ir_fn_avg;
	u16 ir_n;
	u16 ir_fn;
	u16 flame_c, flame_l, flame_r;

	if(moving_avg == 0 ) moving_avg = 1;
	if(moving_avg >= 16) moving_avg = 16;

	angle = (s16) (odometry_get_rotation_since_checkpoint()+0.5);

	//currently our sharp IR lookup table doesn't clamp the readings at the sensors max range, so there could be some large values; need to clamp here
	ir_n = s.ir[IR_N];	   if(ir_n >  600) ir_n =  600;
	ir_fn= ir_n; //s.ir[IR_FAR_N]; if(ir_fn > 600) ir_fn = 600;

	flame_l = s.inputs.analog[AI_FLAME_NW];
	flame_r = s.inputs.analog[AI_FLAME_NE];
	flame_c = (flame_l+flame_r)/2;


	if(cmd==1) //initialize the state and our moving average
	{
		last_angle = angle;
		memset(scan_data,0,sizeof(scan_data));
		flame_avg = flame_c;
		ir_n_avg  = ir_n;
		ir_fn_avg = ir_fn;
		i=0;
		dbg_printf("scan(%d,%d): f=%d, ir_n=%d, ir_fn=%d\n", cmd, moving_avg, flame_avg,ir_n_avg,ir_fn_avg);
	}
	else //update
	{
		//use a moving average to filter out any noise spikes; prevent noise spikes from showing up as "peak" values later
		flame_avg = ((flame_avg * (moving_avg-1)) + (flame_c) ) / (float)moving_avg;
		ir_n_avg  = ((ir_n_avg  * (moving_avg-1)) + (ir_n) ) / moving_avg;
		ir_fn_avg = ((ir_fn_avg * (moving_avg-1)) + (ir_fn) ) / moving_avg;
		if(angle != last_angle) 
		{
			scan_data[i].angle			= angle;
			scan_data[i].abs_angle		= (s16)(s.inputs.theta * K_rad_to_deg);
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
		s_exit_from_room,

		s_none=255
	};
	static enum states state=s_disabled;
	static enum states last_state=s_none;
	static u32 t_entry=0;
	static u08 still_inside_room=0;
	static u32 context_switch_counter=0;
	static u32 t_last=0;
	static u08 stop;
	static u08 last_room=0;
	static float checkpoint;

	DEFINE_CFG2(s16,turn_speed,						9,10);				DEFINE_CFG2(s16,cmd,							9,11);
	DEFINE_CFG2(s16,accel,							9,12);				DEFINE_CFG2(s16,decel,							9,13);
	DEFINE_CFG2(s16,flame_scan_edge_threashold,		9,14);				DEFINE_CFG2(s16,flame_found_threashold ,		9,15);
	DEFINE_CFG2(u16,flame_scan_filter ,				9,16);

	DEFINE_CFG2(s16,room3_enter,					9,20);				DEFINE_CFG2(s16,room3_turn_1,					9,21);
	DEFINE_CFG2(s16,room3_turn_2,					9,22);				DEFINE_CFG2(s16,room3_turn_3,					9,23);

	DEFINE_CFG2(s16,room2_enter,					9,30);				DEFINE_CFG2(s16,room2_turn_1,					9,31);
	DEFINE_CFG2(s16,room2_turn_2,					9,32);				DEFINE_CFG2(s16,room2_turn_3,					9,33);

	DEFINE_CFG2(s16,room1_enter,					9,40);				DEFINE_CFG2(s16,room1_turn_1,					9,41);
	DEFINE_CFG2(s16,room1_turn_2,					9,42);				DEFINE_CFG2(s16,room1_turn_3,					9,43);

	DEFINE_CFG2(s16,dog_scan_distance,				9,51);				DEFINE_CFG2(s16,dog_scan_sensor,				9,52);
	DEFINE_CFG2(s16,dog_scan_angle,					9,53);				DEFINE_CFG2(u16,dog_scan_filter ,				9,54);

	DEFINE_CFG2(s16,find4_distance_1,				9,60);				DEFINE_CFG2(s16,find4_turn_1,					9,61);
	DEFINE_CFG2(s16,find4_distance_2,				9,62);				DEFINE_CFG2(s16,find4_left_margin_1,			9,63);
	DEFINE_CFG2(s16,find4_right_margin_1,			9,64);				DEFINE_CFG2(s16,find4_distance_3,				9,65);
	DEFINE_CFG2(s16,find4_left_margin_2,			9,66);				DEFINE_CFG2(s16,find4_right_margin_2,			9,67);

	DEFINE_CFG2(s16,search4_distance_1,				9,71);				DEFINE_CFG2(s16,search4_turn_1,					9,72);
	DEFINE_CFG2(s16,search4_turn_2,					9,73);

	task_open_1();

	//initialize the parameter logic only the first time through
	if( context_switch_counter==0 )
	{
		PREPARE_CFG2(turn_speed);				PREPARE_CFG2(cmd);									PREPARE_CFG2(accel);
		PREPARE_CFG2(decel);					PREPARE_CFG2(flame_scan_edge_threashold);			PREPARE_CFG2(flame_found_threashold);

		PREPARE_CFG2(room3_enter			);	PREPARE_CFG2(room3_turn_1			);
		PREPARE_CFG2(room3_turn_2			);	PREPARE_CFG2(room3_turn_3			);

		PREPARE_CFG2(room2_enter			);	PREPARE_CFG2(room2_turn_1			);
		PREPARE_CFG2(room2_turn_2			);	PREPARE_CFG2(room2_turn_3			);

		PREPARE_CFG2(room1_enter			);	PREPARE_CFG2(room1_turn_1			);
		PREPARE_CFG2(room1_turn_2			);	PREPARE_CFG2(room1_turn_3			);

		PREPARE_CFG2(dog_scan_distance		);	PREPARE_CFG2(dog_scan_sensor		);
		PREPARE_CFG2(dog_scan_angle			);	PREPARE_CFG2(find4_distance_1		);

		PREPARE_CFG2(find4_turn_1			);	PREPARE_CFG2(find4_distance_2		);
		PREPARE_CFG2(find4_left_margin_1	);	PREPARE_CFG2(find4_right_margin_1	);
		PREPARE_CFG2(find4_distance_3		);	PREPARE_CFG2(find4_left_margin_2	);
		PREPARE_CFG2(find4_right_margin_2	);	
		
		PREPARE_CFG2(search4_distance_1);		PREPARE_CFG2(search4_turn_1	);
		PREPARE_CFG2(search4_turn_2	);	
	}

	//update the parameter values the first time through and then everyh 500ms
	if( (context_switch_counter==0) || (get_ms()-t_last) > 500 )  
	{
		UPDATE_CFG2(turn_speed);					UPDATE_CFG2(cmd);
		UPDATE_CFG2(accel);							UPDATE_CFG2(decel);
		UPDATE_CFG2(flame_scan_edge_threashold);	UPDATE_CFG2(flame_found_threashold);

		UPDATE_CFG2(room3_enter				);		UPDATE_CFG2(room3_turn_1			);
		UPDATE_CFG2(room3_turn_2			);		UPDATE_CFG2(room3_turn_3			);

		UPDATE_CFG2(room2_enter				);		UPDATE_CFG2(room2_turn_1			);
		UPDATE_CFG2(room2_turn_2			);		UPDATE_CFG2(room2_turn_3			);

		UPDATE_CFG2(room1_enter				);		UPDATE_CFG2(room1_turn_1			);
		UPDATE_CFG2(room1_turn_2			);		UPDATE_CFG2(room1_turn_3			);

		UPDATE_CFG2(dog_scan_distance		);		UPDATE_CFG2(dog_scan_sensor		);
		UPDATE_CFG2(dog_scan_angle			);

		UPDATE_CFG2(find4_distance_1		);		UPDATE_CFG2(find4_turn_1			);
		UPDATE_CFG2(find4_distance_2		);		UPDATE_CFG2(find4_left_margin_1	);
		UPDATE_CFG2(find4_right_margin_1	);		UPDATE_CFG2(find4_distance_3		);
		UPDATE_CFG2(find4_left_margin_2	);			UPDATE_CFG2(find4_right_margin_2	);

		UPDATE_CFG2(search4_distance_1);			UPDATE_CFG2(search4_turn_1	);
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


		//-------------------------------------------------------------------------------------------------------
		// Initial state.  Make sure everything is (re)initialized, so we can restart this fsm if a cmd is received
		//-------------------------------------------------------------------------------------------------------
		first_(s_disabled)
		{
			enter_(s_disabled) 
			{  
				HARD_STOP(); //motor_command(cmd,accel,decel,0,0);
				PUMP_OFF();
				STOP_BEHAVIOR(FOLLOW_WALL_FSM);
				//STOP_BEHAVIOR(MASTER_LOGIC_FSM);
				RESET_LINE_DETECTION();
				s.dog_position=0;
				s.inputs.watch[0] = s.inputs.watch[1] = s.inputs.watch[2] = s.inputs.watch[3] = 0;
				odometry_set_checkpoint();
				reset_start_signal(); //mainly required to make sure we don't restart the whole thing in simulation mode
			}

			//if(s.behavior_state[MASTER_LOGIC_FSM]!=0) state = s.behavior_state[MASTER_LOGIC_FSM]; //s_waiting_for_start;
			state = s_waiting_for_start;
			s.current_room = 0;
			s.behavior_state[MASTER_LOGIC_FSM] = s_waiting_for_start;

			exit_(s_disabled)  
			{ 
				NOP();
			}
		}




		//-------------------------------------------------------------------------------------------------------
		//
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
				//if(s.behavior_state[MASTER_LOGIC_FSM]>1) state = s.behavior_state[MASTER_LOGIC_FSM];
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
				checkpoint = s.U;
			}
			
			TURN_IN_PLACE(turn_speed, -90);

			//is something right in front of us?
			//if so, then we were facing south initially, and now we are facing west; turn back...
			if( (s.ir[IR_N] < 120) || (s.inputs.sonar[0] < 120) )  TURN_IN_PLACE(turn_speed, 90);

			//on to the next state...
			state = s_finding_room_3;

			exit_(s_aligning_south) 
			{ 
				NOP();
			}
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
				checkpoint = s.U;
			}

			//take into account the fact that initially we'll be on the home circle
			if( LINE_WAS_DETECTED() )
			{
				RESET_LINE_DETECTION();
				if((s.U - checkpoint > 500) )
				{
					play_note(C(3), 50, 10);
					HARD_STOP();
					STOP_BEHAVIOR(FOLLOW_WALL_FSM);
					state = s_searching_room_3;
				}
			}

			exit_(s_finding_room_3) 
			{ 
				NOP();
			}
		}




		//-------------------------------------------------------------------------------------------------------
		//  We are standing at the entrance to room 3 - let's see if there is a candle in this room
		//-------------------------------------------------------------------------------------------------------
		next_(s_searching_room_3)
		{
			enter_(s_searching_room_3)
			{
				s.current_room = 3;
				checkpoint = s.U;
				task_wait(100);
			}

			if(is_flame_present())
			{
				line_alignment_fsm_v2(1, 0);  while (line_alignment_fsm_v2(0, 0) != 0) { OS_SCHEDULE; }
				START_BEHAVIOR(FIND_FLAME_FSM, 1);
				while (s.behavior_state[FIND_FLAME_FSM]>0)
				{
					OS_SCHEDULE;
				}
				state = s_exit_from_room;
			}
			else
			{
				TURN_IN_PLACE(turn_speed, room3_turn_1);
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
				RESET_LINE_DETECTION();
				if((s.U - checkpoint > 500) )
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
				checkpoint = s.U;
			}


			//odometry_update_postion(27.0f, ((float)(s.ir[IR_NW]))/16.0f , 180.0f);

			if (is_flame_present())
			{
				line_alignment_fsm_v2(1,0);  while(line_alignment_fsm_v2(0,0)!=0) { OS_SCHEDULE; }
				START_BEHAVIOR(FIND_FLAME_FSM, 1);
				while (s.behavior_state[FIND_FLAME_FSM]>0)
				{
					OS_SCHEDULE;
				}
				state = s_exit_from_room;
			}
			else
			{
				TURN_IN_PLACE(turn_speed, room2_turn_1);
				still_inside_room = 1;
				state = s_finding_room_1;
			}

			exit_(s_searching_room_2) 
			{
				NOP();
			}
		}




		//-------------------------------------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------------------------------------
		next_(s_finding_room_1)
		{
			enter_(s_finding_room_1)
			{
				START_BEHAVIOR(FOLLOW_WALL_FSM,RIGHT_WALL); //start following the RIGHT wall
			}

			if( LINE_WAS_DETECTED() )
			{
				RESET_LINE_DETECTION();
				if((s.U - checkpoint > 200) )
				{
					play_note(C(3), 50, 10);
					HARD_STOP();
					STOP_BEHAVIOR(FOLLOW_WALL_FSM);
					state = s_searching_room_1;
				}
			}

			exit_(s_finding_room_1) 
			{ 
				NOP();
			}
		}




		//-------------------------------------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------------------------------------
		next_(s_searching_room_1)
		{
			enter_(s_searching_room_1) 
			{ 
				s.current_room = 1; 
				checkpoint = s.U;
			}

			//line_alignment_fsm_v2(1,0);  while(line_alignment_fsm_v2(0,0)!=0) { OS_SCHEDULE; }

			//which door are we enterhing through?

			if (is_flame_present())
			{
				line_alignment_fsm_v2(1, 0);  while (line_alignment_fsm_v2(0, 0) != 0) { OS_SCHEDULE; }
				START_BEHAVIOR(FIND_FLAME_FSM, 1);
				while (s.behavior_state[FIND_FLAME_FSM]>0)
				{
					OS_SCHEDULE;
				}
				state = s_exit_from_room;
			}
			else
			{
				//if there was no candle, go on to the next room.
				still_inside_room = 1;
				RESET_LINE_DETECTION();
				//we should be facing more or less SW, but too far away from the wall depending on door location
				//we first need to find the wall before we can follow it; let's turn left to face SE, then go straight towards the wall
				TURN_IN_PLACE(turn_speed, room1_turn_1);
				motor_command(6, accel, decel, turn_speed, turn_speed);
				while ((s.ir[IR_NE] > 120) && (s.ir[IR_N] > 100))
				{
					OS_SCHEDULE; //TODO: use parameters here!
				}
				motor_command(cmd, accel, decel, 0, 0);
				state = s_finding_room_4;
			}

			exit_(s_searching_room_1) 
			{
				NOP();
			}
		}




		//-------------------------------------------------------------------------------------------------------
		// Find the door into romm #4
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

			//odometry_update_postion(89.0f, ((float)(s.ir[IR_NE]))/16.0f, 90.0f);

			s.left_turns = 0;
			s.right_turns = 0;
			checkpoint = s.U;


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
				s.dog_position=2;
			}

			if(s.dog_position==0) //if we didn't already see the dog, scan the other hallway for it ...
			{
				//now scan for another 90 degrees, i.e. from NW to SW - this covers the west facing hallway
				TURN_IN_PLACE_AND_SCAN( 40, 90 , dog_scan_filter);
				scan_result = find_path_in_scan(scan_data, 100, dog_scan_distance, 0, dog_scan_sensor); //TODO: adjust this range - is 30" to far?
			}
			else
			{
				TURN_IN_PLACE(turn_speed, 45);	//now we are facing West again
			}

			if( (s.dog_position == 0) && (scan_result.opening < dog_scan_angle) ) //TODO: fix this angle
			{
				s.dog_position=3;
				//if there is a dog / obstacle right in front, then there won't be a 2nd dog to worry about...
				//just fallow the left wall...//eventually we'll wind up inside room 4, either via door on North side or on South side
				//so this takes care of 2 of 6 possible cases
				TURN_IN_PLACE(turn_speed, -(90+45)); //now we are facing north again
				TURN_IN_PLACE(turn_speed, find4_turn_1); //15);
				MOVE2(turn_speed, find4_distance_2,find4_left_margin_1,find4_right_margin_1);
				RESET_LINE_DETECTION();
				START_BEHAVIOR(FOLLOW_WALL_FSM,LEFT_WALL); //start  following the LEFT wall
				s.left_turns=0;
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
			if(s.dog_position==0)//if we still have not yet seen the dog yet, then it means we did a bunch of looking around so we need to re-align ourselves...
			{
				TURN_IN_PLACE(turn_speed, -45);	//now we are facing West again
				s.dog_position=1; //if we have not seen the dog, it must be in position 1 (North)
			}

			//MOVE(turn_speed, 32*25);		//move forward about 30inches so that we are in the position where the door could be
			//MOVE2(turn_speed, 32*25,135,135);		//move forward about 30inches so that we are in the position where the door could be
			HARD_STOP();
			MOVE2(turn_speed, find4_distance_3, find4_left_margin_2, find4_right_margin_2);
			HARD_STOP();
			TURN_IN_PLACE(turn_speed, -90);	//now we would be facing the door

			if( (s.ir[IR_N] > 160) && (s.inputs.sonar[US_N]>160) )	//is there an opening right in front of us?
			{
				//yes...
				//at this point we are facing north and a door into room #4 is right in front of us
				//this takes care of another 2 of the 6 possbile cases
				s.door_position = 1;
				RESET_LINE_DETECTION();
				GO(turn_speed);
				WAIT_FOR_LINE_DETECTION();
				HARD_STOP();
				switch_(s_finding_room_4, s_searching_room_4);
			}


			//nope...there's no door here....we are looking at the south side wall of room #4

			if(s.dog_position!=2) /*..if there was no dog on the east side of room 4 when we checked earlier, then just go around that side....*/
			{
				s.door_position = 2;
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
			
			s.door_position = 2;
			TURN_IN_PLACE(turn_speed, 90);  //turn left to face West
			//keep following the right wall until we have passed through the door;  i.e. going counter-clockwise around Rm #4
			RESET_LINE_DETECTION();
			START_BEHAVIOR(FOLLOW_WALL_FSM,RIGHT_WALL);
			WAIT_FOR_LINE_DETECTION();

			//we reached room 4 - so proceed to the appropriate state
			play_note(C(3), 50, 10);
			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
			HARD_STOP();
			RESET_LINE_DETECTION();
			switch_(s_finding_room_4, s_searching_room_4);

			exit_(s_finding_room_4) 
			{
				if(s.dog_position==3) //distance travelled tells us which door we ultimately went through into Room #4
				{
					if(s.U-checkpoint < 2500)
					{
						s.door_position=2;
					}
					else
					{
						s.door_position=1;
					}
				}
			}
		}




		//-------------------------------------------------------------------------------------------------------
		//    Search for the candle in Room #4
		//-------------------------------------------------------------------------------------------------------
		next_(s_searching_room_4)
		{
			enter_(s_searching_room_4) 
			{ 
				s.current_room = 4; 
				checkpoint = s.U;
			}

			//line_alignment_fsm_v2(1,0);  while(line_alignment_fsm_v2(0,0)!=0) { OS_SCHEDULE; }
			//if(s.inputs.theta < (180.0f*K_deg_to_rad)) 	odometry_update_postion(57.0f, 56.0f, 90.0f);
			//else odometry_update_postion(66.0f, 76.0f, 270.0f);

			//we are facing either North or South, but it doesn't really matter...
			if (is_flame_present())
			{
				line_alignment_fsm_v2(1, 0);  while (line_alignment_fsm_v2(0, 0) != 0) { OS_SCHEDULE; }
				START_BEHAVIOR(FIND_FLAME_FSM, 1);
				while (s.behavior_state[FIND_FLAME_FSM]>0)
				{
					OS_SCHEDULE;
				}
				state = s_exit_from_room;
			}
			else
			{
			}
			//if there was no candle, then we messed up, because Room #4 is alwasy the last room we search!
			//state = s_disabled;

			exit_(s_searching_room_4) 
			{
				NOP();
			}
		}




		//-------------------------------------------------------------------------------------------------------
		//    Done - return home
		//-------------------------------------------------------------------------------------------------------
		next_(s_exit_from_room)
		{
			enter_(s_exit_from_room)
			{
				NOP();
			}

			START_BEHAVIOR(RETURN_HOME_FSM, s.current_room);
			while (s.behavior_state[RETURN_HOME_FSM]>0)
			{
				OS_SCHEDULE;
			}
			state = s_disabled;

			exit_(s_exit_from_room)
			{
				NOP();
			}
		}



		next_(100)
		{
			static t_scan_result scan_result;
			enter_(100) {   NOP();   }
			TURN_IN_PLACE(20,-120);
			TURN_IN_PLACE_AND_SCAN(20,270,2);
			scan_result = find_flame_in_scan(scan_data,360,10);
			TURN_IN_PLACE( turn_speed, -(270-scan_result.center_angle) );
			state = s_disabled;
			exit_(100)  {   NOP();   }
		}


		s.inputs.watch[2]=state;
		if(s.behavior_state[3]!=0) s.behavior_state[3]=state;
		if(state!=last_state) 
		{
			dbg_printf("%6ld: ML:  %d->%d.  Rm#=%d  s.U=%f  s.U-checkpoint=%f  r-t=%d\n", get_ms(), last_state, state, s.current_room, s.U, s.U-checkpoint, s.right_turns);
		}

		//task_wait(25);
 		OS_SCHEDULE;
	}

	task_close();
}
#pragma endregion



