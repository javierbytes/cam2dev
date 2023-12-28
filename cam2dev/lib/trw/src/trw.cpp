#include "trw.h"

#if defined(ESP8266)
#define INTERRUPT_ATTR ICACHE_RAM_ATTR
#elif defined(ESP32)
#define INTERRUPT_ATTR IRAM_ATTR
#else
#define INTERRUPT_ATTR
#endif

WIEGAND::WIEGAND()
{
}

//***********R1*****************
volatile unsigned long WIEGAND::_cardTempHigh_R1 = 0;
volatile unsigned long WIEGAND::_cardTemp_R1 = 0;
volatile unsigned long WIEGAND::_lastWiegand_R1 = 0;
unsigned long WIEGAND::_code_R1 = 0;
volatile int WIEGAND::_bitCount_R1 = 0;
int WIEGAND::_wiegandType_R1 = 0;

unsigned long WIEGAND::getCode_R1()
{
	return _code_R1;
}

int WIEGAND::getWiegandType_R1()
{
	return _wiegandType_R1;
}

bool WIEGAND::available_R1()
{
	bool ret;
	noInterrupts();
	ret = DoWiegandConversion_R1();
	interrupts();
	return ret;
}

void WIEGAND::begin_R1()
{
	begin_R1(14, 12, 18);
}

void WIEGAND::begin_R1(int pinD0_R1, int pinD1_R1, int BZL_R1)
{
	_lastWiegand_R1 = 0;
	_cardTempHigh_R1 = 0;
	_cardTemp_R1 = 0;
	_code_R1 = 0;
	_wiegandType_R1 = 0;
	_bitCount_R1 = 0;
	pinMode(pinD0_R1, INPUT);											  // Set D0 pin as input
	pinMode(pinD1_R1, INPUT);											  // Set D1 pin as input
	pinMode(BZL_R1, OUTPUT);											  // Buzzer y Led
	attachInterrupt(digitalPinToInterrupt(pinD0_R1), ReadD0_R1, FALLING); // Hardware interrupt - high to low pulse
	attachInterrupt(digitalPinToInterrupt(pinD1_R1), ReadD1_R1, FALLING); // Hardware interrupt - high to low pulse
	digitalWrite(BZL_R1, HIGH);
	delay(500);
	digitalWrite(BZL_R1, LOW);
}

INTERRUPT_ATTR void WIEGAND::ReadD0_R1()
{
	_bitCount_R1++;		   // Increament bit count for Interrupt connected to D0
	if (_bitCount_R1 > 31) // If bit count more than 31, process high bits
	{
		_cardTempHigh_R1 |= ((0x80000000 & _cardTemp_R1) >> 31); //	shift value to high bits
		_cardTempHigh_R1 <<= 1;
		_cardTemp_R1 <<= 1;
	}
	else
	{
		_cardTemp_R1 <<= 1; // D0 represent binary 0, so just left shift card data
	}
	_lastWiegand_R1 = millis(); // Keep track of last wiegand bit received
}

INTERRUPT_ATTR void WIEGAND::ReadD1_R1()
{
	_bitCount_R1++;		   // Increment bit count for Interrupt connected to D1
	if (_bitCount_R1 > 31) // If bit count more than 31, process high bits
	{
		_cardTempHigh_R1 |= ((0x80000000 & _cardTemp_R1) >> 31); // shift value to high bits
		_cardTempHigh_R1 <<= 1;
		_cardTemp_R1 |= 1;
		_cardTemp_R1 <<= 1;
	}
	else
	{
		_cardTemp_R1 |= 1;	// D1 represent binary 1, so OR card data with 1 then
		_cardTemp_R1 <<= 1; // left shift card data
	}
	_lastWiegand_R1 = millis(); // Keep track of last wiegand bit received
}

