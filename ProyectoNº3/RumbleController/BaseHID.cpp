#include "BaseHID.h"

float BaseHID::LT(){ return fLeftTrigger; }
float BaseHID::RT(){ return fRightTrigger; }
float BaseHID::LJX(){ return fThumbLX; }
float BaseHID::LJY(){ return fThumbLY; }
float BaseHID::RJX(){ return fThumbRX; }
float BaseHID::RJY(){ return fThumbRY; }
float BaseHID::LJXf(){ return fThumbLXf;  } //revisa
float BaseHID::LJYf(){ return fThumbLYf; }
float BaseHID::RJXf(){ return fThumbRXf; }
float BaseHID::RJYf(){ return fThumbRYf; }
void BaseHID::LV(float p){ fLeftVibration = p; }
void BaseHID::RV(float p){ fRightVibration = p; }
bool BaseHID::BD(WORD Bit){ return (wButtonsDown & Bit); }
bool BaseHID::BU(WORD Bit){ return (wButtonsUp & Bit); }

bool BaseHID::GR(){ return (Ro == np); }

void BaseHID::update()
{
	wlastButtons = wButtons;
	conectado = LeerMando();

	if (conectado)
	{
		Mando2HID();

		wButtonsDown = (~wlastButtons) & (wButtons);
		wButtonsUp = (wlastButtons) & (~wButtons);

		fThumbRXf = (1 - aTR)*fThumbRXf + aTR*fThumbRX;
		fThumbRYf = (1 - aTR)*fThumbRYf + aTR*fThumbRY;


		if ((fThumbLX > 0.8) | (fThumbLX < -0.8))
			fVelX = fVelX + aV*fThumbLX;
		else if ((fThumbLX > 0.01) | (fThumbLX < -0.01))
			fVelX = (fVelX + fThumbLX) / 2;
		else
			fVelX = fVelX*(1 - aV);

		if ((fThumbLY > 0.8) | (fThumbLY < -0.8))
			fVelY = fVelY + aV*fThumbLY;
		else if ((fThumbLY > 0.01) | (fThumbLY < -0.01))
			fVelY = (fVelY + fThumbLY) / 2;
		else
			fVelY = fVelY*(1 - aV);

		if (fVelX > 2.0) fVelX = 2.0;
		if (fVelX < -2.0) fVelX = -2.0;
		if (fVelY > 2.0) fVelY = 2.0;
		if (fVelY < -2.0) fVelY = -2.0;

		if ((fThumbLX > 0) &(fThumbLY > 0))
		{
			Ro == pp;
			tRo = 1.0;
		}
		if (tRo > 0){
			tRo = tRo - T;
			if ((fThumbLX > 0) & (fThumbLY <= 0) &(Ro == pp)) Ro = pn;
			if ((fThumbLX <= 0) & (fThumbLY < 0) &(Ro == pn)) Ro = nn;
			if ((fThumbLX < 0) & (fThumbLY >= 0) &(Ro == nn)) Ro = np;
		}
		else Ro = pp;

		if (wButtonsDown) fRightVibration = 1;
		fRightVibration = fRightVibration*(1 - aFB);
		if (wButtonsUp) fLeftVibration = 1;
		fLeftVibration = fLeftVibration*(1 - aFB);

		EscribirMando();

	}
}

BaseHID::BaseHID()
{
}


BaseHID::~BaseHID()
{
}


