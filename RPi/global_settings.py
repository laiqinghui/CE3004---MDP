import logging


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
    _x = 15
    _y = 20
    MAZEMAP = [[None for i in range(_x)] for j in range(_y)]

    logging.basicConfig(format='%(asctime)s,%(msecs)d %(levelname)-8s [%(filename)s:%(lineno)d] %(message)s',
                        datefmt='%d-%m-%Y:%H:%M:%S',
                        level=logging.INFO)


def print_mazemap(mmap):
    for mazerow in mmap:
        for grid in mazerow:
            if grid is None:
                print " "
            else:
                print grid,
        print " "
