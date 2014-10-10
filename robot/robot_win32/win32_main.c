
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
static int sensor;

void sim_step(void)
{
	int result;

	//simxGetJointPosition(clientID,lm,&(position[0]),simx_opmode_streaming);
	//simxGetJointPosition(clientID,rm,&(position[1]),simx_opmode_streaming);
	simxSetJointTargetVelocity(clientID,lm,((float) m.m2)/10.0f,simx_opmode_oneshot);			
	simxSetJointTargetVelocity(clientID,rm,((float) m.m1)/10.0f,simx_opmode_oneshot);			

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

	simxGetObjectHandle(clientID,"remoteApiControlledBubbleRobLeftMotor",&lm,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"remoteApiControlledBubbleRobRightMotor",&rm,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"remoteApiControlledBubbleRobSensingNose",&sensor,simx_opmode_oneshot_wait);

	simxGetPingTime(clientID,&pingTime);
	printf("pingTime=%d\n",pingTime);

}
