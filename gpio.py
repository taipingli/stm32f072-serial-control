import serial 
from enum import Enum
import time

class PINS(Enum):
	PA_0 = 0
	PA_1 = 1
	PA_2 = 2
	PA_3 = 3
	PA_4 = 4
	PA_5 = 5
	PA_6 = 6
	PA_7 = 7
	PA_8 = 8
	PA_9 = 9
	PA_10 = 10
	PA_15 = 11
	PB_0 = 12
	PB_1 = 13
	PB_2 = 14
	PB_3 = 15
	PB_4 = 16
	PB_5 = 17
	PB_6 = 18
	PB_7 = 19
	PB_9 = 20
	PB_10 = 21
	PB_11 = 22
	PB_12 = 23
	PB_13 = 24
	PB_14 = 25
	PB_15 = 26
	PB_13 = 27
	ADC_TEMP = 28
	ADC_VREF = 29
	ADC_VBAT = 30

class CONTROLLER_BOARD(Enum):
	LED_RED = PINS.PA_15
	LED_GREEN = PINS.PB_3
	LED_BLUE_1 = PINS.PB_4
	LED_BLUE_2 = PINS.PB_5
	VREF = PINS.ADC_VREF
	VBAT = PINS.ADC_VBAT
	TEMP = PINS.ADC_TEMP

class CENTER_CONSOLE(Enum):
	LOW_BEAM_BUTTON = PINS.PA_0
	HAZARDS_BUTTON = PINS.PA_1
	DRL_BUTTON = PINS.PA_4
	DRIVE_BUTTON = PINS.PA_5
	NEUTRAL_BUTTON = PINS.PA_6
	REVERSE_BUTTON = PINS.PA_7
	POWER_BUTTON = PINS.PB_0
	ADC_5V_MONITOR = PINS.PB_1
	FAN_EN = PINS.PB_9

COMMAND_GPO = 0x0
COMMAND_GPI = 0x1
COMMAND_ADC = 0x2

class GPIO():
	def __init__(self, serial_port='/dev/cu.usbserial-FTG43MW4'):
		self.ser = serial.Serial(serial_port, timeout=0)
		self.ser.flushOutput()
		self.ser.flushInput()
		print("Connected to %s\n" % self.ser.name)

	def pack_data(self, command, pin, state):
		data = (command << 8) | (pin << 3) | state
		data = str(data) + '\n'
		print data
		self.ser.write(data)
		time.sleep(0.05)
		return self.ser.readline()

	def pack_long_data(self, command, pin_1, pin_2, pin_3, pin_4, pin_5, pin_6, data, return_data=False):
		data = (command << 48) | (pin_1 << 43) | (pin_2 << 38) | (pin_3 << 33) | (pin_4 << 28) | (pin_5 << 23) | (pin_6 << 18) | 0x00 << 16 | data
		data = str(data) + '\n'
		self.ser.write(data)
		if return_data:
			time.sleep(0.1)
			return_data = self.ser.readline()
			return return_data
		return 

	def get_pin_value(self, pin):
		return pin.value.value

	def set_GPO(self, pin, state):
		self.pack_long_data(COMMAND_GPO, self.get_pin_value(pin), 0, 0, 0, 0, 0, state)

	def read_GPI(self, pin):
		return self.pack_long_data(COMMAND_GPI, self.get_pin_value(pin), 0, 0, 0, 0, 0, 0, True)

	def read_ADC(self, pin):
		return float(self.pack_long_data(COMMAND_ADC, self.get_pin_value(pin), 0, 0, 0, 0, 0, 0, 
			True))

	def close(self):
		self.ser.close()

def only_numerics(seq):
	val = filter(type(seq).isdigit, seq)
	print val
	return val

def test_pack_data():
	start_time = time.time()
	total_time = 0
	counter = 0
	error = 0

	for command in range(0, 16):
		for pin in range(0, 32):
			for state in range(0, 2):
				test = str(command) + str(pin) + str(state)
				result =  str(controller_board.pack_data(command, pin, state))
				if not (only_numerics(result) == only_numerics(test)):
					error = error + 1
				counter = counter + 1

	avg_time = (time.time() - start_time)/counter
	print("Total: %i iterations. Avg time: %f, Errors %i" % (counter, avg_time, error))


if __name__ == "__main__":

	controller_board = GPIO()
	print controller_board.read_ADC(CONTROLLER_BOARD.VBAT)
	print controller_board.read_ADC(CONTROLLER_BOARD.VREF)
	print controller_board.read_ADC(CONTROLLER_BOARD.TEMP)

	while True:
		controller_board.set_GPO(CONTROLLER_BOARD.LED_RED, 0)
		time.sleep(1)
		controller_board.set_GPO(CONTROLLER_BOARD.LED_RED, 1)
		time.sleep(1)

	controller_board.close()
