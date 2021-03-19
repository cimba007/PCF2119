#include "Arduino.h"

#ifndef pcf2119_h
#define pcf2119_h

//#define PCFG2119_DEBUG 1

// Function_set
#define PCF2119_NUM_LINES_1x32	0
#define PCF2119_NUM_LINES_2x16	1

#define PCF2119_MUX_1_18	(uint8_t)0
#define PCF2119_MUX_1_9		(uint8_t)1

#define PCF2119_INSTRUCTION_SET_STANDARD	0
#define PCF2119_INSTRUCTION_SET_EXTENDED	1

#define PCF2119_VLCD_GENERATOR_STAGES_1 	0
#define PCF2119_VLCD_GENERATOR_STAGES_2 	1
#define PCF2119_VLCD_GENERATOR_STAGES_3 	2

// ...
#define PCF2119_RAM_TYPE_DDRAM	0
#define PCF2119_RAM_TYPE_CGRAM	1

// the #include statment and code go here...
class pcf2119
{
	public:
		pcf2119(int8_t rst_pin);
		
		void init(bool i2c_int_pullup = false);
		void setup_function_set(
			uint8_t instruction_set_control = 	PCF2119_INSTRUCTION_SET_STANDARD
			,uint8_t multiplex_mode = 			PCF2119_MUX_1_18
			,uint8_t number_of_display_lines = 	PCF2119_NUM_LINES_1x32
			);
		void clear_screen();
		void set_mux(uint8_t multiplex_mode);
		void set_hv_pump(uint8_t vlcd_generator_stages);
		void return_home(uint8_t type = PCF2119_RAM_TYPE_DDRAM);
		void read_ram(uint8_t type = PCF2119_RAM_TYPE_DDRAM);
		void clear_ram(uint8_t type = PCF2119_RAM_TYPE_DDRAM);
		void printf(const char * format,...);
	private:
		int8_t _rst_pin;
		
		uint8_t _multiplex_mode;
		uint8_t _number_of_display_lines;
		uint8_t _instruction_set_control;
		uint8_t _vlcd_generator_stages = PCF2119_VLCD_GENERATOR_STAGES_1;
};

#endif