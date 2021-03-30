#include "pcf2119.h"
#include <Wire.h>
#include "Arduino.h"
#include <stdio.h>

pcf2119::pcf2119(int8_t rst_pin)
{
	_rst_pin = rst_pin;
}

void pcf2119::init(bool i2c_int_pullup)
{	
	// // Setup IO-Pins
	// if(i2c_int_pullup)
	// {
		// pinMode(SDA,INPUT_PULLUP);
		// pinMode(SCL,INPUT_PULLUP);
	// }
	pinMode(_rst_pin, OUTPUT);
	digitalWrite(_rst_pin, HIGH); delay(10);
	digitalWrite(_rst_pin, LOW);  delay(10);
	
	// i2c setup
	Wire.begin();
	
	Wire.beginTransmission(0x3A);     // Slave Addr Byte (SA=0/RW=0)
	Wire.write(0x00);                 // CO=0, RS=0

	// Standard instructions
	//Wire.write(0x20 | 0b00000);       // Function_set   Standard instructions, ignore everything else as we will set this at the end
	setup_function_set(PCF2119_INSTRUCTION_SET_STANDARD);
	Wire.write(0x04 | 0b10);          // Entry Mode     Addr Increment, No Shift
	Wire.write(0x08 | 0b100);         // Display Ctl    disp On, cursor off, blinking off

	// Extended instructions
	//Wire.write(0x20 | 0b00001);       // Function_set   Extended instructions, ignore everything else as we will set this at the end
	setup_function_set(PCF2119_INSTRUCTION_SET_EXTENDED);
	Wire.write(0x04 | 0b00);          // Disp_conf      Column: Left to Right, Row: Top to Bottom
	Wire.write(0x40 | _vlcd_generator_stages);          // HV_gen         Stage 1 = 2x voltage multiplier (Stage 1 @ 142µA, Stage 2 @ 163µA, Stage 3 @ 198µA
	Wire.write(0x08 | 0b000);         // Icon_ctl       icon mode = char mode, icon blink = disabled, direct mode = off
	if(_multiplex_mode == PCF2119_MUX_1_9)
		Wire.write(0x80 | 0b0000000 | 45); // VLCD_set       VA, 45 = 5.42V	1:9 MUX, one charge pump
	else
		Wire.write(0x80 | 0b0000000 | 58); // VLCD_set       VA, 58 = 6.46V 1:18 MUX, one charge pump
	Wire.write(0x80 | 0b1000000 | 0);  // VLCD_set       VB, 0 = Disabled  

	// Switch back to Standard instructions
	//Wire.write(0x20 | 0b00010);       // Function_set   1x32, 1:9 Multiplex, standard instruction set
	setup_function_set(PCF2119_INSTRUCTION_SET_STANDARD);

	Wire.endTransmission();
}

void pcf2119::set_mux(uint8_t multiplex_mode)
{
	_multiplex_mode = multiplex_mode;
	setup_function_set(_instruction_set_control,_multiplex_mode,_number_of_display_lines);
}

void pcf2119::set_hv_pump(uint8_t vlcd_generator_stages)
{
	_vlcd_generator_stages = vlcd_generator_stages;
}


void pcf2119::setup_function_set(
	 uint8_t instruction_set_control
	,uint8_t multiplex_mode
	,uint8_t number_of_display_lines
	)
{
	_instruction_set_control = instruction_set_control;
	_multiplex_mode = multiplex_mode;
	_number_of_display_lines = number_of_display_lines;
	uint8_t cmd = 0x20 | ((_instruction_set_control) |	(_multiplex_mode << 1) | (_number_of_display_lines << 2));
	#ifdef PCFG2119_DEBUG
	Serial.print(__func__); Serial.print("\t");
	Serial.print("cmd:\t"); Serial.println(cmd,HEX);
	#endif
	Wire.write(cmd);
}

