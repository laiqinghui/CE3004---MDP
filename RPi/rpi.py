import logging
import time
import threading
import sys

from pydispatch import dispatcher
from websocket import create_connection

import global_settings as gs


class RPI(threading.Thread):
    def __init__(self):

        super(RPI, self).__init__()
        self.running = False
        self.autoupdate = True
        self.pc_ws = None

        try:
            self.pc_ws = create_connection("ws://192.168.5.18:8888/ws")
        except:
            pass

        dispatcher.connect(self.command_rpi, signal=gs.ANDROID_SIGNAL, sender=gs.ANDROID_SENDER)
        dispatcher.connect(self.manage_algorithm_signal, signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)
        dispatcher.connect(self.manage_arduino_signal, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)

        logging.info("rpi initialized")

    def command_rpi(self, message):
        if message == "mode":
            self.autoupdate = not self.autoupdate

    def manage_algorithm_signal(self, message):
        """
        Message received from algorithm to be processed and passed to arduino
        """
        # if fastest path
        if len(message) == 1:
            instruction = message[0]
            raw_instruction = ''.join(instruction)
            aggregated_instruction_list = gs.aggregate_instruction(raw_instruction)
            formatted_instruction = 'C' + ','.join(aggregated_instruction_list) + ';'
        # if exploration
        if len(message) > 1:

            instruction = message[0]
            completed = message[1]
            robot_row = message[2][0]
            robot_col = message[2][1]
            robot_dir = message[3]

            raw_instruction = ''.join(instruction)
            aggregated_instruction_list = gs.aggregate_instruction(raw_instruction)

            if completed:
                # exploration completed, arduino do not need to sense environment after moving robot
                formatted_instruction = 'C' + ''.join(aggregated_instruction_list) + ';'
                robot_moving_stop_string_update = '1L'   # robot still going moving after instruction
            if not completed:
                formatted_instruction = 'S' + ''.join(aggregated_instruction_list) + ';'
                robot_moving_stop_string_update = '0L'   # robot no longer moving after instruction

            explore_mdf_string_update = gs.get_mdf_bitstring(gs.MAZEMAP, 1, 0)
            obstacle_mdf_string_update = gs.get_mdf_bitstring(gs.MAZEMAP, 1, 1)

            print "MAP EXPLORE STATUS MDF: " + explore_mdf_string_update
            print "OBSTACLE STATUS MDF: " + obstacle_mdf_string_update

            map_mdf_update_string = "MDF" + explore_mdf_string_update + 'L' + obstacle_mdf_string_update + 'L'
            dir_update_string = "DIR" + str(abs(robot_row - 19)) + 'L' + str(robot_col) + 'L' + str(robot_dir) + 'L' + robot_moving_stop_string_update

            self.feedback_android(map_mdf_update_string)
            self.feedback_android(dir_update_string)

            try:
                self.pc_ws.send(map_mdf_update_string)
                self.pc_ws.send(dir_update_string)
            except:
                pass

            logging.info("robot location: " + str(robot_row) + ", " + str(robot_col))

        dispatcher.send(message=formatted_instruction, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)

        print gs.MAZEMAP
        logging.info("rpi received message from algorithm and write message to arduino: " + str(formatted_instruction))
        print "==============================================================="

    def manage_arduino_signal(self, message):
        """
        - Message received from arduino to be processed and passed to algorithm
        - Updates from Arduino to be processed and passed to Android
        """
        logging.info("sensor value: " + str(message))
        message[0] = message[0] - 13
        message[1] = message[1] - 8
        message[2] = message[2] - 13
        message[3] = message[3] - 12
        message[4] = message[4] - 20

        with open("sensor.txt", "a") as sensor_log:
            sensor_log.write(str(message) + "\n")

        # raw_input("---------press enter to continue-------")

        logging.info("send sensor values to algo:" + str(message))

        dispatcher.send(message=message, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("rpi received message from arduino and send message to algorithm: " + str(message))

    def feedback_android(self, message):
        if self.autoupdate:
            dispatcher.send(message=message, signal=gs.RPI_ANDROID_SIGNAL, sender=gs.RPI_SENDER)

    def start(self):
        self.running = True
        super(RPI, self).start()

    def run(self):
        self.idle()

    def stop(self):
        self.running = False

    def idle(self):
        while(self.running):
            time.sleep(1)
