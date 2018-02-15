import logging
import time
import threading
import sys

from bluetooth import *
from pydispatch import dispatcher

import test_settings as ts


class Android(threading.Thread):
    def __init__(self):

        super(Android, self).__init__()
        dispatcher.connect(self.sendAndroid, signal=ts.RPI_ANDROID_SIGNAL, sender=ts.RPI_SENDER)

        self.running = False
        self.port = 4
        self.server_socket = BluetoothSocket(RFCOMM)
        # self.port = self.server_socket.getsockname()[1]
        self.server_socket.bind(("", self.port))
        self.server_socket.listen(1)
        self.uuid = "00001101-0000-1000-8000-00805F9B34FB"

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
            dispatcher.send(message=msg, signal=ts.ANDROID_SIGNAL, sender=ts.ANDROID_SENDER)
            self.sendAndroid("This message has been received: " + msg)
            # logging.info("Received " + msg)
            return msg
        except BluetoothError:
            logging.info("Bluetooth Error")
            self.connect_bluetooth()

    def closeAndroidConnection(self):
        self.client_sock.close()
        self.server_sock.close()
        logging.info("Bluetooth connection ended.")

    def start(self):
        self.running = True
        super(Android, self).start()

    def run(self):

        advertise_service(self.server_socket, "MDPGroup5", service_id=self.uuid, service_classes=[self.uuid, SERIAL_PORT_CLASS], profiles=[SERIAL_PORT_PROFILE])
        logging.info("Waiting for Bluetooth connection on port " + str(self.port))
        self.client_sock, self.client_info = self.server_socket.accept()
        logging.info("Accepted connection from" + str(self.client_info))

        self.idle()

    def stop(self):
        self.running = False

    def idle(self):
        try:
            self.sendAndroid("Connection Secured")
            while(self.running):
                self.receiveAndroid()
        except IOError:
            pass
