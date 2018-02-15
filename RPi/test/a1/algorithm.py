import logging
import time
import threading

import test_settings as ts

from pydispatch import dispatcher


class Algorithm(threading.Thread):

    def __init__(self):

        super(Algorithm, self).__init__()
        self.running = False

        dispatcher.connect(self.test_message_received, signal=ts.RPI_ALGORITHM_SIGNAL, sender=ts.RPI_SENDER)
        logging.info("algorithm initialized")

    def test_message_received(self, message):
        logging.info("Algorithm receive message '" + str(message) + "' from RPI")
        logging.info("Algorithm now send message '" + str(message) + "' to Arduino")
        dispatcher.send(message="message", signal=ts.ALGORITHM_SIGNAL, sender=ts.ALGORITHM_SENDER)

    def start(self):
        self.running = True
        super(Algorithm, self).start()

    def run(self):
        self.idle()

    def stop(self):
        self.running = False

    def idle(self):
        while(self.running):
            time.sleep(1)
