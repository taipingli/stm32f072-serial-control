#include "mbed.h"
#include "MCP23008.hpp"

const uint64_t LONG_DATA_MAX = (1ULL << 52) - 1; 
Serial pc(PB_6, PB_7);

DigitalOut LED_RED(PA_15); 

struct gpio_data {
	uint8_t command; 
	uint8_t pin; 
	uint8_t reserved; 
	uint8_t state; 
}; 

// Data for I2C Data
// 40 bits in total 
// command: 4 bits, command code
// pin_scl: 5 bits, pin number
// pin_sda: 5 bits, pin number
// reserved: 2 bits, currently un-used
// address: 8 bits, i2c address (mbed uses 8 bit address, left shift 7 bit addr by 1)
// data: 16 bits of data 
struct i2c_data {
	uint8_t command;
	uint8_t pin_scl; 
	uint8_t pin_sda; 
	uint8_t reserved; 
	uint8_t address; 
	uint16_t data; 
};


//Generic Data
// 52 bits
// command: 4 bits
// pins: 6 pins x 5 bits = 30 bits
// reserved: 2 bits
// data: 16 bits 
struct long_data {
	uint8_t command; 
	uint8_t pin_1; 
	uint8_t pin_2; 
	uint8_t pin_3;
	uint8_t pin_4; 
	uint8_t pin_5; 
	uint8_t pin_6; 
	uint8_t reserved; 
	uint16_t data; 
}; 

// Define all pins on the STM32F072CBT6 48QFP 
const int GPIO_PIN_COUNT = 31; 
const PinName GPIO[] = {
    PA_0,  PA_1,  PA_2,  PA_3,
    PA_4,  PA_5,  PA_6,  PA_7,
    PA_8,  PA_9,  PA_10, PA_15,
    PB_0,  PB_1,  PB_2,  PB_3,
    PB_4,  PB_5,  PB_6,  PB_7,
    PB_9,  PB_10, PB_11, PB_12,
    PB_13, PB_14, PB_15, PC_13,
    ADC_TEMP, ADC_VREF, ADC_VBAT
    };
    
void set_gpo(int pin, int state) {

    DigitalOut gpo_pin(GPIO[pin]);

    if (state) {
         gpo_pin = 1;
    }
    
    else {
        gpo_pin = 0;
    }
    
    return; 

}

void read_gpi(int pin) {

    DigitalIn gpi_pin(GPIO[pin]);
    pc.printf("%i\n", gpi_pin.read()); 
}
     
void read_adc(int pin) {

    AnalogIn adc_pin(GPIO[pin]);
    pc.printf("%f\n", 2.99f*adc_pin.read()); 
} 

void MCP23008_set_output (int scl_pin, int sda_pin, uint8_t address, int output) {
   // pc.printf("Setting PE: %i, %i, %i, %i\n", scl_pin, sda_pin, address, output); 
    //MCP23008 i2c_pe = MCP23008(PB_11, PB_10, 0); 
   // MCP23008 i2c_pe = MCP23008(GPIO[sda_pin], GPIO[scl_pin], 0); 
   // pc.printf("end of contstructor"); 
   // i2c_pe.set_output_pins(output); 
   // i2c_pe.write_outputs(output); 
   // pc.printf("End of setoutput\n"); 
    return; 
}

gpio_data gpio_unpack_data(uint16_t data) {
	gpio_data result; 

	int pin_mask = 0xF8; 

	result.command = data >> 8; 
	result.pin = (data & pin_mask) >> 3; 
	result.state = data & 0x1; 

	return result; 
}

i2c_data i2c_unpack_data(uint64_t data) {
	i2c_data result;
	uint64_t command_scl_pin_data = data >> 31; 
	uint64_t command_mask = 0xFUL << 5; 
	uint64_t pin_scl_mask = 0x1FUL;

	uint64_t pin_sda_mask = 0x1FUL << 26; 
	uint64_t address_mask = 0xFFUL << 16; 
	uint64_t data_mask = 0xFFFF; 

	result.command = (command_mask & command_scl_pin_data) >> 5; 
	result.pin_scl = (pin_scl_mask & command_scl_pin_data); 
	result.pin_sda = (pin_sda_mask & data) >> 26;
	result.address = (address_mask & data) >> 16;
	result.data = data_mask & data; 

	return result; 
}

