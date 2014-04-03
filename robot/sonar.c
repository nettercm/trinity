

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
	set_digital_output(IO_US_ECHO_AND_PING_1, LOW);
	//set_digital_output(IO_US_ECHO_AND_PING_2, LOW);
	set_digital_input(IO_US_ECHO_AND_PING_1,HIGH_IMPEDANCE);
	//set_digital_input(IO_US_ECHO_AND_PING_2,HIGH_IMPEDANCE);
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
	/*
	if(addr>0) set_digital_output(IO_A7, HIGH);
	else set_digital_output(IO_A7, LOW);
	*/
	set_digital_output(IO_US_MUX_0, addr & 0x01);
	set_digital_output(IO_US_MUX_1, addr & 0x02);
	set_digital_output(IO_US_MUX_2, addr & 0x04);
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
			task_wait(1);
		}

		if(bitmap & 0x01)
		{
			//ping sensor 1
			//usb_printf("\n\nus: 1\n");
			PING(IO_US_ECHO_AND_PING_1);
			newpulse = new_pulse(US_1_PULSE_CHANNEL);
			t_ping = get_ms();
		
			task_wait(2);

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
				task_wait(2);
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
			task_wait(1);
		}

		if(bitmap & 0x02)
		{
			//ping sensor 2
			PING(IO_US_ECHO_AND_PING_2);
			newpulse = new_pulse(US_2_PULSE_CHANNEL);
			t_ping = get_ms();
		
			task_wait(2);

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
				task_wait(2);
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



#if 0
uint8 ultrasonic_update_fsm(uint32 event)
{
	struct PulseInputStruct pulse_info;
	static uint8 state=0;
	static uint8 addr=1;
	static uint32 t_last=0, t_ping=0;
	uint8 result = 255;
	unsigned char newpulse = 0;
	uint32 pulse = 0, min=0, max=0;
	uint32 pulses[8];
	int i = 0, j=0;
	uint16 distance;

	if(event == FSM_INIT_EVENT)
	{
		set_ultrasonic_mux(us_sequence[1]);
		set_digital_output( IO_US_PING, LOW);
		//pulse_in_start(pulseInPins, 1);		// start measuring pulses on PD0
		get_pulse_info(US_PULSE_CHANNEL, &pulse_info);  // get pulse info for D0
		state  = 1;
		result = 255;
	}
	else
	{
		switch(state)
		{
		case 1:
			/*
			addr++; if(addr>us_sequence[0]) { addr=1; s.us_cycles++; }				
			*/
			set_ultrasonic_mux(us_sequence[addr]);
			t_ping = get_ms();
			//pulse_in_stop();
			//pulse_in_start(pulseInPins, 1);		// start measuring pulses on PD0
			//get_pulse_info(US_PULSE_CHANNEL, &pulse_info);  // get pulse info for D0
			newpulse = new_pulse(US_PULSE_CHANNEL);
			set_digital_output(IO_US_PING, HIGH);
			delay_us(10);
			set_digital_output(IO_US_PING, LOW);
			state = 2;
			result = 255;
			break;
		case 2:
			newpulse = 0;
			if(get_ms() - t_ping >= US_TIMEOUT)
			{
				distance = 4000; //200 inches
				s.inputs.sonar[us_sequence[addr]] = distance;
				//s.inputs.sonar[us_sequence[addr]] = (s.inputs.sonar[us_sequence[addr]] + distance) / 2;
				s.us_avg[us_sequence[addr]] = (s.us_avg[us_sequence[addr]]*3 + distance)/4;
				state = 3;
				result = 255;
				addr++; if(addr>us_sequence[0]) { addr=1; s.us_cycles++; }
				break;
			}
			newpulse = new_high_pulse(US_PULSE_CHANNEL);
			if(newpulse) 
			{
				pulse = pulse_to_microseconds(get_last_high_pulse(US_PULSE_CHANNEL));
				distance = ((pulse*10)/148) + 2;
				s.inputs.sonar[us_sequence[addr]] = distance;
				//s.inputs.sonar[us_sequence[addr]] = (s.inputs.sonar[us_sequence[addr]] + distance) / 2;
				s.us_avg[us_sequence[addr]] = (s.us_avg[us_sequence[addr]]*3 + distance)/4;
				state = 3;
				result = us_sequence[addr];
				addr++; if(addr>us_sequence[0]) { addr=1; s.us_cycles++; }
			}
			break;
		case 3:
			if(get_ms() - t_ping >= US_TIMEOUT) {state = 1; result = -1; }
			break;
		}
	}

	return result;
}
#endif

/*
int servo_update_fsm(uint32 event)
{
	const unsigned char pulseInPins[] = { IO_D1 };
	static struct PulseInputStruct pulse_info;
	static uint8 state=0;
	static uint32 t_servo=0, t_ping=0;
	uint8 result = 0;
	const int target[4]    = {1150,1350,1550,1350};
	const int sensor_id[4] = {   2,   1,   0,   1};
	//const int target[4]    = {1150,1150,1550,1550};
	//const int sensor_id[4] = {   0,   0,   1,   1};
	static int i = 0;
	unsigned char new_pulse = 0;
	uint32 pulse = 0, min=0, max=0;
	uint16 distance;

	if(event == FSM_INIT_EVENT)
	{
		set_digital_output(IO_D0, LOW);
		pulse_in_start(pulseInPins, 1);		// start measuring pulses on PD0
		get_pulse_info(0, &pulse_info);  // get pulse info for D0
		t_servo = get_ms();
		i=0;
		set_servo_target(0,target[i]);
		state  = 1;
		result = 0;
	}
	else
	{
		switch(state)
		{
		case 1: //initiate ping 100ms after servo was moved
			if(get_ms() - t_servo > 250)
			{
				//start ping
				set_digital_output(IO_D0, HIGH);
				delay_us(10);
				set_digital_output(IO_D0, LOW);
				t_ping = get_ms();
				state = 2;
			}
			break;

		case 2: //wait for echo - up to 50ms
			new_pulse = 0;
			if(get_ms() - t_ping >= 50) //if it's been 50ms since the ping, give up
			{
				s.inputs.sonar[sensor_id[i]] = 0; //200 inches
				result = state = 3;
				break;
			}
			new_pulse = new_high_pulse(0);
			if(new_pulse) 
			{
				pulse = pulse_to_microseconds(get_last_high_pulse(0));
				distance = ((pulse*10)/148) + 2;
				if(distance > 4000) distance = 0;
				s.inputs.sonar[sensor_id[i]] = distance;
				result = state = 3;
			}
			break;

		case 3: //wait for 200ms cycle to complete, then move servo and reset the timer
			if(get_ms() - t_servo > 300)
			{
				t_servo = get_ms();
				i++;
				if(i>3) i = 0;
				set_servo_target(0,target[i]);
				state = 1;
			}
			break;
		}
	}

	return result;
}
*/



int ultrasonic_test()
{
const unsigned char pulseInPins[] = { IO_US_ECHO };
static struct PulseInputStruct pulse_info;
static uint32 t_last=0, t_now=0;
unsigned char new_pulse;
uint32 pulse = 0, min=0, max=0;
uint32 pulses[8];
int i = 0, j=0;

set_digital_output(IO_US_PING, LOW);
pulse_in_start(pulseInPins, 1);		// start measuring pulses on PD0
get_pulse_info(0, &pulse_info);  // get pulse info for D0

while(1)
{
	t_now = get_ms();

	set_digital_output(IO_US_PING, HIGH);
	delay_us(10);
	set_digital_output(IO_US_PING, LOW);
	
	new_pulse = 0;
	while( get_ms()-t_now < 20 )
	{
		new_pulse = new_high_pulse(0);
		if(new_pulse) break;
	}

	if(new_pulse)
	{
		pulse = pulse_to_microseconds(get_last_high_pulse(0));
	}
	else
	{
		pulse = 200*148;
	}
	pulses[i] = pulse;
	i++;
	if(i>=8) i=0;
	
	clear();
	lcd_goto_xy(0,0);
	//print_long(pulse);
	printf("%7ld %7ld",pulse,(pulse*10)/148+2);
	lcd_goto_xy(0,1);

	pulse = 0;
	min = 0xffffffff;
	max = 0;
	for(j=0;j<8;j++)
	{
		pulse+=pulses[j];
		if(pulses[j] < min) min=pulses[j];
		if(pulses[j] > max) max=pulses[j];
	}
	printf("%7ld %7ld",pulse/8, max-min);
	//5" == 766
	//4" == 614
	//3" == 415
	//2" == 302
	//1" == 198

	while( get_ms()-t_now < 25);  //wait at least 25ms per cycle
}

return 0;
}

/*
//ping 2 sensors at the same time
int dual_ultrasonic_update_fsm(uint32 event)
{
	const unsigned char pulseInPins[] = { IO_B4 , IO_D1 };
	static struct PulseInputStruct pulse_info;
	static uint8 state=0;
	static uint32 t_ping=0, t_now=0;
	uint8 result = 0;
	static unsigned char new_pulse_0 = 0, new_pulse_1 = 0;
	uint32 pulse = 0, min=0, max=0;
	uint32 pulses[8];
	int i = 0, j=0;
	uint16 distance;

	t_now = get_ms();

	if(event == FSM_INIT_EVENT)
	{
		set_digital_output( IO_B3, LOW);
		set_digital_output( IO_D0, LOW);
		pulse_in_start(pulseInPins, 2);		// start measuring pulses on PD0
		get_pulse_info(0, &pulse_info);  // get pulse info for D0
		state  = 1;
		result = 0;
	}
	else
	{
		switch(state)
		{
		case 1:
			t_ping = get_ms();
			set_digital_output(IO_B3, HIGH);
			set_digital_output(IO_D0, HIGH);
			delay_us(10);
			set_digital_output(IO_B3, LOW);
			set_digital_output(IO_D0, LOW);
			result = state = 2;
			new_pulse_0 = new_pulse_1 = 0;
			break;
		case 2:
			if(new_pulse_0 == 0) //have not yet gotton an echo on sonar 1
			{
				if(t_now - t_ping >= 50) //if it's been too long, give up on sonar 1
				{
					distance = 4000; //200 inches
					s.inputs.sonar[0] = distance;
				}
				new_pulse_0 = new_high_pulse(0);
				if(new_pulse_1) 
				{
					pulse = pulse_to_microseconds(get_last_high_pulse(0));
					distance = ((pulse*10)/148) + 2;
					s.inputs.sonar[0] = distance;
				}
			}
			if(new_pulse_1 == 0) //have not yet gotton an echo on sonar 1
			{
				if(t_now - t_ping >= 50) //if it's been too long, give up on sonar 1
				{
					distance = 4000; //200 inches
					s.inputs.sonar[1] = distance;
				}
				new_pulse_1 = new_high_pulse(1);
				if(new_pulse_1) 
				{
					pulse = pulse_to_microseconds(get_last_high_pulse(1));
					distance = ((pulse*10)/148) + 2;
					s.inputs.sonar[1] = distance;
				}
			}
			if(new_pulse_0 && new_pulse_1) result = state = 3; //if we got a ping on both, go to next state
			if(t_now - t_ping >= 50) result = state = 3; 
			break;
		case 3:
			if(t_now - t_ping >= 50) result = state = 1;
			break;
		}
	}

	return result;
}
*/
