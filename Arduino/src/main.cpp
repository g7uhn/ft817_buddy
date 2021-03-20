/*
  FT-817 Buddy v1.0
  A companion display and programmable soft-buttons for thr Yaesu FT-817.
  Project description:  http://alloutput.com/amateur-radio/ft-817-companion-display-controls/
  Project files:        https://github.com/g7uhn/ft817_buddy

  Building on the work of Clint Turner KA7OEI (http://www.ka7oei.com/ft817_meow.html) 
  and James Buck VE3BUX (https://ve3bux.com/study-guide/arduino-ft-857d-cat-library/)

  Put together by a team:
  Andy Webster G7UHN
  Michael Sansom G0POT
  Pavel Milanes Costa CO7WT

  IMPORTANT NOTE!  You MUST do/check two things before using this code:
  1) By using this code you acknowledge that you accept the risk that comes with writing to the FT-817 EEPROM i.e. an interruption
     or write error may reset your radio to pre-alignment factory defaults.  See KA7OEI's page here (http://www.ka7oei.com/ft817_meow.html)
     for details of how to record your own radio's unique factory settings such that you can restore your radio if the worst happens!
  2) Uncomment the line below to unlock the sketch that may write to the EEPROM.  You did record your own radio's 76 factory
     calibration settings from the "soft calibration" menu... didn't you?
  
  The authors above bear no responsibility for your use of this code and you proceed at your own risk.  
  
  This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License
*/

// uncomment the line below if you want to use this sketch that writes to the EEPROM and accept the responsibility stated above!  :-)
//#define EEPROM_WRITES

// uncomment the line below if you have fitted the Sparkfun DS1307 RTC module to the expansion header
// if no RTC module is fitted, this line should be commented out
//#define RTC_FITTED

// Include libraries
#include <Arduino.h>    // required for PlatformIO IDE (not required if you're using Arduino IDE)
#include <SPI.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <SoftwareSerial.h>
#include <ft817.h>
#include <SparkFunDS1307RTC.h>
#include <Wire.h>

// Declarations
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);    // pins for (CLK,DIN,D/C,CE,RST)
FT817 radio;              // define “radio” so that we may pass CAT and EEPROM commands

// Expansion software serial port: TX = digital pin 12, RX = digital pin 11
SoftwareSerial expansion(12,11);

// Define PCB pins
#define backlightPin 8       // backlight output pin (not the "LIGHT" input button!)
#define buttonPin A0         // SW1-SW6 arrive as different levels on analog input A0
#define sw7pin 11            // SW7 input is D11
#define sw8pin 10            // SW8 input is D10
#define sw9pin 9             // SW9 input is D9
#define keyerSw1 15          // A1 (pin 15) is Keyer SW1
#define keyerSw2 16          // A2 (pin 16) is Keyer SW2
#define keyerSw3 17          // A3 (pin 17) is Keyer SW3
#define keyerSw4 13          // D13 is Keyer SW4
#define SQW_INPUT_PIN 2      // Input pin to read RTC square wave


// Global variables - g7uhn TO DO: Needs a big tidy up here
bool sw7status = 1;       // using pullup, pressing button takes this low
bool sw8status = 1;       // using pullup, pressing button takes this low
bool sw9status = 1;       // using pullup, pressing button takes this low
bool backlightStatus = 0; // Backlight, initialises as OFF
bool timedBacklight = 0;  // is the Buddy performing a timed backlight operation?
bool expansionPage = 0;   // is the Buddy displaying the expansion page?
int backlightCounter = 0;
long freq;
String mode;
byte modeByte;
byte modeBase;    // used for the radio to return to this mode after generating Tune signal... needed to accomodate the CW/CWR/DIG NAR codes
byte modeReturn;
byte abcKeys = B1111;
int buttonStatus = 0;   // analog value of buttonPin...
int button = 0;         // ...button number
byte sMeter;
byte MSB;
byte LSB;
int currentPage = 2;    // initialise at the last page
bool softkeyStatus[6] = {0, 0, 0, 0, 0, 0};
short int hh = 0;             // clock hour
short int mm = 0;             // clock minute
short int ss = 0;             // clock seconds
long tickOver = 0;            // a counter to store time values (?)

