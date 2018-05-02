#include "HIDXBox.h"

#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( defaugLT : 4996 )

#define T 10 //ms para actualizar
#define TARGET_XBOX360
#ifdef TARGET_XBOX360
HIDXbox Control(T);
#elif defined(TARGET_PS3)
HIDPs Control(T);
#elif defined(TARGET_WII)
HIDWii Control(T);
#endif

using namespace std;
UINT_PTR Timer; //Timer
VOID CALLBACK TimerCallBack();

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int)
{
	//Establezco un Timer T=1s;
	Timer = SetTimer(nullptr, 0, T, (TIMERPROC)TimerCallBack);
	//XInputEnable(true);

	// Enter the message loop
	bool bGotMsg;
	MSG msg;
	msg.message = WM_NULL;

	while (WM_QUIT != msg.message)
	{
		// Use PeekMessage() so we can use idle time to render the scene and call pEngine->DoWork()
		bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

		if (bGotMsg)
		{
			// Translate and dispatch the message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Clean up 

	return 0;
}


void GeneraEfectos(HIDXbox* controller) {
	POINT pt, pt0;
	GetCursorPos(&pt);
	if (controller->LJX() > 0.05 || controller->LJY() > 0.05)
	{
		pt.x += 20 * controller->LJX();
		pt.y -= 20 * controller->LJY();
		SetCursorPos(pt.x, pt.y);
	}
	if (controller->RJXf() > 0.05 || controller->RJYf() > 0.05)
	{
		pt.x += 20 * controller->RJXf();
		pt.y -= 20 * controller->RJYf();
		SetCursorPos(pt.x, pt.y);
	}

	if (controller->BD(XINPUT_GAMEPAD_LEFT_SHOULDER))
	{
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, NULL);
	}
	if (controller->BU(XINPUT_GAMEPAD_LEFT_SHOULDER))
	{
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, NULL);
	}
	if (controller->BU(XINPUT_GAMEPAD_RIGHT_SHOULDER))
	{
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, NULL);
	}
	if (controller->BD(XINPUT_GAMEPAD_X))
	{
		keybd_event(VK_HOME, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}
	else if (controller->BD(XINPUT_GAMEPAD_B))
	{
		keybd_event(VK_END, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}

	if (controller->BD(XINPUT_GAMEPAD_Y))
	{
		keybd_event(VK_PRIOR, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}
	else if (controller->BD(XINPUT_GAMEPAD_A))
	{
		keybd_event(VK_NEXT, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}

	if (controller->BD(XINPUT_GAMEPAD_BACK))
	{
		keybd_event(VK_ESCAPE, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}
	else if (controller->BD(XINPUT_GAMEPAD_START))
	{
		keybd_event(VK_RETURN, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}
	if (controller->BD(XINPUT_GAMEPAD_DPAD_UP))
	{
		keybd_event(VK_UP, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}
	else if (controller->BD(XINPUT_GAMEPAD_DPAD_DOWN))
	{
		keybd_event(VK_DOWN, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}
	else if (controller->BD(XINPUT_GAMEPAD_DPAD_RIGHT))
	{
		keybd_event(VK_RIGHT, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}
	else if (controller->BD(XINPUT_GAMEPAD_DPAD_LEFT))
	{
		keybd_event(VK_LEFT, 0X45, KEYEVENTF_EXTENDEDKEY | 0, NULL);
	}
	/*if (controller->LT() > 0)
	{
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_WHEEL, pt.x, pt.y, g_Controllers[i].state.Gamepad.bLeftTrigger / 10, 0);
	}
	else if (controller->RT()> 0)
	{
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_WHEEL, pt.x, pt.y, -g_Controllers[i].state.Gamepad.bRightTrigger / 10, 0);
	}
	*/
}
VOID CALLBACK TimerCallBack()
{
	Control.update(); //Actualiza nuestro HID
	GeneraEfectos(&Control);//Genera los efectos en la aplicación en función de los gestos del control
}LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATEAPP:
	{

		return 0;
		break;
	}
	case WM_DESTROY:
	{
		//Libero el Timer;
		KillTimer(hWnd, Timer);
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);

}