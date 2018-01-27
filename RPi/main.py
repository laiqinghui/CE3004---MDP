import threading
import time

from algorithm import Algorithm

import global_settings as gs

from pydispatch import dispatcher


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


# class Algorithm(threading.Thread):
#     def __init__(self):
#
#         print "algorithm initialized"
#
#         _x = 20
#         _y = 15
#         self.map = [[0 for i in range(_x)] for j in range(_y)]
#         self.robot_x = 0
#         self.robot_y = 0
#
#         dispatcher.connect(self.determine_robot_path, signal=gs.RPI_SIGNAL, sender=gs.RPI_SENDER)
#         self.idle()
#
#     def determine_robot_path(self, message):
#         # message e.g. front and side have obstacle or not
#         print "receive obstacle info: " + str(message) + ", now calculating robot path..."
#         time.sleep(1)
#         dispatcher.send(message='move robot', signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)
#
#     def idle(self):
#         while 1:
#             time.sleep(1)


if __name__ == "__main__":

    gs.init()

    rpi_thread = threading.Thread(target=RPI)
    algo_thread = threading.Thread(target=Algorithm)

    rpi_thread.daemon = True
    algo_thread.daemon = True

    rpi_thread.start()
    algo_thread.start()

    # keep program alive
    data = ""
    while data == "":
        time.sleep(1)
        data = raw_input("Enter the data to be sent to algorithm: ")
        dispatcher.send(message=data, signal=gs.RPI_SIGNAL, sender=gs.RPI_SENDER)
        data = ""