unsigned long WIEGAND::GetCardId_R1(volatile unsigned long *codehigh, volatile unsigned long *codelow, char bitlength)
{
	if (bitlength == 26) // EM tag
		return (*codelow & 0x1FFFFFE) >> 1;

	if (bitlength == 24)
		return (*codelow & 0x7FFFFE) >> 1;

	if (bitlength == 34) // Mifare
	{
		*codehigh = *codehigh & 0x03; // only need the 2 LSB of the codehigh
		*codehigh <<= 30;			  // shift 2 LSB to MSB
		*codelow >>= 1;
		return *codehigh | *codelow;
	}

	if (bitlength == 32)
	{
		return (*codelow & 0x7FFFFFFE) >> 1;
	}

	return *codelow; // EM tag or Mifare without parity bits
}

char translateEnterEscapeKeyPress_R1(char originalKeyPress)
{
	switch (originalKeyPress)
	{
	case 0x0b:		 // 11 or * key
		return 0x0d; // 13 or ASCII ENTER

	case 0x0a:		 // 10 or # key
		return 0x1b; // 27 or ASCII ESCAPE

	default:
		return originalKeyPress;
	}
}

bool WIEGAND::DoWiegandConversion_R1()
{
	unsigned long cardID;
	unsigned long sysTick = millis();

	if ((sysTick - _lastWiegand_R1) > 25) // if no more signal coming through after 25ms
	{
		if ((_bitCount_R1 == 24) || (_bitCount_R1 == 26) || (_bitCount_R1 == 32) || (_bitCount_R1 == 34) || (_bitCount_R1 == 8) || (_bitCount_R1 == 4)) // bitCount for keypress=4 or 8, Wiegand 26=24 or 26, Wiegand 34=32 or 34
		{
			_cardTemp_R1 >>= 1;	   // shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
			if (_bitCount_R1 > 32) // bit count more than 32 bits, shift high bits right to make adjustment
				_cardTempHigh_R1 >>= 1;

			if (_bitCount_R1 == 8) // keypress wiegand with integrity
			{
				// 8-bit Wiegand keyboard data, high nibble is the "NOT" of low nibble
				// eg if key 1 pressed, data=E1 in binary 11100001 , high nibble=1110 , low nibble = 0001
				char highNibble = (_cardTemp_R1 & 0xf0) >> 4;
				char lowNibble = (_cardTemp_R1 & 0x0f);
				_wiegandType_R1 = _bitCount_R1;
				_bitCount_R1 = 0;
				_cardTemp_R1 = 0;
				_cardTempHigh_R1 = 0;

				if (lowNibble == (~highNibble & 0x0f)) // check if low nibble matches the "NOT" of high nibble.
				{
					_code_R1 = (int)translateEnterEscapeKeyPress_R1(lowNibble);
					return true;
				}
				else
				{
					_lastWiegand_R1 = sysTick;
					_bitCount_R1 = 0;
					_cardTemp_R1 = 0;
					_cardTempHigh_R1 = 0;
					return false;
				}

				// TODO: Handle validation failure case!
			}
			else if (4 == _bitCount_R1)
			{
				// 4-bit Wiegand codes have no data integrity check so we just
				// read the LOW nibble.
				_code_R1 = (int)translateEnterEscapeKeyPress_R1(_cardTemp_R1 & 0x0000000F);

				_wiegandType_R1 = _bitCount_R1;
				_bitCount_R1 = 0;
				_cardTemp_R1 = 0;
				_cardTempHigh_R1 = 0;

				return true;
			}
			else // wiegand 26 or wiegand 34
			{
				cardID = GetCardId_R1(&_cardTempHigh_R1, &_cardTemp_R1, _bitCount_R1);
				_wiegandType_R1 = _bitCount_R1;
				_bitCount_R1 = 0;
				_cardTemp_R1 = 0;
				_cardTempHigh_R1 = 0;
				_code_R1 = cardID;
				return true;
			}
		}
		else
		{
			// well time over 25 ms and bitCount !=8 , !=26, !=34 , must be noise or nothing then.
			_lastWiegand_R1 = sysTick;
			_bitCount_R1 = 0;
			_cardTemp_R1 = 0;
			_cardTempHigh_R1 = 0;
			return false;
		}
	}
	else
		return false;
}

