import datetime
import logging
import multiprocessing as mp
import textwrap
import threading
import time

import RPi.GPIO as GPIO
import serial

from pydispatch import dispatcher

import global_settings as gs


class Arduino(threading.Thread):
    def __init__(self):

        super(Arduino, self).__init__()
        self.running = False

        GPIO.setmode(GPIO.BCM)
        GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
        GPIO.add_event_detect(17, GPIO.RISING, callback=self.interruptHandler)

        self.ser = serial.Serial('/dev/serial0')
        self.acknowledged = True

        dispatcher.connect(self.writeData, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("arduino initialized")

    def interruptHandler(self, channel):
        self.ser.reset_input_buffer()
        byte = self.readData()
        # if sensor data
        if byte[0] == "S":
            logging.info("byte[0]) == S")
            message = self.interpret_sensor_values(byte[1:])
            dispatcher.send(message=message, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)

    def interpret_sensor_values(self, arr):
        output = [ord(x) for x in arr[:5]]
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

    def write_packets_and_wait_acknowledgement(self, packeted_instr):
        for instr in packeted_instr[:-1]:
            self.ser.write(instr)
            while(GPIO.input(17) == 0):
                pass
        self.ser.write(packeted_instr[-1])

    def readData(self):
        number = self.ser.read(6)
        return number

    def break_instruction_packets(self, instruction):
        if len(instruction) <= 30:
            return [instruction]
        else:
            logging.info("long instruction received: " + str(instruction))
            packeted_instr = textwrap.wrap(instruction[1:-1], 28)

            # Special char C needs acknowledgement from arduino
            for i in range(len(packeted_instr[:-1])):
                packeted_instr[i] = 'C' + packeted_instr[i] + ';'
            packeted_instr[-1] = instruction[0] + packeted_instr[-1] + ';'
            logging.info("total packets: " + str(packeted_instr))

            return packeted_instr

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
