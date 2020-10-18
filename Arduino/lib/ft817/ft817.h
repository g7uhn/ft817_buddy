/*
ft817.h Arduino library for controlling a Yaesu FT817 radio via CAT commands.

Based on the work of James Buck, VE3BUX and his FT857D lib, see http://www.ve3bux.com

CAT commands for FT-817 radio taken from:
- The Official FT-817D Manual
- KA7OEI FT-817 pages](http://www.ka7oei.com/ft817pg.shtml)
- Hamlib radio control libraries

Library author is Pavel Milanes
- Email: pavelmc@gmail.com
- Github: @stdevPavelmc
- Twitter: @co7wt
- Telegram: @pavelmc

All CAT commands to the radio should be sent as 5-byte blocks. The commands are generally in the form of: 

	{P1,P2,P3,P4,CMD}
		where P1-P4 are parameters
		and CMD is the command code (ie. set mode)

----Lock On / Off------------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_LOCK_ON}
	{0x00,0x00,0x00,0x00,CAT_LOCK_OFF}

	Toggle the lock as: {0x00,0x00,0x00,0x00,CMD}
	CMD byte:
		0x00 = lock on				= CAT_LOCK_ON
		0x80 = lock off				= CAT_LOCK_OFF

	Eg: {0x00,0x00,0x00,0x00,0x00} enables the lock

----PTT On / Off-------------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_PTT_ON}
	{0x00,0x00,0x00,0x00,CAT_PTT_OFF}

	Toggle PTT as: {0x00,0x00,0x00,0x00,CMD}
	CMD byte:
		0x08 = PTT on				= CAT_PTT_ON
		0x88 = PTT off				= CAT_PTT_OFF

	Eg: {0x00,0x00,0x00,0x00,0x08} causes radio to TX

----Set Frequency------------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_FREQ_SET}

	Tune the radio to a frequency as: {P1,P2,P3,P4,0x01}
	Parameters:
		P1-P4 = frequency as: aa,bb,cc,dd
		
	Eg: {0x01,0x40,0x07,0x00,0x01} tunes to 14.070MHz
	Eg2:{0x14,0x43,0x90,0x00,0x01} tunes to 144.390MHz

----Operating Mode-----------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_MODE_SET}

	Set radio mode as: {P1,0x00,0x00,0x00,0x07}
	P1 Byte:
	0x00 = LSB						= CAT_MODE_LSB
	0x01 = USB						= CAT_MODE_USB
	0x02 = CW						= CAT_MODE_CW
	0x03 = CW-R						= CAT_MODE_CWR
	0x04 = AM						= CAT_MODE_AM
	0x06 = WBFM						= CAT_MODE_WBFM <<<<
	0x08 = FM						= CAT_MODE_FM
	0x0A = DIG						= CAT_MODE_DIG
	0x0C = PKT						= CAT_MODE_PKT

	Eg: {0x00,0x00,0x00,0x00,0x07} sets the radio to LSB

----Clar On / Off------------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_CLAR_ON}
	{0x00,0x00,0x00,0x00,CAT_CLAR_OFF}

	Toggle clarifier as: {0x00,0x00,0x00,0x00,CMD}
	CMD byte:
		0x05 = CLAR on				= CAT_CLAR_ON
		0x85 = CLAR off				= CAT_CLAR_OFF

	Eg: {0x00,0x00,0x00,0x00,0x05} turns on the clarifier

----Clar Frequency-----------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_CLAR_SET}

	Set clarifier frequency as: {P1,0x00,P3,P4,0xF5}
	P1 byte:
		0x00 = toggle between + and - offset
				0x00 is positive, any other values set negative
	P3,P4 bytes:
			= frequency as: aa,bb (BCD format)

	Eg: {0x00,0x00,0x12,0x34,0xF5} sets the clarifier to 12.34kHz
	Eg2:{0x01,0x00,0x06,0x50,0xF5} sets the clarifier to -6.5kHz

----VFO A/B------------------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_VFO_AB}

	Toggle between VFO A and B as: {0x00,0x00,0x00,0x00,CMD}
	CMD byte:
		0x81 = chg  between A & B		= CAT_VFO_AB

	Eg: {0x00,0x00,0x00,0x00,0x81} change from VFO A to VFO B

----Split On / Off-----------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_SPLIT_ON}
	{0x00,0x00,0x00,0x00,CAT_SPLIT_OFF}

	Toggle Split operation as: {0x00,0x00,0x00,0x00,CMD}
	CMD byte:
		0x02 = split on				= CAT_SPLIT_ON
		0x82 = split off			= CAT_SPLIT_OFF

	Eg: {0x00,0x00,0x00,0x00,0x02} enables split mode operation

----Repeater Offset---------------------------------------------
	{P1,0x00,0x00,0x00,CAT_RPTR_OFFSET_CMD}

	Set repeater offset as: {P1,0x00,0x00,0x00,0x09}
	P1 byte:
		0x09 = negative shift			= CAT_RPTR_OFFSET_N
		0x49 = positive shift			= CAT_RPTR_OFFSET_P
		0x89 = simplex					= CAT_RPTR_OFFSET_S

	Eg: {0x49,0x00,0x00,0x00,0x09} sets positive repeater shift

----Repeater Offset Frequency-----------------------------------
	{0x00,0x00,0x00,0x00,CAT_RPTR_FREQ_SET}

	Set repeater offset frequency as: {P1,P2,0x00,0x00,0xF9} <<<<
	Parameters:
		P1-P2 = frequency as: aa,bb
			10Mhz/100khz
			1Mhz/10khz
	Eg: {0x00,0x10,0x00,0x00,0xF9} sets offset to 100kHz
	Eg2:{0x00,0x60,0x00,0x00,0xF9} sets offset to 0.600MHz
	
----CTCSS / DCS Mode-------------------------------------------- <<< verify!!!
	{P1,0x00,0x00,0x00,CAT_SQL_CMD}

	Select CTCSS / DCS mode as: {P1,0x00,0x00,0x00,0x0A}
	P1 Byte:
	0x0A = DCS on					= CAT_SQL_DCS
	0x0B = DCS decoder on 			= CAT_SQL_DCS_DECD
	0x0C = DCS encoder on 			= CAT_SQL_DCS_ENCD
	0x2A = CTCSS on					= CAT_SQL_CTCSS
	0x3A = CTCSS decoder on			= CAT_SQL_CTCSS_DECD
	0x4A = DCS/CTCSS encoder on		= CAT_SQL_CTCSS_ENCD
	0x8A = off						= CAT_SQL_OFF

	Eg: {0x2A,0x00,0x00,0x00,0x0A} enables CTCSS tones on TX&RX

----CTCSS Tone-------------------------------------------------- <<< verify!!!
	{0x00,0x00,0x00,0x00,CAT_SQL_CTCSS_SET}

	Set CTCSS tones as: {P1,P2,P3,P4,0x0B}
	Parameters:
		P1-P2 = CTCSS tone on TX as: aa,bb
		P3-P4 = CTCSS tone on RX as: cc,dd
		
	Eg: {0x08,0x85,0x10,0x00,0x0B} CTCSS TX = 88.5Hz, RX = 100Hz
	Eg2:{0x10,0x00,0x10,0x00,0x0B} CTCSS TX&RX = 100Hz
	
----DCS Code---------------------------------------------------- <<< verify!!!
	{0x00,0x00,0x00,0x00,CAT_SQL_DCS_SET}

	Set DCS codes as: {P1,P2,P3,P4,0x0C}
	Parameters:
		P1-P2 = DCS code on TX as: aa,bb
		P3-P4 = DCS code on RX as: cc,dd

	Eg: {0x00,0x23,0x03,0x71,0x0C} DCS TX = 023, RX = 371
	
----Read RX Status (Squelch, CTCSS/DCS, Disc. Ctr, S-Meter)------ <<<<<<
	{0x00,0x00,0x00,0x00,CAT_RX_DATA_CMD}

	Read the RX status as: {0x00,0x00,0x00,0x00,E7}
		ONLY ON RX ON TX THE DATA IS NOT VALID!

	The radio will then output one byte of data:

		0bABCD_EFGH
		- A: squelch
			0 = silenced
			1 = signal present
		- B: CTCSS/DCS
			0 = CTCSS/DCS off or CTCSS/DCS enabled and match
			1 = CTCSS/DCS wrong code if enabled, or no CTCSS/DCS
		- C: discriminator
			0 = non FM mode or centered for FM
			1 = any FM mode and off centered
		- D: no useful information
		- EFGH: S-meter readdings
			00 - 09 = S0-S9
			0A = 10 dB Over
			0B = ...
			0F = 60 dB Over

	Eg: {0x00,0x00,0x00,0x00,0xE7} gets the radio status

----Read TX Status (PTT, SWR Hi/Lo, Split, PO Meter)------------ <<<<<
	{0x00,0x00,0x00,0x00,CAT_TX_DATA_CMD}

	Read the TX status as: {0x00,0x00,0x00,0x00,F7}

	The radio will then output one byte of data:

		0bABCD_EFGH
		- A: PTT
			0 = unkeyed
			1 = keyed
		- B: SWR
			0 = SWR low (no calibration yet)
			1 = SWR high (not calibration yet)
		- C: split
			0 = split on
			1 = split off
		- D: no useful information
		- EFGH: Power readings, no calibration yet

----Read Frequency / Mode Status---------------------------------
	{0x00,0x00,0x00,0x00,CAT_RX_FREQ_CMD}

	Read the frequency and mode as: {0x00,0x00,0x00,0x00,03}

	The radio will then output 5 bytes of data to report the current frequency and mode:

	{D1,D2,D3,D4,D5}

	Data:
		D1-D4 = frequency
		D5 = mode (same as set mode)

	Eg: {0x00,0x00,0x00,0x00,0x03} gets the current freq & mode
	
	Result from radio is:
		{0x43,0x97,0x00,0x00,0x08}
	which means:
		439.700MHz in mode 08 (FM)

==== Some extra features manipulating the EEPROM directly ======

getDisplaySelection()
	Returns a byte from 0x00 to 0x0B (0-11) corresponding to
	00 = VFO A/B, A=B, SPL
	01 = MW, MC, TAG
	02 = STO, RCL, PMS
	03 = RPT, REV, TON
	04 = SCN, PRI, DW
	05 = SSM, SCH, ART
	06 = IPO, ATT, NAR
	07 = NB, AGC
	08 = PWR, MTR
	09 = VOX, BK, KYR
	10 = CHG, VLT, DSP
	11 = TCH, DCH

getVFO()
	0 = VFO A
	1 = VFO B


getBandVFO(VFO)
	0000 = 160 M
	0001 = 75 M
	0010 = 40 M
	0011 = 30 M
	0100 = 20 M
	0101 = 17 M
	0110 = 15 M
	0111 = 12 M
	1000 = 10 M
	1001 = 6 M
	1010 = FM BCB
	1011 = Air
	1100 = 2 M
	1101 = UHF
	1110 = (Phantom)

----------------------------------------------------------------
*/

