
#ifndef _kalman_h_
#define _kalman_h_

#ifdef __cplusplus 
extern "C" {
#endif


typedef struct {
	double q; //process noise covariance
	double r; //measurement noise covariance
	double x; //value
	double p; //estimation error covariance
	double k; //kalman gain
} t_kalman_state;


void kalman_init(t_kalman_state *state, double q, double r, double p, double intial_value);

void kalman_update(t_kalman_state* state, double measurement);


#ifdef __cplusplus 
}
#endif


#endif