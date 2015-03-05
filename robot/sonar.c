

#include "standard_includes.h"



/*
0 = NE
1 = N
2 = NW
3 = E
4 = W
5 = SE
6 = SW
*/
//unsigned char us_sequence[] = {12,4,1,4,2,4,1,4,0,4,1,4,3}; //W=priority 1,  N=priority 2
//unsigned char us_sequence[] = {16,4,1,6,2,4,1,6,0,4,1,6,3,4,1,6,5}; //W=priority 1,  N=priority 2
volatile uint8 us_sequence[] = {7,0,1,2,3,4,5,6}; //W=priority 1,  N=priority 2
volatile uint8 us_timeout = 40;

uint8 us_sequence_uniform[] = {7, US_NE, US_N, US_NW, US_E, US_W, US_SE, US_SW};
uint8 us_sequence_W_priority[] = {7, US_SW, US_N, US_W, US_NE, US_SW, US_NW, US_W};
uint8 us_sequence_N_priority[] = {3, US_N, US_NW, US_NE };
uint8 us_sequence_E_SE_NE[] = {2, US_E, US_SE };

void ultrasonic_hardware_init(void)
{
}

void ultrasonic_set_focus(uint8 sensor_id)
{
	us_sequence[0] = 1;
	us_sequence[1] = sensor_id;
}

void ultrasonic_set_sequence(uint8 *new_sequence)
{
	int i;

	for(i=0; i<= new_sequence[0]; i++)
	{
		us_sequence[i] = new_sequence[i];
	}
	s.us_cycles=0;
}

void ultrasonic_set_timeout(uint8 new_timeout)
{
	us_timeout = new_timeout;
}

void set_ultrasonic_mux(uint8 addr)
{

}


#define PING(pin) \
		set_digital_output(pin, LOW); \
		delay_us(20); \
		set_digital_output(pin, HIGH); \
		delay_us(10); \
		set_digital_output(pin, LOW); \
		delay_us(20); \
		set_digital_input(pin,HIGH_IMPEDANCE)



/*

note:  sound travelles about 6.6m in 20ms, so 3.3m in one direction

time since last ping (any sensor) - should wait at least 20ms between pings
time since last ping (this sensor) - should wait at least 40ms between pings of the same sensor

sonar states:

00ms:  ping #1.
20ms:  ping #2.   check for echo on #1
40ms:  ping #3.   
60ms:  ping #4.
80ms:  ping #5.

idle - waiting before issuing next ping
ping issued - waitinf for echo

*/

const u08 sonar_pin[] = { IO_US_ECHO_AND_PING_1, IO_US_ECHO_AND_PING_2, IO_US_ECHO_AND_PING_3, IO_US_ECHO_AND_PING_4, IO_US_ECHO_AND_PING_5 };

u08 get_next_sensor(void)
{
	static u08 sensor=0;

	sensor++;
	if(sensor > 4) sensor = 0;
	return sensor;
}
	
void ultrasonic_update_fsm(uint8 cmd, uint8 *param)
{
	enum states 
	{ 
		s_waiting_for_ping=0,	//0 
		s_waiting_for_echo,		//1
		s_none=255
	};
	static enum states state=s_waiting_for_ping;
	static enum states last_state=s_none;
	static u32 t_entry=0;
	static u08 initialized=0;
	static u08 newpulse=0;
	static u32 t_ping=0;
	static u32 pulse;
	static u16 distance;
	static u08 sensor=0;
	DEFINE_CFG2(u08,bitmap,4,1);
	DEFINE_CFG2(u32,echo_timeout,4,2);
	DEFINE_CFG2(u32,intra_delay,4,3);
	

	//task_open();

	if(!initialized)
	{
		initialized=1;

		usb_printf("ultrasonic_update_fsm()\n");

		PREPARE_CFG2(bitmap);					
		PREPARE_CFG2(echo_timeout);					
		PREPARE_CFG2(intra_delay);					
	}

	//while(1)
	{
		UPDATE_CFG2(bitmap);					
		UPDATE_CFG2(echo_timeout);					
		UPDATE_CFG2(intra_delay);					


		first_(s_waiting_for_ping)
		{
			enter_(s_waiting_for_ping) 
			{  
				NOP();
			}
			
			if(get_ms() - t_ping >= intra_delay)
			{
				PING(sonar_pin[sensor]);
				newpulse = new_pulse(sensor); //clear the pulse capture state
				t_ping = get_ms();
				state = s_waiting_for_echo;
			}

			exit_(s_waiting_for_ping)  
			{ 
				NOP();
			}
		}

		next_(s_waiting_for_echo)
		{
			enter_(s_waiting_for_echo) 
			{  
				NOP();
			}

			newpulse = new_high_pulse(sensor);
			if(newpulse)
			{
				pulse = pulse_to_microseconds(get_last_high_pulse(sensor));
				distance = ((pulse*10)/148) + 2;
				state = s_waiting_for_ping;
			}
			else if( get_ms()-t_ping >= echo_timeout)
			{
				distance = 4000;
				state = s_waiting_for_ping;
			}

			exit_(s_waiting_for_echo)  
			{ 
				s.inputs.sonar[sensor] = distance;
				s.us_avg[sensor] = (s.us_avg[sensor]*3 + distance)/4;

				sensor=get_next_sensor();
				if(get_ms() - t_ping >= intra_delay)
				{
					PING(sonar_pin[sensor]);
					newpulse = new_pulse(sensor);
					t_ping = get_ms();
					state = s_waiting_for_echo;
				}
			}
		}
	}
}

