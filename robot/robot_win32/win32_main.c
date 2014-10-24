
#include <Windows.h>
#include <timeapi.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include "cocoos/cocoos.h"

#include "standard_includes.h"

#include "extApi.h"

int us_data[100];

static int clientID;

//object handles
static int lm,rm;
static int pan,tilt;
static int ir_left,ir_right,ir_front;
static int line_left,line_right;
static int robot;

typedef struct
{
	float robot_position[3];
	float robot_orientation[3];
} t_simulation_state;

t_simulation_state sim_state;

float *auxValues=NULL;
int *auxValuesCount=NULL;
u08 state;

static float lp1,lp2,lpd,rp1,rp2,rpd;
static float lticks,rticks;

static float pix2=6.283185307179586476925286766559f;
static float pi=3.1415926535897932384626433832795f;

void sim_step(void)
{
	int result;
	static int t_sim,t_sim_last=0;
	static u32 t_real_now, t_real_last=0; 
	static u32 t_m,t_m_last=0;
	int pingTime;

	t_sim = simxGetLastCmdTime(clientID);
	t_real_now = timeGetTime();
	t_m=m.elapsed_milliseconds;
	if(t_real_now-t_real_last >= 1000)
	{
		simxGetPingTime(clientID,&pingTime);		
		printf("dT(sim) = %d,  dT(real)=%d,   dT(model)=%d,  ping time = %d\n", t_sim-t_sim_last,  t_real_now-t_real_last,  t_m-t_m_last, pingTime);
		t_real_last=t_real_now;
		t_sim_last=t_sim;
	}

	result = simxSynchronousTrigger(clientID);
	//if(result != simx_return_ok) printf("simxSynchronousTrigger() failed!  t=%7d\n",t_sim);


	if(_kbhit())
	{
		int c;
		c = _getch();
		if(c==0x1b)
		{
			simxStopSimulation(clientID,simx_opmode_oneshot_wait);
			simxFinish(clientID);
			exit(0);
		}
	}
}



void sim_outputs(void)
{
	//motors
	simxSetJointTargetVelocity(clientID,lm,((((float) m.m2)/1.83f)/5.19695f)*1.1f,simx_opmode_streaming);			
	simxSetJointTargetVelocity(clientID,rm,(((float) m.m1)/1.83f)/5.19695f,simx_opmode_streaming);		

	//servos
	simxSetJointTargetPosition(clientID,pan,(((float)m.servo[1])-1350.0f)/353.0f,simx_opmode_streaming);
	simxSetJointTargetPosition(clientID,tilt,(((float)m.servo[0])-1450.0f)/300.0f,simx_opmode_streaming);
	//printf("0,1 = %5d,%5d\n",m.servo[0],m.servo[1]);

}


