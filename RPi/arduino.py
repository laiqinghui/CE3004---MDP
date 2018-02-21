import datetime
import logging
import multiprocessing as mp
import threading
import time

import RPi.GPIO as GPIO
import smbus

from pydispatch import dispatcher

import global_settings as gs


class Arduino(threading.Thread):
    def __init__(self):

        super(Arduino, self).__init__()
        self.running = False

        GPIO.setmode(GPIO.BCM)
        GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
        GPIO.add_event_detect(17, GPIO.RISING, callback=self.interruptHandler)

        self.address = 0x04
        self.bus = smbus.SMBus(1)
        self.mutex_w = mp.Lock()
        self.acknowledged = False

        dispatcher.connect(self.writeData, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("arduino initialized")

    def interruptHandler(self, channel):
        time.sleep(0.6)
        byte = self.readData()
        logging.info(self.readBytesArray(byte))
        message = "TEST"
        # if acknowledgement byte
        if chr(byte[0]) == "A":
            logging.info("arduino acknowledged")
            self.mutex_w.acquire()
            if self.acknowledged:
                logging.info("Error: acknowledgement byte from arduino not resolved yet")
            self.acknowledged = True
            self.mutex_w.release()
        # if sensor data
        elif chr(byte[0]) == "S":
            logging.info("byte[0]) == S")
            message = self.readBytesArray(byte[1:])
            # message = self.readBytesArray(byte)
            dispatcher.send(message=message, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)

    def readBytesArray(self, arr):
        output = ''.join([chr(x) for x in arr if x != 255])
        return output

    def ConvertStringToBytes(self, src):
        return [ord(x) for x in src]

    def writeData(self, message):
        data = self.ConvertStringToBytes(message)
        try:
            self.bus.write_i2c_block_data(self.address, 0, data)

            dt_started = datetime.datetime.now()
            base_time = 1
            while 1:
                logging.info("waiting for acknowledgement")
                time.sleep(1)
                if self.acknowledged:
                    break
                # send again if acknowledgement not receive after every 5 second
                # pass from last send
                dt_ended = datetime.datetime.now()
                time_passed = (dt_ended - dt_started).seconds / 5
                if time_passed > base_time:
                    base_time = time_passed
                    self.bus.write_i2c_block_data(self.address, 0, data)

            self.mutex_w.acquire()
            self.acknowledged = False
            self.mutex_w.release()

        except IOError:
            logging.info("Please check if arduino connected.")

    def readData(self):
        number = self.bus.read_i2c_block_data(self.address, 0, 32)
        return number

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