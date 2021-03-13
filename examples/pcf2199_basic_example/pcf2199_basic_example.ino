#include "PCF2119.h"

// Based on the work of
// https://www.mikrocontroller.net/topic/434123
// https://www.pollin.de/p/lcd-modul-tian-ma-a2c00096100-121581

#define PinRST 3
pcf2119 display = pcf2119(PinRST);

void setup() {
  Serial.begin(9600);
  Serial.println("Setup");

  display.init();
  display.set_mux(PCF2119_MUX_1_18);
  display.clear_screen();
}

uint16_t loopcounter = 0;
void loop()
{   
    display.return_home();
    
    // in 1:9MUX 10chars are safe, more create ghosting on this particular display
    // in 1:18MUX all chars are safe
    display.printf("%08lums",millis());         // 10 Characters
	
    //display.printf("aabbccddeeffgghhiijjkk");   // 3+(halfish)+2+3+(missing)+2
    // Anything after character #13 is "work in progress"   
    delay(25);
}
