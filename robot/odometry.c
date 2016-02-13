
/*
inputs:
	l			number of stepper motor steps or encoder ticks that the left wheel has moved - >0 means forward, <0 means wheel has moved backward
	r			same as l - but for the right wheel

configuration paramters that ideally can be tweaked while the robot is running (that's why they are an input into the function and not a constant):
	odo_cml		number of millimeters that one step corresponds to for the left wheel
	odo_cmr		number of millimeters that one step corresponds to for the right wheel (note: not necessarily same as odo_cml after calibration)
	odo_b		wheel base, i.e. number of millimeters that the 2 weels (their center) are apart

state / output:
	x			global; robot's current x position
	y			global; robot's current y position 
	theta		global; robot's current heading (in radians, i.e. 180degrees = PI, 360degrees = PI*2)  0 means facing east

Process:
	basically one needs to call this function as frequently as possible since the underlying math makes an approximation that the robot moves 
	in a straight line;  this is not really true since it most cases it will be moving in an arc.

	on my robot I call this cuntion 50 times per second (every 20ms).  My robot moves up to 100 encoder ticks in that time (but every tick is quite small - 0.15mm)
	on an 8-bit atmel avr, it takes 250-290us to run this function
*/

float x=0.0f, y=0.0f, theta=0.0f;

void odometry_update(float l, float r, float odo_cml, float odo_cmr, float odo_b)
{
	float d_theta, d_x, d_y;
	float d_Ul, d_Ur, d_U;

	d_Ul = odo_cml * l;  //how many mm the left wheel has moved, basically the length of the arc described by the left wheel
	d_Ur = odo_cmr * r;  //how many mm the rigth wheel has moved
	d_U = (d_Ul + d_Ur) / 2.0f;  //how many mm the robot has moved; basically the length of the arc described by the center of the robot

	d_theta = (d_Ur - d_Ul) / odo_b;  //how much it has turned
	d_x = d_U * cos(theta);
	d_y = d_U * sin(theta);

	//update our absolute position and heading
	x = x + d_x;
	y = y + d_y;
	theta = theta + d_theta;
	
	//keep our heading to +/- 180 degrees
	if (theta > 1.0f*PI)  theta -= 2.0f*PI;  //so if the heading is 190 degrees it gets converted to -170 for example
	if (theta < -1.0f*PI) theta += 2.0f*PI; //if the heading is -190 degrees, it gets converted to +170 for example
}
