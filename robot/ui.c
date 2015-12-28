
#include "standard_includes.h"

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
			START_BEHAVIOR(FOLLOW_WALL_FSM, LEFT_WALL); 
			//START_BEHAVIOR(MASTER_LOGIC_FSM,9); 
		}
		if(button_is_pressed(BOTTOM_BUTTON))
		{
			play_note(A(3), 50, 10);
			START_BEHAVIOR(MASTER_LOGIC_FSM,2); 
		}


		/***************************************************************************
		// display the selected screen
		***************************************************************************/
		if(s.lcd_screen==0)
		{
			/*
			clear();  
			lcd_goto_xy(0,0);
			lcd_printf("L: %3d %3d", s.inputs.analog[AI_LINE_RIGHT],s.inputs.analog[AI_LINE_LEFT]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 	
			lcd_printf("F: %03d %3d %3d", s.inputs.analog[AI_FLAME_NE],s.inputs.analog[AI_FLAME_N],s.inputs.analog[AI_FLAME_NW]); 
			*/
			clear();  
			lcd_goto_xy(0,0);
			lcd_printf("b:%5ld P:%5ld", lidar_bytes_received,lidar_packets_received); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1);
			lcd_printf("b:%5ld P:%5ld", lidar_bytes_per_second,lidar_packets_per_second); 
		}
		else if(s.lcd_screen==1)
		{
			clear();  
			lcd_goto_xy(0,0); 
			lcd_printf("U: %4d %4d",  s.inputs.sonar[US_E],s.inputs.sonar[US_W]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 
			lcd_printf("%4d %4d %4d",  s.inputs.sonar[US_NE],s.inputs.sonar[US_N],s.inputs.sonar[US_NW]); 
		}
		else if(s.lcd_screen==2)
		{
			clear();  
			lcd_goto_xy(0,0); 
			lcd_printf("i: %03d %03d %03d",s.ir[IR_NE], s.ir[IR_N], s.ir[IR_NW]); 
			OS_SCHEDULE;
			lcd_goto_xy(0,1); 
			lcd_printf("%03d %03d %03d %03d",s.ir[IR_E],s.ir[IR_NR],s.ir[IR_NL],s.ir[IR_W]); 
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