// Forward declaration of functions (required for PlatformIO)
// If you create your own user functions below, you'll need to declare them here for PlatformIO to compile
void drawMainDisplay();       // draw main display
void displayABCkeys();        // EEPROM read
void displaySoftKeyStatus();  // possible EEPROM reads
void getReadableMode();       // standard CAT command
void backlight();             // no FT-817 interaction
void changePage();            // no FT-817 interaction
void tuneSignalOn();          // standard CAT commands
void tuneSignalOff();         // standard CAT commands
void pressKeyerSw1();         // no FT-817 interaction
void pressKeyerSw2();         // no FT-817 interaction
void pressKeyerSw3();         // no FT-817 interaction
void pressKeyerSw4();         // no FT-817 interaction
void longPressKeyerSw1();     // no FT-817 interaction
void incrementSeconds();      // no FT-817 interaction


/////////////   SETUP YOUR SOFT-KEY PAGES HERE!   ////////////////////////
// Single functions from the ft817 library can be called straight from the pageXSoftKeyFunctionY function
// or, if the desired function is a combination of actions, define a function in the USER FUNCTIONS section below
// to be called by the sketch.  If you make a new function, don't forget to forward declare your user function above.

// Page0 items
// SOFT-KEY 1 
String   page0SoftkeyLabel1 = "TUNon ";           // 6 characters
void  page0SoftkeyFunction1() {tuneSignalOn();}
boolean page0SoftkeyStatus1() {}
// SOFT-KEY 2
String   page0SoftkeyLabel2 = "IPO";              // 3 characters
void  page0SoftkeyFunction2() {radio.toggleIPO();}          // EEPROM write
boolean page0SoftkeyStatus2() {return radio.getIPO();}      // EEPROM read
// SOFT-KEY 3 
String   page0SoftkeyLabel3 = "KYR";              // 3 characters
void  page0SoftkeyFunction3() {radio.toggleKeyer();}        // EEPROM write
boolean page0SoftkeyStatus3() {return radio.getKeyer();}    // EEPROM read
// SOFT-KEY 4 
String   page0SoftkeyLabel4 = "TUNoff";           // 6 characters
void  page0SoftkeyFunction4() {tuneSignalOff();}
boolean page0SoftkeyStatus4() {}
// SOFT-KEY 5 
String   page0SoftkeyLabel5 = " BK";              // 3 characters
void  page0SoftkeyFunction5() {radio.toggleBreakIn();}      // EEPROM write
boolean page0SoftkeyStatus5() {return radio.getBreakIn();}  // EEPROM read
// SOFT-KEY 6 
String   page0SoftkeyLabel6 = "NAR";              // 3 characters
void  page0SoftkeyFunction6() {radio.toggleNar();}          // EEPROM write
boolean page0SoftkeyStatus6() {return radio.getNar();}      // EEPROM read


// Page1 items
// SOFT-KEY 1 
String   page1SoftkeyLabel1 = "getRTC";           // 6 characters
void  page1SoftkeyFunction1() {rtc.update(); ss = rtc.second(); mm = rtc.minute(); hh = rtc.hour();}  // get update from RTC and update Arduino's hh/mm/ss
boolean page1SoftkeyStatus1() {}
// SOFT-KEY 2 
String   page1SoftkeyLabel2 = "hh+";              // 3 characters
void  page1SoftkeyFunction2() {hh = (hh + 1) % 24; tickOver = millis(); delay(300);}
boolean page1SoftkeyStatus2() {}
// SOFT-KEY 3 
String   page1SoftkeyLabel3 = "hh-";              // 3 characters
void  page1SoftkeyFunction3() {if(hh==0) {hh=23;} else{hh = (hh - 1) % 24;}; tickOver = millis(); delay(300);}
boolean page1SoftkeyStatus3() {}
// SOFT-KEY 4 
String   page1SoftkeyLabel4 = "setRTC";           // 6 characters
void  page1SoftkeyFunction4() {rtc.setTime(0, mm, hh, 1, 1, 1, 21);}  // set RTC to hh:mm on the 1/1/21 (we're not interested in dates here, only time)
boolean page1SoftkeyStatus4() {}
// SOFT-KEY 5 
String   page1SoftkeyLabel5 = "mm+";              // 3 characters
void  page1SoftkeyFunction5() {mm = (mm + 1) % 60; tickOver = millis(); delay(300);}
boolean page1SoftkeyStatus5() {}
// SOFT-KEY 6
String   page1SoftkeyLabel6 = "mm-";              // 3 characters
void  page1SoftkeyFunction6() {if(mm==0) {mm=59;} else{mm = (mm - 1) % 60;}; tickOver = millis(); delay(300);}
boolean page1SoftkeyStatus6() {}


