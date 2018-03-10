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
    MAZEMAP = np.zeros([_row, _col], dtype=int)

    logging.basicConfig(format='%(asctime)s,%(msecs)d %(levelname)-8s [%(filename)s:%(lineno)d] %(message)s',
                        datefmt='%d-%m-%Y:%H:%M:%S',
                        level=logging.INFO)


def print_modified_mazemap(mmap, robot_row, robot_col, direction):
    robot_symbol = ""
    if direction == 1:
        robot_symbol = "^"
    elif direction == 2:
        robot_symbol = ">"
    elif direction == 3:
        robot_symbol = "v"
    elif direction == 4:
        robot_symbol = "<"

    for row in range(mmap.shape[0]):
        for col in range(mmap.shape[1]):
            if robot_row == row and robot_col == col:
                print robot_symbol,
                continue
            if mmap[row, col] == 0:
                print "0",
                continue
            if mmap[row, col] == 1:
                print "1",
                continue
            if mmap[row, col] == 2:
                print chr(254),
                continue
            print mmap[row, col],
        print ""


def get_obstacle_mazemap(mmap):
    return np.where(mmap > 1, 1, 0)


def get_explore_status_mazemap(mmap):
    return np.where(mmap > 0, 1, 0)


def get_mdf_bitstring(mmap, format=0, exploreOrObstacle=0):
    bl_coord_map = np.flip(mmap, 0)
    mdf_map_string_1_2 = ''.join(str(grid) for mazerow in bl_coord_map for grid in mazerow)

    mdf_string = ""

    if exploreOrObstacle == 0:
        mdf_string = mdf_map_string_1_2.replace("2", "1")
        mdf_string = "11" + mdf_string + "11"
    if exploreOrObstacle == 1:
        mdf_string = mdf_map_string_1_2.replace("0", "")
        mdf_string = mdf_string.replace("1", "0")
        mdf_string = mdf_string.replace("2", "1")
        mdf_string = mdf_string + '0' * (8 - (len(mdf_string) % 8))

    # bitstring = '11' + ''.join(str(grid) for mazerow in mmap for grid in mazerow) + '11'
    if format == 0:     # binary
        return mdf_string
    if format == 1:     # hexadecimal
        return '{:0{}X}'.format(int(mdf_string, 2), len(mdf_string) // 4)

    assert False, "unhandled format"


def aggregate_instruction(instruction):
    if len(instruction) == 0:
        return []

    instruction_list = [ch + '1' for ch in instruction]
    updatedInstruction = [instruction_list[0]]
    for ch in instruction_list[1:]:
        if (ch[0] != updatedInstruction[-1][0]):
            updatedInstruction.append(ch)
        else:
            # updatedInstruction[-1] = updatedInstruction[-1][0] + str(int(updatedInstruction[-1][1:]) + 1)
            updatedInstruction[-1] = updatedInstruction[-1][0] + chr(ord(updatedInstruction[-1][1:]) + 1)
    return updatedInstruction
