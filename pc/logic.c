#include <stdio.h>
#include <conio.h>

#include "messages.h"
#include "serial.h"
#include "debug.h"
#include "kalman.h"
#include "utils.h"
#include "logic.h"
#include "data.h"
#include "../robot/config.h"
#include "odometry.h"
#include "commands.h"

static sint16 m_r_speed=0, m_l_speed=0;
static sint16 m_r_delta=5 ,m_l_delta=5;
static sint16 m_r_min=5, m_l_min=5, m_r_max=260, m_l_max=260;
static sint16 m_r_target=0, m_l_target=0;
static sint16 m_ramp=5;


volatile unsigned int key=0;

void process_user_input(void)
{
	static int turn_direction=0;
	static int turn_count=0;
	static int turn_angle=0;
	static int safety=0;
	static int wall_track_state=0;
	static int wander_state=0;
	static int shift_left_state=0;
	static t_config_value v = {0};
	static sint16 l=0, r=0;
	int ml_bias=0;
	unsigned int c=0;
	int update_required = 0;

	//outputs->cmd = 0;


#ifdef WIN32UI
	if(key) 
	{
		c=key; 
		key=0;
		//log_printf("c = '%c' = 0x%04x\n",c,c);
	}
#endif

	//this works in a UI app as well, provided AllocConsole() was called on startup
	if(_kbhit())
	{
		c = _getch();
		if(c==0x00) c=_getch() + 0xe100;
		if(c==0xe0) c=_getch() + 0xe000;
		//log_printf("c = '%c' = 0x%04x\n",c,c);
	}
//#endif

	update_required=1;

	switch(c)
	{

	case 0x1b:				flag_set(FLAG_EXIT);									break; //exit
	case 'x':				odometry_reset(); CMD_reset_encoders();					break;
	case 's':				CMD_sonar_set_timeout(20);								break;
	case 'S':				CMD_sonar_set_timeout(40);								break;
	case '.':				safety = !safety;										break;
	case 't':				wall_track_state = ! wall_track_state;					break;//track the wall
	case 'w':				wander_state = ! wander_state;							break;	//wander around while avoiding stuff
		
		
	case KEY_F1:			CMD_set_behavior_state(1,0); 							break; //start behavior 1
	case KEY_SHIFT_F1:		CMD_set_behavior_state(1,1); 							break; //stop behavior 1
	case KEY_F2:			CMD_set_behavior_state(2,0); 							break; //start behavior 1
	case KEY_SHIFT_F2:		CMD_set_behavior_state(2,1); 							break; //stop behavior 1
	case KEY_F3:			CMD_set_behavior_state(3,0); 							break; //start behavior 1
	case KEY_SHIFT_F3:		CMD_set_behavior_state(3,1); 							break; //stop behavior 1
	case KEY_F4:			CMD_set_behavior_state(4,0); 							break; //start behavior 1
	case KEY_SHIFT_F4:		CMD_set_behavior_state(4,1); 							break; //stop behavior 1
	case KEY_F5:			CMD_set_behavior_state(5,0); 							break; //start behavior 1
	case KEY_SHIFT_F5:		CMD_set_behavior_state(5,1); 							break; //stop behavior 1
	case KEY_F6:			CMD_set_behavior_state(6,0); 							break; //start behavior 1
	case KEY_SHIFT_F6:		CMD_set_behavior_state(6,1); 							break; //stop behavior 1
	case KEY_F7:			CMD_set_behavior_state(7,0); 							break; //start behavior 1
	case KEY_SHIFT_F7:		CMD_set_behavior_state(7,1); 							break; //stop behavior 1
	case KEY_F8:			CMD_set_behavior_state(8,0); 							break; //start behavior 1
	case KEY_SHIFT_F8:		CMD_set_behavior_state(8,1); 							break; //stop behavior 1
	case KEY_F11:			CMD_set_behavior_state(11,0); 							break; //start behavior 1
	case KEY_SHIFT_F11:		CMD_set_behavior_state(11,1); 							break; //stop behavior 1


	//case 'y':				CMD_motor_command(1,25,2*m_l_min,2*m_r_min);			break;//go forward by the smallest amount possible
	case 'y':				CMD_motor_command(5,1,1,6*m_l_min,6*m_r_min);			break;//go forward by the smallest amount possible
	//case 'b':				CMD_motor_command(1,25,0,-2*m_l_min,-2*m_r_min);		break;//go back by the smallest amount possible
	case 'b':				CMD_motor_command(5,1,1,-6*m_l_min,-6*m_r_min);			break;//go back by the smallest amount possible

	//case '7':				CMD_motor_command(1 ,10,0, -m_l_max , m_r_max);			break;//rotate left by the smallest amount possible
	case '7':				CMD_motor_command(5 ,1,1, -6*m_l_min , 6*m_r_min);		break;//rotate left by the smallest amount possible
	//case '9':				CMD_motor_command(1 ,10,0, m_l_max , -m_r_max);			break;//rotate right by the smallest amount possible amount
	case '9':				CMD_motor_command(5 ,1,1, 6*m_l_min , -6*m_r_min);		break;//rotate right by the smallest amount possible amount
	
//	case '1':				CMD_motor_command(5,1,1,-120 , 120);					break;//rotate left by the smallest amount possible
	case '1':				CMD_motor_command(4 , 1000,1000, -100, 100);	break; //rotate 360deg left at moderate speed
//	case '3':				CMD_motor_command(5,1,1,120, -120);						break;//rotate right by the smallest amount possible amount
	case '3':				CMD_motor_command(4 , 1000,1000, 100 , -100);break;//rotate 360deg right at moderate speed

	case '[':				CMD_motor_command(1 ,866,0, -m_l_max , m_r_max);		break;//turn on a dime at max speed, approx. 1 full rotation
	case ']':				CMD_motor_command(1 ,869,0, m_l_max , -m_r_max);		break;//turn on a dime at max speed, approx. 1 full rotation

	case 'o':																		break;//rotate 180deg left
	case 'p':																		break;//rotate 180deg right

	case 'O':  //rotate 90deg left at half speed
		break;

	case 'P':  //rotate 90deg right at half speed
		break;

	case '{':  //turn left
		break;

	case '}': //turn right
		break;

	case '<': //turn left
		CMD_motor_command(1 , 50,0 , m_l_speed/2.3 , m_r_speed*1.3);
		break;

	case '>': //turn right
		CMD_motor_command(1 , 50,0 , m_l_speed*1.3 , m_r_speed/2.3);
		break;


	case '0':  //ramp speed down to 0, i.e. come to a gradual stop
		m_r_target = m_l_target = 0;
		break;
	
	case '5': //stop immediately
	case ' ':
		//optionally, reverse speed for a short period of time to stop even harder and to compensate for inertia
		//outputs->cmd = 1; outputs->params[0]=25; outputs->params[1]=-m_r_speed; outputs->params[2]=-m_l_speed;
		m_r_speed = m_l_speed = m_r_target = m_l_target = 0;
		l=m_l_speed; r=m_r_speed;
		CMD_motor_command(6 , 8 , 8, 0 , 0);
		break;

	case '2': //go backwards
	case KEY_DOWN:
		if(m_r_target != 0) m_r_target -= m_r_delta; else m_r_target = -1*m_r_min; //6*m_r_delta;
		if(m_l_target != 0) m_l_target -= m_l_delta; else m_l_target = -1*m_l_min; //6*m_l_delta;
		break;

	case '8': //go forward
	case KEY_UP:
		if(m_r_target != 0) m_r_target += m_r_delta; else m_r_target = +1*m_r_min; //6*m_r_delta;
		if(m_l_target != 0) m_l_target += m_l_delta; else m_l_target = +1*m_l_min; //6*m_l_delta;
		break;

	case KEY_CTRL_RIGHT: //ctrl-right
		if(m_l_target != 0) m_l_target += m_l_delta; else m_l_target = +m_l_min; //6*m_l_delta;
		break;

	case '6': //arc/turn right
	case KEY_RIGHT:
		if(m_r_target != 0) m_r_target -= m_r_delta; else m_r_target = -m_r_min; //6*m_r_delta;
		if(m_l_target != 0) m_l_target += m_l_delta; else m_l_target = +m_l_min; //6*m_l_delta;
		break;

	case KEY_CTRL_LEFT: //ctrl-left
		if(m_r_target != 0) m_r_target += m_r_delta; else m_r_target = +m_r_min; //6*m_r_delta;
		break;

	case '4': //arc/turn left
	case KEY_LEFT:
		if(m_r_target != 0) m_r_target += m_r_delta; else m_r_target = +m_r_min; //6*m_r_delta;
		if(m_l_target != 0) m_l_target -= m_l_delta; else m_l_target = -m_l_min; //6*m_l_delta;
		break;

	case 'f': //go forward at max speed - no gradual ramp up
		m_r_target = m_r_speed = m_r_max;
		m_l_target = m_l_speed = m_l_max;
		break;

	case 'F': //go forward at max speed - with gradual ramp up
		m_r_target = m_r_max;
		m_l_target = m_l_max;
		break;

	case 'r': //go backward at max speed - no gradual ramp up
		m_r_target = m_r_speed = -m_r_max;
		m_l_target = m_l_speed = -m_l_max;
		break;

	case 'a': //arc
		m_r_target  = m_r_speed = m_r_target/2;
		break;

	case 'A': //arc
		m_l_target  = m_l_speed = m_l_target/2;
		break;

	case 'v':
		CMD_set_config_value(1,1, (uint8*)&v);
		v.s16 ++;
		CMD_send();
		break;

	default:
		update_required=0;
		break;
	}



	if(m_r_target > 255) m_r_target = 255;
	if(m_r_target < -255) m_r_target = -255;
	if(m_l_target > 255) m_l_target = 255;
	if(m_l_target < -255) m_l_target = -255;



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//apply speed ramp
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(m_r_speed == 0 && m_r_target != 0) 
	{
		m_r_speed = (m_r_target > 0 ? m_r_min : -m_r_min);
	}
	else
	{
		if(m_r_speed < m_r_target) m_r_speed+=m_ramp;  
		if(m_r_speed > m_r_target) m_r_speed-=m_ramp;
	}

	if(m_l_speed == 0 && m_l_target != 0) 
	{
		m_l_speed = (m_l_target > 0 ? m_l_min : -m_l_min);
	}
	else
	{
		if(m_l_speed < m_l_target) m_l_speed+=m_ramp;  
		if(m_l_speed > m_l_target) m_l_speed-=m_ramp;
	}


	//ml_bias = (s.inputs->encoders[0] - s.inputs->encoders[1])*2;
	ml_bias = 0;
	{
		if( (ml_bias!=0) || (l!=m_l_speed) || (r!=m_r_speed) )
		{
			//CMD_set_motors(m_l_speed /* *1.015f*/ - ml_bias, m_r_speed * 1.07f); // for going straight:  80,79
			//CMD_set_motors(m_l_speed , m_r_speed ); // for baby tracks
			CMD_motor_command(7 , 1,1 , m_l_speed , m_r_speed);
			//CMD_motor_command(1 , 65000,65000 , m_l_speed , m_r_speed);
			l=m_l_speed; r=m_r_speed;
			update_required=1;
		}
	}
	//CMD_set_motors(m_l_speed*1.0f, m_r_speed * 0.97f); //for right turns
	if(update_required) CMD_send();

#ifdef WIN32UI
	{
		extern volatile s32 joystick_changed_R,joystick_changed_L, joystick_changed_B,joystick_Ry_1,joystick_Rx_1, joystick_Ly_1,joystick_Lx_1;
		extern volatile u16 joystick_Buttons;
		static u32 jcR=0,jcL=0,jcB=0;

		if(jcB != joystick_changed_B)
		{
			jcB = joystick_changed_B;

			log_printf("joystick buttons:  0x%04x\n",joystick_Buttons);

			if(joystick_Buttons & 0x4000) { CMD_set_behavior_state(11,1); CMD_send(); }
			if(joystick_Buttons & 0x2000) { CMD_set_behavior_state(11,2); CMD_send(); }
		}

		if(jcL != joystick_changed_L)
		{
			t_config_value v;
			u16 x,y;

			jcL = joystick_changed_L;

			v.u16=125+(joystick_Lx_1/2);
			x=v.u16;
			cfg_set_value_by_grp_id(15,6, v);
			CMD_set_config_value(15,6, (uint8*)&v);
			CMD_send();
			
			v.u16=145+(joystick_Ly_1/3);
			y=v.u16;
			cfg_set_value_by_grp_id(15,5, v);
			CMD_set_config_value(15,5, (uint8*)&v);
			CMD_send();
			
			log_printf("joystick:   Lx_1,Ly_1 = %3d,%3d    x,y = %3d,%3d  \n",joystick_Lx_1,joystick_Ly_1,x,y);
		}

		if(jcR != joystick_changed_R)
		{
			//log_printf("joystick:  joystick_Ry_1=%d, joystick_Rx_1=%d\n",joystick_Ry_1,joystick_Rx_1);
			jcR = joystick_changed_R;
			CMD_motor_command(7 , 1,1 , joystick_Ry_1 - joystick_Rx_1, joystick_Ry_1 + joystick_Rx_1);
			CMD_send();
		}
	}
#endif
}




