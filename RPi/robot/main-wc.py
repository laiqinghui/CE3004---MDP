import time
import numpy as np
import copy
from Exploration import Exploration
from FastestPath import FastestPath
import matplotlib.pyplot as plt
import matplotlib.animation as animation

from Constants import START, NORTH, GOAL, EAST

""" To test algorithm:
    - Choose from sample arena 1 to 5
    - Input map descriptor string P1 and P2 on mdpcx3004.sce.ntu.edu.sg/
    - Check results on the website
"""
choice1 = int(raw_input("Enter sample arena choice: "))

print "Exploration Mode Started"

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
elif(choice1 == 12):
    map_name = 'sample_arena_12.txt'
elif(choice1 == 13):
    map_name = 'sample_arena_13.txt'
else:
    map_name = 'arena_week_9.txt'

exp = Exploration(realMap=map_name, timeLimit=5)

step = float(0.1)

temp = exp.explore()
# print temp
# print exp.robot.exploredMap
exploremaps = []
while not temp[1]:
    temp = exp.explore()
    # print temp
    mod_map = copy.deepcopy(exp.robot.exploredMap)
    mod_map[exp.robot.center[0]][exp.robot.center[1]] = 3
    mod_map[exp.robot.center[0]][exp.robot.center[1]+1] = 3
    mod_map[exp.robot.center[0]][exp.robot.center[1]-1] = 3
    mod_map[exp.robot.center[0]+1][exp.robot.center[1]] = 3
    mod_map[exp.robot.center[0]+1][exp.robot.center[1]+1] = 3
    mod_map[exp.robot.center[0]+1][exp.robot.center[1]-1] = 3
    mod_map[exp.robot.center[0]-1][exp.robot.center[1]] = 3
    mod_map[exp.robot.center[0]-1][exp.robot.center[1]+1] = 3
    mod_map[exp.robot.center[0]-1][exp.robot.center[1]-1] = 3
    mod_map[exp.robot.head[0]][exp.robot.head[1]] = 4
    exploremaps.append(mod_map)

fig = plt.figure()
im = plt.imshow(exploremaps[0], animated=True)


def updatefig(j):
    im.set_array(exploremaps[j])
    return im


ani = animation.FuncAnimation(fig, updatefig, frames=range(len(exploremaps)), interval=150)
plt.show()

print exp.robot.exploredMap

print "Map Descriptor 1  -->  " + str(exp.robot.descriptor_1())
print "Map Descriptor 2  -->  " + str(exp.robot.descriptor_2())

choice2 = raw_input("Do you wan to enter fastest path mode (Y/N)?: ")

print "Fastest Mode Started"

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
    elif(choice1 == 13):
        waypoint = [16, 13]
    else:
        waypoint = [2, 7]

    print "Waypoint is " + str(waypoint)
    fsp = FastestPath(exp.currentMap, START, GOAL, NORTH, waypoint)
    exploremaps = []

    fsp.getFastestPath()
    while (fsp.robot.center.tolist() != fsp.goal.tolist()):
        fsp.moveStep()
        mod_map = copy.deepcopy(exp.robot.exploredMap)
        mod_map[fsp.robot.center[0]][fsp.robot.center[1]] = 3
        mod_map[fsp.robot.center[0]][fsp.robot.center[1]+1] = 3
        mod_map[fsp.robot.center[0]][fsp.robot.center[1]-1] = 3
        mod_map[fsp.robot.center[0]+1][fsp.robot.center[1]] = 3
        mod_map[fsp.robot.center[0]+1][fsp.robot.center[1]+1] = 3
        mod_map[fsp.robot.center[0]+1][fsp.robot.center[1]-1] = 3
        mod_map[fsp.robot.center[0]-1][fsp.robot.center[1]] = 3
        mod_map[fsp.robot.center[0]-1][fsp.robot.center[1]+1] = 3
        mod_map[fsp.robot.center[0]-1][fsp.robot.center[1]-1] = 3
        mod_map[fsp.robot.head[0]][fsp.robot.head[1]] = 4
        exploremaps.append(mod_map)

    fig = plt.figure()
    im = plt.imshow(exploremaps[0], animated=True)

    ani = animation.FuncAnimation(fig, updatefig, frames=range(len(exploremaps)), interval=150)
    plt.show()
    # print fsp.fastestPathRun()
else:
    print "Program Terminated"
