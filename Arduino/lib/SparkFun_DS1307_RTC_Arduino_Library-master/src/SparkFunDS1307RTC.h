/******************************************************************************
SparkFunDS1307RTC.h
Jim Lindblom @ SparkFun Electronics
original creation date: October 2, 2016
https://github.com/sparkfun/SparkFun_DS1307_RTC_Arduino_Library

Prototypes the DS1307 class. Defines sketch-usable global variables.

Resources:
Wire.h - Arduino I2C Library

Development environment specifics:
Arduino 1.6.8
SparkFun RedBoard
SparkFun Real Time Clock Module (v14)
******************************************************************************/
#ifndef SPARKFUNDS1307RTC_H
#define SPARKFUNDS1307RTC_H

#include <Arduino.h>
#include <Wire.h>

#define DS1307_RTC_ADDRESS 0x68 // DS1307 only has one I2C address - 0x68

#define TWELVE_HOUR_MODE (1<<6) // 12/24-hour Mode bit in Hour register
#define TWELVE_HOUR_PM (1<<5)   // am/pm bit in hour register

#define AM 0
#define PM 1

#define CONTROL_BIT_RS0  (1<<0) // RS0 bit in control register
#define CONTROL_BIT_RS1  (1<<1) // RS1 bit in control register
#define CONTROL_BIT_SQWE (1<<4) // Square wave enable bit in control register
#define CONTROL_BIT_OUT (1<<7)  // SQW Output value in control register

#define TIME_ARRAY_LENGTH 7 // Total number of writable values in device
enum time_order {
	TIME_SECONDS, // 0
	TIME_MINUTES, // 1
	TIME_HOURS,   // 2
	TIME_DAY,     // 3
	TIME_DATE,    // 4
	TIME_MONTH,   // 5
	TIME_YEAR,    // 6
};

// sqw_rate -- enum for possible SQW pin output settings
enum sqw_rate {
	SQW_SQUARE_1,
	SQW_SQUARE_4K,
	SQW_SQUARE_8K,
	SQW_SQUARE_32K,
	SQW_LOW,
	SQW_HIGH
};

// ds1307_register -- Definition of DS1307 registers
enum ds1307_registers {
	DS1307_REGISTER_SECONDS, // 0x00
	DS1307_REGISTER_MINUTES, // 0x01
	DS1307_REGISTER_HOURS,   // 0x02
	DS1307_REGISTER_DAY,     // 0x03
	DS1307_REGISTER_DATE,    // 0x04
	DS1307_REGISTER_MONTH,   // 0x05
	DS1307_REGISTER_YEAR,    // 0x06
	DS1307_REGISTER_CONTROL  // 0x07
};

// Base register for complete time/date readings
#define DS1307_REGISTER_BASE DS1307_REGISTER_SECONDS

// dayIntToStr -- convert day integer to the string
static const char *dayIntToStr[7] {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"	
};

// dayIntToChar -- convert day integer to character
static const char dayIntToChar[7] = {'U', 'M', 'T', 'W', 'R', 'F', 'S'};

class DS1307
{
public:
	
	////////////////////
	// Initialization //
	////////////////////
	// Constructor -- Initialize class variables to 0
	DS1307();
	// Begin -- Initialize I2C interface
	void begin(void);
	
	///////////////////////
	// Setting the Clock //
	///////////////////////
	// setTime -- Set time and date/day registers of DS1307
	bool setTime(uint8_t sec, uint8_t min, uint8_t hour,
	             uint8_t day, uint8_t date, uint8_t month, uint8_t year);
	// setTime -- Set time and date/day registers of DS1307 (using data array)
	bool setTime(uint8_t * time, uint8_t len);
	// autoTime -- Set time with compiler time/date
	bool autoTime();
	
	// To set specific values of the clock, use the set____ functions:
	bool setSecond(uint8_t s);
	bool setMinute(uint8_t m);
	bool setHour(uint8_t h);
	bool setDay(uint8_t d);
	bool setDate(uint8_t d);
	bool setMonth(uint8_t mo);
	bool setYear(uint8_t y);
	
	///////////////////////
	// Reading the Clock //
	///////////////////////
	// update -- Read all time/date registers and update the _time array
	bool update(void);
	// update should be performed before any of the following. It will update
	//   all values at one time.
	inline uint8_t second(void) { return BCDtoDEC(_time[TIME_SECONDS]); };
	inline uint8_t minute(void) { return BCDtoDEC(_time[TIME_MINUTES]); };
	inline uint8_t hour(void) { return BCDtoDEC(_time[TIME_HOURS]); };
	inline uint8_t day(void) { return BCDtoDEC(_time[TIME_DAY]); };
	inline const char dayChar(void) { return dayIntToChar[BCDtoDEC(_time[TIME_DAY]) - 1]; };
	inline const char * dayStr(void) { return dayIntToStr[BCDtoDEC(_time[TIME_DAY]) - 1]; };
	inline uint8_t date(void) { return BCDtoDEC(_time[TIME_DATE]); };
	inline uint8_t month(void) { return BCDtoDEC(_time[TIME_MONTH]);	};
	inline uint8_t year(void) { return BCDtoDEC(_time[TIME_YEAR]); };
	
	// To read a single value at a time, use the get___ functions:
	uint8_t getSecond(void);
	uint8_t getMinute(void);
	uint8_t getHour(void);
	uint8_t getDay(void);
	uint8_t getDate(void);
	uint8_t getMonth(void);
	uint8_t getYear(void);
	
	// is12Hour -- check if the DS1307 is in 12-hour mode | returns true if 12-hour mode
	bool is12Hour(void);
	// pm -- Check if 12-hour state is AM or PM | returns true if PM
	bool pm(void);
	
	///////////////////////////////
	// SQW Pin Control Functions //
	///////////////////////////////
	void writeSQW(uint8_t high); // Write SQW pin high or low
	void writeSQW(sqw_rate value); // Write SQW pin high, low, or to a set rate
	
	/////////////////////////////
	// Misc. Control Functions //
	/////////////////////////////
	void enable(void); // Enable the oscillator
	void disable(void); // Disable the oscillator (no counting!)
	
	bool set12Hour(bool enable12 = true); // Enable/disable 12-hour mode
	bool set24Hour(bool enable24 = true); // Enable/disable 24-hour mode
	
private:
	uint8_t _time[TIME_ARRAY_LENGTH];
	bool _pm;
	
	uint8_t BCDtoDEC(uint8_t val);
	uint8_t DECtoBCD(uint8_t val);
	
	bool i2cWriteBytes(uint8_t deviceAddress, ds1307_registers reg, uint8_t * values, uint8_t len);
	bool i2cWriteByte(uint8_t deviceAddress, ds1307_registers reg, uint8_t value);
	uint8_t i2cReadByte(uint8_t deviceAddress, ds1307_registers reg);
	bool i2cReadBytes(uint8_t deviceAddress, ds1307_registers reg, uint8_t * dest, uint8_t len);
};

extern DS1307 rtc;

#endif // SPARKFUNDS1307RTC_H