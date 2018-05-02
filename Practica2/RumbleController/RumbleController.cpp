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


//-----------------------------------------------------------------------------
// Function-prototypes
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
HRESULT UpdateControllerState();
void RenderFrame();
void HandleInputWithController(DWORD i);
float velRaton = 1.5f;
float relativeVel = 0;
float relativeVel2 = 0;
UINT_PTR Timer;
float timeVibration = 0.1f;
float auxVibration = 0.1f;


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define MAX_CONTROLLERS     4

#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

bool    g_bDeadZoneOn = true;
//-----------------------------------------------------------------------------
// Struct to hold xinput state
//-----------------------------------------------------------------------------
struct CONTROLER_STATE
{
    XINPUT_STATE lastState;
    XINPUT_STATE state;
    DWORD dwResult;
    bool bLockVibration;
    XINPUT_VIBRATION vibration;
};

CONTROLER_STATE g_Controllers[MAX_CONTROLLERS];
WCHAR g_szMessage[4][1024] = {0};
HWND g_hWnd;
void CALLBACK OnTime();

float auxPoint = 0;


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point for the application.  Since we use a simple dialog for 
//       user interaction we don't need to pump messages.
//-----------------------------------------------------------------------------
int APIENTRY wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, int )
{
    // Register the window class
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
	MSG msg = {};
    msg.message = WM_NULL;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

    // Clean up 
    UnregisterClass( L"XInputSample", NULL );

    return 0;
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

void CALLBACK OnTime()
{
	UpdateControllerState();
	RenderFrame();
}
//-----------------------------------------------------------------------------
void RenderFrame()
{
    bool bRepaint = false;

    WCHAR sz[4][1024];
    for( DWORD i = 0; i < MAX_CONTROLLERS; i++ )
    {
        if( g_Controllers[i].dwResult == ERROR_SUCCESS )
        {
            if( !g_Controllers[i].bLockVibration )
            {
                // Map bLeftTrigger's 0-255 to wLeftMotorSpeed's 0-65535
                if( g_Controllers[i].state.Gamepad.bLeftTrigger > 0 )
                    g_Controllers[i].vibration.wLeftMotorSpeed = ( ( g_Controllers[i].state.Gamepad.bLeftTrigger +
                                                                     1 ) * 256 ) - 1;
                else
                    g_Controllers[i].vibration.wLeftMotorSpeed = 0;

                // Map bRightTrigger's 0-255 to wRightMotorSpeed's 0-65535
                if( g_Controllers[i].state.Gamepad.bRightTrigger > 0 )
                    g_Controllers[i].vibration.wRightMotorSpeed = ( ( g_Controllers[i].state.Gamepad.bRightTrigger +
                                                                      1 ) * 256 ) - 1;
                else
                    g_Controllers[i].vibration.wRightMotorSpeed = 0;
            }

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

		HandleInputWithController(i);
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


void HandleInputWithController(DWORD i)
{
	POINT pt;
	GetCursorPos(&pt);
	WCHAR sz[4][1024];
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
			WORD wButtons = g_Controllers[i].state.Gamepad.wButtons;

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

				if (g_Controllers[i].state.Gamepad.sThumbLX > INPUT_DEADZONE/2)
				{
					relativeVel = velRaton / 2;
					if (g_Controllers[i].state.Gamepad.sThumbLX > INPUT_DEADZONE*2)
					{
						relativeVel = velRaton;
					}
					auxPoint+= relativeVel;
					if (auxPoint > 1)
					{
						auxPoint = 0;
						pt.x += 1;
					}
				}
				else if (g_Controllers[i].state.Gamepad.sThumbLX < -INPUT_DEADZONE/2)
				{
					relativeVel = velRaton / 2;
					if (g_Controllers[i].state.Gamepad.sThumbLX < -INPUT_DEADZONE * 2)
					{
						relativeVel = velRaton;
					}
					auxPoint += relativeVel;
					if (auxPoint > 1)
					{
						auxPoint = 0;
						pt.x -= 1;
					}
				} 
				if (g_Controllers[i].state.Gamepad.sThumbLY > INPUT_DEADZONE/2)
				{
					relativeVel2 = velRaton / 2;
					if (g_Controllers[i].state.Gamepad.sThumbLY > INPUT_DEADZONE * 2)
					{
						relativeVel2 = velRaton;
					}
					auxPoint += relativeVel2;
					if (auxPoint > 1)
					{
						auxPoint = 0;
						pt.y -= 1;
					}
				}
				else if (g_Controllers[i].state.Gamepad.sThumbLY < -INPUT_DEADZONE/2)
				{
					relativeVel2 = velRaton / 2;
					if (g_Controllers[i].state.Gamepad.sThumbLY < -INPUT_DEADZONE * 2)
					{
						relativeVel2 = velRaton;
					}
					auxPoint += relativeVel2;
					if (auxPoint > 1)
					{
						auxPoint = 0;
						pt.y += 1;
					}
				}

				wButtons = g_Controllers[i].state.Gamepad.wButtons;
				WORD lastButton = g_Controllers[i].lastState.Gamepad.wButtons;

				if (!(lastButton & XINPUT_GAMEPAD_LEFT_SHOULDER) && (wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))
				{
					mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0);
				}
				else if ((lastButton & XINPUT_GAMEPAD_LEFT_SHOULDER) && !(wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER))
				{
					mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0);
				}

			    if ( (wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))
				{
					mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0);
				}
				if (wButtons & XINPUT_GAMEPAD_X)
				{
					keybd_event(VK_HOME, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}
				else if (wButtons & XINPUT_GAMEPAD_B)
				{
					keybd_event(VK_END, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}

				if (wButtons & XINPUT_GAMEPAD_Y)
				{
					keybd_event(VK_PRIOR, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}
				else if (wButtons & XINPUT_GAMEPAD_A)
				{
					keybd_event(VK_NEXT, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}

				if (wButtons & XINPUT_GAMEPAD_BACK)
				{
					keybd_event(VK_ESCAPE, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}
				else if (wButtons & XINPUT_GAMEPAD_START)
				{
					keybd_event(VK_RETURN, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}
				if (!(lastButton & XINPUT_GAMEPAD_DPAD_UP) && (wButtons & XINPUT_GAMEPAD_DPAD_UP))
				{
					keybd_event(VK_UP, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}
				else if (!(lastButton & XINPUT_GAMEPAD_DPAD_DOWN) && (wButtons & XINPUT_GAMEPAD_DPAD_DOWN))
				{
					keybd_event(VK_DOWN, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}
				else if (!(lastButton & XINPUT_GAMEPAD_DPAD_RIGHT) && (wButtons & XINPUT_GAMEPAD_DPAD_RIGHT))
				{
					keybd_event(VK_RIGHT, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}
				else if (!(lastButton & XINPUT_GAMEPAD_DPAD_LEFT) && (wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
				{
					keybd_event(VK_LEFT, 0X45, KEYEVENTF_EXTENDEDKEY | 0, 0);
				}
				if (g_Controllers[i].state.Gamepad.bLeftTrigger > 0)
				{
					mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_WHEEL, pt.x, pt.y, g_Controllers[i].state.Gamepad.bLeftTrigger/10, 0);
				}
				else if (g_Controllers[i].state.Gamepad.bRightTrigger > 0)
				{
					mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_WHEEL, pt.x, pt.y, -g_Controllers[i].state.Gamepad.bRightTrigger / 10, 0);
				}
				if (wButtons)
				{
					auxVibration = 0;
				}
				if(auxVibration < timeVibration)
				{
					g_Controllers[i].vibration.wLeftMotorSpeed = (255);
					auxVibration += 0.001f;
				}
				XInputSetState(i, &g_Controllers[i].vibration);


			}


			StringCchPrintfW(sz[i], 1024,
				L"Controller %d: Connected\n"
				L"  Buttons: %s%s%s%s%s%s%s%s%s%s%s%s%s%s\n"
				L"  Left Trigger: %d\n"
				L"  Right Trigger: %d\n"
				L"  Left Thumbstick: %d/%d\n"
				L"  Right Thumbstick: %d/%d", i,
				(wButtons & XINPUT_GAMEPAD_DPAD_UP) ? L"DPAD_UP " : L"",
				(wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? L"DPAD_DOWN " : L"",
				(wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? L"DPAD_LEFT " : L"",
				(wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? L"DPAD_RIGHT " : L"",
				(wButtons & XINPUT_GAMEPAD_START) ? L"START " : L"",
				(wButtons & XINPUT_GAMEPAD_BACK) ? L"BACK " : L"",
				(wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? L"LEFT_THUMB " : L"",
				(wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? L"RIGHT_THUMB " : L"",
				(wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? L"LEFT_SHOULDER " : L"",
				(wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? L"RIGHT_SHOULDER " : L"",
				(wButtons & XINPUT_GAMEPAD_A) ? L"A " : L"",
				(wButtons & XINPUT_GAMEPAD_B) ? L"B " : L"",
				(wButtons & XINPUT_GAMEPAD_X) ? L"X " : L"",
				(wButtons & XINPUT_GAMEPAD_Y) ? L"Y " : L"",
				g_Controllers[i].state.Gamepad.bLeftTrigger,
				g_Controllers[i].state.Gamepad.bRightTrigger,
				g_Controllers[i].state.Gamepad.sThumbLX,
				g_Controllers[i].state.Gamepad.sThumbLY,
				g_Controllers[i].state.Gamepad.sThumbRX,
				g_Controllers[i].state.Gamepad.sThumbRY);
		}
	SetCursorPos(pt.x, pt.y);
	}
//-----------------------------------------------------------------------------
// Window message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	HCURSOR hCurs1, hCurs2;    // cursor handles 

	POINT pt;                  // cursor location  
	RECT rc;                   // client area coordinates 
	static int repeat = 1;     // repeat key counter
    switch( msg )
    {
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

		case WM_CREATE:
			Timer = SetTimer(hWnd, 0, 10, (TIMERPROC)OnTime);
			return 0;

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
			KillTimer(hWnd, Timer);
            PostQuitMessage( 0 );
            break;
        }
		case WM_KEYDOWN:

			if (wParam != VK_LEFT && wParam != VK_RIGHT &&
				wParam != VK_UP && wParam != VK_DOWN)
			{
				break;
			}

			GetCursorPos(&pt);

			// Convert screen coordinates to client coordinates. 

			ScreenToClient(hWnd, &pt);

			switch (wParam)
			{
				// Move the cursor to reflect which 
				// arrow keys are pressed. 

			case VK_LEFT:               // left arrow 
				pt.x -= repeat;
				break;

			case VK_RIGHT:              // right arrow 
				pt.x += repeat;
				break;

			case VK_UP:                 // up arrow 
				pt.y -= repeat;
				break;

			case VK_DOWN:               // down arrow 
				pt.y += repeat;
				break;

			default:
				return 0;
			}

			repeat++;           // Increment repeat count. 

								// Keep the cursor in the client area. 

			GetClientRect(hWnd, &rc);

			if (pt.x >= rc.right)
			{
				pt.x = rc.right - 1;
			}
			else
			{
				if (pt.x < rc.left)
				{
					pt.x = rc.left;
				}
			}

			if (pt.y >= rc.bottom)
				pt.y = rc.bottom - 1;
			else
				if (pt.y < rc.top)
					pt.y = rc.top;

			// Convert client coordinates to screen coordinates. 

			ClientToScreen(hWnd, &pt);
			SetCursorPos(pt.x, pt.y);
			return 0;


		case WM_KEYUP:

			repeat = 1;            // Clear repeat count. 
			return 0;

	}


    return DefWindowProc( hWnd, msg, wParam, lParam );
}



