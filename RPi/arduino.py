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
        logging.info("\n\n=====From Interrupt Handler=====")
        logging.info("Rising edge detected on port 17 and data from arduino is: ")
        time.sleep(0.6)     # delay is essential here
        bytes = self.readData()
        logging.info(self.printBytesArray(bytes))
        logging.info("================================\n")

    def printBytesArray(self, arr):
        output = ''
        for b in arr:
            if b != 255:
                output += chr(b)
        return output

    def ConvertStringToBytes(self, src):
        converted = []
        for b in src:
            converted.append(ord(b))
        return converted

    def writeData(self, message):
        # bus.write_byte(address, value)
        data = self.ConvertStringToBytes(message)
        try:
            self.bus.write_i2c_block_data(self.address, 0, data)
        except IOError:
            logging.info("Not on RPi")
        return -1

    def readData(self):
        # number = bus.read_byte(address)
        number = self.bus.read_i2c_block_data(self.address, 0, 32)
        return number

    def idle(self):
        while 1:
            time.sleep(1)

# while True:
#     # Receives the data from the User
#     data = raw_input("Enter the data to be sent : ")
#     bytesToSend = ConvertStringToBytes(data)
#     writeData(bytesToSend)
