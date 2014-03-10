#include <stdio.h>

#include "typedefs.h"
#include "pid.h"


volatile t_PID_state ps =
{
      100,	//setpoint;
	  0,	//error;
	  10,	//s16 max_pos_error;
	  -10,	//s16 max_neg_error;
	  0,	//last_error;
	  0,	//integral;
	  10,	//integral_max;
	  -10,	//integral_min;
	  0,	//output;
	  2.1*16,	//Kp;
	  0.1*16,	//Kd;
	  0.2*16,	//Ki;
	  1,	//dT;          
	  0		//dEdT;
};

s16 PID_loop(volatile t_PID_state *s, s16 m)
{
      s16 error, dE, dEdT, output;

      //error
      error = m - s->setpoint;
	  if(error > s->max_pos_error) error = s->max_pos_error;
	  if(error < s->max_neg_error) error = s->max_neg_error;

      //update the derivative
      dE = error - s->last_error;
      dEdT = dE / 1; //s->dT;

      //update the integral
      s->integral += error;
      if(s->integral > s->integral_max) s->integral = s->integral_max;  //limit to max, e.g. 200
      if(s->integral < s->integral_min) s->integral = s->integral_min;  //limit to min, e.g. -200
      //if(error == 0) s->integral = 0; //reset the integral if error is 0 or if there is a zero crossing
      //if(error > 0 && s->last_error < 0) s->integral = 0;
      //if(error < 0 && s->last_error > 0) s->integral = 0;

      output  = (s->Kp * error)>>4;
      output += (s->Ki * s->integral)>>4;
      output += (s->Kd * dEdT)>>4;

      s->output         = output;
      s->last_error     = error;
      s->dEdT           = dEdT;

      return output;
}

void PID_init(void)
{
		
}


void test_pid(void)
{
	volatile s16 m;
	volatile u08 i;
	volatile s16 o;

	ps.setpoint = 100;
	ps.last_error = ps.error = ps.integral = 0;

	m=200;
	for(i=0;i<65;i++)
	{
		o=PID_loop(&ps,m);
		//printf("i=%d, m=%d, o=%d\r\n",i, m, o);
		m=m-(ps.output/20);
		if(i==20) ps.setpoint = 0;
	}
	/*
	printf("%d\r\n",pid_loop(&s,100));
	printf("%d\r\n",pid_loop(&s,200));
	printf("%d\r\n",pid_loop(&s,200));
	printf("%d\r\n",pid_loop(&s,200));
	printf("%d\r\n",pid_loop(&s,200));
	printf("%d\r\n",pid_loop(&s,190));
	printf("%d\r\n",pid_loop(&s,180));
	printf("%d\r\n",pid_loop(&s,150));
	printf("%d\r\n",pid_loop(&s,120));
	printf("%d\r\n",pid_loop(&s,100));
	printf("%d\r\n",pid_loop(&s,90));
	printf("%d\r\n",pid_loop(&s,100));
	printf("%d\r\n",pid_loop(&s,101));
	printf("%d\r\n",pid_loop(&s,101));
	printf("%d\r\n",pid_loop(&s,101));
	printf("%d\r\n",pid_loop(&s,101));
	printf("%d\r\n",pid_loop(&s,101));
	printf("%d\r\n",pid_loop(&s,101));
	printf("%d\r\n",pid_loop(&s,200));
	printf("%d\r\n",pid_loop(&s,101));
	printf("%d\r\n",pid_loop(&s,101));
	*/
	printf("\r\n");
}


void PID_test(void)
{
	test_pid();
	ps.Kd = 0;
	test_pid();
	ps.Ki = 0;
	test_pid();
}


#ifdef WIN32
void PID_test_2(void)
	{
		extern int us_data[];
		int i=0;
		t_PID_state s =
		{
			  40,	//setpoint;
			  0,	//error;
			  10,	//s16 max_pos_error;
			  -10,	//s16 max_neg_error;
			  0,	//last_error;
			  0,	//integral;
			  50,	//integral_max;
			  -50,	//integral_min;
			  0,	//output;
			  1,	//Kp;
			  1,	//Kd;
			  1,	//Ki;
			  1,	//dT;          
			  0		//dEdT;
		};

		while(us_data[i]<99999)
		{
			PID_loop(&s,(s16)us_data[i]);
			printf("i=%3d, m=%3d, o=%3d\n",i,us_data[i],s.output);
			i=i+2; //every sample is repeated, so skip the duplicate
		}
	}
#endif