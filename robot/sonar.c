

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

	
	
void ultrasonic_update_fsm(uint8 cmd, uint8 *param)
{
	static u08 cfg_idx_bitmap, cfg_idx_echo_timeout, cfg_idx_intra_delay;
	static u08 bitmap;
	static u32 echo_timeout, intra_delay;
	static u08 newpulse=0;
	static u32 t_ping=0;
	static u32 pulse;
	static u16 distance;
	
	task_open();
	
	cfg_idx_bitmap			= cfg_get_index_by_grp_and_id(4,1);
	cfg_idx_echo_timeout	= cfg_get_index_by_grp_and_id(4,2);
	cfg_idx_intra_delay		= cfg_get_index_by_grp_and_id(4,3);
	
	
	usb_printf("ultrasonic_update_fsm()\n");
	
	while(1)
	{
		bitmap			= cfg_get_u08_by_index(cfg_idx_bitmap);
		echo_timeout	= cfg_get_u32_by_index(cfg_idx_echo_timeout);
		intra_delay		= cfg_get_u32_by_index(cfg_idx_intra_delay);
		
		while( (get_ms() - t_ping) < intra_delay )
		{
			//task_wait(1);
			OS_SCHEDULE;
		}

		if(bitmap & 0x01)
		{
			//ping sensor 1
			//usb_printf("\n\nus: 1\n");
			PING(IO_US_ECHO_AND_PING_1);
			newpulse = new_pulse(US_1_PULSE_CHANNEL);
			t_ping = get_ms();
		
			//task_wait(2);
			OS_SCHEDULE;

			//usb_printf("us: 2\n");

			//wait for echo
			newpulse = 0;
			while(  (newpulse==0) && ((get_ms()-t_ping) < echo_timeout) )
			{
				//usb_printf("us: 3\n");
				newpulse = new_high_pulse(US_1_PULSE_CHANNEL);
				if(newpulse)
				{
					//usb_printf("us: 4\n");
					pulse = pulse_to_microseconds(get_last_high_pulse(US_1_PULSE_CHANNEL));
					distance = ((pulse*10)/148) + 2;
					s.inputs.sonar[0] = distance;
					s.us_avg[0] = (s.us_avg[0]*3 + distance)/4;
				}
				//usb_printf("us: 5\n");
				//task_wait(2);
				OS_SCHEDULE;
				//usb_printf("us: 6\n");
			}
			//usb_printf("us: 7\n");
			if(newpulse==0) 	
			{
				//usb_printf("us: 8\n");
				distance = 4000;
				s.inputs.sonar[0] = distance;
				s.us_avg[0] = (s.us_avg[0]*3 + distance)/4;
			}	
		}				

		while( (get_ms() - t_ping < intra_delay) )
		{
			//task_wait(1);
			OS_SCHEDULE;
		}

		if(bitmap & 0x02)
		{
			//ping sensor 2
			PING(IO_US_ECHO_AND_PING_2);
			newpulse = new_pulse(US_2_PULSE_CHANNEL);
			t_ping = get_ms();
		
			//task_wait(2);
			OS_SCHEDULE;

			//wait for echo
			newpulse = 0;
			while(  (newpulse==0) && (get_ms() - t_ping < echo_timeout) )
			{
				newpulse = new_high_pulse(US_2_PULSE_CHANNEL);
				if(newpulse)
				{
					pulse = pulse_to_microseconds(get_last_high_pulse(US_2_PULSE_CHANNEL));
					distance = ((pulse*10)/148) + 2;
					s.inputs.sonar[1] = distance;
					s.us_avg[1] = (s.us_avg[1]*3 + distance)/4;
				}
				//task_wait(2);
				OS_SCHEDULE;
			}
			if(newpulse==0)
			{
				distance = 4000;
				s.inputs.sonar[1] = distance;
				s.us_avg[1] = (s.us_avg[1]*3 + distance)/4;
			}
		}				
	}
	task_close();
}