// Page2 items
// SOFT-KEY 1 
String   page2SoftkeyLabel1 = "CMD   ";           // 6 characters
void  page2SoftkeyFunction1() {longPressKeyerSw1();}
boolean page2SoftkeyStatus1() {}
// SOFT-KEY 2 
String   page2SoftkeyLabel2 = "M1 ";              // 3 characters
void  page2SoftkeyFunction2() {pressKeyerSw1();}
boolean page2SoftkeyStatus2() {}
// SOFT-KEY 3 
String   page2SoftkeyLabel3 = "M2 ";              // 3 characters
void  page2SoftkeyFunction3() {pressKeyerSw2();}
boolean page2SoftkeyStatus3() {}
// SOFT-KEY 4 
String   page2SoftkeyLabel4 = "   NAR";           // 6 characters
void  page2SoftkeyFunction4() {radio.toggleNar();}
boolean page2SoftkeyStatus4() {return radio.getNar();}
// SOFT-KEY 5 
String   page2SoftkeyLabel5 = " M3";              // 3 characters
void  page2SoftkeyFunction5() {pressKeyerSw3();}
boolean page2SoftkeyStatus5() {}
// SOFT-KEY 6
String   page2SoftkeyLabel6 = " M4";              // 3 characters
void  page2SoftkeyFunction6() {pressKeyerSw4();}
boolean page2SoftkeyStatus6() {}

///////////////  END OF SOFT-KEY PAGE SETUP   ////////////////////
//////////////////////////////////////////////////////////////////


void setup(void) 
{
  // Start serial
  radio.begin(38400);         // start the serial port for the CAT library

  #ifdef RTC_FITTED
  // Initialise RTC library
  rtc.begin();
  rtc.writeSQW(SQW_SQUARE_1);   // Sets SQW output to 1Hz
  attachInterrupt(digitalPinToInterrupt(SQW_INPUT_PIN), incrementSeconds, RISING);
  #endif
  
  // Start expansion software serial
  //expansion.begin(115200);    // start the expansion serial port at 115200 baud for GPS module

  // Set up some pins
  pinMode(backlightPin, OUTPUT);
  pinMode(sw7pin, INPUT_PULLUP);
  pinMode(sw8pin, INPUT_PULLUP);
  pinMode(sw9pin, INPUT_PULLUP);
  pinMode(keyerSw1, OUTPUT);
  pinMode(keyerSw2, OUTPUT);
  pinMode(keyerSw3, OUTPUT);
  pinMode(keyerSw4, OUTPUT);
  pinMode(SQW_INPUT_PIN, INPUT_PULLUP);

  digitalWrite(backlightPin, LOW);
  digitalWrite(keyerSw1, LOW);
  digitalWrite(keyerSw2, LOW);
  digitalWrite(keyerSw3, LOW);
  digitalWrite(keyerSw4, LOW);

  // Initialize Display
  display.begin();
  display.setContrast(62);    // you can change the contrast around to adapt the display for the best viewing! 57 is default and good for Sparkfun
  display.clearDisplay();     // Clear the buffer.

  // Set up the timer interrupt to watch for button status at 50 Hz (needed to catch a quick button press)
  cli();          // stop interrupts
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
  TCNT1  = 0;     // initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 311;   // = (16*10^6) / (1*1024*50) - 1 (must be <65536)
  TCCR1B |= (1 << WGM12);               // turn on CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10);  // Set CS10 and CS12 bits for 1024 prescaler
  TIMSK1 |= (1 << OCIE1A);              // enable timer compare interrupt
  sei();                                // allow interrupts


  // Initial draw of the main display by calling changePage()
  drawMainDisplay();
  changePage();

  #ifdef RTC_FITTED
  // Get RTC time (non-blocking and clock will start at 00:00 if no RTC available????)
  rtc.update();
  ss = rtc.second();
  mm = rtc.minute();
  hh = rtc.hour();
  #endif

} // end setup


