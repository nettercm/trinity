
#include "standard_includes.h"

DEFINE_CFG2(u16,tilt_center_pos,15,9);					
DEFINE_CFG2(u16,pan_center_pos, 15,10);					


void pan_tilt_center(void)
{
	t_config_value v;

	v.u16 = cfg_get_u16_by_grp_id(15,9);
	tilt_absolute(v.u16);
	v.u16 = cfg_get_u16_by_grp_id(15,10);
	pan_absolute(v.u16);
}


void pan_relative(s16 delta_x)
{
	t_config_value v;

	v.u16 = cfg_get_u16_by_grp_id(15,6);
	v.u16 += delta_x;
	cfg_set_value_by_grp_id(15,6, v);
}

void pan_absolute(u16 x)
{
	t_config_value v;

	v.u16 = x;
	cfg_set_value_by_grp_id(15,6, v);
}


void tilt_relative(s16 delta_y)
{
	t_config_value v;

	v.u16 = cfg_get_u16_by_grp_id(15,5);
	v.u16 += delta_y;
	cfg_set_value_by_grp_id(15,5, v);
}

void tilt_absolute(u16 y)
{
	t_config_value v;

	v.u16 = y;
	cfg_set_value_by_grp_id(15,5, v);
}


void servo_task(u08 cmd, u08 *param)
{
	static u08 initialized=0;
	DEFINE_CFG2(u16,s1_speed,15,1);					
	DEFINE_CFG2(u16,s2_speed,15,2);					
	DEFINE_CFG2(u16,s1_pos,15,5);					
	DEFINE_CFG2(u16,s2_pos,15,6);					

	//task_open_1();
	//code between _1() and _2() will get executed every time the scheduler resumes this task

	if(!initialized)
	{
		initialized=1;

		usb_printf("servo_task()\n");

		servos_start(demuxPins, sizeof(demuxPins));

		PREPARE_CFG2(s1_speed);					
		PREPARE_CFG2(s2_speed);					
		PREPARE_CFG2(s1_pos);					
		PREPARE_CFG2(s2_pos);					
		PREPARE_CFG2(tilt_center_pos);					
		PREPARE_CFG2(pan_center_pos);					

		pan_tilt_center();
	}

	//task_open_2();
	//execution below this point will resume wherever it left off when a context switch happens


	//while(1)
	{
		//task_wait(20);
		UPDATE_CFG2(s1_speed);					
		UPDATE_CFG2(s2_speed);					
		UPDATE_CFG2(s1_pos);					
		UPDATE_CFG2(s2_pos);					
		UPDATE_CFG2(tilt_center_pos);					
		UPDATE_CFG2(pan_center_pos);					

		set_servo_speed(0, s1_speed);
		set_servo_speed(1, s2_speed);
		set_servo_target(0, s1_pos*10);
		set_servo_target(1, s2_pos*10);
	}

	//task_close();
}
