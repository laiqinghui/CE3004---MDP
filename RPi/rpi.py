import logging
import time
import threading
import sys

from pydispatch import dispatcher

import global_settings as gs


class RPI(threading.Thread):
    def __init__(self):

        super(RPI, self).__init__()
        self.running = False

        dispatcher.connect(self.command_rpi, signal=gs.ANDROID_SIGNAL, sender=gs.ANDROID_SENDER)
        dispatcher.connect(self.command_arduino, signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)
        dispatcher.connect(self.command_algorithm, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)

        logging.info("rpi initialized")

    def command_rpi(self, message):
        logging.info("rpi received message from android and write message: " + str(message))
        # command = message.split()[0]
        # # exploration with starting point example:
        # # "ex robot_row=1 robot_col=1"
        # if(command == "ex"):
        #     for arg in message.split():
        #         if("robot_row" in arg):
        #             robot_row = arg.split("=",1)[1]
        #         elif ("robot_col" in arg):
        #             robot_col = arg.split("=",1)[1]
        # # fastest path example:
        # # "ex robot_row=1 robot_col=1 waypoint_row=10 waypoint_col=10 goal_row=12 goal_col=12"
        # elif(command == "fp"):
        #     for arg in message.split():
        #         if("robot_row" in arg):
        #             robot_row = arg.split("=",1)[1]
        #         elif ("robot_col" in arg):
        #             robot_col = arg.split("=",1)[1]
        #         elif ("waypoint_row" in arg):
        #             waypoint_row = arg.split("=",1)[1]
        #         elif ("waypoint_col" in arg):
        #             waypoint_col = arg.split("=",1)[1]
        #         elif ("goal_row" in arg):
        #             goal_row = arg.split("=",1)[1]
        #         elif ("goal_col" in arg):
        #             goal_col = arg.split("=",1)[1]
        # # manual move 3 steps example:
        # # move 3
        # elif(command == "move"):
        #     steps = int(message.split()[1])
        #     #dispatcher send 3 steps to ardunino
        #     # dispatcher.send(message=, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
        # elif(command == "rotate"):
        #     degree = int(message.split()[1])
        # # #elif(command == "reset"):

    def command_arduino(self, message):
        """
        Message received from algorithm to be processed and passed to arduino
        """
        instruction = message[0]
        completed = message[1]

        print instruction
        # if completed, send to android it is completed

        dispatcher.send(message=instruction, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("rpi received message from algorithm and write message to arduino: " + str(message))

    def command_algorithm(self, message):
        """
        - Message received from arduino to be processed and passed to algorithm
        - Updates from Arduino to be processed and passed to Android
        """
        dispatcher.send(message=message, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("rpi received message from arduino and send message to algorithm: " + str(message))

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
