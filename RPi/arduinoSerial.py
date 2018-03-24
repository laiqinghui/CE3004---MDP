import datetime
import logging
import threading
import time

from pydispatch import dispatcher
import serial

import global_settings as gs


class Arduino(threading.Thread):
    def __init__(self):

        super(Arduino, self).__init__()
        self.running = False
        self.serialConnection = serial.Serial('/dev/ttyUSB0')

        dispatcher.connect(self.writeData, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("arduino initialized")

    def interpret_sensor_values(self, arr):
        output = [int(x) for x in arr[:5]]
        logging.info("sensor output len should be 5: " + str(output))
        return output

    def ConvertStringToBytes(self, src):
        return [ord(x) for x in src]

    def writeData(self, message):

        data = self.ConvertStringToBytes(message)
        self.serialConnection.write(data)

    def start(self):
        self.running = True
        super(Arduino, self).start()

    def run(self):
        self.idle()

    def stop(self):
        self.running = False

    def listenSerialData(self):
        if(self.serialConnection.inWaiting() > 0):
            byte = self.serialConnection.read(10)
            if chr(byte[0]) == "S":
                logging.info("byte[0]) == S")
                message = self.interpret_sensor_values(byte[1:])
                dispatcher.send(message=message, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)

    def idle(self):
        while(self.running):
            self.listenSerialData()
