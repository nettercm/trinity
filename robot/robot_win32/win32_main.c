
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
static int ir0,ir1,ir2,ir3,ir4,ir5,ir6,ir7;
static int line_left,line_right;
static int flame_sensor;
static int sonar_front;
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
static double lticks=0,rticks=0;

static double pix2=6.283185307179586476925286766559;
static double pi=3.1415926535897932384626433832795;


#define STREAMING_MODE (simx_opmode_streaming+5)
//#define STREAMING_MODE simx_opmode_oneshot_wait

void sim_step(void)
{
	int result;
	static int t_sim,t_sim_last=0;
	static u32 t_real_now, t_real_last=0; 
	static u32 t_m,t_m_last=0;
	static int use_actual_position=0;
	static int inter_step_delay=0;
	int pingTime;

	t_sim = simxGetLastCmdTime(clientID);
	t_real_now = timeGetTime();
	m.elapsed_milliseconds=t_sim;
	t_m=m.elapsed_milliseconds;
	if(0)
	//if(t_real_now-t_real_last >= 1000)
	{
		simxGetPingTime(clientID,&pingTime);		
		printf("dT(sim) = %d,  dT(real)=%d,   dT(model)=%d,  ping time = %d\n", t_sim-t_sim_last,  t_real_now-t_real_last,  t_m-t_m_last, pingTime);
		t_real_last=t_real_now;
		t_sim_last=t_sim;
		t_m_last=t_m;
	}

	result = simxSynchronousTrigger(clientID);
	//if(result != simx_return_ok) printf("simxSynchronousTrigger() failed!  t=%7d\n",t_sim);

	simxGetObjectPosition(clientID,robot,-1,sim_state.robot_position,STREAMING_MODE);
	simxGetObjectOrientation(clientID,robot,-1,sim_state.robot_orientation,STREAMING_MODE);


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

		if(c=='r')
		{
			s.behavior_state[FOLLOW_WALL]=2;
		}

		if(c=='s')
		{
			m.start_signal=1;
		}

		if(c=='x')
		{
			s.inputs.x =	sim_state.robot_position[0]*1000;
			s.inputs.y =	sim_state.robot_position[1]*1000;
			s.inputs.theta= sim_state.robot_orientation[2];
		}

		if(c=='X')
		{
			use_actual_position = !use_actual_position;
		}

		if(c==' ')
		{
			inter_step_delay = !inter_step_delay;
		}
	}

	if(use_actual_position)
	{
		s.inputs.x =	sim_state.robot_position[0]*1000;
		s.inputs.y =	sim_state.robot_position[1]*1000;
		s.inputs.theta= sim_state.robot_orientation[2];
	}

	if(inter_step_delay) Sleep(500);
}



void sim_outputs(void)
{
	//motors
	simxSetJointTargetVelocity(clientID,lm,((((float) m.m2)/1.83f)/5.19695f)*1.0f,STREAMING_MODE);			
	simxSetJointTargetVelocity(clientID,rm,(((float) m.m1)/1.83f)/5.19695f,STREAMING_MODE);		

	//servos
	simxSetJointTargetPosition(clientID,pan,(((float)m.servo[1])-1350.0f)/353.0f,STREAMING_MODE);
	simxSetJointTargetPosition(clientID,tilt,(((float)m.servo[0])-1450.0f)/300.0f,STREAMING_MODE);
	//printf("0,1 = %5d,%5d\n",m.servo[0],m.servo[1]);

}



int read_ir(int handle,int min, int max, float noise_factor)
{
		unsigned char state;
		float point[3],surface[3];
		float distance;
		float noise;
		//float noise_factor = 0.0002f; //  +/- 2%
		int result;

		result = simxReadProximitySensor(clientID,handle,&state,&(point[0]),&handle,&(surface[0]),STREAMING_MODE);
		distance = max;
		if(state) 
		{
			distance=((point[2]*100.0f)/2.54f)*10.0f;
			if(distance < min) distance = min + (min-distance);
			noise = 100 - (rand() % 200);
			noise = noise*noise_factor;
			distance += distance * noise;
		}
		return distance;
}


