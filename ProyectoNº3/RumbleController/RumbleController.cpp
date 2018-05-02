//-----------------------------------------------------------------------------
// File: RumbleController.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <commdlg.h>
#include <XInput.h>
#include <basetsd.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include "resource.h"
#include "BaseHID.h"
#include "HIDXbox.h"

//-----------------------------------------------------------------------------
void GeneraEventos(HIDXbox *Control);
void CALLBACK TimerCallback();

#define TARGET_XBOX360

#ifdef TARGET_XBOX360
	HIDXbox Control;
#elif defined(TARGET_PS3)
	HIDPs Control(T);
#elif defined(TARGET_Wii)
	HIDWii Control(T);
#endif

// Function-prototypes
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
HRESULT UpdateControllerState();
void RenderFrame();



//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define MAX_CONTROLLERS     4
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

//-----------------------------------------------------------------------------
// Struct to hold xinput state
//-----------------------------------------------------------------------------
struct CONTROLER_STAT
{
    XINPUT_STATE lastState;
    XINPUT_STATE state;
    DWORD dwResult;
    bool bLockVibration;
    XINPUT_VIBRATION vibration;
	bool bConnected;
};

CONTROLER_STAT g_Controllers[MAX_CONTROLLERS];
WCHAR g_szMessage[4][1024] = {0};
HWND g_hWnd;
bool    g_bDeadZoneOn = true;


POINT pt; // cursor location                 //
RECT rc; // client area coordinates          //
static int repeat = 1; // repeat key counter //


//SetTimer(g_hWnd, 0, 1000, (TIMERPROC)NULL); 
	


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point for the application.  Since we use a simple dialog for 
//       user interaction we don't need to pump messages.
//-----------------------------------------------------------------------------
int APIENTRY wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, int )
{
	Control.Calibra();
	UINT_PTR pTimerxbox;

	pTimerxbox = SetTimer(NULL, NULL, 10, (TIMERPROC)TimerCallback);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	KillTimer(NULL, pTimerxbox);

   /* // Register the window class
    HBRUSH hBrush = CreateSolidBrush( 0xFF0000 );
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), 0, MsgProc, 0L, 0L, hInst, NULL,
        LoadCursor( NULL, IDC_ARROW ), hBrush,
        NULL, L"XInputSample", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    g_hWnd = CreateWindow( L"XInputSample", L"XInput Sample: RumbleController",
                           WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                           CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
                           NULL, NULL, hInst, NULL );

    // Init state
    ZeroMemory( g_Controllers, sizeof( CONTROLER_STATE ) * MAX_CONTROLLERS );

    // Enter the message loop
    bool bGotMsg;
    MSG msg;
    msg.message = WM_NULL;

    while( WM_QUIT != msg.message )
    {
        // Use PeekMessage() so we can use idle time to render the scene and call pEngine->DoWork()
        bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

        if( bGotMsg )
        {
            // Translate and dispatch the message
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            UpdateControllerState();
            RenderFrame();
        }
    }

    // Clean up 
    UnregisterClass( L"XInputSample", NULL );*/

    return 0;
}

void CALLBACK TimerCallback(){
	Control.update();
	GeneraEventos(&Control);

}

void GeneraEventos(HIDXbox *Control){
	POINT pt, pt0;
	GetCursorPos(&pt);
	HWND hWnd = GetActiveWindow();

	if ((Control->LJX() != 0) | (Control->LJY() != 0))
	{
		pt.x += 10 * Control->LJXf();
		pt.y += 10 * Control->LJYf();
		SetCursorPos(pt.x, pt.y);
	}

	RECT Rect = { 0, 0, 1024, 600 };
	HWND Desk = GetDesktopWindow();
	GetWindowRect(Desk, &Rect);

	if ((Control->RJX() != 0) | (Control->RJY() != 0))
	{
		pt.x = Rect.right / 2 + Rect.right / 2 * Control->RJXf();
		pt.y = Rect.bottom / 2 - Rect.bottom / 2 * Control->RJYf();
		SetCursorPos(pt.x, pt.y);
	}

	//eventos de raton
	if (Control->BD(XINPUT_GAMEPAD_LEFT_SHOULDER))
		mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, NULL);
	if (Control->BU(XINPUT_GAMEPAD_LEFT_SHOULDER))
		mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, NULL);
	if (Control->BD(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, NULL);
	if (Control->BU(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, NULL);
	if (Control->LT() != 0)
		mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, (DWORD)(40 * Control->LT()), NULL); 
	if (Control->RT() != 0)
		mouse_event(MOUSEEVENTF_WHEEL, pt.x, pt.y, -(DWORD)(40 * Control->RT()), NULL);


	//eventos de teclado
	if (Control->BD(XINPUT_GAMEPAD_BACK))
		PostQuitMessage(0);
	if (Control->BD(XINPUT_GAMEPAD_DPAD_UP))
		keybd_event(VK_UP, 0x0, 0, 0);
	if (Control->BU(XINPUT_GAMEPAD_DPAD_UP))
		keybd_event(VK_UP, 0x0, KEYEVENTF_KEYUP, 0);
	if (Control->BD(XINPUT_GAMEPAD_DPAD_DOWN))
		keybd_event(VK_DOWN, 0x0, 0, 0);
	if (Control->BU(XINPUT_GAMEPAD_DPAD_DOWN))
		keybd_event(VK_DOWN, 0x0, KEYEVENTF_KEYUP, 0);
	if (Control->BD(XINPUT_GAMEPAD_DPAD_LEFT))
		keybd_event(VK_LEFT, 0x0, 0, 0);
	if (Control->BU(XINPUT_GAMEPAD_DPAD_LEFT))
		keybd_event(VK_LEFT, 0x0, KEYEVENTF_KEYUP, 0);
	if (Control->BD(XINPUT_GAMEPAD_DPAD_RIGHT))
		keybd_event(VK_RIGHT, 0x0, 0, 0);
	if (Control->BU(XINPUT_GAMEPAD_DPAD_RIGHT))
		keybd_event(VK_RIGHT, 0x0, KEYEVENTF_KEYUP, 0);

}

