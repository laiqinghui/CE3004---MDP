#GPIO2 -> SDA
#GPIO3 -> SCL

import smbus
import time
import RPi.GPIO as GPIO  
GPIO.setmode(GPIO.BCM)  

# for RPI version 1, use "bus = smbus.SMBus(0)"
bus = smbus.SMBus(1)

# The address we setup in the Arduino Program
# Slave Address 
address = 0x04

def interruptHandler(channel):
    print "\n\n=====From Interrupt Handler====="
    print "Rising edge detected on port 17 and data from arduino is: "
    bytes = readData()
    print ''.join(chr(i) for i in bytes)
    print "================================\n"
    
GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) 
GPIO.add_event_detect(17, GPIO.RISING, callback=interruptHandler)  



def ConvertStringToBytes(src):
    converted = []
    for b in src:
        converted.append(ord(b))
    return converted


def writeData(value):
    #bus.write_byte(address, value)
    bus.write_i2c_block_data(address, 0, value)
    return -1

def readData():
    #number = bus.read_byte(address)
    number =  bus.read_i2c_block_data(address, 0, 18)
    return number
    
while True:
	#Receives the data from the User
    data = raw_input("Enter the data to be sent : ")
    bytesToSend = ConvertStringToBytes(data)
    writeData(bytesToSend)
    

    

#End of the Script