
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include <math.h>
#include <hidsdi.h>

#include <XInput.h>
#pragma comment(lib,"xinput9_1_0.lib")

#include "debug.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define MAX_CONTROLLERS 4  // XInput handles up to 4 controllers 
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

typedef struct 
{
    XINPUT_STATE state;
    int bConnected;
} CONTROLLER_STATE;

CONTROLLER_STATE g_Controllers[MAX_CONTROLLERS];
WCHAR g_szMessage[4][1024] = {0};
HWND    g_hWnd;
int    g_bDeadZoneOn = 0;


#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#define WC_MAINFRAME	TEXT("MainFrame")
#define MAX_BUTTONS		128
#define CHECK(exp)		{ if(!(exp)) goto Error; }
#define SAFE_FREE(p)	{ if(p) { HeapFree(hHeap, 0, p); (p) = NULL; } }


volatile BOOL bButtonStates[MAX_BUTTONS];
volatile LONG joystick_Lx_1, joystick_Lx_2;
volatile LONG joystick_Ly_1, joystick_Ly_2;
volatile LONG joystick_Rx_1, joystick_Rx_2;   //right stick - left/right, i.e X axis
volatile LONG joystick_Ry_1, joystick_Ry_2; //right stick - up/down, i.e. Y axis
volatile LONG lHat_1, lHat_2;
volatile INT  g_NumberOfButtons;
volatile LONG joystick_changed_R=0,joystick_changed_L=0;


//-----------------------------------------------------------------------------
void UpdateControllerState(void)
{
    DWORD dwResult;
	DWORD i;
    for( i = 0; i < MAX_CONTROLLERS; i++ )
    {
        // Simply get the state of the controller from XInput.
        dwResult = XInputGetState( i, &g_Controllers[i].state );

        if( dwResult == ERROR_SUCCESS )
		{
            g_Controllers[i].bConnected = 1;

			joystick_Rx_2 =  g_Controllers[i].state.Gamepad.sThumbRX / -256;
			if(abs(joystick_Rx_2)<=3) joystick_Rx_2=0;
			if(joystick_Rx_2 != joystick_Rx_1) { joystick_changed_R++; 	joystick_Rx_1=joystick_Rx_2; }

			joystick_Ry_2 = g_Controllers[i].state.Gamepad.sThumbRY / 256;
			if(abs(joystick_Ry_2)<=3) joystick_Ry_2=0;
			if(joystick_Ry_2 != joystick_Ry_1) { joystick_changed_R++; joystick_Ry_1=joystick_Ry_2; }

			joystick_Lx_2 =  g_Controllers[i].state.Gamepad.sThumbLX / -200;
			if(abs(joystick_Lx_2)<=3) joystick_Lx_2=0;
			if(joystick_Lx_2 != joystick_Lx_1) { joystick_changed_L++; 	joystick_Lx_1=(joystick_Lx_1+joystick_Lx_2)/2; 	}

			joystick_Ly_2 = g_Controllers[i].state.Gamepad.sThumbLY / 256;
			if(abs(joystick_Ly_2)<=3) joystick_Ly_2=0;
			if(joystick_Ly_2 != joystick_Ly_1) { joystick_changed_L++; 	joystick_Ly_1=(joystick_Ly_1+joystick_Ly_2)/2;  }
		}
        else
		{
            g_Controllers[i].bConnected = 0;
		}
    }
}