#ifndef CAT_h
#define CAT_h

#include <Arduino.h>
#include <SoftwareSerial.h>

#define CAT_LOCK_ON			0x00
#define CAT_LOCK_OFF		0x80
#define CAT_PTT_ON			0x08
#define CAT_PTT_OFF			0x88
#define CAT_FREQ_SET		0x01
#define CAT_MODE_SET		0x07
#define CAT_MODE_LSB		0x00
#define CAT_MODE_USB		0x01
#define CAT_MODE_CW			0x02
#define CAT_MODE_CWR		0x03
#define CAT_MODE_AM			0x04
#define CAT_MODE_WBFM		0x06
#define CAT_MODE_FM			0x08
#define CAT_MODE_DIG		0x0A
#define CAT_MODE_PKT		0x0C
#define CAT_CLAR_ON			0x05
#define CAT_CLAR_OFF		0x85
#define CAT_CLAR_SET		0xF5
#define CAT_VFO_AB			0x81
#define CAT_SPLIT_ON		0x02
#define CAT_SPLIT_OFF		0x82
#define CAT_RPTR_OFFSET_CMD	0x09
#define CAT_RPTR_OFFSET_N	0x09 // -
#define CAT_RPTR_OFFSET_P	0x49 // +
#define CAT_RPTR_OFFSET_S	0x89 // simplex
#define CAT_RPTR_FREQ_SET	0xF9
#define CAT_SQL_CMD			0x0A //{P1 ,0x00,0x00,0x00,0x0A}
#define CAT_SQL_DCS			0x0A // all values below are P1
#define CAT_SQL_DCS_DECD	0x0B // only useful in "split"
#define CAT_SQL_DCS_ENCD	0x0C
#define CAT_SQL_CTCSS		0x2A
#define CAT_SQL_CTCSS_DECD	0x3A
#define CAT_SQL_CTCSS_ENCD	0x4A
#define CAT_SQL_OFF			0x8A
#define CAT_SQL_CTCSS_SET	0x0B
#define CAT_SQL_DCS_SET		0x0C
#define CAT_RX_DATA_CMD		0xE7
#define CAT_TX_DATA_CMD		0xF7
#define CAT_RX_FREQ_CMD		0x03
#define CAT_NULL_DATA		0x00

