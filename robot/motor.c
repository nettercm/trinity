
#include "standard_includes.h"

volatile uint16 l_enc_cp=0,r_enc_cp=0;


//250-290us (for absolute pos only); 450us for the whole thing
void odometry_update(s16 l_ticks, s16 r_ticks, float odo_cml, float odo_cmr, float odo_b)
{
	float d_theta, d_x, d_y, l, r;
	float d_Ul, d_Ur, d_U;

	l = (float)l_ticks;
	r = (float)r_ticks;

	d_Ul = odo_cml * l;
	d_Ur = odo_cmr * r;
	d_U  = (d_Ul + d_Ur) / 2.0f;
	d_theta = (d_Ur - d_Ul) / odo_b;

	//update our absolute position
	d_x = d_U * cos(s.inputs.theta);
	d_y = d_U * sin(s.inputs.theta);
	s.inputs.x  = s.inputs.x + d_x;
	s.inputs.y  = s.inputs.y + d_y;
	s.U			= s.U + d_U;

	//TODO: limit theta to +/- 180 degrees

	s.inputs.theta = s.inputs.theta + d_theta;
	if(s.inputs.theta > 1.0f*PI) s.inputs.theta -= 2.0f*PI;
	//if(s.inputs.theta < -2.0f*PI) s.inputs.theta += 2.0f*PI;
	if(s.inputs.theta < -1.0f*PI) s.inputs.theta += 2.0f*PI;

	//TODO: find a smarter way to do "checkpoints" (actually this way may be better once we start to adjust our absolute position using map matching)
	//update relative position change since the last "checkpoint"
	d_x = d_U * cos(s.dtheta);
	d_y = d_U * sin(s.dtheta);
	s.dx  = s.dx + d_x;
	s.dy  = s.dy + d_y;
	s.dU  = s.dU + d_U;
	s.dtheta = s.dtheta + d_theta;
	if(s.dtheta > 2.0*PI) s.dtheta -= 2.0f*PI;
	if(s.dtheta < -2.0*PI) s.dtheta += 2.0f*PI;
}



//t=theta is in degrees;  a value of NO_CHANGE_IN_POSITION (999999.0) indicates "don't make a change"
void odometry_update_postion(float x, float y, float t)
{
#if 0
	dbg_printf("odo_upd_pos(): x,y,t:  %d=>%d, %d=>%d, %d=>%d\n", 
		(int)(s.inputs.x / 25.4f), (int)x,
		(int)(s.inputs.y / 25.4f), (int)y,
		(int)(s.inputs.theta*K_rad_to_deg), (int)t);
	if(x<NO_CHANGE_IN_POSITION) s.inputs.x = x*25.4f;
	if(y<NO_CHANGE_IN_POSITION) s.inputs.y = y*25.4f;
	if(t<NO_CHANGE_IN_POSITION) s.inputs.theta = t * K_deg_to_rad;
#endif
}




void motors_hardware_init(void)
{
	set_motors(0,0);
	svp_set_mode(SVP_MODE_ENCODERS);
	encoders_reset();
}



void encoders_reset(void)
{
	svp_get_counts_and_reset_ab();
	svp_get_counts_and_reset_cd();
	s.inputs.encoders[0] = 0; //svp_get_counts_ab();
	s.inputs.encoders[1] = 0; //svp_get_counts_cd();
	s.inputs.x = s.inputs.y = s.inputs.theta = s.U = 0.0f;
	s.dx = s.dy = s.dtheta = s.dU = 0.0f;
}



void odometry_set_checkpoint(void)
{
	s.dx = s.dy = s.dtheta = s.dU =  0.0f;
}



//returns the change in orientation in degrees 
float odometry_get_rotation_since_checkpoint(void)
{
	return s.dtheta * K_rad_to_deg; //(s.dtheta/(2*PI))*360.0;
}



float odometry_get_distance_since_checkpoint(void)
{
	return s.dx;
}



void encoders_set_checkpoint()
{
	l_enc_cp = svp_get_counts_ab();
	r_enc_cp = svp_get_counts_cd();
}



