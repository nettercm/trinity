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
