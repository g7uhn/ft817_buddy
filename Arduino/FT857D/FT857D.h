/*
  FT857D.h		Arduino library for controlling a Yaesu FT857D
			radio via CAT commands.

 Version:  0.1
 Created:  2012.08.16
Released:  2012.08.17
  Author:  James Buck, VE3BUX
     Web:  http://www.ve3bux.com

CAT commands for FT-857D radio taken from the FT-857D Manual (page 66):
	
http://www.yaesu.co.uk/files/FT-857D_Operating%20Manual.pdf

All CAT commands to the radio should be sent as 5-byte blocks. The commands are generally in the form of: 

	{P1,P2,P3,P4,CMD}
		where P1-P4 are parameters
		and   CMD is the command code (ie. set mode)

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
	0x00 = LSB						 = CAT_MODE_LSB
	0x01 = USB						 = CAT_MODE_USB
  	0x02 = CW						 = CAT_MODE_CW
  	0x03 = CW-R						 = CAT_MODE_CWR
  	0x04 = AM						 = CAT_MODE_AM
  	0x08 = FM						 = CAT_MODE_FM
  	0x0A = DIG						 = CAT_MODE_DIG
  	0x0C = PKT						 = CAT_MODE_PKT
  	0x88 = FM-N						 = CAT_MODE_FMN

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
	P3,P4 bytes:
			= frequency as: aa,bb

	Eg: {0x00,0x12,0x34,0xF5} sets the clarifier to 12.34kHz
	Eg2:{0x00,0x06,0x50,0xF5} sets the clarifier to -6.5kHz

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
		0x82 = split off				= CAT_SPLIT_OFF

	Eg: {0x00,0x00,0x00,0x00,0x02} enables split mode operation

----Repeater Offset---------------------------------------------
	{P1,0x00,0x00,0x00,CAT_RPTR_OFFSET_CMD}

	Set repeater offset as: {P1,0x00,0x00,0x00,0x09}
	P1 byte:
		0x09 = negative shift			= CAT_RPTR_OFFSET_N
		0x49 = positive shift			= CAT_RPTR_OFFSET_P
		0x89 = simplex				= CAT_RPTR_OFFSET_S

	Eg: {0x49,0x00,0x00,0x00,0x09} sets positive repeater shift

----Repeater Offset Frequency-----------------------------------
	{0x00,0x00,0x00,0x00,CAT_RPTR_FREQ_SET}

	Set repeater offset frequency as: {P1,P2,P3,P4,0xF9}
	Parameters:
		P1-P4 = frequency as: aa,bb,cc,dd
		
	Eg: {0x05,0x43,0x21,0x00,0xF9} sets offset to 5.4321MHz
	Eg2:{0x00,0x60,0x00,0x00,0xF9} sets offset to 0.600MHz
	
----CTCSS / DCS Mode--------------------------------------------
	{P1,0x00,0x00,0x00,CAT_SQL_CMD}

	Select CTCSS / DCS mode as: {P1,0x00,0x00,0x00,0x0A}
	P1 Byte:
	0x0A = DCS on					= CAT_SQL_DCS
	0x0B = DCS decoder on 			= CAT_SQL_DCS_DECD
  	0x0C = DCS encoder on 			= CAT_SQL_DCS_ENCD
  	0x2A = CTCSS on					= CAT_SQL_CTCSS
  	0x3A = CTCSS decoder on			= CAT_SQL_CTCSS_DECD
  	0x4A = CTCSS encoder on			= CAT_SQL_CTCSS_ENCD
  	0x8A = off						= CAT_SQL_OFF

	Eg: {0x2A,0x00,0x00,0x00,0x0A} enables CTCSS tones on TX&RX

----CTCSS Tone--------------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_SQL_CTCSS_SET}

	Set CTCSS tones as: {P1,P2,P3,P4,0x0B}
	Parameters:
		P1-P2 = CTCSS tone on TX as: aa,bb
		P3-P4 = CTCSS tone on RX as: cc,dd
		
	Eg: {0x08,0x85,0x10,0x00,0x0B} CTCSS TX = 88.5Hz, RX = 100Hz
	Eg2:{0x10,0x00,0x10,0x00,0x0B} CTCSS TX&RX = 100Hz
	
----DCS Code----------------------------------------------------
	{0x00,0x00,0x00,0x00,CAT_SQL_DCS_SET}

	Set DCS codes as: {P1,P2,P3,P4,0x0C}
	Parameters:
		P1-P2 = DCS code on TX as: aa,bb
		P3-P4 = DCS code on RX as: cc,dd

	Eg: {0x00,0x23,0x03,0x71,0x0C} DCS TX = 023, RX = 371
	
----Read RX Status (Squelch, CTCSS/DCS, Disc. Ctr, S-Meter)------
	{0x00,0x00,0x00,0x00,CAT_RX_DATA_CMD}

	Read the RX status as: {0x00,0x00,0x00,0x00,E7}

	The radio will then output 4 bytes of data to report the 	various parameters:

	{D1,D2,D3,D4}

	Data:
		D1 = {0xij}	i = 0 = squelch off
					i = 1 = squelch on
					j = 0 = CTCSS/DCS matched
					j = 1 = CTCSS/DCS unmatched
		D2 = {0xkl}	k = 0 = discriminator centered
					k = 1 = discriminator offcentered
					l = dummy data
		D3-D4 = S-meter data

	Eg: {0x00,0x00,0x00,0x00,0xE7} gets the radio status

----Read TX Status (PTT, SWR Hi/Lo, Split, PO Meter)------------
	{0x00,0x00,0x00,0x00,CAT_TX_DATA_CMD}

	Read the TX status as: {0x00,0x00,0x00,0x00,F7}

	The radio will then output 4 bytes of data to report the 	various parameters:

	{D1,D2,D3,D4}

	Data:
		D1 = {0xmn}	m = 0 = PTT off
					m = 2 = PTT on
					n = 0 = HI SWR off
					n = 1 = HI SWR on
		D2 = {0xpq}	p = 0 = split on
					p = 1 = split off
					q = dummy data
		D3-D4 = PO meter data

----Read Frequency / Mode Status---------------------------------
	{0x00,0x00,0x00,0x00,CAT_RX_FREQ_CMD}

	Read the frequency and mode as: {0x00,0x00,0x00,0x00,03}

	The radio will then output 5 bytes of data to report the 	current frequency and mode:

	{D1,D2,D3,D4,D5}

	Data:
		D1-D4 = frequency
		D5 = mode (same as set mode)

	Eg: {0x00,0x00,0x00,0x00,0x03} gets the current freq & mode
	
	Result from radio is:
		{0x43,0x97,0x00,0x00,0x08}
	which means:
		439.700MHz in mode 08 (FM)

----------------------------------------------------------------
*/

