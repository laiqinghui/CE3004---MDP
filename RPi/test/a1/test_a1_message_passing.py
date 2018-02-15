"""
Test that information should be able to be passed across following devices
- Android
- Arduino
- RPI

1. Able to establish bluetooth communication and receive command
2. Able to receive bluetooth test message
3. Android thread pass message to RPI
4. RPI pass message to arduino to start sensing environment
5. Arduino send message to RPI to request for move instruction
6. Algorithm send move instruction

# cd RPi/test
# python a1\test_a1_message_passing.py
"""

import time

from android import Android
from arduino import Arduino
from algorithm import Algorithm
from rpi import RPI

import test_settings as ts

if __name__ == "__main__":

    ts.init()

    rpi_thread = RPI()
    algo_thread = Algorithm()
    arduino_thread = Arduino()
    android_thread = Android()

    rpi_thread.daemon = True
    algo_thread.daemon = True
    arduino_thread.daemon = True
    android_thread.daemon = True

    rpi_thread.start()
    algo_thread.start()
    arduino_thread.start()
    android_thread.start()

    while 1:
        time.sleep(1)
