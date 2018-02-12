import logging
import numpy as np


def init():

    global RPI_SENDER
    global RPI_ALGORITHM_SIGNAL
    global RPI_ANDROID_SIGNAL
    global RPI_ARDUINO_SIGNAL

    global ALGORITHM_SENDER
    global ALGORITHM_SIGNAL

    global ANDROID_SENDER
    global ANDROID_SIGNAL

    global ARDUINO_SENDER
    global ARDUINO_SIGNAL

    RPI_SENDER = 'rpi_sender'
    RPI_ALGORITHM_SIGNAL = 'rpi_algorithm_signal'
    RPI_ANDROID_SIGNAL = 'rpi_android_signal'
    RPI_ARDUINO_SIGNAL = 'rpi_arduino_signal'

    ALGORITHM_SENDER = 'algorithm_sender'
    ALGORITHM_SIGNAL = 'algorithm_signal'

    ANDROID_SENDER = 'android_sender'
    ANDROID_SIGNAL = 'android_signal'

    ARDUINO_SENDER = 'arduino_sender'
    ARDUINO_SIGNAL = 'arduino_signal'

    global MAZEMAP
    _row = 20
    _col = 15
    MAZEMAP = np.zeros([_row, _col], dtype=int)         # record maze obstacles, 1 for grid is obstacle
    MAZESTATUS = np.zeros([_row, _col], dtype=int)      # record exploration status, 1 for grid is explored

    logging.basicConfig(format='%(asctime)s,%(msecs)d %(levelname)-8s [%(filename)s:%(lineno)d] %(message)s',
                        datefmt='%d-%m-%Y:%H:%M:%S',
                        level=logging.INFO)


def print_mazemap(mmap):
    for mazerow in mmap:
        for grid in mazerow:
            if grid is None:
                print "?",
            else:
                print grid,
        print ""


def get_mdf_bitstring(mmap, format=0):
    bitstring = '11' + ''.join(str(grid) for mazerow in mmap for grid in mazerow) + '11'
    if format == 0:     # binary
        return bin(int(bitstring, 2))
    if format == 1:     # hexadecimal
        return hex(int(bitstring, 2))
    assert False, "unhandled format"
