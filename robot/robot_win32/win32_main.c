
#include <Windows.h>
#include <timeapi.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include "cocoos/cocoos.h"

#include "standard_includes.h"

extern void vrep_sim_init(void);
extern void vrep_sim_inputs(void);
extern void vrep_sim_outputs(void);
extern void vrep_sim_step(void);

extern void playback_sim_init(char *path);
extern void playback_sim_inputs(void);
extern void playback_sim_outputs(void);
extern void playback_sim_step(void);


int sim_mode=0;

void sim_step(void)
{
	if(sim_mode==0) vrep_sim_step();
	if(sim_mode==1) playback_sim_step();
}



void sim_outputs(void)
{
	if(sim_mode==0) vrep_sim_outputs();
	if(sim_mode==1) playback_sim_outputs();
}


void sim_inputs(void)
{
	if(sim_mode==0) vrep_sim_inputs();
	if(sim_mode==1) playback_sim_inputs();
}


void win32_main(int argc, char **argv)
{
	int result;
	int pingTime;
	timeBeginPeriod(1);
	srand(0);

	if(argc>1) 
	{
		sim_mode=1;
	}

	if(sim_mode==0) vrep_sim_init();
	if(sim_mode==1) playback_sim_init(argv[1]);
}
