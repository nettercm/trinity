
#include "standard_includes.h"

//use this as a template...


void stop_all_fsm(void)
{
	uint8 i;
	
	for(i=0;i<16;i++) s.behavior_state[i] = 0;
}


void fsm_test_task(u08 cmd, u08 *param)
{
	enum states { s_none=0, s_disabled=1, s_foo };
	static enum states state=s_disabled;
	static enum states last_state=s_none;
	static u32 t_entry=0;
	static u32 t_now=0;
	
	task_open();

	for(;;)
	{
		t_now = get_ms();

		first_(s_disabled)
		{
			enter_(s_disabled)  //required!  this updates the "last_state" variable!
			{  
				NOP();
			}

			if(time_since_entry_() > 1000) state = s_foo;


			exit_(s_disabled)  
			{ 
				NOP();
			}
		}

		next_(s_foo)
		{
			enter_(s_foo)  //required!  this updates the "last_state" variable!
			{  
				NOP();
			}

			if(time_since_entry_() > 1000) state = s_disabled;

			exit_(s_foo)  
			{ 
				NOP();
			}
		}
		task_wait(1);
	}
	task_close();
}


#if 1

volatile int _dummy2_;
#define nop() {_dummy2_++;}

#define STATE(s)  if(state==s) 
#define ON_ENTRY() if(( last_state != state ? ((last_state=state)+1) : 0))
#define ON_EXIT() if(state!=last_state)
//#define END_STATE()		if(last_state != state) goto fsm_end
//#define END_FSM() fsm_end: {}
//#define BEGIN_FSM() {}


void fsm_test_2(void)
{
	static int state=1, last_state=0, counter=0;

	//BEGIN_FSM();

	//--------------------------------------------------------------------------------------------------------
	STATE(1)
	{
		ON_ENTRY()
		{
			printf("entering state %d\n",state);
			nop();
			counter=0;
		}

		printf("during state %d\n",state);

		counter++;
		printf("1-a\n");
		if(counter>2) { state = 2; }
		printf("1-b\n");

		ON_EXIT()
		{
			printf("leaving state %d - going to state %d\n",last_state,state);
			nop();
			counter=0;
		}
	}
	//END_STATE();
	//--------------------------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------------------------
	STATE(2)
	{
		ON_ENTRY()
		{
			printf("entering state %d\n",state);
			nop();
			counter=0;
		}

		printf("during state %d\n",state);

		counter++;
		printf("2-a\n");
		if(counter>2) { state = 1;  goto _exit_from_state_2; }
		printf("2-b\n");

		_exit_from_state_2: 
		ON_EXIT()
		{
			printf("leaving state %d - going to state %d\n",last_state,state);
			nop();
			counter=0;
		}
	}
	//END_STATE();
	//--------------------------------------------------------------------------------------------------------

	//END_FSM();
	delay_ms(200);
}









#endif