void ParseRawInput(PRAWINPUT pRawInput)
{
	PHIDP_PREPARSED_DATA pPreparsedData;
	HIDP_CAPS            Caps;
	PHIDP_BUTTON_CAPS    pButtonCaps;
	PHIDP_VALUE_CAPS     pValueCaps;
	USHORT               capsLength;
	UINT                 bufferSize;
	HANDLE               hHeap;
	USAGE                usage[MAX_BUTTONS];
	ULONG                i, usageLength, value;
	int					 changed=0;
	static unsigned long		 t1=0,t2,count=0;


	pPreparsedData = NULL;
	pButtonCaps    = NULL;
	pValueCaps     = NULL;
	hHeap          = GetProcessHeap();

	count++;
	t2=GetTickCount();
	if(t2-t1>=1000)
	{
		t1=t2;
		//log_printf("%lu messages per second\n",count);
		count=0;
	}

	//
	// Get the preparsed data block
	//

	CHECK( GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize) == 0 );
	CHECK( pPreparsedData = (PHIDP_PREPARSED_DATA)HeapAlloc(hHeap, 0, bufferSize) );
	CHECK( (int)GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize) >= 0 );

	//
	// Get the joystick's capabilities
	//

	// Button caps
	CHECK( HidP_GetCaps(pPreparsedData, &Caps) == HIDP_STATUS_SUCCESS )
		CHECK( pButtonCaps = (PHIDP_BUTTON_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps) );

	capsLength = Caps.NumberInputButtonCaps;
	CHECK( HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData) == HIDP_STATUS_SUCCESS )
		g_NumberOfButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;

	// Value caps
	CHECK( pValueCaps = (PHIDP_VALUE_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * Caps.NumberInputValueCaps) );
	capsLength = Caps.NumberInputValueCaps;
	CHECK( HidP_GetValueCaps(HidP_Input, pValueCaps, &capsLength, pPreparsedData) == HIDP_STATUS_SUCCESS )

		//
		// Get the pressed buttons
		//

	usageLength = g_NumberOfButtons;
	CHECK(
		HidP_GetUsages(
		HidP_Input, pButtonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData,
		(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
		) == HIDP_STATUS_SUCCESS );

	ZeroMemory((void*)bButtonStates, sizeof(bButtonStates));
	for(i = 0; i < usageLength; i++)
	{
		bButtonStates[usage[i] - pButtonCaps->Range.UsageMin] = TRUE;
	}

	//
	// Get the state of discrete-valued-controls
	//

	for(i = 0; i < Caps.NumberInputValueCaps; i++)
	{
		CHECK(
			HidP_GetUsageValue(
			HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin, &value, pPreparsedData,
			(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
			) == HIDP_STATUS_SUCCESS );

		switch(pValueCaps[i].Range.UsageMin)
		{
		case 0x30:	// X-axis
			joystick_Lx_2 = (128-(LONG)value)/1;// - 128;
			if(abs(joystick_Lx_2)<=3) joystick_Lx_2=0;
			if(joystick_Lx_2 != joystick_Lx_1) changed++;
			joystick_Lx_1=joystick_Lx_2;
			break;

		case 0x31:	// Y-axis
			joystick_Ly_2 = (128-(LONG)value)/1;// - 128;
			if(abs(joystick_Ly_2)<=3) joystick_Ly_2=0;
			if(joystick_Ly_2 != joystick_Ly_1) changed++;
			joystick_Ly_1=joystick_Ly_2;
			break;

		case 0x32: // Z-axis
			joystick_Rx_2 = (128-(LONG)value)/1;// - 128;
			if(abs(joystick_Rx_2)<=3) joystick_Rx_2=0;
			if(joystick_Rx_2 != joystick_Rx_1) changed++;
			joystick_Rx_1=joystick_Rx_2;
			break;

		case 0x35: // Rotate-Z
			joystick_Ry_2 = (128-(LONG)value)/1;// - 128;
			if(abs(joystick_Ry_2)<=3) joystick_Ry_2=0;
			if(joystick_Ry_2 != joystick_Ry_1) changed++;
			joystick_Ry_1=joystick_Ry_2;
			break;

		case 0x39:	// Hat Switch
			lHat_2 = value;
			if(lHat_2 != lHat_1) changed++;
			lHat_1=lHat_2;
			break;
		}
	}
	if(changed) 
	{
		joystick_changed_R++;
		//log_printf("%5d,%5d,%5d,%5d,%5d\n",joystick_Lx_1,joystick_Ly_1,joystick_Rx_1,joystick_Ry_1,lHat_1);
	}
	//
	// Clean up
	//

Error:
	SAFE_FREE(pPreparsedData);
	SAFE_FREE(pButtonCaps);
	SAFE_FREE(pValueCaps);
}

//******************************************************************************************************************************

void process_wm_input(LPARAM lParam)
{
	PRAWINPUT pRawInput;
	UINT      bufferSize;
	HANDLE    hHeap;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));

	hHeap     = GetProcessHeap();
	pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, bufferSize);
	if(!pRawInput)	return ;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));
	ParseRawInput(pRawInput);

	HeapFree(hHeap, 0, pRawInput);
}

//******************************************************************************************************************************

void register_joystick(HWND hwnd)		
{
	BOOL result;
	//
	// Register for joystick devices
	//

	RAWINPUTDEVICE rid;

	rid.usUsagePage = 1;
	rid.usUsage     = 4;	// Joystick
	rid.dwFlags     = 0;
	rid.hwndTarget  = hwnd; //static_cast<HWND>(Handle.ToPointer());

	result = RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));
}

//******************************************************************************************************************************

void enumerate_raw_input(void)
{
	UINT i;
	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;

	// Pass a null pointer to find out how many devices there are.
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) { log_printf("error 1\n"); }

	// Now, malloc the needed storage, based on the output parameter, NOT the return value.
	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST) malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) { log_printf("error 2\n");}

	// Finally, call for real, passing in the newly allocated buffer.
	if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == (-1)) { log_printf("error 3\n"); }

	for(i=0;i<nDevices;i++)
	{
		char data[255];
		UINT pSize=255;
		UINT result;
		result = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice,RIDI_DEVICENAME, data, &pSize);
		data[result]=0;
		log_printf("%d: %ld   %s\n", i, pRawInputDeviceList[i].dwType, data);
	}
	// after the job, free the RAWINPUTDEVICELIST
	free(pRawInputDeviceList);
}

//******************************************************************************************************************************

