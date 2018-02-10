import logging
import time
import threading

from pydispatch import dispatcher

import global_settings as gs


class RPI(threading.Thread):
    def __init__(self):

        logging.info("rpi initialized")

        dispatcher.connect(self.command_rpi, signal=gs.ANDROID_SIGNAL, sender=gs.ANDROID_SENDER)
        dispatcher.connect(self.command_arduino, signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)
        dispatcher.connect(self.command_algorithm, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)

        self.idle()

    def command_rpi(self, message):
        logging.info("rpi received message from android and write message: " + str(message))

    def command_arduino(self, message):
        """
        Message received from algorithm to be processed and passed to arduino
        """
        dispatcher.send(message=message, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("rpi received message from algorithm and write message to arduino: " + str(message))

    def command_algorithm(self, message):
        """
        - Message received from arduino to be processed and passed to algorithm
        - Updates from Arduino to be processed and passed to Android
        """
        dispatcher.send(message=message, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("rpi received message from arduino and send message to algorithm: " + str(message))

    def idle(self):
        while(1):
            time.sleep(1)
