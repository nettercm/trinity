/*
 * pid.h
 *
 * Created: 11/20/2012 11:30:21 PM
 *  Author: Chris
 */ 


#ifndef PID_H_
#define PID_H_


typedef struct
{
	s16 setpoint;
	s16 error;
	s16 max_pos_error;
	s16 max_neg_error;
	s16 last_error;
	s16 integral;
	s16 integral_max;
	s16 integral_min;
	s16 output;
	s16 Kp;
	s16 Kd;
	s16 Ki;
	s16 dT;                 //loop interval time; simply set this to 1 if its built into Kd
	s16 dEdT;
} t_PID_state;

extern s16 PID_loop(volatile t_PID_state *s, s16 m);
extern void PID_test(void);
extern void PID_init(void);


#endif /* PID_H_ */