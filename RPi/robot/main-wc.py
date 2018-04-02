import time
import numpy as np
import copy
from Exploration import Exploration
from FastestPath import FastestPath
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import colors

from Constants import START, NORTH, GOAL, EAST

""" To test algorithm:
    - Choose from sample arena 1 to 5
    - Input map descriptor string P1 and P2 on mdpcx3004.sce.ntu.edu.sg/
    - Check results on the website
"""
print "Enter sample arena choice: "
choice1 = raw_input()

print "Exploration Mode Started"

map_name = 'sample_arena_' + choice1 + '.txt'

if choice1 == "a9":
    map_name = "arena_week_9.txt"
elif choice1 == "a8":
    map_name = "arena_week_8.txt"
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

# Unexplored, Explored, Obstacle, Robot Body, Robot Head
cmap = colors.ListedColormap(['#b0bf9d','#daf5ce','#311e08','#313956','#d6b71d'])
fig = plt.figure()
# im = plt.imshow(exploremaps[0], animated=True, cmap= cmap)

# def updatefig(j):
#     im.set_array(exploremaps[j])
#     return im

# ani = animation.FuncAnimation(fig, updatefig, frames=range(len(exploremaps)), interval=300)

index = 0
im = plt.imshow(exploremaps[index], cmap= cmap)

def on_keyboard(event):
    global index
    if event.key == 'right':
        index += 1
    elif event.key == 'left':
        index -= 1

    plt.clf()
    im = plt.imshow(exploremaps[index], cmap= cmap)
    plt.draw()

plt.gcf().canvas.mpl_connect('key_press_event', on_keyboard)

plt.show()

print exp.robot.exploredMap

print "Map Descriptor 1  -->  "+str(exp.robot.descriptor_1())
print "Map Descriptor 2  -->  "+str(exp.robot.descriptor_2())

print "Do you wan to enter fastest path mode (Y/N)?"
choice2 = str(raw_input())

print "Fastest Mode Started"

if (choice2 == "Y" or choice2 == "y"):
    if(choice1 == "1"):
        waypoint = [7, 12]
    elif(choice1 == "2"):
        waypoint = [1, 4]
    elif(choice1 == "3"):
        waypoint = [1, 1]
    elif(choice1 == "4"):
        waypoint = [18, 13]
    elif(choice1 == "12"):
        waypoint = [18, 13]
    else:
        waypoint = [16, 13]

    print "Waypoint is " + str(waypoint)
    fsp = FastestPath(exp.currentMap, START, GOAL, NORTH, waypoint)
    exploremaps = []

    fsp.getFastestPath()
    while (fsp.robot.center.tolist() != fsp.goal.tolist()):
        fsp.moveStep()
        mod_map = copy.deepcopy(exp.robot.exploredMap)
        mod_map[fsp.waypoint[0]][fsp.waypoint[1]] = 5
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
    print fsp.waypoint
    print exploremaps[0]
    fig = plt.figure()

    # Explored, Obstacle, Robot Body, Robot Head, Waypoint, FPath
    cmap = colors.ListedColormap(['#daf5ce','#311e08','#313956','#d6b71d', '#5c0909','#77a0a6' ])
    #cmap = colors.ListedColormap(['b','y','g','r', 'm', 'k'])
    # im = plt.imshow(exploremaps[0], animated=True, cmap=cmap)
    # ani = animation.FuncAnimation(fig, updatefig, frames=range(len(exploremaps)), interval=300)
    index = 0
    im = plt.imshow(exploremaps[index], cmap= cmap)

    def on_keyboard(event):
        global index
        if event.key == 'right':
            index += 1
        elif event.key == 'left':
            index -= 1

        plt.clf()
        im = plt.imshow(exploremaps[index], cmap=cmap)
        plt.draw()

    plt.gcf().canvas.mpl_connect('key_press_event', on_keyboard)
    plt.show()

    print fsp.fastestPathRun()
else:
    print "Program Terminated"
