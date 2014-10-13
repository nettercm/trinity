
#include <Windows.h>
#include <timeapi.h>
#include <stdio.h>
#include "cocoos/cocoos.h"

#include "standard_includes.h"

#include "extApi.h"

int us_data[100];

static int clientID;

static int lm;
static int rm;
static int pan;
static int tilt;
static int sensor;
static float lp1,lp2,rp1,rp2;

void sim_step(void)
{
	int result;

	//simxGetJointPosition(clientID,lm,&(position[0]),simx_opmode_streaming);
	//simxGetJointPosition(clientID,rm,&(position[1]),simx_opmode_streaming);
	simxSetJointTargetVelocity(clientID,lm,((float) m.m2)/10.0f,simx_opmode_streaming);			
	simxSetJointTargetVelocity(clientID,rm,((float) m.m1)/10.0f,simx_opmode_streaming);		

	//1632.672 ticks per revolution
	//259.84781924773094164045499171293  ticks per rad
	simxGetJointPosition(clientID,lm,&lp1,simx_opmode_streaming);
	simxGetJointPosition(clientID,rm,&rp1,simx_opmode_streaming);
	printf("%10.8f,%10.8f\n",(lp1-lp2)*259.8478192477f,(rp1-rp2)*259.8478192477f);
	lp2=lp1;
	rp2=rp1;

	simxSetJointTargetPosition(clientID,pan,(((float)m.servo[1])-1350.0f)/353.0f,simx_opmode_streaming);
	simxSetJointTargetPosition(clientID,tilt,(((float)m.servo[0])-1450.0f)/300.0f,simx_opmode_streaming);
	//printf("0,1 = %5d,%5d\n",m.servo[0],m.servo[1]);

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

	simxSynchronousTrigger(clientID);
	if(result != simx_return_ok) printf("simxSynchronousTrigger() failed!\n");

	simxGetObjectHandle(clientID,"left_motor",&lm,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"right_motor",&rm,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"pan_servo",&pan,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"tilt_servo",&tilt,simx_opmode_oneshot_wait);
	//simxGetObjectHandle(clientID,"remoteApiControlledBubbleRobSensingNose",&sensor,simx_opmode_oneshot_wait);

	simxGetPingTime(clientID,&pingTime);
	printf("pingTime=%d\n",pingTime);

}