class FT817
{
	public:
		FT817();
		// setup
		void setSerial(SoftwareSerial portInfo);	// load the softserial into the FT817
		void begin(int baud);						// set the baudrate of the softserial lib 

		// toggles
		void lock(boolean toggle);		// lock/unlock
		void PTT(boolean toggle);		// ptt/un-ptt
		void clar(boolean toggle);		// clar on / clar off
		void split(boolean toggle);		// split / single
		void toggleVFO();				// switch to the other VFO
		bool toggleNar();				// toggle the narrow status for the current VFO, switching
										// breifly to the other VFO and back, returns true is success
		bool toggleIPO();				// toggle the IPO status for the current VFO, switching
										// breifly to the other VFO and back, returns true is success
		bool toggleBreakIn();			// toggle BreakIn
		bool toggleKeyer();				// toggle Keyer status
		bool toggleRfSql();				// toggle RF Gain/SQL control

		// set commands
		void setFreq(long freq);	// in 10' of hz
		void setMode(byte mode);		// in text
		void clarFreq(long freq);		// 
		void switchVFO(bool vfo);		// 0 = A / 1 = B, checks the actual VFO to know if need to change
		void rptrOffset(char *ofst);	// "-" / "+" / "s"
		void rptrOffsetFreq(long freq);
		void squelch(char * mode);
		void squelchFreq(unsigned int, char * sqlType);
		void setKeyerSpeed(int speed);

