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
	printf("PC <-> Robot Interface v1.0  (UI version)\n\n");
	printf("sizeof(t_inputs)      = %3d\n",sizeof(t_inputs));
	printf("sizeof(t_frame_to_pc) = %3d\n",sizeof(t_frame_to_pc));

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
		bw1->RunWorkerAsync();
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	f1::f1(void)
	{
		int i;
		float f;
		String ^s;

		//this->SetStyle(ControlStyles::DoubleBuffer, true);

		timeBeginPeriod(1);

		DWORD t1 = timeGetTime();
		Sleep(1);
		DWORD t2 = timeGetTime();

		ignore_parameter_changes = 1;

		random = gcnew Random();

		UpdateUI_delegate = gcnew UpdateUI( this, &f1::UpdateUI_method );
		Update_textBox1_delegate =   gcnew UpdateUI( this, &f1::Update_textBox1_method );
		Update_textBoxLog_delegate = gcnew UpdateUI( this, &f1::Update_textBoxLog_method );
		UpdateChart_delegate = gcnew UpdateChart( this, &f1::UpdateChart_method );

		InitializeComponent();

		InitializeGraphsTab();

		InitializeParametersTab();

		printf("Sleep(1) = %lu\n",t2-t1);

		g = panel1->CreateGraphics();
		//
		//TODO: Add the constructor code here
		//
		ignore_parameter_changes = 0;
	}




	System::Void f1::btn_estop_Click(System::Object^  sender, System::EventArgs^  e) 
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
		tb_Vbatt->Text = Convert::ToString(((float)s.inputs->vbatt)/1000.0f)+"V";
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::bw1_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) 
	{
		int result = 0;
		static float theta = 0.0f;

		data_init();

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
					/*  the following is already done inside loop()
					inputs = s.inputs;
					inputs_history[history_index] = *inputs;
					history_index++;
					*/

					update_ui();
				}

				//skip--; if(skip==0) { term(s.msg); skip=5;}
			}
		}
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		static HANDLE h;
		BOOL result;
		char port[30];
		System::String ^str;
		str = comboBox1->Text;
		char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi(str);
		//printf(str2);

		sprintf(port,"\\\\.\\COM%s",str2);
		sprintf(s.port,"\\\\.\\COM%s",str2);

		if(checkBox1->Checked)
		{
			log("Opening serial port COM" + str + "\r\n" );
			s.p = serial_init(port,115200,ONESTOPBIT);
			h = s.p;
			printf("h,s.p:  0x%08x, 0x%08x\n",h,s.p);
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
				printf("h,s.p:  0x%08x, 0x%08x\n",h,s.p);
				t1->Enabled = FALSE;
				checkBox2->Checked = false;
			}
		}

		Marshal::FreeHGlobal(System::IntPtr((void*)str2));
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::Update_textBox1_method(String ^str)
	{
		textBox1->AppendText(str);
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::Update_textBoxLog_method(String ^str)
	{
		textBoxLog->AppendText(str);
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	//for the "cmd" text box, basically for catching up,down,left,right etc keystrokes
	System::Void f1::textBox2_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
	{
		String ^s = e->KeyData.ToString();
		e->Handled = true;
		log(s + "\n");
		textBox2->Clear();
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

		printf("Key = 0x%04x\n",key);
	}
}