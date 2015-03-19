#include "stdafx.h"
#include "f1.h"
#include <stdio.h>
#include <windows.h>


FILE *capture_file = NULL;

namespace robot_ui 
{

	System::Void f1::capture_btn_start_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		SYSTEMTIME st;
		char filename[100];
		capture_btn_start->Enabled = false;
		capture_comboBox_path->Enabled = false;
		capture_btn_stop->Enabled = true;
		GetLocalTime(&st);
		sprintf(filename,"c:\\temp\\%4d-%02d-%02d-%02d-%02d-%02d-capture_file.txt",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
		capture_file = fopen(filename,"w");

		capture_timer->Enabled = true;
	}

	System::Void f1::capture_btn_stop_Click(System::Object^  sender, System::EventArgs^  e) 
	{	 
		capture_btn_start->Enabled = true;
		capture_comboBox_path->Enabled = true;
		capture_btn_stop->Enabled = false;

		capture_timer->Enabled = false;
		fflush(capture_file);
		fclose(capture_file);
	}

	System::Void f1::capture_timer_Tick(System::Object^  sender, System::EventArgs^  e) 
	{	 
		static int x=0;
		t_inputs i;

		if(history_index - x > 2000) x=history_index-2000;
		if(x<0) x=0;
		if(x>history_index) x=history_index; //wrap-around

		while(x<history_index)
		{
			memcpy(&i,&(inputs_history[x]),sizeof(t_inputs));
			if(capture_file)
			{
				fprintf(capture_file,
					"%7ld,%5d,%5d,"
					"%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,"
					"%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,"
					"%4d,%4d,%4d,%4d,%4d,%4d,"
					"%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,"
					"%6d,%6d,"
					"%4.1f,%4.1f,%4.1f,"
					"%4d,%4d,"
					"%4d,%4d,"
					"%4d,%4d,"
					"%2d"
					"\n",
					i.timestamp, i.vbatt, i.flags,
					i.analog[0], i.analog[1], i.analog[2], i.analog[3], i.analog[4], i.analog[5], i.analog[6], i.analog[7],
					i.analog[8], i.analog[9], i.analog[10], i.analog[11], i.analog[12], i.analog[13], i.analog[14], i.analog[15],
					i.sonar[0], i.sonar[1], i.sonar[2], i.sonar[3], i.sonar[4], i.sonar[5],
					i.ir[0], i.ir[1], i.ir[2], i.ir[3], i.ir[4], i.ir[5], i.ir[6], i.ir[7],
					i.encoders[0], i.encoders[1],
					i.x, i.y, i.theta,
					i.target_speed[0], i.target_speed[1],
					i.actual_speed[0], i.actual_speed[1],
					i.motors[0], i.motors[1],
					i.watch[2]
				);
				fflush(capture_file);
			}
			x++;
		}
	}

}