		// get commands
		bool getVFO();					// return the actual VFO: 0 = A / 1 = B
		byte getMode();					// return a byte with the mode
		unsigned long getFreqMode();	// in 10' of hz
		byte getBandVFO(bool);			// return the band (see notes in the header of this file)
		boolean chkTX();				//
		byte getDisplaySelection();		// return a number that represents the row (see notes in the header of this file)
		byte getSMeter();				// as a byte (see notes in the header of this file)
		bool getNar();					// get the actual narrow status for the current VFO
		bool getIPO();					// get the IPO status for the actual VFO
		bool getBreakIn();				// get the Break In operation status
		bool getKeyer();				// toggle Keyer

		// vars
		bool eepromValidData = false;	// true of false of the last eeprom read will read 3 times
										// if two give same values on a row we flag it as valid

	private:
		// private & aux functions ands proceduies
		void getBytes(byte count);		// get 5 bytes and place it on the buffer MSBF
		byte getByte();					// get a single byte and return it
		void flushRX();					// empty any char in the softserial buffer
		void flushBuffer();				// zeroing the buffer
		void sendCmd();					// send the commands in the buffer
		byte singleCmd(int cmd);		// simplifies small cmds
		unsigned long from_bcd_be();	// convert the first 4 bytes in buffer to a freq in 10' of hz
		void to_bcd_be(unsigned long freq);		// get a freq in 10'of hz and place it on the buffer
		bool calcVFOaddr();				// calc the VFO address and place it on the MSB/LSB address
										// if calculations are correct eepromValidData will be true
										// and that value will be returned also
		void modAddr(int address, signed int variation);	// modify an address with the variation
															// if address is zero load it from MSB/LSB
		bool readEEPROM();				// read the eeprom, return bool, true if success, false otherwise
										// eeprom address is read from the MSB & LSB variables
										// it returns two bytes, that are loaded in actualByte & nextByte
		bool writeEEPROM(byte data);	// write data (performs a read cycle inside to preserve nextByte)
										// address is loaded from MSB/LSB, if all good return true
										// it returns true if all gone OK and can verify the integrity of
										// the wrote data.
		bool getBitFromEEPROM(byte rbit);		// get a bit position from an eeprom address loaded in MSB/LSB
		bool toggleBitFromEEPROM(byte rbit);	// toggle a bit position from an eeprom address loaded in MSB/LSB
		bool getBitFromVFO(signed int offset, byte rbit);	// this is a nice trick, it will return the bit
															// position you want in the actual VFO with an
															// offset in bytes...
		bool toggleBitFromVFO(signed int offset, byte rbit);	// this is another a nice trick, this will allow us to
																// toggle any bit position in the offset byte for the actual
																// base VFO

		// vars
		unsigned long freq;		// frequency data as a long
		byte mode;					// last mode read
		byte buffer[5];	// buffer used to TX and RX data to the radio
		byte MSB;					// MSB of the eeprom address | both used to calculate the address of
		byte LSB;					// LSB of the eeprom address | the eeprom
		byte actualByte;			// Actual byte requested by any EEPROM read operation
		byte nextByte;				// Next byte, aka: when you read or write you always get/set two bytes
									// for some operations we need to know that byte

};

#endif