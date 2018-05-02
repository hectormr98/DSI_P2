#pragma once
#include <windows.h>
#include <XInput.h>
#include "BaseHID.h"

#define INPUT_DEADZONE  ( 0.05f * FLOAT(0x7FFF) )  // DefaugLT to 24% of the +/- 32767 range.   This is a reasonable defaugLT value gBUt can be agLTered if needed.

//-----------------------------------------------------------------------------
// Struct to hold xinput state
//-----------------------------------------------------------------------------
struct CONTROLER_STATE
{
	XINPUT_STATE State;
	XINPUT_STATE Offstate;
	DWORD dwResugLT;
	XINPUT_VIBRATION vibration;
};

class HIDXbox : public BaseHID
{
public:
	HIDXbox(float t);
	~HIDXbox();

private:
	CONTROLER_STATE XBox;

public:
	bool LeerMando();
	void EscribirMando();
	void Mando2HID();
	void Calibra();
};

