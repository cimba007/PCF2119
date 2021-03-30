#include "PCF2119.h"

// Based on the work of
// https://www.mikrocontroller.net/topic/434123
// https://www.pollin.de/p/lcd-modul-tian-ma-a2c00096100-121581

#define PinRST 3
pcf2119 display = pcf2119(PinRST);

void setup() {
  Serial.begin(9600);
  Serial.println("Setup");

  // -- Display Settings (must be called before init) --
  //display.set_hv_pump(PCF2119_VLCD_GENERATOR_STAGES_1);	// Sufficient for 5V VCC (default)
  //display.set_hv_pump(PCF2119_VLCD_GENERATOR_STAGES_2);	// Needed for 3.3V VCC
  display.set_mux(PCF2119_MUX_1_18);
  
  // -- Display Init --
  display.init();
  display.clear_screen();
}

void loop()
{   
	// Attention: This display has a strange memory layout so the "home" position is 5 characters from the left.
	// For more information see the advanced example
    display.return_home();
    
    // in 1:9MUX 10chars are safe, more create ghosting on this particular display
    // in 1:18MUX all chars are safe
    display.printf("%08lums",millis());         // 10 Characters
    
    delay(25);
}
