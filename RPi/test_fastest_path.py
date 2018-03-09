import getopt
import logging
import sys
import time
import threading

from pydispatch import dispatcher

from robot.algorithm import Algorithm
# from android import Android
# from arduino import Arduino
from rpi import RPI
import global_settings as gs
import numpy as np


def initialise_robot_options(argv):

    gs.init()

    robot_row = 0
    robot_col = 0
    waypoint_row = 0
    waypoint_col = 0
    goal_row = 0
    goal_col = 0
    mode = 0

    try:
        opts, remainders = getopt.getopt(argv, "m:", ["rr=", "rc=", "wr=", "wc=", "gr=", "gc=", "mode=", "dir="])
    except getopt.GetoptError:
        pass

    for opt, arg in opts:

        if opt in ("-r", "--rr"):
            robot_row = int(arg)
        elif opt in ("-c", "--rc"):
            robot_col = int(arg)
        elif opt in ("--wr"):
            waypoint_row = int(arg)
        elif opt in ("--wc"):
            waypoint_col = int(arg)
        elif opt in ("--gr"):
            goal_row = int(arg)
        elif opt in ("--gc"):
            goal_col = int(arg)
        elif opt in ("-m", "--mode"):
            mode = int(arg)
        elif opt in ("--dir"):
            direction = int(arg)
        else:
            assert False, "unhandled option"

    return robot_row, robot_col, waypoint_row, waypoint_col, goal_row, goal_col, mode, direction


# TODO: Convert to method in android.py bluetooth connection file
def start_robot_exploration(rr, rc, wr, wc, gr, gc, m, d, keep_alive=False):
    """
    Function to start the robot exploration. This should be executed as a non-daemon
    thread so that it can be stopped when required.

    This function will need to move to the android.py file as a method.
    """
    rpi_thread = RPI()
    # arduino_thread = Arduino()
    algo_thread = Algorithm(rr, rc, wr, wc, gr, gc, m, d)

    rpi_thread.daemon = True
    # arduino_thread.daemon = True
    # algo_thread.daemon = True

    rpi_thread.start()
    # arduino_thread.start()
    fastestPathInstruction = algo_thread.determine_fastest_path()
    logging.info("Fastest path calculated!")
    algo_thread.run_fatest_path_on(self.fastestPathInstruction)
    logging.info("Now running on fastest path instruction...")

    if keep_alive:
        while 1:
            time.sleep(1)


if __name__ == "__main__":
    """
    For final implementation, only android thread will be initialized first.
    RPI, Algorithm and Arduino thread will only be initialized when command
    received from Android thread.
    """

    """RUN MAIN.PY TO TEST ALGORITHM & RPI INTERFACE"""
    # python test_fastest_path.py --rr=18 --rc=1 --wr=5 --wc=9 --gr=1 --gc=13 --mode=1 --dir=2
    rr, rc, wr, wc, gr, gc, m, d = initialise_robot_options(sys.argv[1:])

# #                           0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
#     gs.MAZEMAP = np.array([[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 0
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 1
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 2
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 3
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 4
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 5
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 6
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 7
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 8
#                            [1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1],   # 9
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 10
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 11
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 12
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 13
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 14
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 15
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 16
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 17
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],   # 18
#                            [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]])  # 19

    # gs.MAZEMAP = np.array()

    raw_input("Enter to calculate fastest path")
    logging.info("Fastest path calculating...")

    start_robot_exploration(rr, rc, wr, wc, gr, gc, m, d)

    while 1:
        time.sleep(1)