void sim_inputs(void)
{
	static int ir_update_countdown=2;
	int pingTime=999;
	int result;
	static int t_sim,t_sim_last=0;
	static u32 t_real_now, t_real_last=0; 
	static u32 t_m,t_m_last=0;

	t_sim = simxGetLastCmdTime(clientID);
	t_real_now = timeGetTime();
	t_m=m.elapsed_milliseconds;
	//if(t_now-t_last >= 1000)
	if(0)
	{
		//printf("%d\n",t_now-t_last);
		pingTime=-1;//simxGetPingTime(clientID,&pingTime);		
		printf("dT(sim) = %d,  dT(real)=%d,   dT(model)=%d,  ping time = %d\n", t_sim-t_sim_last,  t_real_now-t_real_last,  t_m-t_m_last, pingTime);
		t_real_last=t_real_now;
		t_sim_last=t_sim;
		t_m_last=t_m;
	}

	s.inputs.analog[AI_FLAME_N]=255;

	auxValues = NULL; auxValuesCount = NULL;
	result = simxReadVisionSensor(clientID,line_left,&state,&auxValues,&auxValuesCount,simx_opmode_streaming);
	if(result==0)
	{
		//printf("aVC[0]=%d, aVC[1]=%d, av[0]=%f,av[10]=%f,\n",auxValuesCount[0],auxValuesCount[1],auxValues[0],auxValues[10]);
		s.line[LEFT_LINE] = (1.0f-auxValues[10])*255;
	}
	if(auxValues)simxReleaseBuffer((simxUChar*)auxValues);
	if(auxValuesCount)simxReleaseBuffer((simxUChar*)auxValuesCount);

	auxValues = NULL; auxValuesCount = NULL;
	result = simxReadVisionSensor(clientID,line_right,&state,&auxValues,&auxValuesCount,simx_opmode_streaming);
	if(result==0)
	{
		//printf("aVC[0]=%d, aVC[1]=%d, av[0]=%f,av[10]=%f,\n",auxValuesCount[0],auxValuesCount[1],auxValues[0],auxValues[10]);
		s.line[RIGHT_LINE] = (1.0f-auxValues[10])*255;
	}
	if(auxValues)simxReleaseBuffer((simxUChar*)auxValues);
	if(auxValuesCount)simxReleaseBuffer((simxUChar*)auxValuesCount);
	//printf("%3d,%3d\n",s.line[LEFT_LINE],s.line[RIGHT_LINE]);


	//34.014:1 gear with 48cpr encoder =>  1632.672 ticks per revolution   =>   259.84781924773094164045499171293  ticks per rad
	simxGetJointPosition(clientID,lm,&lp1,simx_opmode_streaming);
	simxGetJointPosition(clientID,rm,&rp1,simx_opmode_streaming);

	t_sim=simxGetLastCmdTime(clientID);

	if( (lp1 < -2.0f) && (lp2 > 2.0f) ) 
	{
		lpd=(lp1-lp2)+pix2;
	}
	else if( (lp1 > 2.0f) && (lp2 < -2.0f) ) 
	{
		lpd=(lp1-lp2)-pix2;
	}
	else lpd=lp1-lp2;

	if( (rp1 < -2.0f) && (rp2 > 2.0f) ) 
	{
		rpd=(rp1-rp2)+pix2;
	}
	else if( (rp1 > 2.0f) && (rp2 < -2.0f) ) 
	{
		rpd=(rp1-rp2)-pix2;
	}
	else rpd=rp1-rp2;

	lticks = (lpd)*259.8478192477f;
	rticks = (rpd)*259.8478192477f;
	m.enc_ab += (s16)lticks;
	m.enc_cd += (s16)rticks;
	lp2=lp1;
	rp2=rp1;
	//printf("%7d:  %10.6f,%10.6f\n",t,lticks,rticks);


	//sharp ir sensors update about once every 30ms;  so let's just say once every 40ms, i.e. every other simulation time step
	ir_update_countdown--;
	if(ir_update_countdown<1)
	{
		unsigned char state;
		float point[3],surface[3];
		float distance;
		int handle;
		float noise;
		float noise_factor = 0.0008f; //  +/- 8%

		ir_update_countdown=2;

		result = simxReadProximitySensor(clientID,ir_left,&state,&(point[0]),&handle,&(surface[0]),simx_opmode_streaming);
		distance = 300;
		if(state) 
		{
			distance=((point[2]*100.0f)/2.54f)*10.0f;
			if(distance < 40) distance = 40 + (40-distance);
			noise = 100 - (rand() % 200);
			noise = noise*noise_factor;
			distance += distance * noise;
		}
		s.inputs.ir[0] = s.ir[AI_IR_NW]	= (u16)distance;


		result = simxReadProximitySensor(clientID,ir_right,&state,&(point[0]),&handle,&(surface[0]),simx_opmode_streaming);
		//printf("%7d:    result=%3d,  state=%2d,  point=%5f,%5f,%5f,  handle=%3d,   surface=%5f,%5f,%5f\n",t, result, state,point[0],point[1],point[2],handle,surface[0],surface[1],surface[2]);
		distance = 300;
		if(state) 
		{
			distance=((point[2]*100.0f)/2.54f)*10.0f;
			if(distance < 40) distance = 40 + (40-distance);
			noise = 100 - (rand() % 200);
			noise = noise*noise_factor;
			distance += distance * noise;
		}
		s.inputs.ir[2] = s.ir[AI_IR_NE]	= distance;

		result = simxReadProximitySensor(clientID,ir_front,&state,&(point[0]),&handle,&(surface[0]),simx_opmode_streaming);
		distance = 300;
		if(state) 
		{
			distance=((point[2]*100.0f)/2.54f)*10.0f;
			if(distance < 40) distance = 40 + (40-distance);
			noise = 100 - (rand() % 200);
			noise = noise*noise_factor;
			distance += distance * noise;
		}
		s.inputs.ir[1] = s.ir[AI_IR_N]		= distance;

		s.inputs.ir[3] = s.ir[AI_IR_N_long]	= 600;
	}

	simxGetObjectPosition(clientID,robot,-1,sim_state.robot_position,simx_opmode_streaming);
	simxGetObjectOrientation(clientID,robot,-1,sim_state.robot_orientation,simx_opmode_streaming);
	//printf("%6.2f,%6.2f\n",sim_state.robot_position[0],sim_state.robot_position[1]);
}


