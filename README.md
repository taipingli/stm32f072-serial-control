#STM32F072 Serial Control (Test)
### Why?
When we build the last solar car, MSXII, there was minimal validation done on hardware other than powering it and hoping things don't catch on fire... This lack of validation was due to a mix of time and resources constraints, as you would typically need at least one hardware and one firmware team member working for a week or two to validate a board. 

The goal for this is to simplify any basic validation tasks such as reading a digital input, setting a digital output, and reading an ADC value from the microcontroller. By creating a simple to use Python API, the goal is that pretty much any hardware designer can utlize this during bring-up and validation. More advanced scripting can also be accomplished, an example of this would be to send SCPI commands to an electronic load to test the efficiency of a regulator or to validate the accuracy of voltage and current measurements using a DMM. 

### How to Use
First clone the scripts repo from Github(INSERT LINK). Then import gpio.py and create an instance of GPIO by doing the following: 

```Python
import gpio

controller_board = GPIO() 

```
### Data Format
To standardize the messages being sent to the microcontroller, the following format should be used in all cases. 

|  Command | Pin 1  | Pin 2  |  Pin 3 |  Pin 4 | Pin 5 | Pin 6 |Reserved| Data | 
|:---:     |---     |---     |---     |---     | --- | --- | :---: | --- | 
|51..48|47..43|42..38|37..33|32..28|27..23|22..18| 17..16| 15..0|

#### Command (4 bits)
0x0: Set GPO   
0x1: Read GPI   
0x2: Read ADC  
0x3: Send I2C Data  
0x4: Send SPI Data  
0x5: Set MCP23008 I2C Port Expander Outputs   
0x6: Read MCP23008 I2C Port Expander Inputs   
0x7: Read ADS1015 ADC   

#### Pin Definitions (6 pins x 5 bits/pin)
Each pin is defined as a number between 0-31. Each number is mapped according to the table below. Only 1 pin is used for setting GPO, reading GPI, and reading the ADC, but multiple may be used for I2C or SPI. Note that this is abstracted in the Python class using an enum. 

| Pin | Number | Pin | Number |
|:---: | :---: | :---:| :---:|
| PA 0 | 0 | PB 4 | 16|
| PA 1 | 1 | PB 5 | 17|
| PA 2 | 2 | PB 5 | 17|
| PA 3 | 3 | PB 6 | 18|
| PA 4 | 4 | PB 7 | 19|
| PA 5 | 5 | PB 9 | 20|
| PA 6 | 6 | PB 10 | 21|
| PA 7 | 7 | PB 11| 22|
| PA 8 | 8 | PB 12| 23|
| PA 9 | 9 | PB 13| 24|
| PA 10| 10| PB 14| 25|
| PA 15| 11| PB 15| 26|
| PB 0 | 12| PC 13| 27|
| PB 1 | 13| ADC_TEMP| 28|
| PB 2 | 14| ADC_VREF| 29|
| PB 3 | 15| ADC_VBAT| 30| 

##### Commands 0x0, 0x1, 0x2 (Microcontroller GPIOs)
Pin 1: Pin that the command is being used for  
Pin 2 - Pin 6: Not Used

##### Commands 0x3, 0x5, 0x6, 0x7 (I2C Devices) 
Pin 1: SCL   
Pin 2: SDA   

##### Commands 0x4 (SPI Devices)
Pin 1: SCK   
Pin 2: MISO   
Pin 3: MOSI   
Pin 4: CS/NSS 


#### Reserved (2 bits)
These two bits are currently not used. In the future, they could potentially be used as a check-sum or to expand the size of the commands or data fields. For now, both reserved bits should be 0. 

#### Data (16 bits) 
The data field is mainly used for the SPI/I2C data, but it may also be used for setting general purpose inputs on the microcontroller or external port expanders. 

If only a single bit is needed (e.g. To set GPO with command 0x0), only the LSB should be used. 

