import getopt
import time
import threading
import sys

from pydispatch import dispatcher

from algorithm import Algorithm

import global_settings as gs


class RPI(threading.Thread):
    def __init__(self):

        print "rpi initialized"

        # algorithm pass command to arduino
        dispatcher.connect(self.command_arduino, signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)

        self.idle()

    def command_arduino(self, message):
        print "rpi received message from algorithm and write message to arduino: " + str(message)

    def idle(self):
        while(1):
            time.sleep(1)


def main(argv):

    robot_x = 0
    robot_y = 0

    try:
        opts, remainders = getopt.getopt(argv, "x:y:", ["robot_x=", "robot_y="])
    except getopt.GetoptError:
        pass

    for opt, arg in opts:

        if opt in ("-x", "--robot_x"):
            robot_x = int(arg)
        elif opt in ("-y", "--robot_y"):
            robot_y = int(arg)
        else:
            assert False, "unhandled option"

    return robot_x, robot_y


if __name__ == "__main__":

    robot_x, robot_y = main(sys.argv[1:])

    gs.init()

    rpi_thread = threading.Thread(target=RPI)
    algo_thread = threading.Thread(target=Algorithm, args=(robot_x, robot_y))

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