void loop()  // MAIN LOOP
{
  #ifndef EEPROM_WRITES   // if user has not accepted risk/responsibility of EEPROM writes, display a reminder  :-)
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    display.println(" Please agree ");
    display.println(" to EEPROM    ");
    display.println(" writes to    ");
    display.println(" continue...  ");
    display.println(" See notes in ");
    display.println(" the code     ");
    display.display();
    delay(2000);
  #endif
  
  #ifdef EEPROM_WRITES    // if user has accepted the risk/responsibility of EEPROM writes, go ahead and run the main code block...

  // Using two loops to avoid polling the radio too quickly with things we don't need a fast refresh on:
  // 1) a "fast refresh" loop is run within the main loop for status items that we do want a quick refresh rate e.g. frequency and mode display
  // 2) actions we only need a "slow refresh" are performed outside of the fast loop in the main loop e.g. status bits against soft-key labels

  // FAST REFRESH LOOP starts here
  for (int i = 0; i <=3; i++)
  {
    // Get frequency
    freq = radio.getFreqMode(); //freq is a long integer of 10's of Hz
    int MHz = freq / 100000;
    long remainder = freq % 100000;
    float kHz = remainder / 100.00;

    // Get human readable mode data
    getReadableMode();

  //  // Get Receiver status (S-meter)
  //  byte rxStatusCmd[5] = {0x00, 0x00, 0x00, 0x00, 0xe7};
  //  radioCAT.sendCmd(rxStatusCmd, 5);
  //  sMeter = radioCAT.getByte();
  //  sMeter = sMeter << 4; //left shift the bits by 4 to make this an S-meter byte ranging from 00000000 to 11110000

  // BUTTON ACTIONS

  // If user has pushed one of the soft-keys (1-6)
    if (currentPage == 0)
    {
      if (button == 0) {goto endOfSoftKeyActions;}
      else if (button == 1) { page0SoftkeyFunction1(); softkeyStatus[0] = page0SoftkeyStatus1();}
      else if (button == 2) { page0SoftkeyFunction2(); softkeyStatus[1] = page0SoftkeyStatus2();}
      else if (button == 3) { page0SoftkeyFunction3(); softkeyStatus[2] = page0SoftkeyStatus3();}
      else if (button == 4) { page0SoftkeyFunction4(); softkeyStatus[3] = page0SoftkeyStatus4();}
      else if (button == 5) { page0SoftkeyFunction5(); softkeyStatus[4] = page0SoftkeyStatus5();}
      else if (button == 6) { page0SoftkeyFunction6(); softkeyStatus[5] = page0SoftkeyStatus6();}
      button = 0;  // reset button variable to zero once we've used it
      displaySoftKeyStatus();
    }
    else if (currentPage == 1)
    {
      if (button == 0) {goto endOfSoftKeyActions;}
      else if (button == 1) { page1SoftkeyFunction1(); softkeyStatus[0] = page1SoftkeyStatus1();}
      else if (button == 2) { page1SoftkeyFunction2(); softkeyStatus[1] = page1SoftkeyStatus2();}
      else if (button == 3) { page1SoftkeyFunction3(); softkeyStatus[2] = page1SoftkeyStatus3();}
      else if (button == 4) { page1SoftkeyFunction4(); softkeyStatus[3] = page1SoftkeyStatus4();}
      else if (button == 5) { page1SoftkeyFunction5(); softkeyStatus[4] = page1SoftkeyStatus5();}
      else if (button == 6) { page1SoftkeyFunction6(); softkeyStatus[5] = page1SoftkeyStatus6();}
      button = 0;  // reset button variable to zero once we've used it
      displaySoftKeyStatus();
    }
    else if (currentPage == 2)
    {
      if (button == 0) {goto endOfSoftKeyActions;}
      else if (button == 1) { page2SoftkeyFunction1(); softkeyStatus[0] = page2SoftkeyStatus1();}
      else if (button == 2) { page2SoftkeyFunction2(); softkeyStatus[1] = page2SoftkeyStatus2();}
      else if (button == 3) { page2SoftkeyFunction3(); softkeyStatus[2] = page2SoftkeyStatus3();}
      else if (button == 4) { page2SoftkeyFunction4(); softkeyStatus[3] = page2SoftkeyStatus4();}
      else if (button == 5) { page2SoftkeyFunction5(); softkeyStatus[4] = page2SoftkeyStatus5();}
      else if (button == 6) { page2SoftkeyFunction6(); softkeyStatus[5] = page2SoftkeyStatus6();}
      button = 0;  // reset button variable to zero once we've used it
      displaySoftKeyStatus();
    }
    endOfSoftKeyActions:

  if (sw7status == LOW)   // PAGE button
  {
    changePage();
    delay(200);           // short delay to prevent occasional "double-press"
    sw7status = HIGH;     // reset sw7status to high
  }

  if (sw8status == LOW)   // LIGHT button
  {
    backlight();
    delay(200);           // short delay to prevent occasional "double-press"
    sw8status = HIGH;     // reset sw8status to high
  }

  sw9status = HIGH;     // reset sw9status to high

    // FAST REFRESH STATUS DISPLAY ITEMS

    // Print dial frequency
    if (expansionPage != 1) {     // don't display freq on the expansion page
      display.setCursor(16, 32);
      display.setTextSize(1);
      display.setTextColor(BLACK, WHITE);
      display.print(MHz);
      display.print(",");
      if (kHz < 100) display.print('0');
      if (kHz < 10) display.print('0');
      display.print(kHz);
      if (MHz < 100) display.print(' ');
      if (MHz < 10) display.print(' ');
      display.print(' ');  // extra character to print over the occasional stray "8"
    }

    // Print current mode
    if (expansionPage != 1) {     // don't display mode on the expansion page
      display.setCursor(27, 22);
      display.setTextSize(1);
      display.setTextColor(BLACK, WHITE);
      display.print(mode);
    }

    //// Print S-meter
    //  tft.setCursor(50,120);
    //  int sMeterWidth = 5*(sMeter / 16);
    //  tft.drawRect(51,119,76,8,WHITE);
    //  tft.fillRect(52,120,sMeterWidth,6,WHITE);
    //  tft.fillRect((52+sMeterWidth),120,(74-sMeterWidth),6,BLACK);

    // Format and print time
    char buffer[6]="";    // This is the buffer for the string the sprintf outputs to
    sprintf(buffer, "%02d:%02d", hh, mm);
    display.setCursor(27, 12);
    display.print(buffer);


    #ifndef RTC_FITTED   // if RTC not fitted, use this clunky way out counting up the time in the main loop (RTC method uses hardware interrupt)
    // Calculate time value (at boot this is uptime or user can set hh:mm values, install RTC module to avoid having to set this every boot)
    if ( millis() - tickOver > 59999) {
      if (mm == 59) {
        hh = (hh + 1) % 24; // If mm == 59, increment hh modulo 24
      }
      mm = (mm + 1) % 60;  // Increment mm modulo 60
      tickOver = millis();
    }
    #endif

    delay(200);

    // write to display
    display.display();

  } // END OF FAST REFRESH LOOP (performed 4 times in every main loop)
  
// SLOW REFRESH STUFF


// Timed backlight countdown operation
if(timedBacklight == true)
{
  if(backlightCounter > 0)
  {
    backlightCounter = backlightCounter - 1;
  }
  else
  {
    digitalWrite(backlightPin, 0);    // turn the backlight off
    backlightStatus = 0;
    timedBacklight = 0;               // end the timed backlight
  }
}

display.display();  // update display

#endif
}  // END OF MAIN LOOP