//***********R2*****************
volatile unsigned long WIEGAND::_cardTempHigh_R2 = 0;
volatile unsigned long WIEGAND::_cardTemp_R2 = 0;
volatile unsigned long WIEGAND::_lastWiegand_R2 = 0;
unsigned long WIEGAND::_code_R2 = 0;
volatile int WIEGAND::_bitCount_R2 = 0;
int WIEGAND::_wiegandType_R2 = 0;

unsigned long WIEGAND::getCode_R2()
{
	return _code_R2;
}

int WIEGAND::getWiegandType_R2()
{
	return _wiegandType_R2;
}

bool WIEGAND::available_R2()
{
	bool ret;
	noInterrupts();
	ret = DoWiegandConversion_R2();
	interrupts();
	return ret;
}

void WIEGAND::begin_R2()
{
	begin_R2(13, 15, 19);
}

void WIEGAND::begin_R2(int pinD0_R2, int pinD1_R2, int BZL_R2)
{
	_lastWiegand_R2 = 0;
	_cardTempHigh_R2 = 0;
	_cardTemp_R2 = 0;
	_code_R2 = 0;
	_wiegandType_R2 = 0;
	_bitCount_R2 = 0;
	pinMode(pinD0_R2, INPUT);											  // Set D0 pin as input
	pinMode(pinD1_R2, INPUT);											  // Set D1 pin as input
	pinMode(BZL_R2, OUTPUT);											  // Buzzer y Led
	attachInterrupt(digitalPinToInterrupt(pinD0_R2), ReadD0_R2, FALLING); // Hardware interrupt - high to low pulse
	attachInterrupt(digitalPinToInterrupt(pinD1_R2), ReadD1_R2, FALLING); // Hardware interrupt - high to low pulse
	digitalWrite(BZL_R2, HIGH);
	delay(500);
	digitalWrite(BZL_R2, LOW);
}

INTERRUPT_ATTR void WIEGAND::ReadD0_R2()
{
	_bitCount_R2++;		   // Increament bit count for Interrupt connected to D0
	if (_bitCount_R2 > 31) // If bit count more than 31, process high bits
	{
		_cardTempHigh_R2 |= ((0x80000000 & _cardTemp_R2) >> 31); //	shift value to high bits
		_cardTempHigh_R2 <<= 1;
		_cardTemp_R2 <<= 1;
	}
	else
	{
		_cardTemp_R2 <<= 1; // D0 represent binary 0, so just left shift card data
	}
	_lastWiegand_R2 = millis(); // Keep track of last wiegand bit received
}

INTERRUPT_ATTR void WIEGAND::ReadD1_R2()
{
	_bitCount_R2++;		   // Increment bit count for Interrupt connected to D1
	if (_bitCount_R2 > 31) // If bit count more than 31, process high bits
	{
		_cardTempHigh_R2 |= ((0x80000000 & _cardTemp_R2) >> 31); // shift value to high bits
		_cardTempHigh_R2 <<= 1;
		_cardTemp_R2 |= 1;
		_cardTemp_R2 <<= 1;
	}
	else
	{
		_cardTemp_R2 |= 1;	// D1 represent binary 1, so OR card data with 1 then
		_cardTemp_R2 <<= 1; // left shift card data
	}
	_lastWiegand_R2 = millis(); // Keep track of last wiegand bit received
}

