/*
  Have used the FT-857D CAT library from https://ve3bux.com/?page_id=501#usage but, after installing library to
  Arduino IDE, have modified FT-857D.h to bring the following commands up to public to enable some direct access
  from this sketch:
  // moved from private to make directly available to main sketch...
  void sendByte(byte cmd);
  void sendCmd(byte cmd[], byte len);
  byte singleCmd(int cmd);    // simplifies small cmds
  byte getByte();
  ...and then comment out those lines under "private:"
*/

int buttonPin = A0;
int sw7 = 5;         // SW7 input is D5
int sw8 = 10;        // SW8 input is D10
int sw9 = 9;         // SW9 input is D9

#include <SPI.h>
#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>
//#include <Wire.h>
#include <SoftwareSerial.h>
#include <FT857D.h>

// Declare LCD object for software SPI
// Adafruit_PCD8544(CLK,DIN,D/C,CE,RST);
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

// FT-857D CAT library stuff...
FT857D radio;           // define “radio” so that we may pass CAT commands

int backlight = 8;   // backlight output switch pin (not the "LIGHT" input button!)
int sw8status = 1;
int i;
boolean dataValid = true;
long freq;
String mode;
byte modeByte;
byte modeBase;    // used for the radio to return to this mode after generating Tune signal... needed to accomodate the CW/CWR/DIG NAR codes
byte modeReturn;
int buttonStatus = 0;
int button = 0;
boolean splitState = false;
byte sMeter;
byte targetByte;
byte byteAbove;
byte targetByteReceived[3];
byte byteAboveReceived[3];
byte command[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
byte currentBand;
int currentVfo;
//boolean currentNar;
//boolean requestedNar;
byte MSB;
byte LSB;




void setup(void) {
  Serial.begin(9600);
  radio.begin(9600);     // start the radio serial port (using an Arduino Nano Every now so this is hardware serial, "Serial1")

  //Set up some pins
  pinMode(backlight, OUTPUT);
  pinMode(sw7, INPUT_PULLUP);
  pinMode(sw8, INPUT_PULLUP);
  pinMode(sw9, INPUT_PULLUP);

  //Initialize Display
  display.begin();

  // you can change the contrast around to adapt the display for the best viewing!
  display.setContrast(57);

  // Clear the buffer.
  display.clearDisplay();

  // Set up the timer interrupt to watch for button status at 50 Hz (needed to catch a quick button press)
  cli();//stop interrupts

  //set timer1 interrupt at 50Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 311;// = (16*10^6) / (1*1024*50) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts

  // Read FT-817 soft-keys (A,B,C)
  readEEPROM(0x00, 0x76);  // EEPROM byte 76 indicates the A,B,C functions
  if (dataValid == true) {
    byte mask = B00001111;
    byte abcKeys = mask & targetByte;
    // Print current soft-keys (A B C)
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


  // Draw some lines
  //  display.drawFastHLine(0, 96, tft.width()-1, RED);
  display.drawFastVLine(40, 0, 10, BLACK);
  display.drawFastVLine(21, 10, 21, BLACK);
  display.drawFastVLine(62, 10, 21, BLACK);
  display.drawFastHLine(0, 9, display.width(), BLACK);
  display.drawFastHLine(0, 20, 21, BLACK);
  display.drawFastHLine(62, 20, 21, BLACK);
  display.drawFastHLine(0, 30, 21, BLACK);
  display.drawFastHLine(62, 30, 21, BLACK);

  // Label the soft keys
  // may update this when I implement multiple pages of soft-keys...
  //Button 1
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("TUNon");
  //Button 2
  display.setCursor(0, 11);
  display.setTextSize(1);
  display.print("IPO");
  //Button 3
  display.setCursor(0, 22);
  display.setTextSize(1);
  display.print("KYR");
  //Button 4
  display.setCursor(48, 0);
  display.setTextSize(1);
  display.print("TUNoff");
  //Button 5
  display.setCursor(66, 11);
  display.setTextSize(1);
  display.print(" BK");
  //Button 6
  display.setCursor(66, 22);
  display.setTextSize(1);
  display.print("NAR");



  display.display();

} // end setup


void loop() {

//  digitalWrite(backlight, 1);
//  delay(10000);
//  digitalWrite(backlight, 0);
//  delay(10000);
//  digitalWrite(backlight, 1);
//  delay(10000);
//  digitalWrite(backlight, 0);


  // Get frequency
  freq = radio.getFreqMode(); //freq is a long integer of 10's of Hz
  int MHz = freq / 100000;
  long remainder = freq % 100000;
  float kHz = remainder / 100.00;
  delay(50);

  // Get mode data, make it readable
  getReadableMode();
  delay(50);

  // Get Receiver status (S-meter)
  byte rxStatusCmd[5] = {0x00, 0x00, 0x00, 0x00, 0xe7};
  radio.sendCmd(rxStatusCmd, 5);
  sMeter = radio.getByte();
  sMeter = sMeter << 4; //left shift the bits by 4 to make this an S-meter byte ranging from 00000000 to 11110000

  delay(150);

  // BUTTON ACTION ITEMS
  //set up cursor for printing button status...
  display.setCursor(30, 0);
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);

  if (button == 0) {
  }
  else if (button == 1) {
    tuneSignalOn();
  }
  else if (button == 2) {
    toggleIpo();
  }
  else if (button == 3) {
    toggleKeyer();
  }
  else if (button == 4) {
    tuneSignalOff();
  }
  else if (button == 5) {
    toggleBreakIn();
  }
  else if (button == 6) {
    toggleNar();
  }
  button = 0;  // reset button variable to zero once we've used it

  // STATUS DISPLAY ITEMS
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
  display.setCursor(25, 22);
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.print(mode);

  //// Print S-meter
  //  tft.setCursor(50,120);
  //  int sMeterWidth = 5*(sMeter / 16);
  //  tft.drawRect(51,119,76,8,WHITE);
  //  tft.fillRect(52,120,sMeterWidth,6,WHITE);
  //  tft.fillRect((52+sMeterWidth),120,(74-sMeterWidth),6,BLACK);

  display.display(); // update the display

  // Rough backlight function (blocking)
  sw8status = digitalRead(sw8);
  if (sw8status == LOW){
    digitalWrite(backlight, 1);
    delay(5000);                  // Backlight on for 5s
    digitalWrite(backlight, 0);
  }
}  // end main loop



void tuneSignalOn() {
  // USER FUNCTION: Tune signal ON (AM = 1/4 power carrier)
  // Check SHIFT key is also held down (safety mechanism to avoid accidental keying of radio by knocking the TUN ON button)
  int sw9status = digitalRead(sw9);
  if (sw9status == LOW){
    // Store current operating base mode i.e. ignoring the NAR mode codes
    modeReturn = modeBase;
    // Set mode to AM
    byte rigMode[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    rigMode[0] = CAT_MODE_AM; //  mode
    rigMode[4] = CAT_MODE_SET; // command byte (set mode)
    radio.sendCmd(rigMode, 5);
    radio.getByte();
    delay(200);
    // Set PTT to ON
    radio.PTT(true);
  }
  // END OF USER FUNCTION
}

void toggleVfo() {
  // USER FUNCTION: Switch VFO A/B
  radio.switchVFO();
  // END OF USER FUNCTION
}

void toggleSplit() {
  // USER FUNCTION: Split operation (toggle)
  if (splitState == false) {
    radio.split(true);  // set SPL to true
    splitState = true;  // set splitState to true
  }
  else {
    radio.split(false);  // set SPL to false
    splitState = false;  // set splitState to false
  }
  // END OF USER FUNCTION
}

void tuneSignalOff() {
  // USER FUNCTION: Tune signal OFF
  // Set PTT to OFF
  radio.PTT(false);
  delay(200);
  radio.PTT(false);
  delay(200);
  // Set mode to <retrieve from modeReturn>
  byte rigMode[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
  rigMode[0] = modeReturn; //  mode
  rigMode[4] = CAT_MODE_SET; // command byte (set mode)
  radio.sendCmd(rigMode, 5);
  radio.getByte();
  // END OF USER FUNCTION
}


void toggleNar() {
  // USER FUCNTION:  Toggle NAR filter status on current VFO
 boolean currentNar;
 boolean requestedNar;
  // First, read current VFO A or B status
  readEEPROM(0x00, 0x55);  // EEPROM byte 55 contains VFO A/B status on bit 0 (0=VFO A, 1=VFO B)
  // ...if data is valid, store the current VFO
  if (dataValid == true) {
    currentVfo = bitRead(targetByte, 0);
//    Serial.print("VFO is ");
//    Serial.println(currentVfo);
    // Next, read current band status
    readEEPROM(0x00, 0x59);  // EEPROM byte 59 contains band selection for both VFOs
    // ...if data is valid, store the current band for the selected VFO
    if (dataValid == true) {
      if (currentVfo == 0) {        // VFOA band is the rightmost 4 bits
        byte mask = B00001111;
        currentBand = mask & targetByte;
//        Serial.println("VFO is A");
      }
      else if (currentVfo == 1) {   // VFOB band is the leftmost 4 bits
        currentBand = targetByte >> 4;
//        Serial.println("VFO is B");
      }
//      Serial.print("Current band is ");
//      Serial.println(currentBand, BIN);
      // Determine the base target address of the current VFO/band memory slot on the EEPROM
      int baseTarget = 0x7D + (currentVfo * 390) + (currentBand * 26);
//      Serial.print("Base target hex address is ");
//      Serial.println(baseTarget, HEX);
      // Determine target byte containing the bit we want... NAR is bit 4 in byte (base address + 1)
      int target = baseTarget + 1;   // + 1 for NAR

      // Determine target MSB and LSB for passing to the readEEPROM function
      if (target < 256) {
        MSB = 0x00;
        LSB = target;
      }
      else {
        MSB = highByte(target);
        LSB = lowByte(target);
      }
//      Serial.print("Actual target hex address is ");
//      Serial.println(target, HEX);
//      Serial.print("MSB and LSB for readEEPROM are ");
//      Serial.print(MSB, HEX);
//      Serial.print(",");
//      Serial.println(LSB, HEX);
      // Finally, retreive the NAR status bit for the current VFO/band memory slot
      readEEPROM(MSB, LSB);
      // ...if data is valid, store the current NAR status
      if (dataValid == true) {
        currentNar = bitRead(targetByte, 4);
//        Serial.print("NAR status is ");
//        Serial.println(currentNar);
      }
    }
  }
  // Now that we've read the data from the EEPROM, let's do something with it...
    switch (dataValid) {
    case true:   // Do the good stuff we want to happen...
      radio.switchVFO();   // switch over to the other VFO before we mess with the current one (KA7OEI advice is not to write to the active VFO memory slot)
      delay(200);
      requestedNar = !currentNar;   // request the opposite of our current NAR state
      targetByte = bitWrite(targetByte, 4, requestedNar);   // set the requested state in the target byte
//      Serial.print("New target byte will be ");
//      Serial.println(targetByte, BIN);
//      Serial.print("Write EEPROM message will be (");
//      Serial.print(MSB, HEX);
//      Serial.print(",");
//      Serial.print(LSB, HEX);
//      Serial.print(",");
//      Serial.print(targetByte, BIN);
//      Serial.print(",");
//      Serial.print(byteAbove, BIN);
//      Serial.println(")");
      writeEEPROM(MSB, LSB, targetByte, byteAbove);
      delay(200);
      radio.switchVFO();   // switch VFO back to where we were before we wrote to the EEPROM
      delay(100);
      break;
    case false:   // Escape! ...and let the user know why it didn't work!
      // Print some debug data on the screen
      display.setCursor(0, 41);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print(" read mismatch!!");
      display.display();
      delay(5000);
      break;
    }
  // END OF USER FUNCTION
}


void toggleIpo() {
  // USER FUCNTION:  Toggle IPO status on current VFO
 boolean currentIpo;
 boolean requestedIpo;
  // First, read current VFO A or B status
  readEEPROM(0x00, 0x55);  // EEPROM byte 55 contains VFO A/B status on bit 0 (0=VFO A, 1=VFO B)
  // ...if data is valid, store the current VFO
  if (dataValid == true) {
    currentVfo = bitRead(targetByte, 0);
//    Serial.print("VFO is ");
//    Serial.println(currentVfo);
    // Next, read current band status
    readEEPROM(0x00, 0x59);  // EEPROM byte 59 contains band selection for both VFOs
    // ...if data is valid, store the current band for the selected VFO
    if (dataValid == true) {
      if (currentVfo == 0) {        // VFOA band is the rightmost 4 bits
        byte mask = B00001111;
        currentBand = mask & targetByte;
//        Serial.println("VFO is A");
      }
      else if (currentVfo == 1) {   // VFOB band is the leftmost 4 bits
        currentBand = targetByte >> 4;
//        Serial.println("VFO is B");
      }
//      Serial.print("Current band is ");
//      Serial.println(currentBand, BIN);
      // Determine the base target address of the current VFO/band memory slot on the EEPROM
      int baseTarget = 0x7D + (currentVfo * 390) + (currentBand * 26);
//      Serial.print("Base target hex address is ");
//      Serial.println(baseTarget, HEX);
      // Determine target byte containing the bit we want... IPO is bit 5 in byte (base address + 2)
      int target = baseTarget + 2;   // + 2 for IPO

      // Determine target MSB and LSB for passing to the readEEPROM function
      if (target < 256) {
        MSB = 0x00;
        LSB = target;
      }
      else {
        MSB = highByte(target);
        LSB = lowByte(target);
      }
//      Serial.print("Actual target hex address is ");
//      Serial.println(target, HEX);
//      Serial.print("MSB and LSB for readEEPROM are ");
//      Serial.print(MSB, HEX);
//      Serial.print(",");
//      Serial.println(LSB, HEX);
      // Finally, retreive the IPO status bit for the current VFO/band memory slot
      readEEPROM(MSB, LSB);
      // ...if data is valid, store the current IPO status
      if (dataValid == true) {
        currentIpo = bitRead(targetByte, 5);
//        Serial.print("IPO status is ");
//        Serial.println(currentNar);
      }
    }
  }
  // Now that we've read the data from the EEPROM, let's do something with it...
    switch (dataValid) {
    case true:   // Do the good stuff we want to happen...
      radio.switchVFO();   // switch over to the other VFO before we mess with the current one (KA7OEI advice is not to write to the active VFO memory slot)
      delay(200);
      requestedIpo = !currentIpo;   // request the opposite of our current IPO state
      targetByte = bitWrite(targetByte, 5, requestedIpo);   // set the requested state in the target byte
//      Serial.print("New target byte will be ");
//      Serial.println(targetByte, BIN);
//      Serial.print("Write EEPROM message will be (");
//      Serial.print(MSB, HEX);
//      Serial.print(",");
//      Serial.print(LSB, HEX);
//      Serial.print(",");
//      Serial.print(targetByte, BIN);
//      Serial.print(",");
//      Serial.print(byteAbove, BIN);
//      Serial.println(")");
      writeEEPROM(MSB, LSB, targetByte, byteAbove);
      delay(200);
      radio.switchVFO();   // switch VFO back to where we were before we wrote to the EEPROM
      delay(100);
      break;
    case false:   // Escape! ...and let the user know why it didn't work!
      // Print some debug data on the screen
      display.setCursor(0, 41);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print(" read mismatch!!");
      display.display();
      delay(5000);
      break;
    }
  // END OF USER FUNCTION
}


void toggleBreakIn() {
  // USER FUCNTION:  Toggle BreakIn status
  boolean currentBreakIn;
  boolean requestedBreakIn;
  MSB = 0x00;
  LSB = 0x58;    // BreakIn is set by bit 5 of EEPROM byte 58
      // Retreive the BK status bit (global)
      readEEPROM(MSB, LSB);
      // ...if data is valid, store the current BK status
      if (dataValid == true) {
        currentBreakIn = bitRead(targetByte, 5);
        Serial.print("BK status is ");
        Serial.println(currentBreakIn);
      }
  // Now that we've read the data from the EEPROM, let's do something with it...
    switch (dataValid) {
    case true:   // Do the good stuff we want to happen...
      requestedBreakIn = !currentBreakIn;   // request the opposite of our current BK state
      targetByte = bitWrite(targetByte, 5, requestedBreakIn);   // set the requested state in the target byte
//      Serial.print("New target byte will be ");          // some debug text for checking
//      Serial.println(targetByte, BIN);
//      Serial.print("Write EEPROM message will be (");    // some debug text for checking
//      Serial.print(MSB, HEX);
//      Serial.print(",");
//      Serial.print(LSB, HEX);
//      Serial.print(",");
//      Serial.print(targetByte, BIN);
//      Serial.print(",");
//      Serial.print(byteAbove, BIN);
//      Serial.println(")");
      writeEEPROM(MSB, LSB, targetByte, byteAbove);
      delay(100);
      break;
    case false:   // Escape! ...and let the user know why it didn't work!
      // Print some debug data on the screen
      display.setCursor(0, 41);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print(" read mismatch!!");
      display.display();
      delay(5000);
      break;
    }
  // END OF USER FUNCTION
}


void toggleKeyer() {
  // USER FUCNTION:  Toggle Keyer status
  boolean currentKeyer;
  boolean requestedKeyer;
  MSB = 0x00;
  LSB = 0x58;    // Keyer is set by bit 4 of EEPROM byte 58
      // Retreive the BK status bit (global)
      readEEPROM(MSB, LSB);
      // ...if data is valid, store the current KYR status
      if (dataValid == true) {
        currentKeyer = bitRead(targetByte, 4);
        Serial.print("Keyer status is ");
        Serial.println(currentKeyer);
      }
  // Now that we've read the data from the EEPROM, let's do something with it...
    switch (dataValid) {
    case true:   // Do the good stuff we want to happen...
      requestedKeyer = !currentKeyer;   // request the opposite of our current KYR state
      targetByte = bitWrite(targetByte, 4, requestedKeyer);   // set the requested state in the target byte
//      Serial.print("New target byte will be ");          // some debug text for checking
//      Serial.println(targetByte, BIN);
//      Serial.print("Write EEPROM message will be (");    // some debug text for checking
//      Serial.print(MSB, HEX);
//      Serial.print(",");
//      Serial.print(LSB, HEX);
//      Serial.print(",");
//      Serial.print(targetByte, BIN);
//      Serial.print(",");
//      Serial.print(byteAbove, BIN);
//      Serial.println(")");
      writeEEPROM(MSB, LSB, targetByte, byteAbove);
      delay(100);
      break;
    case false:   // Escape! ...and let the user know why it didn't work!
      // Print some debug data on the screen
      display.setCursor(0, 41);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.print(" read mismatch!!");
      display.display();
      delay(5000);
      break;
    }
  // END OF USER FUNCTION
}


// Interrupt code
ISR(TIMER1_COMPA_vect) { //change the 0 to 1 for timer1 and 2 for timer2
  // Get button status
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
}


// Read from EEPROM function
// NOTE: after checking the received data bytes match over a few reads, this function updates the global
// variables targetByte and byteAbove with the two bytes that have been received from the EEPROM.
// Because we're using global variables (and they will be modifed by subsequent readEEPROM commands) we
// need to take what data we need from targetByte and byteAbove immediately after calling the readEEPROM
// function.
void readEEPROM(byte targetHexMSB, byte targetHexLSB) {

  dataValid = true;   // reset dataValid flag to true (errors will switch it to false and escape the function)

  // Build command to read EEPROM
  memset(command, 0, 5); // zeroise the command array
  command[0] = targetHexMSB; //  MSB EEPROM data byte
  command[1] = targetHexLSB; //  LSB EEPROM data byte
  command[4] = 0xBB; // BB command byte (read EEPROM data)
  // Send the command and read the returned data three times for safety
  for (i = 0; i < 3; i++) {
    radio.sendCmd(command, 5);
    Serial.println("Read EEPROM command sent");
    // Read the two bytes that are returned from the EEPROM
    targetByteReceived[i] = radio.getByte();
    byteAboveReceived[i] = radio.getByte();
    Serial.print("targetByte received = ");
    Serial.println(targetByteReceived[i], BIN);
    Serial.print("byteAbove received = ");
    Serial.println(byteAboveReceived[i], BIN);
    delay(50);
  }
  // Check the received data is all good
  for (i = 1; i < 3; i++) {
    if (targetByteReceived[0] != targetByteReceived[i]) dataValid = false;
    if (byteAboveReceived[0] != byteAboveReceived[i]) dataValid = false;
  }
  // ...if data is valid, update the global variables (and we'll take what we need from these immediately after calling this readEEPROM function)
  if (dataValid == true) {
    targetByte = targetByteReceived[0];
    byteAbove = byteAboveReceived[0];
  }
}


// Write to EEPROM function
// Not sure how we would correct if there's a problem here...!  We already perform checks on data pulled from the EEPROM 
// but if a send is corrupted how would we fix?  Do another read to check and if not correct then perform it again?  Or 
// just warn the user that it didn't work?
void writeEEPROM(byte targetHexMSB, byte targetHexLSB, byte dataForTarget, byte dataForByteAbove) {

  // Build command to write to EEPROM
  memset(command, 0, 5);         //  zeroise the command array
  command[0] = targetHexMSB;     //  MSB EEPROM data byte
  command[1] = targetHexLSB;     //  LSB EEPROM data byte
  command[2] = dataForTarget;    //  data byte for the target address
  command[3] = dataForByteAbove; //  data byte for the byte above the target address
  command[4] = 0xBC;             //  BC command byte (write EEPROM data)
  
  radio.sendCmd(command, 5);
  Serial.println("Write EEPROM command and data sent");
  byte temp = radio.getByte();
  Serial.println(temp);

//    It's recommended to read the EEPROM location after writing to check it's OK... TO DO!

  delay(100);
  }



// TO DO: surely this function can be better performed by some kind of lookup table?
void getReadableMode() {
  modeByte = radio.getMode();
  if (modeByte == 0x00) {
    mode = "LSB   ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x01) {
    mode = "USB   ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x02) {
    mode = "CW    ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x03) {
    mode = "CWR   ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x82) {
    mode = "CW NR ";
    modeBase = 0x02;
  }
  else if (modeByte == 0x83) {
    mode = "CWR NR";
    modeBase = 0x03;
  }
  else if (modeByte == 0x8A) {
    mode = "DIG NR";
    modeBase = 0x0A;
  }
  else if (modeByte == 0x04) {
    mode = "AM    ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x06) {
    mode = "WFM   ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x08) {
    mode = "FM    ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x88) { // this one wasn't listed in the spec?
    mode = "FM-N  ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x0A) {
    mode = "DIG   ";
    modeBase = modeByte;
  }
  else if (modeByte == 0x0C) {
    mode = "PKT   ";
    modeBase = modeByte;
  }
  else {
    char mode[] = "??";
  }
}