// Interrupt code reading buttons at 50Hz
ISR(TIMER1_COMPA_vect) 
{ //change the 0 to 1 for timer1 and 2 for timer2
  // Get button status (levels determined by resistor network)
  buttonStatus = analogRead(buttonPin);
  if (buttonStatus < 80) {
    button = 1;
  }
  else if (buttonStatus < 240) {
    button = 2;
  }
  else if (buttonStatus < 400) {
    button = 3;
  }
  else if (buttonStatus < 570) {
    button = 4;
  }
  else if (buttonStatus < 750) {
    button = 5;
  }
  else if (buttonStatus < 900) {
    button = 6;
  }

  bool sw7 = digitalRead(sw7pin);
  if (!sw7)
  {
    sw7status = LOW;  // holds switch status LOW until it has been used in the main loop
  }

  bool sw8 = digitalRead(sw8pin);
  if (!sw8)
  {
    sw8status = LOW;  // holds switch status LOW until it has been used in the main loop
  }

  bool sw9 = digitalRead(sw9pin);
  if (!sw9)
  {
    sw9status = LOW;  // holds switch status LOW until it has been used in the main loop
  }
}


// Increment the clock driven by RTC 1Hz square wave interrupt
void incrementSeconds()
{
  if (ss == 59) {
    if (mm == 59) {
    hh = (hh + 1) % 24;   // if second = 59 AND minute = 59, increment the hour, modulo 24
    }
    mm = (mm + 1) % 60;   // if second = 59, increment the minute, modulo 60
  }
  ss = (ss + 1) % 60;   // increment the second, modulo 60
}


