
#include <Windows.h>
#include <timeapi.h>
#include <stdio.h>
#include "cocoos/cocoos.h"

#include "standard_includes.h"

#include "extApi.h"

int us_data[100];

static int clientID;

//object handles
static int lm,rm;
static int pan,tilt;
static int ir_left,ir_right;


static float lp1,lp2,lpd,rp1,rp2,rpd;
static float lticks,rticks;

static float pix2=6.283185307179586476925286766559f;
static float pi=3.1415926535897932384626433832795f;

void sim_step(void)
{
	int result;
	int t;

	simxSetJointTargetVelocity(clientID,lm,(((float) m.m2)/1.83f)/5.19695f,simx_opmode_streaming);			
	simxSetJointTargetVelocity(clientID,rm,(((float) m.m1)/1.83f)/5.19695f,simx_opmode_streaming);		

	//34.014:1 gear with 48cpr encoder =>  1632.672 ticks per revolution   =>   259.84781924773094164045499171293  ticks per rad
	simxGetJointPosition(clientID,lm,&lp1,simx_opmode_streaming);
	simxGetJointPosition(clientID,rm,&rp1,simx_opmode_streaming);

	t=simxGetLastCmdTime(clientID);

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

	simxSetJointTargetPosition(clientID,pan,(((float)m.servo[1])-1350.0f)/353.0f,simx_opmode_streaming);
	simxSetJointTargetPosition(clientID,tilt,(((float)m.servo[0])-1450.0f)/300.0f,simx_opmode_streaming);
	//printf("0,1 = %5d,%5d\n",m.servo[0],m.servo[1]);

	{
		unsigned char state;
		float point[3],surface[3];
		u16 distance;
		int handle;
		result = simxReadProximitySensor(clientID,ir_left,&state,&(point[0]),&handle,&(surface[0]),simx_opmode_streaming);
		//printf("%7d:    result=%3d,  state=%2d,  point=%5f,%5f,%5f,  handle=%3d,   surface=%5f,%5f,%5f\n",t, result, state,point[0],point[1],point[2],handle,surface[0],surface[1],surface[2]);
		distance = 300;
		if(state) distance=((point[2]*100.0f)/2.54f)*10.0f;
		s.inputs.ir[0] = s.ir[AI_IR_NW]	= distance;

		result = simxReadProximitySensor(clientID,ir_right,&state,&(point[0]),&handle,&(surface[0]),simx_opmode_streaming);
		//printf("%7d:    result=%3d,  state=%2d,  point=%5f,%5f,%5f,  handle=%3d,   surface=%5f,%5f,%5f\n",t, result, state,point[0],point[1],point[2],handle,surface[0],surface[1],surface[2]);
		distance = 300;
		if(state) distance=((point[2]*100.0f)/2.54f)*10.0f;
		s.inputs.ir[2] = s.ir[AI_IR_NE]	= distance;

		s.inputs.ir[1] = s.ir[AI_IR_N]		= 300;
		s.inputs.ir[3] = s.ir[AI_IR_N_long]	= 600;

	}


	result = simxSynchronousTrigger(clientID);
	if(result != simx_return_ok) printf("simxSynchronousTrigger() failed!\n");
}


void win32_main(void)
{
	int result;
	int pingTime;
	timeBeginPeriod(1);

	clientID=simxStart((simxChar*)"127.0.0.1",19997,1,1,2000,1);
	printf("clientID=%d\n",clientID);

	result = simxStopSimulation(clientID,simx_opmode_oneshot_wait);
	if(result != simx_return_ok) printf("simxStopSimulation() failed!\n");
	Sleep(100);
	result = simxStartSimulation(clientID,simx_opmode_oneshot_wait);
	if(result != simx_return_ok) printf("simxStartSimulation() failed!\n");

	result = simxSynchronous(clientID,1);
	if(result != simx_return_ok) printf("simxSynchronous() failed!\n");

	printf("sim time = %d\n", simxGetLastCmdTime(clientID));
	simxSynchronousTrigger(clientID);
	if(result != simx_return_ok) printf("simxSynchronousTrigger() failed!\n");
	printf("sim time = %d\n", simxGetLastCmdTime(clientID));

	simxGetObjectHandle(clientID,"left_motor",&lm,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"right_motor",&rm,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"pan_servo",&pan,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"tilt_servo",&tilt,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir_left",&ir_left,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir_right",&ir_right,simx_opmode_oneshot_wait);
	//simxGetObjectHandle(clientID,"remoteApiControlledBubbleRobSensingNose",&sensor,simx_opmode_oneshot_wait);

	printf("sim time = %d\n", simxGetLastCmdTime(clientID));
	simxGetPingTime(clientID,&pingTime);
	printf("pingTime=%d\n",pingTime);

}
