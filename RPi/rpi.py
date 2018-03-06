import logging
import time
import threading
import sys

from pydispatch import dispatcher

import global_settings as gs


class RPI(threading.Thread):
    def __init__(self):

        super(RPI, self).__init__()
        self.running = False
        self.autoupdate = True
        self.instrNum = 1

        dispatcher.connect(self.command_rpi, signal=gs.ANDROID_SIGNAL, sender=gs.ANDROID_SENDER)
        dispatcher.connect(self.manage_algorithm_signal, signal=gs.ALGORITHM_SIGNAL, sender=gs.ALGORITHM_SENDER)
        dispatcher.connect(self.manage_arduino_signal, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)

        logging.info("rpi initialized")

    def command_rpi(self, message):
        if message == "mode":
            self.autoupdate = not self.autoupdate
        # logging.info("rpi received message from android and write message: " + str(message))

    def manage_algorithm_signal(self, message):
        """
        Message received from algorithm to be processed and passed to arduino
        """
        # if fastest path
        if len(message) == 1:
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

            if not completed:
                # formatted_instruction = 'S' + ''.join(instruction) + ';'
                formatted_instruction = 'S' + ''.join(aggregated_instruction_list) + ';'
                robot_moving_stop_string_update = '1L'   # robot no longer moving after instruction
            else:
                # exploration completed, arduino do not need to sense environment after moving robot
                # formatted_instruction = 'C' + ''.join(instruction) + ';'
                formatted_instruction = 'C' + ''.join(aggregated_instruction_list) + ';'
                robot_moving_stop_string_update = '0L'   # robot still going moving after instruction

            explore_mdf_string_update = gs.get_mdf_bitstring(gs.get_explore_status_mazemap(gs.MAZEMAP), 1)
            obstacle_mdf_string_update = gs.get_mdf_bitstring(gs.get_obstacle_mazemap(gs.MAZEMAP), 1)

            print "MAP EXPLORE STATUS MDF: " + gs.get_mdf_bitstring(gs.get_explore_status_mazemap(gs.MAZEMAP), 0)
            print "OBSTACLE STATUS MDF: " + gs.get_mdf_bitstring(gs.get_obstacle_mazemap(gs.MAZEMAP), 0)

            map_mdf_update_string = "MDF" + explore_mdf_string_update + obstacle_mdf_string_update
            dir_update_string = "DIR" + str(robot_row) + 'L' + str(robot_col) + 'L' + str(robot_dir) + 'L' + robot_moving_stop_string_update

            self.feedback_android(map_mdf_update_string)
            self.feedback_android(dir_update_string)

            # try:
            #     self.pc_ws.send(map_mdf_update_string)
            #     self.pc_ws.send(dir_update_string)
            # except:
            #     pass

        dispatcher.send(message=formatted_instruction, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("robot location: " + str(robot_row) + ", " + str(robot_col))
        print gs.MAZEMAP
        logging.info("print instr no: " + str(self.instrNum))
        logging.info("number of instruction counts" + str(len(formatted_instruction[1:-1])))
        logging.info("rpi received message from algorithm and write message to arduino: " + str(formatted_instruction))
        self.instrNum = self.instrNum + 1

        print "==============================================================="

    def manage_arduino_signal(self, message):
        """
        - Message received from arduino to be processed and passed to algorithm
        - Updates from Arduino to be processed and passed to Android
        """
        logging.info("sensor value: " + str(message))
        message[0] = message[0] - 13
        message[1] = message[1] - 8
        message[2] = message[2] - 12
        message[3] = message[3] - 13
        message[4] = message[4] - 26

        # raw_input("---------press enter to continue-------")

        logging.info("send sensor values to algo:" + str(message))

        dispatcher.send(message=message, signal=gs.RPI_ALGORITHM_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("rpi received message from arduino and send message to algorithm: " + str(message))

    def feedback_android(self, message):
        if self.autoupdate:
            dispatcher.send(message=message, signal=gs.RPI_ANDROID_SIGNAL, sender=gs.RPI_SENDER)
        # logging.info("rpi send feedback message to android: " + str(message))

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
