
#ifdef WIN32
#include <Windows.h>
#include <timeapi.h>
#endif



/*
Issues:

*   what about carpet bump at the entrance to a room?  how will it affect the line sensor readings?

�	When the flame is close to a wall, the flame�s reflection from the wall can max out the flame sensors => may end up to looking straight at the candle when stopped
o	Do a sweep and use IR and/or flame sensor data to find the correct peak position
o	If the wall is known to be on the left, sweep from the right until the right sensor is maxed out

�	When the candle is in the bottom right corner of room 3, the "door frame" might be seen by the sonar and so the robot won't go closer to the candle
(similar situations possible in other rooms) solution: move further into the room before locating candle?


�	line detection: signal-to-noise ration on analog[10]/right line is bad;   also,  missing the entrance to room 1 (maybe exit from room 2 confuses the line detector)




Low Priority:

�	acceleration from 0 to min speed / turn speed needs to be more smooth if target speed is > 30

�	When exiting room number 1 after extinguishing,  the door chosen depends on where the candle was

�	too slow when make 90deg inside turns

�	when turning left after exiting from room 1, robot is way too far to the right

�	When doing a 360 spin to look for the candle, need to make sure we don�t get confused by any IR potentially coming from outside the room
o	Need to look at the angle associated with the reading, i.e. ignore if it is coming from behind the robot
o	Don�t do a 360 degree spin � turn right >90deg and then left >180deg

Solved Issues:

�	If the candle is in the middle of a large room, the robot will crash into it if we are approaching it off-center (sonar won�t see it)
o	Solution: add a NE and NW facing sonar

�	robot does not go into room 3 if door opening is < 16".
o	may have to do w/ recently added front sensor logic,
o	it improved after tweaking wall_following.c line 120)



Notes:

s.U:
Home:	0
Rm#3:	1500
Rm#2:	3380
Rm#1:	3980  (configuration 1 - door on the bottom)
Rm#1:	4160  (configuration 2 - door on the top)


*/



#include "standard_includes.h"


const char welcome[] PROGMEM = ">g32>>c32";




