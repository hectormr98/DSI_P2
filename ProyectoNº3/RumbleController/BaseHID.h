#pragma once

typedef unsigned short WORD;
enum Rotacion {pp,pn,nn,np};

class BaseHID
{
public:
	
	float LT();
	float RT();
	float LJX();
	float LJY();
	float RJX();
	float RJY();
	float LJXf();
	float LJYf();
	float RJXf();
	float RJYf();
	void LV(float p);
	void RV(float p);
	bool BD(WORD Bit);
	bool BU(WORD Bit);

	void sLR(float cantidad, float tiempo);
	void sRR(float cantidad, float tiempo);

	bool GR();

	BaseHID(float t)
	{
		T = t / 1000.0;
		a = T / (0.1 +	T);
	};
	~BaseHID() {};

	void update();

	/*struct CONTROLER_STATE
	{
		XINPUT_STATE lastState;
		XINPUT_STATE state;
		DWORD dwResult;
		bool bLockVibration;
		XINPUT_VIBRATION vibration;
	};
	*/

protected:
	bool conectado;
	WORD wButtons;
	WORD wlastButtons;

	float fLeftTrigger, fRightTrigger;
	float fThumbLX, fThumbLY, fThumbRX, fThumbRY;
	float fThumbRXf, fThumbRYf, fThumbLXf, fThumbLYf;
	float fLeftVibration, fRightVibration;

	float T;
	float a;

	WORD wButtonsDown;
	WORD wButtonsUp;

	const float aFB = T / (0.1 + T);
	const float aTR = T / (0.1 + T);
	const float aV = 0.1;
	float fVelX, fVelY;
	float tLR = 0.0;//Tiempo que queda de vibración en LR 
	float tRR = 0.0;

	Rotacion Ro;
	float tRo = 0.0;

	virtual bool LeerMando() = 0;
	virtual void EscribirMando() = 0;
	virtual void Mando2HID() = 0;
	virtual void Calibra() = 0;
};