// Draw main display
void drawMainDisplay()
{
  // Initial display setup
  display.drawFastVLine(41, 0, 10, BLACK);
  display.drawFastVLine(24, 10, 21, BLACK);
  display.drawFastVLine(59, 10, 21, BLACK);
  display.drawFastHLine(0, 9, display.width(), BLACK);
  display.drawFastHLine(0, 20, 24, BLACK);
  display.drawFastHLine(60, 20, 24, BLACK);
  display.drawFastHLine(0, 30, 24, BLACK);
  display.drawFastHLine(60, 30, 24, BLACK);
  
  // Update the ABC keys
  displayABCkeys();

  // write to display
  display.display();
}


// Cycle through soft-key pages
void changePage()
{
  if (sw9status == LOW) {  // if SHIFT key is down, draw the location/time expansion page
    expansionPage = 1;
    display.clearDisplay();
    display.setCursor(0, 11);
    display.print("Time:");
    // Time value printed in main loop

    display.setCursor(0, 31);
    display.print("Page reserved");
    display.setCursor(0, 41);
    display.print("for expansion!");
  }
  else {
    if (expansionPage != 1) {           // if we're not coming from the expansion page...
      currentPage = ++currentPage % 3;  // ...increment the current page (3 pages)
    }
    else {                              // if we are coming from the expansion page we need to...
      display.clearDisplay();           // ...clear the display
      display.display();
      drawMainDisplay();
      
    }

    expansionPage = 0;                  // set expansionPage to false
    
    // Label the soft keys
    // may update this when I implement multiple pages of soft-keys...
    if (currentPage == 0)
    {
      display.setTextSize(1);
      //Button 1
      display.setCursor(5, 0);
      display.print(page0SoftkeyLabel1);
      //Button 2
      display.setCursor(5, 11);
      display.print(page0SoftkeyLabel2);
      //Button 3
      display.setCursor(5, 22);
      display.print(page0SoftkeyLabel3);
      //Button 4
      display.setCursor(44, 0);
      display.print(page0SoftkeyLabel4);
      //Button 5
      display.setCursor(62, 11);
      display.print(page0SoftkeyLabel5);
      //Button 6
      display.setCursor(62, 22);
      display.print(page0SoftkeyLabel6);
    }
    else if (currentPage ==1)
    {
      display.setTextSize(1);
      //Button 1
      display.setCursor(5, 0);
      display.print(page1SoftkeyLabel1);
      //Button 2
      display.setCursor(5, 11);
      display.print(page1SoftkeyLabel2);
      //Button 3
      display.setCursor(5, 22);
      display.print(page1SoftkeyLabel3);
      //Button 4
      display.setCursor(44, 0);
      display.print(page1SoftkeyLabel4);
      //Button 5
      display.setCursor(62, 11);
      display.print(page1SoftkeyLabel5);
      //Button 6
      display.setCursor(62, 22);
      display.print(page1SoftkeyLabel6);
    }
    else if (currentPage ==2)
    {
      display.setTextSize(1);
      //Button 1
      display.setCursor(5, 0);
      display.print(page2SoftkeyLabel1);
      //Button 2
      display.setCursor(5, 11);
      display.print(page2SoftkeyLabel2);
      //Button 3
      display.setCursor(5, 22);
      display.print(page2SoftkeyLabel3);
      //Button 4
      display.setCursor(44, 0);
      display.print(page2SoftkeyLabel4);
      //Button 5
      display.setCursor(62, 11);
      display.print(page2SoftkeyLabel5);
      //Button 6
      display.setCursor(62, 22);
      display.print(page2SoftkeyLabel6);
    }

    // Update the soft-key status indicators
    if (currentPage == 0)
    {
      softkeyStatus[0] = page0SoftkeyStatus1();
      softkeyStatus[1] = page0SoftkeyStatus2();
      softkeyStatus[2] = page0SoftkeyStatus3();
      softkeyStatus[3] = page0SoftkeyStatus4();
      softkeyStatus[4] = page0SoftkeyStatus5();
      softkeyStatus[5] = page0SoftkeyStatus6();
    }
    else if (currentPage == 1)
    {
      softkeyStatus[0] = page1SoftkeyStatus1();
      softkeyStatus[1] = page1SoftkeyStatus2();
      softkeyStatus[2] = page1SoftkeyStatus3();
      softkeyStatus[3] = page1SoftkeyStatus4();
      softkeyStatus[4] = page1SoftkeyStatus5();
      softkeyStatus[5] = page1SoftkeyStatus6();
    }
    else if (currentPage == 2)
    {
      softkeyStatus[0] = page2SoftkeyStatus1();
      softkeyStatus[1] = page2SoftkeyStatus2();
      softkeyStatus[2] = page2SoftkeyStatus3();
      softkeyStatus[3] = page2SoftkeyStatus4();
      softkeyStatus[4] = page2SoftkeyStatus5();
      softkeyStatus[5] = page2SoftkeyStatus6();
    }

    // Display the soft-key indicators
    displaySoftKeyStatus();
  }
}

