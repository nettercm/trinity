
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
static int flame0,flame1,flame2;
static int sonar0,sonar1,sonar2,sonar3,sonar4;
static int robot,candle;

const float IGNORE_X = 9999999.0f;
const float IGNORE_Y = 9999999.0f;
const float IGNORE_Z = 9999999.0f;
const float IGNORE_THETA = 9999999.0f;
const float INVALID_XYZ = 9999999.0f;

typedef struct
{
	float robot_position[3];
	float robot_orientation[3];
} t_simulation_state;

t_simulation_state sim_state;

typedef struct
{
	float x;
	float y;
} t_xy;

typedef struct
{
	float x;
	float y;
	float t;
} t_xyt;


float *auxValues=NULL;
int *auxValuesCount=NULL;
u08 state;

static float lp1,lp2,lpd,rp1,rp2,rpd;
static double lticks=0,rticks=0;

static double pix2=6.283185307179586476925286766559;
static double pi=3.1415926535897932384626433832795;


#define STREAMING_MODE (simx_opmode_streaming+5)
//#define STREAMING_MODE simx_opmode_oneshot_wait

void move_candle(float x, float y)
{
	int prop;
	int result;
	float candle_position[3];

	/*
	result = simxGetModelProperty(clientID, candle, &prop, simx_opmode_oneshot_wait);
	prop |= sim_modelproperty_not_dynamic;
	prop |= sim_modelproperty_not_respondable;
	result = simxSetModelProperty(clientID, candle, prop, simx_opmode_oneshot_wait);
	*/
	simxGetObjectPosition(clientID, candle, -1, candle_position, simx_opmode_oneshot_wait);
	candle_position[0] = x;
	candle_position[1] = y;
	simxSetObjectPosition(clientID, candle, -1, candle_position, simx_opmode_oneshot_wait);

}


void move_object(const char *object_name, float x, float y, float theta)
{
	int new_prop, org_prop;
	int result;
	float object_position[3];
	float object_orientation[3];
	int handle;

	result = simxGetObjectHandle(clientID, object_name, &handle, simx_opmode_oneshot_wait);

	/*
	result = simxGetModelProperty(clientID, handle, &org_prop, simx_opmode_oneshot_wait);
	new_prop = org_prop | sim_modelproperty_not_dynamic;
	new_prop = org_prop | sim_modelproperty_not_respondable;
	result = simxSetModelProperty(clientID, handle, new_prop, simx_opmode_oneshot_wait);
	*/

	simxGetObjectOrientation(clientID, handle,-1, object_orientation,simx_opmode_oneshot_wait);
	if(theta!=IGNORE_THETA) object_orientation[2] = theta;
	simxSetObjectOrientation(clientID, handle, -1, object_orientation, simx_opmode_oneshot_wait);

	simxGetObjectPosition(clientID, handle, -1, object_position, simx_opmode_oneshot_wait);
	if(x!=IGNORE_X) object_position[0] = x;
	if(y!=IGNORE_Y) object_position[1] = y;
	simxSetObjectPosition(clientID, handle, -1, object_position, simx_opmode_oneshot_wait);

	//result = simxSetModelProperty(clientID, handle, org_prop, simx_opmode_oneshot_wait);
}