//-----------------------------------------------------------------------------
HRESULT UpdateControllerState()
{
    for( DWORD i = 0; i < MAX_CONTROLLERS; i++ )
    {
        g_Controllers[i].lastState = g_Controllers[i].state;
        g_Controllers[i].dwResult = XInputGetState( i, &g_Controllers[i].state );
    }

    return S_OK;
}

void joystick(int i){
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	if (g_Controllers[i].state.Gamepad.sThumbLX){
		pt.x += 10 * (float)(g_Controllers[i].state.Gamepad.sThumbLX) / MAXINT16;
	}
	else{
		pt.x -= 10 * (float)(g_Controllers[i].state.Gamepad.sThumbLX) / MAXINT16;
	}

	if (g_Controllers[i].state.Gamepad.sThumbLY){




		pt.y -= 10 * (float)(g_Controllers[i].state.Gamepad.sThumbLY) / MAXINT16;
	}
	else{
		pt.y += 10 * (float)(g_Controllers[i].state.Gamepad.sThumbLY) / MAXINT16;
	}
	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
	


}

//-----------------------------------------------------------------------------
void RenderFrame()
{
    bool bRepaint = false;

    WCHAR sz[4][1024];
    for( DWORD i = 0; i < MAX_CONTROLLERS; i++ )
    {
		if (g_bDeadZoneOn)
		{
			// Zero value if thumbsticks are within the dead zone 
			if ((g_Controllers[i].state.Gamepad.sThumbLX < INPUT_DEADZONE &&
				g_Controllers[i].state.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
				(g_Controllers[i].state.Gamepad.sThumbLY < INPUT_DEADZONE &&
				g_Controllers[i].state.Gamepad.sThumbLY > -INPUT_DEADZONE))
			{
				g_Controllers[i].state.Gamepad.sThumbLX = 0;
				g_Controllers[i].state.Gamepad.sThumbLY = 0;
			}

			if ((g_Controllers[i].state.Gamepad.sThumbRX < INPUT_DEADZONE &&
				g_Controllers[i].state.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
				(g_Controllers[i].state.Gamepad.sThumbRY < INPUT_DEADZONE &&
				g_Controllers[i].state.Gamepad.sThumbRY > -INPUT_DEADZONE))
			{
				g_Controllers[i].state.Gamepad.sThumbRX = 0;
				g_Controllers[i].state.Gamepad.sThumbRY = 0;
			}
		}

        if( g_Controllers[i].dwResult == ERROR_SUCCESS )
        {
			

            //if( !g_Controllers[i].bLockVibration )
            //{

                // Map bLeftTrigger's 0-255 to wLeftMotorSpeed's 0-65535
			if (g_Controllers[i].state.Gamepad.sThumbLX)
				joystick(i);
                else
                    g_Controllers[i].vibration.wLeftMotorSpeed = 0;

              
            //}
			WORD wButtons = g_Controllers[i].state.Gamepad.wButtons;
			WORD wlastButtons = g_Controllers[i].lastState.Gamepad.wButtons;

			if ((wButtons & XINPUT_GAMEPAD_X) && !(wlastButtons & XINPUT_GAMEPAD_X))
				mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, NULL);
			if (!(wButtons & XINPUT_GAMEPAD_X) && (wlastButtons & XINPUT_GAMEPAD_X))
				mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, NULL);

			if ((wButtons & XINPUT_GAMEPAD_B) && !(wlastButtons & XINPUT_GAMEPAD_B))
				mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, NULL);
			if (!(wButtons & XINPUT_GAMEPAD_B) && (wlastButtons & XINPUT_GAMEPAD_B))
				mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, NULL);

			if ((wButtons & XINPUT_GAMEPAD_BACK) && !(wlastButtons & XINPUT_GAMEPAD_BACK))
				keybd_event(VK_ESCAPE, 0x18, KEYEVENTF_EXTENDEDKEY | 0, 0);
			if ((wButtons & XINPUT_GAMEPAD_START) && !(wlastButtons & XINPUT_GAMEPAD_START))
				keybd_event(VK_NEXT, 0x18, KEYEVENTF_EXTENDEDKEY | 0, 0);
			if ((wButtons & XINPUT_GAMEPAD_A) && !(wlastButtons & XINPUT_GAMEPAD_A))
				keybd_event(VK_NEXT, 0x18, KEYEVENTF_EXTENDEDKEY | 0, 0);
			if ((wButtons & XINPUT_GAMEPAD_Y) && !(wlastButtons & XINPUT_GAMEPAD_Y))
				keybd_event(VK_PRIOR, 0x18, KEYEVENTF_EXTENDEDKEY | 0, 0);


            if( ( g_Controllers[i].state.Gamepad.wButtons ) &&
                ( g_Controllers[i].lastState.Gamepad.wButtons == 0 ) )
            {
                if( !( !g_Controllers[i].bLockVibration && g_Controllers[i].vibration.wRightMotorSpeed == 0 &&
                       g_Controllers[i].vibration.wLeftMotorSpeed == 0 ) )
                    g_Controllers[i].bLockVibration = !g_Controllers[i].bLockVibration;
            }

            XInputSetState( i, &g_Controllers[i].vibration );

            StringCchPrintfW( sz[i], 1024,
                              L"Controller %d: Connected\n"
                              L"  Left Motor Speed: %d\n"
                              L"  Right Motor Speed: %d\n"
                              L"  Rumble Lock: %d\n", i,
                              g_Controllers[i].vibration.wLeftMotorSpeed,
                              g_Controllers[i].vibration.wRightMotorSpeed,
                              g_Controllers[i].bLockVibration );

        }
        else if( g_Controllers[i].dwResult == ERROR_DEVICE_NOT_CONNECTED )
        {
            StringCchPrintf( sz[i], 1024,
                             L"Controller %d: Not connected", i );
        }
        else
        {
            StringCchPrintf( sz[i], 1024,
                             L"Controller %d: Generic error", i );
        }

        if( wcscmp( sz[i], g_szMessage[i] ) != 0 )
        {
            StringCchCopy( g_szMessage[i], 1024, sz[i] );
            bRepaint = true;
        }
    }

    if( bRepaint )
    {
        // Repaint the window if needed 
        InvalidateRect( g_hWnd, NULL, TRUE );
        UpdateWindow( g_hWnd );
    }

    // This sample doesn't use Direct3D.  Instead, it just yields CPU time to other 
    // apps but this is not typically done when rendering
    Sleep( 10 );
}




