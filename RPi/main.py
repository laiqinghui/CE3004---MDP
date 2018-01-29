import getopt
import time
import threading
import sys

from pydispatch import dispatcher

from algorithm import Algorithm
from rpi import RPI

import global_settings as gs


def main(argv):

    robot_x = 0
    robot_y = 0
    waypoint_x = 0
    waypoint_y = 0
    goal_x = 0
    goal_y = 0
    mode = 0

    try:
        opts, remainders = getopt.getopt(argv, "m:", ["rx=", "ry=", "wx=", "wy=", "gx=", "gy=", "mode="])
    except getopt.GetoptError:
        pass

    for opt, arg in opts:

        if opt in ("-x", "--rx"):
            robot_x = int(arg)
        elif opt in ("-y", "--ry"):
            robot_y = int(arg)
        elif opt in ("--wx"):
            waypoint_x = int(arg)
        elif opt in ("--wy"):
            waypoint_y = int(arg)
        elif opt in ("--gx"):
            goal_x = int(arg)
        elif opt in ("--gy"):
            goal_y = int(arg)
        elif opt in ("-m", "--mode"):
            mode = int(arg)
        else:
            assert False, "unhandled option"

    return robot_x, robot_y, waypoint_x, waypoint_y, goal_x, goal_y, mode


if __name__ == "__main__":

    # python main.py --rx=1 --ry=1 --wx=5 --wy=9 --gx=19 --gy=14 -m 0
    rx, ry, wx, wy, gx, gy, m = main(sys.argv[1:])

    gs.init()

    rpi_thread = threading.Thread(target=RPI)
    algo_thread = threading.Thread(target=Algorithm, args=(rx, ry, wx, wy, gx, gy, m))

    rpi_thread.daemon = True
    algo_thread.daemon = True

    rpi_thread.start()
    algo_thread.start()

    # keep program alive
    data = ""
    while data == "":
        time.sleep(1)
        data = raw_input("Enter the data to be sent to algorithm: ")
        dispatcher.send(message=data, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
        data = ""