sint16 encoders_get_distance_since_checkpoint()
{
	return ( abs(s.inputs.encoders[0] - l_enc_cp) + abs(s.inputs.encoders[1]) - r_enc_cp ) / 2;
}



void motors_reapply_target_speed()
{
	motor_command(2,0,0,s.lm_target,s.rm_target);
}


/*
5	26900
10	21200
20	19100
30	17700
40	17600
50	17500
60	17500
80	16500
100	16400
120	16300

*/
s16 lookup_K(s16 s)
{
	s=abs(s);
	if      (s>=120) return (s16)16300;
	else if (s>=100) return (s16)16400;
	else if (s>= 80) return (s16)16500;
	else if (s>= 60) return (s16)17500;
	else if (s>= 50) return (s16)17500;
	else if (s>= 40) return (s16)17600;
	else if (s>= 30) return (s16)17700;
	else if (s>= 20) return (s16)19100;
	else if (s>= 10) return (s16)21200;
	else             return (s16)26900;
}



int motor_command(unsigned char cmd, uint16 p1, uint16 p2, sint16 lm_speed, sint16 rm_speed)
{
	static uint32 g_delta_T=100;	//static uint8 state = 0; //not turning
	static unsigned long t_stop;
	static sint16 l_enc_delta, r_enc_delta, l_enc_start, r_enc_start;
	static sint16 accelleration, decelleration;
	static uint32 t_now=0,t_last=0;
	static uint8 last_state=0;
	
	t_now = get_ms();
	
	if(cmd==0) //0 means update
	{
		l_enc_delta = svp_get_counts_and_reset_ab();
		r_enc_delta = svp_get_counts_and_reset_cd();
		s.inputs.actual_speed[0] = l_enc_delta;
		s.inputs.actual_speed[1] = r_enc_delta;
		s.inputs.encoders[0] += l_enc_delta;
		s.inputs.encoders[1] += r_enc_delta;
		s.encoder_ticks += (s32)((abs(l_enc_delta) + abs(r_enc_delta))/2);
		
		switch(s.motor_command_state)
		{
			case 0:	//we are not turning or carrying out any specific move/command, so simply re-apply what's consider the current motor speeds i.e. x_actual
				set_motors(s.rm_actual,s.lm_actual);  //TODO: don't call every time to reduce overhead
				s.inputs.motors[0] = s.lm_actual;
				s.inputs.motors[1] = s.rm_actual;
			break; //do nothing
			

			case 1: //currently turning/moving for a specific amount of time; once time is up, re-apply the "target" speed
				if(t_now >= t_stop) //time to stop?
				{
					g_delta_T = t_now-t_stop;
					s.motor_command_state = 0;
					//set_motors(0,0);
					set_motors(s.rm_target,s.lm_target);
					s.inputs.motors[0] = s.lm_actual = s.lm_target;
					s.inputs.motors[1] = s.rm_actual = s.rm_target;
				}
			break;


			case 3: //currently turning/moving for a specific amount of time; once time is up, stop immediately
				if(t_now >= t_stop) //time to stop?
				{
					g_delta_T = t_now-t_stop;
					s.motor_command_state = 0;
					s.lm_target = s.rm_target = 0;
					set_motors(s.rm_target,s.lm_target);
					s.inputs.motors[0] = s.lm_actual = s.lm_target;
					s.inputs.motors[1] = s.rm_actual = s.rm_target;
				}
			break;


			case 4:  //turn/move for a specific number of encoder ticks and then stop both motors as soon as one encoder reached the target
				if( (abs(s.inputs.encoders[0]-l_enc_start) >= l_enc_delta) || (abs(s.inputs.encoders[1]-r_enc_start) >= r_enc_delta) )
				{
					s.motor_command_state = 0;
					s.lm_actual = s.lm_target;
					s.rm_actual = s.rm_target;
					set_motors(s.rm_actual,s.lm_actual);
				}
				s.inputs.motors[0] = s.lm_actual;
				s.inputs.motors[1] = s.rm_actual;
			break;


			case 5:  //turn/move for a specific number of encoder ticks and then stop each motor individually
				if (abs(s.inputs.encoders[0]-l_enc_start) >= l_enc_delta)
				{
					s.lm_actual = s.lm_target;
					set_motors(s.rm_actual,s.lm_actual);
				}
				if (abs(s.inputs.encoders[1]-r_enc_start) >= r_enc_delta)
				{
					s.rm_actual = s.rm_target;
					set_motors(s.rm_actual,s.lm_actual);
				}
				if( (abs(s.inputs.encoders[0]-l_enc_start) >= l_enc_delta) && (abs(s.inputs.encoders[1]-r_enc_start) >= r_enc_delta) )
				{
					s.motor_command_state = 0;
					s.lm_actual = s.lm_target;
					s.rm_actual = s.rm_target;
					set_motors(s.rm_actual,s.lm_actual);
				}
				s.inputs.motors[0] = s.lm_actual;
				s.inputs.motors[1] = s.rm_actual;
			break;
			

			case 6:  //regulate speed - use l/r motor speed as target  (same as mode/cmd 7, but different starting conditiosn)
				if(l_enc_delta > s.lm_target+0) s.lm_actual-=decelleration;
				if(l_enc_delta < s.lm_target-0) s.lm_actual+=accelleration;
				if(r_enc_delta > s.rm_target+0) s.rm_actual-=decelleration;
				if(r_enc_delta < s.rm_target-0) s.rm_actual+=accelleration;
				LIMIT(s.lm_actual,-255,+255);
				LIMIT(s.rm_actual,-255,+255);
				if( (s.lm_target==0) && (s.lm_actual>-10) && (s.lm_actual<10) ) s.lm_actual = 0;
				if( (s.rm_target==0) && (s.rm_actual>-10) && (s.rm_actual<10) ) s.rm_actual = 0;
				s.inputs.motors[0] = s.lm_actual;
				s.inputs.motors[1] = s.rm_actual;
				set_motors(s.rm_actual,s.lm_actual);
			break;
			

			case 7:  //regulate speed - use l/r motor speed as target
				if(l_enc_delta > s.lm_target+0) s.lm_actual--;
				if(l_enc_delta < s.lm_target-0) s.lm_actual++;
				if(r_enc_delta > s.rm_target+0) s.rm_actual--;
				if(r_enc_delta < s.rm_target-0) s.rm_actual++;
				LIMIT(s.lm_actual,-255,+255);
				LIMIT(s.rm_actual,-255,+255);
				if( (s.lm_target==0) && (s.lm_actual>-10) && (s.lm_actual<10) ) s.lm_actual = 0;
				if( (s.rm_target==0) && (s.rm_actual>-10) && (s.rm_actual<10) ) s.rm_actual = 0;
				s.inputs.motors[0] = s.lm_actual;
				s.inputs.motors[1] = s.rm_actual;
				set_motors(s.rm_actual,s.lm_actual);
			break;
			

			case 8: //same as 7, just don't regulate
				LIMIT(s.lm_actual,-255,+255);
				LIMIT(s.rm_actual,-255,+255);
				if( (s.lm_target==0) && (s.lm_actual>-10) && (s.lm_actual<10) ) s.lm_actual = 0;
				if( (s.rm_target==0) && (s.rm_actual>-10) && (s.rm_actual<10) ) s.rm_actual = 0;
				s.inputs.motors[0] = s.lm_actual;
				s.inputs.motors[1] = s.rm_actual;
				set_motors(s.rm_actual,s.lm_actual);
			break;


			case 10:  //regulate the difference in l/r speed
				if( (l_enc_delta - r_enc_delta) > l_enc_start) { s.lm_actual--; s.rm_actual++; }
				if( (l_enc_delta - r_enc_delta) < l_enc_start) { s.lm_actual++; s.rm_actual--; }
				s.inputs.motors[0] = s.lm_actual;
				s.inputs.motors[1] = s.rm_actual;
				set_motors(s.rm_actual,s.lm_actual);
			break;
			

			case 11:  //regulate the difference in l/r speed
				s.lm_actual = s.lm_target;
				s.rm_actual = s.rm_target;
				if( s.inputs.encoders[0] - s.inputs.encoders[1] > 1) { s.lm_actual = (9*s.lm_target)/10; }
				if( s.inputs.encoders[0] - s.inputs.encoders[1] < -1) { s.rm_actual = (9*s.rm_target)/10; }
				s.inputs.motors[0] = s.lm_actual;
				s.inputs.motors[1] = s.rm_actual;
				set_motors(s.rm_actual,s.lm_actual);
			break;
		}
	}
	else //cmd != 0 means we just got a new motor command that we need to start executing
	{
		if(cmd==1) //turn/move for a specific amount of time at the a specific speed
		{
			//start
			s.motor_command_state = 1;
			set_motors(rm_speed,lm_speed);
			s.inputs.motors[0] = s.lm_actual = lm_speed;
			s.inputs.motors[1] = s.rm_actual = rm_speed;
			t_stop = t_now + (unsigned long)p1;
		}


		if(cmd==2) //stop any currently running motor command and immediately apply the new target speed
		{
			s.motor_command_state = 0;
			set_motors(rm_speed,lm_speed);
			s.inputs.motors[0] = lm_speed;
			s.inputs.motors[1] = rm_speed;
			s.lm_target = s.lm_actual = lm_speed;
			s.rm_target = s.rm_actual = rm_speed;
		}


		if(cmd==3) //turn/move for a specific amount of time at the a specific speed; when done, stop
		{
			//start
			s.motor_command_state = 3;
			set_motors(rm_speed,lm_speed);
			s.inputs.motors[0] = s.lm_actual = lm_speed;
			s.inputs.motors[1] = s.rm_actual = rm_speed;
			t_stop = t_now + (unsigned long)p1;
		}


		if(cmd==4) //turn/move for a specific number of encoder ticks and then stop each motor individual
		{
			//start
			s.motor_command_state = 4;
			set_motors(rm_speed,lm_speed);
			s.inputs.motors[0] = s.lm_actual = lm_speed;
			s.inputs.motors[1] = s.rm_actual = rm_speed;
			l_enc_delta = p1;
			r_enc_delta = p2;
			l_enc_start = s.inputs.encoders[0]; // = svp_get_counts_ab();;
			r_enc_start = s.inputs.encoders[1]; // = svp_get_counts_cd();;
		}
		

		if(cmd==5) //turn/move for a specific number of encoder ticks and then go back to target speed
		{
			//start
			s.motor_command_state = 5;
			set_motors(rm_speed,lm_speed);
			s.inputs.motors[0] = s.lm_actual = lm_speed;
			s.inputs.motors[1] = s.rm_actual = rm_speed;
			l_enc_delta = p1;
			r_enc_delta = p2;
			l_enc_start = s.inputs.encoders[0]; // = svp_get_counts_ab();;
			r_enc_start = s.inputs.encoders[1]; // = svp_get_counts_cd();;
		}
		

		if(cmd==6) //regulate speed
		{
			accelleration = p1;  if(accelleration==0) accelleration=1;
			decelleration = p2;  if(decelleration==0) decelleration=1;

			//limit target speed to +/- MAX_SPEED
			LIMIT(lm_speed,-MAX_SPEED,+MAX_SPEED);
			LIMIT(rm_speed,-MAX_SPEED,+MAX_SPEED);
			//start
			if(s.motor_command_state == 6)
			{
				s.lm_target = lm_speed;
				s.rm_target = rm_speed;
			}
			else
			{
				s.motor_command_state = 6;
				//don't apply the target speed - let the regulator gradually ramp up/down as required for smooth movements
				//set_motors(rm_speed,lm_speed);
				//s.inputs.motors[0] = s.lm_actual = lm_speed;
				//s.inputs.motors[1] = s.rm_actual = rm_speed;
				s.lm_target = lm_speed;
				s.rm_target = rm_speed;
			}
			t_last=t_now;
		}
		

		if(cmd==7) //use feed-forward to compensate for battery voltage fluctuations and then regulate speed
		{
			//limit target speed to +/- MAX_SPEED
			LIMIT(lm_speed,-MAX_SPEED,+MAX_SPEED);
			LIMIT(rm_speed,-MAX_SPEED,+MAX_SPEED);
			
			if(s.motor_command_state == 7)
			{
				//if we are already in this mode and adjusting the speed by a large amount, 
				//then do feed-forward control, i.e. apply the new pwm value right away in set_motors() below
				if(abs(lm_speed - s.lm_target)>5) s.lm_actual = (lm_speed * ((s16)lookup_K(lm_speed)/((s16)s.inputs.vbatt/(s16)100)))/100;
				if(abs(rm_speed - s.rm_target)>5) s.rm_actual = (rm_speed * ((s16)lookup_K(rm_speed)/((s16)s.inputs.vbatt/(s16)100)))/100;
			}
			else
			{
				s.lm_actual = (lm_speed * ((s16)lookup_K(lm_speed)/((s16)s.inputs.vbatt/(s16)100)))/100;
				s.rm_actual = (rm_speed * ((s16)lookup_K(rm_speed)/((s16)s.inputs.vbatt/(s16)100)))/100;
			}
			s.lm_target = lm_speed;
			s.rm_target = rm_speed;
			s.motor_command_state = 7;
			LIMIT(s.lm_actual,-255,+255);
			LIMIT(s.rm_actual,-255,+255);
			s.inputs.motors[0] = s.lm_actual; // = (lm_speed * 175)/100;
			s.inputs.motors[1] = s.rm_actual; // = (rm_speed * 175)/100;
			set_motors(s.rm_actual,s.lm_actual);
			t_last=t_now;
		}
		

		if(cmd==8) //use feed-forward to compensate for battery voltage fluctuations, but don't regulate speed
		{
			//limit target speed to +/- MAX_SPEED
			LIMIT(lm_speed,-MAX_SPEED,+MAX_SPEED);
			LIMIT(rm_speed,-MAX_SPEED,+MAX_SPEED);
			
			//pwm = target * K/vbatt
			//K = (pwm * vbatt)/target
			s.lm_actual = (lm_speed * ((s16)lookup_K(lm_speed)/((s16)s.inputs.vbatt/(s16)100)))/100;
			s.rm_actual = (rm_speed * ((s16)lookup_K(rm_speed)/((s16)s.inputs.vbatt/(s16)100)))/100;

			s.lm_target = lm_speed;
			s.rm_target = rm_speed;
			
			s.motor_command_state = 8;
			LIMIT(s.lm_actual,-255,+255);
			LIMIT(s.rm_actual,-255,+255);
			s.inputs.motors[0] = s.lm_actual; // = (lm_speed * 175)/100;
			s.inputs.motors[1] = s.rm_actual; // = (rm_speed * 175)/100;
			set_motors(s.rm_actual,s.lm_actual);
			t_last=t_now;
		}


		if(cmd==10) //regulate the difference in l/r speed (method 1)
		{
			//start
			s.motor_command_state = 10;
			set_motors(rm_speed,lm_speed);
			s.inputs.motors[0] = s.lm_actual = s.lm_target = lm_speed;
			s.inputs.motors[1] = s.rm_actual = s.rm_target = rm_speed;
			l_enc_start = p1;
			t_last=t_now;
		}
		

		if(cmd==11) //regulate the difference in l/r speed (method 2)
		{
			//start
			s.motor_command_state = 11;
			set_motors(rm_speed,lm_speed);
			s.inputs.motors[0] = s.lm_actual = s.lm_target = lm_speed;
			s.inputs.motors[1] = s.rm_actual = s.rm_target = rm_speed;
			l_enc_start = p1;
			t_last=t_now;
		}

	}
	
	if( (s.motor_command_state != last_state) ) //|| (cmd != 0) )
	{
		//the intent here is to only do a printf if something changes and not for udpates, but tasks like "wall following" will continuously issue 'new' motor commands
		//usb_printf("%08ld: motor_command(cmd=%d,%d,%d,lm=%d,rm=%d) lst,st=%d,%d\r\n", t_now, cmd, p1, p2, lm_speed, rm_speed, last_state,s.motor_command_state);
		last_state = s.motor_command_state;
	}

	//TODO: eliminate the duplication between s.inputs and other parts of the state struct s
	s.inputs.target_speed[0] = s.lm_target;
	s.inputs.target_speed[1] = s.rm_target;
	
	return s.motor_command_state;
}


