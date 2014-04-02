
#include "standard_includes.h"

//use this as a template...

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

