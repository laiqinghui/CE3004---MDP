import time
import threading

import global_settings as gs

from pydispatch import dispatcher


class Algorithm(threading.Thread):
    def __init__(self):

        print "algorithm initialized"

        _x = 20
        _y = 15
        self.map = [[0 for i in range(_x)] for j in range(_y)]
        self.robot_x = 0
        self.robot_y = 0

        dispatcher.connect(self.determine_robot_path, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
        self.idle()

    def determine_robot_path(self, message):
        # message e.g. front and side have obstacle or not
        print "Algorithm receive obstacle info: " + str(message) + ", now calculating robot path..."
        time.sleep(1)
        dispatcher.send(message='move robot', signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)

    def idle(self):
        while 1:
            time.sleep(1)
