import getopt
import sys
import time
import threading

from pydispatch import dispatcher

from robot.algorithm import Algorithm
# from android import Android
# from arduino import Arduino
from rpi import RPI
import global_settings as gs


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
    algo_thread.daemon = True

    rpi_thread.start()
    # arduino_thread.start()
    algo_thread.start()

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

    gs.MAZEMAP.fill(1)
    while 1:
        row_col_string = raw_input("Enter row, col:")
        if row_col_string == ";":
            break
        row, col = [int(x) for x in row_col_string.split(',')]
        gs.MAZEMAP[row, col] = 2

    start_robot_exploration(rr, rc, wr, wc, gr, gc, m, d)

    while 1:
        time.sleep(1)
        # data = raw_input("Enter the data to be sent to algorithm: ")
        # dispatcher.send(message=[16.0, 16.0, 16.0, 0, 5.0], signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