// 52 bits
// command: 4 bits (51..49)
// pins: 6 pins x 5 bits = 30 bits (47..18)
// pin 1: 5 bits (47..43)
// pin 2: 5 bits (42..38)
// pin 3: 5 bits (37..33)
// pin 4: 5 bits(32..28)
// pin 5 5 bits (27..23)
// pin 6: 5 bits (22..18)
// reserved: 2 bits (17..16)
// data: 16 bits (15..0)

// Example: Command 5, Pin 1: 5, Pin 2: 10, Pin 3 15, Pin 4: 20, Pin 5: 25, Pin 6: 30, Data: 0xADED
// Encoded: 0101 00101 01010 01111 10100 1100|1 111|10 | 00 | 1010 1101 1110 1101 
// Hex: 52A9F4CF8ADED
// Dec: 1454238563085805

long_data long_unpack_data(uint64_t data) {
	long_data result; 
	// Cast masks to unsigned long long if its shifted more than 32 bits 
	uint64_t command_mask = 0xFULL << 48; 
	uint64_t pin_1_mask = 0x1FULL << 43;
	uint64_t pin_2_mask = 0x1FULL << 38;
	uint64_t pin_3_mask = 0x1FULL << 33; 
	uint64_t pin_4_mask = 0x1FULL << 28;
	uint64_t pin_5_mask = 0x1FUL << 23; 
	uint64_t pin_6_mask = 0x1FUL << 18; 
	uint64_t data_mask = 0xFFFF; 

	result.command = (command_mask & data) >> 48; 
	result.pin_1 = (pin_1_mask & data) >> 43; 
	result.pin_2 = (pin_2_mask & data) >> 38; 
	result.pin_3 = (pin_3_mask & data) >> 33; 
	result.pin_4 = (pin_4_mask & data) >> 28; 
	result.pin_5 = (pin_5_mask & data) >> 23; 
	result.pin_6 = (pin_6_mask & data) >> 18; 	
	result.data = (data_mask & data); 

	return result; 
}



void gpio(gpio_data data) {
	switch(data.command) {
		case 0x00:
			set_gpo(data.pin, data.state);
			break; 
		case 0x01:
			read_gpi(data.pin);
			break;  
		case 0x02:
			read_adc(data.pin); 
			break;
		default:
			return; 
	}
}

void data_rx(long_data data) {
	int pin_state = data.data & 0x1; 

	switch(data.command) {
		case 0x00:
			set_gpo(data.pin_1, pin_state);
			break; 
		case 0x01:
			read_gpi(data.pin_1); 
			break; 
		case 0x02: 
			read_adc(data.pin_1); 
			break; 
		default:
			return; 
	}

	return; 
}


int main() {        
    uint64_t data = 0; 
    gpio_data gpio_serial_data; 
    i2c_data i2c_serial_data; 
    long_data long_serial_data; 
    
    while(1) {
    	if (pc.readable()) {
    		pc.scanf("%llu", &data); 

    		if (data > 4095) {
				long_serial_data = long_unpack_data(data); 
    			//pc.printf("%i, %i, %i, %i, %i, %i, %i, %i\n", long_serial_data.command, long_serial_data.pin_1, long_serial_data.pin_2, long_serial_data.pin_3, long_serial_data.pin_4, long_serial_data.pin_5, long_serial_data.pin_6, long_serial_data.data);
    			data_rx(long_serial_data); 
    		}
/*
    		else if( (data > 4096) && (data < 2^40) ){
    			pc.printf("I2c Data\n"); 
				i2c_serial_data = i2c_unpack_data(data); 
    			pc.printf("Command %i, Pin SCL: %i, Pin SDA %i, Address %i, Data %i\n", i2c_serial_data.command, i2c_serial_data.pin_scl, i2c_serial_data.pin_sda, i2c_serial_data.address, i2c_serial_data.data); 
    			
    		}
*/
    		else {
    			gpio_serial_data = gpio_unpack_data(data);
    			//pc.printf("%i, %i, %i\n", gpio_serial_data.command, gpio_serial_data.pin, gpio_serial_data.state); 
    			gpio(gpio_serial_data); 
    			}

    		}
    	}

    } 
        
                    
    