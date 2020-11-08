/*
ft817.h Arduino library for controlling a Yaesu FT817 radio via CAT commands.

Based on the work of James Buck, VE3BUX and his FT817 lib, see http://www.ve3bux.com

CAT commands for FT-817 radio taken from:
- The Official FT-817D Manual
- KA7OEI FT-817 pages](http://www.ka7oei.com/ft817pg.shtml)
- Hamlib radio control libraries

Library author is Pavel Milanes
- Email: pavelmc@gmail.com
- Github: @stdevPavelmc
- Twitter: @co7wt
- Telegram: @pavelmc

IMPORTANT STUFF
================
As communications are uni-directional we use the variable "buffer"
to send and receive data to and from the radio, whether we send or
receive it always be to and from the buffer.

This allow us to be consistent and save a few bytes of firmware

*/

#include <Arduino.h>
// #include <SoftwareSerial.h>
#include "ft817.h"

// if using software serial, define software serial IO pins here:
// extern SoftwareSerial rigCat(12, 11); // rx,tx

// if using hardware serial, define hardware serial alias here:
#define rigCat Serial

#define dlyTime 5	// delay (in ms) after serial writes

FT817::FT817(){ }	// nothing to do when first instanced


/****** SETUP ********/

// Setup software serial with user defined input
// from the Arduino sketch (function, though very slow)
// void FT817::setSerial(SoftwareSerial portInfo)
// {
// 	rigCat = portInfo;
// }

// similar to Serial.begin(baud); command
void FT817::begin(long baud)
{
	rigCat.begin(baud);
}


/****** TOGGLE COMMANDS ********/

// lock or unlock the radio
void FT817::lock(boolean toggle)
{
	if (toggle)
	{
		singleCmd(CAT_LOCK_ON);
	}
	else
	{
		singleCmd(CAT_LOCK_OFF);
	}
}

// set or release the virtual PTT button
void FT817::PTT(boolean toggle)
{
	if (toggle)
	{
		singleCmd(CAT_PTT_ON);
	}
	else
	{
		singleCmd(CAT_PTT_OFF);
	}
}

// turn the clarifier on or off
void FT817::clar(boolean toggle)
{
	if (toggle)
	{
		singleCmd(CAT_CLAR_ON);
	}
	else
	{
		singleCmd(CAT_CLAR_OFF);
	}
}

// turn split operation on or off
void FT817::split(boolean toggle)
{
	if (toggle)
	{
		singleCmd(CAT_SPLIT_ON);
	}
	else
	{
		singleCmd(CAT_SPLIT_OFF);
	}
}

// toggle VFO (A or B)
void FT817::toggleVFO()
{
	singleCmd(CAT_VFO_AB);
	// mandatory delay to wait for the radio to apply the changes
	delay(200);
}

// Toggle the narrow value for the actual VFO
// with a fast switch of the VFO to apply
// NAR is bit 4 in byte base address + 1
bool FT817::toggleNar()
{
	return toggleBitFromVFO(1, 4);
}

// Toggle the IPO value for the actual VFO
// with a fast switch of the VFO to apply
// NAR is bit 5 in byte base address + 2
bool FT817::toggleIPO()
{
	return toggleBitFromVFO(2, 5);
}

// Toggle the BreakIn option
// BreakIn is bit 5 of EEPROM byte 58
bool FT817::toggleBreakIn()
{
	MSB = 0x00;
	LSB = 0x58;
	return toggleBitFromEEPROM(5);
}

// Toggle the Keyer status
// Keyer is bit 4 of EEPROM byte 58
bool FT817::toggleKeyer()
{
	MSB = 0x00;
	LSB = 0x58;
	return toggleBitFromEEPROM(4);
}

// Toggle the RF Gain / Squelch control
// Set by bit 7 of EEPROM byte 5F
bool FT817::toggleRfSql()
{
	MSB = 0x00;
	LSB = 0x5F;
	return toggleBitFromEEPROM(7);
}

