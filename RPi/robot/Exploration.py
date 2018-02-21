#!/usr/bin/env python
"""Implementation of the exploration algorithm for a maze solving robot."""
import numpy as np
import time

import global_settings as gs

from Constants import NORTH, SOUTH, EAST, WEST, FORWARD, LEFT, RIGHT, START, MAX_ROWS, MAX_COLS


class Exploration:
    """Implementation of the Right-Wall hugging algorithm for a maze solving robot.

       The robot is assumed to start at the bottom-left corner of the map,
       i.e. (Rows - 2, 1). And the robot is facing North

    Attributes:
        currentMap (Numpy array): To store the current state of the map
        exploredArea (int): Count of the number of cells explored
        robot (Robot): Instance of the Robot class
        sensors (list of Numpy arrays): Readings from all sensors
        timeLimit (int): Maximum time allowed for exploration

    """

    def __init__(self, startPos=START, direction=EAST, realMap=None, timeLimit=None, calibrateLim=6, sim=True):
        """To initialise an instance of the Exploration class.

        Args:
            realMap (string): File name for the map during simulation mode
            timeLimit (int): Maximum time allowed for exploration
            sim (bool, optional): To specify simulation mode or real mode

        """
        self.startPos = startPos
        self.timeLimit = timeLimit
        self.exploredArea = 0
        self.currentMap = np.zeros([20, 15])
        # if sim:
        #     from Simulator import Robot
        #     self.robot = Robot(self.currentMap, direction, self.startPos, realMap)
        #     self.sensors = self.robot.getSensors()
        # else:
        from Real import Robot
        self.robot = Robot(self.currentMap, direction, startPos)

        self.exploredNeighbours = dict()
        self.sim = sim
        self.calibrateLim = calibrateLim
        self.virtualWall = [0, 0, MAX_ROWS, MAX_COLS]
        self.visited = dict()
        self.endTime = 0

    def __validInds(self, inds):
        """To check if the input indices are valid or not.

        To be valid the following conditions must be met:
            * A 3x3 neighbourhood around the center should lie within the arena
            * A 3x3 neighbourhood around the center should have no obstacle

        Args:
            inds (list of list): List of coordinates to be checked

        Returns:
            list of list: All indices that were valid

        """
        valid = []

        for i in inds:
            r, c = i
            x, y = np.meshgrid([-1, 0, 1], [-1, 0, 1])
            x, y = x+r, y+c
            if (np.any(x < 0) or np.any(y < 0) or np.any(x >= MAX_ROWS)
               or np.any(y >= MAX_COLS)):
                valid.append(False)
            elif (np.any(self.currentMap[x[0, 0]:x[0, 2]+1, y[0, 0]:y[2, 0]+1] != 1)):
                valid.append(False)
            else:
                valid.append(True)

        return [tuple(inds[i]) for i in range(len(inds)) if valid[i]]

    def explore(self, sensor_vals=None):
        """Run the exploration till the map is fully explored or time runs out.

        Handles the exploration of areas that cannot be explored using
        Right-Wall hugging algorithm by using the Fastest Path algorithm

        Returns:
            move: Next move or moves for the robot to execute
            bool: True is the map is fully explored

        """
        self.endTime = time.time() + self.timeLimit
        steps = 0
        numCycle = 1
        step = float(0.1)

        if (time.time() <= self.endTime and self.exploredArea < 100):
            if (sensor_vals):
                current = self.moveStep(sensor_vals)
            else:
                current = self.moveStep()

            steps += 1
            currentPos = tuple(self.robot.center)

            if (currentPos in self.visited):
                self.visited[currentPos] += 1
                if (self.visited[currentPos] > 3):
                    neighbour = self.getExploredNeighbour()
                    if (neighbour):
                        neighbour = np.asarray(neighbour)
                        fsp = FastestPath(self.currentMap, self.robot.center,
                                          neighbour, self.robot.direction, None)

                        fsp.getFastestPath()
                        while (fsp.robot.center.tolist() != neighbour.tolist()):
                            fsp.moveStep()
                            time.sleep(step)
                        print "Fastest Path to unexplored area!"

                        self.robot.center = neighbour
                        self.robot.head = fsp.robot.head
                        self.robot.direction = fsp.robot.direction

                        print fsp.movement
                        return fsp.movement, False
                    else:
                        return [], False
            else:
                self.visited[currentPos] = 1

                if (current[1]):
                    print "Exploration completed"

                return current

            if (np.array_equal(self.robot.center, self.startPos)):
                numCycle += 1
                if (numCycle > 1 and steps > 4 and self.exploredArea < 100):
                    neighbour = self.getExploredNeighbour()
                    if (neighbour):
                        neighbour = np.asarray(neighbour)
                        fsp = FastestPath(self.currentMap, self.robot.center,
                                          neighbour, self.robot.direction, None)

                        fsp.getFastestPath()
                        while (fsp.robot.center.tolist() != neighbour.tolist()):
                            fsp.moveStep()
                            time.sleep(step)
                        print "Fastest Path to unexplored area!"

                        self.robot.center = neighbour
                        self.robot.head = fsp.robot.head
                        self.robot.direction = fsp.robot.direction
                        self.robot.getSensors()

                        print fsp.movement
                        return fsp.movement, False
                    else:
                        return [], False
            time.sleep(float(step))
        elif (time.time() > self.endTime):
            print "Time limit reached!"

        if (self.exploredArea == 100):
            fsp = FastestPath(self.currentMap, self.robot.center, self.startPos, self.robot.direction, None)
            print "Exploration completed. Back to starting position!"

            fsp.getFastestPath()
            while (fsp.robot.center.tolist() != self.startPos.tolist()):
                fsp.moveStep()
                time.sleep(step)
            print "Starting position reached!"

            print fsp.movement
            return fsp.movement, True

        return [], False

    def moveStep(self, sensor_vals=None):
        """Move the robot one step for exploration.

        Returns:
            bool: True is the map is fully explored

        """
        if (sensor_vals):
            self.robot.getSensors(sensor_vals)
        else:
            self.robot.getSensors()

        move = self.nextMove()
        self.getExploredArea()

        if (self.exploredArea == 100):
            # return move, True, self.robot.center, self.robot.direction
            return move, True, self.robot.center, self.robot.direction
        else:
            return move, False, self.robot.center, self.robot.direction

    def nextMove(self):
        """Decide which direction is free to command robot the next action."""
        move = []
        # multi step
        front = self.frontFree()

        if (self.checkFree([1, 2, 3, 0], self.robot.center)):
            self.robot.moveBot(RIGHT)
            move.append(RIGHT)
            front = self.frontFree()
            for i in range(front):
                self.robot.moveBot(FORWARD)
            move.extend([FORWARD]*front)
        elif (front):
            for i in range(front):
                self.robot.moveBot(FORWARD)
            move.extend([FORWARD]*front)
        elif (self.checkFree([3, 0, 1, 2], self.robot.center)):
            self.robot.moveBot(LEFT)
            move.append(LEFT)
            front = self.frontFree()
            for i in range(front):
                self.robot.moveBot(FORWARD)
            move.extend([FORWARD]*front)
        else:
            self.robot.moveBot(RIGHT)
            self.robot.moveBot(RIGHT)
            move.extend(('O'))

        if not (self.sim):
            calibrate_front = self.robot.can_calibrate_front()
            calibrate_right = self.robot.can_calibrate_right()
            if self.robot.is_corner():
                move.append('L')
            elif (calibrate_right[0]):
                move.append(calibrate_right[1])
            elif (calibrate_front[0]):
                move.append(calibrate_front[1])
        return move

    def getExploredArea(self):
        """Update the total number of cells explored at the current state."""
        temp = np.sum(self.currentMap != 0)
        self.exploredArea = (temp/300.0)*100

    def checkFree(self, order, center):
        """Check if a specific direction is free to move to.

        Args:
            order (list): Ordering for the directionFree list based on the
                          the next move (Right, Left, Forward)

        Returns:
            bool: If the queried direction is free

        """
        nFree = self.northFree(center)
        sFree = self.southFree(center)
        eFree = self.eastFree(center)
        wFree = self.westFree(center)
        directionFree = np.asarray([nFree, eFree, sFree, wFree])
        directionFree = directionFree[order]

        if self.robot.direction == NORTH:
            return directionFree[0]
        elif self.robot.direction == EAST:
            return directionFree[1]
        elif self.robot.direction == SOUTH:
            return directionFree[2]
        else:
            return directionFree[3]

    def northFree(self, center):
        """Check if the north direction is free to move.

        Returns:
            bool: if north is free

        """
        r, c = center
        inds = [[r-2, c], [r-2, c-1], [r-2, c+1]]
        return self.validMove(inds)

    def eastFree(self, center):
        """Check if the east direction is free to move.

        Returns:
            bool: if east is free

        """
        r, c = center
        inds = [[r, c+2], [r-1, c+2], [r+1, c+2]]
        return self.validMove(inds)

    def southFree(self, center):
        """Check if the south direction is free to move.

        Returns:
            bool: if south is free

        """
        r, c = center
        inds = [[r+2, c], [r+2, c-1], [r+2, c+1]]
        return self.validMove(inds)

    def westFree(self, center):
        """Check if the west direction is free to move.

        Returns:
            bool: if west is free

        """
        r, c = center
        inds = [[r, c-2], [r-1, c-2], [r+1, c-2]]
        return self.validMove(inds)

    def frontFree(self):
        """Check if the front direction is free to move.

        Returns:
            counter: number of cells free in front

        """
        r, c = self.robot.center
        counter = 0

        if self.robot.direction == NORTH and self.validMove([[r-2, c], [r-2, c-1], [r-2, c+1]]):
            counter = 1
            while(True):
                if (self.validMove([[r-2-counter, c], [r-2-counter, c-1], [r-2-counter, c+1]])) and\
                        not self.checkFree([1, 2, 3, 0], [r-(counter), c]) and\
                        self.checkExplored([r-(counter), c]):
                    counter += 1
                else:
                    break
        elif self.robot.direction == EAST and self.validMove([[r, c+2], [r-1, c+2], [r+1, c+2]]):
            counter = 1
            while(True):
                if (self.validMove([[r, c+2+counter], [r-1, c+2+counter], [r+1, c+2+counter]])) and\
                        not self.checkFree([1, 2, 3, 0], [r, c+(counter)]) and\
                        self.checkExplored([r, c+(counter)]):
                    counter += 1
                else:
                    break
        elif self.robot.direction == WEST and self.validMove([[r, c-2], [r-1, c-2], [r+1, c-2]]):
            counter = 1
            while(True):
                if (self.validMove([[r, c-2-counter], [r-1, c-2-counter], [r+1, c-2-counter]])) and\
                        not self.checkFree([1, 2, 3, 0], [r, c-(counter)]) and\
                        self.checkExplored([r, c-(counter)]):
                    counter += 1
                else:
                    break
        elif self.robot.direction == SOUTH and self.validMove([[r+2, c], [r+2, c-1], [r+2, c+1]]):
            counter = 1
            while(True):
                if (self.validMove([[r+2+counter, c], [r+2+counter, c-1], [r+2+counter, c+1]])) and\
                        not self.checkFree([1, 2, 3, 0], [r+(counter), c]) and\
                        self.checkExplored([r+(counter), c]):
                    counter += 1
                else:
                    break

        return counter

    def validMove(self, inds):
        """Check if all the three cells on one side of the robot are free.

        Args:
            inds (list of list): List of cell indices to be checked

        Returns:
            bool: If all indices are free (no obstacle)

        """
        for (r, c) in inds:
            if not ((self.virtualWall[0] <= r < self.virtualWall[2]) and
                    (self.virtualWall[1] <= c < self.virtualWall[3])):
                return False

        return (self.currentMap[inds[0][0], inds[0][1]] == 1 and
                self.currentMap[inds[1][0], inds[1][1]] == 1 and
                self.currentMap[inds[2][0], inds[2][1]] == 1)

    def checkExplored(self, center):
        """Check if the cells within sensor range are explored.

        Returns:
            flag (bool): To indicate whether a cell is explored

        """
        r, c = center
        flag = True
        inds = []
        distanceShort = 3
        distanceLong = 5

        if self.robot.direction == NORTH:
            inds.append(zip([r-1]*distanceShort, range(c+2, c+distanceShort+2)))
            inds.append(zip([r+1]*distanceLong, range(c+2, c+distanceLong+2)))
            inds.append(zip([r-1]*distanceLong, range(c-distanceLong-1, c-1))[::-1])
        elif self.robot.direction == EAST:
            inds.append(zip(range(r+2, r+distanceShort+2), [c+1]*distanceShort))
            inds.append(zip(range(r+2, r+distanceLong+2), [c-1]*distanceLong))
            inds.append(zip(range(r-distanceLong-1, r-1), [c+1]*distanceLong)[::-1])
        elif self.robot.direction == WEST:
            inds.append(zip(range(r-distanceShort-1, r-1), [c-1]*distanceShort)[::-1])
            inds.append(zip(range(r-distanceLong-1, r-1), [c+1]*distanceLong)[::-1])
            inds.append(zip(range(r+2, r+distanceLong+2), [c-1]*distanceLong))
        else:
            inds.append(zip([r+1]*distanceShort, range(c-distanceShort-1, c-1))[::-1])
            inds.append(zip([r-1]*distanceLong, range(c-distanceLong-1, c-1))[::-1])
            inds.append(zip([r+1]*distanceLong, range(c+2, c+distanceLong+2)))

        for sensor in inds:
            if flag:
                for (x, y) in sensor:
                    if (x < self.virtualWall[0] or x == self.virtualWall[2]
                       or y < self.virtualWall[1] or y == self.virtualWall[3]
                       or self.currentMap[x, y] == 2):
                        break
                    elif (self.currentMap[x, y] == 0):
                        flag = False
                        break

        return flag

    def getExploredNeighbour(self):
        """Check if the unexplored cells are part of an explored wall.

        Returns:
            neighbour (list): Indicate the cell that is the neighbour of a wall

        """
        locs = np.where(self.currentMap == 0)
        self.virtualWall = [np.min(locs[0]), np.min(locs[1]), np.max(locs[0])+1, np.max(locs[1])+1]

        if ((self.virtualWall[2]-self.virtualWall[0] < 3)
           and self.virtualWall[2] < MAX_ROWS-3):
            self.virtualWall[2] += 3

        locs = np.asarray(zip(locs[0], locs[1]))
        cost1 = np.abs(locs[:, 0] - self.robot.center[0])
        cost2 = np.abs(locs[:, 1] - self.robot.center[1])
        cost = cost1 + cost2
        cost = cost.tolist()
        locs = locs.tolist()

        while (cost):
            position = np.argmin(cost)
            coord = locs.pop(position)
            cost.pop(position)
            neighbours = np.asarray([[-2, 0], [2, 0], [0, -2], [0, 2]]) + coord
            neighbours = self.__validInds(neighbours)
            for neighbour in neighbours:
                if (neighbour not in self.exploredNeighbours):
                    self.exploredNeighbours[neighbour] = True
                    return neighbour

        return None