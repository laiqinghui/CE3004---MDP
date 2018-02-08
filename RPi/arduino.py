import logging
import threading
import time

import RPi.GPIO as GPIO
import smbus

from pydispatch import dispatcher

import global_settings as gs


class Arduino(threading.Thread):
    def __init__(self):

        logging.info("arduino initialized")

        GPIO.setmode(GPIO.BCM)
        GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
        GPIO.add_event_detect(17, GPIO.RISING, callback=self.interruptHandler)

        self.bus = smbus.SMBus(1)
        self.address = 0x04
        dispatcher.connect(self.writeData, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)

        self.idle()

    def interruptHandler(self, channel):
        time.sleep(0.6)     # delay is essential here
        bytes = self.readData()
        message = getBytesArray(bytes)
        dispatcher.send(message=message, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)

    def getBytesArray(self, arr):
        output = ''.join([chr(x) for x in arr if x != 255])
        return output

    def ConvertStringToBytes(self, src):
        return [ord(x) for x in src]

    def writeData(self, message):
        data = self.ConvertStringToBytes(message)
        try:
            self.bus.write_i2c_block_data(self.address, 0, data)
        except IOError:
            logging.info("Not on RPi")
        return -1

    def readData(self):
        number = self.bus.read_i2c_block_data(self.address, 0, 32)
        return number

    def idle(self):
        while 1:
            time.sleep(1)
