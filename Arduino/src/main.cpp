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

  IMPORTANT NOTE!  You must do/check two things before using this code:
  1) In the FT817.cpp library file, check the SoftwareSerial pin number are set to (12, 11) for the FT817 Buddy PCB.
  2) Uncomment the line below to unlock EEPROM writes and acknowledge you accept the risk that comes with writing to the
     FT-817 EEPROM i.e. an interruption or write error may reset your radio to pre-alignment factory defaults.
     The authors above bear no responsibility for your use of this code and you proceed at your own risk.  See KA7OEI's 
     page here (http://www.ka7oei.com/ft817_meow.html) for details of how to record your own radio's unique factory settings
     such that you can restore your radio if the worst happens!  :-)
*/

// Include libraries
#include <Arduino.h>    // required for PlatformIO IDE (not required if you're using Arduino IDE)
#include <SPI.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <FT817.h>      // https://github.com/stdevPavelmc/ft817/tree/main/lib/ft817 

// uncomment this if you want to use the functions that WRITE to the EEPROM 
#define EEPROM_WRITES

// Declarations
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);    // pins for (CLK,DIN,D/C,CE,RST)
FT817 radio;              // define “radio” so that we may pass CAT and EEPROM commands

// Define PCB pins
int backlightPin = 8;     // backlight output pin (not the "LIGHT" input button!)
int buttonPin = A0;       // SW1-SW6 arrive as different levels on analog input A0
int sw7pin = 2;           // SW7 input is D2
int sw8pin = 10;          // SW8 input is D10
int sw9pin = 9;           // SW9 input is D9

// Global variables - g7uhn TO DO: Needs a big tidy up here
bool sw7status = 1;   // using pullup, pressing button takes this low
bool sw8status = 1;   // using pullup, pressing button takes this low
bool sw9status = 1;   // using pullup, pressing button takes this low
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
// New variables for soft-key pages and soft-key status
int currentPage = 1;    // initialise at the last page
bool softkeyStatus[6] = {0, 0, 0, 0, 0, 0};

// Forward declaration of functions (required for PlatformIO)
// If you create your own user functions below, you'll need to declare them here for PlatformIO to compile
void displayABCkeys();
void getReadableMode();
void backlight();
void changePage();
void tuneSignalOn();
void tuneSignalOff();
unsigned char ToByte(bool b[8]);  // debug aid, to be removed when unused


// SETUP SOFT-KEY PAGES
// Single functions from the ft817 library can be called straight from the pageXSoftKeyFunctionY function above
// or, if the desired function is a combination of actions, define a function in the USER FUNCTIONS section below
// to be called by the sketch.  Don't forget to forward declare your user function above.

// Page0 items
// SOFT-KEY 1 
String   page0SoftkeyLabel1 = "TUNon ";           // 6 characters
void  page0SoftkeyFunction1() {tuneSignalOn();}
boolean page0SoftkeyStatus1() {}
// SOFT-KEY 2
String   page0SoftkeyLabel2 = "IPO";              // 3 characters
void  page0SoftkeyFunction2() {radio.toggleIPO();}
boolean page0SoftkeyStatus2() {return radio.getIPO();}
// SOFT-KEY 3 
String   page0SoftkeyLabel3 = "KYR";              // 3 characters
void  page0SoftkeyFunction3() {radio.toggleKeyer();}
boolean page0SoftkeyStatus3() {return radio.getKeyer();}
// SOFT-KEY 4 
String   page0SoftkeyLabel4 = "TUNoff";           // 6 characters
void  page0SoftkeyFunction4() {tuneSignalOff();}
boolean page0SoftkeyStatus4() {}
// SOFT-KEY 5 
String   page0SoftkeyLabel5 = " BK";              // 3 characters
void  page0SoftkeyFunction5() {radio.toggleBreakIn();}
boolean page0SoftkeyStatus5() {return radio.getBreakIn();}
// SOFT-KEY 6 
String   page0SoftkeyLabel6 = "NAR";              // 3 characters
void  page0SoftkeyFunction6() {radio.toggleNar();}
boolean page0SoftkeyStatus6() {return radio.getNar();}

// Page1 items
// SOFT-KEY 1 
String   page1SoftkeyLabel1 = "RF/SQL";           // 6 characters
void  page1SoftkeyFunction1() {radio.toggleRfSql();}
boolean page1SoftkeyStatus1() {}
// SOFT-KEY 2 
String   page1SoftkeyLabel2 = "A/B";              // 3 characters
void  page1SoftkeyFunction2() {radio.toggleVFO();}
boolean page1SoftkeyStatus2() {}
// SOFT-KEY 3 
String   page1SoftkeyLabel3 = "NAR";              // 3 characters
void  page1SoftkeyFunction3() {radio.toggleNar();}
boolean page1SoftkeyStatus3() {return radio.getNar();}
// SOFT-KEY 4 
String   page1SoftkeyLabel4 = "   NAR";           // 6 characters
void  page1SoftkeyFunction4() {radio.toggleNar();}
boolean page1SoftkeyStatus4() {return radio.getNar();}
// SOFT-KEY 5 
String   page1SoftkeyLabel5 = "NAR";              // 3 characters
void  page1SoftkeyFunction5() {radio.toggleNar();}
boolean page1SoftkeyStatus5() {return radio.getNar();}
// SOFT-KEY 6
String   page1SoftkeyLabel6 = "NAR";              // 3 characters
void  page1SoftkeyFunction6() {radio.toggleNar();}
boolean page1SoftkeyStatus6() {return radio.getNar();}


