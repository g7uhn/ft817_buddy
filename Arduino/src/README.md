The main Arduino sketch for the FT-817 Buddy project.

This sketch has been built using Visual Studio Code / PlatformIO IDE (instead of Arduino IDE).  If you want to load this code into Arduino IDE you probably just need to remove the #include <Arduino.h> line at the top of the sketch, Google is your friend.  But it's much nicer to work in VSC/PlatformIO, it's a nice step forward from the Arduino IDE.

Don't forget to include the non-standard ft817 library, found under ft817_buddy/Arduino/lib/

### Current features

- Display current frequency and mode
- Display current A,B,C key functions
- 3 pages of 6 soft-keys
- Backlight
- Tune signal on/off (Transmit an AM carrier, suitable for driving auto-ATU)
- Toggle IPO
- Toggle NAR filter
- Toggle Break-In
- Toggle FT-817 internal Keyer
- Toggle RF Gain/ Squelch
- Toggle VFO A/B
- Set FT-817 internal keyer to preset wpm (12, 15, 18 in my case)
- External keyer: [K1EL K16 chip](https://hamcrafters2.com/files/k16man_R10.pdf) integration
  - Command and M1, M2, M3, M4 buttons
  - Onboard piezo buzzer and LED for K16 feedback
  - Control speed using CMD+paddle (K1EL's "Fast Speed Change" feature)

### Licence
This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License