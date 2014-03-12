
#include "standard_includes.h"

//use this as a template...

void fsm_test_task(void)
{
	enum states { s_none=0, s_disabled=1, s_foo };
	static enum states state=s_disabled;
	static enum states last_state=s_none;
	
	task_open();

	while(1)
	{
		first_(s_disabled)
		{
			enter_(s_disabled)  //required!  this updates the "last_state" variable!
			{  
			}

			state = s_foo;

			leave_(s_disabled);

			exit_(s_disabled)  
			{ 
			}
		}
	}
	task_close();
}

