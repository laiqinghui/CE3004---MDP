import time
import threading

import global_settings as gs

from pydispatch import dispatcher


EXPLORATION = 0
FASTEST_PATH = 1


class Algorithm(threading.Thread):

    def __init__(self, robot_x, robot_y, waypoint_x, waypoint_y, goal_x, goal_y, mode):

        print "algorithm initialized"

        _x = 20
        _y = 15
        self.map = [[0 for i in range(_x)] for j in range(_y)]
        self.robot_x = robot_x
        self.robot_y = robot_y
        self.waypoint_x = 0
        self.waypoint_y = 0
        self.goal_x = 0
        self.goal_y = 0
        self.mode = mode

        if self.mode == EXPLORATION:
            dispatcher.connect(self.determine_exploration_path, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
        else:
            dispatcher.connect(self.determine_fastest_path, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)

        self.idle()

    def determine_exploration_path(self, message):
        # message e.g. front and side have obstacle or not
        print "Exploration robot now at position " + str(self.robot_x) + ", " + str(self.robot_y)
        print "Algorithm receive obstacle info: " + str(message) + ", now calculating robot path..."
        time.sleep(1)
        dispatcher.send(message='move robot', signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)

    def determine_fastest_path(self, message):
        # message e.g. front and side have obstacle or not
        print "Fastest path robot now at position " + str(self.robot_x) + ", " + str(self.robot_y)
        print "Algorithm receive obstacle info: " + str(message) + ", now calculating robot path..."
        time.sleep(1)
        dispatcher.send(message='move robot', signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)

    def idle(self):
        while 1:
            time.sleep(1)
