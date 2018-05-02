#pragma once
#include <windows.h>
#include <XInput.h>
#include "BaseHID.h"

#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

//-----------------------------------------------------------------------------
// Struct to hold xinput state
//-----------------------------------------------------------------------------
struct CONTROLER_STATE
{
	XINPUT_STATE State;
	XINPUT_STATE Offstate;
	DWORD dwResult;
	XINPUT_VIBRATION vibration;
};

class HIDXbox : public BaseHID
{
public:
	HIDXbox();
	~HIDXbox();

private:
	CONTROLER_STATE XBox;

public:
	bool LeerMando();
	void EscribirMando();
	void Mando2HID();
	void Calibra();
};

