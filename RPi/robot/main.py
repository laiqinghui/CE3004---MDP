import time
import numpy as np
from Exploration import Exploration
from FastestPath import FastestPath

from Constants import START, NORTH, GOAL, EAST

""" To test algorithm:
    - Choose from sample arena 1 to 5
    - Input map descriptor string P1 and P2 on mdpcx3004.sce.ntu.edu.sg/
    - Check results on the website
"""
print "Enter sample arena choice: "
choice1 = int(raw_input())

if(choice1 == 1):
    map_name = 'sample_arena_1.txt'
elif(choice1 == 2):
    map_name = 'sample_arena_2.txt'
elif(choice1 == 3):
    map_name = 'sample_arena_3.txt'
elif(choice1 == 4):
    map_name = 'sample_arena_4.txt'
elif(choice1 == 5):
    map_name = 'sample_arena_5.txt'
elif(choice1 == 9):
    map_name = 'sample_arena_9.txt'
else:
    map_name = 'sample_arena_12.txt'

exp = Exploration(realMap=map_name, timeLimit=5)

step = float(0.1)

temp = exp.explore()
print temp
print exp.robot.exploredMap

while not temp[1]:
    temp = exp.explore()
    print temp
    print exp.robot.exploredMap

print "Map Descriptor 1  -->  "+str(exp.robot.descriptor_1())
print "Map Descriptor 2  -->  "+str(exp.robot.descriptor_2())

print "Do you wan to enter fastest path mode (Y/N)?"
choice2 = str(raw_input())

if (choice2 == "Y" or choice2 == "y"):
    if(choice1 == 1):
        waypoint = [7, 12]
    elif(choice1 == 2):
        waypoint = [1, 4]
    elif(choice1 == 3):
        waypoint = [1, 1]
    elif(choice1 == 4):
        waypoint = [18, 13]
    elif(choice1 == 12):
        waypoint = [18, 13]
    else:
        waypoint = [18, 11]

    print "Waypoint is " +str(waypoint)

    fsp = FastestPath(exp.currentMap, START, GOAL, EAST, waypoint)
    print fsp.fastestPathRun()
else:
    print "Program Terminated"
