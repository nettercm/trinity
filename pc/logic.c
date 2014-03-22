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
static sint16 m_r_delta=10 ,m_l_delta=10;
static sint16 m_r_min=10, m_l_min=10, m_r_max=260, m_l_max=260;
static sint16 m_r_target=0, m_l_target=0;
static sint16 m_ramp=5;


void do_wall_tracking(t_inputs *inputs, t_outputs *outputs)
{
	sint16 m_l=0, m_r=0;
	int error;
	float correction;
	static int past_error[4]={0,0,0,0};
	static int last_error=0;
	static float dE=0.0f;
	static int idx=0;

	error = s.inputs->sonar[4] - 40;//s.ir_NW_state.real_history_avg - 40;
	dE = (dE + (abs(error)-abs(last_error)))/2.0f;
	last_error = error;

	m_r = m_r_speed;
	m_l = m_l_speed;

	if(error > 0 && dE>=0 ) //turn left
	{
		if(error > 40) error = 40;
		correction = 1.0f + ((float)error / 100.0f);
		/*
		outputs->cmd = 1;
		outputs->params[0]=50; //50;
		m_r = m_r_speed*correction;// - (m_r_speed<0?-20:20);
		m_l = m_l_speed/correction;// + (m_l_speed<0?-20:20);
		if(m_r > 255) { m_l -= m_r-255; }
		outputs->params[1] = m_r;
		outputs->params[2] = m_l;
		*/
	}
	if(error < 0  && dE<=0 )// turn right
	{
		error *= -1;
		if(error > 40) error = 40;
		correction = 1.0f + ((float)error / 100.0f);
		/*
		outputs->cmd = 1;
		outputs->params[0]=50; //50;
		m_r = m_r_speed/correction;// - (m_r_speed<0?-20:20);
		m_l = m_l_speed*correction;// + (m_l_speed<0?-20:20);
		if(m_l > 255) { m_r -= m_l-255; }
		outputs->params[1] = m_r;
		outputs->params[2] = m_l;
		*/
	}

	printf("%3d,%3d,%5f,%3d,%3d\n",s.inputs->sonar[4],error,dE,m_l,m_r);

}


extern int us_data[];
void test_wall_tracking(void)
{
	int i=0;
	t_inputs inputs;

	m_r_speed=50; 
	m_l_speed=50;

	s.inputs = &inputs;

	while(us_data[i]<99999)
	{
		s.inputs->sonar[4] = us_data[i];
		do_wall_tracking(NULL, &(s.outputs));
		i++;
	}
}




volatile unsigned char key=0;

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

	//outputs->cmd = 0;

/*
#ifdef WIN32UI
	if(key) 
	{
		c=key; 
		key=0;
	}
#else
*/
	//this works in a UI app as well, provided AllocConsole() was called on startup
	if(_kbhit())
	{
		c = _getch();
		if(c==0x00) c=_getch() + 0xe100;
		if(c==0xe0) c=_getch() + 0xe000;
		//printf("c = '%c' = 0x%02x\n",c,c);
	}
//#endif

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
		//printf("c = '%c' = 0x%02x\n",c,c);
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
			CMD_motor_command(6 , 1,1 , m_l_speed , m_r_speed);
			l=m_l_speed; r=m_r_speed;
		}
	}
	//CMD_set_motors(m_l_speed*1.0f, m_r_speed * 0.97f); //for right turns
}





//////////////////////////////////////////////////////////////////////////////


void outputs_update(HANDLE p, t_inputs *inputs, t_outputs *outputs)
{
	static int do_outputs=0;

	if(--do_outputs<=0)
	{
		do_outputs = 2;
		process_user_input();
		CMD_send();
	}
}


