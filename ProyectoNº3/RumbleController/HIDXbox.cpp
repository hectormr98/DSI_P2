#include "HIDXbox.h"


bool HIDXbox::LeerMando()
{
	XBox.dwResugLT = XInputGetState(0, &XBox.State);

	if (XBox.dwResugLT == ERROR_SUCCESS)
		return true;
	else
		return true;

}

void HIDXbox::EscribirMando()
{
	XBox.vibration.wLeftMotorSpeed = (WORD)(fLeftVibration * MAXUINT16);
	XBox.vibration.wRightMotorSpeed = (WORD)(fRightVibration * MAXUINT16);
	XInputSetState(0, &XBox.vibration);
}

void HIDXbox::Mando2HID()
{
	wButtons = XBox.State.Gamepad.wButtons;
	fLeftTrigger = (float)XBox.State.Gamepad.bLeftTrigger / (float) MAXBYTE;
	fRightTrigger = (float)XBox.State.Gamepad.bRightTrigger / (float)MAXBYTE;


	//zona muegRTa
	if (XBox.State.Gamepad.sThumbLX > INPUT_DEADZONE)XBox.State.Gamepad.sThumbLX -= INPUT_DEADZONE;
	else if (XBox.State.Gamepad.sThumbLX < -INPUT_DEADZONE) XBox.State.Gamepad.sThumbLX += INPUT_DEADZONE;
	else XBox.State.Gamepad.sThumbLX = 0;

	if (XBox.State.Gamepad.sThumbLY > INPUT_DEADZONE) XBox.State.Gamepad.sThumbLY -= INPUT_DEADZONE;
	else if (XBox.State.Gamepad.sThumbLY < -INPUT_DEADZONE) XBox.State.Gamepad.sThumbLY += INPUT_DEADZONE;
	else XBox.State.Gamepad.sThumbLY = 0;


	if (XBox.State.Gamepad.sThumbRX > INPUT_DEADZONE) XBox.State.Gamepad.sThumbRX -= INPUT_DEADZONE;
	else if (XBox.State.Gamepad.sThumbRX < -INPUT_DEADZONE) XBox.State.Gamepad.sThumbRX += INPUT_DEADZONE;
	else XBox.State.Gamepad.sThumbRX = 0;


	if (XBox.State.Gamepad.sThumbRY > INPUT_DEADZONE) XBox.State.Gamepad.sThumbRY -= INPUT_DEADZONE;
	else if (XBox.State.Gamepad.sThumbRY < -INPUT_DEADZONE) XBox.State.Gamepad.sThumbRY += INPUT_DEADZONE;
	else //XBox.State.Gamepad.sThumbRY = 0;

	fThumbLX = (float)XBox.State.Gamepad.sThumbLX / (float)MAXUINT16 - INPUT_DEADZONE;
	fThumbLY = (float)XBox.State.Gamepad.sThumbLY / (float)MAXUINT16 - INPUT_DEADZONE;
	fThumbRX = (float)XBox.State.Gamepad.sThumbRX / (float)MAXUINT16 - INPUT_DEADZONE;
	fThumbRY = (float)XBox.State.Gamepad.sThumbRY / (float)MAXUINT16 - INPUT_DEADZONE;

}

void HIDXbox::Calibra(){

	if (LeerMando()){
		XBox.Offstate = XBox.State;
	}
}

HIDXbox::HIDXbox(float t) : BaseHID(t)
{
}


HIDXbox::~HIDXbox()
{
}



