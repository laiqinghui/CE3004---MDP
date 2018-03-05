import logging
import time
import threading
import sys

from pydispatch import dispatcher

import test_settings as ts


class RPI(threading.Thread):
    def __init__(self):

        super(RPI, self).__init__()
        self.running = False

        dispatcher.connect(self.command_rpi, signal=ts.ANDROID_SIGNAL, sender=ts.ANDROID_SENDER)
        dispatcher.connect(self.command_arduino, signal=ts.ALGORITHM_SIGNAL, sender=ts.ALGORITHM_SENDER)
        dispatcher.connect(self.command_algorithm, signal=ts.ARDUINO_SIGNAL, sender=ts.ARDUINO_SENDER)

        logging.info("rpi initialized")

    def command_rpi(self, message):
        logging.info("rpi received message from android and write message: " + str(message))
        self.command_arduino(int(message))

    def command_arduino(self, message):
        """
        Message received from algorithm to be processed and passed to arduino
        """
        dispatcher.send(message="CI" + str(message) + ";", signal=ts.RPI_ARDUINO_SIGNAL, sender=ts.RPI_SENDER)
        logging.info("rpi received message from algorithm and write message to arduino: " + str(message))

    def command_algorithm(self, message):
        """
        - Message received from arduino to be processed and passed to algorithm
        - Updates from Arduino to be processed and passed to Android
        """
        dispatcher.send(message=message, signal=ts.RPI_ALGORITHM_SIGNAL, sender=ts.RPI_SENDER)
        logging.info("rpi received message from arduino and write message to algorithm: " + str(message))

    def start(self):
        self.running = True
        super(RPI, self).start()

    def run(self):
        self.idle()

    def stop(self):
        self.running = False

    def idle(self):
        while(self.running):
            time.sleep(1)
