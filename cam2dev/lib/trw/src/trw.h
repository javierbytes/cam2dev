#ifndef _WIEGAND_H
#define _WIEGAND_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class WIEGAND
{

public:
	WIEGAND();
	//************R1****************
	void begin_R1();
	void begin_R1(int pinD0, int pinD1, int BZL_R1);
	// void begin_R1(int pinD0, int pinIntD0, int pinD1, int pinIntD1);
	bool available_R1();
	unsigned long getCode_R1();
	int getWiegandType_R1();

	//************R2****************

	void begin_R2();
	void begin_R2(int pinD0, int pinD1, int BZL_R1);
	// void begin_R1(int pinD0, int pinIntD0, int pinD1, int pinIntD1);
	bool available_R2();
	unsigned long getCode_R2();
	int getWiegandType_R2();

private:
	//***********R1*****************
	static void ReadD0_R1();
	static void ReadD1_R1();
	static bool DoWiegandConversion_R1();
	static unsigned long GetCardId_R1(volatile unsigned long *codehigh, volatile unsigned long *codelow, char bitlength);

	static volatile unsigned long _cardTempHigh_R1;
	static volatile unsigned long _cardTemp_R1;
	static volatile unsigned long _lastWiegand_R1;
	static volatile int _bitCount_R1;
	static int _wiegandType_R1;
	static unsigned long _code_R1;

	//***********R2*****************

	static void ReadD0_R2();
	static void ReadD1_R2();
	static bool DoWiegandConversion_R2();
	static unsigned long GetCardId_R2(volatile unsigned long *codehigh, volatile unsigned long *codelow, char bitlength);

	static volatile unsigned long _cardTempHigh_R2;
	static volatile unsigned long _cardTemp_R2;
	static volatile unsigned long _lastWiegand_R2;
	static volatile int _bitCount_R2;
	static int _wiegandType_R2;
	static unsigned long _code_R2;
};

#endif
