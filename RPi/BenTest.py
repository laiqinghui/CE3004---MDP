import datetime
import logging
import multiprocessing as mp
import textwrap
import threading
import time

import serial
import wiringpi2 as wiringpi
import RPi.GPIO as GPIO
import smbus

from pydispatch import dispatcher

import global_settings as gs


class Arduino(threading.Thread):
    def __init__(self):

        super(Arduino, self).__init__()
        self.running = False
		
		wiringpi.wiringPiSetup()
		serial = wiringpi.serialOpen('/dev/ttyAMA0',9600)
		wiringpi.serialPuts(serial,'hello world!')
		

        GPIO.setmode(GPIO.BCM)
        GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
        GPIO.add_event_detect(17, GPIO.RISING, callback=self.interruptHandler)

        self.address = 0x04
        self.bus = smbus.SMBus(1)
        self.mutex_w = mp.Lock()
        self.acknowledged = True

        dispatcher.connect(self.writeData, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("arduino initialized")

    def interruptHandler(self, channel):
        time.sleep(0.15)
        byte = self.readData()
        # if acknowledgement byte
        if chr(byte[0]) == "A":
            logging.info("arduino sent acknowledgement")
            self.mutex_w.acquire()
            if self.acknowledged:
                logging.info("Error: acknowledgement byte from arduino not resolved yet")
            self.acknowledged = True
            self.mutex_w.release()
        # if sensor data
        if chr(byte[0]) == "S":
            logging.info("byte[0]) == S")
			while 1 :
			ser.readline()
			
            message = self.interpret_sensor_values(byte[1:])
            dispatcher.send(message=message, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)

    def interpret_sensor_values(self, arr):
        output = [int(x) for x in arr[:5]]
        logging.info("sensor output len should be 5: " + str(output))
        return output

    def readBytesArray(self, arr):
        output = ''.join([chr(x) for x in arr if x != 255])
        return output

    def ConvertStringToBytes(self, src):
        return [ord(x) for x in src]

    def writeData(self, message):

        packeted_instr = self.break_instruction_packets(message)
        self.write_packets_and_wait_acknowledgement(packeted_instr)
        # arduino_write_thread = threading.Thread(target=self.write_packets_and_wait_acknowledgement, args=(packeted_instr,))
        # arduino_write_thread.daemon = True
        # arduino_write_thread.start()
        # data = self.ConvertStringToBytes(message)
        # self.bus.write_i2c_block_data(self.address, 0, data)

    def write_packets_and_wait_acknowledgement(self, packeted_instr):
        for instr in packeted_instr[:-1]:
            data = self.ConvertStringToBytes(instr)
            self.bus.write_i2c_block_data(self.address, 0, data)
            while(GPIO.input(17) == 0):
                pass
            # try:
            #     self.mutex_w.acquire()
            #     self.acknowledged = False
            #     self.mutex_w.release()
            #     data = self.ConvertStringToBytes(instr)
            #     self.bus.write_i2c_block_data(self.address, 0, data)
            #     while not self.acknowledged:
            #         pass
            # except IOError:
            #     logging.info("Please check if arduino connected.")
        data = self.ConvertStringToBytes(packeted_instr[-1])
        self.bus.write_i2c_block_data(self.address, 0, data)

    def write_and_wait_acknowledgement(self, data):
        try:
            self.mutex_w.acquire()
            self.acknowledged = False
            self.mutex_w.release()
            self.bus.write_i2c_block_data(self.address, 0, data)
            dt_started = datetime.datetime.now()
            base_time = 1
            while 1:
                logging.info("waiting for acknowledgement: " + str(data))
                time.sleep(1)
                if self.acknowledged:
                    logging.info("Arduino has acknowledged")
                    break
                # send again if acknowledgement not receive after every 5 second
                # pass from last send
                dt_ended = datetime.datetime.now()
                time_passed = (dt_ended - dt_started).seconds / 5
                if time_passed > base_time:
                    base_time = time_passed
                    self.bus.write_i2c_block_data(self.address, 0, data)

        except IOError:
            logging.info("Please check if arduino connected.")

    def readData(self):
        number = self.bus.read_i2c_block_data(self.address, 0, 32)
        return number

    def break_instruction_packets(self, instruction):
        if len(instruction) <= 30:
            return [instruction]
        else:
            logging.info("long instruction received: " + str(instruction))
            packeted_instr = self.split_str(instruction[1:-1], 28)

            # Special char C needs acknowledgement from arduino
            for i in range(len(packeted_instr[:-1])):
                packeted_instr[i] = 'C' + packeted_instr[i] + ';'
            packeted_instr[-1] = instruction[0] + packeted_instr[-1] + ';'
            logging.info("total packets: " + str(packeted_instr))

            return packeted_instr

    def split_str(self, seq, chunk):
        return textwrap.wrap(seq, chunk)

    def start(self):
        self.running = True
        super(Arduino, self).start()

    def run(self):
        self.idle()

    def stop(self):
        self.running = False

    def idle(self):
        while(self.running):
            time.sleep(1)
