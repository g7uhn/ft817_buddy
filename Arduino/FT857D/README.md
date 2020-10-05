This is the modified FT857D library used by the main FT817_buddy sketch.

This library began life as VE3BUX's FT857D library but was then hacked a bit by G7UHN to make it work with the FT817_buddy PCB.

The plan is to tidy this up and it may be retitled as an FT817 library, obviously with appropriate credit to VE3BUX.

It may be that this library remains limited to the FT817's CAT commands and FT817 EEPROM functions are taken care of in another library.  This is the preference because working with the FT817 EEPROM carries a higher risk than working with the CAT commands so separating the functions into distinct libraries highlights this to the eventual users e.g. CAT commands are 'safe', EEPROM commands are 'risky'.

On the other hand, it may prove too troublesome to have two separate libraries and the main sketch coordinate around a single serial port communicating with the radio so CAT and EEPROM functions may get combined into one library...
