
#include "odometry.h"
#include <math.h>

/*


y too high
left turns less accurate than right turns

*/

//in mm

//34:1  
//70mm wheel dia;  circumfence = 70*pi = 219.91148575128552669238503682957mm
//1633 counts per revolution =  0.1346671682494093856046448480279mm per tick
FLOAT PI = 3.1415926535897932384626433832795;

FLOAT odo_cml= 0.1346671682494093856046448480279; 
FLOAT odo_cmr= 0.1346671682494093856046448480279; 
FLOAT odo_b = 165.0; //87.0; //(89.69375) * 1.0;

/*
float odo_D = 3.53125f;
float odo_RW= 0.82677165354f;
float odo_TR= 48.0f;
float odo_PI= 3.1415926535897932384626433832795f;
*/

FLOAT odo_x=0.0, odo_y=0.0, odo_theta=0.0;
int odo_last_l=0;
int odo_last_r=0;


void odometry_reset(void)
{
	odo_x=0.0f;
	odo_y=0.0f;
	odo_theta=0.0f;
}

void odometry_update(int l_ticks, int r_ticks, FLOAT *x, FLOAT *y, FLOAT *theta)
{
	FLOAT d_theta, d_x, d_y, l, r;
	FLOAT d_Ul, d_Ur, d_U;
	//static int lt[4]={0,0,0,0} , rt[4]={0,0,0,0};
	//static int i=0;
	//int j;

	//lt[i]=l_ticks;
	//rt[i]=r_ticks;
	//i++; if(i==4) i=0;

	//l_ticks=0; 	for(j=0;j<4;j++) l_ticks+=lt[j];	l = (double)l_ticks / 4.0;
	//r_ticks=0; 	for(j=0;j<4;j++) r_ticks+=rt[j];	r = (double)r_ticks / 4.0;

	l = l_ticks;
	r = r_ticks;

	d_Ul = odo_cml * l;
	d_Ur = odo_cmr * r;
	d_U  = (d_Ul + d_Ur) / 2.0;

	d_x = d_U * cos(*theta);
	d_y = d_U * sin(*theta);
	*x     = *x + d_x;
	*y     = *y + d_y;

	d_theta = (d_Ur - d_Ul) / odo_b;
	*theta = *theta + d_theta;
	if(*theta > 2.0*PI) *theta -= 2.0*PI;
	if(*theta < -2.0*PI) *theta += 2.0*PI;


}


void odometry_test(void)
{
	FLOAT x=0,y=0,t=0;
	/*
0	0
1	0
1	1
1	1
1	1
2	2
3	3
4	4
5	5
5	5
*/
	odometry_update(0,0,&x,&y,&t);//0,0
	odometry_update(1,0,&x,&y,&t);//1,0
	odometry_update(0,1,&x,&y,&t);//1,1
	odometry_update(0,0,&x,&y,&t);//1,1
	odometry_update(0,0,&x,&y,&t);//1,1
	odometry_update(1,1,&x,&y,&t);//2,2
	odometry_update(1,1,&x,&y,&t);//3,3
	odometry_update(1,1,&x,&y,&t);//4,4
	odometry_update(1,1,&x,&y,&t);//5,5


}


