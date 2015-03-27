
#include "standard_includes.h"



void return_home_fsm(u08 fsm_cmd, u08 *param)
{
	enum states
	{
		s_disabled		   = 0,		//0 
		s_exit_from_room_1 = 1,		//1
		s_exit_from_room_2 = 2,		//2
		s_exit_from_room_3 = 3,		//3
		s_exit_from_room_4 = 4,		//4

		s_none = 255
	};
	static enum states state = s_disabled;
	static enum states last_state = s_none;
	static u32 t_entry = 0;
	static u32 context_switch_counter = 0;
	static u32 t_last = 0;
	static float checkpoint;

	task_open_1();

	//initialize the parameter logic only the first time through
	if (context_switch_counter == 0)
	{
	}

	//update the parameter values the first time through and then everyh 500ms
	if ((context_switch_counter == 0) || (get_ms() - t_last) > 500)
	{
		t_last = get_ms();
	}

	context_switch_counter++;

	task_open_2();
	//code after this point resumes execution wherever it left off when a context switch happened

	usb_printf("return_home_fsm()\n");

	while (1)
	{
		//the following state transition applies to all states
		if (s.behavior_state[RETURN_HOME_FSM] == 0) state = s_disabled;


		first_(s_disabled)
		{
			enter_(s_disabled)
			{
				s.behavior_state[RETURN_HOME_FSM] = 0;
			}

			if (s.behavior_state[RETURN_HOME_FSM] != 0) state = s.behavior_state[RETURN_HOME_FSM];

			exit_(s_disabled)  
			{ 
			}
		}

		next_(s_exit_from_room_3)
		{
			enter_(s_exit_from_room_3)
			{
				RESET_LINE_DETECTION();
			}

			//at this point we are looking straight at the candle
			TURN_IN_PLACE(50, 90);
			GO(20);
			while ((s.ir[IR_NE] > 120) && (s.ir[IR_N] > 100) && (s.inputs.sonar[0] > 100))
			{
				OS_SCHEDULE; //TODO: use parameters here!
			}
			if ((s.ir[IR_N] < 100) || (s.inputs.sonar[0] < 100))
			{
				TURN_IN_PLACE(50, 90);
			}
			START_BEHAVIOR(FOLLOW_WALL_FSM, RIGHT_WALL);
			WAIT_FOR_LINE_DETECTION();
			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
			HARD_STOP();
			RESET_LINE_DETECTION();
			line_alignment_fsm_v2(1, 0);  while (line_alignment_fsm_v2(0, 0) != 0) { OS_SCHEDULE; }
			MOVE(20, 200);
			TURN_IN_PLACE(20, 90);
			MOVE(20, 180);
			START_BEHAVIOR(FOLLOW_WALL_FSM, LEFT_WALL);

			while (s.inputs.sonar[US_N] > 100)
			{
				OS_SCHEDULE;
			}

			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
			HARD_STOP();
			state = s_disabled;

			exit_(s_exit_from_room_3)
			{
				NOP();
			}
		}


		next_(s_exit_from_room_2)
		{
			enter_(s_exit_from_room_2)
			{
				RESET_LINE_DETECTION();
			}

			//at this point we are looking straight at the candle
			TURN_IN_PLACE(50, 90);
			GO(20);
			while ((s.ir[IR_NE] > 120) && (s.ir[IR_N] > 100) && (s.inputs.sonar[0] > 100))
			{
				OS_SCHEDULE; //TODO: use parameters here!
			}
			if ((s.ir[IR_N] < 100) || (s.inputs.sonar[0] < 100)) //is a wall right in front?
			{
				TURN_IN_PLACE(50, 60); //turning 90 is too much
			}
			START_BEHAVIOR(FOLLOW_WALL_FSM, RIGHT_WALL);
			WAIT_FOR_LINE_DETECTION();
			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
			HARD_STOP();
			RESET_LINE_DETECTION();
			line_alignment_fsm_v2(1, 0);  while (line_alignment_fsm_v2(0, 0) != 0) { OS_SCHEDULE; }
			MOVE(20, 220);
			TURN_IN_PLACE(20, 90);
			odometry_set_checkpoint();
			GO(20);
			while( (s.ir[IR_NW] > 160) && (odometry_get_distance_since_checkpoint()<180) ) { OS_SCHEDULE; }
			//MOVE(20, 180);
			START_BEHAVIOR(FOLLOW_WALL_FSM, LEFT_WALL);
			cfg_set_flt_by_grp_id(10, 13, 600); //corner_distance = 600, i.e. make sure we don't turn into the intersection but keep going straight

			while (s.inputs.sonar[US_N] > 100)
			{
				OS_SCHEDULE;
			}

			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
			HARD_STOP();
			state = s_disabled;

			exit_(s_exit_from_room_2)
			{
				NOP();
			}
		}



		next_(s_exit_from_room_1)
		{
			enter_(s_exit_from_room_1)
			{
				RESET_LINE_DETECTION();
			}

			//at this point we are looking straight at the candle
			TURN_IN_PLACE(50, -90);
			START_BEHAVIOR(FIND_WALL_FSM, LEFT_WALL);
			while (s.behavior_state[FIND_WALL_FSM]>0)
			{
				OS_SCHEDULE;
			}

			START_BEHAVIOR(FOLLOW_WALL_FSM, LEFT_WALL);
			WAIT_FOR_LINE_DETECTION();
			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
			HARD_STOP();
			RESET_LINE_DETECTION();
			line_alignment_fsm_v2(1, 0);  while (line_alignment_fsm_v2(0, 0) != 0) { OS_SCHEDULE; }
			MOVE(20, 270);
			TURN_IN_PLACE(20, -90);
			odometry_set_checkpoint();
			GO(20);
			while( (s.ir[IR_NW] > 160) && (odometry_get_distance_since_checkpoint()<180) ) { OS_SCHEDULE; }
			//MOVE(20, 180);
			START_BEHAVIOR(FOLLOW_WALL_FSM, LEFT_WALL);
			cfg_set_flt_by_grp_id(10, 13, 600);

			while (s.inputs.sonar[US_N] > 100)
			{
				OS_SCHEDULE;
			}

			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
			HARD_STOP();
			state = s_disabled;

			exit_(s_exit_from_room_1)
			{
				NOP();
			}
		}


		next_(s_exit_from_room_4)
		{
			enter_(s_exit_from_room_4)
			{
				RESET_LINE_DETECTION();
			}
			
			//at this point we are looking straight at the candle
			TURN_IN_PLACE(50, 90);  //left
			START_BEHAVIOR(FIND_WALL_FSM, RIGHT_WALL);
			while (s.behavior_state[FIND_WALL_FSM]>0)
			{
				OS_SCHEDULE;
			}
			START_BEHAVIOR(FOLLOW_WALL_FSM, RIGHT_WALL);
			WAIT_FOR_LINE_DETECTION();
			STOP_BEHAVIOR(FOLLOW_WALL_FSM);
			HARD_STOP();
			RESET_LINE_DETECTION();
			line_alignment_fsm_v2(1, 0);  while (line_alignment_fsm_v2(0, 0) != 0) { OS_SCHEDULE; }

			state = s_disabled;

			exit_(s_exit_from_room_4)
			{
				NOP();
			}
		}
		if (s.behavior_state[RETURN_HOME_FSM] != 0) s.behavior_state[RETURN_HOME_FSM] = state;
		OS_SCHEDULE;
	}
	task_close();
}