void displaySoftKeyStatus()
{
  // Display the soft-key status indicators
  if (softkeyStatus[0])
  {
    display.drawFastVLine(0, 0, 9, BLACK);
    display.drawFastVLine(1, 0, 9, BLACK);
    display.drawFastVLine(2, 0, 9, BLACK);
  }
  else
  {
    display.drawFastVLine(0, 0, 9, WHITE);
    display.drawFastVLine(1, 0, 9, WHITE);
    display.drawFastVLine(2, 0, 9, WHITE);
  }
  if (softkeyStatus[1])
  {
    display.drawFastVLine(0, 10, 10, BLACK);
    display.drawFastVLine(1, 10, 10, BLACK);
    display.drawFastVLine(2, 10, 10, BLACK);
  }
  else
  {
    display.drawFastVLine(0, 10, 10, WHITE);
    display.drawFastVLine(1, 10, 10, WHITE);
    display.drawFastVLine(2, 10, 10, WHITE);
  }
  if (softkeyStatus[2])
  {
    display.drawFastVLine(0, 21, 9, BLACK);
    display.drawFastVLine(1, 21, 9, BLACK);
    display.drawFastVLine(2, 21, 9, BLACK);
  }
  else
  {
    display.drawFastVLine(0, 21, 9, WHITE);
    display.drawFastVLine(1, 21, 9, WHITE);
    display.drawFastVLine(2, 21, 9, WHITE);
  }
  if (softkeyStatus[3])
  {
    display.drawFastVLine(83, 0, 9, BLACK);
    display.drawFastVLine(82, 0, 9, BLACK);
    display.drawFastVLine(81, 0, 9, BLACK);
  }
  else
  {
    display.drawFastVLine(83, 0, 9, WHITE);
    display.drawFastVLine(82, 0, 9, WHITE);
    display.drawFastVLine(81, 0, 9, WHITE);
  }
  if (softkeyStatus[4])
  {
    display.drawFastVLine(83, 10, 10, BLACK);
    display.drawFastVLine(82, 10, 10, BLACK);
    display.drawFastVLine(81, 10, 10, BLACK);
  }
  else
  {
    display.drawFastVLine(83, 10, 10, WHITE);
    display.drawFastVLine(82, 10, 10, WHITE);
    display.drawFastVLine(81, 10, 10, WHITE);
  }
  if (softkeyStatus[5])
  {
    display.drawFastVLine(83, 21, 9, BLACK);
    display.drawFastVLine(82, 21, 9, BLACK);
    display.drawFastVLine(81, 21, 9, BLACK);
  }
  else
  {
    display.drawFastVLine(83, 21, 9, WHITE);
    display.drawFastVLine(82, 21, 9, WHITE);
    display.drawFastVLine(81, 21, 9, WHITE);
  }
}