/****** SET COMMANDS ********/

// set radio frequency directly (as a long integer)
// in 10hz steps
void FT817::setFreq(long freq)
{
	to_bcd_be(freq);
	buffer[4] = CAT_FREQ_SET;
	sendCmd();
	getByte();
}

// set radiomode using define values
void FT817::setMode(byte mode)
{
	// check for valid modes
	if ((mode < 0x05) | (mode == 0x06) | (mode == 0x08) | (mode == 0x0A) | (mode == 0x0C))
	{
		flushBuffer();
		buffer[0] = mode;
		buffer[4] = CAT_MODE_SET;
		// missing a sendCmd here...
		sendCmd();
		getByte();     
	}
}

// set the clarifier frequency
void FT817::clarFreq(long freq)
{
	// will come back to this later
}

// switch to a specific VFO
void FT817::switchVFO(bool vfo)
{
	if (getVFO() & !vfo) {
		toggleVFO();
	}
}

// control repeater offset direction
void FT817::rptrOffset(char * ofst)
{
	flushBuffer();
	buffer[0] = CAT_RPTR_OFFSET_S;	  // default to simplex
	buffer[4] = CAT_RPTR_OFFSET_CMD;  // command byte

	if (strcmp(ofst, "-") == 0)
		buffer[0] = CAT_RPTR_OFFSET_N;
	if (strcmp(ofst, "+") == 0)
		buffer[0] = CAT_RPTR_OFFSET_P;
	if (strcmp(ofst, "s") == 0)
		buffer[0] = CAT_RPTR_OFFSET_S;

	sendCmd();
	getByte();
}

// set the freq of the offset
void FT817::rptrOffsetFreq(long freq)
{
	freq = (freq * 100); // convert the incoming value to kHz
	to_bcd_be(freq);
	buffer[4] = CAT_RPTR_FREQ_SET; // command byte
	sendCmd();
	getByte();
}

// enable or disable various CTCSS and DCS squelch options
void FT817::squelch(char * mode)
{
	flushBuffer();
	buffer[0] = CAT_MODE_USB; // default to USB mode
	buffer[4] = CAT_SQL_CMD;  // command byte

	if (strcasecmp(mode,"DCS")==0)
		buffer[0] = CAT_SQL_DCS;
	if (strcasecmp(mode,"DDC")==0)
		buffer[0] = CAT_SQL_DCS_DECD;
	if (strcasecmp(mode,"DEN")==0)
		buffer[0] = CAT_SQL_DCS_ENCD;
	if (strcasecmp(mode,"TSQ")==0)
		buffer[0] = CAT_SQL_CTCSS;
	if (strcasecmp(mode,"TDC")==0)
		buffer[0] = CAT_SQL_CTCSS_DECD;
	if (strcasecmp(mode,"TEN")==0)
		buffer[0] = CAT_SQL_CTCSS_ENCD;
	if (strcasecmp(mode,"OFF")==0)
		buffer[0] = CAT_SQL_OFF;

	sendCmd();
	getByte();
}

void FT817::squelchFreq(unsigned int freq, char * sqlType)
{
	to_bcd_be((long)freq);

	if (strcasecmp(sqlType, "C") == 0)
		buffer[4] = CAT_SQL_CTCSS_SET;
	if (strcasecmp(sqlType, "D") == 0)
		buffer[4] = CAT_SQL_DCS_SET;

	sendCmd();
	getByte();
}

void FT817::setKeyerSpeed(int speed)
{
	byte wpm = constrain(speed, 4, 60);   	// Constrain input between FT-817 min and max keyer speed
	byte keyerSpeedSetting = wpm - 4;
	MSB = 0x00; // set the address to write to
	LSB = 0x62;
	readEEPROM();
	if (eepromValidData)
	{
		byte bitsAbove = 0b11000000 & actualByte;	// bits 6 and 7 from byte 0x62 must be kept (= Battery Charge Time)
		keyerSpeedSetting = bitsAbove | keyerSpeedSetting;
		writeEEPROM(keyerSpeedSetting);
	}
}