#ifndef CAT_h
#define CAT_h

#include <Arduino.h>
#include <SoftwareSerial.h>

#define CAT_LOCK_ON				0x00
#define CAT_LOCK_OFF			0x80
#define CAT_PTT_ON				0x08
#define CAT_PTT_OFF				0x88
#define CAT_FREQ_SET			0x01
#define CAT_MODE_SET			0x07
#define CAT_MODE_LSB			0x00
#define CAT_MODE_USB			0x01
#define CAT_MODE_CW				0x02
#define CAT_MODE_CWR			0x03
#define CAT_MODE_AM				0x04
#define CAT_MODE_FM				0x08
#define CAT_MODE_DIG			0x0A
#define CAT_MODE_PKT			0x0C
#define CAT_MODE_FMN			0x88
#define CAT_CLAR_ON				0x05
#define CAT_CLAR_OFF			0x85
#define CAT_CLAR_SET			0xF5
#define CAT_VFO_AB				0x81
#define CAT_SPLIT_ON			0x02
#define CAT_SPLIT_OFF			0x82
#define CAT_RPTR_OFFSET_CMD		0x09
#define CAT_RPTR_OFFSET_N		0x09 // -
#define CAT_RPTR_OFFSET_P		0x49 // +
#define CAT_RPTR_OFFSET_S		0x89 // simlex
#define CAT_RPTR_FREQ_SET		0xF9
#define CAT_SQL_CMD				0x0A //{P1 ,0x00,0x00,0x00,0x0A}
#define CAT_SQL_DCS				0x0A // all values below are P1
#define CAT_SQL_DCS_DECD		0x0B // only useful in "split"
#define CAT_SQL_DCS_ENCD		0x0C
#define CAT_SQL_CTCSS			0x2A
#define CAT_SQL_CTCSS_DECD		0x3A
#define CAT_SQL_CTCSS_ENCD		0x4A
#define CAT_SQL_OFF				0x8A
#define CAT_SQL_CTCSS_SET		0x0B
#define CAT_SQL_DCS_SET			0x0C
#define CAT_RX_DATA_CMD			0xE7
#define CAT_TX_DATA_CMD			0xF7
#define CAT_RX_FREQ_CMD			0x03
#define CAT_NULL_DATA			0x00

class FT857D
{
  public:
	FT857D();
	void setSerial(SoftwareSerial portInfo);
	void begin(int baud);

	void lock(boolean toggle);
	void PTT(boolean toggle);
	void setFreq(long freq);
	void setMode(char * mode);
	void clar(boolean toggle);
	void clarFreq(long freq);
	void switchVFO();
	void split(boolean toggle);
	void rptrOffset(char * ofst);
	void rptrOffsetFreq(long freq);
	void squelch(char * mode);
	void squelchFreq(unsigned int, char * sqlType);
	byte getMode();
	unsigned long getFreqMode();
	boolean chkTX();
	void flushRX();
// moved from private to make directly available to main sketch...
    void sendByte(byte cmd);
	void sendCmd(byte cmd[], byte len);
	byte singleCmd(int cmd);		// simplifies small cmds
	byte getByte();

  private:
	unsigned char * converted;		// holds the converted freq
	unsigned long freq;			// frequency data as a long
	unsigned char tempWord[4];		// temp value during conv.
	byte mode;

//	void sendCmd(byte cmd[], byte len);
//	byte singleCmd(int cmd);		// simplifies small cmds
//	byte getByte();

//	void sendByte(byte cmd);
	unsigned long from_bcd_be(const byte bcd_data[], unsigned bcd_len);
	unsigned char * to_bcd_be( byte bcd_data[], unsigned long freq, unsigned bcd_len);
	void comError(char * string);
};

#endif