void motor_command_fsm(u08 cmd, u08 *param)
{
	DEFINE_CFG(u08,update_rate,3,1);
	DEFINE_CFG(flt,odo_cml,5,1);
	DEFINE_CFG(flt,odo_cmr,5,2);
	DEFINE_CFG(flt,odo_b,5,3);
	static u08 initialized=0;
		
	//task_open();

	if(!initialized)
	{
		initialized=1;
		
		usb_printf("motor_command_fsm()\n");
	
		PREPARE_CFG(u08,update_rate);
		PREPARE_CFG(flt,odo_cml);
		PREPARE_CFG(flt,odo_cmr);
		PREPARE_CFG(flt,odo_b);
	}

	//while(1)
	{
		UPDATE_CFG(u08,update_rate);
		UPDATE_CFG(flt,odo_cml);
		UPDATE_CFG(flt,odo_cmr);
		UPDATE_CFG(flt,odo_b);
		motor_command(0,0,0,0,0);
		odometry_update(s.inputs.actual_speed[0], s.inputs.actual_speed[1], odo_cml, odo_cmr, odo_b);
		//task_wait(update_rate); //OS_SCHEDULE;
	}
	
	//task_close();
}


void motors_stop(void)
{
	motor_command(2,0,0,0,0);
}

void motors_set(sint16 lm_speed, sint16 rm_speed)
{
	motor_command(2,0,0,lm_speed,rm_speed);
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
uint8 encoders_update_fsm(uint32 event)
{
	static uint8 state=0;
	static uint32 t_last=0, t_now=0;
	uint8 result=0;  //return value of 0 means we did not transition state;

	if(event == FSM_INIT_EVENT)
	{
		result = 0;
		t_now  = 0;
		t_last = get_ms();
		state  = 1;
	}
	else
	{
		switch (state)
		{
			case 1: //first reading
			t_now = get_ms();
			if( (t_now - t_last) < 5) break;
			t_last = t_now;
			state = 2;
			result = state;
			break;

			case 2: //subsequent reading
			t_now = get_ms();
			if( (t_now - t_last) < 5) break;
			s.inputs.encoders[0] = svp_get_counts_ab();
			s.inputs.encoders[1] = svp_get_counts_cd();
			state = 2;
			result = state;
			t_last = t_now;
			break;

			default:
			break;
		}
	}
	return result;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if 0
void encoders_update(void)
{
	uint8 p;
	static uint32 pulse=0;
	p=new_pulse(ENC_LEFT_PULSE_CHANNEL);
	if(p & HIGH_PULSE)
	{
		pulse = get_last_high_pulse(ENC_LEFT_PULSE_CHANNEL);
		pulse=pulse_to_microseconds(pulse);
		pulse=pulse/100;
		s.inputs.p1 = (s.inputs.p1 + pulse)/2;
	}
	
	if(p & LOW_PULSE)
	{
		pulse = get_last_low_pulse(ENC_LEFT_PULSE_CHANNEL);
		pulse=pulse_to_microseconds(pulse);
		pulse=pulse/100;
		s.inputs.p2 = (s.inputs.p2 + pulse)/2;
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if 0
void encoder_test(void)
{
	while(1)
	{
		uint32 t1,t2;
		int i;
		volatile int l=0,r=0;
		
		t1 = get_ticks();
		for(i=0;i<1000;i++) //takes about 18ms;  library updates encoder data only once per every ms
		{
			l = l + svp_get_counts_and_reset_ab();
			r = r + svp_get_counts_and_reset_cd();
		}
		t2 = get_ticks();
		clear();
		lcd_goto_xy(0,0); printf("t = %lu",t2-t1);
		lcd_goto_xy(0,1); printf("l = %d,  r = %d",l,r);
	}
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