/****** GET COMMANDS ********/

// get the actual vfo from the eeprom
// 0 = A / 1 = B
// only valid if eepromDataValid is true
bool FT817::getVFO()
{
	MSB = 0x00;	// set the address to read
	LSB = 0x55;
	readEEPROM();
	return (bool)(actualByte & 0b00000001);    // 0 = VFO A, 1 = VFO B
}

// get the mode indirectly
byte FT817::getMode()
{
	getFreqMode();
	return mode;
}

// get the frequency and the current mode
// if called as getFreqMode() return only the frequency
unsigned long FT817::getFreqMode()
{
	flushBuffer();
	buffer[4] = CAT_RX_FREQ_CMD;

	rigCat.flush();
	sendCmd();
	getBytes(5);

	freq = from_bcd_be();
	mode = buffer[4];

	return freq;
}

// get the bands for the specified VFO
byte FT817::getBandVFO(bool vfo)
{
	// see the band specs in the .h file
	MSB = 0x00;	// set the address to read
	LSB = 0x59;
	readEEPROM();
	byte band = actualByte;
	if (vfo)
	{
		// B
		return band >> 4;
	}
	else
	{
		// A
		return band & 0b00001111;
	}
}

// determine if the radio is in TX state
// unless the radio is actively TX, the result is always
// 0x255 so any value other than 0x255 means TX !
boolean FT817::chkTX()
{
	flushBuffer();
	buffer[4] = CAT_TX_DATA_CMD;

	sendCmd();
	byte reply = getByte();

	if (reply == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

// get display selection
// values from 0x00 to 0x0B
// only valid if eepromDataValid is true
byte FT817::getDisplaySelection()
{
	MSB = 0x00;	// set the address to read
	LSB = 0x76;
	readEEPROM();
	return actualByte & 0b00001111;    // display selection is bits 0-3 of hex address 0x76
}

// get smeter value
byte FT817::getSMeter()
{
	flushBuffer();
	buffer[4] = CAT_RX_DATA_CMD;
	sendCmd();
	return getByte() & 0b00001111;
}

// get narrow state for the actual VFO
// NAR is bit 4 in byte base address + 1
bool FT817::getNar()
{
	return getBitFromVFO(1, 4);
}

// get IPO state for the actual VFO
// IPO is bit 5 in byte base address + 2
bool FT817::getIPO()
{
	return getBitFromVFO(2, 5);
}

// get BrakIn status from bit 5 in EEPROM address 0x58
bool FT817::getBreakIn()
{
	MSB = 0x00;
	LSB = 0x58;
	return getBitFromEEPROM(5);
}

// get Keyer status from bit 4 in EEPROM address 0x58
bool FT817::getKeyer()
{
	MSB = 0x00;
	LSB = 0x58;
	return getBitFromEEPROM(4);
}


/****** AUX PRIVATE  ********/

// gets a byte of input data from the radio
byte FT817::getByte()
{
	unsigned long startTime = millis();
	while (rigCat.available() < 1 && millis() < startTime + 2000) { ; }  // I see this came from the VE3BUX lib... but shouldn't something be inside this while{} ? Maybe a timeout?
	return rigCat.read();
}

// gets x bytes of input data from the radio
// and load it on the buffer MSBF
void FT817::getBytes(byte count)
{
	unsigned long startTime = millis();
	while (rigCat.available() < 1 && millis() < startTime + 2000) { ; }

	flushBuffer();
	for (byte i=0; i<count; i++)
	{
		buffer[i] = rigCat.read();
		delay(5);					// needs a delay in here, otherwise the byte sequence read is incorrect
	}
}

// this is the function which actually does
// the serial transaction to the radio
// it ALWAYS send the 5 bytes in the buffer
void FT817::sendCmd()
{
	for (byte i=0; i<5; i++)
	{
		rigCat.write(buffer[i]);
//		Serial.println(buffer[i]);        // debug aid
	}
}

// this function reduces total code-space by allowing for
// single byte commands to be issued (ie. all the toggles)
byte FT817::singleCmd(int cmd)
{
	flushBuffer();
	buffer[4] = cmd;
	sendCmd();

	return getByte();
}

// flush the rx buffer
void FT817::flushRX()
{
	rigCat.flush();
}

// empty the buffer
void FT817::flushBuffer()
{
	memset(buffer, 0, 5);
}

// read a position in the EEPROM from the MSB & LSB vars
// returns true if valid data (same value two times in a row)
// false if no valid data
// it loads two bytes, they are placed in actualByte & nextByte
bool FT817::readEEPROM()
{
	// set 'valid data' flag to false, we see two consequtive matching reads we set it to true
	eepromValidData = false;
	for (byte i=0; i<4; i++)
	{
		flushBuffer();
		buffer[0] = MSB;  // MSB EEPROM data byte
		buffer[1] = LSB;  // LSB EEPROM data byte
		buffer[4] = 0xBB; // BB command byte (read EEPROM data) for sendCmd();
		sendCmd();
		getBytes(2);
		if ((i > 0) & ((actualByte == buffer[0]) & (nextByte == buffer[1])))
		{
			eepromValidData = true;
			break;
		}
		else
		{
			actualByte = buffer[0];
			nextByte = buffer[1];
		}

		delay(20); // mandatory delay
	}

	return eepromValidData;
}

// write to the eeprom, the address is loaded from the MSB/LSB
// we pass the byte to write and load the nextByte from the last read
// if all goes well we return true, otherwise false
bool FT817::writeEEPROM(byte data)
{
	// perform a read cycle to load the nextByte, with some insistence..
	byte count = 3;
	while (!readEEPROM())
	{
		if (count == 0) { break; }
		count -= 1;
	}

	// test
	if (!eepromValidData) { return eepromValidData; }

	// load the data in the buffer, no need to flush it as it will be overwritten
	buffer[0] = MSB;
	buffer[1] = LSB;
	buffer[2] = data;
	buffer[3] = nextByte;
	buffer[4] = 0xBC;	// EEPROM WRITE (JUST ONE TIME)
	sendCmd();
	getByte();

	// almost all EEPROMs have a write delay, from 1 to 5 msecs
	// we go here for 10 msec, this must be adjusted in practice
	delay(10);

	// read it & check
	count = 3;
	while (!readEEPROM())
	{
		if (count == 0) { break; }
		count -= 1;
	}

	// test
	if (!eepromValidData) { return eepromValidData; }

	// compare
	if (actualByte != data)
	{
		return false;
	}
	else
	{
		return true;
	}
}

// get the bytes in the buffer and return it
// as a frequency in 10hz resolution
unsigned long FT817::from_bcd_be()
{
	// first four bytes from buffer are the freq data in binary coded decimal
	// {0x01,0x40,0x07,0x00,0x01} tunes to 14.070MHz
	freq = 0;
	for (byte i = 0; i < 4; i++)
	{
		freq *= 10;
		freq += buffer[i] >> 4;
		freq *= 10;
		freq += buffer[i] & 0x0f;
	}

	return freq;
}

// get the frequency in 10hz resolution and load
// it on the tx buffer 
void FT817::to_bcd_be(unsigned long f)
{
	unsigned char a;

	// clear the nullpad
	flushBuffer();

	for (int i=3; i>= 0; i--)
	{
		a = f % 10;
		f /= 10;
		a |= (f % 10) << 4;
		f /= 10;
		buffer[i] = a;
	}
}

// calc the eeprom base address of the actual VFO, returns true/false
// true is a confirmation of the eeprom readdings confirmed, also
// eepromValidData has the result also, the target base address will be 
// loaded to MSB/LSB
bool FT817::calcVFOaddr()
{
	// get the current vfo
	bool vfo = getVFO();
	if (!eepromValidData) { return false; }

	// get the vfo band
	byte band = getBandVFO(vfo);
	if (!eepromValidData) { return false; }

	// calc the base address
	unsigned int address = 0x7D + ((int)vfo * 390) + (band * 26);

	// load it on the MSB/LSB
	modAddr(address, 0);

	// return
	return true;
}

// Increment the address in MSB/LSB in a safe way
// you can pass "1" o "-10" safely as an variaton
// if address = 0 then load the values from MSB/LSB
// if you pass an address and zero variation it just
// load the values of address in the MSB/LSB
void FT817::modAddr(int address, signed int variation)
{
	if (address == 0)
	{
		// we need to work with the address in the MSB/LSB
		address = ((int)MSB << 8) + LSB;
	}

	// modify it
	address += variation;

	MSB = (byte)(address >> 8);
	LSB = (byte)(address & 0xFF);
}

// Returns the value of a specific bit from an eeprom address
// Must check the eepromValidData to know if it's output is valid 
// Address is get from MSB/LSB
bool FT817::getBitFromEEPROM(byte rbit)
{
	// get the final value and return it
	byte count = 3;
	while (!readEEPROM())
	{
		if (count == 0) { break; }
		count -= 1;
	}

	// test
	if (!eepromValidData) { return eepromValidData; }

	// return
	return (bool)(bitRead(actualByte, rbit));
}

// Toggle a specific bit from a eeprom address loaded in MSB/LSB
bool FT817::toggleBitFromEEPROM(byte rbit)
{
	// get the bit
	bool targetBit = getBitFromEEPROM(rbit);

	// success?
	if (!eepromValidData) { return eepromValidData; }

	// modify the byte with the toggled bit
	byte newData = bitWrite(actualByte, rbit, !targetBit);

	// write it back, with provisions
	byte count = 3;
	while (!writeEEPROM(newData))
	{
		if (count == 0) { break; }
		count -= 1;
	}

	// success?
	if (eepromValidData) { return true; } else { return false; }
}

// Returns the value of a specific bit counting a offset of x bytes
// from the actual VFO address
//
// Must check the eepromValidData to know if it's output is valid 
bool FT817::getBitFromVFO(signed int offset, byte rbit)
{
	// try to get the base address to the actual VFO
	byte count = 3;
	while (!calcVFOaddr())
	{
		if (count == 0) { break; }
		count -= 1;
	}

	// returns false can't calculate the data
	if (!eepromValidData) { return eepromValidData; }

	// we are targeting base address + offset
	modAddr(0, offset);

	// get the final value and return it
	count = 3;
	while (!readEEPROM())
	{
		if (count == 0) { break; }
		count -= 1;
	}

	// test
	if (!eepromValidData) { return eepromValidData; }

	// return
	return (bool)(bitRead(actualByte, rbit));
}

// Toggle a specific bit from a offset in the actual VFO
// returns true if success, false otherwise
bool FT817::toggleBitFromVFO(signed int offset, byte rbit)
{
	// get the bit
	bool targetBit = getBitFromVFO(offset, rbit);

	// success?
	if (!eepromValidData) { return eepromValidData; }

	// budify the byte with the toggled bit
	byte newData = bitWrite(actualByte, rbit, !targetBit);

	// program it back, but first switch the vfo
	toggleVFO();

	// write it back, with provisions
	byte count = 3;
	while (!writeEEPROM(newData))
	{
		if (count == 0) { break; }
		count -= 1;
	}

	// switch VFO back to target one no matter if success or not
	toggleVFO();

	// success?
	if (eepromValidData) { return true; } else { return false; }
}
