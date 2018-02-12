import algorithm
import logging

import global_settings as gs

from pydispatch import dispatcher


class ExplorationAlgorithm(algorithm.Algorithm):

    def __init__(self, robot_row, robot_col, waypoint_row, waypoint_col, goal_row, goal_col, mode):

        super(ExplorationAlgorithm, self).__init__(robot_row, robot_col, waypoint_row, waypoint_col, goal_row, goal_col, mode)

    def determine_exploration_path(self, message):

        sensor_vals = message

        logging.info("test: " + str(message) + ", now calculating robot path...")
        dispatcher.send(message='move robot', signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)
