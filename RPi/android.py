import logging
import time
import threading
import sys
import global_settings as gs

from bluetooth import *
from pydispatch import dispatcher

from arduino import Arduino
from rpi import RPI
from robot.algorithm import Algorithm

START_ROW = 1
START_COL = 18
FP_GOAL_ROW = 13
FP_GOAL_COL = 1
EXPLORATION = 0
FASTEST_PATH = 1
EAST = 2


class Android(threading.Thread):
    def __init__(self):
        super(Android, self).__init__()
        self.engaged = 0
        gs.init()
        logging.info("Android thread initialized")

        dispatcher.connect(self.sendAndroid, signal=gs.RPI_ANDROID_SIGNAL, sender=gs.RPI_SENDER)

        # android.py will control all thread, algorithm to be initialise later
        self.rpi_thread = RPI()
        self.arduino_thread = Arduino()

        self.rpi_thread.daemon = True
        self.arduino_thread.daemon = True

        self.rpi_thread.start()
        self.arduino_thread.start()

        self.port = 4
        self.server_socket = BluetoothSocket(RFCOMM)
        self.server_socket.bind(("", self.port))
        self.server_socket.listen(1)
        self.uuid = "00001101-0000-1000-8000-00805F9B34FB"
        self.connect()

    def connect(self):
        advertise_service(self.server_socket, "MDPGroup5", service_id=self.uuid, service_classes=[self.uuid, SERIAL_PORT_CLASS], profiles=[SERIAL_PORT_PROFILE])
        logging.info("Waiting for Bluetooth connection on port " + str(self.port))
        self.client_sock, self.client_info = self.server_socket.accept()
        logging.info("Accepted connection from" + str(self.client_info))
        client_MAC = self.client_info[0]
        if client_MAC != "08:60:6E:AA:6D:E2":
            logging.info("There was a connection attempt by an unauthorized device. Attempting to rebroadcast..")
            self.connect()
            return 0
        try:
            self.sendAndroid("Connection Secured")
            self.connected = True
            self.receiveAndroid()
        except IOError:
            pass

    def startAlgorithm(self, robot_row, robot_col, waypoint_row, waypoint_col, goal_row, goal_col, mode, dir):
        self.algo_thread = Algorithm(robot_row, robot_col, waypoint_row, waypoint_col, goal_row, goal_col, mode, dir)
        # only exploration mode need to start thread
        if mode == EXPLORATION:
            self.algo_thread.daemon = True
            self.algo_thread.start()

    def stopAlgorithm(self):
        self.algo_thread.stop()
        del self.algo_thread

    def sendAndroid(self, message):
        """
        Send a message to Android device
        """
        try:
            self.client_sock.send(str(message))
        except BluetoothError:
            logging.info("Bluetooth Error - encountered when attempting to send data to Android device")
            self.connected = False
            self.connect()

    def receiveAndroid(self):
        """
        Receiving message from Android
        """
        try:
            while self.connected:
                msg = self.client_sock.recv(1024)
                # dispatcher.send(message=msg, signal=gs.ANDROID_SIGNAL, sender=gs.ANDROID_SENDER)
                self.sendAndroid("Received: " + msg)
                command = msg.split()[0]
                if(command == "ca"):
                    self.waypoint_row = int(msg.split()[1])
                    self.waypoint_col = int(msg.split()[2])
                    dispatcher.send(message=command, signal=gs.ANDROID_SIGNAL, sender=gs.ANDROID_SENDER)
                elif(command == "ex"):
                    self.startAlgorithm(START_ROW, START_COL, waypoint_row, waypoint_col, FP_GOAL_ROW, FP_GOAL_COL, EXPLORATION, EAST)
                elif(command == "fp"):
                    self.startAlgorithm(START_ROW, START_COL, waypoint_row, waypoint_col, FP_GOAL_ROW, FP_GOAL_COL, FASTEST_PATH, EAST)
                elif(command == "move"):
                    steps = int(msg.split()[1])
                    temp = ""
                    for i in range(steps):
                        temp = temp + "W"
                    movemessage = "C" + temp
                    dispatcher.send(message=movemessage, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
                elif(command == "rotate"):
                    degrees = int(msg.split()[1])
                elif(command == "mode"):    # for toggling modes but android will be handling it so its not needed for now
                    dispatcher.send(message=command, signal=gs.ANDROID_SIGNAL, sender=gs.ANDROID_SENDER)
                elif(command == "reset"):
                    logging.info("reset robot")
                elif(command == "return"):
                    logging.info("trigger return")
                else:
                    logging.info("Invalid message")
                logging.info("Android thread received: " + msg)

        except BluetoothError:
            logging.info("Bluetooth Error - encountered while receiving data from Android device")
            self.connected = False
            self.connect()

    def closeAndroidConnection(self):
        self.client_sock.close()
        self.server_sock.close()
        logging.info("Bluetooth connection ended.")

    # def start(self):
    #     self.running = True
    #     super(Android, self).start()

    # def run(self):
    #     advertise_service(self.server_socket, "MDPGroup5", service_id=self.uuid, service_classes=[self.uuid, SERIAL_PORT_CLASS], profiles=[SERIAL_PORT_PROFILE])
    #     logging.info("Waiting for Bluetooth connection on port " + str(self.port))
    #     self.client_sock, self.client_info = self.server_socket.accept()
    #     logging.info("Accepted connection from" + str(self.client_info))

    #     self.idle()

    # def stop(self):
    #     self.running = False

    # def idle(self):
    #     try:
    #         self.sendAndroid("Connection Secured")
    #         while(self.running):
    #             self.receiveAndroid()
    #     except IOError:
        #     pass