unsigned long WIEGAND::GetCardId_R2(volatile unsigned long *codehigh, volatile unsigned long *codelow, char bitlength)
{
	if (bitlength == 26) // EM tag
		return (*codelow & 0x1FFFFFE) >> 1;

	if (bitlength == 24)
		return (*codelow & 0x7FFFFE) >> 1;

	if (bitlength == 34) // Mifare
	{
		*codehigh = *codehigh & 0x03; // only need the 2 LSB of the codehigh
		*codehigh <<= 30;			  // shift 2 LSB to MSB
		*codelow >>= 1;
		return *codehigh | *codelow;
	}

	if (bitlength == 32)
	{
		return (*codelow & 0x7FFFFFFE) >> 1;
	}

	return *codelow; // EM tag or Mifare without parity bits
}

char translateEnterEscapeKeyPress_R2(char originalKeyPress)
{
	switch (originalKeyPress)
	{
	case 0x0b:		 // 11 or * key
		return 0x0d; // 13 or ASCII ENTER

	case 0x0a:		 // 10 or # key
		return 0x1b; // 27 or ASCII ESCAPE

	default:
		return originalKeyPress;
	}
}

bool WIEGAND::DoWiegandConversion_R2()
{
	unsigned long cardID;
	unsigned long sysTick = millis();

	if ((sysTick - _lastWiegand_R2) > 25) // if no more signal coming through after 25ms
	{
		if ((_bitCount_R2 == 24) || (_bitCount_R2 == 26) || (_bitCount_R2 == 32) || (_bitCount_R2 == 34) || (_bitCount_R2 == 8) || (_bitCount_R2 == 4)) // bitCount for keypress=4 or 8, Wiegand 26=24 or 26, Wiegand 34=32 or 34
		{
			_cardTemp_R2 >>= 1;	   // shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
			if (_bitCount_R2 > 32) // bit count more than 32 bits, shift high bits right to make adjustment
				_cardTempHigh_R2 >>= 1;

			if (_bitCount_R2 == 8) // keypress wiegand with integrity
			{
				// 8-bit Wiegand keyboard data, high nibble is the "NOT" of low nibble
				// eg if key 1 pressed, data=E1 in binary 11100001 , high nibble=1110 , low nibble = 0001
				char highNibble = (_cardTemp_R2 & 0xf0) >> 4;
				char lowNibble = (_cardTemp_R2 & 0x0f);
				_wiegandType_R2 = _bitCount_R2;
				_bitCount_R2 = 0;
				_cardTemp_R2 = 0;
				_cardTempHigh_R2 = 0;

				if (lowNibble == (~highNibble & 0x0f)) // check if low nibble matches the "NOT" of high nibble.
				{
					_code_R2 = (int)translateEnterEscapeKeyPress_R2(lowNibble);
					return true;
				}
				else
				{
					_lastWiegand_R2 = sysTick;
					_bitCount_R2 = 0;
					_cardTemp_R2 = 0;
					_cardTempHigh_R2 = 0;
					return false;
				}

				// TODO: Handle validation failure case!
			}
			else if (4 == _bitCount_R2)
			{
				// 4-bit Wiegand codes have no data integrity check so we just
				// read the LOW nibble.
				_code_R2 = (int)translateEnterEscapeKeyPress_R2(_cardTemp_R2 & 0x0000000F);

				_wiegandType_R2 = _bitCount_R2;
				_bitCount_R2 = 0;
				_cardTemp_R2 = 0;
				_cardTempHigh_R2 = 0;

				return true;
			}
			else // wiegand 26 or wiegand 34
			{
				cardID = GetCardId_R2(&_cardTempHigh_R2, &_cardTemp_R2, _bitCount_R2);
				_wiegandType_R2 = _bitCount_R2;
				_bitCount_R2 = 0;
				_cardTemp_R2 = 0;
				_cardTempHigh_R2 = 0;
				_code_R2 = cardID;
				return true;
			}
		}
		else
		{
			// well time over 25 ms and bitCount !=8 , !=26, !=34 , must be noise or nothing then.
			_lastWiegand_R2 = sysTick;
			_bitCount_R2 = 0;
			_cardTemp_R2 = 0;
			_cardTempHigh_R2 = 0;
			return false;
		}
	}
	else
		return false;
}
