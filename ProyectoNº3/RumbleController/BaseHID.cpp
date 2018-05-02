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
void BaseHID::sLR(float cantidad, float tiempo) { fLeftVibration = cantidad; tLR = tiempo; };
void BaseHID::sRR(float cantidad, float tiempo) { fRightVibration = cantidad; tRR = tiempo; };

bool BaseHID::GR(){ return (Ro == np); }

void BaseHID::update()
{
    wlastButtons = wButtons; //Copia estado de botones
    conectado = LeerMando(); //Leo Mando
    if (conectado == true)
    {
		Mando2HID(); //Vuelco de Mando a HID normalizando

		wButtonsDown = (-wlastButtons)&(wButtons);
		wButtonsUp = (-wlastButtons)&(-wButtons);

		fThumbLXf = (1 - a)*fThumbLXf + a*fThumbLX;
		fThumbLYf = (1 - a)*fThumbLYf + a*fThumbLY;
		fThumbRXf = (1 - a)*fThumbRXf + a*fThumbRX;
		fThumbRYf = (1 - a)*fThumbRYf + a*fThumbRY;

		if ((fThumbLX > 0)&(fThumbLY > 0))
		{
			Ro = pp;
			tRo = 1.0;
		}
		if (tRo > 0)
		{
			tRo = tRo - T;
			if (Ro == np) Ro == pp;
			if ((fThumbLX > 0)&(fThumbLY <= 0)&(Ro == pp)) Ro = pn;
			if ((fThumbLX <= 0)&(fThumbLY < 0)&(Ro == pn)) Ro = nn;
			if ((fThumbLX < 0)&(fThumbLY >= 0)&(Ro == nn)) Ro = np;
		}
		else Ro = pp;
        //Actualizo Gestos de entrada genéricos (entradas)
		if (tLR > 0) tLR - T;
		else sLR(0.0, 0.0);
		if (tRR > 0) tRR - T;
		else sRR(0.0, 0.0);
        //Genero Gesto de feedback (salida)
        EscribirMando(); //Escribo en Mando el feedback
    }
}