// LIGHT = timed backlight, SHIFT+LIGHT = toggle backlight
void backlight()
{
  if (backlightStatus == 1)  // if backlight is already on, turn it off
  {
      digitalWrite(backlightPin, 0);
      backlightStatus = 0;
      timedBacklight = 0;
      backlightCounter = 0;
  }
  else if (sw9status == LOW)   // if SHIFT key is down, turn backlight on
  {
    digitalWrite(backlightPin, 1);
    backlightStatus = 1;
  }
  else                    // if SHIFT key is not down, begin a timed backlight
  {
    timedBacklight = 1;
    backlightCounter = 4;      // set the backlight counter to a number, this will be counted down in the main loop
    digitalWrite(backlightPin, 1);
    backlightStatus = 1;
  }
}


// Functions to trigger K1EL K16 keyer button input
void pressKeyerSw1()
{
  digitalWrite(keyerSw1, HIGH);
  delay(200);
  digitalWrite(keyerSw1, LOW);
}

void pressKeyerSw2()
{
  digitalWrite(keyerSw2, HIGH);
  delay(200);
  digitalWrite(keyerSw2, LOW);
}

void pressKeyerSw3()
{
  digitalWrite(keyerSw3, HIGH);
  delay(200);
  digitalWrite(keyerSw3, LOW);
}

void pressKeyerSw4()
{
  digitalWrite(keyerSw4, HIGH);
  delay(200);
  digitalWrite(keyerSw4, LOW);
}

void longPressKeyerSw1()    // long press of KeyerSw1 = "CMD"
{
  digitalWrite(keyerSw1, HIGH);
  delay(1600);
  digitalWrite(keyerSw1, LOW);
}


// USER FUNCTIONS
// Single functions from the ft817 library can be called straight from the pageXSoftKeyFunctionY function above
// or, if the desired function is a combination of actions, define a function below to be called by the sketch.


void tuneSignalOn() 
{
  sw9status = digitalRead(sw9pin);
  if (sw9status == LOW)     // is SHIFT key held down? (safety measure)
  {
    // Store current operating base mode i.e. ignoring the NAR mode codes
    modeReturn = modeBase;
    // Set mode to AM
    radio.setMode(CAT_MODE_AM);
    delay(200);
    // Set PTT to ON
    radio.PTT(true);
  }
}


void tuneSignalOff()
{
  // Set PTT to OFF
  radio.PTT(false);
  delay(200);
  // Set mode to <retrieve from modeReturn>
  radio.setMode(modeReturn);
}



void displayABCkeys()   // includes EEPROM read
{
  byte currentAbcKeys = radio.getDisplaySelection();

  abcKeys = currentAbcKeys;             // originally this function only updated the display if the abcKeys value changed (was continuously monitored)
  // Print current ABC keys
  display.setCursor(0, 41);
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  switch (abcKeys) {
    case B0000:
      display.print(" A/B  A=B  SPL");
      break;
    case B0001:
      display.print(" MW   MC   TAG");
      break;
    case B0010:
      display.print(" STO  RCL  PMS");
      break;
    case B0011:
      display.print(" RPT  REV  TON");
      break;
    case B0100:
      display.print(" SCN  PRI  DW ");
      break;
    case B0101:
      display.print(" SSM  SCH  ART");
      break;
    case B0110:
      display.print(" IPO  ATT  NAR");
      break;
    case B0111:
      display.print(" NB   AGC     ");
      break;
    case B1000:
      display.print(" PWR  MTR     ");
      break;
    case B1001:
      display.print(" VOX  BK   KYR");
      break;
    case B1010:
      display.print(" CHG  VLT  DSP");
      break;
    case B1011:
      display.print(" TCH  DCH     ");
      break;
  }
}



// g7uhn TO DO: surely this function can be better performed by some kind of lookup table?
void getReadableMode() {
  modeByte = radio.getMode();
  if (modeByte == 0x00) {
    mode = "LSB  ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x01) {
    mode = "USB  ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x02) {
    mode = "CW   ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x03) {
    mode = "CWR  ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x82) {
    mode = "CW N ";
    modeBase = 0x02;
  }
  else if (modeByte == 0x83) {
    mode = "CWR N";
    modeBase = 0x03;
  }
  else if (modeByte == 0x8A) {
    mode = "DIG N";
    modeBase = 0x0A;
  }
  else if (modeByte == 0x04) {
    mode = "AM   ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x06) {
    mode = "WFM  ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x08) {
    mode = "FM   ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x88) { // this one wasn't listed in the spec?
    mode = "FM-N ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x0A) {
    mode = "DIG  ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x0C) {
    mode = "PKT  ";
    modeBase = modeByte;
  }
  else {
    mode = "???";
  }
}