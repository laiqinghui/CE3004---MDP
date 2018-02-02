import logging
import time
import threading

from bluetooth import *
from pydispatch import dispatcher

import global_settings as gs


class Android(threading.Thread):

    def __init__(self):
        logging.info("android thread initialized")
        # dispatcher.connect(self.  , signal=gs.ANDROID_SIGNAL, sender=ANDROID_SENDER)

        try:
            self.server_socket = BluetoothSocket(RFCOMM)
            self.server_socket.bind(("", PORT_ANY))
            self.server_socket.listen(1)
            self.port = self.server_socket.getsockname()[1]

            uuid = "4eed03ae-06c1-4aa5-862a-3c2712446594"

            advertise_service(self.server_socket, "MDPGroup5", service_id=uuid, service_classes=[uuid, SERIAL_PORT_CLASS], profiles=[SERIAL_PORT_PROFILE], protocols=[OBEX_UUID])
            logging.info("Waiting for Bluetooth connection ...")
            self.client_sock, client_info = self.server_sock.accept()
            logging.info("Accepted Bluetooth connection.")
            while True:
                data = client_sock.recv(1024)
                if len(data) == 0:
                    break
                logging.info("received [%s]" % data)
                client_socket.send("a message from pi")

        except:
            print "Error:"

    # def sendAndroid(self, message):
    #     """
    #     Send a message to Android device
    #     """
    #     try:
    #         self.client_socket.send(str(message))
    #     except BluetoothError:
    #         print "Bluetooth Error"
    #         self.connect_bluetooth()

    # def receiveAndroid(self):
    #     """
    #     Receiving messages from Android
    #     """
    #     try:
    #         msg = self.client_socket.recv(2048)
    #         return msg
    #     except BluetoothError:
    #         print "Bluetooth Error"
    #         self.connect_bluetooth()