void setup(void) 
{
  // Start serial
  Serial.begin(9600);       // serial port for the main sketch to talk to radio... is this necessary after the refactor?
  radio.begin(9600);        // start the serial port for the CAT library

  // Set up some pins
  pinMode(backlightPin, OUTPUT);
  pinMode(sw7pin, INPUT_PULLUP);
  pinMode(sw8pin, INPUT_PULLUP);
  pinMode(sw9pin, INPUT_PULLUP);

  // Initialize Display
  display.begin();
  display.setContrast(57);    // you can change the contrast around to adapt the display for the best viewing!
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

  // Initial display setup
  display.drawFastVLine(41, 0, 10, BLACK);
  display.drawFastVLine(24, 10, 21, BLACK);
  display.drawFastVLine(59, 10, 21, BLACK);
  display.drawFastHLine(0, 9, display.width(), BLACK);
  display.drawFastHLine(0, 20, 24, BLACK);
  display.drawFastHLine(60, 20, 24, BLACK);
  display.drawFastHLine(0, 30, 24, BLACK);
  display.drawFastHLine(60, 30, 24, BLACK);
  
  // Display the FT-817 A,B,C keys    g7uhn TO DO: move this into a periodic check of radio status along with SW1-6 status indications
  //displayABCkeys();
  
  // Display the soft keys by calling changePage()
  changePage();

  // write to display
  display.display();

} // end setup


void loop()  // MAIN LOOP
{
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

    if (currentPage == 0)
    {
      if (button == 0) {}
      else if (button == 1) { page0SoftkeyFunction1(); }
      else if (button == 2) { page0SoftkeyFunction2(); }
      else if (button == 3) { page0SoftkeyFunction3(); }
      else if (button == 4) { page0SoftkeyFunction4(); }
      else if (button == 5) { page0SoftkeyFunction5(); }
      else if (button == 6) { page0SoftkeyFunction6(); }
      button = 0;  // reset button variable to zero once we've used it
    }
    else if (currentPage == 1)
    {
      if (button == 0) {}
      else if (button == 1) { page1SoftkeyFunction1(); }
      else if (button == 2) { page1SoftkeyFunction2(); }
      else if (button == 3) { page1SoftkeyFunction3(); }
      else if (button == 4) { page1SoftkeyFunction4(); }
      else if (button == 5) { page1SoftkeyFunction5(); }
      else if (button == 6) { page1SoftkeyFunction6(); }
      button = 0;  // reset button variable to zero once we've used it
    }

    if (sw7status == LOW)   // PAGE button
    {
      changePage();
      delay(300);           // delay prevents series of rapid page changes 
      sw7status = HIGH;     // reset sw7status to high once we've used it
    }

    if (sw8status == LOW)   // LIGHT button
    {
      backlight();
    }

    // FAST REFRESH STATUS DISPLAY ITEMS

    // Print dial frequency
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

    // Print current mode
    display.setCursor(27, 22);
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    display.print(mode);

    //// Print S-meter
    //  tft.setCursor(50,120);
    //  int sMeterWidth = 5*(sMeter / 16);
    //  tft.drawRect(51,119,76,8,WHITE);
    //  tft.fillRect(52,120,sMeterWidth,6,WHITE);
    //  tft.fillRect((52+sMeterWidth),120,(74-sMeterWidth),6,BLACK);

    delay(200);

    // write to display
    display.display();

  } // END OF FAST REFRESH LOOP (performed 4 times in every main loop)
  
// SLOW REFRESH STUFF

// Update the ABC keys
displayABCkeys();

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

display.display();  // update display

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
    sw7status = LOW;
  }

  sw8status = digitalRead(sw8pin);
  sw9status = digitalRead(sw9pin);
}


// Cycle through soft-key pages
void changePage()
{
  currentPage = ++currentPage % 2;  // 2 pages
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
    // write to display
    display.display();
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
}


// Very rough backlight function (blocking)     g7uhn TO DO: make a better (non-blocking) backlight
void backlight()
{
  digitalWrite(backlightPin, 1);
  delay(5000);                  // Backlight on for 5s
  digitalWrite(backlightPin, 0);
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




void displayABCkeys() 
{
  byte currentAbcKeys = radio.getDisplaySelection();

  if (currentAbcKeys != abcKeys)
  {
    abcKeys = currentAbcKeys;
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