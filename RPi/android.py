import logging
import time
import threading

from bluetooth import *
from pydispatch import dispatcher

import global_settings as gs


class Android(threading.Thread):
    def __init__(self):
        gs.init()
        logging.info("android thread initialized")

        # port doesnt have to be hardcoded when app is done
        # self.port = self.server_socket.getsockname()[1]
        self.port = 10
        self.server_socket = BluetoothSocket(RFCOMM)
        self.server_socket.bind(("", self.port))
        self.server_socket.listen(1)

        # needs to be the same as android device when the app is done
        uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"

        advertise_service(self.server_socket, "MDPGroup5", service_id=uuid, service_classes=[uuid, SERIAL_PORT_CLASS], profiles=[SERIAL_PORT_PROFILE])
        logging.info("Waiting for Bluetooth connection on port " + str(self.port))
        self.client_sock, self.client_info = self.server_socket.accept()
        logging.info("Accepted connection from" + str(self.client_info))

        try:
            self.sendAndroid("A test message")
            while True:
                self.receiveAndroid()
                # data = self.client_sock.recv(1024)
                # if len(data) == 0: break
                # logging.info("Received " + data)
                # self.client_sock.send("Pi received data: " + data)
        except IOError:
            pass

    def sendAndroid(self, message):
        """
        Send a message to Android device
        """
        try:
            self.client_sock.send(str(message))
        except BluetoothError:
            logging.info("Bluetooth Error")
            self.connect_bluetooth()

    def receiveAndroid(self):
        """
        Receiving message from Android
        """
        try:
            msg = self.client_sock.recv(1024)
            logging.info("Received " + msg)
            return msg
        except BluetoothError:
            logging.info("Bluetooth Error")
            self.connect_bluetooth()

    def closeAndroidConnection(self):
        self.client_sock.close()
        self.server_sock.close()
        logging.info("Bluetooth connection ended.")