//does not initialize servos
int hardware_init(void)
{
	UCSR1A = 0;
	DDRD = 0;

	//analog
	set_analog_mode(MODE_10_BIT); // 10-bit analog-to-digital conversions

#if 0
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

	set_digital_output(FIRE_ALARM_PIN, HIGH);

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
#endif	
	//set baud rate etc.
	serial_hardware_init();

	lcd_init_printf();

	motors_hardware_init();

	//pulse_in_start(pulseInPins, sizeof(pulseInPins));		// start measuring pulses (1 per each sonar, +1 for sound start)

	return 0;
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


unsigned char lidar_rx_buffer[255];
u32 lidar_packets_received=0, lpr=0;
u32 lidar_bytes_received=0, lbr=0;
u32 lidar_packets_per_second=0;
u32 lidar_bytes_per_second=0;

typedef struct
{
	u08 distance_7_0;
	u08 flags_distance_13_8;
	u08 signal_strength_l;
	u08 signal_strength_h;
} t_lidar_data;


typedef struct
{
	u08 start;
	u08 index;
	u08 speed_l;
	u08 speed_h;
	t_lidar_data reading[4];
	u08 checksum_l;
	u08 checksum_h;
} t_lidar_packet;

t_lidar_packet lidar_packet;

u08 lidar_process_byte(u08 data)
{
	enum states { s_none=0, s_waiting_for_sync=1, s_waiting_for_index, s_receiving_data, s_foo };
	static enum states state=s_waiting_for_sync;
	static enum states last_state=s_none;
	static u32 t_entry=0;
	static u32 t_now=0;
	static u08 bytes_received;
	u08 *lidar_packet_ptr = (u08*) &(lidar_packet);
	u08 result=0;

	first_(s_waiting_for_sync)
	{
		enter_(s_waiting_for_sync)  //required!  this updates the "last_state" variable!
		{  
			NOP();
			result = 0;
			bytes_received = 0;
		}

		if(data==0xfa) state=s_waiting_for_index;

		exit_(s_waiting_for_sync)  
		{ 
			NOP();
			lidar_packet_ptr[bytes_received] = data;
			bytes_received++;
		}
	}

	next_(s_waiting_for_index)
	{
		enter_(s_waiting_for_index)  //required!  this updates the "last_state" variable!
		{  
			NOP();
		}

		if( (data>=((u08)0xa0)) && (data<=((u08)0xf9)) ) 
		{
			state = s_receiving_data;
			lidar_packet_ptr[bytes_received] = (data-0xa0);
			bytes_received++;
		}
		else
		{
			state = s_waiting_for_sync;
		}

		exit_(s_waiting_for_index)  
		{ 
			NOP();
		}
	}


	next_(s_receiving_data)
	{
		enter_(s_receiving_data)  //required!  this updates the "last_state" variable!
		{  
			NOP();
		}

		lidar_packet_ptr[bytes_received]=data;
		bytes_received++;
		if(bytes_received==22) state=s_waiting_for_sync;

		exit_(s_receiving_data)  
		{ 
			NOP();
			result = 1;
		}
	}


	return result;
}


void lidar_receive_fsm(u08 cmd, u08 *param)
{
	static uint8 state=0;
	static uint32 count=0;
	static uint8 wr_idx=0,rd_idx=0,r=0;
	static u08 initialized=0;
	static u32 t1;
	uint8 result = 0;
	uint8 packets_received_this_iteration=0;
	u16 d;
	//task_open();

	if(!initialized)
	{
		initialized=1;
		t1=get_ms();
		dbg_printf("lidar_receive_fsm()\n");
		serial_set_baud_rate(UART0,115200);
		memset(lidar_rx_buffer,0,250);
		serial_receive_ring(UART0, (char*) lidar_rx_buffer, 250);
	}

	//while(1)
	{
		//OS_SCHEDULE;
		wr_idx=serial_get_received_bytes(UART0);
		while(wr_idx!=rd_idx)
		{
			lidar_bytes_received++;
			result = lidar_process_byte(lidar_rx_buffer[rd_idx]);
			if(result)
			{
				lidar_packets_received++;
				packets_received_this_iteration++;
				if(packets_received_this_iteration==1)
				{
					s.inputs.lidar.num_samples=0;
					s.inputs.lidar.angle = lidar_packet.index;
				}
				d = (u16)lidar_packet.reading[0].distance_7_0;   d = d | (  ((u16)lidar_packet.reading[0].flags_distance_13_8)<<8);  d = d & 0x3fff;
				s.inputs.lidar.samples[s.inputs.lidar.num_samples] = d;
				s.inputs.lidar.num_samples++;
				d = (u16)lidar_packet.reading[1].distance_7_0;   d = d | (  ((u16)lidar_packet.reading[1].flags_distance_13_8)<<8);  d = d & 0x3fff;
				s.inputs.lidar.samples[s.inputs.lidar.num_samples] = d;
				s.inputs.lidar.num_samples++;
				d = (u16)lidar_packet.reading[2].distance_7_0;   d = d | (  ((u16)lidar_packet.reading[2].flags_distance_13_8)<<8);  d = d & 0x3fff;
				s.inputs.lidar.samples[s.inputs.lidar.num_samples] = d;
				s.inputs.lidar.num_samples++;
				d = (u16)lidar_packet.reading[3].distance_7_0;   d = d | (  ((u16)lidar_packet.reading[3].flags_distance_13_8)<<8);  d = d & 0x3fff;
				s.inputs.lidar.samples[s.inputs.lidar.num_samples] = d;
				s.inputs.lidar.num_samples++;
			}
			rd_idx++;
			if(rd_idx>=250) 
			{
				rd_idx=0; //wrap-around
			}
			wr_idx=serial_get_received_bytes(UART0);
		}

		if(get_ms()-t1 >= 1000)
		{
			t1=get_ms();
			lidar_bytes_per_second=lidar_bytes_received-lbr;
			lbr=lidar_bytes_received;
			lidar_packets_per_second=lidar_packets_received-lpr;
			lpr=lidar_packets_received;
		}
	}

	//task_close();
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
	//ultrasonic_update_fsm(0,0);
	//analog_update_fsm(0,0);  //we don't want this when running on the PC and/or in simulation mode
	//SHARPIR_update_fsm(0,0);
#else	
	{ extern void sim_inputs(void); sim_inputs(); } //on the PC, IR, Sonar and line sensor readings are provided by V-REP
#endif
	odometry_update_fsm(0,0);
	//line_detection_fsm_v2(0,0);


	//behaviors
	//task_run(master_logic_fsm,0,0); //ml fsm makes sleep statements, so it is a task and not just a simply fsm

	//task_run(find_flame_fsm,0,0);
	//wall_follow_fsm(0,0);
	//wall_finding_fsm(0,0);
	//line_alignment_fsm_v2(0,0);

	//task_run(return_home_fsm, 0, 0); //ml fsm makes sleep statements, so it is a task and not just a simply fsm

	//outputs
	motor_command(0,0,0,0,0);
	servo_task(0,0);

	//testing
	//test_fsm(0,0);

	//lidar
#ifndef SVP_ON_WIN32 //doesn't work yet this way in simulation
	lidar_receive_fsm(0,0);
#endif
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
		//the assumption is that the simulation time step is 20ms.
		//hence no need for a sleep statement
		OS_SCHEDULE;
#endif
	}

	task_close();
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


	play_from_program_space(welcome);
	delay_ms(500);

	usb_printf("robot.c::main()\n");


	//--------  initialize misc support libraries  -----------
	cfg_init();  //initialize the configuration parameter module
	LOOKUP_init();
	SHARPIR_init();


	//--------------  misc unit test stuff ----------------
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

	os_init();  //initialize the cooperative multitasking subsystem and start all tasks

	task_create( main_loop_task,			1,  NULL, 0, 0);
	//task_create( find_flame_fsm	,			2,  NULL, 0, 0);
	//task_create( master_logic_fsm,			3,  NULL, 0, 0);
	task_create( lcd_update_fsm,			4,  NULL, 0, 0);
	//task_create( return_home_fsm,			5,  NULL, 0, 0);

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