void vrep_sim_step(void)
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
		//printf("dT(sim) = %d,  dT(real)=%d,   dT(model)=%d,  ping time = %d\n", t_sim-t_sim_last,  t_real_now-t_real_last,  t_m-t_m_last, pingTime);
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
			const t_xyt rls[]=
			{
				{0.9,2.2,-1.571}, //home
				{0.2758,1.557,1.389}, //Rm 3
				{0.709,0.299,3.028}, //Rm 2
				{1.255,0.208,0.060}, //Rm 1a (bottom)
				{1.239,0.625,-0.036}, //Rm 1b (top)
				{2.23, 0.54, 1.59}, //just before exiting Rm 1 via the north side door
				{1.7, 1.9, -1.644}  //Rm 4 (north side)
			};
			const int start_state[] = {2, 4, 6, 8, 8, 9, 10};
			static int rl = 0;

			m.start_location = start_state[rl];
			move_object("Robot",rls[rl].x, rls[rl].y, rls[rl].t);
			rl++;
			if(rl>6) rl=0;
		}

		if(c=='a')
		{
			s.behavior_state[MASTER_LOGIC_FSM]=m.start_location;
			result = simxSetModelProperty(clientID,robot,0,simx_opmode_oneshot_wait);
		}

		if(c=='b')
		{
			s.behavior_state[FIND_WALL_FSM]=1;
			result = simxSetModelProperty(clientID,robot,0,simx_opmode_oneshot_wait);
		}

		if (c == '0') //assume candle is in room 1
		{
			m.candle_location = 0;
			printf("No candle\n");
		}

		if (c == '1') //assume candle is in room 1
		{
			static int door_configuration = 0;

			m.candle_location = 1;
			move_candle(1.355, 0.845);
			if (door_configuration) { move_object("wall_E", 1.23, 0.23, IGNORE_THETA); door_configuration = 0; }
			else { move_object("wall_E", 1.23, 0.68, IGNORE_THETA); door_configuration = 1; }
			printf("Candle will be in room #1.  door location = %d\n", door_configuration);
		}

		if (c == '2') //assume candle is in room 1
		{
			const t_xy location[] = { { 0.63, 0.97 }, { 0.13, 0.97 }, { 0.13, 0.12 }, { 0.655, 0.545 }, { 0.33, 0.32 } };
			static int cc = 0;
			cc++;
			if (cc > 4) cc = 0;
			m.candle_location = 2;
			move_candle(location[cc].x, location[cc].y);
			printf("Candle will be in room #2, location %d\n",cc);
		}

		if (c == '3') //assume candle is in room 1
		{
			const t_xy location[] = { { 0.655, 1.62 }, { 0.655, 2.32 }, { 0.08, 2.32 }, { 0.305, 2.095 } };
			static int cc = 0;
			cc++;
			if (cc > 3) cc = 0;
			m.candle_location = 3;
			move_candle(location[cc].x,location[cc].y);
			printf("Candle will be in room #3, location %d\n", cc);
		}

		if (c == '4') //assume candle is in room 1
		{
			const t_xy location[] = { { 1.845, 1.475 } };
			static int cc = 0;
			m.candle_location = 4;
			move_candle(location[cc].x,location[cc].y);
			printf("Candle will be in room #3, location %d\n", cc);
		}



		if(c=='s')
		{
			//m.start_signal=1;
			force_start_signal(1);
			result = simxSetModelProperty(clientID,robot,0,simx_opmode_oneshot_wait);

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



void vrep_sim_outputs(void)
{
	//motors
	simxSetJointTargetVelocity(clientID,lm,((((float) m.m2)/1.83f)/5.19695f)*1.0f,STREAMING_MODE);			
	simxSetJointTargetVelocity(clientID,rm,(((float) m.m1)/1.83f)/5.19695f,STREAMING_MODE);		

	//servos
	simxSetJointTargetPosition(clientID,pan,(((float)m.servo[1])-1250.0f)/353.0f,STREAMING_MODE);
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


void vrep_sim_inputs(void)
{
	static int ir_update_countdown=2;
	static int sonar_update_countdown=2;
	int pingTime=999;
	int result;
	static int t_sim,t_sim_last=0;
	static u32 t_real_now, t_real_last=0; 
	static u32 t_m,t_m_last=0;
	t_config_value v;

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
	//s.inputs.sonar[1] = 4000;
	//s.inputs.sonar[2] = 4000;
	//s.inputs.sonar[3] = 4000;

	if(m.start_signal)	
	{ 
		s.inputs.analog[AI_START_BUTTON]=0; 
	}
	else
	{
		s.inputs.analog[AI_START_BUTTON]=255;
	}


	v.u08 = 61;  cfg_set_value_by_grp_id(6,1, v); //black
	v.u08 = 60;  cfg_set_value_by_grp_id(6,2, v); //white


	//------------------------------------------------------------------------------------------------------------------------------------
	//down-facing line sensors
	auxValues = NULL; auxValuesCount = NULL;
	result = simxReadVisionSensor(clientID,line_left,&state,&auxValues,&auxValuesCount,STREAMING_MODE);
	if(result==0)
	{
		//printf("aVC[0]=%d, aVC[1]=%d, av[0]=%f,av[10]=%f,\n",auxValuesCount[0],auxValuesCount[1],auxValues[0],auxValues[10]);
		s.line[LEFT_LINE] = (1.0f-auxValues[10])*255;
		s.inputs.analog[AI_LINE_LEFT] = s.line[LEFT_LINE];
	}
	if(auxValues)simxReleaseBuffer((simxUChar*)auxValues);
	if(auxValuesCount)simxReleaseBuffer((simxUChar*)auxValuesCount);

	auxValues = NULL; auxValuesCount = NULL;
	result = simxReadVisionSensor(clientID,line_right,&state,&auxValues,&auxValuesCount,STREAMING_MODE);
	if(result==0)
	{
		//printf("aVC[0]=%d, aVC[1]=%d, av[0]=%f,av[10]=%f,\n",auxValuesCount[0],auxValuesCount[1],auxValues[0],auxValues[10]);
		s.line[RIGHT_LINE] = (1.0f-auxValues[10])*255;
		s.inputs.analog[AI_LINE_RIGHT] = s.line[RIGHT_LINE];
	}
	if(auxValues)simxReleaseBuffer((simxUChar*)auxValues);
	if(auxValuesCount)simxReleaseBuffer((simxUChar*)auxValuesCount);
	//printf("%3d,%3d\n",s.line[LEFT_LINE],s.line[RIGHT_LINE]);
	//------------------------------------------------------------------------------------------------------------------------------------



	//------------------------------------------------------------------------------------------------------------------------------------
	//Flame sensors
	auxValues = NULL; auxValuesCount = NULL;
	result = simxReadVisionSensor(clientID,flame1,&state,&auxValues,&auxValuesCount,STREAMING_MODE);
	if(result==0)
	{
		float flame;
		//printf("aVC[0]=%d, aVC[1]=%d, av[0]=%f,av[13]=%f,\n",auxValuesCount[0],auxValuesCount[1],auxValues[0],auxValues[13]*4000);
		flame = (auxValues[13]*256.0f);
		if(flame > 255.0f) flame=255.0f;
		s.inputs.analog[AI_FLAME_NE] = (u08)flame;
		//printf("auxValues[13]=%f    s.inputs.analog[AI_FLAME_N]=%d\n",auxValues[13],s.inputs.analog[AI_FLAME_N]);
	}
	if(auxValues)simxReleaseBuffer((simxUChar*)auxValues);
	if(auxValuesCount)simxReleaseBuffer((simxUChar*)auxValuesCount);

	auxValues = NULL; auxValuesCount = NULL;
	result = simxReadVisionSensor(clientID,flame2,&state,&auxValues,&auxValuesCount,STREAMING_MODE);
	if(result==0)
	{
		float flame;
		//printf("aVC[0]=%d, aVC[1]=%d, av[0]=%f,av[13]=%f,\n",auxValuesCount[0],auxValuesCount[1],auxValues[0],auxValues[13]*4000);
		flame = (auxValues[13]*256.0f);
		if(flame > 255.0f) flame=255.0f;
		s.inputs.analog[AI_FLAME_NW] = (u08)flame;
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

	if(1)
	{
		static double enc_ab=0, enc_cd=0;
		enc_ab += lticks;
		enc_cd += rticks;
		//printf("l=%d, r=%d\n",enc_ab,enc_cd);

		printf("actual x,y,theta = %7.4f, %7.4f, %7.4frad/%7.4fdeg   l,r=%7.3f,%7.3f   ab,cd=%7.3f,%7.3f   calc x,y,theta = %7.4f, %7.4f, %7.4f\n",
			sim_state.robot_position[0],sim_state.robot_position[1], sim_state.robot_orientation[2], sim_state.robot_orientation[2] * (180.0f/3.1415926535897932384626433832795f),
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
		s.inputs.ir[IR_N]  = s.ir[IR_N]			= 600; //(u16)read_ir(ir0,60,600,0.0002);

		//s.inputs.ir[3] = s.ir[IR_N_long]		= (u16)read_ir(ir0,60,600,0.0002);

		s.inputs.ir[IR_E]	= s.ir[IR_E]		= (u16)read_ir(ir2,40,400,0.0002);
		s.inputs.ir[IR_NR]	= s.ir[IR_NR]		= (u16)read_ir(ir3,40,400,0.0002);
		s.inputs.ir[IR_NL]	= s.ir[IR_NL]		= (u16)read_ir(ir5,40,400,0.0002);
		s.inputs.ir[IR_W]	= s.ir[IR_W]		= (u16)read_ir(ir6,40,400,0.0002);
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
	{
		static unsigned char sensor=0;
		simxInt sensors[] = {sonar0, sonar1, sonar2, sonar3, sonar4}; 
		float point[3],surface[3];
		float distance;
		int handle;
		float noise;
		float noise_factor = 0.0002f; //  +/- 2%

		result = simxReadProximitySensor(clientID,sensors[sensor],&state,&(point[0]),&handle,&(surface[0]),STREAMING_MODE);
		distance = 4000;
		if(state) 
		{
			distance=((point[2]*100.0f)/2.54f)*10.0f;
			//if(distance < 40) distance = 40 + (40-distance);
			noise = 100 - (rand() % 200);
			noise = noise*noise_factor;
			distance += distance * noise;
		}
		s.inputs.sonar[sensor] = (u16)distance;
		sensor++;
		if(sensor>4) sensor=0;
	}
	//------------------------------------------------------------------------------------------------------------------------------------



	
}


void vrep_sim_init(void)
{
	int result;
	int pingTime;

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

	simxGetObjectHandle(clientID,"flame1",&flame1,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"flame2",&flame2,simx_opmode_oneshot_wait);

	simxGetObjectHandle(clientID,"sonar0",&sonar0,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"sonar1",&sonar1,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"sonar2",&sonar2,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"sonar3",&sonar3,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID,"sonar4",&sonar4,simx_opmode_oneshot_wait);

	simxGetObjectHandle(clientID,"Robot",&robot,simx_opmode_oneshot_wait);
	simxGetObjectHandle(clientID, "Candle", &candle, simx_opmode_oneshot_wait);

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

	//start out w/ Robot properties such that we can programmatically move it
	{
		int prop;
		result = simxGetModelProperty(clientID,robot,&prop,simx_opmode_oneshot_wait);
		prop |= sim_modelproperty_not_dynamic;
		prop |= sim_modelproperty_not_respondable;
		result = simxSetModelProperty(clientID,robot,prop,simx_opmode_oneshot_wait);
	}


#if 0
	//when playing back previously recorded data, we need to programmatically move the robot, but to do this, it must not be dynamic.
	if(1)
	{
		int prop;
		result = simxGetModelProperty(clientID,robot,&prop,simx_opmode_oneshot_wait);
		prop |= sim_modelproperty_not_dynamic;
		prop |= sim_modelproperty_not_respondable;
		result = simxSetModelProperty(clientID,robot,prop,simx_opmode_oneshot_wait);
	}

	while(1)
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
#endif
}