//-----------------------------------------------------------------------------
// Window message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {

	case WM_CREATE:
		SetTimer(g_hWnd, 0, 1000, (TIMERPROC)NULL); 
		return 0;

	case WM_TIMER:
		UpdateControllerState();
		RenderFrame();
		return 0;

        case WM_ACTIVATEAPP:
        {
            if( wParam == TRUE )
            {
                // App is now active, so re-enable XInput
                XInputEnable( true );
            }
            else
            {
                // App is now inactive, so disable XInput to prevent
                // user input from effecting application and to 
                // disable rumble. 
                //XInputEnable( false );
            }
            break;
        }

        case WM_PAINT:
        {
            // Paint some simple explanation text
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint( hWnd, &ps );
            SetBkColor( hDC, 0xFF0000 );
            SetTextColor( hDC, 0xFFFFFF );
            RECT rect;
            GetClientRect( hWnd, &rect );

            rect.top = 20;
            rect.left = 20;
            DrawText( hDC,
                      L"Use the controller's left/right trigger to adjust the speed of the left/right rumble motor.\n"
                      L"Press any controller button to lock or unlock at the current rumble speed.\n",
                      -1, &rect, 0 );

            for( DWORD i = 0; i < MAX_CONTROLLERS; i++ )
            {
                rect.top = i * 80 + 90;
                rect.left = 20;
                DrawText( hDC, g_szMessage[i], -1, &rect, 0 );
            }

            EndPaint( hWnd, &ps );
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage( 0 );
            break;
        }
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}



