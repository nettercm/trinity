
#ifndef _ODOMETRY_H_
#define _ODOMETRY_H_

#define FLOAT float

extern void odometry_reset(void);
extern void odometry_update(int l_ticks, int r_ticks, FLOAT *x, FLOAT *y, FLOAT *theta);
extern void odometry_test(void);

extern FLOAT odo_x, odo_y, odo_theta;
extern int odo_last_l;
extern int odo_last_r;

extern FLOAT PI; // = 3.1415926535897932384626433832795;


#endif