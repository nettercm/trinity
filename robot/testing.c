
#include "standard_includes.h"

extern void scan(u08 cmd, u16 moving_avg);





//480us (math only - no grid update)
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


void test_task(u08 cmd, u08 *param)
{
	static u16 i;
	float time_to_stop=0,distance_to_stop;
	static t_scan_result scan_result;
	DEFINE_CFG2(s16,accel,		99,1);					
	DEFINE_CFG2(s16,decel,		99,2);					
	DEFINE_CFG2(s16,speed,		99,3);					
	DEFINE_CFG2(s16,distance,	99,4);					
	DEFINE_CFG2(s16,angle,		99,5);					

	task_open_1();
	//code between _1() and _2() will get executed every time the scheduler resumes this task

	if(cmd==0) 
	{
		NOP();
	}
	else
	{
		NOP();
		return;
	}
	
	
	task_open_2();
	//execution below this point will resume wherever it left off when a context switch happens

	usb_printf("test_task()\n");

	PREPARE_CFG2(accel);
	PREPARE_CFG2(decel);
	PREPARE_CFG2(speed);
	PREPARE_CFG2(distance);
	PREPARE_CFG2(angle);
	
	test_task(1,(uint8 *)0x1234);

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
		UPDATE_CFG2(angle);

		if(s.behavior_state[TEST_LOGIC_FSM]==1) 
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

			TURN_IN_PLACE_AND_SCAN( 40, 90, 1);
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
			s.behavior_state[TEST_LOGIC_FSM]=0;
		}

		if(s.behavior_state[TEST_LOGIC_FSM]==2)
		{
			TURN_IN_PLACE(speed,angle);
			s.behavior_state[TEST_LOGIC_FSM]=0;
		}

		if(s.behavior_state[TEST_LOGIC_FSM]==3)
		{
			TURN_IN_PLACE(40, -90);
			TURN_IN_PLACE_AND_SCAN(40, 180, 4);
			scan_result = find_flame_in_scan(scan_data,360,30);
			if(scan_result.flame_center_value > 150) //TODO: make the minimum flame value a parameter
			{
				static int i, i_min;
				static u16 min=999;
				static float d;
				static u08 stop=0;
				TURN_IN_PLACE( 40, -(180-scan_result.center_angle+2) );
				/*
				min=999;
				for(i=scan_result.rising_edge_position-10; i<=scan_result.falling_edge_position+10; i++)
				{
					dbg_printf("scan_data[%3d]: ir_n=%3d, a=%d, f=%d\n",i,scan_data[i].ir_north, scan_data[i].angle, scan_data[i].flame);
					task_wait(50);
					if(scan_data[i].ir_north < min) { min=scan_data[i].ir_north; i_min=i; }
				}
				dbg_printf("distance to candle: %d @i=%d,a=%d\n",min,i_min,scan_data[i_min].angle);
				if(min<100) min=100;
				d = (float) (((min-100)*25)/10);
				MOVE2(50,d,60,60);
				*/
				
				stop=0;
				move_manneuver2(1,30,9999,(80),(90)); 
				while(move_manneuver2(0,30,9999,(70),(70))) 
				{
					OS_SCHEDULE;
					if( (s.ir[IR_N] <= 60) ) stop |= 0x01;
					if( (s.ir[IR_NE] <= 60)) stop |= 0x02;
					if( (s.ir[IR_NW] <= 60)) stop |= 0x04;
					if( (s.inputs.sonar[0] <= 100) ) stop |= 0x08;

					if(stop != 0)
					{
						dbg_printf("too close to object/wall! reason: 0x%02x\n",stop);
						HARD_STOP();
						break;
					}
				}
				
				
			}
			s.behavior_state[TEST_LOGIC_FSM]=0;
		}

		if(s.behavior_state[TEST_LOGIC_FSM]==4) 
		{
			PUMP_ON();
			task_wait(1000);
			PUMP_OFF();
			s.behavior_state[TEST_LOGIC_FSM]=0;
		}

		if(s.behavior_state[TEST_LOGIC_FSM]==5) 
		{
			dbg_printf("start = %d\n",is_digital_input_high(IO_B3));
			task_wait(500);
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
				s.behavior_state[TEST_LOGIC_FSM]=0;
			}
			task_wait(20);
		}
		*/
	}

	task_close();
}






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




