
#include "standard_includes.h"


void servo_task(u08 cmd, u08 *param)
{
	static u08 initialized=0;
	DEFINE_CFG2(u16,s1_speed,15,1);					
	DEFINE_CFG2(u16,s2_speed,15,2);					
	DEFINE_CFG2(u16,s3_speed,15,3);					
	DEFINE_CFG2(u16,s4_speed,15,4);					
	DEFINE_CFG2(u16,s1_pos,15,5);					
	DEFINE_CFG2(u16,s2_pos,15,6);					
	DEFINE_CFG2(u16,s3_pos,15,7);					
	DEFINE_CFG2(u16,s4_pos,15,8);					

	//task_open_1();
	//code between _1() and _2() will get executed every time the scheduler resumes this task

	if(!initialized)
	{
		initialized=1;

		usb_printf("servo_task()\n");

		servos_start(demuxPins, sizeof(demuxPins));
	
		PREPARE_CFG2(s1_speed);					
		PREPARE_CFG2(s2_speed);					
		PREPARE_CFG2(s3_speed);					
		PREPARE_CFG2(s4_speed);					
		PREPARE_CFG2(s1_pos);					
		PREPARE_CFG2(s2_pos);					
		PREPARE_CFG2(s3_pos);					
		PREPARE_CFG2(s4_pos);					
	}

	//task_open_2();
	//execution below this point will resume wherever it left off when a context switch happens


	//while(1)
	{
		//task_wait(20);
		UPDATE_CFG2(s1_speed);					
		UPDATE_CFG2(s2_speed);					
		UPDATE_CFG2(s3_speed);					
		UPDATE_CFG2(s4_speed);					
		UPDATE_CFG2(s1_pos);					
		UPDATE_CFG2(s2_pos);					
		UPDATE_CFG2(s3_pos);					
		UPDATE_CFG2(s4_pos);					
		set_servo_speed(0, s1_speed);
		set_servo_speed(1, s2_speed);
		//set_servo_speed(2, s3_speed);
		//set_servo_speed(3, s4_speed);
		set_servo_target(0, s1_pos*10);
		set_servo_target(1, s2_pos*10);
		//set_servo_target(2, s3_pos*10);
		//set_servo_target(3, s4_pos*10);	
	}

	//task_close();
}
