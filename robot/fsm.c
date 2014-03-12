
#include "standard_includes.h"


void fsm_test_task(void)
{
	static char state=1, last_state=-1;
	
	task_open();
	while(1)
	{
		first_(1)
		{
			ENTER(1)
			{
				//dbg("Entering state %d\n",state);
				NOP();
			}
			
			state = 2;
			leave_(1);
			//SWITCH_STATE(1,2);
			NOP();
			
			EXIT(1)
			{
				NOP();
			}
		}
		NEXT_STATE(2)
		{
			enter_(2)
			{
				//dbg("Entering state %d\n",state);
				NOP();
			}
			
			state = 1;
			
			EXIT(2)
			{
				NOP();
			}
		}
		NOP();
		OS_SCHEDULE; //task_wait(1);
	}
	task_close();
}

