import datetime
import logging
import multiprocessing as mp
import textwrap
import threading
import time

from pydispatch import dispatcher
import serial

import global_settings as gs


class Arduino(threading.Thread):
    def __init__(self):

        super(Arduino, self).__init__()
        self.running = False
        self.serialConnection = serial.Serial(port='/dev/serial0', baudrate=9600)
        self.mutex_w = mp.Lock()
        self.acknowledged = True

        dispatcher.connect(self.writeData, signal=gs.RPI_ARDUINO_SIGNAL, sender=gs.RPI_SENDER)
        logging.info("arduino initialized")

    def interpret_sensor_values(self, arr):
        output = [int(x) for x in arr[:5]]
        logging.info("sensor output len should be 5: " + str(output))
        return output

    def ConvertStringToBytes(self, src):
        return [ord(x) for x in src]

    def writeData(self, message):
        packeted_instr = self.break_instruction_packets(message)
        self.write_packets_and_wait_acknowledgement(packeted_instr)

    def write_packets_and_wait_acknowledgement(self, packeted_instr):
        for instr in packeted_instr[:-1]:
            data = self.ConvertStringToBytes(instr)
            self.serialConnection.write(data)
            try:
                self.mutex_w.acquire()
                self.acknowledged = False
                self.mutex_w.release()
                data = self.ConvertStringToBytes(instr)
                self.bus.write_i2c_block_data(self.address, 0, data)
                while not self.acknowledged:
                    pass
            except IOError:
                logging.info("Please check if arduino connected.")
        data = self.ConvertStringToBytes(packeted_instr[-1])
        self.serialConnection.write(data)

    def break_instruction_packets(self, instruction):
        if len(instruction) <= 30:
            return [instruction]
        else:
            logging.info("long instruction received: " + str(instruction))
            packeted_instr = textwrap.wrap(instruction[1:-1], 28)

            # Special char C needs acknowledgement from arduino
            for i in range(len(packeted_instr[:-1])):
                packeted_instr[i] = 'C' + packeted_instr[i] + ';'
            packeted_instr[-1] = instruction[0] + packeted_instr[-1] + ';'
            logging.info("total packets: " + str(packeted_instr))

            return packeted_instr

    def start(self):
        self.running = True
        super(Arduino, self).start()

    def run(self):
        self.idle()

    def stop(self):
        self.running = False

    def listenSerialData(self):
        if(self.serialConnection.inWaiting() > 0):
            byte = self.serialConnection.read(10)
            logging.info("data from arduino" + str(byte))
            if chr(byte[0]) == "S":
                message = self.interpret_sensor_values(byte[1:])
                dispatcher.send(message=message, signal=gs.ARDUINO_SIGNAL, sender=gs.ARDUINO_SENDER)
            if chr(byte[0]) == "A":
                logging.info("acknowledgement received")
                self.mutex_w.acquire()
                if self.acknowledged:
                    logging.info("Error: acknowledgement byte from arduino not resolved yet")
                self.acknowledged = True
                self.mutex_w.release()

    def idle(self):
        while(self.running):
            self.listenSerialData()
