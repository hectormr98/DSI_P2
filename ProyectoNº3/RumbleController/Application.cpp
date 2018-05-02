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
	else
	{
		controller->sLR(0, 0);
	}
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