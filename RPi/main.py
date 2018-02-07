import getopt
import sys
import time
import threading

from pydispatch import dispatcher

from algorithm import Algorithm
from android import Android
from arduino import Arduino
from rpi import RPI
import global_settings as gs


def initialise_robot_options(argv):

    #gs.init()

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


# TODO: Convert to method in android.py bluetooth connection file
def start_robot_exploration(rx, ry, wx, wy, gx, gy, m, keep_alive=False):
    """
    Function to start the robot exploration. This should be executed as a non-daemon
    thread so that it can be stopped when required.

    This function will need to move to the android.py file as a method.
    """
    rpi_thread = threading.Thread(target=RPI)
    algo_thread = threading.Thread(target=Algorithm, args=(rx, ry, wx, wy, gx, gy, m))
    arduino_thread = threading.Thread(target=Arduino)

    rpi_thread.daemon = True
    algo_thread.daemon = True
    arduino_thread.daemon = True

    rpi_thread.start()
    algo_thread.start()
    arduino_thread.start()

    if keep_alive:
        while 1:
            time.sleep(1)


if __name__ == "__main__":
    """
    For final implementation, only android thread will be initialized first.
    RPI, Algorithm and Arduino thread will only be initialized when command
    received from Android thread.
    """
    # TODO: android.py file that connects with Android device using bluetooth
    # android_thread = threading.Thread(target=Android)
    # android_thread.daemon = True
    # android_thread.start()
    #
    # while 1:
    #     time.sleep(1)

    """RUN MAIN.PY TO TEST ALGORITHM & RPI INTERFACE"""
    # python main.py --rx=1 --ry=1 --wx=5 --wy=9 --gx=19 --gy=14 -m 0
    # sys.argv[1:] = ['--rx=1', '--ry=1', '--wx=5', '--wy=9', '--gx=19', '--gy=14', '-m', '0']
    rx, ry, wx, wy, gx, gy, m = initialise_robot_options(sys.argv[1:])

    start_robot_exploration(rx, ry, wx, wy, gx, gy, m)

    while 1:
        time.sleep(1)
        data = raw_input("Enter the data to be sent to algorithm: ")
        dispatcher.send(message=data, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