void pcf2119::return_home(uint8_t type)
{
	// Switch to DDRAM
    Wire.beginTransmission(0x3A);     // Slave Addr Byte (SA=0/RW=0)
    Wire.write(0x00);                 // Command mode
	if(type == PCF2119_RAM_TYPE_DDRAM)
	{
		Wire.write(0x80);               // Set_DDRAM to 0x00
	} else {
		Wire.write(0x40);               // Set_CGRAM to 0x00
	}
    Wire.endTransmission();
	
	// Return cursor home
	uint8_t cmd = 0x02;
	#ifdef PCFG2119_DEBUG
	Serial.print(__func__); Serial.print("\t");
	Serial.print("cmd:\t"); Serial.println(cmd,HEX);
	#endif
	Wire.write(cmd);
	
	pcf_position = 0;
}

void pcf2119::read_ram(uint8_t type)
{
	uint8_t count = 0;
//// Reset DDRAM Addresse so we can read from the beginning    
// Set_DDRAM (Address)
    Wire.beginTransmission(0x3A);     // Slave Addr Byte (SA=0/RW=0)
    Wire.write(0x00);                 // Command mod
	if(type == PCF2119_RAM_TYPE_DDRAM)
	{
		Wire.write(0x80);               // Set_DDRAM to 0x00
	} else {
		Wire.write(0x40);               // Set_CGRAM to 0x00
	}
    Wire.endTransmission();  
// Select Data register & begin reading (still DDRAM)
    Wire.beginTransmission(0x3A);
    Wire.write(0x40);                 // 0x40 = last control byte, data register selected
    Wire.endTransmission();

	if(type == PCF2119_RAM_TYPE_DDRAM) {
	#ifdef PCFG2119_DEBUG
		Serial.print("DDRAM: "); Serial.print("\t");
	#endif
	} else {
	#ifdef PCFG2119_DEBUG
		Serial.print("CGRAM: "); Serial.print("\t");
	#endif
	}
    // Read 32bytes (maximum for wire library
    Wire.requestFrom(0x3A, 32);
    while(Wire.available())
    {
      //Serial.print(count++);
      uint8_t c = Wire.read();    // receive a byte as character
	  #ifdef PCFG2119_DEBUG
      Serial.write(c);         // print the character
	  #endif
      count++;
    }
	#ifdef PCFG2119_DEBUG
    Serial.print("#");
    Serial.print(count);
    Serial.println("#");
    Serial.flush();
	#endif
}

void pcf2119::clear_ram(uint8_t type)
{
//// Overwrite all CGRAM content (disable icons)
// Set_CGRAM (Address)
    Wire.beginTransmission(0x3A);     // Slave Addr Byte (SA=0/RW=0)
    Wire.write(0x00);                 // Command mode
	if(type == PCF2119_RAM_TYPE_DDRAM)
	{
		Wire.write(0x80);               // Set_DDRAM to 0x00
	} else {
		Wire.write(0x40);               // Set_CGRAM to 0x00
	}
    Wire.endTransmission();
// Fill CGRAM
    Wire.beginTransmission(0x3A);
    Wire.write(0x40);   // 0x40 = last control byte, data register selected
    for(int i = 0; i < 64; i++)
    {
      Wire.write(0x00);
    }
    Wire.endTransmission();
}

void pcf2119::clear_screen()
{
//// Clear whole screen
// Clear screen
    Wire.beginTransmission(0x3A);
    Wire.write(0x00);
    Wire.write(0x01);
    Wire.endTransmission();
	pcf_position = 0;
}

void pcf2119::printf(const char * format,...)
{
    va_list args;
    va_start(args, format);
	
	char buffer[32];
    uint8_t buffer_length = vsnprintf(buffer,32,format,args);
    va_end(args);
	
	Wire.beginTransmission(0x3A);
	Wire.write(0x40);   // 0x40 = last control byte, data register selected
	int c = 0;
	while(buffer[c] != 0x00)
	{
		if(pcf_position == 13 || pcf_position == 29 || (pcf_position >= 15 && pcf_position <= 25))
		{
			Wire.write(' ');
		} else {
			Wire.write(buffer[c]);
			c++;
		}
		pcf_position++;
	}
	Wire.endTransmission();
}