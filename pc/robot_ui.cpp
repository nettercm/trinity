// robot_ui.cpp : main project file.

#include "stdafx.h"
#include "f1.h"
#include <stdio.h>
#include <windows.h>


using namespace robot_ui;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	AllocConsole();
	freopen("CON","wb",stdout);
	log_printf("PC <-> Robot Interface v1.0  (UI version)\n\n");
	log_printf("sizeof(t_inputs)      = %3d\n",sizeof(t_inputs));
	log_printf("sizeof(t_frame_to_pc) = %3d\n",sizeof(t_frame_to_pc));

	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew f1());
	return 0;
}



namespace robot_ui 
{
	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::f1_Load(System::Object^  sender, System::EventArgs^  e) 
	{
		main_serial_thread->RunWorkerAsync();
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	f1::f1(void)
	{
		int i;
		float f;
		String ^s;

		log_printf("++ f1::f1(void)\n");

		timeBeginPeriod(1);

		DWORD t1 = timeGetTime();
		Sleep(1);
		DWORD t2 = timeGetTime();

		ignore_parameter_changes = 1;

		random = gcnew Random();

		UpdateUI_delegate = gcnew UpdateUI( this, &f1::UpdateUI_method );
		Update_terminal_txt_delegate =   gcnew UpdateUI( this, &f1::Update_terminal_txt_method );
		Update_log_txt_delegate = gcnew UpdateUI( this, &f1::Update_log_txt_method );
		UpdateChart_delegate = gcnew UpdateChart( this, &f1::UpdateChart_method );

		InitializeComponent();

		InitializeGraphsTab();

		InitializeParametersTab();

		log_printf("Timing test:  Sleep(1) = %lu\n",t2-t1);

		g = radar_tabPage->CreateGraphics();
		//
		//TODO: Add the constructor code here
		//
		ignore_parameter_changes = 0;

		log_printf("-- f1::f1(void)\n");
	}




	System::Void f1::main_btn_estop_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		log("STOP!!!\n");
		CMD_set_behavior_state(1,0);	CMD_send();
		CMD_set_behavior_state(2,0);	CMD_send();
		CMD_set_behavior_state(3,0);	CMD_send();
		CMD_set_behavior_state(4,0);	CMD_send();
		CMD_set_behavior_state(10,0);	CMD_send();
		CMD_set_behavior_state(11,0);	CMD_send();
		CMD_motor_command(2,0,0,0,0);	CMD_send();

	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::UpdateUI_method(String ^str)
	{
		main_textBox_vbatt->Text = Convert::ToString(((float)s.inputs->vbatt)/1000.0f)+"V";
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::main_serial_thread_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) 
	{
		int result = 0;
		static float theta = 0.0f;

		//data_init();

		#if 1

		while(1)
		{
			if(s.p == INVALID_HANDLE_VALUE)
			{
				Sleep(20);
				inputs_history[history_index].analog[0] = history_index;
				inputs_history[history_index].theta = theta;
				inputs_history[history_index].ir[0] = 100;
				inputs_history[history_index].ir[1] = 200;
				inputs_history[history_index].ir[2] = 300;
				inputs_history[history_index].ir[3] = 400;
				history_index++;
				theta += (PI/180.0f);
				if(theta >= 2.0f*PI) 
				{
					theta = 0.0f;
				}
				//log(".");
			}
			else
			{
				result = loop();
				if(result)
				{
					update_ui();
				}
			}
		}

	#else
		while(1)
		{
			Sleep(100);
		}
	#endif
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::serial_timer_Tick(System::Object^  sender, System::EventArgs^  e) 
	{
		int result = 0;
		static float theta = 0.0f;
		
		#if 0

		if(s.p == INVALID_HANDLE_VALUE)
		{
			//Sleep(20);
			inputs_history[history_index].analog[0] = history_index;
			inputs_history[history_index].theta = theta;
			inputs_history[history_index].ir[0] = 100;
			inputs_history[history_index].ir[1] = 200;
			inputs_history[history_index].ir[2] = 300;
			inputs_history[history_index].ir[3] = 400;
			history_index++;
			theta += (PI/180.0f);
			if(theta >= 2.0f*PI) 
			{
				theta = 0.0f;
			}
			//log(".");
		}
		else
		{
			result = loop();
			if(result)
			{
				main_textBox_vbatt->Text = Convert::ToString(((float)s.inputs->vbatt)/1000.0f)+"V";
			}
		}

		#endif
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::main_checkBox_connect_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		static HANDLE h;
		BOOL result;
		char port[30];
		System::String ^str;
		str = main_comboBox_port->Text;
		char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi(str);
		//printf(str2);

		sprintf(port,"\\\\.\\COM%s",str2);
		sprintf(s.port,"\\\\.\\COM%s",str2);

		if(main_checkBox_connect->Checked)
		{
			log("Opening serial port COM" + str + "\r\n" );
			s.p = serial_init(port,115200,ONESTOPBIT);
			h = s.p;
			log_printf("h,s.p:  0x%08x, 0x%08x\n",h,s.p);
			if(s.p == INVALID_HANDLE_VALUE) 
			{
				log("Unable to open the serial port\r\n");
			}
			else
			{
				log("Port open!\r\n");
			}
		}
		else
		{
			sprintf(s.port,"");
			printf("h,s.p:  0x%08x, 0x%08x\n",h,s.p);
			if(s.p != INVALID_HANDLE_VALUE)
			{
				s.p = INVALID_HANDLE_VALUE;
				Sleep(100);
				sprintf(s.port,"");
				log("Closing the serial port\r\n");
				result = CloseHandle(h); if(!result) show_last_error("CloseHandle()");
				log_printf("h,s.p:  0x%08x, 0x%08x\n",h,s.p);
				graphs_timer->Enabled = FALSE;
				graphs_checkBox_enable->Checked = false;
			}
		}

		Marshal::FreeHGlobal(System::IntPtr((void*)str2));
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::Update_terminal_txt_method(String ^str)
	{
		terminal_txt->AppendText(str);
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::Update_log_txt_method(String ^str)
	{
		log_txt->AppendText(str);
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	//for the "cmd" text box, basically for catching up,down,left,right etc keystrokes
	System::Void f1::main_textBox_keyb_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
	{
		String ^s = e->KeyData.ToString();
		e->Handled = true;
		log(s + "\n");
		main_textBox_keyb->Clear();
		key=0;

		if(s == "Left"	) key=KEY_LEFT;
		if(s == "Right"	) key=KEY_RIGHT;
		if(s == "Up"	) key=KEY_UP;
		if(s == "Down"	) key=KEY_DOWN;

		if(s == "Space" ) key=0x0020;

		if(s == "F1"	) key=KEY_F1;
		if(s == "F2"	) key=KEY_F2;
		if(s == "F3"	) key=KEY_F3;
		if(s == "F10"	) key=KEY_F10;
		if(s == "F11"	) key=KEY_F11;
		if(s == "F12"	) key=KEY_F12;

		log_printf("Key = 0x%04x\n",key);
	}


	System::Void f1::main_comboBox_behavior_id_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		update_interval = Convert::ToInt32(main_comboBox_rate->Text);
	}

	System::Void f1::main_btn_start_beh_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		CMD_set_behavior_state(Convert::ToByte(main_comboBox_behavior_id->Text), Convert::ToByte(main_comboBox_behavior_state->Text));
		CMD_send();
	}

	System::Void f1::main_main_btn_stop_beh_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		CMD_set_behavior_state(Convert::ToByte(main_comboBox_behavior_id->Text), 0);
		CMD_send();
	}
}