import logging
import time
import threading

from pydispatch import dispatcher

import global_settings as gs


class RPI(threading.Thread):
    def __init__(self):

        logging.info("rpi initialized")

        # algorithm pass command to arduino
        dispatcher.connect(self.command_arduino, signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)

        self.idle()

    def command_arduino(self, message):
        logging.info("rpi received message from algorithm and write message to arduino: " + str(message))

    def idle(self):
        while(1):
            time.sleep(1)
