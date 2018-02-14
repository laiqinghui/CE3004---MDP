import logging
import numpy as np
import time
import threading

import Exploration
import FastestPath

import global_settings as gs
from Real import Robot

from pydispatch import dispatcher


EXPLORATION = 0
FASTEST_PATH = 1


class Algorithm(threading.Thread):

    def __init__(self, robot_row, robot_col, waypoint_row, waypoint_col, goal_row, goal_col, mode, dir):

        super(Algorithm, self).__init__()
        self.running = False

        self.dir = dir
        self.r_row = robot_row
        self.r_col = robot_col
        self.w_row = waypoint_row
        self.w_col = waypoint_col
        self.g_row = goal_row
        self.g_col = goal_col
        self.mode = mode
        self.currentMap = gs.MAZEMAP
        self.algorithmClass = None

        if self.mode == EXPLORATION:
            self.algorithmClass = Exploration.Exploration(timeLimit=5, sim=False)
            dispatcher.connect(self.determine_exploration_path, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
        elif self.mode == FASTEST_PATH:
            self.algorithmClass = FastestPath.FastestPath(exploredMap=gs.MAZEMAP,
                                                          start=[self.r_row, self.r_col],
                                                          goal=[self.g_row, self.g_col],
                                                          direction=self.dir,
                                                          waypoint=[self.w_row, self.w_col])
            dispatcher.connect(self.determine_fastest_path, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
        else:
            dispatcher.connect(self.test_message_received, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)

        logging.info("algorithm initialized")

    def determine_exploration_path(self, message):

        sensor_vals = message
        instruction = ""

        instruction = self.algorithmClass.moveStep(sensor_vals)

        # message e.g. front and side have obstacle or not
        # logging.info("Exploration robot now at position " + str(self.r_row) + ", " + str(self.r_col))
        # logging.info("Algorithm receive obstacle info: " + str(sensor_vals) + ", now calculating robot path...")
        # time.sleep(1)
        dispatcher.send(message=instruction, signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)

    def determine_fastest_path(self, message):

        sensor_vals = message
        instruction = ""
        # message e.g. front and side have obstacle or not
        # logging.info("Fastest path robot now at position " + str(self.r_row) + ", " + str(self.r_col))
        # logging.info("Algorithm receive obstacle info: " + str(sensor_vals) + ", now calculating robot path...")
        # time.sleep(1)
        # instruction = self.algorithmClass.moveStep(sensor_vals)

        dispatcher.send(message=instruction, signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)

    def test_message_received(self, message):
        logging.info("Algorithm receive message '" + str(message) + "' from RPI")
        logging.info("Algorithm now send message '" + str(message) + "' to Arduino")
        dispatcher.send(message="message", signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)

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