void win32_main(void)
{
	int result;
	int pingTime;
	timeBeginPeriod(1);

	srand(0);

	clientID=simxStart((simxChar*)"127.0.0.1",19997,1,1,2000,1);
	printf("clientID=%d\n",clientID);

	result = simxStopSimulation(clientID,simx_opmode_oneshot_wait);
	if(result != simx_return_ok) printf("simxStopSimulation() failed!\n");

	simxGetObjectHandle(clientID,"left_motor",&lm,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"right_motor",&rm,simx_opmode_oneshot_wait);

	simxGetObjectHandle(clientID,"pan_servo",&pan,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"tilt_servo",&tilt,simx_opmode_oneshot_wait);

	simxGetObjectHandle(clientID,"ir_left",&ir_left,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir_right",&ir_right,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir_front",&ir_front,simx_opmode_oneshot_wait);

	simxGetObjectHandle(clientID,"line_left",&line_left,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"line_right",&line_right,simx_opmode_oneshot_wait);

	simxGetObjectHandle(clientID,"Robot",&robot,simx_opmode_oneshot_wait);

	result = simxStartSimulation(clientID,simx_opmode_oneshot_wait);
	if(result != simx_return_ok) printf("simxStartSimulation() failed!\n");

	result = simxSynchronous(clientID,1);
	if(result != simx_return_ok) printf("simxSynchronous() failed!\n");

	simxSetJointTargetVelocity(clientID,lm,0,simx_opmode_oneshot_wait);			
	simxSetJointTargetVelocity(clientID,rm,0,simx_opmode_oneshot_wait);		
	simxSetJointTargetPosition(clientID,pan,0,simx_opmode_oneshot_wait);
	simxSetJointTargetPosition(clientID,tilt,0,simx_opmode_oneshot_wait);



	printf("sim time = %d\n", simxGetLastCmdTime(clientID));
	simxSynchronousTrigger(clientID);
	if(result != simx_return_ok) printf("simxSynchronousTrigger() failed!\n");
	printf("sim time = %d\n", simxGetLastCmdTime(clientID));
	

	printf("sim time = %d\n", simxGetLastCmdTime(clientID));
	simxGetPingTime(clientID,&pingTime);
	printf("pingTime=%d\n",pingTime);

	//the following is just to test how fast the simulation can run with this "V-REM Remote API client" driving it via the trigger.  k

	simxGetObjectPosition(clientID,robot,-1,sim_state.robot_position,simx_opmode_oneshot_wait);

	//when playing back previously recorded data, we need to programmatically move the robot, but to do this, it must not be dynamic.
	if(0)
	{
		int prop;
		result = simxGetModelProperty(clientID,robot,&prop,simx_opmode_oneshot_wait);
		prop |= sim_modelproperty_not_dynamic;
		prop |= sim_modelproperty_not_respondable;
		result = simxSetModelProperty(clientID,robot,prop,simx_opmode_oneshot_wait);
	}

	while(0)
	{
		static int t,t_last=0;
		static u32 t_real_now, t_real_last=0; 

		simxGetJointPosition(clientID,lm,&lp1,simx_opmode_streaming);
		simxGetJointPosition(clientID,rm,&rp1,simx_opmode_streaming);
		simxReadVisionSensor(clientID,line_left,&state,&auxValues,&auxValuesCount,simx_opmode_streaming);

		simxSetObjectPosition(clientID,robot,-1,sim_state.robot_position,simx_opmode_streaming);
		sim_state.robot_position[1]-=0.002;

		t = simxGetLastCmdTime(clientID);
		t_real_now = timeGetTime();
		if(t_real_now-t_real_last >= 1000)
		{
			simxGetPingTime(clientID,&pingTime);		
			printf("dT(sim) = %d,  dT(real)=%d,   ping time = %d\n", t-t_last, t_real_now-t_real_last, pingTime);
			t_real_last=t_real_now;
			t_last=t;
		}

		result = simxSynchronousTrigger(clientID);

		if(_kbhit())
		{
			int c;
			c = _getch();
			if(c==0x1b)
			{
				simxStopSimulation(clientID,simx_opmode_oneshot_wait);
				simxFinish(clientID);
				exit(0);
			}
		}
	}
}