void sim_inputs(void)
{
	static int ir_update_countdown=2;
	static int sonar_update_countdown=2;
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
		//printf("dT(sim) = %d,  dT(real)=%d,   dT(model)=%d,  ping time = %d\n", t_sim-t_sim_last,  t_real_now-t_real_last,  t_m-t_m_last, pingTime);
		t_real_last=t_real_now;
		t_sim_last=t_sim;
		t_m_last=t_m;
	}

	//initialize un-modeled sensors as if there was not detection
	//s.inputs.sonar[0] = 4000; //north
	s.inputs.sonar[1] = 4000;
	//s.inputs.sonar[2] = 4000;
	//s.inputs.sonar[3] = 4000;


	//------------------------------------------------------------------------------------------------------------------------------------
	//down-facing line sensors
	auxValues = NULL; auxValuesCount = NULL;
	result = simxReadVisionSensor(clientID,line_left,&state,&auxValues,&auxValuesCount,STREAMING_MODE);
	if(result==0)
	{
		//printf("aVC[0]=%d, aVC[1]=%d, av[0]=%f,av[10]=%f,\n",auxValuesCount[0],auxValuesCount[1],auxValues[0],auxValues[10]);
		s.line[LEFT_LINE] = (1.0f-auxValues[10])*255;
	}
	if(auxValues)simxReleaseBuffer((simxUChar*)auxValues);
	if(auxValuesCount)simxReleaseBuffer((simxUChar*)auxValuesCount);

	auxValues = NULL; auxValuesCount = NULL;
	result = simxReadVisionSensor(clientID,line_right,&state,&auxValues,&auxValuesCount,STREAMING_MODE);
	if(result==0)
	{
		//printf("aVC[0]=%d, aVC[1]=%d, av[0]=%f,av[10]=%f,\n",auxValuesCount[0],auxValuesCount[1],auxValues[0],auxValues[10]);
		s.line[RIGHT_LINE] = (1.0f-auxValues[10])*255;
	}
	if(auxValues)simxReleaseBuffer((simxUChar*)auxValues);
	if(auxValuesCount)simxReleaseBuffer((simxUChar*)auxValuesCount);
	//printf("%3d,%3d\n",s.line[LEFT_LINE],s.line[RIGHT_LINE]);
	//------------------------------------------------------------------------------------------------------------------------------------



	//------------------------------------------------------------------------------------------------------------------------------------
	//Flame sensor
	auxValues = NULL; auxValuesCount = NULL;
	result = simxReadVisionSensor(clientID,flame_sensor,&state,&auxValues,&auxValuesCount,STREAMING_MODE);
	if(result==0)
	{
		float flame;
		//printf("aVC[0]=%d, aVC[1]=%d, av[0]=%f,av[13]=%f,\n",auxValuesCount[0],auxValuesCount[1],auxValues[0],auxValues[13]*4000);
		flame = (auxValues[13]*1020.0f);
		if(flame > 255.0f) flame=255.0f;
		s.inputs.analog[AI_FLAME_N] = 255 - (u08)flame;
		//printf("auxValues[13]=%f    s.inputs.analog[AI_FLAME_N]=%d\n",auxValues[13],s.inputs.analog[AI_FLAME_N]);
	}
	if(auxValues)simxReleaseBuffer((simxUChar*)auxValues);
	if(auxValuesCount)simxReleaseBuffer((simxUChar*)auxValuesCount);
	//------------------------------------------------------------------------------------------------------------------------------------



	//------------------------------------------------------------------------------------------------------------------------------------
	//encoders
	//34.014:1 gear with 48cpr encoder =>  1632.672 ticks per revolution   =>   259.84781924773094164045499171293  ticks per rad
	simxGetJointPosition(clientID,lm,&lp1,STREAMING_MODE);
	simxGetJointPosition(clientID,rm,&rp1,STREAMING_MODE);
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

	lticks = (lpd)*259.8478192477;
	rticks = (rpd)*259.8478192477;
	if(abs(lticks)<0.05) lticks=0.0;
	if(abs(rticks)<0.05) rticks=0.0;
	m.enc_ab += lticks;
	m.enc_cd += rticks;
	lp2=lp1;
	rp2=rp1;
	//printf("%7d:  %10.6f,%10.6f\n",t,lticks,rticks);

	simxGetObjectPosition(clientID,robot,-1,sim_state.robot_position,STREAMING_MODE);
	simxGetObjectOrientation(clientID,robot,-1,sim_state.robot_orientation,STREAMING_MODE);

	if(0)
	{
		static double enc_ab=0, enc_cd=0;
		enc_ab += lticks;
		enc_cd += rticks;
		//printf("l=%d, r=%d\n",enc_ab,enc_cd);

		printf("actual x,y,theta = %7.4f, %7.4f, %7.4f   l,r=%7.3f,%7.3f   ab,cd=%7.3f,%7.3f   calc x,y,theta = %7.4f, %7.4f, %7.4f\n",
			sim_state.robot_position[0],sim_state.robot_position[1], sim_state.robot_orientation[2] * (180.0f/3.1415926535897932384626433832795f),
			lticks,rticks,enc_ab,enc_cd,
			s.inputs.x, s.inputs.y, s.inputs.theta * (180.0f/3.1415926535897932384626433832795f)
		);	
	}
	//------------------------------------------------------------------------------------------------------------------------------------




	//------------------------------------------------------------------------------------------------------------------------------------
	//sharp ir sensors update about once every 30ms;  so let's just say once every 40ms, i.e. every other simulation time step
	ir_update_countdown--;
	if(ir_update_countdown<1)
	{
		ir_update_countdown=2;

		s.inputs.ir[IR_NW] = s.ir[IR_NW]		= (u16)read_ir(ir7,40,300,0.0002);
		s.inputs.ir[IR_NE] = s.ir[IR_NE]		= (u16)read_ir(ir1,40,300,0.0002);
		s.inputs.ir[IR_N]  = s.ir[IR_N]			= (u16)read_ir(ir0,60,600,0.0002);

		//s.inputs.ir[3] = s.ir[IR_N_long]		= (u16)read_ir(ir0,60,600,0.0002);

		s.inputs.ir[IR_E]						= (u16)read_ir(ir2,40,400,0.0002);
		s.inputs.ir[IR_SE]						= (u16)read_ir(ir3,40,400,0.0002);
		s.inputs.ir[IR_SW]						= (u16)read_ir(ir5,40,400,0.0002);
		s.inputs.ir[IR_W]						= (u16)read_ir(ir6,40,400,0.0002);
	}
	//------------------------------------------------------------------------------------------------------------------------------------


	if(0)
	{
		float d,x1,y1,t1,t2,x2,y2,x3,y3;
		d=((float)s.ir[IR_NE]/10.0)*25.4;
		x1=50;
		y1=-50;
		t1=-45.0*(PI/180);
		t2= sim_state.robot_orientation[2]; //67.0*(PI/180);
		x2=x1+d*cos(t1);
		y2=y1+d*sin(t1);
		x3=x2*cos(t2)-y2*sin(t2);
		y3=x2*sin(t2)+y2*cos(t2);
		x3+=sim_state.robot_position[0]*1000;
		y3+=sim_state.robot_position[1]*1000;
		printf("d,x2,y2,x3,y3=%f   %f,%f   %f,%f\n",d,x2,y2,x3,y3);


	}


	//------------------------------------------------------------------------------------------------------------------------------------
	//sonar
	sonar_update_countdown--;
	if(sonar_update_countdown<1)
	{
		unsigned char state;
		float point[3],surface[3];
		float distance;
		int handle;
		float noise;
		float noise_factor = 0.0002f; //  +/- 2%

		sonar_update_countdown=2;

		result = simxReadProximitySensor(clientID,sonar_front,&state,&(point[0]),&handle,&(surface[0]),STREAMING_MODE);
		distance = 4000;
		if(state) 
		{
			distance=((point[2]*100.0f)/2.54f)*10.0f;
			//if(distance < 40) distance = 40 + (40-distance);
			noise = 100 - (rand() % 200);
			noise = noise*noise_factor;
			distance += distance * noise;
		}
		s.inputs.sonar[0] = (u16)distance;
	}
	//------------------------------------------------------------------------------------------------------------------------------------



	
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

	simxGetObjectHandle(clientID,"ir0",&ir0,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir1",&ir1,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir2",&ir2,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir3",&ir3,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir4",&ir4,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir5",&ir5,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir6",&ir6,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"ir7",&ir7,simx_opmode_oneshot_wait);

	simxGetObjectHandle(clientID,"line_left",&line_left,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"line_right",&line_right,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"flame_sensor",&flame_sensor,simx_opmode_oneshot_wait);

	simxGetObjectHandle(clientID,"sonar_front",&sonar_front,simx_opmode_oneshot_wait);

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

		simxGetJointPosition(clientID,lm,&lp1,STREAMING_MODE);
		simxGetJointPosition(clientID,rm,&rp1,STREAMING_MODE);
		simxReadVisionSensor(clientID,line_left,&state,&auxValues,&auxValuesCount,STREAMING_MODE);

		simxSetObjectPosition(clientID,robot,-1,sim_state.robot_position,STREAMING_MODE);
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
